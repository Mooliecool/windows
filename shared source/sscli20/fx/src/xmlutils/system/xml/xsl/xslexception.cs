//------------------------------------------------------------------------------
// <copyright file="XslException.cs" company="Microsoft">
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

using System.CodeDom.Compiler;
using System.Diagnostics;
using System.Globalization;
using System.Resources;
using System.Runtime.Serialization;
using System.Security.Permissions;
using System.Text;

namespace System.Xml.Xsl {
    using Res = System.Xml.Utils.Res;

    [Serializable]
    internal class XslTransformException : XsltException {

        protected XslTransformException(SerializationInfo info, StreamingContext context)
            : base(info, context) {}

        public XslTransformException(Exception inner, string res, params string[] args)
            : base(CreateMessage(res, args), inner) {}

        public XslTransformException(string message)
            : base(CreateMessage(message, null), null) {}

        internal XslTransformException(string res, params string[] args)
            : this(null, res, args) {}

        internal static string CreateMessage(string res, params string[] args) {
            string message = null;

            try {
                message = Res.GetString(res, args);
            }
            catch (MissingManifestResourceException) {
            }

            if (message != null) {
                return message;
            }

            StringBuilder sb = new StringBuilder(res);
            if (args != null && args.Length > 0) {
                Debug.Fail("Resource string '" + res + "' was not found");
                sb.Append('(');
                sb.Append(args[0]);
                for (int idx = 1; idx < args.Length; idx++) {
                    sb.Append(", ");
                    sb.Append(args[idx]);
                }
                sb.Append(')');
            }
            return sb.ToString();
        }

        internal virtual string FormatDetailedMessage() {
            return Message;
        }

        public override string ToString() {
            string result = this.GetType().FullName;
            string info = FormatDetailedMessage();
            if (info != null && info.Length > 0) {
                result += ": " + info;
            }
            if (InnerException != null) {
                result += " ---> " + InnerException.ToString() + Environment.NewLine + "   " + CreateMessage(Res.Xml_EndOfInnerExceptionStack);
            }
            if (StackTrace != null) {
                result += Environment.NewLine + StackTrace;
            }
            return result;
        }
    }

    [Serializable]
    internal class XslLoadException : XslTransformException {
        ISourceLineInfo lineInfo;

        protected XslLoadException (SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
            bool hasLineInfo = (bool) info.GetValue("hasLineInfo", typeof(bool));

            if (hasLineInfo) {
                string  fileName;
                int     startLine, startPos, endLine, endPos;

                fileName    = (string)   info.GetValue("FileName"   , typeof(string ));
                startLine   = (int)      info.GetValue("StartLine"  , typeof(int    ));
                startPos    = (int)      info.GetValue("StartPos"   , typeof(int    ));
                endLine     = (int)      info.GetValue("EndLine"    , typeof(int    ));
                endPos      = (int)      info.GetValue("EndPos"     , typeof(int    ));

                lineInfo = new SourceLineInfo(fileName, startLine, startPos, endLine, endPos);
            }
        }

        [SecurityPermissionAttribute(SecurityAction.Demand, SerializationFormatter=true)]
        public override void GetObjectData(SerializationInfo info, StreamingContext context) {
            base.GetObjectData(info, context);
            info.AddValue("hasLineInfo"  , lineInfo != null);

            if (lineInfo != null) {
                info.AddValue("Uri"      , lineInfo.Uri);
                info.AddValue("StartLine", lineInfo.StartLine);
                info.AddValue("StartPos" , lineInfo.StartPos );
                info.AddValue("EndLine"  , lineInfo.EndLine);
                info.AddValue("EndPos"   , lineInfo.EndPos );
            }
        }

        internal XslLoadException(string res, params string[] args)
            : base(null, res, args) {}

        internal XslLoadException(Exception inner, ISourceLineInfo lineInfo)
            : base(inner, Res.Xslt_CompileError2, null)
        {
            this.lineInfo = lineInfo;
        }

        internal XslLoadException(CompilerError error)
            : base(Res.Xml_UserException, new string[] { error.ErrorText })
        {
            SetSourceLineInfo(new SourceLineInfo(error.FileName, error.Line, error.Column, error.Line, error.Column));
        }

        internal void SetSourceLineInfo(ISourceLineInfo lineInfo) {
            this.lineInfo = lineInfo;
        }

        public override string SourceUri {
            get { return lineInfo != null ? lineInfo.Uri : null; }
        }

        public override int LineNumber {
            get { return lineInfo != null ? lineInfo.StartLine : 0; }
        }

        public override int LinePosition {
            get { return lineInfo != null ? lineInfo.StartPos : 0; }
        }

        private static string AppendLineInfoMessage(string message, ISourceLineInfo lineInfo) {
            if (lineInfo != null) {
                string fileName = SourceLineInfo.GetFileName(lineInfo.Uri);
                string lineInfoMessage = CreateMessage(Res.Xml_ErrorFilePosition, fileName, lineInfo.StartLine.ToString(CultureInfo.InvariantCulture), lineInfo.StartPos.ToString(CultureInfo.InvariantCulture));
                if (lineInfoMessage != null && lineInfoMessage.Length > 0) {
                    if (message.Length > 0 && !XmlCharType.Instance.IsWhiteSpace(message[message.Length - 1])) {
                        message += " ";
                    }
                    message += lineInfoMessage;
                }
            }
            return message;
        }

        internal static string CreateMessage(ISourceLineInfo lineInfo, string res, params string[] args) {
            return AppendLineInfoMessage(CreateMessage(res, args), lineInfo);
        }

        internal override string FormatDetailedMessage() {
            return AppendLineInfoMessage(Message, lineInfo);
        }
    }
}
