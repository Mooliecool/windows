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
** Class:  UIntPtr
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
	
	[Serializable(),CLSCompliant(false)] 
[System.Runtime.InteropServices.ComVisible(true)]
    public struct UIntPtr : ISerializable
	{

		unsafe private void* m_value;

        public static readonly UIntPtr Zero;

				
		public unsafe UIntPtr(uint value)
		{
			m_value = (void *)value;
		}
	
		public unsafe UIntPtr(ulong value)
		{
   				#if WIN32
			        m_value = (void *)checked((uint)value);
				#else
					m_value = (void *)value;
				#endif
		}

        [CLSCompliant(false)]
        public unsafe UIntPtr(void* value)
        {
            m_value = value;
        }

        private unsafe UIntPtr(SerializationInfo info, StreamingContext context) {
            ulong l = info.GetUInt64("value");

            if (Size==4 && l>UInt32.MaxValue) {
                throw new ArgumentException(Environment.GetResourceString("Serialization_InvalidPtrValue"));
            }

            m_value = (void *)l;
        }

        unsafe void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context) {
            if (info==null) {
                throw new ArgumentNullException("info");
            }
            info.AddValue("value", (ulong)m_value);
        }

        public unsafe override bool Equals(Object obj) {
			if (obj is UIntPtr) {
				return (m_value == ((UIntPtr)obj).m_value);
            }
			return false;
		}
    
       	public unsafe override int GetHashCode() {
			return unchecked((int)((long)m_value)) & 0x7fffffff;
        }

        public unsafe uint ToUInt32() {
        #if WIN32
			return (uint)m_value;
		#else
			return checked((uint)m_value);
		#endif
        }

        public unsafe ulong ToUInt64() {
            return (ulong)m_value;
        }
      
    	public unsafe override String ToString() {
		#if WIN32
			return ((uint)m_value).ToString(CultureInfo.InvariantCulture);
		#else
			return ((ulong)m_value).ToString(CultureInfo.InvariantCulture);
		#endif
        }

        public static explicit operator UIntPtr (uint value) 
		{
			return new UIntPtr(value);
		}

		public static explicit operator UIntPtr (ulong value) 
		{
			return new UIntPtr(value);
		}
	
		public unsafe static explicit operator uint (UIntPtr  value) 
		{
            #if WIN32
			    return (uint)value.m_value;
		    #else
			    return checked((uint)value.m_value);
		    #endif

		}

		public unsafe static explicit operator ulong (UIntPtr  value) 
		{
			return (ulong)value.m_value;
		}

        [CLSCompliant(false)]
        public static unsafe explicit operator UIntPtr (void* value)
        {
            return new UIntPtr(value);
        }

        [CLSCompliant(false)]
        public static unsafe explicit operator void* (UIntPtr value)
        {
            return value.ToPointer();
        }


		public unsafe static bool operator == (UIntPtr value1, UIntPtr value2) 
		{
			return value1.m_value == value2.m_value;
		}

		public unsafe static bool operator != (UIntPtr value1, UIntPtr value2) 
		{
			return value1.m_value != value2.m_value;
		}

		public static int Size
		{
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
        public unsafe void* ToPointer()
        {
            return m_value;
        }

 	}
}


