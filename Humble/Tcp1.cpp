
#include "Tcp1.h"

H_BNAMSP

SINGLETON_INIT(CTcp1)
CTcp1 objTcp1;

#define TCPBUFLENS_125 125
#define TCPBUFLENS_126 126
#define TCPBUFLENS_127 127

#define TCP_HRAD_MAXLENS  5

CTcp1::CTcp1(void)
{
    setName("tcp1");
}

CTcp1::~CTcp1(void)
{
}

bool CTcp1::readHead(char *pBuffer, const size_t &iLens, size_t &iBufLens, size_t &iHeadLens)
{
    char cFlag = pBuffer[0];
    if (cFlag <= TCPBUFLENS_125)
    {
        iHeadLens = sizeof(cFlag);
        iBufLens = cFlag;
    }
    else if (TCPBUFLENS_126 == cFlag)
    {
        size_t iNeedReadLens = sizeof(cFlag) + sizeof(unsigned short);
        if (iLens < iNeedReadLens)
        {
            return false;
        }

        iBufLens = ntohs(*((unsigned short *)(pBuffer + sizeof(cFlag))));
        iHeadLens = sizeof(cFlag) + sizeof(unsigned short);
    }
    else if (TCPBUFLENS_127 == cFlag)
    {
        size_t iNeedReadLens = sizeof(cFlag) + sizeof(unsigned int);
        if (iLens < iNeedReadLens)
        {
            return false;
        }

        iBufLens = (size_t)ntohl(*((unsigned int *)(pBuffer + sizeof(cFlag))));
        iHeadLens = sizeof(cFlag) + sizeof(unsigned int);
    }
    else
    {
        return false;
    }

    return true;
}

size_t CTcp1::parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, luabridge::LuaRef *pTable)
{
    size_t iParsed(H_INIT_NUMBER);
    size_t iBufLens(H_INIT_NUMBER);
    size_t iHeadLens(H_INIT_NUMBER);
    luabridge::H_LBinary stBinary;

    //*pTable = luabridge::newTable(pTable->state());

    while (true)
    {
        if (iParsed >= iLens)
        {
            break;
        }

        if (!readHead(pAllBuf + iParsed, iLens - iParsed, iBufLens, iHeadLens))
        {
            break;
        }

        if (H_INIT_NUMBER == iBufLens)
        {
            iParsed += iHeadLens;
            continue;
        }

        if (iBufLens > (iLens - iParsed - iHeadLens))
        {
            break;
        }

        stBinary.pBufer = pAllBuf + iParsed + iHeadLens;
        stBinary.iLens = iBufLens;
        iParsed += (iBufLens + iHeadLens);
        (*pTable)[1] = stBinary;
    }

    return iParsed;
}

void CTcp1::creatHead(std::string *pOutBuf, const size_t &iLens)
{
    char acHead[TCP_HRAD_MAXLENS];
    size_t iHeadLens = H_INIT_NUMBER;

    if (iLens <= TCPBUFLENS_125)
    {
        acHead[0] = (char)iLens;
        iHeadLens = sizeof(acHead[0]);
    }
    else if ((iLens > TCPBUFLENS_125) && (iLens <= 0xFFFF))
    {
        acHead[0] = TCPBUFLENS_126;
        unsigned short usLen = ntohs((unsigned short)iLens);
        memcpy(acHead + sizeof(acHead[0]), &usLen, sizeof(usLen));

        iHeadLens = sizeof(acHead[0]) + sizeof(usLen);
    }
    else
    {
        acHead[0] = TCPBUFLENS_127;
        unsigned int uiLens = ntohl(iLens);
        memcpy(acHead + sizeof(acHead[0]), &uiLens, sizeof(uiLens));

        iHeadLens = sizeof(acHead[0]) + sizeof(uiLens);
    }

    pOutBuf->append(acHead, iHeadLens);
}

void CTcp1::creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens)
{
    creatHead(pOutBuf, iLens);
    pOutBuf->append(pszMsg, iLens);
}

H_ENAMSP
