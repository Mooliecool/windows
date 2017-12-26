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
    using System.Text;
    using System;
    using CultureInfo = System.Globalization.CultureInfo;
    using System.Diagnostics.SymbolStore;
    using System.Reflection;
    using System.Security;
    using System.Collections;
    using System.Security.Permissions;
    using System.Runtime.InteropServices;

    [HostProtection(MayLeakOnAbort = true)]
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_MethodBuilder))]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class MethodBuilder : MethodInfo, _MethodBuilder
    {
        #region Private Data Members
        // Identity
        internal String m_strName; // The name of the method
        private MethodToken m_tkMethod; // The token of this method
        internal Module m_module;
        internal TypeBuilder m_containingType;
        private MethodBuilder m_link;

        // IL
        private int[] m_RVAFixups; // The location of all RVA fixups.  Primarily used for Strings.
        private int[] m_mdMethodFixups; // The location of all of the token fixups.
        private SignatureHelper m_localSignature;
        internal LocalSymInfo m_localSymInfo; // keep track debugging local information
        internal ILGenerator m_ilGenerator;
        private byte[] m_ubBody; // The IL for the method
        private int m_numExceptions; // The number of exceptions.  Each try-catch tuple is a separate exception.
        private __ExceptionInstance[] m_exceptions; //The instance of each exception.

        // Flags
        internal bool m_bIsBaked;
        private bool m_bIsGlobalMethod;
        private bool m_fInitLocals; // indicating if the method stack frame will be zero initialized or not.

        // Attributes
        private MethodAttributes m_iAttributes;
        private CallingConventions m_callingConvention;
        private MethodImplAttributes m_dwMethodImplFlags;

        // Parameters
        private SignatureHelper m_signature;
        internal Type[] m_parameterTypes;
        private ParameterBuilder m_retParam;
        internal Type m_returnType;
        private Type[] m_returnTypeRequiredCustomModifiers;
        private Type[] m_returnTypeOptionalCustomModifiers;
        private Type[][] m_parameterTypeRequiredCustomModifiers;
        private Type[][] m_parameterTypeOptionalCustomModifiers;

        // Generics
        private GenericTypeParameterBuilder[] m_inst;
        private bool m_bIsGenMethDef;
        #endregion

        #region Constructor
        internal MethodBuilder(String name, MethodAttributes attributes, CallingConventions callingConvention,
            Type returnType, Type[] parameterTypes, Module mod, TypeBuilder type, bool bIsGlobalMethod) 
        {
            Init(name, attributes, callingConvention, returnType, null, null, parameterTypes, null, null, mod, type, bIsGlobalMethod);
        }

        internal MethodBuilder(String name, MethodAttributes attributes, CallingConventions callingConvention,
            Type returnType, Type[] returnTypeRequiredCustomModifiers, Type[] returnTypeOptionalCustomModifiers,
            Type[] parameterTypes, Type[][] parameterTypeRequiredCustomModifiers, Type[][] parameterTypeOptionalCustomModifiers,
            Module mod, TypeBuilder type, bool bIsGlobalMethod)
        {
            Init(name, attributes, callingConvention, 
                returnType, returnTypeRequiredCustomModifiers, returnTypeOptionalCustomModifiers,
                parameterTypes, parameterTypeRequiredCustomModifiers, parameterTypeOptionalCustomModifiers,
                mod, type, bIsGlobalMethod);
        }

        private void Init(String name, MethodAttributes attributes, CallingConventions callingConvention,
            Type returnType, Type[] returnTypeRequiredCustomModifiers, Type[] returnTypeOptionalCustomModifiers,
            Type[] parameterTypes, Type[][] parameterTypeRequiredCustomModifiers, Type[][] parameterTypeOptionalCustomModifiers, 
            Module mod, TypeBuilder type, bool bIsGlobalMethod)
        {
            if (name == null)
                throw new ArgumentNullException("name");

            if (name.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "name");

            if (name[0] == '\0')
                throw new ArgumentException(Environment.GetResourceString("Argument_IllegalName"), "name");

            if (mod == null)
                throw new ArgumentNullException("mod");

            if (parameterTypes != null)
            {
                foreach(Type t in parameterTypes)
                {
                    if (t == null)
                        throw new ArgumentNullException("parameterTypes");
                }
            }

            m_link = type.m_currentMethod;
            type.m_currentMethod = this;
            m_strName = name;
            m_module = mod;
            m_containingType = type;
            m_localSignature = SignatureHelper.GetLocalVarSigHelper(mod);

            // 
            //if (returnType == null)
            //{
            //    m_returnType = typeof(void);
            //}
            //else
            {
                m_returnType = returnType;
            }

            if ((attributes & MethodAttributes.Static) == 0)
            {
                // turn on the has this calling convention
                callingConvention = callingConvention | CallingConventions.HasThis;
            }
            else if ((attributes & MethodAttributes.Virtual) != 0)
            {
                // A method can't be both static and virtual
                throw new ArgumentException(Environment.GetResourceString("Arg_NoStaticVirtual"));
            }

            if ((attributes & MethodAttributes.SpecialName) != MethodAttributes.SpecialName)
            {
                if ((type.Attributes & TypeAttributes.Interface) == TypeAttributes.Interface)
                {
                    // methods on interface have to be abstract + virtual except special name methods such as type initializer
                    if ((attributes & (MethodAttributes.Abstract | MethodAttributes.Virtual)) != 
                        (MethodAttributes.Abstract | MethodAttributes.Virtual) &&                         
                        (attributes & MethodAttributes.Static) == 0)
                        throw new ArgumentException(Environment.GetResourceString("Argument_BadAttributeOnInterfaceMethod"));               
                }
            }

            m_callingConvention = callingConvention;

            if (parameterTypes != null)
            {
                m_parameterTypes = new Type[parameterTypes.Length];
                Array.Copy(parameterTypes, m_parameterTypes, parameterTypes.Length);
            }
            else
            {
                m_parameterTypes = null;
            }

            m_returnTypeRequiredCustomModifiers = returnTypeRequiredCustomModifiers;
            m_returnTypeOptionalCustomModifiers = returnTypeOptionalCustomModifiers;
            m_parameterTypeRequiredCustomModifiers = parameterTypeRequiredCustomModifiers;
            m_parameterTypeOptionalCustomModifiers = parameterTypeOptionalCustomModifiers;

//            m_signature = SignatureHelper.GetMethodSigHelper(mod, callingConvention, 
//                returnType, returnTypeRequiredCustomModifiers, returnTypeOptionalCustomModifiers,
//                parameterTypes, parameterTypeRequiredCustomModifiers, parameterTypeOptionalCustomModifiers);

            m_iAttributes = attributes;
            m_bIsGlobalMethod = bIsGlobalMethod;
            m_bIsBaked = false;
            m_fInitLocals = true;

            m_localSymInfo = new LocalSymInfo();
            m_ubBody = null;
            m_ilGenerator = null;

            // Default is managed IL. Manged IL has bit flag 0x0020 set off
            m_dwMethodImplFlags = MethodImplAttributes.IL;

            //int i = MetadataTokenInternal;
        }

        #endregion

        #region Internal Members
        internal void CheckContext(params Type[][] typess)
        {
            ((AssemblyBuilder)Module.Assembly).CheckContext(typess);            
        }
        internal void CheckContext(params Type[] types)
        {
            ((AssemblyBuilder)Module.Assembly).CheckContext(types);
        }

        internal void CreateMethodBodyHelper(ILGenerator il)
        {
            // Sets the IL of the method.  An ILGenerator is passed as an argument and the method
            // queries this instance to get all of the information which it needs.

            __ExceptionInfo[]   excp;
            int                 counter=0;
            int[]               filterAddrs;
            int[]               catchAddrs;
            int[]               catchEndAddrs;
            Type[]              catchClass;
            int[]               type;
            int                 numCatch;
            int                 start, end;
            ModuleBuilder       dynMod = (ModuleBuilder) m_module;

            m_containingType.ThrowIfCreated();

            if (m_bIsBaked)
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_MethodHasBody"));
            }

            if (il==null)
            {
                throw new ArgumentNullException("il");
            }

            if (il.m_methodBuilder != this && il.m_methodBuilder != null)
            {
                // you don't need to call CreateMethodBody when you get your ILGenerator
                // through MethodBuilder::GetILGenerator.
                //

                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_BadILGeneratorUsage"));
            }
            
            ThrowIfShouldNotHaveBody();

            if (il.m_ScopeTree.m_iOpenScopeCount != 0)
            {
                // There are still unclosed local scope
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_OpenLocalVariableScope"));
            }


            m_ubBody = il.BakeByteArray();

            m_RVAFixups = il.GetRVAFixups();
            m_mdMethodFixups = il.GetTokenFixups();

            //Okay, now the fun part.  Calculate all of the exceptions.
            excp = il.GetExceptions();
            m_numExceptions = CalculateNumberOfExceptions(excp);
            if (m_numExceptions>0)
            {

                m_exceptions = new __ExceptionInstance[m_numExceptions];

                for (int i=0; i<excp.Length; i++) {
                    filterAddrs = excp[i].GetFilterAddresses();
                    catchAddrs = excp[i].GetCatchAddresses();
                    catchEndAddrs = excp[i].GetCatchEndAddresses();
                    catchClass = excp[i].GetCatchClass();


                    // track the reference of the catch class
                    for (int j=0; j < catchClass.Length; j++)
                    {
                        if (catchClass[j] != null)
                            dynMod.GetTypeToken(catchClass[j]);
                    }

                    numCatch = excp[i].GetNumberOfCatches();
                    start = excp[i].GetStartAddress();
                    end = excp[i].GetEndAddress();
                    type = excp[i].GetExceptionTypes();
                    for (int j=0; j<numCatch; j++) {
                        int tkExceptionClass = 0;
                        if (catchClass[j] != null)
                            tkExceptionClass = dynMod.GetTypeToken(catchClass[j]).Token;
                        switch (type[j]) {
                        case __ExceptionInfo.None:
                        case __ExceptionInfo.Fault:
                        case __ExceptionInfo.Filter:
                            m_exceptions[counter++]=new __ExceptionInstance(start, end, filterAddrs[j], catchAddrs[j], catchEndAddrs[j], type[j], tkExceptionClass);
                            break;

                        case __ExceptionInfo.Finally:
                            m_exceptions[counter++]=new __ExceptionInstance(start, excp[i].GetFinallyEndAddress(), filterAddrs[j], catchAddrs[j], catchEndAddrs[j], type[j], tkExceptionClass);
                            break;
                        }
                    }

                }
            }


            m_bIsBaked=true;

            if (dynMod.GetSymWriter() != null)
            {

                // set the debugging information such as scope and line number
                // if it is in a debug module
                //
                SymbolToken  tk = new SymbolToken(MetadataTokenInternal);
                ISymbolWriter symWriter = dynMod.GetSymWriter();

                // call OpenMethod to make this method the current method
                symWriter.OpenMethod(tk);

                // call OpenScope because OpenMethod no longer implicitly creating
                // the top-levelsmethod scope
                //
                symWriter.OpenScope(0);
                
                if (m_symCustomAttrs != null)
                {
                    foreach(SymCustomAttr symCustomAttr in m_symCustomAttrs)
                        dynMod.GetSymWriter().SetSymAttribute(
                        new SymbolToken (MetadataTokenInternal), 
                            symCustomAttr.m_name, 
                            symCustomAttr.m_data);
                }
                
                if (m_localSymInfo != null)
                    m_localSymInfo.EmitLocalSymInfo(symWriter);
                il.m_ScopeTree.EmitScopeTree(symWriter);
                il.m_LineNumberInfo.EmitLineNumberInfo(symWriter);
                symWriter.CloseScope(il.m_length);
                symWriter.CloseMethod();
            }
        }

        internal override Type[] GetParameterTypes()
        {
            if (m_parameterTypes == null)
                m_parameterTypes = new Type[0];

            return m_parameterTypes;
        }

        internal void SetToken(MethodToken token)
        {
            m_tkMethod = token;
        }

        internal byte[] GetBody()
        {
            // Returns the il bytes of this method.
            // This il is not valid until somebody has called BakeByteArray
            return m_ubBody;
        }

        internal int[] GetTokenFixups()
        {
            return m_mdMethodFixups;
        }

        internal int[] GetRVAFixups()
        {
            return m_RVAFixups;
        }

        internal SignatureHelper GetMethodSignature()
        {
            if (m_parameterTypes == null)
                m_parameterTypes = new Type[0];

            m_signature = SignatureHelper.GetMethodSigHelper (m_module, m_callingConvention, m_inst != null ? m_inst.Length : 0, 
                m_returnType == null ? typeof(void) : m_returnType, m_returnTypeRequiredCustomModifiers, m_returnTypeOptionalCustomModifiers,
                m_parameterTypes, m_parameterTypeRequiredCustomModifiers, m_parameterTypeOptionalCustomModifiers);

            return m_signature;
        }

        internal SignatureHelper GetLocalsSignature()
        {
            if (m_ilGenerator != null)
            {
                if (m_ilGenerator.m_localCount != 0)
                {
                    // If user is using ILGenerator::DeclareLocal, then get local signaturefrom there.
                    return m_ilGenerator.m_localSignature;
                }
            }

            return m_localSignature;
        }

        internal int GetNumberOfExceptions()
        {
            return m_numExceptions;
        }

        internal __ExceptionInstance[] GetExceptionInstances()
        {
            return m_exceptions;
        }

        internal int CalculateNumberOfExceptions(__ExceptionInfo [] excp)
        {
            int num=0;

            if (excp==null) 
            {
                return 0;
            }

            for (int i=0; i<excp.Length; i++) 
            {
                num+=excp[i].GetNumberOfCatches();
            }

            return num;
        }

        internal bool IsTypeCreated()
        { 
            return (m_containingType != null && m_containingType.m_hasBeenCreated); 
        }

        internal TypeBuilder GetTypeBuilder()
        { 
            return m_containingType;
        }
        #endregion

        #region Object Overrides
        public override bool Equals(Object obj) {
            if (!(obj is MethodBuilder)) {
                return false;
            }
            if (!(this.m_strName.Equals(((MethodBuilder)obj).m_strName))) {
                return false;
            }

            if (m_iAttributes!=(((MethodBuilder)obj).m_iAttributes)) {
                return false;
            }

            SignatureHelper thatSig = ((MethodBuilder)obj).GetMethodSignature();
            if (thatSig.Equals(GetMethodSignature())) {
                return true;
            }
            return false;
        }

        public override int GetHashCode()
        {
            return this.m_strName.GetHashCode();
        }

        public override String ToString()
        {
            StringBuilder sb = new StringBuilder(1000);
            sb.Append("Name: " + m_strName + " " + Environment.NewLine);
            sb.Append("Attributes: " + (int)m_iAttributes + Environment.NewLine);
            sb.Append("Method Signature: " + GetMethodSignature() + Environment.NewLine);

            sb.Append(Environment.NewLine);
            return sb.ToString();
        }

        #endregion

        #region MemberInfo Overrides
        public override String Name
        {
            get 
            { 
                return m_strName; 
            }
        }

        internal override int MetadataTokenInternal
        {
            get 
            {
                return GetToken().Token;
            }
        }

        public override Module Module
        {
            get 
            {
                return m_containingType.Module;
            }
        }

        public override Type DeclaringType
        {
            get
            {
                if (m_containingType.m_isHiddenGlobalType == true)
                    return null;
                return m_containingType;
            }
        }

        public override ICustomAttributeProvider ReturnTypeCustomAttributes 
        {
            get 
            {
                return null;
            }
        }

        public override Type ReflectedType
        {
            get
            {
                if (m_containingType.m_isHiddenGlobalType == true)
                    return null;
                return m_containingType;
            }
        }

        #endregion

        #region MethodBase Overrides
        public override Object Invoke(Object obj, BindingFlags invokeAttr, Binder binder, Object[] parameters, CultureInfo culture)
        {
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
        }

        public override MethodImplAttributes GetMethodImplementationFlags()
        {
            return m_dwMethodImplFlags;
        }

        public override MethodAttributes Attributes
        {
            get { return m_iAttributes; }
        }

        public override CallingConventions CallingConvention
        {
            get {return m_callingConvention;}
        }

        public override RuntimeMethodHandle MethodHandle
        {
            get { throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule")); }
        }

        #endregion

        #region MethodInfo Overrides
        public override MethodInfo GetBaseDefinition()
        {
            return this;
        }

        internal override Type GetReturnType() {
                return m_returnType;
        }

        public override ParameterInfo[] GetParameters()
        {
            if (!m_bIsBaked)
                throw new NotSupportedException(Environment.GetResourceString("InvalidOperation_TypeNotCreated"));

            Type rti = m_containingType.m_runtimeType;
            MethodInfo rmi = rti.GetMethod(m_strName, m_parameterTypes);

            return rmi.GetParameters();
        }

        public override ParameterInfo ReturnParameter
        {
            get
            {
                if (!m_bIsBaked)
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_TypeNotCreated"));

                Type rti = m_containingType.m_runtimeType;
                MethodInfo rmi = rti.GetMethod(m_strName, m_parameterTypes);

                return rmi.ReturnParameter;
            }
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

        #region Generic Members
        public override bool IsGenericMethodDefinition { get { return m_bIsGenMethDef; } }

        public override bool ContainsGenericParameters { get { throw new NotSupportedException(); } }

        public override MethodInfo GetGenericMethodDefinition() { if (!IsGenericMethod) throw new InvalidOperationException(); return this; }

        public override bool IsGenericMethod { get { return m_inst != null; } }
         
        public override Type[] GetGenericArguments() { return m_inst; }

        public override MethodInfo MakeGenericMethod(params Type[] typeArguments) 
        {
           	return new MethodBuilderInstantiation(this, typeArguments); 
        }
    
        
        public GenericTypeParameterBuilder[] DefineGenericParameters (params string[] names)
        {
            if (m_inst != null)
                throw new InvalidOperationException ();

            if (names == null)
                throw new ArgumentNullException("names");

            for (int i = 0; i < names.Length; i ++)
                if (names[i] == null)
                    throw new ArgumentNullException("names");

            if (m_tkMethod.Token != 0)
                throw new InvalidOperationException ();

            if (names.Length == 0)
                throw new ArgumentException ();

            m_bIsGenMethDef = true;
            m_inst = new GenericTypeParameterBuilder[names.Length];
            for (int i = 0; i < names.Length;  i ++)
                m_inst[i] = new GenericTypeParameterBuilder(new TypeBuilder(names[i], i, this));

            return m_inst;
        }
        
        internal void ThrowIfGeneric () { if (IsGenericMethod && !IsGenericMethodDefinition) throw new InvalidOperationException (); }
        #endregion

        #region Public Members
        public MethodToken GetToken()
        {
            if (m_tkMethod.Token == 0)
            {
                if (m_link != null)
                    m_link.GetToken();

                int sigLength;
                byte[] sigBytes = GetMethodSignature().InternalGetSignature(out sigLength);

                m_tkMethod = new MethodToken (TypeBuilder.InternalDefineMethod(
                    m_containingType.MetadataTokenInternal, m_strName, sigBytes, sigLength, Attributes, m_module));

                if (m_inst != null)
                    foreach (GenericTypeParameterBuilder tb in m_inst)
                        if (!tb.m_type.IsCreated()) tb.m_type.CreateType();

                TypeBuilder.InternalSetMethodImpl (m_module, MetadataTokenInternal, m_dwMethodImplFlags);
            }

            return m_tkMethod;
        }

        public void SetParameters (params Type[] parameterTypes)
        {
            CheckContext(parameterTypes);
            SetSignature (null, null, null, parameterTypes, null, null);
        }

        public void SetReturnType (Type returnType)
        {
            CheckContext(returnType);
            SetSignature (returnType, null, null, null, null, null);
        }

        public void SetSignature(
            Type returnType, Type[] returnTypeRequiredCustomModifiers, Type[] returnTypeOptionalCustomModifiers, 
            Type[] parameterTypes, Type[][] parameterTypeRequiredCustomModifiers, Type[][] parameterTypeOptionalCustomModifiers)
        {
            CheckContext(returnType);
            CheckContext(returnTypeRequiredCustomModifiers, returnTypeOptionalCustomModifiers, parameterTypes);
            CheckContext(parameterTypeRequiredCustomModifiers);
            CheckContext(parameterTypeOptionalCustomModifiers);
            
            ThrowIfGeneric();

            if (returnType != null)
            {
                m_returnType = returnType;
            }

            if (parameterTypes != null)
            {
                m_parameterTypes = new Type[parameterTypes.Length];
                Array.Copy (parameterTypes, m_parameterTypes, parameterTypes.Length);
            }

            m_returnTypeRequiredCustomModifiers = returnTypeRequiredCustomModifiers;
            m_returnTypeOptionalCustomModifiers = returnTypeOptionalCustomModifiers;
            m_parameterTypeRequiredCustomModifiers = parameterTypeRequiredCustomModifiers;
            m_parameterTypeOptionalCustomModifiers = parameterTypeOptionalCustomModifiers;
        }

       
        public ParameterBuilder DefineParameter(int position, ParameterAttributes attributes, String strParamName)
        {
            ThrowIfGeneric ();
            m_containingType.ThrowIfCreated ();
    
            if (position < 0)
                throw new ArgumentOutOfRangeException(Environment.GetResourceString("ArgumentOutOfRange_ParamSequence"));

            if (position > 0 && (m_parameterTypes == null || position > m_parameterTypes.Length))
                throw new ArgumentOutOfRangeException(Environment.GetResourceString("ArgumentOutOfRange_ParamSequence"));
                
            attributes = attributes & ~ParameterAttributes.ReservedMask;
            return new ParameterBuilder(this, position, attributes, strParamName);
        }

        [Obsolete("An alternate API is available: Emit the MarshalAs custom attribute instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        public void SetMarshal(UnmanagedMarshal unmanagedMarshal)
        {
            ThrowIfGeneric ();

            // set Marshal info for the return type

            m_containingType.ThrowIfCreated();
            
            if (m_retParam == null)
            {
                m_retParam = new ParameterBuilder(this, 0, 0, null);
            }

            m_retParam.SetMarshal(unmanagedMarshal);
        }

        private ArrayList m_symCustomAttrs;
        private struct SymCustomAttr
        {
            public SymCustomAttr(String name, byte[] data)
            {
                m_name = name;
                m_data = data;
            }
            public String m_name;
            public byte[] m_data;
        }

        public void SetSymCustomAttribute(String name, byte[] data)           
        {
            ThrowIfGeneric ();

            // This is different from CustomAttribute. This is stored into the SymWriter.
            m_containingType.ThrowIfCreated();

            ModuleBuilder dynMod = (ModuleBuilder) m_module;
            if ( dynMod.GetSymWriter() == null)
            {
                // Cannot SetSymCustomAttribute when it is not a debug module
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NotADebugModule"));
            }

            if (m_symCustomAttrs == null)
                m_symCustomAttrs = new ArrayList();

            m_symCustomAttrs.Add(new SymCustomAttr(name, data));
        }

        public void AddDeclarativeSecurity(SecurityAction action, PermissionSet pset)
        {
            ThrowIfGeneric ();

            if (pset == null)
                throw new ArgumentNullException("pset");

            if (!Enum.IsDefined(typeof(SecurityAction), action) || 
                action == SecurityAction.RequestMinimum ||
                action == SecurityAction.RequestOptional ||
                action == SecurityAction.RequestRefuse )
                throw new ArgumentOutOfRangeException("action");

            // cannot declarative security after type is created
            m_containingType.ThrowIfCreated();

            // Translate permission set into serialized format (uses standard binary serialization format).
            byte[] blob = null;
            if (!pset.IsEmpty())
                blob = pset.EncodeXml();

            // Write the blob into the metadata.
            TypeBuilder.InternalAddDeclarativeSecurity (m_module, MetadataTokenInternal, action, blob);
        }

        public void CreateMethodBody(byte[] il,int count)
        {
            ThrowIfGeneric ();

            // Note that when user calls this function, there are a few information that client is
            // not able to supply: local signature, exception handlers, max stack size, a list of Token fixup, a list of RVA fixup

            if (m_bIsBaked)
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_MethodBaked"));
            }

            m_containingType.ThrowIfCreated();

            if (il != null && (count < 0 || count > il.Length))
                throw new ArgumentOutOfRangeException("count", Environment.GetResourceString("ArgumentOutOfRange_Index"));
            if (il == null) {
                m_ubBody = null;
                return;
            }
            m_ubBody = new byte[count];
            Array.Copy(il,m_ubBody,count);
            m_bIsBaked=true;
        }

        public void SetImplementationFlags(MethodImplAttributes attributes) 
        {
            ThrowIfGeneric ();

            m_containingType.ThrowIfCreated ();

            m_dwMethodImplFlags = attributes;

            m_canBeRuntimeImpl = true;

            TypeBuilder.InternalSetMethodImpl (m_module, MetadataTokenInternal, attributes);
        }

        public ILGenerator GetILGenerator() {
            ThrowIfGeneric ();
            ThrowIfShouldNotHaveBody();

            if (m_ilGenerator == null)
                m_ilGenerator = new ILGenerator(this);
            return m_ilGenerator;
        }

          public ILGenerator GetILGenerator(int size) {
            ThrowIfGeneric ();
            ThrowIfShouldNotHaveBody();
            
            if (m_ilGenerator == null)
                m_ilGenerator = new ILGenerator(this, size);
            return m_ilGenerator;
        }

        private void ThrowIfShouldNotHaveBody() {
            if ((m_dwMethodImplFlags&MethodImplAttributes.CodeTypeMask) != MethodImplAttributes.IL ||
                (m_dwMethodImplFlags & MethodImplAttributes.Unmanaged) != 0 ||
                (m_iAttributes & MethodAttributes.PinvokeImpl) != 0 ||
                m_isDllImport)
            {
                // cannot attach method body if methodimpl is marked not marked as managed IL
                //
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_ShouldNotHaveMethodBody"));
            }
        }
            

        public bool InitLocals 
        {            
            // Property is set to true if user wishes to have zero initialized stack frame for this method. Default to false.
            get { ThrowIfGeneric (); return m_fInitLocals; }
            set { ThrowIfGeneric (); m_fInitLocals = value; }
        }

        public Module GetModule()
        {
            return m_module;
        }

        public String Signature 
        { 
            get 
            { 
                return GetMethodSignature().ToString(); 
            } 
        }

[System.Runtime.InteropServices.ComVisible(true)]
        public void SetCustomAttribute(ConstructorInfo con, byte[] binaryAttribute)
        {
            ThrowIfGeneric ();
            
            if (con == null)
                throw new ArgumentNullException("con");
            if (binaryAttribute == null)
                throw new ArgumentNullException("binaryAttribute");

            TypeBuilder.InternalCreateCustomAttribute (MetadataTokenInternal,
                ((ModuleBuilder)m_module).GetConstructorToken(con).Token,
                binaryAttribute,
                m_module,
                false);

            if (IsKnownCA(con))
                ParseCA(con, binaryAttribute);
        }

        public void SetCustomAttribute(CustomAttributeBuilder customBuilder)
        {
            ThrowIfGeneric ();
            
            if (customBuilder == null)
                throw new ArgumentNullException("customBuilder");
            
            customBuilder.CreateCustomAttribute((ModuleBuilder)m_module, MetadataTokenInternal);
            
            if (IsKnownCA(customBuilder.m_con))
                ParseCA(customBuilder.m_con, customBuilder.m_blob);
        }

        // this method should return true for any and every ca that requires more work
        // than just setting the ca
        private bool IsKnownCA(ConstructorInfo con)
        {
            Type caType = con.DeclaringType;
            if (caType == typeof(System.Runtime.CompilerServices.MethodImplAttribute)) return true;
            else if (caType == typeof(DllImportAttribute)) return true;
            else return false;
        }

        private void ParseCA(ConstructorInfo con, byte[] blob)
        {
            Type caType = con.DeclaringType;
            if (caType == typeof(System.Runtime.CompilerServices.MethodImplAttribute)) 
            {
                // dig through the blob looking for the MethodImplAttributes flag
                // that must be in the MethodCodeType field

                // for now we simply set a flag that relaxes the check when saving and
                // allows this method to have no body when any kind of MethodImplAttribute is present
                m_canBeRuntimeImpl = true;
            }
            else if (caType == typeof(DllImportAttribute)) {
                m_canBeRuntimeImpl = true;
                m_isDllImport = true;
            }
            
        }

        internal bool m_canBeRuntimeImpl = false;
        internal bool m_isDllImport = false;

        #endregion

        void _MethodBuilder.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _MethodBuilder.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _MethodBuilder.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _MethodBuilder.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }

    }

    internal class LocalSymInfo
    {
        // This class tracks the local variable's debugging information 
        // and namespace information with a given active lexical scope.

        #region Internal Data Members
        internal String[]       m_strName;
        internal byte[][]       m_ubSignature;
        internal int[]          m_iLocalSlot;
        internal int[]          m_iStartOffset;
        internal int[]          m_iEndOffset;
        internal int            m_iLocalSymCount;         // how many entries in the arrays are occupied
        internal String[]       m_namespace;
        internal int            m_iNameSpaceCount;
        internal const int      InitialSize = 16;
        #endregion

        #region Constructor
        internal LocalSymInfo()
        {
            // initialize data variables
            m_iLocalSymCount = 0;
            m_iNameSpaceCount = 0;
        }
        #endregion

        #region Private Members
        private void EnsureCapacityNamespace()
        {
            if (m_iNameSpaceCount == 0)
            {
                m_namespace = new String[InitialSize];
            }
            else if (m_iNameSpaceCount == m_namespace.Length)
            {
                String [] strTemp = new String [m_iNameSpaceCount * 2];
                Array.Copy(m_namespace, strTemp, m_iNameSpaceCount);
                m_namespace = strTemp;

            }
        }

        private void EnsureCapacity()
        {
            if (m_iLocalSymCount == 0)
            {
                // First time. Allocate the arrays.
                m_strName = new String[InitialSize];
                m_ubSignature = new byte[InitialSize][];
                m_iLocalSlot = new int[InitialSize];
                m_iStartOffset = new int[InitialSize];
                m_iEndOffset = new int[InitialSize];
            }
            else if (m_iLocalSymCount == m_strName.Length)
            {
                // the arrays are full. Enlarge the arrays
                int[] temp = new int [m_iLocalSymCount * 2];
                Array.Copy(m_iLocalSlot, temp, m_iLocalSymCount);
                m_iLocalSlot = temp;

                temp = new int [m_iLocalSymCount * 2];
                Array.Copy(m_iStartOffset, temp, m_iLocalSymCount);
                m_iStartOffset = temp;

                temp = new int [m_iLocalSymCount * 2];
                Array.Copy(m_iEndOffset, temp, m_iLocalSymCount);
                m_iEndOffset = temp;

                String [] strTemp = new String [m_iLocalSymCount * 2];
                Array.Copy(m_strName, strTemp, m_iLocalSymCount);
                m_strName = strTemp;

                byte[][] ubTemp = new byte[m_iLocalSymCount * 2][];
                Array.Copy(m_ubSignature, ubTemp, m_iLocalSymCount);
                m_ubSignature = ubTemp;

            }
        }

        #endregion

        #region Internal Members
        internal void AddLocalSymInfo(String strName,byte[] signature,int slot,int startOffset,int endOffset)
        {
            // make sure that arrays are large enough to hold addition info
            EnsureCapacity();
            m_iStartOffset[m_iLocalSymCount] = startOffset;
            m_iEndOffset[m_iLocalSymCount] = endOffset;
            m_iLocalSlot[m_iLocalSymCount] = slot;
            m_strName[m_iLocalSymCount] = strName;
            m_ubSignature[m_iLocalSymCount] = signature;
            m_iLocalSymCount++;
        }

        internal void AddUsingNamespace(String strNamespace)
        {
            EnsureCapacityNamespace();
            m_namespace[m_iNameSpaceCount++] = strNamespace;

        }

        internal virtual void EmitLocalSymInfo(ISymbolWriter symWriter)
        {
            int         i;

            for (i = 0; i < m_iLocalSymCount; i ++)
            {
                symWriter.DefineLocalVariable(
                            m_strName[i],
                            FieldAttributes.PrivateScope,  
                            m_ubSignature[i],
                            SymAddressKind.ILOffset,
                            m_iLocalSlot[i],
                            0,          // addr2 is not used yet
                            0,          // addr3 is not used
                            m_iStartOffset[i],
                            m_iEndOffset[i]);
            }
            for (i = 0; i < m_iNameSpaceCount; i ++)
            {
                symWriter.UsingNamespace(m_namespace[i]);
            }
        }

        #endregion
    }

    internal struct __ExceptionInstance 
    {
        #region Internal Members
        // Keep in sync with unmanged structure. 
        internal int m_exceptionClass;
        internal int m_startAddress;
        internal int m_endAddress;
        internal int m_filterAddress;
        internal int m_handleAddress;
        internal int m_handleEndAddress;
        internal int m_type;
        #endregion

        #region Constructor
        internal __ExceptionInstance(int start,int end,int filterAddr,int handle,int handleEnd,int type,int exceptionClass) 
        {
            m_startAddress=start;
            m_endAddress=end;
            m_filterAddress = filterAddr;
            m_handleAddress=handle;
            m_handleEndAddress=handleEnd;
            m_type=type;
            m_exceptionClass = exceptionClass;
        }
        #endregion

        #region Object Overrides
        public override bool Equals(Object obj)
        {
            if (obj!=null && (obj is __ExceptionInstance)) {
                __ExceptionInstance that = (__ExceptionInstance) obj;
                return that.m_exceptionClass == m_exceptionClass &&
                    that.m_startAddress == m_startAddress && that.m_endAddress == m_endAddress &&
                    that.m_filterAddress == m_filterAddress &&
                    that.m_handleAddress == m_handleAddress && that.m_handleEndAddress == m_handleEndAddress;
            }
            else
                return false;
        }
        
        public override int GetHashCode() 
        {
            return m_exceptionClass ^ m_startAddress ^ m_endAddress ^ m_filterAddress ^ m_handleAddress ^ m_handleEndAddress ^ m_type;
        }
    
        #endregion
    }
}










