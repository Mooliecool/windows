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

//
// Config.cs
//

namespace System.Security.Util {
    using System;
    using System.Security.Util;
    using System.Security.Policy;
    using System.Security.Permissions;
    using System.Collections;
    using System.IO;
    using System.Reflection;
    using System.Globalization;
    using System.Text;
    using System.Runtime.Serialization.Formatters.Binary;
    using System.Threading;
    using System.Runtime.CompilerServices;

    // Duplicated in vm\COMSecurityConfig.h
    [Serializable,Flags]
    internal enum QuickCacheEntryType
    {
        FullTrustZoneMyComputer = 0x1000000,
        FullTrustZoneIntranet = 0x2000000,
        FullTrustZoneInternet = 0x4000000,
        FullTrustZoneTrusted = 0x8000000,
        FullTrustZoneUntrusted = 0x10000000,
        FullTrustAll = 0x20000000,
    }

    internal static class Config {
        private static string m_machineConfig;
        private static string m_userConfig;

        private static void GetFileLocales()
        {
            if (m_machineConfig == null)
                m_machineConfig = _GetMachineDirectory();
            if (m_userConfig == null)
                m_userConfig = _GetUserDirectory();
        }

        internal static string MachineDirectory
        {
            get
            {
                GetFileLocales();
                return m_machineConfig;
            }
        }

        internal static string UserDirectory
        {
            get
            {
                GetFileLocales();
                return m_userConfig;
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern bool SaveDataByte(string path, byte[] data, int offset, int length);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern bool RecoverData(ConfigId id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void SetQuickCache(ConfigId id, QuickCacheEntryType quickCacheFlags);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern bool GetCacheEntry(ConfigId id, int numKey, char[] key, out byte[] data);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void AddCacheEntry(ConfigId id, int numKey, char[] key, byte[] data);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void ResetCacheData(ConfigId id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern string _GetMachineDirectory();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern string _GetUserDirectory();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern bool WriteToEventLog(string message);
    }
}
