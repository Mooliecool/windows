//------------------------------------------------------------------------------
// <copyright file="ArraySubsetEnumerator.cs" company="Microsoft">
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

namespace System.ComponentModel {        

    using System;
    using System.Collections;
    using System.Diagnostics;
    using System.Security.Permissions;

    [HostProtection(SharedState = true)]
    internal class ArraySubsetEnumerator : IEnumerator
    {

        private Array array;
        private int total;
        private int current;

        public ArraySubsetEnumerator(Array array, int count) {
            Debug.Assert(count == 0 || array != null, "if array is null, count should be 0");
            Debug.Assert(array == null || count <= array.Length, "Trying to enumerate more than the array contains");
            this.array = array;
            this.total = count;
            current = -1;
        }

        public bool MoveNext() {
            if (current < total - 1) {
                current++;
                return true;
            }
            else {
                return false;
            }
        }

        public void Reset() {
            current = -1;
        }

        public object Current {
            get {
                if (current == -1) {
                    throw new InvalidOperationException();
                }
                else {
                    return array.GetValue(current);
                }
            }
        }
    }
}

