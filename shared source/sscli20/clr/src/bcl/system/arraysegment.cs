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
** Class:  ArraySegment<T>
**
**
** Purpose: Convenient wrapper for an array, an offset, and
**          a count.  Ideally used in streams & collections.
**          Net Classes will consume an array of these.
**
**
===========================================================*/

using System.Runtime.InteropServices;

namespace System {

    [Serializable]
    public struct ArraySegment<T> 
    {
        private T[] _array;
        private int _offset;
        private int _count;
        
        public ArraySegment(T[] array)
        {
            if (array == null)
                throw new ArgumentNullException("array");

            _array = array;
            _offset = 0;
            _count = array.Length;
        }

        public ArraySegment(T[] array, int offset, int count)
        {
            if (array == null)
                throw new ArgumentNullException("array");
            if (offset < 0)
                throw new ArgumentOutOfRangeException("offset", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (count < 0)
                throw new ArgumentOutOfRangeException("count", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (array.Length - offset < count)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidOffLen"));

            _array = array;
            _offset = offset;
            _count = count;
        }

        public T[] Array {
            get { return _array; }
        }

        public int Offset {
            get { return _offset; }
        }

        public int Count {
            get { return _count; }
        }
        
        public override int GetHashCode()
        {
            return _array.GetHashCode() ^ _offset   ^ _count;
        }

        public override bool Equals(Object obj)
        {
            if (obj is ArraySegment<T>)
                return Equals((ArraySegment<T>)obj);
            else
                return false;
        }
    
        public bool Equals(ArraySegment<T> obj)
        {
            return obj._array == _array && obj._offset == _offset && obj._count == _count;
        }
    
        public static bool operator ==(ArraySegment<T> a, ArraySegment<T> b)
        {
            return a.Equals(b);
        }
        
        public static bool operator !=(ArraySegment<T> a, ArraySegment<T> b)
        {
            return !(a == b);
        }        
        
    }
}
