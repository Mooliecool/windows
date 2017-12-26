//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    The AutoGdiObject template class takes care of 
//                  automatically deleting GDI objects such as pens, brushes 
//                  and bitmaps.
//
//*****************************************************************************

#pragma once

namespace Kerr
{
    template <typename T>
    class AutoGdiObject
    {
    public:

        AutoGdiObject() :
            m_handle(0)
        {
            // Do nothing
        }

        ~AutoGdiObject()
        {
            if (0 != m_handle)
            {
                BOOL result = ::DeleteObject(m_handle);
                Debug::Assert(0 != result);
            }
        }

        T m_handle;

    private:

        AutoGdiObject(const AutoGdiObject&);
        AutoGdiObject& operator=(const AutoGdiObject&);

    };
}
