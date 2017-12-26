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
/*=============================================================================
**
** Class: Exception
**
**
** Purpose: The base class for all exceptional conditions.
**
**
=============================================================================*/

namespace System {
    using System;
    using System.Runtime.InteropServices;
    using System.Runtime.CompilerServices;
    using MethodInfo = System.Reflection.MethodInfo;
    using MethodBase = System.Reflection.MethodBase;
    using System.Runtime.Serialization;
    using System.Diagnostics;
    using System.Security.Permissions;
    using System.Security;
    using System.IO;
    using System.Text;
    using System.Reflection;
    using System.Collections;
    using System.Globalization;

        
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_Exception))]
    [Serializable()] 
    [ComVisible(true)]
    public class Exception : ISerializable, _Exception
    {
        public Exception() {
            _message = null;
            _stackTrace = null;
            _dynamicMethods = null;
            HResult = __HResults.COR_E_EXCEPTION;
            _xcode = _COMPlusExceptionCode;
            _xptrs = (IntPtr) 0;
        }
    
        public Exception(String message) {
            _message = message;
            _stackTrace = null;
            _dynamicMethods = null;
            HResult = __HResults.COR_E_EXCEPTION;
            _xcode = _COMPlusExceptionCode;
            _xptrs = (IntPtr) 0;
        }
    
        // Creates a new Exception.  All derived classes should 
        // provide this constructor.
        // Note: the stack trace is not started until the exception 
        // is thrown
        // 
        public Exception (String message, Exception innerException) {
            _message = message;
            _stackTrace = null;
            _dynamicMethods = null;
            _innerException = innerException;
            HResult = __HResults.COR_E_EXCEPTION;
            _xcode = _COMPlusExceptionCode;
            _xptrs = (IntPtr) 0;
        }

        protected Exception(SerializationInfo info, StreamingContext context) 
        {
            if (info==null)
                throw new ArgumentNullException("info");
    
            _className = info.GetString("ClassName");
            _message = info.GetString("Message");
            _data = (IDictionary)(info.GetValueNoThrow("Data",typeof(IDictionary)));
            _innerException = (Exception)(info.GetValue("InnerException",typeof(Exception)));
            _helpURL = info.GetString("HelpURL");
            _stackTraceString = info.GetString("StackTraceString");
            _remoteStackTraceString = info.GetString("RemoteStackTraceString");
            _remoteStackIndex = info.GetInt32("RemoteStackIndex");

            _exceptionMethodString = (String)(info.GetValue("ExceptionMethod",typeof(String)));
            HResult = info.GetInt32("HResult");
            _source = info.GetString("Source");
    
            if (_className == null || HResult==0)
                throw new SerializationException(Environment.GetResourceString("Serialization_InsufficientState"));
            
            // If we are constructing a new exception after a cross-appdomain call...
            if (context.State == StreamingContextStates.CrossAppDomain)
            {
                // ...this new exception may get thrown.  It is logically a re-throw, but 
                //  physically a brand-new exception.  Since the stack trace is cleared 
                //  on a new exception, the "_remoteStackTraceString" is provided to 
                //  effectively import a stack trace from a "remote" exception.  So,
                //  move the _stackTraceString into the _remoteStackTraceString.  Note
                //  that if there is an existing _remoteStackTraceString, it will be 
                //  preserved at the head of the new string, so everything works as 
                //  expected.
                // Even if this exception is NOT thrown, things will still work as expected
                //  because the StackTrace property returns the concatenation of the
                //  _remoteStackTraceString and the _stackTraceString.
                _remoteStackTraceString = _remoteStackTraceString + _stackTraceString;
                _stackTraceString = null;
            }

        }
        
        
        public virtual String Message {
               get {  
                if (_message == null) {
                    if (_className==null) {
                        _className = GetClassName();
                    }
                    return String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Exception_WasThrown"), _className);
                } else {
                    return _message;
                }
            }
        }

        public virtual IDictionary Data { 
            get {
                return GetDataInternal();
            }
        }
    
        // This method is internal so that callers can't override which function they call.
        internal IDictionary GetDataInternal()
        {
            if (_data == null)
                if (IsImmutableAgileException(this))
                    _data = new EmptyReadOnlyDictionaryInternal();
                else
                    _data = new ListDictionaryInternal();
                
            return _data;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern bool IsImmutableAgileException(Exception e);

    
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern String GetClassName();
    
        // Retrieves the lowest exception (inner most) for the given Exception.
        // This will traverse exceptions using the innerException property.
        //
        public virtual Exception GetBaseException() 
        {
            Exception inner = InnerException;
            Exception back = this;
            
            while (inner != null) {
                back = inner;
                inner = inner.InnerException;
            }
            
            return back;
        }
        
        // Returns the inner exception contained in this exception
        // 
        public Exception InnerException {
            get { return _innerException; }
        }
        
            
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        static extern unsafe private void* _InternalGetMethod(Object stackTrace);
        static unsafe private RuntimeMethodHandle InternalGetMethod(Object stackTrace)
        {
            return new RuntimeMethodHandle(_InternalGetMethod(stackTrace));
        }
    
    
        public MethodBase TargetSite {
            get {
                return GetTargetSiteInternal();
            }
        }
    

        // this function is provided as a private helper to avoid the security demand
        private MethodBase GetTargetSiteInternal() {
            if (_exceptionMethod!=null) {
                return _exceptionMethod;
            }
            if (_stackTrace==null) {
                return null;
            }

            if (_exceptionMethodString!=null) {
                _exceptionMethod = GetExceptionMethodFromString();
            } else {
                RuntimeMethodHandle method = InternalGetMethod(_stackTrace).GetTypicalMethodDefinition();
                _exceptionMethod = RuntimeType.GetMethodBase(method);
            }
            return _exceptionMethod;
        }
    
        // Returns the stack trace as a string.  If no stack trace is
        // available, null is returned.
        public virtual String StackTrace 
        {
            get 
            {
                // if no stack trace, try to get one
                if (_stackTraceString!=null) 
                {
                    return _remoteStackTraceString + _stackTraceString;
                }
                if (_stackTrace==null) 
                {
                    return _remoteStackTraceString;
                }

                // Obtain the stack trace string. Note that since Environment.GetStackTrace
                // will add the path to the source file if the PDB is present and a demand
                // for PathDiscoveryPermission succeeds, we need to make sure we don't store
                // the stack trace string in the _stackTraceString member variable.
                String tempStackTraceString = Environment.GetStackTrace(this, true);
                return _remoteStackTraceString + tempStackTraceString;
            }
        }
    
        internal void SetErrorCode(int hr)
        {
            HResult = hr;
        }
        
        // Sets the help link for this exception.
        // This should be in a URL/URN form, such as:
        // "file:///C:/Applications/Bazzal/help.html#ErrorNum42"
        // Changed to be a read-write String and not return an exception
        public virtual String HelpLink
        {
            get
            {
                return _helpURL;
            }
            set
            {
                _helpURL = value;
            }
        }
    
        public virtual String Source {
            get { 
                if (_source == null)
                {
                    StackTrace st = new StackTrace(this,true);
                    if (st.FrameCount>0)
                    {
                        StackFrame sf = st.GetFrame(0);
                        MethodBase method = sf.GetMethod();
                        _source = method.Module.Assembly.nGetSimpleName();
                    }
                }

                return _source;
            }
            set { _source = value; }
        }
        
        public override String ToString() {
            String message = Message;
            String s;
            if (_className==null) {
                _className = GetClassName();
            }

            if (message == null || message.Length <= 0) {
                s = _className;
            }
            else {
                s = _className + ": " + message;
            }

            if (_innerException!=null) {
                s = s + " ---> " + _innerException.ToString() + Environment.NewLine + "   " + Environment.GetResourceString("Exception_EndOfInnerExceptionStack");
            }


            if (StackTrace != null)
                s += Environment.NewLine + StackTrace;

            return s;
        }
    
        private String GetExceptionMethodString() {
            MethodBase methBase = GetTargetSiteInternal();
            if (methBase==null) {
                return null;
            }
            // Note that the newline separator is only a separator, chosen such that
            //  it won't (generally) occur in a method name.  This string is used 
            //  only for serialization of the Exception Method.
            char separator = '\n';
            StringBuilder result = new StringBuilder();
            if (methBase is ConstructorInfo) {
                RuntimeConstructorInfo rci = (RuntimeConstructorInfo)methBase;
                Type t = rci.ReflectedType;
                result.Append((int)MemberTypes.Constructor);
                result.Append(separator);
                result.Append(rci.Name);
                if (t!=null)
                {
                    result.Append(separator);
                    result.Append(t.Assembly.FullName);
                    result.Append(separator);
                    result.Append(t.FullName);
                }
                result.Append(separator);
                result.Append(rci.ToString());
            } else {
                BCLDebug.Assert(methBase is MethodInfo, "[Exception.GetExceptionMethodString]methBase is MethodInfo");
                RuntimeMethodInfo rmi = (RuntimeMethodInfo)methBase;
                Type t = rmi.DeclaringType;
                result.Append((int)MemberTypes.Method);
                result.Append(separator);
                result.Append(rmi.Name);
                result.Append(separator);
                result.Append(rmi.Module.Assembly.FullName);
                result.Append(separator);
                if (t != null)
                {
                    result.Append(t.FullName);
                    result.Append(separator);
                }
                result.Append(rmi.ToString());
            }
            
            return result.ToString();
        }

        private MethodBase GetExceptionMethodFromString() {
            BCLDebug.Assert(_exceptionMethodString != null, "Method string cannot be NULL!");
            String[] args = _exceptionMethodString.Split(new char[]{'\0', '\n'});
            if (args.Length!=5) {
                throw new SerializationException();
            }
            SerializationInfo si = new SerializationInfo(typeof(MemberInfoSerializationHolder), new FormatterConverter());
            si.AddValue("MemberType", (int)Int32.Parse(args[0], CultureInfo.InvariantCulture), typeof(Int32));
            si.AddValue("Name", args[1], typeof(String));
            si.AddValue("AssemblyName", args[2], typeof(String));
            si.AddValue("ClassName", args[3]);
            si.AddValue("Signature", args[4]);
            MethodBase result;
            StreamingContext sc = new StreamingContext(StreamingContextStates.All);
            try {
                result = (MethodBase)new MemberInfoSerializationHolder(si, sc).GetRealObject(sc);
            } catch (SerializationException) {
                result = null;
            }
            return result;
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)]
        public virtual void GetObjectData(SerializationInfo info, StreamingContext context) 
        {
            String tempStackTraceString = _stackTraceString;
        
            if (info==null) 
            {
                throw new ArgumentNullException("info");
            }
            if (_className==null) 
            {
                _className=GetClassName();
            }
    
            if (_stackTrace!=null) 
            {
                if (tempStackTraceString==null) 
                {
                    tempStackTraceString = Environment.GetStackTrace(this, true);
                }
                if (_exceptionMethod==null) 
                {
                    RuntimeMethodHandle method = InternalGetMethod(_stackTrace).GetTypicalMethodDefinition();
                    _exceptionMethod = RuntimeType.GetMethodBase(method);
                }
            }

            if (_source == null) 
            {
                _source = Source; // Set the Source information correctly before serialization
            }
    
            info.AddValue("ClassName", _className, typeof(String));
            info.AddValue("Message", _message, typeof(String));
            info.AddValue("Data", _data, typeof(IDictionary));
            info.AddValue("InnerException", _innerException, typeof(Exception));
            info.AddValue("HelpURL", _helpURL, typeof(String));
            info.AddValue("StackTraceString", tempStackTraceString, typeof(String));
            info.AddValue("RemoteStackTraceString", _remoteStackTraceString, typeof(String));
            info.AddValue("RemoteStackIndex", _remoteStackIndex, typeof(Int32));
            info.AddValue("ExceptionMethod", GetExceptionMethodString(), typeof(String));
            info.AddValue("HResult", HResult);
            info.AddValue("Source", _source, typeof(String));
        }

        // This is used by remoting to preserve the server side stack trace
        // by appending it to the message ... before the exception is rethrown
        // at the client call site.
        internal Exception PrepForRemoting()
        {
            String tmp = null;

            if (_remoteStackIndex == 0)
            {
                tmp = Environment.NewLine+ "Server stack trace: " + Environment.NewLine
                    + StackTrace 
                    + Environment.NewLine + Environment.NewLine 
                    + "Exception rethrown at ["+_remoteStackIndex+"]: " + Environment.NewLine;
            }
            else
            {
                tmp = StackTrace 
                    + Environment.NewLine + Environment.NewLine 
                    + "Exception rethrown at ["+_remoteStackIndex+"]: " + Environment.NewLine;
            }

            _remoteStackTraceString = tmp;
            _remoteStackIndex++;
            return this;
        }
    
        // This is used by the runtime when re-throwing a managed exception.  It will
        //  copy the stack trace to _remoteStackTraceString.
        internal void InternalPreserveStackTrace()
        {
            string tmpStackTraceString = StackTrace;
            if (tmpStackTraceString != null && tmpStackTraceString.Length > 0)
                _remoteStackTraceString = tmpStackTraceString + Environment.NewLine;
            _stackTrace = null;
            _stackTraceString = null;
        }

        private String _className;  //Needed for serialization.  
        private MethodBase _exceptionMethod;  //Needed for serialization.  
        private String _exceptionMethodString; //Needed for serialization. 
        internal String _message;
        private IDictionary _data;
        private Exception _innerException;
        private String _helpURL;
        private Object _stackTrace;

        private String _stackTraceString; //Needed for serialization.  
        private String _remoteStackTraceString;
        private int _remoteStackIndex;
#pragma warning disable 414  // Field is not used from managed.		
        // _dynamicMethods is an array of System.Resolver objects, used to keep
        // DynamicMethodDescs alive for the lifetime of the exception. We do this because
        // the _stackTrace field holds MethodDescs, and a DynamicMethodDesc can be destroyed
        // unless a System.Resolver object roots it.
        private Object _dynamicMethods; 
#pragma warning restore 414

        // @MANAGED: HResult is used from within the EE!  Rename with care - check VM directory
        internal int _HResult;     // HResult

        protected int HResult
        {
            get
            {
                return _HResult;
            }
            set
            {
                _HResult = value;
            }
        }
        
        private String _source;         // Mainly used by VB. 
        // WARNING: Don't delete/rename _xptrs and _xcode - used by functions
        // on Marshal class.  Native functions are in COMUtilNative.cpp & AppDomain
        private IntPtr _xptrs;             // Internal EE stuff 
#pragma warning disable 414  // Field is not used from managed.
        private int _xcode;             // Internal EE stuff 
#pragma warning restore 414
    // See clr\src\vm\excep.h's EXCEPTION_COMPLUS definition:
        private const int _COMPlusExceptionCode = unchecked((int)0xe0524f54);   // Win32 exception code for COM+ exceptions

        internal virtual String InternalToString()
        {
            try 
            {
                SecurityPermission sp= new SecurityPermission(SecurityPermissionFlag.ControlEvidence | SecurityPermissionFlag.ControlPolicy);
                sp.Assert();
            }
            catch  
            {
            }
            return ToString();
        }

        // this method is required so Object.GetType is not made virtual by the compiler
        public new Type GetType()
        {
            return base.GetType();
        }

        internal bool IsTransient
        {
            get {
                return nIsTransient(_HResult);
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern static bool nIsTransient(int hr);


        // This piece of infrastructure exists to help avoid deadlocks 
        // between parts of mscorlib that might throw an exception while 
        // holding a lock that are also used by mscorlib's ResourceManager
        // instance.  As a special case of code that may throw while holding
        // a lock, we also need to fix our asynchronous exceptions to use
        // Win32 resources as well (assuming we ever call a managed 
        // constructor on instances of them).  We should grow this set of
        // exception messages as we discover problems, then move the resources
        // involved to native code.
        internal enum ExceptionMessageKind
        {
            ThreadAbort = 1,
            ThreadInterrupted = 2,
            OutOfMemory = 3
        }

        // See comment on ExceptionMessageKind
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern String GetMessageFromNativeResources(ExceptionMessageKind kind);
    }
}

