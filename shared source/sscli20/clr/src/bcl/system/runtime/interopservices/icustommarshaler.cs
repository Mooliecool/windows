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
/*=============================================================================
**
** Class: ICustomMarshaler
**
**
** Purpose: This the base interface that must be implemented by all custom
**          marshalers.
**
**
=============================================================================*/

namespace System.Runtime.InteropServices {
	using System;

[System.Runtime.InteropServices.ComVisible(true)]
    public interface ICustomMarshaler
    {		
        Object MarshalNativeToManaged( IntPtr pNativeData );

        IntPtr MarshalManagedToNative( Object ManagedObj );

        void CleanUpNativeData( IntPtr pNativeData );

        void CleanUpManagedData( Object ManagedObj );

        int GetNativeDataSize();
    }
}
