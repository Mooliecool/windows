//------------------------------------------------------------------------------
// <copyright file="SettingsBindableAttribute.cs" company="Microsoft">
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
    using System.Diagnostics;
    using System.Security.Permissions;

    /// <devdoc>
    ///    Use this attribute to specify typical properties on components that can be bound 
    ///    to application settings.
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Property)]
    public sealed class SettingsBindableAttribute : Attribute {
        /// <devdoc>
        ///       Specifies that a property is appropriate to bind settings to. 
        /// </devdoc>
        public static readonly SettingsBindableAttribute Yes = new SettingsBindableAttribute(true);

        /// <devdoc>
        ///       Specifies that a property is not appropriate to bind settings to. 
        /// </devdoc>
        public static readonly SettingsBindableAttribute No = new SettingsBindableAttribute(false);

        private bool _bindable   = false;
        
        public SettingsBindableAttribute(bool bindable) {
            _bindable = bindable;
        }
        
        /// <devdoc>
        ///     Gets a value indicating whether a property is appropriate to bind settings to.
        /// </devdoc>
        public bool Bindable {
            get {
                return _bindable;
            }
        }

        public override bool Equals(object obj) {
            if (obj == this) {
                return true;
            }

            if (obj != null && obj is SettingsBindableAttribute) {
                return (((SettingsBindableAttribute)obj).Bindable == _bindable);
            }

            return false;
        }

        public override int GetHashCode() {
            return _bindable.GetHashCode();
        }
    }
}
