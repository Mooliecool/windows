//------------------------------------------------------------------------------
// <copyright file="IBindingList.cs" company="Microsoft">
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
[assembly: System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Naming", "CA1710:IdentifiersShouldHaveCorrectSuffix", Scope="type", Target="System.ComponentModel.IBindingList")]

namespace System.ComponentModel {
    using System.Collections;

    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public interface IBindingList : IList {
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        bool AllowNew { get;}
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        object AddNew();
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>

        bool AllowEdit { get; }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        bool AllowRemove { get; }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>

        bool SupportsChangeNotification { get; }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>

        bool SupportsSearching { get; }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>

        bool SupportsSorting { get; }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>

        bool IsSorted { get; }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        PropertyDescriptor SortProperty { get; }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        ListSortDirection SortDirection { get; }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>

        event ListChangedEventHandler ListChanged;
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>

        void AddIndex(PropertyDescriptor property);
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        void ApplySort(PropertyDescriptor property, ListSortDirection direction);
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        int Find(PropertyDescriptor property, object key);
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        void RemoveIndex(PropertyDescriptor property);
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        void RemoveSort();
    }
}

