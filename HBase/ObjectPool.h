
#ifndef H_OBJPOOL_H_
#define H_OBJPOOL_H_

#include "Macros.h"

H_BNAMSP

//简单对象池 非线程安全
template <typename T>
class CObjPool
{
public:
    CObjPool(void);
    CObjPool(const unsigned int uiInitNum, const char *pszPoolName = "");
    virtual ~CObjPool(void);

    virtual T* Malloc(void);
    virtual void Free(T *pObj);

private:
    H_DISALLOWCOPY(CObjPool);

private:
#ifdef H_OS_WIN 
    #define usedit std::unordered_map<T*, bool>::iterator
    #define used_map std::unordered_map<T*, bool>
#else
    #define usedit std::tr1::unordered_map<T*, bool>::iterator
    #define used_map std::tr1::unordered_map<T*, bool>
#endif
    long m_lCount;
    used_map m_mapUsed;
    std::queue<T*> m_quFree;
    std::string m_strName;
};

template <typename T>
CObjPool<T>::CObjPool(void) : m_lCount(H_INIT_NUMBER)
{

}

template <typename T>
CObjPool<T>::CObjPool(const unsigned int uiInitNum, const char *pszPoolName) : 
    m_lCount(H_INIT_NUMBER), m_strName(pszPoolName)
{
    if (H_INIT_NUMBER == uiInitNum)
    {
        return;
    }

    T *pVal = NULL;
    for (unsigned int ui = 0; ui < uiInitNum; ui++)
    {
        pVal = new(std::nothrow) T;
        H_ASSERT(NULL != pVal, "malloc memory error.");

        m_quFree.push(pVal);
    }
}

template <typename T>
CObjPool<T>::~CObjPool(void)
{
    if (H_INIT_NUMBER != m_lCount)
    {
        H_Printf("warning: object pool %s maybe some object not free.", m_strName.c_str());
    }

    T *pTmp = NULL;
    typename usedit itUsed;
    for (itUsed = m_mapUsed.begin(); m_mapUsed.end() != itUsed; ++itUsed)
    {
        pTmp = itUsed->first;
        H_SafeDelete(pTmp);
    }
    m_mapUsed.clear();

    while(!m_quFree.empty())
    {
        pTmp = m_quFree.front();
        m_quFree.pop();
        H_SafeDelete(pTmp);
    }
}

template <typename T>
T* CObjPool<T>::Malloc(void)
{
    T *pTmp = NULL;

    if (!m_quFree.empty())
    {
        pTmp = m_quFree.front();
        m_quFree.pop();
    }
    else
    {
        pTmp = new(std::nothrow) T;
        H_ASSERT(NULL != pTmp, "malloc memory error.");
    }

    m_mapUsed.insert(std::make_pair(pTmp, true));
    m_lCount++;

    return pTmp;
}

template <typename T>
void CObjPool<T>::Free(T *pObj)
{
    if (NULL == pObj)
    {
        return;
    }

    typename usedit itUsed;
    itUsed = m_mapUsed.find(pObj);
    if (m_mapUsed.end() != itUsed)
    {
        m_mapUsed.erase(itUsed);
        m_quFree.push(pObj);
        m_lCount--;
    }
}

H_ENAMSP

#endif//H_OBJPOOL_H_
