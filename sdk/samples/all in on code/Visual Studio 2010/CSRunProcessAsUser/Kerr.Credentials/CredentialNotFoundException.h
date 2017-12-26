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

#pragma once

namespace Kerr
{
    [Serializable]
    public ref class CredentialNotFoundException : Exception
    {
    internal:

        CredentialNotFoundException(ComponentModel::Win32Exception^ innerException);

    protected:

        CredentialNotFoundException(Runtime::Serialization::SerializationInfo^ info,
                                    Runtime::Serialization::StreamingContext context);
    };
}
