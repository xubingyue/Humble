
#include "NetWorker.h"
#include "Sender.h"
#include "NETAddr.h"
#include "Linker.h"

H_BNAMSP

SINGLETON_INIT(CNetWorker)
CNetWorker objNetWorker;

CNetWorker::CNetWorker(void) : m_uiLinkID(H_INIT_NUMBER), m_pIntf(NULL)
{

}

CNetWorker::~CNetWorker(void)
{
    std::vector<struct evconnlistener *>::iterator itListener;
    for (itListener = m_vcListener.begin(); m_vcListener.end() != itListener; ++itListener)
    {
        evconnlistener_free(*itListener);
    }
    m_vcListener.clear();

    std::list<struct event *>::iterator itMonEv;
    for (itMonEv = m_lstLinkMonitorEv.begin(); m_lstLinkMonitorEv.end() != itMonEv; ++itMonEv)
    {
        event_free(*itMonEv);
    }
    m_lstLinkMonitorEv.clear();

    std::list<H_TcpLink*>::iterator itLink;
    for (itLink = m_lstTcpLink.begin(); m_lstTcpLink.end() != itLink; ++itLink)
    {
        H_SafeDelete(*itLink);
    }
    m_lstTcpLink.clear();
}

void CNetWorker::setIntf(CNetIntf *pIntf)
{
    m_pIntf = pIntf;
}

CNetIntf *CNetWorker::getIntf(void)
{
    return m_pIntf;
}

void CNetWorker::addMapListener(H_SOCK &sock, const unsigned short &usSockType)
{
    m_mapLstType[sock] = usSockType;
}

bool CNetWorker::getListenerType(H_SOCK sock, unsigned short &usType)
{
    listenerit itListener;
    itListener = m_mapLstType.find(sock);
    if (m_mapLstType.end() == itListener)
    {
        return false;
    }

    usType = itListener->second;

    return true;
}

void CNetWorker::acceptCB(struct evconnlistener *pListener, H_SOCK sock, struct sockaddr *,
    int, void *arg)
{
    CNetWorker *pNetWorker = (CNetWorker *)arg;
    if (RSTOP_NONE != pNetWorker->getReadyStop())
    {
        evutil_closesocket(sock);
        return;
    }

    unsigned short usType(H_INIT_NUMBER);
    if (!(pNetWorker->getListenerType(evconnlistener_get_fd(pListener), usType)))
    {
        evutil_closesocket(sock);
        return;
    }
    
    (void)pNetWorker->addTcpEv(sock, usType);
}

void CNetWorker::addTcpListen(H_Order *pOrder)
{
    CNETAddr objAddr;
    if (H_RTN_OK != objAddr.setAddr(pOrder->acHost, pOrder->usPort))
    {
        H_Printf("%s", "setAddr error.");
        return;
    }

    struct evconnlistener *pListener = evconnlistener_new_bind(getBase(), acceptCB, this,
        LEV_OPT_CLOSE_ON_FREE, -1,
        objAddr.getAddr(), (int)objAddr.getAddrSize());
    if (NULL == pListener)
    {
        H_Printf("%s", "evconnlistener_new_bind error.");
        return;
    }

    m_vcListener.push_back(pListener);
    H_SOCK sock = evconnlistener_get_fd(pListener);
    addMapListener(sock, pOrder->usSockType);
}

void CNetWorker::monitorCB(H_SOCK, short, void *arg)
{
    H_TcpLink *pTcpLink = (H_TcpLink *)arg;
    if (H_INVALID_SOCK == pTcpLink->sock)
    {
        CLinker::getSingletonPtr()->addLink(pTcpLink->uiID, pTcpLink->acHost, pTcpLink->usPort);
    }
}

struct event *CNetWorker::monitorLink(H_TcpLink *pTcpLink)
{
    timeval tVal;
    size_t uiMS = 5000;
    evutil_timerclear(&tVal);
    if (uiMS >= 1000)
    {
        tVal.tv_sec = uiMS / 1000;
        tVal.tv_usec = (uiMS % 1000) * (1000);
    }
    else
    {
        tVal.tv_usec = (uiMS * 1000);
    }

    struct event *pEvent = event_new(getBase(),
        -1, EV_PERSIST, monitorCB, pTcpLink);
    H_ASSERT(NULL != pEvent, "event_new error.");
    (void)event_add(pEvent, &tVal);

    return pEvent;
}

void CNetWorker::onOrder(H_Order *pOrder)
{
    switch (pOrder->usCmd)
    {
        case ORDER_TCPLISTEN:
        {
            addTcpListen(pOrder);
        }
        break;
        case ORDER_ADDLINK:
        {
            size_t iLens(strlen(pOrder->acHost));
            H_TcpLink *pTcpLink = new(std::nothrow) H_TcpLink;
            H_ASSERT(NULL != pTcpLink, "malloc memory error.");
            memcpy(pTcpLink->acHost, pOrder->acHost, iLens);
            pTcpLink->acHost[iLens] = '\0';
            pTcpLink->sock = H_INVALID_SOCK;
            pTcpLink->uiID = ++m_uiLinkID;
            pTcpLink->usPort = pOrder->usPort;
            pTcpLink->usSockType = pOrder->usSockType;

            struct event *pMonitor = monitorLink(pTcpLink);
            m_lstTcpLink.push_back(pTcpLink);
            m_lstLinkMonitorEv.push_back(pMonitor);
            CLinker::getSingletonPtr()->addLink(pTcpLink->uiID, pTcpLink->acHost, pTcpLink->usPort);
        }
        break;
        case ORDER_ADDLINKEV:
        {
            std::list<H_TcpLink*>::iterator itLink;
            for (itLink = m_lstTcpLink.begin(); m_lstTcpLink.end() != itLink; ++itLink)
            {
                if ((*itLink)->uiID == pOrder->uiSession)
                {
                    (*itLink)->sock = pOrder->sock;
                    H_Session *pSession = addTcpEv((*itLink)->sock, (*itLink)->usSockType);
                    pSession->bLinker = true;

                    break;
                }
            }
        }
        break;
        default:
            break;
    }
}

void CNetWorker::onStart(void)
{
    m_pIntf->onStart();
}

void CNetWorker::onReadyStop(void)
{
    m_pIntf->onStop();
}

void CNetWorker::onClose(H_Session *pSession)
{
    CSender::getSingletonPtr()->delSock(pSession->sock, pSession->uiSession);
    if (pSession->bLinker)
    {
        std::list<H_TcpLink*>::iterator itLink;
        for (itLink = m_lstTcpLink.begin(); m_lstTcpLink.end() != itLink; ++itLink)
        {
            (*itLink)->sock = H_INVALID_SOCK;
            break;
        }
    }
    m_pIntf->onTcpClose(pSession);
}

void CNetWorker::onLinked(H_Session *pSession)
{
    CSender::getSingletonPtr()->addSock(pSession->sock, pSession->uiSession);
    m_pIntf->onTcpLinked(pSession);
}

void CNetWorker::onRead(H_Session *pSession)
{
    m_pIntf->onTcpRead(pSession);
}

void CNetWorker::tcpListen(const unsigned short usSockType, const char *pszHost, const unsigned short usPort)
{
    H_Order stOrder;
    stOrder.usCmd = ORDER_TCPLISTEN;
    stOrder.usSockType = usSockType;
    stOrder.usPort = usPort;
    size_t iLens = strlen(pszHost);
    memcpy(stOrder.acHost, pszHost, iLens);
    stOrder.acHost[iLens] = '\0';

    sendOrder(&stOrder, sizeof(stOrder));
}

void CNetWorker::addTcpLink(const unsigned short usSockType, const char *pszHost, const unsigned short usPort)
{
    H_Order stOrder;
    stOrder.usCmd = ORDER_ADDLINK;
    stOrder.usSockType = usSockType;
    stOrder.usPort = usPort;
    size_t iLens = strlen(pszHost);
    memcpy(stOrder.acHost, pszHost, iLens);
    stOrder.acHost[iLens] = '\0';

    sendOrder(&stOrder, sizeof(stOrder));    
}

void CNetWorker::addLinkEv(const H_SOCK &sock, const unsigned int &uiID)
{
    H_Order stOrder;
    stOrder.usCmd = ORDER_ADDLINKEV;
    stOrder.sock = sock;
    stOrder.uiSession = uiID;

    sendOrder(&stOrder, sizeof(stOrder));
}

H_ENAMSP
