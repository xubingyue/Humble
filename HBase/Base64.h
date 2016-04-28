
#ifndef H_BASE64_H_
#define H_BASE64_H_

#include "Macros.h"

H_BNAMSP

/*Base64 ±àÂë½âÂëÀà*/
class CBase64
{
public:
    CBase64(void);
    ~CBase64(void);

    /*base64±àÂë*/
    static std::string Encode(const unsigned char *pszData, const size_t iLens);
    /*base64½âÂë*/
    static std::string Decode(const unsigned char *pszData, const size_t iLens);
};

H_ENAMSP

#endif//H_BASE64_H_
