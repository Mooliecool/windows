//------------------------------------------------------------------------------
// <copyright file="BinHexEncoder.cs" company="Microsoft">
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

namespace System.Xml {
    internal abstract class BinHexEncoder {

        private const string s_hexDigits = "0123456789ABCDEF";
        private const int CharsChunkSize = 128;

        internal static void Encode( byte[] buffer, int index, int count, XmlWriter writer ) {
            if ( buffer == null ) {
                throw new ArgumentNullException( "buffer" );
            }
            if ( index < 0 ) {
                throw new ArgumentOutOfRangeException( "index" );
            }
            if ( count < 0 ) {
                throw new ArgumentOutOfRangeException( "count" );
            }
            if ( count > buffer.Length - index ) {
                throw new ArgumentOutOfRangeException( "count" );
            }

            char[] chars = new char[ ( count * 2 ) < CharsChunkSize ? ( count * 2 ) : CharsChunkSize ];
            int endIndex = index + count;
            while ( index < endIndex ) {
                int cnt = ( count < CharsChunkSize/2 ) ? count : CharsChunkSize/2;
                int charCount = Encode( buffer, index, cnt, chars );
                writer.WriteRaw( chars, 0, charCount );
                index += cnt;
                count -= cnt;
            }
        }

        internal static string Encode(byte[] inArray, int offsetIn, int count) {
            if (null == inArray) {
                throw new ArgumentNullException("inArray");
            }
            if (0 > offsetIn) {
                throw new ArgumentOutOfRangeException("offsetIn");
            }
            if (0 > count) {
                throw new ArgumentOutOfRangeException("count");
            }
            if (count > inArray.Length - offsetIn) {
                throw new ArgumentOutOfRangeException("count");
            }

            char[] outArray = new char[2 * count];
            int lenOut =  Encode(inArray, offsetIn, count, outArray);
            return new String(outArray, 0, lenOut);
        }

        private static int Encode(byte[] inArray, int offsetIn, int count, char[] outArray) {
            int curOffsetOut =0, offsetOut = 0;
            byte b;
            int lengthOut = outArray.Length;

            for (int j=0; j<count; j++) {
                b = inArray[offsetIn ++];
                outArray[curOffsetOut ++] = s_hexDigits[b >> 4];
                if (curOffsetOut == lengthOut) {
                    break;
                }
                outArray[curOffsetOut ++] = s_hexDigits[b & 0xF];
                if (curOffsetOut == lengthOut) {
                    break;
                }
            }
            return curOffsetOut - offsetOut;
        } // function

    } // class
} // namespace
