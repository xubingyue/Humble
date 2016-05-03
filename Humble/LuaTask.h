
#ifndef H_LUATASK_H_
#define H_LUATASK_H_

#include "../HBase/HBase.h"

H_BNAMSP

class CLuaTask : public CWorkerTask
{
public:
    CLuaTask(void);
    ~CLuaTask(void);

    void initTask(const char *pName);
    void runTask(CChan *pChan);
    void destroyTask(void);

private:
    H_DISALLOWCOPY(CLuaTask);
    enum
    {
        LTASK_INIT = 0,
        LTASK_RUN,
        LTASK_DESTROY,

        LTASK_COUNT,
    };

private:    
    struct lua_State *m_pLState;
    luabridge::LuaRef **m_pLFunc;
    std::string m_strName;
};

H_ENAMSP

#endif //H_LUATASK_H_
