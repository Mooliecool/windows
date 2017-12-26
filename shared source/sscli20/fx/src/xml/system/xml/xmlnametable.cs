//------------------------------------------------------------------------------
// <copyright file="XmlNameTable.cs" company="Microsoft">
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

    /// <include file='doc\XmlNameTable.uex' path='docs/doc[@for="XmlNameTable"]/*' />
    /// <devdoc>
    ///    <para> Table of atomized string objects. This provides an
    ///       efficient means for the XML parser to use the same string object for all
    ///       repeated element and attribute names in an XML document. This class is
    ///    <see langword='abstract'/>
    ///    .</para>
    /// </devdoc>
    public abstract class XmlNameTable {
        /// <include file='doc\XmlNameTable.uex' path='docs/doc[@for="XmlNameTable.Get"]/*' />
        /// <devdoc>
        ///    <para>Gets the atomized String object containing the same
        ///       chars as the specified range of chars in the given char array.</para>
        /// </devdoc>
        public abstract String Get(char[] array, int offset, int length);

        /// <include file='doc\XmlNameTable.uex' path='docs/doc[@for="XmlNameTable.Get1"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Gets the atomized String object containing the same
        ///       value as the specified string.
        ///    </para>
        /// </devdoc>
        public abstract String Get(String array);

        /// <include file='doc\XmlNameTable.uex' path='docs/doc[@for="XmlNameTable.Add"]/*' />
        /// <devdoc>
        ///    <para>Creates a new atom for the characters at the specified range
        ///       of chararacters in the specified string.</para>
        /// </devdoc>
        public abstract String Add(char[] array, int offset, int length);

        /// <include file='doc\XmlNameTable.uex' path='docs/doc[@for="XmlNameTable.Add1"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Creates a new atom for the specified string.
        ///    </para>
        /// </devdoc>
        public abstract String Add(String array);
    }
}
