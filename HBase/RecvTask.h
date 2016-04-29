
#ifndef H_SERVICETASK_H_
#define H_SERVICETASK_H_

#include "Task.h"
#include "LockThis.h"

H_BNAMSP

//接收型服务基类
template <typename T>
class CRecvTask : public CTask
{
public:
    CRecvTask(void);
    ~CRecvTask(void);

    void Run(void);
    virtual void runTask(T *pMsg) = 0;
    
    void Join(void);
    void waitStart(void);

protected:
    void addTask(T *pMsg);
    T *newT(void);
    void setDel(const bool bDel);

private:
    H_DISALLOWCOPY(CRecvTask);
    enum
    {
        CANADD = 0,
        CANTADD,
    };
    enum
    {
        RS_RUN = 0,
        RS_STOP,
    };

private:
    bool m_bDel;
    long m_lExit;
    long m_lCanAdd;
    long m_lCount;
    std::queue<T*> m_vcTask;
    pthread_mutex_t m_quLock;
    pthread_cond_t m_objCond;
};

template <typename T>
CRecvTask<T>::CRecvTask(void) : m_bDel(true), m_lExit(RS_RUN), m_lCanAdd(CANADD),
    m_lCount(H_INIT_NUMBER)
{
    pthread_mutex_init(&m_quLock, NULL);
    pthread_cond_init(&m_objCond, NULL);
}

template <typename T>
CRecvTask<T>::~CRecvTask(void)
{
    Join();
    pthread_cond_destroy(&m_objCond);
    pthread_mutex_destroy(&m_quLock);
}

template <typename T>
void CRecvTask<T>::Run(void)
{
    T *pNode = NULL;
    H_AtomicAdd(&m_lCount, 1);

    while(RS_RUN == H_AtomicGet(&m_lExit))
    {
        if (NULL != pNode)
        {
            runTask(pNode);
            if (m_bDel)
            {
                H_SafeDelete(pNode);
            }

            pNode = NULL;
        }

        CLckThis objLckThis(&m_quLock);
        if (!m_vcTask.empty())
        {
            pNode = m_vcTask.front();
            m_vcTask.pop();
        }
        else
        {
            pthread_cond_wait(&m_objCond, objLckThis.getMutex());
        }
    }

    H_AtomicAdd(&m_lCount, -1);
}

template <typename T>
void CRecvTask<T>::waitStart(void)
{
    for (;;)
    {
        if (H_INIT_NUMBER != H_AtomicGet(&m_lCount))
        {
            return;
        }

        H_Sleep(10);
    }
}

template <typename T>
void CRecvTask<T>::addTask(T *pMsg)
{
    if (CANADD != H_AtomicGet(&m_lCanAdd))
    {
        return;
    }

    H_ASSERT(NULL != pMsg, "pointer is null.");

    CLckThis objLckThis(&m_quLock);
    m_vcTask.push(pMsg);
    pthread_cond_signal(&m_objCond);
}

template <typename T>
T *CRecvTask<T>::newT(void)
{
    return new T;
}

template <typename T>
void CRecvTask<T>::setDel(const bool bDel)
{
    m_bDel = bDel;
}

template <typename T>
void CRecvTask<T>::Join(void)
{
    if (RS_RUN != H_AtomicGet(&m_lExit))
    {
        return;
    }

    //等待任务队列完成
    H_AtomicSet(&m_lCanAdd, CANTADD);
    H_AtomicSet(&m_lExit, RS_STOP);
    for(;;)
    {
        pthread_cond_broadcast(&m_objCond);
        if (H_INIT_NUMBER == H_AtomicGet(&m_lCount))
        {
            break;
        }

        H_Sleep(10);
    }
}

H_ENAMSP

#endif//H_SERVICETASK_H_
