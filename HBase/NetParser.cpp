
#include "NetParser.h"

H_BNAMSP

SINGLETON_INIT(CNetParser)
CNetParser objNetParser;

CNetParser::CNetParser(void)
{
}

CNetParser::~CNetParser(void)
{
}

void CNetParser::addParser(CParser *pParser)
{

}

void CNetParser::setParser(unsigned short usType, const char *pszName)
{

}

CParser *CNetParser::getParser(unsigned short &usType)
{
    return NULL;
}

H_ENAMSP
