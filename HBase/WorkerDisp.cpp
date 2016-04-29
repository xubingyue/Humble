
#include "WorkerDisp.h"
#include "Thread.h"
#include "LockThis.h"
#include "NetWorker.h"

H_BNAMSP

SINGLETON_INIT(CWorkerDisp)
CWorkerDisp objWorker;

CWorkerDisp::CWorkerDisp(void) : m_usThreadNum(H_INIT_NUMBER), m_lExit(RS_RUN),
    m_lCount(H_INIT_NUMBER), m_pWorker(NULL)
{
    pthread_mutex_init(&m_taskLock, NULL);
    pthread_cond_init(&m_taskCond, NULL);
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

    pthread_mutex_destroy(&m_taskLock);
    pthread_cond_destroy(&m_taskCond);
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

CChan *CWorkerDisp::regSendChan(const char *pszChanName, const char *pszTaskName, const unsigned int uiCount)
{
    H_ASSERT(NULL != pszChanName && 0 != strlen(pszChanName), "chan name error.");
    H_ASSERT(NULL != pszTaskName && 0 != strlen(pszTaskName), "task name error.");

    std::string strName(pszChanName);
    CChan *pChan = NULL;

    m_objChanLock.wLock();
    chanit itChan = m_mapChan.find(strName);
    if (m_mapChan.end() == itChan)
    {
        pChan = new(std::nothrow) CChan(uiCount);
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

CChan *CWorkerDisp::regRecvChan(const char *pszChanName, const char *pszTaskName, const unsigned int uiCount)
{
    H_ASSERT(NULL != pszChanName && 0 != strlen(pszChanName), "chan name error.");
    H_ASSERT(NULL != pszTaskName && 0 != strlen(pszTaskName), "task name error.");

    std::string strName(pszChanName);
    CChan *pChan = NULL;

    m_objChanLock.wLock();
    chanit itChan = m_mapChan.find(strName);
    if (m_mapChan.end() == itChan)
    {
        pChan = new(std::nothrow) CChan(uiCount);
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
    H_ASSERT(NULL != pszName && 0 != strlen(pszName), " task name error.");
    H_ASSERT(NULL != pTask, "pointer is null.");

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
    CLckThis objLckThis(&m_taskLock);
    m_quTask.push(pstrName);
    pthread_cond_signal(&m_taskCond);
}

void CWorkerDisp::stopNet(void)
{
    CNetWorker *pNet = CNetWorker::getSingletonPtr();
    pNet->setReadyStop(RSTOP_RUN);
    while (RSTOP_RAN != pNet->getReadyStop())
    {
        H_Sleep(pNet->getTick());
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
    std::string *pTaskNam = NULL;

    while (!m_quTask.empty())
    {
        pTaskNam = m_quTask.front();
        m_quTask.pop();
        pWorkerTask = getWorkerTask(pTaskNam);
        if (NULL == pWorkerTask)
        {
            continue;
        }

        pWorkerTask->runTask();
    }
}

void CWorkerDisp::destroyTask(void)
{
    for (taskit itTask = m_mapTask.begin(); m_mapTask.end() != itTask; ++itTask)
    {
        itTask->second->destroyTask();
    }
}

void CWorkerDisp::closeChan(void)
{
    for (chanit itChan = m_mapChan.begin(); m_mapChan.end() != itChan; ++itChan)
    {
        itChan->second->Close();
    }
}

void CWorkerDisp::Run(void)
{
    CWorker *pWorker = NULL;
    CWorkerTask *pWorkerTask = NULL;
    std::string *pTaskNam = NULL;

    H_AtomicAdd(&m_lCount, 1);

    //正常调度
    while (RS_RUN == H_AtomicGet(&m_lExit))
    {
        CLckThis objLckThis(&m_taskLock);
        if (!m_quTask.empty())
        {
            pTaskNam = m_quTask.front();
            m_quTask.pop();
            pWorkerTask = getWorkerTask(pTaskNam);
            if (NULL == pWorkerTask)
            {
                continue;
            }
            if (H_INIT_NUMBER != pWorkerTask->getRef())
            {
                //正在执行的重新加入到调度队列
                m_quTask.push(pTaskNam);
                continue;
            }

            pWorker = getFreeWorker();
            pWorker->setBusy();
            pWorker->addWorker(pWorkerTask);
        }
        else
        {
            pthread_cond_wait(&m_taskCond, objLckThis.getMutex());
        }
    }

    //停止网络、定时器
    stopNet();
    //停止工作线程
    stopWorker();
    //执行剩余的任务
    runSurpTask();    
    //任务清理
    destroyTask();
    //关闭消息管道
    closeChan();

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
