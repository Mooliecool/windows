//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    Specifies the persistence of a credential.
//
//*****************************************************************************

#pragma once

namespace Kerr
{
    public enum class CredentialPersistence
    {
        None = 0,
        Session = CRED_PERSIST_SESSION,
        LocalComputer = CRED_PERSIST_LOCAL_MACHINE,
        Enterprise = CRED_PERSIST_ENTERPRISE
    };
}
