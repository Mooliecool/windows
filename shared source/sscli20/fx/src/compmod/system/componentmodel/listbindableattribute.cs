//------------------------------------------------------------------------------
// <copyright file="ListBindableAttribute.cs" company="Microsoft">
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

/*
 */
namespace System.ComponentModel {
    using System;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.Security.Permissions;

    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.All)]
    public sealed class ListBindableAttribute : Attribute {
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static readonly ListBindableAttribute Yes = new ListBindableAttribute(true);

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static readonly ListBindableAttribute No = new ListBindableAttribute(false);

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static readonly ListBindableAttribute Default = Yes;

        private bool listBindable   = false;
        private bool isDefault  = false;

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public ListBindableAttribute(bool listBindable) {
            this.listBindable = listBindable;
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public ListBindableAttribute(BindableSupport flags) {
            this.listBindable = (flags != BindableSupport.No);
            this.isDefault = (flags == BindableSupport.Default);
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool ListBindable {
            get {
                return listBindable;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public override bool Equals(object obj) {
            if (obj == this) {
                return true;
            }
            
            ListBindableAttribute other = obj as ListBindableAttribute;
            return other != null && other.ListBindable == listBindable;
        }
        
        /// <devdoc>
        ///    <para>
        ///       Returns the hashcode for this object.
        ///    </para>
        /// </devdoc>
        public override int GetHashCode() {
            return base.GetHashCode();
        }


        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public override bool IsDefaultAttribute() {
            return (this.Equals(Default) || isDefault);
        }
    }
}
