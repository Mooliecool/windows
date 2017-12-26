//------------------------------------------------------------------------------
// <copyright file="ListSortDescription.cs" company="Microsoft">
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
    using System.Security.Permissions;
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [HostProtection(SharedState = true)]
    public class ListSortDescription {
        PropertyDescriptor property;
        ListSortDirection sortDirection;
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public ListSortDescription(PropertyDescriptor property, ListSortDirection direction) {
            this.property = property;
            this.sortDirection = direction;
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public PropertyDescriptor PropertyDescriptor {
            get {
                return this.property;
            }
            set {
                this.property = value;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public ListSortDirection SortDirection {
            get {
                return this.sortDirection;
            }
            set {
                this.sortDirection = value;
            }
        }
    }
}
