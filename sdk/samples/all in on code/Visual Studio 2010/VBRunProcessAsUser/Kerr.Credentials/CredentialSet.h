//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    The CredentialSet class allows you to enumerate the 
//                  credentials in the user's credential set.
//
//*****************************************************************************

#pragma once

#include "Credential.h"

namespace Kerr
{
    public ref class CredentialSet :
        Collections::Generic::IEnumerable<Credential^>
    {
    public:

        [SecurityPermission(SecurityAction::LinkDemand, Flags=SecurityPermissionFlag::UnmanagedCode)]
        CredentialSet();
        
        [SecurityPermission(SecurityAction::LinkDemand, Flags=SecurityPermissionFlag::UnmanagedCode)]
        CredentialSet(String^ filter);

        ~CredentialSet();

#ifdef _DEBUG
        !CredentialSet()
        {
            Debug::Assert(m_disposed,
                          "CredentialSet object was not disposed.");
        }
#endif

        property int Count
        {
            int get();
        }

        property Credential^ default[int]
        {
            Credential^ get(int index);
        }

    private:

        virtual Collections::Generic::IEnumerator<Credential^>^ GetGenericEnumerator() sealed = Collections::Generic::IEnumerable<Credential^>::GetEnumerator;
        virtual Collections::IEnumerator^ GetEnumerator() sealed = Collections::IEnumerable::GetEnumerator;

        void Load(String^ filter);

        void CheckNotDisposed();

        bool m_disposed;

        typedef Collections::Generic::List<Credential^> CredentialList;
        CredentialList^ m_list;

    };
}
