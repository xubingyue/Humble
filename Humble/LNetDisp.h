
#ifndef H_LNETDISP_H_
#define H_LNETDISP_H_

#include "Reg2Lua.h"

H_BNAMSP

class CLNetDisp : public CNetIntf
{
public:
    CLNetDisp(void);
    ~CLNetDisp(void);

    void onStart(void);
    void onTimer(const unsigned uiTick, const unsigned uiCount);
    void onTcpLinked(struct H_Session *pSession);
    void onTcpClose(struct H_Session *pSession);
    void onTcpRead(struct H_Session *pSession);
    //void onUdpRead(H_SOCK &sock, const unsigned short &usType);

private:
    enum
    {
        LOnstart = 0,
        LOnTimer,
        LOnTcpLinked,
        LOnTcpClose,
        LOnTcpRead,
        LOnUdpRead,

        LCount,
    };

private:
    //socket_t m_iFromLens;
    struct lua_State *m_pLState;
    luabridge::LuaRef **m_pLFunc;
    //char m_acUdpBuffer[H_MAXUDPBUF];
    //sockaddr m_stFromAddr;
    //CBinary m_objUdpBinary;
    CEvBuffer m_objTcpEvBuffer;
};

H_ENAMSP

#endif//H_LNETDISP_H_
