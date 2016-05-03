
#ifndef H_NETWORKER_H_
#define H_NETWORKER_H_

#include "NetBase.h"
#include "NetIntf.h"
#include "Singleton.h"

H_BNAMSP

struct H_TcpLink
{
    unsigned short usSockType;
    unsigned short usPort;
    unsigned int uiID;
    H_SOCK sock;
    char acHost[H_IPLENS];
};

class CNetWorker : public CNetBase, public CSingleton<CNetWorker>
{
public:
    CNetWorker(void);
    ~CNetWorker(void);

    void onOrder(H_Order *pOrder);
    void onStart(void);
    void onReadyStop(void);
    void onClose(H_Session *pSession);
    void onLinked(H_Session *pSession);
    void onRead(H_Session *pSession);

    void setIntf(CNetIntf *pIntf);
    CNetIntf *getIntf(void);    

    void tcpListen(const unsigned short usSockType, const char *pszHost, const unsigned short usPort);
    void addTcpLink(const unsigned short usSockType, const char *pszHost, const unsigned short usPort);
    void addLinkEv(const H_SOCK &sock, const unsigned int &uiID);

    void addMapListener(H_SOCK &sock, const unsigned short &usSockType);
    bool getListenerType(H_SOCK sock, unsigned short &usType);

public:
    static void acceptCB(struct evconnlistener *pListener, H_SOCK sock, struct sockaddr *,
        int, void *arg);
    static void monitorCB(H_SOCK, short, void *arg);

private:
    void addTcpListen(H_Order *pOrder);
    struct event *monitorLink(H_TcpLink *pTcpLink);

private:
    H_DISALLOWCOPY(CNetWorker);
#ifdef H_OS_WIN 
    #define listenerit std::unordered_map<H_SOCK, unsigned short>::iterator
    #define listener_map std::unordered_map<H_SOCK, unsigned short>
#else
    #define listenerit std::tr1::unordered_map<H_SOCK, unsigned short>::iterator
    #define listener_map std::tr1::unordered_map<H_SOCK, unsigned short>
#endif

private:
    unsigned int m_uiLinkID;
    CNetIntf *m_pIntf;
    listener_map m_mapLstType;
    std::vector<struct evconnlistener *> m_vcListener;
    std::list<H_TcpLink*> m_lstTcpLink;
    std::list<struct event *> m_lstLinkMonitorEv;
};

H_ENAMSP

#endif//H_NETWORKER_H_
