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
//-------------------------------------------------------------
// FusionInterfaces.cs
//
// This implements wrappers to Fusion interfaces
//-------------------------------------------------------------
namespace Microsoft.Win32
{
    using System;
    using System.IO;
    using System.Collections;
    using System.Runtime.InteropServices;
    using System.Runtime.CompilerServices;
    using System.Globalization;     
    using StringBuilder = System.Text.StringBuilder;
    using Microsoft.Win32.SafeHandles;

    
    
    internal static class ASM_CACHE
    {
         public const uint ZAP          = 0x1;
         public const uint GAC          = 0x2;
         public const uint DOWNLOAD     = 0x4;
    }

    internal static class CANOF
    {
        public const uint PARSE_DISPLAY_NAME = 0x1;
        public const uint SET_DEFAULT_VALUES = 0x2;
    }
    
    internal static class ASM_NAME
    {   
         public const uint PUBLIC_KEY            = 0;
         public const uint PUBLIC_KEY_TOKEN      = PUBLIC_KEY + 1;
         public const uint HASH_VALUE            = PUBLIC_KEY_TOKEN + 1;
         public const uint NAME                  = HASH_VALUE + 1;
         public const uint MAJOR_VERSION         = NAME + 1;
         public const uint MINOR_VERSION         = MAJOR_VERSION + 1;
         public const uint BUILD_NUMBER          = MINOR_VERSION + 1;
         public const uint REVISION_NUMBER       = BUILD_NUMBER + 1;
         public const uint CULTURE               = REVISION_NUMBER + 1;
         public const uint PROCESSOR_ID_ARRAY    = CULTURE + 1;
         public const uint OSINFO_ARRAY          = PROCESSOR_ID_ARRAY + 1;
         public const uint HASH_ALGID            = OSINFO_ARRAY + 1;
         public const uint ALIAS                 = HASH_ALGID + 1;
         public const uint CODEBASE_URL          = ALIAS + 1;
         public const uint CODEBASE_LASTMOD      = CODEBASE_URL + 1;
         public const uint NULL_PUBLIC_KEY       = CODEBASE_LASTMOD + 1;
         public const uint NULL_PUBLIC_KEY_TOKEN  = NULL_PUBLIC_KEY + 1;
         public const uint CUSTOM                = NULL_PUBLIC_KEY_TOKEN + 1;
         public const uint NULL_CUSTOM           = CUSTOM + 1;
         public const uint MVID                  = NULL_CUSTOM + 1;
         public const uint _32_BIT_ONLY          = MVID + 1;
         public const uint MAX_PARAMS            = _32_BIT_ONLY + 1;
    }
    
    internal static class Fusion
    {
        public static void ReadCache(ArrayList alAssems, String name, uint nFlag)
        {
            SafeFusionHandle aEnum      = null;
            SafeFusionHandle aNameEnum  = null;
            SafeFusionHandle AppCtx     = SafeFusionHandle.InvalidHandle;
            int hr;
            
            if (name != null) {
                hr = Win32Native.CreateAssemblyNameObject(out aNameEnum, name, CANOF.PARSE_DISPLAY_NAME, IntPtr.Zero);
                if (hr != 0)
                    Marshal.ThrowExceptionForHR(hr);
            }

            using (aNameEnum) {
            
                hr = Win32Native.CreateAssemblyEnum(out aEnum, AppCtx, aNameEnum, nFlag, IntPtr.Zero);
                if (hr != 0)
                    Marshal.ThrowExceptionForHR(hr);
            
                using (aEnum) {
            
                    for (;;) using (SafeFusionHandle aAppCtx = new SafeFusionHandle(), aName = new SafeFusionHandle())
                    {
                        if (!GetNextAssembly(aEnum, aAppCtx, aName, 0))
                            break;

                        String sDisplayName = GetDisplayName(aName, 0);
                        if (sDisplayName == null)
                            continue;

                        alAssems.Add(sDisplayName);
                    }
            
                } // using (aEnum)
            } // using (aNameEnum)
        }


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        static extern bool GetNextAssembly(SafeFusionHandle pEnum, SafeFusionHandle pAppCtx, SafeFusionHandle pName, uint dwFlags);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        static extern String GetDisplayName(SafeFusionHandle pName, uint dwDisplayFlags);

    }
}
