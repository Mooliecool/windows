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
** Class: EventWaitHandle	
**
**
** Purpose: Base class for representing Events
**
**
=============================================================================*/

namespace System.Threading
{
    using System;
    using System.Threading;
    using System.Runtime.CompilerServices;
    using System.Security.Permissions;
    using System.IO;
    using Microsoft.Win32;
    using Microsoft.Win32.SafeHandles;
    using System.Runtime.InteropServices;
    using System.Runtime.Versioning;

    [HostProtection(Synchronization=true, ExternalThreading=true)]
    [ComVisibleAttribute(true)]
    public class EventWaitHandle : WaitHandle
    {
        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public EventWaitHandle(bool initialState, EventResetMode mode) : this(initialState,mode,null) { }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand,Flags=SecurityPermissionFlag.UnmanagedCode)]
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public EventWaitHandle(bool initialState, EventResetMode mode, string name)
        {
            if(null != name && System.IO.Path.MAX_PATH < name.Length)
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_WaitHandleNameTooLong",name));
            }
            
            SafeWaitHandle _handle = null;
            switch(mode)
            {
                case EventResetMode.ManualReset:
                    _handle = Win32Native.CreateEvent(null, true, initialState, name);
                    break;
                case EventResetMode.AutoReset:
                    _handle = Win32Native.CreateEvent(null, false, initialState, name);
                    break;

                default:
                    throw new ArgumentException(Environment.GetResourceString("Argument_InvalidFlag",name));
            };
                
            if (_handle.IsInvalid)
            {
                int errorCode = Marshal.GetLastWin32Error();
            
                _handle.SetHandleAsInvalid();
                if(null != name && 0 != name.Length && Win32Native.ERROR_INVALID_HANDLE == errorCode)
                    throw new WaitHandleCannotBeOpenedException(Environment.GetResourceString("Threading.WaitHandleCannotBeOpenedException_InvalidHandle",name));

                __Error.WinIOError(errorCode, "");
            }	
            SetHandleInternal(_handle);
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand,Flags=SecurityPermissionFlag.UnmanagedCode)]
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public EventWaitHandle(bool initialState, EventResetMode mode, string name, out bool createdNew)
        {
            if(null != name && System.IO.Path.MAX_PATH < name.Length)
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_WaitHandleNameTooLong",name));
            }
            Win32Native.SECURITY_ATTRIBUTES secAttrs = null;

            SafeWaitHandle _handle = null;
            Boolean isManualReset;
            switch(mode)
            {
                case EventResetMode.ManualReset:
                    isManualReset = true;
                    break;
                case EventResetMode.AutoReset:
                    isManualReset = false;
                    break;

                default:
                    throw new ArgumentException(Environment.GetResourceString("Argument_InvalidFlag",name));
            };

            _handle = Win32Native.CreateEvent(secAttrs, isManualReset, initialState, name);
            int errorCode = Marshal.GetLastWin32Error();

            if (_handle.IsInvalid)
            {

                _handle.SetHandleAsInvalid();
                if(null != name && 0 != name.Length && Win32Native.ERROR_INVALID_HANDLE == errorCode)
                    throw new WaitHandleCannotBeOpenedException(Environment.GetResourceString("Threading.WaitHandleCannotBeOpenedException_InvalidHandle",name));

                __Error.WinIOError(errorCode, name);
            }
            createdNew = errorCode != Win32Native.ERROR_ALREADY_EXISTS;
            SetHandleInternal(_handle);
        }

        private EventWaitHandle(SafeWaitHandle handle)
        {
            SetHandleInternal(handle);
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand,Flags=SecurityPermissionFlag.UnmanagedCode)]
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static EventWaitHandle OpenExisting(string name)
        {
            if (name == null)
            {
                throw new ArgumentNullException("name", Environment.GetResourceString("ArgumentNull_WithParamName"));
            }

            if(name.Length  == 0)
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "name");
            }

            if(null != name && System.IO.Path.MAX_PATH < name.Length)
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_WaitHandleNameTooLong",name));
            }
            

            SafeWaitHandle myHandle = Win32Native.OpenEvent(Win32Native.EVENT_MODIFY_STATE | Win32Native.SYNCHRONIZE, false, name);
            
            if (myHandle.IsInvalid)
            {
                int errorCode = Marshal.GetLastWin32Error();

                if(Win32Native.ERROR_FILE_NOT_FOUND == errorCode || Win32Native.ERROR_INVALID_NAME == errorCode)
                    throw new WaitHandleCannotBeOpenedException();
                if(null != name && 0 != name.Length && Win32Native.ERROR_INVALID_HANDLE == errorCode)
                    throw new WaitHandleCannotBeOpenedException(Environment.GetResourceString("Threading.WaitHandleCannotBeOpenedException_InvalidHandle",name));
                //this is for passed through Win32Native Errors
                __Error.WinIOError(errorCode,"");
            }
            return new EventWaitHandle(myHandle);
        }
        public bool Reset()
        {
            bool res = Win32Native.ResetEvent(safeWaitHandle);
            if (!res)
                __Error.WinIOError();
            return res;
        }
        public bool Set()
        {
            bool res = Win32Native.SetEvent(safeWaitHandle);

            if (!res)
                __Error.WinIOError();

            return res;
        }

    }
}

