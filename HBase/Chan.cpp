
#include "Chan.h"
#include "Funcs.h"
#include "WorkerDisp.h"

H_BNAMSP

CChan::CChan(const unsigned int &uiCount) : m_uiCount(uiCount), m_uiRWait(H_INIT_NUMBER),
    m_uiWWait(H_INIT_NUMBER)
{
    pthread_mutex_init(&m_quLock, NULL);
    pthread_cond_init(&m_pWCond, NULL);
    pthread_cond_init(&m_pRCond, NULL);
}

CChan::~CChan(void)
{
    void *pData = NULL;
    while (!m_quData.empty())
    {
        pData = m_quData.front();
        m_quData.pop();
        free(pData);
    }

    pthread_cond_destroy(&m_pWCond);
    pthread_cond_destroy(&m_pRCond);
    pthread_mutex_destroy(&m_quLock);
}

void CChan::Send(void *pszVal)
{
    CLckThis objLock(&m_quLock);
    while ((m_quData.size() == m_uiCount)
        && (H_INIT_NUMBER != m_uiCount))
    {
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

void *CChan::Recv(void)
{
    void *pVal = NULL;

    CLckThis objLock(&m_quLock);
    while (m_quData.empty())
    {
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

bool CChan::canSend(void)
{
    CLckThis objLock(&m_quLock);
    return ((H_INIT_NUMBER == m_uiCount) ? true : (m_quData.size() < m_uiCount));
}

bool CChan::canRecv(void)
{
    CLckThis objLock(&m_quLock);
    return !(m_quData.empty());
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
