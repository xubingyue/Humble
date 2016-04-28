
#ifndef H_WORKER_H_
#define H_WORKER_H_

#include "RecvTask.h"

H_BNAMSP

enum
{
    WS_FREE = 0,
    WS_BUSY,
};

class CWorkerTask : public CTask
{
public:
    CWorkerTask() : m_uiRef(H_INIT_NUMBER)
    {};
    ~CWorkerTask()
    {};

    void Run(void)
    {
        runTask();
    };

    virtual void initTask(const char *) = 0;
    virtual void runTask(void) = 0;
    virtual void destroyTask(void) = 0;

    void addRef(void)
    {
        H_AtomicAdd(&m_uiRef, 1);
    };
    void subRef(void)
    {
        H_AtomicAdd(&m_uiRef, -1);
    };
    unsigned int getRef(void)
    {
        return H_AtomicGet(&m_uiRef);
    };
    void setName(const char *pszName)
    {
        m_strName = pszName;
    };
    const char * getName(void)
    {
        return m_strName.c_str();
    };

private:
    unsigned int m_uiRef;
    std::string m_strName;
};

class CWorker : public CRecvTask<CWorkerTask>
{
public:
    CWorker(void);
    ~CWorker(void);

    void runTask(CWorkerTask *pMsg);
    void addWorker(CWorkerTask *pWorker)
    {
        pWorker->addRef();
        addTask(pWorker);
    };

    unsigned int getStatus(void);
    void setBusy(void);

private:
    H_DISALLOWCOPY(CWorker);

private:
    unsigned int m_uiStatus;
};

H_ENAMSP

#endif
