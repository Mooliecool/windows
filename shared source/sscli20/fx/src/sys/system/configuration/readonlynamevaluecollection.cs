//------------------------------------------------------------------------------
// <copyright file="ReadOnlyNameValueCollection.cs" company="Microsoft">
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

namespace System.Configuration {
    using System.Collections;
    using System.Collections.Specialized;

    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    internal class ReadOnlyNameValueCollection : NameValueCollection {

        internal ReadOnlyNameValueCollection(IEqualityComparer equalityComparer) : base(equalityComparer) {
        }

        internal ReadOnlyNameValueCollection(ReadOnlyNameValueCollection value) : base(value) {
        }

        internal void SetReadOnly() {
            IsReadOnly = true;
        }
    }
}
