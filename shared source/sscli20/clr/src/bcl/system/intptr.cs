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
** Class:  IntPtr
**
**
** Purpose: Platform independent integer
**
** 
===========================================================*/

namespace System {
    
    using System;
    using System.Globalization;
    using System.Runtime.Serialization;
    using System.Runtime.CompilerServices;
    using System.Runtime.ConstrainedExecution;
    
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public struct IntPtr : ISerializable
    {

        unsafe private void* m_value; // The compiler treats void* closest to uint hence explicit casts are required to preserve int behavior
                
        public static readonly IntPtr Zero;

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal unsafe bool IsNull()
        {
            return (this.m_value == null);
        }

        [ReliabilityContract(Consistency.MayCorruptInstance, Cer.MayFail)]
        public unsafe IntPtr(int value)
        {
            #if WIN32
                m_value = (void *)value;
            #else
                m_value = (void *)(long)value;
            #endif
        }
    
        [ReliabilityContract(Consistency.MayCorruptInstance, Cer.MayFail)]
        public unsafe IntPtr(long value)
        {
            #if WIN32
                m_value = (void *)checked((int)value);
            #else
                m_value = (void *)value;
            #endif
        }

        [CLSCompliant(false)]
        [ReliabilityContract(Consistency.MayCorruptInstance, Cer.MayFail)]
        public unsafe IntPtr(void* value)
        {
            m_value = value;
        }

        private unsafe IntPtr(SerializationInfo info, StreamingContext context) {
            long l = info.GetInt64("value");

            if (Size==4 && (l>Int32.MaxValue || l<Int32.MinValue)) {
                throw new ArgumentException(Environment.GetResourceString("Serialization_InvalidPtrValue"));
            }

            m_value = (void *)l;
        }

        unsafe void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context) {
            if (info==null) {
                throw new ArgumentNullException("info");
            }
            #if WIN32
                info.AddValue("value", (long)((int)m_value));
            #else
                info.AddValue("value", (long)(m_value));
            #endif
        }

        public unsafe override bool Equals(Object obj) {
            if (obj is IntPtr) {
                return (m_value == ((IntPtr)obj).m_value);
            }
            return false;
        }
    
        public unsafe override int GetHashCode() {
            return unchecked((int)((long)m_value));
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public unsafe int ToInt32() {
            #if WIN32
                return (int)m_value;
            #else
                long l = (long)m_value;
                return checked((int)l);
            #endif
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public unsafe long ToInt64() {
            #if WIN32
                return (long)(int)m_value;
            #else
                return (long)m_value;
            #endif
        }

        public unsafe override String ToString() {
            #if WIN32
                return ((int)m_value).ToString(CultureInfo.InvariantCulture);
            #else
                return ((long)m_value).ToString(CultureInfo.InvariantCulture);
            #endif
        }

        public unsafe  String ToString(String format) 
        {
            #if WIN32
                return ((int)m_value).ToString(format, CultureInfo.InvariantCulture);
            #else
                return ((long)m_value).ToString(format, CultureInfo.InvariantCulture);
            #endif
        }


        [ReliabilityContract(Consistency.MayCorruptInstance, Cer.MayFail)]
        public static explicit operator IntPtr (int value) 
        {
            return new IntPtr(value);
        }

        [ReliabilityContract(Consistency.MayCorruptInstance, Cer.MayFail)]
        public static explicit operator IntPtr (long value) 
        {
            return new IntPtr(value);
        }

        [CLSCompliant(false), ReliabilityContract(Consistency.MayCorruptInstance, Cer.MayFail)]
        public static unsafe explicit operator IntPtr (void* value)
        {
            return new IntPtr(value);
        }

        [CLSCompliant(false)]
        public static unsafe explicit operator void* (IntPtr value)
        {
            return value.ToPointer();
        }

        public unsafe static explicit operator int (IntPtr  value) 
        {
            #if WIN32
                return (int)value.m_value;
            #else
                long l = (long)value.m_value;
                return checked((int)l);
            #endif
        }

        public unsafe static explicit operator long (IntPtr  value) 
        {
            #if WIN32
                return (long)(int)value.m_value;
            #else
                return (long)value.m_value;
            #endif
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public unsafe static bool operator == (IntPtr value1, IntPtr value2) 
        {
            return value1.m_value == value2.m_value;
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public unsafe static bool operator != (IntPtr value1, IntPtr value2) 
        {
            return value1.m_value != value2.m_value;
        }

        public static int Size
        {
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            get
            {
                #if WIN32
                    return 4;
                #else
                    return 8;
                #endif
            }
        }
    

        [CLSCompliant(false)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public unsafe void* ToPointer()
        {
            return m_value;
        }
    }
}


