
#include "mqtt.h"
#include "Binary.h"

H_BNAMSP

SINGLETON_INIT(CMQTT)
CMQTT objMQTT;

CMQTT::CMQTT(void)
{
    setName("mqtt");
}

CMQTT::~CMQTT(void)
{
}

int CMQTT::parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, class CBinary *pBinary)
{
    return 0;
}

void CMQTT::creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens)
{

}

H_ENAMSP
