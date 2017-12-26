using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Reflection;
using System.Diagnostics;

namespace Microsoft.SDK.Samples.VistaBridge.Interop
{
    internal class DllVersionManager
    {

        internal static IntPtr GetNativeFunctionPointer(string dllName, string functionName)
        {
            IntPtr hModule = NativeMethods.LoadLibrary(dllName);
            if (hModule == IntPtr.Zero)
                throw new FileNotFoundException("Couldn't find DLL '" + dllName + "'");
            IntPtr procAddress = NativeMethods.GetProcAddress(hModule, functionName);
            if (procAddress == IntPtr.Zero)
                throw new EntryPointNotFoundException(
                    "Can't find function '" + functionName + "' in DLL '" + dllName
                    + "'. Certain DLLs (e.g. comctl32.dll) require an application manifest be provided with the executable so the correct version of the DLL is loaded.");
            return procAddress;
        }
    }
}
