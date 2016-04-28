#include "LNetDisp.h"
using namespace Humble;

#ifdef H_OS_WIN
#include "../vld/vld.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libevent.lib")
#pragma comment(lib, "libevent_core.lib")
#pragma comment(lib, "libevent_extras.lib")
#ifdef H_MYSQL
#pragma comment(lib, "libmysql.lib")
#endif
#pragma comment(lib, "HBase.lib")
#endif

std::string g_strProPath;
std::string g_strScriptPath;
pthread_cond_t g_ExitCond;
pthread_mutex_t g_objExitMu;
CCoreDump m_objDump();

void freeCondMu(void)
{
    pthread_mutex_destroy(&g_objExitMu);
    pthread_cond_destroy(&g_ExitCond);
}

#ifdef H_OS_WIN
BOOL WINAPI consoleHandler(DWORD msgType)
{
    BOOL bRtn = FALSE;

    switch (msgType)
    {
        //CTRL+C
    case CTRL_C_EVENT:
        bRtn = TRUE;
        break;

        //关闭控制台
    case CTRL_CLOSE_EVENT:
        bRtn = TRUE;
        break;

        //CTRL+BREAK 
    case CTRL_BREAK_EVENT:
        bRtn = TRUE;
        break;

        //用户退出
    case CTRL_LOGOFF_EVENT:
        bRtn = TRUE;
        break;

        //系统被关闭
    case CTRL_SHUTDOWN_EVENT:
        bRtn = TRUE;
        break;

    default:
        bRtn = FALSE;
        break;
    }

    if (bRtn)
    {
        CLckThis objLckThis(&g_objExitMu);
        pthread_cond_signal(&g_ExitCond);
    }

    return bRtn;
}
#else
void sigHandEntry(int iSigNum)
{
    H_LOG(LOGLV_INFO, "catch signal %d.", iSigNum);

    CLckThis objLckThis(&g_objExitMu);
    pthread_cond_signal(&g_ExitCond);
}
#endif

int main(int argc, char *argv[])
{
    g_strProPath = H_GetProPath();
    g_strScriptPath = H_FormatStr("%s%s%s", g_strProPath.c_str(), "script", H_PATH_SEPARATOR);
#ifdef H_OS_WIN
    (void)SetConsoleCtrlHandler((PHANDLER_ROUTINE)consoleHandler, TRUE);
#else
    signal(SIGPIPE, SIG_IGN);//若某一端关闭连接，而另一端仍然向它写数据，第一次写数据后会收到RST响应，此后再写数据，内核将向进程发出SIGPIPE信号
    signal(SIGINT, sigHandEntry);//终止进程
    signal(SIGHUP, sigHandEntry);//终止进程
    signal(SIGTSTP, sigHandEntry);//ctrl+Z
    signal(SIGTERM, sigHandEntry);//终止一个进程
    signal(SIGKILL, sigHandEntry);//立即结束程序
    signal(SIGABRT, sigHandEntry);//中止一个程序
    signal(H_SIGNAL_EXIT, sigHandEntry);
    H_Printf("exit service by command \"kill -%d %d\".", H_SIGNAL_EXIT, getpid());
#endif

    pthread_cond_init(&g_ExitCond, NULL);
    pthread_mutex_init(&g_objExitMu, NULL);
    atexit(freeCondMu);

    std::string strLuaFile = g_strScriptPath + "start.lua";
    lua_State *pLState = luaL_newstate();
    H_ASSERT(NULL != pLState, "luaL_newstate error.");
    luaL_openlibs(pLState);
    H_RegAll(pLState);
    if (H_RTN_OK != luaL_dofile(pLState, strLuaFile.c_str()))
    {
        H_Printf("%s", lua_tostring(pLState, -1));
        lua_close(pLState);
        H_ASSERT(false, "do lua file start.lua error.");
    }
    lua_close(pLState);
    
    CLNetDisp objNetIntf;
    CLog *pLog = CLog::getSingletonPtr();
    CMail *pMail = CMail::getSingletonPtr();
    CNetWorker *pNet = CNetWorker::getSingletonPtr();
    CSender *pSender = CSender::getSingletonPtr();
    CWorkerDisp *pWorker = CWorkerDisp::getSingletonPtr();

    pNet->setIntf(&objNetIntf);

    CThread::Creat(pLog);
    pLog->waitStart();
    if (pMail->getSetted())
    {
        CThread::Creat(pMail);
        pMail->waitStart();
    }
    CThread::Creat(pSender);
    pSender->waitStart();
    CThread::Creat(pWorker);
    pWorker->waitStart();
    CThread::Creat(pNet);
    pNet->waitStart();

    CThread::Creat(CWorkerDisp::getSingletonPtr());
    {
        CLckThis objLckThis(&g_objExitMu);
        pthread_cond_wait(&g_ExitCond, objLckThis.getMutex());
    }
    
    pWorker->Join();
    pNet->Join();
    pSender->Join();
    if (pMail->getSetted())
    {
        pMail->Join();
    }
    pLog->Join();

    return H_RTN_OK;
}
