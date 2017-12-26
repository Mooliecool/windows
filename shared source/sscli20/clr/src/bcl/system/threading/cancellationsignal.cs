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
/*============================================================
**
** Class:  CancellationSignal
**
**
** Purpose: Defines an identifier to ensure you can only 
** cancel synchronous IO requests that you were expecting to 
** cancel.
**
** 
===========================================================*/

using System.IO;
using System.Security.Permissions;
using System.Collections.Generic;
using System.Runtime.ConstrainedExecution;
using Microsoft.Win32;

namespace System.Threading {


    public sealed class CancellationSignal
    {
        // Warning - Thread contains a stack of CancellationSignals, which
        // places a restriction on this class.  All types used within a 
        // CancellationSignal must be AppDomain-agile.  Otherwise, we will 
        // bleed objects across appdomains, just as we do with CultureInfo 
        // today.  Be very careful about adding new members here.  We've
        // sealed the class to prevent users from accidentally causing this
        // problem for themselves in subclasses of CancellationSignal.
        private Thread _thread;
        private bool _cancelable;
        private bool _wasCancelRequested;

        private static bool SupportsCancelSyncIOChecked;
        private static bool OSSupportsCancelSyncIO;

        [ReliabilityContract(Consistency.MayCorruptInstance, Cer.MayFail)]
        public CancellationSignal()
        {
            _cancelable = true;
        }

        [ReliabilityContract(Consistency.MayCorruptInstance, Cer.MayFail)]
        internal CancellationSignal(bool cancelable)
        {
            _cancelable = cancelable;
        }

        [HostProtection(ExternalThreading=true)]
        [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
        public void Cancel()
        {
            _wasCancelRequested = true;
            throw new PlatformNotSupportedException();
        }

        private static void CheckSupportsCancelSyncIO()
        {
            OSSupportsCancelSyncIO = false;
            SupportsCancelSyncIOChecked = true;
        }

        // To allow CPU-intensive tasks to use our cancellation model to 
        // opt into cancellation via polling.  We could also use this in 
        // select places to help avoid races inherent in the OS's 
        // cancellation model, such as attempting to cancel CopyFile while
        // it is in the middle of some CPU-intensive calculation instead of
        // blocked waiting for IO.  
        // Note that if you cancel one region, it should cancel any nested
        // cancellable regions.  See Cancel for this logic.
        public bool CancelRequested {
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            get { return _wasCancelRequested; }
        }

        internal Thread Thread {
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            get { return _thread; }
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
            set {
                if (_thread != null && value != null)
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CancellationSignalReuse"));
                _thread = value;
            }
        }
    }
}
