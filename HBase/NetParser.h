
#ifndef H_NETPARSER_H_
#define H_NETPARSER_H_

#include "RWLock.h"
#include "Singleton.h"

H_BNAMSP

class CParser
{
public:
    CParser(void) {};
    virtual ~CParser(void) {};
    
    const char *getName()
    {
        return m_strName.c_str();
    };

    virtual size_t parsePack(struct H_Session *pSession, char *pAllBuf, const size_t &iLens, 
        class CBinary *pBinary) = 0;
    virtual void creatPack(std::string *pOutBuf, const char *pszMsg, const size_t &iLens) = 0;
    virtual void onClose(struct H_Session *pSession) {};

protected:
    void setName(const char *pszNam)
    {
        m_strName = pszNam;
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
    void setParser(const unsigned short usType, const char *pszName);
    CParser *getParser(const unsigned short &usType);

private:
    H_DISALLOWCOPY(CNetParser);
#ifdef H_OS_WIN 
    #define nameit std::unordered_map<std::string, CParser *>::iterator
    #define name_map std::unordered_map<std::string, CParser *>
    #define typeit std::unordered_map<unsigned short, CParser*>::iterator
    #define type_map std::unordered_map<unsigned short, CParser*>
#else
    #define nameit std::tr1::unordered_map<std::string, CParser *>::iterator
    #define name_map std::tr1::unordered_map<std::string, CParser *>
    #define typeit std::tr1::unordered_map<unsigned short, CParser*>::iterator
    #define type_map std::tr1::unordered_map<unsigned short, CParser*>
#endif

    name_map m_mapName;
    type_map m_mapType;
    CRWLock m_objNameLck;
    CRWLock m_objTypeLck;
};

H_ENAMSP

#endif//H_NETPARSER_H_
