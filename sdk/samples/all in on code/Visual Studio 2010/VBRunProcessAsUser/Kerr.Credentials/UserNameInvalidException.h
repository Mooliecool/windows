//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    UserNameInvalidException is the exception that is thrown 
//                  when an invalid user name is specified.
//
//*****************************************************************************

#pragma once

namespace Kerr
{
    [Serializable]
    public ref class UserNameInvalidException : Exception
    {
    internal:

        UserNameInvalidException(ComponentModel::Win32Exception^ innerException);

    protected:

        UserNameInvalidException(Runtime::Serialization::SerializationInfo^ info,
                                 Runtime::Serialization::StreamingContext context);
    };
}
