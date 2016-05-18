
#ifndef H_HTTPD_H_
#define H_HTTPD_H_

#include "../HBase/HBase.h"

H_BNAMSP

class CHttp : public CParser, public CSingleton<CHttp>
{
public:
    CHttp(void);
    ~CHttp(void);

    size_t parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, class CBinary *pBinary);
    void creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens);

private:
    H_DISALLOWCOPY(CHttp);

};

H_ENAMSP

#endif //H_HTTPD_H_
