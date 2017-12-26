// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==

using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections;
using System.Collections.Generic;
using System.Resources;
using System.Diagnostics;
using System.Globalization;
using System.Security;
using System.Security.Permissions;
using System.Runtime.ConstrainedExecution;

namespace System.Reflection 
{
    [Serializable()] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class CustomAttributeData
    {
        #region Public Static Members
        public static IList<CustomAttributeData> GetCustomAttributes(MemberInfo target)
        {
            if (target == null)
                throw new ArgumentNullException("target");

            IList<CustomAttributeData> cad = GetCustomAttributes(target.Module, target.MetadataToken);

            int pcaCount = 0;
            Attribute[] a = null;
            if (target is RuntimeType)
                a = PseudoCustomAttribute.GetCustomAttributes((RuntimeType)target, typeof(object), false, out pcaCount);
            else if (target is RuntimeMethodInfo)
                a = PseudoCustomAttribute.GetCustomAttributes((RuntimeMethodInfo)target, typeof(object), false, out pcaCount);
            else if (target is RuntimeFieldInfo)
                a = PseudoCustomAttribute.GetCustomAttributes((RuntimeFieldInfo)target, typeof(object), out pcaCount);

            if (pcaCount == 0)
                return cad;

            CustomAttributeData[] pca = new CustomAttributeData[cad.Count + pcaCount];
            cad.CopyTo(pca, pcaCount);
            for (int i = 0; i < pcaCount; i++)
            {
                if (PseudoCustomAttribute.IsSecurityAttribute(a[i].GetType()))
                    continue;

                pca[i] = new CustomAttributeData(a[i]);
            }

            return Array.AsReadOnly(pca);
        }

        public static IList<CustomAttributeData> GetCustomAttributes(Module target)
        {
            if (target == null)
                throw new ArgumentNullException("target");

            if (target.IsResourceInternal())
                return new List<CustomAttributeData>();

            return GetCustomAttributes(target, target.MetadataToken);
        }

        public static IList<CustomAttributeData> GetCustomAttributes(Assembly target)
        {
            if (target == null)
                throw new ArgumentNullException("target");

            return GetCustomAttributes(target.ManifestModule, target.AssemblyHandle.GetToken());
        }

        public static IList<CustomAttributeData> GetCustomAttributes(ParameterInfo target)
        {
            if (target == null)
                throw new ArgumentNullException("target");

            IList<CustomAttributeData> cad = GetCustomAttributes(target.Member.Module, target.MetadataToken);
            
            int pcaCount = 0;
            Attribute[] a = PseudoCustomAttribute.GetCustomAttributes((ParameterInfo)target, typeof(object), out pcaCount);

            if (pcaCount == 0)
                return cad;

            CustomAttributeData[] pca = new CustomAttributeData[cad.Count + pcaCount];
            cad.CopyTo(pca, pcaCount);
            for (int i = 0; i < pcaCount; i++)
                pca[i] = new CustomAttributeData(a[i]);

            return Array.AsReadOnly(pca);
        }
        #endregion

        #region Private Static Methods
        private static CustomAttributeEncoding TypeToCustomAttributeEncoding(Type type)
        {
            if (type == typeof(int))
                return CustomAttributeEncoding.Int32;

            if (type.IsEnum)
                return CustomAttributeEncoding.Enum;

            if (type == typeof(string))
                return CustomAttributeEncoding.String;

            if (type == typeof(Type))
                return CustomAttributeEncoding.Type;

            if (type == typeof(object))
                return CustomAttributeEncoding.Object;

            if (type.IsArray)
                return CustomAttributeEncoding.Array;

            if (type == typeof(char))
                return CustomAttributeEncoding.Char;

            if (type == typeof(bool))
                return CustomAttributeEncoding.Boolean;

            if (type == typeof(byte))
                return CustomAttributeEncoding.Byte;

            if (type == typeof(sbyte))
                return CustomAttributeEncoding.SByte;

            if (type == typeof(short))
                return CustomAttributeEncoding.Int16;

            if (type == typeof(ushort))
                return CustomAttributeEncoding.UInt16;

            if (type == typeof(uint))
                return CustomAttributeEncoding.UInt32;

            if (type == typeof(long))
                return CustomAttributeEncoding.Int64;

            if (type == typeof(ulong))
                return CustomAttributeEncoding.UInt64;

            if (type == typeof(float))
                return CustomAttributeEncoding.Float;

            if (type == typeof(double))
                return CustomAttributeEncoding.Double;

            if (type.IsClass)
                return CustomAttributeEncoding.Object;

            if (type.IsInterface)
                return CustomAttributeEncoding.Object;

            if (type.IsValueType)
                return CustomAttributeEncoding.Undefined;

            throw new ArgumentException(Environment.GetResourceString("Argument_InvalidKindOfTypeForCA"), "type");
        }
        private static CustomAttributeType InitCustomAttributeType(Type parameterType, Module scope)
        {
            CustomAttributeEncoding encodedType = CustomAttributeData.TypeToCustomAttributeEncoding(parameterType);
            CustomAttributeEncoding encodedArrayType = CustomAttributeEncoding.Undefined;
            CustomAttributeEncoding encodedEnumType = CustomAttributeEncoding.Undefined;
            string enumName = null;

            if (encodedType == CustomAttributeEncoding.Array)
            {
                parameterType = parameterType.GetElementType();
                encodedArrayType = CustomAttributeData.TypeToCustomAttributeEncoding(parameterType);
            }

            if (encodedType == CustomAttributeEncoding.Enum || encodedArrayType == CustomAttributeEncoding.Enum)
            {
                encodedEnumType = TypeToCustomAttributeEncoding(Enum.GetUnderlyingType(parameterType));

                if (parameterType.Module == scope)
                    enumName = parameterType.FullName;
                else
                    enumName = parameterType.AssemblyQualifiedName;
            }

            return new CustomAttributeType(encodedType, encodedArrayType, encodedEnumType, enumName);
        }
        private static IList<CustomAttributeData> GetCustomAttributes(Module module, int tkTarget)
        {
            CustomAttributeRecord[] records = GetCustomAttributeRecords(module, tkTarget);

            CustomAttributeData[] customAttributes = new CustomAttributeData[records.Length];
            for (int i = 0; i < records.Length; i++)
                customAttributes[i] = new CustomAttributeData(module, records[i]);

            return Array.AsReadOnly(customAttributes);
        } 
        #endregion 

        #region Internal Static Members
        internal unsafe static CustomAttributeRecord[] GetCustomAttributeRecords(Module module, int targetToken)
        {
            MetadataImport scope = module.MetadataImport;

            int cCustomAttributeTokens = scope.EnumCustomAttributesCount(targetToken);
            int* tkCustomAttributeTokens = stackalloc int[cCustomAttributeTokens];
            scope.EnumCustomAttributes(targetToken, tkCustomAttributeTokens, cCustomAttributeTokens);

            CustomAttributeRecord[] records = new CustomAttributeRecord[cCustomAttributeTokens];

            for (int i = 0; i < cCustomAttributeTokens; i++)
            {
                scope.GetCustomAttributeProps(
                    tkCustomAttributeTokens[i], out records[i].tkCtor.Value, out records[i].blob);
            }

            return records;
        }
        
        internal static CustomAttributeTypedArgument Filter(IList<CustomAttributeData> attrs, Type caType, string name)
        {
            for (int i = 0; i < attrs.Count; i++)
            {
                if (attrs[i].Constructor.DeclaringType == caType)
                {
                    IList<CustomAttributeNamedArgument> namedArguments = attrs[i].NamedArguments;
                    for (int j = 0; j < namedArguments.Count; j++)
                    {
                        if (namedArguments[j].MemberInfo.Name.Equals(name))
                            return namedArguments[j].TypedValue;
                    }
                }
            }

            return new CustomAttributeTypedArgument();
        }
        
        internal static CustomAttributeTypedArgument Filter(IList<CustomAttributeData> attrs, Type caType, int parameter)
        {
            for (int i = 0; i < attrs.Count; i++)
            {
                if (attrs[i].Constructor.DeclaringType == caType)
                {
                    return attrs[i].ConstructorArguments[parameter];
                }
            }

            return new CustomAttributeTypedArgument();
        }
        #endregion

        #region Private Data Members
        private ConstructorInfo m_ctor;
        private Module m_scope;
        private MemberInfo[] m_members;
        private CustomAttributeCtorParameter[] m_ctorParams;
        private CustomAttributeNamedParameter[] m_namedParams;
        private IList<CustomAttributeTypedArgument> m_typedCtorArgs;
        private IList<CustomAttributeNamedArgument> m_namedArgs;
        #endregion

        #region Constructor
        internal CustomAttributeData(Module scope, CustomAttributeRecord caRecord)
        {
            m_scope = scope;
            m_ctor = (ConstructorInfo)RuntimeType.GetMethodBase(scope, caRecord.tkCtor);

            ParameterInfo[] parameters = m_ctor.GetParametersNoCopy();            
            m_ctorParams = new CustomAttributeCtorParameter[parameters.Length];
            for (int i = 0; i < parameters.Length; i++)
                m_ctorParams[i] = new CustomAttributeCtorParameter(InitCustomAttributeType(parameters[i].ParameterType, scope));

            FieldInfo[] fields = m_ctor.DeclaringType.GetFields(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
            PropertyInfo[] properties = m_ctor.DeclaringType.GetProperties(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
            m_namedParams = new CustomAttributeNamedParameter[properties.Length + fields.Length];
            for (int i = 0; i < fields.Length; i++)
                m_namedParams[i] = new CustomAttributeNamedParameter(
                    fields[i].Name, CustomAttributeEncoding.Field, InitCustomAttributeType(fields[i].FieldType, scope));
            for (int i = 0; i < properties.Length; i++)
                m_namedParams[i + fields.Length] = new CustomAttributeNamedParameter(
                    properties[i].Name, CustomAttributeEncoding.Property, InitCustomAttributeType(properties[i].PropertyType, scope));

            m_members = new MemberInfo[fields.Length + properties.Length];
            fields.CopyTo(m_members, 0);
            properties.CopyTo(m_members, fields.Length);

            CustomAttributeEncodedArgument.ParseAttributeArguments(caRecord.blob, ref m_ctorParams, ref m_namedParams, m_scope);
        }
        #endregion

        #region Pseudo Custom Attribute Constructor
        internal CustomAttributeData(Attribute attribute)
        {
            if (attribute is DllImportAttribute)
                Init((DllImportAttribute)attribute);
            else if (attribute is FieldOffsetAttribute)
                Init((FieldOffsetAttribute)attribute);
            else if (attribute is MarshalAsAttribute)
                Init((MarshalAsAttribute)attribute);
            else
                Init(attribute);
        }
        private void Init(DllImportAttribute dllImport)
        {
            Type type = typeof(DllImportAttribute);
            m_ctor = type.GetConstructors(BindingFlags.Public | BindingFlags.Instance)[0];
            m_typedCtorArgs = Array.AsReadOnly(new CustomAttributeTypedArgument[]
            {
                new CustomAttributeTypedArgument(dllImport.Value),
            });

            m_namedArgs = Array.AsReadOnly(new CustomAttributeNamedArgument[]
            {
                new CustomAttributeNamedArgument(type.GetField("EntryPoint"), dllImport.EntryPoint),
                new CustomAttributeNamedArgument(type.GetField("CharSet"), dllImport.CharSet),
                new CustomAttributeNamedArgument(type.GetField("ExactSpelling"), dllImport.ExactSpelling),
                new CustomAttributeNamedArgument(type.GetField("SetLastError"), dllImport.SetLastError),
                new CustomAttributeNamedArgument(type.GetField("PreserveSig"), dllImport.PreserveSig),
                new CustomAttributeNamedArgument(type.GetField("CallingConvention"), dllImport.CallingConvention),
                new CustomAttributeNamedArgument(type.GetField("BestFitMapping"), dllImport.BestFitMapping),
                new CustomAttributeNamedArgument(type.GetField("ThrowOnUnmappableChar"), dllImport.ThrowOnUnmappableChar)

            });
        }
        private void Init(FieldOffsetAttribute fieldOffset)
        {
            m_ctor = typeof(FieldOffsetAttribute).GetConstructors(BindingFlags.Public | BindingFlags.Instance)[0];
            m_typedCtorArgs = Array.AsReadOnly(new CustomAttributeTypedArgument[] {
                new CustomAttributeTypedArgument(fieldOffset.Value)
            });
            m_namedArgs = Array.AsReadOnly(new CustomAttributeNamedArgument[0]);
        }
        private void Init(MarshalAsAttribute marshalAs)
        {
            Type type = typeof(MarshalAsAttribute);
            m_ctor = type.GetConstructors(BindingFlags.Public | BindingFlags.Instance)[0];
            m_typedCtorArgs = Array.AsReadOnly(new CustomAttributeTypedArgument[]
            {
                new CustomAttributeTypedArgument(marshalAs.Value),
            });

            int i = 3; // ArraySubType, SizeParamIndex, SizeConst
            if (marshalAs.MarshalType != null) i++;
            if (marshalAs.MarshalTypeRef != null) i++;
            if (marshalAs.MarshalCookie != null) i++;
            CustomAttributeNamedArgument[] namedArgs = new CustomAttributeNamedArgument[i];

            i = 0;
            namedArgs[i++] = new CustomAttributeNamedArgument(type.GetField("ArraySubType"), marshalAs.ArraySubType);
            namedArgs[i++] = new CustomAttributeNamedArgument(type.GetField("SizeParamIndex"), marshalAs.SizeParamIndex);
            namedArgs[i++] = new CustomAttributeNamedArgument(type.GetField("SizeConst"), marshalAs.SizeConst);
            if (marshalAs.MarshalType != null)
                namedArgs[i++] = new CustomAttributeNamedArgument(type.GetField("MarshalType"), marshalAs.MarshalType);
            if (marshalAs.MarshalTypeRef != null)
                namedArgs[i++] = new CustomAttributeNamedArgument(type.GetField("MarshalTypeRef"), marshalAs.MarshalTypeRef);
            if (marshalAs.MarshalCookie != null)
                namedArgs[i++] = new CustomAttributeNamedArgument(type.GetField("MarshalCookie"), marshalAs.MarshalCookie);

            m_namedArgs = Array.AsReadOnly(namedArgs);
        }
        private void Init(object pca)
        {
            m_ctor = pca.GetType().GetConstructors(BindingFlags.Public | BindingFlags.Instance)[0];
            m_typedCtorArgs = Array.AsReadOnly(new CustomAttributeTypedArgument[0]);
            m_namedArgs = Array.AsReadOnly(new CustomAttributeNamedArgument[0]);
        }
        #endregion

        #region Object Override
        public override string ToString()
        {
            string ctorArgs = "";
            for (int i = 0; i < ConstructorArguments.Count; i ++)
                ctorArgs += String.Format(CultureInfo.CurrentCulture, i == 0 ? "{0}" : ", {0}", ConstructorArguments[i]);

            string namedArgs = "";
            for (int i = 0; i < NamedArguments.Count; i ++)
                namedArgs += String.Format(CultureInfo.CurrentCulture, i == 0 && ctorArgs.Length == 0 ? "{0}" : ", {0}", NamedArguments[i]);

            return String.Format(CultureInfo.CurrentCulture, "[{0}({1}{2})]", Constructor.DeclaringType.FullName, ctorArgs, namedArgs);
        }
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }
        public override bool Equals(object obj)
        {
            return obj == (object)this;
        }
        #endregion

        #region Public Members
        [System.Runtime.InteropServices.ComVisible(true)]
        public ConstructorInfo Constructor { get { return m_ctor; } }

        [System.Runtime.InteropServices.ComVisible(true)]
        public IList<CustomAttributeTypedArgument> ConstructorArguments
        {
            get
            {
                if (m_typedCtorArgs == null)
                {
                    CustomAttributeTypedArgument[] typedCtorArgs = new CustomAttributeTypedArgument[m_ctorParams.Length];

                    for (int i = 0; i < typedCtorArgs.Length; i++)
                    {
                        CustomAttributeEncodedArgument encodedArg = m_ctorParams[i].CustomAttributeEncodedArgument;
                        
                        typedCtorArgs[i] = new CustomAttributeTypedArgument(m_scope, m_ctorParams[i].CustomAttributeEncodedArgument);
                    }

                    m_typedCtorArgs = Array.AsReadOnly(typedCtorArgs);
                }

                return m_typedCtorArgs;
            }
        }
        public IList<CustomAttributeNamedArgument> NamedArguments
        {
            get
            {
                if (m_namedArgs == null)
                {
                
                    if (m_namedParams == null)
                        return null;

                    int cNamedArgs = 0;
                    for (int i = 0; i < m_namedParams.Length; i ++)
                    {
                        if (m_namedParams[i].EncodedArgument.CustomAttributeType.EncodedType != CustomAttributeEncoding.Undefined)
                            cNamedArgs++;
                    }

                    CustomAttributeNamedArgument[] namedArgs = new CustomAttributeNamedArgument[cNamedArgs];

                    for (int i = 0, j = 0; i < m_namedParams.Length; i++)
                    {
                        if (m_namedParams[i].EncodedArgument.CustomAttributeType.EncodedType != CustomAttributeEncoding.Undefined)
                            namedArgs[j++] = new CustomAttributeNamedArgument(
                                m_members[i], new CustomAttributeTypedArgument(m_scope, m_namedParams[i].EncodedArgument));
                    }

                    m_namedArgs = Array.AsReadOnly(namedArgs);
                }

                return m_namedArgs;
            }
        }
        #endregion
    }

    [Serializable()]    
    [System.Runtime.InteropServices.ComVisible(true)]
    public struct CustomAttributeNamedArgument
    {
        #region Public Static Members
        public static bool operator ==(CustomAttributeNamedArgument left, CustomAttributeNamedArgument right)
        {
            return left.Equals(right);
        }
        public static bool operator !=(CustomAttributeNamedArgument left, CustomAttributeNamedArgument right)
        {
            return !left.Equals(right);
        }
        #endregion

        #region Private Data Members
        private MemberInfo m_memberInfo;
        private CustomAttributeTypedArgument m_value;
        #endregion

        #region Constructor
        internal CustomAttributeNamedArgument(MemberInfo memberInfo, object value)
        {
            m_memberInfo = memberInfo;
            m_value = new CustomAttributeTypedArgument(value);
        }
        internal CustomAttributeNamedArgument(MemberInfo memberInfo, CustomAttributeTypedArgument value)
        {
            m_memberInfo = memberInfo;
            m_value = value;
        }
        #endregion

        #region Object Override
        public override string ToString()
        {
            return String.Format(CultureInfo.CurrentCulture, "{0} = {1}", MemberInfo.Name, TypedValue.ToString(ArgumentType != typeof(object))); 
        }
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }
        public override bool Equals(object obj)
        {
            return obj == (object)this;
        }
        #endregion

        #region Internal Members
        internal Type ArgumentType 
        { 
            get 
            { 
                return m_memberInfo is FieldInfo ? 
                    ((FieldInfo)m_memberInfo).FieldType : 
                    ((PropertyInfo)m_memberInfo).PropertyType; 
            } 
        }
        #endregion

        #region Public Members
        public MemberInfo MemberInfo { get { return m_memberInfo; } }
        public CustomAttributeTypedArgument TypedValue { get { return m_value; } }
        #endregion
    }

    [Serializable()]    
    [ComVisible(true)]
    public struct CustomAttributeTypedArgument
    {
        #region Public Static Members
        public static bool operator ==(CustomAttributeTypedArgument left, CustomAttributeTypedArgument right)
        {
            return left.Equals(right);
        }
        public static bool operator !=(CustomAttributeTypedArgument left, CustomAttributeTypedArgument right)
        {
            return !left.Equals(right);
        }
        #endregion

        #region Private Static Methods
        private static Type CustomAttributeEncodingToType(CustomAttributeEncoding encodedType)
        {
            switch (encodedType)
            {
                case (CustomAttributeEncoding.Enum):
                    return typeof(Enum);

                case (CustomAttributeEncoding.Int32):
                    return typeof(int);

                case (CustomAttributeEncoding.String):
                    return typeof(string);

                case (CustomAttributeEncoding.Type):
                    return typeof(Type);

                case (CustomAttributeEncoding.Array):
                    return typeof(Array);

                case (CustomAttributeEncoding.Char):
                    return typeof(char);

                case (CustomAttributeEncoding.Boolean):
                    return typeof(bool);

                case (CustomAttributeEncoding.SByte):
                    return typeof(sbyte);

                case (CustomAttributeEncoding.Byte):
                    return typeof(byte);

                case (CustomAttributeEncoding.Int16):
                    return typeof(short);

                case (CustomAttributeEncoding.UInt16):
                    return typeof(ushort);

                case (CustomAttributeEncoding.UInt32):
                    return typeof(uint);

                case (CustomAttributeEncoding.Int64):
                    return typeof(long);

                case (CustomAttributeEncoding.UInt64):
                    return typeof(ulong);

                case (CustomAttributeEncoding.Float):
                    return typeof(float);

                case (CustomAttributeEncoding.Double):
                    return typeof(double);

                case (CustomAttributeEncoding.Object):
                    return typeof(object);

                default :
                    throw new ArgumentException(Environment.GetResourceString("Arg_EnumIllegalVal", (int)encodedType), "encodedType");
            }
        }
        private static object EncodedValueToRawValue(long val, CustomAttributeEncoding encodedType)
        {
            switch (encodedType)
            {
                case CustomAttributeEncoding.Boolean:
                    return (byte)val != 0;

                case CustomAttributeEncoding.Char:
                    return (char)val;

                case CustomAttributeEncoding.Byte:
                    return (byte)val;

                case CustomAttributeEncoding.SByte:
                    return (sbyte)val;

                case CustomAttributeEncoding.Int16:
                    return (short)val;

                case CustomAttributeEncoding.UInt16:
                    return (ushort)val;

                case CustomAttributeEncoding.Int32:
                    return (int)val;

                case CustomAttributeEncoding.UInt32:
                    return (uint)val;

                case CustomAttributeEncoding.Int64:
                    return (long)val;

                case CustomAttributeEncoding.UInt64:
                    return (ulong)val;

                case CustomAttributeEncoding.Float:
                    unsafe { return *(float*)&val; }

                case CustomAttributeEncoding.Double:
                    unsafe { return *(double*)&val; }

                default:
                    throw new ArgumentException(Environment.GetResourceString("Arg_EnumIllegalVal", (int)val), "val");
            }
        }
        private static Type ResolveType(Module scope, string typeName)
        {
            Type type = RuntimeTypeHandle.GetTypeByNameUsingCARules(typeName, scope);

            if (type == null)
                throw new InvalidOperationException(
                    String.Format(CultureInfo.CurrentUICulture, Environment.GetResourceString("Arg_CATypeResolutionFailed"), typeName));

            return type;
        }
        #endregion

        #region Private Data Members
        private object m_value;
        private Type m_argumentType;
        #endregion

        #region Constructor
        internal CustomAttributeTypedArgument(object value)
        {
            m_argumentType = value.GetType();
            if (m_argumentType.IsEnum)
            {
                if (Enum.GetUnderlyingType(m_argumentType) == typeof(int))
                    m_value = (int)value;
                else if (Enum.GetUnderlyingType(m_argumentType) == typeof(short))
                    m_value = (short)value;
                else
                    throw new ArgumentException(Environment.GetResourceString("Argument_EnumIsNotIntOrShort"), "value");
            }
            else
            {
                m_value = value;
            }
        }
        internal CustomAttributeTypedArgument(Module scope, CustomAttributeEncodedArgument encodedArg)
        {
            CustomAttributeEncoding encodedType = encodedArg.CustomAttributeType.EncodedType;

            if (encodedType == CustomAttributeEncoding.Undefined)
                throw new ArgumentException("encodedArg");

            else if (encodedType == CustomAttributeEncoding.Enum)
            {
                m_argumentType = ResolveType(scope, encodedArg.CustomAttributeType.EnumName);
                m_value = EncodedValueToRawValue(encodedArg.PrimitiveValue, encodedArg.CustomAttributeType.EncodedEnumType);
            }
            else if (encodedType == CustomAttributeEncoding.String)
            {
                m_argumentType = typeof(string);
                m_value = encodedArg.StringValue;
            }
            else if (encodedType == CustomAttributeEncoding.Type)
            {
                m_argumentType = typeof(Type);
                
                m_value = null;

                if (encodedArg.StringValue != null)
                    m_value = ResolveType(scope, encodedArg.StringValue);
            }
            else if (encodedType == CustomAttributeEncoding.Array)
            {                
                encodedType = encodedArg.CustomAttributeType.EncodedArrayType;
                Type elementType;
                
                if (encodedType == CustomAttributeEncoding.Enum)
                {
                    elementType = ResolveType(scope, encodedArg.CustomAttributeType.EnumName);
                }
                else
                {
                    elementType = CustomAttributeEncodingToType(encodedType);
                }

                m_argumentType = elementType.MakeArrayType();
        
                if (encodedArg.ArrayValue == null)
                {
                    m_value = null;                    
                }
                else
                {
                    CustomAttributeTypedArgument[] arrayValue = new CustomAttributeTypedArgument[encodedArg.ArrayValue.Length];
                    for (int i = 0; i < arrayValue.Length; i++)
                        arrayValue[i] = new CustomAttributeTypedArgument(scope, encodedArg.ArrayValue[i]);

                    m_value = Array.AsReadOnly(arrayValue);
                }
            }
            else
            {
                m_argumentType = CustomAttributeEncodingToType(encodedType);
                m_value = EncodedValueToRawValue(encodedArg.PrimitiveValue, encodedType);
            }
        }
        #endregion

        #region Object Overrides
        public override string ToString() { return ToString(false); }
        internal string ToString(bool typed)
        {
            if (ArgumentType.IsEnum)
                return String.Format(CultureInfo.CurrentCulture, typed ? "{0}" : "({1}){0}", Value, ArgumentType.FullName);

            else if (Value == null)
                return String.Format(CultureInfo.CurrentCulture, typed ? "null" : "({0})null", ArgumentType.Name);

            else if (ArgumentType == typeof(string))
                return String.Format(CultureInfo.CurrentCulture, "\"{0}\"", Value);

            else if (ArgumentType == typeof(char))
                return String.Format(CultureInfo.CurrentCulture, "'{0}'", Value);

            else if (ArgumentType == typeof(Type))
                return String.Format(CultureInfo.CurrentCulture, "typeof({0})", ((Type)Value).FullName);

            else if (ArgumentType.IsArray)
            {
                string result = null;
                IList<CustomAttributeTypedArgument> array = Value as IList<CustomAttributeTypedArgument>;

                Type elementType = ArgumentType.GetElementType();
                result = String.Format(CultureInfo.CurrentCulture, @"new {0}[{1}] {{ ", elementType.IsEnum ? elementType.FullName : elementType.Name, array.Count);

                for (int i = 0; i < array.Count; i++)
                    result += String.Format(CultureInfo.CurrentCulture, i == 0 ? "{0}" : ", {0}", array[i].ToString(elementType != typeof(object)));

                return result += " }";
            }

            return String.Format(CultureInfo.CurrentCulture, typed ? "{0}" : "({1}){0}", Value, ArgumentType.Name);
        }
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }
        public override bool Equals(object obj)
        {
            return obj == (object)this;
        }
        #endregion

        #region Public Members
        public Type ArgumentType
        {
            get
            {
                return m_argumentType;
            }
        }
        public object Value 
        { 
            get 
            {
                return m_value;
            } 
        }
        #endregion
    }

    [Serializable()]    
    internal struct CustomAttributeRecord
    {
        internal ConstArray blob;
        internal MetadataToken tkCtor;
    }

    [Serializable()]    
    internal enum CustomAttributeEncoding : int
    {
        Undefined = 0,
        Boolean = CorElementType.Boolean,
        Char = CorElementType.Char,
        SByte = CorElementType.I1,
        Byte = CorElementType.U1,
        Int16 = CorElementType.I2,
        UInt16 = CorElementType.U2,
        Int32 = CorElementType.I4,
        UInt32 = CorElementType.U4,
        Int64 = CorElementType.I8,
        UInt64 = CorElementType.U8,
        Float = CorElementType.R4,
        Double = CorElementType.R8,
        String = CorElementType.String,
        Array = CorElementType.SzArray,
        Type = 0x50,
        Object = 0x51,
        Field = 0x53,
        Property = 0x54,
        Enum = 0x55
    }

    [Serializable()]    
    [StructLayout(LayoutKind.Auto)]
    internal struct CustomAttributeEncodedArgument
    { 
        #region Parser
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void ParseAttributeArguments(
            IntPtr pCa, 
            int cCa, 
            ref CustomAttributeCtorParameter[] CustomAttributeCtorParameters,
            ref CustomAttributeNamedParameter[] CustomAttributeTypedArgument,
            IntPtr assembly);

        internal static void ParseAttributeArguments(ConstArray attributeBlob, 
            ref CustomAttributeCtorParameter[] customAttributeCtorParameters,
            ref CustomAttributeNamedParameter[] customAttributeNamedParameters,
            Module customAttributeModule)
        {
            if (customAttributeModule == null)
                throw new ArgumentNullException("customAttributeModule");

            if (customAttributeNamedParameters == null)
                customAttributeNamedParameters = new CustomAttributeNamedParameter[0];

            CustomAttributeCtorParameter[] _customAttributeCtorParameters = customAttributeCtorParameters;
            CustomAttributeNamedParameter[] _customAttributeNamedParameters = customAttributeNamedParameters;

            unsafe 
            {
                ParseAttributeArguments(
                    attributeBlob.Signature, 
                    (int)attributeBlob.Length, 
                    ref _customAttributeCtorParameters, 
                    ref _customAttributeNamedParameters,
                    (IntPtr)customAttributeModule.Assembly.AssemblyHandle.Value);
            }

            customAttributeCtorParameters = _customAttributeCtorParameters;
            customAttributeNamedParameters = _customAttributeNamedParameters;
        }
        #endregion

        #region Private Data Members
        private long m_primitiveValue;
        private CustomAttributeEncodedArgument[] m_arrayValue;
        private string m_stringValue;
        private CustomAttributeType m_type;
        #endregion

        #region Public Members
        public CustomAttributeType CustomAttributeType { get { return m_type; } }
        public long PrimitiveValue { get { return m_primitiveValue; } }
        public CustomAttributeEncodedArgument[] ArrayValue { get { return m_arrayValue; } }
        public string StringValue { get { return m_stringValue; } }
        #endregion
    }
    
    [Serializable()]    
    [StructLayout(LayoutKind.Auto)]
    internal struct CustomAttributeNamedParameter
    {
        #region Private Data Members
        private string m_argumentName;
        private CustomAttributeEncoding m_fieldOrProperty;
        private CustomAttributeEncoding m_padding;
        private CustomAttributeType m_type;
        private CustomAttributeEncodedArgument m_encodedArgument;
        #endregion

        #region Constructor
        public CustomAttributeNamedParameter(string argumentName, CustomAttributeEncoding fieldOrProperty, CustomAttributeType type)
        {
            if (argumentName == null)
                throw new ArgumentNullException("argumentName");

            m_argumentName = argumentName;
            m_fieldOrProperty = fieldOrProperty;
            m_padding = fieldOrProperty;
            m_type = type;
            m_encodedArgument = new CustomAttributeEncodedArgument();
        }
        #endregion

        #region Public Members
        public CustomAttributeEncodedArgument EncodedArgument { get { return m_encodedArgument; } }
        #endregion
    }
    
    [Serializable()]    
    [StructLayout(LayoutKind.Auto)]
    internal struct CustomAttributeCtorParameter
    {
        #region Private Data Members
        private CustomAttributeType m_type;
        private CustomAttributeEncodedArgument m_encodedArgument;
        #endregion

        #region Constructor
        public CustomAttributeCtorParameter(CustomAttributeType type)
        {
            m_type = type;
            m_encodedArgument = new CustomAttributeEncodedArgument();
        }
        #endregion

        #region Public Members
        public CustomAttributeEncodedArgument CustomAttributeEncodedArgument { get { return m_encodedArgument; } }
        #endregion
    }

    // Note: This is a managed representation of a frame type defined in vm\frames.h; please ensure the layout remains
    // synchronized.
    [StructLayout(LayoutKind.Sequential)]
    internal struct SecurityContextFrame
    {
        IntPtr m_GSCookie;      // This is actually at a negative offset in the real frame definition
        IntPtr __VFN_table;     // This is the real start of the SecurityContextFrame
        IntPtr m_Next;
        IntPtr m_Assembly;

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Push(Assembly assembly);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public extern void Pop();
    }

    [Serializable()]    
    [StructLayout(LayoutKind.Auto)]
    internal struct CustomAttributeType
    {
        #region Private Data Members
        /// The most complicated type is an enum[] in which case...
        private string m_enumName; // ...enum name
        private CustomAttributeEncoding m_encodedType; // ...array
        private CustomAttributeEncoding m_encodedEnumType; // ...enum
        private CustomAttributeEncoding m_encodedArrayType; // ...enum type
        private CustomAttributeEncoding m_padding; 
        #endregion

        #region Constructor
        public CustomAttributeType(CustomAttributeEncoding encodedType, CustomAttributeEncoding encodedArrayType, 
            CustomAttributeEncoding encodedEnumType, string enumName)
        {
            m_encodedType = encodedType;
            m_encodedArrayType = encodedArrayType;
            m_encodedEnumType = encodedEnumType;
            m_enumName = enumName;
            m_padding = m_encodedType; 
        }
        #endregion

        #region Public Members
        public CustomAttributeEncoding EncodedType { get { return m_encodedType; } }
        public CustomAttributeEncoding EncodedEnumType { get { return m_encodedEnumType; } }
        public CustomAttributeEncoding EncodedArrayType { get { return m_encodedArrayType; } }
        [System.Runtime.InteropServices.ComVisible(true)]
        public string EnumName { get { return m_enumName; } }
        #endregion
    }

    internal unsafe static class CustomAttribute
    {
        #region Internal Static Members
        internal static bool IsDefined(RuntimeType type, RuntimeType caType, bool inherit)
        {
            ASSERT.PRECONDITION(type != null);

            if (type.GetElementType() != null) 
                return false;

            if (PseudoCustomAttribute.IsDefined(type, caType))
                return true;

            if (IsCustomAttributeDefined(type.Module, type.MetadataToken, caType))
                return true;

            if (!inherit)
                return false;

            type = type.BaseType as RuntimeType;

            while (type != null)
            {
                if (IsCustomAttributeDefined(type.Module, type.MetadataToken, caType, inherit))
                    return true;

                type = type.BaseType as RuntimeType;
            }

            return false;
        }

        internal static bool IsDefined(RuntimeMethodInfo method, RuntimeType caType, bool inherit)
        {
            ASSERT.PRECONDITION(method != null);

            if (PseudoCustomAttribute.IsDefined(method, caType))
                return true;

            if (IsCustomAttributeDefined(method.Module, method.MetadataToken, caType))
                return true;

            if (!inherit)
                return false;

            method = method.GetParentDefinition() as RuntimeMethodInfo;

            while (method != null)
            {
                if (IsCustomAttributeDefined(method.Module, method.MetadataToken, caType, inherit))
                    return true;

                method = method.GetParentDefinition() as RuntimeMethodInfo;
            }

            return false;
        }

        internal static bool IsDefined(RuntimeConstructorInfo ctor, RuntimeType caType)
        {
            ASSERT.PRECONDITION(ctor != null);

            if (PseudoCustomAttribute.IsDefined(ctor, caType))
                return true;

            return IsCustomAttributeDefined(ctor.Module, ctor.MetadataToken, caType);
        }

        internal static bool IsDefined(RuntimePropertyInfo property, RuntimeType caType)
        {
            ASSERT.PRECONDITION(property != null);

            if (PseudoCustomAttribute.IsDefined(property, caType))
                return true;

            return IsCustomAttributeDefined(property.Module, property.MetadataToken, caType);
        }

        internal static bool IsDefined(RuntimeEventInfo e, RuntimeType caType)
        {
            ASSERT.PRECONDITION(e != null);

            if (PseudoCustomAttribute.IsDefined(e, caType))
                return true;

            return IsCustomAttributeDefined(e.Module, e.MetadataToken, caType);
        }

        internal static bool IsDefined(RuntimeFieldInfo field, RuntimeType caType)
        {
            ASSERT.PRECONDITION(field != null);

            if (PseudoCustomAttribute.IsDefined(field, caType))
                return true;

            return IsCustomAttributeDefined(field.Module, field.MetadataToken, caType);
        }

        internal static bool IsDefined(ParameterInfo parameter, RuntimeType caType)
        {
            if (PseudoCustomAttribute.IsDefined(parameter, caType))
                return true;

            return IsCustomAttributeDefined(parameter.Member.Module, parameter.MetadataToken, caType);
        }

        internal static bool IsDefined(Assembly assembly, RuntimeType caType) 
        {
            if (PseudoCustomAttribute.IsDefined(assembly, caType))
                return true;

            return IsCustomAttributeDefined(assembly.ManifestModule, assembly.AssemblyHandle.GetToken(), caType);
        }

        internal static bool IsDefined(Module module, RuntimeType caType) 
        {
            if (PseudoCustomAttribute.IsDefined(module, caType))
                return true;

            return IsCustomAttributeDefined(module, module.MetadataToken, caType);
        }

        internal static Object[] GetCustomAttributes(RuntimeType type, RuntimeType caType, bool inherit)
        {
            if (type.GetElementType() != null) 
                return (caType.IsValueType) ? new object[0] : (Object[])Array.CreateInstance(caType, 0);

            if (type.IsGenericType && !type.IsGenericTypeDefinition)
                type = type.GetGenericTypeDefinition() as RuntimeType;

            int pcaCount = 0;
            Attribute[] pca = PseudoCustomAttribute.GetCustomAttributes(type, caType, true, out pcaCount);

            // if we are asked to go up the hierarchy chain we have to do it now and regardless of the
            // attribute usage for the specific attribute because a derived attribute may override the usage...           
            // ... however if the attribute is sealed we can rely on the attribute usage
            if (!inherit || (caType.IsSealed && !CustomAttribute.GetAttributeUsage(caType).Inherited))
            {
                object[] attributes = GetCustomAttributes(type.Module, type.MetadataToken, pcaCount, caType);
                if (pcaCount > 0) Array.Copy(pca, 0, attributes, attributes.Length - pcaCount, pcaCount);
                return attributes;
            }

            List<object> result = new List<object>();
            bool mustBeInheritable = false;
            bool useObjectArray = (caType == null || caType.IsValueType || caType.ContainsGenericParameters);
            Type arrayType = useObjectArray ? typeof(object) : caType;

            while (pcaCount > 0)
                result.Add(pca[--pcaCount]);

            while (type != typeof(object) && type != null)
            {
                object[] attributes = GetCustomAttributes(type.Module, type.MetadataToken, 0, caType, mustBeInheritable, result);
                mustBeInheritable = true;
                for (int i = 0; i < attributes.Length; i++)
                    result.Add(attributes[i]);

                type = type.BaseType as RuntimeType;
            }

            object[] typedResult = Array.CreateInstance(arrayType, result.Count) as object[];
            Array.Copy(result.ToArray(), 0, typedResult, 0, result.Count);
            return typedResult;
        }

        internal static Object[] GetCustomAttributes(RuntimeMethodInfo method, RuntimeType caType, bool inherit)
        {
            if (method.IsGenericMethod && !method.IsGenericMethodDefinition)
                method = method.GetGenericMethodDefinition() as RuntimeMethodInfo;

            int pcaCount = 0;
            Attribute[] pca = PseudoCustomAttribute.GetCustomAttributes(method, caType, true, out pcaCount);

            // if we are asked to go up the hierarchy chain we have to do it now and regardless of the
            // attribute usage for the specific attribute because a derived attribute may override the usage...           
            // ... however if the attribute is sealed we can rely on the attribute usage
            if (!inherit || (caType.IsSealed && !CustomAttribute.GetAttributeUsage(caType).Inherited))
            {
                object[] attributes = GetCustomAttributes(method.Module, method.MetadataToken, pcaCount, caType);
                if (pcaCount > 0) Array.Copy(pca, 0, attributes, attributes.Length - pcaCount, pcaCount);
                return attributes;
            }

            List<object> result = new List<object>();
            bool mustBeInheritable = false;
            bool useObjectArray = (caType == null || caType.IsValueType || caType.ContainsGenericParameters);
            Type arrayType = useObjectArray ? typeof(object) : caType;

            while (pcaCount > 0) 
                result.Add(pca[--pcaCount]);
                
            while (method != null)
            {
                object[] attributes = GetCustomAttributes(method.Module, method.MetadataToken, 0, caType, mustBeInheritable, result);
                mustBeInheritable = true;
                for (int i = 0; i < attributes.Length; i++)
                    result.Add(attributes[i]);

                method = method.GetParentDefinition() as RuntimeMethodInfo;
            }

            object[] typedResult = Array.CreateInstance(arrayType, result.Count) as object[];
            Array.Copy(result.ToArray(), 0, typedResult, 0, result.Count);
            return typedResult;
        }

        internal static Object[] GetCustomAttributes(RuntimeConstructorInfo ctor, RuntimeType caType)
        {
            int pcaCount = 0;
            Attribute[] pca = PseudoCustomAttribute.GetCustomAttributes(ctor, caType, true, out pcaCount);
            object[] attributes = GetCustomAttributes(ctor.Module, ctor.MetadataToken, pcaCount, caType);
            if (pcaCount > 0) Array.Copy(pca, 0, attributes, attributes.Length - pcaCount, pcaCount);
            return attributes;
        }

        internal static Object[] GetCustomAttributes(RuntimePropertyInfo property, RuntimeType caType)
        {
            int pcaCount = 0;
            Attribute[] pca = PseudoCustomAttribute.GetCustomAttributes(property, caType, out pcaCount);
            object[] attributes = GetCustomAttributes(property.Module, property.MetadataToken, pcaCount, caType);
            if (pcaCount > 0) Array.Copy(pca, 0, attributes, attributes.Length - pcaCount, pcaCount);
            return attributes;
        }

        internal static Object[] GetCustomAttributes(RuntimeEventInfo e, RuntimeType caType)
        {
            int pcaCount = 0;
            Attribute[] pca = PseudoCustomAttribute.GetCustomAttributes(e, caType, out pcaCount);
            object[] attributes = GetCustomAttributes(e.Module, e.MetadataToken, pcaCount, caType);
            if (pcaCount > 0) Array.Copy(pca, 0, attributes, attributes.Length - pcaCount, pcaCount);
            return attributes;
        }

        internal static Object[] GetCustomAttributes(RuntimeFieldInfo field, RuntimeType caType)
        {
            int pcaCount = 0;
            Attribute[] pca = PseudoCustomAttribute.GetCustomAttributes(field, caType, out pcaCount);
            object[] attributes = GetCustomAttributes(field.Module, field.MetadataToken, pcaCount, caType);
            if (pcaCount > 0) Array.Copy(pca, 0, attributes, attributes.Length - pcaCount, pcaCount);
            return attributes;
        }

        internal static Object[] GetCustomAttributes(ParameterInfo parameter, RuntimeType caType)
        {
            int pcaCount = 0;
            Attribute[] pca = PseudoCustomAttribute.GetCustomAttributes(parameter, caType, out pcaCount);
            object[] attributes = GetCustomAttributes(parameter.Member.Module, parameter.MetadataToken, pcaCount, caType);
            if (pcaCount > 0) Array.Copy(pca, 0, attributes, attributes.Length - pcaCount, pcaCount);
            return attributes;
        }

        internal static Object[] GetCustomAttributes(Assembly assembly, RuntimeType caType) 
        {
            int pcaCount = 0;
            Attribute[] pca = PseudoCustomAttribute.GetCustomAttributes(assembly, caType, out pcaCount);
            object[] attributes = GetCustomAttributes(assembly.ManifestModule, assembly.AssemblyHandle.GetToken(), pcaCount, caType);
            if (pcaCount > 0) Array.Copy(pca, 0, attributes, attributes.Length - pcaCount, pcaCount);
            return attributes;
        }

        internal static Object[] GetCustomAttributes(Module module, RuntimeType caType) 
        {
            int pcaCount = 0;
            Attribute[] pca = PseudoCustomAttribute.GetCustomAttributes(module, caType, out pcaCount);
            object[] attributes = GetCustomAttributes(module, module.MetadataToken, pcaCount, caType);
            if (pcaCount > 0) Array.Copy(pca, 0, attributes, attributes.Length - pcaCount, pcaCount);
            return attributes;
        }       

        internal static bool IsCustomAttributeDefined(
            Module decoratedModule, int decoratedMetadataToken, RuntimeType attributeFilterType)
        {
            return IsCustomAttributeDefined(decoratedModule, decoratedMetadataToken, attributeFilterType, false);
        }

        internal static bool IsCustomAttributeDefined(
            Module decoratedModule, int decoratedMetadataToken, RuntimeType attributeFilterType, bool mustBeInheritable)
        {
            if (decoratedModule.Assembly.ReflectionOnly)
                throw new InvalidOperationException(Environment.GetResourceString("Arg_ReflectionOnlyCA"));

            MetadataImport scope = decoratedModule.MetadataImport;
            CustomAttributeRecord[] car = CustomAttributeData.GetCustomAttributeRecords(decoratedModule, decoratedMetadataToken);
            RuntimeType attributeType;
            RuntimeMethodHandle ctor;
            bool ctorHasParameters, isVarArg;

            // Optimization for the case where attributes decorate entities in the same assembly in which case 
            // we can cache the successful APTCA check between the decorated and the declared assembly.
            Assembly lastAptcaOkAssembly = null;
            
            for (int i = 0; i < car.Length; i++)
            {           
                CustomAttributeRecord caRecord = car[i];
            
                if (FilterCustomAttributeRecord(caRecord, scope, ref lastAptcaOkAssembly, 
                    decoratedModule, decoratedMetadataToken, attributeFilterType, mustBeInheritable, null, null,
                    out attributeType, out ctor, out ctorHasParameters, out isVarArg))
                    return true;           
            }

            return false;
        }

        internal unsafe static object[] GetCustomAttributes(
            Module decoratedModule, int decoratedMetadataToken, int pcaCount, RuntimeType attributeFilterType)
        {
            return GetCustomAttributes(decoratedModule, decoratedMetadataToken, pcaCount, attributeFilterType, false, null);
        }
        
        internal unsafe static object[] GetCustomAttributes(
            Module decoratedModule, int decoratedMetadataToken, int pcaCount, 
            RuntimeType attributeFilterType, bool mustBeInheritable, IList derivedAttributes)
        {
            if (decoratedModule.Assembly.ReflectionOnly)
                throw new InvalidOperationException(Environment.GetResourceString("Arg_ReflectionOnlyCA"));

            MetadataImport scope = decoratedModule.MetadataImport;
            CustomAttributeRecord[] car = CustomAttributeData.GetCustomAttributeRecords(decoratedModule, decoratedMetadataToken);

            bool useObjectArray = (attributeFilterType == null || attributeFilterType.IsValueType || attributeFilterType.ContainsGenericParameters);
            Type arrayType = useObjectArray ? typeof(object) : attributeFilterType;

            if (attributeFilterType == null && car.Length == 0)
                return Array.CreateInstance(arrayType, 0) as object[];

            object[] attributes = Array.CreateInstance(arrayType, car.Length) as object[];
            int cAttributes = 0;

            // Custom attribute security checks are done with respect to the assembly *decorated* with the 
            // custom attribute as opposed to the *caller of GetCustomAttributes*.
            // Since this assembly might not be on the stack and the attribute ctor or property setters we're about to invoke may
            // make security demands, we push a frame on the stack as a proxy for the decorated assembly (this frame will be picked
            // up an interpreted by the security stackwalker).
            // Once we push the frame it will be automatically popped in the event of an exception, so no need to use CERs or the
            // like.
            SecurityContextFrame frame = new SecurityContextFrame();
            frame.Push(decoratedModule.Assembly);

            // Optimization for the case where attributes decorate entities in the same assembly in which case 
            // we can cache the successful APTCA check between the decorated and the declared assembly.
            Assembly lastAptcaOkAssembly = null;
            
            for (int i = 0; i < car.Length; i++)
            {
                object attribute = null;
                CustomAttributeRecord caRecord = car[i];

                RuntimeMethodHandle ctor = new RuntimeMethodHandle();
                RuntimeType attributeType = null;
                bool ctorHasParameters, isVarArg;
                int cNamedArgs = 0;
                
                IntPtr blobStart = caRecord.blob.Signature;
                IntPtr blobEnd = (IntPtr)((byte*)blobStart + caRecord.blob.Length);
                
                if (!FilterCustomAttributeRecord(caRecord, scope, ref lastAptcaOkAssembly, 
                                                 decoratedModule, decoratedMetadataToken, attributeFilterType, mustBeInheritable, 
                                                 attributes, derivedAttributes,
                                                 out attributeType, out ctor, out ctorHasParameters, out isVarArg))
                    continue;

                if (!ctor.IsNullHandle())
                {
                    // Linktime demand checks 
                    // decoratedMetadataToken needed as it may be "transparent" in which case we do a full stack walk
                    ctor.CheckLinktimeDemands(decoratedModule, decoratedMetadataToken);
                }
                else
                {
                }

                // Leverage RuntimeConstructorInfo standard .ctor verfication
                RuntimeConstructorInfo.CheckCanCreateInstance(attributeType, isVarArg); 

                // Create custom attribute object
                if (ctorHasParameters)
                {
                    attribute = CreateCaObject(decoratedModule, ctor, ref blobStart, blobEnd, out cNamedArgs); 
                }
                else
                {
                    attribute = attributeType.TypeHandle.CreateCaInstance(ctor);

                    if (Marshal.ReadInt16(blobStart) != 1)
                        throw new CustomAttributeFormatException();
                    blobStart = (IntPtr)((byte*)blobStart + 2); // skip 0x0001 prefix

                    cNamedArgs = Marshal.ReadInt16(blobStart);
                    blobStart = (IntPtr)((byte*)blobStart + 2); // skip namedArgs count
                }

                for (int j = 0; j < cNamedArgs; j++)
                {
                    #region // Initialize named properties and fields
                    string name;
                    bool isProperty;
                    Type type;
                    object value;
                    
                    IntPtr blobItr = caRecord.blob.Signature;

                    GetPropertyOrFieldData(decoratedModule, ref blobStart, blobEnd, out name, out isProperty, out type, out value);

                    try
                    {
                        if (isProperty)
                        {
                            #region // Initialize property
                            if (type == null && value != null)
                                type = (value.GetType() == typeof(RuntimeType)) ? typeof(Type) : value.GetType();

                            RuntimePropertyInfo property = null;

                            if (type == null)
                                property = attributeType.GetProperty(name) as RuntimePropertyInfo;
                            else
                                property = attributeType.GetProperty(name, type, Type.EmptyTypes) as RuntimePropertyInfo;

                            RuntimeMethodInfo setMethod = property.GetSetMethod(true) as RuntimeMethodInfo;
                            
                            // Public properties may have non-public setter methods
                            if (!setMethod.IsPublic)
                                continue;

                            setMethod.MethodHandle.CheckLinktimeDemands(decoratedModule, decoratedMetadataToken);

                            setMethod.Invoke(attribute, BindingFlags.Default, null, new object[] { value }, null, true);
                            #endregion
                        }
                        else
                        {
                            RtFieldInfo field = attributeType.GetField(name) as RtFieldInfo;

                            field.InternalSetValue(attribute, value, BindingFlags.Default, Type.DefaultBinder, null, false);
                        }
                    }
                    catch (Exception e)
                    {
                        throw new CustomAttributeFormatException(
                            String.Format(CultureInfo.CurrentUICulture, Environment.GetResourceString(
                                isProperty ? "RFLCT.InvalidPropFail" : "RFLCT.InvalidFieldFail"), name), e);
                    }
                    #endregion
                }

                if (!blobStart.Equals(blobEnd))
                    throw new CustomAttributeFormatException();

                attributes[cAttributes++] = attribute;
            }

            // The frame will be popped automatically if we take an exception any time after we pushed it. So no need of a catch or
            // finally or CERs here.
            frame.Pop();

            if (cAttributes == car.Length && pcaCount == 0)
                return attributes;

            if (cAttributes == 0)
                Array.CreateInstance(arrayType, 0);

            object[] result = Array.CreateInstance(arrayType, cAttributes + pcaCount) as object[];
            Array.Copy(attributes, 0, result, 0, cAttributes);
            return result;
        }

        internal unsafe static bool FilterCustomAttributeRecord(
            CustomAttributeRecord caRecord, MetadataImport scope, ref Assembly lastAptcaOkAssembly, 
            Module decoratedModule, MetadataToken decoratedToken, RuntimeType attributeFilterType, bool mustBeInheritable,
            object[] attributes, IList derivedAttributes,
            out RuntimeType attributeType, out RuntimeMethodHandle ctor, out bool ctorHasParameters, out bool isVarArg)
        {
            ctor = new RuntimeMethodHandle();
            attributeType = null;
            ctorHasParameters = false;
            isVarArg = false;
            
            IntPtr blobStart = caRecord.blob.Signature;
            IntPtr blobEnd = (IntPtr)((byte*)blobStart + caRecord.blob.Length);

            // Resolve attribute type from ctor parent token found in decorated decoratedModule scope
            attributeType = decoratedModule.ResolveType(scope.GetParentToken(caRecord.tkCtor), null, null) as RuntimeType;

            // Test attribute type against user provided attribute type filter
            if (!(attributeFilterType.IsAssignableFrom(attributeType)))
                return false;

            if (!AttributeUsageCheck(attributeType, mustBeInheritable, attributes, derivedAttributes))
                return false;

            // APTCA checks
            if (attributeType.Assembly != lastAptcaOkAssembly && 
                !attributeType.Assembly.AptcaCheck(decoratedModule.Assembly))
                return false;

            // Cache last successful APTCA check (optimization)
            lastAptcaOkAssembly = decoratedModule.Assembly;

            // Resolve the attribute ctor
            ConstArray ctorSig = scope.GetMethodSignature(caRecord.tkCtor);
            isVarArg = (ctorSig[0] & 0x05) != 0;
            ctorHasParameters = ctorSig[1] != 0;

            if (ctorHasParameters)
            {
                // Resolve method ctor token found in decorated decoratedModule scope
                ctor = decoratedModule.ModuleHandle.ResolveMethodHandle(caRecord.tkCtor);
            }
            else
            {
                // Resolve method ctor token from decorated decoratedModule scope
                ctor = attributeType.GetTypeHandleInternal().GetDefaultConstructor();

                if (ctor.IsNullHandle() && !attributeType.IsValueType)
                    throw new MissingMethodException(".ctor");
            }

            // Visibility checks
            if (ctor.IsNullHandle())
            {
                if (!attributeType.IsVisible && !attributeType.TypeHandle.IsVisibleFromModule(decoratedModule.ModuleHandle))
                    return false;            
                
                return true;
            }
            
            if (ctor.IsVisibleFromModule(decoratedModule))
                return true;
            
            MetadataToken tkParent = new MetadataToken();
                
            if (decoratedToken.IsParamDef)
            {
                tkParent = new MetadataToken(scope.GetParentToken(decoratedToken));
                tkParent = new MetadataToken(scope.GetParentToken(tkParent));
            }               
            else if (decoratedToken.IsMethodDef || decoratedToken.IsProperty || decoratedToken.IsEvent || decoratedToken.IsFieldDef) 
            {
                tkParent = new MetadataToken(scope.GetParentToken(decoratedToken));
            }
            else if (decoratedToken.IsTypeDef)
            {
                tkParent = decoratedToken;
            }
            
            if (tkParent.IsTypeDef)
                return ctor.IsVisibleFromType(decoratedModule.ModuleHandle.ResolveTypeHandle(tkParent));
            
            return false;
        }
        #endregion

        #region Private Static Methods
        private static bool AttributeUsageCheck(
            RuntimeType attributeType, bool mustBeInheritable, object[] attributes, IList derivedAttributes)
        {
            AttributeUsageAttribute attributeUsageAttribute = null;

            if (mustBeInheritable)
            {
                attributeUsageAttribute = CustomAttribute.GetAttributeUsage(attributeType);

                if (!attributeUsageAttribute.Inherited)
                    return false;
            }

            // Legacy: AllowMultiple ignored for none inheritable attributes

            if (derivedAttributes == null)
                return true;

            for (int i = 0; i < derivedAttributes.Count; i++)
            {
                if (derivedAttributes[i].GetType() == attributeType)
                {
                    if (attributeUsageAttribute == null)
                        attributeUsageAttribute = CustomAttribute.GetAttributeUsage(attributeType);

                    return attributeUsageAttribute.AllowMultiple;
                }
            }

            return true;
        }

        internal static AttributeUsageAttribute GetAttributeUsage(RuntimeType decoratedAttribute)
        {
            Module decoratedModule = decoratedAttribute.Module;
            MetadataImport scope = decoratedModule.MetadataImport;
            CustomAttributeRecord[] car = CustomAttributeData.GetCustomAttributeRecords(decoratedModule, decoratedAttribute.MetadataToken);

            AttributeUsageAttribute attributeUsageAttribute = null;

            for (int i = 0; i < car.Length; i++)
            {
                CustomAttributeRecord caRecord = car[i];
                RuntimeType attributeType = decoratedModule.ResolveType(scope.GetParentToken(caRecord.tkCtor), null, null) as RuntimeType;

                if (attributeType != typeof(AttributeUsageAttribute))
                    continue;

                if (attributeUsageAttribute != null)
                    throw new FormatException(String.Format(
                        CultureInfo.CurrentUICulture, Environment.GetResourceString("Format_AttributeUsage"), attributeType));

                AttributeTargets targets;
                bool inherited, allowMultiple;
                ParseAttributeUsageAttribute(caRecord.blob, out targets, out inherited, out allowMultiple);
                attributeUsageAttribute = new AttributeUsageAttribute(targets, allowMultiple, inherited);
            }

            if (attributeUsageAttribute == null)
                return AttributeUsageAttribute.Default;

            return attributeUsageAttribute;
        }
        #endregion

        #region Private Static FCalls
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _ParseAttributeUsageAttribute(
            IntPtr pCa, int cCa, out int targets, out bool inherited, out bool allowMultiple);
        private static void ParseAttributeUsageAttribute(
            ConstArray ca, out AttributeTargets targets, out bool inherited, out bool allowMultiple)
        {
            int _targets;
            _ParseAttributeUsageAttribute(ca.Signature, ca.Length, out _targets, out inherited, out allowMultiple);
            targets = (AttributeTargets)_targets;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static unsafe extern Object _CreateCaObject(void* pModule, void* pCtor, byte** ppBlob, byte* pEndBlob, int* pcNamedArgs);
        private static unsafe Object CreateCaObject(Module module, RuntimeMethodHandle ctor, ref IntPtr blob, IntPtr blobEnd, out int namedArgs)
        {
            byte* pBlob = (byte*)blob;
            byte* pBlobEnd = (byte*)blobEnd;
            int cNamedArgs; 
            object ca = _CreateCaObject((void*)module.ModuleHandle.Value, (void*)ctor.Value, &pBlob, pBlobEnd, &cNamedArgs);
            blob = (IntPtr)pBlob;
            namedArgs = cNamedArgs;
            return ca;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private unsafe extern static void _GetPropertyOrFieldData(
            IntPtr pModule, byte** ppBlobStart, byte* pBlobEnd, out string name, out bool bIsProperty, out Type type, out object value);
        private unsafe static void GetPropertyOrFieldData(
            Module module, ref IntPtr blobStart, IntPtr blobEnd, out string name, out bool isProperty, out Type type, out object value)
        {
            byte* pBlobStart = (byte*)blobStart;
            _GetPropertyOrFieldData(
                (IntPtr)module.ModuleHandle.Value, &pBlobStart, (byte*)blobEnd, out name, out isProperty, out type, out value);
            blobStart = (IntPtr)pBlobStart;
        }
        #endregion
    }  
 
    internal static class PseudoCustomAttribute
    {
        #region Private Static Data Members
        private static Hashtable s_pca;
        private static int s_pcasCount;
        #endregion

        #region FCalls
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        unsafe private static extern void _GetSecurityAttributes(void* module, int token, out object[] securityAttributes);
        unsafe internal static void GetSecurityAttributes(ModuleHandle module, int token, out object[] securityAttributes)
        {
            _GetSecurityAttributes(module.Value, token, out securityAttributes);
        }
        #endregion

        #region Static Constructor
        static PseudoCustomAttribute()
        {
            Type[] pcas = new Type[]  
            {
                // See //depot/DevDiv/private/Main/ndp/clr/src/MD/Compiler/CustAttr.cpp
                typeof(FieldOffsetAttribute), // field
                typeof(SerializableAttribute), // class, struct, enum, delegate
                typeof(MarshalAsAttribute), // parameter, field, return-value
                typeof(ComImportAttribute), // class, interface 
                typeof(NonSerializedAttribute), // field, inherited
                typeof(InAttribute), // parameter
                typeof(OutAttribute), // parameter
                typeof(OptionalAttribute), // parameter
                typeof(DllImportAttribute), // method
                typeof(PreserveSigAttribute), // method
            };

            s_pcasCount = pcas.Length;
            s_pca = new Hashtable(s_pcasCount);
            for (int i = 0; i < s_pcasCount; i ++)
            {
                VerifyPseudoCustomAttribute(pcas[i]);
                s_pca[pcas[i]] = pcas[i];
            }
        }      
  
        [Conditional("_DEBUG")]
        private static void VerifyPseudoCustomAttribute(Type pca)
        {
            // If any of these are invariants are no longer true will have to 
            // re-architect the PCA product logic and test cases -- you've been warned!
            BCLDebug.Assert(pca.BaseType == typeof(Attribute), "Pseudo CA Error");
            AttributeUsageAttribute usage = CustomAttribute.GetAttributeUsage(pca as RuntimeType);   
            BCLDebug.Assert(usage.Inherited == false, "Pseudo CA Error");
            BCLDebug.Assert(usage.AllowMultiple == false, "Pseudo CA Error");
        }
        #endregion

        #region Internal Static
        internal static bool IsSecurityAttribute(Type type)
        {
            return type == typeof(SecurityAttribute) || type.IsSubclassOf(typeof(SecurityAttribute));
        }

        internal static Attribute[] GetCustomAttributes(RuntimeType type, Type caType, bool includeSecCa, out int count) 
        {
            ASSERT.PRECONDITION(type != null);
            ASSERT.PRECONDITION(caType != null);

            count = 0;

            bool all = caType == typeof(object) || caType == typeof(Attribute);
            if (!all && s_pca[caType]  == null && !IsSecurityAttribute(caType))
                return new Attribute[0];

            List<Attribute> pcas = new List<Attribute>();
            Attribute pca = null;

            if (all || caType == typeof(SerializableAttribute))
            {               
                pca = SerializableAttribute.GetCustomAttribute(type);
                if (pca != null) pcas.Add(pca);
            }
            if (all || caType == typeof(ComImportAttribute))
            {               
                pca = ComImportAttribute.GetCustomAttribute(type);
                if (pca != null) pcas.Add(pca);
            }
            if (includeSecCa && (all || IsSecurityAttribute(caType)))
            {               
                if (!type.IsGenericParameter)
                {
                    if (type.IsGenericType)
                        type = (RuntimeType)type.GetGenericTypeDefinition();
                    
                    object[] securityAttributes;
                    GetSecurityAttributes(type.Module.ModuleHandle, type.MetadataToken, out securityAttributes);
                    if (securityAttributes != null)
                        foreach(object a in securityAttributes)
                            if (caType == a.GetType() || a.GetType().IsSubclassOf(caType))
                                pcas.Add((Attribute)a);
                }
            }

            count = pcas.Count;
            return pcas.ToArray();
        }
        internal static bool IsDefined(RuntimeType type, Type caType) 
        {
            bool all = caType == typeof(object) || caType == typeof(Attribute);
            if (!all && s_pca[caType] == null && !IsSecurityAttribute(caType))
                return false;

            if (all || caType == typeof(SerializableAttribute)) 
            { 
                if (SerializableAttribute.IsDefined(type)) return true;
            }
            if (all || caType == typeof(ComImportAttribute)) 
            { 
                if (ComImportAttribute.IsDefined(type)) return true;
            }
            if (all || IsSecurityAttribute(caType))
            {
                int count;
                if (GetCustomAttributes(type, caType, true, out count).Length != 0)
                    return true;
            }

            return false;
        }

        internal static Attribute[] GetCustomAttributes(RuntimeMethodInfo method, Type caType, bool includeSecCa, out int count) 
        {
            ASSERT.PRECONDITION(method != null);
            ASSERT.PRECONDITION(caType != null);

            count = 0;

            bool all = caType == typeof(object) || caType == typeof(Attribute);
            if (!all && s_pca[caType] == null && !IsSecurityAttribute(caType))
                return new Attribute[0];

            List<Attribute> pcas = new List<Attribute>();
            Attribute pca = null;

            if (all || caType == typeof(DllImportAttribute))
            {               
                pca = DllImportAttribute.GetCustomAttribute(method);
                if (pca != null) pcas.Add(pca);
            }
            if (all || caType == typeof(PreserveSigAttribute))
            {               
                pca = PreserveSigAttribute.GetCustomAttribute(method);
                if (pca != null) pcas.Add(pca);
            }
            if (includeSecCa && (all || IsSecurityAttribute(caType)))
            {
                object[] securityAttributes;

                GetSecurityAttributes(method.Module.ModuleHandle, method.MetadataToken, out securityAttributes);
                if (securityAttributes != null)
                    foreach (object a in securityAttributes)
                        if (caType == a.GetType() || a.GetType().IsSubclassOf(caType))
                            pcas.Add((Attribute)a);
            }

            count = pcas.Count;
            return pcas.ToArray();
        }
        internal static bool IsDefined(RuntimeMethodInfo method, Type caType) 
        {
            bool all = caType == typeof(object) || caType == typeof(Attribute);
            if (!all && s_pca[caType] == null)
                return false;

            if (all || caType == typeof(DllImportAttribute)) 
            { 
                if (DllImportAttribute.IsDefined(method)) return true;
            }
            if (all || caType == typeof(PreserveSigAttribute)) 
            { 
                if (PreserveSigAttribute.IsDefined(method)) return true;
            }
            if (all || IsSecurityAttribute(caType))
            {
                int count;

                if (GetCustomAttributes(method, caType, true, out count).Length != 0)
                    return true;
            }

            return false;
        }

        internal static Attribute[] GetCustomAttributes(ParameterInfo parameter, Type caType, out int count) 
        {
            ASSERT.PRECONDITION(parameter != null);
            ASSERT.PRECONDITION(caType != null);

            count = 0;

            bool all = caType == typeof(object) || caType == typeof(Attribute);
            if (!all && s_pca[caType] == null)
                return null;

            Attribute[] pcas = new Attribute[s_pcasCount];
            Attribute pca = null;

            if (all || caType == typeof(InAttribute))
            {               
                pca = InAttribute.GetCustomAttribute(parameter);
                if (pca != null) pcas[count++] = pca;
            }
            if (all || caType == typeof(OutAttribute))
            {               
                pca = OutAttribute.GetCustomAttribute(parameter);
                if (pca != null) pcas[count++] = pca;
            }
            if (all || caType == typeof(OptionalAttribute))
            {               
                pca = OptionalAttribute.GetCustomAttribute(parameter);
                if (pca != null) pcas[count++] = pca;
            }
            if (all || caType == typeof(MarshalAsAttribute))
            {               
                pca = MarshalAsAttribute.GetCustomAttribute(parameter);
                if (pca != null) pcas[count++] = pca;
            }
            return pcas;
        }
        internal static bool IsDefined(ParameterInfo parameter, Type caType) 
        {
            bool all = caType == typeof(object) || caType == typeof(Attribute);
            if (!all && s_pca[caType] == null)
                return false;


            if (all || caType == typeof(InAttribute)) 
            { 
                if (InAttribute.IsDefined(parameter)) return true;
            }
            if (all || caType == typeof(OutAttribute)) 
            { 
                if (OutAttribute.IsDefined(parameter)) return true;
            }
            if (all || caType == typeof(OptionalAttribute)) 
            { 
                if (OptionalAttribute.IsDefined(parameter)) return true;
            }
            if (all || caType == typeof(MarshalAsAttribute)) 
            { 
                if (MarshalAsAttribute.IsDefined(parameter)) return true;
            }

            return false;
        }

        internal static Attribute[] GetCustomAttributes(Assembly assembly, Type caType, out int count)
        {
            count = 0;

            bool all = caType == typeof(object) || caType == typeof(Attribute);

            if (!all && s_pca[caType] == null && !IsSecurityAttribute(caType))
                return new Attribute[0];

            List<Attribute> pcas = new List<Attribute>();

            if (all || IsSecurityAttribute(caType))
            {
                object[] securityAttributes;

                GetSecurityAttributes(assembly.ManifestModule.ModuleHandle, assembly.AssemblyHandle.GetToken(), out securityAttributes);
                if (securityAttributes != null)
                    foreach (object a in securityAttributes)
                        if (caType == a.GetType() || a.GetType().IsSubclassOf(caType))
                            pcas.Add((Attribute)a);
            }

            count = pcas.Count;
            return pcas.ToArray();
        }
        internal static bool IsDefined(Assembly assembly, Type caType)
        {
            int count;
            return GetCustomAttributes(assembly, caType, out count).Length > 0;
        }

        internal static Attribute[] GetCustomAttributes(Module module, Type caType, out int count) 
        {
            count = 0;
            return null;
        }
        internal static bool IsDefined(Module module, Type caType) 
        {
            return false;
        }

        internal static Attribute[] GetCustomAttributes(RuntimeFieldInfo field, Type caType, out int count) 
        {
            ASSERT.PRECONDITION(field != null);
            ASSERT.PRECONDITION(caType != null);

            count = 0;

            bool all = caType == typeof(object) || caType == typeof(Attribute);
            if (!all && s_pca[caType] == null)
                return null;

            Attribute[] pcas = new Attribute[s_pcasCount];
            Attribute pca = null;

            if (all || caType == typeof(MarshalAsAttribute))
            {               
                pca = MarshalAsAttribute.GetCustomAttribute(field);
                if (pca != null) pcas[count++] = pca;
            }
            if (all || caType == typeof(FieldOffsetAttribute))
            {               
                pca = FieldOffsetAttribute.GetCustomAttribute(field);
                if (pca != null) pcas[count++] = pca;
            }
            if (all || caType == typeof(NonSerializedAttribute))
            {               
                pca = NonSerializedAttribute.GetCustomAttribute(field);
                if (pca != null) pcas[count++] = pca;
            }
            return pcas;
        }
        internal static bool IsDefined(RuntimeFieldInfo field, Type caType) 
        {
            bool all = caType == typeof(object) || caType == typeof(Attribute);
            if (!all && s_pca[caType] == null)
                return false;

            if (all || caType == typeof(MarshalAsAttribute)) 
            { 
                if (MarshalAsAttribute.IsDefined(field)) return true;
            }
            if (all || caType == typeof(FieldOffsetAttribute)) 
            { 
                if (FieldOffsetAttribute.IsDefined(field)) return true;
            }
            if (all || caType == typeof(NonSerializedAttribute)) 
            { 
                if (NonSerializedAttribute.IsDefined(field)) return true;
            }

            return false;
        }

        internal static Attribute[] GetCustomAttributes(RuntimeConstructorInfo ctor, Type caType, bool includeSecCa, out int count) 
        {
            count = 0;

            bool all = caType == typeof(object) || caType == typeof(Attribute);

            if (!all && s_pca[caType] == null && !IsSecurityAttribute(caType))
                return new Attribute[0];

            List<Attribute> pcas = new List<Attribute>();

            if (includeSecCa && (all || IsSecurityAttribute(caType)))
            {
                object[] securityAttributes;

                GetSecurityAttributes(ctor.Module.ModuleHandle, ctor.MetadataToken, out securityAttributes);
                if (securityAttributes != null)
                    foreach (object a in securityAttributes)
                        if (caType == a.GetType() || a.GetType().IsSubclassOf(caType))
                            pcas.Add((Attribute)a);
            }

            count = pcas.Count;
            return pcas.ToArray();
        }
        internal static bool IsDefined(RuntimeConstructorInfo ctor, Type caType) 
        {
            bool all = caType == typeof(object) || caType == typeof(Attribute);

            if (!all && s_pca[caType] == null)
                return false;

            if (all || IsSecurityAttribute(caType))
            {
                int count;

                if (GetCustomAttributes(ctor, caType, true, out count).Length != 0)
                    return true;
            }

            return false;
        }

        internal static Attribute[] GetCustomAttributes(RuntimePropertyInfo property, Type caType, out int count) 
        {
            count = 0;
            return null;
        }
        internal static bool IsDefined(RuntimePropertyInfo property, Type caType) 
        {
            return false;
        }

        internal static Attribute[] GetCustomAttributes(RuntimeEventInfo e, Type caType, out int count) 
        {
            count = 0;
            return null;
        }
        internal static bool IsDefined(RuntimeEventInfo e, Type caType) 
        {
            return false;
        }
        #endregion
    }
}
























