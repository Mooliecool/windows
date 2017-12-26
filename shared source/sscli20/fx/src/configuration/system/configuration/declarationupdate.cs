//------------------------------------------------------------------------------
// <copyright file="DeclarationUpdate.cs" company="Microsoft">
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

    //
    // Trivially derived class of Update to represent an update
    // to the declaration of a section.
    //
    internal class DeclarationUpdate : Update {
        internal DeclarationUpdate(string configKey, bool moved, string updatedXml) : base(configKey, moved, updatedXml) {
        }
    }
}
