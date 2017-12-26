// ==++==
//
//  
//   Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//  
//   The use and distribution terms for this software are contained in the file
//   named license.txt, which can be found in the root of this distribution.
//   By using this software in any fashion, you are agreeing to be bound by the
//   terms of this license.
//  
//   You must not remove this notice, or any other, from this software.
//  
//
//  zlib.h -- interface of the 'zlib' general purpose compression library
//  version 1.2.1, November 17th, 2003
//
//  Copyright (C) 1995-2003 Jean-loup Gailly and Mark Adler
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any damages
//  arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any purpose,
//  including commercial applications, and to alter it and redistribute it
//  freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software. If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//  2. Altered source versions must be plainly marked as such, and must not be
//     misrepresented as being the original software.
//  3. This notice may not be removed or altered from any source distribution.
//
//
// ==--==
// Compression engine

namespace System.IO.Compression {
    using System;
    using System.Diagnostics;

    internal class Deflater {
        private FastEncoder encoder;

        public Deflater(bool doGZip) {
            encoder = new FastEncoder(doGZip);
        }    

        public void SetInput(byte[] input, int startIndex, int count) {
            encoder.SetInput(input, startIndex, count);
        }

        public int GetDeflateOutput(byte[] output) {
            Debug.Assert(output != null, "Can't pass in a null output buffer!");
            return encoder.GetCompressedOutput(output);
        }

        public bool NeedsInput() {
            return encoder.NeedsInput();
        }

        public int Finish(byte[] output) {
            Debug.Assert(output != null, "Can't pass in a null output buffer!");
            return encoder.Finish(output);
        }
    }
}
