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
    using TextWriter = System.IO.TextWriter;
    using System.Diagnostics.SymbolStore;
    using System.Runtime.InteropServices;
    using System.Reflection;
    using System.Security.Permissions;
    using System.Globalization;
    
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_ILGenerator))]
    [System.Runtime.InteropServices.ComVisible(true)]
    public class ILGenerator : _ILGenerator
    {
        #region Const Members
        internal const byte PrefixInstruction = (byte)0xFF;
        internal const int defaultSize=16;
        internal const int DefaultFixupArraySize = 64;
        internal const int DefaultLabelArraySize = 16;
        internal const int DefaultExceptionArraySize = 8;
        #endregion

        #region Internal Statics
        internal static int[] EnlargeArray(int[] incoming)
        {
            int[] temp = new int [incoming.Length*2];
            Array.Copy(incoming, temp, incoming.Length);
            return temp;
        }

        internal static byte[] EnlargeArray(byte[] incoming)
        {
            byte [] temp = new byte [incoming.Length*2];
            Array.Copy(incoming, temp, incoming.Length);
            return temp;
        }

        internal static byte[] EnlargeArray(byte[] incoming, int requiredSize)
        {
            byte [] temp = new byte [requiredSize];
            Array.Copy(incoming, temp, incoming.Length);
            return temp;
        }

        internal static __FixupData[] EnlargeArray(__FixupData[] incoming)
        {
            __FixupData [] temp = new __FixupData[incoming.Length*2];
            //Does arraycopy work for value classes?
            Array.Copy(incoming, temp, incoming.Length);
            return temp;
        }

        internal static Type[] EnlargeArray(Type[] incoming)
        {
            Type[] temp = new Type[incoming.Length*2];
            Array.Copy(incoming, temp, incoming.Length);
            return temp;
        }

        internal static __ExceptionInfo[] EnlargeArray(__ExceptionInfo[] incoming)
        {
            __ExceptionInfo[] temp = new __ExceptionInfo[incoming.Length*2];
            Array.Copy(incoming, temp, incoming.Length);
            return temp;
        }
        
        static internal int CalculateNumberOfExceptions(__ExceptionInfo [] excp) 
        {
            int num=0;
            if (excp==null) 
                return 0;
            for (int i=0; i<excp.Length; i++) 
                num+=excp[i].GetNumberOfCatches();
            return num;
        }

        #endregion

        #region Internal Data Members
        internal int        m_length;
        internal byte[]     m_ILStream;

        internal int[]      m_labelList;
        internal int        m_labelCount;

        internal __FixupData[] m_fixupData;
        //internal Label[]    m_fixupLabel;
        //internal int[]      m_fixupPos;

       
         
         
        internal int        m_fixupCount;

        internal int[]      m_RVAFixupList;
        internal int        m_RVAFixupCount;

        internal int[]      m_RelocFixupList;
        internal int        m_RelocFixupCount;

        internal int        m_exceptionCount;
        internal int        m_currExcStackCount;
        internal __ExceptionInfo[] m_exceptions;        //This is the list of all of the exceptions in this ILStream.
        internal __ExceptionInfo[] m_currExcStack;      //This is the stack of exceptions which we're currently in.

        internal ScopeTree  m_ScopeTree;                // this variable tracks all debugging scope information
        internal LineNumberInfo m_LineNumberInfo;       // this variable tracks all line number information

        internal MethodInfo m_methodBuilder;
        internal int        m_localCount;
        internal SignatureHelper m_localSignature;

        internal int m_maxStackSize = 0;                // Maximum stack size not counting the exceptions.

        internal int m_maxMidStack = 0;                 // Maximum stack size for a given basic block.
        internal int m_maxMidStackCur = 0;              // Running count of the maximum stack size for the current basic block.
        #endregion

        #region Constructor
        // package private constructor. This code path is used when client create
        // ILGenerator through MethodBuilder.
        internal ILGenerator(MethodInfo methodBuilder) : this(methodBuilder, 64)
        {
        }

        internal ILGenerator(MethodInfo methodBuilder, int size)
        {
            if (size < defaultSize)
            {
                m_ILStream = new byte[defaultSize];
            }
            else
            {
                m_ILStream = new byte[size];
            }

            m_length = 0;

            m_labelCount = 0;
            m_fixupCount = 0;
            m_labelList = null;

            m_fixupData = null;

            m_exceptions = null; 
            m_exceptionCount = 0;
            m_currExcStack = null; 
            m_currExcStackCount = 0;

            m_RelocFixupList = new int[DefaultFixupArraySize];
            m_RelocFixupCount = 0;
            m_RVAFixupList = new int[DefaultFixupArraySize];
            m_RVAFixupCount = 0;

            // initialize the scope tree
            m_ScopeTree = new ScopeTree();
            m_LineNumberInfo = new LineNumberInfo();
            m_methodBuilder = methodBuilder;

            // initialize local signature
            m_localCount = 0;
            MethodBuilder mb = m_methodBuilder as MethodBuilder;
            if (mb == null) 
                m_localSignature = SignatureHelper.GetLocalVarSigHelper(null);
            else
                m_localSignature = SignatureHelper.GetLocalVarSigHelper(mb.GetTypeBuilder().Module);
        }

        internal ILGenerator(int size)
        {
            if (size < defaultSize)
                m_ILStream = new byte[defaultSize];
            else
                m_ILStream = new byte[size];
            m_length = 0;

            m_labelCount = 0;
            m_fixupCount = 0;
            m_labelList = null;

            m_fixupData = null;

            m_exceptions = null; 
            m_exceptionCount = 0;
            m_currExcStack = null; 
            m_currExcStackCount = 0;

            m_RelocFixupList = new int[DefaultFixupArraySize];
            m_RelocFixupCount = 0;
            m_RVAFixupList = new int[DefaultFixupArraySize];
            m_RVAFixupCount = 0;

            // initialize the scope tree
            m_ScopeTree = new ScopeTree();
            m_LineNumberInfo = new LineNumberInfo();
            m_methodBuilder = null;

            // initialize local signature
            m_localCount = 0;
            m_localSignature = SignatureHelper.GetLocalVarSigHelper(null);
        }

        #endregion

        #region Private Members
        private void RecordTokenFixup()
        {
            if (m_RelocFixupCount >= m_RelocFixupList.Length)
                m_RelocFixupList = EnlargeArray(m_RelocFixupList);

            m_RelocFixupList[m_RelocFixupCount++] = m_length;
        }

        #endregion

        #region Internal Members
        internal void InternalEmit(OpCode opcode)
        {
            if (opcode.m_size == 1)
            {
                m_ILStream[m_length++] = opcode.m_s2;
            } 
            else
            {
                m_ILStream[m_length++] = opcode.m_s1;
                m_ILStream[m_length++] = opcode.m_s2;
            }

            UpdateStackSize(opcode, opcode.StackChange());

        }

        internal void UpdateStackSize(OpCode opcode, int stackchange)
        {
            // Updates internal variables for keeping track of the stack size
            // requirements for the function.  stackchange specifies the amount
            // by which the stacksize needs to be updated.

            // Special case for the Return.  Returns pops 1 if there is a
            // non-void return value.

             
             
              
             
             
               
               

            // Update the running stacksize.  m_maxMidStack specifies the maximum
            // amount of stack required for the current basic block irrespective of
            // where you enter the block.
            m_maxMidStackCur += stackchange;
            if (m_maxMidStackCur > m_maxMidStack)
                m_maxMidStack = m_maxMidStackCur;
            else if (m_maxMidStackCur < 0)
                m_maxMidStackCur = 0;

            // If the current instruction signifies end of a basic, which basically
            // means an unconditional branch, add m_maxMidStack to m_maxStackSize.
            // m_maxStackSize will eventually be the sum of the stack requirements for
            // each basic block.
            if (opcode.EndsUncondJmpBlk())
            {
                m_maxStackSize += m_maxMidStack;
                m_maxMidStack = 0;
                m_maxMidStackCur = 0;
            }
        }

        internal virtual int GetMethodToken(MethodBase method, Type[] optionalParameterTypes)
        {
            int tk = 0;
            ModuleBuilder modBuilder = (ModuleBuilder) m_methodBuilder.Module;

            if (method.IsGenericMethod)
            {
                // Given M<Bar> unbind to M<S>
                MethodInfo methodInfoUnbound = method as MethodInfo;
                
                if (!method.IsGenericMethodDefinition && method is MethodInfo)
                    methodInfoUnbound = ((MethodInfo)method).GetGenericMethodDefinition();

                if (methodInfoUnbound.Module != m_methodBuilder.Module 
                    || (methodInfoUnbound.DeclaringType != null && methodInfoUnbound.DeclaringType.IsGenericType))
                    tk = GetMemberRefToken(methodInfoUnbound, null);
                else
                    tk = modBuilder.GetMethodToken(methodInfoUnbound).Token;
            
                // Create signature of method instantiation M<Bar>
                int sigLength;
                byte[] sigBytes = SignatureHelper.GetMethodSpecSigHelper(
                    modBuilder, method.GetGenericArguments()).InternalGetSignature(out sigLength);
            
                // Create MethodSepc M<Bar> with parent G?.M<S> 
                tk = TypeBuilder.InternalDefineMethodSpec(tk, sigBytes, sigLength, modBuilder);
            }            
            else 
            {
                if (((method.CallingConvention & CallingConventions.VarArgs) == 0) && 
                    (method.DeclaringType == null || !method.DeclaringType.IsGenericType))
                {
                    if (method is MethodInfo)
                        tk = modBuilder.GetMethodToken(method as MethodInfo).Token;
                    else
                        tk = modBuilder.GetConstructorToken(method as ConstructorInfo).Token;
                }
                else
                {
                    tk = GetMemberRefToken(method, optionalParameterTypes);
                }
            }

            return tk;
        }

        internal virtual int GetMemberRefToken(MethodBase method, Type[] optionalParameterTypes)
        {
            Type[] parameterTypes;
            Type returnType;
            int tkParent;
            ModuleBuilder modBuilder = (ModuleBuilder)m_methodBuilder.Module;
            int cGenericParameters = 0;

            if (method.IsGenericMethod)
            {
                if (!method.IsGenericMethodDefinition)
                    throw new InvalidOperationException();
            
                cGenericParameters = method.GetGenericArguments().Length;
            }

            if (optionalParameterTypes != null)
            {
                if ((method.CallingConvention & CallingConventions.VarArgs) == 0)
                {
                    // Client should not supply optional parameter in default calling convention
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NotAVarArgCallingConvention"));
                }
            }

            if (method.DeclaringType.IsGenericType)
            {
                MethodBase methDef = null; // methodInfo = G<Foo>.M<Bar> ==> methDef = G<T>.M<S>
                
                if (method is MethodOnTypeBuilderInstantiation)
                {
                    methDef = (method as MethodOnTypeBuilderInstantiation).m_method;
                }
                else if (method is ConstructorOnTypeBuilderInstantiation)
                {
                    methDef = (method as ConstructorOnTypeBuilderInstantiation).m_ctor;
                }
                else if (method is MethodBuilder || method is ConstructorBuilder)
                {
                    // methodInfo must be GenericMethodDefinition; trying to emit G<?>.M<S>
                    methDef = method;
                }
                else if (method.IsGenericMethod)
                {
                    methDef = ((MethodInfo)method).GetGenericMethodDefinition();
                    methDef = methDef.Module.ResolveMethod(
                        methDef.MetadataTokenInternal, 
                        methDef.GetGenericArguments(), 
                        methDef.DeclaringType != null ? methDef.DeclaringType.GetGenericArguments() : null) as MethodBase;
                }
                else
                {
                    methDef = method;
                    methDef = method.Module.ResolveMethod(
                        method.MetadataTokenInternal, 
                        null, 
                        methDef.DeclaringType != null ? methDef.DeclaringType.GetGenericArguments() : null) as MethodBase;
                }

                parameterTypes = methDef.GetParameterTypes();
                returnType = methDef.GetReturnType();
            }
            else
            {
                parameterTypes = method.GetParameterTypes();
                returnType = method.GetReturnType();
            }

            if (method.DeclaringType.IsGenericType)
            {
                int length;
                byte[] sig = SignatureHelper.GetTypeSigToken(modBuilder, method.DeclaringType).InternalGetSignature(out length);
                tkParent = modBuilder.InternalGetTypeSpecTokenWithBytes(sig, length);
            }
            else if (method.Module != modBuilder)
            {                
                // Use typeRef as parent because the method's declaringType lives in a different assembly                
                tkParent = modBuilder.GetTypeToken(method.DeclaringType).Token;
            }
            else
            {
                // Use methodDef as parent because the method lives in this assembly and its declaringType has no generic arguments
                if (method is MethodInfo)
                    tkParent = modBuilder.GetMethodToken(method as MethodInfo).Token;
                else
                    tkParent = modBuilder.GetConstructorToken(method as ConstructorInfo).Token;
            }

            int sigLength;
            byte[] sigBytes = GetMemberRefSignature(
                method.CallingConvention, returnType, parameterTypes, 
                optionalParameterTypes, cGenericParameters).InternalGetSignature(out sigLength); 

            return modBuilder.InternalGetMemberRefFromSignature(tkParent, method.Name, sigBytes, sigLength);
        }

        internal virtual SignatureHelper GetMemberRefSignature(CallingConventions call, Type returnType, 
            Type[] parameterTypes, Type[] optionalParameterTypes)
        {
            return GetMemberRefSignature(call, returnType, parameterTypes, optionalParameterTypes, 0);
        }

        internal virtual SignatureHelper GetMemberRefSignature(CallingConventions call, Type returnType, 
            Type[] parameterTypes, Type[] optionalParameterTypes, int cGenericParameters)
        {
            int cParams;
            int i;
            SignatureHelper sig;
            
            if (parameterTypes == null)
            {
                cParams = 0;
            }
            else
            {
                cParams = parameterTypes.Length;
            }
            
            sig = SignatureHelper.GetMethodSigHelper(m_methodBuilder.Module, call, returnType, cGenericParameters);
            
            for (i=0; i < cParams; i++)
                sig.AddArgument(parameterTypes[i]);

            if (optionalParameterTypes != null && optionalParameterTypes.Length != 0)
            {
                // add the sentinel
                sig.AddSentinel();
                
                for (i=0; i < optionalParameterTypes.Length; i++)
                    sig.AddArgument(optionalParameterTypes[i]);
            }
            
            return sig;
        }

        internal virtual byte[] BakeByteArray()
        {
            // BakeByteArray is a package private function designed to be called by MethodDefinition to do
            // all of the fixups and return a new byte array representing the byte stream with labels resolved, etc.
            //
            

            int newSize;
            int updateAddr;
            byte []newBytes;

            if (m_currExcStackCount != 0)
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_UnclosedExceptionBlock"));
            }
            if (m_length == 0)
                return null;

            //Calculate the size of the new array.
            newSize = m_length;

            //Allocate space for the new array.
            newBytes = new byte[newSize];

            //Copy the data from the old array
            Array.Copy(m_ILStream, newBytes, newSize);

            //Do the fixups.
            //This involves iterating over all of the labels and
            //replacing them with their proper values.
            for (int i =0; i < m_fixupCount; i++)
            {
                updateAddr = GetLabelPos(m_fixupData[i].m_fixupLabel) - (m_fixupData[i].m_fixupPos + m_fixupData[i].m_fixupInstSize);

                //Handle single byte instructions
                //Throw an exception if they're trying to store a jump in a single byte instruction that doesn't fit.
                if (m_fixupData[i].m_fixupInstSize == 1)
                {

                    //Verify that our one-byte arg will fit into a Signed Byte.
                    if (updateAddr < SByte.MinValue || updateAddr > SByte.MaxValue)
                    {

                        throw new NotSupportedException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("NotSupported_IllegalOneByteBranch"),m_fixupData[i].m_fixupPos, updateAddr));
                    }

                    //Place the one-byte arg
                    if (updateAddr < 0)
                    {
                        
                        newBytes[m_fixupData[i].m_fixupPos] = (byte)(256 + updateAddr);
                    } else
                    {
                        newBytes[m_fixupData[i].m_fixupPos] = (byte)updateAddr;
                    }
                } else
                {
                    //Place the four-byte arg
                    PutInteger4(updateAddr, m_fixupData[i].m_fixupPos, newBytes);
                }
            }
            return newBytes;
        }

        internal virtual __ExceptionInfo[] GetExceptions()
        {
            __ExceptionInfo []temp;
            if (m_currExcStackCount != 0)
            {
                throw new NotSupportedException(Environment.GetResourceString(ResId.Argument_UnclosedExceptionBlock));
            }
            
            if (m_exceptionCount == 0)
            {
                return null;
            }
            
            temp = new __ExceptionInfo[m_exceptionCount];
            Array.Copy(m_exceptions, temp, m_exceptionCount);
            SortExceptions(temp);
            return temp;
        }

        internal virtual void EnsureCapacity(int size)
        {
            // Guarantees an array capable of holding at least size elements.
            if (m_length + size >= m_ILStream.Length)
            {
                if (m_length + size >= 2 * m_ILStream.Length)
                {
                    m_ILStream = EnlargeArray(m_ILStream, m_length + size);
                }
                else
                {
                    m_ILStream = EnlargeArray(m_ILStream);
                }
            }
        }

        internal virtual int PutInteger4(int value, int startPos, byte []array)
        {
            // Puts an Int32 onto the stream. This is an internal routine, so it does not do any error checking.

            array[startPos++] = (byte)value;
            array[startPos++] = (byte)(value >>8);
            array[startPos++] = (byte)(value >>16);
            array[startPos++] = (byte)(value >>24);
            return startPos;
        }

        internal virtual int GetLabelPos(Label lbl)
        {
            // Gets the position in the stream of a particular label.
            // Verifies that the label exists and that it has been given a value.

            int index = lbl.GetLabelValue();
            
            if (index < 0 || index >= m_labelCount)
                throw new ArgumentException(Environment.GetResourceString("Argument_BadLabel"));

            if (m_labelList[index] < 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_BadLabelContent"));

            return m_labelList[index];
        }

        internal virtual void AddFixup(Label lbl, int pos, int instSize)
        {
            // Notes the label, position, and instruction size of a new fixup.  Expands
            // all of the fixup arrays as appropriate.

            if (m_fixupData == null)
            {
                m_fixupData = new __FixupData[DefaultFixupArraySize];
            }

            if (m_fixupCount >= m_fixupData.Length)
            {
                m_fixupData = EnlargeArray(m_fixupData);
            }

            m_fixupData[m_fixupCount].m_fixupPos = pos;
            m_fixupData[m_fixupCount].m_fixupLabel = lbl;
            m_fixupData[m_fixupCount].m_fixupInstSize = instSize;
            m_fixupCount++;
        }

        internal virtual int GetMaxStackSize()
        {
            MethodBuilder methodBuilder = m_methodBuilder as MethodBuilder;
            if (methodBuilder == null) 
                throw new NotSupportedException();
            return m_maxStackSize + methodBuilder.GetNumberOfExceptions();
        }

        internal virtual void SortExceptions(__ExceptionInfo []exceptions)
        {

            int least;
            __ExceptionInfo temp;
            int length = exceptions.Length;
            for (int i =0; i < length; i++)
            {
                least = i;
                for (int j =i + 1; j < length; j++)
                {
                    if (exceptions[least].IsInner(exceptions[j]))
                    {
                        least = j;
                    }
                }
                temp = exceptions[i];
                exceptions[i] = exceptions[least];
                exceptions[least] = temp;
            }
        }

        internal virtual int []GetTokenFixups()
        {
             
            int[] narrowTokens = new int[m_RelocFixupCount];
            Array.Copy(m_RelocFixupList, narrowTokens, m_RelocFixupCount);
            return narrowTokens;
        }

        internal virtual int []GetRVAFixups()
        {
            
            int[] narrowRVAs = new int[m_RVAFixupCount];
            Array.Copy(m_RVAFixupList, narrowRVAs, m_RVAFixupCount);
            return narrowRVAs;
        }
        #endregion

        #region Public Members

        #region Emit
        public virtual void Emit(OpCode opcode)
        {
            EnsureCapacity(3);
            InternalEmit(opcode);

        }

        public virtual void Emit(OpCode opcode, byte arg) 
        {
            EnsureCapacity(4);
            InternalEmit(opcode);
            m_ILStream[m_length++]=arg;
        }

        [CLSCompliant(false)]
        public void Emit(OpCode opcode, sbyte arg) 
        {
            // Puts opcode onto the stream of instructions followed by arg

            EnsureCapacity(4);
            InternalEmit(opcode);
            if (arg<0) {
                m_ILStream[m_length++]=(byte)(256+arg);
            } else {
                m_ILStream[m_length++]=(byte) arg;
            }
        }

        public virtual void Emit(OpCode opcode, short arg) 
        {
            // Puts opcode onto the stream of instructions followed by arg
            EnsureCapacity(5);
            InternalEmit(opcode);
            m_ILStream[m_length++]=(byte) arg;
            m_ILStream[m_length++]=(byte) (arg>>8);
        }

        public virtual void Emit(OpCode opcode, int arg) 
        {
            // Puts opcode onto the stream of instructions followed by arg
            EnsureCapacity(7);
            InternalEmit(opcode);
            m_length=PutInteger4(arg, m_length, m_ILStream);
        }

        public virtual void Emit(OpCode opcode, MethodInfo meth)
        {
            if (opcode.Equals(OpCodes.Call) || opcode.Equals(OpCodes.Callvirt) || opcode.Equals(OpCodes.Newobj))
            {
                EmitCall(opcode, meth, null);
            }
            else
            {
                int stackchange = 0;

                if (meth == null)
                    throw new ArgumentNullException("meth");

                int tk = GetMethodToken(meth, null);

                EnsureCapacity(7);
                InternalEmit(opcode);

                UpdateStackSize(opcode, stackchange);
                RecordTokenFixup();
                m_length=PutInteger4(tk, m_length, m_ILStream);        
            }
        }


        public virtual void EmitCalli(OpCode opcode, CallingConventions callingConvention, 
            Type returnType, Type[] parameterTypes, Type[] optionalParameterTypes)
        {
            int stackchange = 0;
            SignatureHelper     sig;
            if (optionalParameterTypes != null)
            {
                if ((callingConvention & CallingConventions.VarArgs) == 0)
                {
                    // Client should not supply optional parameter in default calling convention
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NotAVarArgCallingConvention"));
                }
            }

            ModuleBuilder modBuilder = (ModuleBuilder) m_methodBuilder.Module;
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

            RecordTokenFixup();
            m_length=PutInteger4(modBuilder.GetSignatureToken(sig).Token, m_length, m_ILStream);
        }

        public virtual void EmitCalli(OpCode opcode, CallingConvention unmanagedCallConv, Type returnType, Type[] parameterTypes)
        {
            int             stackchange = 0;
            int             cParams = 0;
            int             i;
            SignatureHelper sig;
            
            ModuleBuilder modBuilder = (ModuleBuilder) m_methodBuilder.Module;

            // The opcode passed in must be the calli instruction.
            BCLDebug.Assert(opcode.Equals(OpCodes.Calli),
                            "Unexpected opcode passed to EmitCalli.");
            if (parameterTypes != null)
            {
                cParams = parameterTypes.Length;
            }
            
            sig = SignatureHelper.GetMethodSigHelper(
                modBuilder, 
                unmanagedCallConv, 
                returnType);
                            
            if (parameterTypes != null)
            {
                for (i = 0; i < cParams; i++) 
                {
                    sig.AddArgument(parameterTypes[i]);
                }
            }
                                  
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
            RecordTokenFixup();
            m_length=PutInteger4(modBuilder.GetSignatureToken(sig).Token, m_length, m_ILStream);
        }

        public virtual void EmitCall(OpCode opcode, MethodInfo methodInfo, Type[] optionalParameterTypes)
        {
            int stackchange = 0;

            if (methodInfo == null)
                throw new ArgumentNullException("methodInfo");

            int tk = GetMethodToken(methodInfo, optionalParameterTypes);

            EnsureCapacity(7);
            InternalEmit(opcode);

            // The opcode must be one of call, callvirt, or newobj.
            BCLDebug.Assert(opcode.Equals(OpCodes.Call) ||
                            opcode.Equals(OpCodes.Callvirt) ||
                            opcode.Equals(OpCodes.Newobj),
                            "Unexpected opcode passed to EmitCall.");
            // Push the return value if there is one.
            if (methodInfo.GetReturnType() != typeof(void))
                stackchange++;
            // Pop the parameters.
            if (methodInfo.GetParameterTypes() != null)
                stackchange -= methodInfo.GetParameterTypes().Length;

            // Pop the this parameter if the method is non-static and the
            // instruction is not newobj.
            if (!(methodInfo is SymbolMethod) && methodInfo.IsStatic == false && !(opcode.Equals(OpCodes.Newobj)))
                stackchange--;
            // Pop the optional parameters off the stack.
            if (optionalParameterTypes != null)
                stackchange -= optionalParameterTypes.Length;
            UpdateStackSize(opcode, stackchange);

            RecordTokenFixup();
            m_length=PutInteger4(tk, m_length, m_ILStream);
        }

        public virtual void Emit(OpCode opcode, SignatureHelper signature)
        {
            int stackchange = 0;
            ModuleBuilder modBuilder = (ModuleBuilder) m_methodBuilder.Module;
            if (signature == null)
                throw new ArgumentNullException("signature");

            SignatureToken sig = modBuilder.GetSignatureToken(signature);

            int tempVal = sig.Token;

            EnsureCapacity(7);
            InternalEmit(opcode);

            // The only IL instruction that has VarPop behaviour, that takes a
            // Signature token as a parameter is calli.  Pop the parameters and
            // the native function pointer.  To be conservative, do not pop the
            // this pointer since this information is not easily derived from
            // SignatureHelper.
            if (opcode.m_pop == StackBehaviour.Varpop)
            {
                BCLDebug.Assert(opcode.Equals(OpCodes.Calli),
                                "Unexpected opcode encountered for StackBehaviour VarPop.");
                // Pop the arguments..
                stackchange -= signature.ArgumentCount;
                // Pop native function pointer off the stack.
                stackchange--;
                UpdateStackSize(opcode, stackchange);
            }

            RecordTokenFixup();
            m_length=PutInteger4(tempVal, m_length, m_ILStream);
        }

        [System.Runtime.InteropServices.ComVisible(true)]
        public virtual void Emit(OpCode opcode, ConstructorInfo con) {
            int stackchange = 0;

            int tk = GetMethodToken(con, null);

            EnsureCapacity(7);
            InternalEmit(opcode);

            // Make a conservative estimate by assuming a return type and no
            // this parameter.
            if (opcode.m_push == StackBehaviour.Varpush)
            {
                // Instruction must be one of call or callvirt.
                BCLDebug.Assert(opcode.Equals(OpCodes.Call) ||
                                opcode.Equals(OpCodes.Callvirt),
                                "Unexpected opcode encountered for StackBehaviour of VarPush.");
                stackchange++;
            }
            if (opcode.m_pop == StackBehaviour.Varpop)
            {
                // Instruction must be one of call, callvirt or newobj.
                BCLDebug.Assert(opcode.Equals(OpCodes.Call) ||
                                opcode.Equals(OpCodes.Callvirt) ||
                                opcode.Equals(OpCodes.Newobj),
                                "Unexpected opcode encountered for StackBehaviour of VarPop.");

                if (con.GetParameterTypes() != null)
                    stackchange -= con.GetParameterTypes().Length;
            }
            UpdateStackSize(opcode, stackchange);

            RecordTokenFixup();
            m_length=PutInteger4(tk, m_length, m_ILStream);
        }

        public virtual void Emit(OpCode opcode, Type cls)
        {
            // Puts opcode onto the stream and then the metadata token represented
            // by cls.  The location of cls is recorded so that the token can be
            // patched if necessary when persisting the module to a PE.

            ModuleBuilder modBuilder = (ModuleBuilder) m_methodBuilder.Module;
            int tempVal = modBuilder.GetTypeToken( cls ).Token;

            EnsureCapacity(7);
            InternalEmit(opcode);
            RecordTokenFixup();
            m_length=PutInteger4(tempVal, m_length, m_ILStream);
        }

        public virtual void Emit(OpCode opcode, long arg) {
            EnsureCapacity(11);
            InternalEmit(opcode);
            m_ILStream[m_length++] = (byte) arg;
            m_ILStream[m_length++] = (byte) (arg>>8);
            m_ILStream[m_length++] = (byte) (arg>>16);
            m_ILStream[m_length++] = (byte) (arg>>24);
            m_ILStream[m_length++] = (byte) (arg>>32);
            m_ILStream[m_length++] = (byte) (arg>>40);
            m_ILStream[m_length++] = (byte) (arg>>48);
            m_ILStream[m_length++] = (byte) (arg>>56);
        }

        unsafe public virtual void Emit(OpCode opcode, float arg) {
            EnsureCapacity(7);
            InternalEmit(opcode);
            uint tempVal = *(uint*)&arg;
            m_ILStream[m_length++] = (byte) tempVal;
            m_ILStream[m_length++] = (byte) (tempVal>>8);
            m_ILStream[m_length++] = (byte) (tempVal>>16);
            m_ILStream[m_length++] = (byte) (tempVal>>24);
        }

        unsafe public virtual void Emit(OpCode opcode, double arg) {
            EnsureCapacity(11);
            InternalEmit(opcode);
            ulong tempVal = *(ulong*)&arg;           
            m_ILStream[m_length++] = (byte) tempVal;
            m_ILStream[m_length++] = (byte) (tempVal>>8);
            m_ILStream[m_length++] = (byte) (tempVal>>16);
            m_ILStream[m_length++] = (byte) (tempVal>>24);
            m_ILStream[m_length++] = (byte) (tempVal>>32);
            m_ILStream[m_length++] = (byte) (tempVal>>40);
            m_ILStream[m_length++] = (byte) (tempVal>>48);
            m_ILStream[m_length++] = (byte) (tempVal>>56);
        }

        public virtual void Emit(OpCode opcode, Label label) 
        {
            // Puts opcode onto the stream and leaves space to include label
            // when fixups are done.  Labels are created using ILGenerator.DefineLabel and
            // their location within the stream is fixed by using ILGenerator.MarkLabel.
            // If a single-byte instruction (designated by the _S suffix in OpCodes.cs) is used,
            // the label can represent a jump of at most 127 bytes along the stream.
            //
            // opcode must represent a branch instruction (although we don't explicitly
            // verify this).  Since branches are relative instructions, label will be replaced with the
            // correct offset to branch during the fixup process.
            
            int tempVal = label.GetLabelValue();
            EnsureCapacity(7);

             
            InternalEmit(opcode);
            if (OpCodes.TakesSingleByteArgument(opcode)) {
                AddFixup(label, m_length, 1);
                m_length++;
            } else {
                AddFixup(label, m_length, 4);
                m_length+=4;
            }
        }

        public virtual void Emit(OpCode opcode, Label[] labels)
        {
            // Emitting a switch table

            int i;
            int remaining;                  // number of bytes remaining for this switch instruction to be substracted
            // for computing the offset

            int count = labels.Length;

            EnsureCapacity( count * 4 + 7 );
            InternalEmit(opcode);
            m_length = PutInteger4( count, m_length, m_ILStream );
            for ( remaining = count * 4, i = 0; remaining > 0; remaining -= 4, i++ ) {
                AddFixup( labels[i], m_length, remaining );
                m_length += 4;
            }
        }

        public virtual void Emit(OpCode opcode, FieldInfo field)
        {
            
            ModuleBuilder modBuilder = (ModuleBuilder) m_methodBuilder.Module;
            int tempVal = modBuilder.GetFieldToken( field ).Token;
            EnsureCapacity(7);
            InternalEmit(opcode);
            RecordTokenFixup();
            m_length=PutInteger4(tempVal, m_length, m_ILStream);
        }

        public virtual void Emit(OpCode opcode, String str) 
        {
            // Puts the opcode onto the IL stream followed by the metadata token
            // represented by str.  The location of str is recorded for future
            // fixups if the module is persisted to a PE.

            ModuleBuilder modBuilder = (ModuleBuilder) m_methodBuilder.Module;
            int tempVal = modBuilder.GetStringConstant(str).Token;
            EnsureCapacity(7);
            InternalEmit(opcode);
            m_length=PutInteger4(tempVal, m_length, m_ILStream);
        }

        public virtual void Emit(OpCode opcode, LocalBuilder local)
        {
            // Puts the opcode onto the IL stream followed by the information for local variable local.

            if (local == null)
            {
                throw new ArgumentNullException("local");
            }
            int tempVal = local.GetLocalIndex();
            if (local.GetMethodBuilder() != m_methodBuilder)
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_UnmatchedMethodForLocal"), "local");
            }
            // If the instruction is a ldloc, ldloca a stloc, morph it to the optimal form.
            if (opcode.Equals(OpCodes.Ldloc))
            {
                switch(tempVal)
                {
                    case 0:
                        opcode = OpCodes.Ldloc_0;
                        break;
                    case 1:
                        opcode = OpCodes.Ldloc_1;
                        break;
                    case 2:
                        opcode = OpCodes.Ldloc_2;
                        break;
                    case 3:
                        opcode = OpCodes.Ldloc_3;
                        break;
                    default:
                        if (tempVal <= 255)
                            opcode = OpCodes.Ldloc_S;
                        break;
                }
            }
            else if (opcode.Equals(OpCodes.Stloc))
            {
                switch(tempVal)
                {
                    case 0:
                        opcode = OpCodes.Stloc_0;
                        break;
                    case 1:
                        opcode = OpCodes.Stloc_1;
                        break;
                    case 2:
                        opcode = OpCodes.Stloc_2;
                        break;
                    case 3:
                        opcode = OpCodes.Stloc_3;
                        break;
                    default:
                        if (tempVal <= 255)
                            opcode = OpCodes.Stloc_S;
                        break;
                }
            }
            else if (opcode.Equals(OpCodes.Ldloca))
            {
                if (tempVal <= 255)
                    opcode = OpCodes.Ldloca_S;
            }

            EnsureCapacity(7);
            InternalEmit(opcode);
             
            if (opcode.OperandType == OperandType.InlineNone)
                return;
            else if (!OpCodes.TakesSingleByteArgument(opcode))
            {
                m_ILStream[m_length++]=(byte) tempVal;
                m_ILStream[m_length++]=(byte) (tempVal>>8);
            }
            else
            {
                //Handle stloc_1, ldloc_1
                if (tempVal > Byte.MaxValue)
                {
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_BadInstructionOrIndexOutOfBound"));
                }
                m_ILStream[m_length++]=(byte)tempVal;
            }
        }
        #endregion

        #region Exceptions
        public virtual Label BeginExceptionBlock() 
        {
            // Begin an Exception block.  Creating an Exception block records some information,
            // but does not actually emit any IL onto the stream.  Exceptions should be created and
            // marked in the following form:
            //
            // Emit Some IL
            // BeginExceptionBlock
            // Emit the IL which should appear within the "try" block
            // BeginCatchBlock
            // Emit the IL which should appear within the "catch" block
            // Optional: BeginCatchBlock (this can be repeated an arbitrary number of times
            // EndExceptionBlock

            // Delay init
            if (m_exceptions == null)
            {
                m_exceptions = new __ExceptionInfo[DefaultExceptionArraySize];
            }

            if (m_currExcStack == null)
            {
                m_currExcStack = new __ExceptionInfo[DefaultExceptionArraySize];
            }

            if (m_exceptionCount>=m_exceptions.Length) {
                m_exceptions=EnlargeArray(m_exceptions);
            }

            if (m_currExcStackCount>=m_currExcStack.Length) {
                m_currExcStack = EnlargeArray(m_currExcStack);
            }

            Label endLabel = DefineLabel();
            __ExceptionInfo exceptionInfo = new __ExceptionInfo(m_length, endLabel);

            // add the exception to the tracking list
            m_exceptions[m_exceptionCount++] = exceptionInfo;

            // Make this exception the current active exception
            m_currExcStack[m_currExcStackCount++] = exceptionInfo;
            return endLabel;
        }

        public virtual void EndExceptionBlock() {
            if (m_currExcStackCount==0) {
                throw new NotSupportedException(Environment.GetResourceString("Argument_NotInExceptionBlock"));
            }

           // Pop the current exception block
            __ExceptionInfo current = m_currExcStack[m_currExcStackCount-1];
            m_currExcStack[m_currExcStackCount-1] = null;
            m_currExcStackCount--;

            Label endLabel = current.GetEndLabel();
            int state = current.GetCurrentState();

            if (state == __ExceptionInfo.State_Filter ||
                state == __ExceptionInfo.State_Try)
            {
                 
                 
                throw new InvalidOperationException(Environment.GetResourceString("Argument_BadExceptionCodeGen"));
            }

            if (state == __ExceptionInfo.State_Catch) {
                this.Emit(OpCodes.Leave, endLabel);
            } else if (state == __ExceptionInfo.State_Finally || state == __ExceptionInfo.State_Fault) {
                this.Emit(OpCodes.Endfinally);
            }

            //Check if we've alredy set this label.
            //The only reason why we might have set this is if we have a finally block.
            if (m_labelList[endLabel.GetLabelValue()]==-1) {
                MarkLabel(endLabel);
            } else {
                MarkLabel(current.GetFinallyEndLabel());
            }

            current.Done(m_length);
        }

        public virtual void BeginExceptFilterBlock() 
        {
            // Begins a eception filter block.  Emits a branch instruction to the end of the current exception block.

            if (m_currExcStackCount == 0)
                throw new NotSupportedException(Environment.GetResourceString("Argument_NotInExceptionBlock"));

            __ExceptionInfo current = m_currExcStack[m_currExcStackCount-1];

            Label endLabel = current.GetEndLabel();
            this.Emit(OpCodes.Leave, endLabel);

            current.MarkFilterAddr(m_length);
        }

        public virtual void BeginCatchBlock(Type exceptionType) 
        {
            // Begins a catch block.  Emits a branch instruction to the end of the current exception block.

            if (m_currExcStackCount==0) {
                throw new NotSupportedException(Environment.GetResourceString("Argument_NotInExceptionBlock"));
            }
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

                Label endLabel = current.GetEndLabel();
                this.Emit(OpCodes.Leave, endLabel);

            }

            current.MarkCatchAddr(m_length, exceptionType);
        }

        public virtual void BeginFaultBlock()
        {
            if (m_currExcStackCount==0) {
                throw new NotSupportedException(Environment.GetResourceString("Argument_NotInExceptionBlock"));
            }
            __ExceptionInfo current = m_currExcStack[m_currExcStackCount-1];

            // emit the leave for the clause before this one.
            Label endLabel = current.GetEndLabel();
            this.Emit(OpCodes.Leave, endLabel);

            current.MarkFaultAddr(m_length);
        }

        public virtual void BeginFinallyBlock() 
        {
            if (m_currExcStackCount==0) {
                throw new NotSupportedException(Environment.GetResourceString("Argument_NotInExceptionBlock"));
            }
            __ExceptionInfo current = m_currExcStack[m_currExcStackCount-1];
            int         state = current.GetCurrentState();
            Label       endLabel = current.GetEndLabel();
            int         catchEndAddr = 0;
            if (state != __ExceptionInfo.State_Try)
            {
                // generate leave for any preceeding catch clause
                this.Emit(OpCodes.Leave, endLabel);                
                catchEndAddr = m_length;
            }
            
            MarkLabel(endLabel);


            Label finallyEndLabel = this.DefineLabel();
            current.SetFinallyEndLabel(finallyEndLabel);
            
            // generate leave for try clause                                                  
            this.Emit(OpCodes.Leave, finallyEndLabel);
            if (catchEndAddr == 0)
                catchEndAddr = m_length;
            current.MarkFinallyAddr(m_length, catchEndAddr);
        }

        #endregion

        #region Labels
        public virtual Label DefineLabel() 
        {
            // Declares a new Label.  This is just a token and does not yet represent any particular location
            // within the stream.  In order to set the position of the label within the stream, you must call
            // Mark Label.

            // Delay init the lable array in case we dont use it
            if (m_labelList == null){
                m_labelList = new int[DefaultLabelArraySize];
            }

            if (m_labelCount>=m_labelList.Length) {
                m_labelList = EnlargeArray(m_labelList);
            }
            m_labelList[m_labelCount]=-1;
            return new Label(m_labelCount++);
        }

        public virtual void MarkLabel(Label loc) 
        {
            // Defines a label by setting the position where that label is found within the stream.
            // Does not allow a label to be defined more than once.

            int labelIndex = loc.GetLabelValue();

            //This should never happen.
            if (labelIndex<0 || labelIndex>=m_labelList.Length) {
                throw new ArgumentException (Environment.GetResourceString("Argument_InvalidLabel"));
            }

            if (m_labelList[labelIndex]!=-1) {
                throw new ArgumentException (Environment.GetResourceString("Argument_RedefinedLabel"));
            }

            m_labelList[labelIndex]=m_length;
        }

        #endregion

        #region IL Macros
        public virtual void ThrowException(Type excType)
        {
            // Emits the il to throw an exception

            if (excType==null) {
                throw new ArgumentNullException("excType");
            }

            ModuleBuilder  modBuilder = (ModuleBuilder) m_methodBuilder.Module;

            if (!excType.IsSubclassOf(typeof(Exception)) && excType!=typeof(Exception)) {
                throw new ArgumentException(Environment.GetResourceString("Argument_NotExceptionType"));
            }
            ConstructorInfo con = excType.GetConstructor(Type.EmptyTypes);
            if (con==null) {
                throw new ArgumentException(Environment.GetResourceString("Argument_MissingDefaultConstructor"));
            }
            this.Emit(OpCodes.Newobj, con);
            this.Emit(OpCodes.Throw);
        }

        public virtual void EmitWriteLine(String value)
        {
            // Emits the IL to call Console.WriteLine with a string.

            Emit(OpCodes.Ldstr, value);
            Type[] parameterTypes = new Type[1];
            parameterTypes[0] = typeof(String);
            MethodInfo mi = typeof(Console).GetMethod("WriteLine", parameterTypes);
            Emit(OpCodes.Call, mi);
        }

        public virtual void EmitWriteLine(LocalBuilder localBuilder) 
        {
            // Emits the IL necessary to call WriteLine with lcl.  It is
            // an error to call EmitWriteLine with a lcl which is not of
            // one of the types for which Console.WriteLine implements overloads. (e.g.
            // we do *not* call ToString on the locals.

            Object          cls;
            if (m_methodBuilder==null)
            {
                throw new ArgumentException(Environment.GetResourceString("InvalidOperation_BadILGeneratorUsage"));
            }

            MethodInfo prop = typeof(Console).GetMethod("get_Out");
            Emit(OpCodes.Call, prop);
            Emit(OpCodes.Ldloc, localBuilder);
            Type[] parameterTypes = new Type[1];
            cls = localBuilder.LocalType;
            if (cls is TypeBuilder || cls is EnumBuilder) {
                throw new ArgumentException(Environment.GetResourceString("NotSupported_OutputStreamUsingTypeBuilder"));
            }
            parameterTypes[0] = (Type)cls;
            MethodInfo mi = typeof(TextWriter).GetMethod("WriteLine", parameterTypes);
             if (mi==null) {
                throw new ArgumentException(Environment.GetResourceString("Argument_EmitWriteLineType"), "localBuilder");
            }

            Emit(OpCodes.Callvirt, mi);
        }

        public virtual void EmitWriteLine(FieldInfo fld)
        {
            // Emits the IL necessary to call WriteLine with fld.  It is
            // an error to call EmitWriteLine with a fld which is not of
            // one of the types for which Console.WriteLine implements overloads. (e.g.
            // we do *not* call ToString on the fields.

            Object cls;

            if (fld == null)
            {
                throw new ArgumentNullException("fld");
            }
            
            MethodInfo prop = typeof(Console).GetMethod("get_Out");
            Emit(OpCodes.Call, prop);

            if ((fld.Attributes & FieldAttributes.Static)!=0) {
                Emit(OpCodes.Ldsfld, fld);
            } else {
                Emit(OpCodes.Ldarg, (short)0); //Load the this ref.
                Emit(OpCodes.Ldfld, fld);
            }
            Type[] parameterTypes = new Type[1];
            cls = fld.FieldType;
            if (cls is TypeBuilder || cls is EnumBuilder) {
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_OutputStreamUsingTypeBuilder"));
            }
            parameterTypes[0] = (Type)cls;
            MethodInfo mi = typeof(TextWriter).GetMethod("WriteLine", parameterTypes);
            if (mi==null) {
                throw new ArgumentException(Environment.GetResourceString("Argument_EmitWriteLineType"), "fld");
            }
            Emit(OpCodes.Callvirt, mi);
        }

        #endregion

        #region Debug API
        public virtual LocalBuilder DeclareLocal(Type localType)
        {
            return DeclareLocal(localType, false);
        }

        public virtual LocalBuilder DeclareLocal(Type localType, bool pinned)
        {
            // Declare a local of type "local". The current active lexical scope
            // will be the scope that local will live.

            LocalBuilder    localBuilder;

            MethodBuilder methodBuilder = m_methodBuilder as MethodBuilder;
            if (methodBuilder == null) 
                throw new NotSupportedException();

            if (methodBuilder.IsTypeCreated())
            {
                // cannot change method after its containing type has been created
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_TypeHasBeenCreated"));
            }

            if (localType==null) {
                throw new ArgumentNullException("localType");
            }

            if (methodBuilder.m_bIsBaked) {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_MethodBaked"));
            }

            // add the localType to local signature
            m_localSignature.AddArgument(localType, pinned);

            localBuilder = new LocalBuilder(m_localCount, localType, methodBuilder, pinned);
            m_localCount++;
            return localBuilder;
        }

        public virtual void UsingNamespace(String usingNamespace)
        {
            // Specifying the namespace to be used in evaluating locals and watches
            // for the current active lexical scope.
            int index;

            if (usingNamespace == null)
                throw new ArgumentNullException("usingNamespace");

            if (usingNamespace.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "usingNamespace");

            MethodBuilder methodBuilder = m_methodBuilder as MethodBuilder;
            if (methodBuilder == null) 
                throw new NotSupportedException();

            index = methodBuilder.GetILGenerator().m_ScopeTree.GetCurrentActiveScopeIndex();
            if (index == -1)
            {
                methodBuilder.m_localSymInfo.AddUsingNamespace(usingNamespace);
            }
            else
            {
                m_ScopeTree.AddUsingNamespaceToCurrentScope(usingNamespace);
            }
        }

        public virtual void MarkSequencePoint(
            ISymbolDocumentWriter document,
            int startLine,       // line number is 1 based
            int startColumn,     // column is 0 based
            int endLine,         // line number is 1 based
            int endColumn)       // column is 0 based
        {
            if (startLine == 0 || startLine < 0 || endLine == 0 || endLine < 0)
            {
                throw new ArgumentOutOfRangeException("startLine");
            }
            m_LineNumberInfo.AddLineNumberInfo(document, m_length, startLine, startColumn, endLine, endColumn);
        }

        public virtual void BeginScope()
        {
            m_ScopeTree.AddScopeInfo(ScopeAction.Open, m_length);
        }

        public virtual void EndScope()
        {
            m_ScopeTree.AddScopeInfo(ScopeAction.Close, m_length);
        }

        #endregion

        #endregion

        void _ILGenerator.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _ILGenerator.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _ILGenerator.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _ILGenerator.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }
    }

    internal struct __FixupData
    {
        internal Label m_fixupLabel;
        internal int m_fixupPos;
         
         
        internal int m_fixupInstSize;

    }

    internal class __ExceptionInfo {

        internal const int None             = 0x0000;  //COR_ILEXCEPTION_CLAUSE_NONE
        internal const int Filter           = 0x0001;  //COR_ILEXCEPTION_CLAUSE_FILTER
        internal const int Finally          = 0x0002;  //COR_ILEXCEPTION_CLAUSE_FINALLY
        internal const int Fault            = 0x0004;  //COR_ILEXCEPTION_CLAUSE_FAULT
        internal const int PreserveStack    = 0x0004;  //COR_ILEXCEPTION_CLAUSE_PRESERVESTACK

        internal const int State_Try = 0;
        internal const int State_Filter =1;
        internal const int State_Catch = 2;
        internal const int State_Finally = 3;
        internal const int State_Fault = 4;
        internal const int State_Done = 5;

        internal int m_startAddr;
        internal int []m_filterAddr;
        internal int []m_catchAddr;
        internal int []m_catchEndAddr;
        internal int []m_type;
        internal Type []m_catchClass;
        internal Label m_endLabel;
        internal Label m_finallyEndLabel;
        internal int m_endAddr;
        internal int m_endFinally;
        internal int m_currentCatch;

        int m_currentState;


        //This will never get called.  The values exist merely to keep the
        //compiler happy.
        private __ExceptionInfo() {
            m_startAddr = 0;
            m_filterAddr = null;
            m_catchAddr = null;
            m_catchEndAddr = null;
            m_endAddr = 0;
            m_currentCatch = 0;
            m_type = null;
            m_endFinally = -1;
            m_currentState = State_Try;
        }

        internal __ExceptionInfo(int startAddr, Label endLabel) {
            m_startAddr=startAddr;
            m_endAddr=-1;
            m_filterAddr=new int[4];
            m_catchAddr=new int[4];
            m_catchEndAddr=new int[4];
            m_catchClass=new Type[4];
            m_currentCatch=0;
            m_endLabel=endLabel;
            m_type=new int[4];
            m_endFinally=-1;
            m_currentState = State_Try;
        }

        private void MarkHelper(
            int         catchorfilterAddr,      // the starting address of a clause
            int         catchEndAddr,           // the end address of a previous catch clause. Only use when finally is following a catch
            Type        catchClass,             // catch exception type
            int         type)                   // kind of clause
        {
            if (m_currentCatch>=m_catchAddr.Length) {
                m_filterAddr=ILGenerator.EnlargeArray(m_filterAddr);
                m_catchAddr=ILGenerator.EnlargeArray(m_catchAddr);
                m_catchEndAddr=ILGenerator.EnlargeArray(m_catchEndAddr);
                m_catchClass=ILGenerator.EnlargeArray(m_catchClass);
                m_type = ILGenerator.EnlargeArray(m_type);
            }
            if (type == Filter)
            {
                m_type[m_currentCatch]=type;
                m_filterAddr[m_currentCatch] = catchorfilterAddr;
                m_catchAddr[m_currentCatch] = -1;
                if (m_currentCatch > 0)
                {
                    BCLDebug.Assert(m_catchEndAddr[m_currentCatch-1] == -1,"m_catchEndAddr[m_currentCatch-1] == -1");
                    m_catchEndAddr[m_currentCatch-1] = catchorfilterAddr;
                }
            }
            else
            {
                // catch or Fault clause
                m_catchClass[m_currentCatch]=catchClass;
                if (m_type[m_currentCatch] != Filter)
                {
                    m_type[m_currentCatch]=type;
                }
                m_catchAddr[m_currentCatch]=catchorfilterAddr;
                if (m_currentCatch > 0)
                {
                        if (m_type[m_currentCatch] != Filter)
                        {
                            BCLDebug.Assert(m_catchEndAddr[m_currentCatch-1] == -1,"m_catchEndAddr[m_currentCatch-1] == -1");
                            m_catchEndAddr[m_currentCatch-1] = catchEndAddr;
                        }
                }
                m_catchEndAddr[m_currentCatch]=-1;  
                m_currentCatch++;
            }

            if (m_endAddr==-1)
            {
                m_endAddr=catchorfilterAddr;
            }
        }

        internal virtual void MarkFilterAddr(int filterAddr)
        {
            m_currentState = State_Filter;
            MarkHelper(filterAddr, filterAddr, null, Filter);
        }

        internal virtual void MarkFaultAddr(int faultAddr)
        {
            m_currentState = State_Fault;
            MarkHelper(faultAddr, faultAddr, null, Fault);
        }

        internal virtual void MarkCatchAddr(int catchAddr, Type catchException) {
            m_currentState = State_Catch;
            MarkHelper(catchAddr, catchAddr, catchException, None);
        }

        internal virtual void MarkFinallyAddr(int finallyAddr, int endCatchAddr) {
            if (m_endFinally!=-1) {
                throw new ArgumentException(Environment.GetResourceString("Argument_TooManyFinallyClause"));
            } else {
                m_currentState = State_Finally;
                m_endFinally=finallyAddr;
            }
            MarkHelper(finallyAddr, endCatchAddr, null, Finally);
        }

        internal virtual void Done(int endAddr) {
            BCLDebug.Assert(m_currentCatch > 0,"m_currentCatch > 0");
            BCLDebug.Assert(m_catchAddr[m_currentCatch-1] > 0,"m_catchAddr[m_currentCatch-1] > 0");
            BCLDebug.Assert(m_catchEndAddr[m_currentCatch-1] == -1,"m_catchEndAddr[m_currentCatch-1] == -1");
            m_catchEndAddr[m_currentCatch-1] = endAddr;
            m_currentState = State_Done;
        }

        internal virtual int GetStartAddress() {
            return m_startAddr;
        }

        internal virtual int GetEndAddress() {
            return m_endAddr;
        }

        internal virtual int GetFinallyEndAddress() {
            return m_endFinally;
        }

        internal virtual Label GetEndLabel() {
            return m_endLabel;
        }

        internal virtual int [] GetFilterAddresses() {
            return m_filterAddr;
        }

        internal virtual int [] GetCatchAddresses() {
            return m_catchAddr;
        }

        internal virtual int [] GetCatchEndAddresses() {
            return m_catchEndAddr;
        }

        internal virtual Type [] GetCatchClass() {
            return m_catchClass;
        }

        internal virtual int GetNumberOfCatches() {
            return m_currentCatch;
        }

        internal virtual int[] GetExceptionTypes() {
            return m_type;
        }

        internal virtual void SetFinallyEndLabel(Label lbl) {
            m_finallyEndLabel=lbl;
        }

        internal virtual Label GetFinallyEndLabel() {
            return m_finallyEndLabel;
        }

        // Specifies whether exc is an inner exception for "this".  The way
        // its determined is by comparing the end address for the last catch
        // clause for both exceptions.  If they're the same, the start address
        // for the exception is compared.
        // WARNING: This is not a generic function to determine the innerness
        // of an exception.  This is somewhat of a mis-nomer.  This gives a
        // random result for cases where the two exceptions being compared do
        // not having a nesting relation. 
        internal bool IsInner(__ExceptionInfo exc) {
            BCLDebug.Assert(m_currentCatch > 0,"m_currentCatch > 0");
            BCLDebug.Assert(exc.m_currentCatch > 0,"exc.m_currentCatch > 0");

            int exclast = exc.m_currentCatch - 1;
            int last = m_currentCatch - 1;

            if (exc.m_catchEndAddr[exclast]  < m_catchEndAddr[last])
                return true;
            else if (exc.m_catchEndAddr[exclast] == m_catchEndAddr[last])
            {
                BCLDebug.Assert(exc.GetEndAddress() != GetEndAddress(),
                                "exc.GetEndAddress() != GetEndAddress()");
                if (exc.GetEndAddress() > GetEndAddress())
                    return true;
            }
            return false;
        }

        // 0 indicates in a try block
        // 1 indicates in a filter block
        // 2 indicates in a catch block
        // 3 indicates in a finally block
        // 4 indicates Done
        internal virtual int GetCurrentState() {
            return m_currentState;
        }
    }


    /***************************
    *
    * Scope Tree is a class that track the scope structure within a method body
    * It keeps track two parallel array. m_ScopeAction keeps track the action. It can be
    * OpenScope or CloseScope. m_iOffset records the offset where the action
    * takes place.
    *
    ***************************/
    [Serializable]
    enum ScopeAction
    {
        Open        = 0x0,
        Close       = 0x1,
    }

    internal class ScopeTree
    {
        internal ScopeTree()
        {
            // initialize data variables
            m_iOpenScopeCount = 0;
            m_iCount = 0;
        }

        /***************************
        *
        * Find the current active lexcial scope. For example, if we have
        * "Open Open Open Close",
        * we will return 1 as the second BeginScope is currently active.
        *
        ***************************/
        internal int GetCurrentActiveScopeIndex()
        {
            int         cClose = 0;
            int         i = m_iCount - 1;

            if (m_iCount == 0)
            {
                return -1;
            }
            for (; cClose > 0 || m_ScopeActions[i] == ScopeAction.Close; i--)
            {
                if (m_ScopeActions[i] == ScopeAction.Open)
                {
                    cClose--;
                }
                else
                    cClose++;
            }

            return i;
        }

        internal void AddLocalSymInfoToCurrentScope(
            String          strName,
            byte[]          signature,
            int             slot,
            int             startOffset,
            int             endOffset)
        {
            int         i = GetCurrentActiveScopeIndex();
            if (m_localSymInfos[i] == null)
            {
                m_localSymInfos[i] = new LocalSymInfo();
            }
            m_localSymInfos[i].AddLocalSymInfo(strName, signature, slot, startOffset, endOffset);
        }

        internal void AddUsingNamespaceToCurrentScope(
            String          strNamespace)
        {
            int         i = GetCurrentActiveScopeIndex();
            if (m_localSymInfos[i] == null)
            {
                m_localSymInfos[i] = new LocalSymInfo();
            }
            m_localSymInfos[i].AddUsingNamespace(strNamespace);
        }

        internal void AddScopeInfo(ScopeAction sa, int iOffset)
        {
            if (sa == ScopeAction.Close && m_iOpenScopeCount <=0)
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_UnmatchingSymScope"));
            }

            // make sure that arrays are large enough to hold addition info
            EnsureCapacity();

             
             
             
            m_ScopeActions[m_iCount] = sa;
            m_iOffsets[m_iCount] = iOffset;
            m_localSymInfos[m_iCount] = null;
            m_iCount++;
            if (sa == ScopeAction.Open)
            {
                m_iOpenScopeCount++;
            }
            else
                m_iOpenScopeCount--;

        }

        /**************************
        *
        * Helper to ensure arrays are large enough
        *
        **************************/
        internal void EnsureCapacity()
        {
            if (m_iCount == 0)
            {
                // First time. Allocate the arrays.
                m_iOffsets = new int[InitialSize];
                m_ScopeActions = new ScopeAction[InitialSize];
                m_localSymInfos = new LocalSymInfo[InitialSize];
            }
            else if (m_iCount == m_iOffsets.Length)
            {

                // the arrays are full. Enlarge the arrays
                int[] temp = new int [m_iCount * 2];
                Array.Copy(m_iOffsets, temp, m_iCount);
                m_iOffsets = temp;

                ScopeAction[] tempSA = new ScopeAction[m_iCount * 2];
                Array.Copy(m_ScopeActions, tempSA, m_iCount);
                m_ScopeActions = tempSA;

                LocalSymInfo[] tempLSI = new LocalSymInfo[m_iCount * 2];
                Array.Copy(m_localSymInfos, tempLSI, m_iCount);
                m_localSymInfos = tempLSI;

            }
        }

        internal void EmitScopeTree(ISymbolWriter symWriter)
        {
            int         i;
            for (i = 0; i < m_iCount; i++)
            {
                if (m_ScopeActions[i] == ScopeAction.Open)
                {
                    symWriter.OpenScope(m_iOffsets[i]);
                }
                else
                {
                    symWriter.CloseScope(m_iOffsets[i]);
                }
                if (m_localSymInfos[i] != null)
                {
                    m_localSymInfos[i].EmitLocalSymInfo(symWriter);
                }
            }
        }

        internal int[]          m_iOffsets;                 // array of offsets
        internal ScopeAction[]  m_ScopeActions;             // array of scope actions
        internal int            m_iCount;                   // how many entries in the arrays are occupied
        internal int            m_iOpenScopeCount;          // keep track how many scopes are open
        internal const int      InitialSize = 16;
        internal LocalSymInfo[] m_localSymInfos;            // keep track debugging local information
    }


    /***************************
    *
    * This class tracks the line number info
    *
    ***************************/
    internal class LineNumberInfo
    {
        internal LineNumberInfo()
        {
            // initialize data variables
            m_DocumentCount = 0;
            m_iLastFound = 0;
        }

        internal void AddLineNumberInfo(
            ISymbolDocumentWriter document,
            int             iOffset,
            int             iStartLine,
            int             iStartColumn,
            int             iEndLine,
            int             iEndColumn)
        {
            int         i;
            
            // make sure that arrays are large enough to hold addition info
            i = FindDocument(document);
            
            BCLDebug.Assert(i < m_DocumentCount, "Bad document look up!");
            m_Documents[i].AddLineNumberInfo(document, iOffset, iStartLine, iStartColumn, iEndLine, iEndColumn);
        }
        
        // Find a REDocument representing document. If we cannot find one, we will add a new entry into
        // the REDocument array.
        //
        internal int FindDocument(ISymbolDocumentWriter document)
        {
            int         i;
            
            // This is an optimization. The chance that the previous line is coming from the same
            // document is very high.
            //                
            if (m_iLastFound < m_DocumentCount && m_Documents[m_iLastFound] == document)
                return m_iLastFound;
                
            for (i = 0; i < m_DocumentCount; i++)
            {
                if (m_Documents[i].m_document == document)
                {
                    m_iLastFound = i;
                    return m_iLastFound;
                }
            }
            
            // cannot find an existing document so add one to the array                                       
            EnsureCapacity();
            m_iLastFound = m_DocumentCount;
            m_Documents[m_DocumentCount++] = new REDocument(document);
            return m_iLastFound;
        }

        /**************************
        *
        * Helper to ensure arrays are large enough
        *
        **************************/
        internal void EnsureCapacity()
        {
            if (m_DocumentCount == 0)
            {
                // First time. Allocate the arrays.
                m_Documents = new REDocument[InitialSize];
            }
            else if (m_DocumentCount == m_Documents.Length)
            {
                // the arrays are full. Enlarge the arrays
                REDocument[] temp = new REDocument [m_DocumentCount * 2];
                Array.Copy(m_Documents, temp, m_DocumentCount);
                m_Documents = temp;
            }
        }

        internal void EmitLineNumberInfo(ISymbolWriter symWriter)
        {
            for (int i = 0; i < m_DocumentCount; i++)
                m_Documents[i].EmitLineNumberInfo(symWriter);
        }

        internal int         m_DocumentCount;           // how many documents that we have right now
        internal REDocument[]  m_Documents;             // array of documents
        internal const int   InitialSize = 16;
        private  int         m_iLastFound;
    }


    /***************************
    *
    * This class tracks the line number info
    *
    ***************************/
    internal class REDocument
    {
        internal REDocument(ISymbolDocumentWriter document)
        {
            // initialize data variables
            m_iLineNumberCount = 0;
            m_document = document;
        }

        internal void AddLineNumberInfo(
            ISymbolDocumentWriter document,
            int             iOffset,
            int             iStartLine,
            int             iStartColumn,
            int             iEndLine,
            int             iEndColumn)
        {
            BCLDebug.Assert(document == m_document, "Bad document look up!");
            
            // make sure that arrays are large enough to hold addition info
            EnsureCapacity();
            
            m_iOffsets[m_iLineNumberCount] = iOffset;
            m_iLines[m_iLineNumberCount] = iStartLine;
            m_iColumns[m_iLineNumberCount] = iStartColumn;
            m_iEndLines[m_iLineNumberCount] = iEndLine;
            m_iEndColumns[m_iLineNumberCount] = iEndColumn;
            m_iLineNumberCount++;
        }

        /**************************
        *
        * Helper to ensure arrays are large enough
        *
        **************************/
        internal void EnsureCapacity()
        {
            if (m_iLineNumberCount == 0)
            {
                // First time. Allocate the arrays.
                m_iOffsets = new int[InitialSize];
                m_iLines = new int[InitialSize];
                m_iColumns = new int[InitialSize];
                m_iEndLines = new int[InitialSize];
                m_iEndColumns = new int[InitialSize];
            }
            else if (m_iLineNumberCount == m_iOffsets.Length)
            {
                
                 

                // the arrays are full. Enlarge the arrays
                int[] temp = new int [m_iLineNumberCount * 2];
                Array.Copy(m_iOffsets, temp, m_iLineNumberCount);
                m_iOffsets = temp;

                temp = new int [m_iLineNumberCount * 2];
                Array.Copy(m_iLines, temp, m_iLineNumberCount);
                m_iLines = temp;

                temp = new int [m_iLineNumberCount * 2];
                Array.Copy(m_iColumns, temp, m_iLineNumberCount);
                m_iColumns = temp;

                temp = new int [m_iLineNumberCount * 2];
                Array.Copy(m_iEndLines, temp, m_iLineNumberCount);
                m_iEndLines = temp;

                temp = new int [m_iLineNumberCount * 2];
                Array.Copy(m_iEndColumns, temp, m_iLineNumberCount);
                m_iEndColumns = temp;
            }
        }

        internal void EmitLineNumberInfo(ISymbolWriter symWriter)
        {
            int[]       iOffsetsTemp;
            int[]       iLinesTemp;
            int[]       iColumnsTemp;
            int[]       iEndLinesTemp;
            int[]       iEndColumnsTemp;

            if (m_iLineNumberCount == 0)
                return;
            // reduce the array size to be exact
            iOffsetsTemp = new int [m_iLineNumberCount];
            Array.Copy(m_iOffsets, iOffsetsTemp, m_iLineNumberCount);

            iLinesTemp = new int [m_iLineNumberCount];
            Array.Copy(m_iLines, iLinesTemp, m_iLineNumberCount);

            iColumnsTemp = new int [m_iLineNumberCount];
            Array.Copy(m_iColumns, iColumnsTemp, m_iLineNumberCount);

            iEndLinesTemp = new int [m_iLineNumberCount];
            Array.Copy(m_iEndLines, iEndLinesTemp, m_iLineNumberCount);

            iEndColumnsTemp = new int [m_iLineNumberCount];
            Array.Copy(m_iEndColumns, iEndColumnsTemp, m_iLineNumberCount);

            symWriter.DefineSequencePoints(m_document, iOffsetsTemp, iLinesTemp, iColumnsTemp, iEndLinesTemp, iEndColumnsTemp); 
        }

        internal int[]       m_iOffsets;                 // array of offsets
        internal int[]       m_iLines;                   // array of offsets
        internal int[]       m_iColumns;                 // array of offsets
        internal int[]       m_iEndLines;                // array of offsets
        internal int[]       m_iEndColumns;              // array of offsets
        internal ISymbolDocumentWriter m_document;       // The ISymbolDocumentWriter that this REDocument is tracking.
        internal int         m_iLineNumberCount;         // how many entries in the arrays are occupied
        internal const int InitialSize = 16;
    }       // end of REDocument
}




/*
methDef = methodInfo.GetGenericMethodDefinition();
int tk = methDef.MetadataTokenInternal;
foreach (MethodInfo m in methDef.DeclaringType.GetGenericTypeDefinition().GetMethods())
{
    if (m.MetadataTokenInternal == tk)
    {
        methDef = m;
        break;
    }
}
*/

