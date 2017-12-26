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
using System;
using System.Threading;
using System.Globalization;

namespace System.Text
{
    [Serializable]
    public abstract class DecoderFallback
    {
        internal bool                  bIsMicrosoftBestFitFallback = false;

        private static DecoderFallback replacementFallback; // Default fallback, uses no best fit & "?"
        private static DecoderFallback exceptionFallback;

        // Private object for locking instead of locking on a internal type for SQL reliability work.
        private static Object s_InternalSyncObject;
        private static Object InternalSyncObject
        {
            get
            {
                if (s_InternalSyncObject == null)
                {
                    Object o = new Object();
                    Interlocked.CompareExchange(ref s_InternalSyncObject, o, null);
                }
                return s_InternalSyncObject;
            }
        }

        // Get each of our generic fallbacks.

        public static DecoderFallback ReplacementFallback
        {
            get
            {
                if (replacementFallback == null)
                    lock(InternalSyncObject)
                        if (replacementFallback == null)
                            replacementFallback = new DecoderReplacementFallback();

                return replacementFallback;
            }
        }


        public static DecoderFallback ExceptionFallback
        {
            get
            {
                if (exceptionFallback == null)
                    lock(InternalSyncObject)
                        if (exceptionFallback == null)
                            exceptionFallback = new DecoderExceptionFallback();

                return exceptionFallback;
            }
        }

        // Fallback
        //
        // Return the appropriate unicode string alternative to the character that need to fall back.
        // Most implimentations will be:
        //      return new MyCustomDecoderFallbackBuffer(this);

        public abstract DecoderFallbackBuffer CreateFallbackBuffer();

        // Maximum number of characters that this instance of this fallback could return

        public abstract int MaxCharCount { get; }

        internal bool IsMicrosoftBestFitFallback
        {
            get
            {
                return bIsMicrosoftBestFitFallback;
            }
        }
    }


    public abstract class DecoderFallbackBuffer
    {


        public abstract bool Fallback(byte[] bytesUnknown, int index);

        // Get next character

        public abstract char GetNextChar();

        // Back up a character

        public abstract bool MovePrevious();

        // How many chars left in this fallback?

        public abstract int Remaining { get; }

        // Clear the buffer

        public virtual void Reset()
        {
            while (GetNextChar() != (char)0);
        }

        // Internal items to help us figure out what we're doing as far as error messages, etc.
        // These help us with our performance and messages internally
        internal     unsafe byte*    byteStart = null;
        internal     unsafe char*    charEnd = null;

        // Internal Reset
        internal unsafe void InternalReset()
        {
            byteStart = null;
            Reset();
        }

        // Set the above values
        // This can't be part of the constructor because DecoderFallbacks would have to know how to impliment these.
        internal unsafe void InternalInitialize(byte* byteStart, char* charEnd)
        {
            this.byteStart = byteStart;
            this.charEnd = charEnd;
        }

        internal unsafe virtual bool InternalFallback(byte[] bytes, byte* pBytes, ref char* chars)
        {
            // Copy bytes to array (slow, but right now that's what we get to do.
          //  byte[] bytesUnknown = new byte[count];
//            for (int i = 0; i < count; i++)
//                bytesUnknown[i] = *(bytes++);

            BCLDebug.Assert(byteStart != null, "[DecoderFallback.InternalFallback]Used InternalFallback without calling InternalInitialize");

            // See if there's a fallback character and we have an output buffer then copy our string.
            if (this.Fallback(bytes, (int)(pBytes - byteStart - bytes.Length)))
            {
                // Copy the chars to our output
                char ch;
                char* charTemp = chars;
                bool bHighSurrogate = false;
                while ((ch = GetNextChar()) != 0)
                {
                    // Make sure no mixed up surrogates
                    if (Char.IsSurrogate(ch))
                    {
                        if (Char.IsHighSurrogate(ch))
                        {
                            // High Surrogate
                            if (bHighSurrogate)
                                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidCharSequenceNoIndex"));
                            bHighSurrogate = true;
                        }
                        else
                        {
                            // Low surrogate
                            if (bHighSurrogate == false)
                                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidCharSequenceNoIndex"));
                            bHighSurrogate = false;
                        }
                    }

                    if (charTemp >= charEnd)
                    {
                        // No buffer space
                        return false;
                    }

                    *(charTemp++) = ch;
                }

                // Need to make sure that bHighSurrogate isn't true
                if (bHighSurrogate)
                    throw new ArgumentException(Environment.GetResourceString("Argument_InvalidCharSequenceNoIndex"));

                // Now we aren't going to be false, so its OK to update chars
                chars = charTemp;
            }

            return true;
        }

        // This version just counts the fallback and doesn't actually copy anything.
        internal unsafe virtual int InternalFallback(byte[] bytes, byte* pBytes)
        {
            // Copy bytes to array (slow, but right now that's what we get to do.
//            byte[] bytesUnknown = new byte[count];
//            for (int i = 0; i < count; i++)
  //              bytesUnknown[i] = *(bytes++);

            BCLDebug.Assert(byteStart != null, "[DecoderFallback.InternalFallback]Used InternalFallback without calling InternalInitialize");

            // See if there's a fallback character and we have an output buffer then copy our string.
            if (this.Fallback(bytes, (int)(pBytes - byteStart - bytes.Length)))
            {
                int count = 0;

                char ch;
                bool bHighSurrogate = false;
                while ((ch = GetNextChar()) != 0)
                {
                    // Make sure no mixed up surrogates
                    if (Char.IsSurrogate(ch))
                    {
                        if (Char.IsHighSurrogate(ch))
                        {
                            // High Surrogate
                            if (bHighSurrogate)
                                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidCharSequenceNoIndex"));
                            bHighSurrogate = true;
                        }
                        else
                        {
                            // Low surrogate
                            if (bHighSurrogate == false)
                                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidCharSequenceNoIndex"));
                            bHighSurrogate = false;
                        }
                    }

                    count++;
                }

                // Need to make sure that bHighSurrogate isn't true
                if (bHighSurrogate)
                    throw new ArgumentException(Environment.GetResourceString("Argument_InvalidCharSequenceNoIndex"));

                return count;
            }

            // If no fallback return 0
            return 0;
        }

        // private helper methods
        internal void ThrowLastBytesRecursive(byte[] bytesUnknown)
        {
            // Create a string representation of our bytes.
            StringBuilder strBytes = new StringBuilder(bytesUnknown.Length * 3);
            int i;
            for (i = 0; i < bytesUnknown.Length && i < 20; i++)
            {
                if (strBytes.Length > 0)
                    strBytes.Append(" ");
                strBytes.Append(String.Format(CultureInfo.InvariantCulture, "\\x{0:X2}", bytesUnknown[i]));
            }
            // In case the string's really long
            if (i == 20)
                strBytes.Append(" ...");

            // Throw it, using our complete bytes
            throw new ArgumentException(
                Environment.GetResourceString("Argument_RecursiveFallbackBytes",
                    strBytes.ToString()), "bytesUnknown");
        }

    }
}
