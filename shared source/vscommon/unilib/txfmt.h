// TxFmt.h -- Plain Text Format files
//------------------------------------------------------------------------------
// Copyright (c) 1999, Microsoft Corporation, All Rights Reserved
// Information Contained Herein Is Proprietary and Confidential.
//
// Author: Paul Chase Dempsey [paulde]
//------------------------------------------------------------------------------
#pragma once

// these flags correspond to the VSTFF enum used by the environment text editor

enum LTI_FLAGS { // (L)oad (T)ext (I)mage flags

    // format    
    LTIF_SIGNATURE      = 0x00010000,   // use file signature
    LTIF_BYTESWAPPED    = 0x00020000,   // when codepage is CP_UNICODE, it's the byte-swapped form

    // i/o flags
    LTIF_DETECT         = 0x01000000,   // perform extra analysis to detect encoding
    LTIF_HTML           = 0x02000000,   // HTML
    LTIF_XML            = 0x04000000,   // XML
    LTIF_BINARY         = 0x08000000,   // binary
    LTIF_NOUTF8_NOSIG   = 0x10000000,   // turn off detection of UTF8 w/o signature
    LTIF_KEEPANSI       = 0x20000000    // if text appears to be ANSI, don't convert
};

typedef DWORD LTI_INFO;

// Masks for parts of LTI_INFO
// low word:    Windows codepage (see codepage.h for codepage #defines)
//high word:    LTI_FLAGS
#define LTI_CPMASK 0x0000FFFF
#define LTI_FLMASK 0xFFFF0000

// set the codepage part of an LTI_INFO
#define LTI_SETCP(lti, cp) (lti = ((lti & ~LTI_CPMASK)|(DWORD)cp))

// get the codepage part of an LTI_INFO
#define LTI_CP(lti)    (LTI_CPMASK & lti)

// get the flags part of an LTI_INFO
#define LTI_FLAGS(lti) (LTI_FLMASK & lti)

#ifndef CP_INVALID
#define CP_INVALID        0xFFFF // useful invalid codepage value
#endif

//NOTE: Take care to use an UNSIGNED variable for codepage!

// LTIF_HTML implies LTF_DETECT unless codepage is CP_UNICODE or CP_UTF8.

/* PERF HINT: 

  Once you've loaded a file with LTIF_DETECT, LTIF_HTML, or LTIF_XML, turn them off when 
  saving or getting size unless the image has been modified (i.e. the tag might have changed).

  Similarly, if you get the size with LTIF_DETECT, LTIF_HTML, or LTIF_XML, turn them off 
  when saving.

*/

#ifndef PCBYTE
typedef const BYTE * PCBYTE;
#endif

//------------------------------------------------------------------------------
// FreeCharsetResources - Free resources acquired by APIs in this file.
//
// You must call FreeCharsetResources once at app termination if you use any
// of CodepageFromCharsetNameA|W, ScanHTML|XML*, GetImageFormat, LoadTextImage*
// otherwise you may fail to release cached resources.
//
void WINAPI FreeCharsetResources (void);

//------------------------------------------------------------------------------
// CreateTempName - Create a temporary file name
// 
// If pszSrc is NULL, uses the system TEMP path.
//
// Otherwise, creates a temp filename based on the path of pszSrc.
//
// Returns: TRUE on success, FALSE on failure.
//
// On failure, you can call GetLastError to diagnose the cause.
//
BOOL WINAPI CreateTempName (
    PCWSTR pszSrc,
    _Out_z_cap_(MAX_PATH) PWSTR  pszTemp  // assumed to be at least MAX_PATH
    );

//------------------------------------------------------------------------------
// GetImageFormat - detect the type of text image
//
// Info     Detection instructions. 
// cbData   Count of data bytes.
// pData    Image data.
// pInfo    Result of detection.
//
// The flags and codepage in Info/pInfo are used as follows:
//
// <codepage>       Codepage returned if the function cannot otherwise determine the encoding.
//                  You can pass CP_INVALID to distinguish a parsed/detected codepage from the 
//                  default CP_ACP (0).
//
// LTIF_SIGNATURE   Signatures are always checked and override any other processing.
//                  A signature is the Unicode byte-order mark and UTF-8 file signature.
//                  A file beginning with a signature may still not be in that format, so
//                  prepare for errors in conversions.
//
//                  On output, indicates a signature was found.
//
// LTIF_DETECT      Run data analysis.  Required to detect UTF-8 without a signature.
//                  For reliable results, you must make the entire image available in pData.
//
// LTIF_HTML        Scan for HTML META charset tag.  pData must contain all data up to 
//                  the <BODY> tag (this can be a substantial portion of the image).
//
//                  On output, indicates that the no syntax errors were encountered while
//                  parsing HTML. Beware of relying on this bit: because of the extremely lax 
//                  HTML standard, many non-HTML files are valid HTML.
//
// LTIF_XML         Check for XML declaration at start of file.  Requires only
//                  enough data to hold the XML declaration (~150 bytes or so).
//
//                  On output, indicates file began with a valid XML declaration. This is 
//                  a reliable indicator that the file is XML.
//
// LTIF_BINARY      Check for non-ANSI text data. Caution: usually detects Unicode as binary.
//                  if cbData > 250, only the first 250 bytes are examined.
//
// LTIF_BYTESWAPPED Ignored on input. Can be set in output. Meaningful only for CP_UNICODE.
//
// Detection proceeds in the most efficient order: signature, binary, XML, HTML, data analysis.
// The first error-free positive identification wins. If an XML file has a signature,
// LTIF_XML is NOT set in the output. If you want to know if a file is XML, you can call
// ScanXMLEncoding(A|W) to see if it begins with a valid XML declaration.
//
// The XML declaration must appear at the very start of the image. An example is: 
//
//     <?xml version="1.0" encoding="UTF-8"?>
//
// The 'encoding' part of the declaration is optional.  If the encoding is not 
// specified, the XML standard default encoding CP_UTF8 is returned.  No further 
// analysis is perfomed to verify that the file is actually valid UTF-8 (or XML).  
// Be aware that XML is commonly written without a declared encoding, but actually 
// saved in the current system codepage. It appears that most Microsoft XML 
// processors tolerate this non-conforming practice.
//
// To verify that the file is actually UTF-8, you can use the code:
//
//     GetImageFormat (LTIF_DETECT, cbData, pData, &ltiResult);
//     if (CP_UTF8 == LTI_CP(ltiResult))
//         ; // file is valid UTF-8
//
// Or, you can use the UniLib UTF-8 functions directly for speed, precision, and more
// detailed diagnostic information.
//
//
HRESULT WINAPI GetImageFormat (
  _In_                  LTI_INFO     Info,      // detection instructions
  _In_                  DWORD        cbData,
  _Out_bytecap_(cbData) PCBYTE       pData,
  _Inout_               LTI_INFO *   pInfo      // result
  );

//------------------------------------------------------------------------------
// GetImageFormat2 - detect the type of text image
// This replaces GetImageFormat which doesn't honor the LTIF_NOUTF8_NOSIG flag
//
// WARNING: If you change the detection code in this routine please update LoadTextImageFromMemoryAux
HRESULT WINAPI GetImageFormat2 (
  _In_                  LTI_INFO     Info,      // detection instructions
  _In_                  DWORD        cbData,
  _Out_bytecap_(cbData) PCBYTE       pData,
  _Inout_               LTI_INFO *   pInfo      // result
  );

//------------------------------------------------------------------------------
// Simple signatures-only check (no parsing or data analysis)
HRESULT WINAPI GetImageSignature (
  _In_                  DWORD        cbData,
  _Out_bytecap_(cbData) PCBYTE       pData,
  _Inout_               LTI_INFO *   pInfo
  );

//------------------------------------------------------------------------------
// TextImageUnicodeSize - get the exact Unicode size of a plain-text disk image
//
// Does not detect: ignores LTIF_DETECT, LTIF_HTML, LTIF_XML
// lti must have been created by running GetImageSignature or GetImageFormat
//
HRESULT WINAPI TextImageUnicodeSize (
  LTI_INFO    lti,      // format of image
  DWORD       cbData,   // image size (including signature)
  PCBYTE      pData,    // image data (including signature)
  DWORD *     pdwSize   // Unicode size
  );

//------------------------------------------------------------------------------
// GetTextImageSize - Get plain-text disk image size of a Unicode buffer
//
// Get the byte size guaranteed to hold the text when converted from Unicode to 
// the specified/detected file format.
//
// if fExact is true, returns the exact size required. This can be expensive to calculate.
// if fExact is false, returns the minimum size guaranteed to hold the data.
//
HRESULT WINAPI GetTextImageSize (
  LTI_INFO      lti,            // [in]   Format, detection instructions
  BOOL          fExact,         // [in]   Exact (TRUE) or minimum (FALSE) to calculate
  DWORD         cch,            // [in]   Count of characters in pwch
  _In_count_(cch) PCWSTR        pwch,           // [in]   Unicode data
  _Out_ LTI_INFO *    pInfo,          // [out]  Format used
  _Out_ DWORD *       pcb             // [out]  Count of bytes for image
  );

//------------------------------------------------------------------------------
// TextImageToUnicode - Converts a plaint-text file image to a Unicode buffer
//
// Does not detect: ignores LTIF_DETECT, LTIF_HTML, LTIF_XML
// lti must have been created by running GetImageSignature or GetImageFormat
//
HRESULT WINAPI TextImageToUnicode (
  LTI_INFO    lti,        // [in]  File format
  BOOL        fCheckSize, // [in]  Check that cch is large enough?
  DWORD       cbData,     // [in]  Size of text data (including signature)
  PCBYTE      pData,      // [in]  Text data (including signature)
  DWORD       cch,        // [in]  Size of Unicode buffer
  _Out_cap_(cch) PWSTR       pwch        // [out] Unicode buffer
  );

//------------------------------------------------------------------------------
// LoadTextFile - Load a plain-text file into a BSTR
// See GetImageFormat for information on how lti is set/returned.
//
HRESULT WINAPI LoadTextFile (
  LPCOLESTR     pszFilename,    // file name
  LTI_INFO      lti,            // Instructions for format, detection
  BSTR *        pbstr,          // [out] text of file
  LTI_INFO *    plti = NULL     // [out] format used
  );

HRESULT WINAPI LoadTextFileFromHandle (
  HANDLE        hFile,          // file handle
  LTI_INFO      lti,            // Instructions for format, detection
  BSTR *        pbstr,          // [out] text of file
  LTI_INFO *    plti = NULL     // [out] format used
  );


//------------------------------------------------------------------------------
// LoadBasicTextFile  - Load a basic plain-text file into a BSTR
//
// Use only for files known to be simple text files (NOT HTML/XML).
// Checks only file signatures to diagnose format, similar to Notepad.
// Does NOT recognize XML or HTML or detect UTF-8 or other codepages.
//
HRESULT WINAPI LoadBasicTextFile (
  LPCOLESTR     pszFilename, // [in ] filename
  BSTR *        pbstr        // [out] text of file's contents
  );

