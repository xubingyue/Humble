
#include "Sender.h"
#include "Funcs.h"

H_BNAMSP

SINGLETON_INIT(CSender)
CSender objSender;

enum
{
    AddSock,
    DelSock,
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
    itSender = m_mapSender.find(fd);
    if (m_mapSender.end() != itSender)
    {
        if (uiSession == itSender->second)
        {
            H_SockWrite(fd, pBuf, iLens);
        }
    }
}

void CSender::runTask(H_Sender *pMsg)
{
    switch (pMsg->usCmd)
    {
        case AddSock:
        {
            m_mapSender[pMsg->stSock.sock] = pMsg->stSock.uiSession;
        }
        break;
        case DelSock:
        {
            senderit itSender;
            itSender = m_mapSender.find(pMsg->stSock.sock);
            if (m_mapSender.end() != itSender)
            {
                if (itSender->second == pMsg->stSock.uiSession)
                {
                    m_mapSender.erase(itSender);
                }
            }
        }
        break;
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

void CSender::lbroadCast(luabridge::LuaRef lTable, const char *pBuf, const size_t iLens)
{
    int iCount(lTable.length());
    if (0 >= iCount)
    {
        return;
    }

    H_SenderSock *pSock = new(std::nothrow) H_SenderSock[iCount];
    H_ASSERT(NULL != pSock, "malloc momory error.");
    for (int i = 1; i <= iCount; ++i)
    {
        luabridge::LuaRef objTmp = lTable[i];
        pSock[i - 1].sock = objTmp[1].cast<H_SOCK>();
        pSock[i - 1].uiSession = objTmp[2].cast<unsigned int>();
    }

    broadCast(pSock, iCount, pBuf, iLens);
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

void CSender::addSock(H_SOCK sock, const unsigned int uiSession)
{
    H_Sender *pSender = newT();

    pSender->usCmd = AddSock;
    pSender->stSock.sock = sock;
    pSender->stSock.uiSession = uiSession;

    addTask(pSender);
}

void CSender::delSock(H_SOCK sock, const unsigned int uiSession)
{
    H_Sender *pSender = newT();

    pSender->usCmd = DelSock;
    pSender->stSock.sock = sock;
    pSender->stSock.uiSession = uiSession;

    addTask(pSender);
}

H_ENAMSP
