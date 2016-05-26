
#include "mqtt.h"
#include "Binary.h"

H_BNAMSP

SINGLETON_INIT(CMQTT)
CMQTT objMQTT;

#define H_MQTTHEADLENS 2

CMQTT::CMQTT(void)
{
    setName("mqtt");
}

CMQTT::~CMQTT(void)
{

}

size_t CMQTT::parseHead(char *pAllBuf, const size_t &iLens, size_t &iBufLens)
{
    if (H_MQTTHEADLENS > iLens)
    {
        return H_INIT_NUMBER;
    }

    unsigned char ucByte;
    size_t iHeadLens(1);
    size_t iMultiplier(1);

    do
    {
        ucByte = (unsigned char)pAllBuf[iHeadLens];
        iBufLens += (ucByte & 127) * iMultiplier;
        iMultiplier *= 128;

        ++iHeadLens;
        if (iHeadLens + 1 > iLens)
        {
            return H_INIT_NUMBER;
        }

    } while (H_INIT_NUMBER != (ucByte & 128));

    return iHeadLens;
}

void CMQTT::parseByte1(const unsigned char &ucByte, class CBinary *pBinary)
{
    pBinary->setUint8((ucByte & 0xF0) >> 4);//Message Type
    pBinary->setUint8((ucByte & 0x8) >> 3);//DUP
    pBinary->setUint8((ucByte & 0x6) >> 1);//QoS
    pBinary->setUint8(ucByte & 0x1);//RETAIN
}

int CMQTT::parsePack(struct H_Session *, char *pAllBuf, const size_t &iLens, class CBinary *pBinary)
{
    size_t iBufferLens(H_INIT_NUMBER);
    size_t iHeadLens = parseHead(pAllBuf, iLens, iBufferLens);
    if (H_INIT_NUMBER == iHeadLens)
    {
        return H_INIT_NUMBER;
    }

    unsigned char ucByte = (unsigned char)pAllBuf[0];
    if (H_INIT_NUMBER == iBufferLens)
    {
        parseByte1(ucByte, pBinary);

        return iHeadLens;
    }

    if (iHeadLens + iBufferLens > iLens)
    {
        return H_INIT_NUMBER;
    }

    pBinary->setReadBuffer(pAllBuf + iHeadLens, iBufferLens);    
    parseByte1(ucByte, pBinary);
    pBinary->setUint32(iBufferLens);

    return (int)(iHeadLens + iBufferLens);
}

H_ENAMSP
