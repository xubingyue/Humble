
#include "Chan.h"
#include "Funcs.h"
#include "WorkerDisp.h"

H_BNAMSP

CChan::CChan(const unsigned int &uiCount) : m_bClose(false), m_uiRWait(H_INIT_NUMBER),
    m_uiWWait(H_INIT_NUMBER)
{
    H_ASSERT(uiCount >0 , "count must big than zero.");
    m_uiCount = uiCount;

    pthread_mutex_init(&m_quLock, NULL);
    pthread_cond_init(&m_pWCond, NULL);
    pthread_cond_init(&m_pRCond, NULL);
}

CChan::~CChan(void)
{
    pthread_cond_destroy(&m_pWCond);
    pthread_cond_destroy(&m_pRCond);
    pthread_mutex_destroy(&m_quLock);
}

void CChan::Close(void)
{
    CLckThis objLock(&m_quLock);
    m_bClose = true;
    pthread_cond_broadcast(&m_pWCond);
    pthread_cond_broadcast(&m_pRCond);
}

bool CChan::canSend(void)
{
    CLckThis objLock(&m_quLock);
    return  m_bClose ? false : (m_quData.size() < m_uiCount);
}

void CChan::Send(void *pszVal)
{
    CLckThis objLock(&m_quLock);
    if (m_bClose)
    {
        return;
    }

    while (m_quData.size() == m_uiCount)
    {
        if (m_bClose)
        {
            return;
        }

        ++m_uiWWait;
        pthread_cond_wait(&m_pWCond, objLock.getMutex());
        --m_uiWWait;
    }

    m_quData.push(pszVal);
    CWorkerDisp::getSingletonPtr()->Notify(&m_strRecvTask);

    if (m_uiRWait > H_INIT_NUMBER)
    {
        pthread_cond_signal(&m_pRCond);
    }
}

bool CChan::canRecv(void)
{
    CLckThis objLock(&m_quLock);
    return m_bClose ? false : !(m_quData.empty());
}

void *CChan::Recv(void)
{
    void *pVal = NULL;

    CLckThis objLock(&m_quLock);
    while (m_quData.empty())
    {
        if (m_bClose)
        {
            return NULL;
        }

        ++m_uiRWait;
        pthread_cond_wait(&m_pRCond, objLock.getMutex());
        --m_uiRWait;        
    }

    pVal = m_quData.front();
    m_quData.pop();
    CWorkerDisp::getSingletonPtr()->Notify(&m_strSendTask);

    if (m_uiWWait > H_INIT_NUMBER)
    {
        pthread_cond_signal(&m_pWCond);
    }

    return pVal;
}

void CChan::setChanNam(const char *pszName)
{
    m_strChanNam = pszName;
}

void CChan::setSendTaskNam(const char *pszName)
{
    if (!m_strSendTask.empty())
    {
        std::string strErro = H_FormatStr("chan %s already register send in task %s.",
            m_strChanNam.c_str(), m_strSendTask.c_str());
        H_ASSERT(false, strErro.c_str());
    }

    m_strSendTask = pszName;
}

void CChan::setRecvTaskNam(const char *pszName)
{
    if (!m_strRecvTask.empty())
    {
        std::string strErro = H_FormatStr("chan %s already register recv in task %s.",
            m_strChanNam.c_str(), m_strRecvTask.c_str());
        H_ASSERT(false, strErro.c_str());
    }

    m_strRecvTask = pszName;
}

H_ENAMSP
