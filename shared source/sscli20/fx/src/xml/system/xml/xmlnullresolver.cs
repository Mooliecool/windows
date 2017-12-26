//------------------------------------------------------------------------------
// <copyright file="XmlNullResolver.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

namespace System.Xml {
    using System;

    internal class XmlNullResolver : XmlUrlResolver {
        public static readonly XmlNullResolver Singleton = new XmlNullResolver();
        
        public override Object GetEntity(Uri absoluteUri, string role, Type ofObjectToReturn) {
            throw new XmlException(Res.Xml_NullResolver, string.Empty);
        }
    }
}
