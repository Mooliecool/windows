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

#define DEBUG_PTRS

namespace System 
{
    using System;
    using System.Reflection;
    using System.Reflection.Emit;
    using System.Runtime.ConstrainedExecution;
    using System.Diagnostics;
    using System.Runtime.Serialization;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;
    using System.Text;
    using System.Globalization;
    using Microsoft.Win32.SafeHandles;
    using StackCrawlMark = System.Threading.StackCrawlMark;
    
    internal unsafe struct MethodDescChunkHandle
    {
        private IntPtr m_ptr;

        internal MethodDescChunkHandle(void* ptr)
        {
            m_ptr = new IntPtr(ptr);
        }

        internal bool IsNullHandle() 
        { 
            return m_ptr.ToPointer() == null; 
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _GetNextMethodDescChunk();
        internal MethodDescChunkHandle GetNextMethodDescChunk()
        {
            return new MethodDescChunkHandle(_GetNextMethodDescChunk());
        }
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _GetMethodAt(int index);
        internal RuntimeMethodHandle GetMethodAt(int index)
        {
            return new RuntimeMethodHandle(_GetMethodAt(index));
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern int GetMethodCount();
    }

    internal struct FastArrayHandle
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void SetValueAt(Array target, int index, object element);
/*        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]            
        internal static extern void CopyArray (Array soruce, Array destination);
*/        
    }

    [Serializable()]
    [System.Runtime.InteropServices.ComVisible(true)]
    public unsafe struct RuntimeTypeHandle : ISerializable
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool IsInstanceOfType(Object o);

        internal unsafe static IntPtr GetTypeHelper(IntPtr th, IntPtr pGenericArgs, int cGenericArgs, IntPtr pModifiers, int cModifiers)
        {
            RuntimeTypeHandle typeHandle = new RuntimeTypeHandle(th.ToPointer());
            Type type = typeHandle.GetRuntimeType();
            if (type == null)
                return th;

            if (cGenericArgs > 0)
            {
                Type[] genericArgs = new Type[cGenericArgs];
                void** arGenericArgs = (void**)pGenericArgs.ToPointer();
                for (int i = 0; i < genericArgs.Length; i++)
                {
                    RuntimeTypeHandle genericArg = new RuntimeTypeHandle((void*)Marshal.ReadIntPtr((IntPtr)arGenericArgs, i * sizeof(void*)));
                    genericArgs[i] = Type.GetTypeFromHandle(genericArg);
                    
                    if (genericArgs[i] == null)
                        return (IntPtr)0;
                }

                type = type.MakeGenericType(genericArgs);
            }

            if (cModifiers > 0)
            {
                int* arModifiers = (int*)pModifiers.ToPointer();
                for(int i = 0; i < cModifiers; i++)
                {
                    if ((CorElementType)Marshal.ReadInt32((IntPtr)arModifiers, i * sizeof(int)) == CorElementType.Ptr)
                        type = type.MakePointerType();
                    
                    else if ((CorElementType)Marshal.ReadInt32((IntPtr)arModifiers, i * sizeof(int)) == CorElementType.ByRef)
                        type = type.MakeByRefType();

                    else if ((CorElementType)Marshal.ReadInt32((IntPtr)arModifiers, i * sizeof(int)) == CorElementType.SzArray)
                        type = type.MakeArrayType();

                    else
                        type = type.MakeArrayType(Marshal.ReadInt32((IntPtr)arModifiers, ++i * sizeof(int)));
                }
            }
            
            return type.GetTypeHandleInternal().Value;
        }

        public static bool operator ==(RuntimeTypeHandle left, object right) { return left.Equals(right); }
        
        public static bool operator ==(object left, RuntimeTypeHandle right) { return right.Equals(left); }
        
        public static bool operator !=(RuntimeTypeHandle left, object right) { return !left.Equals(right); }

        public static bool operator !=(object left, RuntimeTypeHandle right) { return !right.Equals(left); }
/*
        internal new Type GetType() 
        { 
            Console.WriteLine("RuntimeTypeHandle.GetType() called"); 
            throw new Exception("RuntimeTypeHandle.GetType() called. Chances are you want to call GetRuntimeType()");
            return base.GetType(); 
        }
*/
        private const int MAX_CLASS_NAME = 1024;
        internal static readonly RuntimeTypeHandle EmptyHandle = new RuntimeTypeHandle(null);

        // this is the TypeHandle/MethodTable for the type
        private IntPtr m_ptr;

        public override int GetHashCode()
        {
            return m_ptr.GetHashCode();
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public override bool Equals(object obj)
        {
            if(!(obj is RuntimeTypeHandle))
                return false;

            RuntimeTypeHandle handle =(RuntimeTypeHandle)obj;
            return handle.m_ptr == m_ptr;
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public bool Equals(RuntimeTypeHandle handle)
        {
            return handle.m_ptr == m_ptr;
        }

        public IntPtr Value { get { return m_ptr; } }

        internal RuntimeTypeHandle(void* rth) 
        {
            m_ptr = new IntPtr(rth);
        }

        internal bool IsNullHandle() 
        { 
            return m_ptr.ToPointer() == null; 
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
	    internal static extern Object CreateInstance(RuntimeType type, bool publicOnly, bool noCheck, ref bool canBeCached, ref RuntimeMethodHandle ctor, ref bool bNeedSecurityCheck);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
	    internal extern Object CreateCaInstance(RuntimeMethodHandle ctor);

	    [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern Object Allocate();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern Object CreateInstanceForAnotherGenericParameter(Type genericParameter);
        
        internal RuntimeType GetRuntimeType()
        {            
            if(!IsNullHandle())
                return(System.RuntimeType)Type.GetTypeFromHandle(this);
            else 
                return null;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern CorElementType GetCorElementType(); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _GetAssemblyHandle();
        internal AssemblyHandle GetAssemblyHandle()
        {
            return new AssemblyHandle(_GetAssemblyHandle());
        }
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        private extern void* _GetModuleHandle();

        [CLSCompliant(false)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public ModuleHandle GetModuleHandle()
        {
            return new ModuleHandle(_GetModuleHandle());
        }
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _GetBaseTypeHandle();
        internal RuntimeTypeHandle GetBaseTypeHandle()
        {
            return new RuntimeTypeHandle(_GetBaseTypeHandle());
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern TypeAttributes GetAttributes(); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _GetElementType();
        internal RuntimeTypeHandle GetElementType()
        {
            return new RuntimeTypeHandle(_GetElementType());
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern RuntimeTypeHandle GetCanonicalHandle();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern int GetArrayRank(); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern int GetToken(); 
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _GetMethodAt(int slot);
        internal RuntimeMethodHandle GetMethodAt(int slot)
        {
            return new RuntimeMethodHandle(_GetMethodAt(slot));
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _GetMethodDescChunk();
        internal MethodDescChunkHandle GetMethodDescChunk()
        {
            return new MethodDescChunkHandle(_GetMethodDescChunk());
        }
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool GetFields(int** result, int* count);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern RuntimeTypeHandle[] GetInterfaces();
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern RuntimeTypeHandle[] GetConstraints();
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern IntPtr GetGCHandle(GCHandleType type); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern void FreeGCHandle(IntPtr handle); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _GetMethodFromToken(int tkMethodDef);
        internal RuntimeMethodHandle GetMethodFromToken(int tkMethodDef)
        {
            return new RuntimeMethodHandle(_GetMethodFromToken(tkMethodDef));
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern int GetNumVtableSlots(); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern int GetInterfaceMethodSlots();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern int GetFirstSlotForInterface(IntPtr interfaceHandle);
        internal int GetFirstSlotForInterface(RuntimeTypeHandle interfaceHandle)
        {
            return GetFirstSlotForInterface(interfaceHandle.Value);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern int GetInterfaceMethodImplementationSlot(IntPtr interfaceHandle, IntPtr interfaceMethodHandle);
        internal int GetInterfaceMethodImplementationSlot(RuntimeTypeHandle interfaceHandle, RuntimeMethodHandle interfaceMethodHandle)
        {
            return GetInterfaceMethodImplementationSlot(interfaceHandle.Value, interfaceMethodHandle.Value);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool IsComObject(bool isGenericCOM); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool IsContextful(); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool IsVisible(); 
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool _IsVisibleFromModule(IntPtr module); 
        internal bool IsVisibleFromModule(ModuleHandle module)
        {
            return _IsVisibleFromModule((IntPtr)module.Value);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool HasProxyAttribute(); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern string ConstructName(bool nameSpace, bool fullInst, bool assembly);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _GetUtf8Name();

        internal Utf8String GetUtf8Name()
        {
            return new Utf8String(_GetUtf8Name());
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern bool CanCastTo(IntPtr target);
        internal bool CanCastTo(RuntimeTypeHandle target)
        {
            return CanCastTo(target.Value);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern RuntimeTypeHandle GetDeclaringType();
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _GetDeclaringMethod();
        internal RuntimeMethodHandle GetDeclaringMethod()
        {
            return new RuntimeMethodHandle(_GetDeclaringMethod());
        }
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _GetDefaultConstructor();        
        internal RuntimeMethodHandle GetDefaultConstructor()       
        {
            return new RuntimeMethodHandle(_GetDefaultConstructor());
        }
       
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool SupportsInterface(object target);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void* _GetTypeByName(string name, bool throwOnError, bool ignoreCase, bool reflectionOnly, ref StackCrawlMark stackMark, bool loadTypeFromPartialName);
        internal static RuntimeTypeHandle GetTypeByName(string name, bool throwOnError, bool ignoreCase, bool reflectionOnly, ref StackCrawlMark stackMark)
        {
            if (name == null || name.Length == 0)
            {
                if (throwOnError)
                    throw new TypeLoadException(Environment.GetResourceString("Arg_TypeLoadNullStr"));

                return new RuntimeTypeHandle();
            }
            
            return new RuntimeTypeHandle(_GetTypeByName(name, throwOnError, ignoreCase, reflectionOnly, ref stackMark, false));
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void* _GetTypeByNameUsingCARules(string name, IntPtr scope);
        internal static Type GetTypeByNameUsingCARules(string name, Module scope)
        {
            if (name == null || name.Length == 0)
                throw new ArgumentException(); 

            return new RuntimeTypeHandle(_GetTypeByNameUsingCARules(name, (IntPtr)scope.GetModuleHandle().Value)).GetRuntimeType();
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern RuntimeTypeHandle[] GetInstantiation(); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _Instantiate(RuntimeTypeHandle[] inst);
        internal RuntimeTypeHandle Instantiate(RuntimeTypeHandle[] inst)
        {
            return new RuntimeTypeHandle(_Instantiate(inst));
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _MakeArray(int rank);
        internal RuntimeTypeHandle MakeArray(int rank)
        {
            return new RuntimeTypeHandle(_MakeArray(rank));
        }
/*
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void* _GetCallersType(ref StackCrawlMark stackMark);
        internal static RuntimeTypeHandle GetCallersType(ref StackCrawlMark stackMark)
        {
            return new RuntimeTypeHandle(_GetCallersType(ref stackMark));
        }
*/
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _MakeSZArray();
        internal RuntimeTypeHandle MakeSZArray()
        {
            return new RuntimeTypeHandle(_MakeSZArray());
        }
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _MakeByRef();
        internal RuntimeTypeHandle MakeByRef()
        {
            return new RuntimeTypeHandle(_MakeByRef());
        }
       
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _MakePointer();
        internal RuntimeTypeHandle MakePointer()
        {
            return new RuntimeTypeHandle(_MakePointer());
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool HasInstantiation();
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _GetGenericTypeDefinition();
        internal RuntimeTypeHandle GetGenericTypeDefinition()
        {
            return new RuntimeTypeHandle(_GetGenericTypeDefinition());
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool IsGenericTypeDefinition(); 
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool IsGenericVariable(); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern int GetGenericVariableIndex(); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool ContainsGenericVariables();
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool SatisfiesConstraints(RuntimeTypeHandle[] typeContext, RuntimeTypeHandle[] methodContext, RuntimeTypeHandle toType);
        
        private RuntimeTypeHandle(SerializationInfo info, StreamingContext context)
        {
            if(info == null) 
                throw new ArgumentNullException("info");

            Type m = (RuntimeType)info.GetValue("TypeObj", typeof(RuntimeType));

            m_ptr = m.TypeHandle.Value;

            if(m_ptr.ToPointer() == null)
                throw new SerializationException(Environment.GetResourceString("Serialization_InsufficientState"));
        }

        public void GetObjectData(SerializationInfo info, StreamingContext context) 
        {
            if(info == null) 
                throw new ArgumentNullException("info");

            if(m_ptr.ToPointer() == null)
                throw new SerializationException(Environment.GetResourceString("Serialization_InvalidFieldState")); 

            RuntimeType type = (RuntimeType)Type.GetTypeFromHandle(this); 

            info.AddValue("TypeObj", type, typeof(RuntimeType));
        }
    }                                       

    [Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public unsafe struct RuntimeMethodHandle : ISerializable
    {
        internal static RuntimeMethodHandle EmptyHandle { get { return new RuntimeMethodHandle(null); } }
        
        private IntPtr m_ptr;
        
        internal RuntimeMethodHandle(void* pMethod) 
        {
            m_ptr = new IntPtr(pMethod);
        }
        
        internal RuntimeMethodHandle(IntPtr pMethod) 
        {
            m_ptr = pMethod;
        }
        
        // ISerializable interface
        private RuntimeMethodHandle(SerializationInfo info, StreamingContext context)
        {
            if(info == null)
                throw new ArgumentNullException("info");
            
            MethodInfo m =(RuntimeMethodInfo)info.GetValue("MethodObj", typeof(RuntimeMethodInfo));

            m_ptr = m.MethodHandle.Value;
            
            if(m_ptr.ToPointer() == null)
                throw new SerializationException(Environment.GetResourceString("Serialization_InsufficientState"));
        }

        public void GetObjectData(SerializationInfo info, StreamingContext context) 
        {
            if (info == null) 
                throw new ArgumentNullException("info");
            
            if (m_ptr.ToPointer() == null)
                throw new SerializationException(Environment.GetResourceString("Serialization_InvalidFieldState"));
            
            RuntimeMethodInfo methodInfo = (RuntimeMethodInfo)RuntimeType.GetMethodBase(this); 
            
            info.AddValue("MethodObj", methodInfo, typeof(RuntimeMethodInfo));
        }       
        
        public IntPtr Value { get {  return m_ptr; } }

        public override int GetHashCode()
        {
            return m_ptr.GetHashCode();
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public override bool Equals(object obj)
        {
            if (!(obj is RuntimeMethodHandle))
                return false;

            RuntimeMethodHandle handle = (RuntimeMethodHandle)obj;

            return handle.m_ptr == m_ptr;
        }

        public static bool operator ==(RuntimeMethodHandle left, RuntimeMethodHandle right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(RuntimeMethodHandle left, RuntimeMethodHandle right)
        {
            return !left.Equals(right);
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public bool Equals(RuntimeMethodHandle handle)
        {
            return handle.m_ptr == m_ptr;
        }

        internal bool IsNullHandle() 
        { 
            return m_ptr.ToPointer() == null; 
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern IntPtr GetFunctionPointer();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private unsafe extern void _CheckLinktimeDemands(void* module, int metadataToken);
        internal void CheckLinktimeDemands(Module module, int metadataToken)
        {
            _CheckLinktimeDemands((void*)module.ModuleHandle.Value, metadataToken);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private unsafe extern bool _IsVisibleFromModule(void* source);
        internal bool IsVisibleFromModule(Module source)
        {
            return _IsVisibleFromModule(source.ModuleHandle.Value);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private unsafe extern bool _IsVisibleFromType(IntPtr source);
        internal bool IsVisibleFromType(RuntimeTypeHandle source)
        {
            return _IsVisibleFromType(source.Value);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void* _GetCurrentMethod(ref StackCrawlMark stackMark);
        internal static RuntimeMethodHandle GetCurrentMethod(ref StackCrawlMark stackMark)
        {
            return new RuntimeMethodHandle(_GetCurrentMethod(ref stackMark));
        }
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern MethodAttributes GetAttributes();
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern MethodImplAttributes GetImplAttributes();
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern string ConstructInstantiation();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern RuntimeTypeHandle GetDeclaringType();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern int GetSlot();
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern int GetMethodDef();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern string GetName();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void* _GetUtf8Name();

        internal Utf8String GetUtf8Name()
        {
            return new Utf8String(_GetUtf8Name());
        }

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern object _InvokeMethodFast(
            object target, object[] arguments, ref SignatureStruct sig, MethodAttributes methodAttributes, RuntimeTypeHandle typeOwner);

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        internal object InvokeMethodFast(object target, object[] arguments, Signature sig, MethodAttributes methodAttributes, RuntimeTypeHandle typeOwner)
        {
              SignatureStruct _sig = sig.m_signature;
              object obj1 = _InvokeMethodFast(target, arguments, ref _sig, methodAttributes, typeOwner);
              sig.m_signature = _sig;
              return obj1;
        }
       
        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        private extern object _InvokeConstructor(object[] args, ref SignatureStruct signature, IntPtr declaringType);

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        internal object InvokeConstructor (object[] args, SignatureStruct signature, RuntimeTypeHandle declaringType)
        {
            return _InvokeConstructor(args, ref signature, declaringType.Value);
        }

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        private extern void _SerializationInvoke(
            Object target, ref SignatureStruct declaringTypeSig, SerializationInfo info, StreamingContext context);

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        internal void SerializationInvoke(
            Object target, SignatureStruct declaringTypeSig, SerializationInfo info, StreamingContext context)
        {
            _SerializationInvoke(target, ref declaringTypeSig, info, context);
        }


        [MethodImplAttribute (MethodImplOptions.InternalCall)]        
        internal extern bool IsILStub();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern RuntimeTypeHandle[] GetMethodInstantiation(); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern bool HasMethodInstantiation(); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern RuntimeMethodHandle GetInstantiatingStub(RuntimeTypeHandle declaringTypeHandle, RuntimeTypeHandle[] methodInstantiation);
        
        internal RuntimeMethodHandle GetInstantiatingStubIfNeeded(RuntimeTypeHandle declaringTypeHandle)
        {
            return GetInstantiatingStub(declaringTypeHandle, null);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern RuntimeMethodHandle GetMethodFromCanonical(RuntimeTypeHandle declaringTypeHandle);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern bool IsGenericMethodDefinition(); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        private extern void* _GetTypicalMethodDefinition(); 
        internal RuntimeMethodHandle GetTypicalMethodDefinition()
        {
            return new RuntimeMethodHandle(_GetTypicalMethodDefinition());
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        private extern void* _StripMethodInstantiation(); 
        internal RuntimeMethodHandle StripMethodInstantiation()
        {
            return new RuntimeMethodHandle(_StripMethodInstantiation());
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern bool IsDynamicMethod();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern Resolver GetResolver(); 
    
        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern void Destroy(); 

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern MethodBody _GetMethodBody(IntPtr declaringType);
        internal MethodBody GetMethodBody(RuntimeTypeHandle declaringType)
        {
            return _GetMethodBody(declaringType.Value);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern bool IsConstructor();
    }

    [Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public unsafe struct RuntimeFieldHandle : ISerializable
    {
        private IntPtr m_ptr;

        internal RuntimeFieldHandle(void* pFieldHandle) 
        {
            m_ptr = new IntPtr(pFieldHandle);
        }
        
        public IntPtr Value { get { return m_ptr; } }

        internal bool IsNullHandle() 
        { 
            return m_ptr.ToPointer() == null; 
        }

        public override  int GetHashCode()
        {
            return m_ptr.GetHashCode();
        }
        
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public override  bool Equals(object obj)
        {
            if (!(obj is RuntimeFieldHandle))
                return false;

            RuntimeFieldHandle handle = (RuntimeFieldHandle)obj;

            return handle.m_ptr == m_ptr;
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public unsafe bool Equals(RuntimeFieldHandle handle)
        {
            return handle.m_ptr == m_ptr;
        }

        public static bool operator ==(RuntimeFieldHandle left, RuntimeFieldHandle right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(RuntimeFieldHandle left, RuntimeFieldHandle right)
        {
            return !left.Equals(right);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern String GetName(); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        private extern unsafe void* _GetUtf8Name(); 

        internal unsafe Utf8String GetUtf8Name() { return new Utf8String(_GetUtf8Name()); }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern FieldAttributes GetAttributes(); 
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern RuntimeTypeHandle GetApproxDeclaringType();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern int GetToken(); 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern Object GetValue(Object instance, RuntimeTypeHandle fieldType, RuntimeTypeHandle declaringType, ref bool domainInitialized);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern Object GetValueDirect(RuntimeTypeHandle fieldType, TypedReference obj, RuntimeTypeHandle contextType);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern void SetValue(Object obj, Object value, RuntimeTypeHandle fieldType, FieldAttributes fieldAttr, RuntimeTypeHandle declaringType, ref bool domainInitialized);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern void SetValueDirect(RuntimeTypeHandle fieldType, TypedReference obj, Object value, RuntimeTypeHandle contextType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern RuntimeFieldHandle GetStaticFieldForGenericType(RuntimeTypeHandle declaringType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern bool AcquiresContextFromThis();

        // ISerializable interface
        private RuntimeFieldHandle(SerializationInfo info, StreamingContext context)
        {
            if(info==null)
                throw new ArgumentNullException("info");
            
            FieldInfo f =(RuntimeFieldInfo) info.GetValue("FieldObj", typeof(RuntimeFieldInfo));
            
            if( f == null)
                throw new SerializationException(Environment.GetResourceString("Serialization_InsufficientState"));

            m_ptr = f.FieldHandle.Value;

            if (m_ptr.ToPointer() == null)
                throw new SerializationException(Environment.GetResourceString("Serialization_InsufficientState"));
        }

        public void GetObjectData(SerializationInfo info, StreamingContext context) 
        {
            if (info == null) 
                throw new ArgumentNullException("info");
            
            if (m_ptr.ToPointer() == null)
                throw new SerializationException(Environment.GetResourceString("Serialization_InvalidFieldState"));

            RuntimeFieldInfo fldInfo = (RuntimeFieldInfo)RuntimeType.GetFieldInfo(this); 
            
            info.AddValue("FieldObj",fldInfo, typeof(RuntimeFieldInfo));
        }
    }

    internal unsafe struct AssemblyHandle
    {        
        #region Private Data Members
        private IntPtr m_ptr;
        #endregion

        internal void* Value { get { return m_ptr.ToPointer(); } }

        #region Constructor
        internal AssemblyHandle(void* pAssembly)
        {
            m_ptr = new IntPtr(pAssembly);
        }
        #endregion

        #region Internal Members
                
        public override  int GetHashCode()
        {
            return m_ptr.GetHashCode();
        }

        public override  bool Equals(object obj)
        {
            if (!(obj is AssemblyHandle))
                return false;

            AssemblyHandle handle = (AssemblyHandle)obj;

            return handle.m_ptr == m_ptr;
        }

        public unsafe bool Equals(AssemblyHandle handle)
        {
            return handle.m_ptr == m_ptr;
        }
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern Assembly GetAssembly();
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        private extern void* _GetManifestModule();
        internal ModuleHandle GetManifestModule()
        {
            return new ModuleHandle(_GetManifestModule());
        }
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        private extern bool _AptcaCheck(IntPtr sourceAssembly);
        internal bool AptcaCheck(AssemblyHandle sourceAssembly)
        {
            return _AptcaCheck((IntPtr)sourceAssembly.Value);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern int GetToken();
        #endregion
    }

[System.Runtime.InteropServices.ComVisible(true)]
    public unsafe struct ModuleHandle
    {
        #region Public Static Members
        public static readonly ModuleHandle EmptyHandle = new ModuleHandle(null);
        #endregion

        #region Private Data Members
        private IntPtr m_ptr;
        #endregion
    
        #region Constructor
        internal ModuleHandle(void* pModule) 
        {
            m_ptr = new IntPtr(pModule);
        }
        #endregion

        #region Internal FCalls
        internal void* Value { get { return m_ptr.ToPointer(); } }

        internal bool IsNullHandle() 
        { 
            return m_ptr.ToPointer() == null; 
        }
        
        public override  int GetHashCode()
        {           
            return m_ptr.GetHashCode();
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public override bool Equals(object obj)
        {
            if (!(obj is ModuleHandle))
                return false;

            ModuleHandle handle = (ModuleHandle)obj;

            return handle.m_ptr == m_ptr;
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public unsafe bool Equals(ModuleHandle handle)
        {
            return handle.m_ptr == m_ptr;
        }

        public static bool operator ==(ModuleHandle left, ModuleHandle right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(ModuleHandle left, ModuleHandle right)
        {
            return !left.Equals(right);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern RuntimeTypeHandle GetCallerType(ref StackCrawlMark stackMark);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void* GetDynamicMethod(void* module, string name, byte[] sig, Resolver resolver);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern int GetToken();

        internal static RuntimeTypeHandle[] CopyRuntimeTypeHandles(RuntimeTypeHandle[] inHandles)
        {
            if (inHandles == null || inHandles.Length == 0) return inHandles;
            RuntimeTypeHandle[] outHandles = new RuntimeTypeHandle[inHandles.Length];
            for (int i=0; i < inHandles.Length; i++)
            {
                outHandles[i] = inHandles[i];
            }
            return outHandles;
        }

        private void ValidateModulePointer()
        {
            // Make sure we have a valid Module to resolve against.
            if (IsNullHandle())
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NullModuleHandle"));
        }

        // SQL-CLR LKG9 Compiler dependency
        public RuntimeTypeHandle GetRuntimeTypeHandleFromMetadataToken(int typeToken) { return ResolveTypeHandle(typeToken); }
        public RuntimeTypeHandle ResolveTypeHandle(int typeToken) { return ResolveTypeHandle(typeToken, null, null); }
        public RuntimeTypeHandle ResolveTypeHandle(int typeToken, RuntimeTypeHandle[] typeInstantiationContext, RuntimeTypeHandle[] methodInstantiationContext)
        {
            ValidateModulePointer();
            
            // defensive copy to be sure array is not mutated from the outside during processing
            typeInstantiationContext = CopyRuntimeTypeHandles(typeInstantiationContext);
            methodInstantiationContext = CopyRuntimeTypeHandles(methodInstantiationContext);
            
            unsafe
            {
                if (typeInstantiationContext == null || typeInstantiationContext.Length == 0) 
                {
                    if (methodInstantiationContext == null || methodInstantiationContext.Length == 0) 
                    {
                        // no context
                        return ResolveType(typeToken, null, 0, null, 0);
                    }
                    // only method context available
                    int instCount = methodInstantiationContext.Length;
                    fixed (RuntimeTypeHandle* instArgs = methodInstantiationContext) {
                        return ResolveType(typeToken, null, 0, instArgs, instCount);
                    }
                }
                if (methodInstantiationContext == null || methodInstantiationContext.Length == 0) 
                {
                    // only type context available
                    int instCount = typeInstantiationContext.Length;
                    fixed (RuntimeTypeHandle* instArgs = typeInstantiationContext) {
                        return ResolveType(typeToken, instArgs, instCount, null, 0);
                    }
                }
                // pin both
                int typeInstCount = typeInstantiationContext.Length;
                int methodInstCount = methodInstantiationContext.Length;
                fixed (RuntimeTypeHandle* typeInstArgs = typeInstantiationContext, methodInstArgs = methodInstantiationContext) {
                    return ResolveType(typeToken, typeInstArgs, typeInstCount, methodInstArgs, methodInstCount);
                }
            }
        }
        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        private extern unsafe RuntimeTypeHandle ResolveType(int typeToken, 
                                                            RuntimeTypeHandle* typeInstArgs, 
                                                            int typeInstCount,
                                                            RuntimeTypeHandle* methodInstArgs,
                                                            int methodInstCount);

        // SQL-CLR LKG9 Compiler dependency
        public RuntimeMethodHandle GetRuntimeMethodHandleFromMetadataToken(int methodToken) { return ResolveMethodHandle(methodToken); }
        public RuntimeMethodHandle ResolveMethodHandle(int methodToken) { return ResolveMethodHandle(methodToken, null, null); }
        public RuntimeMethodHandle ResolveMethodHandle(int methodToken, RuntimeTypeHandle[] typeInstantiationContext, RuntimeTypeHandle[] methodInstantiationContext)
        {
            ValidateModulePointer();
            
            // defensive copy to be sure array is not mutated from the outside during processing
            typeInstantiationContext = CopyRuntimeTypeHandles(typeInstantiationContext);
            methodInstantiationContext = CopyRuntimeTypeHandles(methodInstantiationContext);
            
            unsafe
            {
                if (typeInstantiationContext == null || typeInstantiationContext.Length == 0) 
                {
                    if (methodInstantiationContext == null || methodInstantiationContext.Length == 0) 
                    {
                        // no context
                        return ResolveMethod(methodToken, null, 0, null, 0);
                    }
                    // only method context available
                    int instCount = methodInstantiationContext.Length;
                    fixed (RuntimeTypeHandle* instArgs = methodInstantiationContext) {
                        return ResolveMethod(methodToken, null, 0, instArgs, instCount);
                    }
                }
                if (methodInstantiationContext == null || methodInstantiationContext.Length == 0) 
                {
                    // only type context available
                    int instCount = typeInstantiationContext.Length;
                    fixed (RuntimeTypeHandle* instArgs = typeInstantiationContext) {
                        return ResolveMethod(methodToken, instArgs, instCount, null, 0);
                    }
                }
                // pin both
                int typeInstCount = typeInstantiationContext.Length;
                int methodInstCount = methodInstantiationContext.Length;
                fixed (RuntimeTypeHandle* typeInstArgs = typeInstantiationContext, methodInstArgs = methodInstantiationContext) {
                    return ResolveMethod(methodToken, typeInstArgs, typeInstCount, methodInstArgs, methodInstCount);
                }
            }
        }
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern RuntimeMethodHandle ResolveMethod(int methodToken, 
                                                        RuntimeTypeHandle* typeInstArgs, 
                                                        int typeInstCount,
                                                        RuntimeTypeHandle* methodInstArgs,
                                                        int methodInstCount);

        // SQL-CLR LKG9 Compiler dependency
        public RuntimeFieldHandle GetRuntimeFieldHandleFromMetadataToken(int fieldToken) { return ResolveFieldHandle(fieldToken); }
        public RuntimeFieldHandle ResolveFieldHandle(int fieldToken) { return ResolveFieldHandle(fieldToken, null, null); }
        public RuntimeFieldHandle ResolveFieldHandle(int fieldToken, RuntimeTypeHandle[] typeInstantiationContext, RuntimeTypeHandle[] methodInstantiationContext)
        {
            ValidateModulePointer();
            
            // defensive copy to be sure array is not mutated from the outside during processing
            typeInstantiationContext = CopyRuntimeTypeHandles(typeInstantiationContext);
            methodInstantiationContext = CopyRuntimeTypeHandles(methodInstantiationContext);
            
            unsafe
            {
                if (typeInstantiationContext == null || typeInstantiationContext.Length == 0) 
                {
                    if (methodInstantiationContext == null || methodInstantiationContext.Length == 0) 
                    {
                        // no context
                        return ResolveField(fieldToken, null, 0, null, 0);
                    }
                    // only method context available
                    int instCount = methodInstantiationContext.Length;
                    fixed (RuntimeTypeHandle* instArgs = methodInstantiationContext) {
                        return ResolveField(fieldToken, null, 0, instArgs, instCount);
                    }
                }
                if (methodInstantiationContext == null || methodInstantiationContext.Length == 0) 
                {
                    // only type context available
                    int instCount = typeInstantiationContext.Length;
                    fixed (RuntimeTypeHandle* instArgs = typeInstantiationContext) {
                        return ResolveField(fieldToken, instArgs, instCount, null, 0);
                    }
                }
                // pin both
                int typeInstCount = typeInstantiationContext.Length;
                int methodInstCount = methodInstantiationContext.Length;
                fixed (RuntimeTypeHandle* typeInstArgs = typeInstantiationContext, methodInstArgs = methodInstantiationContext) {
                    return ResolveField(fieldToken, typeInstArgs, typeInstCount, methodInstArgs, methodInstCount);
                }
            }
        }
        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        private extern RuntimeFieldHandle ResolveField(int fieldToken, 
                                                      RuntimeTypeHandle* typeInstArgs, 
                                                      int typeInstCount,
                                                      RuntimeTypeHandle* methodInstArgs,
                                                      int methodInstCount);
        
//        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
//        private extern void* _GetAssemblyHandle();
//        internal AssemblyHandle GetAssemblyHandle()
//        {
//            return new AssemblyHandle(_GetAssemblyHandle());
//        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern Module GetModule();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern void* _GetModuleTypeHandle();
        internal RuntimeTypeHandle GetModuleTypeHandle()
        {
            return new RuntimeTypeHandle(_GetModuleTypeHandle());
        }        
 
        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern void _GetPEKind(out int peKind, out int machine);
   
        // making this internal, used by Module.GetPEKind
        internal void GetPEKind(out PortableExecutableKinds peKind, out ImageFileMachine machine)
        {
            int _peKind;
            int _machine;

            _GetPEKind(out _peKind, out _machine);

            peKind = (PortableExecutableKinds)_peKind;
            machine = (ImageFileMachine)_machine;
        }
   
	[MethodImplAttribute(MethodImplOptions.InternalCall)]        
        internal extern int _GetMDStreamVersion();
        public int MDStreamVersion
        {
	    get {  return _GetMDStreamVersion(); }
        }
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern void * _GetMetadataImport(); 
        internal MetadataImport GetMetadataImport()
        {
            return new MetadataImport((IntPtr)_GetMetadataImport());
        }
        #endregion
    }


    internal unsafe class Signature
    {
        #region Implict Cast Operators
        public static implicit operator SignatureStruct(Signature pThis) { return pThis.m_signature; }
        #endregion

        #region Definitions
        internal enum MdSigCallingConvention : byte
        {
            Generics            = 0x10,
            HasThis             = 0x20,
            ExplicitThis        = 0x40,
            CallConvMask        = 0x0F,
            Default             = 0x00,
            C                   = 0x01,
            StdCall             = 0x02,
            ThisCall            = 0x03,
            FastCall            = 0x04,
            Vararg              = 0x05,
            Field               = 0x06,
            LocalSig            = 0x07,
            Property            = 0x08,
            Unmgd               = 0x09,
            GenericInst         = 0x0A,
            Max                 = 0x0B,
        }
        #endregion

        #region FCalls
        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        private static extern void _GetSignature(
            ref SignatureStruct signature, // This parameter must be passed as a pointer to the stack to prevent GC holes
            void* pCorSig, int cCorSig,
            IntPtr fieldHandle, IntPtr methodHandle, IntPtr declaringTypeHandle);
        private static void GetSignature(
            ref SignatureStruct signature, // This parameter must be passed as a pointer to the stack to prevent GC holes
            void* pCorSig, int cCorSig,
            RuntimeFieldHandle fieldHandle, RuntimeMethodHandle methodHandle, RuntimeTypeHandle declaringTypeHandle)
        {
            _GetSignature(ref signature, pCorSig, cCorSig, fieldHandle.Value, methodHandle.Value, declaringTypeHandle.Value);
        }

        internal static void GetSignatureForDynamicMethod(
            ref SignatureStruct signature, // This parameter must be passed as a pointer to the stack to prevent GC holes
            RuntimeMethodHandle methodHandle)
        {
            _GetSignature(ref signature, null, 0, (IntPtr)0, methodHandle.Value, (IntPtr)0);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]       
        private static extern void GetCustomModifiers(
            ref SignatureStruct signature, int parameter, 
            out RuntimeTypeHandle[] required, out RuntimeTypeHandle[] optional); 
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]        
        private static extern bool CompareSig(ref SignatureStruct left, ref SignatureStruct right); 
        #endregion

        #region Private Data Members
        internal SignatureStruct m_signature;
        #endregion

        #region Constructors
        public Signature (            
            RuntimeMethodHandle method,
            RuntimeTypeHandle[] arguments,
            RuntimeTypeHandle returnType,
            CallingConventions callingConvention)
        {
            SignatureStruct pinnedSignature = new SignatureStruct(
                method, arguments, returnType, callingConvention);
            GetSignatureForDynamicMethod(ref pinnedSignature, method);
            m_signature = pinnedSignature;
        }

        public Signature(RuntimeMethodHandle methodHandle, RuntimeTypeHandle declaringTypeHandle)
        {
            SignatureStruct pinnedSignature = new SignatureStruct();
            GetSignature(ref pinnedSignature, null, 0, new RuntimeFieldHandle(null), methodHandle, declaringTypeHandle);
            m_signature = pinnedSignature;
        }

        public Signature(RuntimeFieldHandle fieldHandle, RuntimeTypeHandle declaringTypeHandle)
        {
            SignatureStruct pinnedSignature = new SignatureStruct();
            GetSignature(ref pinnedSignature, null, 0, fieldHandle, new RuntimeMethodHandle(null), declaringTypeHandle);
            m_signature = pinnedSignature;
        }

        public Signature(void* pCorSig, int cCorSig, RuntimeTypeHandle declaringTypeHandle)
        {
            SignatureStruct pinnedSignature = new SignatureStruct();
            GetSignature(ref pinnedSignature, pCorSig, cCorSig, new RuntimeFieldHandle(null), new RuntimeMethodHandle(null), declaringTypeHandle);
            m_signature = pinnedSignature;
        }
        #endregion

        #region Internal Members
        internal CallingConventions CallingConvention { get { return m_signature.m_managedCallingConvention &(CallingConventions)0x000000FF; } }
        internal RuntimeTypeHandle[] Arguments { get { return m_signature.m_arguments; } }
        internal RuntimeTypeHandle ReturnTypeHandle { get { return m_signature.m_returnTypeORfieldType; } }
        internal RuntimeTypeHandle FieldTypeHandle { get { return m_signature.m_returnTypeORfieldType; } }
        internal bool DiffSigs(object o)
        {
            if(o is Signature)
            {              
                Signature sig = o as Signature; 
                SignatureStruct pinnedSig = sig;
                SignatureStruct pinnedThis = this;
                return CompareSig(ref pinnedThis, ref pinnedSig);
            }

            return false;
        }

        public Type[] GetCustomModifiers(int position, bool required)
        {
            RuntimeTypeHandle[] req = null, opt = null;

            SignatureStruct pinnedSig = this;
            GetCustomModifiers(ref pinnedSig, position, out req, out opt);

            Type[] custMod = new Type[required ? req.Length : opt.Length];

            if(required)
            {
                for(int i = 0; i < custMod.Length; i++)
                    custMod[i] = req[i].GetRuntimeType();
            }
            else
            {
                for(int i = 0; i < custMod.Length; i++)
                    custMod[i] = opt[i].GetRuntimeType();
            }

            return custMod;
        }
        #endregion
    }

    //
    // WARNING: SignatureStructs should not be share between methods! Call targets are cachded!
    //
    internal unsafe struct SignatureStruct
    {
        #region Constructor
        public SignatureStruct (
            RuntimeMethodHandle method,
            RuntimeTypeHandle[] arguments,
            RuntimeTypeHandle returnType,
            CallingConventions callingConvention)
        {
            m_pMethod = method;
            m_arguments = arguments;
            m_returnTypeORfieldType = returnType;
            m_managedCallingConvention = callingConvention;
            m_sig = null;
            m_pCallTarget = null;
            m_csig = 0;
            m_numVirtualFixedArgs = 0;
            m_64bitpad = 0;
            m_declaringType = new RuntimeTypeHandle();
        }
        #endregion

        #region Pivate Data Members
        internal RuntimeTypeHandle[] m_arguments;
        internal void* m_sig; 
        internal void* m_pCallTarget;
        internal CallingConventions m_managedCallingConvention;
        internal int m_csig; 
        internal int m_numVirtualFixedArgs; 
        internal int m_64bitpad; 
        internal RuntimeMethodHandle m_pMethod; 
        internal RuntimeTypeHandle m_declaringType; 
        internal RuntimeTypeHandle m_returnTypeORfieldType;        
        #endregion
    }


    internal abstract class Resolver
    {
        internal struct CORINFO_EH_CLAUSE 
        {
            internal int Flags;
            internal int TryOffset;
            internal int TryLength;
            internal int HandlerOffset;
            internal int HandlerLength;
            internal int ClassTokenOrFilterOffset; 
        }

        internal const int COR_ILEXCEPTION_CLAUSE_CACHED_CLASS     = 0x10000000;
        internal const int COR_ILEXCEPTION_CLAUSE_MUST_CACHE_CLASS = 0x20000000;

        internal const int TypeToken    = 0x1;
        internal const int MethodToken  = 0x2;
        internal const int FieldToken   = 0x4;

        // ILHeader info
        internal abstract void GetJitContext(ref int securityControlFlags, ref RuntimeTypeHandle typeOwner);
        internal abstract byte[] GetCodeInfo(ref int stackSize, ref int initLocals, ref int EHCount);
        internal abstract byte[] GetLocalsSignature();
        internal abstract unsafe void GetEHInfo(int EHNumber, void* exception);
        internal abstract unsafe byte[] GetRawEHInfo();
        // token resolution
        internal abstract String GetStringLiteral(int token);
        internal abstract unsafe void* ResolveToken(int token);
        internal abstract unsafe int ParentToken(int token);
        internal abstract byte[] ResolveSignature(int token, int fromMethod);
        internal abstract int IsValidToken(int token);
        internal abstract unsafe void* GetInstantiationInfo(int token);
        // 
        internal abstract MethodInfo GetDynamicMethod();
    }

}
