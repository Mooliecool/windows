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
** File:        StringFreezingAttribute.cs
**
**
** Purpose:     Custom attribute to indicate that strings should be frozen
**
**
===========================================================*/

namespace System.Runtime.CompilerServices
{
    
    [AttributeUsage(AttributeTargets.Assembly, Inherited = false),Serializable] 
    public sealed class StringFreezingAttribute : Attribute
    {
        public StringFreezingAttribute()
        {
        }
    }
}
