
#ifndef H_SENDER_H_
#define H_SENDER_H_

#include "RecvTask.h"
#include "Singleton.h"
#include "Binary.h"
#include "RWLock.h"
#include "NETAddr.h"

H_BNAMSP

struct H_SenderSock
{
    unsigned int uiSession;
    H_SOCK sock;
    H_SenderSock(void) : uiSession(H_INIT_NUMBER), sock(H_INVALID_SOCK)
    {};
};
struct H_Sender
{
    unsigned short usCmd;
    unsigned short usCount;
    size_t iBufLens;
    H_SenderSock stSock;
    H_SenderSock *pSock;
    char *pBuffer;
    H_Sender(void) : usCmd(H_INIT_NUMBER), usCount(H_INIT_NUMBER), iBufLens(H_INIT_NUMBER),
        pSock(NULL), pBuffer(NULL)
    {};
};

//ÍøÂç·¢ËÍ·þÎñ
class CSender : public CRecvTask<H_Sender>, public CSingleton<CSender>
{
public:
    CSender(void);
    ~CSender(void);

    void runTask(H_Sender *pMsg);

    void Send(H_SOCK sock, const unsigned int uiSession, const char *pBuf, const size_t iLens);
    void sendBinary(H_SOCK sock, const unsigned int uiSession, CBinary *pBinary);

    //{{sock,session},...}
    void lbroadCast(luabridge::LuaRef lTable, const char *pBuf, const size_t iLens);
    void broadCastBinary(luabridge::LuaRef lTable, CBinary *pBinary);

    //udp
    void sendU(H_SOCK sock, const char *pstHost, unsigned short usPort, 
        const char *pBuf, const size_t iLens);
    void sendUBinary(H_SOCK sock, const char *pstHost, unsigned short usPort, CBinary *pBinary);
    void broadCastU(H_SOCK sock, unsigned short usPort, const char *pBuf, const size_t iLens);
    void broadCastUBinary(H_SOCK sock, unsigned short usPort, CBinary *pBinary);

    void addSock(H_SOCK &sock, const unsigned int &uiSession);
    void delSock(H_SOCK &sock); 

private:
    void sendToSock(H_SOCK &fd, const unsigned int &uiSession, const char *pBuf, const size_t &iLens);
    void broadCast(H_SenderSock *pSock, const int &iCount, const char *pBuf, const size_t &iLens);
    H_SenderSock *createSenderSock(luabridge::LuaRef &lTable);

private:
    H_DISALLOWCOPY(CSender); 
#ifdef H_OS_WIN 
    #define senderit std::unordered_map<H_SOCK, H_SenderSock>::iterator
    #define sender_map std::unordered_map<H_SOCK, H_SenderSock>
#else
    #define senderit std::tr1::unordered_map<H_SOCK, H_SenderSock>::iterator
    #define sender_map std::tr1::unordered_map<H_SOCK, H_SenderSock>
#endif

private:
    sender_map m_mapSender;
    CNETAddr m_objAddr;
};

H_ENAMSP

#endif//H_SENDER_H_
