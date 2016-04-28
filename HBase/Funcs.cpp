
#include "Funcs.h"
#include "event2/util.h"

H_BNAMSP

#define H_TCPKEEPCOUNT    5

//大小端
static union  
{
    char a[4];
    unsigned int ul;
}endian = {{'L', '?', '?', 'B'}}; 
#define Q_ENDIAN ((char)endian.ul) 

uint64_t ntohl64(uint64_t host)
{
    if ('L' == Q_ENDIAN)
    {
        uint64_t uiRet(H_INIT_NUMBER);
        unsigned long ulHigh, ulLow;

        ulLow = host & 0xFFFFFFFF;
        ulHigh = (host >> 32) & 0xFFFFFFFF;

        ulLow = ntohl(ulLow); 
        ulHigh = ntohl(ulHigh);

        uiRet = ulLow;
        uiRet <<= 32;
        uiRet |= ulHigh;

        return uiRet;
    }
    else
    {
        return host;
    }
}

std::string H_FormatVa(const char *pcFormat, va_list args)
{
    if (NULL == pcFormat)
    {
        return "";
    }
    int iNum(H_INIT_NUMBER);
    unsigned int uiSize(H_ONEK / 2);
    char *pcBuff = NULL;
    std::string strRtn;

    pcBuff = new(std::nothrow) char[uiSize];
    if (NULL == pcBuff)
    {
        return strRtn;
    }
    while(true)
    {        H_Zero(pcBuff, uiSize);
        iNum = vsnprintf(pcBuff, uiSize, pcFormat, args);
        if ((iNum > -1)
            && (iNum < (int)uiSize))
        {
            strRtn = pcBuff;
            H_SafeDelArray(pcBuff);

            return strRtn;
        }
        //分配更大空间
        uiSize = (size_t)((iNum > -1) ? (iNum + 1) : uiSize*2);
        H_SafeDelArray(pcBuff);
        pcBuff = new(std::nothrow) char[uiSize];
        if (NULL == pcBuff)
        {
            return "";
        }
    }
    H_SafeDelArray(pcBuff);

    return "";
}

std::string H_FormatStr(const char *pcFormat, ...)
{
    if (NULL == pcFormat)
    {
        return "";
    }

    va_list va;
    std::string strVa;

    va_start(va, pcFormat);
    strVa = H_FormatVa(pcFormat, va);
    va_end(va);

    return strVa;
}

std::string H_Now(void)
{
    char acTimeStr[H_TIME_LENS] = {0};
    time_t t(time(NULL));	

    strftime(acTimeStr, sizeof(acTimeStr) - 1, "%Y-%m-%d %H:%M:%S", localtime(&t));

    return std::string(acTimeStr);
}

int H_FileExist(const char *pszFileName)
{
    H_ASSERT(pszFileName, "pointer is null.");

    return H_ACCESS(pszFileName, 0);
}

int H_FileSize(const char *pszFileName, unsigned long &ulSize)
{
    int iRtn(H_RTN_OK);
    struct H_STAT stBuffer;

    ulSize = H_INIT_NUMBER;

    iRtn = H_STAT(pszFileName, &stBuffer);
    if (H_RTN_OK != iRtn)
    {
        return iRtn;
    }

    ulSize = (unsigned long)stBuffer.st_size;

    return H_RTN_OK;
}

unsigned int H_GetThreadID(void)
{
#ifdef H_OS_WIN
    return (unsigned int)GetCurrentThreadId();
#else
    return (unsigned int)pthread_self();
#endif
}

unsigned short H_GetCoreCount(void)
{
    unsigned short usCount(1);
#ifdef H_OS_WIN
    SYSTEM_INFO stInfo;
    GetSystemInfo(&stInfo);
    usCount = (unsigned short)stInfo.dwNumberOfProcessors;
#else
    usCount = sysconf(_SC_NPROCESSORS_CONF);
#endif

    return usCount;
}

int H_DirName(const char *pszPath, std::string &strPath)
{
    if (NULL == pszPath)
    {
        return H_RTN_FAILE;
    }

    size_t iLens(strlen(pszPath));

    if (iLens < 2
        || iLens >= H_FILEPATH_LENS)
    {
        return H_RTN_FAILE;
    }

    strPath.clear();

#ifdef H_OS_WIN
    size_t iPos(H_INIT_NUMBER);

    strPath = pszPath;
    iPos = strPath.find_last_of(H_PATH_SEPARATOR);
    if (std::string::npos != iPos)
    {
        strPath = strPath.substr(0, iPos);
    }
#else
    strPath = dirname((char*)pszPath);
#endif 

    return H_RTN_OK;
}

std::string H_GetProPath(void)
{
    int iSize(H_INIT_NUMBER);
    char acPath[H_FILEPATH_LENS] = {0};

#ifdef H_OS_WIN 
    iSize = (int)GetModuleFileName(NULL, acPath, sizeof(acPath) - 1);   
#else
    iSize = readlink("/proc/self/exe", acPath, sizeof(acPath) - 1);
#endif
    if (0 >= iSize 
        || sizeof(acPath) <= (size_t)iSize)
    {
        return "";
    }

    acPath[iSize] = '\0';

    std::string strPath;
    if (H_RTN_OK != H_DirName(acPath, strPath))
    {
        return "";
    }

    strPath += std::string(H_PATH_SEPARATOR);

    return strPath;
}

void H_Split(const std::string &strSource, const char *pszFlag,
    std::list<std::string> &lstRst)
{
    lstRst.clear();

    if ((NULL == pszFlag)
        || (0 == strlen(pszFlag))
        || strSource.empty())
    {
        return;
    }

    size_t iFlagLens(strlen(pszFlag));
    std::string::size_type loc;
    loc = strSource.find(pszFlag, 0);
    if(std::string::npos == loc)
    {
        lstRst.push_back(strSource);

        return;
    }

    std::string strTmp;
    std::string strRst;

    strTmp = strSource;

    while(std::string::npos != loc)
    {
        strRst.clear();
        strRst = strTmp.substr(0, loc);
        lstRst.push_back(strRst);
        strTmp = strTmp.substr(loc + iFlagLens);

        loc = strTmp.find(pszFlag, 0);
        if(std::string::npos == loc)
        {
            strRst = strTmp;
            lstRst.push_back(strRst);
        }
    }

    return;
}

std::string H_TrimLeft(std::string strSource)
{
    if (strSource.empty())
    {
        return strSource;
    }

    std::string::iterator itSrc;
    for (itSrc = strSource.begin(); strSource.end() != itSrc;)
    {
        if((' ' == *itSrc)
            || ('\n' == *itSrc)
            || ('\r' == *itSrc))
        {
            itSrc = strSource.erase(itSrc);
            continue;
        }

        break;
    }

    return strSource;
}

std::string H_TrimRight(std::string strSource)
{
    if (strSource.empty())
    {
        return strSource;
    }

    size_t iFlag(H_INIT_NUMBER);
    size_t iSize(H_INIT_NUMBER);
    std::string::reverse_iterator reitSrc;

    iSize = strSource.size();

    for (reitSrc = strSource.rbegin(); strSource.rend() != reitSrc; reitSrc++, iFlag++)
    {
        if((' ' != *reitSrc)
            && ('\n' != *reitSrc)
            && ('\r' != *reitSrc))
        {
            break;
        }
    }

    if (0 == iFlag)
    {
        return strSource;
    }

    if (iSize == iFlag)
    {
        strSource.clear();

        return strSource;
    }

    std::string::iterator itSrc;
    itSrc = (strSource.begin() + (iSize - iFlag));
    while(strSource.end() != itSrc)
    {
        itSrc = strSource.erase(itSrc);
    }

    return strSource;
}

std::string H_Trim(std::string strSource)
{
    return H_TrimLeft(H_TrimRight(strSource));
}

std::string H_GetFrontOfFlag(const std::string &strSource, const char *pszFlag)
{
    std::string::size_type iPos = H_INIT_NUMBER;
    iPos = strSource.find_first_of(pszFlag);
    if (std::string::npos == iPos)
    {
        return strSource;
    }

    return strSource.substr(0, iPos);
}

std::string H_GetLastOfFlag(const std::string &strSource, const char *pszFlag)
{
    std::string::size_type iPos = H_INIT_NUMBER;
    iPos = strSource.find_last_of(pszFlag);
    if (std::string::npos == iPos)
    {
        return strSource;
    }

    return strSource.substr(iPos + strlen(pszFlag), strSource.size());
}

const char* H_StrStr(const char* pszStr,const char* pszsubStr)
{
    if (NULL == pszStr
        || NULL == pszsubStr)
    {
        return NULL;
    }

    size_t iLens(strlen(pszsubStr));
    if(0 == iLens)
    {
        return NULL;
    }

    while(*pszStr)
    {
        if(0 == H_Strncasecmp(pszStr, pszsubStr, iLens))
        {
            return pszStr;
        }
        ++pszStr;
    }

    return NULL;
}

void H_GetTimeOfDay(struct timeval &stTime)
{
    evutil_timerclear(&stTime);
    (void)evutil_gettimeofday(&stTime, NULL);
}

H_ENAMSP
