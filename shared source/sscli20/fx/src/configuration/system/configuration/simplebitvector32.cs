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

namespace System.Configuration {
    using System;

    //
    // This is a cut down copy of System.Collections.Specialized.BitVector32. The
    // reason this is here is because it is used rather intensively by Control and
    // WebControl. As a result, being able to inline this operations results in a
    // measurable performance gain, at the expense of some maintainability.
    //
    [Serializable]
    internal struct SimpleBitVector32 {
        private int data;

        internal SimpleBitVector32(int data) {
            this.data = data;
        }

        internal int Data {
            get { return data; }
        }

        internal bool this[int bit] {
            get {
                return (data & bit) == bit;
            }
            set {
                int _data = data;
                if(value) {
                    data = _data | bit;
                }
                else {
                    data = _data & ~bit;
                }
            }
        }

    }
}
