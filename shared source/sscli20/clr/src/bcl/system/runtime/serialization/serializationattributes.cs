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
** Class: OptionallySerializableAttribute
**
**
** Purpose: Various Attributes for Serialization 
**
**
============================================================*/
namespace System.Runtime.Serialization
{
	using System.Reflection;

    [AttributeUsage(AttributeTargets.Field, Inherited=false)]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class OptionalFieldAttribute : Attribute 
    {
        int versionAdded = 1;
        public OptionalFieldAttribute() { }
        
        public int VersionAdded 
        {
            get {
                return this.versionAdded;
            }
            set {
                if (value < 1)
                    throw new ArgumentException(Environment.GetResourceString("Serialization_OptionalFieldVersionValue"));
                this.versionAdded = value;
            }
        }
    }

    [AttributeUsage(AttributeTargets.Method, Inherited=false)]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class OnSerializingAttribute : Attribute 
    {
    }

    [AttributeUsage(AttributeTargets.Method, Inherited=false)]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class OnSerializedAttribute : Attribute 
    {
    }

    [AttributeUsage(AttributeTargets.Method, Inherited=false)]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class OnDeserializingAttribute : Attribute 
    {
    }

    [AttributeUsage(AttributeTargets.Method, Inherited=false)]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class OnDeserializedAttribute : Attribute 
    {
    }

}
