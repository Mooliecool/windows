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

namespace System.Reflection.Emit {

    using System;
	using System.Globalization;
    using TextWriter = System.IO.TextWriter;
    using System.Diagnostics.SymbolStore;
    using System.Runtime.InteropServices;
    using System.Reflection;
    using System.Collections;
    using System.Security.Permissions;
    
    internal class DynamicILGenerator : ILGenerator {
    
        internal DynamicScope m_scope;
        private int m_methodSigToken;
      
        internal unsafe DynamicILGenerator(DynamicMethod method, byte[] methodSignature, int size) 
            : base(method, size) 
        {
            m_scope = new DynamicScope();
            m_methodSigToken = m_scope.GetTokenFor(methodSignature);
        }

         
        internal unsafe RuntimeMethodHandle GetCallableMethod(void* module) {
            return new RuntimeMethodHandle(ModuleHandle.GetDynamicMethod(
                                                module,
                                                m_methodBuilder.Name,
                                                (byte[])m_scope[m_methodSigToken], 
                                                new DynamicResolver(this)));
        }

        // *** ILGenerator api ***

        public override LocalBuilder DeclareLocal(Type localType, bool pinned) {
            LocalBuilder localBuilder;
            if (localType == null) 
                throw new ArgumentNullException("localType");
            if (localType.GetType() != typeof(RuntimeType)) 
                throw new ArgumentException(Environment.GetResourceString("Argument_MustBeRuntimeType"));
            localBuilder = new LocalBuilder(m_localCount, localType, m_methodBuilder);
            // add the localType to local signature
            m_localSignature.AddArgument(localType, pinned);
            m_localCount++;
            return localBuilder;
        }

        //
        //
        // Token resolution calls
        //
        //
        public override void Emit(OpCode opcode, MethodInfo meth) { 
            if (meth == null) 
                throw new ArgumentNullException("meth");

            int stackchange = 0;
            int tempVal = 0;
            DynamicMethod dynMeth = DynamicMethod.AsDynamicMethod(meth);
            if (dynMeth == null) {
                if (!(meth is RuntimeMethodInfo))
                    throw new ArgumentException(Environment.GetResourceString("Argument_MustBeRuntimeMethodInfo"), "meth");               

                if (meth.DeclaringType != null && (meth.DeclaringType.IsGenericType || meth.DeclaringType.IsArray)) 
                    tempVal = m_scope.GetTokenFor(meth.MethodHandle, meth.DeclaringType.TypeHandle);
                else
                    tempVal = m_scope.GetTokenFor(meth.MethodHandle);
            }
            else {
                // rule out not allowed operations on DynamicMethods
                if (opcode.Equals(OpCodes.Ldtoken) || opcode.Equals(OpCodes.Ldftn) || opcode.Equals(OpCodes.Ldvirtftn)) {
                    throw new ArgumentException(Environment.GetResourceString("Argument_InvalidOpCodeOnDynamicMethod"));
                }
                tempVal = m_scope.GetTokenFor(dynMeth);
            }

            EnsureCapacity(7);
            InternalEmit(opcode);

            if (opcode.m_push == StackBehaviour.Varpush 
                && meth.ReturnType != typeof(void)) {
                stackchange++;
            } 
            if (opcode.m_pop  == StackBehaviour.Varpop) { 
               stackchange -= meth.GetParametersNoCopy().Length;
            }
            // Pop the "this" parameter if the method is non-static,
            //  and the instruction is not newobj/ldtoken/ldftn.
            if (!meth.IsStatic &&
                !(opcode.Equals(OpCodes.Newobj) || opcode.Equals(OpCodes.Ldtoken) || opcode.Equals(OpCodes.Ldftn))) {
                stackchange--;
            }

            UpdateStackSize(opcode, stackchange);

            m_length=PutInteger4(tempVal, m_length, m_ILStream);
        }
        
        [System.Runtime.InteropServices.ComVisible(true)]
        public override void Emit(OpCode opcode, ConstructorInfo con) {
            if (con == null || !(con is RuntimeConstructorInfo)) 
                throw new ArgumentNullException("con");
            if (con.DeclaringType != null && con.DeclaringType.IsGenericType) 
                Emit(opcode, con.MethodHandle, con.DeclaringType.TypeHandle);
            else
                Emit(opcode, con.MethodHandle);
        }
        
        public void Emit(OpCode opcode, RuntimeMethodHandle meth) { 
            if (meth.IsNullHandle()) 
                throw new ArgumentNullException("meth");

            // need to sort out the stack size story
            //int   stackchange = 0;
            int tempVal = m_scope.GetTokenFor(meth);
            EnsureCapacity(7);
            InternalEmit(opcode);
            
            // need to sort out the stack size story
            /* 
            if (opcode.m_push == StackBehaviour.Varpush 
                && meth.ReturnType != typeof(void)) { stackchange++; } 
            if (opcode.m_pop  == StackBehaviour.Varpop) { 
               stackchange -= meth.GetParametersNoCopy().Length; }
            if (!meth.IsStatic && !(opcode.Equals(OpCodes.Newobj))) {stackchange--; }
            
            UpdateStackSize(opcode, stackchange);
            */
            // need to sort out the stack size story
            UpdateStackSize(opcode, 1);
            
            m_length=PutInteger4(tempVal, m_length, m_ILStream);
        }
        
        public void Emit(OpCode opcode, RuntimeMethodHandle meth, RuntimeTypeHandle typeContext) { 
            if (meth.IsNullHandle()) 
                throw new ArgumentNullException("meth");
            if (typeContext.IsNullHandle()) 
                throw new ArgumentNullException("typeContext");

            // need to sort out the stack size story
            //int   stackchange = 0;
            int tempVal = m_scope.GetTokenFor(meth, typeContext);
            EnsureCapacity(7);
            InternalEmit(opcode);
            
            // need to sort out the stack size story
            /* 
            if (opcode.m_push == StackBehaviour.Varpush 
                && meth.ReturnType != typeof(void)) { stackchange++; } 
            if (opcode.m_pop  == StackBehaviour.Varpop) { 
               stackchange -= meth.GetParametersNoCopy().Length; }
            if (!meth.IsStatic && !(opcode.Equals(OpCodes.Newobj))) {stackchange--; }
            
            UpdateStackSize(opcode, stackchange);
            */
            // need to sort out the stack size story
            UpdateStackSize(opcode, 1);
            
            m_length=PutInteger4(tempVal, m_length, m_ILStream);
        }
        
        public override void Emit(OpCode opcode, Type type) {   
            if (type == null) 
                throw new ArgumentNullException("type");
            Emit(opcode, type.TypeHandle);   
        }
        
        public void Emit(OpCode opcode, RuntimeTypeHandle typeHandle) {   
            if (typeHandle.IsNullHandle()) 
                throw new ArgumentNullException("typeHandle");
    
            int tempVal = m_scope.GetTokenFor(typeHandle);   
            EnsureCapacity(7);
            InternalEmit(opcode);
            m_length=PutInteger4(tempVal, m_length, m_ILStream);
        }
        
        public override void Emit(OpCode opcode, FieldInfo field) {
            if (field == null) 
                throw new ArgumentNullException("field");
            if (!(field is RuntimeFieldInfo)) 
                throw new ArgumentNullException("field");
            Emit(opcode, field.FieldHandle);
        }
        
        public void Emit(OpCode opcode, RuntimeFieldHandle fieldHandle) {
            if (fieldHandle.IsNullHandle()) 
                throw new ArgumentNullException("fieldHandle");
    
            int tempVal = m_scope.GetTokenFor(fieldHandle);
            EnsureCapacity(7);
            InternalEmit(opcode);
            m_length=PutInteger4(tempVal, m_length, m_ILStream);
        }
        
        public override void Emit(OpCode opcode, String str) {
            if (str == null) 
                throw new ArgumentNullException("str");
    
            int tempVal = AddStringLiteral(str);
            tempVal = tempVal | 0x70000000;
            EnsureCapacity(7);
            InternalEmit(opcode);
            m_length=PutInteger4(tempVal, m_length, m_ILStream);
        }
    
        //
        //
        // Signature related calls (vararg, calli)
        //
        //
        public override void EmitCalli(OpCode opcode,
                                       CallingConventions callingConvention,
                                       Type returnType,
                                       Type[] parameterTypes,
                                       Type[] optionalParameterTypes) {
            int stackchange = 0;
            SignatureHelper     sig;
            if (optionalParameterTypes != null)
                if ((callingConvention & CallingConventions.VarArgs) == 0)
                     
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NotAVarArgCallingConvention"));

            sig = GetMemberRefSignature(callingConvention,
                                        returnType,
                                        parameterTypes,
                                        optionalParameterTypes);

            EnsureCapacity(7);
            Emit(OpCodes.Calli);
            
            // The opcode passed in must be the calli instruction.
            BCLDebug.Assert(opcode.Equals(OpCodes.Calli),
                            "Unexpected opcode passed to EmitCalli.");
            // If there is a non-void return type, push one.
            if (returnType != typeof(void))
                stackchange++;
            // Pop off arguments if any.
            if (parameterTypes != null)
                stackchange -= parameterTypes.Length;
            // Pop off vararg arguments.
            if (optionalParameterTypes != null)
                stackchange -= optionalParameterTypes.Length;
            // Pop the this parameter if the method has a this parameter.
            if ((callingConvention & CallingConventions.HasThis) == CallingConventions.HasThis)
                stackchange--;
            // Pop the native function pointer.
            stackchange--;
            UpdateStackSize(opcode, stackchange);
            
            int token = AddSignature(sig.GetSignature(true));
            m_length=PutInteger4(token, m_length, m_ILStream);
        }

        public override void EmitCalli(OpCode opcode,
                                       CallingConvention unmanagedCallConv,
                                       Type returnType,
                                       Type[] parameterTypes) {
            int             stackchange = 0;
            int             cParams = 0;
            int             i;
            SignatureHelper sig;
            
            // The opcode passed in must be the calli instruction.
            BCLDebug.Assert(opcode.Equals(OpCodes.Calli),
                            "Unexpected opcode passed to EmitCalli.");
            if (parameterTypes != null)
                cParams = parameterTypes.Length;
            
            sig = SignatureHelper.GetMethodSigHelper(unmanagedCallConv, returnType);
                            
            if (parameterTypes != null)
                for (i = 0; i < cParams; i++) 
                    sig.AddArgument(parameterTypes[i]);
                                  
            // If there is a non-void return type, push one.
            if (returnType != typeof(void))
                stackchange++;
                
            // Pop off arguments if any.
            if (parameterTypes != null)
                stackchange -= cParams;
                
            // Pop the native function pointer.
            stackchange--;
            UpdateStackSize(opcode, stackchange);
            
            EnsureCapacity(7);
            Emit(OpCodes.Calli);
            int token = AddSignature(sig.GetSignature(true));
            m_length = PutInteger4(token, m_length, m_ILStream);
        }

        public override void EmitCall(OpCode opcode, MethodInfo methodInfo, Type[] optionalParameterTypes) {
            int         tk;
            int         stackchange = 0;

            if (methodInfo == null)
                throw new ArgumentNullException("methodInfo");

            if (methodInfo.ContainsGenericParameters)
                throw new ArgumentException(Environment.GetResourceString("Argument_GenericsInvalid"), "methodInfo");

            if (methodInfo.DeclaringType != null && methodInfo.DeclaringType.ContainsGenericParameters)
                throw new ArgumentException(Environment.GetResourceString("Argument_GenericsInvalid"), "methodInfo");

            tk = GetMemberRefToken(methodInfo, optionalParameterTypes);

            EnsureCapacity(7);
            InternalEmit(opcode);

            // The opcode must be one of call, callvirt, or newobj.
            BCLDebug.Assert(opcode.Equals(OpCodes.Call) ||
                            opcode.Equals(OpCodes.Callvirt) ||
                            opcode.Equals(OpCodes.Newobj),
                            "Unexpected opcode passed to EmitCall.");
            
            // Push the return value if there is one.
            if (methodInfo.ReturnType != typeof(void))
                stackchange++;
            // Pop the parameters.
            stackchange -= methodInfo.GetParameterTypes().Length;
            // Pop the this parameter if the method is non-static and the
            // instruction is not newobj.
            if (!(methodInfo is SymbolMethod) && methodInfo.IsStatic == false && !(opcode.Equals(OpCodes.Newobj)))
                stackchange--;
            // Pop the optional parameters off the stack.
            if (optionalParameterTypes != null)
                stackchange -= optionalParameterTypes.Length;
            UpdateStackSize(opcode, stackchange);

            m_length = PutInteger4(tk, m_length, m_ILStream);
        }

        public override void Emit(OpCode opcode, SignatureHelper signature) {
            int stackchange = 0;
            if (signature == null)
                throw new ArgumentNullException("signature");

            EnsureCapacity(7);
            InternalEmit(opcode);

            // The only IL instruction that has VarPop behaviour, that takes a
            // Signature token as a parameter is calli.  Pop the parameters and
            // the native function pointer.  To be conservative, do not pop the
            // this pointer since this information is not easily derived from
            // SignatureHelper.
            if (opcode.m_pop == StackBehaviour.Varpop) {
                BCLDebug.Assert(opcode.Equals(OpCodes.Calli),
                                "Unexpected opcode encountered for StackBehaviour VarPop.");
                // Pop the arguments..
                stackchange -= signature.ArgumentCount;
                // Pop native function pointer off the stack.
                stackchange--;
                UpdateStackSize(opcode, stackchange);
            }

            int token = AddSignature(signature.GetSignature(true));;
            m_length = PutInteger4(token, m_length, m_ILStream);
        }

        //
        //
        // Exception related generation
        //
        //
        public override Label BeginExceptionBlock() {
            return base.BeginExceptionBlock();
        }

        public override void EndExceptionBlock() {
            base.EndExceptionBlock();
        }

        public override void BeginExceptFilterBlock() {
            throw new NotSupportedException(Environment.GetResourceString("InvalidOperation_NotAllowedInDynamicMethod"));
        }

        public override void BeginCatchBlock(Type exceptionType) {
            if (m_currExcStackCount==0)
                throw new NotSupportedException(Environment.GetResourceString("Argument_NotInExceptionBlock"));
            
            __ExceptionInfo current = m_currExcStack[m_currExcStackCount-1];

            if (current.GetCurrentState() == __ExceptionInfo.State_Filter) {
                if (exceptionType != null) {
                    throw new ArgumentException(Environment.GetResourceString("Argument_ShouldNotSpecifyExceptionType"));
                }

                this.Emit(OpCodes.Endfilter);
            } else {
                // execute this branch if previous clause is Catch or Fault
                if (exceptionType==null) {
                    throw new ArgumentNullException("exceptionType");
                }

                if (exceptionType.GetType() != typeof(RuntimeType)) 
                    throw new ArgumentException(Environment.GetResourceString("Argument_MustBeRuntimeType"));

                Label endLabel = current.GetEndLabel();
                this.Emit(OpCodes.Leave, endLabel);

                // if this is a catch block the exception will be pushed on the stack and we need to update the stack info
                UpdateStackSize(OpCodes.Nop, 1);
            }

            current.MarkCatchAddr(m_length, exceptionType);

            
            // this is relying on too much implementation details of the base and so it's highly breaking
            // Need to have a more integreted story for exceptions
            current.m_filterAddr[current.m_currentCatch-1] = m_scope.GetTokenFor(exceptionType.TypeHandle);
        }

        public override void BeginFaultBlock() {
            throw new NotSupportedException(Environment.GetResourceString("InvalidOperation_NotAllowedInDynamicMethod"));
        }

        public override void BeginFinallyBlock() {
            base.BeginFinallyBlock();
        }

        //
        //
        // debugger related calls. 
        //
        //
        public override void UsingNamespace(String ns) {
            throw new NotSupportedException(Environment.GetResourceString("InvalidOperation_NotAllowedInDynamicMethod"));
        }

        public override void MarkSequencePoint(ISymbolDocumentWriter document,
                                               int startLine,
                                               int startColumn,
                                               int endLine,
                                               int endColumn) {
            throw new NotSupportedException(Environment.GetResourceString("InvalidOperation_NotAllowedInDynamicMethod"));
        }

        public override void BeginScope() {
            throw new NotSupportedException(Environment.GetResourceString("InvalidOperation_NotAllowedInDynamicMethod"));
        }

        public override void EndScope() {
            throw new NotSupportedException(Environment.GetResourceString("InvalidOperation_NotAllowedInDynamicMethod"));
        }

        //
        // private implementation
        //
        internal override int GetMaxStackSize() {
           return m_maxStackSize;
        }

        internal override int GetMemberRefToken(MethodBase methodInfo, Type[] optionalParameterTypes) {
            Type[] parameterTypes;

            if (optionalParameterTypes != null)
                if ((methodInfo.CallingConvention & CallingConventions.VarArgs) == 0)
                     
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NotAVarArgCallingConvention"));
                
            if (!(methodInfo is RuntimeMethodInfo) && DynamicMethod.AsDynamicMethod(methodInfo) == null)
                throw new ArgumentException(Environment.GetResourceString("Argument_MustBeRuntimeMethodInfo"), "methodInfo");               

            ParameterInfo[] paramInfo = methodInfo.GetParametersNoCopy();
            if (paramInfo != null && paramInfo.Length != 0) {
                parameterTypes = new Type[paramInfo.Length];
                for (int i = 0; i < paramInfo.Length; i++) {
                    parameterTypes[i] = paramInfo[i].ParameterType;
                }
            }
            else
                parameterTypes = null;

            SignatureHelper sig = GetMemberRefSignature(methodInfo.CallingConvention,
                                                     methodInfo.GetReturnType(),
                                                     parameterTypes,
                                                     optionalParameterTypes);
            return m_scope.GetTokenFor(new VarArgMethod(methodInfo as MethodInfo, sig));
        }

        internal override SignatureHelper GetMemberRefSignature(
                                                CallingConventions  call,
                                                Type                returnType,
                                                Type[]              parameterTypes,
                                                Type[]              optionalParameterTypes) {
            int             cParams;
            int             i;
            SignatureHelper sig;
            if (parameterTypes == null)
                cParams = 0;
            else
                cParams = parameterTypes.Length;
            sig = SignatureHelper.GetMethodSigHelper(call, returnType);
            for (i=0; i < cParams; i++)
                sig.AddArgument(parameterTypes[i]);
            if (optionalParameterTypes != null && optionalParameterTypes.Length != 0) {
                // add the sentinel
                sig.AddSentinel();
                for (i=0; i < optionalParameterTypes.Length; i++)
                    sig.AddArgument(optionalParameterTypes[i]);
            }
            return sig;
        }

        private int AddStringLiteral(String s) {
            int token = m_scope.GetTokenFor(s);
            return token | 0x70000000;
        }

        private int AddSignature(byte[] sig) {
            int token = m_scope.GetTokenFor(sig);
            return token | 0x11000000;
        }

    }

    internal class DynamicResolver : Resolver 
    {
        #region Private Data Members
        private __ExceptionInfo[] m_exceptions;
        private byte[] m_exceptionHeader;
        private DynamicMethod m_method;
        private byte[] m_code;
        private byte[] m_localSignature;
        private int m_stackSize;
        private DynamicScope m_scope;
        #endregion

        #region Internal Methods
        internal DynamicResolver(DynamicILGenerator ilGenerator) {
            m_stackSize = ilGenerator.GetMaxStackSize();
            m_exceptions = ilGenerator.GetExceptions();
            m_code = ilGenerator.BakeByteArray();
            m_localSignature = ilGenerator.m_localSignature.InternalGetSignatureArray(); 
            m_scope = ilGenerator.m_scope;
            
            m_method = (DynamicMethod)ilGenerator.m_methodBuilder;
            m_method.m_resolver = this;
        }

        internal DynamicResolver(DynamicILInfo dynamicILInfo)
        {
            m_stackSize = dynamicILInfo.MaxStackSize;
            m_code = dynamicILInfo.Code;
            m_localSignature = dynamicILInfo.LocalSignature; 
            m_exceptionHeader = dynamicILInfo.Exceptions;
            //m_exceptions = dynamicILInfo.Exceptions;
            m_scope = dynamicILInfo.DynamicScope;
            
            m_method = dynamicILInfo.DynamicMethod;
            m_method.m_resolver = this;
        }

        //
        //
        ~DynamicResolver() {
            DynamicMethod method = m_method;

            if (method == null)
                return;

            if (method.m_method.IsNullHandle())
                return;

            DestroyScout scout = null;
            try {
                scout = new DestroyScout();
            } catch {
                // We go over all DynamicMethodDesc during AppDomain shutdown and make sure
                // that everything associated with them is released. So it is ok to skip reregistration
                // for finalization during appdomain shutdown
                if (!Environment.HasShutdownStarted && 
                    !AppDomain.CurrentDomain.IsFinalizingForUnload())
                {
                    // Try again later.
                    GC.ReRegisterForFinalize(this);
                }
                return;
            }

            // We can never ever have two active destroy scouts for the same method. We need to initialize the scout
            // outside the try/reregister block to avoid possibility of reregistration for finalization with active scout.
            scout.m_method = method.m_method;
        }

        private class DestroyScout
        {
            internal RuntimeMethodHandle m_method;

            ~DestroyScout()
             {
                if (m_method.IsNullHandle())
                    return;

                // It is not safe to destroy the method if the managed resolver is alive.
                if (m_method.GetResolver() != null)
                {
                    if (!Environment.HasShutdownStarted &&
                        !AppDomain.CurrentDomain.IsFinalizingForUnload())
                    {
                        // Somebody might have been holding a reference on us via weak handle.
                        // We will keep trying. It will be hopefully released eventually.
                        GC.ReRegisterForFinalize(this);
                    }
                    return;
                }

                m_method.Destroy();
            }
        }

        // Keep in sync with vm/dynamicmethod.h
        internal enum SecurityControlFlags
        {
            SkipVisibilityChecks = 0x1
        }
        
        internal override void GetJitContext(ref int securityControlFlags, ref RuntimeTypeHandle typeOwner)
        {
            securityControlFlags = (m_method.m_skipVisibility) ? (int) SecurityControlFlags.SkipVisibilityChecks:0;
            typeOwner = (m_method.m_typeOwner != null) ? m_method.m_typeOwner.TypeHandle : RuntimeTypeHandle.EmptyHandle;
        }

        internal override byte[] GetCodeInfo(
            ref int stackSize, ref int initLocals, ref int EHCount) 
        {
            stackSize = m_stackSize;
            if (m_exceptionHeader != null && m_exceptionHeader.Length != 0)
            {
                if (m_exceptionHeader.Length < 4)
                    throw new FormatException();

                byte header = m_exceptionHeader[0];                

                if ((header & 0x40) != 0) // Fat
                {
                    byte[] size = new byte[4];
                    for (int q = 0; q < 3; q++)
                        size[q] = m_exceptionHeader[q + 1];
                    EHCount = (BitConverter.ToInt32(size, 0) - 4) / 24;
                }
                else
                    EHCount = (m_exceptionHeader[1] - 2) / 12;
            }
            else
            {
                EHCount = ILGenerator.CalculateNumberOfExceptions(m_exceptions);
            }
            initLocals = (m_method.InitLocals) ? 1 : 0;
            return m_code;
        }

        internal override byte[] GetLocalsSignature() 
        {
            return m_localSignature; 
        }

        internal override unsafe byte[] GetRawEHInfo()
        {
            return m_exceptionHeader;
        }

        internal override unsafe void GetEHInfo(int excNumber, void* exc)
        {
            CORINFO_EH_CLAUSE* exception = (CORINFO_EH_CLAUSE*)exc;
            for (int i = 0; i < m_exceptions.Length; i++) {
                int excCount = m_exceptions[i].GetNumberOfCatches();
                if (excNumber < excCount) {
                    // found the right exception block
                    exception->Flags = m_exceptions[i].GetExceptionTypes()[excNumber];
                    exception->Flags |= Resolver.COR_ILEXCEPTION_CLAUSE_MUST_CACHE_CLASS;
                    exception->TryOffset = m_exceptions[i].GetStartAddress();
                    if ((exception->Flags & __ExceptionInfo.Finally) != __ExceptionInfo.Finally) 
                        exception->TryLength = m_exceptions[i].GetEndAddress() - exception->TryOffset;
                    else
                        exception->TryLength = m_exceptions[i].GetFinallyEndAddress() - exception->TryOffset;
                    exception->HandlerOffset = m_exceptions[i].GetCatchAddresses()[excNumber];
                    exception->HandlerLength = m_exceptions[i].GetCatchEndAddresses()[excNumber] - exception->HandlerOffset;
                    // this is cheating because the filter address is the token of the class only for light code gen
                    exception->ClassTokenOrFilterOffset = m_exceptions[i].GetFilterAddresses()[excNumber];
                    break;
                }
                excNumber -= excCount;
            }
        }

        internal override String GetStringLiteral(int token) { return m_scope.GetString(token); }

        internal override int IsValidToken(int token)
        {
            return (m_scope[token] != null) ? 1 : 0;
        }

        internal override unsafe void* GetInstantiationInfo(int token)
        {
            GenericMethodInfo gmi = m_scope[token] as GenericMethodInfo;
            if (gmi != null)
            {
                return (void*)gmi.m_context.Value;
            }
            return null;
        }

        internal override unsafe void* ResolveToken(int token)
        {
            Object handle = m_scope[token];

            if (handle is RuntimeTypeHandle) 
            {
                RuntimeTypeHandle typeHandle = (RuntimeTypeHandle)handle;
                return (void*)typeHandle.Value;
            }
            else if (handle is RuntimeMethodHandle) 
            {
                RuntimeMethodHandle methodHandle = (RuntimeMethodHandle)handle;
                return (void*)methodHandle.Value;
            }
            else if (handle is RuntimeFieldHandle) 
            {
                RuntimeFieldHandle fieldHandle = (RuntimeFieldHandle)handle;
                return (void*)fieldHandle.Value;
            }
            else if (handle is DynamicMethod) 
            {
                DynamicMethod dynMeth = (DynamicMethod)handle;
                RuntimeMethodHandle methodHandle = dynMeth.GetMethodDescriptor();
                return (void*)methodHandle.Value;
            }
            else if (handle is GenericMethodInfo) 
            {
                GenericMethodInfo gmi = (GenericMethodInfo)handle;
                return (void*)gmi.m_method.Value;
            }
            else if (handle is VarArgMethod) 
            {
                VarArgMethod vaMeth = (VarArgMethod)handle;
                DynamicMethod dm = vaMeth.m_method as DynamicMethod;
                if (dm == null) 
                    return (void*)vaMeth.m_method.MethodHandle.Value;
                else
                    return (void*)dm.GetMethodDescriptor().Value;
            }

            return null;
        }

        internal override byte[] ResolveSignature(int token, int fromMethod)
        {                
            return m_scope.ResolveSignature(token, fromMethod);
        }
        
        internal override unsafe int ParentToken(int token)
        {
            RuntimeTypeHandle owner = RuntimeTypeHandle.EmptyHandle;
            
            Object handle = m_scope[token];

            if (handle is RuntimeMethodHandle) 
            {
                RuntimeMethodHandle method = (RuntimeMethodHandle)handle;
                owner = method.GetDeclaringType();
            }
            else if (handle is RuntimeFieldHandle) 
            {
                RuntimeFieldHandle field = (RuntimeFieldHandle)handle;
                owner = field.GetApproxDeclaringType();
            }
            else if (handle is DynamicMethod) 
            {
                DynamicMethod dynMeth = (DynamicMethod)handle;
                owner = dynMeth.m_method.GetDeclaringType();
            }
            else if (handle is GenericMethodInfo) 
            {
                GenericMethodInfo gmi = (GenericMethodInfo)handle;
                owner = gmi.m_context;
            }
            else if (handle is VarArgMethod) 
            {
                VarArgMethod varargMeth = (VarArgMethod)handle;
                if (varargMeth.m_method is DynamicMethod)
                    owner = ((DynamicMethod)varargMeth.m_method).m_method.GetDeclaringType();
                else if (varargMeth.m_method is DynamicMethod.RTDynamicMethod) 
                    owner = ((DynamicMethod.RTDynamicMethod)varargMeth.m_method).m_owner.m_method.GetDeclaringType();
                else
                    owner = varargMeth.m_method.MethodHandle.GetDeclaringType();
            }

            if (owner.IsNullHandle()) 
                return -1;

            return m_scope.GetTokenFor(owner);
        }

        internal override MethodInfo GetDynamicMethod() 
        {
            return m_method.GetMethodInfo();
        }
        #endregion
    }

[System.Runtime.InteropServices.ComVisible(true)]
    public class DynamicILInfo
    {
        #region Private Data Members
        private DynamicMethod m_method;
        private DynamicScope m_scope;
        private byte[] m_exceptions;
        private byte[] m_code;
        private byte[] m_localSignature;
        private int m_maxStackSize;
        private int m_methodSignature;
        #endregion

        #region Constructor
        internal DynamicILInfo(DynamicScope scope, DynamicMethod method, byte[] methodSignature)
        {
            m_method = method;
            m_scope = scope;
            m_methodSignature = m_scope.GetTokenFor(methodSignature);
            m_exceptions = new byte[0];
            m_code = new byte[0];
            m_localSignature = new byte[0];
        }
        #endregion

        #region Internal Methods
        internal unsafe RuntimeMethodHandle GetCallableMethod(void* module)
        {
            return new RuntimeMethodHandle(ModuleHandle.GetDynamicMethod(
                module, m_method.Name, (byte[])m_scope[m_methodSignature], new DynamicResolver(this)));
        }
        internal byte[] LocalSignature 
        { 
            get 
            { 
                if (m_localSignature == null)
                    m_localSignature = SignatureHelper.GetLocalVarSigHelper().InternalGetSignatureArray();

                return m_localSignature; 
            } 
        }
        internal byte[] Exceptions { get { return m_exceptions; } }
        internal byte[] Code { get { return m_code; } }
        internal int MaxStackSize { get { return m_maxStackSize; } }
		#endregion

        #region Public ILGenerator Methods
        public DynamicMethod DynamicMethod { get { return m_method; } }
        internal DynamicScope DynamicScope { get { return m_scope; } }

        public void SetCode(byte[] code, int maxStackSize)
        {
            if (code == null)
                code = new byte[0];

            m_code = (byte[])code.Clone();
            m_maxStackSize = maxStackSize;
        }
        
        [CLSCompliant(false)]
        public unsafe void SetCode(byte* code, int codeSize, int maxStackSize)
        {
            if (codeSize < 0)
                throw new ArgumentOutOfRangeException("codeSize", Environment.GetResourceString("ArgumentOutOfRange_GenericPositive"));

            m_code = new byte[codeSize];
            for (int i = 0; i < codeSize; i++)
            {
                m_code[i] = *code;
                code++;
            }

            m_maxStackSize = maxStackSize;
        }
        
        public void SetExceptions(byte[] exceptions)
        {
            if (exceptions == null)
                exceptions = new byte[0];

            m_exceptions = (byte[])exceptions.Clone();
        }
        
        [CLSCompliant(false)]
        public unsafe void SetExceptions(byte* exceptions, int exceptionsSize)
        {
            if (exceptionsSize < 0)
                throw new ArgumentOutOfRangeException("exceptionsSize", Environment.GetResourceString("ArgumentOutOfRange_GenericPositive"));

            m_exceptions = new byte[exceptionsSize];
            
            for (int i = 0; i < exceptionsSize; i++)
            {
                m_exceptions[i] = *exceptions;
                exceptions++;
            }
        }
        
        public void SetLocalSignature(byte[] localSignature)
        {
            if (localSignature == null)
                localSignature = new byte[0];
                
            m_localSignature = (byte[])localSignature.Clone();
        }
        
        [CLSCompliant(false)]
        public unsafe void SetLocalSignature(byte* localSignature, int signatureSize)
        {
            if (signatureSize < 0)
                throw new ArgumentOutOfRangeException("signatureSize", Environment.GetResourceString("ArgumentOutOfRange_GenericPositive"));

            m_localSignature = new byte[signatureSize];
            for (int i = 0; i < signatureSize; i++)
            {
                m_localSignature[i] = *localSignature;
                localSignature++;
            }
        }
        #endregion

        #region Public Scope Methods
        public int GetTokenFor(RuntimeMethodHandle method) { return DynamicScope.GetTokenFor(method); }
        public int GetTokenFor(DynamicMethod method) { return DynamicScope.GetTokenFor(method); }
        public int GetTokenFor(RuntimeMethodHandle method, RuntimeTypeHandle contextType) { return DynamicScope.GetTokenFor(method, contextType); }
        public int GetTokenFor(RuntimeFieldHandle field) { return DynamicScope.GetTokenFor(field); }
        public int GetTokenFor(RuntimeTypeHandle type) { return DynamicScope.GetTokenFor(type); }
        public int GetTokenFor(string literal) { return DynamicScope.GetTokenFor(literal); }
        public int GetTokenFor(byte[] signature) { return DynamicScope.GetTokenFor(signature); }
        #endregion
    }

    internal class DynamicScope 
    {   
        #region Private Data Members
        internal ArrayList m_tokens;
        #endregion
          
        #region Constructor
        internal unsafe DynamicScope() 
        {
            m_tokens = new ArrayList();
            m_tokens.Add(null);
        }
        #endregion

        #region Internal Methods
        internal object this[int token] 
        { 
            get 
            { 
                token &= 0x00FFFFFF;

                if (token < 0 || token > m_tokens.Count)
                    return null;

                return m_tokens[token]; 
            } 
        }

        internal int GetTokenFor(VarArgMethod varArgMethod) { return m_tokens.Add(varArgMethod) | (int)MetadataTokenType.MemberRef; }
        internal string GetString(int token) { return this[token] as string; }

        internal byte[] ResolveSignature(int token, int fromMethod)
        {
            if (fromMethod == 0)
                return (byte[])this[token];

            VarArgMethod vaMethod = this[token] as VarArgMethod;

            if (vaMethod == null)
                return null;

            return vaMethod.m_signature.GetSignature(true);
        }
        #endregion

        #region Public Methods
        public int GetTokenFor(RuntimeMethodHandle method) 
        { 
            MethodBase m = RuntimeType.GetMethodBase(method);

            if (m.DeclaringType != null && m.DeclaringType.IsGenericType)
                throw new ArgumentException(String.Format(
                    CultureInfo.CurrentCulture, Environment.GetResourceString("Argument_MethodDeclaringTypeGenericLcg"), 
                    m, m.DeclaringType.GetGenericTypeDefinition()));
 
        	return m_tokens.Add(method) | (int)MetadataTokenType.MethodDef; 
		}
        public int GetTokenFor(RuntimeMethodHandle method, RuntimeTypeHandle typeContext) { return m_tokens.Add(new GenericMethodInfo(method, typeContext)) | (int)MetadataTokenType.MethodDef; }
        public int GetTokenFor(DynamicMethod method) { return m_tokens.Add(method) | (int)MetadataTokenType.MethodDef; }
        public int GetTokenFor(RuntimeFieldHandle field) { return m_tokens.Add(field) | (int)MetadataTokenType.FieldDef; }
        public int GetTokenFor(RuntimeTypeHandle type) { return m_tokens.Add(type) | (int)MetadataTokenType.TypeDef; }
        public int GetTokenFor(string literal) { return m_tokens.Add(literal) | (int)MetadataTokenType.String; }
        public int GetTokenFor(byte[] signature) { return m_tokens.Add(signature) | (int)MetadataTokenType.Signature; }
        #endregion
    }

    internal class GenericMethodInfo
    {
        internal RuntimeMethodHandle m_method;
        internal RuntimeTypeHandle m_context;

        internal GenericMethodInfo(RuntimeMethodHandle method, RuntimeTypeHandle context) 
        {
            m_method = method;
            m_context = context;
        }
    }

    internal class VarArgMethod
    {
        internal MethodInfo m_method;
        internal SignatureHelper m_signature;

        internal VarArgMethod(MethodInfo method, SignatureHelper signature) 
        {
            m_method = method;
            m_signature = signature;
        }
    }

}



