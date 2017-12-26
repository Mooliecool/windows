//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    CredentialSetNotAvailableException is the exception that is 
//                  thrown when an attempt is made to access the user's 
//                  credential set but one is not associated with the logon 
//                  session.
//
//*****************************************************************************

#include "stdafx.h"
#include "CredentialSetNotAvailableException.h"
#include "Strings.h"

using namespace Runtime::Serialization;

Kerr::CredentialSetNotAvailableException::CredentialSetNotAvailableException(ComponentModel::Win32Exception^ innerException) :
    Exception(Strings::Get("CredentialSetNotAvailableException.Message"), innerException)
{
    // Do nothing
}

Kerr::CredentialSetNotAvailableException::CredentialSetNotAvailableException(SerializationInfo^ info,
                                                                             StreamingContext context) :
    Exception(info, context)
{
    // Do nothing
}
