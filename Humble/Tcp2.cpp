
#include "Tcp2.h"

H_BNAMSP

SINGLETON_INIT(CTcp2)
CTcp2 objTcp2;

CTcp2::CTcp2(void)
{
    setName("tcp2");
}

CTcp2::~CTcp2(void)
{
}

size_t CTcp2::parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, class CBinary *pBinary)
{
    return 0;
}

void CTcp2::creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens)
{

}

H_ENAMSP
