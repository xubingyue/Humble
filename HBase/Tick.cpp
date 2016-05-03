
#include "Tick.h"
#include "WorkerDisp.h"

H_BNAMSP

SINGLETON_INIT(CTick)
CTick objTick;

CTick::CTick(void) : m_uiTick(H_INIT_NUMBER), m_uiTickCount(H_INIT_NUMBER), 
    m_pTickEvent(NULL), m_pIntf(NULL)
{

}

CTick::~CTick(void)
{
    if (NULL != m_pTickEvent)
    {
        event_free(m_pTickEvent);
        m_pTickEvent = NULL;
    }
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

void CTick::timeCB(H_SOCK, short, void *arg)
{
    CTick *pTick = (CTick *)arg;
    pTick->addTickCount();
    pTick->getIntf()->onTime(pTick->getTick(), pTick->getTickCount());
}

struct event *CTick::intiTick(const unsigned int &uiMS)
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
        -1, EV_PERSIST, timeCB, this);
    H_ASSERT(NULL != pEvent, "event_new error.");
    (void)event_add(pEvent, &tVal);

    return pEvent;
}

void CTick::onStart(void)
{
    m_pIntf->onStart();
    m_pTickEvent = intiTick(m_uiTick);
}

H_ENAMSP
