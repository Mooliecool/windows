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
** Class: CacheObjType
**
**
** Purpose: Enum specifying the various cache obj types.
**
**
============================================================*/
namespace System.Reflection.Cache {

    [Serializable]
	internal enum CacheObjType {
        EmptyElement  = 0,
        ParameterInfo = 1,
        TypeName      = 2,
        RemotingData  = 3,
        SerializableAttribute = 4,
        AssemblyName = 5,
        ConstructorInfo = 6,
        FieldType = 7,
        FieldName = 8,
        DefaultMember = 9
    }
}
