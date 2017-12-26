//------------------------------------------------------------------------------
// <copyright file="XmlSpace.cs" company="Microsoft">
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
    // An enumeration for the xml:space scope used in XmlReader and XmlWriter.
    public enum XmlSpace
    {
        // xml:space scope has not been specified.
        None          = 0,

        // The xml:space scope is "default".
        Default       = 1,

        // The xml:space scope is "preserve".
        Preserve      = 2
    }
}
