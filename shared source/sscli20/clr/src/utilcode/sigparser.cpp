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
// sigparser.cpp
//
// Signature parsing code
//
#include "stdafx.h"
#include "sigparser.h"
#include "contract.h"

HRESULT SigParser::SkipExactlyOne()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    CorElementType typ;
    HRESULT hr = GetElemType(&typ);
    
    IfFailRet(hr);
        
    if (!CorIsPrimitiveType((CorElementType)typ))
    {
        switch (typ)
        {
            default:
                // _ASSERT(!"Illegal or unimplement type in COM+ sig.");
                return META_E_BAD_SIGNATURE;
                break;
            case ELEMENT_TYPE_VAR:
            case ELEMENT_TYPE_MVAR:
                IfFailRet(GetData(NULL));      // Skip variable number
                break;
            case ELEMENT_TYPE_OBJECT:
            case ELEMENT_TYPE_STRING:
            case ELEMENT_TYPE_TYPEDBYREF:
                break;

            case ELEMENT_TYPE_BYREF: //fallthru
            case ELEMENT_TYPE_PTR:
            case ELEMENT_TYPE_PINNED:
            case ELEMENT_TYPE_SZARRAY:
                IfFailRet(SkipExactlyOne());              // Skip referenced type
                break;

            case ELEMENT_TYPE_VALUETYPE: //fallthru
            case ELEMENT_TYPE_CLASS:
                IfFailRet(GetToken(NULL));          // Skip RID
                break;

            case ELEMENT_TYPE_MODULE:
                IfFailRet(GetData(NULL));      // Skip index
                IfFailRet(SkipExactlyOne()); // Skip type
                break;

#ifdef VALUE_ARRAYS
            case ELEMENT_TYPE_VALUEARRAY: 
                IfFailRet(SkipExactlyOne());         // Skip element type
                IfFailRet(GetData(NULL));      // Skip array size
                break;
#endif

            case ELEMENT_TYPE_FNPTR:
                IfFailRet(SkipSignature());
                break;

            case ELEMENT_TYPE_ARRAY:
                {
                    IfFailRet(SkipExactlyOne());     // Skip element type
                    ULONG rank;
                    IfFailRet(GetData(&rank));    // Get rank
                    if (rank)
                    {
                        ULONG nsizes;
                        IfFailRet(GetData(&nsizes)); // Get # of sizes
                        while (nsizes--)
                        {
                            IfFailRet(GetData(NULL));           // Skip size
                        }

                        ULONG nlbounds;
                        IfFailRet(GetData(&nlbounds)); // Get # of lower bounds
                        while (nlbounds--)
                        {
                            IfFailRet(GetData(NULL));           // Skip lower bounds
                        }
                    }

                }
                break;

            case ELEMENT_TYPE_SENTINEL:
                // Should be unreachable since GetElem strips it
                break;

            case ELEMENT_TYPE_INTERNAL:
                SkipBytes(sizeof(void *));
                break;

            case ELEMENT_TYPE_GENERICINST:
              IfFailRet(SkipExactlyOne());          // Skip generic type
              ULONG argCnt;
              IfFailRet(GetData(&argCnt)); // Get number of parameters
              while (argCnt--)
              {
                IfFailRet(SkipExactlyOne());        // Skip the parameters
              }
              break;

        }
    }

    return hr;
}

//
// Skip only a method header signature - not the sigs of the args to the method.
//
HRESULT SigParser::SkipMethodHeaderSignature(ULONG *pcArgs)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    HRESULT hr = S_OK;

    // Skip calling convention;
    ULONG uCallConv;
    IfFailRet(GetData(&uCallConv));

    if ((uCallConv & IMAGE_CEE_CS_CALLCONV_MASK) == IMAGE_CEE_CS_CALLCONV_FIELD)
        return META_E_BAD_SIGNATURE;   

    // Skip type parameter count
    if (uCallConv & IMAGE_CEE_CS_CALLCONV_GENERIC)
        IfFailRet(GetData(NULL));

    // Get arg count;
    IfFailRet(GetData(pcArgs));

    // Skip return type;
    IfFailRet(SkipExactlyOne());

    return hr;
}

// Skip a sub signature (as immediately follows an ELEMENT_TYPE_FNPTR).
HRESULT SigParser::SkipSignature()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    HRESULT hr = S_OK;

    ULONG cArgs;
    
    IfFailRet(SkipMethodHeaderSignature(&cArgs));

    // Skip args.
    while (cArgs) {
        IfFailRet(SkipExactlyOne());
        cArgs--;
    }

    return hr;
}

//------------------------------------------------------------------------
// Get info about the single dimensional array type
//------------------------------------------------------------------------
HRESULT SigParser::PeekSDArrayElementProps(SigParser *pElemType, ULONG *pElemCount) const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        PRECONDITION(CheckPointer(pElemType));
        PRECONDITION(CheckPointer(pElemCount));
    }
    CONTRACTL_END

    HRESULT hr = S_OK;
    SigParser sp = *this;

    CorElementType typ;
    IfFailRet(sp.GetElemType(&typ));

    if (typ != ELEMENT_TYPE_VALUEARRAY && typ != ELEMENT_TYPE_SZARRAY)
        return META_E_BAD_SIGNATURE;

    *pElemType = sp;

    IfFailRet(sp.SkipExactlyOne());
    IfFailRet(sp.GetData(pElemCount));

    return hr;
}

