//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Copy safe class for holding Text information
//
//-------------------------------------------------------------------------------------------------

#pragma once

template <typename T>
struct RefCountedData
{
    struct Data 
    {
        friend struct RefCountedData<T>;
    private:
        int refCount;
    protected:
        Data() : refCount(0)
        {
        }
    public:
        virtual ~Data()
        {
            VSASSERT( 0 == refCount, "Non-zero ref count delete");
            refCount = -1; // Check for double delete
        }
    };

    RefCountedData() : m_pData(NULL)
    {
        TemplateUtil::CompileAssertIsChildOf<Data,T>();
    }

    RefCountedData( _In_ T* pData) : m_pData(pData)
    {
        TemplateUtil::CompileAssertIsChildOf<Data,T>();
        ThrowIfNull(pData);
        pData->refCount++;
    }

    RefCountedData( _In_ const RefCountedData<T>& other) 
    {
        TemplateUtil::CompileAssertIsChildOf<Data,T>();
        m_pData = other.m_pData;
        if ( m_pData )
        {
            m_pData->refCount++;
        }
    }

    ~RefCountedData()
    {
        Release();
    }

    void Attach( _In_ T* pData)
    {
        ThrowIfNull(pData);
        RefCountedData<T> temp(pData);
        *this = temp;
    }

    void Release()
    {
        if ( !m_pData )
        {
            return;
        }

        m_pData->refCount--;
        if ( 0 == m_pData->refCount )
        {
            delete m_pData;
        }

        m_pData = NULL;
    }

    RefCountedData& operator=(const RefCountedData& other)
    {
        RefCountedData<T> temp(other);
        TemplateUtil::Swap(&m_pData, &temp.m_pData);
        return *this;
    }

    operator T*() const
    {
        return m_pData;
    }

    T& operator *() const
    {
        return *m_pData;
    }

    T* operator ->()
    {
        VSASSERT(m_pData, "Accessing NULL Pointer");
        return m_pData;
    }

    const T* operator ->() const
    {
        VSASSERT(m_pData, "Accessing NULL Pointer");
        return m_pData;
    }

private:
    T* m_pData;
};

