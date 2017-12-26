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

using System;

namespace System.Runtime.CompilerServices
{
    using System;
    [AttributeUsage(AttributeTargets.Assembly, AllowMultiple=true, Inherited=false)]
    public sealed class TypeForwardedToAttribute : Attribute
    {
        private Type _destination;

        public TypeForwardedToAttribute(Type destination)
        {
            _destination = destination;
        }

        public Type Destination
        {
            get {
                return _destination;
            }
        }
    }
}


 

