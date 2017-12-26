//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
// AutoArray 
//
//-------------------------------------------------------------------------------------------------

#pragma once

template <class T>
class AutoStackPtr 
{
public:
    explicit AutoStackPtr(_In_ T *pData, bool ownsMemory) :
        m_pData(pData),
        m_ownsMemory(ownsMemory)
    {
    }

    AutoStackPtr() :
        m_pData(NULL),
        m_ownsMemory(false)
    {

    }

    ~AutoStackPtr()
    {
        Release();
    }

    void AttachUnowned(_In_ T* pData)
    {
        Release();
        m_pData = pData;
        m_ownsMemory = false;
    }

    void Atta----ned(_In_ T* pData)
    {
        Release();
        m_pData = pData;
        m_ownsMemory = false;
    }

    T* Detach()
    {
        T* pTemp = m_pData;
        m_pData = NULL;
        m_ownsMemory = false;
        return pTemp;
    }

    void Release()
    {
        if ( m_ownsMemory && m_pData )
        {
            delete m_pData;
        }

        m_ownsMemory = false;
        m_pData = NULL;
    }

    operator T *()
    {
        return m_pData;
    }

    T* operator ->()
    {
        return m_pData;
    }

private:
    T* m_pData;
    bool m_ownsMemory;
};

