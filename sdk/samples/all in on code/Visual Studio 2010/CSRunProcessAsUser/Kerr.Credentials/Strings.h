//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    The internal Strings class is used to load strings from the
//                  embedded string table.
//
//*****************************************************************************

#pragma once

namespace Kerr
{
    ref class Strings abstract sealed
    {
    public:

        static String^ Get(String^ key);

    private:

        static Strings();
        static initonly Resources::ResourceManager^ m_resources;

    };
}
