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
    [AttributeUsage(AttributeTargets.Class)]
    public sealed class DataObjectAttribute : Attribute {

        public static readonly DataObjectAttribute DataObject = new DataObjectAttribute(true);

        public static readonly DataObjectAttribute NonDataObject = new DataObjectAttribute(false);

        public static readonly DataObjectAttribute Default = NonDataObject;

        private bool _isDataObject;

        public DataObjectAttribute() : this(true) {
        }

        public DataObjectAttribute(bool isDataObject) {
            _isDataObject = isDataObject;
        }

        public bool IsDataObject {
            get {
                return _isDataObject;
            }
        }

        /// <internalonly/>
        public override bool Equals(object obj) {
            if (obj == this) {
                return true;
            }

            DataObjectAttribute other = obj as DataObjectAttribute;
            return (other != null) && (other.IsDataObject == IsDataObject);
        }

        /// <internalonly/>
        public override int GetHashCode() {
            return _isDataObject.GetHashCode();
        }

        /// <internalonly/>
        public override bool IsDefaultAttribute() {
            return (this.Equals(Default));
        }
    }
}
