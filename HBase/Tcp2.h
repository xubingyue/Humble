
#ifndef H_TCP2_H_
#define H_TCP2_H_

#include "NetParser.h"
#include "Singleton.h"

H_BNAMSP

class CTcp2 : public CParser, public CSingleton<CTcp2>
{
public:
    CTcp2(void);
    ~CTcp2(void);

    int parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, class CBinary *pBinary);
    void creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens);

private:
    H_DISALLOWCOPY(CTcp2);

};

H_ENAMSP

#endif //H_TCP2_H_
