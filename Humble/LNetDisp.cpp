
#include "LNetDisp.h"

H_BNAMSP

CLNetDisp::CLNetDisp(void) : m_iBufLens(H_INIT_NUMBER), m_iParsed(H_INIT_NUMBER),
    m_pBuf(NULL), m_pLState(NULL), m_pLFunc(NULL), m_pTable(NULL), m_pSockTable(NULL), m_pParser(NULL)
{
    m_pLFunc = new(std::nothrow) luabridge::LuaRef *[LCount];
    H_ASSERT(NULL != m_pLFunc, "malloc memory error.");    

    m_pLState = luaL_newstate();
    H_ASSERT(NULL != m_pLState, "luaL_newstate error.");
    luaL_openlibs(m_pLState);

    luabridge::LuaRef *pRef = NULL;
    for (int i = 0; i < LCount; ++i)
    {
        pRef = new(std::nothrow) luabridge::LuaRef(m_pLState);
        H_ASSERT(NULL != pRef, "malloc memory error.");
        m_pLFunc[i] = pRef;
    }

    m_pTable = new(std::nothrow) luabridge::LuaRef(m_pLState);
    H_ASSERT(NULL != m_pTable, "malloc memory error.");
    *m_pTable = luabridge::newTable(m_pLState);

    m_pSockTable = new(std::nothrow) luabridge::LuaRef(m_pLState);
    H_ASSERT(NULL != m_pSockTable, "malloc memory error.");
    *m_pSockTable = luabridge::newTable(m_pLState);

    try
    {
        H_RegAll(m_pLState);
        luabridge::setGlobal(m_pLState, *m_pTable, "g_pBuffer");
        luabridge::setGlobal(m_pLState, *m_pSockTable, "g_pCurSock");
    }
    catch (luabridge::LuaException &e)
    {
        H_ASSERT(false, e.what());
    }

    std::string strLuaFile = g_strScriptPath +  "start.lua";
    if (H_RTN_OK != luaL_dofile(m_pLState, strLuaFile.c_str()))
    {
        const char *pError = lua_tostring(m_pLState, -1);
        H_ASSERT(false, pError);
    }

    *(m_pLFunc[LOnstart]) = luabridge::getGlobal(m_pLState, "onStart");
    *(m_pLFunc[LOnStop]) = luabridge::getGlobal(m_pLState, "onStop");
    *(m_pLFunc[LOnTcpLinked]) = luabridge::getGlobal(m_pLState, "onTcpLinked");
    *(m_pLFunc[LOnTcpClose]) = luabridge::getGlobal(m_pLState, "onTcpClose");
    *(m_pLFunc[LOnTcpRead]) = luabridge::getGlobal(m_pLState, "onTcpRead");
}

CLNetDisp::~CLNetDisp(void)
{
    if (NULL != m_pLFunc)
    {
        luabridge::LuaRef *pRef = NULL;
        for (int i = 0; i < LCount; ++i)
        {
            pRef = m_pLFunc[i];
            H_SafeDelete(pRef);
        }
        H_SafeDelArray(m_pLFunc);
    }

    H_SafeDelete(m_pSockTable);
    H_SafeDelete(m_pTable);

    if (NULL != m_pLState)
    {
        lua_close(m_pLState);
        m_pLState = NULL;
    }
}

void CLNetDisp::initCurSock(struct H_Session *pSession)
{
    (*m_pSockTable)[1] = pSession->sock;
    (*m_pSockTable)[2] = pSession->uiSession;
    (*m_pSockTable)[3] = pSession->usSockType;
}

void CLNetDisp::onStart(void)
{
    try
    {
        (*(m_pLFunc[LOnstart]))();
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

double dTime = 0.0;
int iCount = 0;

void CLNetDisp::onStop(void)
{
    try
    {
        H_Printf("%f", dTime);
        H_Printf("%d", iCount);
        (*(m_pLFunc[LOnStop]))();
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

H_INLINE void CLNetDisp::onTcpLinked(struct H_Session *pSession)
{
    try
    {
        initCurSock(pSession);
        (*(m_pLFunc[LOnTcpLinked]))();
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

H_INLINE void CLNetDisp::onTcpClose(struct H_Session *pSession)
{
    m_pParser = CNetParser::getSingletonPtr()->getParser(pSession->usSockType);
    if (NULL == m_pParser)
    {
        H_LOG(LOGLV_ERROR, "get parser by type %d error.", pSession->usSockType);
        return;
    }

    m_pParser->onClose(pSession);

    try
    {
        initCurSock(pSession);
        (*(m_pLFunc[LOnTcpClose]))();
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

H_INLINE void CLNetDisp::onTcpRead(struct H_Session *pSession)
{
    m_objClock.reStart();

    m_pParser = CNetParser::getSingletonPtr()->getParser(pSession->usSockType);
    if (NULL == m_pParser)
    {
        H_LOG(LOGLV_ERROR, "get parser by type %d error.", pSession->usSockType);
        return;
    }

    m_objEvBuffer.setEvBuf(pSession->pBev);
    m_iBufLens = m_objEvBuffer.getTotalLens();
    m_pBuf = m_objEvBuffer.readBuffer(m_iBufLens);
    if (NULL == m_pBuf)
    {
        return;
    }

    try
    {
        m_iParsed = m_pParser->parsePack(pSession, m_pBuf, m_iBufLens, m_pTable);
        m_objEvBuffer.delBuffer(m_iParsed);

        if (H_INIT_NUMBER != m_iParsed)
        {
            initCurSock(pSession);
            (*(m_pLFunc[LOnTcpRead]))();
        }        
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }

    dTime += m_objClock.Elapsed();
    ++iCount;
}

H_ENAMSP
