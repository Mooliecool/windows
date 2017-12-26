//------------------------------------------------------------------------------
// <copyright file="SourceLevels.cs" company="Microsoft">
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

namespace System.Diagnostics {

    [Flags]
    public enum SourceLevels {
        Off         = 0,
        Critical    = 0x01,
        Error       = 0x03,
        Warning     = 0x07,
        Information = 0x0F,
        Verbose     = 0x1F,

        [EditorBrowsable(EditorBrowsableState.Advanced)]
        ActivityTracing = 0xFF00,
        All             = unchecked ((int) 0xFFFFFFFF),
    }
}

