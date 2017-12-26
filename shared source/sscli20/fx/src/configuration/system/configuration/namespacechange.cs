//------------------------------------------------------------------------------
// <copyright file="NamespaceChange.cs" company="Microsoft">
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

namespace System.Configuration {
    using System.Configuration.Internal;
    using System.CodeDom.Compiler;
    using System.Collections;
    using System.Configuration;
    using System.Globalization;
    using System.IO;
    using System.Reflection;
    using System.Security.Permissions;
    using System.Security;
    using System.Text;
    using System.Xml;
    using System.Collections.Specialized;

    enum NamespaceChange {
        None    = 0,
        Add     = 1,
        Remove  = 2,
    }
}
