//------------------------------------------------------------------------------
// <copyright file="CollectionsUtil.cs" company="Microsoft">
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

// Wrapper for a case insensitive Hashtable.

namespace System.Collections.Specialized {

    using System.Collections;

    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class CollectionsUtil {

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static Hashtable CreateCaseInsensitiveHashtable()  {
            return new Hashtable(StringComparer.CurrentCultureIgnoreCase);
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static Hashtable CreateCaseInsensitiveHashtable(int capacity)  {
            return new Hashtable(capacity, StringComparer.CurrentCultureIgnoreCase);
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static Hashtable CreateCaseInsensitiveHashtable(IDictionary d)  {
            return new Hashtable(d, StringComparer.CurrentCultureIgnoreCase);
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static SortedList CreateCaseInsensitiveSortedList() {
            return new SortedList(CaseInsensitiveComparer.Default);
        }

    }

}
