
#ifndef H_SVINTF_H_
#define H_SVINTF_H_

#include "Macros.h"

H_BNAMSP

class CSVIntf
{
public:
    CSVIntf(void) {};
    virtual ~CSVIntf(void) {};

    virtual void onStart(void) {};
    virtual void onStop(void) {};
    virtual void onTcpLinked(struct H_Session *pSession) {};
    virtual void onTcpClose(struct H_Session *pSession) {};
    virtual void onTcpRead(struct H_Session *pSession) {};
    virtual void onTime(const unsigned int &uiTick, const unsigned int &uiCount) {};
};

H_ENAMSP

#endif//H_SVINTF_H_
