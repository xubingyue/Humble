
#include "Chan.h"
#include "Funcs.h"
#include "WorkerDisp.h"

H_BNAMSP

CChan::CChan(void) : m_uiClose(H_INIT_NUMBER)
{
}

CChan::~CChan(void)
{
}

void CChan::Close(void)
{
    H_AtomicAdd(&m_uiClose, 1);
}

void CChan::Send(void *pszVal)
{
    if (H_INIT_NUMBER != H_AtomicGet(&m_uiClose))
    {
        return;
    }

    m_quLock.Lock();
    m_quData.push(pszVal);
    m_quLock.unLock();

    CWorkerDisp::getSingletonPtr()->Notify(&m_strRecvTask);
}

void *CChan::Recv(void)
{
    void *pVal = NULL;

    m_quLock.Lock();
    if (!m_quData.empty())
    {
        pVal = m_quData.front();
        m_quData.pop();
    }    
    m_quLock.unLock();
    
    if (H_INIT_NUMBER == H_AtomicGet(&m_uiClose)
        && NULL != pVal)
    {
        CWorkerDisp::getSingletonPtr()->Notify(&m_strSendTask);
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
