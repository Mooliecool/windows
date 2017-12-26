//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    UserNameInvalidException is the exception that is thrown 
//                  when an invalid user name is specified.
//
//*****************************************************************************

#include "stdafx.h"
#include "UserNameInvalidException.h"
#include "Strings.h"

using namespace Runtime::Serialization;

Kerr::UserNameInvalidException::UserNameInvalidException(ComponentModel::Win32Exception^ innerException) :
    Exception(Strings::Get("UserNameInvalidException.Message"), innerException)
{
    // Do nothing
}

Kerr::UserNameInvalidException::UserNameInvalidException(SerializationInfo^ info,
                                                         StreamingContext context) :
    Exception(info, context)
{
    // Do nothing
}
