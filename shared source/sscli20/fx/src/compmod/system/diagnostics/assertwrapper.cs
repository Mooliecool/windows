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
using System.Diagnostics;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.Security.Permissions;
using System.Security;
using System.Collections;
using System.Globalization;
using System.Runtime.Versioning;

namespace System.Diagnostics {

    internal static class AssertWrapper {

        
        public static void ShowAssert(string stackTrace, StackFrame frame, string message, string detailMessage) {
            ShowMessageBoxAssert(stackTrace, message, detailMessage);                                  
        }



        [ResourceExposure(ResourceScope.None)]
        private static void ShowMessageBoxAssert(string stackTrace, string message, string detailMessage) {
            string fullMessage = message + "\r\n" + detailMessage + "\r\n" + stackTrace;


            int flags = 0x00000002 /*AbortRetryIgnore*/ | 0x00000200 /*DefaultButton3*/ | 0x00000010 /*IconHand*/ |
                        0x00040000 /* TopMost */;


            if (IsRTLResources)
                flags = flags | SafeNativeMethods.MB_RIGHT | SafeNativeMethods.MB_RTLREADING;
                
            int rval = SafeNativeMethods.MessageBox(NativeMethods.NullHandleRef, fullMessage, SR.GetString(SR.DebugAssertTitle), flags);
            switch (rval) {
                case 3: // abort
                    new SecurityPermission(SecurityPermissionFlag.UnmanagedCode).Assert();
                    try {
                        Environment.Exit(1);
                    }
                    finally {
                        CodeAccessPermission.RevertAssert();    
                    }
                    break;
                case 4: // retry
                    if (!System.Diagnostics.Debugger.IsAttached) {
                        System.Diagnostics.Debugger.Launch();
                    }
                    System.Diagnostics.Debugger.Break();
                    break;
            }
        }


        private static bool IsRTLResources {
            get {
                return SR.GetString(SR.RTL) != "RTL_False";
            }
        }

    }
}
