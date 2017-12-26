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
** Interface:  DictionaryEntry
**
**
** Purpose: Return Value for IDictionaryEnumerator::GetEntry
**
** 
===========================================================*/
namespace System.Collections {
    
    using System;
    // A DictionaryEntry holds a key and a value from a dictionary.
    // It is returned by IDictionaryEnumerator::GetEntry().
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public struct DictionaryEntry
    {
        private Object _key;
        private Object _value;
    
        // Constructs a new DictionaryEnumerator by setting the Key
        // and Value fields appropriately.
        public DictionaryEntry(Object key, Object value) {
            _key = key;
            _value = value;
        }

        public Object Key {
            get {
                return _key;
            }
            
            set {
                _key = value;
            }
        }

        public Object Value {
            get {
                return _value;
            }

            set {
                _value = value;
            }
        }
    }
}
