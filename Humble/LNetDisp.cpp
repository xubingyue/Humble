
#include "LNetDisp.h"

H_BNAMSP

CLNetDisp::CLNetDisp(void) : /*m_iFromLens(sizeof(m_stFromAddr)),*/ m_pLState(NULL), m_pLFunc(NULL)
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

    std::string strLuaFile = g_strScriptPath +  "netdisp.lua";
    if (H_RTN_OK != luaL_dofile(m_pLState, strLuaFile.c_str()))
    {
        const char *pError = lua_tostring(m_pLState, -1);
        H_ASSERT(false, pError);
    }

    *(m_pLFunc[LOnstart]) = luabridge::getGlobal(m_pLState, "onstart");
    *(m_pLFunc[LOnTimer]) = luabridge::getGlobal(m_pLState, "ontimer");
    *(m_pLFunc[LOnTcpLinked]) = luabridge::getGlobal(m_pLState, "ontcplinked");
    *(m_pLFunc[LOnTcpClose]) = luabridge::getGlobal(m_pLState, "ontcpclose");
    *(m_pLFunc[LOnTcpRead]) = luabridge::getGlobal(m_pLState, "ontcpread");
    //*(m_pLFunc[LOnUdpRead]) = luabridge::getGlobal(m_pLState, "onUdpRead");
}

CLNetDisp::~CLNetDisp(void)
{
    if (NULL != m_pLFunc)
    {
        luabridge::LuaRef *pRef = NULL;
        for (int i = 0; i < LCount; i++)
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

void CLNetDisp::onTimer(const unsigned uiTick, const unsigned uiCount)
{
    try
    {
        (*(m_pLFunc[LOnTimer]))(uiTick, uiCount);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLNetDisp::onTcpLinked(struct H_Session *pSession)
{
    try
    {
        (*(m_pLFunc[LOnTcpLinked]))(pSession);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLNetDisp::onTcpClose(struct H_Session *pSession)
{
    try
    {
        (*(m_pLFunc[LOnTcpClose]))(pSession);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLNetDisp::onTcpRead(struct H_Session *pSession)
{
    try
    {
        m_objTcpEvBuffer.setEvBuf(pSession->pBev);
        (*(m_pLFunc[LOnTcpClose]))(pSession);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

//void CLNetDisp::onUdpRead(H_SOCK &sock, const unsigned short &usType)
//{
//    int iLens = recvfrom(sock, m_acUdpBuffer, sizeof(m_acUdpBuffer), 0, &m_stFromAddr, &m_iFromLens);
//    if (iLens < 0)
//    {
//        return;
//    }
//
//    CNETAddr objAddr;
//    objAddr.setAddr(&m_stFromAddr);
//    m_objUdpBinary.setReadBuffer(m_acUdpBuffer, iLens);
//
//    try
//    {
//        (*(m_pLFunc[LOnTcpClose]))(sock, usType, objAddr.getIp(), objAddr.getPort(), &m_objUdpBinary);
//    }
//    catch (luabridge::LuaException &e)
//    {
//        H_LOG(LOGLV_ERROR, "%s", e.what());
//    }
//}

H_ENAMSP
