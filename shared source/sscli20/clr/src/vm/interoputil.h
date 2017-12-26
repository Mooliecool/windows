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
#ifndef _H_INTEROP_UTIL
#define _H_INTEROP_UTIL

#include "debugmacros.h"

#include "interopconverter.h"

class TypeHandle;
struct VariantData;

// Out of memory helper.
#define IfNullThrow(EXPR) \
do {if ((EXPR) == 0) {ThrowOutOfMemory();} } while (0)


// Helper to determine the version number from an int.
#define GET_VERSION_USHORT_FROM_INT(x) ((x < 0) || (x > (INT)((USHORT)-1))) ? 0 : x

// This is the context flags that are passed to CoCreateInstance. This defined
// should be used throught the runtime in all calls to CoCreateInstance.
#define EE_COCREATE_CLSCTX_FLAGS CLSCTX_SERVER

// The format string to use to format unknown members to be passed to
// invoke member
#define DISPID_NAME_FORMAT_STRING                       L"[DISPID=%i]"




class FieldDesc;
struct ExceptionData;

//--------------------------------------------------------------------------------
// helper for DllCanUnload now
HRESULT STDMETHODCALLTYPE EEDllCanUnloadNow(void);

//------------------------------------------------------------------
 // setup error info for exception object
//
HRESULT SetupErrorInfo(OBJECTREF pThrownObject);

//--------------------------------------------------------------------------------
 // Release helper, enables and disables GC during call-outs
ULONG SafeRelease(IUnknown* pUnk, RCW* pRCW = NULL);

//--------------------------------------------------------------------------------
// Determines if a COM object can be cast to the specified type.
BOOL CanCastComObject(OBJECTREF obj, TypeHandle hndType);

//---------------------------------------------------------
// Read the BestFit custom attribute info from 
// both assembly level and interface level
//---------------------------------------------------------
VOID ReadBestFitCustomAttribute(MethodDesc* pMD, BOOL* BestFit, BOOL* ThrowOnUnmappableChar);
VOID ReadBestFitCustomAttribute(IMDInternalImport* pInternalImport, mdTypeDef cl, BOOL* BestFit, BOOL* ThrowOnUnmappableChar);
int  InternalWideToAnsi(__in_ecount(iNumWideChars) LPCWSTR szWideString, int iNumWideChars, __out_ecount_opt(cbAnsiBufferSize) LPSTR szAnsiString, int cbAnsiBufferSize, BOOL fBestFit, BOOL fThrowOnUnmappableChar);

//-------------------------------------------------------------------
 // Called from DLLMain, to initialize com specific data structures.
//-------------------------------------------------------------------
void FillExceptionData(ExceptionData* pedata, IErrorInfo* pErrInfo);

//------------------------------------------------------------------------------
 // helper to access fields from an object
INT64 FieldAccessor(FieldDesc* pFD, OBJECTREF oref, INT64 val, BOOL isGetter, U1 cbSize);

//---------------------------------------------------------------------------
// Returns the index of the LCID parameter if one exists and -1 otherwise.
int GetLCIDParameterIndex(IMDInternalImport *pInternalImport, mdMethodDef md);

//---------------------------------------------------------------------------
// Transforms an LCID into a CultureInfo.
void GetCultureInfoForLCID(LCID lcid, OBJECTREF *pCultureObj);

//---------------------------------------------------------------------------
// This method determines if a member is visible from COM.
BOOL IsMemberVisibleFromCom(IMDInternalImport *pInternalImport, mdToken tk, mdMethodDef mdAssociate);

//--------------------------------------------------------------------------------
// This method generates a stringized version of an interface that contains the
// name of the interface along with the signature of all the methods.
SIZE_T GetStringizedItfDef(TypeHandle InterfaceType, CQuickArray<BYTE> &rDef);

//--------------------------------------------------------------------------------
// Helper to get the stringized form of typelib guid.
HRESULT GetStringizedTypeLibGuidForAssembly(Assembly *pAssembly, CQuickArray<BYTE> &rDef, ULONG cbCur, ULONG *pcbFetched);

//--------------------------------------------------------------------------------
// GetErrorInfo helper, enables and disables GC during call-outs
HRESULT SafeGetErrorInfo(IErrorInfo **ppIErrInfo);

//--------------------------------------------------------------------------------
// QI helper, enables and disables GC during call-outs
HRESULT SafeQueryInterface(IUnknown* pUnk, REFIID riid, IUnknown** pResUnk);

// Convert an IUnknown to CCW, returns NULL if the pUnk is not on
// a managed tear-off (OR) if the pUnk is to a managed tear-off that
// has been aggregated
ComCallWrapper* GetCCWFromIUnknown(IUnknown* pUnk);


inline HRESULT EnsureComStartedNoThrow()
{
    LEAF_CONTRACT;
    
    return S_OK;
}

inline VOID EnsureComStarted()
{
    LEAF_CONTRACT;
}
    
__inline VOID LogInteropRelease(IUnknown* pUnk, ULONG cbRef, __in_z LPSTR szMsg)
{
    LEAF_CONTRACT;
}


IUnknown* MarshalObjectToInterface(OBJECTREF* ppObject, MethodTable* pItfMT, MethodTable* pClassMT, DWORD dwFlags);
void UnmarshalObjectFromInterface(OBJECTREF* ppObjectDest, IUnknown* pUnkSrc, MethodTable* pItfMT, MethodTable* pClassMT, DWORD dwFlags);

#endif
