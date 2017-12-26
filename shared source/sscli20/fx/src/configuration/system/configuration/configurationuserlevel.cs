//------------------------------------------------------------------------------
// <copyright file="ConfigurationUserLevel.cs" company="Microsoft">
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

using ClassConfiguration = System.Configuration.Configuration;
using System.Collections;
using System.Configuration;
using System.Configuration.Internal;
using System.IO;
using System.Reflection;
using System.Security;
using System.Security.Permissions;
using System.Threading;

namespace System.Configuration {

    // Represents which user.config files are included in the configuration.
    public enum ConfigurationUserLevel {
        None                   = 0,
        PerUserRoaming         = 10,
        PerUserRoamingAndLocal = 20,
    }
}

