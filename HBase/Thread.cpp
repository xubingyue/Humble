
#include "Thread.h"

H_BNAMSP

CThread::CThread(void)
{

}

CThread::~CThread(void)
{

}

#ifdef H_OS_WIN
unsigned int __stdcall CThread::threadCB(void *arg)
#else
void *CThread::threadCB(void *arg)
#endif
{
    CTask *pTask = (CTask *)arg;
    pTask->Run();

    return NULL;
}

pthread_t CThread::Creat(CTask *pTask)
{
    H_ASSERT(pTask, "pointer in null.");

    pthread_t pThread;
#ifdef H_OS_WIN
    pThread = (HANDLE)_beginthreadex(NULL, 0, threadCB, pTask, 0, NULL);
    H_ASSERT(pThread, "_beginthreadex error.");
#else
    H_ASSERT((H_RTN_OK == pthread_create(&pThread, NULL, threadCB, pTask)), "pthread_create error.");
#endif

    return pThread;
}

void CThread::Join(pthread_t pThread)
{
#ifdef H_OS_WIN
    (void)WaitForSingleObject(pThread, INFINITE);
#else
    (void)pthread_join(pThread, NULL);
#endif    
}

H_ENAMSP
