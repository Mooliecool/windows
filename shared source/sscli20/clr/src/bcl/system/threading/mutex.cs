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
** Class: Mutex	
**
**
** Purpose: synchronization primitive that can also be used for interprocess synchronization
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
    using System.Runtime.ConstrainedExecution;
    using System.Runtime.Versioning;
    using System.Security.Principal;
    using System.Security;
    

    [HostProtection(Synchronization=true, ExternalThreading=true)]
    [ComVisible(true)]
    public sealed class Mutex : WaitHandle
    {
        static bool dummyBool;

       
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public Mutex(bool initiallyOwned, String name, out bool createdNew)
        {
            if(null != name && System.IO.Path.MAX_PATH < name.Length)
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_WaitHandleNameTooLong",name));
            }            
            Win32Native.SECURITY_ATTRIBUTES secAttrs = null;
            SafeWaitHandle mutexHandle = null;
            bool newMutex = false;
            RuntimeHelpers.CleanupCode cleanupCode = new RuntimeHelpers.CleanupCode(MutexCleanupCode);
            MutexCleanupInfo cleanupInfo = new MutexCleanupInfo(mutexHandle, false);            
            RuntimeHelpers.ExecuteCodeWithGuaranteedCleanup(
                delegate(object userData)  {  // try block                
                    RuntimeHelpers.PrepareConstrainedRegions();
                    try {
                    }
                    finally {
                        if (initiallyOwned) {
                            cleanupInfo.inCriticalRegion = true;
                            Thread.BeginThreadAffinity();
                            Thread.BeginCriticalRegion();
                        }
                    }

                    int errorCode = 0;                    
                    RuntimeHelpers.PrepareConstrainedRegions();
                    try {
                    }
                    finally {
                        errorCode = CreateMutexHandle(initiallyOwned, name, secAttrs, out mutexHandle);
                    }                    

                    if (mutexHandle.IsInvalid) {
                        mutexHandle.SetHandleAsInvalid();
                        if(null != name && 0 != name.Length && Win32Native.ERROR_INVALID_HANDLE == errorCode)
                            throw new WaitHandleCannotBeOpenedException(Environment.GetResourceString("Threading.WaitHandleCannotBeOpenedException_InvalidHandle", name));
                        __Error.WinIOError(errorCode, name);
                    }
                    newMutex = errorCode != Win32Native.ERROR_ALREADY_EXISTS;
                    SetHandleInternal(mutexHandle);
                    mutexHandle.SetAsMutex();

                    hasThreadAffinity = true;

                },
                cleanupCode,
                cleanupInfo);             
                createdNew = newMutex;

}

        [PrePrepareMethod]
        private void MutexCleanupCode(Object userData, bool exceptionThrown)
        {
            MutexCleanupInfo cleanupInfo = (MutexCleanupInfo) userData;
            
            // If hasThreadAffinity isn’t true, we’ve thrown an exception in the above try, and we must free the mutex 
            // on this OS thread before ending our thread affninity.                
            if(!hasThreadAffinity) {
                if (cleanupInfo.mutexHandle != null && !cleanupInfo.mutexHandle.IsInvalid) {
                    if( cleanupInfo.inCriticalRegion) {
                        Win32Native.ReleaseMutex(cleanupInfo.mutexHandle);                    
                    }
                    cleanupInfo.mutexHandle.Dispose();                        
                    
                }
                    
                if( cleanupInfo.inCriticalRegion) {
                    Thread.EndCriticalRegion();
                    Thread.EndThreadAffinity();
                }                    
            }
        }

        internal class MutexCleanupInfo
        {
            internal SafeWaitHandle mutexHandle;
            internal bool inCriticalRegion;
            internal MutexCleanupInfo(SafeWaitHandle mutexHandle, bool inCriticalRegion)
            {
                this.mutexHandle = mutexHandle;
                this.inCriticalRegion = inCriticalRegion;
            }
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public Mutex(bool initiallyOwned, String name) : this(initiallyOwned, name, out dummyBool) {
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public Mutex(bool initiallyOwned) : this(initiallyOwned, null, out dummyBool)
        {
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public Mutex() : this(false, null, out dummyBool)
        {
        }
		
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        private Mutex(SafeWaitHandle handle)
        {
            SetHandleInternal(handle);
            handle.SetAsMutex();
            hasThreadAffinity = true;
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand,Flags=SecurityPermissionFlag.UnmanagedCode)]
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static Mutex OpenExisting(string name)
        {
            if (name == null)
            {
                throw new ArgumentNullException("name", Environment.GetResourceString("ArgumentNull_WithParamName"));
            }

            if(name.Length  == 0)
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "name");
            }
            if(System.IO.Path.MAX_PATH < name.Length)
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_WaitHandleNameTooLong",name));
            }
            

            // To allow users to view & edit the ACL's, call OpenMutex
            // with parameters to allow us to view & edit the ACL.  This will
            // fail if we don't have permission to view or edit the ACL's.  
            // If that happens, ask for less permissions.
            SafeWaitHandle myHandle = Win32Native.OpenMutex(Win32Native.MUTEX_MODIFY_STATE | Win32Native.SYNCHRONIZE, false, name);

            int errorCode = 0;
            if (myHandle.IsInvalid)
            {
                errorCode = Marshal.GetLastWin32Error();

                if(Win32Native.ERROR_FILE_NOT_FOUND == errorCode || Win32Native.ERROR_INVALID_NAME == errorCode)
                {
                    throw new WaitHandleCannotBeOpenedException();
                }
                
                if(null != name && 0 != name.Length && Win32Native.ERROR_INVALID_HANDLE == errorCode) 
                {
                    throw new WaitHandleCannotBeOpenedException(Environment.GetResourceString("Threading.WaitHandleCannotBeOpenedException_InvalidHandle", name));
                }

                // this is for passed through Win32Native Errors
                __Error.WinIOError(errorCode,name);
            }

            return new Mutex(myHandle);
        }

        // Note: To call ReleaseMutex, you must have an ACL granting you
        // MUTEX_MODIFY_STATE rights (0x0001).  The other interesting value
        // in a Mutex's ACL is MUTEX_ALL_ACCESS (0x1F0001).
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]        
        public void ReleaseMutex()
        {
            if (Win32Native.ReleaseMutex(safeWaitHandle))
            {
                Thread.EndCriticalRegion();
                Thread.EndThreadAffinity();
            }
            else
            {
                throw new ApplicationException(Environment.GetResourceString("Arg_SynchronizationLockException"));
            }                                                               
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        static int CreateMutexHandle(bool initiallyOwned, String name, Win32Native.SECURITY_ATTRIBUTES securityAttribute, out SafeWaitHandle mutexHandle) {            
            int errorCode;  
            bool fReservedMutexObtained = false;
            bool fAffinity = false;
			
            while(true) {
                mutexHandle = Win32Native.CreateMutex(securityAttribute, initiallyOwned, name);
                errorCode = Marshal.GetLastWin32Error();                                
                if( !mutexHandle.IsInvalid) {
                    break;                
                }

                if( errorCode == Win32Native.ERROR_ACCESS_DENIED) {
                    // If a mutex with the name already exists, OS will try to open it with FullAccess.
                    // It might fail if we don't have enough access. In that case, we try to open the mutex will modify and synchronize access.
                    //
                    
                    RuntimeHelpers.PrepareConstrainedRegions();
                    try 
                    {
                        try 
                        {
                        } 
                        finally 
                        {
                            Thread.BeginThreadAffinity();
                            fAffinity = true;
                        }
                        AcquireReservedMutex(ref fReservedMutexObtained);		
                        mutexHandle = Win32Native.OpenMutex(Win32Native.MUTEX_MODIFY_STATE | Win32Native.SYNCHRONIZE, false, name);
                        if(!mutexHandle.IsInvalid)
                        {
                            errorCode = Win32Native.ERROR_ALREADY_EXISTS;
                        }
                        else
                        {
                            errorCode = Marshal.GetLastWin32Error();
                        }
                    }
                    finally 
                    {
                        if (fReservedMutexObtained)
                            ReleaseReservedMutex();
                        if (fAffinity)						
                            Thread.EndThreadAffinity();						                        
                    }

                    // There could be a race here, the other owner of the mutex can free the mutex,
                    // We need to retry creation in that case.
                    if( errorCode != Win32Native.ERROR_FILE_NOT_FOUND) {
                        if( errorCode == Win32Native.ERROR_SUCCESS) {
                            errorCode =  Win32Native.ERROR_ALREADY_EXISTS;
                        }                        
                        break;
                    }
                }
                else {
                    break;
                }
            }                        
            return errorCode;
        }
        


        // Enables workaround for known OS bug at 
        // http://support.microsoft.com/default.aspx?scid=kb;en-us;889318		
        // One machine-wide mutex serializes all OpenMutex and CloseHandle operations.
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        internal static unsafe void AcquireReservedMutex(ref bool bHandleObtained)
        {
		bHandleObtained = true;
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        internal static void ReleaseReservedMutex()
        {
        }

    }
}
