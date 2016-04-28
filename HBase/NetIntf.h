
#ifndef H_NETINTF_H_
#define H_NETINTF_H_

#include "Macros.h"

H_BNAMSP

class CNetIntf
{
public:
    CNetIntf(void) {};
    virtual ~CNetIntf(void) {};

    virtual void onStart(void)
    {

    };
    virtual void onTimer(const unsigned uiTick, const unsigned uiCount)
    {
    };
    virtual void onTcpLinked(struct H_Session *pSession) 
    {
    };
    virtual void onTcpClose(struct H_Session *pSession) 
    {
    };
    virtual void onTcpRead(struct H_Session *pSession) 
    {
    };
    virtual void onUdpRead(H_SOCK &sock, const unsigned short &usType) 
    {
    };
};

H_ENAMSP

#endif//H_NETINTF_H_
