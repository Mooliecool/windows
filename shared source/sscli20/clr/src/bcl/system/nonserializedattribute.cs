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
** Class: NonSerializedAttribute
**
**
** Purpose: Used to mark a member as being not-serialized
**
**
============================================================*/
namespace System 
{
	using System.Reflection;

    [AttributeUsage(AttributeTargets.Field, Inherited=false)]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class NonSerializedAttribute : Attribute 
    {
		internal static Attribute GetCustomAttribute(RuntimeFieldInfo field) 
        { 
            if ((field.Attributes & FieldAttributes.NotSerialized) == 0)
                return null;

            return new NonSerializedAttribute();
        }

		internal static bool IsDefined(RuntimeFieldInfo field) 
        { 
            return (field.Attributes & FieldAttributes.NotSerialized) != 0;
        }

        public NonSerializedAttribute() { }
    }
}
