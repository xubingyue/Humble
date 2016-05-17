
#ifndef H_NETPARSER_H_
#define H_NETPARSER_H_

#include "Singleton.h"

H_BNAMSP

class CParser
{
public:
    CParser(void);
    virtual ~CParser(void);

    void setName(const char *pszNam)
    {
        m_strName = pszNam;
    };
    const char *getName()
    {
        return m_strName.c_str();
    };

private:
    H_DISALLOWCOPY(CParser);

private:
    std::string m_strName;
};

class CNetParser : public CSingleton<CNetParser>
{
public:
    CNetParser(void);
    ~CNetParser(void);

    void addParser(CParser *pParser);
    void setParser(unsigned short usType, const char *pszName);
    CParser *getParser(unsigned short &usType);

private:
    H_DISALLOWCOPY(CNetParser);
};

H_ENAMSP

#endif//H_NETPARSER_H_
