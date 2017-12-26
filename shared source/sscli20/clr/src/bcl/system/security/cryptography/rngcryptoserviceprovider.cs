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

//
// RNGCryptoServiceProvider.cs
//

namespace System.Security.Cryptography {
    using Microsoft.Win32;
    using System.Runtime.InteropServices;

[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class RNGCryptoServiceProvider : RandomNumberGenerator {

        //
        // public constructors
        //

        public RNGCryptoServiceProvider() { }


        //
        // public methods
        //

        public override void GetBytes(byte[] data) {
            if (data == null) throw new ArgumentNullException("data");
            if (!Win32Native.Random(true, data, data.Length))
                throw new CryptographicException(Marshal.GetLastWin32Error());
        }

        public override void GetNonZeroBytes(byte[] data) {
            if (data == null)
                throw new ArgumentNullException("data");

            GetBytes(data);

            int indexOfFirst0Byte = data.Length;
            for (int i = 0; i < data.Length; i++) {
                if (data[i] == 0) {
                    indexOfFirst0Byte = i;
                    break;
                }
            }
            for (int i = indexOfFirst0Byte; i < data.Length; i++) {
                if (data[i] != 0) {
                    data[indexOfFirst0Byte++] = data[i];
                }
            }

            while (indexOfFirst0Byte < data.Length) {
                // this should be more than enough to fill the rest in one iteration
                byte[] tmp = new byte[2 * (data.Length - indexOfFirst0Byte)];
                GetBytes(tmp);

                for (int i = 0; i < tmp.Length; i++) {
                    if (tmp[i] != 0) {
                        data[indexOfFirst0Byte++] = tmp[i];
                        if (indexOfFirst0Byte >= data.Length) break;
                    }
                }
            }
        }
    }
}
