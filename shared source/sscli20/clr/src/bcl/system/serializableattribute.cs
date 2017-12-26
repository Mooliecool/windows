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
** Class: SerializableAttribute
**
**
** Purpose: Used to mark a class as being serializable
**
**
============================================================*/
namespace System {

    using System;
    using System.Reflection;

    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Enum | AttributeTargets.Delegate, Inherited = false)]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class SerializableAttribute : Attribute 
    {
        internal static Attribute GetCustomAttribute(Type type) 
        { 
            return (type.Attributes & TypeAttributes.Serializable) == TypeAttributes.Serializable ? new SerializableAttribute() : null; 
        }
        internal static bool IsDefined(Type type) 
        { 
            return type.IsSerializable; 
        }

        public SerializableAttribute() {
        }
    }
}
