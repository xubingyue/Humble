
#ifndef H_MQTT_H_
#define H_MQTT_H_

#include "NetParser.h"
#include "Singleton.h"

H_BNAMSP

class CMQTT : public CParser, public CSingleton<CMQTT>
{
public:
    CMQTT(void);
    ~CMQTT(void);

    int parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, class CBinary *pBinary);
    void creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens);

private:
    H_DISALLOWCOPY(CMQTT);

};

H_ENAMSP

#endif//H_MQTT_H_

