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

#pragma once

namespace Kerr
{
    [Serializable]
    public ref class CredentialSetNotAvailableException : Exception
    {
    internal:

        CredentialSetNotAvailableException(ComponentModel::Win32Exception^ innerException);

    protected:

        CredentialSetNotAvailableException(Runtime::Serialization::SerializationInfo^ info,
                                           Runtime::Serialization::StreamingContext context);
    };
}
