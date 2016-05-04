
#include "Tick.h"
#include "WorkerDisp.h"
#include "Log.h"

H_BNAMSP

SINGLETON_INIT(CTick)
CTick objTick;

CTick::CTick(void) : m_usMonitorNum(H_INIT_NUMBER), m_uiTick(H_INIT_NUMBER), m_uiTickCount(H_INIT_NUMBER),
    m_pTickEvent(NULL), m_pMonitorEv(NULL), m_pIntf(NULL), m_pMonitor(NULL)
{

}

CTick::~CTick(void)
{
    if (NULL != m_pTickEvent)
    {
        event_free(m_pTickEvent);
        m_pTickEvent = NULL;
    }
    if (NULL != m_pMonitorEv)
    {
        event_free(m_pMonitorEv);
        m_pMonitorEv = NULL;
    }

    H_SafeDelArray(m_pMonitor);
}

void CTick::setTick(const unsigned int uiMS)
{
    H_ASSERT(uiMS > 0, "tick must big than zero.");
    m_uiTick = uiMS;
}

const unsigned int CTick::getTick(void)
{
    return m_uiTick;
}

void CTick::addTickCount(void)
{
    ++m_uiTickCount;
}

unsigned int CTick::getTickCount(void)
{
    return m_uiTickCount;
}

void CTick::setThreadNum(const unsigned short usNum)
{
    m_usMonitorNum = usNum;
    m_pMonitor = new(std::nothrow) ThreadMonitor[m_usMonitorNum];
    H_ASSERT(NULL != m_pMonitor, "malloc memory error.");
}

void CTick::monitorTrigger(const unsigned short &usIndex, const char *pName)
{
    ThreadMonitor *pMonitor = &m_pMonitor[usIndex];
    pMonitor->pName = (char*)pName;
    H_AtomicAdd(&(pMonitor->uiVersion), 1);
}

void CTick::Monitor(void)
{
    unsigned int uiVer(H_INIT_NUMBER);
    ThreadMonitor *pMonitor = NULL;

    for (unsigned short usI = H_INIT_NUMBER;  usI < m_usMonitorNum; ++usI)
    {
        pMonitor = &m_pMonitor[usI];
        uiVer = H_AtomicGet(&(pMonitor->uiVersion));
        if (uiVer == pMonitor->uiCheckVersion)
        {
            if (NULL != pMonitor->pName)
            {
                H_LOG(LOGLV_WARN, "task %s maybe in an endless loop.", pMonitor->pName);
            }
        }
        else
        {
            pMonitor->uiCheckVersion = uiVer;
        }
    }
}

void CTick::timeCB(H_SOCK, short, void *arg)
{
    CTick *pTick = (CTick *)arg;
    pTick->addTickCount();
    pTick->getIntf()->onTime(pTick->getTick(), pTick->getTickCount());
}

void CTick::monitorCB(H_SOCK, short, void *arg)
{
    CTick *pTick = (CTick *)arg;
    pTick->Monitor();
}

struct event *CTick::initTimeEv(const unsigned int uiMS, event_callback_fn func)
{
    timeval tVal;
    evutil_timerclear(&tVal);
    if (uiMS >= 1000)
    {
        tVal.tv_sec = uiMS / 1000;
        tVal.tv_usec = (uiMS % 1000) * (1000);
    }
    else
    {
        tVal.tv_usec = (uiMS * 1000);
    }

    struct event *pEvent = event_new(getBase(),
        -1, EV_PERSIST, func, this);
    H_ASSERT(NULL != pEvent, "event_new error.");
    (void)event_add(pEvent, &tVal);

    return pEvent;
}

void CTick::onStart(void)
{
    m_pIntf->onStart();
    m_pTickEvent = initTimeEv(m_uiTick, timeCB);
    m_pMonitorEv = initTimeEv(5000, monitorCB);
}

void CTick::onReadyStop(void)
{
    m_pIntf->onStop();
}

H_ENAMSP
