
#include "NetWorker.h"
#include "Sender.h"
#include "NETAddr.h"

H_BNAMSP

SINGLETON_INIT(CNetWorker)
CNetWorker objNetWorker;

CNetWorker::CNetWorker(void) : m_uiExit(RS_RUN), m_uiCount(H_INIT_NUMBER), m_uiTick(H_INIT_NUMBER),
    m_uiTickCount(H_INIT_NUMBER), m_uiSession(H_INIT_NUMBER), m_pTickEvent(NULL), m_pIntf(NULL)
{
#ifdef H_OS_WIN
    WORD wVersionReq;
    WSAData wsData;
    wVersionReq = MAKEWORD(2, 2);
    (void)WSAStartup(wVersionReq, &wsData);
#endif

    m_pBase = event_base_new();
    H_ASSERT(NULL != m_pBase, "event_base_new error.");
}

CNetWorker::~CNetWorker(void)
{
    std::vector<struct evconnlistener *>::iterator itListener;
    for (itListener = m_vcListener.begin(); m_vcListener.end() != itListener; ++itListener)
    {
        evconnlistener_free(*itListener);
    }
    m_vcListener.clear();

    sessionit itSession;
    for (itSession = m_mapSession.begin(); m_mapSession.end() != itSession; ++itSession)
    {
        freeSession(itSession->second);
    }
    m_mapSession.clear();

    std::vector<H_SOCK>::iterator itUdpSock;
    for (itUdpSock = m_vcUdpSock.begin(); m_vcUdpSock.end() != itUdpSock; ++itUdpSock)
    {
        evutil_closesocket(*itUdpSock);
    }
    m_vcUdpSock.clear();

    std::vector<struct event *>::iterator itUdpEv;
    for (itUdpEv = m_vcUdpListener.begin(); m_vcUdpListener.end() != itUdpEv; ++itUdpEv)
    {
        event_free(*itUdpEv);
    }
    m_vcUdpListener.clear();

    if (NULL != m_pTickEvent)
    {
        event_free(m_pTickEvent);
        m_pTickEvent = NULL;
    }

    if (NULL != m_pBase)
    {
        event_base_free(m_pBase);
        m_pBase = NULL;
    }

#ifdef H_OS_WIN    
    (void)WSACleanup();
#endif
}

void CNetWorker::setTick(const unsigned int uiMS)
{
    m_uiTick = uiMS;
}

const unsigned int CNetWorker::getTick(void)
{
    return m_uiTick;
}

void CNetWorker::setIntf(CNetIntf *pIntf)
{
    m_pIntf = pIntf;
}

CNetIntf *CNetWorker::getIntf(void)
{
    return m_pIntf;
}

void CNetWorker::Join(void)
{
    if (RS_RUN != H_AtomicGet(&m_uiExit))
    {
        return;
    }

    H_AtomicSet(&m_uiExit, RS_STOP);
    while (true)
    {
        if (H_INIT_NUMBER == H_AtomicGet(&m_uiCount))
        {
            break;
        }

        H_Sleep(10);
    }
}

bool CNetWorker::Exit(void)
{
    return RS_STOP == H_AtomicGet(&m_uiExit);
}

struct event_base * CNetWorker::getBase(void)
{
    return m_pBase;
}

void CNetWorker::addTick(void)
{
    ++m_uiTickCount;
}

unsigned int CNetWorker::getTickCount(void)
{
    return m_uiTickCount;
}

void CNetWorker::timeCB(H_SOCK, short, void *arg)
{
    CNetWorker *pNetWorker = (CNetWorker *)arg;
    if (pNetWorker->Exit())
    {
        (void)event_base_loopbreak(pNetWorker->getBase());
        return;
    }
    
    pNetWorker->addTick();
    pNetWorker->getIntf()->onTimer(pNetWorker->getTick(), pNetWorker->getTickCount());
}

struct event *CNetWorker::intiTick(const unsigned int &uiMS)
{
    timeval tVal;
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

    struct event *pEvent = event_new(m_pBase,
        -1, EV_PERSIST, timeCB, this);
    H_ASSERT(NULL != pEvent, "event_new error.");
    (void)event_add(pEvent, &tVal);

    return pEvent;
}

void CNetWorker::Run(void)
{
    H_AtomicAdd(&m_uiCount, 1);
    m_pTickEvent = intiTick(m_uiTick);
    m_pIntf->onStart();
    (void)event_base_dispatch(m_pBase);
    H_AtomicAdd(&m_uiCount, -1);
}

void CNetWorker::waitStart(void)
{
    for (unsigned int uiTime = 0; uiTime < 1000; uiTime += 10)
    {
        if (H_INIT_NUMBER != H_AtomicGet(&m_uiCount))
        {
            return;
        }

        H_Sleep(10);
    }

    H_Printf("%s", "waitStart error.");
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

unsigned int CNetWorker::getSession(void)
{
    return ++m_uiSession;
}

void CNetWorker::addSession(H_SOCK &sock, H_Session *pSesson)
{
    CSender::getSingletonPtr()->addSock(sock, pSesson->uiSession);
    m_mapSession[sock] = pSesson;
}

H_Session *CNetWorker::getSession(const H_SOCK &sock)
{
    sessionit itSession;
    itSession = m_mapSession.find(sock);
    if (m_mapSession.end() == itSession)
    {
        return NULL;
    }

    return itSession->second;
}

void CNetWorker::delSession(H_SOCK &sock)
{
    sessionit itSession;
    itSession = m_mapSession.find(sock);
    if (m_mapSession.end() == itSession)
    {
        return;
    }

    CSender::getSingletonPtr()->delSock(sock, itSession->second->uiSession);
    freeSession(itSession->second);
    m_mapSession.erase(itSession);
}

void CNetWorker::freeSession(H_Session *pSession)
{
    bufferevent_free(pSession->pBev);
    H_SafeDelete(pSession);
}

void CNetWorker::tcpReadCB(struct bufferevent *bev, void *arg)
{
    H_Session *pSession = (H_Session *)arg;
    pSession->pNetWorker->getIntf()->onTcpRead(pSession);
}

void CNetWorker::tcpEventCB(struct bufferevent *bev, short, void *arg)
{
    H_Session *pSession = (H_Session *)arg;
    pSession->pNetWorker->getIntf()->onTcpClose(pSession);
    pSession->pNetWorker->delSession(pSession->sock);
}

H_Session *CNetWorker::addTcpEv(H_SOCK &sock, const unsigned short &usSockType)
{
    int iFlag = 1;

    (void)setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&iFlag, sizeof(iFlag));
    (void)evutil_make_socket_nonblocking(sock);

    struct bufferevent *pBev = bufferevent_socket_new(getBase(), sock, BEV_OPT_CLOSE_ON_FREE);
    if (NULL == pBev)
    {
        H_Printf("%s", "bufferevent_socket_new error.");
        evutil_closesocket(sock);
        return NULL;
    }

    H_Session *pSession = new(std::nothrow) H_Session;
    H_ASSERT(NULL != pSession, "malloc memory error.");
    pSession->pNetWorker = this;
    pSession->sock = sock;
    pSession->uiSession = getSession();
    pSession->usSockType = usSockType;
    pSession->usStatus = H_INIT_NUMBER;
    pSession->pBev = pBev;
    pSession->uiID = H_INIT_NUMBER;

    bufferevent_setcb(pBev, tcpReadCB, NULL, tcpEventCB, pSession);
    (void)bufferevent_enable(pBev, EV_READ);
    addSession(sock, pSession);

    return pSession;
}

void CNetWorker::acceptCB(struct evconnlistener *pListener, H_SOCK sock, struct sockaddr *,
    int, void *arg)
{
    CNetWorker *pNetWorker = (CNetWorker *)arg;
    unsigned short usType(H_INIT_NUMBER);
    if (!(pNetWorker->getListenerType(evconnlistener_get_fd(pListener), usType)))
    {
        evutil_closesocket(sock);
        return;
    }
    
    H_Session *pSesson = pNetWorker->addTcpEv(sock, usType);
    pNetWorker->getIntf()->onTcpLinked(pSesson);
}

bool CNetWorker::tcpListen(const unsigned short usSockType, const char *pszHost, const unsigned short usPort)
{
    CNETAddr objAddr;
    if (H_RTN_OK != objAddr.setAddr(pszHost, usPort))
    {
        H_Printf("%s", "setAddr error.");
        return false;
    }

    struct evconnlistener *pListener = evconnlistener_new_bind(m_pBase, acceptCB, this,
        LEV_OPT_CLOSE_ON_FREE, -1,
        objAddr.getAddr(), (int)objAddr.getAddrSize());
    if (NULL == pListener)
    {
        H_Printf("%s", "evconnlistener_new_bind error.");
        return false;
    }

    m_vcListener.push_back(pListener);
    H_SOCK sock = evconnlistener_get_fd(pListener);
    addMapListener(sock, usSockType);

    return true;
}

H_SOCK CNetWorker::initSock(const char *pszHost, const unsigned short &usPort)
{
    CNETAddr objAddr;
    if (H_RTN_OK != objAddr.setAddr(pszHost, usPort))
    {
        H_Printf("%s", "setAddr error.");
        return H_INVALID_SOCK;
    }

    //´´½¨socket
    H_SOCK sock = socket(AF_INET, SOCK_STREAM, 0);
    if (H_INVALID_SOCK == sock)
    {
        H_Printf("%s", "creat socket error.");
        return H_INVALID_SOCK;
    }

    if (0 != connect(sock, objAddr.getAddr(), (int)objAddr.getAddrSize()))
    {
        H_Printf("%s", "connect error.");
        evutil_closesocket(sock);

        return H_INVALID_SOCK;
    }

    return sock;
}

H_SOCK CNetWorker::addTcpLink(const unsigned short usSockType, const char *pszHost, const unsigned short usPort)
{
    H_SOCK sock = initSock(pszHost, usPort);
    if (H_INVALID_SOCK == sock)
    {
        return H_INVALID_SOCK;
    }

    H_Session *pSession = addTcpEv(sock, usSockType);
    m_pIntf->onTcpLinked(pSession);

    return sock;
}

void CNetWorker::closeSock(const H_SOCK sock, const unsigned int uiSession)
{
    H_Session *pSession = getSession(sock);
    if (NULL == pSession)
    {
        return;
    }
    if (uiSession != pSession->uiSession)
    {
        return;
    }

    m_pIntf->onTcpClose(pSession);
    delSession(pSession->sock);
}

H_SOCK CNetWorker::initUDP(const unsigned short &usPort)
{
    sockaddr_in stAddr;
    const int iBroad = 1;
    H_Zero(&stAddr, sizeof(stAddr));
    stAddr.sin_family = AF_INET;
    stAddr.sin_port = htons(usPort);
    stAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    H_SOCK sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (H_INVALID_SOCK == sock)
    {
        H_Printf("%s", "creat socket error.");
        return H_INVALID_SOCK;
    }

    (void)setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&iBroad, sizeof(iBroad));

    if (bind(sock, (struct sockaddr *)&(stAddr), sizeof(stAddr)) < 0)
    {
        H_Printf("%s", "bind error.");
        evutil_closesocket(sock);

        return H_INVALID_SOCK;
    }

    return sock;
}

void CNetWorker::udpReadCB(H_SOCK sock, short ev, void *arg)
{
    CNetWorker *pNetWorker = (CNetWorker *)arg;
    unsigned short usType = H_INIT_NUMBER;
    if (!(pNetWorker->getListenerType(sock, usType)))
    {
        return;
    }
    
    pNetWorker->getIntf()->onUdpRead(sock, usType);
}

bool CNetWorker::udpListen(const unsigned short usSockType, const unsigned short usPort)
{
    H_SOCK sock = initUDP(usPort);
    if (H_INVALID_SOCK == sock)
    {
        return false;
    }

    struct event *pEv = event_new(m_pBase, sock, EV_READ | EV_PERSIST, udpReadCB, this);
    (void)event_add(pEv, NULL);

    m_vcUdpSock.push_back(sock);
    m_vcUdpListener.push_back(pEv);
    addMapListener(sock, usSockType);

    return true;
}

void CNetWorker::sendTo(H_SOCK sock, const char *pszHost, const unsigned short usPort, 
    const char *pBuf, const size_t iLens)
{
    CNETAddr objAddr;
    objAddr.setAddr(pszHost, usPort);
    sendto(sock, pBuf, (int)iLens, 0, objAddr.getAddr(), (int)objAddr.getAddrSize());
}

H_ENAMSP
