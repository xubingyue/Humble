
#ifndef H_DEFPARSER_H_
#define H_DEFPARSER_H_

#include "NetParser.h"
#include "Singleton.h"

H_BNAMSP

//对包不作任何处理
class CDefParser : public CParser, public CSingleton<CDefParser>
{
public:
    CDefParser(void);
    ~CDefParser(void);

    int parsePack(struct H_Session *pSession, char *pAllBuf, const size_t &iLens,
        class CBinary *pBinary);
    void creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens) 
    {

    };
    const char *creatPack(const char *pszMsg)
    {
        return pszMsg;
    };

private:
    H_DISALLOWCOPY(CDefParser);
};

H_ENAMSP

#endif //H_DEFPARSER_H_
