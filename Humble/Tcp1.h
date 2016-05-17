
#ifndef H_TCP1_H_
#define H_TCP1_H_

#include "../HBase/HBase.h"

H_BNAMSP

class CTcp1 : public CParser, public CSingleton<CTcp1>
{
public:
    CTcp1(void);
    ~CTcp1(void);

    size_t parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, luabridge::LuaRef *pTable);
    void creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens);

private:
    H_DISALLOWCOPY(CTcp1);
    bool readHead(char *pBuffer, const size_t &iLens, size_t &iBufLens, size_t &iHeadLens);
    void creatHead(std::string *pOutBuf, const size_t &iLens);
};

H_ENAMSP

#endif //H_TCP1_H_
