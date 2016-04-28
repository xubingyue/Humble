
#include "Charset.h"

H_BNAMSP

CCharset::CCharset(void)
{
    m_pCharset = uchardet_new();
    H_ASSERT(NULL != m_pCharset, "uchardet_new error.");
}

CCharset::~CCharset(void)
{
    if (NULL != m_pCharset)
    {
        uchardet_delete(m_pCharset);
        m_pCharset = NULL;
    }
}

std::string CCharset::getStrCharset(const char *pszData, const unsigned int iSize)
{
    uchardet_reset(m_pCharset);
    if (H_RTN_OK != uchardet_handle_data(m_pCharset, pszData, iSize))
    {
        H_Printf("%s", "uchardet_handle_data error.");
        return "";
    }
    uchardet_data_end(m_pCharset);

    return std::string(uchardet_get_charset(m_pCharset));
}

std::string CCharset::getFileCharset(const char *pszFile)
{
    FILE *pFile = fopen(pszFile, "r");
    H_ASSERT(NULL != pszFile, "open file error.");

    std::string strCharset(getCharset(pFile));

    fclose(pFile);

    return strCharset;
}

std::string CCharset::getCharset(FILE *pFile)
{
    H_ASSERT(NULL != pFile, "file pointer is null.");
    size_t iReadSize(H_INIT_NUMBER);
    char acTmp[H_ONEK * 4];

    uchardet_reset(m_pCharset);

    while(!feof(pFile))
    {
        iReadSize = fread(acTmp, 1, sizeof(acTmp), pFile);
        if (H_RTN_OK != uchardet_handle_data(m_pCharset, acTmp, iReadSize))
        {
            return "";
        }
    }

    uchardet_data_end(m_pCharset);

    return std::string(uchardet_get_charset(m_pCharset));
}

H_ENAMSP
