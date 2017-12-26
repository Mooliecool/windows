//------------------------------------------------------------------------------
// <copyright file="DataObjectAttribute.cs" company="Microsoft">
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
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Method)]
    public sealed class DataObjectMethodAttribute : Attribute {

        private bool _isDefault;
        private DataObjectMethodType _methodType;

        public DataObjectMethodAttribute(DataObjectMethodType methodType) : this(methodType, false) {
        }

        public DataObjectMethodAttribute(DataObjectMethodType methodType, bool isDefault) {
            _methodType = methodType;
            _isDefault = isDefault;
        }

        public bool IsDefault {
            get {
                return _isDefault;
            }
        }

        public DataObjectMethodType MethodType {
            get {
                return _methodType;
            }
        }

        /// <internalonly/>
        public override bool Equals(object obj) {
            if (obj == this) {
                return true;
            }

            DataObjectMethodAttribute other = obj as DataObjectMethodAttribute;
            return (other != null) && (other.MethodType == MethodType) && (other.IsDefault == IsDefault);
        }

        /// <internalonly/>
        public override int GetHashCode() {
            return ((int)_methodType).GetHashCode() ^ _isDefault.GetHashCode();
        }

        /// <internalonly/>
        public override bool Match(object obj) {
            if (obj == this) {
                return true;
            }

            DataObjectMethodAttribute other = obj as DataObjectMethodAttribute;
            return (other != null) && (other.MethodType == MethodType);
        }
    }
}
