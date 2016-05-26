
#ifndef H_TCP2_H_
#define H_TCP2_H_

#include "NetParser.h"
#include "Singleton.h"

H_BNAMSP

//unsigned short + data
class CTcp2 : public CParser, public CSingleton<CTcp2>
{
public:
    CTcp2(void);
    ~CTcp2(void);

    int parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, class CBinary *pBinary);

private:
    H_DISALLOWCOPY(CTcp2);

};

H_ENAMSP

#endif //H_TCP2_H_
