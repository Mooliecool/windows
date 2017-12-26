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
** Class:  ISymbolNamespace
**
**
** Represents a namespace within a symbol reader.
**
** 
===========================================================*/
namespace System.Diagnostics.SymbolStore {
    
    using System;
	
	// Interface does not need to be marked with the serializable attribute
[System.Runtime.InteropServices.ComVisible(true)]
    public interface ISymbolNamespace
    {
        // Get the name of this namespace
        String Name { get; }
    
        // Get the children of this namespace
        ISymbolNamespace[] GetNamespaces();
    
        // Get the variables in this namespace
        ISymbolVariable[] GetVariables();
    }
}
