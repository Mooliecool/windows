//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    The internal Strings class is used to load strings from the
//                  embedded string table.
//
//*****************************************************************************

#include "stdafx.h"
#include "Strings.h"

String^ Kerr::Strings::Get(String^ key)
{
    Debug::Assert(nullptr != key);
    Debug::Assert(0 < key->Length);

    String^ value = m_resources->GetString(key);

    Debug::Assert(nullptr != value);
    return value;
}

static Kerr::Strings::Strings()
{
    Reflection::Assembly^ assembly = Reflection::Assembly::GetExecutingAssembly();

    m_resources = gcnew Resources::ResourceManager("KerrCredentials.Strings",
                                                   assembly);
}
