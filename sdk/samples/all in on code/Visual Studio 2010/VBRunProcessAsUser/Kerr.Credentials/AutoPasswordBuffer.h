//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    The AutoPasswordBuffer template class takes care of 
//                  automatically and securely zeroing out a simple buffer.
//
//*****************************************************************************

#pragma once

namespace Kerr
{
    template <typename int Length>
    class AutoPasswordBuffer
    {
    public:

        AutoPasswordBuffer()
        {
            ::ZeroMemory(m_buffer,
                         sizeof (Length));
        }

        ~AutoPasswordBuffer()
        {
            ::SecureZeroMemory(m_buffer,
                               sizeof (Length));
        }

        wchar_t m_buffer[Length];

    };
}
