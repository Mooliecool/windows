//------------------------------------------------------------------------------
// <copyright file="SafeWriter.cs" company="Microsoft">
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

namespace System.Xml.Serialization {
    using System.Xml;

    /// <include file='doc\IXmlTextParser.uex' path='docs/doc[@for="IXmlTextParser"]/*' />
    ///<internalonly/>
    /// <devdoc>
    /// <para>This class is <see langword='interface'/> .</para>
    /// </devdoc>
    public interface IXmlTextParser {
        /// <include file='doc\IXmlTextParser.uex' path='docs/doc[@for="IXmlTextParser.Normalized"]/*' />
        /// <internalonly/>
        bool Normalized { get; set; }

        /// <include file='doc\IXmlTextParser.uex' path='docs/doc[@for="IXmlTextParser.WhitespaceHandling"]/*' />
        /// <internalonly/>
        WhitespaceHandling WhitespaceHandling { get; set; }
    }
}
