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
#include "stdafx.h"


#include "ipcmanagerinterface.h"
#include "ex.h"




//-----------------------------------------------------------------------------
// Helper to get the proper decorated name
// Caller ensures that pBufSize is large enough. We'll assert just to check,
// but no runtime failure.
// pBuf - the output buffer to write the decorated name in
// cBufSizeInChars - the size of the buffer in characters, including the null.
// pPrefx - The undecorated name of the event.
//-----------------------------------------------------------------------------
void GetPidDecoratedName(__out_z __out_ecount(cBufSizeInChars) WCHAR * pBuf, int cBufSizeInChars, const WCHAR * pPrefix, DWORD pid)
{
    const WCHAR szGlobal[] = L"Global\\";
    int szGlobalLen;
    szGlobalLen = NumItems(szGlobal) - 1;

    // Caller should always give us a big enough buffer.
    _ASSERTE(cBufSizeInChars > (int) wcslen(pPrefix) + szGlobalLen);


    int ret;
    ret = _snwprintf_s(pBuf, cBufSizeInChars, _TRUNCATE, pPrefix, pid);

    // Since this is all determined at compile time, we know we should have enough buffer.
    _ASSERTE (ret != STRUNCATE);
}

HRESULT FindNativeInfoInILVariableArray(DWORD dwIndex,
                                        SIZE_T ip,
                                        ICorJitInfo::NativeVarInfo **ppNativeInfo,
                                        unsigned int nativeInfoCount,
                                        ICorJitInfo::NativeVarInfo *nativeInfo)
{
    // A few words about this search: it must be linear, and the
    // comparison of startOffset and endOffset to ip must be
    // <=/>. startOffset points to the first instruction that will
    // make the variable's home valid. endOffset points to the first
    // instruction at which the variable's home invalid.
    int lastGoodOne = -1;
    for (unsigned int i = 0; i < nativeInfoCount; i++)
    {
        if (nativeInfo[i].varNumber == dwIndex)
        {
            if ( (lastGoodOne == -1) ||
                 (nativeInfo[lastGoodOne].startOffset < nativeInfo[i].startOffset) )
            {
                lastGoodOne = i;
            }

            if ((nativeInfo[i].startOffset <= ip) &&
                (nativeInfo[i].endOffset > ip))
            {
                *ppNativeInfo = &nativeInfo[i];

                return S_OK;
            }
        }
    }

    // Hmmm... didn't find it. Was the endOffset of the last range for
    // this variable equal to the current IP? If so, go ahead and
    // report that as the variable's home for now.
    //
    // The rational here is that by being on the first instruction
    // after the last range a variable was alive, we're essentially
    // assuming that since that instruction hasn't been executed yet,
    // and since there isn't a new home for the variable, that the
    // last home is still good. This actually turns out to be true
    // 99.9% of the time, so we'll go with it for now.
    //
    // -- Thu Sep 23 15:38:27 1999

    if ((lastGoodOne > -1) && (nativeInfo[lastGoodOne].endOffset == ip))
    {
        *ppNativeInfo = &nativeInfo[lastGoodOne];
        return S_OK;
    }

    return CORDBG_E_IL_VAR_NOT_AVAILABLE;
}

// The 'internal' version of our IL to Native map (the DebuggerILToNativeMap struct)
// has an extra field - ICorDebugInfo::SourceTypes source.  The 'external/user-visible'
// version (COR_DEBUG_IL_TO_NATIVE_MAP) lacks that field, so we need to translate our
// internal version to the external version.
// "Export" seemed more succinct than "CopyInternalToExternalILToNativeMap" :)
void ExportILToNativeMap(ULONG32 cMap,             // [in] Min size of mapExt, mapInt
             COR_DEBUG_IL_TO_NATIVE_MAP mapExt[],  // [in] Filled in here
             struct DebuggerILToNativeMap mapInt[],// [in] Source of info
             SIZE_T sizeOfCode)                    // [in] Total size of method (bytes)
{
    ULONG32 iMap;
    _ASSERTE(mapExt != NULL);
    _ASSERTE(mapInt != NULL);

    for(iMap=0; iMap < cMap; iMap++)
    {
        mapExt[iMap].ilOffset = mapInt[iMap].ilOffset ;
        mapExt[iMap].nativeStartOffset = mapInt[iMap].nativeStartOffset ;
        mapExt[iMap].nativeEndOffset = mapInt[iMap].nativeEndOffset ;

        // An element that has an end offset of zero, means "till the end of
        // the method".  Pretty this up so that customers don't have to care about
        // this.
        if ((DWORD)mapInt[iMap].source & (DWORD)ICorDebugInfo::NATIVE_END_OFFSET_UNKNOWN)
        {
            mapExt[iMap].nativeEndOffset = (ULONG32)sizeOfCode;
        }

#if defined(_DEBUG)
        {
            // UnsafeGetConfigDWORD
            SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
            static int fReturnSourceTypeForTesting = -1;
            if (fReturnSourceTypeForTesting == -1)
                fReturnSourceTypeForTesting = REGUTIL::GetConfigDWORD(L"ReturnSourceTypeForTesting", 0);

            if (fReturnSourceTypeForTesting)
            {
                // Steal the most significant four bits from the native end offset for the source type.
                _ASSERTE( (mapExt[iMap].nativeEndOffset >> 28) == 0x0 );
                _ASSERTE( (ULONG32)(mapInt[iMap].source) < 0xF );
                mapExt[iMap].nativeEndOffset |= ((ULONG32)(mapInt[iMap].source) << 28);
            }
        }
#endif // _DEBUG
    }
}



