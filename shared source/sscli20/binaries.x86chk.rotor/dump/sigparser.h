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
// sigparser.h
//
#ifndef _H_SIGPARSER
#define _H_SIGPARSER

#include "utilcode.h"
#include "corhdr.h"
#include "corinfo.h"
#include "corpriv.h"

//---------------------------------------------------------------------------------------
// These macros define how arguments are mapped to the stack in the managed calling convention.
// We assume to be walking a method's signature left-to-right, in the virtual calling convention.
// See MethodDesc::Call for details on this virtual calling convention.
// These macros tell us whether the arguments we see as we proceed with the signature walk are mapped
//   to increasing or decreasing stack addresses. This is valid only for arguments that go on the stack.
//---------------------------------------------------------------------------------------
#if defined(_X86_)
#define STACK_GROWS_DOWN_ON_ARGS_WALK
#else
#define STACK_GROWS_UP_ON_ARGS_WALK
#endif


//
// skip any var-arg sentinel
//
inline HRESULT CorSigEatAnyVASentinel(
    PCCOR_SIGNATURE  *pData,             // [IN] compressed data
    DWORD            dwLenOfSig,         // [IN] Length of the signature
    DWORD            *pdwLenOfCustomMod  // [OUT] Length of the signature that should be eaten
    )              
{
    WRAPPER_CONTRACT;
    HRESULT hr = S_OK;
    PCCOR_SIGNATURE pDataTemp = *pData;

    if (dwLenOfSig < 1)
        return META_E_BAD_SIGNATURE;
   
    if (*pDataTemp == ELEMENT_TYPE_SENTINEL)
    {
        pDataTemp++;
        dwLenOfSig--;
    }

    if (pdwLenOfCustomMod != NULL)
    {
        *pdwLenOfCustomMod = (DWORD)(pDataTemp - *pData);
    }

    *pData = pDataTemp;

    return hr;
}


//
// skip any custom modifier prefixes 
//
inline HRESULT CorSigEatCustomModifiers(
    PCCOR_SIGNATURE  *pData,             // [IN] compressed data
    DWORD            dwLenOfSig,        // [IN] Length of the signature
    DWORD            *dwLenOfCustomMod  // [OUT] Length of the custom modifiers
    )              
{
    WRAPPER_CONTRACT;
    HRESULT hr = S_OK;
    PCCOR_SIGNATURE pDataTemp = *pData;

    hr = CorSigEatAnyVASentinel(&pDataTemp, dwLenOfSig, NULL);

    if (FAILED(hr))
    {
        return hr;
    }

    while (dwLenOfSig > 0 && (ELEMENT_TYPE_CMOD_REQD == *pDataTemp || ELEMENT_TYPE_CMOD_OPT == *pDataTemp))
    {
        pDataTemp++;
        dwLenOfSig--;

        mdToken token;
        DWORD dwLenOfToken;

        hr = CorSigUncompressToken(pDataTemp, dwLenOfSig, &token, &dwLenOfToken);

        if (FAILED(hr))
            return hr;

        pDataTemp += dwLenOfToken;
        dwLenOfSig -= dwLenOfToken;
    }

    if (dwLenOfSig == 0)
        return META_E_BAD_SIGNATURE;

    if (dwLenOfCustomMod != NULL)
        *dwLenOfCustomMod = (DWORD)(pDataTemp - *pData);

    *pData = pDataTemp;

    return hr;
}

//
// skip any weird and custom modifier prefixes
//
inline HRESULT CorSigEatFunkyAndCustomModifiers(
    PCCOR_SIGNATURE  *pData,             // [IN] compressed data
    DWORD            dwLenOfSig,        // [IN] Length of the signature
    DWORD            *dwLenOfCustomMod  // [OUT] Length of the custom modifiers
    )              
{
    WRAPPER_CONTRACT;
    HRESULT hr = S_OK;
    PCCOR_SIGNATURE pDataTemp = *pData;

    hr = CorSigEatAnyVASentinel(&pDataTemp, dwLenOfSig, NULL);

    if (FAILED(hr))
    {
        return hr;
    }
    
    while (dwLenOfSig > 0 && 
           (ELEMENT_TYPE_CMOD_REQD == *pDataTemp || 
            ELEMENT_TYPE_CMOD_OPT == *pDataTemp ||
            ELEMENT_TYPE_MODIFIER == *pDataTemp ||
            ELEMENT_TYPE_PINNED == *pDataTemp))
    {
        pDataTemp++;
        dwLenOfSig--;

        mdToken token;
        DWORD dwLenOfToken;

        hr = CorSigUncompressToken(pDataTemp, dwLenOfSig, &token, &dwLenOfToken);

        if (FAILED(hr))
            return hr;

        pDataTemp += dwLenOfToken;
        dwLenOfSig -= dwLenOfToken;
    }

    if (dwLenOfSig == 0)
        return META_E_BAD_SIGNATURE;

    if (dwLenOfCustomMod != NULL)
        *dwLenOfCustomMod = (DWORD)(pDataTemp - *pData);

    *pData = pDataTemp;

    return hr;
}

// uncompress encoded element type. throw away any custom modifier prefixes along
// the way.
inline HRESULT CorSigEatCustomModifiersAndUncompressElementType(
    PCCOR_SIGNATURE *pData,
    DWORD            dwLenOfSig,         // [IN] Length of the signature
    DWORD            *dwLenOfCustomMod,  // [OUT] Length of the custom modifiers
    CorElementType  *etype               // [OUT] Element type

)             // [IN,OUT] compressed data 
{
    WRAPPER_CONTRACT;

    DWORD dwLen = 0;

    HRESULT hr = CorSigEatCustomModifiers(pData, dwLenOfSig, &dwLen);

    if (SUCCEEDED(hr))
    {
        if (dwLenOfSig >= dwLen + 1)
        {
            *etype = (CorElementType)**pData;
            (*pData)++;
            dwLen++;
            if (dwLenOfCustomMod != NULL)
                *dwLenOfCustomMod = dwLen;

            return S_OK;
        }
    }
    return META_E_BAD_SIGNATURE;
}

// CorSig helpers which won't overflow your buffer

inline ULONG CorSigCompressDataSafe(ULONG iLen, BYTE *pDataOut, BYTE *pDataMax)
{
    WRAPPER_CONTRACT;

    BYTE buffer[4];
    ULONG result = CorSigCompressData(iLen, buffer);
    if (pDataOut + result < pDataMax)
        pDataMax = pDataOut + result;
    if (pDataMax > pDataOut)
        CopyMemory(pDataOut, buffer, pDataMax - pDataOut);
    return result;
}

inline ULONG CorSigCompressTokenSafe(mdToken tk, BYTE *pDataOut, BYTE *pDataMax)
{
    WRAPPER_CONTRACT;

    BYTE buffer[4];
    ULONG result = CorSigCompressToken(tk, buffer);
    if (pDataOut + result < pDataMax)
        pDataMax = pDataOut + result;
    if (pDataMax > pDataOut)
        CopyMemory(pDataOut, buffer, pDataMax - pDataOut);
    return result;
}

inline ULONG CorSigCompressSignedIntSafe(int iData, BYTE *pDataOut, BYTE *pDataMax)
{
    WRAPPER_CONTRACT;

    BYTE buffer[4];
    ULONG result = CorSigCompressSignedInt(iData, buffer);
    if (pDataOut + result < pDataMax)
        pDataMax = pDataOut + result;
    if (pDataMax > pDataOut)
        CopyMemory(pDataOut, buffer, pDataMax - pDataOut);
    return result;
}

inline ULONG CorSigCompressElementTypeSafe(CorElementType et, 
                                           BYTE *pDataOut, BYTE *pDataMax)
{
    WRAPPER_CONTRACT;

    if (pDataMax > pDataOut)
        return CorSigCompressElementType(et, pDataOut);
    else
        return 1;
}


inline HRESULT CorSigPeekElementTypeSize(
    PCCOR_SIGNATURE  pData,
    DWORD            dwLenOfSig,         // [IN] Length of the signature
    ULONG           *pSize)
{
    WRAPPER_CONTRACT;
    
    HRESULT hr = S_OK;
    PCCOR_SIGNATURE pDataTemp = pData;

    hr = CorSigEatAnyVASentinel(&pDataTemp, dwLenOfSig, NULL);

    if (FAILED(hr))
    {
        return hr;
    }

    dwLenOfSig = dwLenOfSig - (DWORD)(pDataTemp - pData);

    if (pSize != NULL)
    {
        *pSize = 0;
    }

    switch (*pDataTemp)
    {
    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_U8:
    case ELEMENT_TYPE_R8:

        if (pSize != NULL)
        {
            *pSize = 8;
        }
        return hr;

    case ELEMENT_TYPE_I4:
    case ELEMENT_TYPE_U4:
    case ELEMENT_TYPE_R4:
    case ELEMENT_TYPE_I:
    case ELEMENT_TYPE_U:

        if (pSize != NULL)
        {
            *pSize = 4;
        }
        return hr;

    case ELEMENT_TYPE_I2:
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_CHAR:
        if (pSize != NULL)
        {
            *pSize = 2;
        }
        return hr;

    case ELEMENT_TYPE_I1:
    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_BOOLEAN:
        if (pSize != NULL)
        {
            *pSize = 1;
        }
        return hr;

    case ELEMENT_TYPE_STRING:
    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_BYREF:
    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_OBJECT:
    case ELEMENT_TYPE_FNPTR:
    case ELEMENT_TYPE_TYPEDBYREF:
    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_SZARRAY:
        if (pSize != NULL)
        {
            *pSize = sizeof(void *);
        }
        return hr;

    case ELEMENT_TYPE_VOID:
        return hr;

    case ELEMENT_TYPE_END:
    case ELEMENT_TYPE_CMOD_REQD:
    case ELEMENT_TYPE_CMOD_OPT:
        _ASSERTE(!"Asked for the size of an element that doesn't have a size!");
        return E_INVALIDARG;

    case ELEMENT_TYPE_VALUETYPE:
        return hr;

    default:

        PCCOR_SIGNATURE pDataTemp2 = pDataTemp;

        hr = CorSigEatAnyVASentinel(&pDataTemp2, dwLenOfSig, NULL);

        if (FAILED(hr))
        {
            return hr;
        }

        if (pDataTemp2 != pDataTemp)
        {
            dwLenOfSig = dwLenOfSig - (DWORD)(pDataTemp2 - pDataTemp);
            return CorSigPeekElementTypeSize(pDataTemp2, dwLenOfSig, pSize);
        }

        _ASSERTE( !"CorSigGetElementTypeSize given bogus value to size!" );
        return E_INVALIDARG;
    }
}



BOOL    IsFP(CorElementType etyp);
BOOL    IsBaseElementType(CorElementType etyp);

//----------------------------------------------------------------------------
// enum StringType
// defines the various string types
enum StringType
{
    enum_WSTR = 0,
    enum_CSTR = 1,
};


//------------------------------------------------------------------------
// Encapsulates how compressed integers and typeref tokens are encoded into
// a bytestream.
//
// As you use this class please understand the implicit normalizations 
// on the CorElementType's returned by the various methods, especially
// for variable types (e.g. !0 in generic signatures), string types
// (i.e. E_T_STRING), object types (E_T_OBJECT), constructed types 
// (e.g. List<int>) and enums.
//------------------------------------------------------------------------
class SigParser
{
    protected:
        PCCOR_SIGNATURE m_ptr;
        DWORD           m_dwLen;

        FORCEINLINE void SkipBytes(ULONG cb)
        {
            _ASSERT(cb <= m_dwLen);
            m_ptr += cb;
            m_dwLen -= cb;
        }

    public:
        //------------------------------------------------------------------------
        // Constructor.
        //------------------------------------------------------------------------
        SigParser() { LEAF_CONTRACT; m_ptr = NULL; m_dwLen = 0; }
        SigParser(const SigParser &sig);

        //------------------------------------------------------------------------
        // Initialize 
        //------------------------------------------------------------------------
        FORCEINLINE SigParser(PCCOR_SIGNATURE ptr)
        {
            LEAF_CONTRACT;

            m_ptr = ptr;
            // We don't know the size of the signature, so we'll say it's "big enough"
            m_dwLen = 0xffffffff;
        }

        FORCEINLINE SigParser(PCCOR_SIGNATURE ptr, DWORD len)
        {
            LEAF_CONTRACT;

            m_ptr = ptr;
            m_dwLen = len;
        }

        inline void SetSig(PCCOR_SIGNATURE ptr)
        {
            LEAF_CONTRACT;
            
            m_ptr = ptr;
            // We don't know the size of the signature, so we'll say it's "big enough"
            m_dwLen = 0xffffffff;
        }

        inline void SetSig(PCCOR_SIGNATURE ptr, DWORD len)
        {
            LEAF_CONTRACT;
            
            m_ptr = ptr;
            m_dwLen = len;
        }

        /*
         * LengthOfSig
         *
         * Computes the length of a well-formed signature.  
         *
         * NOTE! NOTE!
         *
         * The only known time you should be using this is when using a VarArg signature, which
         * the CLR does not store the length of.  This case may also be incorrect from a security
         * perspective, but since we do not store the length at this time, there is no way around
         * this at the moment.
         *
         */
        static DWORD LengthOfSig(PCCOR_SIGNATURE szMetaSig)
        {
            WRAPPER_CONTRACT;

            DWORD nBytes;
            SigParser sigParser(szMetaSig);
            sigParser.SkipSignature();

            nBytes = (DWORD)(sigParser.GetPtr() - szMetaSig);
            CONSISTENCY_CHECK(nBytes >= 0);

            return (DWORD)nBytes;
        }



    //=========================================================================
    // The RAW interface for reading signatures.  You see exactly the signature,
    // apart from custom modifiers which for historical reasons tend to get eaten.
    //
    // DO NOT USE THESE METHODS UNLESS YOU'RE TOTALLY SURE YOU WANT
    // THE RAW signature.  You nearly always want GetElemTypeClosed() or 
    // PeekElemTypeClosed() or one of the MetaSig functions.  See the notes above.
    // These functions will return E_T_INTERNAL, E_T_VAR, E_T_MVAR and such
    // so the caller must be able to deal with those
    //=========================================================================

        //------------------------------------------------------------------------
        // Remove one compressed integer (using CorSigUncompressData) from
        // the head of the stream and return it.
        //------------------------------------------------------------------------
        FORCEINLINE HRESULT GetData(ULONG* data)
        {
            WRAPPER_CONTRACT;

            ULONG sizeOfData = 0;
            ULONG tempData;

            if (data == NULL)
                data = &tempData;
            
            HRESULT hr = CorSigUncompressData(m_ptr, m_dwLen, data, &sizeOfData);

            if (SUCCEEDED(hr))
            {
                SkipBytes(sizeOfData);
            }
            
            return hr;
        }


        //-------------------------------------------------------------------------
        // Remove one byte and return it.
        //-------------------------------------------------------------------------
        FORCEINLINE HRESULT GetByte(BYTE *data)
        {
            LEAF_CONTRACT;

            if (m_dwLen > 0)
            {
                if (data != NULL)
                    *data = *m_ptr;

                SkipBytes(1);
                
                return S_OK;
            }
                
            return META_E_BAD_SIGNATURE;
        }

        //-------------------------------------------------------------------------
        // The element type as defined in CorElementType. No normalization for
        // generics (E_T_VAR, E_T_MVAR,..) or dynamic methods (E_T_INTERNAL occurs)
        //-------------------------------------------------------------------------
        FORCEINLINE HRESULT GetElemType(CorElementType *etype)
        {
            WRAPPER_CONTRACT;

            DWORD dwLen = 0;
            CorElementType tmpEType;

            if (etype == NULL)
                etype = &tmpEType;


            HRESULT hr = CorSigEatCustomModifiersAndUncompressElementType(&m_ptr, m_dwLen, &dwLen, etype);

            if (SUCCEEDED(hr))
                m_dwLen -= dwLen;

            return hr;            
        }



        HRESULT GetCallingConvInfo(ULONG *data)  
        {   
            WRAPPER_CONTRACT;

            ULONG tmpData;

            if (data == NULL)
                data = &tmpData;
            
            HRESULT hr = CorSigUncompressCallingConv(m_ptr, m_dwLen, data);
            if (SUCCEEDED(hr))
            {
                SkipBytes(1);
            }

            return hr;
        }   

        HRESULT GetCallingConv(ULONG* data)  
        {   
            WRAPPER_CONTRACT;
            ULONG info;
            HRESULT hr = GetCallingConvInfo(&info);

            if (SUCCEEDED(hr) && data != NULL)
            {
                *data = IMAGE_CEE_CS_CALLCONV_MASK & info;
            }

            return hr; 
        }   

        //------------------------------------------------------------------------
        // Non-destructive read of compressed integer.
        //------------------------------------------------------------------------
        HRESULT PeekData(ULONG *data) const
        {
            WRAPPER_CONTRACT;
            _ASSERTE(data != NULL);

            ULONG sizeOfData = 0;
            return CorSigUncompressData(m_ptr, m_dwLen, data, &sizeOfData);
        }


        //------------------------------------------------------------------------
        // Non-destructive read of element type.
        //
        // This routine makes it look as if the String type is encoded
        // via ELEMENT_TYPE_CLASS followed by a token for the String class,
        // rather than the ELEMENT_TYPE_STRING. This is partially to avoid
        // rewriting client code which depended on this behavior previously.
        // But it also seems like the right thing to do generally.
        // No normalization for generics (E_T_VAR, E_T_MVAR,..) or 
        // dynamic methods (E_T_INTERNAL occurs)
        //------------------------------------------------------------------------
        HRESULT PeekElemType(CorElementType *etype) const
        {
            WRAPPER_CONTRACT;
            _ASSERTE(etype != NULL);

            PCCOR_SIGNATURE tmp = m_ptr;
            HRESULT hr = CorSigEatCustomModifiersAndUncompressElementType(&tmp, m_dwLen, NULL, etype);
            if (SUCCEEDED(hr) && (*etype == ELEMENT_TYPE_STRING || *etype == ELEMENT_TYPE_OBJECT))
                *etype = ELEMENT_TYPE_CLASS;

            return hr;
        }

        //-------------------------------------------------------------------------
        // Returns the raw size of the type next in the signature, or returns
        // E_INVALIDARG for base types that have variables sizes.
        //-------------------------------------------------------------------------
        FORCEINLINE HRESULT PeekElemTypeSize(ULONG *pSize)
        {
            WRAPPER_CONTRACT;

            DWORD dwSize = 0;
            
            if (pSize == NULL)
            {
                pSize = &dwSize;
            }

            return CorSigPeekElementTypeSize(m_ptr, m_dwLen, pSize);         
        }

        //------------------------------------------------------------------------
        // Is this at the Sentinal (the ... in a varargs signature) that marks
        // the begining of varguments that are not decared at the target

        bool AtSentinel() const
        {
            if (m_dwLen > 0)
                return *m_ptr == ELEMENT_TYPE_SENTINEL;
            else
                return false;
        }

        FORCEINLINE HRESULT GetToken(mdToken *token)
        {
            WRAPPER_CONTRACT;
            DWORD dwLen;
            mdToken tempToken;

            if (token == NULL)
                token = &tempToken;
            
            HRESULT hr = CorSigUncompressToken(m_ptr, m_dwLen, token, &dwLen);

            if (SUCCEEDED(hr))
            {
                SkipBytes(dwLen);
            }

            return hr;
        }


        //------------------------------------------------------------------------
        // Tests if two SigParsers point to the same location in the stream.
        //------------------------------------------------------------------------
        FORCEINLINE BOOL Equals(SigParser sp) const
        {
            LEAF_CONTRACT;

            return m_ptr == sp.m_ptr;
        }

        FORCEINLINE HRESULT SkipCustomModifiers()
        {
            WRAPPER_CONTRACT;
            DWORD dwLen;

            HRESULT hr = CorSigEatCustomModifiers(&m_ptr, m_dwLen, &dwLen);

            if (SUCCEEDED(hr))
                m_dwLen -= dwLen;

            return hr;

        }// SkipCustomModifiers

        FORCEINLINE HRESULT SkipFunkyAndCustomModifiers()
        {
            WRAPPER_CONTRACT;
            DWORD dwLen;

            HRESULT hr = CorSigEatFunkyAndCustomModifiers(&m_ptr, m_dwLen, &dwLen);

            if (SUCCEEDED(hr))
                m_dwLen -= dwLen;

            return hr;

        }// SkipFunkyAndCustomModifiers


        FORCEINLINE HRESULT SkipAnyVASentinel()
        {
            WRAPPER_CONTRACT;
            DWORD dwLen;

            HRESULT hr = CorSigEatAnyVASentinel(&m_ptr, m_dwLen, &dwLen);

            if (SUCCEEDED(hr))
                m_dwLen -= dwLen;

            return hr;

        }// SkipAnyVASentinel

        //------------------------------------------------------------------------
        // Assumes that the SigParser points to the start of an element type
        // (i.e. function parameter, function return type or field type.)
        // Advances the pointer to the first data after the element type.  
        //------------------------------------------------------------------------
        HRESULT SkipExactlyOne();

        //------------------------------------------------------------------------
        // Skip only the method header of the signature, not the signature of 
        // the arguments.
        //------------------------------------------------------------------------
        HRESULT SkipMethodHeaderSignature(ULONG *pcArgs);

        //------------------------------------------------------------------------
        // Skip a sub signature (as immediately follows an ELEMENT_TYPE_FNPTR).
        //------------------------------------------------------------------------
        HRESULT SkipSignature();

        //------------------------------------------------------------------------
        // Get info about single-dimensional arrays
        // PLEASE DON'T USE THIS.
        // 
        // It's hard to resist, but please try
        // not to use this.  Certainly don't use it if there's any chance of the
        // signature containing generic type variables.
        // 
        // (dsyme, Nov 11 2002) When I last checked it is only
        // used in one place: Old code for value arrays in the JIT interface. 
        //------------------------------------------------------------------------
        HRESULT PeekSDArrayElementProps(SigParser *pElemType, ULONG *pElemCount) const;

    //=========================================================================
    // The CLOSED interface for reading signatures.  With the following
    // methods you see the signature "as if" all type variables are 
    // replaced by the given instantiations.  However, no type loads happen.
    //
    // In general this is what you want to use if the signature may include
    // generic type variables.  Even if you know it doesn't you can always
    // pass in NULL for the instantiations and put a comment to that effect.
    //
    // The CLOSED api also hides E_T_INTERNAL by return E_T_CLASS or E_T_VALUETYPE
    // appropriately (as directed by the TypeHandle following E_T_INTERNAL)
    //=========================================================================


public:

        //------------------------------------------------------------------------
        // Return pointer
        // PLEASE DON'T USE THIS.
        // 
        // Return the internal pointer.  It's hard to resist, but please try
        // not to use this.  Certainly don't use it if there's any chance of the
        // signature containing generic type variables.
        // 
        // (dsyme, Nov 11 2002) It's currently only used for hacking on the 
        // signatures stored in TypeSpec tokens (we should add a new abstraction,
        // i.e. on MetaSig for this) and a couple of places to do with COM
        // and native interop signature hacking.   
        //------------------------------------------------------------------------
        PCCOR_SIGNATURE GetPtr() const
        {
            LEAF_CONTRACT;
            return m_ptr;
        }

};

//------------------------------------------------------------------------
FORCEINLINE 
SigParser::SigParser(
    const SigParser &sig)
    : m_ptr(sig.m_ptr), m_dwLen(sig.m_dwLen)
{
    LEAF_CONTRACT;
}


#ifdef _DEBUG
#define MAX_CACHED_SIG_SIZE     3       // To excercize non-cached code path
#else
#define MAX_CACHED_SIG_SIZE     15
#endif


//=========================================================================
// Indicates whether an argument is to be put in a register using the
// default IL calling convention. This should be called on each parameter
// in the order it appears in the call signature. For a non-static method,
// this function should also be called once for the "this" argument, prior
// to calling it for the "real" arguments. Pass in a typ of IMAGE_CEE_CS_OBJECT.
//
//  *pNumRegistersUsed:  [in,out]: keeps track of the number of argument
//                       registers assigned previously. The caller should
//                       initialize this variable to 0 - then each call
//                       will update it.
//
//  typ:                 the signature type
//  structSize:          for structs, the size in bytes
//  fThis:               is this about the "this" pointer?
//  callconv:            see IMAGE_CEE_CS_CALLCONV_*
//  *pOffsetIntoArgumentRegisters:
//                       If this function returns TRUE, then this out variable
//                       receives the identity of the register, expressed as a
//                       byte offset into the ArgumentRegisters structure.
//
// 
//=========================================================================
BOOL IsArgumentInRegister(int   *pNumRegistersUsed,
                          BYTE   typ,
                          UINT32 structSize,
                          BOOL   fThis,
                          BYTE   callconv,
                          int    *pOffsetIntoArgumentRegisters);


/*****************************************************************/
/* CorTypeInfo is a single global table that you can hang information
   about ELEMENT_TYPE_* */

class CorTypeInfo {
public:

    static LPCUTF8 GetName(CorElementType type)
    {
        LEAF_CONTRACT;
        
        _ASSERTE(type < infoSize);
        return info[type].className;
    }

    static LPCUTF8 GetNamespace(CorElementType type)
    {
        LEAF_CONTRACT;

        _ASSERTE(type < infoSize);
        return info[type].nameSpace;
    }

    static void CheckConsistancy()
    {
        LEAF_CONTRACT;

        for(int i=0; i < infoSize; i++)
            _ASSERTE(info[i].type == i);
    }

    static CorInfoGCType GetGCType(CorElementType type);

    static BOOL IsObjRef(CorElementType type)
    {
        WRAPPER_CONTRACT;
        return (GetGCType(type) == TYPE_GC_REF);
    }

    static BOOL IsGenericVariable(CorElementType type)
    {
        LEAF_CONTRACT;

        return (type == ELEMENT_TYPE_VAR || type == ELEMENT_TYPE_MVAR);
    }

    static BOOL IsArray(CorElementType type)
    {
        LEAF_CONTRACT;

        _ASSERTE(type < infoSize);
        return info[type].isArray;
    }

    static BOOL IsFloat(CorElementType type)
    {
        LEAF_CONTRACT;

        _ASSERTE(type < infoSize);
        return info[type].isFloat;
    }

    static BOOL IsModifier(CorElementType type)
    {
        LEAF_CONTRACT;

        _ASSERTE(type < infoSize);
        return info[type].isModifier;
    }

    static BOOL IsPrimitiveType(CorElementType type)
    {
        LEAF_CONTRACT;

        _ASSERTE(type < infoSize);
        return info[type].isPrim;
    }

    static unsigned Size(CorElementType type);

    static CorElementType FindPrimitiveType(LPCUTF8 fullName);
    static CorElementType FindPrimitiveType(LPCUTF8 nameSp, LPCUTF8 name);
protected:
    struct CorTypeInfoEntry {
        CorElementType type;
        LPCUTF8        nameSpace;
        LPCUTF8        className;
        unsigned       size         : 8;
        CorInfoGCType  gcType       : 3;
        unsigned       isArray      : 1;
        unsigned       isPrim       : 1;
        unsigned       isFloat      : 1;
        unsigned       isModifier   : 1;
    };

    static CorTypeInfoEntry info[];
    static const int infoSize;
};


// Returns the address of the payload inside the stackelem
inline void* StackElemEndianessFixup(void* pStackElem, UINT cbSize) {
    LEAF_CONTRACT;

    BYTE *pRetVal = (BYTE*)pStackElem;

#if BIGENDIAN
    switch (cbSize)
    {
    case 1:
        pRetVal += sizeof(void*)-1;
        break;
    case 2:
        pRetVal += sizeof(void*)-2;
        break;
    default:
        // nothing to do
        break;
    }
#endif

    return pRetVal;
}

#endif /* _H_SIGINFOBASE */

