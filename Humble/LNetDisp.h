
#ifndef H_LNETDISP_H_
#define H_LNETDISP_H_

#include "Reg2Lua.h"

H_BNAMSP

class CLNetDisp : public CSVIntf
{
public:
    CLNetDisp(void);
    ~CLNetDisp(void);

    void onStart(void);
    void onStop(void);
    void onTcpLinked(struct H_Session *pSession);
    void onTcpClose(struct H_Session *pSession);
    void onTcpRead(struct H_Session *pSession);

private:
    H_DISALLOWCOPY(CLNetDisp);
    enum
    {
        LOnstart = 0,
        LOnStop,
        LOnTcpLinked,
        LOnTcpClose,
        LOnTcpRead,

        LCount,
    };

private:
    struct lua_State *m_pLState;
    luabridge::LuaRef **m_pLFunc;
    CEvBuffer m_objTcpEvBuffer;
};

H_ENAMSP

#endif//H_LNETDISP_H_
