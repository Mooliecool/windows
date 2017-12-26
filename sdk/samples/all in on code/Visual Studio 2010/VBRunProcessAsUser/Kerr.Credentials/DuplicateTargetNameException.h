//*****************************************************************************
//
//  Author:         Kenny Kerr
//  Date created:   31 July 2005
//
//  Description:    DuplicateTargetNameException is the exception that is 
//                  thrown when an attempt is made to change a credential's
//                  target name but a different credential already occupies 
//                  that name.
//
//*****************************************************************************

#pragma once

namespace Kerr
{
    [Serializable]
    public ref class DuplicateTargetNameException : Exception
    {
    internal:

        DuplicateTargetNameException(ComponentModel::Win32Exception^ innerException);

    protected:

        DuplicateTargetNameException(Runtime::Serialization::SerializationInfo^ info,
                                     Runtime::Serialization::StreamingContext context);
    };
}
