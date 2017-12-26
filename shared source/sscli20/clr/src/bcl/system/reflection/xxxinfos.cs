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
using System.Runtime.ConstrainedExecution;
using System.Globalization;
using System.Threading;
using System.Diagnostics;
using System.Security.Permissions;
using System.Collections;
using System.Runtime.CompilerServices;
using System.Security;
using System.Text; 
using System.Reflection.Emit;
using System.Runtime.InteropServices;
using RuntimeTypeCache = System.RuntimeType.RuntimeTypeCache;
using CorElementType = System.Reflection.CorElementType;
using System.Runtime.Serialization;
using System.Reflection.Cache;
using MdToken = System.Reflection.MetadataToken;

namespace System.Reflection
{
    [Serializable()] 
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_MemberInfo))]
    [PermissionSetAttribute(SecurityAction.InheritanceDemand, Name="FullTrust")]    
    [System.Runtime.InteropServices.ComVisible(true)]
    public abstract class MemberInfo : ICustomAttributeProvider, _MemberInfo
    {
        #region Consts
        //
        // Invocation cached flags. Those are used in unmanaged code as well
        // so be careful if you change them
        //
        internal const uint INVOCATION_FLAGS_UNKNOWN = 0x00000000;
        internal const uint INVOCATION_FLAGS_INITIALIZED = 0x00000001; 
        // it's used for both method and field to signify that no access is allowed
        internal const uint INVOCATION_FLAGS_NO_INVOKE = 0x00000002;
        internal const uint INVOCATION_FLAGS_NEED_SECURITY = 0x00000004;
        internal const uint INVOCATION_FLAGS_NO_CTOR_INVOKE = 0x00000008;
        // because field and method are different we can reuse the same bits
        // method
        internal const uint INVOCATION_FLAGS_IS_CTOR = 0x00000010;
        internal const uint INVOCATION_FLAGS_RISKY_METHOD = 0x00000020;
        internal const uint INVOCATION_FLAGS_SECURITY_IMPOSED = 0x00000040;
        internal const uint INVOCATION_FLAGS_IS_DELEGATE_CTOR = 0x00000080;
        internal const uint INVOCATION_FLAGS_CONTAINS_STACK_POINTERS = 0x00000100;
        // field
        internal const uint INVOCATION_FLAGS_SPECIAL_FIELD = 0x00000010;
        internal const uint INVOCATION_FLAGS_FIELD_SPECIAL_CAST = 0x00000020;

        // temporary flag used for flagging invocation of method vs ctor
        // this flag never appears on the instance m_invocationFlag and is simply
        // passed down from within ConstructorInfo.Invoke()
        internal const uint INVOCATION_FLAGS_CONSTRUCTOR_INVOKE = 0x10000000;
        #endregion

        #region Constructor
        protected MemberInfo() {}
        #endregion

        #region Internal Methods
        internal virtual bool CacheEquals(object o) { throw new NotImplementedException(); } 
        #endregion

        #region Legacy Remoting Cache
        // The size of CachedData is accounted for by BaseObjectWithCachedData in object.h.
        // This member is currently being used by Remoting for caching remoting data. If you
        // need to cache data here, talk to the Remoting team to work out a mechanism, so that
        // both caching systems can happily work together.
        private InternalCache m_cachedData;

        internal InternalCache Cache
        {
            get
            {
                // This grabs an internal copy of m_cachedData and uses
                // that instead of looking at m_cachedData directly because
                // the cache may get cleared asynchronously.  This prevents
                // us from having to take a lock.
                InternalCache cache = m_cachedData;
                if (cache == null)
                {
                    cache = new InternalCache("MemberInfo");
                    InternalCache ret = Interlocked.CompareExchange(ref m_cachedData, cache, null);
                    if (ret != null)
                        cache = ret;
                    GC.ClearCache += new ClearCacheHandler(OnCacheClear);
                }
                return cache;
            } 
        }
    

        internal void OnCacheClear(Object sender, ClearCacheEventArgs cacheEventArgs)
        {
            m_cachedData = null;
        }
        #endregion

        #region Public Abstract\Virtual Members
        public abstract MemberTypes MemberType { get; }

        public abstract String Name { get; }

        public abstract Type DeclaringType { get; }

        public abstract Type ReflectedType { get; }

        public abstract Object[] GetCustomAttributes(bool inherit);

        public abstract Object[] GetCustomAttributes(Type attributeType, bool inherit);

        public abstract bool IsDefined(Type attributeType, bool inherit);
    
        public virtual int MetadataToken { get { throw new InvalidOperationException(); } }

        internal virtual int MetadataTokenInternal { get { return MetadataToken; } }
    
        public virtual Module Module
        { 
            get
            { 
                if (this is Type)
                    return((Type)this).Module;
                
                throw new NotImplementedException(); 
            } 
        }
        

        // this method is required so Object.GetType is not made final virtual by the compiler
        Type _MemberInfo.GetType() { return base.GetType(); }
        #endregion

        void _MemberInfo.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _MemberInfo.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _MemberInfo.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _MemberInfo.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }
    }


    [Serializable()] 
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_MethodBase))]
    [PermissionSetAttribute( SecurityAction.InheritanceDemand, Name="FullTrust" )]    
    [System.Runtime.InteropServices.ComVisible(true)]
    public abstract class MethodBase : MemberInfo, _MethodBase
    {
        #region Static Members
        public static MethodBase GetMethodFromHandle(RuntimeMethodHandle handle)
        {
            if (handle.IsNullHandle())
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidHandle"));
            
            MethodBase m = RuntimeType.GetMethodBase(handle);

            if (m.DeclaringType != null && m.DeclaringType.IsGenericType)
                throw new ArgumentException(String.Format(
                    CultureInfo.CurrentCulture, Environment.GetResourceString("Argument_MethodDeclaringTypeGeneric"), 
                    m, m.DeclaringType.GetGenericTypeDefinition()));
 
            return m;
        }

        [System.Runtime.InteropServices.ComVisible(false)]
        public static MethodBase GetMethodFromHandle(RuntimeMethodHandle handle, RuntimeTypeHandle declaringType)
        {
            if (handle.IsNullHandle())
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidHandle"));

            return RuntimeType.GetMethodBase(declaringType, handle);
        }

        [DynamicSecurityMethod] // Specify DynamicSecurityMethod attribute to prevent inlining of the caller.
        public static MethodBase GetCurrentMethod()
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return RuntimeMethodInfo.InternalGetCurrentMethod(ref stackMark);
        }
        #endregion

        #region Constructor
        protected MethodBase() { }
        #endregion

        #region Internal Members
        internal virtual bool IsOverloaded { get { throw new NotSupportedException(Environment.GetResourceString("InvalidOperation_Method")); } }
        internal virtual RuntimeMethodHandle GetMethodHandle() { return MethodHandle; }
        #endregion

        #region Public Abstract\Virtual Members
        internal virtual Type GetReturnType() { throw new NotImplementedException(); }

        internal virtual ParameterInfo[] GetParametersNoCopy() { return GetParameters (); }

        public abstract ParameterInfo[] GetParameters();

        public abstract MethodImplAttributes GetMethodImplementationFlags();

        public abstract RuntimeMethodHandle MethodHandle { get; }   

        public abstract MethodAttributes Attributes  { get; }    

        public abstract Object Invoke(Object obj, BindingFlags invokeAttr, Binder binder, Object[] parameters, CultureInfo culture);

        public virtual CallingConventions CallingConvention { get { return CallingConventions.Standard; } }

        [System.Runtime.InteropServices.ComVisible(true)]
        public virtual Type[] GetGenericArguments() { throw new NotSupportedException(Environment.GetResourceString("NotSupported_SubclassOverride")); }
        
        public virtual bool IsGenericMethodDefinition { get { return false; } }

        public virtual bool ContainsGenericParameters { get { return false; } }

        public virtual bool IsGenericMethod { get { return false; } }
        #endregion

        #region _MethodBase Implementation
        Type _MethodBase.GetType() { return base.GetType(); }
        bool _MethodBase.IsPublic { get { return IsPublic; } }
        bool _MethodBase.IsPrivate { get { return IsPrivate; } }
        bool _MethodBase.IsFamily { get { return IsFamily; } }
        bool _MethodBase.IsAssembly { get { return IsAssembly; } }
        bool _MethodBase.IsFamilyAndAssembly { get { return IsFamilyAndAssembly; } }
        bool _MethodBase.IsFamilyOrAssembly { get { return IsFamilyOrAssembly; } }
        bool _MethodBase.IsStatic { get { return IsStatic; } }
        bool _MethodBase.IsFinal { get { return IsFinal; } }
        bool _MethodBase.IsVirtual { get { return IsVirtual; } }
        bool _MethodBase.IsHideBySig { get { return IsHideBySig; } }
        bool _MethodBase.IsAbstract { get { return IsAbstract; } }
        bool _MethodBase.IsSpecialName { get { return IsSpecialName; } }
        bool _MethodBase.IsConstructor { get { return IsConstructor; } }
        #endregion

        #region Public Members
        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public Object Invoke(Object obj, Object[] parameters)
        {
            return Invoke(obj,BindingFlags.Default,null,parameters,null);
        }

        public bool IsPublic  { get { return(Attributes & MethodAttributes.MemberAccessMask) == MethodAttributes.Public; } }

        public bool IsPrivate { get { return(Attributes & MethodAttributes.MemberAccessMask) == MethodAttributes.Private; } }

        public bool IsFamily { get { return(Attributes & MethodAttributes.MemberAccessMask) == MethodAttributes.Family; } }

        public bool IsAssembly { get { return(Attributes & MethodAttributes.MemberAccessMask) == MethodAttributes.Assembly; } }

        public bool IsFamilyAndAssembly { get { return(Attributes & MethodAttributes.MemberAccessMask) == MethodAttributes.FamANDAssem; } }

        public bool IsFamilyOrAssembly { get {return(Attributes & MethodAttributes.MemberAccessMask) == MethodAttributes.FamORAssem; } }

        public bool IsStatic { get { return(Attributes & MethodAttributes.Static) != 0; } }

        public bool IsFinal { get { return(Attributes & MethodAttributes.Final) != 0; }
        }
        public bool IsVirtual { get { return(Attributes & MethodAttributes.Virtual) != 0; }
        }   
        public bool IsHideBySig { get { return(Attributes & MethodAttributes.HideBySig) != 0; } }  

        public bool IsAbstract { get { return(Attributes & MethodAttributes.Abstract) != 0; } }

        public bool IsSpecialName { get { return(Attributes & MethodAttributes.SpecialName) != 0; } }

[System.Runtime.InteropServices.ComVisible(true)]
        public bool IsConstructor 
        {
            get 
            {
                return((Attributes & MethodAttributes.RTSpecialName) != 0) && 
                     Name.Equals(ConstructorInfo.ConstructorName);
            }
        }

        [ReflectionPermissionAttribute(SecurityAction.Demand, Flags=ReflectionPermissionFlag.MemberAccess)]            
        public virtual MethodBody GetMethodBody()
        {
            throw new InvalidOperationException();
        }        
        #endregion
        
        #region Private Invocation Helpers
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        static extern internal uint GetSpecialSecurityFlags(RuntimeMethodHandle method);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        static extern internal void PerformSecurityCheck(Object obj, RuntimeMethodHandle method, IntPtr parent, uint invocationFlags);
        #endregion

        #region Internal Methods
        internal virtual Type[] GetParameterTypes()
        {
            ParameterInfo[] paramInfo = GetParametersNoCopy();
            Type[] parameterTypes = null;

            parameterTypes = new Type[paramInfo.Length];
            for (int i = 0; i < paramInfo.Length; i++)
                parameterTypes[i] = paramInfo[i].ParameterType;

            return parameterTypes;
        }

        virtual internal uint GetOneTimeFlags()
        {
            RuntimeMethodHandle handle = MethodHandle;
            
            uint invocationFlags = 0;
            Type declaringType = DeclaringType;

            //
            // first take care of all the NO_INVOKE cases
            if (
                (ContainsGenericParameters) ||
                (declaringType != null && declaringType.ContainsGenericParameters) ||
                ((CallingConvention & CallingConventions.VarArgs) == CallingConventions.VarArgs) ||
                ((Attributes & MethodAttributes.RequireSecObject) == MethodAttributes.RequireSecObject) ||
                (Module.Assembly.GetType() == typeof(AssemblyBuilder) && ((AssemblyBuilder)Module.Assembly).m_assemblyData.m_access == AssemblyBuilderAccess.Save)
               )
                invocationFlags |= INVOCATION_FLAGS_NO_INVOKE;
            
            //
            // this should be an invocable method, determine the other flags that participate in invocation
            else
            {
                invocationFlags |= MethodBase.GetSpecialSecurityFlags(handle);

                if ((invocationFlags & INVOCATION_FLAGS_NEED_SECURITY) == 0)
                {
                    // determine whether the method needs security
                    if (
                        ((Attributes & MethodAttributes.MemberAccessMask) != MethodAttributes.Public) ||
                        (declaringType != null && !declaringType.IsVisible)
                       )
                        invocationFlags |= INVOCATION_FLAGS_NEED_SECURITY;

                    else if (IsGenericMethod)
                    {
                        Type[] genericArguments = GetGenericArguments();

                        for (int i = 0; i < genericArguments.Length; i++)
                        {
                            if (!genericArguments[i].IsVisible)
                            {
                                invocationFlags |= INVOCATION_FLAGS_NEED_SECURITY;
                                break;
                            }
                        }
                    }
                }

            }

            invocationFlags |= GetOneTimeSpecificFlags();

            invocationFlags |= INVOCATION_FLAGS_INITIALIZED;
            return invocationFlags;
        }

        // only ctors have special flags for now
        internal virtual uint GetOneTimeSpecificFlags()
        {
            return 0;
        }

        internal Object[] CheckArguments(Object[] parameters, Binder binder, 
            BindingFlags invokeAttr, CultureInfo culture, Signature sig)
        {
            int actualCount = (parameters != null) ? parameters.Length : 0;
            // copy the arguments in a different array so we detach from any user changes 
            Object[] copyOfParameters = new Object[actualCount];
            
            ParameterInfo[] p = null;
            for (int i = 0; i < actualCount; i++)
            {
                Object arg = parameters[i];
                RuntimeTypeHandle argRTH = sig.Arguments[i];
                
                if (arg == Type.Missing)
                {
                    if (p == null) 
                        p = GetParametersNoCopy();
                    if (p[i].DefaultValue == System.DBNull.Value)
                        throw new ArgumentException(Environment.GetResourceString("Arg_VarMissNull"),"parameters");
                    arg = p[i].DefaultValue;
                }
                if (argRTH.IsInstanceOfType(arg))
                    copyOfParameters[i] = arg;
                else
                    copyOfParameters[i] = argRTH.GetRuntimeType().CheckValue(arg, binder, culture, invokeAttr);
            }

            return copyOfParameters;
        }
        #endregion

        void _MethodBase.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _MethodBase.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _MethodBase.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _MethodBase.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }
    }


    [Serializable()] 
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_ConstructorInfo))]
    [PermissionSetAttribute( SecurityAction.InheritanceDemand, Name="FullTrust" )]    
    [System.Runtime.InteropServices.ComVisible(true)]
    public abstract class ConstructorInfo : MethodBase, _ConstructorInfo
    {
        #region Static Members
        [System.Runtime.InteropServices.ComVisible(true)]
        public readonly static String ConstructorName = ".ctor";

        [System.Runtime.InteropServices.ComVisible(true)]
        public readonly static String TypeConstructorName = ".cctor";
        #endregion

        #region Constructor
        protected ConstructorInfo() { }
        #endregion

        #region MemberInfo Overrides
        [System.Runtime.InteropServices.ComVisible(true)]
        public override MemberTypes MemberType { get { return System.Reflection.MemberTypes.Constructor; } }
        #endregion
    
        #region Public Abstract\Virtual Members
        public abstract Object Invoke(BindingFlags invokeAttr, Binder binder, Object[] parameters, CultureInfo culture);
        #endregion

        #region Public Members
        internal override Type GetReturnType() { return DeclaringType; }

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public Object Invoke(Object[] parameters)
        {
            return Invoke(BindingFlags.Default, null, parameters, null);
        }
        #endregion

        #region COM Interop Support
        Type _ConstructorInfo.GetType()
        {
            return base.GetType();
        }
        
        Object _ConstructorInfo.Invoke_2(Object obj, BindingFlags invokeAttr, Binder binder, Object[] parameters, CultureInfo culture)
        {
            return Invoke(obj, invokeAttr, binder, parameters, culture);
        }
        
        Object _ConstructorInfo.Invoke_3(Object obj, Object[] parameters)
        {
            return Invoke(obj, parameters);
        }
        
        Object _ConstructorInfo.Invoke_4(BindingFlags invokeAttr, Binder binder, Object[] parameters, CultureInfo culture)
        {
            return Invoke(invokeAttr, binder, parameters, culture);
        }
        
        Object _ConstructorInfo.Invoke_5(Object[] parameters)
        {
            return Invoke(parameters);
        }

        void _ConstructorInfo.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _ConstructorInfo.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _ConstructorInfo.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _ConstructorInfo.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }
        #endregion
    }


    [Serializable()] 
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_MethodInfo))]
    [PermissionSetAttribute( SecurityAction.InheritanceDemand, Name="FullTrust" )]    
    [System.Runtime.InteropServices.ComVisible(true)]
    public abstract class MethodInfo : MethodBase, _MethodInfo
    {
        #region Constructor
        protected MethodInfo() { }
        #endregion
    
        #region MemberInfo Overrides
        public override MemberTypes MemberType { get { return System.Reflection.MemberTypes.Method; } }
        #endregion
    
        #region Internal Members
        internal virtual MethodInfo GetParentDefinition() { return null; }
        #endregion

        #region Public Abstract\Virtual Members
        public virtual Type ReturnType { get { return GetReturnType(); } }
        internal override Type GetReturnType() { return ReturnType; }
    
        public virtual ParameterInfo ReturnParameter { get { throw new NotImplementedException(); } }

        public abstract ICustomAttributeProvider ReturnTypeCustomAttributes { get;  }

        public abstract MethodInfo GetBaseDefinition();

        [System.Runtime.InteropServices.ComVisible(true)]
        public override Type[] GetGenericArguments() { throw new NotSupportedException(Environment.GetResourceString("NotSupported_SubclassOverride")); }
        
        [System.Runtime.InteropServices.ComVisible(true)]
        public virtual MethodInfo GetGenericMethodDefinition() { throw new NotSupportedException(Environment.GetResourceString("NotSupported_SubclassOverride")); }

        public override bool IsGenericMethodDefinition { get { return false; } }

        public override bool ContainsGenericParameters { get { return false; } }

        public virtual MethodInfo MakeGenericMethod(params Type[] typeArguments) { throw new NotSupportedException(Environment.GetResourceString("NotSupported_SubclassOverride")); }

        public override bool IsGenericMethod { get { return false; } }
	    #endregion

        Type _MethodInfo.GetType()
        {
            return base.GetType();
        }

        void _MethodInfo.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _MethodInfo.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _MethodInfo.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _MethodInfo.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }
    }


    [Serializable()] 
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_FieldInfo))]
    [PermissionSetAttribute( SecurityAction.InheritanceDemand, Name="FullTrust" )]    
    [System.Runtime.InteropServices.ComVisible(true)]
    public abstract class FieldInfo : MemberInfo, _FieldInfo
    {
        #region Static Members
        public static FieldInfo GetFieldFromHandle(RuntimeFieldHandle handle)
        {
            if (handle.IsNullHandle())
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidHandle"));
                
            FieldInfo f = RuntimeType.GetFieldInfo(handle);
                       
            if (f.DeclaringType != null && f.DeclaringType.IsGenericType)
                throw new ArgumentException(String.Format(
                    CultureInfo.CurrentCulture, Environment.GetResourceString("Argument_FieldDeclaringTypeGeneric"), 
                    f.Name, f.DeclaringType.GetGenericTypeDefinition()));

            return f;            
        }           
        
        [System.Runtime.InteropServices.ComVisible(false)]
        public static FieldInfo GetFieldFromHandle(RuntimeFieldHandle handle, RuntimeTypeHandle declaringType)
        {
            if (handle.IsNullHandle())
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidHandle"));

            return RuntimeType.GetFieldInfo(declaringType, handle);
        }           
        #endregion

        #region Constructor
        protected FieldInfo() { }       
        #endregion

        #region MemberInfo Overrides
        public override MemberTypes MemberType { get { return System.Reflection.MemberTypes.Field; } }          
        #endregion

        #region Public Abstract\Virtual Members
        
        public virtual Type[] GetRequiredCustomModifiers()
        {
            throw new NotImplementedException();
        }

        public virtual Type[] GetOptionalCustomModifiers()
        {
            throw new NotImplementedException();
        }

        [CLSCompliant(false)]
        public virtual void SetValueDirect(TypedReference obj, Object value)
        {
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_AbstractNonCLS"));
        }

        [CLSCompliant(false)]
        public virtual Object GetValueDirect(TypedReference obj)
        {
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_AbstractNonCLS"));
        }    

        public abstract RuntimeFieldHandle FieldHandle { get; }

        public abstract Type FieldType { get; }    
     
        public abstract Object GetValue(Object obj);
        public virtual Object GetRawConstantValue() { throw new NotSupportedException(Environment.GetResourceString("NotSupported_AbstractNonCLS")); }

        public abstract void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo culture);

        public abstract FieldAttributes Attributes { get; }
        #endregion

        #region Public Members
        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public void SetValue(Object obj, Object value)
        {
            SetValue(obj, value, BindingFlags.Default, Type.DefaultBinder, null);
        }

        public bool IsPublic { get { return(Attributes & FieldAttributes.FieldAccessMask) == FieldAttributes.Public; } }

        public bool IsPrivate { get { return(Attributes & FieldAttributes.FieldAccessMask) == FieldAttributes.Private; } }

        public bool IsFamily { get { return(Attributes & FieldAttributes.FieldAccessMask) == FieldAttributes.Family; } }

        public bool IsAssembly { get { return(Attributes & FieldAttributes.FieldAccessMask) == FieldAttributes.Assembly; } }

        public bool IsFamilyAndAssembly { get { return(Attributes & FieldAttributes.FieldAccessMask) == FieldAttributes.FamANDAssem; } }

        public bool IsFamilyOrAssembly { get { return(Attributes & FieldAttributes.FieldAccessMask) == FieldAttributes.FamORAssem; } }

        public bool IsStatic { get { return(Attributes & FieldAttributes.Static) != 0; } }

        public bool IsInitOnly { get { return(Attributes & FieldAttributes.InitOnly) != 0; } }

        public bool IsLiteral { get { return(Attributes & FieldAttributes.Literal) != 0; } }

        public bool IsNotSerialized { get { return(Attributes & FieldAttributes.NotSerialized) != 0; } }

        public bool IsSpecialName  { get { return(Attributes & FieldAttributes.SpecialName) != 0; } }

        public bool IsPinvokeImpl { get { return(Attributes & FieldAttributes.PinvokeImpl) != 0; } }

        #endregion

        Type _FieldInfo.GetType()
        {
            return base.GetType();
        }

        void _FieldInfo.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _FieldInfo.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _FieldInfo.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _FieldInfo.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }
    }


    [Serializable()] 
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_EventInfo))]
    [PermissionSetAttribute( SecurityAction.InheritanceDemand, Name="FullTrust" )]    
    [System.Runtime.InteropServices.ComVisible(true)]
    public abstract class EventInfo : MemberInfo, _EventInfo
    {
        #region Constructor
        protected EventInfo() { }
        #endregion

        #region MemberInfo Overrides
        public override MemberTypes MemberType { get { return MemberTypes.Event; } }
        #endregion

        #region Public Abstract\Virtual Members
        public virtual MethodInfo[] GetOtherMethods(bool nonPublic)
        {
            throw new NotImplementedException();
        }

        public abstract MethodInfo GetAddMethod(bool nonPublic);

        public abstract MethodInfo GetRemoveMethod(bool nonPublic);

        public abstract MethodInfo GetRaiseMethod(bool nonPublic);

        public abstract EventAttributes Attributes { get; }
        #endregion

        #region Public Members
        public MethodInfo[] GetOtherMethods() { return GetOtherMethods(false); }

        public MethodInfo GetAddMethod() { return GetAddMethod(false); }
   
        public MethodInfo GetRemoveMethod() { return GetRemoveMethod(false); }

        public MethodInfo GetRaiseMethod() { return GetRaiseMethod(false); }

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public void AddEventHandler(Object target, Delegate handler)
        {
            MethodInfo addMethod = GetAddMethod();

            if (addMethod == null)
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NoPublicAddMethod"));

            addMethod.Invoke(target, new object[] { handler });       
        }
        
        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public void RemoveEventHandler(Object target, Delegate handler)
        {
            MethodInfo removeMethod = GetRemoveMethod();

            if (removeMethod == null)
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NoPublicRemoveMethod"));

            removeMethod.Invoke(target, new object[] { handler });       
        }
                
        public Type EventHandlerType 
        {
            get 
            {
                
                
                MethodInfo m = GetAddMethod(true);

                ParameterInfo[] p = m.GetParametersNoCopy();

                Type del = typeof(Delegate);

                for(int i = 0; i < p.Length; i++) 
                {
                    Type c = p[i].ParameterType;

                    if (c.IsSubclassOf(del))
                        return c;
                }
                return null;
            }
        }
        
        public bool IsSpecialName 
        {
            get 
            {
                return(Attributes & EventAttributes.SpecialName) != 0;
            }
        }

        public bool IsMulticast 
        {
            get 
            {
                Type cl = EventHandlerType;
                Type mc = typeof(MulticastDelegate);
                return mc.IsAssignableFrom(cl);
            }
        } 
        #endregion

        Type _EventInfo.GetType()
        {
            return base.GetType();
        }

        void _EventInfo.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _EventInfo.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _EventInfo.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _EventInfo.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }
    }


    [Serializable()] 
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_PropertyInfo))]
    [PermissionSetAttribute( SecurityAction.InheritanceDemand, Name="FullTrust" )]    
    [System.Runtime.InteropServices.ComVisible(true)]
    public abstract class PropertyInfo : MemberInfo, _PropertyInfo
    {
        #region Constructor
        protected PropertyInfo() {}
        #endregion

        #region MemberInfo Overrides
        public override MemberTypes MemberType { get { return System.Reflection.MemberTypes.Property; } }
        #endregion

        #region Public Abstract\Virtual Members
        public virtual object GetConstantValue()
        {
            throw new NotImplementedException();
        }

        public virtual object GetRawConstantValue()
        {
            throw new NotImplementedException();
        }

        public abstract Type PropertyType { get; }

        public abstract void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, Object[] index, CultureInfo culture);
        
        public abstract MethodInfo[] GetAccessors(bool nonPublic);
        
        public abstract MethodInfo GetGetMethod(bool nonPublic);
        
        public abstract MethodInfo GetSetMethod(bool nonPublic);
                
        public abstract ParameterInfo[] GetIndexParameters();
            
        public abstract PropertyAttributes Attributes { get; }

        public abstract bool CanRead { get; }
                                        
        public abstract bool CanWrite { get; }
        
        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public virtual Object GetValue(Object obj,Object[] index)
        {
            return GetValue(obj, BindingFlags.Default, null, index, null);
        }

        public abstract Object GetValue(Object obj, BindingFlags invokeAttr, Binder binder, Object[] index, CultureInfo culture);
        
        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public virtual void SetValue(Object obj, Object value, Object[] index)
        {
            SetValue(obj, value, BindingFlags.Default, null, index, null);
        }
        #endregion

        #region Public Members
        public virtual Type[] GetRequiredCustomModifiers() { return new Type[0]; }

        public virtual Type[] GetOptionalCustomModifiers() { return new Type[0]; }

        public MethodInfo[] GetAccessors() { return GetAccessors(false); }

        public MethodInfo GetGetMethod() { return GetGetMethod(false); }

        public MethodInfo GetSetMethod() { return GetSetMethod(false); }

        public bool IsSpecialName { get { return(Attributes & PropertyAttributes.SpecialName) != 0; } }
        #endregion

        Type _PropertyInfo.GetType()
        {
            return base.GetType();
        }

        void _PropertyInfo.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _PropertyInfo.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _PropertyInfo.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _PropertyInfo.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }
    }

}

namespace System.Reflection
{
    internal enum MemberListType
    {
        All,
        CaseSensitive,
        CaseInsensitive,
        HandleToInfo
    }

    [Serializable()]
    internal sealed class RuntimeMethodInfo : MethodInfo, ISerializable
    {       
        #region Static Members
        internal static string ConstructParameters(ParameterInfo[] parameters, CallingConventions callingConvention)
        {
            Type[] parameterTypes = new Type[parameters.Length];

            for(int i = 0; i < parameters.Length; i++)
                parameterTypes[i] = parameters[i].ParameterType;

            return ConstructParameters(parameterTypes, callingConvention);
        }

        internal static string ConstructParameters(Type[] parameters, CallingConventions callingConvention)
        {
            string toString = "";
            string comma = "";

            for(int i = 0; i < parameters.Length; i++)
            {
                Type t = parameters[i];

                toString += comma;
                toString += t.SigToString();
                if (t.IsByRef)
                {
                    toString = toString.TrimEnd(new char[] { '&' });
                    toString += " ByRef";
                }

                comma = ", ";
            }

            if ((callingConvention & CallingConventions.VarArgs) == CallingConventions.VarArgs)
            {
                toString += comma;
                toString += "...";
            }

            return toString;
        }

        internal static string ConstructName(MethodBase mi)
        {
            // Serialization uses ToString to resolve MethodInfo overloads.
            string toString = null;

            toString += mi.Name;

            RuntimeMethodInfo rmi = mi as RuntimeMethodInfo;

            if (rmi != null && rmi.IsGenericMethod)
                toString += rmi.m_handle.ConstructInstantiation();

            toString += "(" + ConstructParameters(mi.GetParametersNoCopy(), mi.CallingConvention) + ")";
       
            return toString;
        }
        #endregion      

        #region Private Data Members
        private RuntimeMethodHandle m_handle;        
        private RuntimeTypeCache m_reflectedTypeCache;
        private string m_name;
        private string m_toString;
        private ParameterInfo[] m_parameters;
        private ParameterInfo m_returnParameter;
        private BindingFlags m_bindingFlags;
        private MethodAttributes m_methodAttributes;
        private Signature m_signature;
        private RuntimeType m_declaringType;
        private uint m_invocationFlags;
        #endregion

        #region Constructor
        internal RuntimeMethodInfo()
        {
            // Used for dummy head node during population
        }
        internal RuntimeMethodInfo(
            RuntimeMethodHandle handle, RuntimeTypeHandle declaringTypeHandle, 
            RuntimeTypeCache reflectedTypeCache, MethodAttributes methodAttributes, BindingFlags bindingFlags)
        {
            ASSERT.PRECONDITION(!handle.IsNullHandle());            
            ASSERT.PRECONDITION(methodAttributes == handle.GetAttributes());            

            m_toString = null;
            m_bindingFlags = bindingFlags;
            m_handle = handle;
            m_reflectedTypeCache = reflectedTypeCache;
            m_parameters = null; // Created lazily when GetParameters() is called.
            m_methodAttributes = methodAttributes;
            m_declaringType = declaringTypeHandle.GetRuntimeType();
            ASSERT.POSTCONDITION(!m_handle.IsNullHandle());    
        }
        #endregion

        #region Private Methods
        private RuntimeTypeHandle ReflectedTypeHandle
        { 
            get 
            { 
                return m_reflectedTypeCache.RuntimeTypeHandle; 
            } 
        }
        
        internal ParameterInfo[] FetchNonReturnParameters()
        {
            if (m_parameters == null)
                m_parameters = ParameterInfo.GetParameters(this, this, Signature);

            return m_parameters;
        }
        
        internal ParameterInfo FetchReturnParameter()
        {
            if (m_returnParameter == null)
                m_returnParameter = ParameterInfo.GetReturnParameter(this, this, Signature);

            return m_returnParameter;
        }

        #endregion

        #region Internal Members
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal override bool CacheEquals(object o) 
        { 
            RuntimeMethodInfo m = o as RuntimeMethodInfo;

            if (m == null)
                return false;

            return m.m_handle.Equals(m_handle);
        } 

        internal Signature Signature
        {
            get
            {
                if (m_signature == null)
                    m_signature = new Signature(m_handle, m_declaringType.GetTypeHandleInternal());

                return m_signature;
            }
        }

        internal BindingFlags BindingFlags { get { return m_bindingFlags; } }

        internal override RuntimeMethodHandle GetMethodHandle()
        {
            return m_handle;
        }

        internal override MethodInfo GetParentDefinition()
        {
            if (!IsVirtual || m_declaringType.IsInterface)
                return null;

            Type parent = m_declaringType.BaseType;

            if (parent == null)
                return null;

            int slot = m_handle.GetSlot();

            if (parent.GetTypeHandleInternal().GetNumVtableSlots() <= slot)
                return null;

            return(MethodInfo)RuntimeType.GetMethodBase(parent.GetTypeHandleInternal(), parent.GetTypeHandleInternal().GetMethodAt(slot));
        }

        internal override uint GetOneTimeFlags()
        {
            uint invocationFlags = 0;
            if (ReturnType.IsByRef)
                invocationFlags = INVOCATION_FLAGS_NO_INVOKE;
            invocationFlags |= base.GetOneTimeFlags();
            return invocationFlags;
        }

        #endregion

        #region Object Overrides
        public override String ToString() 
        {
            if (m_toString == null)
                m_toString = ReturnType.SigToString() + " " + ConstructName(this);

            return m_toString;
        }

        public override int GetHashCode()
        {
            return m_handle.GetHashCode();
        }

        public override bool Equals(object obj)
        {
            if (!IsGenericMethod)
                return obj == this;

            RuntimeMethodInfo mi = obj as RuntimeMethodInfo;
            
            RuntimeMethodHandle handle1 = GetMethodHandle().StripMethodInstantiation();
            RuntimeMethodHandle handle2 = mi.GetMethodHandle().StripMethodInstantiation();
            if (handle1 != handle2) 
                return false;

            if (mi == null || !mi.IsGenericMethod)
                return false;

            Type[] lhs = GetGenericArguments();
            Type[] rhs = mi.GetGenericArguments();

            if (lhs.Length != rhs.Length)
                return false;

            for (int i = 0; i < lhs.Length; i++)
            {
                if (lhs[i] != rhs[i])
                    return false;
            }

            return true;
        }
        #endregion

        #region ICustomAttributeProvider
        public override Object[] GetCustomAttributes(bool inherit)
        {
            return CustomAttribute.GetCustomAttributes(this, typeof(object) as RuntimeType as RuntimeType, inherit);
        }

        public override Object[] GetCustomAttributes(Type attributeType, bool inherit)
        {
            if (attributeType == null)
                throw new ArgumentNullException("attributeType");

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"attributeType");

            return CustomAttribute.GetCustomAttributes(this, attributeRuntimeType, inherit);
        }

        public override bool IsDefined(Type attributeType, bool inherit)
        {
            if (attributeType == null)
                throw new ArgumentNullException("attributeType");

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"attributeType");

            return CustomAttribute.IsDefined(this, attributeRuntimeType, inherit);
        }
        #endregion

        #region MemberInfo Overrides
        public override String Name 
        {
            get
            {
                if (m_name == null)
                    m_name = m_handle.GetName();

                return m_name;
            }
        }

        public override Type DeclaringType 
        {
            get
            {
                if (m_reflectedTypeCache.IsGlobal)
                    return null;

                return m_declaringType;
            }
        }

        public override Type ReflectedType 
        {
            get
            {
                if (m_reflectedTypeCache.IsGlobal)
                    return null;

                return m_reflectedTypeCache.RuntimeType;
            }
        }

        public override MemberTypes MemberType { get { return MemberTypes.Method; } }
        public override int MetadataToken { get { return m_handle.GetMethodDef(); } }        
        public override Module Module { get { return m_declaringType.Module; } }
        #endregion

        #region MethodBase Overrides
        internal override ParameterInfo[] GetParametersNoCopy()
        {
            FetchNonReturnParameters();

            return m_parameters;
        }

        public override ParameterInfo[] GetParameters()
        {
            FetchNonReturnParameters();

            if (m_parameters.Length == 0)
                return m_parameters;

            ParameterInfo[] ret = new ParameterInfo[m_parameters.Length];

            Array.Copy(m_parameters, ret, m_parameters.Length);

            return ret;
        }

        public override MethodImplAttributes GetMethodImplementationFlags()
        {
            return m_handle.GetImplAttributes();
        }

        internal override bool IsOverloaded
        {
            get 
            {
                return m_reflectedTypeCache.GetMethodList(MemberListType.CaseSensitive, Name).Count > 1;
            }
        }

        public override RuntimeMethodHandle MethodHandle 
        { 
            get 
            { 
                Type declaringType = DeclaringType;
                if ((declaringType == null && Module.Assembly.ReflectionOnly) || declaringType is ReflectionOnlyType)
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NotAllowedInReflectionOnly"));
                return m_handle; 
            } 
        }

        public override MethodAttributes Attributes { get { return m_methodAttributes; } }
        public override CallingConventions CallingConvention { get { return Signature.CallingConvention; } }

        [ReflectionPermissionAttribute(SecurityAction.Demand, Flags = ReflectionPermissionFlag.MemberAccess)]
        public override MethodBody GetMethodBody()
        {
            MethodBody mb = m_handle.GetMethodBody(ReflectedTypeHandle);
            if (mb != null) 
                mb.m_methodBase = this;
            return mb;
        }        
        #endregion

        #region Invocation Logic(On MemberBase)
        private void CheckConsistency(Object target) 
        {
            // only test instance methods
            if ((m_methodAttributes & MethodAttributes.Static) != MethodAttributes.Static) 
            {
                if (!m_declaringType.IsInstanceOfType(target))
                {
                    if (target == null) 
                        throw new TargetException(Environment.GetResourceString("RFLCT.Targ_StatMethReqTarg"));
                    else
                        throw new TargetException(Environment.GetResourceString("RFLCT.Targ_ITargMismatch"));
                }
            }
        }

        private void ThrowNoInvokeException()
        {
            // method is ReflectionOnly
            Type declaringType = DeclaringType;
            if ((declaringType == null && Module.Assembly.ReflectionOnly) || declaringType is ReflectionOnlyType)
                throw new InvalidOperationException(Environment.GetResourceString("Arg_ReflectionOnlyInvoke"));
                       
            // method is on a class that contains stack pointers
            if (DeclaringType.GetRootElementType() == typeof(ArgIterator))
                throw new NotSupportedException();
            
            // method is vararg
            else if ((CallingConvention & CallingConventions.VarArgs) == CallingConventions.VarArgs)
                throw new NotSupportedException();
                        
            // method is generic or on a generic class
            else if (DeclaringType.ContainsGenericParameters || ContainsGenericParameters)
                throw new InvalidOperationException(Environment.GetResourceString ("Arg_UnboundGenParam"));
            
            // method is abstract class
            else if (IsAbstract)
                throw new MemberAccessException();
            
            // ByRef return are not allowed in reflection
            else if (ReturnType.IsByRef)
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_ByRefReturn"));

            throw new TargetException();
        }
        
        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public override Object Invoke(Object obj, BindingFlags invokeAttr, Binder binder, Object[] parameters, CultureInfo culture)
        {
            return Invoke(obj, invokeAttr, binder, parameters, culture, false);
        }
        
        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        internal object Invoke(Object obj, BindingFlags invokeAttr, Binder binder, Object[] parameters, CultureInfo culture, bool skipVisibilityChecks)
        {
            // get the signature 
            int formalCount = Signature.Arguments.Length;
            int actualCount =(parameters != null) ? parameters.Length : 0;
            
            // set one time info for invocation
            if ((m_invocationFlags & INVOCATION_FLAGS_INITIALIZED) == 0) 
                m_invocationFlags = GetOneTimeFlags();
            
            if ((m_invocationFlags & INVOCATION_FLAGS_NO_INVOKE) != 0) 
                ThrowNoInvokeException();
            
            // check basic method consistency. This call will throw if there are problems in the target/method relationship
            CheckConsistency(obj);
            
            if (formalCount != actualCount)
                throw new TargetParameterCountException(Environment.GetResourceString("Arg_ParmCnt"));

            // Don't allow more than 65535 parameters.
            if (actualCount > UInt16.MaxValue)
                throw new TargetParameterCountException(Environment.GetResourceString("NotSupported_TooManyArgs"));

            if (!skipVisibilityChecks && (m_invocationFlags &(INVOCATION_FLAGS_RISKY_METHOD | INVOCATION_FLAGS_NEED_SECURITY)) != 0) 
            {
                if ((m_invocationFlags & INVOCATION_FLAGS_RISKY_METHOD) != 0) 
                    CodeAccessPermission.DemandInternal(PermissionType.ReflectionMemberAccess);
                
                if ((m_invocationFlags & INVOCATION_FLAGS_NEED_SECURITY)  != 0) 
                    PerformSecurityCheck(obj, m_handle, m_declaringType.TypeHandle.Value, m_invocationFlags);
            }
            
            // if we are here we passed all the previous checks. Time to look at the arguments
            RuntimeTypeHandle declaringTypeHandle = RuntimeTypeHandle.EmptyHandle;
            if (!m_reflectedTypeCache.IsGlobal)
                declaringTypeHandle = m_declaringType.TypeHandle;
            
            if (actualCount == 0) 
                return m_handle.InvokeMethodFast(obj, null, Signature, m_methodAttributes, declaringTypeHandle);

            Object[] arguments = CheckArguments(parameters, binder, invokeAttr, culture, Signature);

            Object retValue = m_handle.InvokeMethodFast(obj, arguments, Signature, m_methodAttributes, declaringTypeHandle);

            // copy out. This should be made only if ByRef are present.
            for(int index = 0; index < actualCount; index++) 
                parameters[index] = arguments[index];

            return retValue;
        }
        #endregion

        #region MethodInfo Overrides
        public override Type ReturnType 
        { 
            get { return Signature.ReturnTypeHandle.GetRuntimeType(); } 
        }

        public override ICustomAttributeProvider ReturnTypeCustomAttributes 
        { 
            get { return ReturnParameter; } 
        }

        public override ParameterInfo ReturnParameter 
        {
            get
            {
                FetchReturnParameter();
                ASSERT.POSTCONDITION(m_returnParameter != null);
                return m_returnParameter as ParameterInfo;
            }
        }

        public override MethodInfo GetBaseDefinition()
        {
            if (!IsVirtual || m_declaringType.IsInterface)
                return this;

            int slot = m_handle.GetSlot();
            RuntimeTypeHandle parent = m_handle.GetDeclaringType();
            RuntimeMethodHandle baseMethodHandle = RuntimeMethodHandle.EmptyHandle;
            do
            {
                int cVtblSlots = parent.GetNumVtableSlots();

                if (cVtblSlots <= slot)
                    break;

                baseMethodHandle = parent.GetMethodAt(slot);
                parent = baseMethodHandle.GetDeclaringType().GetBaseTypeHandle();
            } while(!parent.IsNullHandle());

            ASSERT.CONSISTENCY_CHECK((baseMethodHandle.GetAttributes() & MethodAttributes.Virtual) != 0);

            return(MethodInfo)RuntimeType.GetMethodBase(baseMethodHandle);
        }
        #endregion

        #region Generics
        public override MethodInfo MakeGenericMethod(params Type[] methodInstantiation)
        {
          if (methodInstantiation == null)
                throw new ArgumentNullException("methodInstantiation");

            Type[] methodInstantiationCopy = new Type[methodInstantiation.Length];
            for (int i = 0; i < methodInstantiation.Length; i ++)
                methodInstantiationCopy[i] = methodInstantiation[i];
            methodInstantiation = methodInstantiationCopy;

            if (!IsGenericMethodDefinition)
                throw new InvalidOperationException(
                    String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Arg_NotGenericMethodDefinition"), this));

            for (int i = 0; i < methodInstantiation.Length; i++)
            {
                if (methodInstantiation[i] == null)
                    throw new ArgumentNullException();
            
                if (!(methodInstantiation[i] is RuntimeType))
                    return MethodBuilderInstantiation.MakeGenericMethod(this, methodInstantiation);
            }

            Type[] genericParameters = GetGenericArguments();

            RuntimeType.SanityCheckGenericArguments(methodInstantiation, genericParameters);

            RuntimeTypeHandle[] typeHandles = new RuntimeTypeHandle[methodInstantiation.Length];

            for(int i = 0; i < methodInstantiation.Length; i++) 
                typeHandles[i] = methodInstantiation[i].GetTypeHandleInternal();

            MethodInfo ret = null;
                
            try
            {
                ret = RuntimeType.GetMethodBase(m_reflectedTypeCache.RuntimeTypeHandle, 
                    m_handle.GetInstantiatingStub(m_declaringType.GetTypeHandleInternal(), typeHandles)) as MethodInfo;
            }
            catch (VerificationException e)
            {
                RuntimeType.ValidateGenericArguments(this, methodInstantiation, e);
                throw e;
            }
            
            return ret;
        }

        public override Type[] GetGenericArguments() 
        {
            RuntimeType[] rtypes = null;
            RuntimeTypeHandle[] types = m_handle.GetMethodInstantiation();

            if (types != null)
            {
                rtypes = new RuntimeType[types.Length]; 

                for(int i = 0; i < types.Length; i++) 
                    rtypes[i] = types[i].GetRuntimeType();
            }
            else
            {
                 
                rtypes = new RuntimeType[0];
            }
            return rtypes;
        }

        public override MethodInfo GetGenericMethodDefinition() 
        {
            if (!IsGenericMethod)
                throw new InvalidOperationException();
            
            return RuntimeType.GetMethodBase(m_declaringType.GetTypeHandleInternal(), m_handle.StripMethodInstantiation()) as MethodInfo;
        }

        public override bool IsGenericMethod { get { return m_handle.HasMethodInstantiation(); } }

        public override bool IsGenericMethodDefinition { get { return m_handle.IsGenericMethodDefinition(); } } 

        public override bool ContainsGenericParameters 
        { 
            get 
            {
                if (DeclaringType != null && DeclaringType.ContainsGenericParameters)
                    return true;

                if (!IsGenericMethod)
                    return false;

                Type[] pis = GetGenericArguments(); 
                for (int i = 0; i < pis.Length; i++)
                {
                    if (pis[i].ContainsGenericParameters)
                        return true;
                }

                return false;
            } 
        }
        #endregion

        #region ISerializable Implementation
        public void GetObjectData(SerializationInfo info, StreamingContext context) 
        {
            if (info == null) 
                throw new ArgumentNullException("info");

            if (m_reflectedTypeCache.IsGlobal)
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_GlobalMethodSerialization"));

            MemberInfoSerializationHolder.GetSerializationInfo(
            info, Name,  ReflectedTypeHandle.GetRuntimeType(), ToString(), MemberTypes.Method, 
                IsGenericMethod & !IsGenericMethodDefinition ? GetGenericArguments() : null);
        }
        #endregion

        #region Legacy Internal
        internal static MethodBase InternalGetCurrentMethod(ref StackCrawlMark stackMark)
        {
            RuntimeMethodHandle method = RuntimeMethodHandle.GetCurrentMethod(ref stackMark);

            if (method.IsNullHandle()) 
                return null;

            // If C<Foo>.m<Bar> was called, GetCurrentMethod returns C<object>.m<object>. We cannot
            // get know that the instantiation used Foo or Bar at that point. So the next best thing
            // is to return C<T>.m<P> and that's what GetTypicalMethodDefinition will do for us. 
            method = method.GetTypicalMethodDefinition();
            
            return RuntimeType.GetMethodBase(method);
        }
        #endregion
    }


    [Serializable()] 
    internal sealed class RuntimeConstructorInfo : ConstructorInfo, ISerializable
    {
        #region Private Data Members
        private RuntimeMethodHandle m_handle;
        private RuntimeTypeCache m_reflectedTypeCache;
        private RuntimeType m_declaringType;
        private string m_toString;
        private MethodAttributes m_methodAttributes;
        private BindingFlags m_bindingFlags;
        private ParameterInfo[] m_parameters = null; // Created lazily when GetParameters() is called.
        private uint m_invocationFlags;
        private Signature m_signature;
        #endregion

        #region Constructor
        internal RuntimeConstructorInfo()
        {
            // Used for dummy head node during population
        }
        internal RuntimeConstructorInfo(
            RuntimeMethodHandle handle, RuntimeTypeHandle declaringTypeHandle, RuntimeTypeCache reflectedTypeCache,
            MethodAttributes methodAttributes, BindingFlags bindingFlags)
        {
            ASSERT.POSTCONDITION(methodAttributes == handle.GetAttributes());

            m_bindingFlags = bindingFlags;
            m_handle = handle;
            m_reflectedTypeCache = reflectedTypeCache;
            m_declaringType = declaringTypeHandle.GetRuntimeType();
            m_parameters = null; // Created lazily when GetParameters() is called.
            m_toString = null;
            m_methodAttributes = methodAttributes;
        }
        #endregion

        #region NonPublic Methods
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal override bool CacheEquals(object o)
        {
            RuntimeConstructorInfo m = o as RuntimeConstructorInfo;

            if (m == null)
                return false;

            return m.m_handle.Equals(m_handle);
        }

        private Signature Signature
        {
            get
            {
                if (m_signature == null)
                    m_signature = new Signature(m_handle, m_declaringType.GetTypeHandleInternal());

                return m_signature;
            }
        }

        private RuntimeTypeHandle ReflectedTypeHandle
        { 
            get 
            { 
                return m_reflectedTypeCache.RuntimeTypeHandle; 
            } 
        }

        private void CheckConsistency(Object target) 
        {
            if (target == null && IsStatic)
                return;

            if (!m_declaringType.IsInstanceOfType(target))
            {
                if (target == null) 
                    throw new TargetException(Environment.GetResourceString("RFLCT.Targ_StatMethReqTarg"));

                throw new TargetException(Environment.GetResourceString("RFLCT.Targ_ITargMismatch"));
            }
        }
        
        internal BindingFlags BindingFlags { get { return m_bindingFlags; } }

        internal override RuntimeMethodHandle GetMethodHandle()
        {
            return m_handle;
        }

        internal override bool IsOverloaded
        { 
            get 
            { 
                return m_reflectedTypeCache.GetConstructorList(MemberListType.CaseSensitive, Name).Count > 1;
            }
        } 

        internal override uint GetOneTimeSpecificFlags() 
        {
            uint invocationFlags = INVOCATION_FLAGS_IS_CTOR; // this is a given

            if (
                (DeclaringType != null && DeclaringType.IsAbstract) ||
                (IsStatic)
               )
                invocationFlags |= INVOCATION_FLAGS_NO_CTOR_INVOKE;
            else if (DeclaringType == typeof(void))
                invocationFlags |= INVOCATION_FLAGS_NO_INVOKE;
            // Check for attempt to create a delegate class, we demand unmanaged
            // code permission for this since it's hard to validate the target address.
            else if (typeof(Delegate).IsAssignableFrom(DeclaringType)) 
                invocationFlags |= INVOCATION_FLAGS_IS_DELEGATE_CTOR;
            
            return invocationFlags;
        }
        #endregion

        #region Object Overrides
        public override String ToString() 
        {
            if (m_toString == null)
                m_toString = "Void " + RuntimeMethodInfo.ConstructName(this);

            return m_toString;
        }
        #endregion

        #region ICustomAttributeProvider
        public override Object[] GetCustomAttributes(bool inherit)
        {
            return CustomAttribute.GetCustomAttributes(this, typeof(object) as RuntimeType);
        }

        public override Object[] GetCustomAttributes(Type attributeType, bool inherit)
        {
            if (attributeType == null)
                throw new ArgumentNullException("attributeType");

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"attributeType");

            return CustomAttribute.GetCustomAttributes(this, attributeRuntimeType);
        }

        public override bool IsDefined(Type attributeType, bool inherit)
        {
            if (attributeType == null)
                throw new ArgumentNullException("attributeType");

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"attributeType");

            return CustomAttribute.IsDefined(this, attributeRuntimeType);
        }
        #endregion

        #region MemberInfo Overrides
        public override String Name { get { return m_handle.GetName(); } }
[System.Runtime.InteropServices.ComVisible(true)]
        public override MemberTypes MemberType { get { return MemberTypes.Constructor; } }
        public override Type DeclaringType { get { return m_reflectedTypeCache.IsGlobal ? null : m_declaringType; } }
        public override Type ReflectedType { get { return m_reflectedTypeCache.IsGlobal ? null : m_reflectedTypeCache.RuntimeType; } }
        public override int MetadataToken { get { return m_handle.GetMethodDef(); } }
        public override Module Module { get { return m_declaringType.GetTypeHandleInternal().GetModuleHandle().GetModule(); } }
        #endregion

        #region MethodBase Overrides
        internal override Type GetReturnType() { return Signature.ReturnTypeHandle.GetRuntimeType(); } 
        
        internal override ParameterInfo[] GetParametersNoCopy()
        {
            if (m_parameters == null)
                m_parameters = ParameterInfo.GetParameters(this, this, Signature);

            return m_parameters;
        }

        public override ParameterInfo[] GetParameters()
        {
            ParameterInfo[] parameters = GetParametersNoCopy();

            if (parameters.Length == 0)
                return parameters;
            
            ParameterInfo[] ret = new ParameterInfo[parameters.Length];
            Array.Copy(parameters, ret, parameters.Length);
            return ret;
        }

        public override MethodImplAttributes GetMethodImplementationFlags()
        {
            return m_handle.GetImplAttributes();
        }

        public override RuntimeMethodHandle MethodHandle 
        {
            get
            {
                Type declaringType = DeclaringType;
                if ((declaringType == null && Module.Assembly.ReflectionOnly) || declaringType is ReflectionOnlyType)
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NotAllowedInReflectionOnly"));
                return m_handle;
            }
        }

        public override MethodAttributes Attributes 
        {
            get
            {
                return m_methodAttributes;
            }
        }

        public override CallingConventions CallingConvention 
        {
            get
            {
                return Signature.CallingConvention;
            }
        }

        internal static void CheckCanCreateInstance(Type declaringType, bool isVarArg)
        {
            if (declaringType == null)
                throw new ArgumentNullException("declaringType");
            
            // ctor is ReflectOnly
            if (declaringType is ReflectionOnlyType)
                throw new InvalidOperationException(Environment.GetResourceString("Arg_ReflectionOnlyInvoke"));
            
            // ctor is declared on interface class
            else if (declaringType.IsInterface)
                throw new MemberAccessException(
                    String.Format(CultureInfo.CurrentUICulture, Environment.GetResourceString("Acc_CreateInterfaceEx"), declaringType));
            
            // ctor is on an abstract class
            else if (declaringType.IsAbstract)
                throw new MemberAccessException(
                    String.Format(CultureInfo.CurrentUICulture, Environment.GetResourceString("Acc_CreateAbstEx"), declaringType));
            
            // ctor is on a class that contains stack pointers
            else if (declaringType.GetRootElementType() == typeof(ArgIterator))
                throw new NotSupportedException();
            
            // ctor is vararg
            else if (isVarArg)
                throw new NotSupportedException();
                        
            // ctor is generic or on a generic class
            else if (declaringType.ContainsGenericParameters)
                throw new MemberAccessException(
                    String.Format(CultureInfo.CurrentUICulture, Environment.GetResourceString("Acc_CreateGenericEx"), declaringType));
            
            // ctor is declared on System.Void
            else if (declaringType == typeof(void))
                throw new MemberAccessException(Environment.GetResourceString("Access_Void"));
        }

        internal void ThrowNoInvokeException()
        {
            CheckCanCreateInstance(DeclaringType, (CallingConvention & CallingConventions.VarArgs) == CallingConventions.VarArgs);

            // ctor is .cctor
            if ((Attributes & MethodAttributes.Static) == MethodAttributes.Static)
                throw new MemberAccessException(Environment.GetResourceString("Acc_NotClassInit"));
            
            throw new TargetException();
        }

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public override Object Invoke(
            Object obj, BindingFlags invokeAttr, Binder binder, Object[] parameters, CultureInfo culture)
        {
            // set one time info for invocation
            if (m_invocationFlags == INVOCATION_FLAGS_UNKNOWN) 
                m_invocationFlags = GetOneTimeFlags();
            
            if ((m_invocationFlags & INVOCATION_FLAGS_NO_INVOKE) != 0) 
                ThrowNoInvokeException();

            // check basic method consistency. This call will throw if there are problems in the target/method relationship
            CheckConsistency(obj);

            if (obj != null)
                new SecurityPermission(SecurityPermissionFlag.SkipVerification).Demand();

            if ((m_invocationFlags &(INVOCATION_FLAGS_RISKY_METHOD | INVOCATION_FLAGS_NEED_SECURITY)) != 0) 
            {
                if ((m_invocationFlags & INVOCATION_FLAGS_RISKY_METHOD) != 0) 
                    CodeAccessPermission.DemandInternal(PermissionType.ReflectionMemberAccess);
                if ((m_invocationFlags & INVOCATION_FLAGS_NEED_SECURITY)  != 0) 
                    PerformSecurityCheck(obj, m_handle, m_declaringType.TypeHandle.Value, m_invocationFlags);
            }
            
            // get the signature
            int formalCount = Signature.Arguments.Length;
            int actualCount =(parameters != null) ? parameters.Length : 0;
            if (formalCount != actualCount)
                throw new TargetParameterCountException(Environment.GetResourceString("Arg_ParmCnt"));
            
            // if we are here we passed all the previous checks. Time to look at the arguments
            if (actualCount > 0) 
            {
                Object[] arguments = CheckArguments(parameters, binder, invokeAttr, culture, Signature);
                Object retValue = m_handle.InvokeMethodFast(obj, arguments, Signature, m_methodAttributes, (ReflectedType != null) ? ReflectedType.TypeHandle : RuntimeTypeHandle.EmptyHandle);
                // copy out. This should be made only if ByRef are present.
                for(int index = 0; index < actualCount; index++) 
                    parameters[index] = arguments[index];
                return retValue;
            }
            return m_handle.InvokeMethodFast(obj, null, Signature, m_methodAttributes, (DeclaringType != null) ? DeclaringType.TypeHandle : RuntimeTypeHandle.EmptyHandle);
        }
        

        [ReflectionPermissionAttribute(SecurityAction.Demand, Flags = ReflectionPermissionFlag.MemberAccess)]
        public override MethodBody GetMethodBody()
        {
            MethodBody mb = m_handle.GetMethodBody(ReflectedTypeHandle);
            if (mb != null) 
                mb.m_methodBase = this;
            return mb;
        }        
        #endregion

        #region ConstructorInfo Overrides
        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public override Object Invoke(BindingFlags invokeAttr, Binder binder, Object[] parameters, CultureInfo culture)
        {
            // get the declaring TypeHandle early for consistent exceptions in IntrospectionOnly context
            RuntimeTypeHandle declaringTypeHandle = m_declaringType.TypeHandle;
            
            // set one time info for invocation
            if (m_invocationFlags == INVOCATION_FLAGS_UNKNOWN) 
                m_invocationFlags = GetOneTimeFlags();

            if ((m_invocationFlags & (INVOCATION_FLAGS_NO_INVOKE | INVOCATION_FLAGS_CONTAINS_STACK_POINTERS | INVOCATION_FLAGS_NO_CTOR_INVOKE)) != 0) 
                ThrowNoInvokeException();

            if ((m_invocationFlags & (INVOCATION_FLAGS_RISKY_METHOD | INVOCATION_FLAGS_NEED_SECURITY | INVOCATION_FLAGS_IS_DELEGATE_CTOR)) != 0) 
            {
                if ((m_invocationFlags & INVOCATION_FLAGS_RISKY_METHOD) != 0) 
                    CodeAccessPermission.DemandInternal(PermissionType.ReflectionMemberAccess);
                if ((m_invocationFlags & INVOCATION_FLAGS_NEED_SECURITY)  != 0) 
                    PerformSecurityCheck(null, m_handle, m_declaringType.TypeHandle.Value, m_invocationFlags & INVOCATION_FLAGS_CONSTRUCTOR_INVOKE);
                if ((m_invocationFlags & INVOCATION_FLAGS_IS_DELEGATE_CTOR) != 0)
                    new SecurityPermission(SecurityPermissionFlag.UnmanagedCode).Demand();
            }

            // get the signature 
            int formalCount = Signature.Arguments.Length;
            int actualCount =(parameters != null) ? parameters.Length : 0;
            if (formalCount != actualCount)
                throw new TargetParameterCountException(Environment.GetResourceString("Arg_ParmCnt"));

            // make sure the class ctor has been run
            RuntimeHelpers.RunClassConstructor(declaringTypeHandle);

            // if we are here we passed all the previous checks. Time to look at the arguments
            if (actualCount > 0) 
            {
                Object[] arguments = CheckArguments(parameters, binder, invokeAttr, culture, Signature);
                Object retValue = m_handle.InvokeConstructor(arguments, Signature, declaringTypeHandle);
                // copy out. This should be made only if ByRef are present.
                for(int index = 0; index < actualCount; index++) 
                    parameters[index] = arguments[index];
                return retValue;
            }
            return m_handle.InvokeConstructor(null, Signature, declaringTypeHandle);
        }
        #endregion
    
        #region ISerializable Implementation
        public void GetObjectData(SerializationInfo info, StreamingContext context) 
        {
            if (info==null)
                throw new ArgumentNullException("info");
            MemberInfoSerializationHolder.GetSerializationInfo(
                info, Name, ReflectedTypeHandle.GetRuntimeType(), ToString(), MemberTypes.Constructor);
        }

        internal void SerializationInvoke(Object target, SerializationInfo info, StreamingContext context)
        {
            MethodHandle.SerializationInvoke(target, Signature, info, context);
        }
       #endregion
    }


    [Serializable()] 
    internal abstract class RuntimeFieldInfo : FieldInfo
    {
        #region Private Data Members
        private BindingFlags m_bindingFlags;
        protected RuntimeTypeCache m_reflectedTypeCache;
        protected RuntimeType m_declaringType;        
        #endregion

        #region Constructor
        protected RuntimeFieldInfo()
        {
            // Used for dummy head node during population
        }
        protected RuntimeFieldInfo(RuntimeTypeCache reflectedTypeCache, RuntimeType declaringType, BindingFlags bindingFlags)
        {
            m_bindingFlags = bindingFlags;
            m_declaringType = declaringType;
            m_reflectedTypeCache = reflectedTypeCache;
        }
        #endregion

        #region NonPublic Members
        internal BindingFlags BindingFlags { get { return m_bindingFlags; } }
        private RuntimeTypeHandle ReflectedTypeHandle
        { 
            get 
            { 
                return m_reflectedTypeCache.RuntimeTypeHandle; 
            } 
        }
        internal RuntimeTypeHandle DeclaringTypeHandle
        {
            get
            {
                Type declaringType = DeclaringType;

                if (declaringType == null)
                    return Module.GetModuleHandle().GetModuleTypeHandle();

                return declaringType.GetTypeHandleInternal();
            }
        }
        
        internal virtual RuntimeFieldHandle GetFieldHandle() 
        {
            return FieldHandle;
        }

        #endregion

        #region MemberInfo Overrides
        public override MemberTypes MemberType { get { return MemberTypes.Field; } }
        public override Type ReflectedType { get { return m_reflectedTypeCache.IsGlobal ? null : m_reflectedTypeCache.RuntimeType; } }
        public override Type DeclaringType { get { return m_reflectedTypeCache.IsGlobal ? null : m_declaringType; } }        
        #endregion

        #region Object Overrides
        public unsafe override String ToString() 
        {
            return FieldType.SigToString() + " " + Name;
        }
        #endregion

        #region ICustomAttributeProvider
        public override Object[] GetCustomAttributes(bool inherit)
        {
            return CustomAttribute.GetCustomAttributes(this, typeof(object) as RuntimeType);
        }

        public override Object[] GetCustomAttributes(Type attributeType, bool inherit)
        {
            if (attributeType == null)
                throw new ArgumentNullException("attributeType");

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"attributeType");

            return CustomAttribute.GetCustomAttributes(this, attributeRuntimeType);
        }

        public override bool IsDefined(Type attributeType, bool inherit)
        {
            if (attributeType == null)
                throw new ArgumentNullException("attributeType");

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"attributeType");

            return CustomAttribute.IsDefined(this, attributeRuntimeType);
        }
        #endregion

        #region FieldInfo Overrides
        // All implemented on derived classes
        #endregion

        #region ISerializable Implementation
        public void GetObjectData(SerializationInfo info, StreamingContext context) 
        {
            if (info==null) 
                throw new ArgumentNullException("info");
            MemberInfoSerializationHolder.GetSerializationInfo(
                info, this.Name, this.ReflectedType, this.ToString(), MemberTypes.Field);
        }
        #endregion
    }


    [Serializable()]
    internal unsafe sealed class RtFieldInfo : RuntimeFieldInfo, ISerializable
    {
        #region FCalls
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        static private extern void PerformVisibilityCheckOnField(IntPtr field, Object target, IntPtr declaringType, FieldAttributes attr, uint invocationFlags);
        #endregion

        #region Private Data Members
        // agressive caching
        private RuntimeFieldHandle m_fieldHandle;
        private FieldAttributes m_fieldAttributes;
        // lazy caching
        private string m_name;
        private RuntimeType m_fieldType;
        private uint m_invocationFlags;
        #endregion

        #region Constructor
        internal RtFieldInfo()
        {
            // Used for dummy head node during population
        }
        internal RtFieldInfo(
            RuntimeFieldHandle handle, RuntimeType declaringType, RuntimeTypeCache reflectedTypeCache, BindingFlags bindingFlags) 
            : base(reflectedTypeCache, declaringType, bindingFlags)
        {
            m_fieldHandle = handle;
            m_fieldAttributes = m_fieldHandle.GetAttributes();
        }
        #endregion

        #region Private Members
        private void GetOneTimeFlags() 
        {
            Type declaringType = DeclaringType;

            // first take care of all the NO_INVOKE cases
            if (declaringType != null && declaringType.ContainsGenericParameters) 
            {
                m_invocationFlags |= INVOCATION_FLAGS_NO_INVOKE;
            }
            else if (Module.Assembly.GetType() == typeof(AssemblyBuilder)) 
            {
                AssemblyBuilder ab = Module.Assembly as AssemblyBuilder;
                if (ab.m_assemblyData.m_access == AssemblyBuilderAccess.Save)
                    m_invocationFlags |= INVOCATION_FLAGS_NO_INVOKE;
            }
            if ((declaringType == null && Module.Assembly.ReflectionOnly) || declaringType is ReflectionOnlyType)
            {
                m_invocationFlags |= INVOCATION_FLAGS_NO_INVOKE;
            }
            
            
            // this should be an usable field, determine the other flags 
            else 
            {
                if ((m_fieldAttributes & FieldAttributes.InitOnly) !=(FieldAttributes)0) 
                    m_invocationFlags |= INVOCATION_FLAGS_SPECIAL_FIELD;

                if ((m_fieldAttributes & FieldAttributes.HasFieldRVA) !=(FieldAttributes)0) 
                    m_invocationFlags |= INVOCATION_FLAGS_SPECIAL_FIELD;

                if (((m_fieldAttributes & FieldAttributes.FieldAccessMask) != FieldAttributes.Public) || (declaringType != null && !declaringType.IsVisible)) 
                    m_invocationFlags |= INVOCATION_FLAGS_NEED_SECURITY;

                // find out if the field type is one of the following: Primitive, Enum or Pointer
                Type fieldType = FieldType;
                if (fieldType.IsPointer || fieldType.IsEnum || fieldType.IsPrimitive) 
                    m_invocationFlags |= INVOCATION_FLAGS_FIELD_SPECIAL_CAST;
            }

            // must be last to avoid threading problems
            m_invocationFlags |= INVOCATION_FLAGS_INITIALIZED;
        }
        
        private void CheckConsistency(Object target) 
        {
            // only test instance fields
            if ((m_fieldAttributes & FieldAttributes.Static) != FieldAttributes.Static) 
            {
                if (!m_declaringType.IsInstanceOfType(target))
                {
                    if (target == null) 
                        throw new TargetException(Environment.GetResourceString("RFLCT.Targ_StatFldReqTarg"));
                    else
                    {
                        throw new ArgumentException(
                            String.Format(CultureInfo.CurrentUICulture, Environment.GetResourceString("Arg_FieldDeclTarget"), 
                                Name, m_declaringType, target.GetType()));
                    }
                }
            }
        }

        #endregion

        #region Internal Members
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal override bool CacheEquals(object o)
        {
            RtFieldInfo m = o as RtFieldInfo;

            if (m == null)
                return false;

            return m.m_fieldHandle.Equals(m_fieldHandle);
        }

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        internal void InternalSetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo culture, bool doVisibilityCheck)
        {
            InternalSetValue(obj, value, invokeAttr, binder, culture, doVisibilityCheck, true);
        }
        
        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        internal void InternalSetValue(Object obj, Object value, BindingFlags invokeAttr, 
            Binder binder, CultureInfo culture, bool doVisibilityCheck, bool doCheckConsistency)
        {
            RuntimeType declaringType = DeclaringType as RuntimeType;
            
            if ((m_invocationFlags & INVOCATION_FLAGS_INITIALIZED) == 0) 
                GetOneTimeFlags();
            
            if ((m_invocationFlags & INVOCATION_FLAGS_NO_INVOKE) != 0) 
            {
                if (declaringType != null && declaringType.ContainsGenericParameters)
                    throw new InvalidOperationException(Environment.GetResourceString ("Arg_UnboundGenField"));

                if ((declaringType == null && Module.Assembly.ReflectionOnly) || declaringType is  ReflectionOnlyType)
                    throw new InvalidOperationException(Environment.GetResourceString("Arg_ReflectionOnlyField"));

                throw new FieldAccessException();
            }

            if (doCheckConsistency)
                CheckConsistency(obj);
            
            value =((RuntimeType)FieldType).CheckValue(value, binder, culture, invokeAttr);

            if (doVisibilityCheck &&(m_invocationFlags &(INVOCATION_FLAGS_SPECIAL_FIELD | INVOCATION_FLAGS_NEED_SECURITY)) != 0) 
                PerformVisibilityCheckOnField(m_fieldHandle.Value, obj, m_declaringType.TypeHandle.Value, m_fieldAttributes, m_invocationFlags);

            bool domainInitialized = false;
            if (declaringType == null)
            {
                m_fieldHandle.SetValue(obj, value, FieldType.TypeHandle, m_fieldAttributes, RuntimeTypeHandle.EmptyHandle, ref domainInitialized);
            }
            else
            {
                domainInitialized = declaringType.DomainInitialized;
                m_fieldHandle.SetValue(obj, value, FieldType.TypeHandle, m_fieldAttributes, DeclaringType.TypeHandle, ref domainInitialized);
                declaringType.DomainInitialized = domainInitialized;
            }               

        } 

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        internal Object InternalGetValue(Object obj, bool doVisibilityCheck)
        {
            return InternalGetValue(obj, doVisibilityCheck, true);
        }
        
        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        internal Object InternalGetValue(Object obj, bool doVisibilityCheck, bool doCheckConsistency)
        {
            RuntimeType declaringType = DeclaringType as RuntimeType;

            if ((m_invocationFlags & INVOCATION_FLAGS_INITIALIZED) == 0) 
                GetOneTimeFlags();
            
            if ((m_invocationFlags & INVOCATION_FLAGS_NO_INVOKE) != 0) 
            {
                if (declaringType != null && DeclaringType.ContainsGenericParameters)
                    throw new InvalidOperationException(Environment.GetResourceString ("Arg_UnboundGenField"));

                if ((declaringType == null && Module.Assembly.ReflectionOnly) || declaringType is ReflectionOnlyType)
                    throw new InvalidOperationException(Environment.GetResourceString("Arg_ReflectionOnlyField"));

                throw new FieldAccessException();
            }

            if (doCheckConsistency)
                CheckConsistency(obj);

            RuntimeTypeHandle fieldType = FieldType.TypeHandle;
            if (doVisibilityCheck &&(m_invocationFlags & INVOCATION_FLAGS_NEED_SECURITY) != 0) 
                PerformVisibilityCheckOnField(m_fieldHandle.Value, obj, m_declaringType.TypeHandle.Value,                                               m_fieldAttributes, m_invocationFlags & ~INVOCATION_FLAGS_SPECIAL_FIELD);

            bool domainInitialized = false;
            if (declaringType == null)
            {
                return m_fieldHandle.GetValue(obj, fieldType, RuntimeTypeHandle.EmptyHandle, ref domainInitialized);
            }
            else
            {
                domainInitialized = declaringType.DomainInitialized;
                object retVal = m_fieldHandle.GetValue(obj, fieldType, DeclaringType.TypeHandle, ref domainInitialized);
                declaringType.DomainInitialized = domainInitialized;
                return retVal;
            }               
        } 

        #endregion

        #region MemberInfo Overrides
        public override String Name 
        {
            get
            {
                if (m_name == null)
                    m_name = m_fieldHandle.GetName();

                return m_name;
            }
        }

        public override int MetadataToken { get { return m_fieldHandle.GetToken(); } }      
        public override Module Module { get { return m_fieldHandle.GetApproxDeclaringType().GetModuleHandle().GetModule(); } }
        #endregion

        #region FieldInfo Overrides
        public override Object GetValue(Object obj)
        {
            return InternalGetValue(obj, true);
        } 
        
        public override object GetRawConstantValue() { throw new InvalidOperationException(); }

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public override Object GetValueDirect(TypedReference obj)
        {
            if (obj.IsNull)
                throw new ArgumentException(Environment.GetResourceString("Arg_TypedReference_Null"));
            
            return m_fieldHandle.GetValueDirect(FieldType.TypeHandle, obj, (DeclaringType == null) ? RuntimeTypeHandle.EmptyHandle : DeclaringType.TypeHandle);
        }

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo culture)
        {
            InternalSetValue(obj, value, invokeAttr, binder, culture, true);
        }

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public override void SetValueDirect(TypedReference obj, Object value)
        {
            if (obj.IsNull)
                throw new ArgumentException(Environment.GetResourceString("Arg_TypedReference_Null"));
            
            m_fieldHandle.SetValueDirect(FieldType.TypeHandle, obj, value, (DeclaringType == null) ? RuntimeTypeHandle.EmptyHandle : DeclaringType.TypeHandle);
        }

        public override RuntimeFieldHandle FieldHandle 
        {
            get
            {
                Type declaringType = DeclaringType;
                if ((declaringType == null && Module.Assembly.ReflectionOnly) || declaringType is ReflectionOnlyType)
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NotAllowedInReflectionOnly"));
                return m_fieldHandle;
            }
        }

        internal override RuntimeFieldHandle GetFieldHandle() 
        {
            return m_fieldHandle;
        }

        public override FieldAttributes Attributes 
        {
            get
            {
                return m_fieldAttributes;
            }
        }

        public override Type FieldType 
        {
            get
            {
                if (m_fieldType == null) 
                    m_fieldType = new Signature(m_fieldHandle, DeclaringTypeHandle).FieldTypeHandle.GetRuntimeType();

                return m_fieldType;
            }
        }       
        
        public override Type[] GetRequiredCustomModifiers()
        {
            return new Signature(m_fieldHandle, DeclaringTypeHandle).GetCustomModifiers(1, true);
        }

        public override Type[] GetOptionalCustomModifiers()
        {
            return new Signature(m_fieldHandle, DeclaringTypeHandle).GetCustomModifiers(1, false);
        }

        #endregion
    }


    [Serializable()]
    internal sealed unsafe class MdFieldInfo : RuntimeFieldInfo, ISerializable
    {
        #region Private Data Members
        private int m_tkField;
        private string m_name;
        private Type m_fieldType;
        private FieldAttributes m_fieldAttributes;
        #endregion

        #region Constructor
        internal MdFieldInfo(
        int tkField, FieldAttributes fieldAttributes, RuntimeTypeHandle declaringTypeHandle, RuntimeTypeCache reflectedTypeCache, BindingFlags bindingFlags)
            : base(reflectedTypeCache, declaringTypeHandle.GetRuntimeType(), bindingFlags)
        {
            m_tkField = tkField;
            m_name = null; 
            m_fieldAttributes = fieldAttributes;
        }
        #endregion

        #region Internal Members
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal override bool CacheEquals(object o)
        {
            MdFieldInfo m = o as MdFieldInfo;

            if (m == null)
                return false;

            return m.m_tkField == m_tkField && 
                m_declaringType.GetTypeHandleInternal().GetModuleHandle().Equals(
                    m.m_declaringType.GetTypeHandleInternal().GetModuleHandle());
        }
        #endregion

        #region MemberInfo Overrides
        public override String Name 
        {
            get
            {
                if (m_name == null)
                    m_name = Module.MetadataImport.GetName(m_tkField).ToString();

                return m_name;
            }
        }

        public override int MetadataToken { get { return m_tkField; } }
        public override Module Module { get { return m_declaringType.Module; } }
        #endregion

        #region FieldInfo Overrides
        public override RuntimeFieldHandle FieldHandle { get { throw new NotSupportedException(); } }
        public override FieldAttributes Attributes { get { return m_fieldAttributes; } }

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public override Object GetValueDirect(TypedReference obj)
        {
            return GetValue(null);
        }

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public override void SetValueDirect(TypedReference obj,Object value)
        {
            throw new FieldAccessException(Environment.GetResourceString("Acc_ReadOnly"));
        }

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public unsafe override Object GetValue(Object obj)
        {
            return GetValue(false);
        }

        public unsafe override Object GetRawConstantValue() { return GetValue(true); }
        
        internal unsafe Object GetValue(bool raw)
        {
            // Cannot cache these because they could be user defined non-agile enumerations

            Object value = MdConstant.GetValue(Module.MetadataImport, m_tkField, FieldType.GetTypeHandleInternal(), raw);

            if (value == DBNull.Value)
                throw new NotSupportedException(Environment.GetResourceString("Arg_EnumLitValueNotFound"));

            return value;
        } 

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo culture)
        {
            throw new FieldAccessException(Environment.GetResourceString("Acc_ReadOnly"));
        }

        public override Type FieldType 
        {
            get
            {
                if (m_fieldType == null)
                {
                    ConstArray fieldMarshal = Module.MetadataImport.GetSigOfFieldDef(m_tkField);

                    m_fieldType = new Signature(fieldMarshal.Signature.ToPointer(), 
                        (int)fieldMarshal.Length, m_declaringType.GetTypeHandleInternal()).FieldTypeHandle.GetRuntimeType();
                }

                return m_fieldType;
            }
        }       
    
        public override Type[] GetRequiredCustomModifiers()
        {
            return new Type[0];
        }

        public override Type[] GetOptionalCustomModifiers()
        {
            return new Type[0];
        }

        #endregion
    }


    internal static class MdConstant
    {
        public static unsafe Object GetValue(MetadataImport scope, int token, RuntimeTypeHandle fieldTypeHandle, bool raw)
        {
            CorElementType corElementType = 0;
            long buffer = 0;
            int length;

            scope.GetDefaultValue(token, out buffer, out length, out corElementType);

            Type fieldType = Type.GetTypeFromHandle(fieldTypeHandle);

            if (fieldType.IsEnum && raw == false)
            {
                long defaultValue = 0;

                switch (corElementType)
                {
                    #region Switch
                    case CorElementType.Void:
                        return DBNull.Value;

                    case CorElementType.Char:
                        defaultValue = *(char*)&buffer;
                        break;

                    case CorElementType.I1:
                        defaultValue = *(sbyte*)&buffer;
                        break;

                    case CorElementType.U1:
                        defaultValue = *(byte*)&buffer;
                        break;

                    case CorElementType.I2:
                        defaultValue = *(short*)&buffer;
                        break;

                    case CorElementType.U2:
                        defaultValue = *(ushort*)&buffer;
                        break;

                    case CorElementType.I4:
                        defaultValue = *(int*)&buffer;
                        break;

                    case CorElementType.U4:
                        defaultValue = *(uint*)&buffer;
                        break;

                    case CorElementType.I8:
                        defaultValue = buffer;
                        break;

                    case CorElementType.U8:
                        defaultValue = buffer;
                        break;
                
                    default:
                        throw new FormatException(Environment.GetResourceString("Arg_BadLiteralFormat"));
                    #endregion
                }

                return RuntimeType.CreateEnum(fieldTypeHandle, defaultValue);                       
            }
            else if (fieldType == typeof(DateTime))
            {
                long defaultValue = 0;

                switch (corElementType)
                {
                    #region Switch
                    case CorElementType.Void:
                        return DBNull.Value;
                        
                    case CorElementType.I8:
                        defaultValue = buffer;
                        break;

                    case CorElementType.U8:
                        defaultValue = buffer;
                        break;
                
                    default:
                        throw new FormatException(Environment.GetResourceString("Arg_BadLiteralFormat"));
                    #endregion  
                }

                return new DateTime(defaultValue);
            }
            else
            {
                switch (corElementType)
                {
                    #region Switch
                    case CorElementType.Void:
                        return DBNull.Value;

                    case CorElementType.Char:
                        return *(char*)&buffer;

                    case CorElementType.I1:
                        return *(sbyte*)&buffer;

                    case CorElementType.U1:
                        return *(byte*)&buffer;

                    case CorElementType.I2:
                        return *(short*)&buffer;

                    case CorElementType.U2:
                        return *(ushort*)&buffer;

                    case CorElementType.I4:
                        return *(int*)&buffer;

                    case CorElementType.U4:
                        return *(uint*)&buffer;

                    case CorElementType.I8:
                        return buffer;

                    case CorElementType.U8:
                        return (ulong)buffer;
                
                    case CorElementType.Boolean :
                        return (*(byte*)&buffer != 0);

                    case CorElementType.R4 :
                        return *(float*)&buffer;

                    case CorElementType.R8:
                        return *(double*)&buffer;

                    case CorElementType.String:
                        return new String((char*)buffer, 0, length/2);

                    case CorElementType.Class:
                        return null;
                    
                    default:
                        throw new FormatException(Environment.GetResourceString("Arg_BadLiteralFormat"));
                    #endregion
                }
            }
        } 
    }


    internal static class Associates
    {
        [Flags]
        internal enum Attributes
        {
            ComposedOfAllVirtualMethods = 0x1,
            ComposedOfAllPrivateMethods = 0x2, 
            ComposedOfNoPublicMembers   = 0x4,
            ComposedOfNoStaticMembers   = 0x8,
        }

        internal static bool IncludeAccessor(MethodInfo associate, bool nonPublic)
        {
            if (associate == null)
                return false;

            if (nonPublic)
                return true;

            if (associate.IsPublic)
                return true;

            return false;
        }

        internal static unsafe RuntimeMethodInfo AssignAssociates(
            int tkMethod,
            RuntimeTypeHandle declaredTypeHandle,
            RuntimeTypeHandle reflectedTypeHandle)
        {
            if (MetadataToken.IsNullToken(tkMethod))
                return null;

            ASSERT.PRECONDITION(!declaredTypeHandle.IsNullHandle());
            ASSERT.PRECONDITION(!reflectedTypeHandle.IsNullHandle());

            bool isInherited = !declaredTypeHandle.Equals(reflectedTypeHandle);

            RuntimeMethodHandle associateMethodHandle = declaredTypeHandle.GetModuleHandle().ResolveMethodHandle(tkMethod, declaredTypeHandle.GetInstantiation(), new RuntimeTypeHandle[0]);
            //RuntimeMethodHandle associateMethodHandle = declaredTypeHandle.GetMethodFromToken(tkMethod);
            ASSERT.CONSISTENCY_CHECK(!associateMethodHandle.IsNullHandle(), "Failed to resolve associateRecord methodDef token");

            MethodAttributes methAttr = associateMethodHandle.GetAttributes();
            bool isPrivate =(methAttr & MethodAttributes.MemberAccessMask) == MethodAttributes.Private;
            bool isVirtual =(methAttr & MethodAttributes.Virtual) != 0;

            if (isInherited)
            {
                // ECMA MethodSemantics: "All methods for a given Property or Event shall have the same accessibility 
                //(ie the MemberAccessMask subfield of their Flags row) and cannot be CompilerControlled  [CLS]"
                // Consequently, a property may be composed of public and private methods. If the declared type !=
                // the reflected type, the private methods should not be exposed. Note that this implies that the 
                // identity of a property includes it's reflected type.
                if (isPrivate)
                    return null;

                // Note this is the first time the property was encountered walking from the most derived class 
                // towards the base class. It would seem to follow that any associated methods would not
                // be overriden -- but this is not necessarily true. A more derived class may have overriden a
                // virtual method associated with a property in a base class without associating the override with 
                // the same or any property in the derived class. 
                if (isVirtual)
                {
                    bool declaringTypeIsClass = 
                        (declaredTypeHandle.GetAttributes() & TypeAttributes.ClassSemanticsMask) == TypeAttributes.Class;

                    ASSERT.CONSISTENCY_CHECK(LOGIC.BIJECTION(declaringTypeIsClass, 
                        (reflectedTypeHandle.GetAttributes() & TypeAttributes.ClassSemanticsMask) == TypeAttributes.Class));

                    // It makes no sense to search for a virtual override of a method declared on an interface.
                    if (declaringTypeIsClass)
                    {
                        int slot = associateMethodHandle.GetSlot();

                        // Find the override visible from the reflected type
                        associateMethodHandle = reflectedTypeHandle.GetMethodAt(slot);
                    }
                }
            }

            MethodAttributes visibility = methAttr & MethodAttributes.MemberAccessMask;
            bool isPublic = visibility == MethodAttributes.Public;
            bool isNonProtectedInternal = visibility == MethodAttributes.Assembly;
            bool isStatic =(methAttr & MethodAttributes.Static) != 0;

            RuntimeMethodInfo associateMethod = 
                RuntimeType.GetMethodBase(reflectedTypeHandle, associateMethodHandle) as RuntimeMethodInfo;

            // suppose a property was mapped to a method not in the derivation hierarchy of the reflectedTypeHandle
            if (associateMethod == null)
                associateMethod = reflectedTypeHandle.GetRuntimeType().Module.ResolveMethod(tkMethod, null, null) as RuntimeMethodInfo;

            return associateMethod;
        }

        internal static unsafe void AssignAssociates(
            AssociateRecord* associates,
            int cAssociates, 
            RuntimeTypeHandle declaringTypeHandle,
            RuntimeTypeHandle reflectedTypeHandle,
            out RuntimeMethodInfo addOn,
            out RuntimeMethodInfo removeOn,
            out RuntimeMethodInfo fireOn,
            out RuntimeMethodInfo getter,
            out RuntimeMethodInfo setter,
            out MethodInfo[] other,
            out bool composedOfAllPrivateMethods,
            out BindingFlags bindingFlags)
        {
            addOn = removeOn = fireOn = getter = setter = null;
            other = null;

            Attributes attributes = 
                Attributes.ComposedOfAllPrivateMethods |
                Attributes.ComposedOfAllVirtualMethods |
                Attributes.ComposedOfNoPublicMembers |
                Attributes.ComposedOfNoStaticMembers;

            while(reflectedTypeHandle.IsGenericVariable())
                reflectedTypeHandle = reflectedTypeHandle.GetRuntimeType().BaseType.GetTypeHandleInternal();

            bool isInherited = !declaringTypeHandle.Equals(reflectedTypeHandle);

            ArrayList otherList = new ArrayList();

            for (int i = 0; i < cAssociates; i++)
            {   
                #region Assign each associate
                RuntimeMethodInfo associateMethod = 
                    AssignAssociates(associates[i].MethodDefToken, declaringTypeHandle, reflectedTypeHandle);

                if (associateMethod == null)
                    continue;

                MethodAttributes methAttr = associateMethod.Attributes;
                bool isPrivate =(methAttr & MethodAttributes.MemberAccessMask) == MethodAttributes.Private;
                bool isVirtual =(methAttr & MethodAttributes.Virtual) != 0;

                MethodAttributes visibility = methAttr & MethodAttributes.MemberAccessMask;
                bool isPublic = visibility == MethodAttributes.Public;
                bool isNonProtectedInternal = visibility == MethodAttributes.Assembly;
                bool isStatic =(methAttr & MethodAttributes.Static) != 0;

                if (isPublic)
                {
                    attributes &= ~Attributes.ComposedOfNoPublicMembers;
                    attributes &= ~Attributes.ComposedOfAllPrivateMethods;
                }
                else if (!isPrivate)
                {
                    attributes &= ~Attributes.ComposedOfAllPrivateMethods;
                }

                if (isStatic)
                    attributes &= ~Attributes.ComposedOfNoStaticMembers;

                if (!isVirtual)
                    attributes &= ~Attributes.ComposedOfAllVirtualMethods;
                #endregion

                if (associates[i].Semantics == MethodSemanticsAttributes.Setter)
                    setter = associateMethod;
                else if (associates[i].Semantics == MethodSemanticsAttributes.Getter)
                    getter = associateMethod;
                else if (associates[i].Semantics == MethodSemanticsAttributes.Fire)
                    fireOn = associateMethod;
                else if (associates[i].Semantics == MethodSemanticsAttributes.AddOn)
                    addOn = associateMethod;
                else if (associates[i].Semantics == MethodSemanticsAttributes.RemoveOn)
                    removeOn = associateMethod;
                else
                    otherList.Add(associateMethod);
            }

            bool isPseudoPublic = (attributes & Attributes.ComposedOfNoPublicMembers) == 0;
            bool isPseudoStatic = (attributes & Attributes.ComposedOfNoStaticMembers) == 0;
            bindingFlags = RuntimeType.FilterPreCalculate(isPseudoPublic, isInherited, isPseudoStatic);

            composedOfAllPrivateMethods =(attributes & Attributes.ComposedOfAllPrivateMethods) != 0;

            other = (MethodInfo[])otherList.ToArray(typeof(MethodInfo));
        }
    }


    [Serializable()]
    internal unsafe sealed class RuntimePropertyInfo : PropertyInfo, ISerializable
    {
        #region Private Data Members
        private int m_token;
        private string m_name;
        private void* m_utf8name;
        private PropertyAttributes m_flags;
        private RuntimeTypeCache m_reflectedTypeCache;
        private RuntimeMethodInfo m_getterMethod;
        private RuntimeMethodInfo m_setterMethod;
        private MethodInfo[] m_otherMethod;
        private RuntimeType m_declaringType;
        private BindingFlags m_bindingFlags;
        private Signature m_signature;
        #endregion

        #region Constructor
        internal RuntimePropertyInfo(
            int tkProperty, RuntimeType declaredType, RuntimeTypeCache reflectedTypeCache, out bool isPrivate)
        {
            ASSERT.PRECONDITION(declaredType != null);
            ASSERT.PRECONDITION(reflectedTypeCache != null);
            ASSERT.PRECONDITION(!reflectedTypeCache.IsGlobal);

            MetadataImport scope = declaredType.Module.MetadataImport;

            m_token = tkProperty;
            m_reflectedTypeCache = reflectedTypeCache;    
            m_declaringType = declaredType;
            
            RuntimeTypeHandle declaredTypeHandle = declaredType.GetTypeHandleInternal();
            RuntimeTypeHandle reflectedTypeHandle = reflectedTypeCache.RuntimeTypeHandle;
            RuntimeMethodInfo dummy;

            scope.GetPropertyProps(tkProperty, out m_utf8name, out m_flags, out MetadataArgs.Skip.ConstArray);
            int cAssociateRecord = scope.GetAssociatesCount(tkProperty);
            AssociateRecord* associateRecord = stackalloc AssociateRecord[cAssociateRecord];
            scope.GetAssociates(tkProperty, associateRecord, cAssociateRecord);
            Associates.AssignAssociates(associateRecord, cAssociateRecord, declaredTypeHandle, reflectedTypeHandle, 
                out dummy, out dummy, out dummy,
                out m_getterMethod, out m_setterMethod, out m_otherMethod,
                out isPrivate, out m_bindingFlags);
        }
        #endregion

        #region Internal Members
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal override bool CacheEquals(object o)
        {
            RuntimePropertyInfo m = o as RuntimePropertyInfo;

            if (m == null)
                return false;

            return m.m_token == m_token &&
                m_declaringType.GetTypeHandleInternal().GetModuleHandle().Equals(
                    m.m_declaringType.GetTypeHandleInternal().GetModuleHandle());
        }

        internal Signature Signature
        {
            get
            {
                if (m_signature == null)
                {
                    ConstArray sig;

                    void* name;
                    Module.MetadataImport.GetPropertyProps(
                        m_token, out name, out MetadataArgs.Skip.PropertyAttributes, out sig);

                    m_signature = new Signature(sig.Signature.ToPointer(), (int)sig.Length, m_declaringType.GetTypeHandleInternal());
                }

                return m_signature;
            }
        }
        internal BindingFlags BindingFlags { get { return m_bindingFlags; } }
        internal bool EqualsSig(RuntimePropertyInfo target)
        {
            //@Asymmetry - Legacy policy is to remove duplicate properties, including hidden properties. 
            //             The comparison is done by name and by sig. The EqualsSig comparison is expensive 
            //             but forutnetly it is only called when an inherited property is hidden by name or
            //             when an interfaces declare properies with the same signature. 

            ASSERT.PRECONDITION(Name.Equals(target.Name));
            ASSERT.PRECONDITION(this != target);
            ASSERT.PRECONDITION(this.ReflectedType == target.ReflectedType);

            return Signature.DiffSigs(target.Signature);
        }
        #endregion

        #region Object Overrides
        public override String ToString() 
        {
            string toString = PropertyType.SigToString() + " " + Name;

            RuntimeTypeHandle[] argumentHandles = Signature.Arguments;
            if (argumentHandles.Length > 0)
            {
                Type[] paramters = new Type[argumentHandles.Length];
                for(int i = 0; i < paramters.Length; i++)
                    paramters[i] = argumentHandles[i].GetRuntimeType();

                toString += " [" + RuntimeMethodInfo.ConstructParameters(paramters, Signature.CallingConvention) + "]";
            }

            return toString; 
        }
        #endregion

        #region ICustomAttributeProvider
        public override Object[] GetCustomAttributes(bool inherit)
        {
            return CustomAttribute.GetCustomAttributes(this, typeof(object) as RuntimeType);
        }

        public override Object[] GetCustomAttributes(Type attributeType, bool inherit)
        {
            if (attributeType == null)
                throw new ArgumentNullException("attributeType");

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"attributeType");

            return CustomAttribute.GetCustomAttributes(this, attributeRuntimeType);
        }

        public override bool IsDefined(Type attributeType, bool inherit)
        {
            if (attributeType == null)
                throw new ArgumentNullException("attributeType");

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"attributeType");

            return CustomAttribute.IsDefined(this, attributeRuntimeType);
        }
        #endregion

        #region MemberInfo Overrides
        public override MemberTypes MemberType { get { return MemberTypes.Property; } }
        public override String Name 
        {
            get 
            {
                if (m_name == null)
                    m_name = new Utf8String(m_utf8name).ToString();
                
                return m_name; 
            } 
        }
        public override Type DeclaringType { get { return m_declaringType; } }
        public override Type ReflectedType { get { return m_reflectedTypeCache.RuntimeType; } }
        public override int MetadataToken { get { return m_token; } }
        public override Module Module { get { return m_declaringType.Module; } }
        #endregion

        #region PropertyInfo Overrides

        #region Non Dynamic

        public override Type[] GetRequiredCustomModifiers()
        {
            return Signature.GetCustomModifiers(0, true);
        }
        
        public override Type[] GetOptionalCustomModifiers()
        {
            return Signature.GetCustomModifiers(0, false);
        }

        internal object GetConstantValue(bool raw)
        {
            Object defaultValue = MdConstant.GetValue(Module.MetadataImport, m_token, PropertyType.GetTypeHandleInternal(), raw);
        
            if (defaultValue == DBNull.Value)
                // Arg_EnumLitValueNotFound -> "Literal value was not found."
                throw new InvalidOperationException(Environment.GetResourceString("Arg_EnumLitValueNotFound"));
        
            return defaultValue;
        }
            
        public override object GetConstantValue() { return GetConstantValue(false); }

        public override object GetRawConstantValue() { return GetConstantValue(true); }

        public override MethodInfo[] GetAccessors(bool nonPublic) 
        {
            ArrayList accessorList = new ArrayList();

            if (Associates.IncludeAccessor(m_getterMethod, nonPublic))
                accessorList.Add(m_getterMethod);

            if (Associates.IncludeAccessor(m_setterMethod, nonPublic))
                accessorList.Add(m_setterMethod);

            if (m_otherMethod != null)
            {
                for(int i = 0; i < m_otherMethod.Length; i ++)
                {
                    if (Associates.IncludeAccessor(m_otherMethod[i] as MethodInfo, nonPublic))
                        accessorList.Add(m_otherMethod[i]);
                }
            }
            return accessorList.ToArray(typeof(MethodInfo)) as MethodInfo[];
        }

        public override Type PropertyType 
        {
            get { return Signature.ReturnTypeHandle.GetRuntimeType(); }
        }

        public override MethodInfo GetGetMethod(bool nonPublic) 
        {
            if (!Associates.IncludeAccessor(m_getterMethod, nonPublic))
                return null;

            return m_getterMethod;
        }

        public override MethodInfo GetSetMethod(bool nonPublic) 
        {
            if (!Associates.IncludeAccessor(m_setterMethod, nonPublic))
                return null;

            return m_setterMethod;
        }

        public override ParameterInfo[] GetIndexParameters() 
        {
            // @History - Logic ported from RTM

            int numParams = 0;
            ParameterInfo[] methParams = null;

            // First try to get the Get method.
            MethodInfo m = GetGetMethod(true);
            if (m != null)
            {
                // There is a Get method so use it.
                methParams = m.GetParametersNoCopy();
                numParams = methParams.Length;
            }
            else
            {
                // If there is no Get method then use the Set method.
                m = GetSetMethod(true);

                if (m != null)
                {
                    methParams = m.GetParametersNoCopy();
                    numParams = methParams.Length - 1;
                }
            }

            // Now copy over the parameter info's and change their 
            // owning member info to the current property info.

            if (methParams != null && methParams.Length == 0)
                return methParams;
            
            ParameterInfo[] propParams = new ParameterInfo[numParams];
            
            for(int i = 0; i < numParams; i++)
                propParams[i] = new ParameterInfo(methParams[i], this);

            return propParams;
        }

        public override PropertyAttributes Attributes 
        {
            get
            {
                return m_flags;
            }
        }

        public override bool CanRead 
        {
            get
            {
                return m_getterMethod != null;
            }
        }

        public override bool CanWrite 
        {
            get
            {
                return m_setterMethod != null;
            }
        }
        #endregion

        #region Dynamic
        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public override Object GetValue(Object obj,Object[] index) 
        {
            return GetValue(obj, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Static, 
                null, index, null);
        }

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public override Object GetValue(Object obj, BindingFlags invokeAttr, Binder binder, Object[] index, CultureInfo culture) 
        {
            
            MethodInfo m = GetGetMethod(true);
            if (m == null)
                throw new ArgumentException(System.Environment.GetResourceString("Arg_GetMethNotFnd"));
            return m.Invoke(obj, invokeAttr, binder, index, null); 
        }

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public override void SetValue(Object obj, Object value, Object[] index)
        {
            SetValue(obj,
                    value,
                    BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Static, 
                    null, 
                    index, 
                    null);
        }

        [DebuggerStepThroughAttribute]
        [Diagnostics.DebuggerHidden]
        public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, Object[] index, CultureInfo culture)
        {
             
            MethodInfo m = GetSetMethod(true);

            if (m == null)
                throw new ArgumentException(System.Environment.GetResourceString("Arg_SetMethNotFnd"));

            Object[] args = null;

            if (index != null) 
            {
                args = new Object[index.Length + 1];

                for(int i=0;i<index.Length;i++)
                    args[i] = index[i];

                args[index.Length] = value;
            }
            else 
            {
                args = new Object[1];
                args[0] = value;
            }

            m.Invoke(obj, invokeAttr, binder, args, culture);
        }
        #endregion

        #endregion

        #region ISerializable Implementation
        public void GetObjectData(SerializationInfo info, StreamingContext context) 
        {
            if (info == null) 
                throw new ArgumentNullException("info");

            MemberInfoSerializationHolder.GetSerializationInfo(info, Name, ReflectedType, ToString(), MemberTypes.Property);
        }
        #endregion
    }


    [Serializable()]
    internal unsafe sealed class RuntimeEventInfo : EventInfo, ISerializable
    {
        #region Private Data Members
        private int m_token;
        private EventAttributes m_flags;
        private string m_name;
        private void* m_utf8name;
        private RuntimeTypeCache m_reflectedTypeCache;
        private RuntimeMethodInfo m_addMethod;
        private RuntimeMethodInfo m_removeMethod;
        private RuntimeMethodInfo m_raiseMethod;
        private MethodInfo[] m_otherMethod;        
        private RuntimeType m_declaringType;
        private BindingFlags m_bindingFlags;
        #endregion

        #region Constructor
        internal RuntimeEventInfo()
        {
            // Used for dummy head node during population
        }
        internal RuntimeEventInfo(int tkEvent, RuntimeType declaredType, RuntimeTypeCache reflectedTypeCache, out bool isPrivate)
        {
            ASSERT.PRECONDITION(declaredType != null);
            ASSERT.PRECONDITION(reflectedTypeCache != null);
            ASSERT.PRECONDITION(!reflectedTypeCache.IsGlobal);

            MetadataImport scope = declaredType.Module.MetadataImport;

            m_token = tkEvent;
            m_reflectedTypeCache = reflectedTypeCache;        
            m_declaringType = declaredType;
            

            RuntimeTypeHandle declaredTypeHandle = declaredType.GetTypeHandleInternal();
            RuntimeTypeHandle reflectedTypeHandle = reflectedTypeCache.RuntimeTypeHandle;
            RuntimeMethodInfo dummy;

            scope.GetEventProps(tkEvent, out m_utf8name, out m_flags);
            int cAssociateRecord = scope.GetAssociatesCount(tkEvent);
            AssociateRecord* associateRecord = stackalloc AssociateRecord[cAssociateRecord];
            scope.GetAssociates(tkEvent, associateRecord, cAssociateRecord);
            Associates.AssignAssociates(associateRecord, cAssociateRecord, declaredTypeHandle, reflectedTypeHandle, 
                out m_addMethod, out m_removeMethod, out m_raiseMethod, 
                out dummy, out dummy, out m_otherMethod, out isPrivate, out m_bindingFlags);
        }
        #endregion

        #region Internal Members
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal override bool CacheEquals(object o)
        {
            RuntimeEventInfo m = o as RuntimeEventInfo;

            if (m == null)
                return false;

            return m.m_token == m_token &&
                m_declaringType.GetTypeHandleInternal().GetModuleHandle().Equals(
                    m.m_declaringType.GetTypeHandleInternal().GetModuleHandle());
        }

        internal BindingFlags BindingFlags { get { return m_bindingFlags; } }
        #endregion

        #region Object Overrides
        public override String ToString() 
        {
            if (m_addMethod == null || m_addMethod.GetParametersNoCopy().Length == 0)
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NoPublicAddMethod"));

            return m_addMethod.GetParametersNoCopy()[0].ParameterType.SigToString() + " " + Name;
        }
        #endregion

        #region ICustomAttributeProvider
        public override Object[] GetCustomAttributes(bool inherit)
        {
            return CustomAttribute.GetCustomAttributes(this, typeof(object) as RuntimeType);
        }

        public override Object[] GetCustomAttributes(Type attributeType, bool inherit)
        {
            if (attributeType == null)
                throw new ArgumentNullException("attributeType");

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"attributeType");

            return CustomAttribute.GetCustomAttributes(this, attributeRuntimeType);
        }

        public override bool IsDefined(Type attributeType, bool inherit)
        {
            if (attributeType == null)
                throw new ArgumentNullException("attributeType");

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"attributeType");

            return CustomAttribute.IsDefined(this, attributeRuntimeType);
        }
        #endregion

        #region MemberInfo Overrides
        public override MemberTypes MemberType { get { return MemberTypes.Event; } }
        public override String Name 
        {
            get 
            {
                if (m_name == null)
                    m_name = new Utf8String(m_utf8name).ToString();
                
                return m_name; 
            } 
        }
        public override Type DeclaringType { get { return m_declaringType; } }
        public override Type ReflectedType { get { return m_reflectedTypeCache.RuntimeType; } }
        public override int MetadataToken { get { return m_token; } }       
        public override Module Module { get { return m_declaringType.Module; } }
        #endregion

        #region ISerializable
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)] 		
        public void GetObjectData(SerializationInfo info, StreamingContext context) 
        {
            if (info == null)
                throw new ArgumentNullException("info");

            MemberInfoSerializationHolder.GetSerializationInfo(info, Name, ReflectedType, null, MemberTypes.Event);
        }
        #endregion

        #region EventInfo Overrides
        public override MethodInfo[] GetOtherMethods(bool nonPublic) 
        {
            ArrayList ret = new ArrayList();

            if (m_otherMethod == null)
                return new MethodInfo[0];

            for(int i = 0; i < m_otherMethod.Length; i ++)
            {
                if (Associates.IncludeAccessor((MethodInfo)m_otherMethod[i], nonPublic))
                    ret.Add(m_otherMethod[i]);
            }
            
            return ret.ToArray(typeof(MethodInfo)) as MethodInfo[];
        }

        public override MethodInfo GetAddMethod(bool nonPublic)
        {
            if (!Associates.IncludeAccessor(m_addMethod, nonPublic))
                return null;

            return m_addMethod;
        }

        public override MethodInfo GetRemoveMethod(bool nonPublic)
        {
            if (!Associates.IncludeAccessor(m_removeMethod, nonPublic))
                return null;

            return m_removeMethod;
        }

        public override MethodInfo GetRaiseMethod(bool nonPublic)
        {
            if (!Associates.IncludeAccessor(m_raiseMethod, nonPublic))
                return null;

            return m_raiseMethod;
        }

        public override EventAttributes Attributes 
        {
            get
            {
                return m_flags;
            }
        }
        #endregion    
    }


    [Serializable()]
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_ParameterInfo))]
[System.Runtime.InteropServices.ComVisible(true)]
    public unsafe class ParameterInfo : _ParameterInfo, ICustomAttributeProvider
    {
        #region Static Members
        internal unsafe static ParameterInfo[] GetParameters(MethodBase method, MemberInfo member, Signature sig)
        {
            ParameterInfo dummy;
            return GetParameters(method, member, sig, out dummy, false);
        }
        
        internal unsafe static ParameterInfo GetReturnParameter(MethodBase method, MemberInfo member, Signature sig)
        {
            ParameterInfo returnParameter;
            GetParameters(method, member, sig, out returnParameter, true);
            return returnParameter;
        }
        
        internal unsafe static ParameterInfo[] GetParameters(
            MethodBase method, MemberInfo member, Signature sig, out ParameterInfo returnParameter, bool fetchReturnParameter)
        {
            RuntimeMethodHandle methodHandle = method.GetMethodHandle();
            returnParameter = null;
            int sigArgCount = sig.Arguments.Length;
            ParameterInfo[] args = fetchReturnParameter ? null : new ParameterInfo[sigArgCount];

            int tkMethodDef = methodHandle.GetMethodDef();
            int cParamDefs = 0;

            // Not all methods have tokens. Arrays, pointers and byRef types do not have tokens as they
            // are generated on the fly by the runtime. 
            if (!MdToken.IsNullToken(tkMethodDef))
            {
                MetadataImport scope = methodHandle.GetDeclaringType().GetModuleHandle().GetMetadataImport();
                cParamDefs = scope.EnumParamsCount(tkMethodDef);
                int* tkParamDefs = stackalloc int[cParamDefs];
                scope.EnumParams(tkMethodDef, tkParamDefs, cParamDefs);

                // Not all parameters have tokens. Parameters may have no token 
                // if they have no name and no attributes.
                ASSERT.CONSISTENCY_CHECK(cParamDefs <= sigArgCount + 1 /* return type */);
              

                for(uint i = 0; i < cParamDefs; i ++)
                {
                    #region Populate ParameterInfos
                    ParameterAttributes attr;
                    int position, tkParamDef = tkParamDefs[i];

                    scope.GetParamDefProps(tkParamDef, out position, out attr);

                    position --;

                    if (fetchReturnParameter == true && position == -1)
                    {
                        ASSERT.CONSISTENCY_CHECK(returnParameter == null);
                        returnParameter = new ParameterInfo(sig, scope, tkParamDef, position, attr, member);
                    }
                    else if (fetchReturnParameter == false && position >= 0)
                    {
                        ASSERT.CONSISTENCY_CHECK(position < sigArgCount);
                        args[position] = new ParameterInfo(sig, scope, tkParamDef, position, attr, member);
                    }
                    #endregion
                }
            }

            // Fill in empty ParameterInfos for those without tokens
            if (fetchReturnParameter)
            {
                if (returnParameter == null) 
                {
                    returnParameter = new ParameterInfo(sig, MetadataImport.EmptyImport, 0, -1,(ParameterAttributes)0, member);
                }
            }
            else
            {
                if (cParamDefs < args.Length + 1)
                {
                    for(int i = 0; i < args.Length; i ++)
                    {
                        if (args[i] != null)
                            continue;

                        args[i] = new ParameterInfo(sig, MetadataImport.EmptyImport, 0, i,(ParameterAttributes)0, member);
                    }
                }
            }

            return args;
        }
        #endregion

        #region Private Statics
        private static readonly Type s_DecimalConstantAttributeType = typeof(DecimalConstantAttribute);
        private static readonly Type s_CustomConstantAttributeType = typeof(CustomConstantAttribute);
        private static Type ParameterInfoType = typeof(System.Reflection.ParameterInfo);
        #endregion

        #region Definitions
        [Flags]
        private enum WhatIsCached
        {
            Nothing = 0x0,
            Name = 0x1,
            ParameterType = 0x2,
            DefaultValue = 0x4,
            All = Name | ParameterType | DefaultValue
        }
        #endregion

        #region Legacy Protected Members
        protected String NameImpl; 
        protected Type ClassImpl; 
        protected int PositionImpl; 
        protected ParameterAttributes AttrsImpl; 
        protected Object DefaultValueImpl; // cannot cache this as it may be non agile user defined enum
        protected MemberInfo MemberImpl;
        #endregion

        #region Legacy Private Members
        // These are here only for backwards compatibility -- they are not set
        // until this instance is serialized, so don't rely on their values from
        // arbitrary code.
#pragma warning disable 414
        private IntPtr _importer;
        private int _token;
        private bool bExtraConstChecked;
#pragma warning restore 414
        #endregion

        #region Private Data Members
        [NonSerialized] private int m_tkParamDef;
        [NonSerialized] private MetadataImport m_scope;
        [NonSerialized] private Signature m_signature;
        [NonSerialized] private volatile bool m_nameIsCached = false;
        [NonSerialized] private readonly bool m_noDefaultValue = false;        
        #endregion

        #region VTS magic to serialize/deserialized to/from pre-Whidbey endpoints.
        [OnSerializing]
        private void OnSerializing(StreamingContext context)
        {

            Object dummy;
            dummy = ParameterType;
            dummy = Name;
            DefaultValueImpl = DefaultValue;

            // Now set the legacy fields that the current implementation doesn't
            // use any more. Note that _importer is a raw pointer that should
            // never have been serialized in V1. We set it to zero here; if the
            // deserializer uses it (by calling GetCustomAttributes() on this
            // instance) they'll AV, but at least it will be a well defined
            // exception and not a random AV.
            _importer = IntPtr.Zero;
            _token = m_tkParamDef;
            bExtraConstChecked = false;
        }

        [OnDeserialized]
        private void OnDeserialized(StreamingContext context)
        {
            // Once all the serializable fields have come in we can setup this
            // instance based on just two of them (MemberImpl and PositionImpl).
            // Use these members to lookup a template ParameterInfo then clone
            // that instance into this one.

            ParameterInfo targetInfo = null;

            if (MemberImpl == null) 
                throw new SerializationException(Environment.GetResourceString(ResId.Serialization_InsufficientState));
    
            ParameterInfo[] args = null;

            switch (MemberImpl.MemberType) 
            {
                case MemberTypes.Constructor:
                case MemberTypes.Method:
                    if (PositionImpl == -1)
                    {
                        if (MemberImpl.MemberType == MemberTypes.Method)
                            targetInfo = ((MethodInfo)MemberImpl).ReturnParameter;
                        else
                            throw new SerializationException(Environment.GetResourceString(ResId.Serialization_BadParameterInfo));
                    }
                    else
                    {
                        args = ((MethodBase)MemberImpl).GetParametersNoCopy();

                        if (args != null && PositionImpl < args.Length) 
                            targetInfo = args[PositionImpl];
                        else
                            throw new SerializationException(Environment.GetResourceString(ResId.Serialization_BadParameterInfo));
                    }
                    break;

                case MemberTypes.Property:
                    args = ((PropertyInfo)MemberImpl).GetIndexParameters();

                    if (args != null && PositionImpl > -1 && PositionImpl < args.Length) 
                        targetInfo = args[PositionImpl];
                    else
                        throw new SerializationException(Environment.GetResourceString(ResId.Serialization_BadParameterInfo));
                    break;

                default:
                    throw new SerializationException(Environment.GetResourceString(ResId.Serialization_NoParameterInfo));
            }   

            // We've got a ParameterInfo that matches the incoming information,
            // clone it into ourselves. We really only need to copy the private
            // members we didn't receive via serialization.
            ASSERT.PRECONDITION(targetInfo != null);

            m_tkParamDef = targetInfo.m_tkParamDef;
            m_scope = targetInfo.m_scope;
            m_signature = targetInfo.m_signature;
            m_nameIsCached = true;
        }
        #endregion

        #region Constructor
        protected ParameterInfo() 
        { 
            m_nameIsCached = true;
            m_noDefaultValue = true;
        } 
        
        internal ParameterInfo(ParameterInfo accessor, RuntimePropertyInfo property) : this(accessor, (MemberInfo)property)
        {
            m_signature = property.Signature;
        }

        internal ParameterInfo(ParameterInfo accessor, MethodBuilderInstantiation method) : this(accessor, (MemberInfo)method)
        {
            m_signature = accessor.m_signature;

            if (ClassImpl.IsGenericParameter)
                ClassImpl = method.GetGenericArguments()[ClassImpl.GenericParameterPosition];
        }

        private ParameterInfo(ParameterInfo accessor, MemberInfo member)
        {
            // Change ownership
            MemberImpl = member;

            // Populate all the caches -- we inherit this behavior from RTM
            NameImpl = accessor.Name; 
            m_nameIsCached = true;
            ClassImpl = accessor.ParameterType;
            PositionImpl = accessor.Position;
            AttrsImpl = accessor.Attributes;

            // Strictly speeking, property's don't contain paramter tokens
            // However we need this to make ca's work... oh well...
            m_tkParamDef = MdToken.IsNullToken(accessor.MetadataToken) ? (int)MetadataTokenType.ParamDef : accessor.MetadataToken;
            m_scope = accessor.m_scope;
        }

        private ParameterInfo(
            Signature signature, MetadataImport scope, int tkParamDef, 
            int position, ParameterAttributes attributes, MemberInfo member) 
        {
            ASSERT.PRECONDITION(member != null);
            ASSERT.PRECONDITION(LOGIC.BIJECTION(MdToken.IsNullToken(tkParamDef), scope.Equals(null)));
            ASSERT.PRECONDITION(LOGIC.IMPLIES(!MdToken.IsNullToken(tkParamDef), 
                MdToken.IsTokenOfType(tkParamDef, MetadataTokenType.ParamDef)));

            PositionImpl = position;
            MemberImpl = member;
            m_signature = signature;
            m_tkParamDef = MdToken.IsNullToken(tkParamDef) ? (int)MetadataTokenType.ParamDef : tkParamDef;
            m_scope = scope;
            AttrsImpl = attributes;

            ClassImpl = null;
            NameImpl = null;
        }

        // ctor for no metadata MethodInfo
        internal ParameterInfo(MethodInfo owner, String name, RuntimeType parameterType, int position)
        {
            MemberImpl = owner;
            NameImpl = name;
            m_nameIsCached = true;
            m_noDefaultValue = true;
            ClassImpl = parameterType;
            PositionImpl = position;
            AttrsImpl = ParameterAttributes.None;
            m_tkParamDef = (int)MetadataTokenType.ParamDef;
            m_scope = MetadataImport.EmptyImport;
        }
        #endregion

        #region Private Members
        private bool IsLegacyParameterInfo { get { return GetType() != typeof(ParameterInfo); } }
        #endregion

        #region Internal Members
        internal void SetName(String name) 
        {
            NameImpl = name;
        }
        
        internal void SetAttributes(ParameterAttributes attributes) 
        {
            AttrsImpl = attributes;
        }
        #endregion

        #region Public Methods
        public virtual Type ParameterType 
        { 
            get 
            { 
                // only instance of ParameterInfo has ClassImpl, all its subclasses don't
                if (ClassImpl == null && this.GetType () == typeof (ParameterInfo))
                {
                    RuntimeTypeHandle parameterTypeHandle;
                    if (PositionImpl == -1)
                        parameterTypeHandle = m_signature.ReturnTypeHandle;
                    else
                        parameterTypeHandle = m_signature.Arguments[PositionImpl];

                    ASSERT.CONSISTENCY_CHECK(!parameterTypeHandle.IsNullHandle());
                    // different thread could only write ClassImpl to the same value, so race is not a problem here
                    ClassImpl = parameterTypeHandle.GetRuntimeType();
                }
                
                BCLDebug.Assert (ClassImpl != null ||this.GetType () != typeof (ParameterInfo), 
                                        "ClassImple should already be initialized for ParameterInfo class");

                return ClassImpl; 
            } 
        }            
        
        public virtual String Name 
        { 
            get 
            { 
                if (!m_nameIsCached)
                {
                    if (!MdToken.IsNullToken(m_tkParamDef))
                    {
                        string name;
                        name = m_scope.GetName(m_tkParamDef).ToString(); 
                        NameImpl = name;                        
                    }  
                    
                    // other threads could only write it to true, so race is OK
                    // this field is volatile, so the write ordering is guaranteed
                    m_nameIsCached = true;                    
                }
                
                // name may be null
                return NameImpl; 
            } 
        }               
        public virtual Object DefaultValue { get { return GetDefaultValue(false); } }
        public virtual Object RawDefaultValue  { get { return GetDefaultValue(true); } } 
        internal Object GetDefaultValue(bool raw)
        {
            // Cannot cache because default value could be non-agile user defined enumeration.
            object defaultValue = null;

            // for dynamic method we pretend to have cached the value so we do not go to metadata
            if (!m_noDefaultValue) 
            {
                if (ParameterType == typeof(DateTime))
                {
                    if (raw)
                    {
                        CustomAttributeTypedArgument value = 
                            CustomAttributeData.Filter(
                                CustomAttributeData.GetCustomAttributes(this), typeof(DateTimeConstantAttribute), 0);
                        
                        if (value.ArgumentType != null)
                            return new DateTime((long)value.Value);
                    }
                    else
                    {
                        object[] dt = GetCustomAttributes(typeof(DateTimeConstantAttribute), false);
                        if (dt != null && dt.Length != 0)
                            return ((DateTimeConstantAttribute)dt[0]).Value;                   
                    }
                }
                           
                #region Look for a default value in metadata
                if (!MdToken.IsNullToken(m_tkParamDef))
                {
                    defaultValue = MdConstant.GetValue(m_scope, m_tkParamDef, ParameterType.GetTypeHandleInternal(), raw);
                }
                #endregion

                if (defaultValue == DBNull.Value) 
                {
                    #region Look for a default value in the custom attributes
                    if (raw)
                    {
                        System.Collections.Generic.IList<CustomAttributeData> attrs = CustomAttributeData.GetCustomAttributes(this);
                        CustomAttributeTypedArgument value = CustomAttributeData.Filter(
                            attrs, s_CustomConstantAttributeType, "Value");

                        if (value.ArgumentType == null)
                        {
                            value = CustomAttributeData.Filter(
                                attrs, s_DecimalConstantAttributeType, "Value");

                            
                            if (value.ArgumentType == null)
                            {
                                for (int i = 0; i < attrs.Count; i++)
                                {
                                    if (attrs[i].Constructor.DeclaringType == s_DecimalConstantAttributeType)
                                    {
                                        ParameterInfo[] parameters = attrs[i].Constructor.GetParameters();

                                        if (parameters.Length != 0)
                                        {
                                            if (parameters[2].ParameterType == typeof(uint))
                                            {
                                                System.Collections.Generic.IList<CustomAttributeTypedArgument> args = attrs[i].ConstructorArguments;
                                                int low = (int)(UInt32)args[4].Value;
                                                int mid = (int)(UInt32)args[3].Value;
                                                int hi = (int)(UInt32)args[2].Value;
                                                byte sign = (byte)args[1].Value;
                                                byte scale = (byte)args[0].Value;
                                                value = new CustomAttributeTypedArgument(
                                                    new System.Decimal(low, mid, hi, (sign != 0), scale));
                                            }
                                            else
                                            {
                                                System.Collections.Generic.IList<CustomAttributeTypedArgument> args = attrs[i].ConstructorArguments;
                                                int low = (int)args[4].Value;
                                                int mid = (int)args[3].Value;
                                                int hi = (int)args[2].Value;
                                                byte sign = (byte)args[1].Value;
                                                byte scale = (byte)args[0].Value;
                                                value = new CustomAttributeTypedArgument(
                                                    new System.Decimal(low, mid, hi, (sign != 0), scale));
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        if (value.ArgumentType != null)
                            defaultValue = value.Value;
                    }
                    else
                    {
                        Object[] CustomAttrs = GetCustomAttributes(s_CustomConstantAttributeType, false);
                        if (CustomAttrs.Length != 0)
                        {
                            defaultValue =((CustomConstantAttribute)CustomAttrs[0]).Value;
                        }
                        else 
                        {
                            CustomAttrs = GetCustomAttributes(s_DecimalConstantAttributeType, false);
                            if (CustomAttrs.Length != 0)
                            {
                                defaultValue = ((DecimalConstantAttribute)CustomAttrs[0]).Value;
                            }
                        }
                    }
                    #endregion
                }

                if (defaultValue == DBNull.Value) 
                {
                    #region Handle case if no default value was found
                    if (IsOptional)
                    {
                        // If the argument is marked as optional then the default value is Missing.Value.
                        defaultValue = Type.Missing;
                    }
                    #endregion
                }
            }

            return defaultValue;
        }
        public virtual int Position { get { return PositionImpl; } }                                    
        public virtual ParameterAttributes Attributes { get { return AttrsImpl; } }      
        public virtual MemberInfo Member { get { return MemberImpl; } }                            
        public bool IsIn { get { return((Attributes & ParameterAttributes.In) != 0); } }        
        public bool IsOut { get { return((Attributes & ParameterAttributes.Out) != 0); } }        
        public bool IsLcid { get { return((Attributes & ParameterAttributes.Lcid) != 0); } }        
        public bool IsRetval { get { return((Attributes & ParameterAttributes.Retval) != 0); } }        
        public bool IsOptional { get { return((Attributes & ParameterAttributes.Optional) != 0); } } 
        public int MetadataToken { get { return m_tkParamDef; } }       
        
        public virtual Type[] GetRequiredCustomModifiers() 
        {
            if (IsLegacyParameterInfo)
                return new Type[0];

            return m_signature.GetCustomModifiers(PositionImpl + 1, true);
        }

        public virtual Type[] GetOptionalCustomModifiers() 
        {
            if (IsLegacyParameterInfo)
                return new Type[0];

            return m_signature.GetCustomModifiers(PositionImpl + 1, false);            
        }

        #endregion

        #region Object Overrides
        public override String ToString() 
        {
            return ParameterType.SigToString() + " " + Name;
        }
        #endregion

        #region ICustomAttributeProvider
        public virtual Object[] GetCustomAttributes(bool inherit)
        {
            if (IsLegacyParameterInfo)
                return null;
            
            if (MdToken.IsNullToken(m_tkParamDef))
                return new object[0];

            return CustomAttribute.GetCustomAttributes(this, typeof(object) as RuntimeType);
        }

        public virtual Object[] GetCustomAttributes(Type attributeType, bool inherit)
        {
            if (IsLegacyParameterInfo)
                return null;
            
            if (attributeType == null)
                throw new ArgumentNullException("attributeType");
            
            if (MdToken.IsNullToken(m_tkParamDef))
                return new object[0];

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"attributeType");

            return CustomAttribute.GetCustomAttributes(this, attributeRuntimeType);
        }

        public virtual bool IsDefined(Type attributeType, bool inherit)
        {
            if (IsLegacyParameterInfo)
                return false;
            
            if (attributeType == null)
                throw new ArgumentNullException("attributeType");
            
            if (MdToken.IsNullToken(m_tkParamDef))
                return false;

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"attributeType");

            return CustomAttribute.IsDefined(this, attributeRuntimeType);
        }
        #endregion

        #region Remoting Cache
        private InternalCache m_cachedData;

        internal InternalCache Cache 
        {
            get 
            {
                // This grabs an internal copy of m_cachedData and uses
                // that instead of looking at m_cachedData directly because
                // the cache may get cleared asynchronously.  This prevents
                // us from having to take a lock.
                InternalCache cache = m_cachedData;
                if (cache == null) 
                {
                    cache = new InternalCache("ParameterInfo");
                    InternalCache ret = Interlocked.CompareExchange(ref m_cachedData, cache, null);
                    if (ret != null)
                        cache = ret;

                    GC.ClearCache+=new ClearCacheHandler(OnCacheClear);
                }
                return cache;
            } 
        }

        internal void OnCacheClear(Object sender, ClearCacheEventArgs cacheEventArgs) 
        {
            m_cachedData = null;
        }
        #endregion

        void _ParameterInfo.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _ParameterInfo.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _ParameterInfo.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _ParameterInfo.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }
    }
}

