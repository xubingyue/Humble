
#include "WebSock.h"

H_BNAMSP

SINGLETON_INIT(CWebSock)
CWebSock objWebSock;

CWebSock::CWebSock(void)
{
    setName("websock");
}

CWebSock::~CWebSock(void)
{
}

size_t CWebSock::parsePack(struct H_Session *pSession, char *pAllBuf, const size_t &iLens, class CBinary *pBinary)
{
    return 0;
}

void CWebSock::creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens)
{

}

void CWebSock::onClose(struct H_Session *pSession)
{

}

H_ENAMSP
