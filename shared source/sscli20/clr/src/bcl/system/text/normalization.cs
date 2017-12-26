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
namespace System.Text
{
    using System;
    using System.Globalization;
    using System.Text;
    using System.Runtime.CompilerServices;

    // This is the enumeration for Normalization Forms
[System.Runtime.InteropServices.ComVisible(true)]
    public enum NormalizationForm
    {
        FormC    = 1,
        FormD    = 2,
        FormKC   = 5,
        FormKD   = 6
    }

    internal enum ExtendedNormalizationForms
    {
        FormC    = 1,
        FormD    = 2,
        FormKC   = 5,
        FormKD   = 6,
        FormIdna = 0xd,
        FormCDisallowUnassigned     = 0x101,
        FormDDisallowUnassigned     = 0x102,
        FormKCDisallowUnassigned    = 0x105,
        FormKDDisallowUnassigned    = 0x106,
        FormIdnaDisallowUnassigned  = 0x10d
    }

    // This internal class wraps up our normalization behavior

    internal class Normalization
    {
        private static Normalization NFC;
        private static Normalization NFD;
        private static Normalization NFKC;
        private static Normalization NFKD;
        private static Normalization IDNA;
        private static Normalization NFCDisallowUnassigned;
        private static Normalization NFDDisallowUnassigned;
        private static Normalization NFKCDisallowUnassigned;
        private static Normalization NFKDDisallowUnassigned;
        private static Normalization IDNADisallowUnassigned;

        private NormalizationForm normalizationForm;

        // These are error codes we get back from the Normalization DLL
        private const int ERROR_SUCCESS = 0;
        private const int ERROR_NOT_ENOUGH_MEMORY = 8;
        private const int ERROR_INVALID_PARAMETER = 87;
        private const int ERROR_INSUFFICIENT_BUFFER = 122;
        private const int ERROR_NO_UNICODE_TRANSLATION = 1113;

        internal unsafe Normalization(NormalizationForm form, String strDataFile)
        {
            // Remember which form we are
            this.normalizationForm = form;
            // Load the DLL
            if (!nativeLoadNormalizationDLL())
            {
                // Unable to load the normalization DLL!
                throw new ArgumentException(
                    Environment.GetResourceString("Argument_InvalidNormalizationForm"));
            }

            // Tell the DLL where to find our data
            byte* pTables = GlobalizationAssembly.GetGlobalizationResourceBytePtr(
                typeof(Normalization).Assembly, strDataFile);
            if (pTables == null)
            {
                // Unable to load the specified normalizationForm,
                // tables not loaded from file
                throw new ArgumentException(
                    Environment.GetResourceString("Argument_InvalidNormalizationForm"));
            }

            // All we have to do is let the .dll know how to load it, then
            // we can ignore the returned pointer.
            byte* objNorm = nativeNormalizationInitNormalization(form, pTables);
            if (objNorm == null)
            {
                // Unable to load the specified normalizationForm
                // native library class not initialized correctly
                throw new OutOfMemoryException(
                    Environment.GetResourceString("Arg_OutOfMemoryException"));
            }
        }

        static internal Normalization GetNormalization(NormalizationForm form)
        {
            switch ((ExtendedNormalizationForms)form)
            {
                case ExtendedNormalizationForms.FormC:
                    return GetFormC();
                case ExtendedNormalizationForms.FormD:
                    return GetFormD();
                case ExtendedNormalizationForms.FormKC:
                    return GetFormKC();
                case ExtendedNormalizationForms.FormKD:
                    return GetFormKD();
                case ExtendedNormalizationForms.FormIdna:
                    return GetFormIDNA();
                case ExtendedNormalizationForms.FormCDisallowUnassigned:
                    return GetFormCDisallowUnassigned();
                case ExtendedNormalizationForms.FormDDisallowUnassigned:
                    return GetFormDDisallowUnassigned();
                case ExtendedNormalizationForms.FormKCDisallowUnassigned:
                    return GetFormKCDisallowUnassigned();
                case ExtendedNormalizationForms.FormKDDisallowUnassigned:
                    return GetFormKDDisallowUnassigned();
                case ExtendedNormalizationForms.FormIdnaDisallowUnassigned:
                    return GetFormIDNADisallowUnassigned();
            }

            // They were supposed to have a form that we know about!
            throw new ArgumentException(
                Environment.GetResourceString("Argument_InvalidNormalizationForm"));
        }

        static internal Normalization GetFormC()
        {
            if (NFC != null)
                return NFC;

            NFC = new Normalization(NormalizationForm.FormC, "normnfc.nlp");
            return NFC;
        }

        static internal Normalization GetFormD()
        {
            if (NFD != null)
                return NFD;

            NFD = new Normalization(NormalizationForm.FormD, "normnfd.nlp");
            return NFD;
        }

        static internal Normalization GetFormKC()
        {
            if (NFKC != null)
                return NFKC;

            NFKC = new Normalization(NormalizationForm.FormKC, "normnfkc.nlp");
            return NFKC;
        }

        static internal Normalization GetFormKD()
        {
            if (NFKD != null)
                return NFKD;

            NFKD = new Normalization(NormalizationForm.FormKD, "normnfkd.nlp");
            return NFKD;
        }

        static internal Normalization GetFormIDNA()
        {
            if (IDNA != null)
                return IDNA;

            IDNA = new Normalization((NormalizationForm)ExtendedNormalizationForms.FormIdna, "normidna.nlp");
            return IDNA;
        }

        static internal Normalization GetFormCDisallowUnassigned()
        {
            if (NFCDisallowUnassigned != null)
                return NFCDisallowUnassigned;

            NFCDisallowUnassigned = new Normalization(
                (NormalizationForm)ExtendedNormalizationForms.FormCDisallowUnassigned, "normnfc.nlp");
            return NFCDisallowUnassigned;
        }

        static internal Normalization GetFormDDisallowUnassigned()
        {
            if (NFDDisallowUnassigned != null)
                return NFDDisallowUnassigned;

            NFDDisallowUnassigned = new Normalization(
                (NormalizationForm)ExtendedNormalizationForms.FormDDisallowUnassigned, "normnfd.nlp");
            return NFDDisallowUnassigned;
        }

        static internal Normalization GetFormKCDisallowUnassigned()
        {
            if (NFKCDisallowUnassigned != null)
                return NFKCDisallowUnassigned;

            NFKCDisallowUnassigned = new Normalization(
                (NormalizationForm)ExtendedNormalizationForms.FormKCDisallowUnassigned, "normnfkc.nlp");
            return NFKCDisallowUnassigned;
        }

        static internal Normalization GetFormKDDisallowUnassigned()
        {
            if (NFKDDisallowUnassigned != null)
                return NFKDDisallowUnassigned;

            NFKDDisallowUnassigned = new Normalization(
                (NormalizationForm)ExtendedNormalizationForms.FormKDDisallowUnassigned, "normnfkd.nlp");
            return NFKDDisallowUnassigned;
        }

        static internal Normalization GetFormIDNADisallowUnassigned()
        {
            if (IDNADisallowUnassigned!= null)
                return IDNADisallowUnassigned;

            IDNADisallowUnassigned = new Normalization(
                (NormalizationForm)ExtendedNormalizationForms.FormIdnaDisallowUnassigned, "normidna.nlp");
            return IDNADisallowUnassigned;
        }

        internal static bool IsNormalized(String strInput, NormalizationForm normForm)
        {
            return GetNormalization(normForm).IsNormalized(strInput);
        }

        private bool IsNormalized(String strInput)
        {
            if (strInput == null)
                throw new ArgumentNullException(
                    Environment.GetResourceString("ArgumentNull_String"), "strInput");

            int iError = ERROR_SUCCESS;
            int iTest = nativeNormalizationIsNormalizedString(
                normalizationForm, ref iError, strInput, strInput.Length);

            switch(iError)
            {
                // Success doesn't need to do anything
                case ERROR_SUCCESS:
                    break;

                // Do appropriate stuff for the individual errors:
                // Only possible value here is ERROR_NO_UNICODE_TRANSLATION
                case ERROR_NO_UNICODE_TRANSLATION:
                    throw new ArgumentException(
                        Environment.GetResourceString("Argument_InvalidCharSequenceNoIndex" ),
                        "strInput");
                case ERROR_NOT_ENOUGH_MEMORY:
                    throw new OutOfMemoryException(
                        Environment.GetResourceString("Arg_OutOfMemoryException"));
                default:
                    throw new InvalidOperationException(
                        Environment.GetResourceString("UnknownError_Num", iError));
            }

            // Bit 1 is true, 0 is false from our return value.
            return ((iTest & 1) == 1);
        }

        internal static String Normalize(String strInput, NormalizationForm normForm)
        {
            return GetNormalization(normForm).Normalize(strInput);
        }

        internal String Normalize(String strInput)
        {
            if (strInput == null)
                throw new ArgumentNullException( "strInput",
                    Environment.GetResourceString("ArgumentNull_String"));

            // Guess our buffer size first
            int iLength = GuessLength(strInput);

            // Don't break for empty strings (only possible for D & KD and not really possible at that)
            if (iLength == 0) return String.Empty;

            // Someplace to stick our buffer
            char[] cBuffer = null;

            int iError = ERROR_INSUFFICIENT_BUFFER;
            while (iError == ERROR_INSUFFICIENT_BUFFER)
            {
                // (re)allocation buffer and normalize string
                cBuffer = new char[iLength];
                iLength = nativeNormalizationNormalizeString(
                    normalizationForm, ref iError,
                    strInput, strInput.Length, cBuffer, cBuffer.Length);

                // Could have an error (actually it'd be quite hard to have an error here)
                if (iError != ERROR_SUCCESS)
                {
                    switch(iError)
                    {
                        // Do appropriate stuff for the individual errors:
                        case ERROR_INSUFFICIENT_BUFFER:
                            BCLDebug.Assert(iLength > cBuffer.Length, "Buffer overflow should have iLength > cBuffer.Length");
                            continue;
                        case ERROR_NO_UNICODE_TRANSLATION:
                            // Illegal code point or order found.  Ie: FFFE or D800 D800, etc.
                            throw new ArgumentException(
                                Environment.GetResourceString("Argument_InvalidCharSequence", iLength ),
                                "strInput");
                        case ERROR_NOT_ENOUGH_MEMORY:
                            throw new OutOfMemoryException(
                                Environment.GetResourceString("Arg_OutOfMemoryException"));
                        case ERROR_INVALID_PARAMETER:
                            // Shouldn't have invalid parameters here unless we have a bug, drop through...
                        default:
                            // We shouldn't get here...
                            throw new InvalidOperationException(
                                Environment.GetResourceString("UnknownError_Num", iError));
                    }
                }
            }

            // Copy our buffer into our new string, which will be the appropriate size
            String strReturn = new String(cBuffer, 0, iLength);

            // Return our output string
            return strReturn;
        }

        internal int GuessLength(String strInput)
        {
            if (strInput == null)
                throw new ArgumentNullException( "strInput",
                    Environment.GetResourceString("ArgumentNull_String"));

            // Get our guess
            int iError = 0;
            int iGuess = nativeNormalizationNormalizeString(
                normalizationForm, ref iError, strInput, strInput.Length, null, 0);

            // Could have an error (actually it'd be quite hard to have an error here)
            BCLDebug.Assert(iError == ERROR_SUCCESS, "GuessLength() shouldn't return errors.");
            if (iError != ERROR_SUCCESS)
            {
                // We shouldn't really be able to get here..., guessing length is
                // a trivial math function...
                // Can't really be Out of Memory, but just in case:
                if (iError == ERROR_NOT_ENOUGH_MEMORY)
                    throw new OutOfMemoryException(
                        Environment.GetResourceString("Arg_OutOfMemoryException"));

                // Who knows what happened?  Not us!
                throw new InvalidOperationException(
                    Environment.GetResourceString("UnknownError_Num", iError));
            }

            // Well, we guessed it
            return iGuess;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        unsafe private static extern bool nativeLoadNormalizationDLL();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        unsafe private static extern int nativeNormalizationNormalizeString(
            NormalizationForm NormForm, ref int iError,
            String lpSrcString, int cwSrcLength,
            char[] lpDstString, int cwDstLength);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        unsafe private static extern int nativeNormalizationIsNormalizedString(
            NormalizationForm NormForm, ref int iError,
            String lpString, int cwLength);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        unsafe private static extern byte* nativeNormalizationInitNormalization(
            NormalizationForm NormForm, byte* pTableData);

    }
}
