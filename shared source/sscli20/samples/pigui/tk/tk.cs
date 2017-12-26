//------------------------------------------------------------------------------
// <copyright file="Tk.cs" company="Microsoft">
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
using System.Collections;
using System.Runtime.InteropServices;

namespace SharedSourceCLI.TK
{
    public class TclException : Exception {
        
        public TclException() {
        }
    }
    
    public struct TclCmdToken {
        internal IntPtr _token;
    }
    
    public delegate void TclAppInitProc(TclInterp interp);
    public delegate void TclCmdProc(TclInterp interp, string[] argv);
    public delegate void TclCmdDeleteProc();
    
    public class TclInterp {

        // innner unmanaged object
        unsafe Tcl_Interp* _interp;

        // hashtable to keep marshalled delegates alive
        private Hashtable _keepAlive;

        public TclInterp() {
            unsafe {
                _interp = TclNative.Tcl_CreateInterp();
                if (_interp == null)
                    throw new OutOfMemoryException();
            }
            _keepAlive = new Hashtable();
        }

        internal void AddKeepAlive(Object o) {
            _keepAlive.Add(o, null);
        }

        internal void RemoveKeepAlive(Object o) {
            _keepAlive.Remove(o);
        }
        
        internal void HandleError(int res) {
            if (res != TclNative.TCL_OK)
                throw new TclException();
        }
        
        public void TclInit() {
            unsafe {
                HandleError(TclNative.Tcl_Init(_interp));
            }
        }
        
        public void TkInit() {
            unsafe {
                HandleError(TkNative.Tk_Init(_interp));
            }
        }
        
        public void TkMain(TclAppInitProc initProc, string[] argv) {
            TclAppInitProcWrapper initWrapper = new TclAppInitProcWrapper(this, initProc);
            
            unsafe {               
                TkNative.Tk_MainEx(argv.Length, argv, initWrapper.Callback, _interp);
            }
            
            GC.KeepAlive(initWrapper);
            GC.KeepAlive(this);
        }
                
        public void TkMain(TclAppInitProc initProc) {
            TkMain(initProc, Environment.GetCommandLineArgs());
        }

        public void Eval(string str) {
            unsafe {
                HandleError(TclNative.Tcl_Eval(_interp, str));
            }
        }

        public string GetVar(string varName, int flags) {
            unsafe {
                return Marshal.PtrToStringAnsi(TclNative.Tcl_GetVar(_interp, varName, flags));
            }
        }
        
        public string GetVar(string varName) {
            return GetVar(varName, 0);
        }
        
        public TclCmdToken CreateCommand(string cmdName, TclCmdProc proc, TclCmdDeleteProc deleteProc) {
            TclCmdToken token;
            
            TclCmdProcWrapper cmdWrapper = new TclCmdProcWrapper(this, proc);
            TclCmdDeleteProcWrapper deleteWrapper = new TclCmdDeleteProcWrapper(this, deleteProc, cmdWrapper);
        
            unsafe {
                token._token = TclNative.Tcl_CreateCommand(_interp, cmdName, cmdWrapper.Callback, 
                    IntPtr.Zero, deleteWrapper.Callback);
            }
            
            return token;
        }

        public TclCmdToken CreateCommand(string cmdName, TclCmdProc proc) {
            return CreateCommand(cmdName, proc, null);
        }
    }
}
