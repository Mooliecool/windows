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
namespace System {
    
    using System;
    using System.Runtime.ConstrainedExecution;
    
    [Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public class UnhandledExceptionEventArgs : EventArgs {
        private Object _Exception;
        private bool _IsTerminating;

        public UnhandledExceptionEventArgs(Object exception, bool isTerminating) {
            _Exception = exception;
            _IsTerminating = isTerminating;
        }
        public Object ExceptionObject { 
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            get { return _Exception; }
        }
        public bool IsTerminating {
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            get { return _IsTerminating; }
        }
    }
}
