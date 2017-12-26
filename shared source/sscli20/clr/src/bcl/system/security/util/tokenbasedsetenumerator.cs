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
// TokenBasedSetEnumerator.cs
//

namespace System.Security.Util 
{
    using System;
    using System.Collections;

    internal struct TokenBasedSetEnumerator
    {
        public Object Current;
        public int Index;
                
        private TokenBasedSet _tb;
                            
        public bool MoveNext()
        {
            return _tb != null ? _tb.MoveNext(ref this) : false;
        }
                
        public void Reset()
        {
            Index = -1;
            Current = null;
        }
                            
        public TokenBasedSetEnumerator(TokenBasedSet tb)
        {
            Index = -1;
            Current = null;
            _tb = tb;
        }
    }
}

