//------------------------------------------------------------------------------
// <copyright file="CharEntitiesEncodingFallback.cs" company="Microsoft">
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

using System.Text;
using System.Diagnostics;
using System.Globalization;

namespace System.Xml {

//
// CharEntityEncoderFallback
//

    internal class CharEntityEncoderFallback : EncoderFallback {
        private CharEntityEncoderFallbackBuffer fallbackBuffer;

        private int[]   textContentMarks;
        private int     endMarkPos;
        private int     curMarkPos;
        private int     startOffset;

        internal CharEntityEncoderFallback() {
        }

        public override EncoderFallbackBuffer CreateFallbackBuffer() {
            if ( fallbackBuffer == null ) { 
                fallbackBuffer = new CharEntityEncoderFallbackBuffer( this );
            }
            return fallbackBuffer;
        }
 
        public override int MaxCharCount {
            get {
                return 12;
            }
        }

        internal int StartOffset {
            get {
                return startOffset;
            }
            set {
                startOffset = value;
            }
        }

        internal void Reset( int[] textContentMarks, int endMarkPos ) {
            this.textContentMarks = textContentMarks;
            this.endMarkPos = endMarkPos;
            curMarkPos = 0;
        }

        internal bool CanReplaceAt( int index ) {
            int mPos = curMarkPos;
            int charPos = startOffset + index;
            while ( mPos < endMarkPos && charPos >= textContentMarks[mPos+1] ) {
                mPos++;
            }
            curMarkPos = mPos;

            return (mPos & 1) != 0;
        }
    }
 
//
// CharEntityFallbackBuffer
//
    internal class CharEntityEncoderFallbackBuffer : EncoderFallbackBuffer {
        private CharEntityEncoderFallback parent;

        private string  charEntity = string.Empty;
        private int     charEntityIndex = -1;

        internal CharEntityEncoderFallbackBuffer( CharEntityEncoderFallback parent ) {
            this.parent = parent;
        }
 
        public override bool Fallback( char charUnknown, int index ) {
            // If we are already in fallback, throw, it's probably at the suspect character in charEntity
            if ( charEntityIndex >= 0 ) {
                (new EncoderExceptionFallbackBuffer()).Fallback( charUnknown, index );
            }
 
            // find out if we can replace the character with entity
            if ( parent.CanReplaceAt( index ) ) {
                // Create the replacement character entity
                charEntity = string.Format( CultureInfo.InvariantCulture, "&#x{0:X};", new object[] { (int)charUnknown } );
                charEntityIndex = 0;
                return true;
            }
            else {
                EncoderFallbackBuffer errorFallbackBuffer = ( new EncoderExceptionFallback() ).CreateFallbackBuffer();
                errorFallbackBuffer.Fallback( charUnknown, index );
                return false;
            }
        }
 
        public override bool Fallback( char charUnknownHigh, char charUnknownLow, int index ) {
            // check input surrogate pair
            if ( !char.IsSurrogatePair( charUnknownHigh, charUnknownLow ) ) {
                throw XmlConvert.CreateInvalidSurrogatePairException( charUnknownHigh, charUnknownLow );
            }

            // If we are already in fallback, throw, it's probably at the suspect character in charEntity
            if ( charEntityIndex >= 0 ) {
                (new EncoderExceptionFallbackBuffer()).Fallback( charUnknownHigh, charUnknownLow, index );
            }
 
            if ( parent.CanReplaceAt( index ) ) {
                // Create the replacement character entity
                charEntity = string.Format( CultureInfo.InvariantCulture, "&#x{0:X};", new object[] { char.ConvertToUtf32( charUnknownHigh, charUnknownLow ) } );
                charEntityIndex = 0;
                return true;
            }
            else {
                EncoderFallbackBuffer errorFallbackBuffer = ( new EncoderExceptionFallback() ).CreateFallbackBuffer();
                errorFallbackBuffer.Fallback( charUnknownHigh, charUnknownLow, index );
                return false;
            }
        }
 
        public override char GetNextChar() {
            if ( charEntityIndex == -1 ) {
                return (char)0;
            }
            else {
                Debug.Assert( charEntityIndex < charEntity.Length );
                char ch = charEntity[charEntityIndex++];
                if ( charEntityIndex == charEntity.Length ) {
                    charEntityIndex = -1;
                }
                return ch;
            }
        }
  
        public override bool MovePrevious() {
            if ( charEntityIndex == -1 ) {
                return false;
            }
            else {
                Debug.Assert( charEntityIndex < charEntity.Length );
                if ( charEntityIndex > 0 ) {
                    charEntityIndex--;
                    return true;
                }
                else {
                    return false;
                }
            }
        }
          

        public override int Remaining {
            get {
                if ( charEntityIndex == -1 ) {
                    return 0;
                }
                else {
                    return charEntity.Length - charEntityIndex;
                }
            }
        }
 
        public override void Reset() {
            charEntityIndex = -1;
        }
    }
}
