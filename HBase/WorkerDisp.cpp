
#include "WorkerDisp.h"
#include "Thread.h"
#include "LockThis.h"

H_BNAMSP

SINGLETON_INIT(CWorkerDisp)
CWorkerDisp objWorker;

CWorkerDisp::CWorkerDisp(void) : m_usThreadNum(H_INIT_NUMBER), m_lExit(RS_RUN),
    m_lCount(H_INIT_NUMBER), m_pWorker(NULL)
{
    pthread_mutex_init(&m_dispTaskLock, NULL);
    pthread_cond_init(&m_dispTaskCond, NULL);
}

CWorkerDisp::~CWorkerDisp(void)
{
    chanit itChan;
    for (itChan = m_mapChan.begin(); m_mapChan.end() != itChan; ++itChan)
    {
        H_SafeDelete(itChan->second);
    }
    m_mapChan.clear();

    taskit itTask;
    for (itTask = m_mapTask.begin(); m_mapTask.end() != itTask; ++itTask)
    {
        H_SafeDelete(itTask->second);
    }
    m_mapTask.clear();

    H_SafeDelArray(m_pWorker);

    pthread_mutex_destroy(&m_dispTaskLock);
    pthread_cond_destroy(&m_dispTaskCond);
}

void CWorkerDisp::setThreadNum(const unsigned short usNum)
{
    m_usThreadNum = ((H_INIT_NUMBER == usNum) ? H_GetCoreCount() * 2 : usNum);
    m_pWorker = new(std::nothrow) CWorker[m_usThreadNum];    
    H_ASSERT(NULL != m_pWorker, "malloc memory error.");

    for (unsigned short usI = H_INIT_NUMBER; usI < m_usThreadNum; ++usI)
    {
        CThread::Creat(&m_pWorker[usI]);
        m_pWorker[usI].waitStart();
    }
}

CChan *CWorkerDisp::regSendChan(const char *pszChanName, const char *pszTaskName, const size_t iCount)
{
    std::string strName(pszChanName);
    CChan *pChan = NULL;

    m_objChanLock.wLock();
    chanit itChan = m_mapChan.find(strName);
    if (m_mapChan.end() == itChan)
    {
        pChan = new(std::nothrow) CChan(iCount);
        H_ASSERT(NULL != pChan, "malloc memory error.");
        pChan->setChanNam(pszChanName);
        pChan->setSendTaskNam(pszTaskName);

        m_mapChan.insert(std::make_pair(strName, pChan));
    }
    else
    {        
        pChan = itChan->second;
        pChan->setSendTaskNam(pszTaskName);        
    }
    m_objChanLock.unLock();

    return pChan;
}

CChan *CWorkerDisp::regRecvChan(const char *pszChanName, const char *pszTaskName, const size_t iCount)
{
    std::string strName(pszChanName);
    CChan *pChan = NULL;

    m_objChanLock.wLock();
    chanit itChan = m_mapChan.find(strName);
    if (m_mapChan.end() == itChan)
    {
        pChan = new(std::nothrow) CChan(iCount);
        H_ASSERT(NULL != pChan, "malloc memory error.");
        pChan->setChanNam(pszChanName);
        pChan->setRecvTaskNam(pszTaskName);

        m_mapChan.insert(std::make_pair(strName, pChan));
    }
    else
    {
        pChan = itChan->second;
        pChan->setRecvTaskNam(pszTaskName);
    }
    m_objChanLock.unLock();

    return pChan;
}

CChan *CWorkerDisp::getChan(const char *pszChanName)
{
    std::string strName(pszChanName);
    CChan *pChan = NULL;

    m_objChanLock.rLock();
    chanit itChan = m_mapChan.find(strName);
    if (m_mapChan.end() != itChan)
    {
        pChan = itChan->second;
    }
    m_objChanLock.unLock();

    return pChan;
}

void CWorkerDisp::regTask(const char *pszName, CWorkerTask *pTask)
{
    H_ASSERT(NULL != pszName && NULL != pTask, "pointer is null.");
    std::string strName(pszName);

    taskit itTask = m_mapTask.find(strName);
    if (m_mapTask.end() == itTask)
    {
        pTask->initTask(pszName);
        m_mapTask.insert(std::make_pair(strName, pTask));
    }
    else
    {
        std::string strErro = H_FormatStr("task name %s already exist.", pszName);
        H_ASSERT(false, strErro.c_str());
    }
}

CWorker *CWorkerDisp::getFreeWorker(void)
{
    while (true)
    {
        for (unsigned short usI = H_INIT_NUMBER; usI < m_usThreadNum; ++usI)
        {
            if (WS_FREE == m_pWorker[usI].getStatus())
            {
                return &m_pWorker[usI];
            }
        }

        H_Sleep(0);
    }

    return NULL;
}

CWorkerTask* CWorkerDisp::getWorkerTask(std::string *pstrName)
{
    taskit itTask = m_mapTask.find(*pstrName);
    if (m_mapTask.end() == itTask)
    {
        return NULL;
    }
    
    return itTask->second;
}

void CWorkerDisp::Notify(std::string *pstrName)
{
    CLckThis objLckThis(&m_dispTaskLock);
    m_quDisTask.push(pstrName);
    pthread_cond_signal(&m_dispTaskCond);
}

void CWorkerDisp::Run(void)
{    
    CWorker *pWorker = NULL;
    CWorkerTask *pWorkerTask = NULL;
    std::string *pTaskNam = NULL;

    H_AtomicAdd(&m_lCount, 1);
    while (RS_RUN == H_AtomicGet(&m_lExit))
    {
        CLckThis objLckThis(&m_dispTaskLock);
        if (!m_quDisTask.empty())
        {
            pTaskNam = m_quDisTask.front();
            m_quDisTask.pop();
            pWorkerTask = getWorkerTask(pTaskNam);
            if (NULL == pWorkerTask)
            {
                continue;
            }
            if (H_INIT_NUMBER != pWorkerTask->getRef())
            {
                continue;                
            }

            pWorker = getFreeWorker();
            pWorker->setBusy();
            pWorker->addWorker(pWorkerTask);
        }
        else
        {
            pthread_cond_wait(&m_dispTaskCond, objLckThis.getMutex());
        }
    }

    for (unsigned short usI = H_INIT_NUMBER; usI < m_usThreadNum; ++usI)
    {
        m_pWorker[usI].Join();
    }
    
    for (taskit itTask = m_mapTask.begin(); m_mapTask.end() != itTask; ++itTask)
    {
        itTask->second->destroyTask();
    }

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
        pthread_cond_broadcast(&m_dispTaskCond);
        if (H_INIT_NUMBER == H_AtomicGet(&m_lCount))
        {
            break;
        }

        H_Sleep(10);
    }
}

void CWorkerDisp::waitStart(void)
{
    for (unsigned int uiTime = 0; uiTime < 1000; uiTime += 10)
    {
        if (H_INIT_NUMBER != H_AtomicGet(&m_lCount))
        {
            return;
        }

        H_Sleep(10);
    }

    H_Printf("%s", "waitStart error.");
}

H_ENAMSP
