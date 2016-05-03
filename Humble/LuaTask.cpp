
#include "LuaTask.h"
#include "Reg2Lua.h"

H_BNAMSP

CLuaTask::CLuaTask(void)
{
    m_pLFunc = new(std::nothrow) luabridge::LuaRef *[LTASK_COUNT];
    H_ASSERT(NULL != m_pLFunc, "malloc memory error.");

    m_pLState = luaL_newstate();
    H_ASSERT(NULL != m_pLState, "luaL_newstate error.");
    luaL_openlibs(m_pLState);

    luabridge::LuaRef *pRef = NULL;
    for (int i = 0; i < LTASK_COUNT; ++i)
    {
        pRef = new(std::nothrow) luabridge::LuaRef(m_pLState);
        H_ASSERT(NULL != pRef, "malloc memory error.");
        m_pLFunc[i] = pRef;
    }

    try
    {
        H_RegAll(m_pLState);
    }
    catch (luabridge::LuaException &e)
    {
        H_ASSERT(false, e.what());
    }
}

CLuaTask::~CLuaTask(void)
{
    if (NULL != m_pLFunc)
    {
        luabridge::LuaRef *pRef = NULL;
        for (int i = 0; i < LTASK_COUNT; ++i)
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

void CLuaTask::initTask(const char *pName)
{
    m_strName = pName;
    std::string strLuaFile = H_FormatStr("%s%s.lua", g_strScriptPath.c_str(), pName);
    if (H_RTN_OK != luaL_dofile(m_pLState, strLuaFile.c_str()))
    {
        const char *pError = lua_tostring(m_pLState, -1);
        H_Printf("%s", pError);

        return;
    }

    *(m_pLFunc[LTASK_INIT]) = luabridge::getGlobal(m_pLState, "initTask");
    *(m_pLFunc[LTASK_RUN]) = luabridge::getGlobal(m_pLState, "runTask");
    *(m_pLFunc[LTASK_DESTROY]) = luabridge::getGlobal(m_pLState, "destroyTask");

    try
    {
        (*(m_pLFunc[LTASK_INIT]))();
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLuaTask::runTask(CChan *pChan)
{
    try
    {
        (*(m_pLFunc[LTASK_RUN]))(pChan);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLuaTask::destroyTask(void)
{
    try
    {
        (*(m_pLFunc[LTASK_DESTROY]))();
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

H_ENAMSP
