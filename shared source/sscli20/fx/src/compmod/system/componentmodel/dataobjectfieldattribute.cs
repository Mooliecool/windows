//------------------------------------------------------------------------------
// <copyright file="DataObjectFieldAttribute.cs" company="Microsoft">
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
    /// Represents a field of a DataObject. Use this attribute on a field to indicate
    /// properties such as primary key, identity, nullability, and length.
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Property)]
    public sealed class DataObjectFieldAttribute : Attribute {
        private bool _primaryKey;
        private bool _isIdentity;
        private bool _isNullable;
        private int _length;

        public DataObjectFieldAttribute(bool primaryKey) : this(primaryKey, false, false, -1) {
        }

        public DataObjectFieldAttribute(bool primaryKey, bool isIdentity) : this(primaryKey, isIdentity, false, -1) {
        }

        public DataObjectFieldAttribute(bool primaryKey, bool isIdentity, bool isNullable) : this(primaryKey, isIdentity, isNullable, -1){
        }

        public DataObjectFieldAttribute(bool primaryKey, bool isIdentity, bool isNullable, int length) {
            _primaryKey = primaryKey;
            _isIdentity = isIdentity;
            _isNullable = isNullable;
            _length = length;
        }

        public bool IsIdentity {
            get {
                return _isIdentity;
            }
        }

        public bool IsNullable {
            get {
                return _isNullable;
            }
        }

        public int Length {
            get {
                return _length;
            }
        }

        public bool PrimaryKey {
            get {
                return _primaryKey;
            }
        }

        public override bool Equals(object obj) {
            if (obj == this) {
                return true;
            }

            DataObjectFieldAttribute other = obj as DataObjectFieldAttribute;
            return (other != null) &&
                (other.IsIdentity == IsIdentity) &&
                (other.IsNullable == IsNullable) &&
                (other.Length == Length) &&
                (other.PrimaryKey == PrimaryKey);
        }

        public override int GetHashCode() {
            return base.GetHashCode();
        }
    }
}
