//------------------------------------------------------------------------------
// <copyright file="EditorBrowsableAttribute.cs" company="Microsoft">
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

namespace System.ComponentModel
{
    using System.Security.Permissions;

    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Enum | AttributeTargets.Constructor | AttributeTargets.Method | AttributeTargets.Property | AttributeTargets.Field | AttributeTargets.Event | AttributeTargets.Delegate | AttributeTargets.Interface)]
    public sealed class EditorBrowsableAttribute :Attribute
    {
        private EditorBrowsableState browsableState;


        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public EditorBrowsableAttribute (EditorBrowsableState state) {
            browsableState = state;
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public EditorBrowsableAttribute () :this (EditorBrowsableState.Always) {}
        
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public EditorBrowsableState State {
            get { return browsableState;}
        }

        public override bool Equals(object obj) {
            if (obj == this) {
                return true;
            }

            EditorBrowsableAttribute other = obj as EditorBrowsableAttribute;

            return (other != null) && other.browsableState == browsableState;
        }

        public override int GetHashCode() {
            return base.GetHashCode();
        }
   }
 
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public enum EditorBrowsableState
    {
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        Always,
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        Never,
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        Advanced
    }
}
