
#ifndef H_CHAN_H_
#define H_CHAN_H_

#include "LockThis.h"

H_BNAMSP

class CChan
{
public:
    //0 ²»ÏÞÖÆ
    explicit CChan(const unsigned int &uiCount);
    ~CChan(void);

    void Send(void *pszVal);
    void *Recv(void);

    bool canSend(void);
    bool canRecv(void);

    void setSendTaskNam(const char *pszName);
    void setRecvTaskNam(const char *pszName);
    void setChanNam(const char *pszName);

private:
    H_DISALLOWCOPY(CChan);
    CChan(void);

private:
    unsigned int m_uiCount;
    unsigned int m_uiRWait;
    unsigned int m_uiWWait;
    std::queue<void *> m_quData;
    pthread_mutex_t m_quLock;
    pthread_cond_t m_pWCond;
    pthread_cond_t m_pRCond;
    std::string m_strSendTask;
    std::string m_strRecvTask;
    std::string m_strChanNam;
};

H_ENAMSP

#endif//H_CHAN_H_
