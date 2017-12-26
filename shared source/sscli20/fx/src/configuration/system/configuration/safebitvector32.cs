//------------------------------------------------------------------------------
// <copyright file="SimpleBitVector32.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

using System;
using System.Threading;

namespace System.Configuration {
    //
    // This is a multithreadsafe version of System.Collections.Specialized.BitVector32.
    //
    [Serializable]
    internal struct SafeBitVector32 {
        private volatile int _data;
    
        internal SafeBitVector32(int data) {
            this._data = data;
        }


        internal bool this[int bit] {
            get {
                int data = _data;
                return (data & bit) == bit;
            }
            set {
                for (;;) {
                    int oldData = _data;
                    int newData;
                    if (value) {
                        newData = oldData | bit;
                    }
                    else {
                        newData = oldData & ~bit;
                    }

#pragma warning disable 0420
                    int result = Interlocked.CompareExchange(ref _data, newData, oldData);
#pragma warning restore 0420

                    if (result == oldData) {
                        break;
                    }
                }
            }
        }
    }
}

