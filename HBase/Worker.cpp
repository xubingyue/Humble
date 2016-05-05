
#include "Worker.h"
#include "Tick.h"

H_BNAMSP

CWorker::CWorker(void) : m_usIndex(H_INIT_NUMBER), m_uiStatus(WS_FREE)
{
    setDel(false);
}

CWorker::~CWorker(void)
{

}

void CWorker::setBusy(void)
{
    H_AtomicSet(&m_uiStatus, WS_BUSY);
}

void CWorker::runTask(CWorkerTask *pMsg)
{
    CTick::getSingletonPtr()->monitorTrigger(m_usIndex, pMsg->getName());
    pMsg->Run();
    CTick::getSingletonPtr()->monitorTrigger(m_usIndex, NULL);
    H_AtomicSet(&m_uiStatus, WS_FREE);
    pMsg->subRef();
}

unsigned int CWorker::getStatus(void)
{
    return H_AtomicGet(&m_uiStatus);
}

H_ENAMSP
