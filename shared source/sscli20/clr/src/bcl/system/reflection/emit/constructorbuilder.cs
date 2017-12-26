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
    using System;
    using System.Reflection;
    using CultureInfo = System.Globalization.CultureInfo;
    using System.Diagnostics.SymbolStore;
    using System.Security;
    using System.Security.Permissions;
    using System.Runtime.InteropServices;
    
    [HostProtection(MayLeakOnAbort = true)]
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_ConstructorBuilder))]
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class ConstructorBuilder : ConstructorInfo, _ConstructorBuilder
    { 
        #region Private Data Members
        internal MethodBuilder m_methodBuilder;
        internal bool m_ReturnILGen;
        #endregion

        #region Constructor
        private ConstructorBuilder()
        {
        }
        
        internal ConstructorBuilder(String name, MethodAttributes attributes, CallingConventions callingConvention,
            Type[] parameterTypes, Type[][] requiredCustomModifiers, Type[][] optionalCustomModifiers, Module mod, TypeBuilder type)
        {
            int sigLength;
            byte[] sigBytes;
            MethodToken token;

            m_methodBuilder = new MethodBuilder(name, attributes, callingConvention, null, null, null, 
                parameterTypes, requiredCustomModifiers, optionalCustomModifiers, mod, type, false);

            type.m_listMethods.Add(m_methodBuilder);
            
            sigBytes = m_methodBuilder.GetMethodSignature().InternalGetSignature(out sigLength);
    
            token = m_methodBuilder.GetToken();
            m_ReturnILGen = true;
        }

        internal ConstructorBuilder(String name, MethodAttributes attributes, CallingConventions callingConvention,
            Type[] parameterTypes, Module mod, TypeBuilder type) : 
            this(name, attributes, callingConvention, parameterTypes, null, null, mod, type)
        {
        }

        #endregion

        #region Internal
        internal override Type[] GetParameterTypes()
        {
            return m_methodBuilder.GetParameterTypes();
        }

        #endregion

        #region Object Overrides
        public override String ToString()
        {
            return m_methodBuilder.ToString();
        }
        
        #endregion

        #region MemberInfo Overrides
        internal override int MetadataTokenInternal
        {
            get { return m_methodBuilder.MetadataTokenInternal; }
        }
        
        public override Module Module
        {
            get { return m_methodBuilder.Module; }
        }
        
        public override Type ReflectedType
        {
            get { return m_methodBuilder.ReflectedType; }
        }

        public override Type DeclaringType
        {
            get { return m_methodBuilder.DeclaringType; }
        }
    
        public override String Name 
        {
            get { return m_methodBuilder.Name; }
        }

        #endregion

        #region MethodBase Overrides
        public override Object Invoke(Object obj, BindingFlags invokeAttr, Binder binder, Object[] parameters, CultureInfo culture) 
        {
             
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule")); 
        }

        public override ParameterInfo[] GetParameters()
        {
            if (!m_methodBuilder.m_bIsBaked)
                throw new NotSupportedException(Environment.GetResourceString("InvalidOperation_TypeNotCreated"));

            Type rti = m_methodBuilder.GetTypeBuilder().m_runtimeType;
            ConstructorInfo rci = rti.GetConstructor(m_methodBuilder.m_parameterTypes);

            return rci.GetParameters();
        }
                    
        public override MethodAttributes Attributes
        {
            get { return m_methodBuilder.Attributes; }
        }

        public override MethodImplAttributes GetMethodImplementationFlags()
        {
            return m_methodBuilder.GetMethodImplementationFlags();
        }
        
        public override RuntimeMethodHandle MethodHandle 
        {
            get { return m_methodBuilder.MethodHandle; }
        }
        
        #endregion

        #region ConstructorInfo Overrides
        public override Object Invoke(BindingFlags invokeAttr, Binder binder, Object[] parameters, CultureInfo culture)
        {
            
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule")); 
        }
    
        #endregion

        #region ICustomAttributeProvider Implementation
        public override Object[] GetCustomAttributes(bool inherit)
        {
            return m_methodBuilder.GetCustomAttributes(inherit);
        }

        public override Object[] GetCustomAttributes(Type attributeType, bool inherit)
        {
            return m_methodBuilder.GetCustomAttributes(attributeType, inherit);
        }
        
        public override bool IsDefined (Type attributeType, bool inherit)
        {
            return m_methodBuilder.IsDefined(attributeType, inherit);
        }

        #endregion

        #region Public Members
        public MethodToken GetToken()
        {
            return m_methodBuilder.GetToken();
        }
    
        public ParameterBuilder DefineParameter(int iSequence, ParameterAttributes attributes, String strParamName)
        {
            // MD will assert if we try to set the reserved bits explicitly
            attributes = attributes & ~ParameterAttributes.ReservedMask;
            return m_methodBuilder.DefineParameter(iSequence, attributes, strParamName);
        }
    
        public void SetSymCustomAttribute(String name, byte[] data)
        {
            m_methodBuilder.SetSymCustomAttribute(name, data);
        }
    
        public ILGenerator GetILGenerator() 
        {
            if (!m_ReturnILGen)
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_DefaultConstructorILGen"));

            return m_methodBuilder.GetILGenerator();
        }
        
        public ILGenerator GetILGenerator(int streamSize)
        {
            if (!m_ReturnILGen)
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_DefaultConstructorILGen"));

            return m_methodBuilder.GetILGenerator(streamSize);
        }

        public void AddDeclarativeSecurity(SecurityAction action, PermissionSet pset)
        {
            if (pset == null)
                throw new ArgumentNullException("pset");

            if (!Enum.IsDefined(typeof(SecurityAction), action) || 
                action == SecurityAction.RequestMinimum ||
                action == SecurityAction.RequestOptional ||
                action == SecurityAction.RequestRefuse )
                throw new ArgumentOutOfRangeException("action");

            // Cannot add declarative security after type is created.
            if (m_methodBuilder.IsTypeCreated())
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_TypeHasBeenCreated"));
    
            // Translate permission set into serialized format (use standard binary serialization).
            byte[] blob = pset.EncodeXml();
    
            // Write the blob into the metadata.
            TypeBuilder.InternalAddDeclarativeSecurity(GetModule(), GetToken().Token, action, blob);
        }
    
        public override CallingConventions CallingConvention 
        { 
            get 
            { 
                if (DeclaringType.IsGenericType)
                    return CallingConventions.HasThis;
                    
                return CallingConventions.Standard; 
            } 
        }
    
        public Module GetModule()
        {
            return m_methodBuilder.GetModule();
        }
    
    
        public Type ReturnType
        {
            get { return m_methodBuilder.GetReturnType(); }
        }
        
        internal override Type GetReturnType() 
        {
            return m_methodBuilder.GetReturnType();
        }
                                
        public String Signature 
        {
            get { return m_methodBuilder.Signature; }
        }
    
        [System.Runtime.InteropServices.ComVisible(true)]
        public void SetCustomAttribute(ConstructorInfo con, byte[] binaryAttribute)
        {
            m_methodBuilder.SetCustomAttribute(con, binaryAttribute);
        }

        public void SetCustomAttribute(CustomAttributeBuilder customBuilder)
        {
            m_methodBuilder.SetCustomAttribute(customBuilder);
        }

        public void SetImplementationFlags(MethodImplAttributes attributes) 
        {
            m_methodBuilder.SetImplementationFlags(attributes);
        }
                
        public bool InitLocals 
        {
            get { return m_methodBuilder.InitLocals; }
            set { m_methodBuilder.InitLocals = value; }
        }

        #endregion

        void _ConstructorBuilder.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _ConstructorBuilder.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _ConstructorBuilder.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _ConstructorBuilder.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }
    }
}

