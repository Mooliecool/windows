using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace ShellDll
{
    [ComImport]
    [Guid("0000010f-0000-0000-C000-000000000046")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IAdviseSink
    {
        // Advises that data has changed
        [PreserveSig]
        void OnDataChange(
            ref ShellAPI.FORMATETC pformatetcIn,
            ref ShellAPI.STGMEDIUM pmedium);

        // Advises that view of object has changed
        [PreserveSig]
        void OnViewChange(
            int dwAspect, 
            int lindex);

        // Advises that name of object has changed
        [PreserveSig]
        void OnRename(
            IntPtr pmk);

        // Advises that object has been saved to disk
        [PreserveSig]
        void OnSave();

        // Advises that object has been closed
        [PreserveSig]
        void OnClose();
    }
}
