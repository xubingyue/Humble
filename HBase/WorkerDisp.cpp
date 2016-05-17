
#include "WorkerDisp.h"
#include "Thread.h"
#include "LockThis.h"
#include "NetWorker.h"
#include "Log.h"

H_BNAMSP

SINGLETON_INIT(CWorkerDisp)
CWorkerDisp objWorker;

CWorkerDisp::CWorkerDisp(void) : m_usThreadNum(H_INIT_NUMBER), m_uiWait(H_INIT_NUMBER),
    m_lExit(RS_RUN), m_lCount(H_INIT_NUMBER), m_pWorker(NULL)
{
    pthread_mutex_init(&m_taskLock, NULL);
    pthread_cond_init(&m_taskCond, NULL);
}

CWorkerDisp::~CWorkerDisp(void)
{
    for (taskit itTask = m_mapTask.begin(); m_mapTask.end() != itTask; ++itTask)
    {
        H_SafeDelete(itTask->second);
    }
    m_mapTask.clear();

    H_SafeDelArray(m_pWorker);

    pthread_mutex_destroy(&m_taskLock);
    pthread_cond_destroy(&m_taskCond);
}

void CWorkerDisp::setThreadNum(const unsigned short usNum)
{
    m_usThreadNum = ((H_INIT_NUMBER == usNum) ? H_GetCoreCount() : usNum);
    m_pWorker = new(std::nothrow) CWorker[m_usThreadNum];    
    H_ASSERT(NULL != m_pWorker, "malloc memory error.");

    for (unsigned short usI = H_INIT_NUMBER; usI < m_usThreadNum; ++usI)
    {
        m_pWorker[usI].setIndex(usI);
        CThread::Creat(&m_pWorker[usI]);
        m_pWorker[usI].waitStart();
    }
}

CChan *CWorkerDisp::getChan(const char *pszTaskName)
{
    taskit itTask = m_mapTask.find(std::string(pszTaskName));
    if (m_mapTask.end() != itTask)
    {
        return itTask->second->getChan();
    }

    return NULL;
}

void CWorkerDisp::regTask(const char *pszName, CWorkerTask *pTask)
{
    std::string strName(pszName);

    taskit itTask = m_mapTask.find(strName);
    H_ASSERT(m_mapTask.end() == itTask, H_FormatStr("task %s already exist.", pszName).c_str());
            
    pTask->setName(pszName);
    m_mapTask.insert(std::make_pair(strName, pTask));
}

CWorker *CWorkerDisp::getFreeWorker(unsigned short &usIndex)
{
    while (true)
    {
        for (unsigned short usI = H_INIT_NUMBER; usI < m_usThreadNum; ++usI)
        {
            if (WS_FREE == m_pWorker[usI].getStatus())
            {
                usIndex = usI;
                return &m_pWorker[usI];
            }
        }

        H_Sleep(0);
    }

    return NULL;
}

CWorkerTask* CWorkerDisp::getTask(std::string *pstrName)
{
    taskit itTask = m_mapTask.find(*pstrName);
    if (m_mapTask.end() != itTask)
    {
        return itTask->second;
    }
    
    return NULL;
}

void CWorkerDisp::stopNet(void)
{
    CNetWorker *pNet = CNetWorker::getSingletonPtr();
    pNet->readyStop();
    while (RSTOP_RAN != pNet->getReadyStop())
    {
        H_Sleep(10);
    }
}

void CWorkerDisp::stopWorker(void)
{
    for (unsigned short usI = H_INIT_NUMBER; usI < m_usThreadNum; ++usI)
    {
        m_pWorker[usI].Join();
    }
}

void CWorkerDisp::runSurpTask(void)
{
    CWorkerTask *pWorkerTask = NULL;
    std::string *pTaskNam; 

    for (taskit itTask = m_mapTask.begin(); m_mapTask.end() != itTask; ++itTask)
    {
        for (size_t i = H_INIT_NUMBER; i < itTask->second->getChan()->getSize(); ++i)
        {
            CLckThis objLckThis(&m_taskLock);
            m_quTask.push(itTask->second->getName());
        }
    }
   
    while (true)
    {
        pTaskNam = NULL;
        {
            CLckThis objLckThis(&m_taskLock);
            if (!m_quTask.empty())
            {
                pTaskNam = m_quTask.front();
                m_quTask.pop();
            }
        }

        if (NULL == pTaskNam)
        {
            break;
        }

        pWorkerTask = getTask(pTaskNam);
        if (NULL != pWorkerTask)
        {
            pWorkerTask->runTask();
        }
    }
}

void CWorkerDisp::destroyTask(void)
{
    for (taskit itTask = m_mapTask.begin(); m_mapTask.end() != itTask; ++itTask)
    {
        itTask->second->destroyTask();
    }
}

void CWorkerDisp::Run(void)
{
    CWorker *pWorker = NULL;
    CWorkerTask *pWorkerTask = NULL;
    std::string *pTaskNam;
    unsigned short usIndex(H_INIT_NUMBER);

    for (taskit itTask = m_mapTask.begin(); m_mapTask.end() != itTask; ++itTask)
    {
        itTask->second->initTask();
    }

    H_AtomicAdd(&m_lCount, 1);

    //正常调度
    while (RS_RUN == H_AtomicGet(&m_lExit))
    {
        pTaskNam = NULL;
        {
            CLckThis objLckThis(&m_taskLock);
            if (!m_quTask.empty())
            {
                pTaskNam = m_quTask.front();
                m_quTask.pop();
            }
            else
            {
                ++m_uiWait;
                pthread_cond_wait(&m_taskCond, objLckThis.getMutex());
                --m_uiWait;
                continue;
            }
        }

        pWorkerTask = getTask(pTaskNam);
        if (NULL == pWorkerTask)
        {
            continue;
        }
        if (H_INIT_NUMBER != pWorkerTask->getRef())
        {
            CLckThis objLckThis(&m_taskLock);
            m_quTask.push(pTaskNam);
            continue;
        }

        pWorker = getFreeWorker(usIndex);
        pWorker->setBusy();
        pWorker->addWorker(pWorkerTask);
    }

    //停止网络
    stopNet();
    //停止工作线程
    stopWorker();
    //执行剩余的任务
    runSurpTask();    
    //任务清理
    destroyTask();

    H_AtomicAdd(&m_lCount, -1);
}

void CWorkerDisp::Join(void)
{
    if (RS_RUN != H_AtomicGet(&m_lExit))
    {
        return;
    }

    H_AtomicSet(&m_lExit, RS_STOP);

    for (;;)
    {
        pthread_cond_broadcast(&m_taskCond);
        if (H_INIT_NUMBER == H_AtomicGet(&m_lCount))
        {
            break;
        }

        H_Sleep(10);
    }
}

void CWorkerDisp::waitStart(void)
{
    for (;;)
    {
        if (H_INIT_NUMBER != H_AtomicGet(&m_lCount))
        {
            return;
        }

        H_Sleep(10);
    }
}

H_ENAMSP
