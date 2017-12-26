//------------------------------------------------------------------------------
// <copyright file="FastEncoder.cs" company="Microsoft">
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

namespace System.IO.Compression {
    using System;
    using System.Diagnostics;
    using System.Globalization;

    internal class FastEncoder {
        // have we output "bfinal=1"?
        //private bool marked_final_block;

        private bool hasBlockHeader;    // did we output the block header
        private bool hasGzipHeader;     // did we output the gzip header
        private bool usingGzip;         // are we generating gzip stream
        private uint gzipCrc32;         // CRC for gzip stream
        private uint inputStreamSize;   // stream size for gzip stream

        private FastEncoderWindow inputWindow; // input history window
        private DeflateInput inputBuffer;
        private Output output;         
        private Match currentMatch;     // current match in history window
        private bool needsEOB;          // indicates whether or not we have compressed any data     

        public FastEncoder(bool doGZip) {
            usingGzip = doGZip;
            inputWindow = new FastEncoderWindow();
            inputBuffer = new DeflateInput();
            output = new Output();
            currentMatch = new Match();
        }

        //
        // This function sets the input we need to compress
        // Note we don't make a copy of the buffer for performance reason,
        // so the caller need to be make sure the input is not reused if we didn't finish
        // compressing the input.
        //
        public void SetInput(byte[] input, int startIndex, int count) {
            Debug.Assert(inputBuffer.Count == 0, "We have something left in previous input!"); 
            inputBuffer.Buffer = input;
            inputBuffer.Count = count;
            inputBuffer.StartIndex = startIndex;
        }

        // Returns true if we finished compressing the bytes in history window and input buffer
        public bool NeedsInput() {
            return inputBuffer.Count == 0 && inputWindow.BytesAvailable == 0 ;
        }

        //
        // Copy the compressed byte to outputBuffer
        // Returns the bytes we have copied. The caller needs to provide the buffer 
        // to avoid extra coping.
        //
        public int GetCompressedOutput( byte[] outputBuffer) {
            Debug.Assert(!NeedsInput(), "call SetInput before trying to compress!"); 

            output.UpdateBuffer(outputBuffer);
            if (usingGzip && !hasGzipHeader) {
                // Write the GZIP header only once
                output.WriteGzipHeader(3);
                hasGzipHeader = true;
            }

            if (!hasBlockHeader) {
                // Output dynamic block header only once
                hasBlockHeader = true;
                output.WritePreamble();
            }

            do {
                // read more input data into the window if there is space available
                int bytesToCopy = (inputBuffer.Count < inputWindow.FreeWindowSpace) ? 
                                         inputBuffer.Count : inputWindow.FreeWindowSpace;
                if (bytesToCopy > 0) {
                    // copy data into history window
                    inputWindow.CopyBytes(inputBuffer.Buffer, inputBuffer.StartIndex, bytesToCopy);

                    if( usingGzip) {
                        // update CRC for gzip stream
                        gzipCrc32 = DecodeHelper.UpdateCrc32(gzipCrc32, inputBuffer.Buffer, inputBuffer.StartIndex, bytesToCopy);
                        
                        uint n = inputStreamSize + (uint)bytesToCopy;                        
                        if( n < inputStreamSize) {  // overflow, gzip doesn't support compressing more than Int32.Maxvalue bytes.
                            throw new InvalidDataException(SR.GetString(SR.StreamSizeOverflow));                            
                        }
                        inputStreamSize = n;
                    }

                    inputBuffer.ConsumeBytes(bytesToCopy);
                }

                // compress the bytes in input history window 
                while( inputWindow.BytesAvailable > 0 && output.SafeToWriteTo()) {
                    // Find next match. A match can be a symbol, 
                    // a distance/length pair, a symbol followed by a distance/Length pair

                    inputWindow.GetNextSymbolOrMatch(currentMatch);
                    
                    if( currentMatch.State == MatchState.HasSymbol ) {
                        output.WriteChar(currentMatch.Symbol);
                    }
                    else if( currentMatch.State == MatchState.HasMatch) {
                        output.WriteMatch(currentMatch.Length, currentMatch.Position);
                    }
                    else {
                        output.WriteChar(currentMatch.Symbol);
                        output.WriteMatch(currentMatch.Length, currentMatch.Position);
                    }
                } 
            } while (output.SafeToWriteTo() && !NeedsInput());

            // update book keeping needed to write end of block data
            needsEOB = true;

            return output.BytesWritten; // number of bytes we have written
        }

        //
        // Finish the compression. To simply this function, it should only be 
        // called when all input are compressed. The left bits and gzip footer will 
        // be copied to  the outputBuffer.
        // 
        public int Finish(byte[] outputBuffer) {
            Debug.Assert(NeedsInput(), "only call this when we have no more data to compress!");

            output.UpdateBuffer(outputBuffer);
            
            // write EOB data iff we have written anything so far
            if (needsEOB) {
                // The fast encoder outputs one long block, so it just needs to terminate this block
                const int EndOfBlockCode = 256;
                uint code_info = FastEncoderStatics.FastEncoderLiteralCodeInfo[EndOfBlockCode];
                int code_len = (int) (code_info & 31); 
                output.WriteBits(code_len, code_info >> 5);
                output.FlushBits();

                if (usingGzip) {
                    output.WriteGzipFooter(gzipCrc32, inputStreamSize);
                }
            }

            return output.BytesWritten;
        }

        internal class Output {
            private byte[] outputBuf;      // output buffer 
            private int    outputPos;      // output position
            private uint   bitBuf;         // store uncomplete bits 
            private int    bitCount;       // number of bits in bitBuffer 

            //static private byte[] lengthLookup;
            static private byte[] distLookup;

            static Output() {
                //lengthLookup = new byte[512];
                distLookup = new byte[512];

                GenerateSlotTables();
            }

            // Generate the global slot tables which allow us to convert a distance
            // (0..32K) to a distance slot (0..29) 
            //
            // Distance table
            //   Extra           Extra               Extra
            // Code Bits Dist  Code Bits   Dist     Code Bits Distance
            // ---- ---- ----  ---- ----  ------    ---- ---- --------
            //   0   0    1     10   4     33-48    20    9   1025-1536
            //   1   0    2     11   4     49-64    21    9   1537-2048
            //   2   0    3     12   5     65-96    22   10   2049-3072
            //   3   0    4     13   5     97-128   23   10   3073-4096
            //   4   1   5,6    14   6    129-192   24   11   4097-6144
            //   5   1   7,8    15   6    193-256   25   11   6145-8192
            //   6   2   9-12   16   7    257-384   26   12  8193-12288
            //   7   2  13-16   17   7    385-512   27   12 12289-16384
            //   8   3  17-24   18   8    513-768   28   13 16385-24576
            //   9   3  25-32   19   8   769-1024   29   13 24577-32768

            static internal void GenerateSlotTables() {
                // Initialize the mapping length (0..255) -> length code (0..28)
                //int length = 0;
                //for (code = 0; code < FastEncoderStatics.NumLengthBaseCodes-1; code++) {
                //    for (int n = 0; n < (1 << FastEncoderStatics.ExtraLengthBits[code]); n++)
                //        lengthLookup[length++] = (byte) code;
                //}
                //lengthLookup[length-1] = (byte) code;

                // Initialize the mapping dist (0..32K) -> dist code (0..29)
                int dist = 0;
                int code;
                for (code = 0 ; code < 16; code++) {
                    for (int n = 0; n < (1 << FastEncoderStatics.ExtraDistanceBits[code]); n++)
                        distLookup[dist++] = (byte) code;
                }

                dist >>= 7; // from now on, all distances are divided by 128 

                for ( ; code < FastEncoderStatics.NumDistBaseCodes; code++) {
                    for (int n = 0; n < (1 << (FastEncoderStatics.ExtraDistanceBits[code]-7)); n++)
                        distLookup[256 + dist++] = (byte) code;
                }
            }        
            
            // set the output buffer we will be using
            internal void UpdateBuffer( byte[] output) {
                outputBuf = output;
                outputPos = 0;
            }

            internal bool SafeToWriteTo() {  // can we safely continue writing to output buffer
                return outputBuf.Length -outputPos > 16;
            }            

            internal int BytesWritten {
                get {
                    return outputPos;
                }
            }

            internal int FreeBytes {
                get {
                    return outputBuf.Length - outputPos;
                }
            }

            // Output the block type and tree structure for our hard-coded trees.
            // Contains following data:
            //  "final" block flag 1 bit
            //  BLOCKTYPE_DYNAMIC 2 bits
            //  FastEncoderLiteralTreeLength
            //  FastEncoderDistanceTreeLength
            //
            internal void WritePreamble() {
                Debug.Assert( bitCount == 0, "bitCount must be zero before writing tree bit!");
                Debug.Assert( FreeBytes >= FastEncoderStatics.FastEncoderTreeStructureData.Length, "Not enough space in output buffer!"); 
                Array.Copy(FastEncoderStatics.FastEncoderTreeStructureData, 0, outputBuf, outputPos, FastEncoderStatics.FastEncoderTreeStructureData.Length);
                outputPos += FastEncoderStatics.FastEncoderTreeStructureData.Length;

                const uint FastEncoderPostTreeBitBuf = 0x0022;
                const int FastEncoderPostTreeBitCount = 9;
                bitCount = FastEncoderPostTreeBitCount; 
                bitBuf= FastEncoderPostTreeBitBuf;
            }

            internal void WriteMatch(int matchLen, int matchPos) {
                Debug.Assert(matchLen >= FastEncoderWindow.MinMatch && matchLen <= FastEncoderWindow.MaxMatch, "Illegal currentMatch length!"); 
                Debug.WriteLineIf(CompressionTracingSwitch.Verbose, String.Format(CultureInfo.InvariantCulture, "Match: {0}:{1}", matchLen, matchPos), "Compression");

                // Get the code information for a match code
                uint codeInfo = FastEncoderStatics.FastEncoderLiteralCodeInfo[(FastEncoderStatics.NumChars + 1 - FastEncoderWindow.MinMatch) + matchLen]; 
                int codeLen = (int)codeInfo & 31; 
                Debug.Assert(codeLen != 0, "Invalid Match Length!"); 
                if (codeLen <= 16) {
                    WriteBits(codeLen, codeInfo >> 5); 
                } else {
                    WriteBits(16, (codeInfo >> 5) & 65535); 
                    WriteBits(codeLen - 16,  codeInfo >> (5 + 16));
                } 

                // Get the code information for a distance code
                codeInfo = FastEncoderStatics.FastEncoderDistanceCodeInfo[GetSlot(matchPos)]; 
                WriteBits((int)(codeInfo & 15), codeInfo >> 8); 
                int extraBits = (int)(codeInfo >> 4) & 15; 
                if (extraBits != 0) {
                    WriteBits(extraBits, (uint)matchPos & FastEncoderStatics.BitMask[extraBits]); 
                }
            }

            // write gzip footer
            internal void WriteGzipFooter(uint gzipCrc32, uint inputStreamSize) {
                Debug.Assert(FreeBytes >= 8, "No enough space in output buffer!");
                outputBuf[outputPos++] = (byte)(gzipCrc32 & 255);
                outputBuf[outputPos++] = (byte)((gzipCrc32 >> 8) & 255);
                outputBuf[outputPos++] = (byte)((gzipCrc32 >> 16) & 255);
                outputBuf[outputPos++] = (byte)((gzipCrc32 >>24) & 255);

                outputBuf[outputPos++] = (byte)(inputStreamSize & 255);
                outputBuf[outputPos++] = (byte)((inputStreamSize >> 8) & 255);
                outputBuf[outputPos++] = (byte)((inputStreamSize >> 16) & 255);
                outputBuf[outputPos++] = (byte)((inputStreamSize >>24) & 255);
            }

            // write gzip header
            internal void WriteGzipHeader( int compression_level) {
                // only need 11 bytes
                Debug.Assert(FreeBytes >= 16, "No enough space in output buffer!");
                Debug.Assert(outputPos == 0 , "GZIP header must be at the begining of output!");
                outputBuf[outputPos++] =  0x1F; // ID1
                outputBuf[outputPos++] =  0x8B; // ID2
                outputBuf[outputPos++] =  8; // CM = deflate
                outputBuf[outputPos++] =  0; // FLG, no text, no crc, no extra, no name, no comment

                outputBuf[outputPos++] =  0; // MTIME (Modification Time) - no time available
                outputBuf[outputPos++] =  0;
                outputBuf[outputPos++] =  0;
                outputBuf[outputPos++] =  0;

                // XFL
                // 2 = compressor used max compression, slowest algorithm
                // 4 = compressor used fastest algorithm
                if (compression_level == 10)
                    outputBuf[outputPos++] = 2;
                else
                    outputBuf[outputPos++] = 4; 

                outputBuf[outputPos++] = 0; // OS: 0 = FAT filesystem (MS-DOS, OS/2, NT/Win32)
            }

            internal void WriteChar(byte b) {
                Debug.WriteLineIf(CompressionTracingSwitch.Verbose, String.Format(CultureInfo.InvariantCulture, "Literal: {0}", b ), "Compression");
                uint code = FastEncoderStatics.FastEncoderLiteralCodeInfo[b];
                WriteBits((int)code & 31, code >> 5);
            }

            internal void WriteBits(int n, uint bits) {
                Debug.Assert( n <= 16, "length must be larger than 16!");
                bitBuf |= bits << bitCount; 
                bitCount += n; 
                if (bitCount >= 16) {
                   Debug.Assert(outputBuf.Length - outputPos >= 2, "No enough space in output buffer!");
                   outputBuf[outputPos++] = unchecked((byte) bitBuf);
                   outputBuf[outputPos++] = unchecked((byte) (bitBuf >> 8)); 
                   bitCount -= 16; 
                   bitBuf >>= 16; 
                } 
            }     
            
            // Return the position slot (0...29) of a match offset (0...32767)
            internal int GetSlot(int pos) {
                return distLookup[((pos) < 256) ? (pos) : (256 + ((pos) >> 7))];
            }

            // write the bits left in the output as bytes
            internal void FlushBits() {
                // flush bits from bit buffer to output buffer
                while (bitCount >= 8) {   
                    outputBuf[outputPos++] = unchecked((byte) bitBuf);
                    bitCount -= 8; 
                    bitBuf >>= 8; 
                }
                
                if( bitCount > 0 ) {
                    outputBuf[outputPos++] = unchecked((byte) bitBuf);
                    bitCount = 0; 
                }
            }
        }
    } 

    internal class DeflateInput {
        private byte[] buffer;
        private int    count;
        private int    startIndex;

        internal byte[] Buffer {
            get {
                return buffer;
            }
            set {
                buffer = value;
            }
        }

        internal int Count {
            get {
                return count;
            }
            set {
                count = value;
            }
        }

        internal int StartIndex {
            get {
                return startIndex;
            }
            set {
                startIndex = value;
            }
        }

        internal void ConsumeBytes(int n) {
            Debug.Assert( n <= count, "Should use more bytes than what we have in the buffer");
            startIndex += n;
            count -= n;
            Debug.Assert(startIndex + count <= buffer.Length, "Input buffer is in invalid state!");
        }
    }


    internal enum MatchState {
        HasSymbol = 1, 
        HasMatch  = 2, 
        HasSymbolAndMatch  = 3 
    }

    // This class represents a match in the history window
    internal class Match {
        MatchState state;
        int pos;
        int  len;        
        byte symbol;

        internal MatchState State {
            get { return state;}
            set { state = value; }
        }

        internal int Position {
            get { return pos;}
            set { pos = value; }
        }

        internal int Length {
            get { return len;}
            set { len = value; }
        }

        internal byte Symbol {
            get { return symbol;}
            set { symbol = value; }

        }
    }

    internal static class FastEncoderStatics {
        // static information for encoding, DO NOT MODIFY

        internal static readonly byte[] FastEncoderTreeStructureData = {
            0xed,0xbd,0x07,0x60,0x1c,0x49,0x96,0x25,0x26,0x2f,0x6d,0xca,
            0x7b,0x7f,0x4a,0xf5,0x4a,0xd7,0xe0,0x74,0xa1,0x08,0x80,0x60,
            0x13,0x24,0xd8,0x90,0x40,0x10,0xec,0xc1,0x88,0xcd,0xe6,0x92,
            0xec,0x1d,0x69,0x47,0x23,0x29,0xab,0x2a,0x81,0xca,0x65,0x56,
            0x65,0x5d,0x66,0x16,0x40,0xcc,0xed,0x9d,0xbc,0xf7,0xde,0x7b,
            0xef,0xbd,0xf7,0xde,0x7b,0xef,0xbd,0xf7,0xba,0x3b,0x9d,0x4e,
            0x27,0xf7,0xdf,0xff,0x3f,0x5c,0x66,0x64,0x01,0x6c,0xf6,0xce,
            0x4a,0xda,0xc9,0x9e,0x21,0x80,0xaa,0xc8,0x1f,0x3f,0x7e,0x7c,
            0x1f,0x3f,
        };

        // Output a currentMatch with length matchLen (>= MIN_MATCH) and displacement matchPos
        //
        // Optimisation: unlike the other encoders, here we have an array of codes for each currentMatch
        // length (not just each currentMatch length slot), complete with all the extra bits filled in, in
        // a single array element.  
        //
        // There are many advantages to doing this:
        //
        // 1. A single array lookup on g_FastEncoderLiteralCodeInfo, instead of separate array lookups
        //    on g_LengthLookup (to get the length slot), g_FastEncoderLiteralTreeLength, 
        //    g_FastEncoderLiteralTreeCode, g_ExtraLengthBits, and g_BitMask
        //
        // 2. The array is an array of ULONGs, so no access penalty, unlike for accessing those USHORT
        //    code arrays in the other encoders (although they could be made into ULONGs with some
        //    modifications to the source).
        //
        // Note, if we could guarantee that codeLen <= 16 always, then we could skip an if statement here.
        //
        // A completely different optimisation is used for the distance codes since, obviously, a table for 
        // all 8192 distances combining their extra bits is not feasible.  The distance codeinfo table is 
        // made up of code[], len[] and # extraBits for this code.
        //
        // The advantages are similar to the above; a ULONG array instead of a USHORT and BYTE array, better
        // cache locality, fewer memory operations.
        //


        // Encoding information for literal and Length.
        // The least 5 significant bits are the length 
        // and the rest is the code bits.

        internal static readonly uint [] FastEncoderLiteralCodeInfo = {
            0x0000d7ee,0x0004d7ee,0x0002d7ee,0x0006d7ee,0x0001d7ee,0x0005d7ee,0x0003d7ee,
            0x0007d7ee,0x000037ee,0x0000c7ec,0x00000126,0x000437ee,0x000237ee,0x000637ee,
            0x000137ee,0x000537ee,0x000337ee,0x000737ee,0x0000b7ee,0x0004b7ee,0x0002b7ee,
            0x0006b7ee,0x0001b7ee,0x0005b7ee,0x0003b7ee,0x0007b7ee,0x000077ee,0x000477ee,
            0x000277ee,0x000677ee,0x000017ed,0x000177ee,0x00000526,0x000577ee,0x000023ea,
            0x0001c7ec,0x000377ee,0x000777ee,0x000217ed,0x000063ea,0x00000b68,0x00000ee9,
            0x00005beb,0x000013ea,0x00000467,0x00001b68,0x00000c67,0x00002ee9,0x00000768,
            0x00001768,0x00000f68,0x00001ee9,0x00001f68,0x00003ee9,0x000053ea,0x000001e9,
            0x000000e8,0x000021e9,0x000011e9,0x000010e8,0x000031e9,0x000033ea,0x000008e8,
            0x0000f7ee,0x0004f7ee,0x000018e8,0x000009e9,0x000004e8,0x000029e9,0x000014e8,
            0x000019e9,0x000073ea,0x0000dbeb,0x00000ce8,0x00003beb,0x0002f7ee,0x000039e9,
            0x00000bea,0x000005e9,0x00004bea,0x000025e9,0x000027ec,0x000015e9,0x000035e9,
            0x00000de9,0x00002bea,0x000127ec,0x0000bbeb,0x0006f7ee,0x0001f7ee,0x0000a7ec,
            0x00007beb,0x0005f7ee,0x0000fbeb,0x0003f7ee,0x0007f7ee,0x00000fee,0x00000326,
            0x00000267,0x00000a67,0x00000667,0x00000726,0x00001ce8,0x000002e8,0x00000e67,
            0x000000a6,0x0001a7ec,0x00002de9,0x000004a6,0x00000167,0x00000967,0x000002a6,
            0x00000567,0x000117ed,0x000006a6,0x000001a6,0x000005a6,0x00000d67,0x000012e8,
            0x00000ae8,0x00001de9,0x00001ae8,0x000007eb,0x000317ed,0x000067ec,0x000097ed,
            0x000297ed,0x00040fee,0x00020fee,0x00060fee,0x00010fee,0x00050fee,0x00030fee,
            0x00070fee,0x00008fee,0x00048fee,0x00028fee,0x00068fee,0x00018fee,0x00058fee,
            0x00038fee,0x00078fee,0x00004fee,0x00044fee,0x00024fee,0x00064fee,0x00014fee,
            0x00054fee,0x00034fee,0x00074fee,0x0000cfee,0x0004cfee,0x0002cfee,0x0006cfee,
            0x0001cfee,0x0005cfee,0x0003cfee,0x0007cfee,0x00002fee,0x00042fee,0x00022fee,
            0x00062fee,0x00012fee,0x00052fee,0x00032fee,0x00072fee,0x0000afee,0x0004afee,
            0x0002afee,0x0006afee,0x0001afee,0x0005afee,0x0003afee,0x0007afee,0x00006fee,
            0x00046fee,0x00026fee,0x00066fee,0x00016fee,0x00056fee,0x00036fee,0x00076fee,
            0x0000efee,0x0004efee,0x0002efee,0x0006efee,0x0001efee,0x0005efee,0x0003efee,
            0x0007efee,0x00001fee,0x00041fee,0x00021fee,0x00061fee,0x00011fee,0x00051fee,
            0x00031fee,0x00071fee,0x00009fee,0x00049fee,0x00029fee,0x00069fee,0x00019fee,
            0x00059fee,0x00039fee,0x00079fee,0x00005fee,0x00045fee,0x00025fee,0x00065fee,
            0x00015fee,0x00055fee,0x00035fee,0x00075fee,0x0000dfee,0x0004dfee,0x0002dfee,
            0x0006dfee,0x0001dfee,0x0005dfee,0x0003dfee,0x0007dfee,0x00003fee,0x00043fee,
            0x00023fee,0x00063fee,0x00013fee,0x00053fee,0x00033fee,0x00073fee,0x0000bfee,
            0x0004bfee,0x0002bfee,0x0006bfee,0x0001bfee,0x0005bfee,0x0003bfee,0x0007bfee,
            0x00007fee,0x00047fee,0x00027fee,0x00067fee,0x00017fee,0x000197ed,0x000397ed,
            0x000057ed,0x00057fee,0x000257ed,0x00037fee,0x000157ed,0x00077fee,0x000357ed,
            0x0000ffee,0x0004ffee,0x0002ffee,0x0006ffee,0x0001ffee,0x00000084,0x00000003,
            0x00000184,0x00000044,0x00000144,0x000000c5,0x000002c5,0x000001c5,0x000003c6,
            0x000007c6,0x00000026,0x00000426,0x000003a7,0x00000ba7,0x000007a7,0x00000fa7,
            0x00000227,0x00000627,0x00000a27,0x00000e27,0x00000068,0x00000868,0x00001068,
            0x00001868,0x00000369,0x00001369,0x00002369,0x00003369,0x000006ea,0x000026ea,
            0x000046ea,0x000066ea,0x000016eb,0x000036eb,0x000056eb,0x000076eb,0x000096eb,
            0x0000b6eb,0x0000d6eb,0x0000f6eb,0x00003dec,0x00007dec,0x0000bdec,0x0000fdec,
            0x00013dec,0x00017dec,0x0001bdec,0x0001fdec,0x00006bed,0x0000ebed,0x00016bed,
            0x0001ebed,0x00026bed,0x0002ebed,0x00036bed,0x0003ebed,0x000003ec,0x000043ec,
            0x000083ec,0x0000c3ec,0x000103ec,0x000143ec,0x000183ec,0x0001c3ec,0x00001bee,
            0x00009bee,0x00011bee,0x00019bee,0x00021bee,0x00029bee,0x00031bee,0x00039bee,
            0x00041bee,0x00049bee,0x00051bee,0x00059bee,0x00061bee,0x00069bee,0x00071bee,
            0x00079bee,0x000167f0,0x000367f0,0x000567f0,0x000767f0,0x000967f0,0x000b67f0,
            0x000d67f0,0x000f67f0,0x001167f0,0x001367f0,0x001567f0,0x001767f0,0x001967f0,
            0x001b67f0,0x001d67f0,0x001f67f0,0x000087ef,0x000187ef,0x000287ef,0x000387ef,
            0x000487ef,0x000587ef,0x000687ef,0x000787ef,0x000887ef,0x000987ef,0x000a87ef,
            0x000b87ef,0x000c87ef,0x000d87ef,0x000e87ef,0x000f87ef,0x0000e7f0,0x0002e7f0,
            0x0004e7f0,0x0006e7f0,0x0008e7f0,0x000ae7f0,0x000ce7f0,0x000ee7f0,0x0010e7f0,
            0x0012e7f0,0x0014e7f0,0x0016e7f0,0x0018e7f0,0x001ae7f0,0x001ce7f0,0x001ee7f0,
            0x0005fff3,0x000dfff3,0x0015fff3,0x001dfff3,0x0025fff3,0x002dfff3,0x0035fff3,
            0x003dfff3,0x0045fff3,0x004dfff3,0x0055fff3,0x005dfff3,0x0065fff3,0x006dfff3,
            0x0075fff3,0x007dfff3,0x0085fff3,0x008dfff3,0x0095fff3,0x009dfff3,0x00a5fff3,
            0x00adfff3,0x00b5fff3,0x00bdfff3,0x00c5fff3,0x00cdfff3,0x00d5fff3,0x00ddfff3,
            0x00e5fff3,0x00edfff3,0x00f5fff3,0x00fdfff3,0x0003fff3,0x000bfff3,0x0013fff3,
            0x001bfff3,0x0023fff3,0x002bfff3,0x0033fff3,0x003bfff3,0x0043fff3,0x004bfff3,
            0x0053fff3,0x005bfff3,0x0063fff3,0x006bfff3,0x0073fff3,0x007bfff3,0x0083fff3,
            0x008bfff3,0x0093fff3,0x009bfff3,0x00a3fff3,0x00abfff3,0x00b3fff3,0x00bbfff3,
            0x00c3fff3,0x00cbfff3,0x00d3fff3,0x00dbfff3,0x00e3fff3,0x00ebfff3,0x00f3fff3,
            0x00fbfff3,0x0007fff3,0x000ffff3,0x0017fff3,0x001ffff3,0x0027fff3,0x002ffff3,
            0x0037fff3,0x003ffff3,0x0047fff3,0x004ffff3,0x0057fff3,0x005ffff3,0x0067fff3,
            0x006ffff3,0x0077fff3,0x007ffff3,0x0087fff3,0x008ffff3,0x0097fff3,0x009ffff3,
            0x00a7fff3,0x00affff3,0x00b7fff3,0x00bffff3,0x00c7fff3,0x00cffff3,0x00d7fff3,
            0x00dffff3,0x00e7fff3,0x00effff3,0x00f7fff3,0x00fffff3,0x0001e7f1,0x0003e7f1,
            0x0005e7f1,0x0007e7f1,0x0009e7f1,0x000be7f1,0x000de7f1,0x000fe7f1,0x0011e7f1,
            0x0013e7f1,0x0015e7f1,0x0017e7f1,0x0019e7f1,0x001be7f1,0x001de7f1,0x001fe7f1,
            0x0021e7f1,0x0023e7f1,0x0025e7f1,0x0027e7f1,0x0029e7f1,0x002be7f1,0x002de7f1,
            0x002fe7f1,0x0031e7f1,0x0033e7f1,0x0035e7f1,0x0037e7f1,0x0039e7f1,0x003be7f1,
            0x003de7f1,0x000047eb,
        };

        internal static readonly uint[] FastEncoderDistanceCodeInfo = {
            0x00000f06,0x0001ff0a,0x0003ff0b,0x0007ff0b,0x0000ff19,0x00003f18,0x0000bf28,
            0x00007f28,0x00001f37,0x00005f37,0x00000d45,0x00002f46,0x00000054,0x00001d55,
            0x00000864,0x00000365,0x00000474,0x00001375,0x00000c84,0x00000284,0x00000a94,
            0x00000694,0x00000ea4,0x000001a4,0x000009b4,0x00000bb5,0x000005c4,0x00001bc5,
            0x000007d5,0x000017d5,0x00000000,0x00000100,
        };

        internal static readonly uint[] BitMask = {0,1,3,7,15,31,63,127,255,511,1023,2047,4095,8191,16383,32767};
        internal static readonly byte[] ExtraLengthBits = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};
        internal static readonly byte[] ExtraDistanceBits = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,0,0};
        internal const int NumChars = 256;
        internal const int NumLengthBaseCodes = 29;
        internal const int NumDistBaseCodes = 30;


    }
}
