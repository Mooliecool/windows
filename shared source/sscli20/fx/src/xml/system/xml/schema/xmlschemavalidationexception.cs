//------------------------------------------------------------------------------
// <copyright file="XmlSchemaValidationException.cs" company="Microsoft">
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
    using System;
    using System.IO;
    using System.Text;  
    using System.Resources;
    using System.Runtime.Serialization;
    using System.Diagnostics;
	using System.Security.Permissions;

    /// <include file='doc\XmlSchemaException.uex' path='docs/doc[@for="XmlSchemaException"]/*' />
    [Serializable]
    public class XmlSchemaValidationException : XmlSchemaException {
        
        private Object sourceNodeObject;
               
        /// <include file='doc\XmlSchemaException.uex' path='docs/doc[@for="XmlSchemaException.XmlSchemaException5"]/*' />
        protected XmlSchemaValidationException(SerializationInfo info, StreamingContext context) : base(info, context) {}
            

        /// <include file='doc\XmlSchemaException.uex' path='docs/doc[@for="XmlSchemaException.GetObjectData"]/*' />
        [SecurityPermissionAttribute(SecurityAction.Demand,SerializationFormatter=true)]
        public override void GetObjectData(SerializationInfo info, StreamingContext context) {
            base.GetObjectData(info, context);
        }

        /// <include file='doc\XmlSchemaException.uex' path='docs/doc[@for="XmlSchemaException.XmlSchemaException1"]/*' />
        public XmlSchemaValidationException() : base(null) {
        }

        /// <include file='doc\XmlSchemaException.uex' path='docs/doc[@for="XmlSchemaException.XmlSchemaException2"]/*' />
        public XmlSchemaValidationException(String message) : base (message, ((Exception)null), 0, 0) {
        }
        
        /// <include file='doc\XmlSchemaException.uex' path='docs/doc[@for="XmlSchemaException.XmlSchemaException0"]/*' />
        public XmlSchemaValidationException(String message, Exception innerException) : base (message, innerException, 0, 0) {
        } 

        /// <include file='doc\XmlSchemaException.uex' path='docs/doc[@for="XmlSchemaException.XmlSchemaException3"]/*' />
        public XmlSchemaValidationException(String message, Exception innerException, int lineNumber, int linePosition) : 
            base(message, innerException, lineNumber, linePosition) {
	    }
            
        internal XmlSchemaValidationException(string res, string[] args) : base(res, args, null, null, 0, 0, null) {
        }
        
        internal XmlSchemaValidationException(string res, string arg) : base(res, new string[] { arg }, null, null, 0, 0, null) {
        }

        internal XmlSchemaValidationException(string res, string arg, string sourceUri, int lineNumber, int linePosition) :
            base(res, new string[] { arg }, null, sourceUri, lineNumber, linePosition, null) {
        }

        internal XmlSchemaValidationException(string res, string sourceUri, int lineNumber, int linePosition) :
            base(res, (string[])null, null, sourceUri, lineNumber, linePosition, null) {
        }

        internal XmlSchemaValidationException(string res, string[] args, string sourceUri, int lineNumber, int linePosition) :
            base(res, args, null, sourceUri, lineNumber, linePosition, null) {
        }

        internal XmlSchemaValidationException(string res, string[] args, Exception innerException, string sourceUri, int lineNumber, int linePosition) :
            base(res, args, innerException, sourceUri, lineNumber, linePosition, null) {
        }

        internal XmlSchemaValidationException(string res, string[] args, object sourceNode) :
            base(res, args, null, null,  0, 0, null) {
                this.sourceNodeObject = sourceNode;
        }

        internal XmlSchemaValidationException(string res, string[] args, string sourceUri, object sourceNode) :
            base(res, args, null, sourceUri,  0, 0, null) {
                this.sourceNodeObject = sourceNode;
        }

        internal XmlSchemaValidationException(string res, string[] args, string sourceUri, int lineNumber, int linePosition, XmlSchemaObject source, object sourceNode) :
            base(res, args, null, sourceUri, lineNumber, linePosition, source) {
                this.sourceNodeObject = sourceNode;
        }
        
         /// <include file='doc\XmlSchemaException.uex' path='docs/doc[@for="XmlSchemaException.SourceUri"]/*' />
        public Object SourceObject {
            get { return this.sourceNodeObject; }
        }

        protected internal void SetSourceObject (Object sourceObject){
            this.sourceNodeObject = sourceObject;
        }

    };
} // namespace System.Xml.Schema


