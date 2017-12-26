//------------------------------------------------------------------------------
// <copyright file="ExternDll.cs" company="Microsoft">
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

namespace System {
    internal static class ExternDll {


 #if !PLATFORM_UNIX
        internal const String DLLPREFIX = "";
        internal const String DLLSUFFIX = ".dll";
 #else // !PLATFORM_UNIX
  #if __APPLE__
        internal const String DLLPREFIX = "lib";
        internal const String DLLSUFFIX = ".dylib";
  #else
        internal const String DLLPREFIX = "lib";
        internal const String DLLSUFFIX = ".so";
  #endif
 #endif // !PLATFORM_UNIX

        public const string Kernel32 = DLLPREFIX + "rotor_pal" + DLLSUFFIX;
        public const string User32 = DLLPREFIX + "rotor_pal" + DLLSUFFIX;
        public const string Mscoree  = DLLPREFIX + "sscoree" + DLLSUFFIX;
    }
}
