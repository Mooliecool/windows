//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Thread Local Storage Value
//-------------------------------------------------------------------------------------------------

#pragma once

//namespace CompilerPackage
//{
    template <typename T>
    class TlsValue
    {
    public:
        TlsValue(DWORD index, const T& defaultValue=T()) :
            m_pValue(NULL),
            m_index(index),
            m_owns(false)
        {
            m_pValue = reinterpret_cast<T*>(::TlsGetValue(m_index));
            if ( !m_pValue )
            {
                m_pValue = new (zeromemory) T(defaultValue);
                m_owns = true;
                ::TlsSetValue(m_index, m_pValue);
            }
        }
        ~TlsValue()
        {
            if ( m_owns )
            {
                ::TlsSetValue(m_index, NULL);
                delete m_pValue;
            }
        }

        T* Value() const
        {
            return m_pValue;
        }

    private:
        // Do not auto generate
        TlsValue();
        TlsValue(const TlsValue<T>&);
        TlsValue& operator=(const TlsValue<T>&);

        T* m_pValue;
        DWORD m_index;
        bool m_owns;
    };

//}

