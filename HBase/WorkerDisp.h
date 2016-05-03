
#ifndef H_WORKERDISP_H_
#define H_WORKERDISP_H_

#include "Chan.h"
#include "Worker.h"
#include "Singleton.h"
#include "Funcs.h"
#include "RWLock.h"

H_BNAMSP

class CWorkerDisp : public CTask, public CSingleton<CWorkerDisp>
{
public:
    CWorkerDisp(void);
    ~CWorkerDisp(void);

    CChan *regSendChan(const char *pszChanName, const char *pszTaskName);
    CChan *regRecvChan(const char *pszChanName, const char *pszTaskName);
    CChan *getChan(const char *pszChanName);

    void setThreadNum(const unsigned short usNum);

    void regTask(const char *pszName, CWorkerTask *pTask);

    void Run(void);
    void Join(void);
    void waitStart(void);

    void Notify(std::string *pstrName);

private:
    CWorker *getFreeWorker(void);
    CWorkerTask* getWorkerTask(std::string *pstrName);
    void stopNet(void);
    void stopWorker(void);
    void runSurpTask(void);
    void destroyTask(void);
    void closeChan(void);

private:
    H_DISALLOWCOPY(CWorkerDisp);
#ifdef H_OS_WIN 
    #define chanit std::unordered_map<std::string, CChan*>::iterator
    #define chan_map std::unordered_map<std::string, CChan*>
    #define taskit std::unordered_map<std::string, CWorkerTask*>::iterator
    #define task_map std::unordered_map<std::string, CWorkerTask*>
#else
    #define chanit std::tr1::unordered_map<std::string, CChan*>::iterator
    #define chan_map std::tr1::unordered_map<std::string, CChan*>
    #define taskit std::tr1::unordered_map<std::string, CWorkerTask*>::iterator
    #define task_map std::tr1::unordered_map<std::string, CWorkerTask*>
#endif
    enum
    {
        RS_RUN = 0,
        RS_STOP,
    };

private:
    unsigned short m_usThreadNum;
    long m_lExit;
    long m_lCount;
    CWorker *m_pWorker;
    CRWLock m_objChanLock;
    chan_map m_mapChan;
    task_map m_mapTask;
    pthread_mutex_t m_taskLock;
    pthread_cond_t m_taskCond;
    std::queue<std::string*> m_quTask;
};

H_ENAMSP

#endif //H_WORKERDISP_H_
