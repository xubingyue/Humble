
#include "Sender.h"
#include "Funcs.h"

H_BNAMSP

SINGLETON_INIT(CSender)
CSender objSender;

enum
{
    SendTo,
    BroadCast,
};

CSender::CSender(void)
{

}

CSender::~CSender(void)
{

}

void CSender::sendToSock(H_SOCK &fd, const unsigned int &uiSession, const char *pBuf, const size_t &iLens)
{
    senderit itSender;

    m_objLck.rLock();
    itSender = m_mapSender.find(fd);
    if (m_mapSender.end() != itSender)
    {
        if (uiSession == itSender->second)
        {
            H_SockWrite(fd, pBuf, iLens);
        }
    }
    m_objLck.unLock();
}

void CSender::runTask(H_Sender *pMsg)
{
    switch (pMsg->usCmd)
    {
        case SendTo:
        {
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
                sendToSock(pMsg->pSock[usI].sock, pMsg->pSock[usI].uiSession, pMsg->pBuffer, pMsg->iBufLens);
            }

            H_SafeDelArray(pMsg->pSock);
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
    Send(sock, uiSession, pBinary->getWritedBuf().c_str(), pBinary->getWritedBuf().size());
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

    broadCast(pSock, lTable.length(), pBinary->getWritedBuf().c_str(), pBinary->getWritedBuf().size());
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

void CSender::addSock(H_SOCK &sock, const unsigned int &uiSession)
{
    m_objLck.wLock();
    m_mapSender[sock] = uiSession;
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
