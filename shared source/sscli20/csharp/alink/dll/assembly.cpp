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
// File: assembly.cpp
//
// ===========================================================================

#include "stdafx.h"
#include "alink.h"
#include "asmlink.h"
#include "assembly.h"
#include "msgsids.h"
#include "merge.h"
#include "oleauto.h"

static const unsigned char pbMscorlibKey [] = { 0, 0, 0, 0, 0, 0, 0, 0,  4, 0, 0, 0, 0, 0, 0, 0};
static const unsigned char pbMscorlibToken [] = { 0xB7, 0x7A, 0x5C, 0x56, 0x19, 0x34, 0xE0, 0x89};

#define OPTION(name, type, flags, id, shortname, longname, CA) {CA, (AssemblyOptions)name, id, type, flags},
#define INCLUDE_PRIVATE
const _OptionCA OptionCAs [] = {
#include "options.h"
    {L"", (AssemblyOptions)optLastPriAssemOption, IDS_InternalError, VT_EMPTY, 0x02}  // 2 = Custom Attribute Only
};

#define ERRORDEF(num, sev, name, id) {num, sev, id},
const int ERROR_DEFS[][3] = {
#include "errors.h"
};

#define SAFEFREE(var) if ((var) != NULL) { delete [] (var); (var) = NULL; }

__forceinline bool IsValidPublicKeyBlob(const PublicKeyBlob *p, const size_t len) {
    return ((VAL32(p->cbPublicKey) + (sizeof(ULONG) * 3)) == len &&        // do the lengths match?
            GET_ALG_CLASS(VAL32(p->SigAlgID)) == ALG_CLASS_SIGNATURE &&    // is it a valid signature alg?
            GET_ALG_CLASS(VAL32(p->HashAlgID)) == ALG_CLASS_HASH);         // is it a valid hash alg?
}

// Helper functions that wrap calls to IMetaDataImport methods.
static HRESULT __stdcall CallEnumCustomAttributes(IMetaDataImport *pImport,
                                HCORENUM *phEnum, mdToken tk, mdToken tkType,
                                mdCustomAttribute rCustomValues[], ULONG cMax,
                                ULONG *pcCustomValues)
{
    return pImport->EnumCustomAttributes(phEnum, tk, tkType, rCustomValues, cMax, pcCustomValues);
}

static HRESULT __stdcall CallEnumUserStrings(IMetaDataImport *pImport,
                                HCORENUM *phEnum, mdString rStrings[],
                                ULONG cmax, ULONG *pcStrings)
{
    return pImport->EnumUserStrings(phEnum, rStrings, cmax, pcStrings);
}

static HRESULT __stdcall CallEnumSignatures(IMetaDataImport *pImport,
                                HCORENUM *phEnum, mdSignature rSignatures[],
                                ULONG cmax, ULONG *pcSignatures)
{
    return pImport->EnumSignatures(phEnum, rSignatures, cmax, pcSignatures);
}

static HRESULT __stdcall CallEnumTypeDefs(IMetaDataImport *pImport,
                                HCORENUM *phEnum, mdTypeDef rTypeDefs[],
                                ULONG cMax, ULONG *pcTypeDefs)
{
    return pImport->EnumTypeDefs(phEnum, rTypeDefs, cMax, pcTypeDefs);
}

static HRESULT __stdcall CallEnumMembers(IMetaDataImport *pImport,
                                HCORENUM *phEnum, mdTypeDef cl,
                                mdToken rMembers[], ULONG cMax,
                                ULONG *pcTokens)
{
    return pImport->EnumMembers(phEnum, cl, rMembers, cMax, pcTokens);
}

static HRESULT __stdcall CallEnumTypeRefs(IMetaDataImport *pImport,
                                HCORENUM *phEnum, mdTypeRef rTypeRefs[],
                                ULONG cMax, ULONG* pcTypeRefs)
{
    return pImport->EnumTypeRefs(phEnum, rTypeRefs, cMax, pcTypeRefs);
}

static HRESULT __stdcall CallEnumMemberRefs(IMetaDataImport *pImport,
                                HCORENUM *phEnum, mdToken tkParent,
                                mdMemberRef rMemberRefs[], ULONG cMax,
                                ULONG *pcTokens)
{
    return pImport->EnumMemberRefs(phEnum, tkParent, rMemberRefs, cMax, pcTokens);
}

static HRESULT __stdcall CallEnumTypeSpecs(IMetaDataImport *pImport,
                                HCORENUM *phEnum, mdTypeSpec rTypeSpecs[],
                                ULONG cmax, ULONG *pcTypeSpecs)
{
    return pImport->EnumTypeSpecs(phEnum, rTypeSpecs, cmax, pcTypeSpecs);
}

/*
 * Helper function and load and format a message. Uses Unicode
 * APIs, so it won't work on Win95/98.
 */
static bool LoadAndFormatW(HINSTANCE hModule, int resid, va_list args, __out_ecount(cchMax) LPWSTR buffer, size_t cchMax)
{
    LPWSTR formatString = (LPWSTR)_alloca(cchMax * sizeof(WCHAR));

    return (LoadStringW (hModule, resid, formatString, (int) cchMax) != 0) &&
           (FormatMessageW (FORMAT_MESSAGE_FROM_STRING, formatString, 0, 0, buffer, (DWORD) cchMax, &args) != 0);
}

/*
 * Helper function to load and format a message using ANSI functions.
 * Used as a backup when Unicode ones are not available.
 */
static bool LoadAndFormatA(HINSTANCE hModule, int resid, va_list args, __out_ecount(cchMax) LPWSTR buffer, size_t cchMax)
{
    return false;
}

/*
 * Helper function and load and format a message. Uses Unicode
 *  and ANSI APIs, so it works on all platforms.
 */
static bool W_LoadAndFormat(HINSTANCE hModule, int resid, va_list args, __out_ecount(cchMax) LPWSTR buffer, size_t cchMax)
{
    if (hModule == NULL)
        return SUCCEEDED(StringCchCopyW( buffer, cchMax, L"Unable to find messages file '" MESSAGE_DLLW L"'"));

    return LoadAndFormatW (hModule, resid, args, buffer, cchMax) ||
           LoadAndFormatA (hModule, resid, args, buffer, cchMax);
}

inline int FindOption(LPCWSTR CustAttrName)
{
    for (int opt = 0; opt < optLastPriAssemOption; opt++) {
        if (wcscmp(CustAttrName, OptionCAs[opt].name) == 0)
            return opt;
    }
    return optLastPriAssemOption;
}

LPCWSTR ErrorOptionName(AssemblyOptions opt) {
    static WCHAR buffer[1024];
    HINSTANCE hRes = GetALinkMessageDll();

    // Load the message and fill in arguments. Try using Unicode function first,
    // then back off to ANSI.
    if (!W_LoadAndFormat (hRes, OptionCAs[opt].optName, NULL, buffer, 1024))
    {
        // Not a lot we can do if we can't report an error. Assert in debug so we know
        // what is going on.
        VSFAIL("Failed to load error message");
        return NULL;
    }

    // This will probably fail if we're in low-memory conditions.
    return buffer;
}

CFile::CFile() :
    m_tkError(mdTokenNil),
    m_Name(NULL),
    m_Path(NULL),
    m_SrcFile(NULL),
    m_modName(NULL),
    m_tkFile(mdTokenNil),
    m_tkMscorlibRef(mdAssemblyRefNil),
    m_dwPEKind(peNot),
    m_dwMachine(0),
    m_isAssembly(false),
    m_isStdLib(false),
    m_isUBM(false),
    m_isImport(false),
    m_isDeclared(false),
    m_isCTDeclared(false),
    m_bIsExe(false),
    m_bNoMetaData(false),
    m_bHasPE(false),
    m_bCheckedPE(false),
    m_bEmittedPublicTypes(false),
    m_bEmittedInternalTypes(false),
    m_bEmittedTypeForwarders(false),
    m_pEmit(NULL),
    m_pImport(NULL),
    m_pError(NULL),
    m_pLinker(NULL),
    m_pAImportCached(NULL)
{
    m_tkAssemblyAttrib[0][0] = mdTokenNil;
    m_tkAssemblyAttrib[0][1] = mdTokenNil;
    m_tkAssemblyAttrib[1][0] = mdTokenNil;
    m_tkAssemblyAttrib[1][1] = mdTokenNil;
}

HRESULT CFile::Init(LPCWSTR pszFilename, IMetaDataEmit* pEmitter, IMetaDataError *pError, CAsmLink* pLinker)
{
    m_pError = pError;
    m_pLinker = pLinker;
    if (pEmitter) {
        HRESULT hr = pEmitter->QueryInterface(IID_IMetaDataImport, (void**)&m_pImport);
        if (FAILED(hr))
            return hr;
        m_pEmit = pEmitter;
        m_pEmit->AddRef();
        m_isDeclared = true;
    } else {
        m_isCTDeclared = true;
        m_isUBM = true;
    }

    // The name is everything after the last '/' or '\'
    m_Path = VSAllocStr(pszFilename);
    if (m_Path == NULL) {
        m_pError = NULL;
        m_pLinker = NULL;
        if (m_pEmit != NULL) {
            m_pEmit->Release();
            m_pEmit = NULL;
        }
        if (m_pImport != NULL) {
            m_pImport->Release();
            m_pImport = NULL;
        }
        return E_OUTOFMEMORY;
    }

    LPCWSTR temp;
    temp = wcsrchr( m_Path, L'\\');
    if (temp == NULL)
        temp = m_Path;
    else
        temp++;
    m_Name = wcsrchr( temp, L'/');
    if (m_Name == NULL)
        m_Name = temp;
    else 
        m_Name++;

    size_t len = wcslen(m_Name);
    m_bIsExe = ((len > 4) && (_wcsicmp(m_Name + (len - 4), L".exe") == 0));

    return S_OK;
}

HRESULT CFile::Init(IMetaDataImport* pImporter, IMetaDataError *pError, CAsmLink* pLinker)
{
    m_isImport = true;
    m_pError = pError;
    m_pLinker = pLinker;
    m_pImport = pImporter;
    m_pImport->AddRef();
    m_pEmit = NULL;

    m_Name = new WCHAR[MAX_IDENT_LEN];
    if (m_Name == NULL)
       return E_OUTOFMEMORY;
    ULONG chName = 0;
    HRESULT hr = m_pImport->GetScopeProps( (LPWSTR)m_Name, MAX_IDENT_LEN, &chName, NULL);
    if (FAILED(hr)) {
        delete [] ((LPWSTR)m_Name);
        m_Name = NULL;
        return hr;
    }
    m_Path = m_Name;

    m_bIsExe = ((chName > 4) && (m_Name != NULL && _wcsicmp(m_Name + (chName - 4), L".exe") == 0));

    m_isUBM = true;
    m_isDeclared = false;

    return S_OK;
}

HRESULT CFile::Init(LPCWSTR pszFilename, IMetaDataImport* pImporter, IMetaDataError *pError, CAsmLink* pLinker)
{
    m_isImport = true;
    m_pError = pError;
    m_pLinker = pLinker;
    m_pImport = pImporter;
    m_pImport->AddRef();
    m_pEmit = NULL;
    m_isDeclared = false;

    // The name is everything after the last '/' or '\'
    m_Path = VSAllocStr(pszFilename);
    if (m_Path == NULL)
        return E_OUTOFMEMORY;
    LPCWSTR temp;
    temp = wcsrchr( m_Path, L'\\');
    if (temp == NULL)
        temp = m_Path;
    else
        temp++;
    m_Name = wcsrchr( temp, L'/');
    if (m_Name == NULL)
        m_Name = temp;
    else
        m_Name++;

    size_t len = wcslen(m_Name);
    m_bIsExe = ((len > 4) && (_wcsicmp(m_Name + (len - 4), L".exe") == 0));

    return S_OK;
}

CFile::~CFile()
{
    if (m_pEmit) {
        m_pEmit->Release();
        m_pEmit = NULL;
    }
    if (m_pImport) {
        m_pImport->Release();
        m_pImport = NULL;
    }
    if (m_CAs.Count()) {
        for (DWORD l = 0; l < m_CAs.Count(); l++) {
            if (m_CAs.GetAt(l))
                delete [] m_CAs.GetAt(l)->pVal;
        }
    }
    SAFEFREE(m_Path);
    m_Name = NULL;
    SAFEFREE(m_SrcFile);
    SAFEFREE(m_modName);
    m_pError = NULL;
}

void CFile::PreClose()
{
    if (m_pEmit) {
        m_pEmit->Release();
        m_pEmit = NULL;
    }
    if (m_pImport) {
        m_pImport->Release();
        m_pImport = NULL;
    }
}

HRESULT CFile::ImportFile(DWORD *pdwCountOfTypes, CAssembly* pAEmit)
{
    ASSERT(m_pImport != NULL || (m_isDeclared && m_isCTDeclared));
    HRESULT hr = S_OK;

    if ( !m_isDeclared) {
        HCORENUM enumTypes;
        mdTypeDef typedefs[32];
        ULONG cTypedefs, iTypedef;
        Type *newType;
        DWORD dwFlags = 0;
        mdToken tkParent;
    
        m_Types.ClearAll();
        m_InternalTypes.ClearAll();

        // Enumeration all the types in the scope.
        enumTypes = 0;
        do {
            // Get next batch of types.
            hr = m_pImport->EnumTypeDefs(&enumTypes, typedefs, lengthof(typedefs), &cTypedefs);
            if (FAILED(hr))
                break;

            // Process each type.
            for (iTypedef = 0; iTypedef < cTypedefs; ++iTypedef) {
                newType = NULL;
                tkParent = 0;
                dwFlags = 0;

                hr = m_pImport->GetTypeDefProps( typedefs[iTypedef], NULL, 0, NULL, &dwFlags, NULL);
                if (FAILED(hr)) // Don't ReportError because this is a COM+ error!
                    break;

                // We assume that metadata always gives us stuff back in token order!
                ASSERT(m_Types.Count() == 0 || typedefs[iTypedef] > m_Types.GetAt(m_Types.Count()-1)->TypeDef);
                ASSERT(m_InternalTypes.Count() == 0 || typedefs[iTypedef] > m_InternalTypes.GetAt(m_InternalTypes.Count()-1)->TypeDef);

                if (IsTdNested(dwFlags) && FAILED(hr = m_pImport->GetNestedClassProps( typedefs[iTypedef], &tkParent))) {
                    // Don't ReportError because this is a COM+ error!
                    break;
                }
                if (!IsTdNestedPrivate(dwFlags)) {
                    // Capture all the non-private types so that we can later emit them
                    // if this assembly has a friend ;)

                    if ((IsTdPublic(dwFlags) || IsTdNestedPublic(dwFlags) ||
                        IsTdNestedFamily(dwFlags) || IsTdNestedFamORAssem(dwFlags)) ||
                        (m_isUBM && (IsTdNestedAssembly(dwFlags) || IsTdNestedFamANDAssem(dwFlags) || IsTdNotPublic(dwFlags))))
                    {
                        hr = m_Types.Add( NULL, &newType);
                    }
                    else
                    {
                        hr = m_InternalTypes.Add( NULL, &newType);
                    }

                    if (FAILED(hr)) {
                        hr = ReportError(hr); // This always E_OUTOFMEMORY?
                        break;
                    }
                    newType->TypeDef = typedefs[iTypedef];
                    newType->ParentTypeDef = tkParent;
                    newType->ComType = mdExportedTypeNil;
                }
            }
        } while (cTypedefs > 0 && SUCCEEDED(hr));
    
        m_pImport->CloseEnum(enumTypes);
    } // if (!m_isDeclared)

    if (SUCCEEDED(hr)) {
        m_isDeclared = true;

        if (!m_isCTDeclared || pAEmit)
            hr = ImportCAs(pAEmit);

        if (pAEmit && SUCCEEDED(hr)) {
            // Don't emit the internal types into the Exported type table just yet,
            // wait until we know if the assembly has any friends
            hr = EmitComTypes( pAEmit, false);
        }

        if (SUCCEEDED(hr)) {
            if (pdwCountOfTypes)
                *pdwCountOfTypes = m_Types.Count();
            // ImportCAs can return S_FALSE indicating there are no
            // Custom Attributes, but we only want to return S_FALSE
            // if there are no Types!
            hr = m_Types.Count() > 0 ? S_OK : S_FALSE;
        } else if (pdwCountOfTypes) {
            // Set this to 0 for the error case
            *pdwCountOfTypes = 0;
        }
    }
    
    return hr;
}

HRESULT CFile::EmitComTypes(CAssembly* pAEmit, bool fEmitInternals)
{
    HRESULT hr = S_FALSE;      // result will be S_FALSE if we've already emitted types and there is nothing new to emit.
    ASSERT(pAEmit);

    // Propegate any type forwarders found on this module to the assembly.
    if (!m_bEmittedTypeForwarders) {
        hr = CopyTypeForwarders(pAEmit);
        if (SUCCEEDED(hr))
            m_bEmittedTypeForwarders = true;
    }

    if (SUCCEEDED(hr) && m_Types.Count() > 0 && !m_bEmittedPublicTypes) {
        ULONG iTypedef;
		m_bEmittedPublicTypes = true;
        for (iTypedef = 0; iTypedef < m_Types.Count(); iTypedef++) {
            hr = EmitComType( pAEmit, m_Types.GetAt(iTypedef), fEmitInternals);
			if (hr != S_OK)
				m_bEmittedPublicTypes = false;
            if (FAILED(hr))
                break;
        }
    }

    if (fEmitInternals && SUCCEEDED(hr) && !m_bEmittedInternalTypes) {
        ULONG iTypedef;
        ASSERT(!m_bEmittedInternalTypes);
		m_bEmittedInternalTypes = true;
        for (iTypedef = 0; iTypedef < m_InternalTypes.Count(); iTypedef++) {
            hr = EmitComType( pAEmit, m_InternalTypes.GetAt(iTypedef), true);
			if (hr != S_OK)
				m_bEmittedInternalTypes = false;
            if (FAILED(hr))
                break;
        }
    }

    return hr;
}

HRESULT CFile::EmitComType(CAssembly* pAEmit, Type * type, bool fEmitInternals)
{
    DWORD dwFlags = 0;
    ULONG cchName;
    WCHAR name[MAX_IDENT_LEN];
    mdToken tkParent = mdTokenNil;
    HRESULT hr = E_FAIL;

    if (type->ComType == (mdToken)-1) // This means we've recursed, which means illegal metadata!
        return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);

    if (!IsNilToken(type->ComType))
        return S_OK;

    type->ComType = (mdToken)-1; // Set this to detect recursion


    hr = m_pImport->GetTypeDefProps( type->TypeDef, name, lengthof(name), &cchName, &dwFlags, NULL);
    if (FAILED(hr)) // Don't ReportError because this is a COM+ error!
        return hr;

    if (type->ParentTypeDef != 0) {
        Type t;
        t.TypeDef = type->ParentTypeDef;
        // This assumes that types are imported in token order
        Type * newType = (Type*)bsearch( &t, m_Types.Base(), m_Types.Count(), sizeof(t), (int (__cdecl*)(const void*,const void*))Type::CompareTypeDefs);
        if (newType == NULL && fEmitInternals) {
            // if the parent isn't in the public types list, check the internals type list
            newType = (Type*)bsearch( &t, m_InternalTypes.Base(), m_InternalTypes.Count(), sizeof(t), (int (__cdecl*)(const void*,const void*))Type::CompareTypeDefs);
        }

        // Assume that if the parent doesn't exist, it's because it's non-public
        // so this must be non-public
        if (newType != NULL) {
            hr = EmitComType( pAEmit, newType, fEmitInternals);
            if (FAILED(hr))
                return hr;
            if (hr == S_FALSE || IsNilToken(newType->ComType)) {
                type->ComType = mdExportedTypeNil;
                return S_FALSE;
            }
            tkParent = newType->ComType;
            newType = NULL;
        } else {
            type->ComType = mdExportedTypeNil;
            return S_FALSE;
        }
    } else {
        tkParent = m_tkFile;
    }

    if ((IsTdPublic(dwFlags) || IsTdNestedPublic(dwFlags) || IsTdNestedFamily(dwFlags) || IsTdNestedFamORAssem(dwFlags)) ||
        (fEmitInternals && (IsTdNestedAssembly(dwFlags) || IsTdNestedFamANDAssem(dwFlags) || IsTdNotPublic(dwFlags))))
    {
        hr = pAEmit->AddExportType( tkParent, type->TypeDef, name, dwFlags, &type->ComType);
        // Don't ReportError because AddExportType did!
    } 
    else
    {
        // Not externally visible, so hide it
        type->ComType = mdExportedTypeNil;
        hr = S_FALSE;
    }
    
    return hr;
}

// Copy this type forwarder from it's current scope and emit to the output assembly.
// Note:  This will not emit if the exported type is not a type forwarder to another assembly.  For any
// exported types defined in the module scope, it will not emit them and will return S_FALSE instead.
HRESULT CAssembly::DupTypeForwarder(mdExportedType *tkExportedType, IMetaDataAssemblyImport* pAImport)
{
    HRESULT hr;
    WCHAR wszTypeName[MAX_CLASS_NAME];
    ULONG cchName = 0;
    mdToken tkImplementation;
    mdToken tkTarget;
    mdTypeDef tkTypeDef;
    DWORD flags;

    IfFailGo(pAImport->GetExportedTypeProps(*tkExportedType, wszTypeName, lengthof(wszTypeName), &cchName, &tkImplementation, &tkTypeDef, &flags));

    // We only want to propagate type forwarders from the modules to the assembly, nothing else in the exported type table because that table is
    // undefined (ignored) by the CLR.

    // The type forwarders spec says type forwarders should have the tdForwarder flag set, however, ilasm and the CLR respect
    // assembly ref targets without this bit set, so we currently allow those to pass as well.

    // Check for assemblyrefs, and exported types as targets.  Assembly refs are for the actual type forwarders, exported type
    // targets indicate a nested type which was forwarded (i.e. the exported type it points to is a type forwarder).
    tkTarget = tkImplementation;
    if (TypeFromToken(tkImplementation) == mdtAssemblyRef) {
        // Add this to the output's assembly references, if it has not already been added.
        IfFailGo(DupAssemblyRef(&tkTarget, pAImport));
    } else if (TypeFromToken(tkImplementation) != mdtExportedType) {
        return S_FALSE;
    }

    IfFailGo(AddExportType( tkTarget, tkTypeDef, wszTypeName, flags, tkExportedType));

ErrExit:
    if (FAILED(hr))
        tkExportedType = mdTokenNil;

    return hr;
}

// Import type forwarders from the current module, and emit them to the assembly's exported type table.
HRESULT CFile::CopyTypeForwarders(CAssembly *pAEmit)
{
    HRESULT hr;
    CComPtr<IMetaDataAssemblyImport> pAImport = NULL;
    if (FAILED(hr = GetAssemblyImporter(pAImport)))
        return hr;

    HCORENUM enumExportedTypes = NULL;
    mdExportedType mdExportedTypes[32];
    ULONG cTypes;
    do {
        if (FAILED(hr = pAImport->EnumExportedTypes(&enumExportedTypes, mdExportedTypes, lengthof(mdExportedTypes), &cTypes)))
            break;

        for (ULONG index = 0; index < cTypes; index++)
            if (FAILED(hr = pAEmit->DupTypeForwarder(&mdExportedTypes[index], pAImport)))
                return hr;

    } while (cTypes != 0);

    return hr;
}

HRESULT CFile::ImportResources(CAssembly* pAEmit)
{
    ASSERT(m_pImport != NULL && pAEmit != NULL);

    HRESULT hr = S_OK;
    HCORENUM enumRes;
    mdManifestResource res[32];
    ULONG cRes, iRes;
    DWORD dwFlags = 0, dwOffset = 0;
    mdToken tkLoc;

    CComPtr<IMetaDataAssemblyImport> pAImport = NULL;
    if (FAILED(hr = GetAssemblyImporter(pAImport)))
        // Don't ReportError because this is a COM+ error!
        return hr;
    
    // Enumeration all the resources in the scope.
    enumRes = 0;
    do {
        // Get next batch of resources.
        hr = pAImport->EnumManifestResources(&enumRes, res, lengthof(res), &cRes);
        if (FAILED(hr))
            // Don't ReportError because this is a COM+ error!
            break;

        LPWSTR name;
        DWORD cchName;

        // Process each type.
        for (iRes = 0; iRes < cRes; ++iRes) {
            hr = pAImport->GetManifestResourceProps( res[iRes], NULL, 0, &cchName,
                &tkLoc, &dwOffset, &dwFlags);
            if (FAILED(hr))
                // Don't ReportError because this is a COM+ error!
                break;
            ASSERT (IsNilToken(tkLoc));
            if ((name = new WCHAR[++cchName]) == NULL)
                break;

            hr = pAImport->GetManifestResourceProps( res[iRes], name, cchName, &cchName,
                NULL, NULL, NULL);
            if (FAILED(hr)) {
                // Don't ReportError because this is a COM+ error!
                delete [] name;
                break;
            }

            hr = pAEmit->DefineManifestResource( pAEmit->GetEmitter(), m_tkFile, name, dwOffset, dwFlags);
            delete [] name;
            if (FAILED(hr))
                // Don't ReportError because this is a COM+ error!
                break;
        }
    } while (cRes > 0 && SUCCEEDED(hr));
    
    pAImport->CloseEnum(enumRes);

    return hr;
}

HRESULT CFile::ImportCAs(CAssembly* pAEmit)
{
    HRESULT hr = S_OK;
    bool bReport = false;
    ASSERT(m_pImport != NULL || m_isCTDeclared);

    if (m_isCTDeclared) {
        if (pAEmit == NULL)
            return m_CAs.Count() == 0 ? S_FALSE : S_OK;
        else {
            CA *ca;
            DWORD i, j;
            
            for (i = 0, j = m_CAs.Count(); i < j && SUCCEEDED(hr); i++) {
                ca = m_CAs.GetAt(i);
                hr = pAEmit->AddCustomAttribute(ca->tkType, ca->pVal, ca->cbVal, ca->bSecurity, ca->bAllowMulti, this);
                if (hr == S_FALSE) {
                    // This means the CA over-wrote an assembly bit previously set
                    // we must report the warning
                    LPWSTR name;
                    int opt;
                    GetCAName(ca->tkType, &name);
                    opt = FindOption(name);
                    if (opt != optLastPriAssemOption)
                        ReportError(WRN_OptionConflicts, opt, &bReport, ErrorOptionName((AssemblyOptions)opt));
                    delete [] name;
                }
            }
            return hr;
        }
    }

    mdToken tkAssem, tkAR;
    const static WCHAR names [4][3] = {  {0,0,0}, {'S', 0, 0}, {'M', 0, 0}, {'S', 'M', 0} };
    const static BOOL vals[4][2] = { {FALSE, FALSE}, {TRUE, FALSE}, {FALSE, TRUE}, {TRUE, TRUE} };

    hr = FindStdLibRef(&tkAR);

    for (int i = 0; i < 4 && SUCCEEDED(hr); i++) {
        HRESULT hr2;
        WCHAR name[1024];
        hr2 = StringCchCopyW(name, lengthof (name), MODULE_CA_LOCATION);
        ASSERT (SUCCEEDED (hr2));
        hr2 = StringCchCatW (name, lengthof (name), names[i]);
        ASSERT (SUCCEEDED (hr2));
        // Try both a scoped and unscoped reference
        if (FAILED(hr = m_pImport->FindTypeRef(tkAR, name, &tkAssem)) &&
            (hr != CLDB_E_RECORD_NOTFOUND || tkAR == mdTokenNil || FAILED(hr = m_pImport->FindTypeRef(mdTokenNil, name, &tkAssem)))) {

            if (hr == CLDB_E_RECORD_NOTFOUND) {
                hr = S_FALSE;
            }
            continue;
        }

        if (IsNilToken(tkAssem)) {
            hr = S_OK;
            continue;
        }

        hr = ReadCAs( tkAssem, pAEmit, vals[i][1], vals[i][0]);
    }

    if (SUCCEEDED(hr))
        m_isCTDeclared = true;

    return hr;
}

HRESULT CFile::ReadCAs(mdToken tkParent, CAssembly* pAEmit, BOOL bMulti, BOOL bSecurity)
{
    HRESULT hr = S_OK;

    HCORENUM enumCAs = NULL;
    mdCustomAttribute CAs[32];
    ULONG cCAs, iCAs;
    mdToken tkType;
    bool bReport = false;

    // Enumeration all the CAs in the scope.
    do {
        // Get next batch of CAs.
        hr = m_pImport->EnumCustomAttributes(&enumCAs, tkParent, 0, CAs, lengthof(CAs), &cCAs);
        if (FAILED(hr))
            // Don't ReportError because this is a COM+ error!
            break;

        LPVOID pVal;
        DWORD cbVal;

        // Process each CA.
        for (iCAs = 0; iCAs < cCAs; ++iCAs) {
            hr = m_pImport->GetCustomAttributeProps(CAs[iCAs], NULL, &tkType, (const void**)&pVal, &cbVal);
            if (FAILED(hr))
                // Don't ReportError because this is a COM+ error!
                break;
            if (pAEmit) {
                hr = pAEmit->AddCustomAttribute( tkType, pVal, cbVal, bSecurity, bMulti, this);
                if (hr == S_FALSE) {
                    // This means the CA over-wrote an assembly bit previously set
                    // we must report the warning
                    LPWSTR name;
                    int opt;
                    GetCAName(tkType, &name);
                    opt = FindOption(name);
                    if (opt != optLastPriAssemOption)
                        ReportError(WRN_OptionConflicts, opt, &bReport, ErrorOptionName((AssemblyOptions)opt));
                    delete [] name;
                }
            } else {
                CA *ca;
                // Allocate memory up front so we can clean up properly
                BYTE * pb = new BYTE[cbVal];
                if (pb != NULL)
                    hr = m_CAs.Add( NULL, &ca);
                else
                    hr = E_OUTOFMEMORY;
                if (SUCCEEDED(hr) && SUCCEEDED(hr = GetCustomAttributeCompareToken( tkType, &ca->tkCompare))) {
                    ca->tkCA = CAs[iCAs];
                    ca->cbVal = cbVal;
                    ca->tkType = tkType;
                    ca->bSecurity = (bSecurity != FALSE);
                    ca->bAllowMulti = (bMulti != FALSE);
                    ca->pVal = pb;
                    memcpy(pb, pVal, cbVal);
                } else {
                    if (pb != NULL)
                        delete [] pb;
                    // Report this one
                    hr = ReportError(hr);
                }
            }
            if (FAILED(hr))
                // Don't ReportError because this is a COM+ error!
                break;
        }
    } while (cCAs > 0 && SUCCEEDED(hr));

    m_pImport->CloseEnum(enumCAs);
    return hr;
}

// Attempt to find an IMetaDataAssemblyImport from either the emitter or the importer for this module.
HRESULT CFile::GetAssemblyImporter(CComPtr<IMetaDataAssemblyImport> &pAImport)
{
    HRESULT hr = S_OK;
    if (!m_pAImportCached) {

        // We test both the emitter and importer because this CFile object could have been created with
        // one or the other (not guaranteed to have both).
        hr = E_FAIL;
        if (m_pEmit)
            hr = m_pEmit->QueryInterface(IID_IMetaDataAssemblyImport, (void**)&m_pAImportCached);
        if (FAILED(hr) && m_pImport)
            hr = m_pImport->QueryInterface(IID_IMetaDataAssemblyImport, (void**)&m_pAImportCached);
    }
    pAImport = m_pAImportCached;
    return hr;
}

HRESULT CFile::GetType(DWORD index, mdTypeDef* pType)
{
    if (index >= m_Types.Count()) {
        index -= m_Types.Count();
        if (index >= m_InternalTypes.Count())
            return E_INVALIDARG;
        *pType = m_InternalTypes.GetAt(index)->TypeDef;
    } else {
        *pType = m_Types.GetAt(index)->TypeDef;
    }
    return S_OK;
}

HRESULT CFile::GetComType(DWORD index, mdExportedType* pType)
{
    if (index >= m_Types.Count()) {
        index -= m_Types.Count();
        if (index >= m_InternalTypes.Count())
            return E_INVALIDARG;
        *pType = m_InternalTypes.GetAt(index)->ComType;
    } else {
        *pType = m_Types.GetAt(index)->ComType;
    }
    return S_OK;
}

HRESULT CFile::GetCA(DWORD index, mdToken filter, mdCustomAttribute* pCA)
{
    if (index >= m_CAs.Count())
        return E_INVALIDARG;
    CA *ca = m_CAs.GetAt(index);
    if (filter == 0 || ca->tkType == filter) {
        *pCA = m_CAs.GetAt(index)->tkCA;
        return S_OK;
    } else {
        *pCA = NULL;
        return S_FALSE;
    }

}

HRESULT CFile::GetStdLibRef(mdAssemblyRef *tkRef)
{
    if (!tkRef)
        return E_POINTER;
    if (!IsNilToken(m_tkMscorlibRef)) {
        *tkRef = m_tkMscorlibRef;
        return S_OK;
    }

    CAssembly *pAsm = m_pLinker->GetStdLib();
    if (pAsm)
        return pAsm->MakeAssemblyRef(m_pEmit, tkRef);

    HRESULT hr = FindStdLibRef(tkRef);
    if (hr == S_FALSE) {
        ASSERT(*tkRef == mdTokenNil);
        return E_FAIL;
    }
    return hr;
}

HRESULT CFile::FindStdLibRef(mdAssemblyRef *tkRef)
{
    if (!tkRef)
        return E_POINTER;
    if (!IsNilToken(m_tkMscorlibRef)) {
        *tkRef = m_tkMscorlibRef;
        return S_OK;
    }

    HRESULT hr;
    CComPtr<IMetaDataAssemblyImport> pAImport;    
    if (FAILED(hr = GetAssemblyImporter(pAImport)))
        return hr;

    ULONG count;
    mdAssemblyRef ar[4];
    HCORENUM hEnum = NULL;

    while (SUCCEEDED(pAImport->EnumAssemblyRefs( &hEnum, ar, lengthof(ar), &count)) && count > 0) {
        ULONG len = 0;
        WCHAR buffer[32];
        const void * pbPubKey = NULL;
        ULONG cbPubKey = 0;
        DWORD dwFlags = 0; 

        for (ULONG i = 0; i < count; i++) {
            if (FAILED(hr = pAImport->GetAssemblyRefProps( ar[i], &pbPubKey, &cbPubKey, buffer, lengthof(buffer), &len, NULL, NULL, NULL, &dwFlags))) {
                pAImport->CloseEnum(hEnum);
                return hr;
            }
           
            if (len == 9 && wcscmp(buffer, L"mscorlib") == 0 && pbPubKey != NULL) {
                if ((!!(dwFlags & afPublicKey) && cbPubKey == sizeof(pbMscorlibKey) &&
                    memcmp( pbPubKey, pbMscorlibKey, cbPubKey) == 0) ||
                    (!(dwFlags & afPublicKey) && cbPubKey == sizeof(pbMscorlibToken) &&
                    memcmp( pbPubKey, pbMscorlibToken, cbPubKey) == 0))
                {
                    *tkRef = ar[i];
                    pAImport->CloseEnum(hEnum);
                    return S_OK;
                }
            }
        }
    }

    pAImport->CloseEnum(hEnum);
    *tkRef = mdTokenNil;
    return S_FALSE;
}

// Returns a TypeRef or TypeDef token for the given typename
// If needed, it will create a TypeRef
// TypeRefs will always be scoped by an AssemblyRef to MSCORLIB
HRESULT CFile::GetTypeRef(LPCWSTR wszTypeName, mdToken * tkType)
{
    HRESULT hr;
    mdAssemblyRef tkAR = m_tkMscorlibRef;

    if (IsNilToken(tkAR) && FAILED(hr = GetStdLibRef(&tkAR)))
        return hr;
    else
        hr = S_OK;

    if (!m_pImport && !m_pEmit)
        return E_FAIL;
    if (!m_pImport && FAILED(hr = m_pEmit->QueryInterface(IID_IMetaDataImport, (void**)&m_pImport)))
        return hr;

    // Find or Make the TypeRef
    hr = m_pImport->FindTypeRef(tkAR, wszTypeName, tkType);
    if (hr == CLDB_E_RECORD_NOTFOUND)
         hr = m_pImport->FindTypeDefByName( wszTypeName, mdTokenNil, tkType);
    if (hr == CLDB_E_RECORD_NOTFOUND && m_pEmit)
         hr = m_pEmit->DefineTypeRefByName( tkAR, wszTypeName, tkType);

    return hr;
}

void CFile::SetPEKind(DWORD dwPEKind, DWORD dwMachine)
{
    m_dwPEKind = dwPEKind;
    m_dwMachine = dwMachine;
    m_bHasPE = true;
}

HRESULT CFile::GetPEKind(DWORD *pdwPEKind, DWORD *pdwMachine)
{
    if (!m_bHasPE) {
        HRESULT hr = E_FAIL;
        DWORD dwPEKind = peNot, dwMachine = 0;
        bool bHasPE = false;

        if (HasMetaData()) {
            CComPtr<IMetaDataImport2> imdi2;

            ASSERT(m_pImport || m_pEmit);
            if (m_pImport)
                hr = m_pImport->QueryInterface( IID_IMetaDataImport2, (void**)&imdi2);
            if (FAILED(hr) && m_pEmit)
                hr = m_pEmit->QueryInterface( IID_IMetaDataImport2, (void**)&imdi2);
            if (SUCCEEDED(hr)) {
                if (FAILED(hr = imdi2->GetPEKind( &dwPEKind, &dwMachine)))
                    return hr;
                if (hr == S_OK)
                    bHasPE = true;
            }
        }

        if (m_Path && !bHasPE && !InMemory()) {
            if (FAILED(hr = PEFile::GetPEKind(m_Path, &dwPEKind, &dwMachine)))
                return hr = ReportError(hr);
            bHasPE = true;
        }

        m_dwPEKind = dwPEKind;
        m_dwMachine = dwMachine;
        m_bHasPE = bHasPE;
    }

    if (pdwPEKind != NULL)
        *pdwPEKind = m_dwPEKind;
    if (pdwMachine != NULL)
        *pdwMachine = m_dwMachine;

    return m_bHasPE ? S_OK : S_FALSE;
}

HRESULT CFile::AddResource(mdToken Location, LPCWSTR pszName, DWORD offset, DWORD flags)
{
    HRESULT hr;
    CComPtr<IMetaDataAssemblyEmit> pAEmit = NULL;

    hr = m_pEmit->QueryInterface( IID_IMetaDataAssemblyEmit, (void**)&pAEmit);
    if (SUCCEEDED(hr))
        hr = DefineManifestResource( pAEmit, Location, pszName, offset, flags);
    // Don't ReportError because this is a COM+ error!

    return hr;
}

HRESULT CFile::DefineManifestResource(IMetaDataAssemblyEmit* pAEmit, mdToken Location, LPCWSTR pszName, DWORD offset, DWORD flags)
{
    HRESULT hr;
    mdManifestResource mr = mdTokenNil;

    hr = pAEmit->DefineManifestResource( pszName, Location, offset, flags, &mr);
    // Don't ReportError because this is a COM+ error!

    if (hr == META_S_DUPLICATE) {
        mdToken tkOtherLocation;
        DWORD otherFlags;

        CComPtr<IMetaDataAssemblyImport> pAImport;        
        if (FAILED(hr = GetAssemblyImporter(pAImport)))
            return hr;

        hr = pAImport->GetManifestResourceProps( mr, NULL, 0, NULL, &tkOtherLocation, NULL, &otherFlags);
        if (FAILED(hr))
            return hr;

        // if the original definition is different we need to report an error
        if (tkOtherLocation != Location || flags != otherFlags)
        {
            bool bReport = false;
            hr = ReportError(ERR_DupResourceIdent, mr, &bReport, pszName);
            if (bReport) // If we were able to report it through the callback, continue as if it succeeded
                hr = S_OK;
        }
        else
        {
            // Everything matches up so let the caller now it's a dupe
            hr = META_S_DUPLICATE;
        }
    }

    return hr;
}


HRESULT CFile::AddCustomAttribute(mdToken tkType, const void* pValue, DWORD cbValue, BOOL bSecurity, BOOL bAllowMultiple, CFile *source)
{
    ASSERT(m_pEmit != NULL && (source == this || source == NULL));
    if (IsNilToken(m_tkAssemblyAttrib[bSecurity][bAllowMultiple])) {
        ASSERT(m_isUBM);
        HRESULT hr;
        WCHAR name[1024];
        if (FAILED (hr = StringCchCopyW (name, lengthof (name), MODULE_CA_LOCATION))) {
            return hr;
        }
        if (IsNilToken(m_tkMscorlibRef) && FAILED(hr = GetStdLibRef(&m_tkMscorlibRef)))
            return hr;

        // two cats in a row isn't the speediest code in the world - something
        // to change if this function is performance critical.
        if (bSecurity != FALSE)
            if (FAILED (hr = StringCchCatW (name, lengthof (name), L"S")))
                return hr;
        if (bAllowMultiple != FALSE)
            if (FAILED (hr = StringCchCatW (name, lengthof (name), L"M")))
                return hr;
        if (FAILED(hr = GetTypeRef(name, &m_tkAssemblyAttrib[bSecurity][bAllowMultiple])))
            // Don't ReportError because this is a COM+ error!
            return hr;
    }
    return m_pEmit->DefineCustomAttribute( m_tkAssemblyAttrib[bSecurity][bAllowMultiple], tkType, pValue, cbValue, NULL);
}

HRESULT CFile::MakeModuleRef(IMetaDataEmit *pEmit, mdModuleRef *pmr)
{
    return pEmit->DefineModuleRef(GetModuleName(), pmr);
}

HRESULT CFile::ReportError(HRESULT hr, mdToken tkError, bool * pbReport)
{
    if (hr == S_OK || hr == S_FALSE)
        return hr;

    HRESULT hr2;
    CComPtr<ICreateErrorInfo>   cerr;
    CComPtr<IErrorInfo>         err;

    if (tkError == mdTokenNil)
        tkError = m_tkError;

    if (SUCCEEDED(hr2 = CreateErrorInfo( &cerr))) {
        VERIFY(SUCCEEDED(hr2 = cerr->SetGUID(IID_IALink)));
        ASSERT(HIWORD(hr) != 0);
        // this is a normal HRESULT
        VERIFY(SUCCEEDED(hr2 = cerr->SetDescription((LPWSTR)ErrorHR(hr))));

        VERIFY(SUCCEEDED(hr2 = cerr->QueryInterface(IID_IErrorInfo, (void**)&err)));
    }
    SetErrorInfo( 0, err);

    if (pbReport != NULL) {
        if (m_pError != NULL)
            *pbReport = (S_OK == m_pError->OnError(hr, tkError));
        else
            *pbReport = false;
    }

    return hr;
}

HRESULT CFile::ReportError(ERRORS err, mdToken tkError, bool * pbReport, ...)
{
    ASSERT(HIWORD(err) == 0);

    HRESULT hr2, hr = E_FAIL;
    CComPtr<ICreateErrorInfo>   cerr;
    CComPtr<IErrorInfo>         error;
    CComBSTR                    msg;

    if (tkError == mdTokenNil)
        tkError = m_tkError;

    if (SUCCEEDED(hr2 = CreateErrorInfo( &cerr))) {
        VERIFY(SUCCEEDED(hr2 = cerr->SetGUID(IID_IALink)));
        // This one of our errors
        va_list args;
        va_start( args, pbReport);
        msg.Attach(ErrorMessage(ERROR_DEFS[err][2], args));
        VERIFY(SUCCEEDED(hr2 = cerr->SetDescription(msg)));
        va_end(args);
        hr = MAKE_HRESULT(((ERROR_DEFS[err][1] < 1) ? SEVERITY_ERROR : SEVERITY_SUCCESS), FACILITY_ITF, ERROR_DEFS[err][0] & 0x0000FFFF);

        VERIFY(SUCCEEDED(hr2 = cerr->QueryInterface(IID_IErrorInfo, (void**)&error)));
    }
    SetErrorInfo( 0, error);

    if (pbReport != NULL) {
        if (m_pError != NULL)
            *pbReport = (S_OK == m_pError->OnError(hr, tkError));
        else
            *pbReport = false;
    }

    return hr;
}


/*
 * Get a string (PWSTR) for an HRESULT (Attempt to use GetErrorInfo)
 */
LPCWSTR CFile::ErrorHR(HRESULT hr, IUnknown *pUnk, REFIID iid)
{
    CComPtr<ISupportErrorInfo> pErrInfo;
    static WCHAR buffer[2048];
    bool UseErrorInfo = false;

    if (pUnk && SUCCEEDED(pUnk->QueryInterface(IID_ISupportErrorInfo, (void**)&pErrInfo)) &&
        pErrInfo &&
        S_OK == pErrInfo->InterfaceSupportsErrorInfo( iid))
        UseErrorInfo = true;

    if (!UseErrorInfo && iid == IID_IMetaDataEmit)
        UseErrorInfo = true;

    // See if there is more detailed error message available via GetErrorInfo.
    CComPtr<IErrorInfo> err;
    CComBSTR str;
    GUID guid;

    if (S_OK == GetErrorInfo( 0, &err) && err != NULL &&
        (UseErrorInfo || (SUCCEEDED(err->GetGUID( &guid)) && guid == iid)) &&
        SUCCEEDED(err->GetDescription( &str)) &&
        SysStringLen(str) < lengthof(buffer)) {
            memcpy(buffer, str, SysStringLen(str) * sizeof(WCHAR));
            return buffer;
    }
    return ErrorHR(hr);
}
        

/*
 * Get a string (PWSTR) for an HRESULT
 */
LPCWSTR CFile::ErrorHR(HRESULT hr)
{
    static WCHAR buffer[2048];
    int r;

    DWORD dwFormatMessageFlags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

    // Use FormatMessage to get the error string for the HRESULT from the system.
    r = FormatMessageW(dwFormatMessageFlags,
                        NULL, hr, 0,
                        buffer, lengthof(buffer),
                        NULL);


    // Check for errors, and possibly give an 'UnknownError
    if (r == 0)
        return UnknownError(buffer, 2048, hr);

    return buffer;
}

LPWSTR UnknownErrorHelper(__inout_ecount(cchMax) LPWSTR buffer, size_t cchMax, ...)
{
    HINSTANCE hRes = GetALinkMessageDll();
    ASSERT(cchMax <= ULONG_MAX);

    // Load the message and fill in arguments. Try using Unicode function first,
    // then back off to ANSI.
    va_list args;
    va_start(args, cchMax); // We assume that the 1 and only argument is an HRESULT
    if (!W_LoadAndFormat (hRes, IDS_UnknownError, args, buffer, cchMax))
    {
        // Not a lot we can do if we can't report an error. Assert in debug so we know
        // what is going on.
        VSFAIL("FormatMessage failed to load error message");
        buffer[0] = L'\0';
    }
    va_end(args);

    // This will probably fail if we're in low-memory conditions.
    return buffer;
}

LPWSTR CFile::UnknownError(__inout_ecount(cchMax) LPWSTR buffer, size_t cchMax, HRESULT hr)
{
    return UnknownErrorHelper(buffer, cchMax, hr);
}

BSTR CFile::ErrorMessage(DWORD err_id, va_list args)
{
    WCHAR pszBuffer[2048];
    HINSTANCE hRes = GetALinkMessageDll();

    // Load the message and fill in arguments. Try using Unicode function first,
    // then back off to ANSI.
    if (!W_LoadAndFormat (hRes, err_id, args, pszBuffer, 2048))
    {
        // Not a lot we can do if we can't report an error. Assert in debug so we know
        // what is going on.
        VSFAIL("FormatMessage failed to load error message");
        pszBuffer[0] = L'\0';
    }

    // This will probably fail if we're in low-memory conditions.
    return SysAllocString(pszBuffer);
}


HRESULT CFile::SetSource(LPCWSTR pszSourceFile)
{
    if (m_SrcFile)
        delete [] m_SrcFile;

    return (m_SrcFile = VSAllocStr(pszSourceFile)) == NULL ? E_OUTOFMEMORY : S_OK;
}

HRESULT CFile::CopyFile(IMetaDataDispenserEx *pDisp, bool bCleanup)
{
    HRESULT hr = S_OK;
    if (m_SrcFile != NULL && _wcsicmp(m_SrcFile, m_Path) != 0) {
        if (W_CopyFile(m_SrcFile, m_Path, FALSE) == FALSE)
            return ReportError(HRESULT_FROM_WIN32(GetLastError()));
        bCleanup = true;
    } else {
        hr = S_FALSE; // FALSE because we did nothing
        if (!bCleanup && !m_bNoMetaData) {
            HRESULT hr2;
            // We need to check if the ModuleDef record matches the actual filename
            if (m_pImport == NULL) {
                if (m_pEmit == NULL)
                    return E_FAIL;
                if(FAILED(hr2 = m_pEmit->QueryInterface(IID_IMetaDataImport, (void**)&m_pImport)))
                    return hr2;
            }
            ULONG chName = 0;
            if (FAILED(hr2 = m_pImport->GetScopeProps( NULL, 0, &chName, NULL)) || chName == 0)
                return hr2;
            LPWSTR szModName = (LPWSTR)_alloca(sizeof(WCHAR) * chName); // chName includes NULL character
            if (FAILED(hr2 = m_pImport->GetScopeProps( szModName, chName, &chName, NULL)))
                return hr2;
            if (_wcsicmp( szModName, GetModuleName()) != 0) {
                bool fReport = false;
                hr2 = ReportError( ERR_ModuleNameDifferent, m_tkError, &fReport, m_Name, szModName);
                if (!fReport)
                    return hr2;
            }
        }
    }

    if (bCleanup && !m_bNoMetaData) {
        mdToken tkAR, tkAttrib[4] = {mdTokenNil, mdTokenNil, mdTokenNil, mdTokenNil };
        const static WCHAR names [4][2] = {  {0,0}, {'S', 0}, {'M', 0}, {'S', 'M'} };
        CTokenMap Map;
        PEFile module;
        CComPtr<IMetaDataEmit> pNewEmit, pOldEmit;
        CComPtr<IMetaDataAssemblyImport> pAImport;
        CComPtr<IMetaDataFilter> pFilter;
        ULONG count;
        mdAssemblyRef ar[4];
        HCORENUM hEnum = NULL;

        if (FAILED(hr = module.OpenFileAs( pDisp, m_Path, true, true)))
            // A COM+ or WIN32 error
            goto CLEANUP;

        if (FAILED(hr = pDisp->DefineScope(CLSID_CorMetaDataRuntime,   // Format of metadata
                                           0,                           // flags
                                           IID_IMetaDataEmit,           // Emitting interface
                                           (IUnknown **) &pNewEmit)))
            goto CLEANUP;

        if (FAILED(hr = module.pImport->QueryInterface(IID_IMetaDataAssemblyImport, (void**)&pAImport))||
            FAILED(hr = module.pImport->QueryInterface(IID_IMetaDataEmit, (void**)&pOldEmit)) ||
            FAILED(hr = module.pImport->QueryInterface(IID_IMetaDataFilter, (void**)&pFilter)))
            goto CLEANUP;


        while (SUCCEEDED(pAImport->EnumAssemblyRefs( &hEnum, ar, lengthof(ar), &count)) && count > 0) {
            ULONG len = 0;
            WCHAR buffer[32];
            const void * pbPubKey = NULL;
            ULONG cbPubKey = 0;
            DWORD dwFlags = 0;

            for (ULONG i = 0; i < count; i++) {
                if (FAILED(hr = pAImport->GetAssemblyRefProps( ar[i], &pbPubKey, &cbPubKey, buffer, lengthof(buffer), &len, NULL, NULL, NULL, &dwFlags))) {
                    pAImport->CloseEnum(hEnum);
                    goto CLEANUP;
                }
                if (len == 9 && wcscmp(buffer, L"mscorlib") == 0 && pbPubKey != NULL) {
                    if ((!!(dwFlags & afPublicKey) && cbPubKey == sizeof(pbMscorlibKey) &&
                        memcmp( pbPubKey, pbMscorlibKey, cbPubKey) == 0) ||
                        (!(dwFlags & afPublicKey) && cbPubKey == sizeof(pbMscorlibToken) &&
                        memcmp( pbPubKey, pbMscorlibToken, cbPubKey) == 0))
                    {
                        tkAR = ar[i];
                        pAImport->CloseEnum(hEnum);
                        goto FoundMscorLib;
                    }
                }
            }
        }
        pAImport->CloseEnum(hEnum);
        tkAR = mdTokenNil;

FoundMscorLib:
        pAImport = NULL; // Release it

        for (int i = 0; i < 4 && SUCCEEDED(hr); i++) {
            WCHAR name[1024];
            hr = StringCchCopyW(name, lengthof (name), MODULE_CA_LOCATION);
            ASSERT (SUCCEEDED (hr));
            mdToken tkAssem = mdTokenNil;
            hr = StringCchCatW(name, lengthof (name), names[i]);
            ASSERT (SUCCEEDED (hr));
            // Try both a scoped and unscoped reference
            if (FAILED(hr = module.pImport->FindTypeRef(tkAR, name, &tkAssem)) &&
                (hr != CLDB_E_RECORD_NOTFOUND || tkAR == mdTokenNil || FAILED(hr = module.pImport->FindTypeRef(mdTokenNil, name, &tkAssem)))) {
                if (hr == CLDB_E_RECORD_NOTFOUND) {
                    hr = S_FALSE;
                    continue;
                } else
                    break;
            }

            tkAttrib[i] = tkAssem;
            if (!IsNilToken(tkAssem)) {
                DeleteToken func(pOldEmit);
                hr = EnumAllExcept( module.pImport, &func, tkAssem, NULL, 0, NULL, &CallEnumCustomAttributes);
            }
        }
        pOldEmit = NULL;
        if (FAILED(hr) || FAILED(hr = pFilter->UnmarkAll()))
            goto CLEANUP;

        {
            MarkToken func(pFilter);

            if (FAILED(hr = EnumAllExcept( module.pImport, &func, &CallEnumUserStrings, NULL, 0, NULL)) ||
                FAILED(hr = EnumAllExcept( module.pImport, &func, &CallEnumSignatures, NULL, 0, NULL)) ||
                FAILED(hr = EnumAllExcept( module.pImport, &func, &CallEnumTypeDefs, NULL, 0, &CallEnumMembers)) ||
                // Get Global functions/methods
                FAILED(hr = EnumAllExcept( module.pImport, &func, mdTokenNil, NULL, 0, &CallEnumMembers)) ||
                FAILED(hr = EnumAllExcept( module.pImport, &func, &CallEnumTypeRefs, tkAttrib, lengthof(tkAttrib), &CallEnumMemberRefs)) ||
                // Get Global functions/methods
                FAILED(hr = EnumAllExcept( module.pImport, &func, mdTokenNil, tkAttrib, lengthof(tkAttrib), &CallEnumMemberRefs)) ||
                FAILED(hr = EnumAllExcept( module.pImport, &func, &CallEnumTypeSpecs, NULL, 0, NULL)))
                goto CLEANUP;
        }

        if (FAILED(hr = pNewEmit->SetModuleProps(GetModuleName())))
            goto CLEANUP;


        if (FAILED(hr = pNewEmit->Merge( module.pImport, &Map, NULL)) ||
            FAILED(hr = pNewEmit->MergeEnd()) ||
            FAILED(hr = pNewEmit->GetSaveSize((CorSaveSize)(cssAccurate | cssDiscardTransientCAs), &count)))
            goto CLEANUP;

        if (FAILED(hr = module.EmitMembers( NULL, NULL, NULL, NULL, NULL, &Map, pNewEmit)) ||
            FAILED(hr = module.WriteNewMetaData(pNewEmit, count)) ||
            FAILED(hr = module.Close()))
            hr = ReportError(hr);

CLEANUP:;
    }

    return hr;
}

HRESULT CFile::EnumAllExcept(IMetaDataImport * pImport, TokenEnumFunc * pFunc, EnumMemberFunc enumfunc, mdToken *tkExcept, size_t cExceptCount, EnumMemberNestedFunc enumnestedfunc)
{
    HRESULT     hr = E_FAIL;
    mdToken     tkParent[16];
    ULONG       cParent;
    HCORENUM    hParent = NULL;

    do {
        if (SUCCEEDED(hr = (*enumfunc)(pImport, &hParent, tkParent, lengthof(tkParent), &cParent))) {
            for (ULONG i = 0; i < cParent; i++) {
                bool bMark = true;
                if (cExceptCount > 0) {
                    ASSERT(tkExcept != NULL);
                    for (size_t e = 0; e < cExceptCount; e++) {
                        if (tkParent[i] == tkExcept[e]) {
                            bMark = false;
                            break;
                        }
                    }
                }

                if (bMark && SUCCEEDED(hr = pFunc->DoSomething(tkParent[i])) && enumnestedfunc != NULL) {
                    hr = EnumAllExcept(pImport, pFunc, tkParent[i], tkExcept, cExceptCount, enumnestedfunc);
                }
                if (FAILED(hr))
                    break;
            }
        }
    } while (hr == S_OK && cParent == lengthof(tkParent));

    return hr;
}

HRESULT CFile::EnumAllExcept(IMetaDataImport * pImport, TokenEnumFunc * pFunc, mdToken tkParent, mdToken *tkExcept, size_t cExceptCount, EnumMemberNestedFunc enumnestedfunc, EnumMemberNested2Func enumnestedfunc2)
{
    HRESULT     hr = E_FAIL;
    mdToken     tkNested[16];
    ULONG       cNested = 0;
    HCORENUM    hNested = NULL;

    do {
        if ((enumnestedfunc && SUCCEEDED(hr = (*enumnestedfunc)(pImport, &hNested, tkParent, tkNested, lengthof(tkNested), &cNested))) ||
            (enumnestedfunc2 && SUCCEEDED(hr = (*enumnestedfunc2)(pImport, &hNested, tkParent, mdTokenNil, tkNested, lengthof(tkNested), &cNested)))) {
            for (ULONG i = 0; i < cNested; i++) {
                bool bMark = true;
                if (cExceptCount > 0) {
                    ASSERT(tkExcept != NULL);
                    for (size_t e = 0; e < cExceptCount; e++) {
                        if (tkNested[i] == tkExcept[e]) {
                            bMark = false;
                            break;
                        }
                    }
                }

                if (bMark)
                    hr = pFunc->DoSomethignNested(tkParent, tkNested[i]);
                if (FAILED(hr))
                    break;
            }
        }
    } while (hr == S_OK && cNested == lengthof(tkNested));

    return hr;
}

LPCWSTR CFile::GetModuleName()
{
    if (m_modName != NULL)
        return m_modName;

    // Always use the filename for ModuleRefs to modules
    // Also assume that the ModuleDef == filename (but it doesn't really matter
    // because the ModuleDef of a module is never used)
    if ((m_modName = VSAllocStr(m_Name))) {
        return m_modName;
    }

    // Even if we run out of memory, this function should never fail
    return m_Name;
}

LPCWSTR CAssembly::GetModuleName()
{
    DWORD   chName;

    if (m_modName != NULL)
        return m_modName;

    // Use the ModuleDef for ModuleRefs to the assembly
    // If the file is being renamed, assume the ModuleDef will
    // be the same as the new filename.
    if (m_SrcFile == NULL || _wcsicmp(m_SrcFile, m_Path) == 0) {
        if (m_pImport == NULL) {
            if (m_pEmit == NULL || FAILED(m_pEmit->QueryInterface(IID_IMetaDataImport, (void**)&m_pImport)))
                goto FAIL;
        }
        if (FAILED(m_pImport->GetScopeProps( NULL, 0, &chName, NULL)) || chName == 0)
            goto FAIL;
        if ((m_modName = new WCHAR[chName]) == NULL)
            goto FAIL;
        if (SUCCEEDED(m_pImport->GetScopeProps( (LPWSTR)m_modName, chName, &chName, NULL)) &&
            chName > 0)
            return m_modName;

         delete [] m_modName;
         m_modName = NULL;
    }

FAIL:
    if ((m_modName = VSAllocStr(m_Name))) {
        return m_modName;
    }

    // Even if we run out of memory, this function should never fail
    return m_Name;
}

HRESULT CFile::UpdateModuleName()
{
    HRESULT hr = E_FAIL;
    if (m_pEmit == NULL) {
        if (m_pImport == NULL || FAILED(hr = m_pImport->QueryInterface(IID_IMetaDataEmit, (void**)&m_pEmit)))
            return hr;
    }

    return m_pEmit->SetModuleProps(GetModuleName());
}

HRESULT CFile::DupModuleRef(mdToken *tkModule, IMetaDataImport* pImport)
{
    HRESULT hr = S_OK;
    LPWSTR  name;
    DWORD   chName;
    if (FAILED(hr = pImport->GetModuleRefProps( *tkModule, NULL, 0, &chName)))
        return hr;
    if ((name = new WCHAR[chName]) == NULL)
        return ReportError(E_OUTOFMEMORY);
    if (SUCCEEDED(hr = pImport->GetModuleRefProps( *tkModule, name, chName, &chName))) {
        if (wcscmp(name, GetModuleName()) != 0) {
            hr = m_pEmit->DefineModuleRef( name, tkModule);
        } else {
            hr = S_OK;
            *tkModule = TokenFromRid(1, mdtModule);
        }
    }
    return hr;
}

CAssembly::CAssembly() : CFile(), 
    m_szAsmName(NULL),
    m_Title(NULL),
    m_Description(NULL),
    m_Company(NULL),
    m_Product(NULL),
    m_ProductVersion(NULL),
    m_FileVersion(NULL),
    m_Copyright(NULL),
    m_Trademark(NULL),
    m_KeyName(NULL),
    m_KeyFile(NULL),
    m_iAlgID(0),
    m_dwFlags(0),
    m_pbHash(NULL),
    m_cbHash(0),
    m_doHalfSign(false),
    m_hasFriend(false),
    m_isVersionSet(false),
    m_isAlgIdSet(false),
    m_isFlagsSet(false),
    m_isHalfSignSet(false),
    m_isAutoKeyName(false),
    m_bAllowExes(false),
    m_bIsInMemory(false),
    m_bClean(false),
    m_bDoHash(true),
    m_bDoDupTypeCheck(false),
    m_bDoDupeCheckTypeFwds(false),
    m_pAEmit(NULL),
    m_pAImport(NULL),
    m_pbKeyPair(NULL),
    m_cbKeyPair(0),
    m_pPubKey(NULL),
    m_cbPubKey(0),
    m_hKeyFile(INVALID_HANDLE_VALUE),
    m_hKeyMap(NULL),
    m_tkAssembly(0)
{
    memset(&m_adata, 0 ,sizeof(m_adata));
    m_isAssembly = true;
}

HRESULT CAssembly::Init(LPCWSTR pszFilename, AssemblyFlags afFlags, IMetaDataAssemblyEmit* pAEmitter, IMetaDataEmit* pEmitter, IMetaDataError *pError, CAsmLink* pLinker)
{
    HRESULT hr = CFile::Init( pszFilename, pEmitter, pError, pLinker);
    if (FAILED(hr))
        return hr;
    if (FAILED(hr = pAEmitter->QueryInterface(IID_IMetaDataAssemblyImport, (void**)&m_pAImport)))
        return hr;
    if (m_bIsInMemory = (afFlags & afInMemory))
        m_bIsExe = false;
    m_pAEmit = pAEmitter;
    m_pAEmit->AddRef();
    m_bClean = (afFlags & afCleanModules) ? true : false;
    m_isStdLib = (m_Name != NULL && _wcsicmp( m_Name, L"mscorlib.dll") == 0);
    m_bDoHash = !(afFlags & afNoRefHash);
    m_bDoDupTypeCheck  = !(afFlags & afNoDupTypeCheck);
    m_bDoDupeCheckTypeFwds = m_bDoDupTypeCheck || (afFlags & afDupeCheckTypeFwds);

    return S_OK;
}

HRESULT CAssembly::Init(IMetaDataError *pError, CAsmLink* pLinker, IMetaDataAssemblyEmit *pAEmitter)
{
    HRESULT hr = CFile::Init( L"", (IMetaDataEmit*)NULL, pError, pLinker);
    if (FAILED(hr))
        return hr;

    if (pAEmitter) {
        if (FAILED(hr = pAEmitter->QueryInterface(IID_IMetaDataAssemblyImport, (void**)&m_pAImport)))
            return hr;

        m_pAEmit = pAEmitter;
        m_pAEmit->AddRef();
    } else {
        m_pAEmit = NULL;
        m_pAImport = NULL;
    }
    m_isUBM = true;
    m_bAllowExes = true;

    return S_OK;
}

HRESULT CAssembly::Init(LPCWSTR pszPath, IMetaDataAssemblyImport* pAImporter, IMetaDataImport* pImporter, IMetaDataError *pError, CAsmLink* pLinker)
{
    HRESULT hr = CFile::Init(pszPath, pImporter, pError, pLinker);
    if (FAILED(hr))
        return hr;

    pAImporter->AddRef();
    m_pAImport = pAImporter;
    m_pAEmit = NULL;
    m_isUBM = false;
    m_isStdLib = false;
    if (m_Name != NULL && pAImporter != NULL && _wcsicmp( m_Name, L"mscorlib.dll") == 0) {
        // We have a possible mscorlib.dll
        mdAssembly tkAssembly;
        const void * pbPubKey = NULL;
        ULONG cbPubKey = NULL;

        if (SUCCEEDED(pAImporter->GetAssemblyFromScope( &tkAssembly)) &&
            SUCCEEDED(pAImporter->GetAssemblyProps( tkAssembly, &pbPubKey, &cbPubKey, NULL, NULL, 0, NULL, NULL, NULL)) &&
            cbPubKey == sizeof(pbMscorlibKey) && pbPubKey != NULL &&
            memcmp( pbPubKey, pbMscorlibKey, cbPubKey) == 0)
        {
            m_isStdLib = true;
        }
    }

    return S_OK;
}

CAssembly::~CAssembly()
{
    if (m_pAEmit) {
        m_pAEmit->Release();
        m_pAEmit = NULL;
    }
    if (m_pAImport) {
        m_pAImport->Release();
        m_pAImport = NULL;
    }

    if (m_Files.Count()) {
        for (DWORD l = 0; l < m_Files.Count(); l++) {
            if (m_Files[l])
                delete m_Files[l];
        }
    }
    CloseCryptoFile();

    SAFEFREE(m_szAsmName);
    SAFEFREE(m_pPubKey);
    m_cbPubKey = 0;
    SAFEFREE(m_adata.szLocale);
    SAFEFREE(m_adata.rOS);
    SAFEFREE(m_adata.rProcessor);
    memset(&m_adata, 0, sizeof(m_adata));
    SAFEFREE(m_Title);
    SAFEFREE(m_Description);
    SAFEFREE(m_Company);
    SAFEFREE(m_Product);
    SAFEFREE(m_ProductVersion);
    SAFEFREE(m_FileVersion);
    SAFEFREE(m_Copyright);
    SAFEFREE(m_Trademark);
    SAFEFREE(m_KeyName);
    SAFEFREE(m_KeyFile);
    SAFEFREE(m_pbHash);
    m_cbHash = 0;

}

void CAssembly::PreClose()
{
    if (m_pAEmit) {
        m_pAEmit->Release();
        m_pAEmit = NULL;
    }
    if (m_pAImport) {
        m_pAImport->Release();
        m_pAImport = NULL;
    }

    if (m_Files.Count()) {
        for (DWORD l = 0; l < m_Files.Count(); l++) {
            if (m_Files[l])
                m_Files[l]->PreClose();
        }
    }

    CFile::PreClose();
}

HRESULT CAssembly::AddFile(CFile *file, DWORD dwFlags, mdFile * tkFile)
{   
    HRESULT hr = S_OK;

    if (!m_bAllowExes && file->m_bIsExe)
        return ReportError(ERR_CantAddExes, mdTokenNil, NULL, file->m_Path);
    if (!m_isUBM && file->m_isAssembly)
        return ReportError(ERR_CantAddAssembly, mdTokenNil, NULL, file->m_Path);

    if (dwFlags & ffContainsNoMetaData)
        file->m_bNoMetaData = true;

    if (!m_isUBM) {
        // Yes this is a slow O(n^2) algorithm, but typically n will be small
        // and it's not worth the memory for anything faster.
        LPCWSTR szModuleName = file->GetModuleName();
        for (DWORD i = 0; i < m_Files.Count(); i++) {
            if (0 == _wcsicmp(m_Files[i]->GetModuleName(), szModuleName)) {
                return ReportError(ERR_DuplicateModule, m_Files[i]->m_tkFile, NULL, szModuleName);
            }
        }
    }

    // Don't emit the hash now
    if (m_pAEmit && !m_isUBM)
        hr = m_pAEmit->DefineFile(file->GetModuleName(), NULL, 0, dwFlags, &file->m_tkFile);
    if (FAILED(hr))
        return hr;

    file->m_isUBM |= m_isUBM; 
    if (FAILED(hr = m_Files.Add( file)))
        return ReportError(hr); // Out of memory

    if (tkFile)
        *tkFile = TokenFromRid( mdtFile, m_Files.Count() - 1);

    return hr;
}

HRESULT CAssembly::AddFile(CAssembly *file, DWORD dwFlags, mdFile * tkFile)
{
    HRESULT hr;
    if (SUCCEEDED(hr = AddFile((CFile*)file, dwFlags, tkFile)) && tkFile)
        *tkFile = TokenFromRid( mdtAssemblyRef, m_Files.Count() - 1);
    return hr;
}

HRESULT CAssembly::GetHash(const void ** ppvHash, DWORD *pcbHash)
{
    ASSERT( ppvHash && pcbHash);
    if (IsInMemory()) {
        // We can't hash an InMemory file
        *ppvHash = NULL;
        *pcbHash = 0;
        return S_FALSE;
    }

    if (!m_bDoHash || (m_cbHash && m_pbHash != NULL)) {
        *ppvHash = m_pbHash;
        *pcbHash = m_cbHash;
        return S_OK;
    }

    DWORD cchSize = 0, result;
    
    // AssemblyRefs ALWAYS use CALG_SHA1
    ALG_ID alg = CALG_SHA1;
    if (StrongNameHashSize( alg, &cchSize) == FALSE)
        return ReportError(StrongNameErrorInfo());

    if ((m_pbHash = new BYTE[cchSize]) == NULL)
        return ReportError(E_OUTOFMEMORY);
    m_cbHash = cchSize;
    
    if ((result = GetHashFromAssemblyFileW(m_Path, &alg, (BYTE*)m_pbHash, cchSize, &m_cbHash)) != 0) {
        delete [] m_pbHash;
        m_pbHash = 0;
        m_cbHash = 0;
    }
    *ppvHash = m_pbHash;
    *pcbHash = m_cbHash;
    
    return result == 0 ? S_OK : ReportError(HRESULT_FROM_WIN32(result));
}

HRESULT CAssembly::ImportAssembly(DWORD *pdwCountOfScopes, BOOL fImportTypes, DWORD dwOpenFlags, IMetaDataDispenserEx *pDisp)
{
    ASSERT(m_pAImport != NULL);

    HRESULT hr = S_OK;
    DWORD chName = 0;
    void *pbOrig = NULL;
    DWORD cbOrig = 0;
    DWORD dwFlags = 0;

    mdAssembly tkAsm;
    if (FAILED(hr = m_pAImport->GetAssemblyFromScope( &tkAsm)))
        // Don't ReportError because this is a COM+ error!
        return hr;
    if (FAILED(hr = m_pAImport->GetAssemblyProps( tkAsm, NULL, NULL, NULL, NULL, 0, &chName, &m_adata, &dwFlags)))
        // Don't ReportError because this is a COM+ error!
        return hr;

    m_dwFlags |= (dwFlags & ~afPublicKey);
    if ((m_szAsmName = new WCHAR[chName]) == NULL)
        return ReportError(E_OUTOFMEMORY);
    if (m_adata.cbLocale) {
        if (NULL == (m_adata.szLocale = new WCHAR[m_adata.cbLocale]))
            return ReportError(E_OUTOFMEMORY);
    } else
        m_adata.szLocale = NULL;
    if (m_adata.ulOS) {
        if (NULL == (m_adata.rOS = new OSINFO[m_adata.ulOS]))
            return ReportError(E_OUTOFMEMORY);
    } else
        m_adata.ulOS = NULL;
    if (m_adata.ulProcessor) {
        if (NULL == (m_adata.rProcessor = new ULONG[m_adata.ulProcessor]))
            return ReportError(E_OUTOFMEMORY);
    } else
        m_adata.rProcessor = NULL;

    if (FAILED(hr = m_pAImport->GetAssemblyProps( tkAsm, (const void**)&pbOrig, &cbOrig, NULL, (LPWSTR)m_szAsmName, chName, NULL, &m_adata, NULL)))
        // Don't ReportError because this is a COM+ error!
        return hr;

    if (cbOrig > 0) {
        if (!StrongNameTokenFromPublicKey( (PBYTE)pbOrig, cbOrig, (PBYTE*)&pbOrig, &cbOrig))
            return ReportError(StrongNameErrorInfo());
    }
    if ((m_pPubKey = (PublicKeyBlob*)new BYTE[cbOrig]) == NULL)
        return ReportError(E_OUTOFMEMORY);
    memcpy(m_pPubKey, pbOrig, cbOrig);
    m_cbPubKey = cbOrig;
    if (cbOrig > 0)
        StrongNameFreeBuffer( (BYTE*)pbOrig);


    // Get the CAs
    if (FAILED(hr = ReadCAs(tkAsm, NULL, FALSE, FALSE))) // We don't know and we don't care for the import case.
        return hr;
    // The manifest file always imports it's types implicitly
    if (fImportTypes) {
        if (FAILED(hr = ImportFile(NULL, NULL)))
            return hr;
    }

    HCORENUM enumFiles;
    mdFile filedefs[32];
    ULONG cFiledefs, iFiledef;
    WCHAR *FileName = NULL, *filepart = NULL;
    DWORD len, cchName;

    cchName = (DWORD)wcslen(m_Path) + MAX_PATH;
    FileName = (LPWSTR)_alloca(sizeof(WCHAR) * cchName);
    hr = StringCchCopyW (FileName, cchName, m_Path);
    if (FAILED (hr)) {
        return hr;
    }
    filepart = wcsrchr(FileName, L'\\');
    if (filepart)
        filepart++;
    else
        filepart = FileName;

    len = cchName - (DWORD)(filepart - FileName);
    
    ASSERT(m_Files.Count() == 0);

    // Enumeration all the Files in this assembly.
    enumFiles= 0;
    do {
        // Get next batch of files.
        hr = m_pAImport->EnumFiles(&enumFiles, filedefs, lengthof(filedefs), &cFiledefs);
        if (FAILED(hr))
            // Don't ReportError because this is a COM+ error!
            break;

        // Process each file.
        for (iFiledef = 0; iFiledef < cFiledefs && SUCCEEDED(hr); ++iFiledef) {
            CFile *file = NULL;
            hr = m_pAImport->GetFileProps( filedefs[iFiledef], filepart, len, &cchName, NULL, NULL, &dwFlags);
            if (FAILED(hr))
                // Don't ReportError because this is a COM+ error!
                break;

            if (IsFfContainsMetaData(dwFlags)) {
                IMetaDataImport* pImport = NULL;
                hr = pDisp->OpenScope( FileName, dwOpenFlags, IID_IMetaDataImport, (IUnknown**)&pImport);
                if (SUCCEEDED(hr)) {
                    if (NULL == (file = new CFile()))
                        hr = ReportError(E_OUTOFMEMORY);
                    else if (FAILED(hr = file->Init( FileName, pImport, m_pError, m_pLinker)))
                        hr = ReportError(hr);
                    else {
                        pImport->Release();
                        if (FAILED(hr = m_Files.Add(file)))
                            hr = ReportError(hr);
                        if (fImportTypes)
                            hr = file->ImportFile( NULL, NULL);
                    }
                } else {
                    hr = ReportError(ERR_AssemblyModuleImportError, m_tkError, NULL, m_Path, filepart, ErrorHR(hr, pDisp, IID_IMetaDataDispenserEx));
                }
            } else {
                if (NULL == (file = new CFile()))
                    hr = ReportError(E_OUTOFMEMORY);
                else if (FAILED(hr = file->Init( FileName, (IMetaDataEmit*)NULL, m_pError, m_pLinker)))
                    hr = ReportError(hr);
                else {
                    file->m_bNoMetaData = file->m_isDeclared = file->m_isCTDeclared = true;
                    if (FAILED(hr = m_Files.Add(file)))
                        hr = ReportError(hr);
                }
            }
            if (FAILED(hr) && file) {
                delete file;
                file = NULL;
            } else if (file) {
                file->m_tkError = file->m_tkFile = filedefs[iFiledef];
                // We need to keep these guys sorted
                ASSERT(m_Files.Count() == 1 || m_Files[m_Files.Count() - 2]->m_tkFile < filedefs[iFiledef]);
            }
        }
    } while (cFiledefs > 0 && SUCCEEDED(hr));
    
    m_pImport->CloseEnum(enumFiles);
    if (pdwCountOfScopes) *pdwCountOfScopes = (DWORD)m_Files.Count() + 1;

#ifdef _DEBUG
    // We use bsearch on the files, so they better stay in token order
    for (DWORD debug_only_i = 1; debug_only_i < m_Files.Count(); debug_only_i++)
        ASSERT(m_Files[debug_only_i-1]->m_tkFile < m_Files[debug_only_i]->m_tkFile);
#endif

    return hr;
}

HRESULT CAssembly::GetFile(DWORD index, CFile** file)
{
    if (!file)
        return E_POINTER;

    if (RidFromToken(index) < m_Files.Count()) {
        if ((*file = m_Files.GetAt(RidFromToken(index))))
            return S_OK;
    }
    return ReportError(E_INVALIDARG);
}

HRESULT CAssembly::RemoveFile(DWORD index)
{
    if (RidFromToken(index) < m_Files.Count()) {
        m_Files.Base()[RidFromToken(index)] = NULL;
        return S_OK;
    }
    return ReportError(E_INVALIDARG);
}

HRESULT CAssembly::ReportDuplicateType( mdTypeDef tkTypeDef, LPCWSTR pszTypeName, mdToken tkFirstLoc, mdToken tkOtherLoc)
{
    HRESULT hr;
    bool bReport = false;
    LPWSTR otherLoc;
    LPWSTR firstLoc;

    if (FAILED(hr = GetParentName(tkFirstLoc, &firstLoc)))
        return hr;
    if (FAILED(hr = GetParentName(tkOtherLoc, &otherLoc))) {
        delete [] firstLoc;
        return hr;
    }

    bool fFirstIsForwarder = (TypeFromToken(tkFirstLoc) == mdtAssemblyRef);
    bool fSecondIsForwarder = (TypeFromToken(tkOtherLoc) == mdtAssemblyRef);
    if (fFirstIsForwarder && fSecondIsForwarder) {
        // We assert here because the current implementation handles this error in AddExportType(), so we should never have two type forwarders here.
        ASSERT(!"Invalid duplicate type in exported type table.  Why wasn't this caught by dupe Checking in AddExportType()?");
        hr = ReportError(ERR_DuplicateTypeForwarders, tkTypeDef, &bReport, pszTypeName, firstLoc, otherLoc);
    }
    else if (fFirstIsForwarder)
        hr = ReportError(ERR_TypeFwderMatchesDeclared, tkTypeDef, &bReport, pszTypeName, otherLoc, firstLoc);
    else if (fSecondIsForwarder)
        hr = ReportError(ERR_TypeFwderMatchesDeclared, tkTypeDef, &bReport, pszTypeName, firstLoc, otherLoc);
    else 
        hr = ReportError(ERR_DuplicateExportedType, tkTypeDef, &bReport, pszTypeName, firstLoc, otherLoc);

    delete [] otherLoc;
    delete [] firstLoc;
    if (bReport) // If we were able to report it through the callback, continue as if it succeeded
        return S_OK;

    return hr;
}

HRESULT CAssembly::AddExportType(mdToken tkParent, mdTypeDef tkTypeDef, LPCWSTR pszTypeName, DWORD flags, mdExportedType *pType)
{
    HRESULT hr;

    hr = m_pAEmit->DefineExportedType(pszTypeName, tkParent, tkTypeDef, flags, pType);

    if (hr == META_S_DUPLICATE && (m_bDoDupTypeCheck || m_bDoDupeCheckTypeFwds)) {
        mdToken tkOtherParent;
        mdTypeDef tkOtherDef;
        DWORD otherFlags;

        hr = GetImporter()->GetExportedTypeProps( *pType, NULL, 0, NULL, &tkOtherParent, &tkOtherDef, &otherFlags);
        if (FAILED(hr))
            return hr;

        // for two type forwarders, check that they point to the same assemblyref location.
        if ((TypeFromToken(tkParent) == TypeFromToken(tkOtherParent)) && (TypeFromToken(tkParent) == mdtAssemblyRef))
        {
            HRESULT hr;
            LPWSTR pwszParent;
            LPWSTR pwszOther;

            if (FAILED(hr = GetParentName(tkParent, &pwszParent)))
                return hr;
            if (FAILED(hr = GetParentName(tkOtherParent, &pwszOther))) {
                delete [] pwszParent;
                return hr;
            }

            if (wcscmp(pwszParent, pwszOther) != 0) {
                bool fReport = false;                                     
                hr = ReportError(ERR_DuplicateTypeForwarders, tkTypeDef, &fReport, pszTypeName, pwszParent, pwszOther);
                if (fReport) // If we were able to report it through the callback, return S_OK to be consistent with ReportDuplicateType();
                    hr = S_OK;
            } else {
                hr = META_S_DUPLICATE;   // let the caller know it's a dupe
            }

            delete [] pwszParent;
            delete [] pwszOther;

        }
        // if the original definition is different we need to report an error
        else if (tkOtherParent != tkParent || tkOtherDef != tkTypeDef || flags != otherFlags)
        {
            hr = ReportDuplicateType( tkTypeDef, pszTypeName, tkParent, tkOtherParent);
        }
        else
        {
            // Everything matches up so let the caller know it's a dupe
            hr = META_S_DUPLICATE;
        }
    }

    return hr;
}

HRESULT CAssembly::AddResource(mdToken Location, LPCWSTR pszName, DWORD offset, DWORD flags)
{
    return DefineManifestResource( m_pAEmit, Location, pszName, offset, flags);
}

BSTR CAssembly::ExtractString(const void * pvData, DWORD * cbData)
{
    // String is stored as compressed length, UTF8.
    if (*(const BYTE*)pvData == 0xFF) {
        pvData = (const BYTE*)pvData + 1;
        return SysAllocStringLen(NULL, 0);
    } else {
        PCCOR_SIGNATURE sig = (PCCOR_SIGNATURE)pvData;
        *cbData -= CorSigUncompressedDataSize(sig);
        ASSERT(*cbData >= 0);
        DWORD len = CorSigUncompressData(sig);
        pvData = (const void *)sig;
        ASSERT(*cbData >= len);
        DWORD cchLen = UnicodeLengthOfUTF8((PCSTR)pvData, len);
        LPWSTR str = (LPWSTR)_alloca(sizeof(WCHAR)*cchLen);
        UTF8ToUnicode((const char*)pvData, len, str, cchLen);
        pvData = (const BYTE*)pvData + len;
        *cbData -= len;
        return SysAllocStringLen(str, cchLen);
    }
}

HRESULT CAssembly::AddCustomAttribute(mdToken tkType, const void* pValue, DWORD cbValue, BOOL bSecurity, BOOL bAllowMultiple, CFile *source)
{
    ASSERT(m_pEmit != NULL);
    ASSERT(lengthof(OptionCAs) == optLastPriAssemOption + 1);
    CA ca;
    CA *pca = &ca;
    HRESULT hr = S_OK;
    int opt;

    if (m_isUBM)
        return CFile::AddCustomAttribute(tkType, pValue, cbValue, bSecurity, bAllowMultiple, source);

    if (source == NULL)
        source = this;
    
    // check to see if this is a 'special' CA
    LPWSTR name = NULL;
    if (FAILED(hr = source->GetCAName(tkType, &name)))
        return hr;
    
    opt = FindOption(name);

#ifdef _DEBUG
    if (opt != optLastPriAssemOption) {
        ASSERT(bAllowMultiple == ((OptionCAs[opt].flag & 0x04) == 0x04));
        ASSERT(bSecurity == ((OptionCAs[opt].flag & 0x08) == 0x08));
    }
#endif

    if (OptionCAs[opt].flag & 0x02) {
        // This attribute is a real attribute
        if (source != this) {
            if (FAILED(hr = ConvertCAToken( &tkType, source))) {
                goto FAIL_ATTRIBUTE;
            }
        }

        if (FAILED(hr = GetCustomAttributeCompareToken( tkType, &ca.tkCompare))) {
            goto FAIL_ATTRIBUTE;
        }

        // Just a normal CA
        ca.tkCA = mdTokenNil;
        ca.cbVal = cbValue;
        ca.tkType = tkType;
        ca.pVal = (BYTE *)pValue;
        ca.bSecurity = (bSecurity != FALSE);
        ca.bAllowMulti = (bAllowMultiple != FALSE);

        UINT count = m_CAs.Count();
        CA *found = NULL;
        if (ca.bAllowMulti) {
            // Check for an exact duplicate
# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4996)
# endif
            found = (CA*)_lfind(&ca, m_CAs.Base(), &count, sizeof(CA), CA::CompareVal);
# ifdef _MSC_VER
#  pragma warning(pop)
# endif
            ASSERT(count == m_CAs.Count());

            if (found == NULL) {
                // It allows multiples and an identical one was not found, so add it
                BYTE *pTemp = new BYTE[cbValue];
                if (pTemp == NULL)
                    hr = E_OUTOFMEMORY;
                else if (FAILED(hr = m_CAs.Add( NULL, &pca)))
                    delete [] pTemp;
                if (SUCCEEDED(hr)) {
                    pca->tkCA = mdTokenNil;
                    pca->cbVal = cbValue;
                    pca->tkType = tkType;
                    pca->tkCompare = ca.tkCompare;
                    pca->bSecurity = (bSecurity != FALSE);
                    pca->bAllowMulti = (bAllowMultiple != FALSE);
                    pca->pVal = pTemp;
                    memcpy(pTemp, pValue, cbValue);
                } else {
                    hr = ReportError(hr);
                }
            } else
                hr = META_S_DUPLICATE;
        } else {
            // No duplicates allowed
# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4996)
# endif
            found = (CA*)_lfind(&ca, m_CAs.Base(), &count, sizeof(CA), CA::CompareToken);
# ifdef _MSC_VER
#  pragma warning(pop)
# endif
            ASSERT(count == m_CAs.Count());
            if (found == NULL) {
                // Add it
                BYTE *pTemp = new BYTE[cbValue];
                if (pTemp == NULL)
                    hr = E_OUTOFMEMORY;
                else if (FAILED(hr = m_CAs.Add( NULL, &pca)))
                    delete [] pTemp;
                if (SUCCEEDED(hr)) {
                    pca->tkCA = mdTokenNil;
                    pca->cbVal = cbValue;
                    pca->tkType = tkType;
                    pca->tkCompare = ca.tkCompare;
                    pca->bSecurity = (bSecurity != FALSE);
                    pca->bAllowMulti = (bAllowMultiple != FALSE);
                    pca->pVal = pTemp;
                    memcpy(pTemp, pValue, cbValue);
                } else {
                    hr = ReportError(hr);
                }
            } else {
                // If the one found is identical, then ignore it, otherwise give an error
                if (CA::CompareVal(&ca, found) != 0) {
                    if (opt < optLastPriAssemOption) {
                        // It's a known CA, so replace it
                        hr = S_FALSE;
                        delete [] found->pVal;
                        pca = found;
                        BYTE *pTemp = new BYTE[cbValue];
                        if (pTemp == NULL)
                            hr = ReportError(E_OUTOFMEMORY);
                        else {
                            pca->tkCA = mdTokenNil;
                            pca->cbVal = cbValue;
                            pca->tkType = tkType;
                            pca->tkCompare = ca.tkCompare;
                            pca->bSecurity = (bSecurity != FALSE);
                            pca->bAllowMulti = (bAllowMultiple != FALSE);
                            pca->pVal = pTemp;
                            memcpy(pTemp, pValue, cbValue);
                        }
                    } else
                        hr = ReportError(ERR_DuplicateCA, tkType, NULL, name);
                } else
                    hr = META_S_DUPLICATE;
            }
        }
    }
FAIL_ATTRIBUTE:
    delete [] name;

    if (SUCCEEDED(hr) && (OptionCAs[opt].flag & 0x01)) {
        // This is a 'known' CA
        const BYTE * pvData = (const BYTE *)pValue;
        DWORD  cbSize = cbValue;
        VARIANT var;
        
        // Make sure that data is in the correct format. Check the prolog, then
        // move beyond it.
        if (cbSize < sizeof(WORD) || GET_UNALIGNED_VAL16(pvData) != 1)
            return ReportError(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
        pvData += sizeof(WORD);
        cbSize -= sizeof(WORD);
        VariantInit(&var);

        switch(OptionCAs[opt].vt) {
        case VT_BSTR:
            if (opt == optAssemOS) {
                ULONG temp = 0;
                if (cbSize < sizeof(ULONG)) {
                    hr = ReportError(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
                    break;
                }
                temp = VAL32((*(ULONG*)pvData));
                cbSize -= sizeof(ULONG);
                pvData += sizeof(ULONG);
                CComBSTR str;
                HRESULT hr2;
                str.Attach(ExtractString(pvData, &cbSize));
                size_t os_len = str.Length() + 12;
                LPWSTR os = (LPWSTR)_alloca(SizeMul(os_len, sizeof(WCHAR)));
                LPWSTR os_end = NULL;
                hr2 = StringCchPrintfExW (os, os_len, &os_end, NULL, STRSAFE_NULL_ON_FAILURE, L"%d.", temp);
                ASSERT (SUCCEEDED (hr2));
                memcpy(os_end, str, str.Length()*sizeof(WCHAR));
                V_VT(&var) = VT_BSTR;
                V_BSTR(&var) = SysAllocStringLen(os, (UINT)SizeAdd(str.Length(), os_end - os));
            } else {
                V_VT(&var) = VT_BSTR;
                V_BSTR(&var) = ExtractString(pvData, &cbSize);
            }
            hr = SetOption((AssemblyOptions)opt, var);
            break;
        case VT_BOOL:
            if (cbSize < sizeof(BYTE)) {
                hr = ReportError(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
                break;
            }
            V_VT(&var) = VT_BOOL;
            V_BOOL(&var) = ((*(BYTE*)pvData) ? VARIANT_TRUE : VARIANT_FALSE);
            cbSize -= sizeof(BYTE);
            pvData += sizeof(BYTE);
            hr = SetOption((AssemblyOptions)opt, var);
            break;

        case VT_UI4:
            if (cbSize < sizeof(ULONG)) {
                hr = ReportError(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
                break;
            }
            V_VT(&var) = VT_UI4;
            V_UI4(&var) = GET_UNALIGNED_VAL32(pvData);
            cbSize -= sizeof(ULONG);
            pvData += sizeof(ULONG);
            hr = SetOption((AssemblyOptions)opt, var);
            break;

        default:
            hr = ReportError(E_INVALIDARG);
        }
        VariantClear(&var);
    }

    if (bAllowMultiple && hr == S_FALSE)
        hr = S_OK;

    return hr;
}

HRESULT CAssembly::SetOption(AssemblyOptions option, VARIANT value)
{
    HRESULT hr = S_OK;
    LPWSTR *target = NULL;
    LPWSTR str = NULL;
    size_t len = 0;
    VARIANT v;
    bool bCheckVersionString = false;
    bool bRequireVersionString = false;
    // String encoded as: "Major.Minor.Build.Revision"
    ULONG val[4] = {0,0,0,0};
    LPWSTR internal = NULL;
    LPWSTR end = NULL;

    if ((PrivateAssemblyOptions)option >= optLastPriAssemOption || OptionCAs[option].flag & 0x40)
        return E_INVALIDARG;

    VariantInit(&v);

    switch(option) {
    case optAssemTitle:
        target = (LPWSTR*)&m_Title;
        break;
    case optAssemDescription:
        target = (LPWSTR*)&m_Description;
        break;
    case optAssemConfig:
        VSFAIL("We should never be setting optAssemConfig!");
        hr = E_INVALIDARG;
        goto CLEAN_UP;
    case optAssemLocale:
        target = (LPWSTR*)&m_adata.szLocale;
        break;
    case optAssemCompany:
        target = (LPWSTR*)&m_Company;
        break;
    case optAssemProduct:
        target = (LPWSTR*)&m_Product;
        break;
    case optAssemProductVersion:
        target = (LPWSTR*)&m_ProductVersion;
        bCheckVersionString = true;
        break;
    case optAssemFileVersion:
        target = (LPWSTR*)&m_FileVersion;
        bCheckVersionString = true;
        break;
    case optAssemSatelliteVer:
        target = &str;
        bCheckVersionString = true;
        bRequireVersionString = true;
        break;
    case optAssemCopyright:
        target = (LPWSTR*)&m_Copyright;
        break;
    case optAssemTrademark:
        target = (LPWSTR*)&m_Trademark;
        break;
    case optAssemKeyFile:
        target = (LPWSTR*)&m_KeyFile;
        break;
    case optAssemKeyName:
        target = (LPWSTR*)&m_KeyName;
        break;
    case optAssemOS:
    case optAssemVersion:
        target = &str;
        break;
    case optAssemProcessor:      // ULONG
        hr = VariantChangeType( &v, &value, 0, VT_UI4);
        if (FAILED(hr)) {
            hr = ReportError(hr);
            goto CLEAN_UP;
        }

        if (m_adata.ulProcessor > 0) {
            hr = S_FALSE;
            // Don't add duplicates
            for (ULONG u = 0; u < m_adata.ulProcessor; u++) {
                if (m_adata.rProcessor[u] == V_UI4(&v)) {
                    hr = S_FALSE;
                    goto CLEAN_UP;
                }
            }

            {
                ULONG * temp = (ULONG*)VSRealloc( m_adata.rProcessor, (m_adata.ulProcessor + 1) * sizeof(ULONG));
                if (temp == NULL) {
                    hr = E_OUTOFMEMORY;
                    goto CLEAN_UP;
                }
                m_adata.rProcessor = temp;
            }
        } else {
            m_adata.rProcessor = (ULONG*)VSAlloc( sizeof(ULONG));
        }
        if (m_adata.rProcessor == NULL) {
            hr = E_OUTOFMEMORY;
            goto CLEAN_UP;
        }
        m_adata.rProcessor[m_adata.ulProcessor++] = V_UI4(&v);
        goto CLEAN_UP;
    case optAssemAlgID:       // ULONG
        hr = VariantChangeType( &v, &value, 0, VT_UI4);
        if (FAILED(hr)) {
            hr = ReportError(hr);
            goto CLEAN_UP;
        }
        if (SUCCEEDED(hr) && m_isAlgIdSet && m_iAlgID != V_UI4(&v))
            hr = S_FALSE;
        m_iAlgID = V_UI4(&v);
        m_isAlgIdSet = true;
        goto CLEAN_UP;
    case optAssemFlags:       // ULONG
        hr = VariantChangeType( &v, &value, 0, VT_UI4);
        if (FAILED(hr)) {
            hr = ReportError(hr);
            goto CLEAN_UP;
        }
        if (SUCCEEDED(hr) && m_isFlagsSet && m_dwFlags != V_UI4(&v))
            hr = S_FALSE;
        m_dwFlags = V_UI4(&v);
        m_isFlagsSet = true;
        goto CLEAN_UP;
    case optAssemHalfSign:       // Bool
        hr = VariantChangeType( &v, &value, 0, VT_BOOL);
        if (FAILED(hr)) {
            hr = ReportError(hr);
            goto CLEAN_UP;
        }
        if (SUCCEEDED(hr) && m_isHalfSignSet && m_doHalfSign != (V_BOOL(&v) != FALSE))
            hr = S_FALSE;
        m_doHalfSign = (V_BOOL(&v) != FALSE);
        m_isHalfSignSet = true;
        goto CLEAN_UP;
    case optPriAssemFriendAssem:
        m_hasFriend = true;
        goto CLEAN_UP;

    default:
        hr = E_INVALIDARG;
        goto CLEAN_UP;

    }
    if (target != NULL) {
        hr = VariantChangeType( &v, &value, 0, VT_BSTR);
        if (FAILED(hr)) {
            hr = ReportError(hr);
            goto CLEAN_UP;
        }
        // Need to support embedded NULLs
        len = SysStringLen(V_BSTR(&v));
        if ((OptionCAs[option].flag & 0x10) && len > MAX_PATH) {
            hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
            goto CLEAN_UP;
        }
        if (*target != NULL) {
            if (memcmp(*target, V_BSTR(&v), len * sizeof(WCHAR)) == 0 && len == wcslen(*target)) {
                // It's a dupe, but it has the same value, so it's OK
                hr = S_OK;
                goto CLEAN_UP;
            }
            hr = S_FALSE;
            delete [] *target;
        }
        LPWSTR tempstr = new WCHAR[len+1];
        if (tempstr == NULL) {
            hr = E_OUTOFMEMORY;
            goto CLEAN_UP;
        }
        *target = (LPWSTR)memcpy(tempstr, V_BSTR(&v), sizeof(WCHAR)*len);
        (*target)[len] = L'\0';
        if (option == optAssemLocale) {
            m_adata.cbLocale = (ULONG)len;
            if (m_bIsExe && m_adata.cbLocale > 0)
                hr = ReportError(ERR_ExeHasCulture);
        }
    }

    if (option == optAssemVersion) {
        // String encoded as: "Major[.Minor[.Build[.Revision]]]"
        bool BadTime = false;
        internal = str;
        hr = ERR_InvalidVersionFormat;

        for (int i = 0; i < 4; i++) {
            if (i > 1 && *internal == L'*' && internal + 1 == str + len) {
                // Auto-gen
                WORD rev, bld;
                hr = MakeVersion( i == 2 ? &rev : NULL, &bld);
                if (hr == E_FAIL)
                    BadTime = true;
                else if (FAILED(hr))
                    goto SUCCEED;
                // all done
                val[3] = bld;
                if (i == 2)
                    val[2] = rev;
                break;
            }
            if (iswspace(*internal) || *internal == L'-' || *internal == L'+')
                // we don't allow these
                goto FAIL;
            val[i] = wcstoul( internal, &end, 10); // Allow only decimal
            if (end == internal || val[i] >= USHRT_MAX)  // 65535 is not valid (because of metadata restrictions)
                // We didn't parse anything, or it wasn't valid
                goto FAIL;
            if (end == str + len)
                // we're done
                break;
            if (*end != L'.' || i == 3)
                // Need a dot to continue (or this should have been the end)
                goto FAIL;
            internal = end + 1;
        }

        delete [] str;
        if (m_isVersionSet &&
            (m_adata.usMajorVersion != (unsigned short)val[0] ||
             m_adata.usMinorVersion != (unsigned short)val[1] ||
             m_adata.usRevisionNumber != (unsigned short)val[3] ||
             m_adata.usBuildNumber != (unsigned short)val[2]))
            hr = BadTime ? ReportError(WRN_InvalidTime, NULL, NULL) : S_FALSE;
        else
            hr = BadTime ? ReportError(WRN_InvalidTime, NULL, NULL) : S_OK;
    
        m_adata.usMajorVersion = (unsigned short)val[0];
        m_adata.usMinorVersion = (unsigned short)val[1];
        m_adata.usRevisionNumber = (unsigned short)val[3];
        m_adata.usBuildNumber = (unsigned short)val[2];
        m_isVersionSet = true;
    } else if (option == optAssemOS) {
        // String encoded as: "dwOSPlatformId.dwOSMajorVersion.dwOSMinorVersion"
        internal = str;

        hr = ERR_InvalidOSString;
        for (int i = 0; i < 3; i++) {
            if (iswspace(*internal) || *internal == L'-' || *internal == L'+')
                // we don't allow these
                goto FAIL;
            val[i] = wcstoul( internal, &end, 10); // Allow only decimal
            if (end == internal || (val[i] == ULONG_MAX && errno == ERANGE)) 
                // We didn't parse anything, or it wasn't valid
                goto FAIL;
            if (end == str + len)
                // we're done
                break;
            if (*end != L'.' || i == 2)
                // Need a dot to continue (or this should have been the end)
                goto FAIL;
            internal = end + 1;
        }

        if (m_adata.ulOS > 0) {
            hr = S_FALSE;
            for (ULONG u = 0; u < m_adata.ulOS; u++) {
                if (m_adata.rOS[u].dwOSPlatformId == val[0] &&
                    m_adata.rOS[u].dwOSMajorVersion == val[1] &&
                    m_adata.rOS[u].dwOSMinorVersion == val[2])
                    goto SUCCEED;
            }

            {
                OSINFO * temp = (OSINFO*)VSRealloc( m_adata.rOS, SizeMul(m_adata.ulOS + 1, sizeof(OSINFO)));
                if (temp == NULL) {
                    hr = E_OUTOFMEMORY;
                    goto SUCCEED;
                }
                m_adata.rOS = temp;
            }
        } else {
            hr = S_OK;
            m_adata.rOS = (OSINFO*)VSAlloc( sizeof(OSINFO));
        }
        if (m_adata.rOS == NULL) {
            hr = E_OUTOFMEMORY;
            goto SUCCEED;
        }
        m_adata.rOS[m_adata.ulOS].dwOSPlatformId = val[0];
        m_adata.rOS[m_adata.ulOS].dwOSMajorVersion = val[1];
        m_adata.rOS[m_adata.ulOS++].dwOSMinorVersion = val[2];
        goto SUCCEED;
FAIL:
        if (hr != S_OK)
            hr = ReportError((ERRORS)hr, mdTokenNil, NULL, V_BSTR(&v));
SUCCEED:
        delete [] str;
    } else if (bCheckVersionString) {
        // String encoded as: "Major.Minor.Build.Revision"
        internal = *target;

        for (int i = 0; i < 4; i++) {
            if (iswspace(*internal) || *internal == L'-' || *internal == L'+')
                // we don't allow these
                break;
            val[i] = wcstoul( internal, &end, 10); // Allow only decimal
            if (end == internal || (bRequireVersionString ? (val[i] >= USHRT_MAX) : (val[i] > USHRT_MAX)))  // 65535 is not valid (because of metadata restrictions)
                // We didn't parse anything, or it wasn't valid
                break;
            if (end == *target + len) {
                // we're done
                goto OK_VERSION;
            }
            if (*end != L'.' || i == 3)
                // Need a dot to continue (or this should have been the end)
                break;
            internal = end + 1;
        }
        if (bRequireVersionString)
            hr = ReportError(ERR_InvalidVersionString, mdTokenNil, NULL, ErrorOptionName(option), *target);
        else {
            bool bReported = false;
            HRESULT hr2 = ReportError(WRN_InvalidVersionString, mdTokenNil, &bReported, ErrorOptionName(option), *target);
            if (!bReported)
                hr = hr2;
        }
OK_VERSION:

        if (option == optAssemSatelliteVer)
            delete [] str;
    }

CLEAN_UP:
    VariantClear(&v);
    return hr;
}

HRESULT CAssembly::MakeRes(CFile *file, BOOL fDll, LPCWSTR pszIconFile, const void **ppBlob, DWORD *pcbBlob)
{
    HRESULT hr = S_OK;
    DWORD cbBlob = 0;

    *ppBlob = NULL;
    *pcbBlob = cbBlob;

    return hr;
}

HRESULT CAssembly::EmitManifest(DWORD *pdwReserveSize, mdAssembly *ptkAssembly)
{
    ASSERT(pdwReserveSize);
    // Emit the Manifest

    BOOLEAN b = FALSE;
    HRESULT hr;
    
    // Do not check for more CAs that might have gotten merged in
    // Instead rely on the user calling EndMerge to force the correct
    // Ordering between custom attributes and command-line options

    *pdwReserveSize = 0;
    if (FAILED(hr = ReadCryptoKey(&m_pPubKey, m_cbPubKey)))
        return hr;

    if ((!m_KeyFile || m_KeyFile[0] == 0) && 
        (!m_KeyName || m_KeyName[0] == 0) &&
        m_doHalfSign)
    {
        // No key file or key name, but /delaysign+
        bool fDontCare;
        hr = ReportError( WRN_DelaySignWithNoKey, mdTokenNil, &fDontCare);
        // We really don't care if this warning was reported or not
    }

    if (!m_isAlgIdSet) {
        m_iAlgID = CALG_SHA1;
        m_isAlgIdSet = true;
    }

    // Make sure we have 'created' the assembly name
    if (NULL == GetAsmName())
        return E_OUTOFMEMORY;
    hr = m_pAEmit->DefineAssembly( m_pPubKey, m_cbPubKey, m_iAlgID, GetAsmName(),
        &m_adata, m_dwFlags, &m_tkAssembly);

    if (hr == META_S_DUPLICATE) {
        // Incremental Update and they want to change the Assembly Info
        void * PK = m_pPubKey;
        if (m_cbPubKey == 0) {
            // if PK is NULL, the metadata will leave the old value there!
            ASSERT(PK == NULL);
            PK = &m_cbPubKey;
        }
        hr = m_pAEmit->SetAssemblyProps( m_tkAssembly, PK, m_cbPubKey, m_iAlgID,
            GetAsmName(), &m_adata, m_dwFlags);
    }

    // NOTE: we can't put the digest at the begining of the file because 
    // the APIs don't like a RVA of 0.
    // Just get the size of the digest
    if (SUCCEEDED(hr) && m_pPubKey) {

        if (pdwReserveSize) {
            b = StrongNameSignatureSize( (PBYTE)m_pPubKey, m_cbPubKey, pdwReserveSize);
            if (b == FALSE) {
                *pdwReserveSize = 0;
                hr = ReportError(StrongNameErrorInfo());
            }
        }
    } else if (pdwReserveSize) {
        *pdwReserveSize = 0;
    }

    if (SUCCEEDED(hr) && ptkAssembly)
        *ptkAssembly = m_tkAssembly;

    return hr;
}

HRESULT CAssembly::EmitInternalTypes()
{
    HRESULT hr = S_FALSE;
    for (DWORD i = 0, l = m_Files.Count(); i < l; i++) {
        CFile *temp = m_Files.GetAt(i);
        if (temp->GetImportScope()) {
            // Emit the internal types to the ExportedType table now that we know
            if (FAILED(hr = temp->EmitComTypes( this, true)))
                break;
        }
    }

    return hr;
}

HRESULT CAssembly::EmitAssembly(IMetaDataDispenserEx *pDisp)
{
    // Emit all the cached CAs, the imported CAs, and the hashes of files, and the global Refs
    if (m_tkAssembly == 0)
        // Somehow an error prevented us from emitting an assembly,
        // so don't compound the problem further, just quit.
        return S_FALSE;

    DWORD i, l;
    BOOLEAN b = FALSE;
    PBYTE pHash;
    DWORD cbHash, ccbHash;
    HRESULT hr = S_OK;
    DWORD SecurityCAs = 0;

    for (i = 0, l = m_CAs.Count(); i < l; i++) {
        CA *temp = m_CAs.GetAt(i);
        if (temp->bSecurity) {
            SecurityCAs++;
        }
        else {
            hr = m_pEmit->DefineCustomAttribute( m_tkAssembly, temp->tkType, temp->pVal, temp->cbVal, &temp->tkCA);
            if (FAILED(hr)) {
                LPWSTR buffer = NULL;
                if (SUCCEEDED(GetCAName(temp->tkType, &buffer)))
                    hr = ReportError(ERR_EmitCAFailed, temp->tkCA, NULL, buffer, ErrorHR(hr, m_pEmit, IID_IMetaDataEmit));
                delete [] buffer;
                return hr;
            }
        }
    }

    if (SecurityCAs > 0) {
        // Emit the security CAs
        DWORD j, Err;
        COR_SECATTR *attrs = new COR_SECATTR[SecurityCAs];
        if (attrs == NULL)
            return ReportError(E_OUTOFMEMORY);
        for (i = j = 0, l = m_CAs.Count(); i < l && j < SecurityCAs; i++) {
            CA *temp = m_CAs.GetAt(i);
            if (temp->bSecurity) {
                attrs[j].tkCtor = temp->tkType;
                attrs[j].pCustomAttribute = temp->pVal;
                attrs[j].cbCustomAttribute = temp->cbVal;
                j++;
            }
        }

        hr = m_pEmit->DefineSecurityAttributeSet( m_tkAssembly, attrs, j, &Err);


        delete [] attrs;
        if (FAILED(hr))
            // Don't Report the error (COM+ already did)
            return hr;
    }

    if (m_Files.Count() > 0) {
        b = StrongNameHashSize( m_iAlgID, &cbHash);
        if (b == FALSE)
            return ReportError(ERR_CryptoHashFailed, mdTokenNil, NULL, ErrorHR(StrongNameErrorInfo()));
    
        pHash = new BYTE [cbHash];
        if (pHash == NULL)
            return E_OUTOFMEMORY;
        for (i = 0, l = m_Files.Count(); i < l; i++) {
            CFile *temp = m_Files.GetAt(i);
    
            // Copy all the AssemblyRefs
            if (temp->GetImportScope()) {
                IMetaDataAssemblyImport *asrc = NULL;
                // But only if we have metadata
                if (SUCCEEDED(hr = temp->GetImportScope()->QueryInterface(IID_IMetaDataAssemblyImport, (void**)&asrc))) {
                    HCORENUM hEnum = 0;
                    mdAssemblyRef mdAR[8];
                    DWORD cAR = 0;
    
                    do {
                        if (SUCCEEDED(hr = asrc->EnumAssemblyRefs( &hEnum, mdAR, lengthof(mdAR), &cAR))) {
                            for (DWORD j = 0; j < cAR && SUCCEEDED(hr); j++) {
                                hr = DupAssemblyRef( &mdAR[j], asrc);
                            }
                        }
                    } while (SUCCEEDED(hr) && cAR != 0);
                    asrc->CloseEnum(hEnum);
                    asrc->Release();
                    if (FAILED(hr))
                        break;
                } else {
                    hr = temp->ReportError(hr);
                    break;
                }

                // Emit the ExportedType table (now that we know whether or not to emit internal stuff)
                if (FAILED(hr = temp->EmitComTypes( this, m_hasFriend)))
                    break;
            }
    
            // Finish the files
            if (FAILED(hr = temp->CopyFile(pDisp, m_bClean)))
                break;
            HANDLE hFile = OpenFileEx(temp->m_Path, NULL);
            if (hFile == INVALID_HANDLE_VALUE) {
                hr = ReportError(HRESULT_FROM_WIN32(GetLastError()), temp->m_tkError);
                break;
            }
            DWORD result = GetHashFromHandle(hFile, &m_iAlgID, pHash, cbHash, &ccbHash);
            CloseHandle(hFile);
            if (FAILED(hr = HRESULT_FROM_WIN32(result))) {
                hr = ReportError(ERR_CryptoHashFailed, temp->m_tkError, NULL, ErrorHR(hr));
                break;
            }
            ASSERT(cbHash == ccbHash);
            if (FAILED(hr = m_pAEmit->SetFileProps(temp->m_tkFile, pHash, ccbHash, ULONG_MAX))) // ULONG_MAX = Don't change
                break;

            if (FAILED(hr = ComparePEKind(temp, true)))
                break;
        }
        delete [] pHash;
    }

    if (SUCCEEDED(hr) && m_bDoDupTypeCheck) {
        if (!m_pAImport)
            IfFailRet(m_pAEmit->QueryInterface(IID_IMetaDataAssemblyImport, (void**)&m_pAImport));
        if (!m_pImport)
            IfFailRet(m_pAEmit->QueryInterface(IID_IMetaDataImport, (void**)&m_pImport));

        HCORENUM hEnum = 0;
        mdExportedType mdET[32];
        DWORD cET = 0, j;
        WCHAR wszTypeName[MAX_CLASS_NAME];
        mdToken tkImpl;

        do {
            if (SUCCEEDED(hr = m_pAImport->EnumExportedTypes( &hEnum, mdET, lengthof(mdET), &cET))) {
                for (j = 0; j < cET && SUCCEEDED(hr); j++) {
                    mdTypeDef tkOther;

                    if (FAILED(hr = m_pAImport->GetExportedTypeProps( mdET[j], wszTypeName, lengthof(wszTypeName), NULL, &tkImpl, NULL, NULL)))
                        break;
                    if (TypeFromToken(tkImpl) != mdtFile) {
                        ASSERT(TypeFromToken(tkImpl) == mdtAssemblyRef || TypeFromToken(tkImpl) == mdtExportedType);
                        // No need to dup-check these because they aren't really top-level exported types
                        // For nested types, the parent type will hit

                        // Note:  Type forwarders are checked if the m_bDoDupeCheckTypeFwds flag is set when type forwarders are added.
                        continue;
                    }
                    hr = m_pImport->FindTypeDefByName( wszTypeName, mdTokenNil, &tkOther);
                    if (hr == CLDB_E_RECORD_NOTFOUND) {
                        // no duplicate, it passes
                        hr = S_OK;
                        continue;
                    }
                    if (!FAILED(hr)) {
                        // Try to report it and continue
                        hr = ReportDuplicateType( tkOther, wszTypeName, mdTokenNil, tkImpl);
                    }
                }
            }
        } while (SUCCEEDED(hr) && cET != 0);

        m_pAImport->CloseEnum(hEnum);
    }

    // Check references
    if (SUCCEEDED(hr)) {
        if (!m_pAImport)
            IfFailRet(m_pAEmit->QueryInterface(IID_IMetaDataAssemblyImport, (void**)&m_pAImport));

        HCORENUM hEnum = 0;
        mdAssemblyRef mdAR[8];
        DWORD cAR = 0, j;

        do {
            if (SUCCEEDED(hr = m_pAImport->EnumAssemblyRefs( &hEnum, mdAR, lengthof(mdAR), &cAR))) {
                for (j = 0; j < cAR && SUCCEEDED(hr); j++) {
                    hr = CheckAssemblyRef( mdAR[j]);
                }
            }
        } while (SUCCEEDED(hr) && cAR != 0);

        m_pAImport->CloseEnum(hEnum);
    }

    return hr;
}

HRESULT CAssembly::CheckAssemblyRef( mdAssemblyRef tkRef)
{
    HRESULT hr;
    const void * pbPubKey = NULL;
    ULONG cbPubKey = 0, chName = 0;
    ASSEMBLYMETADATA data;
    memset(&data, 0, sizeof(data));

    if (SUCCEEDED(hr = m_pAImport->GetAssemblyRefProps( tkRef, &pbPubKey, &cbPubKey, NULL, 0, &chName, &data, NULL, NULL, NULL))) {

        ASSERT(data.cbLocale != 1); // This would just be a "\0" right?
        bool bRefNotStrong = (m_pPubKey && (pbPubKey == NULL || cbPubKey == 0)); // This dependant is not strongly-named, but we are?
        bool bRefHasCulture = (data.cbLocale > 1); // This dependant has a non-empty Culture setting

        if (bRefNotStrong || bRefHasCulture) {
            bool bReported = false;
            LPWSTR wszName = (LPWSTR)_alloca(sizeof(WCHAR)*(chName+1));
            if (FAILED(hr = m_pAImport->GetAssemblyRefProps( tkRef, NULL, NULL,
                wszName, chName, NULL, NULL, NULL, NULL, NULL))) {
                wszName[0] = L'\0';
            }
            
            if (bRefNotStrong) {
                HRESULT hr2 = ReportError( ERR_RefNotStrong, tkRef, &bReported, wszName);
                if (!bReported)
                    hr = hr2;
            }
            if (SUCCEEDED(hr) && bRefHasCulture) {
                HRESULT hr2 = ReportError( WRN_RefHasCulture, tkRef, &bReported, wszName);
                if (!bReported)
                    hr = hr2;
            }
        }
    }

    return hr;
}

HRESULT CFile::GetCustomAttributeCompareToken(mdToken tkType, mdToken * tkCompare)
{
    HRESULT hr = S_OK;
    mdToken parent;

    if (TypeFromToken(tkType) == mdtMemberRef) {
        if (FAILED(hr = m_pImport->GetMemberRefProps( tkType, &parent, NULL, 0, NULL, NULL, NULL)))
            return hr;
    } else if (TypeFromToken(tkType) == mdtTypeRef || TypeFromToken(tkType) == mdtTypeDef) {
        parent = tkType;
    } else if (TypeFromToken(tkType) == mdtMethodDef) {
        if (FAILED(hr = m_pImport->GetMemberProps( tkType, &parent, NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)))
            return hr;
    } else {
        return E_INVALIDARG;
    }

    ASSERT(TypeFromToken(parent) == mdtTypeDef || TypeFromToken(parent) == mdtTypeRef);
    *tkCompare = parent;

    return hr;
}

HRESULT CAssembly::ConvertCAToken(mdToken *tkCA, CFile *src)
{
    HRESULT hr = S_OK;
    mdToken parent;

    if (TypeFromToken(*tkCA) == mdtMemberRef) {
        DWORD cchMember;
        LPWSTR member;
        PCCOR_SIGNATURE sig = NULL;
        ULONG cbSig = 0;

        if (FAILED(hr = src->GetImportScope()->GetMemberRefProps( *tkCA, &parent, NULL, 0, &cchMember, NULL, NULL)))
            return hr;
        member = (LPWSTR)_alloca(sizeof(WCHAR)*cchMember);
        if (FAILED(hr = src->GetImportScope()->GetMemberRefProps( *tkCA, NULL, member, cchMember, &cchMember, &sig, &cbSig)))
            return hr;
        
        if (FAILED(hr = CopyType(&parent, src)))
            return hr;

        ULONG cbNewSig = cbSig * 2; // Can't be more than twice original size right?
        PCOR_SIGNATURE newSig = (PCOR_SIGNATURE)_alloca(cbNewSig);
        if (FAILED(hr = m_pEmit->TranslateSigWithScope(NULL, NULL, 0, src->GetImportScope(), sig, cbSig,
            m_pAEmit, m_pEmit, newSig, cbNewSig, &cbNewSig)))
            return hr;

        if (TypeFromToken(parent) == mdtTypeDef) {
            // It's defined in this file!?!?
            hr = m_pImport->FindMember(parent, member, newSig, cbNewSig, tkCA);
        } else {
            hr = m_pImport->FindMemberRef(parent, member, newSig, cbNewSig, tkCA);
            if (hr == CLDB_E_RECORD_NOTFOUND) {
                // Make a memberref
                hr = m_pEmit->DefineMemberRef(parent, member, newSig, cbNewSig, tkCA);
            }
        }
    } else if (TypeFromToken(*tkCA) == mdtTypeRef) {
        // A TypeRef
        hr = CopyType(tkCA, src);
    } else {
        // This must be a Def
        ASSERT(TypeFromToken(*tkCA) == mdtMethodDef);
        if (FAILED(hr = src->GetImportScope()->GetMemberProps( *tkCA, &parent, NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)))
            return hr;

        ASSERT(TypeFromToken(parent) == mdtTypeDef);
        if (FAILED(hr = CopyType(&parent, src)))
            return hr;
        ASSERT(TypeFromToken(parent) == mdtTypeRef);

        if (src->m_isAssembly) {
            CAssembly *asrc = (CAssembly*)src;
            const void * pvHash = NULL;
            DWORD pbHash = 0;
            if (FAILED(hr = asrc->GetHash( &pvHash, &pbHash)))
                return hr;
            hr = m_pEmit->DefineImportMember(asrc->GetImporter(), pvHash, pbHash, src->GetImportScope(),
                *tkCA, m_pAEmit, parent, tkCA);
        } else {
            hr = m_pEmit->DefineImportMember( NULL, NULL, 0, src->GetImportScope(), *tkCA, m_pAEmit, parent, tkCA);
        }
    }
    return hr;
}

LPCWSTR CAssembly::GetAsmName()
{
    if (m_szAsmName)
        return m_szAsmName;

    if (IsInMemory()) {
        return m_Path;
    } else {
        // strip the extension from the name for the manifest
        m_szAsmName = VSAllocStr(m_Name);
        if (m_szAsmName == NULL)
            return NULL;
        LPWSTR pExt = (LPWSTR)wcsrchr(m_szAsmName, L'.');
        if (pExt) *pExt = L'\0';
    }

    return m_szAsmName;
}

HRESULT CAssembly::MakeAssemblyRef(IMetaDataAssemblyEmit *pEmit, mdAssemblyRef *par)
{
    const void * pTemp;
    DWORD cTemp;
    HRESULT hr;

    // Call GetHash to make sure we have a valid hash of the assembly
    if (FAILED(hr = GetHash( &pTemp, &cTemp)))
        return hr;

    LPCWSTR pszAsmName = GetAsmName();
    if (pszAsmName == NULL)
        return E_OUTOFMEMORY;
    return pEmit->DefineAssemblyRef( m_pPubKey, m_cbPubKey, pszAsmName, &m_adata, m_pbHash, m_cbHash, m_dwFlags, par);
}

HRESULT CAssembly::MakeAssemblyRef(IMetaDataEmit *pEmit, mdAssemblyRef *par)
{
    HRESULT hr;
    CComPtr<IMetaDataAssemblyEmit> pAEmit;

    // Get the right interface, then call the real function
    if (FAILED(hr = pEmit->QueryInterface(IID_IMetaDataAssemblyEmit, (void**)&pAEmit)))
        return hr;

    return MakeAssemblyRef( pAEmit, par);
}

HRESULT CAssembly::CopyType(mdToken *tkType, CFile *src)
{
    HRESULT hr;

    if (TypeFromToken(*tkType) == mdtTypeDef) {
        // Use the standard APIs
        if (src->m_isAssembly) {
            CAssembly *asrc = (CAssembly*)src;
            const void * pvHash = NULL;
            DWORD pbHash = 0;
            if (FAILED(hr = asrc->GetHash( &pvHash, &pbHash)))
                return hr;
            hr = m_pEmit->DefineImportType(asrc->GetImporter(), pvHash, pbHash, src->GetImportScope(),
                *tkType, m_pAEmit, tkType);
        } else {
            hr = m_pEmit->DefineImportType( NULL, NULL, 0, src->GetImportScope(), *tkType, m_pAEmit, tkType);
        }
    } else {
        mdToken tkScope;
        DWORD cchName;
        LPWSTR name;
        IMetaDataImport* imp = src->GetImportScope();

        ASSERT(TypeFromToken(*tkType) == mdtTypeRef);
        if (FAILED(hr = imp->GetTypeRefProps(*tkType, &tkScope, NULL, 0, &cchName)))
            return hr;

        name = (LPWSTR)_alloca(cchName * sizeof(WCHAR));
        if (FAILED(hr = imp->GetTypeRefProps(*tkType, NULL, name, cchName, &cchName)))
            return hr;
        else {
            if (SUCCEEDED(hr = m_pImport->FindTypeDefByName(name, mdTokenNil, tkType)))
                return hr;
        }

        switch(TypeFromToken(tkScope)) {
        case mdtModule:
            hr = src->MakeModuleRef( m_pEmit, &tkScope);
            break;
        case mdtModuleRef:
            hr = DupModuleRef(&tkScope, src->GetImportScope());
            break;
        case mdtAssemblyRef:
            if (src->m_isAssembly)
                hr = DupAssemblyRef( &tkScope, ((CAssembly*)src)->GetImporter());
            else {
                IMetaDataAssemblyImport *asrc = NULL;
                if (SUCCEEDED(hr = imp->QueryInterface(IID_IMetaDataAssemblyImport, (void**)&asrc))) {
                    hr = DupAssemblyRef( &tkScope, asrc);
                    asrc->Release();
                } else {
                    hr = ReportError(hr, tkScope);
                }
            }
            break;
        default:
            return ReportError(E_INVALIDARG, tkScope);
        }
        if (FAILED(hr))
            return hr;

        if (FAILED(hr = m_pImport->FindTypeRef(tkScope, name, tkType)))
            hr = m_pEmit->DefineTypeRefByName(tkScope, name, tkType);
    }
    return hr;
}

HRESULT CAssembly::DupAssemblyRef(mdToken *tkAR, IMetaDataAssemblyImport* pImport)
{
    // ASSUME: the Import scope will NEVER have an AssemblyRef to the assembly we are building

    ASSEMBLYMETADATA data;
    DWORD chName, cbOrig, cbHash, dwFlags;
    const void *pOrig, *pHash;
    LPWSTR name = NULL;
    HRESULT hr = S_OK;

    memset(&data, 0, sizeof(data));
    chName = cbOrig = cbHash = dwFlags = 0;
    pOrig = pHash = NULL;

    hr = pImport->GetAssemblyRefProps(*tkAR,
        NULL, NULL,                     // Originator
        NULL, 0, &chName,               // Name
        &data,                          // MetaData Struct
        NULL, NULL,                     // Hash
        NULL);                          // Flags
    if (FAILED(hr))
        return hr;

#define AllocArray(var, ch, datum) if ((ch) > 0) { if ((var = new datum[ch]) == NULL) { hr = ReportError(E_OUTOFMEMORY); goto FAIL;}}
    AllocArray(name, chName, WCHAR);
    AllocArray(data.szLocale, data.cbLocale, WCHAR);
    AllocArray(data.rOS, data.ulOS, OSINFO);
    AllocArray(data.rProcessor, data.ulProcessor, ULONG);
#undef AllocArray

    hr = pImport->GetAssemblyRefProps(*tkAR,
        &pOrig, &cbOrig,                     // Originator
        name, chName, &chName,               // Name
        &data,                               // MetaData Struct
        &pHash, &cbHash,                     // Hash
        &dwFlags);                           // Flags
    if (FAILED(hr))
        goto FAIL;

    hr = m_pAEmit->DefineAssemblyRef(
        pOrig, cbOrig,
        name,
        &data,
        pHash, cbHash,
        dwFlags,
        tkAR);

FAIL:
    SAFEFREE(name);
    SAFEFREE(data.szLocale);
    SAFEFREE(data.rOS);
    SAFEFREE(data.rProcessor);

    if (FAILED(hr))
        *tkAR = mdTokenNil;

    return hr;
}

HRESULT CFile::GetCAName(mdToken tkCA, __out LPWSTR *ppszName)
{
    HRESULT hr = S_OK;
    DWORD cchName;
    LPWSTR name;

    *ppszName = NULL;
    if (m_pImport == NULL &&
        FAILED(hr = m_pEmit->QueryInterface(IID_IMetaDataImport, (void**)&m_pImport)))
        return hr;
    
    if (TypeFromToken(tkCA) == mdtMemberRef) {
        mdToken parent;
        if (FAILED(hr = m_pImport->GetMemberRefProps( tkCA, &parent, NULL, 0, NULL, NULL, NULL)))
            return hr;
        tkCA = parent;
    } else if (TypeFromToken(tkCA) == mdtMethodDef) {
        mdToken parent;
        if (FAILED(hr = m_pImport->GetMemberProps( tkCA, &parent, NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)))
            return hr;
        tkCA = parent;
    }

    if (TypeFromToken(tkCA) == mdtTypeRef) {
        // A TypeRef
        if (FAILED(hr = m_pImport->GetTypeRefProps(tkCA, NULL, NULL, 0, &cchName)))
            return hr;
        if ((name = new WCHAR[cchName+1]) == NULL)
            return E_OUTOFMEMORY;
        hr = m_pImport->GetTypeRefProps(tkCA, NULL, name, cchName, &cchName);
    } else {
        ASSERT(TypeFromToken(tkCA) == mdtTypeDef);
        if (FAILED(hr = m_pImport->GetTypeDefProps(tkCA, NULL, 0, &cchName, NULL, NULL)))
            return hr;
        if ((name = new WCHAR[cchName+1]) == NULL)
            return E_OUTOFMEMORY;
        hr = m_pImport->GetTypeDefProps(tkCA, name, cchName, &cchName, NULL, NULL);
    }
    if (SUCCEEDED(hr))
        *ppszName = name;
    else
        delete [] name;
    return hr;
}

HRESULT CAssembly::ReadCryptoKey(PublicKeyBlob **ppPubKey, DWORD &dwLen)
{
    *ppPubKey = NULL;
    dwLen = 0;
    PBYTE buffer = NULL;
    DWORD result = FALSE;
    HRESULT hr = E_FAIL;
    bool bPublicKeyFileOnly = false;
    
    if ((!m_KeyFile || m_KeyFile[0] == 0) && 
        (!m_KeyName || m_KeyName[0] == 0))
        return S_FALSE;

    // Use the given name if it exists
    if (m_KeyName == NULL || m_KeyName[0] == L'\0') {
        // Let mscorsn to create the temporary name for us
        if (m_KeyName) delete [] m_KeyName;
        m_KeyName = NULL;

        m_isAutoKeyName = true;
    } else
        m_isAutoKeyName = false;

    // try by name
    if (!m_isAutoKeyName) {
        // Try to get the public key for the originator, by name
        if ((result = StrongNameGetPublicKey( m_KeyName, m_pbKeyPair,
            m_cbKeyPair, (BYTE**)&buffer, &dwLen)) != TRUE) {
            ASSERT(buffer == NULL && dwLen == 0);
            hr = StrongNameErrorInfo();
        } else {
            hr = S_OK;
        }
    }

    // try by file
    if (FAILED(hr) && m_KeyFile) {
        bool bErrorReported = false;
        PAL_TRY {
            if (SUCCEEDED(hr = ReadCryptoFile())) {
                if (!m_isAutoKeyName) {
                    // try installing the file
                    if ((result = StrongNameKeyInstall( m_KeyName, m_pbKeyPair, m_cbKeyPair)) == TRUE) {
                        if (FAILED(result = CloseCryptoFile())) {
                            hr = ReportError(result);
                            bErrorReported = true;
                            goto Done;
                        } else
                            hr = S_OK;
                    } else {
                        hr = StrongNameErrorInfo();
                        ASSERT(FAILED(hr));
                    }
                }
            
                // try to get the public key part
                if (SUCCEEDED(hr) &&
                    (result = StrongNameGetPublicKey( m_KeyName, m_pbKeyPair, m_cbKeyPair, &buffer, &dwLen)) == FALSE)
                    hr = StrongNameErrorInfo();

                // Do we have a public Key blob?
                if (FAILED(hr) && m_pbKeyPair &&
                    IsValidPublicKeyBlob((PublicKeyBlob*)m_pbKeyPair, m_cbKeyPair))
                    bPublicKeyFileOnly = true;

            } else if (m_isAutoKeyName) {
                hr = ReportError( ERR_CryptoFileFailed, mdTokenNil, NULL, m_KeyFile, ErrorHR(hr));
                bErrorReported = true;
            }
    Done: ;
        } PAL_FINALLY {
            HRESULT hr2 = ClearCryptoFile();
            if (SUCCEEDED(hr) && FAILED(hr2))
                hr = hr2;
        }
        PAL_ENDTRY
        if (bErrorReported) {
            ASSERT(FAILED(hr));
            return hr;
        }
    }

    // try a public-only key blob
    if (FAILED(hr) && m_doHalfSign && bPublicKeyFileOnly) {
        ASSERT(FAILED(hr) && m_KeyFile);

        // Re-read the bits
        if (SUCCEEDED(hr = ReadCryptoFile())) {
            // and then hand them out (because we're closing the file!)
            *ppPubKey = (PublicKeyBlob*)m_pbKeyPair;
            // We don't have to clear the key because it's only a public key
            // there's no private data in the file
            m_pbKeyPair = NULL;
            dwLen = m_cbKeyPair;
            m_cbKeyPair = 0;
        }
        // Note that at this point m_pbKeyPair is not really a KeyPair!
        return CloseCryptoFile();

    } else if (SUCCEEDED(hr)) {
        size_t len = VAL32(((PublicKeyBlob*)buffer)->cbPublicKey) + (sizeof(ULONG) * 3);
        void *temp = new BYTE[len];
        if (temp != NULL) {
            memcpy(temp, buffer, len);
            StrongNameFreeBuffer((BYTE*)buffer);
            *ppPubKey = (PublicKeyBlob*)temp;
            return S_OK;
        } else {
            StrongNameFreeBuffer((BYTE*)buffer);
            CloseCryptoFile();
            return E_OUTOFMEMORY;
        }
    } else {
        // This is the first Crypto-call so we need to check for SN_* errors
        if (hr == SN_CRYPTOAPI_CALL_FAILED ||
            hr == SN_NO_SUITABLE_CSP) {
            hr = ReportError(ERR_CryptoFailed);
        } else if (bPublicKeyFileOnly && !m_doHalfSign) {
            // Give both errors if appropriate
            if (!m_isAutoKeyName) {
                bool temp = false;
                ReportError( ERR_CryptoNoKeyContainer, mdTokenNil, &temp, m_KeyName);
            }
            hr = ReportError(ERR_NeedPrivateKey, mdTokenNil, NULL, m_KeyFile);
        } else if (m_KeyName && !m_isAutoKeyName) {
            hr = ReportError( ERR_CryptoNoKeyContainer, mdTokenNil, NULL, m_KeyName);
        } else {
            hr = ReportError(hr);
        }

        // The call failed so pretend like there's no key-info
        if (buffer != NULL)
            StrongNameFreeBuffer((BYTE*)buffer);
        HRESULT hr2 = CloseCryptoFile();
        if (FAILED(hr2))
            ReportError(hr2);

        return hr;
    }
}

HRESULT CAssembly::OpenCryptoFile()
{
    ASSERT(m_hKeyFile==INVALID_HANDLE_VALUE);
    HRESULT hr = S_OK;

    // Open and Read in the Key Blob
    m_hKeyFile = OpenFileEx( m_KeyFile, &m_cbKeyPair, IsInMemory() ? NULL : m_Path);
    if (m_hKeyFile == INVALID_HANDLE_VALUE) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto CRYPTO_FAILED;
    }

    m_pbKeyPair = new BYTE[m_cbKeyPair];
    if (m_pbKeyPair == NULL) {
        hr = E_OUTOFMEMORY;
        goto CRYPTO_FAILED;
    }

    if (FAILED(hr)) {
CRYPTO_FAILED:
        // The call failed so pretend like there's no key-info
        CloseCryptoFile(); // Ignore any errors from this
    }

    return hr;
}

HRESULT CAssembly::ReadCryptoFile()
{
    DWORD dwRead = 0;
    HRESULT hr = S_OK;

    // Open the file if needed
    if (m_hKeyFile==INVALID_HANDLE_VALUE) {
        if (FAILED(hr = OpenCryptoFile()))
            return hr;
    }

    // Read in the Key Blob
    if (FALSE == ReadFile( m_hKeyFile, m_pbKeyPair, m_cbKeyPair, &dwRead, NULL)) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto CRYPTO_FAILED;
    } else if (dwRead != m_cbKeyPair) {
        hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
        goto CRYPTO_FAILED;
    }

    return hr;

CRYPTO_FAILED:

    // The call failed so pretend like there's no key-info
    CloseCryptoFile(); // Ignore any errors from this
    return hr;
}

HRESULT CAssembly::ClearCryptoFile()
{
    if (m_pbKeyPair != NULL) {
        memset(m_pbKeyPair, (*(int*)m_pbKeyPair==-1?0:-1), m_cbKeyPair);
    }
    if (m_hKeyFile != INVALID_HANDLE_VALUE) {
        if (0 != SetFilePointer( m_hKeyFile, 0, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());
    }
    return S_OK;
}

HRESULT CAssembly::CloseCryptoFile()
{
    HRESULT hr = S_OK;
    if (m_pbKeyPair != NULL) {
        hr = ClearCryptoFile();
        delete [] m_pbKeyPair;
    }
    m_pbKeyPair = NULL;
    m_cbKeyPair = 0;

    if (m_hKeyFile != INVALID_HANDLE_VALUE)
        CloseHandle(m_hKeyFile);
    m_hKeyFile = INVALID_HANDLE_VALUE;

    return hr;
}

HRESULT CAssembly::SignAssembly()
{
    HRESULT hr = S_FALSE;

    if (!m_doHalfSign && (m_pbKeyPair != NULL || (m_KeyName && *m_KeyName && !m_isAutoKeyName))){
        PAL_TRY {
            if (IsInMemory()) {
                VSFAIL("You can't fully sign an InMemory assembly!");
                hr = E_INVALIDARG;
            } else if (m_pbKeyPair == NULL || SUCCEEDED(hr = ReadCryptoFile())) {
                if (FALSE != StrongNameSignatureGeneration( m_Path, m_KeyName,
                    m_pbKeyPair, m_cbKeyPair, NULL, NULL)) {
                    hr = S_OK;
                    ASSERT(StrongNameSignatureVerification(m_Path, SN_INFLAG_FORCE_VER | SN_INFLAG_INSTALL | SN_INFLAG_ALL_ACCESS, NULL));
                } else {
                    DWORD err = StrongNameErrorInfo();
                    if (!FAILED(err)) err = HRESULT_FROM_WIN32(err);
                    hr = ReportError(err);
                }
            }
        } PAL_FINALLY {
            HRESULT hr2 = ClearCryptoFile();
            if (SUCCEEDED(hr) && FAILED(hr2))
                hr = hr2;
        } PAL_ENDTRY
    }

    return hr;
}

// NOTE: rev is optional (may be NULL), but build is NOT
HRESULT CAssembly::MakeVersion(WORD *rev, WORD *build)
{
    tm temptime;
    time_t current, initial;
    double diff;
    bool BadTime = false;

    // Get the current time
    time( &current);
    if (current < 0 && rev)
        // We have a date previous to 1970, and they want a revision number
        // Can't do it, so just fail here
        return E_FAIL;
    else if (current < (60 * 60 * 24) && !rev) {
        // Keep adding a day until we are just above 1 day
        // Thus arriving at the number of seconds since midnight (GMT) plus 1 day
        // so it will always be after 1/1/1970 even in local time
        while (current < (60 * 60 * 24))
            current += (60 * 60 * 24);
        BadTime = true;
    }

    temptime = *localtime( &current);

    if (temptime.tm_year < 100 && !rev) {
        // If we only want a build number, then
        // make sure the year is greater than 2000
        temptime.tm_year = 100;
        BadTime = true;
    }
    // convert to useful struct
    current = mktime(&temptime);
    if (current == (time_t)-1)
        return E_FAIL;

    // Make Jan. 1 2000, 00:00:00.00
    memset(&temptime, 0, sizeof(temptime));
    temptime.tm_year = 100; // 2000 - 1900
    temptime.tm_mday = 1; // Jan. 1
    temptime.tm_isdst = -1; // Let the CRT figure out if it is Daylight savings time
    initial = mktime(&temptime);


    // Get difference in seconds
    diff = (double)current - (double)initial;
    if (diff < 0.0 && rev) {
        return E_FAIL;
    }

    if (rev != NULL) {
        ASSERT(diff >= 0.0);
        *rev = (WORD)floor(diff / (60 * 60 * 24)); // divide by 60 seconds, 60 minutes, 24 hours, to get difference in days
    }

    if (build != NULL) {
        if (diff < 0.0) {
            diff = fmod(diff, 60 * 60 * 24) + (60 * 60 * 24); // make it positive by moving it and adding 1 day
            ASSERT(diff >= 0.0);
            BadTime = true;
        }
        *build = (WORD)(fmod(diff, 60 * 60 * 24) / 2); // Get the seconds since midnight div 2 for 2 second granularity
        ASSERT(*build != 65535);
    } else {
        return E_INVALIDARG;
    }

    return BadTime ? E_FAIL : S_OK;
}

#undef SAFEFREE

HRESULT CAssembly::GetParentName(mdToken tkParent, __out LPWSTR * pszName)
{
    HRESULT hr;
    *pszName = NULL;

    switch (TypeFromToken(tkParent)) {
    case mdTokenNil:
        *pszName = VSAllocStr(m_Name);
        hr = (*pszName == NULL ? E_OUTOFMEMORY : S_OK);
        break;
    case mdtFile:
        {
            DWORD cchName = 0;
            hr = GetImporter()->GetFileProps( tkParent, NULL, 0, &cchName, NULL, NULL, NULL);
            if (FAILED(hr))
                return hr;
            *pszName = new WCHAR [cchName];
            if (NULL == *pszName)
                return E_OUTOFMEMORY;
            hr = GetImporter()->GetFileProps( tkParent, *pszName, cchName, NULL, NULL, NULL, NULL);
            if (FAILED(hr)) {
                delete [] *pszName;
                *pszName = NULL;
            }
        }
        break;
    case mdtExportedType:
        {
            mdToken tkOuterParent;
            hr = GetImporter()->GetExportedTypeProps( tkParent, NULL, 0, NULL, &tkOuterParent, NULL, NULL);
            if (FAILED(hr))
                return hr;
            return GetParentName( tkOuterParent, pszName);
        }
        break;
    case mdtAssemblyRef:
        {
            DWORD cchName = 0;
            hr = GetImporter()->GetAssemblyRefProps( tkParent, NULL, NULL, NULL, 0, &cchName, NULL, NULL, NULL, NULL);
            if (FAILED(hr))
                return hr;
            *pszName = new WCHAR [cchName];
            if (NULL == *pszName)
                return E_OUTOFMEMORY;
            hr = GetImporter()->GetAssemblyRefProps( tkParent, NULL, NULL, *pszName, cchName, NULL, NULL, NULL, NULL, NULL);
            if (FAILED(hr)) {
                delete [] *pszName;
                *pszName = NULL;
            }
        }
        break;
    default:
        VSFAIL("Unknonw parent token type");
        hr = E_FAIL;
        break;
    }
    return hr;
}

HRESULT CAssembly::GetPublicKeyToken(void * pvPublicKeyToken, DWORD * pcbPublicKeyToken)
{
    HRESULT hr;
    if (FAILED(hr = ReadCryptoKey(&m_pPubKey, m_cbPubKey)))
        return hr;

    BYTE * pbSNPubKey;
    DWORD  cbSNPubKey;
    if (!StrongNameTokenFromPublicKey( (PBYTE)m_pPubKey, m_cbPubKey, &pbSNPubKey, &cbSNPubKey))
        return ReportError(StrongNameErrorInfo());

    hr = S_OK;
    if (*pcbPublicKeyToken < cbSNPubKey) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }
    else if (pvPublicKeyToken != NULL) {
        memcpy( pvPublicKeyToken, pbSNPubKey, cbSNPubKey);
    }
    *pcbPublicKeyToken = cbSNPubKey;

    if (cbSNPubKey > 0)
        StrongNameFreeBuffer(pbSNPubKey);

    return hr;
}

HRESULT CAssembly::ComparePEKind(CFile * file, bool fModule)
{
    // VB and possibly others specfically set the PEKind to peNot
    // to stop the checking from happening (to prevent duplicate errors)
    if (!m_bHasPE || m_dwPEKind == peNot || file->m_bCheckedPE)
        return S_OK;

    DWORD dwPEKind, dwMachine;
    ERRORS errId = (ERRORS)-1;
    HRESULT hr;

    if (FAILED(hr = file->GetPEKind( &dwPEKind, &dwMachine))) {
        // This means something bad happened getting the PE, let's not try again
        file->m_bCheckedPE = true;
        return hr;
    }

    if (hr == S_FALSE) { // If we can't get the PE kind give up
        // But don't set the checked bit so we can check it again
        //  when we do get the PE kind
        ASSERT(dwPEKind == peNot);
        return S_OK;
    }

    file->m_bCheckedPE = true;

    if (dwPEKind == peNot) // it's not a PE
        return S_OK;

    if (dwMachine == IMAGE_FILE_MACHINE_I386 && peILonly == (dwPEKind & (peILonly | pe32BitRequired))) {
        // Other file is agnostic, this is always safe
        return S_OK;
    }

    if (m_dwMachine == IMAGE_FILE_MACHINE_I386 && peILonly == (m_dwPEKind & (peILonly | pe32BitRequired))) {
        // We are agnostic, but the reference is not
        if (fModule)
            errId = ERR_AgnosticToMachine;
        else {
            // Some users may want to know that they're doign this, but most don't
            // because the processor specific assembly probably has a different version
            // for each processor, thus making it effectively agnostic
            return S_OK;
        }
    }

    if (errId == -1 && m_dwMachine != dwMachine) {
        // Different machine types, and neither are agnostic
        // So it is a conflict (warn for references, error for modules)
        errId = fModule ? ERR_ConflictingMachine : WRN_ConflictingMachine;
    }

    if (errId == -1)
        return S_OK;

    bool bReported = false;
    //                         mscorlib has a Module name different from it's filename which breaks a ton of rules...
    hr = ReportError( errId, file->m_tkError, &bReported, file->IsStdLib() ? file->m_Name : file->GetModuleName());
    if (bReported)
        hr = S_OK;

    return hr;
}

