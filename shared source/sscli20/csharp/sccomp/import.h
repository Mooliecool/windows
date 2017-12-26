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
// File: import.h
//
// Defines the structures used to import COM+ metadata into the symbol table.
// ===========================================================================

#ifndef __import_h__
#define __import_h__

#define MAX_IDENT_SIZE    512               // Maximum identifier size we allow.  This is the max the compiler allows
#define MAX_FULLNAME_SIZE (MAX_CLASS_NAME)  // Maximum namespace or fully qualified identifier size we allow. Comes from corhdr.h
#define NESTED_CLASS_SEP (L'+')             // Separator for nested classes.

#define MAGIC_SYMBOL_VALUE ((SYM*)-1)

typedef HRESULT (__stdcall *PfnCompareAssemblyIdentity)(PCWSTR pwzAssemblyIdentity1, BOOL fUnified1, PCWSTR pwzAssemblyIdentity2, BOOL fUnified2, BOOL *pfEquivalent, AssemblyComparisonResult *pResult); 

class IMPORTER;

struct IMPORTED_CUSTOM_ATTRIBUTES
{
    // decimal literal
    bool hasDecimalLiteral;
    DECIMAL decimalLiteral;

    // deprecated
    bool isDeprecated;
    bool isDeprecatedError;
    WCHAR *deprecatedString;

    // CLS
    bool hasCLSattribute;
    bool isCLS;

    // attribute
    bool allowMultiple;
    CorAttributeTargets attributeKind;

    // conditional
    NAMELIST **conditionalHead;
    NAMELIST *conditionalSymbols;

    // parameter lists
    bool isParamListArray;

    // RequiredCustomAttribute
    bool hasRequiredAttribute;

    // default member
    WCHAR * defaultMember;

    // ComImport/CoClass
    WCHAR * CoClassName;

    // For fixed sized buffers
    TYPESYM * fixedBuffer;
    int fixedBufferElementCount;

    // For CompilationRelaxationsAttribute(CompilationsRelaxations.NoStringInterning)
    // Only checked at the assembly level (on added modules)
    bool fCompilationRelaxations;
    
    // For RuntimeCompatibilityAttribute
    // Only checked at the assembly level (on added modules)
    bool fRuntimeCompatibility;
    bool fWrapNonExceptionThrows;

    // for security attribute warning
    bool hasLinkDemand;

    // Whether the assembly or module declares any friends.
    bool fHasFriends;

};


class ImportScope
{
public:
    virtual IMetaDataImport * GetMetaImport() = 0;
    virtual IMetaDataAssemblyImport * GetAssemblyImport() = 0;
    virtual MODULESYM * GetModule() = 0; // May return NULL!
    virtual int GetAssemblyID() = 0;
    virtual int GetModuleID() = 0;
    virtual PCWSTR GetFileName() = 0;
};


class ImportScopeModule : public ImportScope
{
private:
    IMPORTER * m_import;
    MODULESYM * m_mod;

public:
    ImportScopeModule(IMPORTER * import, MODULESYM * mod) {
        m_import = import;
        m_mod = mod;
    }

    virtual IMetaDataImport * GetMetaImport();
    virtual IMetaDataAssemblyImport * GetAssemblyImport();
    virtual MODULESYM * GetModule() { return m_mod; }
    virtual int GetAssemblyID() { return m_mod->GetAssemblyID(); }
    virtual int GetModuleID() { return m_mod->GetModuleID(); }
    virtual PCWSTR GetFileName() { return m_mod->getInputFile()->name->text; }
};

// wrapper class for comparing assembly references via the Fusion APIs
class AssemblyIdentityComparison
{
private:
    PfnCompareAssemblyIdentity m_pfnCompareAssemblyIdentity;
    BOOL m_fEqual;
    AssemblyComparisonResult m_acrResult;
    bool m_fAllowUnification;
public:
    AssemblyIdentityComparison(PfnCompareAssemblyIdentity pfnCompare);
    HRESULT Compare(PCWSTR strAssembly1, bool fUnify1, PCWSTR strAssembly2, bool fUnify2);
    bool IsEquivalent() { return !!m_fEqual; }
    bool IsEquivalentNoUnify();
    bool CouldUnify(bool fOnlyFXUnification = false); 
    bool NonEquivalentDueToVersions();
    AssemblyComparisonResult GetResult() { return m_acrResult; }

};




// Options for ResolveTypeName. When the name is unresolvable for some reason, these
// values determine whether ResolveTypeName creates and returns an UNRESAGGSYM or
// returns NULL.
namespace NameResOptions {
    enum _Enum {
        // Create UNRESAGGSYM only if the aid is for an unresolved module we resolve
        // to a type forwarder which is not resolvable.
        Normal,

        // Never create an UNRESAGGSYM
        FavorNull,

        // Create an UNRESAGGSYM (avoid NULL) whenever possible
        FavorUnres,
    };
};
DECLARE_ENUM_TYPE(NameResOptions);


class IMPORTER
{
public:
    IMPORTER();
    ~IMPORTER();
    void Init();
    void Term();

    COMPILER * compiler();

    void ImportAllTypes();
    void LoadTypesInNsAid(NSSYM * ns, int aid, INFILESYM * infile);
    void LoadTypesInNsMod(NSSYM * ns, MODULESYM * mod);

    void GetBaseTokenAndFlags(AGGSYM *sym, AGGTYPESYM **base, DWORD *flags);
    ACCESS AccessFromTypeFlags(uint flags, INFILESYM * infile);
    void GetEnumUnderlyingType(AGGSYM * agg, MODULESYM * scope);
    void ResolveInheritance(AGGSYM * sym);
    void DefineBounds(PARENTSYM * parent);
    void DefineImportedType(PAGGSYM sym);
    void ImportMethodProps(METHSYM * sym);
    void GetTypeRefAssemblyName(MODULESYM *scope, mdToken token, __out_ecount(cchAssemblyName) PWSTR assemblyName, ULONG cchAssemblyName);

#ifdef DEBUG
    void DeclareAllTypes(PPARENTSYM parent);
#endif //DEBUG

    bool GetTypeFromCache(ImportScope & scope, mdToken tok, TypeArray * typeArgs, TYPESYM ** ptype);
    bool GetAggFromCache(ImportScope & scope, mdTypeDef tok, AGGSYM ** pagg);
    bool GetSymFromCache(ImportScope & scope, mdToken tok, SYM ** psym);
    void SetSymInCache(ImportScope & scope, mdToken tok, SYM * sym);

    bool OpenAssembly(PINFILESYM infile);

    AGGSYM *ImportOneType(MODULESYM * mod, mdTypeDef token);

    mdToken SigUncompressToken(ImportScope & scope, PCCOR_SIGNATURE * sigPtr, PCCOR_SIGNATURE sigPtrEnd);
    ULONG SigUncompressData(ImportScope & scope, PCCOR_SIGNATURE * sigPtr, PCCOR_SIGNATURE sigPtrEnd);
    BYTE SigGetByte(ImportScope & scope, PCCOR_SIGNATURE * sigPtr, PCCOR_SIGNATURE sigPtrEnd);
    BYTE SigPeekByte(ImportScope & scope, PCCOR_SIGNATURE sigPtr, PCCOR_SIGNATURE sigPtrEnd);

    ULONG SigPeekUncompressData(MODULESYM * mod, PCCOR_SIGNATURE sigPtr, PCCOR_SIGNATURE sigPtrEnd);

    const void *CheckBufferAccess(MODULESYM * mod, const void *buffer, size_t cbRequired, size_t cbActual);

    enum ImportSigOptions {
        kfisoNone = 0x00,
        kfisoAllowVoid = 0x01,
        kfisoAllowByref = 0x02,
        kfisoIncludeModOpts = 0x04,
    };

    PTYPESYM ImportSigType(ImportScope & scope, PCCOR_SIGNATURE * sigPtr, PCCOR_SIGNATURE sigPtrEnd, int grfiso, int *pmodOptCount,
        TypeArray *pClassTypeFormals = NULL, TypeArray *pMethTypeFormals = NULL, bool convertPinnedToPtr = true);
    void ImportSignatureWithModOpts(MODULESYM *scope, PCCOR_SIGNATURE sig, PCCOR_SIGNATURE sigEnd, TYPESYM **retType, TypeArray ** params,
        int *pmodOptCount, TypeArray *pClassTypeFormals, TypeArray *pMethTypeFormals);

    PCWSTR GetAssemblyName(SYM * sym);
    HRESULT GetAssemblyName(IMetaDataAssemblyImport * assemImport, NAME ** nameAsNAME, BSTR * nameAsBSTR, WORD * assemblyVersion);
    NAME * GetAssemblyName(ImportScope & scopeSource, mdAssemblyRef tkAsmRef);
    IAssemblyName * GetOutputAssemblyName(PEFile * fileOutput, NAME ** pnameOutput);
    void ConfirmMatchesThisAssembly(IAssemblyName * panOutput, NAME * nameOutput, NAME * nameAssemblyRef, SYMLIST * listModSrc, bool fIsFriendAssemblyRefOnly);
    int MapAssemblyRefToAid(NAME * nameAssemblyRef, ImportScope & scopeSource, bool fIsFriendAssemblyRef);
    int MapAssemblyRefToAid(ImportScope & scopeSource, mdAssemblyRef tkAssemblyRef);

    NSSYM * ResolveNamespace(PCWCH namespaceText, int cch);
    TYPESYM * ResolveTypeName(PCWSTR className, BAGSYM * bagPar, int aid, TypeArray * typeArgs, NameResOptionsEnum nro, mdTypeDef token = mdTypeDefNil);

    UNRESAGGSYM *CreateUnresolvedAgg(NAME *name, BAGSYM *bagPar, int cvar);

    NSAIDSYM * ResolveNamespaceOfClassName(PCWSTR * ppsz, int aid);
    TYPESYM * ResolveFullMetadataTypeName(MODULESYM * scope, PCWSTR className, bool * fIsInvalidSig);
    TYPESYM * ResolveTypeRefOrSpec(MODULESYM *scope, mdToken tokenBase,
        TypeArray * pClassTypeFormals, TypeArray * pMethTypeFormals = NULL);
    TYPESYM * ResolveTypeRefOrDef(MODULESYM * scope, mdToken token, TypeArray * typeArgs);
    TYPESYM * ResolveTypeRefOrDef(ImportScope & scope, mdToken token, TypeArray * typeArgs);
    TYPESYM * ResolveTypeRef(ImportScope & scope, mdTypeRef token, TypeArray * typeArgs);
    TYPESYM * ResolveTypeDef(ImportScope & scope, mdTypeDef token, TypeArray * typeArgs);
    SYM * ResolveParentScope(ImportScope & scope, mdToken tkPar, PCWSTR * ppsz);

    // This never causes types to be loaded.
    AGGSYM * FindAggName(NAME * name, BAGSYM * bagPar, int aid, mdTypeDef tok);

    static int ComputeArityFromName(PCWSTR className, PCWSTR * startOfArity);

    bool IsValidAssemblyName(PCWSTR szAsmName);
    bool CheckFriendAssemblyName (BASENODE *tree, PCWSTR szAsmName, OUTFILESYM *context);

    void CheckHR(HRESULT hr, INFILESYM * infile) {
        ASSERT(infile);
        if (FAILED(hr))
            MetadataFailure(hr, infile->name->text);
        SetErrorInfo(0, NULL);
    }
    void CheckHR(HRESULT hr, MODULESYM * mod) {
        ASSERT(mod);
        if (FAILED(hr))
            MetadataFailure(hr, mod->getInputFile()->name->text);
        SetErrorInfo(0, NULL);
    }
    void CheckHR(HRESULT hr, ImportScope & scope) {
        if (FAILED(hr))
            MetadataFailure(hr, scope.GetFileName());
        SetErrorInfo(0, NULL);
    }
    void CheckHR(HRESULT hr, AGGSYM * agg) {
        if (FAILED(hr))
            MetadataFailure(hr, agg->GetModule()->getInputFile()->name->text);
        SetErrorInfo(0, NULL);
    }

private:
    void CheckHR(int errid, HRESULT hr, INFILESYM * infile) {
        ASSERT(infile);
        if (FAILED(hr))
            MetadataFailure(errid, hr, infile->name->text);
        SetErrorInfo(0, NULL);
    }
    void CheckHR(int errid, HRESULT hr, MODULESYM * mod) {
        ASSERT(mod);
        if (FAILED(hr))
            MetadataFailure(errid, hr, mod->getInputFile()->name->text);
        SetErrorInfo(0, NULL);
    }
    void MetadataFailure(HRESULT hr, PCWSTR pszFile);
    void MetadataFailure(int errid, HRESULT hr, PCWSTR pszFile);
    void BogusMetadataFailure(PCWSTR pszFile);
    void BogusMetadataFailure(ImportScope & scope) { BogusMetadataFailure(scope.GetFileName()); }
    void CheckTruncation(int requiredSize, int bufferLength, INFILESYM * infile);
    void CheckTruncation(int requiredSize, int bufferLength, MODULESYM * scope);
    void CheckTruncation(int requiredSize, int bufferLength, ImportScope & scope);
    IMetaDataDispenser * GetDispenser();
    static ACCESS ConvertAccessLevel(uint flags, INFILESYM * infile, bool fDontHide);
    bool ImportConstant(CONSTVAL * constVal, ULONG cch, PTYPESYM valType, DWORD constType, const void * constValue);
    PAGGTYPESYM ImportInterface(MODULESYM *scope, mdInterfaceImpl tokenIntf, PAGGSYM symDerived);
    void ImportField(PAGGSYM parent, PAGGDECLSYM decl, mdFieldDef tokenField);
    void ImportMethod(PAGGSYM parent, PAGGDECLSYM decl, mdMethodDef tokenMethod);
    void ImportMethodPropsWorker(PAGGSYM parent, mdToken tokenMethod, PCCOR_SIGNATURE sig, ULONG cbSignature, METHSYM * meth);
    void ImportProperty(PAGGSYM parent, PAGGDECLSYM decl, mdProperty tokenProperty, PNAME defaultMember);
    void ImportEvent(PAGGSYM parent, PAGGDECLSYM decl, mdEvent tokenProperty);
    PMETHSYM FindMethodDef(PAGGSYM parent, mdMethodDef token);
    PTYPESYM ImportFieldType(MODULESYM * mod, PCCOR_SIGNATURE sig, PCCOR_SIGNATURE sigEnd, bool * isVolatile,
        TypeArray *pClassTypeFormals);
    int StripArityFromName(PCWSTR className, NAME ** pname, NAME ** pnameWithArity);
    int GetSignatureCParams(PCCOR_SIGNATURE sig);
    void ImportMethodOrPropSignature(mdMethodDef token, PCCOR_SIGNATURE sig, PCCOR_SIGNATURE sigEnd, PMETHPROPSYM sym);
    bool ImportSignature(ImportScope & scope, mdMethodDef token, PCCOR_SIGNATURE sig, PCCOR_SIGNATURE sigEnd,
        TYPESYM ** ptypeRet, TypeArray ** pparams, byte * pcallConv, int * pcmod, int * pcvar,
        TypeArray * typeVarsCls, TypeArray * typeVarsMeth);
    BOOL IsParamArray(MODULESYM * scope, mdMethodDef methodToken, int iParam);
    PNSDECLSYM GetNSDecl(NSSYM * ns, PINFILESYM infile);
    bool GetTypeRefFullName(MODULESYM *scope, mdToken token, __out_ecount(cchBuffer) PWSTR fullnameText, ULONG cchBuffer);
    bool GetTypeRefFullNameWithOuter(IMetaDataImport * pmdi, mdToken tok, StringBldr & str);

    PAGGTYPESYM ResolveBaseRef(MODULESYM *scope, mdToken tokenBase, PAGGSYM symDerived, bool fRequired);
    void DefineMethodTypeFormals(mdToken methodToken, METHSYM * meth, SYM * symCtx, TypeArray **ppMethTypeFormals);

    TYPESYM * ResolveTypeNameCore(MODULESYM * mod, ITypeName * ptnType);
    AGGTYPESYM * ResolveTypeNames(MODULESYM * mod, int aid, bool fTryMsCorLib, ITypeName * ptnType);
    AGGTYPESYM * ResolveTypeArgs(MODULESYM * mod, AGGTYPESYM * ats, ITypeName * ptnType);
    TYPESYM * ResolveModifiers(MODULESYM * mod, AGGTYPESYM * ats, ITypeName * ptnType);

    void ImportCustomAttributes(MODULESYM *scope, INFILESYM * infile, IMPORTED_CUSTOM_ATTRIBUTES *attributes, mdToken token);
    void ImportOneCustomAttribute(MODULESYM *scope, INFILESYM * infile, IMPORTED_CUSTOM_ATTRIBUTES *attributes, mdToken attrToken, const void * pvData, ULONG cbSize);
    bool ImportCustomAttrArgBool(MODULESYM *scope, LPCVOID * ppvData, ULONG * pcbSize);
    WCHAR * ImportCustomAttrArgString(MODULESYM *scope, LPCVOID * ppvData, ULONG * pcbSize);
    int ImportCustomAttrArgInt(MODULESYM *scope, LPCVOID * ppvData, ULONG * pcbSize);
    WORD ImportCustomAttrArgWORD(MODULESYM *scope, LPCVOID * ppvData, ULONG * pcbSize);
    BYTE ImportCustomAttrArgBYTE(MODULESYM *scope, LPCVOID * ppvData, ULONG * pcbSize);
    void ImportNamedCustomAttrArg(MODULESYM *scope, LPCVOID *ppvData, ULONG *pcbSize, PCWSTR *pname, TYPESYM **type);

    HRESULT MakeAssemblyName(PCWSTR szName, ULONG cchName, const ASSEMBLYMETADATA & data, LPBYTE pbPublicKey, const ULONG cbPublicKey, const DWORD dwFlags, BSTR * nameAsBSTR, NAME ** nameAsNAME);
    MODULESYM * GetUnresolvedModule(ImportScope & scopeSource, mdToken token);
    int MapModuleRefToAid(ImportScope & scopeSource, mdModuleRef tkModuleRef);
    bool MatchAssemblySimpleName(IAssemblyName * asmnameRef, ImportScope & scopeSource, OUTFILESYM * manifest);
    bool MatchAssemblySimpleName(IAssemblyName * asmnameRef, ImportScope & scopeSource, NAME *simpleName);
    bool CompareAssemblySimpleNames(LPCWSTR assemblyRef, LPCWSTR assemblyDef);
    bool MatchAssemblyPublicKeyToken(IAssemblyName * asmnameRef, ImportScope & scopeSource, OUTFILESYM * manifest);
    bool MatchesThisAssembly(NAME * nameAssemblyRef, ImportScope & scopeSource);
    bool CompareImports( INFILESYM * infile1, INFILESYM * infile2);

    int CompareVersions(ImportScope & scopeSource, IAssemblyName * panRef, INFILESYM * infileCompare);
    static HRESULT ComparePartialAssemblyIdentity(IAssemblyName * panRef, IAssemblyName * panDef, BOOL * pfEquivalent, AssemblyComparisonResult * pResult);
    typedef HRESULT (__stdcall *PfnCreateAssemblyNameObject)(LPASSEMBLYNAME *ppAssemblyNameObj, PCWSTR szAssemblyName, DWORD dwFlags, LPVOID pvReserved);
    HRESULT InitFusionAPIs();
    HRESULT ParseAssemblyNameNoError(PCWSTR szAsmName, IAssemblyName **ppAssemblyNameObj);
    void ParseAssemblyName(PCWSTR szAsmName, ImportScope &scopeSource, IAssemblyName ** ppan);

    FWDAGGSYM *ImportOneTypeForwarder(MODULESYM * mod, mdExportedType token);
    void ImportTypeForwarders(MODULESYM * mod);

    // Helpers for ImportOneType to save some stack space in that method
    void ImportOneType_SetAggKind( /*inout*/ AGGSYM * agg, MODULESYM * mod, LPCWSTR szFull, mdToken tkExtends, bool cvarEqualcvarOuter, DWORD flags );
    bool ImportOneType_ProcessTypeVariable( IMetaDataImport2 * metaimportV2, mdGenericParam tokVar, INFILESYM *infile, LONG cvar, LONG cvarOuter, TYVARSYM ** prgvar, BAGSYM * bagPar, AGGSYM * agg );

    void ImportNamespaces(INFILESYM * infile, bool fTypes);
    void ImportNamespaces(MODULESYM * mod, bool fTypes);
    void SortModTypeInfos(ModTypeInfo * prgmti, int cmti);

    bool inited;
    bool fLoadingTypes;

    PfnCreateAssemblyNameObject m_pfnCreateAssemblyNameObject;
    PfnCompareAssemblyIdentity m_pfnCompareAssemblyIdentity;
    NAME * m_nameErrorAssem;
};

#endif // __import_h__
