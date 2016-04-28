
#ifndef H_FUNCS_H_
#define H_FUNCS_H_

#include "Macros.h"

H_BNAMSP

uint64_t ntohl64(uint64_t host);

std::string H_FormatVa(const char *pcFormat, va_list args);
std::string H_FormatStr(const char *pcFormat, ...);
std::string H_Now(void);

int H_FileExist(const char *pszFileName);
int H_FileSize(const char *pszFileName, unsigned long &ulSize);
int H_DirName(const char *pszPath, std::string &strPath);
std::string H_GetProPath(void);

unsigned int H_GetThreadID(void);
unsigned short H_GetCoreCount(void);

void H_Split(const std::string &strSource, const char *pszFlag,
    std::list<std::string> &lstRst);
std::string H_TrimLeft(std::string strSource);
std::string H_TrimRight(std::string strSource);
std::string H_Trim(std::string strSource);
std::string H_GetFrontOfFlag(const std::string &strSource, const char *pszFlag);
std::string H_GetLastOfFlag(const std::string &strSource, const char *pszFlag);
//���Ҳ����ִ�Сд
const char *H_StrStr(const char *pszStr,const char *pszsubStr);

unsigned int H_GetSessionID(void);
void H_GetTimeOfDay(struct timeval &stTime);

/*ת�ַ���*/
template<typename T>
std::string H_ToString(T number)
{
    std::stringstream objStream;

    objStream << number;

    return objStream.str();
};

/*ת����*/
template<typename T>
T H_ToNumber(const char *pszNumber)
{
    T  iValue(H_INIT_NUMBER);
    std::stringstream objStream;

    objStream << pszNumber;
    objStream >> iValue;

    return iValue;
};

H_ENAMSP

#endif//H_FUNCS_H_