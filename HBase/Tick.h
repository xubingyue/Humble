
#ifndef H_TICK_H_
#define H_TICK_H_

#include "NetBase.h"
#include "Singleton.h"
#include "Chan.h"
#include "TickIntf.h"

H_BNAMSP

class CTick : public CNetBase, public CSingleton<CTick>
{
public:
    CTick(void);
    ~CTick(void);

    void onStart(void);

    void setIntf(class CTickIntf *pIntf)
    {
        m_pIntf = pIntf;
    };
    class CTickIntf * getIntf(void)
    {
        return m_pIntf;
    };

    void setTick(const unsigned int uiMS);
    const unsigned int getTick(void);

    void addTickCount(void);
    unsigned int getTickCount(void);

    static void timeCB(H_SOCK, short, void *arg);

private:
    H_DISALLOWCOPY(CTick);
    struct event *intiTick(const unsigned int &uiMS);

private:
    unsigned int m_uiTick;
    unsigned int m_uiTickCount;
    struct event *m_pTickEvent;
    CTickIntf *m_pIntf;
};

H_ENAMSP

#endif //H_TICK_H_
