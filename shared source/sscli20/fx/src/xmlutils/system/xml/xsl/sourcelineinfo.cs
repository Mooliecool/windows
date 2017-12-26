//------------------------------------------------------------------------------
// <copyright file="SourceLineInfo.cs" company="Microsoft">
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

using System.Diagnostics;

namespace System.Xml.Xsl {

    [DebuggerDisplay("{uriString} [{startLine},{startPos} -- {endLine},{endPos}]")]
    internal class SourceLineInfo : ISourceLineInfo {
        private string  uriString;
        private int     startLine;
        private int     startPos;
        private int     endLine;
        private int     endPos;

        public SourceLineInfo(string uriString, int startLine, int startPos, int endLine, int endPos) {
            this.uriString = uriString;
            this.startLine = startLine;
            this.startPos  = startPos;
            this.endLine   = endLine;
            this.endPos    = endPos;
        }

        public string Uri       { get { return this.uriString; } }
        public int    StartLine { get { return this.startLine; } }
        public int    StartPos  { get { return this.startPos ; } }
        public int    EndLine   { get { return this.endLine  ; } }
        public int    EndPos    { get { return this.endPos   ; } }

        internal void SetEndLinePos(int endLine, int endPos) {
            this.endLine = endLine;
            this.endPos  = endPos;
        }

        /// <summary>
        /// Magic number 0xfeefee is used in PDB to denote a section of IL that does not map to any user code.
        /// When VS debugger steps into IL marked with 0xfeefee, it will continue the step until it reaches
        /// some user code.
        /// </summary>
        private const int NoSourceMagicNumber = 0xfeefee;

        public static SourceLineInfo NoSource = new SourceLineInfo(string.Empty, NoSourceMagicNumber, 0, NoSourceMagicNumber, 0);

        public bool IsNoSource {
            get { return this.startLine == NoSourceMagicNumber; }
        }

        [Conditional("DEBUG")]
        public static void Validate(ISourceLineInfo lineInfo) {
            if (lineInfo.StartLine == 0 || lineInfo.StartLine == NoSourceMagicNumber) {
                Debug.Assert(lineInfo.StartLine == lineInfo.EndLine);
                Debug.Assert(lineInfo.StartPos == 0 && lineInfo.EndPos == 0);
            } else {
                Debug.Assert(0 < lineInfo.StartLine && lineInfo.StartLine <= lineInfo.EndLine);
                if (lineInfo.StartLine == lineInfo.EndLine) {
                    Debug.Assert(0 < lineInfo.StartPos && lineInfo.StartPos < lineInfo.EndPos);
                } else {
                    Debug.Assert(0 < lineInfo.StartPos && 0 < lineInfo.EndPos);
                }
            }
        }

        // Returns file path for local and network URIs. Used for PDB generating and error reporting.
        public static string GetFileName(string uriString) {
            Uri uri;

            if (uriString.Length != 0 &&
                System.Uri.TryCreate(uriString, UriKind.Absolute, out uri) &&
                uri.IsFile
            ) {
                return uri.LocalPath;
            }
            return uriString;
        }
    }
}
