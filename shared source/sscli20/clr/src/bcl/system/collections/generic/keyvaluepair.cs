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
** Interface:  KeyValuePair
**
**
** Purpose: Generic key-value pair for dictionary enumerators.
**
** 
===========================================================*/
namespace System.Collections.Generic {
    
    using System;
    using System.Text;    

    // A KeyValuePair holds a key and a value from a dictionary.
    // It is returned by IDictionaryEnumerator::GetEntry().
    [Serializable()]    

    public struct KeyValuePair<TKey, TValue> {
        private TKey key;
        private TValue value;

        public KeyValuePair(TKey key, TValue value) {
            this.key = key;
            this.value = value;
        }

        public TKey Key {
            get { return key; }
        }

        public TValue Value {
            get { return value; }
        }

        public override string ToString() {
            StringBuilder s = new StringBuilder();
            s.Append('[');
            if( Key != null) {
                s.Append(Key.ToString());
            }
            s.Append(", ");
            if( Value != null) {
               s.Append(Value.ToString());
            }
            s.Append(']');
            return s.ToString();
        }
    }
}
