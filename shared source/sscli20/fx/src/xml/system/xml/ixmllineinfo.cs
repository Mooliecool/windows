//------------------------------------------------------------------------------
// <copyright file="IXmlLineInfo.cs" company="Microsoft">
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
    /// <include file='doc\IXmlLineInfo.uex' path='docs/doc[@for="IXmlLineInfo"]/*' />
    public interface IXmlLineInfo {
        /// <include file='doc\IXmlLineInfo.uex' path='docs/doc[@for="IXmlLineInfo.HasLineInfo"]/*' />
        bool HasLineInfo();
        /// <include file='doc\IXmlLineInfo.uex' path='docs/doc[@for="IXmlLineInfo.LineNumber"]/*' />
        int LineNumber { get; }
        /// <include file='doc\IXmlLineInfo.uex' path='docs/doc[@for="IXmlLineInfo.LinePosition"]/*' />
        int LinePosition { get; }
    }

    internal class PositionInfo : IXmlLineInfo {
        public virtual bool HasLineInfo() { return false; }
        public virtual int LineNumber { get { return 0;} }
        public virtual int LinePosition { get { return 0;} }

        public static PositionInfo GetPositionInfo(Object o) {
            IXmlLineInfo li = o as IXmlLineInfo;
            if (li != null) {
                return new ReaderPositionInfo(li);
            }
            else {
                return new PositionInfo();
            }
        }
    }

    internal class ReaderPositionInfo: PositionInfo {
        private IXmlLineInfo lineInfo;

        public ReaderPositionInfo(IXmlLineInfo lineInfo) {
            this.lineInfo = lineInfo;
        }

        public override bool HasLineInfo() { 
            return lineInfo.HasLineInfo(); 
        }

        public override int LineNumber { 
            get { 
                return lineInfo.LineNumber;
            } 
        }

        public override int LinePosition { 
            get { 
                return lineInfo.LinePosition;
            } 
        }

    }
}// namespace
