#ifndef H_TICKINTF_H_
#define H_TICKINTF_H_

#include "Macros.h"

H_BNAMSP

class CTickIntf
{
public:
    CTickIntf(void) {};
    virtual ~CTickIntf(void) {};
    virtual void onStart(void)
    {

    };
    virtual void onTime(const unsigned int &uiTick, const unsigned int &uiCount)
    {

    };
};

H_ENAMSP

#endif//H_TICKINTF_H_
