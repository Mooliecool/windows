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
////////////////////////////////////////////////////////////////////////////////
//
// This is a wrapper class for Pointers
//
// 
//  
//
namespace System.Reflection {
    using System;
    using CultureInfo = System.Globalization.CultureInfo;
    using System.Runtime.Serialization;

    [CLSCompliant(false)]
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
	public sealed class Pointer: ISerializable {
		unsafe private void* _ptr;
		private Type _ptrType;

		private Pointer() {}

		private unsafe Pointer(SerializationInfo info, StreamingContext context)
		{
		    _ptr = ((IntPtr)(info.GetValue("_ptr", typeof(IntPtr)))).ToPointer();
		    _ptrType = (Type)info.GetValue("_ptrType", typeof(Type));
		}

		// This method will box an pointer.  We save both the
		//	value and the type so we can access it from the native code
		//	during an Invoke.
		public static unsafe Object Box(void *ptr,Type type) {
			if (type == null)
				throw new ArgumentNullException("type");
			if (!type.IsPointer)
				throw new ArgumentException(Environment.GetResourceString("Arg_MustBePointer"),"ptr");

			Pointer x = new Pointer();
			x._ptr = ptr;
			x._ptrType = type;
			return x;
		}

		// Returned the stored pointer.
		public static unsafe void* Unbox(Object ptr) {
			if (!(ptr is Pointer))
				throw new ArgumentException(Environment.GetResourceString("Arg_MustBePointer"),"ptr");
			return ((Pointer)ptr)._ptr;
		}
	
		internal Type GetPointerType() {
			return _ptrType;
		}
    
		internal unsafe Object GetPointerValue() {
			return (IntPtr)_ptr;
		}
        
         
        
        unsafe void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context) {
            info.AddValue("_ptr", new IntPtr(_ptr));
            info.AddValue("_ptrType", _ptrType);
        }
    }
}
