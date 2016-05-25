
#include "DefParser.h"
#include "Binary.h"

H_BNAMSP

SINGLETON_INIT(CDefParser)
CDefParser objDefParser;

CDefParser::CDefParser(void)
{
    setName("default");
}

CDefParser::~CDefParser(void)
{
}

int CDefParser::parsePack(struct H_Session *pSession, char *pAllBuf, const size_t &iLens,
    class CBinary *pBinary)
{
    pBinary->setReadBuffer(pAllBuf, iLens);

    return (int)iLens;
}

H_ENAMSP
