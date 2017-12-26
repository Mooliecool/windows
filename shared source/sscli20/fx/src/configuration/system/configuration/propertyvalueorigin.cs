//------------------------------------------------------------------------------
// <copyright file="PropertyValueOrigin.cs" company="Microsoft">
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
using System.Configuration;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Collections;
using System.Runtime.Serialization;

namespace System.Configuration {

    public enum PropertyValueOrigin {
        Default = 0,   // Default is retrieved
        Inherited = 1, // It is inherited
        SetHere = 2    // It was set here
    }
}
