
#ifndef H_WEBSOCK_H_
#define H_WEBSOCK_H_

#include "NetParser.h"
#include "Singleton.h"
#include "SHA1.h"

H_BNAMSP

class CWebSock : public CParser, public CSingleton<CWebSock>
{
public:
    CWebSock(void);
    ~CWebSock(void);

    size_t parsePack(struct H_Session *pSession, char *pAllBuf, const size_t &iLens, class CBinary *pBinary);
    void creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens);
    void onClose(struct H_Session *pSession);

private:
    H_DISALLOWCOPY(CWebSock);
    size_t shakeHands(struct H_Session *pSession, char *pAllBuf, const size_t &iLens);
    std::string parseKey(std::list<std::string> &lstShakeHands) const;
    std::string createKey(std::string &strKey);
    std::string createResponse(const std::string &strKey) const;

    size_t parseHead(struct WebSockFram *pFram, const char *pBuffer, const size_t &iLens);
    void parseData(struct WebSockFram *pFram, char *pBuffer, const size_t &iLens);
    bool handleFrame(struct H_Session *pSession, struct WebSockFram *pFram,
        char *pBuffer, const size_t &iLens, class CBinary *pBinary);
    void initMapFram(H_SOCK &sock, const char *pBuf, const size_t &iLens);

    void createHead(std::string *pBuf, const bool &bFin, const unsigned int &uiCode,
        const size_t &iDataLens);

#ifdef H_OS_WIN 
    #define frameit std::unordered_map<H_SOCK, std::string>::iterator
    #define frame_map std::unordered_map<H_SOCK, std::string>
#else
    #define frameit std::tr1::unordered_map<H_SOCK, std::string>::iterator
    #define frame_map std::tr1::unordered_map<H_SOCK, std::string>
#endif

private:
    size_t m_iEndFlagLens;
    std::string m_strVersion;
    frame_map m_mapFrame;
    CSHA1 m_objSha1;
};

H_ENAMSP

#endif //H_WEBSOCK_H_
