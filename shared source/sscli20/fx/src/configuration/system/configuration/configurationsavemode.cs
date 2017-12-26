//------------------------------------------------------------------------------
// <copyright file="ConfigurationSaveMode.cs" company="Microsoft">
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

    // Determines how much of configuration is written out on save.
    public enum ConfigurationSaveMode {
        // If a setting is modified, it'll get written no matter it's
        // same as the parent or not.
        Modified = 0,

        // If a setting is the same as in its parent, it won't get written
        Minimal  = 1,

        // It writes out all the properties in the configurationat that level,
        // including the one from the parents.  Used for writing out the
        // full config settings at a file.
        Full     = 2,
    }
}

