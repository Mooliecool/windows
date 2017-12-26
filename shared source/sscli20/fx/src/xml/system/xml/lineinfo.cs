//------------------------------------------------------------------------------
// <copyright file="LineInfo.cs" company="Microsoft">
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

    internal struct LineInfo {
        internal int lineNo;
        internal int linePos;

        public LineInfo( int lineNo, int linePos ) {
            this.lineNo = lineNo;
            this.linePos = linePos;
        }

        public void Set( int lineNo, int linePos ) {
            this.lineNo = lineNo;
            this.linePos = linePos;
        }
    }
}
