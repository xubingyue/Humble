
#include "LNetDisp.h"

H_BNAMSP

CLNetDisp::CLNetDisp(void) : m_pLState(NULL), m_pLFunc(NULL)
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

    try
    {
        H_RegAll(m_pLState);
        luabridge::setGlobal(m_pLState, &m_objTcpEvBuffer, "g_pTcpBuffer");
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

    if (NULL != m_pLState)
    {
        lua_close(m_pLState);
        m_pLState = NULL;
    }
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

void CLNetDisp::onStop(void)
{
    try
    {
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
        (*(m_pLFunc[LOnTcpLinked]))(pSession->sock, pSession->uiSession, pSession->usSockType);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

H_INLINE void CLNetDisp::onTcpClose(struct H_Session *pSession)
{
    try
    {
        (*(m_pLFunc[LOnTcpClose]))(pSession->sock, pSession->uiSession, pSession->usSockType);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

H_INLINE void CLNetDisp::onTcpRead(struct H_Session *pSession)
{
    try
    {
        m_objTcpEvBuffer.setEvBuf(pSession->pBev);
        (*(m_pLFunc[LOnTcpRead]))(pSession->sock, pSession->uiSession, pSession->usSockType);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

H_ENAMSP
