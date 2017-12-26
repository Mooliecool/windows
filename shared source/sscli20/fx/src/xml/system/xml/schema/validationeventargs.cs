//------------------------------------------------------------------------------
// <copyright file="ValidationEventArgs.cs" company="Microsoft">
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
    /// <include file='doc\ValidationEventArgs.uex' path='docs/doc[@for="ValidationEventArgs"]/*' />
    /// <devdoc>
    ///    Returns detailed information relating to
    ///    the ValidationEventhandler.
    /// </devdoc>
    public class ValidationEventArgs : EventArgs {
        XmlSchemaException ex;
        XmlSeverityType severity;

        internal ValidationEventArgs( XmlSchemaException ex ) : base() {
            this.ex = ex; 
            severity = XmlSeverityType.Error;
        }
 
        internal ValidationEventArgs( XmlSchemaException ex , XmlSeverityType severity ) : base() {
            this.ex = ex; 
            this.severity = severity;
        }

        /// <include file='doc\ValidationEventArgs.uex' path='docs/doc[@for="ValidationEventArgs.Severity"]/*' />
        public XmlSeverityType Severity {
            get { return severity;}
        }

        /// <include file='doc\ValidationEventArgs.uex' path='docs/doc[@for="ValidationEventArgs.Exception"]/*' />
        public XmlSchemaException Exception {
            get { return ex;}
        }

        /// <include file='doc\ValidationEventArgs.uex' path='docs/doc[@for="ValidationEventArgs.Message"]/*' />
        /// <devdoc>
        ///    <para>Gets the text description corresponding to the
        ///       validation error.</para>
        /// </devdoc>
        public String Message {
            get { return ex.Message;}
        }
    }
}
