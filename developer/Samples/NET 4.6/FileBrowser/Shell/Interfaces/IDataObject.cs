using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace ShellDll
{
    [ComImport]
    [Guid("0000010e-0000-0000-C000-000000000046")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IDataObject
    {
        // Renders the data described in a FORMATETC structure 
        // and transfers it through the STGMEDIUM structure
        [PreserveSig]
        Int32 GetData(
            ref ShellAPI.FORMATETC pformatetcIn,
            ref ShellAPI.STGMEDIUM pmedium);

        // Renders the data described in a FORMATETC structure 
        // and transfers it through the STGMEDIUM structure allocated by the caller
        [PreserveSig]
        Int32 GetDataHere(
            ref ShellAPI.FORMATETC pformatetcIn,
            ref ShellAPI.STGMEDIUM pmedium);

        // Determines whether the data object is capable of 
        // rendering the data described in the FORMATETC structure
        [PreserveSig]
        Int32 QueryGetData(
            ref ShellAPI.FORMATETC pformatetc);

        // Provides a potentially different but logically equivalent FORMATETC structure
        [PreserveSig]
        Int32 GetCanonicalFormatEtc(
            ref ShellAPI.FORMATETC pformatetc,
            ref ShellAPI.FORMATETC pformatetcout);

        // Provides the source data object with data described by a 
        // FORMATETC structure and an STGMEDIUM structure
        [PreserveSig]
        Int32 SetData(
            ref ShellAPI.FORMATETC pformatetcIn,
            ref ShellAPI.STGMEDIUM pmedium, 
            bool frelease);

        // Creates and returns a pointer to an object to enumerate the 
        // FORMATETC supported by the data object
        [PreserveSig]
        Int32 EnumFormatEtc(
            int dwDirection,
            ref IEnumFORMATETC ppenumFormatEtc);

        // Creates a connection between a data object and an advise sink so 
        // the advise sink can receive notifications of changes in the data object
        [PreserveSig]
        Int32 DAdvise(
            ref ShellAPI.FORMATETC pformatetc,
            ref ShellAPI.ADVF advf,
            ref IAdviseSink pAdvSink, 
            ref int pdwConnection);

        // Destroys a notification previously set up with the DAdvise method
        [PreserveSig]
        Int32 DUnadvise(
            int dwConnection);
        
        // Creates and returns a pointer to an object to enumerate the current advisory connections
        [PreserveSig]
        Int32 EnumDAdvise(
            ref object ppenumAdvise);
    }
}
