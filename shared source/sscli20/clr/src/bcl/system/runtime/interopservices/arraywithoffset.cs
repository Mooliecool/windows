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
namespace System.Runtime.InteropServices {

    using System;
    using System.Runtime.CompilerServices;

    [Serializable]
    [System.Runtime.InteropServices.ComVisible(true)]
    public struct ArrayWithOffset
    {
        //private ArrayWithOffset()
        //{
        //    throw new Exception();
        //}
    
        public ArrayWithOffset(Object array, int offset)
        {
            m_array  = array;
            m_offset = offset;
            m_count  = 0;
            m_count  = CalculateCount();
        }
    
        public Object GetArray()
        {
            return m_array;
        }
    
        public int GetOffset()
        {
            return m_offset;
        }
    
        public override int GetHashCode()
        {
            return m_count + m_offset;
        }
        
        public override bool Equals(Object obj)
        {
            if (obj is ArrayWithOffset)
                return Equals((ArrayWithOffset)obj);
            else
                return false;
        }

        public bool Equals(ArrayWithOffset obj)
        {
            return obj.m_array == m_array && obj.m_offset == m_offset && obj.m_count == m_count;
        }
    
        public static bool operator ==(ArrayWithOffset a, ArrayWithOffset b)
        {
            return a.Equals(b);
        }
        
        public static bool operator !=(ArrayWithOffset a, ArrayWithOffset b)
        {
            return !(a == b);
        }
    	       
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern int CalculateCount();
    
        private Object m_array;
        private int    m_offset;
        private int    m_count;
    }

}
