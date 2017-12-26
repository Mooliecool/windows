//------------------------------------------------------------------------------
// <copyright file="TclNative.cs" company="Microsoft">
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
    [StructLayout( LayoutKind.Sequential )]
    internal struct Tcl_Interp {
        internal IntPtr result;        
        internal IntPtr freeProc;
        internal int errorLine;  
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal unsafe delegate int Tcl_AppInitProc(Tcl_Interp* interp);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal unsafe delegate int Tcl_CmdProc(IntPtr clientData,
        Tcl_Interp* interp, int argc, char** argv);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal unsafe delegate int Tcl_CmdDeleteProc(IntPtr clientData);

    internal unsafe class TclNative {

#if !PLATFORM_UNIX
        internal const String DLLPREFIX = "";
        internal const String DLLSUFFIX = ".dll";
        internal const String DLLNAME = "tcl84";
#else // !PLATFORM_UNIX
        internal const String DLLPREFIX = "lib";
        internal const String DLLNAME = "tcl8.4";
 #if __APPLE__
        internal const String DLLSUFFIX = ".dylib";
 #else
        internal const String DLLSUFFIX = ".so";
 #endif
#endif // !PLATFORM_UNIX    

        internal const String TCL_LIB = DLLPREFIX + DLLNAME + DLLSUFFIX;

        internal const int TCL_OK       = 0;
        internal const int TCL_ERROR    = 1;

        [DllImport(TCL_LIB, EntryPoint="Tcl_CreateInterp", CallingConvention=CallingConvention.Cdecl)]
        internal static extern Tcl_Interp* Tcl_CreateInterp();
        
        [DllImport(TCL_LIB, EntryPoint="Tcl_Init", CallingConvention=CallingConvention.Cdecl)]
        internal static extern int Tcl_Init(Tcl_Interp* interp);
        
        [DllImport(TCL_LIB, EntryPoint="Tcl_Eval", CallingConvention=CallingConvention.Cdecl)]
        internal static extern int Tcl_Eval(Tcl_Interp* interp,
            [MarshalAs(UnmanagedType.LPStr)]string str);

        [DllImport(TCL_LIB, EntryPoint="Tcl_GetVar", CallingConvention=CallingConvention.Cdecl)]
        internal static extern IntPtr Tcl_GetVar(Tcl_Interp* interp,
            [MarshalAs(UnmanagedType.LPStr)] string varName, int flags);
            
        [DllImport(TCL_LIB, EntryPoint="Tcl_CreateCommand", CallingConvention=CallingConvention.Cdecl)]    
        internal static extern IntPtr Tcl_CreateCommand(Tcl_Interp* interp,
              [MarshalAs(UnmanagedType.LPStr)]string cmdName, Tcl_CmdProc proc,
              IntPtr clientData, Tcl_CmdDeleteProc deleteProc);
    }
}
