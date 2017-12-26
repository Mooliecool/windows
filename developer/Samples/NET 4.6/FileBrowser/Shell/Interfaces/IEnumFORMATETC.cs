using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace ShellDll
{
    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("00000103-0000-0000-C000-000000000046")]
    public interface IEnumFORMATETC
    {
        // Retrieves the specified number of FORMATETC structures in the enumeration 
        // sequence and advances the current position by the number of items retrieved
        [PreserveSig]
        int GetNext(
            int celt, 
            ref ShellAPI.FORMATETC rgelt, 
            ref int pceltFetched);

        // Skips over the specified number of elements in the enumeration sequence
        [PreserveSig]
        int Skip(int celt);

        // Returns to the beginning of the enumeration sequence
        [PreserveSig]
        int Reset();

        // Creates a new item enumeration object with the same contents and state as the current one
        [PreserveSig]
        int Clone(
            ref IEnumFORMATETC ppenum);
    }
}
