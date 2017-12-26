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
** Interface:  IList
**
**
** Purpose: Base interface for all generic lists.
**
** 
===========================================================*/
namespace System.Collections.Generic {
    
    using System;
    using System.Runtime.CompilerServices;


    [TypeDependencyAttribute("System.SZArrayHelper")]
    public interface IList<T> : ICollection<T>
    {
        // The Item property provides methods to read and edit entries in the List.
        T this[int index] {
            get;
            set;
        }
    
        // Returns the index of a particular item, if it is in the list.
        // Returns -1 if the item isn't in the list.
        int IndexOf(T item);
    
        // Inserts value into the list at position index.
        // index must be non-negative and less than or equal to the 
        // number of elements in the list.  If index equals the number
        // of items in the list, then value is appended to the end.
        void Insert(int index, T item);
        
        // Removes the item at position index.
        void RemoveAt(int index);
    }
}
