
#ifndef H_WEBSOCK_H_
#define H_WEBSOCK_H_

#include "../HBase/HBase.h"

H_BNAMSP

class CWebSock : public CParser, public CSingleton<CWebSock>
{
public:
    CWebSock(void);
    ~CWebSock(void);

    size_t parsePack(struct H_Session *pSession, char *pAllBuf, const size_t &iLens, luabridge::LuaRef *pTable);
    void creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens);
    void onClose(struct H_Session *pSession);

private:
    H_DISALLOWCOPY(CWebSock);

};

H_ENAMSP

#endif //H_WEBSOCK_H_
