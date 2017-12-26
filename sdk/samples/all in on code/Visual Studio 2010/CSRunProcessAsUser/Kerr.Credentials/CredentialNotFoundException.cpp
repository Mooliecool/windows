//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    CredentialNotFoundException is the exception that is thrown
//                  when an attempt to load or reference a credential that does
//                  not exist fails.
//
//*****************************************************************************

#include "stdafx.h"
#include "CredentialNotFoundException.h"
#include "Strings.h"

using namespace Runtime::Serialization;

Kerr::CredentialNotFoundException::CredentialNotFoundException(ComponentModel::Win32Exception^ innerException) :
    Exception(Strings::Get("CredentialNotFoundException.Message"), innerException)
{
    // Do nothing
}

Kerr::CredentialNotFoundException::CredentialNotFoundException(SerializationInfo^ info,
                                                               StreamingContext context) :
    Exception(info, context)
{
    // Do nothing
}
