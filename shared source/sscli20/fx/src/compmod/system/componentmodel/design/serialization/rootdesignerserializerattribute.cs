//------------------------------------------------------------------------------
// <copyright file="RootDesignerSerializerAttribute.cs" company="Microsoft">
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

namespace System.ComponentModel.Design.Serialization {
    using System.Security.Permissions;
    /// <devdoc>
    ///     This attribute can be placed on a class to indicate what serialization
    ///     object should be used to serialize the class at design time if it is
    ///     being used as a root object.
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Interface, AllowMultiple = true, Inherited = true)]
    [Obsolete("This attribute has been deprecated. Use DesignerSerializerAttribute instead.  For example, to specify a root designer for CodeDom, use DesignerSerializerAttribute(...,typeof(TypeCodeDomSerializer)).  http://go.microsoft.com/fwlink/?linkid=14202")]
    public sealed class RootDesignerSerializerAttribute : Attribute {
        private bool reloadable;
        private string serializerTypeName;
        private string serializerBaseTypeName;
        private string typeId;
    
        /// <devdoc>
        ///     Creates a new designer serialization attribute.
        /// </devdoc>
        public RootDesignerSerializerAttribute(Type serializerType, Type baseSerializerType, bool reloadable) {
            this.serializerTypeName = serializerType.AssemblyQualifiedName;
            this.serializerBaseTypeName = baseSerializerType.AssemblyQualifiedName;
            this.reloadable = reloadable;
        }
    
        /// <devdoc>
        ///     Creates a new designer serialization attribute.
        /// </devdoc>
        public RootDesignerSerializerAttribute(string serializerTypeName, Type baseSerializerType, bool reloadable) {
            this.serializerTypeName = serializerTypeName;
            this.serializerBaseTypeName = baseSerializerType.AssemblyQualifiedName;
            this.reloadable = reloadable;
        }
        
        /// <devdoc>
        ///     Creates a new designer serialization attribute.
        /// </devdoc>
        public RootDesignerSerializerAttribute(string serializerTypeName, string baseSerializerTypeName, bool reloadable) {
            this.serializerTypeName = serializerTypeName;
            this.serializerBaseTypeName = baseSerializerTypeName;
            this.reloadable = reloadable;
        }
        
        /// <devdoc>
        ///     Indicates that this root serializer supports reloading.  If false, the design document
        ///     will not automatically perform a reload on behalf of the user.  It will be the user's
        ///     responsibility to reload the document themselves.
        /// </devdoc>
        public bool Reloadable {
            get {
                return reloadable;
            }
        }
    
        /// <devdoc>
        ///     Retrieves the fully qualified type name of the serializer.
        /// </devdoc>
        public string SerializerTypeName {
            get {
                return serializerTypeName;
            }
        }
    
        /// <devdoc>
        ///     Retrieves the fully qualified type name of the serializer base type.
        /// </devdoc>
        public string SerializerBaseTypeName {
            get {
                return serializerBaseTypeName;
            }
        }
        
        /// <internalonly/>
        /// <devdoc>
        ///    <para>
        ///       This defines a unique ID for this attribute type. It is used
        ///       by filtering algorithms to identify two attributes that are
        ///       the same type. For most attributes, this just returns the
        ///       Type instance for the attribute. EditorAttribute overrides
        ///       this to include the type of the editor base type.
        ///    </para>
        /// </devdoc>
        public override object TypeId {
            get {
                if (typeId == null) {
                    string baseType = serializerBaseTypeName;
                    int comma = baseType.IndexOf(',');
                    if (comma != -1) {
                        baseType = baseType.Substring(0, comma);
                    }
                    typeId = GetType().FullName + baseType;
                }
                return typeId;
            }
        }
    }
}

