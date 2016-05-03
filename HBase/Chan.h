
#ifndef H_CHAN_H_
#define H_CHAN_H_

#include "Atomic.h"

H_BNAMSP

class CChan
{
public:
    CChan(void);
    ~CChan(void);

    void Send(void *pszVal);
    void *Recv(void);

    void Close(void);

    void setSendTaskNam(const char *pszName);
    void setRecvTaskNam(const char *pszName);
    void setChanNam(const char *pszName);

private:
    H_DISALLOWCOPY(CChan);

private:
    unsigned int m_uiClose;
    std::queue<void *> m_quData;
    CAtomic m_quLock;
    std::string m_strSendTask;
    std::string m_strRecvTask;
    std::string m_strChanNam;
};

H_ENAMSP

#endif//H_CHAN_H_
