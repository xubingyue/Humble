
#ifndef H_LTICK_H_
#define H_LTICK_H_

#include "Reg2Lua.h"

H_BNAMSP

class CLTick : public CSVIntf
{
public:
    CLTick(void);
    ~CLTick(void);

    void onStart(void);
    void onStop(void);
    void onTime(const unsigned int &uiTick, const unsigned int &uiCount);

private:
    H_DISALLOWCOPY(CLTick);
    enum
    {
        LOnStart = 0,
        LOnStop,
        LOnTime,

        LCount,
    };

private:
    struct lua_State *m_pLState;
    luabridge::LuaRef **m_pLFunc;
    luabridge::LuaRef *m_pTable;
};

H_ENAMSP

#endif //H_LTICK_H_
