//------------------------------------------------------------------------------
// <copyright file="ConfigurationPropertyOptions.cs" company="Microsoft">
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

using System;
using System.ComponentModel;

namespace System.Configuration {
    [Flags]
    public enum ConfigurationPropertyOptions {
        None = 0,
        IsDefaultCollection = 0x00000001,
        IsRequired = 0x00000002,
        IsKey = 0x00000004,
    }
}
