//------------------------------------------------------------------------------
// <copyright file="ComplexBindingPropertiesAttribute.cs" company="Microsoft">
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
    ///    <para>Specifies the data source and data member properties for a component.</para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Class)]
    public sealed class ComplexBindingPropertiesAttribute : Attribute {

        private readonly string dataSource;
        private readonly string dataMember;

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of
        ///       the <see cref='System.ComponentModel.ComplexBindingPropertiesAttribute'/> class.
        ///    </para>
        /// </devdoc>
        public ComplexBindingPropertiesAttribute() {
            this.dataSource = null;
            this.dataMember = null;
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of
        ///       the <see cref='System.ComponentModel.ComplexBindingPropertiesAttribute'/> class.
        ///    </para>
        /// </devdoc>
        public ComplexBindingPropertiesAttribute(string dataSource) {
            this.dataSource = dataSource;
            this.dataMember = null;
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of
        ///       the <see cref='System.ComponentModel.ComplexBindingPropertiesAttribute'/> class.
        ///    </para>
        /// </devdoc>
        public ComplexBindingPropertiesAttribute(string dataSource, string dataMember) {
            this.dataSource = dataSource;
            this.dataMember = dataMember;
        }

        /// <devdoc>
        ///    <para>
        ///       Gets the name of the data source property for the component this attribute is
        ///       bound to.
        ///    </para>
        /// </devdoc>
        public string DataSource {
            get {
                return dataSource;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets the name of the data member property for the component this attribute is
        ///       bound to.
        ///    </para>
        /// </devdoc>
        public string DataMember {
            get {
                return dataMember;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Specifies the default value for the <see cref='System.ComponentModel.ComplexBindingPropertiesAttribute'/>, which is <see langword='null'/>. This
        ///    <see langword='static '/>field is read-only. 
        ///    </para>
        /// </devdoc>
        public static readonly ComplexBindingPropertiesAttribute Default = new ComplexBindingPropertiesAttribute();

        public override bool Equals(object obj) {
            ComplexBindingPropertiesAttribute other = obj as ComplexBindingPropertiesAttribute; 
            return other != null &&
                   other.DataSource == dataSource &&
                   other.DataMember == dataMember;
        }

        public override int GetHashCode() {
            return base.GetHashCode();
        }
    }
}
