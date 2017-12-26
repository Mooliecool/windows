//------------------------------------------------------------------------------
// <copyright file="XmlSeverityType.cs" company="Microsoft">
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
// <owner current="true" primary="true">priyal</owner>                                                                
//------------------------------------------------------------------------------

namespace System.Xml.Schema {
    //UE Atention
    /// <include file='doc\XmlSeverityType.uex' path='docs/doc[@for="XmlSeverityType"]/*' />
    public enum XmlSeverityType {
        /// Errors that can be recovered from. 
        /// <include file='doc\XmlSeverityType.uex' path='docs/doc[@for="XmlSeverityType.Error"]/*' />
        Error,
        /// Errors that can be ignored
        /// <include file='doc\XmlSeverityType.uex' path='docs/doc[@for="XmlSeverityType.Warning"]/*' />
        Warning
    }
}

