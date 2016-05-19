
#include "Sender.h"
#include "Funcs.h"
#include "NetParser.h"

H_BNAMSP

SINGLETON_INIT(CSender)
CSender objSender;

struct H_Host
{
    unsigned short usPort;
    char acHost[H_IPLENS];
    H_Host(void)
    {
        H_Zero(acHost, sizeof(acHost));
    };
};

enum
{
    SendTo,
    BroadCast,
    USendTo,
    UBroadCast,
};

CSender::CSender(void)
{

}

CSender::~CSender(void)
{

}

std::string *CSender::creatPack(const char *pBuf, const size_t &iLens, const unsigned short &usType)
{
    std::string *pstrBuf = getBuffer(usType);
    CParser *pParser = CNetParser::getSingletonPtr()->getParser(usType);
    if ((NULL == pstrBuf)
        || (NULL == pParser))
    {
        return NULL;
    }
    
    if (pstrBuf->empty())
    {
        pParser->creatPack(pstrBuf, pBuf, iLens);
    }

    return pstrBuf;
}

void CSender::sendToSock(H_SOCK &fd, const unsigned int &uiSession, const char *pBuf, const size_t &iLens)
{
    m_objLck.rLock();
    senderit itSender = m_mapSender.find(fd);
    if (m_mapSender.end() != itSender)
    {
        if (uiSession == itSender->second.uiSession)
        {
            std::string *pstrBuf = creatPack(pBuf, iLens, itSender->second.usType);
            if (NULL != pstrBuf)
            {
                H_SockWrite(fd, pstrBuf->c_str(), pstrBuf->size());
            }
        }
    }
    m_objLck.unLock();
}

std::string *CSender::getBuffer(const unsigned short &usType)
{
    std::string *pBuffer = NULL;

    m_objBufferLck.rLock();
    bufferit itBuf = m_mapBuffer.find(usType);
    if (m_mapBuffer.end() != itBuf)
    {
        pBuffer = &itBuf->second;
    }
    else
    {
        H_Printf("get send buffer by type %d error.", usType);
    }
    m_objBufferLck.unLock();

    return pBuffer;
}

void CSender::cleanBuffer(H_SOCK &fd, const unsigned int &uiSession)
{
    m_objLck.rLock();
    senderit itSender = m_mapSender.find(fd);
    if (m_mapSender.end() != itSender)
    {
        if (uiSession == itSender->second.uiSession)
        {
            std::string *pBuffer = getBuffer(itSender->second.usType);
            if (NULL != pBuffer)
            {
                pBuffer->clear();
            }
        }
    }
    m_objLck.unLock();
}

void CSender::addBuffer(const unsigned short &usType)
{
    m_objBufferLck.wLock();
    bufferit itBuf = m_mapBuffer.find(usType);
    if (m_mapBuffer.end() == itBuf)
    {
        m_mapBuffer.insert(std::make_pair(usType, std::string()));
    }
    m_objBufferLck.unLock();
}

void CSender::runTask(H_Sender *pMsg)
{
    switch (pMsg->usCmd)
    {
        case SendTo:
        {
            cleanBuffer(pMsg->stSock.sock, pMsg->stSock.uiSession);
            sendToSock(pMsg->stSock.sock, pMsg->stSock.uiSession, pMsg->pBuffer, pMsg->iBufLens);         
            H_SafeDelete(pMsg->pBuffer);
        }
        break;

        case BroadCast:
        {
            if (NULL == pMsg->pSock)
            {
                break;
            }

            for (unsigned short usI = H_INIT_NUMBER; usI < pMsg->usCount; ++usI)
            {
                cleanBuffer(pMsg->pSock[usI].sock, pMsg->pSock[usI].uiSession);
            }
            for (unsigned short usI = H_INIT_NUMBER; usI < pMsg->usCount; ++usI)
            {
                sendToSock(pMsg->pSock[usI].sock, pMsg->pSock[usI].uiSession, pMsg->pBuffer, pMsg->iBufLens);
            }

            H_SafeDelArray(pMsg->pSock);
            H_SafeDelete(pMsg->pBuffer);
        }
        break;

        case USendTo:
        {
            H_Host *pHost = (H_Host *)pMsg->pBuffer;
            char *pBuf = pMsg->pBuffer + sizeof(H_Host);
            (void)m_objAddr.setAddr(pHost->acHost, pHost->usPort);

            (void)sendto(pMsg->stSock.sock, pBuf, pMsg->iBufLens - sizeof(H_Host), 0,
                m_objAddr.getAddr(), m_objAddr.getAddrSize());

            H_SafeDelete(pMsg->pBuffer);
        }
        break;

        case UBroadCast:
        {
            struct sockaddr_in stAddr;
            stAddr.sin_family = AF_INET;
            stAddr.sin_addr.s_addr = INADDR_BROADCAST;
            stAddr.sin_port = htons((u_short)pMsg->usCount);
            
            (void)sendto(pMsg->stSock.sock, pMsg->pBuffer, pMsg->iBufLens, 0, 
                (struct sockaddr *)&stAddr, sizeof(struct sockaddr));

            H_SafeDelete(pMsg->pBuffer);
        }
        break;

        default:
            break;
    }
}

void CSender::Send(H_SOCK sock, const unsigned int uiSession, const char *pBuf, const size_t iLens)
{
    H_Sender *pSender = newT();
    char *pNewBuf = new(std::nothrow) char[iLens];
    H_ASSERT(NULL != pNewBuf, "malloc memory error.");

    memcpy(pNewBuf, pBuf, iLens);
    pSender->usCmd = SendTo;
    pSender->stSock.sock = sock;
    pSender->stSock.uiSession = uiSession;
    pSender->pBuffer = pNewBuf;
    pSender->iBufLens = iLens;

    addTask(pSender);
}

void CSender::sendBinary(H_SOCK sock, const unsigned int uiSession, CBinary *pBinary)
{
    std::string strBuf = pBinary->getWritedBuf();
    Send(sock, uiSession, strBuf.c_str(), strBuf.size());
}

H_SenderSock *CSender::createSenderSock(luabridge::LuaRef &lTable)
{
    int iCount(lTable.length());
    if (0 >= iCount)
    {
        return NULL;
    }

    H_SenderSock *pSock = new(std::nothrow) H_SenderSock[iCount];
    H_ASSERT(NULL != pSock, "malloc momory error.");
    for (int i = 1; i <= iCount; ++i)
    {
        luabridge::LuaRef objTmp = lTable[i];
        pSock[i - 1].sock = objTmp[1].cast<H_SOCK>();
        pSock[i - 1].uiSession = objTmp[2].cast<unsigned int>();
    }

    return pSock;
}

void CSender::lbroadCast(luabridge::LuaRef lTable, const char *pBuf, const size_t iLens)
{
    H_SenderSock *pSock = createSenderSock(lTable);
    if (NULL == pSock)
    {
        return;
    }

    broadCast(pSock, lTable.length(), pBuf, iLens);
}

void CSender::broadCastBinary(luabridge::LuaRef lTable, CBinary *pBinary)
{
    H_SenderSock *pSock = createSenderSock(lTable);
    if (NULL == pSock)
    {
        return;
    }

    std::string strBuf = pBinary->getWritedBuf();
    broadCast(pSock, lTable.length(), strBuf.c_str(), strBuf.size());
}

void CSender::broadCast(H_SenderSock *pSock, const int &iCount, const char *pBuf, const size_t &iLens)
{
    H_Sender *pSender = newT();
    char *pNewBuf = new(std::nothrow) char[iLens];
    H_ASSERT(NULL != pNewBuf, "malloc memory error."); 

    memcpy(pNewBuf, pBuf, iLens);
    pSender->usCmd = BroadCast;
    pSender->pSock = pSock;
    pSender->usCount = (unsigned short)iCount;
    pSender->pBuffer = pNewBuf;
    pSender->iBufLens = iLens;

    addTask(pSender);
}

void CSender::sendU(H_SOCK sock, const char *pstHost, unsigned short usPort,
    const char *pBuf, const size_t iLens)
{
    H_Host stHost;
    H_Sender *pSender = newT();
    char *pNewBuf = new(std::nothrow) char[iLens + sizeof(stHost)];
    H_ASSERT(NULL != pNewBuf, "malloc memory error.");

    stHost.usPort = usPort;
    memcpy(stHost.acHost, pstHost, strlen(pstHost));

    memcpy(pNewBuf, &stHost, sizeof(stHost));
    memcpy(pNewBuf + sizeof(stHost), pBuf, iLens);
    pSender->usCmd = USendTo;
    pSender->stSock.sock = sock;
    pSender->pBuffer = pNewBuf;
    pSender->iBufLens = iLens + sizeof(stHost);

    addTask(pSender);
}

void CSender::sendUBinary(H_SOCK sock, const char *pstHost, unsigned short usPort, CBinary *pBinary)
{
    std::string strBuf = pBinary->getWritedBuf();
    sendU(sock, pstHost, usPort, strBuf.c_str(), strBuf.size());
}

void CSender::broadCastU(H_SOCK sock, unsigned short usPort, const char *pBuf, const size_t iLens)
{
    H_Sender *pSender = newT();
    char *pNewBuf = new(std::nothrow) char[iLens];
    H_ASSERT(NULL != pNewBuf, "malloc memory error.");

    memcpy(pNewBuf, pBuf, iLens);
    pSender->usCmd = UBroadCast;
    pSender->stSock.sock = sock;
    pSender->pBuffer = pNewBuf;
    pSender->iBufLens = iLens;
    pSender->usCount = usPort;

    addTask(pSender);
}

void CSender::broadCastUBinary(H_SOCK sock, unsigned short usPort, CBinary *pBinary)
{
    std::string strBuf = pBinary->getWritedBuf();
    broadCastU(sock, usPort, strBuf.c_str(), strBuf.size());
}

void CSender::addSock(H_SOCK &sock, const unsigned int &uiSession, unsigned short &usType)
{
    H_TypeSession stInfo;
    stInfo.uiSession = uiSession;
    stInfo.usType = usType;

    m_objLck.wLock();
    m_mapSender[sock] = stInfo;
    m_objLck.unLock();
}

void CSender::delSock(H_SOCK &sock)
{
    m_objLck.wLock();
    senderit itSender = m_mapSender.find(sock);
    if (m_mapSender.end() != itSender)
    {        
        m_mapSender.erase(itSender);
    }
    m_objLck.unLock();
}

H_ENAMSP
