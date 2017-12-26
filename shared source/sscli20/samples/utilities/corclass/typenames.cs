//------------------------------------------------------------------------------
// <copyright file="typenames.cs" company="Microsoft">
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


// Really a bitfield
namespace DefaultNamespace {

    using System.Diagnostics;
    using System;

    [Flags]
    public enum TypeNames {

        AbbrPrimitives = 0x01,

        NoAbbrPrimitives = 0x00,

        PkgNames = 0x02,

        NoPkgNames = 0x00,
    
        AbbrPkgNames = 0x04,

        NoAbbrPkgNames = 0x00,
    
        Long   = PkgNames   | NoAbbrPrimitives | NoAbbrPkgNames,

        Medium = PkgNames   | AbbrPrimitives   | AbbrPkgNames,

        Short  = NoPkgNames | AbbrPrimitives,
    }
}
