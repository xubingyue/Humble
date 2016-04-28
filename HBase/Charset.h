
#ifndef H_CHARSET_H_
#define H_CHARSET_H_

#include "Macros.h"
#include "uchardet/uchardet.h"

H_BNAMSP

class CCharset
{
public:
    CCharset(void);
    ~CCharset(void);

    std::string getStrCharset(const char *pszData, const unsigned int iSize);
    std::string getFileCharset(const char *pszFile);

private:
    H_DISALLOWCOPY(CCharset);
    std::string getCharset(FILE *pFile);    

private:
    uchardet_t m_pCharset;
};

H_ENAMSP

#endif//H_CHARSET_H_
