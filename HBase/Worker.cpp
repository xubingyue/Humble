
#include "Worker.h"

H_BNAMSP

CWorker::CWorker(void) : m_uiStatus(WS_FREE)
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
    pMsg->Run();
    H_AtomicSet(&m_uiStatus, WS_FREE);
    pMsg->subRef();
}

unsigned int CWorker::getStatus(void)
{
    return H_AtomicGet(&m_uiStatus);
}

H_ENAMSP
