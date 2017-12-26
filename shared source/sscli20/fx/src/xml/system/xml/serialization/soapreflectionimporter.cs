//------------------------------------------------------------------------------
// <copyright file="SoapReflectionImporter.cs" company="Microsoft">
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
// <owner current="true" primary="true">ElenaK</owner>                                                                
//------------------------------------------------------------------------------

namespace System.Xml.Serialization {

    using System.Reflection;
    using System;
    using System.Globalization;
    using System.Xml.Schema;
    using System.Collections;
    using System.ComponentModel;
    using System.Threading;

    /// <include file='doc\SoapReflectionImporter.uex' path='docs/doc[@for="SoapReflectionImporter"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class SoapReflectionImporter {
        TypeScope typeScope;
        SoapAttributeOverrides attributeOverrides;
        NameTable types = new NameTable();      // xmltypename + xmlns -> Mapping
        StructMapping root;
        string defaultNs;
        ModelScope modelScope;

       
        /// <include file='doc\SoapReflectionImporter.uex' path='docs/doc[@for="SoapReflectionImporter.SoapReflectionImporter"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public SoapReflectionImporter() : this(null, null) {
        }

        /// <include file='doc\SoapReflectionImporter.uex' path='docs/doc[@for="SoapReflectionImporter.SoapReflectionImporter1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public SoapReflectionImporter(string defaultNamespace) : this(null, defaultNamespace) {
        }

        /// <include file='doc\SoapReflectionImporter.uex' path='docs/doc[@for="SoapReflectionImporter.SoapReflectionImporter2"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public SoapReflectionImporter(SoapAttributeOverrides attributeOverrides) : this(attributeOverrides, null) {
        }

        /// <include file='doc\SoapReflectionImporter.uex' path='docs/doc[@for="SoapReflectionImporter.SoapReflectionImporter3"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public SoapReflectionImporter(SoapAttributeOverrides attributeOverrides, string defaultNamespace) {
            if (defaultNamespace == null)
                defaultNamespace = String.Empty;
            if (attributeOverrides == null)
                attributeOverrides = new SoapAttributeOverrides();
            this.attributeOverrides = attributeOverrides;
            this.defaultNs = defaultNamespace;
            this.typeScope = new TypeScope();
            this.modelScope = new ModelScope(this.typeScope);
        }

        /// <include file='doc\SoapReflectionImporter.uex' path='docs/doc[@for="SoapReflectionImporter.IncludeTypes"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void IncludeTypes(ICustomAttributeProvider provider) {
            object[] attrs = provider.GetCustomAttributes(typeof(SoapIncludeAttribute), false);
            for (int i = 0; i < attrs.Length; i++) {
                IncludeType(((SoapIncludeAttribute)attrs[i]).Type);
            }
        }

        /// <include file='doc\SoapReflectionImporter.uex' path='docs/doc[@for="SoapReflectionImporter.IncludeType"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void IncludeType(Type type) {
            ImportTypeMapping(modelScope.GetTypeModel(type));
        }

        /// <include file='doc\SoapReflectionImporter.uex' path='docs/doc[@for="XmlReflectionImporter.ImportTypeMapping"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlTypeMapping ImportTypeMapping(Type type) {
            return ImportTypeMapping(type, null);
        }

        /// <include file='doc\SoapReflectionImporter.uex' path='docs/doc[@for="XmlReflectionImporter.ImportTypeMapping1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlTypeMapping ImportTypeMapping(Type type, string defaultNamespace) {
            ElementAccessor element = new ElementAccessor();
            element.IsSoap = true;
            element.Mapping = ImportTypeMapping(modelScope.GetTypeModel(type));
            element.Name = element.Mapping.DefaultElementName;
            element.Namespace = element.Mapping.Namespace == null ? defaultNamespace : element.Mapping.Namespace;
            element.Form = XmlSchemaForm.Qualified;
            XmlTypeMapping xmlMapping = new XmlTypeMapping(typeScope, element);
            xmlMapping.SetKeyInternal(XmlMapping.GenerateKey(type, null, defaultNamespace));
            xmlMapping.IsSoap = true;
            xmlMapping.GenerateSerializer = true;
            return xmlMapping;
        }

        /// <include file='doc\SoapReflectionImporter.uex' path='docs/doc[@for="SoapReflectionImporter.ImportMembersMapping"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlMembersMapping ImportMembersMapping(string elementName, string ns, XmlReflectionMember[] members) {
            return ImportMembersMapping(elementName, ns, members, true, true, false);
        }

        /// <include file='doc\SoapReflectionImporter.uex' path='docs/doc[@for="SoapReflectionImporter.ImportMembersMapping1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlMembersMapping ImportMembersMapping(string elementName, string ns, XmlReflectionMember[] members, bool hasWrapperElement, bool writeAccessors) {
            return ImportMembersMapping(elementName, ns, members, hasWrapperElement, writeAccessors, false);
        }

        /// <include file='doc\SoapReflectionImporter.uex' path='docs/doc[@for="SoapReflectionImporter.ImportMembersMapping2"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlMembersMapping ImportMembersMapping(string elementName, string ns, XmlReflectionMember[] members, bool hasWrapperElement, bool writeAccessors, bool validate) {
            return ImportMembersMapping(elementName, ns, members, hasWrapperElement, writeAccessors, validate, XmlMappingAccess.Read | XmlMappingAccess.Write);
        }
        /// <include file='doc\SoapReflectionImporter.uex' path='docs/doc[@for="SoapReflectionImporter.ImportMembersMapping3"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlMembersMapping ImportMembersMapping(string elementName, string ns, XmlReflectionMember[] members, bool hasWrapperElement, bool writeAccessors, bool validate, XmlMappingAccess access) {
            ElementAccessor element = new ElementAccessor();
            element.IsSoap = true;
            element.Name = elementName == null || elementName.Length == 0 ? elementName : XmlConvert.EncodeLocalName(elementName);

            element.Mapping = ImportMembersMapping(members, ns, hasWrapperElement, writeAccessors, validate);
            element.Mapping.TypeName = elementName;
            element.Namespace = element.Mapping.Namespace == null ? ns : element.Mapping.Namespace;
            element.Form = XmlSchemaForm.Qualified;
            XmlMembersMapping xmlMapping = new XmlMembersMapping(typeScope, element, access);
            xmlMapping.IsSoap = true;
            xmlMapping.GenerateSerializer = true;
            return xmlMapping;
        }

        Exception ReflectionException(string context, Exception e) {
            return new InvalidOperationException(Res.GetString(Res.XmlReflectionError, context), e);
        }

        SoapAttributes GetAttributes(Type type) {
            SoapAttributes attrs = attributeOverrides[type];
            if (attrs != null) return attrs;
            return new SoapAttributes(type);
        }

        SoapAttributes GetAttributes(MemberInfo memberInfo) {
            SoapAttributes attrs = attributeOverrides[memberInfo.DeclaringType, memberInfo.Name];
            if (attrs != null) return attrs;
            return new SoapAttributes(memberInfo);
        }

        TypeMapping ImportTypeMapping(TypeModel model) {
            return ImportTypeMapping(model, String.Empty);
        }

        TypeMapping ImportTypeMapping(TypeModel model, string dataType) {
            if (dataType.Length > 0) {
                if (!model.TypeDesc.IsPrimitive) {
                    throw new InvalidOperationException(Res.GetString(Res.XmlInvalidDataTypeUsage, dataType, "SoapElementAttribute.DataType"));
                }
                TypeDesc td = typeScope.GetTypeDesc(dataType, XmlSchema.Namespace);
                if (td == null) {
                    throw new InvalidOperationException(Res.GetString(Res.XmlInvalidXsdDataType, dataType, "SoapElementAttribute.DataType", new XmlQualifiedName(dataType, XmlSchema.Namespace).ToString()));
                }
                if (model.TypeDesc.FullName != td.FullName) {
                    throw new InvalidOperationException(Res.GetString(Res.XmlDataTypeMismatch, dataType, "SoapElementAttribute.DataType", model.TypeDesc.FullName));
                }
            }

            SoapAttributes a = GetAttributes(model.Type);
            
            if ((a.SoapFlags & ~SoapAttributeFlags.Type) != 0)
                throw new InvalidOperationException(Res.GetString(Res.XmlInvalidTypeAttributes, model.Type.FullName));

            switch (model.TypeDesc.Kind) {
                case TypeKind.Enum: 
                    return ImportEnumMapping((EnumModel)model);
                case TypeKind.Primitive:
                    return ImportPrimitiveMapping((PrimitiveModel)model, dataType);
                case TypeKind.Array:
                case TypeKind.Collection:
                case TypeKind.Enumerable:
                    return ImportArrayLikeMapping((ArrayModel)model);
                case TypeKind.Root:
                case TypeKind.Class:
                case TypeKind.Struct:
                    if (model.TypeDesc.IsOptionalValue) {
                        TypeDesc baseTypeDesc = model.TypeDesc.BaseTypeDesc;
                        SoapAttributes baseAttributes = GetAttributes(baseTypeDesc.Type);
                        string typeNs = defaultNs;
                        if (baseAttributes.SoapType != null && baseAttributes.SoapType.Namespace != null)
                            typeNs = baseAttributes.SoapType.Namespace;

                        TypeMapping baseMapping = GetTypeMapping(baseTypeDesc.Name, typeNs, baseTypeDesc);
                        if (baseMapping == null)
                            baseMapping = ImportTypeMapping(modelScope.GetTypeModel(baseTypeDesc.Type, true), dataType);
                        return CreateNullableMapping(baseMapping, model.TypeDesc);
                    }
                    else {
                        return ImportStructLikeMapping((StructModel)model);
                    }
                default:
                    throw new NotSupportedException(Res.GetString(Res.XmlUnsupportedSoapTypeKind, model.TypeDesc.FullName));
            }
        }

        StructMapping CreateRootMapping() {
            TypeDesc typeDesc = typeScope.GetTypeDesc(typeof(object));
            StructMapping mapping = new StructMapping();
            mapping.IsSoap = true;
            mapping.TypeDesc = typeDesc;
            mapping.Members = new MemberMapping[0];
            mapping.IncludeInSchema = false;
            mapping.TypeName = Soap.UrType;
            mapping.Namespace = XmlSchema.Namespace;
            return mapping;
        }
        
        StructMapping GetRootMapping() {
            if (root == null) {
                root = CreateRootMapping();
                typeScope.AddTypeMapping(root);
            }
            return root;
        }

        TypeMapping GetTypeMapping(string typeName, string ns, TypeDesc typeDesc) {
            TypeMapping mapping = (TypeMapping)types[typeName, ns];
            if (mapping == null) return null;
            if (mapping.TypeDesc != typeDesc) 
                throw new InvalidOperationException(Res.GetString(Res.XmlTypesDuplicate, typeDesc.FullName, mapping.TypeDesc.FullName, typeName, ns));
            return mapping;
        }

        NullableMapping CreateNullableMapping(TypeMapping baseMapping, TypeDesc typeDesc) {
            TypeMapping existingMapping = (TypeMapping)types[typeDesc.Name, baseMapping.Namespace];
            NullableMapping mapping;
            if (existingMapping != null) {
                if (existingMapping is NullableMapping) {
                    mapping = (NullableMapping)existingMapping;
                    if (mapping.BaseMapping is PrimitiveMapping && baseMapping is PrimitiveMapping)
                        return mapping;
                    else if (mapping.BaseMapping == baseMapping) {
                        return mapping;
                    }
                    else {
                        throw new InvalidOperationException(Res.GetString(Res.XmlTypesDuplicate, typeDesc.FullName, existingMapping.TypeDesc.FullName, typeDesc.Name, existingMapping.Namespace));
                    }
                }
                else if (!(baseMapping is PrimitiveMapping)){
                    throw new InvalidOperationException(Res.GetString(Res.XmlTypesDuplicate, typeDesc.FullName, existingMapping.TypeDesc.FullName, typeDesc.Name, existingMapping.Namespace));
                }
            }
            mapping = new NullableMapping();
            mapping.BaseMapping = baseMapping;
            mapping.TypeDesc = typeDesc;
            mapping.TypeName = baseMapping.TypeName;
            mapping.Namespace = baseMapping.Namespace;
            mapping.IncludeInSchema = false; //baseMapping.IncludeInSchema;
            types.Add(typeDesc.Name, baseMapping.Namespace, mapping);
            typeScope.AddTypeMapping(mapping);
            return mapping;
        }

        StructMapping ImportStructLikeMapping(StructModel model) {
            if (model.TypeDesc.Kind == TypeKind.Root) return GetRootMapping();

            SoapAttributes a = GetAttributes(model.Type);

            string typeNs = defaultNs;
            if (a.SoapType != null && a.SoapType.Namespace != null)
                typeNs = a.SoapType.Namespace;
            string typeName = XsdTypeName(model.Type, a, model.TypeDesc.Name);
            typeName = XmlConvert.EncodeLocalName(typeName);

            StructMapping mapping = (StructMapping)GetTypeMapping(typeName, typeNs, model.TypeDesc);
            if (mapping == null) {
                mapping = new StructMapping();
                mapping.IsSoap = true;
                mapping.TypeDesc = model.TypeDesc;
                mapping.Namespace = typeNs;
                mapping.TypeName = typeName;
                if (a.SoapType != null) mapping.IncludeInSchema = a.SoapType.IncludeInSchema;
                typeScope.AddTypeMapping(mapping);
                types.Add(typeName, typeNs, mapping);
                if (model.TypeDesc.BaseTypeDesc != null) {
                    mapping.BaseMapping = ImportStructLikeMapping((StructModel)modelScope.GetTypeModel(model.Type.BaseType, false));
                }
                ArrayList members = new ArrayList();
                foreach (MemberInfo memberInfo in model.GetMemberInfos()) {
                    if ((memberInfo.MemberType & (MemberTypes.Field | MemberTypes.Property)) == 0)
                        continue;
                    SoapAttributes memberAttrs = GetAttributes(memberInfo);
                    if (memberAttrs.SoapIgnore) continue;
                    FieldModel fieldModel = model.GetFieldModel(memberInfo);
                    if (fieldModel == null) continue;
                    MemberMapping member = ImportFieldMapping(fieldModel, memberAttrs, mapping.Namespace);
                    if (member == null) continue;

                    if (!member.TypeDesc.IsPrimitive && !member.TypeDesc.IsEnum && !member.TypeDesc.IsOptionalValue) {
                        if (model.TypeDesc.IsValueType)
                            throw new NotSupportedException(Res.GetString(Res.XmlRpcRefsInValueType, model.TypeDesc.FullName));
                        if (member.TypeDesc.IsValueType)
                            throw new NotSupportedException(Res.GetString(Res.XmlRpcNestedValueType, member.TypeDesc.FullName));
                    }
                    if (mapping.BaseMapping != null) {
                        if (mapping.BaseMapping.Declares(member, mapping.TypeName)) continue;
                    }
                    members.Add(member);
                }
                mapping.Members = (MemberMapping[])members.ToArray(typeof(MemberMapping));
                if (mapping.BaseMapping == null) mapping.BaseMapping = GetRootMapping();
                IncludeTypes(model.Type);
            }
            return mapping;
        }

        ArrayMapping ImportArrayLikeMapping(ArrayModel model) {

            ArrayMapping mapping = new ArrayMapping();
            mapping.IsSoap = true;
            TypeMapping itemTypeMapping = ImportTypeMapping(model.Element);

            if (itemTypeMapping.TypeDesc.IsValueType && !itemTypeMapping.TypeDesc.IsPrimitive && !itemTypeMapping.TypeDesc.IsEnum)
                throw new NotSupportedException(Res.GetString(Res.XmlRpcArrayOfValueTypes, model.TypeDesc.FullName));
            
            mapping.TypeDesc = model.TypeDesc;
            mapping.Elements = new ElementAccessor[] { 
                CreateElementAccessor(itemTypeMapping, mapping.Namespace) };
            SetArrayMappingType(mapping);

            // in the case of an ArrayMapping we can have more that one mapping correspond to a type
            // examples of that are ArrayList and object[] both will map tp ArrayOfur-type
            // so we create a link list for all mappings of the same XSD type
            ArrayMapping existingMapping = (ArrayMapping)types[mapping.TypeName, mapping.Namespace];
            if (existingMapping != null) {
                ArrayMapping first = existingMapping;
                while (existingMapping != null) {
                    if (existingMapping.TypeDesc == model.TypeDesc)
                        return existingMapping;
                    existingMapping = existingMapping.Next;
                }
                mapping.Next = first;
                types[mapping.TypeName, mapping.Namespace] = mapping;
                return mapping;
            }
            typeScope.AddTypeMapping(mapping);
            types.Add(mapping.TypeName, mapping.Namespace, mapping);
            IncludeTypes(model.Type);
            return mapping;
        }

        void SetArrayMappingType(ArrayMapping mapping) {
            bool useDefaultNs = false;

            string itemTypeName;
            string itemTypeNamespace;

            TypeMapping itemTypeMapping;
            if (mapping.Elements.Length == 1)
                itemTypeMapping = mapping.Elements[0].Mapping;
            else
                itemTypeMapping = null;

            if (itemTypeMapping is EnumMapping) {
                itemTypeNamespace = itemTypeMapping.Namespace;
                itemTypeName = itemTypeMapping.TypeName;
            }
            else if (itemTypeMapping is PrimitiveMapping) {
                itemTypeNamespace = itemTypeMapping.TypeDesc.IsXsdType ? XmlSchema.Namespace : UrtTypes.Namespace;
                itemTypeName = itemTypeMapping.TypeDesc.DataType.Name;
                useDefaultNs = true;
            }
            else if (itemTypeMapping is StructMapping) {
                if (itemTypeMapping.TypeDesc.IsRoot) {
                    itemTypeNamespace = XmlSchema.Namespace;
                    itemTypeName = Soap.UrType;
                    useDefaultNs = true;
                }
                else {
                    itemTypeNamespace = itemTypeMapping.Namespace;
                    itemTypeName = itemTypeMapping.TypeName;
                }
            }
            else if (itemTypeMapping is ArrayMapping) {
                itemTypeNamespace = itemTypeMapping.Namespace;
                itemTypeName = itemTypeMapping.TypeName;
            }
            else {
                throw new InvalidOperationException(Res.GetString(Res.XmlInvalidSoapArray, mapping.TypeDesc.FullName));
            }

            itemTypeName = CodeIdentifier.MakePascal(itemTypeName);
            string uniqueName = "ArrayOf" + itemTypeName;
            string ns = useDefaultNs ? defaultNs : itemTypeNamespace;
            int i = 1;
            TypeMapping existingMapping = (TypeMapping)types[uniqueName, ns];
            while (existingMapping != null) {
                if (existingMapping is ArrayMapping) {
                    ArrayMapping arrayMapping = (ArrayMapping)existingMapping;
                    if (AccessorMapping.ElementsMatch(arrayMapping.Elements, mapping.Elements)) {
                        break;
                    }
                }
                // need to re-name the mapping
                uniqueName = itemTypeName + i.ToString(CultureInfo.InvariantCulture);
                existingMapping = (TypeMapping)types[uniqueName, ns];
                i++;
            }
            mapping.Namespace = ns;
            mapping.TypeName = uniqueName;
        }

        PrimitiveMapping ImportPrimitiveMapping(PrimitiveModel model, string dataType) {
            PrimitiveMapping mapping = new PrimitiveMapping();
            mapping.IsSoap = true;
            if (dataType.Length > 0) {
                mapping.TypeDesc = typeScope.GetTypeDesc(dataType, XmlSchema.Namespace);
                if (mapping.TypeDesc == null) {
                    // try it as a non-Xsd type
                    mapping.TypeDesc = typeScope.GetTypeDesc(dataType, UrtTypes.Namespace);
                    if (mapping.TypeDesc == null) {
                        throw new InvalidOperationException(Res.GetString(Res.XmlUdeclaredXsdType, dataType));
                    }
                }
            }
            else {
                mapping.TypeDesc = model.TypeDesc;
            }
            mapping.TypeName = mapping.TypeDesc.DataType.Name;
            mapping.Namespace = mapping.TypeDesc.IsXsdType ? XmlSchema.Namespace : UrtTypes.Namespace;
            return mapping;
        }
       
        EnumMapping ImportEnumMapping(EnumModel model) {
            SoapAttributes a = GetAttributes(model.Type);
            string typeNs = defaultNs;
            if (a.SoapType != null && a.SoapType.Namespace != null)
                typeNs = a.SoapType.Namespace;
            string typeName = XsdTypeName(model.Type, a, model.TypeDesc.Name);
            typeName = XmlConvert.EncodeLocalName(typeName);

            EnumMapping mapping = (EnumMapping)GetTypeMapping(typeName, typeNs, model.TypeDesc);
            if (mapping == null) {
                mapping = new EnumMapping();
                mapping.IsSoap = true;
                mapping.TypeDesc = model.TypeDesc;
                mapping.TypeName = typeName;
                mapping.Namespace = typeNs;
                mapping.IsFlags =  model.Type.IsDefined(typeof(FlagsAttribute), false);
                typeScope.AddTypeMapping(mapping);
                types.Add(typeName, typeNs, mapping);
                ArrayList constants = new ArrayList();
                for (int i = 0; i < model.Constants.Length; i++) {
                    ConstantMapping constant = ImportConstantMapping(model.Constants[i]);
                    if (constant != null) constants.Add(constant);
                }
                if (constants.Count == 0) {
                    throw new InvalidOperationException(Res.GetString(Res.XmlNoSerializableMembers, model.TypeDesc.FullName));
                }
                mapping.Constants = (ConstantMapping[])constants.ToArray(typeof(ConstantMapping));
            }
            return mapping;
        }
        
        ConstantMapping ImportConstantMapping(ConstantModel model) {
            SoapAttributes a = GetAttributes(model.FieldInfo);
            if (a.SoapIgnore) return null;
            if ((a.SoapFlags & ~SoapAttributeFlags.Enum) != 0)
                throw new InvalidOperationException(Res.GetString(Res.XmlInvalidEnumAttribute));
            if (a.SoapEnum == null)
                a.SoapEnum = new SoapEnumAttribute();

            ConstantMapping constant = new ConstantMapping();
            constant.XmlName = a.SoapEnum.Name.Length == 0 ? model.Name : a.SoapEnum.Name;
            constant.Name = model.Name;
            constant.Value = model.Value;
            return constant;
        }
        
        MembersMapping ImportMembersMapping(XmlReflectionMember[] xmlReflectionMembers, string ns, bool hasWrapperElement, bool writeAccessors, bool validateWrapperElement) {
            MembersMapping members = new MembersMapping();
            members.TypeDesc = typeScope.GetTypeDesc(typeof(object[]));
            MemberMapping[] mappings = new MemberMapping[xmlReflectionMembers.Length];
            for (int i = 0; i < mappings.Length; i++) {
                try {
                    XmlReflectionMember member = xmlReflectionMembers[i];
                    MemberMapping mapping = ImportMemberMapping(member, ns, xmlReflectionMembers, hasWrapperElement ? XmlSchemaForm.Unqualified : XmlSchemaForm.Qualified);
                    if (member.IsReturnValue && writeAccessors) { // no special treatment for return values with doc/enc
                        if (i > 0) throw new InvalidOperationException(Res.GetString(Res.XmlInvalidReturnPosition));
                        mapping.IsReturnValue = true;
                    }
                    mappings[i] = mapping;
                }
                catch (Exception e) {
                    if (e is ThreadAbortException || e is StackOverflowException || e is OutOfMemoryException) {
                        throw;
                    }
                    throw ReflectionException(xmlReflectionMembers[i].MemberName, e);
                }
                catch {
                    throw ReflectionException(xmlReflectionMembers[i].MemberName, null);
                }
            }
            members.Members = mappings;
            members.HasWrapperElement = hasWrapperElement;
            if (hasWrapperElement) {
                members.ValidateRpcWrapperElement = validateWrapperElement;
            }
            members.WriteAccessors = writeAccessors;
            members.IsSoap = true;
            if (hasWrapperElement && !writeAccessors)
                members.Namespace = ns;
            return members;
        }
        
        MemberMapping ImportMemberMapping(XmlReflectionMember xmlReflectionMember, string ns, XmlReflectionMember[] xmlReflectionMembers, XmlSchemaForm form) {
            SoapAttributes a = xmlReflectionMember.SoapAttributes;
            if (a.SoapIgnore) return null;
            MemberMapping member = new MemberMapping();
            member.IsSoap = true;
            member.Name = xmlReflectionMember.MemberName;
            bool checkSpecified = XmlReflectionImporter.FindSpecifiedMember(xmlReflectionMember.MemberName, xmlReflectionMembers) != null;
            FieldModel model = new FieldModel(xmlReflectionMember.MemberName, xmlReflectionMember.MemberType, typeScope.GetTypeDesc(xmlReflectionMember.MemberType), checkSpecified, false);
            member.CheckShouldPersist = model.CheckShouldPersist;
            member.CheckSpecified = model.CheckSpecified;
            member.ReadOnly = model.ReadOnly; // || !model.FieldTypeDesc.HasDefaultConstructor;
            ImportAccessorMapping(member, model, a, ns, form);
            if (xmlReflectionMember.OverrideIsNullable)
                member.Elements[0].IsNullable = false;
            return member;
        }

        MemberMapping ImportFieldMapping(FieldModel model, SoapAttributes a, string ns) {
            if (a.SoapIgnore) return null;
            MemberMapping member = new MemberMapping();
            member.IsSoap = true;
            member.Name = model.Name;
            member.CheckShouldPersist = model.CheckShouldPersist;
            member.CheckSpecified = model.CheckSpecified;
            member.ReadOnly = model.ReadOnly; // || !model.FieldTypeDesc.HasDefaultConstructor;
            ImportAccessorMapping(member, model, a, ns, XmlSchemaForm.Unqualified);
            return member;
        }

        void ImportAccessorMapping(MemberMapping accessor, FieldModel model, SoapAttributes a, string ns, XmlSchemaForm form) {
            Type accessorType = model.FieldType;
            string accessorName = model.Name;
            accessor.TypeDesc = typeScope.GetTypeDesc(accessorType);
            if (accessor.TypeDesc.IsVoid) {
                throw new InvalidOperationException(Res.GetString(Res.XmlInvalidVoid));
            }

            SoapAttributeFlags flags = a.SoapFlags;
            if ((flags & SoapAttributeFlags.Attribute) == SoapAttributeFlags.Attribute) {
                if (!accessor.TypeDesc.IsPrimitive && !accessor.TypeDesc.IsEnum)
                    throw new InvalidOperationException(Res.GetString(Res.XmlIllegalSoapAttribute, accessorName, accessor.TypeDesc.FullName));

                if ((flags & SoapAttributeFlags.Attribute) != flags)
                    throw new InvalidOperationException(Res.GetString(Res.XmlInvalidElementAttribute));
                
                AttributeAccessor attribute = new AttributeAccessor();
                attribute.Name = Accessor.EscapeQName(a.SoapAttribute == null || a.SoapAttribute.AttributeName.Length == 0 ? accessorName : a.SoapAttribute.AttributeName);
                attribute.Namespace = a.SoapAttribute == null || a.SoapAttribute.Namespace == null ? ns : a.SoapAttribute.Namespace;
                attribute.Form = XmlSchemaForm.Qualified; // attributes are always qualified since they're only used for encoded soap headers
                attribute.Mapping = ImportTypeMapping(modelScope.GetTypeModel(accessorType), (a.SoapAttribute == null ? String.Empty : a.SoapAttribute.DataType));
                attribute.Default = GetDefaultValue(model.FieldTypeDesc, a);
                accessor.Attribute = attribute;
                accessor.Elements = new ElementAccessor[0];
            }
            else {
                if ((flags & SoapAttributeFlags.Element) != flags)
                    throw new InvalidOperationException(Res.GetString(Res.XmlInvalidElementAttribute));

                ElementAccessor element = new ElementAccessor();
                element.IsSoap = true;
                element.Name = XmlConvert.EncodeLocalName(a.SoapElement == null || a.SoapElement.ElementName.Length == 0 ? accessorName : a.SoapElement.ElementName);
                element.Namespace = ns;
                element.Form = form;
                element.Mapping = ImportTypeMapping(modelScope.GetTypeModel(accessorType), (a.SoapElement == null ? String.Empty : a.SoapElement.DataType));
                if (a.SoapElement != null)
                    element.IsNullable = a.SoapElement.IsNullable;
                accessor.Elements = new ElementAccessor[] { element };
            }
        }

        static ElementAccessor CreateElementAccessor(TypeMapping mapping, string ns) {
            ElementAccessor element = new ElementAccessor();
            element.IsSoap = true;
            element.Name = mapping.TypeName; //XmlConvert.EncodeLocalName(name == null || name.Length == 0 ? mapping.TypeName : name);
            element.Namespace = ns;
            element.Mapping = mapping;
            return element;
        }

        object GetDefaultValue(TypeDesc fieldTypeDesc, SoapAttributes a) {
            if (a.SoapDefaultValue == null || a.SoapDefaultValue == DBNull.Value) return null;
            if (!(fieldTypeDesc.Kind == TypeKind.Primitive || fieldTypeDesc.Kind == TypeKind.Enum))  {
                a.SoapDefaultValue = null;
                return a.SoapDefaultValue;
            }
            // for enums validate and return a string representation
            if (fieldTypeDesc.Kind == TypeKind.Enum) {
                if (fieldTypeDesc != typeScope.GetTypeDesc(a.SoapDefaultValue.GetType()))
                    throw new InvalidOperationException(Res.GetString(Res.XmlInvalidDefaultEnumValue, a.SoapDefaultValue.GetType().FullName, fieldTypeDesc.FullName));
                string strValue = Enum.Format(a.SoapDefaultValue.GetType(), a.SoapDefaultValue, "G").Replace(",", " ");
                string numValue = Enum.Format(a.SoapDefaultValue.GetType(), a.SoapDefaultValue, "D");
                if (strValue == numValue) // means enum value wasn't recognized
                    throw new InvalidOperationException(Res.GetString(Res.XmlInvalidDefaultValue, strValue, a.SoapDefaultValue.GetType().FullName));
                return strValue;
            }
            return a.SoapDefaultValue;
        }

        internal string XsdTypeName(Type type) {
            if (type == typeof(object)) return Soap.UrType;
            TypeDesc typeDesc = typeScope.GetTypeDesc(type);
            if (typeDesc.IsPrimitive && typeDesc.DataType != null && typeDesc.DataType.Name != null && typeDesc.DataType.Name.Length > 0)
                return typeDesc.DataType.Name;
            return XsdTypeName(type, GetAttributes(type), typeDesc.Name);
        }
        internal string XsdTypeName(Type type, SoapAttributes a, string name) {
            string typeName = name;
            if (a.SoapType != null && a.SoapType.TypeName.Length > 0)
                typeName = a.SoapType.TypeName;

            if (type.IsGenericType && typeName.IndexOf('{') >= 0) {
                Type genType = type.GetGenericTypeDefinition();
                Type[] names = genType.GetGenericArguments();
                Type[] types = type.GetGenericArguments();

                for (int i = 0; i < names.Length; i++) {
                    string argument = "{" + names[i] + "}";
                    if (typeName.Contains(argument)) {
                        typeName = typeName.Replace(argument, XsdTypeName(types[i]));
                        if (typeName.IndexOf('{') < 0) {
                            break;
                        }
                    }
                }
            }
            return typeName;
        }
    }
}
