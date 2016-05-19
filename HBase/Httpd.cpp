
#include "Httpd.h"
#include "Funcs.h"
#include "Binary.h"

H_BNAMSP

SINGLETON_INIT(CHttp)
CHttp objHttp;

#define Http_HeadEndFlag "\r\n\r\n"
#define Http_ChunkEndFlag "\r\n0\r\n\r\n"

CHttp::CHttp(void)
{
    m_iHeadEndFlagLens = strlen(Http_HeadEndFlag);
    m_iChunkEndFlagLens = strlen(Http_ChunkEndFlag);
    setName("http");
}

CHttp::~CHttp(void)
{
}

size_t CHttp::getHeadLens(const char *pBuffer)
{
    const char *pFind = strstr(pBuffer, Http_HeadEndFlag);
    if (NULL == pFind)
    {
        return H_INIT_NUMBER;
    }

    return pFind - pBuffer + m_iHeadEndFlagLens;
}

bool CHttp::getContentLens(const char *pszHead, const size_t &iHeadLens, size_t &iContentLens)
{
    const char *pBegin = H_StrStr(pszHead, "content-length:");
    if (NULL == pBegin)
    {
        return false;
    }

    iContentLens = atoi(pBegin + strlen("content-length:"));

    return true;
}

bool CHttp::checkChunk(const char *pszHead, const size_t &iHeadLens)
{
    if (NULL == H_StrStr(pszHead, "transfer-encoding: chunked"))
    {
        return false;
    }

    return true;
}

bool CHttp::getChunkLens(const char *pBuffer, size_t &iChunkLens)
{
    const char *pChunck = strstr(pBuffer, Http_ChunkEndFlag);
    if (NULL == pChunck)
    {
        return false;
    }

    iChunkLens = pChunck - pBuffer + m_iChunkEndFlagLens;

    return true;
}

int CHttp::parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, class CBinary *pBinary)
{
    size_t iHeadLens = getHeadLens(pAllBuf);
    if (H_INIT_NUMBER == iHeadLens)
    {
        return H_INIT_NUMBER;
    }

    size_t iContentLens = H_INIT_NUMBER;
    if (!getContentLens(pAllBuf, iHeadLens, iContentLens))
    {
        if (checkChunk(pAllBuf, iHeadLens))
        {
            if (!getChunkLens(pAllBuf + iHeadLens, iContentLens))
            {
                return H_INIT_NUMBER;
            }
        }
    }    

    if (H_INIT_NUMBER == iContentLens)
    {
        pBinary->setReadBuffer(pAllBuf, iHeadLens);

        return iHeadLens;
    }

    size_t iTotalLens(iHeadLens + iContentLens);
    if (iLens < iTotalLens)
    {
        return H_INIT_NUMBER;
    }

    pBinary->setReadBuffer(pAllBuf, iTotalLens);

    return (int)iTotalLens;
}

void CHttp::creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens)
{
    pOutBuf->append(pszMsg, iLens);
}

H_ENAMSP
