using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace ShellDll
{
    [ComImport()]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [GuidAttribute("000214e8-0000-0000-c000-000000000046")]
    public interface IShellExtInit
    {
        [PreserveSig()]
        int Initialize(
            IntPtr pidlFolder, 
            IntPtr lpdobj, 
            uint hKeyProgID);
    }
}
