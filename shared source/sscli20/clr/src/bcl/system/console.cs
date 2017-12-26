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
** Class: Console
**
**
** Purpose: This class provides access to the standard input, standard output
**          and standard error streams.
**
**
=============================================================================*/
namespace System {
    using System;
    using System.IO;
    using System.Text;
    using System.Globalization;
    using System.Security.Permissions;
    using Microsoft.Win32;
    using System.Runtime.CompilerServices;
    using System.Threading;
    using System.Runtime.InteropServices;
    using Microsoft.Win32.SafeHandles;
    using System.Runtime.ConstrainedExecution;
    using System.Runtime.Versioning;

    // Provides static fields for console input & output.  Use 
    // Console.In for input from the standard input stream (stdin),
    // Console.Out for output to stdout, and Console.Error
    // for output to stderr.  If any of those console streams are 
    // redirected from the command line, these streams will be redirected.
    // A program can also redirect its own output or input with the 
    // SetIn, SetOut, and SetError methods.
    // 
    // The distinction between Console.Out & Console.Error is useful
    // for programs that redirect output to a file or a pipe.  Note that
    // stdout & stderr can be output to different files at the same
    // time from the DOS command line:
    // 
    // someProgram 1> out 2> err
    // 
    //Contains only static data.  Serializable attribute not required.
    public static class Console
    {   
        private const int _DefaultConsoleBufferSize = 256;


        private static TextReader _in;
        private static TextWriter _out;
        private static TextWriter _error;

        private static ConsoleCancelEventHandler _cancelCallbacks;
        private static ControlCHooker _hooker;


        private static bool _wasOutRedirected;
        private static bool _wasErrorRedirected;

        // Private object for locking instead of locking on a public type for SQL reliability work.
        private static Object s_InternalSyncObject;
        private static Object InternalSyncObject {
            get {
                if (s_InternalSyncObject == null) {
                    Object o = new Object();
                    Interlocked.CompareExchange(ref s_InternalSyncObject, o, null);
                }
                return s_InternalSyncObject;
            }
        }

        // About reliability: I'm not using SafeHandle here.  We don't 
        // need to close these handles, and we don't allow the user to close
        // them so we don't have many of the security problems inherent in
        // something like file handles.  Additionally, in a host like SQL 
        // Server, we won't have a console.
        private static IntPtr _consoleInputHandle;
        private static IntPtr _consoleOutputHandle;

        private static IntPtr ConsoleInputHandle {
            [ResourceExposure(ResourceScope.Process)]
            [ResourceConsumption(ResourceScope.Process)]
            get {
                if (_consoleInputHandle == IntPtr.Zero) {
                    _consoleInputHandle = Win32Native.GetStdHandle(Win32Native.STD_INPUT_HANDLE);
                }
                return _consoleInputHandle;
            }
        }

        private static IntPtr ConsoleOutputHandle {
            [ResourceExposure(ResourceScope.Process)]
            [ResourceConsumption(ResourceScope.Process)]
            get {
                if (_consoleOutputHandle == IntPtr.Zero) {
                    _consoleOutputHandle = Win32Native.GetStdHandle(Win32Native.STD_OUTPUT_HANDLE);
                }
                return _consoleOutputHandle;
            }
        }

        public static TextWriter Error {
            [HostProtection(UI=true)]
            get { 
                // Hopefully this is inlineable.
                if (_error == null)
                    InitializeStdOutError(false);
                return _error;
            }
        }

        public static TextReader In {
            [HostProtection(UI=true)]
            get {
                // Because most applications don't use stdin, we can delay 
                // initialize it slightly better startup performance.
                if (_in == null) {
                    lock(InternalSyncObject) {
                        if (_in == null) {
                            // Set up Console.In
                            Stream s = OpenStandardInput(_DefaultConsoleBufferSize);
                            TextReader tr;
                            if (s == Stream.Null)
                                tr = StreamReader.Null;
                            else {
                                // Hopefully Encoding.GetEncoding doesn't load as many classes now.
                                Encoding enc = Encoding.GetEncoding((int)Win32Native.GetConsoleCP());
                                tr = TextReader.Synchronized(new StreamReader(s, enc, false, _DefaultConsoleBufferSize, false));                                    
                            }
                            System.Threading.Thread.MemoryBarrier();
                            _in = tr;
                        }
                    }
                }
                return _in;
            }
        }

        public static TextWriter Out {
            [HostProtection(UI=true)]
            get { 
                // Hopefully this is inlineable.
                if (_out == null)
                    InitializeStdOutError(true);
                return _out;
            }
        }

        // For console apps, the console handles are set to values like 3, 7, 
        // and 11 OR if you've been created via CreateProcess, possibly -1
        // or 0.  -1 is definitely invalid, while 0 is probably invalid.
        // Also note each handle can independently be invalid or good.
        // For Windows apps, the console handles are set to values like 3, 7, 
        // and 11 but are invalid handles - you may not write to them.  However,
        // you can still spawn a Windows app via CreateProcess and read stdout
        // and stderr.
        // So, we always need to check each handle independently for validity
        // by trying to write or read to it, unless it is -1.
        
        // We do not do a security check here, under the assumption that this
        // cannot create a security hole, but only waste a user's time or 
        // cause a possible denial of service attack.
        private static void InitializeStdOutError(bool stdout)
        {
            // Set up Console.Out or Console.Error.
            lock(InternalSyncObject) {
                if (stdout && _out != null)
                    return;
                else if (!stdout && _error != null)
                    return;

                TextWriter writer = null;
                Stream s;
                if (stdout)
                    s = OpenStandardOutput(_DefaultConsoleBufferSize);
                else
                    s = OpenStandardError(_DefaultConsoleBufferSize);

                if (s == Stream.Null) {
#if _DEBUG
                    if (CheckOutputDebug())
                        writer = MakeDebugOutputTextWriter((stdout) ? "Console.Out: " : "Console.Error: ");
                    else
#endif // _DEBUG
                        writer = TextWriter.Synchronized(StreamWriter.Null);
                }
                else {
                    int codePage = (int) Win32Native.GetConsoleOutputCP();
                    Encoding encoding = Encoding.GetEncoding(codePage);
                    StreamWriter stdxxx = new StreamWriter(s, encoding, _DefaultConsoleBufferSize, false);
                    stdxxx.HaveWrittenPreamble = true;
                    stdxxx.AutoFlush = true;
                    writer = TextWriter.Synchronized(stdxxx);
                }
                if (stdout)
                    _out = writer;
                else
                    _error = writer;
                BCLDebug.Assert((stdout && _out != null) || (!stdout && _error != null), "Didn't set Console::_out or _error appropriately!");
            }
        }        

        // This is ONLY used in debug builds.  If you have a registry key set,
        // it will redirect Console.Out & Error on console-less applications to
        // your debugger's output window.
#if _DEBUG
        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        private static bool CheckOutputDebug()
        {
            const int parameterValueLength = 255;
            StringBuilder parameterValue = new StringBuilder(parameterValueLength);
            bool rc = Win32Native.FetchConfigurationString(true, "ConsoleSpewToDebugger", parameterValue, parameterValueLength);
            if (rc) {
                if (0 != parameterValue.Length) {
                    int value = Convert.ToInt32(parameterValue.ToString());
                    if (0 != value)
                        return true;
                }
            }
            return false;
        }
#endif // _DEBUG


#if _DEBUG
        private static TextWriter MakeDebugOutputTextWriter(String streamLabel)
        {
            TextWriter output = new __DebugOutputTextWriter(streamLabel);
            output.WriteLine("Output redirected to debugger from a bit bucket.");
            return TextWriter.Synchronized(output);
        }
#endif // _DEBUG

        // This method is only exposed via methods to get at the console.
        // We won't use any security checks here.
        [ResourceExposure(ResourceScope.Process)]
        [ResourceConsumption(ResourceScope.Process)]
        private static Stream GetStandardFile(int stdHandleName, FileAccess access, int bufferSize) {
            // We shouldn't close the handle for stdout, etc, or we'll break
            // unmanaged code in the process that will print to console.
            // We should have a better way of marking this on SafeHandle.
            IntPtr handle = Win32Native.GetStdHandle(stdHandleName);
            SafeFileHandle sh = new SafeFileHandle(handle, false);

            // If someone launches a managed process via CreateProcess, stdout
            // stderr, & stdin could independently be set to INVALID_HANDLE_VALUE.
            // Additionally they might use 0 as an invalid handle.
            if (sh.IsInvalid) {
                // Minor perf optimization - get it out of the finalizer queue.
                sh.SetHandleAsInvalid();
                return Stream.Null;
            }

            // Check whether we can read or write to this handle.
            if (stdHandleName != Win32Native.STD_INPUT_HANDLE && !ConsoleHandleIsValid(sh)) {
                //BCLDebug.ConsoleError("Console::ConsoleHandleIsValid for std handle "+stdHandleName+" failed, setting it to a null stream");
                return Stream.Null;
            }

            //BCLDebug.ConsoleError("Console::GetStandardFile for std handle "+stdHandleName+" succeeded, returning handle number "+handle.ToString());
            Stream console = new __ConsoleStream(sh, access);
            // Do not buffer console streams, or we can get into situations where
            // we end up blocking waiting for you to hit enter twice.  It was
            // redundant.  
            return console;
        }

        public static Encoding InputEncoding {
            get {
                uint cp = Win32Native.GetConsoleCP();
                return Encoding.GetEncoding((int) cp);
            }
            set {
                if (value == null)
                    throw new ArgumentNullException("value");

                if (Environment.IsWin9X())
                    throw new PlatformNotSupportedException(Environment.GetResourceString("PlatformNotSupported_Win9x"));

                new UIPermission(UIPermissionWindow.SafeTopLevelWindows).Demand();

                uint cp = (uint) value.CodePage;
                bool r = Win32Native.SetConsoleCP(cp);
                if (!r)
                    __Error.WinIOError();

                // We need to reinitialize Console.In in the next call to _in
                // This will discard the current StreamReader, potentially 
                // losing buffered data
                _in = null;
            }
        }

        public static Encoding OutputEncoding {
            get {
                uint cp = Win32Native.GetConsoleOutputCP();
                return Encoding.GetEncoding((int) cp);
            }
            set {
                if (value == null)
                    throw new ArgumentNullException("value");

                if (Environment.IsWin9X())
                    throw new PlatformNotSupportedException(Environment.GetResourceString("PlatformNotSupported_Win9x"));

                new UIPermission(UIPermissionWindow.SafeTopLevelWindows).Demand();

                // Before changing the code page we need to flush the data 
                // if Out hasn't been redirected. Also, have the next call to  
                // _out reinitialize the console code page.

                if (_out != null && !_wasOutRedirected) {
                    _out.Flush();
                    _out = null;
                }
                if (_error != null && !_wasErrorRedirected) {
                    _error.Flush();
                    _error = null;
                }
                
                uint cp = (uint) value.CodePage;
                bool r = Win32Native.SetConsoleOutputCP(cp);
                if (!r)
                    __Error.WinIOError();
            }
        }


        // During an appdomain unload, we must call into the OS and remove
        // our delegate from the OS's list of console control handlers.  If
        // we don't do this, the OS will call back on a delegate that no
        // longer exists.  So, subclass CriticalFinalizableObject.
        // This problem would theoretically exist during process exit for a
        // single appdomain too, so using a critical finalizer is probably
        // better than the appdomain unload event (I'm not sure we call that
        // in the default appdomain during process exit).
        internal sealed class ControlCHooker : CriticalFinalizerObject
        {
            private bool _hooked;
            private Win32Native.ConsoleCtrlHandlerRoutine _handler;

            internal ControlCHooker()
            {
                _handler = new Win32Native.ConsoleCtrlHandlerRoutine(BreakEvent);
            }

            [ResourceExposure(ResourceScope.None)]
            [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
            ~ControlCHooker()
            {
                Unhook();
            }

            [ResourceExposure(ResourceScope.Process)]
            [ResourceConsumption(ResourceScope.Process)]
            internal void Hook()
            {
                if (!_hooked) {
                    bool r = Win32Native.SetConsoleCtrlHandler(_handler, true);
                    if (!r)
                        __Error.WinIOError();
                    _hooked = true;
                }
            }

            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            [ResourceExposure(ResourceScope.Process)]
            [ResourceConsumption(ResourceScope.Process)]
            internal void Unhook()
            {
                if (_hooked) {
                    bool r = Win32Native.SetConsoleCtrlHandler(_handler, false);
                    if (!r)
                        __Error.WinIOError();
                    _hooked = false;
                }
            }
        }
        
        // A class with data so ControlC handlers can be called on a Threadpool thread.
        private sealed class ControlCDelegateData {
            internal ConsoleSpecialKey ControlKey;
            internal bool Cancel;
            internal bool DelegateStarted;
            internal ManualResetEvent CompletionEvent;
            internal ConsoleCancelEventHandler CancelCallbacks;
            internal ControlCDelegateData(ConsoleSpecialKey controlKey, ConsoleCancelEventHandler cancelCallbacks) {
                this.ControlKey = controlKey;                
                this.CancelCallbacks = cancelCallbacks;
                this.CompletionEvent = new ManualResetEvent(false);
                // this.Cancel defaults to false
                // this.DelegateStarted defaults to false
            }
        }

        // Returns true if we've "handled" the break request, false if
        // we want to terminate the process (or at least let the next
        // control handler function have a chance).
        private static bool BreakEvent(int controlType) {
        
            // The thread that this gets called back on has a very small stack on 64 bit systems. There is
            // not enough space to handle a managed exception being caught and thrown. So, queue up a work
            // item on another thread for the actual event callback.
        
            if (controlType == Win32Native.CTRL_C_EVENT || 
                controlType == Win32Native.CTRL_BREAK_EVENT) {
                
                // To avoid race between remove handler and raising the event
                ConsoleCancelEventHandler cancelCallbacks = Console._cancelCallbacks;                
                if (cancelCallbacks == null) {
                    return false;
                }
                
                // Create the delegate
                ConsoleSpecialKey controlKey = (controlType == 0) ? ConsoleSpecialKey.ControlC : ConsoleSpecialKey.ControlBreak;                
                ControlCDelegateData delegateData = new ControlCDelegateData(controlKey, cancelCallbacks);
                WaitCallback controlCCallback = new WaitCallback(ControlCDelegate);
                
                // Queue the delegate
                if (!ThreadPool.QueueUserWorkItem(controlCCallback, delegateData)) {
                    BCLDebug.Assert(false, "ThreadPool.QueueUserWorkItem returned false without throwing. Unable to execute ControlC handler");
                    return false;
                }
                // Block until the delegate is done. We need to be robust in the face of the work item not executing
                // but we also want to get control back immediately after it is done and we don't want to give the
                // handler a fixed time limit in case it needs to display UI. Wait on the event twice, once with a
                // timout and a second time without if we are sure that the handler actually started.
                TimeSpan controlCWaitTime = new TimeSpan(0, 0, 30); // 30 seconds
                delegateData.CompletionEvent.WaitOne(controlCWaitTime, false);
                if (!delegateData.DelegateStarted) {
                    BCLDebug.Assert(false, "ThreadPool.QueueUserWorkItem did not execute the handler within 30 seconds.");
                    return false;
                }
                delegateData.CompletionEvent.WaitOne();
                delegateData.CompletionEvent.Close();
                return delegateData.Cancel;
                
            }
            return false;
        }
        
        // This is the worker delegate that is called on the Threadpool thread to fire the actual events. It must guarentee that it
        // signals the caller on the ControlC thread so that it does not block indefinitely.
        private static void ControlCDelegate(object data) {
            ControlCDelegateData controlCData = (ControlCDelegateData)data;
            try {        
                controlCData.DelegateStarted = true;
                ConsoleCancelEventArgs args = new ConsoleCancelEventArgs(controlCData.ControlKey);
                controlCData.CancelCallbacks(null, args);
                controlCData.Cancel = args.Cancel;
            }
            finally {
                controlCData.CompletionEvent.Set();
            }            
        }

        // Note: hooking this event allows you to prevent Control-C from 
        // killing a console app, which is somewhat surprising for users.
        // Some permission seems appropriate.  We chose UI permission for lack
        // of a better one.  However, we also applied host protection 
        // permission here as well, for self-affecting process management.
        // This allows hosts to prevent people from adding a handler for
        // this event.
        public static event ConsoleCancelEventHandler CancelKeyPress {
            [ResourceExposure(ResourceScope.Process)]
            [ResourceConsumption(ResourceScope.Process)]
            add {
                new UIPermission(UIPermissionWindow.SafeTopLevelWindows).Demand();

                lock(InternalSyncObject) {
                    // Add this delegate to the pile.
                    _cancelCallbacks += value;

                    // If we haven't registered our control-C handler, do it.
                    if (_hooker == null) {
                        _hooker = new ControlCHooker();
                        _hooker.Hook();
                    }
                }
            }
            [ResourceExposure(ResourceScope.Process)]
            [ResourceConsumption(ResourceScope.Process)]
            remove {
                new UIPermission(UIPermissionWindow.SafeTopLevelWindows).Demand();

                lock(InternalSyncObject) {
                    // If count was 0, call SetConsoleCtrlEvent to remove cb.
                    _cancelCallbacks -= value;
                    BCLDebug.Assert(_cancelCallbacks == null || _cancelCallbacks.GetInvocationList().Length > 0, "Teach Console::CancelKeyPress to handle a non-null but empty list of callbacks");
                    if (_hooker != null && _cancelCallbacks == null)
                        _hooker.Unhook();
                }
            }
        }

        [HostProtection(UI=true)]
        [ResourceExposure(ResourceScope.Process)]
        [ResourceConsumption(ResourceScope.Process)]
        public static Stream OpenStandardError() {
            return OpenStandardError(_DefaultConsoleBufferSize);
        }
    
        [HostProtection(UI=true)]
        [ResourceExposure(ResourceScope.Process)]
        [ResourceConsumption(ResourceScope.Process)]
        public static Stream OpenStandardError(int bufferSize) {
            if (bufferSize < 0)
                throw new ArgumentOutOfRangeException("bufferSize", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            return GetStandardFile(Win32Native.STD_ERROR_HANDLE,
                                   FileAccess.Write, bufferSize);
        }
    
        [HostProtection(UI=true)]
        [ResourceExposure(ResourceScope.Process)]
        [ResourceConsumption(ResourceScope.Process)]
        public static Stream OpenStandardInput() {
            return OpenStandardInput(_DefaultConsoleBufferSize);
        }
        
        [HostProtection(UI=true)]
        [ResourceExposure(ResourceScope.Process)]
        [ResourceConsumption(ResourceScope.Process)]
        public static Stream OpenStandardInput(int bufferSize) {
            if (bufferSize < 0)
                throw new ArgumentOutOfRangeException("bufferSize", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            return GetStandardFile(Win32Native.STD_INPUT_HANDLE,
                                   FileAccess.Read, bufferSize);
        }
        
        [HostProtection(UI=true)]
        [ResourceExposure(ResourceScope.Process)]
        [ResourceConsumption(ResourceScope.Process)]
        public static Stream OpenStandardOutput() {
            return OpenStandardOutput(_DefaultConsoleBufferSize);
        }
        
        [HostProtection(UI=true)]
        [ResourceExposure(ResourceScope.Process)]
        [ResourceConsumption(ResourceScope.Process)]
        public static Stream OpenStandardOutput(int bufferSize) {
            if (bufferSize < 0)
                throw new ArgumentOutOfRangeException("bufferSize", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            return GetStandardFile(Win32Native.STD_OUTPUT_HANDLE,
                                   FileAccess.Write, bufferSize);
        }
        
        [HostProtection(UI=true)]
        [ResourceExposure(ResourceScope.AppDomain)]
        public static void SetIn(TextReader newIn) {
            if (newIn == null)
                throw new ArgumentNullException("newIn");
            new SecurityPermission(SecurityPermissionFlag.UnmanagedCode).Demand();

            newIn = TextReader.Synchronized(newIn);
            _in = newIn;
        }
    
        [HostProtection(UI=true)]
        [ResourceExposure(ResourceScope.AppDomain)]
        public static void SetOut(TextWriter newOut) {
            if (newOut == null)
                throw new ArgumentNullException("newOut");
            new SecurityPermission(SecurityPermissionFlag.UnmanagedCode).Demand();
            
            _wasOutRedirected = true;
            newOut = TextWriter.Synchronized(newOut);
            _out = newOut;
        }
    
        [HostProtection(UI=true)]
        [ResourceExposure(ResourceScope.AppDomain)]
        public static void SetError(TextWriter newError) {
            if (newError == null)
                throw new ArgumentNullException("newError");
            new SecurityPermission(SecurityPermissionFlag.UnmanagedCode).Demand();

            _wasErrorRedirected = true;
            newError = TextWriter.Synchronized(newError);
            _error = newError;
        }
        
        [HostProtection(UI=true)]
        public static int Read()
        {
            return In.Read();
        }
        
        [HostProtection(UI=true)]
        public static String ReadLine()
        {
            return In.ReadLine();
        }
        
        [HostProtection(UI=true)]
        public static void WriteLine()
        {
            Out.WriteLine();
        }
        
        [HostProtection(UI=true)]
        public static void WriteLine(bool value)
        {
            Out.WriteLine(value);
        }
        
        [HostProtection(UI=true)]
        public static void WriteLine(char value)
        {
            Out.WriteLine(value);
        }   
        
        [HostProtection(UI=true)]
        public static void WriteLine(char[] buffer)
        {
            Out.WriteLine(buffer);
        }
                   
        [HostProtection(UI=true)]
        public static void WriteLine(char[] buffer, int index, int count)
        {
            Out.WriteLine(buffer, index, count);
        }
        
        [HostProtection(UI=true)]
        public static void WriteLine(decimal value)
        {
            Out.WriteLine(value);
        }   

        [HostProtection(UI=true)]
        public static void WriteLine(double value)
        {
            Out.WriteLine(value);
        }   
        
        [HostProtection(UI=true)]
        public static void WriteLine(float value)
        {
            Out.WriteLine(value);
        }   
           
        [HostProtection(UI=true)]
        public static void WriteLine(int value)
        {
            Out.WriteLine(value);
        }
        
        [HostProtection(UI=true)]
        [CLSCompliant(false)]
        public static void WriteLine(uint value)
        {
            Out.WriteLine(value);
        }
    
        [HostProtection(UI=true)]
        public static void WriteLine(long value)
        {
            Out.WriteLine(value);
        }
        
        [HostProtection(UI=true)]
        [CLSCompliant(false)]
        public static void WriteLine(ulong value)
        {
            Out.WriteLine(value);
        }
    
        [HostProtection(UI=true)]
        public static void WriteLine(Object value)
        {
            Out.WriteLine(value);
        }
        
        [HostProtection(UI=true)]
        public static void WriteLine(String value)
        {
            Out.WriteLine(value);
        }
    
    
        [HostProtection(UI=true)]
        public static void WriteLine(String format, Object arg0)
        {
            Out.WriteLine(format, arg0);
        }
    
        [HostProtection(UI=true)]
        public static void WriteLine(String format, Object arg0, Object arg1)
        {
            Out.WriteLine(format, arg0, arg1);
        }
        
        [HostProtection(UI=true)]
        public static void WriteLine(String format, Object arg0, Object arg1, Object arg2)
        {
            Out.WriteLine(format, arg0, arg1, arg2);
        }
        
        [HostProtection(UI=true)]
        [CLSCompliant(false)] 
        public static void WriteLine(String format, Object arg0, Object arg1, Object arg2,Object arg3, __arglist) 
        {
            Object[]   objArgs;
            int        argCount;
                
            ArgIterator args = new ArgIterator(__arglist);

            //+4 to account for the 4 hard-coded arguments at the beginning of the list.
            argCount = args.GetRemainingCount() + 4;
    
            objArgs = new Object[argCount];
            
            //Handle the hard-coded arguments
            objArgs[0] = arg0;
            objArgs[1] = arg1;
            objArgs[2] = arg2;
            objArgs[3] = arg3;
            
            //Walk all of the args in the variable part of the argument list.
            for (int i=4; i<argCount; i++) {
                objArgs[i] = TypedReference.ToObject(args.GetNextArg());
            }

            Out.WriteLine(format, objArgs);
        }


        [HostProtection(UI=true)]
        public static void WriteLine(String format, params Object[] arg)
        {
            Out.WriteLine(format, arg);
        }
        
        [HostProtection(UI=true)]
        public static void Write(String format, Object arg0)
        {
            Out.Write(format, arg0);
        }
        
        [HostProtection(UI=true)]
        public static void Write(String format, Object arg0, Object arg1)
        {
            Out.Write(format, arg0, arg1);
        }
        
        [HostProtection(UI=true)]
        public static void Write(String format, Object arg0, Object arg1, Object arg2)
        {
            Out.Write(format, arg0, arg1, arg2);
        }

        [HostProtection(UI=true)]
        [CLSCompliant(false)] 
        public static void Write(String format, Object arg0, Object arg1, Object arg2, Object arg3, __arglist) 
        {
            Object[]   objArgs;
            int        argCount;
                
            ArgIterator args = new ArgIterator(__arglist);

            //+4 to account for the 4 hard-coded arguments at the beginning of the list.
            argCount = args.GetRemainingCount() + 4;
    
            objArgs = new Object[argCount];
            
            //Handle the hard-coded arguments
            objArgs[0] = arg0;
            objArgs[1] = arg1;
            objArgs[2] = arg2;
            objArgs[3] = arg3;
            
            //Walk all of the args in the variable part of the argument list.
            for (int i=4; i<argCount; i++) {
                objArgs[i] = TypedReference.ToObject(args.GetNextArg());
            }

            Out.Write(format, objArgs);
        }

            
        [HostProtection(UI=true)]
        public static void Write(String format, params Object[] arg)
        {
            Out.Write(format, arg);
        }
        
        [HostProtection(UI=true)]
        public static void Write(bool value)
        {
            Out.Write(value);
        }
    
        [HostProtection(UI=true)]
        public static void Write(char value)
        {
            Out.Write(value);
        }   
        
        [HostProtection(UI=true)]
        public static void Write(char[] buffer)
        {
            Out.Write(buffer);
        }
        
        [HostProtection(UI=true)]
        public static void Write(char[] buffer, int index, int count)
        {
            Out.Write(buffer, index, count);
        }
        
        [HostProtection(UI=true)]
        public static void Write(double value)
        {
            Out.Write (value);
        }   
        
        [HostProtection(UI=true)]
        public static void Write(decimal value)
        {
            Out.Write (value);
        }   
        
        [HostProtection(UI=true)]
        public static void Write(float value)
        {
            Out.Write (value);
        }   
        
        [HostProtection(UI=true)]
        public static void Write(int value)
        {
            Out.Write (value);
        }
        
        [HostProtection(UI=true)]
        [CLSCompliant(false)]
        public static void Write(uint value)
        {
            Out.Write (value);
        }
    
        [HostProtection(UI=true)]
        public static void Write(long value)
        {
            Out.Write (value);
        }
        
        [HostProtection(UI=true)]
        [CLSCompliant(false)]
        public static void Write(ulong value)
        {
            Out.Write (value);
        }
    
        [HostProtection(UI=true)]
        public static void Write(Object value)
        {
            Out.Write (value);
        }
        
        [HostProtection(UI=true)]
        public static void Write(String value)
        {
            Out.Write (value);
        }
    
        // Checks whether stdout or stderr are writable.  Do NOT pass
        // stdin here.
        [ResourceExposure(ResourceScope.Process)]
        [ResourceConsumption(ResourceScope.Process)]
        private static unsafe bool ConsoleHandleIsValid(SafeFileHandle handle)
        {
            // Do NOT call this method on stdin!

            // Windows apps may have non-null valid looking handle values for 
            // stdin, stdout and stderr, but they may not be readable or 
            // writable.  Verify this by calling ReadFile & WriteFile in the 
            // appropriate modes.
            // This must handle console-less Windows apps.
            if (handle.IsInvalid)
                return false;  // WinCE would return here, if we ran on CE.
            
            int bytesWritten;
            byte junkByte = 0x41;
            int r;
            r = __ConsoleStream.WriteFile(handle, &junkByte, 0, out bytesWritten, IntPtr.Zero);
            // In Win32 apps w/ no console, bResult should be 0 for failure.
            return r != 0;
        }
    }
}
