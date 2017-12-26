//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    Specifies the type of a credential. The type cannot be 
//                  changed after a particular credential is created.
//
//*****************************************************************************

#pragma once

namespace Kerr
{
    public enum class CredentialType
    {
        None = 0,
        Generic = CRED_TYPE_GENERIC,
        DomainPassword = CRED_TYPE_DOMAIN_PASSWORD,
        DomainCertificate = CRED_TYPE_DOMAIN_CERTIFICATE,
        DomainVisiblePassword = CRED_TYPE_DOMAIN_VISIBLE_PASSWORD
    };
}
