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
#include "common.h"
#include "vars.hpp"
#include "excep.h"
#include "interoputil.h"
#include "cachelinealloc.h"
#include "comutilnative.h"
#include "field.h"
#include "guidfromname.h"
#include "comvariant.h"
#include "eeconfig.h"
#include "mlinfo.h"
#include "remoting.h"
#include "appdomain.hpp"
#include "comreflectioncache.hpp"
#include "prettyprintsig.h"
#include "util.hpp"
#include "interopconverter.h"
#include "wrappers.h"
#include "invokeutil.h"
#include "mdaassistantsptr.h"
#include "comcallablewrapper.h"
#include "../md/compiler/custattr.h"
#include "marshaler.h"
#include "siginfo.hpp"
#include "eemessagebox.h"


//-------------------------------------------------------------------
// HRESULT STDMETHODCALLTYPE EEDllCanUnloadNow(void)
// DllCanUnloadNow delegates the call here
//-------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE EEDllCanUnloadNow(void)
{
    LEAF_CONTRACT;
    
    //we should never unload unless the process is dying
    return S_FALSE;
}

//------------------------------------------------------------------
// setup error info for exception object
//
HRESULT SetupErrorInfo(OBJECTREF pThrownObject)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);      // disabled due to ordering of this FS:0 handler and the C++ FS:0 handler
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    HRESULT hr = E_FAIL;


    GCPROTECT_BEGIN(pThrownObject)
    {
        EX_TRY
        {
            // Calls to COM up ahead.
            hr = EnsureComStartedNoThrow();
            if (SUCCEEDED(hr) && pThrownObject != NULL)
            {
#ifdef _DEBUG            
                EX_TRY
                {
                    StackSString message;
                    GetExceptionMessage(pThrownObject, message);

                    if (g_pConfig->ShouldExposeExceptionsInCOMToConsole())
                    {
                        PrintToStdOutW(L".NET exception in COM\n");
                        if (!message.IsEmpty()) 
                            PrintToStdOutW(message.GetUnicode());
                        else
                            PrintToStdOutW(L"No exception info available");
                    }

                    if (g_pConfig->ShouldExposeExceptionsInCOMToMsgBox())
                    {
                        GCX_PREEMP();
                        if (!message.IsEmpty()) 
                            EEMessageBoxNonLocalizedDebugOnly((LPWSTR)message.GetUnicode(), L".NET exception in COM", MB_ICONSTOP | MB_OK);
                        else
                            EEMessageBoxNonLocalizedDebugOnly(L"No exception information available", L".NET exception in COM",MB_ICONSTOP | MB_OK);
                    }
                }
                EX_CATCH
                {
                }
                EX_END_CATCH (SwallowAllExceptions);
#endif

                IErrorInfo* pErr = NULL;
                ICreateErrorInfo* pCErr = NULL;
                ExceptionData ED;
                ZeroMemory(&ED, sizeof(ED));

                EX_TRY
                {
                    ExceptionNative::GetExceptionData(pThrownObject, &ED);

                    hr = ED.hr;

                    if (FAILED(CreateErrorInfo(&pCErr)))
                        goto lFail;
                    if (FAILED(pCErr->SetDescription(ED.bstrDescription)))
                        goto lFail;
                    if (FAILED(pCErr->SetSource(ED.bstrSource)))
                        goto lFail;
                    if (FAILED(pCErr->SetHelpFile(ED.bstrHelpFile)))
                        goto lFail;
                    if (FAILED(pCErr->SetHelpContext(ED.dwHelpContext)))
                        goto lFail;
                    if (FAILED(SafeQueryInterface(pCErr, IID_IErrorInfo, (IUnknown **)&pErr)))
                        goto lFail;

                    {
                        GCX_PREEMP();
                        LeaveRuntimeHolder lrh((size_t)SetErrorInfo);
                        SetErrorInfo(0, pErr);
                    }

lFail: ;
                }
                EX_CATCH
                {
                }
                EX_END_CATCH(SwallowAllExceptions);

                if (SUCCEEDED(hr))
                    hr = E_FAIL;

                if (pErr)
                    pErr->Release();
                if (pCErr)
                    pCErr->Release();
                
                FreeExceptionData(&ED); // free the BStrs            
            }
        }
        EX_CATCH
        {
            if (SUCCEEDED(hr))
                hr = E_FAIL;
        }
        EX_END_CATCH(SwallowAllExceptions);
    }
    GCPROTECT_END();
    return hr;
}

//-------------------------------------------------------------------
// Called from DLLMain, to initialize com specific data structures.
//-------------------------------------------------------------------
void FillExceptionData(ExceptionData* pedata, IErrorInfo* pErrInfo)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pedata));
    }
    CONTRACTL_END;
    
    if (pErrInfo != NULL)
    {
        Thread* pThread = GetThread();
        if (pThread != NULL)
        {
            GCX_PREEMP();
            
            pErrInfo->GetSource (&pedata->bstrSource);
            pErrInfo->GetDescription (&pedata->bstrDescription);
            pErrInfo->GetHelpFile (&pedata->bstrHelpFile);
            pErrInfo->GetHelpContext (&pedata->dwHelpContext );
            pErrInfo->GetGUID(&pedata->guid);
            
            ULONG cbRef = SafeRelease(pErrInfo); // release the IErrorInfo interface pointer
            LogInteropRelease(pErrInfo, cbRef, "IErrorInfo");
        }
    }
}



//---------------------------------------------------------------------------
// Returns the index of the LCID parameter if one exists and -1 otherwise.
int GetLCIDParameterIndex(IMDInternalImport *pInternalImport, mdMethodDef md)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pInternalImport));
    }
    CONTRACTL_END;
    
    int             iLCIDParam = -1;
    HRESULT         hr;
    const BYTE *    pVal;
    ULONG           cbVal;

    // Check to see if the method has the LCIDConversionAttribute.
    hr = pInternalImport->GetCustomAttributeByName(md, INTEROP_LCIDCONVERSION_TYPE, (const void**)&pVal, &cbVal);
    if (hr == S_OK)
    {
        CustomAttributeParser caLCID(pVal, cbVal);
        CaArg args[1];
        args[0].Init(SERIALIZATION_TYPE_I4, 0);
        IfFailGo(ParseKnownCaArgs(caLCID, args, lengthof(args)));
        iLCIDParam = args[0].val.i4;
    }

ErrExit:
    return iLCIDParam;
}

//---------------------------------------------------------------------------
// Transforms an LCID into a CultureInfo.
void GetCultureInfoForLCID(LCID lcid, OBJECTREF *pCultureObj)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pCultureObj));
    }
    CONTRACTL_END;

    static MethodTable* s_pCultureInfoMT = NULL;

    // Retrieve the CultureInfo type handle.
    if (s_pCultureInfoMT == NULL)
        s_pCultureInfoMT = g_Mscorlib.GetClass(CLASS__CULTURE_INFO);

    OBJECTREF CultureObj = NULL;
    GCPROTECT_BEGIN(CultureObj)
    {
        // Allocate a CultureInfo with the specified LCID.
        CultureObj = AllocateObject(s_pCultureInfoMT);

        MethodDescCallSite cultureInfoCtor(METHOD__CULTURE_INFO__INT_CTOR, &CultureObj);

        // Call the CultureInfo(int culture) constructor.
        ARG_SLOT pNewArgs[] = {
            ObjToArgSlot(CultureObj),
            (ARG_SLOT)lcid
        };
        cultureInfoCtor.Call(pNewArgs);

        // Set the returned culture object.
        *pCultureObj = CultureObj;
    }
    GCPROTECT_END();
}

//---------------------------------------------------------------------------
// This method determines if a member is visible from COM.
BOOL IsMemberVisibleFromCom(IMDInternalImport *pInternalImport, mdToken tk, mdMethodDef mdAssociate)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pInternalImport));
    }
    CONTRACTL_END;
    
    HRESULT                 hr;
    const BYTE *            pVal;
    ULONG                   cbVal;
    DWORD                   dwFlags;

    // Check to see if the member is public.
    switch (TypeFromToken(tk))
    {
        case mdtFieldDef:
            _ASSERTE(IsNilToken(mdAssociate));
            dwFlags = pInternalImport->GetFieldDefProps(tk);
            if (!IsFdPublic(dwFlags))
                return FALSE;
            break;

        case mdtMethodDef:
            _ASSERTE(IsNilToken(mdAssociate));
            dwFlags = pInternalImport->GetMethodDefProps(tk);
            if (!IsMdPublic(dwFlags))
                return FALSE;
            {
                // Generic Methods are not visible from COM
                MDEnumHolder hEnumTyPars(pInternalImport);
                if (FAILED(pInternalImport->EnumInit(mdtGenericParam, tk, &hEnumTyPars)))
                    return FALSE;

                if (pInternalImport->EnumGetCount(&hEnumTyPars) != 0)
                    return FALSE;
            }
            break;

        case mdtProperty:
            _ASSERTE(!IsNilToken(mdAssociate));
            dwFlags = pInternalImport->GetMethodDefProps(mdAssociate);
            if (!IsMdPublic(dwFlags))
                    return FALSE;

            // Check to see if the associate has the ComVisible attribute set.
            hr = pInternalImport->GetCustomAttributeByName(mdAssociate, INTEROP_COMVISIBLE_TYPE, (const void**)&pVal, &cbVal);
            if (hr == S_OK)
            {
                _ASSERTE("The ComVisible custom attribute is invalid" && cbVal);
                return (BOOL)*(pVal + 2);
            }
            break;

        default:
            _ASSERTE(!"The type of the specified member is not handled by IsMemberVisibleFromCom");
            break;
    }

    // Check to see if the member has the ComVisible attribute set.
    hr = pInternalImport->GetCustomAttributeByName(tk, INTEROP_COMVISIBLE_TYPE, (const void**)&pVal, &cbVal);
    if (hr == S_OK)
    {
        _ASSERTE("The ComVisible custom attribute is invalid" && cbVal);
        return (BOOL)*(pVal + 2);
    }

    // The member is visible.
    return TRUE;
}


ULONG GetStringizedMethodDef(IMDInternalImport *pMDImport, mdToken tkMb, CQuickArray<BYTE> &rDef, ULONG cbCur)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMDImport));
    }
    CONTRACTL_END;
    
    CQuickBytes     rSig;
    MDEnumHolder    ePm(pMDImport);         // For enumerating  params.
    mdParamDef      tkPm;                   // A param token.
    DWORD           dwFlags;                // Param flags.
    USHORT          usSeq;                  // Sequence of a parameter.
    ULONG           cPm;                    // Count of params.
    PCCOR_SIGNATURE pSig;
    ULONG           cbSig;

    // Don't count invisible members.
    if (!IsMemberVisibleFromCom(pMDImport, tkMb, mdMethodDefNil))
        return cbCur;
    
    // accumulate the signatures.
    pSig = pMDImport->GetSigOfMethodDef(tkMb, &cbSig);
    IfFailThrow(::PrettyPrintSigInternal(pSig, cbSig, "", &rSig, pMDImport));
    
    // Get the parameter flags.
    IfFailThrow(pMDImport->EnumInit(mdtParamDef, tkMb, &ePm));
    cPm = pMDImport->EnumGetCount(&ePm);
    
    // Resize for sig and params.  Just use 1 byte of param.
    rDef.ReSizeThrows(cbCur + rSig.Size() + cPm);
    memcpy(rDef.Ptr() + cbCur, rSig.Ptr(), rSig.Size());
    cbCur += (ULONG)(rSig.Size()-1);
    
    // Enumerate through the params and get the flags.
    while (pMDImport->EnumNext(&ePm, &tkPm))
    {
        pMDImport->GetParamDefProps(tkPm, &usSeq, &dwFlags);
        if (usSeq == 0)     // Skip return type flags.
            continue;
        rDef[cbCur++] = (BYTE)dwFlags;
    }

    // Return the number of bytes.
    return cbCur;
} // void GetStringizedMethodDef()


ULONG GetStringizedFieldDef(IMDInternalImport *pMDImport, mdToken tkMb, CQuickArray<BYTE> &rDef, ULONG cbCur)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMDImport));
    }
    CONTRACTL_END;

    CQuickBytes         rSig;
    PCCOR_SIGNATURE     pSig;
    ULONG               cbSig;

    // Don't count invisible members.
    if (!IsMemberVisibleFromCom(pMDImport, tkMb, mdMethodDefNil))
        return cbCur;
    
    // accumulate the signatures.
    pSig = pMDImport->GetSigOfFieldDef(tkMb, &cbSig);
    IfFailThrow(::PrettyPrintSigInternal(pSig, cbSig, "", &rSig, pMDImport));
    rDef.ReSizeThrows(cbCur + rSig.Size());
    memcpy(rDef.Ptr() + cbCur, rSig.Ptr(), rSig.Size());
    cbCur += (ULONG)(rSig.Size()-1);

    // Return the number of bytes.
    return cbCur;
} // void GetStringizedFieldDef()

//--------------------------------------------------------------------------------
// This method generates a stringized version of an interface that contains the
// name of the interface along with the signature of all the methods.
SIZE_T GetStringizedItfDef(TypeHandle InterfaceType, CQuickArray<BYTE> &rDef)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    MethodTable* pIntfMT = InterfaceType.GetMethodTable();
    PREFIX_ASSUME(pIntfMT != NULL);

    IMDInternalImport* pMDImport = pIntfMT->GetMDImport();
    PREFIX_ASSUME(pMDImport != NULL);
    
    LPCWSTR             szName;                 
    ULONG               cchName;
    MDEnumHolder        eMb(pMDImport);                         // For enumerating methods and fields.
    mdToken             tkMb;                                   // A method or field token.
    SIZE_T              cbCur;

    // Make sure the specified type is an interface with a valid token.
    _ASSERTE(!IsNilToken(pIntfMT->GetCl()) && pIntfMT->IsInterface());

    // Get the name of the class.
    DefineFullyQualifiedNameForClassW();
    szName = GetFullyQualifiedNameForClassNestedAwareW(pIntfMT->GetClass());

    cchName = (ULONG)wcslen(szName);

    // Start with the interface name.
    cbCur = cchName * sizeof(WCHAR);
    rDef.ReSizeThrows(cbCur + sizeof(WCHAR));
    wcscpy_s(reinterpret_cast<LPWSTR>(rDef.Ptr()), rDef.Size()/sizeof(WCHAR), szName);

    // Enumerate the methods...
    IfFailThrow(pMDImport->EnumInit(mdtMethodDef, pIntfMT->GetCl(), &eMb));
    while(pMDImport->EnumNext(&eMb, &tkMb))
    {   // accumulate the signatures.
        cbCur = GetStringizedMethodDef(pMDImport, tkMb, rDef, (ULONG)cbCur);
    }
    pMDImport->EnumClose(&eMb);

    // Enumerate the fields...
    IfFailThrow(pMDImport->EnumInit(mdtFieldDef, pIntfMT->GetCl(), &eMb));
    while(pMDImport->EnumNext(&eMb, &tkMb))
    {   // accumulate the signatures.
        cbCur = GetStringizedFieldDef(pMDImport, tkMb, rDef, (ULONG)cbCur);
    }

    // Return the number of bytes.
    return cbCur;
} // ULONG GetStringizedItfDef()

//--------------------------------------------------------------------------------
// Helper to get the stringized form of typelib guid.
HRESULT GetStringizedTypeLibGuidForAssembly(Assembly *pAssembly, CQuickArray<BYTE> &rDef, ULONG cbCur, ULONG *pcbFetched)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS; 
        MODE_ANY;
        PRECONDITION(CheckPointer(pAssembly));
        PRECONDITION(CheckPointer(pcbFetched));        
    }
    CONTRACTL_END;

    HRESULT     hr = S_OK;              // A result.
    LPCUTF8     pszName = NULL;         // Library name in UTF8.
    ULONG       cbName;                 // Length of name, UTF8 characters.
    LPWSTR      pName;                  // Pointer to library name.
    ULONG       cchName;                // Length of name, wide chars.
    LPWSTR      pch=0;                  // Pointer into lib name.
    const void  *pSN=NULL;              // Pointer to public key.
    DWORD       cbSN=0;                 // Size of public key.
    USHORT      usMajorVersion;         // The major version number.
    USHORT      usMinorVersion;         // The minor version number.
    USHORT      usBuildNumber;          // The build number.
    USHORT      usRevisionNumber;       // The revision number.
    const BYTE  *pbData;                // Pointer to a custom attribute data.
    ULONG       cbData;                 // Size of custom attribute data.
    static char szTypeLibKeyName[] = {"TypeLib"};
 
    // Get the name, and determine its length.
    pszName = pAssembly->GetSimpleName();
    cbName=(ULONG)strlen(pszName);
    cchName = WszMultiByteToWideChar(CP_ACP,0, pszName,cbName+1, 0,0);
    
    // See if there is a public key.
    pSN = pAssembly->GetPublicKey(&cbSN);

    // If the ComCompatibleVersionAttribute is set, then use the version
    // number in the attribute when generating the GUID.
    IfFailGo(pAssembly->GetManifestImport()->GetCustomAttributeByName(TokenFromRid(1, mdtAssembly), INTEROP_COMCOMPATIBLEVERSION_TYPE, (const void**)&pbData, &cbData));
    if (hr == S_OK && cbData >= (2 + 4 * sizeof(INT32)))
    {
        // Assert that the metadata blob is valid and of the right format.
        _ASSERTE("TypeLibVersion custom attribute does not have the right format" && (*pbData == 0x01) && (*(pbData + 1) == 0x00));

        // Skip the header describing the type of custom attribute blob.
        pbData += 2;
        cbData -= 2;

        // Retrieve the major and minor version from the attribute.
        usMajorVersion = GET_VERSION_USHORT_FROM_INT(GET_UNALIGNED_32(pbData));
        usMinorVersion = GET_VERSION_USHORT_FROM_INT(GET_UNALIGNED_32( ((INT32*)pbData) + 1));
        usBuildNumber = GET_VERSION_USHORT_FROM_INT(GET_UNALIGNED_32( ((INT32*)pbData) + 2));
        usRevisionNumber = GET_VERSION_USHORT_FROM_INT(GET_UNALIGNED_32( ((INT32*)pbData) + 3));
    }
    else
    {
        pAssembly->GetVersion(&usMajorVersion, &usMinorVersion, &usBuildNumber, &usRevisionNumber);
    }

    // Get the version information.
    struct  versioninfo
    {
        USHORT      usMajorVersion;         // Major Version.   
        USHORT      usMinorVersion;         // Minor Version.
        USHORT      usBuildNumber;          // Build Number.
        USHORT      usRevisionNumber;       // Revision Number.
    } ver;

    // There is a bug here in that usMajor is used twice and usMinor not at all.
    //  We're not fixing that because everyone has a major version, so all the
    //  generated guids would change, which is breaking.  To compensate, if 
    //  the minor is non-zero, we add it separately, below.
    ver.usMajorVersion = usMajorVersion;
    ver.usMinorVersion =  usMajorVersion;  // Don't fix this line!
    ver.usBuildNumber =  usBuildNumber;
    ver.usRevisionNumber =  usRevisionNumber;
    
    // Resize the output buffer.
    IfFailGo(rDef.ReSizeNoThrow(cbCur + cchName*sizeof(WCHAR) + sizeof(szTypeLibKeyName)-1 + cbSN + sizeof(ver)+sizeof(USHORT)));
                                                                                                          
    // Put it all together.  Name first.
    WszMultiByteToWideChar(CP_ACP,0, pszName,cbName+1, (LPWSTR)(&rDef[cbCur]),cchName);
    pName = (LPWSTR)(&rDef[cbCur]);
    for (pch=pName; *pch; ++pch)
        if (*pch == '.' || *pch == ' ')
            *pch = '_';
    else
        if (iswupper(*pch))
            *pch = towlower(*pch);
    cbCur += (cchName-1)*sizeof(WCHAR);
    memcpy(&rDef[cbCur], szTypeLibKeyName, sizeof(szTypeLibKeyName)-1);
    cbCur += sizeof(szTypeLibKeyName)-1;
        
    // Version.
    memcpy(&rDef[cbCur], &ver, sizeof(ver));
    cbCur += sizeof(ver);

    // If minor version is non-zero, add it to the hash.  It should have been in the ver struct,
    //  but due to a bug, it was omitted there, and fixing it "right" would have been
    //  breaking.  So if it isn't zero, add it; if it is zero, don't add it.  Any
    //  possible value of minor thus generates a different guid, and a value of 0 still generates
    //  the guid that the original, buggy, code generated.
    if (usMinorVersion != 0)
    {
        SET_UNALIGNED_16(&rDef[cbCur], usMinorVersion);
        cbCur += sizeof(USHORT);
    }

    // Public key.
    memcpy(&rDef[cbCur], pSN, cbSN);
    cbCur += cbSN;

    if (pcbFetched)
        *pcbFetched = cbCur;

ErrExit:
    return hr;
}

ULONG ReleaseTransitionHelper(IUnknown* pUnknown)
{
    WRAPPER_CONTRACT;
    
    ULONG result = 0;
    BEGIN_SO_TOLERANT_CODE(GetThread());
    result = pUnknown->Release();
    END_SO_TOLERANT_CODE;
    return result;
}


//--------------------------------------------------------------------------------
// Release helper, enables and disables GC during call-outs
ULONG SafeReleaseHelper(IUnknown* pUnk, RCW* pRCW)
{    
    CONTRACTL {
        DISABLED(NOTHROW);
        WRAPPER(GC_TRIGGERS);
        MODE_PREEMPTIVE;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pUnk));
    } CONTRACTL_END;

    ULONG res = 0;

    BEGIN_ENTRYPOINT_VOIDRET;
    
    //memsage pump could happen, so arbitrary managed code could run 
    CONTRACT_VIOLATION(ThrowsViolation | FaultViolation);

    BOOL fException = FALSE;
    
    PAL_CPP_TRY
    {
        // This is a holder to tell the contract system that we're catching all exceptions.
        CLR_TRY_MARKER();

        // Its very possible that the punk has gone bad before we could release it. This is a common application
        // error. We may AV trying to call Release, and that AV will show up as an AV in mscorwks, so we'll take
        // down the Runtime. Mark that an AV is alright, and handled, in this scope using this holder.
        AVInRuntimeImplOkayHolder AVOkay(TRUE);

        LeaveRuntimeHolder lrh(*((*(size_t**)pUnk)+2));
        res = ReleaseTransitionHelper(pUnk);
    }
    PAL_CPP_CATCH_ALL
    {
#if defined(STACK_GUARDS_DEBUG)
        // Catching and just swallowing an exception means we need to tell
        // the SO code that it should go back to normal operation, as it
        // currently thinks that the exception is still on the fly.
        GetThread()->GetCurrentStackGuard()->RestoreCurrentGuard();
#endif

        fException = TRUE;
    }
    PAL_CPP_ENDTRY;

    if (fException)
    {
    
    }

    
    END_ENTRYPOINT_VOIDRET;

    return res;
}

ULONG SafeRelease(IUnknown* pUnk, RCW* pRCW)
{   
    WRAPPER_CONTRACT;
    
    if (pUnk == NULL)
        return 0;

    ULONG res = 0;
    GCX_PREEMP();

    res = SafeReleaseHelper(pUnk, pRCW);

    return res;
}

//--------------------------------------------------------------------------------
// Determines if a COM object can be cast to the specified type.
BOOL CanCastComObject(OBJECTREF obj, TypeHandle hndType)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    if (!obj)
        return TRUE;

    if (hndType.GetMethodTable()->IsInterface())
    {
        return Object::SupportsInterface(obj, hndType.GetMethodTable());
    }
    else
    {
        return TypeHandle(obj->GetMethodTable()).CanCastTo(hndType);
    }
}


VOID
ReadBestFitCustomAttribute(MethodDesc* pMD, BOOL* BestFit, BOOL* ThrowOnUnmappableChar)
{
    WRAPPER_CONTRACT;
    
    ReadBestFitCustomAttribute(pMD->GetMDImport(),
        pMD->GetMethodTable()->GetCl(),
        BestFit, ThrowOnUnmappableChar);
}

VOID
ReadBestFitCustomAttribute(IMDInternalImport* pInternalImport, mdTypeDef cl, BOOL* BestFit, BOOL* ThrowOnUnmappableChar)
{
    // Set the attributes to their defaults, just to be safe.
    *BestFit = TRUE;
    *ThrowOnUnmappableChar = FALSE;

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pInternalImport));
    }
    CONTRACTL_END;
    
    HRESULT     hr;
    BYTE*       pData;
    ULONG       cbCount; 

    // A well-formed BestFitMapping attribute will have at least 5 bytes
    // 1,2 for the prolog (should be 0x1, 0x0)
    // 3 for the BestFitMapping bool
    // 4,5 for the number of named parameters (will be 0 if ThrowOnUnmappableChar doesn't exist)
    // 6 - 29 for the description of ThrowOnUnmappableChar
    // 30 for the ThrowOnUnmappableChar bool

    // Try the assembly first
    hr = pInternalImport->GetCustomAttributeByName(TokenFromRid(1, mdtAssembly), INTEROP_BESTFITMAPPING_TYPE, (const VOID**)(&pData), &cbCount);
    if ((hr == S_OK) && (pData) && (cbCount > 4) && (pData[0] == 1) && (pData[1] == 0))
    {
        _ASSERTE((cbCount == 5) || (cbCount == 30));
        
        // index to 2 to skip prolog
        *BestFit = pData[2] != 0;

        // If this parameter exists,
        if (cbCount == 30)
            // index to end of data to skip description of named argument
            *ThrowOnUnmappableChar = pData[29] != 0;
    }

    // Now try the interface/class/struct
    if (IsNilToken(cl))
        return;
    hr = pInternalImport->GetCustomAttributeByName(cl, INTEROP_BESTFITMAPPING_TYPE, (const VOID**)(&pData), &cbCount);
    if ((hr == S_OK) && (pData) && (cbCount > 4) && (pData[0] == 1) && (pData[1] == 0))
    {
        _ASSERTE((cbCount == 5) || (cbCount == 30));

        // index to 2 to skip prolog    
        *BestFit = pData[2] != 0;
        
        // If this parameter exists,
        if (cbCount == 30)
            // index to end of data to skip description of named argument
            *ThrowOnUnmappableChar = pData[29] != 0;
    }
}


int InternalWideToAnsi(__in_ecount(iNumWideChars) LPCWSTR szWideString, int iNumWideChars, __out_ecount_opt(cbAnsiBufferSize) LPSTR szAnsiString, int cbAnsiBufferSize, BOOL fBestFit, BOOL fThrowOnUnmappableChar)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;


    if ((szWideString == 0) || (iNumWideChars == 0) || (szAnsiString == 0) || (cbAnsiBufferSize == 0))
        return 0;

    DWORD flags = 0;
    int retval;

    if (fBestFit == FALSE)
        flags = WC_NO_BEST_FIT_CHARS;

    if (fThrowOnUnmappableChar)
    {
        BOOL DefaultCharUsed = FALSE;
        retval = WszWideCharToMultiByte(CP_ACP,
                                    flags,
                                    szWideString,
                                    iNumWideChars,
                                    szAnsiString,
                                    cbAnsiBufferSize,
                                    NULL,
                                    &DefaultCharUsed);
        DWORD lastError = GetLastError();

        if (retval == 0)
        {
            INSTALL_UNWIND_AND_CONTINUE_HANDLER; 
            COMPlusThrowHR(HRESULT_FROM_WIN32(lastError));
            UNINSTALL_UNWIND_AND_CONTINUE_HANDLER; 
        }

        if (DefaultCharUsed)
        {
            struct HelperThrow
            {
                static void Throw()
                {
                    COMPlusThrow( kArgumentException, IDS_EE_MARSHAL_UNMAPPABLE_CHAR );
                }
            };
            
            ENCLOSE_IN_EXCEPTION_HANDLER( HelperThrow::Throw );
        }

    }
    else
    {
        retval = WszWideCharToMultiByte(CP_ACP,
                                    flags,
                                    szWideString,
                                    iNumWideChars,
                                    szAnsiString,
                                    cbAnsiBufferSize,
                                    NULL,
                                    NULL);
        DWORD lastError = GetLastError();

        if (retval == 0)
        {
            INSTALL_UNWIND_AND_CONTINUE_HANDLER; 
            COMPlusThrowHR(HRESULT_FROM_WIN32(lastError));
            UNINSTALL_UNWIND_AND_CONTINUE_HANDLER; 
        }
    }

    return retval;
}

//--------------------------------------------------------------------------------
// GetErrorInfo helper, enables and disables GC during call-outs
HRESULT SafeGetErrorInfo(IErrorInfo **ppIErrInfo)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(ppIErrInfo));   
    }
    CONTRACTL_END;

    GCX_PREEMP();

    *ppIErrInfo = NULL;
    HRESULT hr = S_OK;
    EX_TRY
    {
        LeaveRuntimeHolder lrh((size_t)GetErrorInfo);
        hr = GetErrorInfo(0, ppIErrInfo);
    }
    EX_CATCH
    {
        hr = E_OUTOFMEMORY;
    }
    EX_END_CATCH(SwallowAllExceptions);
    
    return hr;
}

HRESULT SafeQueryInterfaceHelper(IUnknown* pUnk, REFIID riid, IUnknown** pResUnk)
{
    WRAPPER_CONTRACT;

    HRESULT hr = E_FAIL;
    BEGIN_SO_TOLERANT_CODE(GetThread());
    hr = pUnk->QueryInterface(riid, (void**) pResUnk);
    END_SO_TOLERANT_CODE;
    return hr;
}

//--------------------------------------------------------------------------------
// QI helper, enables and disables GC during call-outs
HRESULT SafeQueryInterface(IUnknown* pUnk, REFIID riid, IUnknown** pResUnk)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pUnk));
        PRECONDITION(CheckPointer(pResUnk));
    }
    CONTRACTL_END;

    CONTRACT_VIOLATION(ThrowsViolation); //memsage pump could happen, so arbitrary managed code could run
    GCX_PREEMP();

    HRESULT hr = E_FAIL;

    *pResUnk = NULL;

    PAL_CPP_TRY
    {
        // This is a holder to tell the contract system that we're catching all exceptions.
        CLR_TRY_MARKER();

        LeaveRuntimeHolder lrh(*((*(size_t**)pUnk)+0));
        hr = SafeQueryInterfaceHelper(pUnk, riid, pResUnk);
    }
    PAL_CPP_CATCH_ALL
    {
#if defined(STACK_GUARDS_DEBUG)
        // Catching and just swallowing an exception means we need to tell
        // the SO code that it should go back to normal operation, as it
        // currently thinks that the exception is still on the fly.
        GetThread()->GetCurrentStackGuard()->RestoreCurrentGuard();
#endif
        // ignore the exception
    }
    PAL_CPP_ENDTRY
    
    LOG((LF_INTEROP, LL_EVERYTHING, hr == S_OK ? "QI Succeeded\n" : "QI Failed\n")); 

    // Ensure if the QI returned ok that it actually set a pointer.
    if (hr == S_OK)
    {
        if (*pResUnk == NULL)
            hr = E_NOINTERFACE;
    }

    return hr;
}


ComCallWrapper* GetCCWFromIUnknown(IUnknown* pUnk)
{
    CONTRACT (ComCallWrapper*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pUnk));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    RETURN ComCallWrapper::GetCCWFromIUnknown(pUnk);
}





IUnknown* MarshalObjectToInterface(OBJECTREF* ppObject, MethodTable* pItfMT, MethodTable* pClassMT, DWORD dwFlags)
{
    CONTRACTL
    {
        THROWS;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
    }
    CONTRACTL_END;
    
    // Convert the ObjectRef to a COM IP.
    SafeComHolder<IUnknown> pUnk;

    pUnk = GetComIPFromObjectRef(ppObject);

    pUnk.SuppressRelease();
    return pUnk;
}

void UnmarshalObjectFromInterface(OBJECTREF* ppObjectDest, IUnknown* pUnkSrc, MethodTable* pItfMT, MethodTable* pClassMT, DWORD dwFlags)
{
    CONTRACTL
    {
        THROWS;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
    }
    CONTRACTL_END;
    
    _ASSERTE(!pClassMT || !pClassMT->IsInterface());

    OBJECTREF managedObj = NULL;
    
    managedObj = GetObjectRefFromComIP(pUnkSrc, pClassMT);
    
    SetObjectReference(ppObjectDest, managedObj, GetAppDomain());

    // Make sure the interface is supported.

    if (pItfMT != NULL && pItfMT->IsInterface())
    {
        if (!(*ppObjectDest)->IsThunking() && !Object::SupportsInterface(*ppObjectDest, pItfMT))
        {
            pClassMT = (*ppObjectDest)->GetMethodTable();
            {
                COMPlusThrowInvalidCastException(TypeHandle(pClassMT), TypeHandle(pItfMT));
            }                        
        }
    }
}

