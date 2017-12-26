//------------------------------------------------------------------------------
// <copyright file="HttpVersion.cs" company="Microsoft">
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

namespace System.Net {

    /// <devdoc>
    ///    <para>
    ///       Defines the HTTP version number supported by the <see cref='System.Net.HttpWebRequest'/> and
    ///    <see cref='System.Net.HttpWebResponse'/> classes.
    ///    </para>
    /// </devdoc>
    public class HttpVersion {

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static readonly Version  Version10 = new Version(1,0);
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static readonly Version  Version11 = new Version(1,1);

    } // class HttpVersion

} // namespace System.Net
