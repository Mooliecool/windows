//------------------------------------------------------------------------------
// <copyright file="PasswordPropertyTextAttribute.cs" company="Microsoft">
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
    
    using System;
    using System.Security.Permissions;

    /// <devdoc>
    ///     If this attribute is placed on a property or a type, its text representation in a property window
    ///     will appear as dots or astrisks to indicate a password field.  This indidation in no way
    ///     represents any type of encryption or security.
    /// </devdoc>
    [AttributeUsage(AttributeTargets.All)]
    public sealed class PasswordPropertyTextAttribute : Attribute {

        /// <devdoc>
        ///     Sets the System.ComponentModel.Design.PasswordPropertyText
        ///     attribute by default to true.
        /// </devdoc>
        public static readonly PasswordPropertyTextAttribute Yes = new PasswordPropertyTextAttribute(true);

        /// <devdoc>
        ///     Sets the System.ComponentModel.Design.PasswordPropertyText
        ///     attribute by default to false.
        /// </devdoc>
        public static readonly PasswordPropertyTextAttribute No = new PasswordPropertyTextAttribute(false);


        /// <devdoc>
        ///     Sets the System.ComponentModel.Design.PasswordPropertyText
        ///     attribute by default to false.
        /// </devdoc>
        public static readonly PasswordPropertyTextAttribute Default = No;

        private bool _password;
        
        /// <devdoc>
        ///    Creates a default PasswordPropertyTextAttribute.
        /// </devdoc>
        public PasswordPropertyTextAttribute() : this(false) {
        }
        
        /// <devdoc>
        ///    Creates a PasswordPropertyTextAttribute with the given password value.
        /// </devdoc>
        public PasswordPropertyTextAttribute(bool password) {
            _password = password;
        }

        /// <devdoc>
        ///     Gets a value indicating if the property this attribute is defined for should be shown as password text.
        /// </devdoc>
        public bool Password {
            get {
                return _password;
            }
        }

        /// <devdoc>
        ///     Overload for object equality
        /// </devdoc>
        public override bool Equals(object o) {
            if (o is PasswordPropertyTextAttribute) {
                return ((PasswordPropertyTextAttribute)o).Password == _password;
            }
            return false;
        }
        
        /// <devdoc>
        ///     Returns the hashcode for this object.
        /// </devdoc>
        public override int GetHashCode() {
            return base.GetHashCode();
        }

        /// <devdoc>
        ///     Gets a value indicating whether this attribute is set to true by default.
        /// </devdoc>
        public override bool IsDefaultAttribute() {
            return this.Equals(Default);
        }
    }
}
