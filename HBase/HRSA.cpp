
#include "HRSA.h"

H_BNAMSP

CRSA::CRSA(void) : m_pPubKey(NULL), m_pPriKey(NULL), m_pRandom(NULL)
{

}

CRSA::~CRSA(void)
{

}

void CRSA::setKey(R_RSA_PUBLIC_KEY *pPubKey, R_RSA_PRIVATE_KEY *pPriKey, R_RANDOM_STRUCT *pRandom)
{
    m_pPubKey = pPubKey;
    m_pPriKey = pPriKey;
    m_pRandom = pRandom;
}

std::string CRSA::RSAEncrypt(RSAEnType emEnType, const char* pszData, 
    const size_t &iDataLens)
{
    int iRtn(H_RTN_OK);
    unsigned int iStep(H_INIT_NUMBER);
    unsigned int iBufLens(H_INIT_NUMBER);
    unsigned int iTmpSize(H_INIT_NUMBER);
    unsigned char *pTmp = NULL;
    unsigned char acOutBuf[MAX_RSA_MODULUS_LEN];
    std::string strRtn;

    pTmp = (unsigned char*)(pszData);
    if (EnType_Pub == emEnType)
    {
        H_ASSERT((NULL != m_pPubKey) && (NULL != m_pRandom), "pointer is null.");
        iStep = (m_pPubKey->bits + 7) / 8 - 11;
    }
    else
    {
        H_ASSERT((NULL != m_pPriKey), "pointer is null.");
        iStep = (m_pPriKey->bits + 7) / 8 - 11;
    } 

    for (size_t i = 0; i < iDataLens; i += iStep)
    {
        iBufLens = (unsigned int)(((i + iStep) > iDataLens) ? (iDataLens - i) : iStep);
        if (EnType_Pub == emEnType)
        {
            iRtn = RSAPublicEncrypt(acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                m_pPubKey, m_pRandom);
        }
        else
        {
            iRtn = RSAPrivateEncrypt(acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                m_pPriKey);
        }

        if (H_RTN_OK != iRtn)
        {
            H_Printf("%s", "RSAPublicEncrypt error.");

            return "";
        }

        strRtn.append((const char*)acOutBuf, iTmpSize);
    }

    return strRtn;
}

std::string CRSA::RSADecrypt(RSADeType emEnType, const char* pszData, 
    const size_t &iDataLens)
{
    int iRtn(H_RTN_OK);
    unsigned int iStep(H_INIT_NUMBER);
    unsigned int iBufLens(H_INIT_NUMBER);
    unsigned int iTmpSize(H_INIT_NUMBER);
    unsigned char *pTmp = NULL;
    unsigned char acOutBuf[MAX_RSA_MODULUS_LEN];
    std::string strRtn;

    pTmp = (unsigned char*)(pszData);
    if (DeType_Pub == emEnType)
    {
        H_ASSERT(NULL != m_pPubKey, "pointer is null.");
        iStep = (m_pPubKey->bits + 7) / 8;
    }
    else
    {
        H_ASSERT(NULL != m_pPriKey, "pointer is null.");
        iStep = (m_pPriKey->bits + 7) / 8;
    } 

    for (size_t i = 0; i < iDataLens; i += iStep)
    {
        iBufLens = (unsigned int)(((i + iStep) > iDataLens) ? (iDataLens - i) : iStep);
        if (DeType_Pub == emEnType)
        {
            iRtn = RSAPublicDecrypt(acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                m_pPubKey);
        }
        else
        {
            iRtn = RSAPrivateDecrypt(acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                m_pPriKey);
        }

        if (H_RTN_OK != iRtn)
        {
            H_Printf("%s", "RSAPrivateDecrypt error.");

            return "";
        }

        strRtn.append((const char*)acOutBuf, iTmpSize);
    }

    return strRtn;
}

std::string CRSA::pubEncrypt(const char* pszData, const size_t iLens)
{
    return RSAEncrypt(EnType_Pub, pszData, iLens);
}

std::string CRSA::priDecrypt(const char* pszData, const size_t iLens)
{
    return RSADecrypt(DeType_Pri, pszData, iLens);
}

std::string CRSA::priEncrypt(const char* pszData, const size_t iLens)
{
    return RSAEncrypt(EnType_Pri, pszData, iLens);
}

std::string CRSA::pubDecrypt(const char* pszData, const size_t iLens)
{
    return RSADecrypt(DeType_Pub, pszData, iLens);
}

H_ENAMSP
