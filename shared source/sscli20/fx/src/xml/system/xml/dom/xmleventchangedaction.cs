//------------------------------------------------------------------------------
// <copyright file="XmlEventChangedAction.cs" company="Microsoft">
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

namespace System.Xml
{
    // Specifies the type of node change
    public enum XmlNodeChangedAction
    {
        // A node is beeing inserted in the tree.
        Insert = 0,

        // A node is beeing removed from the tree.
        Remove = 1,

        // A node value is beeing changed.
        Change = 2
    }
}
