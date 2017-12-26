//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    The AutoPasswordBstr template class takes care of 
//                  automatically and securely zeroing out and freeing a BSTR.
//
//*****************************************************************************

#pragma once

namespace Kerr
{
    class AutoPasswordBstr
    {
    public:

        AutoPasswordBstr() :
            m_bstr(0)
        {
            // Do nothing
        }

        ~AutoPasswordBstr()
        {
            if (0 != m_bstr)
            {
                ::SecureZeroMemory(m_bstr,
                                   ::SysStringByteLen(m_bstr));

                ::SysFreeString(m_bstr);
            }
        }

        BSTR m_bstr;

    private:

        AutoPasswordBstr(const AutoPasswordBstr&);
        AutoPasswordBstr& operator=(const AutoPasswordBstr&);

    };
}
