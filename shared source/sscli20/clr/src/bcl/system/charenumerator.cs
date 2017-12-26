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
** Class: CharEnumerator
**
**
** Purpose: Enumerates the characters on a string.  skips range
**          checks.
**
**
============================================================*/
namespace System {

    using System.Collections;
    using System.Collections.Generic;    

[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable] 
    public sealed class CharEnumerator : IEnumerator, ICloneable, IEnumerator<char> {
        private String str;
        private int index;
        private char currentElement;

        internal CharEnumerator(String str) {
            this.str = str;
            this.index = -1;
        }

        public Object Clone() {
            return MemberwiseClone();
        }
    
        public bool MoveNext() {
            if (index < (str.Length-1)) {
                index++;
                currentElement = str[index];
                return true;
            }
            else
                index = str.Length;
            return false;

        }

        void IDisposable.Dispose() {
        }
    
        /// <internalonly/>
        Object IEnumerator.Current {
            get {
                if (index == -1)
                    throw new InvalidOperationException(Environment.GetResourceString(ResId.InvalidOperation_EnumNotStarted));
                if (index >= str.Length)
                    throw new InvalidOperationException(Environment.GetResourceString(ResId.InvalidOperation_EnumEnded));                        
                    
                return currentElement;
            }
        }
    
        public char Current {
            get {
                if (index == -1)
                    throw new InvalidOperationException(Environment.GetResourceString(ResId.InvalidOperation_EnumNotStarted));
                if (index >= str.Length)
                    throw new InvalidOperationException(Environment.GetResourceString(ResId.InvalidOperation_EnumEnded));                                            
                return currentElement;
            }
        }

        public void Reset() {
            currentElement = (char)0;
            index = -1;
        }
    }
}
