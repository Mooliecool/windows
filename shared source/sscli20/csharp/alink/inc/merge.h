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
// ===========================================================================
// File: merge.h
//
// code to do merging of PE stuff
// ===========================================================================

#ifndef __merge_h__
#define __merge_h__

#undef malloc
#undef realloc
#undef free
#define malloc(a)     VSAlloc(a)
#define realloc(a, b) VSRealloc(a, b)
#define free(pv)      VSFree(pv)

#undef IfFailGo
#undef IfFailGoto
#include <corhlpr.h>

typedef struct _tagOpCode {
    BYTE b1;
    BYTE b2;
    BYTE OpLen;
    BYTE TotLen;
    bool HasToken;      //  0x8000
    bool Special;       //  0x4000
    bool Unhandled;     //  0x2000
} OpCode;

class PEFile;

class CDataMember {
protected:
    friend class PEFile;
    mdToken                 tkMember;
    DWORD                   dwRVA;
    DWORD                   dwSize;
    void *                  pvData;
    DWORD                   dwAlignment;

public:
    CDataMember( mdToken tk, DWORD RVA, DWORD Size, void * data);
    CDataMember();

    inline bool IsMethod() { return TypeFromToken(tkMember) == mdtMethodDef; }
};

class CResource : public CDataMember {
public:
    LPWSTR                  pszName;
    LPWSTR                  pszDescription;
    LPWSTR                  pszMime;
    LPWSTR                  pszLocale;
    DWORD                   dwFlags;

    CResource();
    CResource(mdToken tk, DWORD RVA, DWORD Size, void * data) : CDataMember( tk, RVA, Size, data), pszName(NULL), pszDescription(NULL), pszMime(NULL), pszLocale(NULL), dwFlags(UINT_MAX) {};
    ~CResource();
};

struct _TokenMap {
    mdToken                 tkOldToken;
    mdToken                 tkNewToken;

    _TokenMap() { tkOldToken = tkNewToken = mdTokenNil; }
    _TokenMap(const _TokenMap &other) { tkOldToken = other.tkOldToken; tkNewToken = other.tkNewToken; }

    static int _cdecl CompareOld(_TokenMap m1, _TokenMap m2) {
        return m1.tkOldToken - m2.tkOldToken;
    }
    static int _cdecl CompareNew(_TokenMap m1, _TokenMap m2) {
        return m1.tkNewToken - m2.tkNewToken;
    }
};

typedef tree<_TokenMap>   TokenSet;

class CTokenMap : public IMapToken {
public:
    CTokenMap();
    ~CTokenMap();

    bool ReMapToken(mdToken *pToken, bool fSwap);

    void CleanupMap() { m_Tokens.Cleanup(); }
    
    // COM Interface
    STDMETHOD(QueryInterface)(const GUID &iid, void** ppUnk) {
        if (iid == IID_IMapToken) {
            *ppUnk = (IMapToken*)this;
            return S_OK;
        } else {
            *ppUnk = NULL;
            return E_FAIL;
        }
    }
    STDMETHOD_(DWORD, AddRef)() { return 1; }
    STDMETHOD_(DWORD, Release)() { return 1; }
    STDMETHOD(Map)(mdToken oldToken, mdToken newToken);

protected:
    TokenSet  m_Tokens;
    mdToken   m_Mask;
};

class PEFile {
public:
    IMAGE_COR20_HEADER         *pICH;
    BYTE                       *pbMapAddress;
    DWORD                       dwLength;
    IMetaDataImport            *pImport;

    HRESULT OpenFileAs(IMetaDataDispenserEx *pDisp, LPCWSTR pszFileName, bool AllowNonIl, bool bWriteable);
    HRESULT EmitMembers(ICeeFileGen *pOutFile, HCEEFILE hCeeFile, HCEESECTION hIlSection, HCEESECTION hResSection, DWORD * pdwOffset, CTokenMap *pMap, IMetaDataEmit *pEmit); // Write out new method bodies and resources, also calls MapTokens
    HRESULT WriteNewMetaData(IMetaDataEmit *pEmit, DWORD newSize);
    HRESULT Close();

    static HRESULT GetPEKind(LPCWSTR pszFilename, DWORD *pdwPEKind, DWORD *pdwMachine);

    PEFile();
    ~PEFile();
protected:

    HRESULT OpenFile( LPCWSTR pszFilename, bool bWriteable);
    HRESULT FindTokens(void *pMethod, CTokenMap *pMap);
    OpCode *FindOpCode(BYTE *code);
    bool    bIs64;
    bool    bIsWriteable;

    union {
        IMAGE_NT_HEADERS32     *pNT32;
        IMAGE_NT_HEADERS64     *pNT64;
    };
    PIMAGE_SECTION_HEADER   ImageNextSection(PIMAGE_SECTION_HEADER section);
    PIMAGE_SECTION_HEADER   ImageRvaToSection(ULONG Rva);
    PVOID                   ImageRvaToVa(ULONG Rva);
    bool                    setCOMHeader();
    bool                    FindNTHeader();
};

#endif // __merge_h__

