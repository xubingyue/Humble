
#ifndef H_LTICK_H_
#define H_LTICK_H_

#include "Reg2Lua.h"

H_BNAMSP

class CLTick : public CTickIntf
{
public:
    CLTick(void);
    ~CLTick(void);

    void onTime(const unsigned int &uiTick, const unsigned int &uiCount);

private:
    H_DISALLOWCOPY(CLTick);
    enum
    {
        LOnTime = 0,

        LCount,
    };

private:
    struct lua_State *m_pLState;
    luabridge::LuaRef **m_pLFunc;
};

H_ENAMSP

#endif //H_LTICK_H_
