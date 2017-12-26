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
using System.Reflection;
using System.Reflection.Emit;
using System.Collections;
using System.Globalization;

namespace System.Reflection.Emit
{
    internal sealed class MethodOnTypeBuilderInstantiation : MethodInfo
    {
        #region Private Static Members
        internal static MethodInfo GetMethod(MethodInfo method, TypeBuilderInstantiation type)
        {
            return new MethodOnTypeBuilderInstantiation(method, type);
        }
        #endregion

        #region Private Data Mebers
        internal MethodInfo m_method;
        private TypeBuilderInstantiation m_type;
        #endregion

        #region Constructor
        internal MethodOnTypeBuilderInstantiation(MethodInfo method, TypeBuilderInstantiation type)
        {
            m_method = method;
            m_type = type;
        }
        #endregion
        
        internal override Type[] GetParameterTypes()
        {
            return m_method.GetParameterTypes();
        }

        #region MemberInfo Overrides
        public override MemberTypes MemberType { get { return m_method.MemberType;  } }
        public override String Name { get { return m_method.Name; } }
        public override Type DeclaringType { get { return m_type; } }
        public override Type ReflectedType { get { return m_type; } }
        public override Object[] GetCustomAttributes(bool inherit) { return m_method.GetCustomAttributes(inherit); } 
        public override Object[] GetCustomAttributes(Type attributeType, bool inherit) { return m_method.GetCustomAttributes(attributeType, inherit); }
        public override bool IsDefined(Type attributeType, bool inherit) { return m_method.IsDefined(attributeType, inherit); }
        internal override int MetadataTokenInternal { get { return m_method.MetadataTokenInternal; } }
        public override Module Module { get { return m_method.Module; } }              
        public new Type GetType() { return base.GetType(); }
        #endregion

        #region MethodBase Members
        public override ParameterInfo[] GetParameters() { return m_method.GetParameters(); }
        public override MethodImplAttributes GetMethodImplementationFlags() { return m_method.GetMethodImplementationFlags(); }
        public override RuntimeMethodHandle MethodHandle { get { return m_method.MethodHandle; } }
        public override MethodAttributes Attributes { get { return m_method.Attributes; } }
        public override Object Invoke(Object obj, BindingFlags invokeAttr, Binder binder, Object[] parameters, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
        public override CallingConventions CallingConvention { get { return m_method.CallingConvention; } }
        public override Type [] GetGenericArguments() { return m_method.GetGenericArguments(); }
        public override MethodInfo GetGenericMethodDefinition() { return m_method; }
        public override bool IsGenericMethodDefinition { get { return m_method.IsGenericMethodDefinition; } }
        public override bool ContainsGenericParameters { get { return m_method.ContainsGenericParameters; } }
        public override MethodInfo MakeGenericMethod(params Type[] typeArgs)
        {
            if (!IsGenericMethodDefinition)
                throw new InvalidOperationException(Environment.GetResourceString("Arg_NotGenericMethodDefinition"));

            return MethodBuilderInstantiation.MakeGenericMethod(this, typeArgs);
        }

        public override bool IsGenericMethod { get { return m_method.IsGenericMethod; } }
      
        #endregion

        #region Public Abstract\Virtual Members
        internal override Type GetReturnType() { return m_method.GetReturnType(); }
        public override ParameterInfo ReturnParameter { get { throw new NotSupportedException(); } }
        public override ICustomAttributeProvider ReturnTypeCustomAttributes { get { throw new NotSupportedException(); } }
        public override MethodInfo GetBaseDefinition() { throw new NotSupportedException(); }
        #endregion    
    }

    internal sealed class ConstructorOnTypeBuilderInstantiation : ConstructorInfo
    {
        #region Private Static Members
        internal static ConstructorInfo GetConstructor(ConstructorInfo Constructor, TypeBuilderInstantiation type)
        {
            return new ConstructorOnTypeBuilderInstantiation(Constructor, type);
        }
        #endregion

        #region Private Data Mebers
        internal ConstructorInfo m_ctor;
        private TypeBuilderInstantiation m_type;
        #endregion

        #region Constructor
        internal ConstructorOnTypeBuilderInstantiation(ConstructorInfo constructor, TypeBuilderInstantiation type)
        {
            m_ctor = constructor;
            m_type = type;
        }
        #endregion
        
        internal override Type[] GetParameterTypes()
        {
            return m_ctor.GetParameterTypes();
        }

        #region MemberInfo Overrides
        public override MemberTypes MemberType { get { return m_ctor.MemberType;  } }
        public override String Name { get { return m_ctor.Name; } }
        public override Type DeclaringType { get { return m_type; } }
        public override Type ReflectedType { get { return m_type; } }
        public override Object[] GetCustomAttributes(bool inherit) { return m_ctor.GetCustomAttributes(inherit); } 
        public override Object[] GetCustomAttributes(Type attributeType, bool inherit) { return m_ctor.GetCustomAttributes(attributeType, inherit); }
        public override bool IsDefined(Type attributeType, bool inherit) { return m_ctor.IsDefined(attributeType, inherit); }
        internal override int MetadataTokenInternal 
        { 
            get 
            { 
                throw new NotSupportedException();
            } 
        }
        public override Module Module { get { return m_ctor.Module; } }              
        public new Type GetType() { return base.GetType(); }
        #endregion

        #region MethodBase Members
        public override ParameterInfo[] GetParameters() { return m_ctor.GetParameters(); }
        public override MethodImplAttributes GetMethodImplementationFlags() { return m_ctor.GetMethodImplementationFlags(); }
        public override RuntimeMethodHandle MethodHandle { get { return m_ctor.MethodHandle; } }
        public override MethodAttributes Attributes { get { return m_ctor.Attributes; } }
        public override Object Invoke(Object obj, BindingFlags invokeAttr, Binder binder, Object[] parameters, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
        public override CallingConventions CallingConvention { get { return m_ctor.CallingConvention; } }
        public override Type[] GetGenericArguments() { return m_ctor.GetGenericArguments(); }
        public override bool IsGenericMethodDefinition { get { return false; } }
        public override bool ContainsGenericParameters { get { return false; } }

        public override bool IsGenericMethod { get { return false; } }
        #endregion

        #region ConstructorInfo Members
        public override Object Invoke(BindingFlags invokeAttr, Binder binder, Object[] parameters, CultureInfo culture)
        {
            throw new InvalidOperationException();
        }
        #endregion
    }

    internal sealed class FieldOnTypeBuilderInstantiation : FieldInfo
    {
        #region Private Static Members
        private static Hashtable m_hashtable = new Hashtable();
        private struct Entry
        {
            public Entry(FieldInfo Field, TypeBuilderInstantiation type) { m_field = Field; m_type = type; }

            public FieldInfo m_field;
            public TypeBuilderInstantiation m_type;

            public override int GetHashCode() { return m_field.GetHashCode(); }

            public override bool Equals(object o)
            {
                if (o is Entry)
                    return Equals((Entry)o);
                else
                    return false;
            }
    
            public bool Equals(Entry obj)
            {
                return obj.m_field == m_field && obj.m_type == m_type;
            }                            
        }
        internal static FieldInfo GetField(FieldInfo Field, TypeBuilderInstantiation type)
        {
            Entry entry = new Entry(Field, type);

            if (m_hashtable.Contains(entry))
                return m_hashtable[entry] as FieldInfo;

            FieldInfo m = new FieldOnTypeBuilderInstantiation(Field, type);

            m_hashtable[entry] = m;
            return m;
        }
        #endregion

        #region Private Data Members
        private FieldInfo m_field;
        private TypeBuilderInstantiation m_type;
        #endregion

        #region Constructor
        internal FieldOnTypeBuilderInstantiation(FieldInfo field, TypeBuilderInstantiation type) { m_field = field; m_type = type; }       
        #endregion

        internal FieldInfo FieldInfo { get { return m_field; } }

        #region MemberInfo Overrides
        public override MemberTypes MemberType { get { return System.Reflection.MemberTypes.Field; } }          
        public override String Name { get { return m_field.Name; } }
        public override Type DeclaringType { get { return m_type; } }
        public override Type ReflectedType { get { return m_type; } }
        public override Object[] GetCustomAttributes(bool inherit) { return m_field.GetCustomAttributes(inherit); } 
        public override Object[] GetCustomAttributes(Type attributeType, bool inherit) { return m_field.GetCustomAttributes(attributeType, inherit); }
        public override bool IsDefined(Type attributeType, bool inherit) { return m_field.IsDefined(attributeType, inherit); }
        internal override int MetadataTokenInternal 
        { 
            get 
            { 
                throw new NotImplementedException();
            } 
        }
        public override Module Module { get { return m_field.Module; } }              
        public new Type GetType() { return base.GetType(); }
        #endregion

        #region Public Abstract\Virtual Members
        public override Type[] GetRequiredCustomModifiers() { return m_field.GetRequiredCustomModifiers(); }
        public override Type[] GetOptionalCustomModifiers() { return m_field.GetOptionalCustomModifiers(); }
        public override void SetValueDirect(TypedReference obj, Object value)
        {
            throw new NotImplementedException();
        }
        public override Object GetValueDirect(TypedReference obj)
        {
            throw new NotImplementedException();
        }    
        public override RuntimeFieldHandle FieldHandle 
        {
            get { throw new NotImplementedException(); }
        }    
        public override Type FieldType { get { throw new NotImplementedException(); } }
        public override Object GetValue(Object obj) { throw new InvalidOperationException(); }
        public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo culture) { throw new InvalidOperationException(); }
        public override FieldAttributes Attributes { get { return m_field.Attributes;  } }
        #endregion
    }
}








































