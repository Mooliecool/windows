
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
/*============================================================
**
** Class:  ISymbolBinder
**
**
** Represents a symbol binder for managed code.
**
** 
===========================================================*/
namespace System.Diagnostics.SymbolStore {
    
    using System;
    using System.Text;
    using System.Runtime.InteropServices;
	
    // Interface does not need to be marked with the serializable attribute
[System.Runtime.InteropServices.ComVisible(true)]
    public interface ISymbolBinder
    {
        [Obsolete("The recommended alternative is ISymbolBinder1.GetReader. ISymbolBinder1.GetReader takes the importer interface pointer as an IntPtr instead of an Int32, and thus works on both 32-bit and 64-bit architectures. http://go.microsoft.com/fwlink/?linkid=14202=14202")]
        ISymbolReader GetReader(int importer, String filename,
                                String searchPath);
    }

    // This interface has a revised ISymbolBinder.GetReader() with the proper signature.
    // It is not called ISymbolBinder2 because it maps to the IUnmanagedSymbolBinder interfaces, and 
    // does not wrap the IUnmanagedSymbolBinder2 interfaces declared in CorSym.idl.
[System.Runtime.InteropServices.ComVisible(true)]
    public interface ISymbolBinder1
    {
    
        ISymbolReader GetReader(IntPtr importer, String filename,
                                String searchPath);
    }

}
