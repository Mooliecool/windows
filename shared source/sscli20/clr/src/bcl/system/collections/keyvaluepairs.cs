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
** Class:  KeyValuePairs
**
**
** Purpose: KeyValuePairs to display items in collection class under debugger
**
**
===========================================================*/

namespace System.Collections {
    using System.Diagnostics;
    
    [DebuggerDisplay("{value}", Name = "[{key}]", Type = "" )]
    internal class KeyValuePairs {
        [DebuggerBrowsable(DebuggerBrowsableState.Never)]
        private object key;

        [DebuggerBrowsable(DebuggerBrowsableState.Never)]
        private object value;

        public KeyValuePairs(object key, object value) {
            this.value = value;
            this.key = key;
        }

        public object Key {
            get { return key; }
        }

        public object Value {
            get { return value; }
        }
    }    
}
