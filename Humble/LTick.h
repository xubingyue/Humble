
#ifndef H_LTICK_H_
#define H_LTICK_H_

#include "Reg2Lua.h"

H_BNAMSP

//定时器服务接口实现
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
};

H_ENAMSP

#endif //H_LTICK_H_
