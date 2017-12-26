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
** Interface:  ICollection
**
**
** Purpose: Base interface for all generic collections.
**
** 
===========================================================*/
namespace System.Collections.Generic {
    using System;
    using System.Runtime.CompilerServices;


    [TypeDependencyAttribute("System.SZArrayHelper")]
    public interface ICollection<T> : IEnumerable<T>
    {
        // Interfaces are not serialable
        // Number of items in the collections.        
        int Count { get; }

        bool IsReadOnly { get; }

        void Add(T item);

        void Clear();

        bool Contains(T item); 
                
        // CopyTo copies a collection into an Array, starting at a particular
        // index into the array.
        // 
        void CopyTo(T[] array, int arrayIndex);
                
        //void CopyTo(int sourceIndex, T[] destinationArray, int destinationIndex, int count);

        bool Remove(T item);


    }
}
