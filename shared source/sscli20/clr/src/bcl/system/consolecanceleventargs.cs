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
** Class: ConsoleCancelEventArgs
**
**
** Purpose: This class provides support goop for hooking Control-C and 
**          Control-Break, then preventing Control-C from interrupting the 
**          process.
**
**
=============================================================================*/
namespace System {
    using System;

    public delegate void ConsoleCancelEventHandler(Object sender, ConsoleCancelEventArgs e);


    [Serializable]
    public sealed class ConsoleCancelEventArgs : EventArgs
    {
        private ConsoleSpecialKey _type;
        private bool _cancel;  // Whether to cancel the CancelKeyPress event

        internal ConsoleCancelEventArgs(ConsoleSpecialKey type)
        {
            _type = type;
            _cancel = false;
        }

        // Whether to cancel the break event.  By setting this to true, the
        // Control-C will not kill the process.
        public bool Cancel {
            get { return _cancel; }
            set {
                if (_type == ConsoleSpecialKey.ControlBreak && value == true)
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CantCancelCtrlBreak"));
                _cancel = value;
            }
        }

        public ConsoleSpecialKey SpecialKey {
            get { return _type; }
        }
    }
}
