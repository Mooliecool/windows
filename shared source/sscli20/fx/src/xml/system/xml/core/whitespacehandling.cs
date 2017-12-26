//------------------------------------------------------------------------------
// <copyright file="WhiteSpaceHandling.cs" company="Microsoft">
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
    // Specifies how whitespace is handled in XmlTextReader.
    public enum WhitespaceHandling
    {
        // Return all Whitespace and SignificantWhitespace nodes. This is the default.
        All              = 0,

        // Return just SignificantWhitespace, i.e. whitespace nodes that are in scope of xml:space="preserve"
        Significant      = 1,

        // Do not return any Whitespace or SignificantWhitespace nodes.
        None             = 2
    }
}
