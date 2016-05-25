
#ifndef H_TICK_H_
#define H_TICK_H_

#include "NetBase.h"
#include "Singleton.h"
#include "Chan.h"
#include "SVIntf.h"

H_BNAMSP

//定时器服务
class CTick : public CNetBase, public CSingleton<CTick>
{
public:
    CTick(void);
    ~CTick(void);

    void onStart(void);
    void onReadyStop(void);

    void setIntf(class CSVIntf *pIntf)
    {
        m_pIntf = pIntf;
    };
    class CSVIntf * getIntf(void)
    {
        return m_pIntf;
    };

    void setTick(const unsigned int uiMS);
    const unsigned int getTick(void);

    void addTickCount(void);
    unsigned int getTickCount(void);

    void setThreadNum(const unsigned short usNum);
    void Monitor(void);
    void monitorTrigger(const unsigned short &usIndex, const char *pName);

public:
    static void timeCB(H_SOCK, short, void *arg);
    static void monitorCB(H_SOCK, short, void *arg);

private:
    H_DISALLOWCOPY(CTick);
    struct event *initTimeEv(const unsigned int uiMS, event_callback_fn func);
    struct ThreadMonitor
    {
        unsigned int uiVersion;
        unsigned int uiCheckVersion;
        char *pName;
        ThreadMonitor(void) : uiVersion(H_INIT_NUMBER), uiCheckVersion(H_INIT_NUMBER), pName(NULL)
        {};
    };

private:
    unsigned short m_usMonitorNum;
    unsigned int m_uiTick;
    unsigned int m_uiTickCount;
    struct event *m_pTickEvent;
    struct event *m_pMonitorEv;
    CSVIntf *m_pIntf;
    ThreadMonitor *m_pMonitor;
};

H_ENAMSP

#endif //H_TICK_H_
