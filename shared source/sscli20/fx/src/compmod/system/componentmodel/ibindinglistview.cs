//------------------------------------------------------------------------------
// <copyright file="IBindingListView.cs" company="Microsoft">
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



namespace System.ComponentModel {
    using System.Collections;

    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Naming", "CA1710:IdentifiersShouldHaveCorrectSuffix")]
    public interface IBindingListView : IBindingList {
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        void ApplySort(ListSortDescriptionCollection sorts);
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        string Filter {get;set;}
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        ListSortDescriptionCollection SortDescriptions {get;}
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        void RemoveFilter();
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        bool SupportsAdvancedSorting{get;}
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        bool SupportsFiltering{get;}
    }

}

