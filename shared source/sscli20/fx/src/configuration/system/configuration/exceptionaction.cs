//------------------------------------------------------------------------------
// <copyright file="ExceptionAction.cs" company="Microsoft">
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
    using System.Collections;
    using System.Collections.Specialized;
    using System.Collections.Generic;
    using System.Configuration;
    using System.Globalization;
    using System.IO;
    using System.Runtime.InteropServices;
    using System.Security;
    using System.Security.Permissions;
    using System.Text;
    using System.Xml;
    using System.Net;

    // ExceptionAction
    //
    // Value to change how we handle the Exception
    //
    internal enum ExceptionAction {
        NonSpecific,    // Not specific to a particular section, nor a global schema error
        Local,          // Error specific to a particular section
        Global,         // Error in the global (file) schema 
    }
}
