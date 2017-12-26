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
** Class:    LockCookie
**
**
** Purpose: Defines the lock that implements 
**          single-writer/multiple-reader semantics
**
**
===========================================================*/

namespace System.Threading {

    using System;
    [System.Runtime.InteropServices.ComVisible(true)]
    public struct LockCookie
    {
        private int _dwFlags;
        private int _dwWriterSeqNum;
        private int _wReaderAndWriterLevel;
        private int _dwThreadID;

        public override int GetHashCode()
        {
            return _dwFlags + _dwWriterSeqNum + _wReaderAndWriterLevel + _dwThreadID;
        }
        
        public override bool Equals(Object obj)
        {
            if (obj is LockCookie)
                return Equals((LockCookie)obj);
            else
                return false;
        }
        
        public bool Equals(LockCookie obj)
        {
            return obj._dwFlags == _dwFlags && obj._dwWriterSeqNum == _dwWriterSeqNum &&
                obj._wReaderAndWriterLevel == _wReaderAndWriterLevel && obj._dwThreadID == _dwThreadID;
        }
        
        public static bool operator ==(LockCookie a, LockCookie b)
        {
            return a.Equals(b);
        }
        
        public static bool operator !=(LockCookie a, LockCookie b)
        {
            return !(a == b);
        }
    }
}

