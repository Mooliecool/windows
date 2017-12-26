//------------------------------------------------------------------------------
// <copyright file="TkNative.cs" company="Microsoft">
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
    internal unsafe class TkNative {
    
#if !PLATFORM_UNIX
        internal const String DLLPREFIX = "";
        internal const String DLLSUFFIX = ".dll";
        internal const String DLLNAME = "tk84";
#else // !PLATFORM_UNIX
        internal const String DLLPREFIX = "lib";
        internal const String DLLNAME = "tk8.4";
 #if __APPLE__
        internal const String DLLSUFFIX = ".dylib";
 #else
        internal const String DLLSUFFIX = ".so";
 #endif
#endif // !PLATFORM_UNIX    

        internal const String TK_LIB = DLLPREFIX + DLLNAME + DLLSUFFIX;

        [DllImport(TK_LIB, EntryPoint="Tk_Init", CallingConvention=CallingConvention.Cdecl)]
        internal static extern int Tk_Init(Tcl_Interp* interp);

        [DllImport(TK_LIB, EntryPoint="Tk_MainEx", CallingConvention=CallingConvention.Cdecl)]
        internal static extern void Tk_MainEx(int argc, [In, MarshalAs(UnmanagedType.LPArray)] string[] argv,
            Tcl_AppInitProc appInitProc, Tcl_Interp* interp);       
    }
}
