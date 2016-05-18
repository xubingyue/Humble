
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

H_INLINE size_t CTcp2::parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, class CBinary *pBinary)
{
    unsigned short uiBufLens(H_INIT_NUMBER);
    size_t iHeadLens(sizeof(uiBufLens));
    if (iHeadLens > iLens)
    {
        return H_INIT_NUMBER;
    }

    uiBufLens = ntohs(*(unsigned short*)(pAllBuf));
    if (H_INIT_NUMBER == uiBufLens)
    {
        pBinary->setReadBuffer(NULL, uiBufLens);

        return iHeadLens;
    }
    if (uiBufLens + iHeadLens > iLens)
    {
        return H_INIT_NUMBER;
    }

    pBinary->setReadBuffer(pAllBuf + iHeadLens, uiBufLens);

    return uiBufLens + iHeadLens;
}

H_INLINE void CTcp2::creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens)
{
    unsigned short usLens(ntohs((unsigned short)iLens));
    pOutBuf->append((const char*)&usLens, sizeof(usLens));
    pOutBuf->append(pszMsg, iLens);
}

H_ENAMSP
