//------------------------------------------------------------------------------
// <copyright file="XmlException.cs" company="Microsoft">
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

namespace System.Xml {
    using System;
    using System.IO;
    using System.Resources;
    using System.Runtime.Serialization;
    using System.Text;
    using System.Diagnostics;
    using System.Security.Permissions;
    using System.Globalization;
    using System.Threading;

    /// <devdoc>
    ///    <para>Returns detailed information about the last parse error, including the error
    ///       number, line number, character position, and a text description.</para>
    /// </devdoc>
    [Serializable]
    public class XmlException : SystemException {
        string res;
        string[] args; // this field is not used, it's here just V1.1 serialization compatibility
        int lineNumber;
        int linePosition;

        [OptionalField] 
        string sourceUri;

        string message;

        protected XmlException(SerializationInfo info, StreamingContext context) : base(info, context) {
            res                 = (string)  info.GetValue("res"  , typeof(string));
            args                = (string[])info.GetValue("args", typeof(string[]));
            lineNumber          = (int)     info.GetValue("lineNumber", typeof(int));
            linePosition        = (int)     info.GetValue("linePosition", typeof(int));

            // deserialize optional members
            sourceUri = string.Empty;
            string version = null;
            foreach ( SerializationEntry e in info ) {
                switch ( e.Name ) {
                    case "sourceUri":
                        sourceUri = (string)e.Value;
                        break;
                    case "version":
                        version = (string)e.Value;
                        break;
                }
            }

            if ( version == null ) {
                // deserializing V1 exception
                message = CreateMessage( res, args, lineNumber, linePosition );
            }
            else {
                // deserializing V2 or higher exception -> exception message is serialized by the base class (Exception._message)
                message = null;
            }
        }

        [SecurityPermissionAttribute(SecurityAction.Demand,SerializationFormatter=true)]
        public override void GetObjectData(SerializationInfo info, StreamingContext context) {
            base.GetObjectData(info, context);
            info.AddValue("res",                res);
            info.AddValue("args",               args);
            info.AddValue("lineNumber",         lineNumber);
            info.AddValue("linePosition",       linePosition);
            info.AddValue("sourceUri",          sourceUri);
            info.AddValue("version",            "2.0");
        }

        //provided to meet the ECMA standards
        public XmlException() : this(null) {
        }

        //provided to meet the ECMA standards
        public XmlException(String message) : this (message, ((Exception)null), 0, 0) {
#if DEBUG
            Debug.Assert(message == null || !message.StartsWith("Xml_", StringComparison.Ordinal), "Do not pass a resource here!");
#endif
        }
        
        //provided to meet ECMA standards
        public XmlException(String message, Exception innerException) : this (message, innerException, 0, 0) {
        } 

	//provided to meet ECMA standards
        public XmlException(String message, Exception innerException, int lineNumber, int linePosition) : 
            this( message, innerException, lineNumber, linePosition, null ) {
        }

        internal XmlException(String message, Exception innerException, int lineNumber, int linePosition, string sourceUri) : 
            this((message == null ? Res.Xml_DefaultException : Res.Xml_UserException), new string[] { message }, innerException, lineNumber, linePosition, sourceUri ) {
        }

        internal XmlException(string res, string[] args) :
            this(res, args, null, 0, 0, null) {}

        internal XmlException(string res, string[] args, string sourceUri) :
            this(res, args, null, 0, 0, sourceUri) {}

        internal XmlException(string res, string arg) :
            this(res, new string[] { arg }, null, 0, 0, null) {}

        internal XmlException(string res, string arg, string sourceUri) :
            this(res, new string[] { arg }, null, 0, 0, sourceUri) {}

        internal XmlException(string res, String arg,  IXmlLineInfo lineInfo) :
            this(res, new string[] { arg }, lineInfo, null) {}

        internal XmlException(string res, String arg, Exception innerException, IXmlLineInfo lineInfo) :
            this(res, new string[] { arg }, innerException, (lineInfo == null ? 0 : lineInfo.LineNumber), (lineInfo == null ? 0 : lineInfo.LinePosition), null) {}

        internal XmlException(string res, String arg,  IXmlLineInfo lineInfo, string sourceUri) :
            this(res, new string[] { arg }, lineInfo, sourceUri) {}

        internal XmlException(string res, string[] args,  IXmlLineInfo lineInfo) :
            this(res, args, lineInfo, null) {}

        internal XmlException(string res, string[] args,  IXmlLineInfo lineInfo, string sourceUri) :
            this (res, args, null, (lineInfo == null ? 0 : lineInfo.LineNumber), (lineInfo == null ? 0 : lineInfo.LinePosition), sourceUri) {
        }

        internal XmlException(string res,  int lineNumber, int linePosition) :
            this(res, (string[])null, null, lineNumber, linePosition) {}

        internal XmlException(string res, string arg, int lineNumber, int linePosition) :
            this(res,  new string[] { arg }, null, lineNumber, linePosition, null) {}

        internal XmlException(string res, string arg, int lineNumber, int linePosition, string sourceUri) :
            this(res,  new string[] { arg }, null, lineNumber, linePosition, sourceUri) {}

        internal XmlException(string res, string[] args, int lineNumber, int linePosition) :
            this( res, args, null, lineNumber, linePosition, null ) {}

        internal XmlException(string res, string[] args, int lineNumber, int linePosition, string sourceUri) :
            this( res, args, null, lineNumber, linePosition, sourceUri ) {}

        internal XmlException(string res, string[] args, Exception innerException, int lineNumber, int linePosition) : 
            this( res, args, innerException, lineNumber, linePosition, null ) {}

        internal XmlException(string res, string[] args, Exception innerException, int lineNumber, int linePosition, string sourceUri) :
            base( CreateMessage(res, args, lineNumber, linePosition), innerException ) {
            HResult = HResults.Xml;
            this.res = res;
            this.args = args;
            this.sourceUri = sourceUri;
            this.lineNumber = lineNumber;
            this.linePosition = linePosition;
        }

        private static string CreateMessage(string res, string[] args, int lineNumber, int linePosition) {
            try {
                string message = Res.GetString(res, args);

                if (lineNumber != 0) {
                    string[] msg = new string[2];
                    msg[0] = lineNumber.ToString(CultureInfo.InvariantCulture);
                    msg[1] = linePosition.ToString(CultureInfo.InvariantCulture);
                    message += " " + Res.GetString(Res.Xml_ErrorPosition, msg);
                }
                return message;
            }
            catch ( MissingManifestResourceException ) {
                return "UNKNOWN("+res+")";
            }
        }

        internal static string[] BuildCharExceptionStr(char ch) {
            string[] aStringList= new string[2];
            if ( (int)ch == 0 ) {
                aStringList[0] = ".";
            }
            else {
                aStringList[0] = ch.ToString(CultureInfo.InvariantCulture);
            }
            aStringList[1] = "0x"+ ((int)ch).ToString("X2", CultureInfo.InvariantCulture);
            return aStringList;
        }

        public int LineNumber {
            get { return this.lineNumber; }
        }

        public int LinePosition {
            get { return this.linePosition; }
        }

        public string SourceUri {
            get { return this.sourceUri; }
        }

        public override string Message {
            get { 
                return ( message == null ) ? base.Message : message;
            }
        }

        internal string ResString {
            get {
                return res;
            }
        }

        internal static bool IsCatchableException(Exception e) {
            Debug.Assert(e != null, "Unexpected null exception");
            return !(
                e is StackOverflowException ||
                e is OutOfMemoryException ||
                e is ThreadAbortException ||
                e is ThreadInterruptedException ||
                e is NullReferenceException ||
                e is AccessViolationException
            );
        }
    };
} // namespace System.Xml
