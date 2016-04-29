
#ifndef H_NETWORKER_H_
#define H_NETWORKER_H_

#include "NetIntf.h"
#include "Task.h"
#include "Singleton.h"

H_BNAMSP

struct H_Session
{
    unsigned short usSockType;
    unsigned short usStatus;
    unsigned int uiSession;
    struct bufferevent *pBev;
    class CNetWorker *pNetWorker;
    H_SOCK sock;
    uint64_t uiID;
    H_Session(void) : usSockType(H_INIT_NUMBER), usStatus(H_INIT_NUMBER), uiSession(H_INIT_NUMBER),
        pBev(NULL), pNetWorker(NULL), sock(H_INVALID_SOCK), uiID(H_INIT_NUMBER)
    { };
};

//准备停止
enum
{
    RSTOP_NONE = 0,//初始状态
    RSTOP_RUN,//准备执行
    RSTOP_RAN,//已经执行
};

class CNetWorker : public CTask, public CSingleton<CNetWorker>
{
public:
    CNetWorker(void);
    ~CNetWorker(void);

    void setTick(const unsigned int uiMS);
    const unsigned int getTick(void);
    void setIntf(CNetIntf *pIntf);
    CNetIntf *getIntf(void);
    bool Exit(void);
    struct event_base * getBase(void);
    void addTick(void);
    unsigned int getTickCount(void);

    void Run(void);
    void Join(void);
    void waitStart(void);
    void setReadyStop(const unsigned int iVal);
    unsigned int getReadyStop(void);

    bool tcpListen(const unsigned short usSockType, const char *pszHost, const unsigned short usPort);
    H_SOCK addTcpLink(const unsigned short usSockType, const char *pszHost, const unsigned short usPort);
    void closeSock(const H_SOCK sock, const unsigned int uiSession);

    void addMapListener(H_SOCK &sock, const unsigned short &usSockType);
    bool getListenerType(H_SOCK sock, unsigned short &usType);
    unsigned int getSession(void);
    H_Session *addTcpEv(H_SOCK &sock, const unsigned short &usSockType);
    H_Session *getSession(const H_SOCK &sock);
    void delSession(H_SOCK &sock);

public:
    static void timeCB(H_SOCK, short, void *arg);
    static void acceptCB(struct evconnlistener *pListener, H_SOCK sock, struct sockaddr *,
        int, void *arg);
    static void tcpReadCB(struct bufferevent *bev, void *arg);
    static void tcpEventCB(struct bufferevent *bev, short, void *arg);

private:
    struct event *intiTick(const unsigned int &uiMS);
    void addSession(H_SOCK &sock, H_Session *pSesson);
    void freeSession(H_Session *pSession);
    H_SOCK initSock(const char *pszHost, const unsigned short &usPort);

private:
    H_DISALLOWCOPY(CNetWorker);
    enum
    {
        RS_RUN = 0,
        RS_STOP,
    };    
#ifdef H_OS_WIN 
    #define listenerit std::unordered_map<H_SOCK, unsigned short>::iterator
    #define listener_map std::unordered_map<H_SOCK, unsigned short>
    #define sessionit std::unordered_map<H_SOCK, H_Session *>::iterator
    #define session_map std::unordered_map<H_SOCK, H_Session *>
#else
    #define listenerit std::tr1::unordered_map<H_SOCK, unsigned short>::iterator
    #define listener_map std::tr1::unordered_map<H_SOCK, unsigned short>
    #define sessionit std::tr1::unordered_map<H_SOCK, H_Session *>::iterator
    #define session_map std::tr1::unordered_map<H_SOCK, H_Session *>
#endif

private:
    unsigned int m_uiExit;
    unsigned int m_uiCount;
    unsigned int m_uiTick;
    unsigned int m_uiTickCount;
    unsigned int m_uiSession;
    unsigned int m_uiReadyStop;
    struct event_base *m_pBase;
    struct event *m_pTickEvent;
    CNetIntf *m_pIntf;
    listener_map m_mapLstType;
    session_map m_mapSession;
    std::vector<struct evconnlistener *> m_vcListener;
    std::vector<struct event *> m_vcUdpListener;
    std::vector<H_SOCK> m_vcUdpSock;
};

H_ENAMSP

#endif//H_NETWORKER_H_
