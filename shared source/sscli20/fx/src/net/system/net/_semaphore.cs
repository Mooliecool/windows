//------------------------------------------------------------------------------
// <copyright file="Semaphore.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

#pragma warning disable 618

namespace System.Net 
{  


	using System;
	using System.Threading;
    using System.Security.Permissions;


    // used for Connection Pooling
    internal sealed class Semaphore : WaitHandle
    {
        internal Semaphore(int initialCount, int maxCount) : base() {
            lock (this) {
                Handle = UnsafeNclNativeMethods.CreateSemaphore(IntPtr.Zero, initialCount, maxCount, IntPtr.Zero);
            }
        }

        /*
        //                                    
        public Semaphore(int initialCount, int maxCount, string name) : base() {
            lock (this) {
                //                                                                          
                Handle = UnsafeNclNativeMethods.CreateSemaphore(IntPtr.Zero, initialCount, maxCount, name);
            }
        }
        */

        internal bool ReleaseSemaphore() {
#if DEBUG        
            int previousCount;
            bool success = UnsafeNclNativeMethods.ReleaseSemaphore(Handle, 1, out previousCount);        
            GlobalLog.Print("ReleaseSemaphore#"+ValidationHelper.HashString(this)+" success:"+success+" previousCount:"+previousCount.ToString());
            return success;
#else            
            return UnsafeNclNativeMethods.ReleaseSemaphore(Handle, 1, IntPtr.Zero);
#endif            
        }

        /*
        //                                    
        internal bool ReleaseSemaphore(int releaseCount, out int previousCount) {
            return UnsafeNclNativeMethods.ReleaseSemaphore(Handle, releaseCount, out previousCount);        
        }
        */
    }
}
