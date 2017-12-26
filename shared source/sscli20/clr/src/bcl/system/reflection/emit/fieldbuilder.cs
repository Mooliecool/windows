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

namespace System.Reflection.Emit 
{
    using System.Runtime.InteropServices;
    using System;
    using CultureInfo = System.Globalization.CultureInfo;
    using System.Reflection;
    using System.Security.Permissions;
    
    [HostProtection(MayLeakOnAbort = true)]
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_FieldBuilder))]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class FieldBuilder : FieldInfo, _FieldBuilder
    {
        #region Private Data Members
        private int m_fieldTok;
        private FieldToken m_tkField;
        private TypeBuilder m_typeBuilder;
        private String m_fieldName;
        private FieldAttributes m_Attributes;
        private Type m_fieldType;
        internal byte[] m_data;
        #endregion

        #region Constructor
        internal FieldBuilder(TypeBuilder typeBuilder, String fieldName, Type type, 
            Type[] requiredCustomModifiers, Type[] optionalCustomModifiers, FieldAttributes attributes)
        {
            if (fieldName == null)
                throw new ArgumentNullException("fieldName");

            if (fieldName.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "fieldName");

            if (fieldName[0] == '\0')
                throw new ArgumentException(Environment.GetResourceString("Argument_IllegalName"), "fieldName");

            if (type == null)
                throw new ArgumentNullException("type");

            if (type == typeof(void))
                throw new ArgumentException(Environment.GetResourceString("Argument_BadFieldType"));

            m_fieldName = fieldName;
            m_typeBuilder = typeBuilder;
            m_fieldType = type;
            m_Attributes = attributes & ~FieldAttributes.ReservedMask;
            
            SignatureHelper sigHelp = SignatureHelper.GetFieldSigHelper(m_typeBuilder.Module);
            sigHelp.AddArgument(type, requiredCustomModifiers, optionalCustomModifiers);

            int sigLength;
            byte[] signature = sigHelp.InternalGetSignature(out sigLength);
            
            m_fieldTok = TypeBuilder.InternalDefineField(
                typeBuilder.TypeToken.Token, fieldName, signature, sigLength, m_Attributes, m_typeBuilder.Module);

            m_tkField = new FieldToken(m_fieldTok, type);
        }

        #endregion

        #region Internal Members
        internal void SetData(byte[] data, int size)
        {
            if (data != null)
            {
                m_data = new byte[data.Length];
                Array.Copy(data, m_data, data.Length);
            }

            m_typeBuilder.Module.InternalSetFieldRVAContent(m_tkField.Token, data, size);
        }

        internal TypeBuilder GetTypeBuilder() { return m_typeBuilder; }
        #endregion

        #region MemberInfo Overrides
        internal override int MetadataTokenInternal
        {
            get { return m_fieldTok; }
        }
        
        public override Module Module
        {
            get { return m_typeBuilder.Module; }
        }

        public override String Name 
        {
            get {return m_fieldName; }
        }

        public override Type DeclaringType 
        {
            get 
            {
                if (m_typeBuilder.m_isHiddenGlobalType == true)
                    return null;

                return m_typeBuilder;
            }
        }
        
        public override Type ReflectedType 
        {
            get 
            {
                if (m_typeBuilder.m_isHiddenGlobalType == true)
                    return null;

                return m_typeBuilder;
            }
        }

        #endregion

        #region FieldInfo Overrides
        public override Type FieldType 
        {
            get { return m_fieldType; }
        }

        public override Object GetValue(Object obj)
        { 
            // NOTE!!  If this is implemented, make sure that this throws 
            // a NotSupportedException for Save-only dynamic assemblies.
            // Otherwise, it could cause the .cctor to be executed.

            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
        }

        public override void SetValue(Object obj,Object val,BindingFlags invokeAttr,Binder binder,CultureInfo culture)
        { 
            // NOTE!!  If this is implemented, make sure that this throws 
            // a NotSupportedException for Save-only dynamic assemblies.
            // Otherwise, it could cause the .cctor to be executed.

            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
        }

        public override RuntimeFieldHandle FieldHandle 
        {
            get { throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule")); }
        }

        public override FieldAttributes Attributes 
        {
            get { return m_Attributes; }
        }

        #endregion

        #region ICustomAttributeProvider Implementation
        public override Object[] GetCustomAttributes(bool inherit)
        {
             
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
        }
            
        public override Object[] GetCustomAttributes(Type attributeType, bool inherit)
        {
             
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
        }

        public override bool IsDefined(Type attributeType, bool inherit)
        {
             
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
        }

        #endregion

        #region Public Members
        public FieldToken GetToken() 
        {
            return m_tkField;
        }

        public void SetOffset(int iOffset) 
        {
            m_typeBuilder.ThrowIfCreated();     
   
            TypeBuilder.InternalSetFieldOffset(m_typeBuilder.Module, GetToken().Token, iOffset);
        }

        [Obsolete("An alternate API is available: Emit the MarshalAs custom attribute instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        public void SetMarshal(UnmanagedMarshal unmanagedMarshal)
        {
            m_typeBuilder.ThrowIfCreated();        
            
            if (unmanagedMarshal == null)
                throw new ArgumentNullException("unmanagedMarshal");

            byte[] ubMarshal = unmanagedMarshal.InternalGetBytes();

            TypeBuilder.InternalSetMarshalInfo(m_typeBuilder.Module, GetToken().Token, ubMarshal, ubMarshal.Length);
        }

        public void SetConstant(Object defaultValue) 
        {
            m_typeBuilder.ThrowIfCreated();  
      
            TypeBuilder.SetConstantValue(m_typeBuilder.Module, GetToken().Token, m_fieldType, defaultValue);
        }
        
[System.Runtime.InteropServices.ComVisible(true)]
        public void SetCustomAttribute(ConstructorInfo con, byte[] binaryAttribute)
        {
            ModuleBuilder module = m_typeBuilder.Module as ModuleBuilder;

            m_typeBuilder.ThrowIfCreated();   
     
            if (con == null)
                throw new ArgumentNullException("con");

            if (binaryAttribute == null)
                throw new ArgumentNullException("binaryAttribute");
            
            TypeBuilder.InternalCreateCustomAttribute(
                m_tkField.Token, module.GetConstructorToken(con).Token, binaryAttribute, module, false);
        }

        public void SetCustomAttribute(CustomAttributeBuilder customBuilder)
        {
            m_typeBuilder.ThrowIfCreated();      
  
            if (customBuilder == null)
                throw new ArgumentNullException("customBuilder");

            ModuleBuilder module = m_typeBuilder.Module as ModuleBuilder;

            customBuilder.CreateCustomAttribute(module, m_tkField.Token);
        }

        #endregion

        void _FieldBuilder.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _FieldBuilder.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _FieldBuilder.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _FieldBuilder.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }
    }
}
