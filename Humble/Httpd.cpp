
#include "Httpd.h"

H_BNAMSP

SINGLETON_INIT(CHttp)
CHttp objHttp;

CHttp::CHttp(void)
{
    setName("http");
}

CHttp::~CHttp(void)
{
}

size_t CHttp::parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, class CBinary *pBinary)
{
    return 0;
}

void CHttp::creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens)
{

}

H_ENAMSP
