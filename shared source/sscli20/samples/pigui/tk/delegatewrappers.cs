//------------------------------------------------------------------------------
// <copyright file="DelegateWrappers.cs" company="Microsoft">
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

using System;
using System.Runtime.InteropServices;

namespace SharedSourceCLI.TK
{
    internal class TclAppInitProcWrapper {
    
        TclInterp _interp;
        TclAppInitProc _proc;
        Tcl_AppInitProc _callback;
        
        internal TclAppInitProcWrapper(TclInterp interp, TclAppInitProc proc) {
            _interp = interp;
            _proc = proc;
            unsafe { _callback = new Tcl_AppInitProc(this.CallbackProc); }
        }

        internal Tcl_AppInitProc Callback {
            get { return _callback; }
        }

        private unsafe int CallbackProc(Tcl_Interp* interp)
        {
            try {
                _proc(_interp);
            }
            catch (Exception) {
                return TclNative.TCL_ERROR;
            }
            
            return TclNative.TCL_OK;
        }
    }

    internal class TclCmdProcWrapper {
    
        TclInterp _interp;
        TclCmdProc  _proc;
        Tcl_CmdProc _callback;

        internal TclCmdProcWrapper(TclInterp interp, TclCmdProc proc) {
            _interp = interp;
            _proc = proc;
            unsafe { _callback = new Tcl_CmdProc(this.CallbackProc); }
        }

        internal Tcl_CmdProc Callback {
            get { return _callback; }
        }

        private unsafe int CallbackProc(IntPtr clientData, 
            Tcl_Interp* interp, int argc, char** argv)
        {
            try {
                string[] arr = new string[argc];
                for(int i=0; i<argc; i++)
                {
                    arr[i] = Marshal.PtrToStringAnsi((IntPtr)argv[i]);
                }
            
                _proc(_interp, arr);
            }
            catch (Exception) {
                return TclNative.TCL_ERROR;
            }
            
            return TclNative.TCL_OK;
        }    
    }

    internal class TclCmdDeleteProcWrapper {
    
        TclInterp _interp;
        TclCmdDeleteProc _proc;
        Tcl_CmdDeleteProc _callback;
        Object _deletee;

        internal TclCmdDeleteProcWrapper(TclInterp interp, TclCmdDeleteProc proc, Object deletee) {
            _interp = interp;
            _proc = proc;
            unsafe { _callback = new Tcl_CmdDeleteProc(this.CallbackProc); }
            _deletee = deletee;
            
            _interp.AddKeepAlive(this);
            _interp.AddKeepAlive(_deletee);
        }

        internal Tcl_CmdDeleteProc Callback {
            get { return _callback; }
        }

        private unsafe int CallbackProc(IntPtr clientData)
        {
            try {                
                _interp.RemoveKeepAlive(_deletee);
                _interp.RemoveKeepAlive(this);

                if (_proc != null)
                    _proc();
            }
            catch (Exception) {
                return TclNative.TCL_ERROR;
            }
            
            return TclNative.TCL_OK;
        }    
    }
}
