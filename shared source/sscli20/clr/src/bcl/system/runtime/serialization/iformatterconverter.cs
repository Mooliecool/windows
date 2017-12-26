// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
/*============================================================
**
** Class: IFormatterConverter
**
**
** Purpose: The interface provides the connection between an
** instance of SerializationInfo and the formatter-provided
** class which knows how to parse the data inside the 
** SerializationInfo.
**
**
============================================================*/
namespace System.Runtime.Serialization {
    using System;

    [CLSCompliant(false)]
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IFormatterConverter {
        Object Convert(Object value, Type type);
        Object Convert(Object value, TypeCode typeCode);
        bool   ToBoolean(Object value);
        char   ToChar(Object value);
        sbyte  ToSByte(Object value);
        byte   ToByte(Object value);
        short  ToInt16(Object value);
        ushort ToUInt16(Object value);
        int    ToInt32(Object value);
        uint   ToUInt32(Object value);
        long   ToInt64(Object value);
        ulong  ToUInt64(Object value);
        float  ToSingle(Object value);
        double ToDouble(Object value);
        Decimal ToDecimal(Object value);
        DateTime ToDateTime(Object value);
        String   ToString(Object value);
    }
}
