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
** Interface:  IDictionary
**
**
** Purpose: Base interface for all generic dictionaries.
**
** 
===========================================================*/
namespace System.Collections.Generic {
    using System;

    // An IDictionary is a possibly unordered set of key-value pairs.
    // Keys can be any non-null object.  Values can be any object.
    // You can look up a value in an IDictionary via the default indexed
    // property, Items.  

    public interface IDictionary<TKey, TValue> : ICollection<KeyValuePair<TKey, TValue>>
    {
        // Interfaces are not serializable
        // The Item property provides methods to read and edit entries 
        // in the Dictionary.
        TValue this[TKey key] {
            get;
            set;
        }
    
        // Returns a collections of the keys in this dictionary.
        ICollection<TKey> Keys {
            get;
        }
    
        // Returns a collections of the values in this dictionary.
        ICollection<TValue> Values {
            get;
        }
    
        // Returns whether this dictionary contains a particular key.
        //
        bool ContainsKey(TKey key);
    
        // Adds a key-value pair to the dictionary.
        // 
        void Add(TKey key, TValue value);
    
        // Removes a particular key from the dictionary.
        //
        bool Remove(TKey key);

        bool TryGetValue(TKey key, out TValue value);
    }
}
