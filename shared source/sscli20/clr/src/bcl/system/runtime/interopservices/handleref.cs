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
namespace System.Runtime.InteropServices
{
    
    using System;

    [System.Runtime.InteropServices.ComVisible(true)]
    public struct HandleRef
    {

        // ! Do not add or rearrange fields as the EE depends on this layout.
        //------------------------------------------------------------------
        internal Object m_wrapper;
        internal IntPtr m_handle;
        //------------------------------------------------------------------


        public HandleRef(Object wrapper, IntPtr handle)
        {
            m_wrapper = wrapper;
            m_handle  = handle;
        }

        public Object Wrapper {
            get {
                return m_wrapper;
            }
        }
    
        public IntPtr Handle {
            get {
                return m_handle;
            }
        }
    
    
        public static explicit operator IntPtr(HandleRef value)
        {
            return value.m_handle;
        }

        public static IntPtr ToIntPtr(HandleRef value)
        {
            return value.m_handle;
        }
    }
}
