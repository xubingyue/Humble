
#ifndef H_MQTT_H_
#define H_MQTT_H_

#include "NetParser.h"
#include "Singleton.h"

H_BNAMSP

//MQTT
class CMQTT : public CParser, public CSingleton<CMQTT>
{
public:
    CMQTT(void);
    ~CMQTT(void);

    int parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, class CBinary *pBinary);
    const char *creatPack(const char *pszMsg)
    {
        return pszMsg;
    };
    void creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens);

private:
    H_DISALLOWCOPY(CMQTT);
    size_t parseHead(char *pAllBuf, const size_t &iLens, size_t &iBufLens);
    void parseByte1(const unsigned char &ucByte, class CBinary *pBinary);
};

H_ENAMSP

#endif//H_MQTT_H_

