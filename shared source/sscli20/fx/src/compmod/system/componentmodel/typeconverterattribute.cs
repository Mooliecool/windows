//------------------------------------------------------------------------------
// <copyright file="TypeConverterAttribute.cs" company="Microsoft">
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
    using System.Diagnostics;    
    using System.Globalization;
    using System.Runtime.Serialization.Formatters;
    using System.Security.Permissions;

    /// <devdoc>
    ///    <para>Specifies what type to use as
    ///       a converter for the object
    ///       this
    ///       attribute is bound to. This class cannot
    ///       be inherited.</para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.All)]
    public sealed class TypeConverterAttribute : Attribute {
        private string typeName;

        /// <devdoc>
        ///    <para> Specifies the type to use as
        ///       a converter for the object this attribute is bound to. This
        ///    <see langword='static '/>field is read-only. </para>
        /// </devdoc>
        public static readonly TypeConverterAttribute Default = new TypeConverterAttribute();

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='System.ComponentModel.TypeConverterAttribute'/> class with the
        ///       default type converter, which
        ///       is an
        ///       empty string ("").
        ///    </para>
        /// </devdoc>
        public TypeConverterAttribute() {
            this.typeName = string.Empty;
        }
        
        /// <devdoc>
        /// <para>Initializes a new instance of the <see cref='System.ComponentModel.TypeConverterAttribute'/> class, using 
        ///    the specified type as the data converter for the object this attribute
        ///    is bound
        ///    to.</para>
        /// </devdoc>
        public TypeConverterAttribute(Type type) {
            this.typeName = type.AssemblyQualifiedName;
        }

        /// <devdoc>
        /// <para>Initializes a new instance of the <see cref='System.ComponentModel.TypeConverterAttribute'/> class, using 
        ///    the specified type name as the data converter for the object this attribute is bound to.</para>
        /// </devdoc>
        public TypeConverterAttribute(string typeName) {
            string temp = typeName.ToUpper(CultureInfo.InvariantCulture);
            Debug.Assert(temp.IndexOf(".DLL") == -1, "Came across: " + typeName + " . Please remove the .dll extension");
            this.typeName = typeName;
        }

        /// <devdoc>
        /// <para>Gets the fully qualified type name of the <see cref='System.Type'/>
        /// to use as a converter for the object this attribute
        /// is bound to.</para>
        /// </devdoc>
        public string ConverterTypeName {
            get {
                return typeName;
            }
        }

        public override bool Equals(object obj) {
            TypeConverterAttribute other = obj as TypeConverterAttribute; 
            return (other != null) && other.ConverterTypeName == typeName;
        }

        public override int GetHashCode() {
            return typeName.GetHashCode();
        }
    }
}

