//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    The CredentialSet class allows you to enumerate the 
//                  credentials in the user's credential set.
//
//*****************************************************************************

#include "stdafx.h"
#include "CredentialSet.h"
#include "AutoCredentialBuffer.h"
#include "CredentialSetNotAvailableException.h"
#include "Strings.h"

//*****************************************************************************
//
//  Initializes a new instance of the CredentialSet class, loading all of the 
//  user's credentials.
//
//*****************************************************************************
Kerr::CredentialSet::CredentialSet()
{
    Load(String::Empty);
}

//*****************************************************************************
//
//  Initializes a new instance of the CredentialSet class, loading only those
//  credentials that match the filter.
//
//*****************************************************************************
Kerr::CredentialSet::CredentialSet(String^ filter)
{
    Load(filter);
}

//*****************************************************************************
//
//  Deletes all the Credential objects held by the CredentialSet object.
//
//*****************************************************************************
Kerr::CredentialSet::~CredentialSet()
{
    if (!m_disposed)
    {
        if (nullptr != m_list)
        {
            for each (Credential^ credential in m_list)
            {
                delete credential;
            }

            m_list = nullptr;
        }
    }

    Debug::Assert(nullptr == m_list);
}

//*****************************************************************************
//
//  Gets the number of Credential objects currently loaded.
//
//*****************************************************************************
int Kerr::CredentialSet::Count::get()
{
    CheckNotDisposed();
    return m_list->Count;
}

//*****************************************************************************
//
//  Gets the Credential object at the specified index.
//
//*****************************************************************************
Kerr::Credential^ Kerr::CredentialSet::default::get(int index)
{
    CheckNotDisposed();
    return m_list[index];
}

//*****************************************************************************
//
//  Gets a strongly-typed IEnumerator that can be used to enumerate over the 
//  Credential objects.
//
//*****************************************************************************
Collections::Generic::IEnumerator<Kerr::Credential^>^ Kerr::CredentialSet::GetGenericEnumerator()
{
    CheckNotDisposed();
    return m_list->GetEnumerator();
}

//*****************************************************************************
//
//  Gets an IEnumerator that can be used to enumerate over the Credential 
//  objects.
//
//*****************************************************************************
Collections::IEnumerator^ Kerr::CredentialSet::GetEnumerator()
{
    CheckNotDisposed();
    return m_list->GetEnumerator();
}

//*****************************************************************************
//
//  The private Load method is used by the constructors to actually load the
//  Credential objects.
//
//*****************************************************************************
void Kerr::CredentialSet::Load(String^ filter)
{
    Debug::Assert(nullptr == m_list);

    pin_ptr<const wchar_t> pinnedFilter = PtrToStringChars(filter);

    DWORD count = 0;
    AutoCredentialBuffer<PCREDENTIAL*> buffer;

    if (!::CredEnumerate(pinnedFilter,
                         0, // reserved
                         &count,
                         &buffer.m_p))
    {
        DWORD result = ::GetLastError();
        ComponentModel::Win32Exception^ innerException = gcnew ComponentModel::Win32Exception;

        switch (result)
        {
            case ERROR_NOT_FOUND:
            {
                // Don't fail since the credential set is simply empty.

                m_list = gcnew CredentialList;
                break;
            }
            case ERROR_NO_SUCH_LOGON_SESSION:
            {
                throw gcnew CredentialSetNotAvailableException(innerException);
            }
            default:
            {
                throw innerException;
            }
        }
    }
    else
    {
        m_list = gcnew CredentialList(count);

        for (DWORD index = 0; index != count; ++index)
        {
            m_list->Add(gcnew Credential(buffer.m_p[index]));
        }
    }
}

//*****************************************************************************
//
//  This private method is called by all the public members to check that the 
//  object has not been disposed.
//
//*****************************************************************************
void Kerr::CredentialSet::CheckNotDisposed()
{
    if (m_disposed)
    {
        throw gcnew ObjectDisposedException(String::Empty,
                                            Strings::Get("ObjectDisposedException.Message"));
    }
}
