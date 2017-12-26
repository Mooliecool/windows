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
// File: import.cpp
//
// Routines for importing COM+ metadata into the symbol table.
// ===========================================================================

#include "stdafx.h"

// do a wcschr in a non-null terminated string
inline static PCWCH wcsnchr(__in_ecount(cch) PCWCH pch, WCHAR ch, size_t cch)
{
    while (cch > 0 && *pch != ch) {
        cch -= 1;
        pch += 1;
    }

    return pch;
}

/* Kinds of custom attributes we care about, for table below. */
enum IMPORTEDCUSTOMATTRKIND {
    CUSTOMATTR_NONE,
    CUSTOMATTR_ATTRIBUTEUSAGE_VOID,
    CUSTOMATTR_ATTRIBUTEUSAGE_VALIDON, 
    CUSTOMATTR_DEPRECATED_VOID,
    CUSTOMATTR_DEPRECATED_STR,
    CUSTOMATTR_DEPRECATED_STRBOOL,
    CUSTOMATTR_CONDITIONAL,
    CUSTOMATTR_DECIMALLITERAL,
    CUSTOMATTR_DEPRECATEDHACK,
    CUSTOMATTR_ATTRIBUTEHACK,
    CUSTOMATTR_CLSCOMPLIANT,
    CUSTOMATTR_PARAMS,
    CUSTOMATTR_DEFAULTMEMBER,
    CUSTOMATTR_DEFAULTMEMBER2,
    CUSTOMATTR_REQUIRED,
    CUSTOMATTR_COCLASS,
    CUSTOMATTR_FIXEDBUFFER,
    CUSTOMATTR_COMPILATIONRELAXATIONS,
    CUSTOMATTR_RUNTIMECOMPATIBILITY,
    CUSTOMATTR_FRIENDASSEMBLY,

    CUSTOMATTR_MAX
};

/*
 * Table of custom attributes that we care about when importing.
 *
 * The following table defines all the custom attribute kinds we care about
 * on import, by class name and constructor signature.
 */
struct IMPORTEDCUSTOMATTR {
    IMPORTEDCUSTOMATTRKIND attrKind;
    PREDEFNAME className;
    int cbSig;             // if <0, don't check sig.
    COR_SIGNATURE sig[8];  // Increase size this if needed.
};

const IMPORTEDCUSTOMATTR g_importedAttributes[CUSTOMATTR_MAX-1] = {
    { CUSTOMATTR_DEPRECATED_VOID,    PN_OBSOLETE_CLASS,  3, { IMAGE_CEE_CS_CALLCONV_HASTHIS, 0, ELEMENT_TYPE_VOID} },
    { CUSTOMATTR_DEPRECATED_STR,     PN_OBSOLETE_CLASS,  4, { IMAGE_CEE_CS_CALLCONV_HASTHIS, 1, ELEMENT_TYPE_VOID, ELEMENT_TYPE_STRING } },
    { CUSTOMATTR_DEPRECATED_STRBOOL, PN_OBSOLETE_CLASS,  5, { IMAGE_CEE_CS_CALLCONV_HASTHIS, 2, ELEMENT_TYPE_VOID, ELEMENT_TYPE_STRING, ELEMENT_TYPE_BOOLEAN } },
    { CUSTOMATTR_ATTRIBUTEUSAGE_VOID,   PN_ATTRIBUTEUSAGE_CLASS,  3, { IMAGE_CEE_CS_CALLCONV_HASTHIS, 0, ELEMENT_TYPE_VOID} },
    { CUSTOMATTR_ATTRIBUTEUSAGE_VALIDON,PN_ATTRIBUTEUSAGE_CLASS, -1, {0} },
    { CUSTOMATTR_CONDITIONAL,        PN_CONDITIONAL_CLASS,    4, { IMAGE_CEE_CS_CALLCONV_HASTHIS, 1, ELEMENT_TYPE_VOID, ELEMENT_TYPE_STRING } },
    { CUSTOMATTR_DEPRECATEDHACK,     PN_DEPRECATEDHACK_CLASS, 0, {0}},
    { CUSTOMATTR_CLSCOMPLIANT,       PN_CLSCOMPLIANT,         4, { IMAGE_CEE_CS_CALLCONV_HASTHIS, 1, ELEMENT_TYPE_VOID, ELEMENT_TYPE_BOOLEAN }},
    { CUSTOMATTR_REQUIRED,           PN_REQUIRED_CLASS,       -1, {0} },
    { CUSTOMATTR_FRIENDASSEMBLY,     PN_FRIENDASSEMBLY_CLASS,  4, { IMAGE_CEE_CS_CALLCONV_HASTHIS, 1, ELEMENT_TYPE_VOID, ELEMENT_TYPE_STRING } },
    { CUSTOMATTR_PARAMS,             PN_PARAMARRAY_CLASS,     -1, {0} },
    { CUSTOMATTR_DEFAULTMEMBER,      PN_DEFAULTMEMBER_CLASS,  4, { IMAGE_CEE_CS_CALLCONV_HASTHIS, 1, ELEMENT_TYPE_VOID, ELEMENT_TYPE_STRING } },
    { CUSTOMATTR_DEFAULTMEMBER2,     PN_DEFAULTMEMBER_CLASS2, 4, { IMAGE_CEE_CS_CALLCONV_HASTHIS, 1, ELEMENT_TYPE_VOID, ELEMENT_TYPE_STRING } },
    { CUSTOMATTR_DECIMALLITERAL,     PN_DECIMALLITERAL_CLASS, 8, { IMAGE_CEE_CS_CALLCONV_HASTHIS, 5, ELEMENT_TYPE_VOID, ELEMENT_TYPE_U1, ELEMENT_TYPE_U1, ELEMENT_TYPE_U4, ELEMENT_TYPE_U4, ELEMENT_TYPE_U4}},
    { CUSTOMATTR_COCLASS,            PN_COCLASS_CLASS,        -1, {0} },
    { CUSTOMATTR_FIXEDBUFFER,        PN_FIXEDBUFFER_CLASS,    -1, {0} }, // Can't check the signature because it's non-primitive
    { CUSTOMATTR_COMPILATIONRELAXATIONS, PN_COMPILATIONRELAXATIONS_CLASS, -1, {0} }, // Don't care if they use enum or int
    { CUSTOMATTR_RUNTIMECOMPATIBILITY,   PN_RUNTIMECOMPATIBILTY_CLASS, -1, {0} }, 

};


/*
 * Constructor
 */
IMPORTER::IMPORTER()
{
    m_pfnCreateAssemblyNameObject = NULL;
    m_pfnCompareAssemblyIdentity = NULL;
    m_nameErrorAssem = NULL;
    inited = false;
    fLoadingTypes = false;
}


/*
 * Destructor
 */
IMPORTER::~IMPORTER()
{
    Term();
}


/*
 * Handle a generic meta-data API failure.
 */
void IMPORTER::MetadataFailure(HRESULT hr, PCWSTR pszFile)
{
    MetadataFailure(FTL_MetadataImportFailure, hr, pszFile);
}

/*
 * Handle an API failure. The passed error code is expected to take one insertion string
 * that is filled in with the HRESULT.
 */
void IMPORTER::MetadataFailure(int errid, HRESULT hr, PCWSTR pszFile)
{
    compiler()->Error(NULL, errid, compiler()->ErrHR(hr), pszFile);
}

/*
 * Report data truncation errors for imported identifiers which are too long.
 */
inline void IMPORTER::CheckTruncation(int requiredSize, int bufferLength, INFILESYM * infile)
{
    if (requiredSize > bufferLength) {
        VSFAIL("Truncation error!");
        CheckHR(CLDB_E_TRUNCATION, infile);
    }
}

/*
 * Report data truncation errors for imported identifiers which are too long.
 */
inline void IMPORTER::CheckTruncation(int requiredSize, int bufferLength, MODULESYM * scope)
{
    if (requiredSize > bufferLength) {
        VSFAIL("Truncation error!");
        CheckHR(CLDB_E_TRUNCATION, scope->getInputFile());
    }
}

/*
 * Report data truncation errors for imported identifiers which are too long.
 */
inline void IMPORTER::CheckTruncation(int requiredSize, int bufferLength, ImportScope & scope)
{
    if (requiredSize > bufferLength) {
        VSFAIL("Truncation error!");
        CheckHR(CLDB_E_TRUNCATION, scope);
    }
}

/*
 * Report badly formed metadata file.
 */
inline void IMPORTER::BogusMetadataFailure(PCWSTR pszFile)
{
    compiler()->Error(NULL, FTL_MalformedMetadata, pszFile);
}

/*
 * Initialize everything.
 */
void IMPORTER::Init()
{
    inited = true;
}

/*
 * Terminate everything.
 */
void IMPORTER::Term()
{
    if (inited) {
        POUTFILESYM outfile;
        PINFILESYM  infile;

        // Close all the outstanding meta-data scopes. They are all
        // children of the empty outfile symbol.
        outfile = compiler()->getBSymmgr().GetMDFileRoot();

        if (outfile) {
            // Go through all the inputfile symbols.
            for (infile = outfile->firstInfile();
                 infile != NULL;
                 infile = infile->nextInfile())
            {
                if (!infile->isSource) {
                    FOREACHCHILD(infile, symT)
                        if (symT->isMODULESYM())
                            symT->asMODULESYM()->Clear();
                    ENDFOREACHCHILD;
                }
                if (! infile->isSource && infile->assemimport) {
                    // Release the meta-data import scope.
                    infile->assemimport->Release();
                    infile->assemimport = NULL;
                }
            }
        }

        inited = false;
    }
}

int IMPORTER::ComputeArityFromName(PCWSTR className, PCWSTR * startOfArity)
{
    *startOfArity = NULL;

    for (PCWSTR pch = className; ; ) {
        for ( ; (*pch != L'!' && *pch != L'`'); pch++) {
            if (!*pch)
                return 0;
        }

        ASSERT(*pch == L'`' || *pch == L'!');
        PCWSTR pchLimName = pch++;

        // The number of type variables is limited to 16 bits by the metadata.
        int cvar;

        for (cvar = 0; *pch && iswdigit(*pch) && cvar < 0x10000; ) {
            cvar = cvar * 10 + (*pch++ - L'0');
        }

        if (!*pch) {
            // End of the string.
            if (0 < cvar && cvar < 0x10000) {
                *startOfArity = pchLimName;
                return cvar;
            }
            return 0;
        }
    }
}


/******************************************************************************
    If className has an arity specification (eg, !2):
        * Sets *pname to the name without arity
        * Sets *pnameWithArity to className
        * Returns the arity

    If className has no arity specification or an invalid specification (eg, !0):
        * Sets *pname to className
        * Sets *pnameWithArity to NULL
        * Returns 0
******************************************************************************/
int IMPORTER::StripArityFromName(PCWSTR className, NAME ** pname, NAME ** pnameWithArity)
{
    *pname = compiler()->namemgr->AddString(className);
    *pnameWithArity = NULL;

    PCWSTR beginningOfArity;
    int res = ComputeArityFromName(className, &beginningOfArity);
    if (beginningOfArity) {
        *pnameWithArity = *pname;
        *pname = compiler()->namemgr->AddString(className, (int)(beginningOfArity - className));
    }
    return res;
}

// Helper for ImportOneType. Extrated out of ImportOneType to save some stack
// space in that function and help prevent stack overflows for very deeply
// nested classes.
void IMPORTER::ImportOneType_SetAggKind( /*inout*/ AGGSYM * agg, MODULESYM * mod, LPCWSTR szFull, mdToken tkExtends, bool cvarEqualcvarOuter, DWORD flags )
{
    // Check to see if we extend System.ValueType, and are thus a value type.
    WCHAR baseClassText[MAX_FULLNAME_SIZE]; // name of the base class.

    // Default to class.
    agg->SetAggKind(AggKind::Class);

    if (!IsNilToken(tkExtends) && GetTypeRefFullName(mod, tkExtends, baseClassText, lengthof(baseClassText))) {
        if (!wcscmp(baseClassText, compiler()->getBSymmgr().GetFullName(PT_ENUM))) {
            agg->SetAggKind((cvarEqualcvarOuter) ? AggKind::Enum : AggKind::Struct);
            agg->isSealed = true;
        }
        else if (!(flags & tdAbstract) &&
            (!wcscmp(baseClassText, compiler()->getBSymmgr().GetFullName(PT_MULTIDEL)) ||
            !wcscmp(baseClassText, compiler()->getBSymmgr().GetFullName(PT_DELEGATE)) &&
            wcscmp(szFull, compiler()->getBSymmgr().GetFullName(PT_MULTIDEL))))
        {
            // We'll verify later that it has the correct constructor and an invoke method.
            agg->SetAggKind(AggKind::Delegate);
            agg->isSealed = !!(flags & tdSealed);
        }
        else if (!wcscmp(baseClassText, compiler()->getBSymmgr().GetFullName(PT_VALUE)) &&
            wcscmp(szFull, compiler()->getBSymmgr().GetFullName(PT_ENUM)))
        {
            agg->SetAggKind(AggKind::Struct);
            agg->isSealed = true;
        }
    }

    if (agg->AggKind() == AggKind::Class) {
        agg->isAbstract = !!(flags & tdAbstract);
        agg->isSealed = !!(flags & tdSealed);
    }
}

// Helper for ImportOneType. Extrated out of ImportOneType to save some stack
// space in that function and help prevent stack overflows for very deeply
// nested classes.
bool IMPORTER::ImportOneType_ProcessTypeVariable( IMetaDataImport2 * metaimportV2, mdGenericParam tokVar, INFILESYM *infile, LONG cvar, LONG cvarOuter, TYVARSYM ** prgvar, BAGSYM * bagPar, AGGSYM * agg )
{
    LONG ivar;
    DWORD flagsTypeVar;
    mdToken tokPar;
    WCHAR rgchName[MAX_FULLNAME_SIZE];
    ULONG cchName;

    CheckHR(metaimportV2->GetGenericParamProps(
        tokVar,
        (ULONG *)&ivar,
        &flagsTypeVar,
        &tokPar,
        NULL,
        rgchName,
        lengthof(rgchName),
        &cchName),
        infile);

    if (ivar < 0 || ivar >= cvar || prgvar[ivar]) {
        // Bad or duplicate index.
        return false;
    }

    if (ivar < cvarOuter) {
        prgvar[ivar] = bagPar->asAGGSYM()->typeVarsAll->ItemAsTYVARSYM(ivar);
        ASSERT(prgvar[ivar]->indexTotal == ivar);
    }
    else {
        NAME * name = compiler()->namemgr->AddString(rgchName);
        TYVARSYM * var = compiler()->getBSymmgr().CreateTyVar(name, agg);
        ASSERT(!var->isMethTyVar);
        var->SetAccess(ACC_PRIVATE);
        var->indexTotal = (short)ivar;
        var->index = (short)(ivar - cvarOuter);
        var->parseTree = NULL;
        prgvar[ivar] = var;
    }

    return true;
}

// Import one type. Create a symbol for the type in the symbol table. The symbol is created
// "undeclared" - no information about it is known except its:
// * fully qualified name
// * type variables (but not constraints)
// * accessibility
// * whether it is a value type (struct or enum) / interface / or other
// This expressly does not determine whether it's a delegate or attribute.
//
// Returns NULL if a fatal (for this input file) error occured.
/*
 * Import one type. Create a symbol for the type in the symbol
 * table. The symbol is created "undeclared" - no information about
 * it is known except its name.
 *
 * returns false if a fatal(for this input file) error occured
 *
 * REVIEW ShonK: is determining whether the type is a value type strictly necessary? We used to do when it was cheap,
 * but now that it requires checking the base class we should see if we could get rid of returning that information.
 * I'm not sure why we really need to know this information before the type is fully imported.
 */
AGGSYM *IMPORTER::ImportOneType(MODULESYM * mod, mdTypeDef token)
{
    if (TypeFromToken(token) != mdtTypeDef || token == mdTypeDefNil) {
        return NULL;
    }

    ImportScopeModule scope(this, mod);

    AGGSYM * agg;

    if (GetAggFromCache(scope, token, &agg))
        return agg;
    agg = NULL;

    DWORD flags;
    WCHAR szFull[MAX_FULLNAME_SIZE];  // full name of type.
    ULONG cchFull;
    PCWSTR pszAgg;

    BAGSYM * bagPar;   // parent (outer) bag (NS or AGG)
    DECLSYM * declPar; // parent (outer) declaration
    int aid;           // The assembly index (should always be the same assembly)
    mdToken tkExtends;

    IMetaDataImport *metaimport = mod->GetMetaImport(compiler());
    IMetaDataImport2 *metaimportV2 = mod->GetMetaImportV2(compiler());
    INFILESYM *infile = mod->getInputFile();

    // Get namespace, name, and flags for the type.
    CheckHR(metaimport->GetTypeDefProps(token, szFull, lengthof(szFull), &cchFull,
        &flags, &tkExtends), mod);
    CheckTruncation(cchFull, lengthof(szFull), mod);

    ASSERT(!COMPILER::IsRegString(szFull, L"declbreak"));

    ACCESS access; access = AccessFromTypeFlags(flags, infile);

    bool fMDPrivate; fMDPrivate = false;
    switch (access) {
    case ACC_UNKNOWN:
        ASSERT(0);
    case ACC_PRIVATE:
        fMDPrivate = infile->GetAssemblyID() != kaidThisAssembly;
        break;
    case ACC_INTERNAL:
        fMDPrivate = infile->GetAssemblyID() != kaidThisAssembly && !infile->InternalsVisibleTo(kaidThisAssembly);
        break;
    case ACC_PROTECTED:
    case ACC_INTERNALPROTECTED:
    case ACC_PUBLIC:
        ASSERT(!fMDPrivate);
        break;
    }

    // Unfortunately we have to import inaccessible types. There are a couple reasons for this.
    // 1) Suppose an interface I is private and derives from a public interface J. If class C implements
    //    I then it also implements J. We must import I in order to know that it implements J.
    // 2) To determine whether a struct is managed we need to import the types of all fields.
    // 3) It is legal in metadata to have an accessible member have an inaccessible return type.
    //
    // if (fMDPrivate)
    //    goto LDone;

    // Nested classes are handled differently. </STRIP>
    if (IsTdNested(flags)) {
        mdToken tkOuter;

        // Get the class this class is nested within.
        CheckHR(metaimport->GetNestedClassProps(token, &tkOuter), mod);

        if (TypeFromToken(tkOuter) != mdtTypeDef)
            goto LDone;

        if (!GetAggFromCache(scope, tkOuter, (AGGSYM **)&bagPar)) {
            // Hasn't been imported yet. We need to import it now for its type variables.
            bagPar = ImportOneType(mod, tkOuter);
        }
        if (!bagPar)
            goto LDone;

        declPar = bagPar->asAGGSYM()->DeclOnly();
        fMDPrivate |= bagPar->asAGGSYM()->isMDPrivate;
        aid = bagPar->asAGGSYM()->GetModuleID();

        // Use the full name whether or not there is a '.' in the name.
        pszAgg = szFull;
    }
    else  {
        pszAgg = szFull;
        SYM * symPar = ResolveParentScope(scope, mdModuleNil, &pszAgg);
        if (!symPar)
            goto LDone;
        ASSERT(symPar->isNSAIDSYM());
        bagPar = symPar->asNSAIDSYM()->GetNS();
        aid = symPar->asNSAIDSYM()->GetAid();
        declPar = GetNSDecl(bagPar->asNSSYM(), infile);
    }

    ASSERT(bagPar && declPar && declPar->Bag() == bagPar);
    ASSERT(aid == mod->GetModuleID());

    // Count the type variables for the agg.
    HCORENUM enumVars; enumVars = 0;
    LONG cvar; cvar = 0; // Total arity.

    if (metaimportV2) {
        CheckHR(metaimportV2->EnumGenericParams(&enumVars, token, NULL, 0, (ULONG *)&cvar), infile);
        CheckHR(metaimportV2->CountEnum(enumVars, (ULONG *)&cvar), infile);
    }

    ASSERT(cvar >= 0);

    int cvarOuter; cvarOuter = bagPar->isAGGSYM() ? bagPar->asAGGSYM()->typeVarsAll->size : 0;

    NAME * nameAgg;
    NAME * nameWithArity;
    int cvarFromName; // Incremental arity in name.

    // Strip any arity and place name of type in name table.
    cvarFromName = StripArityFromName(pszAgg, &nameAgg, &nameWithArity);
    ASSERT(!cvarFromName == !nameWithArity);

    // If the name contained the arity, it must match the imported metadata.
    if (cvarFromName != 0 && cvarFromName + cvarOuter != cvar) {
        nameAgg = nameWithArity;
        cvarFromName = 0;
    }

    // See if we've already imported it. This can happen if we encountered
    // a nested type before this one.
    agg = FindAggName(nameAgg, bagPar, aid, token);
    if (agg) {
        if (metaimportV2)
            metaimportV2->CloseEnum(enumVars);
        goto LDone;
    }

    if (mod->GetAssemblyID() == kaidThisAssembly) {
        // Check for any conflicting classes or namespaces in this assembly
        BAGSYM * bagConflict = NULL;
        symbmask_t mask = (cvarFromName > 0) ? MASK_AGGSYM : MASK_AGGSYM | MASK_NSSYM;

        // Make sure NOT to load any types!
        for (BAGSYM * bag = compiler()->getBSymmgr().LookupGlobalSymCore(nameAgg, bagPar, mask)->asBAGSYM();
            bag;
            bag = compiler()->getBSymmgr().LookupNextSym(bag, bagPar, mask)->asBAGSYM())
        {
            if (!bag->InAlias(compiler(), kaidThisAssembly))
                continue;

            if (bag->isNSSYM()) {
                ASSERT(cvarFromName == 0);
                ASSERT(!bagConflict);
                bagConflict = bag;
                // Keep looking in case there's a class
            }
            else {
                ASSERT(!bag->asAGGSYM()->isArityInName || bag->asAGGSYM()->typeVarsThis->size > 0);
                if (cvarFromName == (bag->asAGGSYM()->isArityInName ? bag->asAGGSYM()->typeVarsThis->size : 0)) {
                    bagConflict = bag;
                    break;
                }
            }
        }

        if (bagConflict != NULL) {
            compiler()->Error(mod->getInputFile(),
                bagPar->isAGGSYM() ? ERR_DuplicateNameInClass : ERR_DuplicateNameInNS,
                nameAgg, bagPar, ErrArgRefOnly(bagConflict->DeclFirst()));
            goto LDone;
        }
    }

    // Create the symbol for the new type.
    agg = compiler()->getBSymmgr().CreateAgg(nameAgg, declPar);
    compiler()->getBSymmgr().CreateAggDecl(agg, declPar);

    // Remember the token.
    ASSERT(agg->AggState() == AggState::None);
    agg->tokenImport = token;
    agg->module = mod;
    agg->SetAccess(access);
    agg->isMDPrivate = fMDPrivate;

    // Set what we can on the symbol structure from the flags.
    if (IsTdInterface(flags)) {
        // interface
        agg->SetAggKind(AggKind::Interface);
        agg->isAbstract = true;
    }
    else {
        // Check to see if we extend System.ValueType, and are thus a value type.
        ImportOneType_SetAggKind( agg, mod, szFull, tkExtends, cvar == cvarOuter, flags );
    }

    agg->isComImport = !!IsTdImport(flags);
    agg->isArityInName = (cvarFromName > 0);

    // If the inner type has fewer arguments than the outer type, it's bogus.
    if (cvar < cvarOuter) {
        ASSERT(metaimportV2);
        metaimportV2->CloseEnum(enumVars);
        goto LBogus;
    }

    // Now set the type variables, but not the bounds.
    if (cvar == 0) {
        ASSERT(cvarOuter == 0);
        if (metaimportV2) {
            metaimportV2->CloseEnum(enumVars);
        }
        agg->typeVarsThis = BSYMMGR::EmptyTypeArray();
        agg->typeVarsAll = BSYMMGR::EmptyTypeArray();
    }
    else {
        if (agg->IsStruct())
            agg->isManagedStruct = true;

        mdGenericParam * prgtokVars = STACK_ALLOC_ZERO(mdGenericParam, cvar);
        TYVARSYM ** prgvar = STACK_ALLOC_ZERO(TYVARSYM *, cvar);
        LONG ctok;

        CheckHR(metaimportV2->EnumGenericParams(&enumVars, agg->tokenImport, prgtokVars, cvar, (ULONG *)&ctok), infile);
        metaimportV2->CloseEnum(enumVars);

        ASSERT(ctok == cvar);

        // Process each type variable.
        for (int itok = 0; itok < cvar; ++itok) {
            if(!ImportOneType_ProcessTypeVariable( metaimportV2, prgtokVars[itok], infile, cvar, cvarOuter, prgvar, bagPar, agg )) {
                goto LBogus;
            }
        }

        if (cvarOuter > 0) {
            // We should have gotten the outer type variables.
            ASSERT(cvarOuter == bagPar->asAGGSYM()->typeVarsAll->size);
            ASSERT(0 == memcmp(prgvar, bagPar->asAGGSYM()->typeVarsAll->ItemPtr(0), cvarOuter * sizeof(TYVARSYM *)));

            agg->typeVarsThis = compiler()->getBSymmgr().AllocParams(cvar - cvarOuter, (TYPESYM **)prgvar + cvarOuter);
            agg->typeVarsAll = compiler()->getBSymmgr().AllocParams(cvar, (TYPESYM **)prgvar);
        }
        else {
            agg->typeVarsAll = compiler()->getBSymmgr().AllocParams(cvar, (TYPESYM **)prgvar);
            agg->typeVarsThis = agg->typeVarsAll;
        }
    }

    ASSERT(agg->typeVarsThis && agg->typeVarsAll);
    agg->SetAggState(AggState::Declared);

    goto LDone;

LBogus:
    // Set it bogus. Inherit outer type variables and assume no new ones.
    agg->typeVarsThis = BSYMMGR::EmptyTypeArray();
    if (cvarOuter > 0)
        agg->typeVarsAll = bagPar->asAGGSYM()->typeVarsAll;
    else
        agg->typeVarsAll = agg->typeVarsThis;

    agg->setBogus(true);
    agg->SetAggState(AggState::Declared);

LDone:
    SetSymInCache(scope, token, agg);

    return agg;
}


/***************************************************************************************************
    Create namespace declarations for all the namespaces in the given infile. If fTypes is true,
    also load the types. Otherwise, record all the type defs for deferred loading.
***************************************************************************************************/
void IMPORTER::ImportNamespaces(INFILESYM * infile, bool fTypes)
{
    SETLOCATIONFILE(infile);

    ASSERT(!infile->isAddedModule || infile->cscope == 1);

    // Enumerate all the types in each scope.
    for (int i = 0; i < infile->cscope; i++) {

        CComPtr<IMetaDataImport2> qmdi;

        CheckHR(FTL_MetadataCantOpenFile,
            compiler()->linker->GetScope2(compiler()->assemID, infile->mdImpFile, i, &qmdi),
            infile);

        if (qmdi == NULL) {
            // A non-metadata file
            continue;
        }

        // Get the module name and create the MODULESYM.
        WCHAR rgch[MAX_FULLNAME_SIZE];
        ulong cch;

	    ASSERT(sizeof(ulong) == sizeof(ULONG));
        CheckHR(qmdi->GetScopeProps(rgch, lengthof(rgch), (ULONG *)&cch, NULL), infile);
        CheckTruncation(cch, lengthof(rgch), infile);

        NAME * name = compiler()->getNamemgr()->AddString(rgch);

        MODULESYM * mod = compiler()->getBSymmgr().CreateModule(name, infile);
        mod->Init(i, qmdi);
        if (!infile->moduleManifest) {
            ASSERT(i == 0);
            infile->moduleManifest = mod;

            // We only check assembly attributes here, module attributes are in ImportTypes()
            IMPORTED_CUSTOM_ATTRIBUTES attributes;
            ImportCustomAttributes(infile->moduleManifest, infile, &attributes, mdtAssembly);

            if (attributes.hasCLSattribute) {
                infile->hasCLSattribute = true;
                infile->isCLS = attributes.isCLS;
            } else if (!infile->isAddedModule) {
                // if there's no assembly level attribute
                // assume CLSCompliant(false), except for added modules
                // because they might have a module level attribute
                infile->hasCLSattribute = true;
                infile->isCLS = false;
            }

            if (infile->isAddedModule && attributes.fCompilationRelaxations) {
                compiler()->SuppressRelaxations();
            }

            if (compiler()->BuildAssembly() && infile->isAddedModule && attributes.fRuntimeCompatibility) {
                compiler()->SuppressRuntimeCompatibility();
                if (!attributes.fWrapNonExceptionThrows)
                    compiler()->SuppressWrapNonExceptionThrows();
            }

            if (attributes.fHasFriends && infile->isAddedModule) {
                compiler()->SetFriendsDeclared();
            }
        }

        // Import all types for added modules because they will be emitted to this assembly and 
        // we need to make sure we do not have duplicate types (i.e. a declared type and an exported type to the module).
        ImportNamespaces(mod, fTypes || infile->isAddedModule);

        if (infile->isAddedModule) {
            // Import module-level CLS attributes if we're doing an add-module.
            IMPORTED_CUSTOM_ATTRIBUTES attributes;
            mdModule impModule;

            CheckHR(qmdi->GetModuleFromScope(&impModule), infile);
            ImportCustomAttributes(mod, NULL, &attributes, impModule);

            infile->hasModuleCLSattribute = attributes.hasCLSattribute;
            if (!infile->hasCLSattribute)
                infile->isCLS = attributes.isCLS;
        }
    }
}


/***************************************************************************************************
    Create namespace declarations for all the namespaces in the given module. If fTypes is true,
    also load the types. Otherwise, record all the type defs for deferred loading. Asks the global
    symbol table whether a type needs to be pre-loaded. This is for predefined types.
***************************************************************************************************/
void IMPORTER::ImportNamespaces(MODULESYM * mod, bool fTypes)
{
    if (mod == mod->getInputFile()->moduleManifest)
        ImportTypeForwarders(mod);

    IMetaDataImport * pmdi = mod->GetMetaImport(compiler());

    WCHAR rgch[MAX_FULLNAME_SIZE];
    ulong cch;
    HCORENUM enumTypeDefs = NULL;
    ulong ctok;

    ASSERT(sizeof(ulong) == sizeof(ULONG));
    CheckHR(pmdi->EnumTypeDefs(&enumTypeDefs, NULL, 0, (ULONG *)&ctok), mod);

    CheckHR(pmdi->CountEnum(enumTypeDefs, (ULONG *)&ctok), mod);

    if (!ctok) {
        mod->prgmti = NULL;
        mod->cmti = 0;
        pmdi->CloseEnum(enumTypeDefs);
        return;
    }

    if (fTypes) {
        mdToken rgtok[32];

        do {
            // Get next batch of types.
            CheckHR(pmdi->EnumTypeDefs(&enumTypeDefs, rgtok, lengthof(rgtok), (ULONG* )&ctok), mod);

            // Process each type.
            for (ulong itok = 0; itok < ctok; ++itok)
                ImportOneType(mod, rgtok[itok]);

        } while (ctok > 0);

        mod->prgmti = NULL;
        mod->cmti = 0;
        pmdi->CloseEnum(enumTypeDefs);
        return;
    }

    mod->prgmti = (ModTypeInfo *)compiler()->getGlobalSymAlloc().Alloc(ctok * (sizeof(ModTypeInfo)));
    mod->cmti = (int)ctok;

    // Temporarily put all the tokens at the end of the block.
    mdToken * prgtok = (mdToken *)(mod->prgmti + ctok) - ctok;

    ulong ctokTmp;

    CheckHR(pmdi->EnumTypeDefs(&enumTypeDefs, prgtok, ctok, (ULONG *)&ctokTmp), mod);
    pmdi->CloseEnum(enumTypeDefs);

    ASSERT(ctok == ctokTmp);
    mod->cmti = (int)ctokTmp;

    ImportScopeModule scope(this, mod);

    // Process each type def.
    int imti = 0;
    for (int itok = 0; itok < mod->cmti; ++itok) {
        mdToken tok = prgtok[itok];
        if (TypeFromToken(tok) != mdtTypeDef || tok == mdTypeDefNil) {
            // Bad token so just skip it (don't increment imti).
            continue;
        }


        DWORD flags;
        mdToken tokBase;

        // Get namespace, name, and flags for the type.
        CheckHR(pmdi->GetTypeDefProps(tok, rgch, lengthof(rgch), (ULONG *)&cch,
            &flags, &tokBase), mod);
        CheckTruncation(cch, lengthof(rgch), mod);

        // Nested classes are handled differently.
        if (IsTdNested(flags)) {
            mdToken tokOuter;

            // Get the class this class is nested within.
            CheckHR(pmdi->GetNestedClassProps(tok, &tokOuter), mod);

            if (tokOuter == tok)
                continue;

            // Search for tokOuter.
            int imtiOuter;

            for (imtiOuter = imti; --imtiOuter >= 0 && mod->prgmti[imtiOuter].tok != tokOuter; )
                ;
            if (imtiOuter < 0) {
                int itokOuter;
                for (itokOuter = itok; ++itokOuter < mod->cmti && prgtok[itokOuter] != tokOuter; )
                    ;
                if (itokOuter >= mod->cmti) {
                    // Can't find outer type so see if we already loaded it for some reason.
                    // If not, just skip it (don't increment imti).
                    AGGSYM * aggPar;
                    if (GetAggFromCache(scope, tokOuter, &aggPar) && aggPar)
                        ImportOneType(mod, tok);
                    continue;
                }

                prgtok[itok] = tokOuter;
                prgtok[itokOuter] = tok;
                // Redo this slot.
                itok--;
                continue;
            }

            ASSERT(0 <= imtiOuter && imtiOuter < imti);
            mod->prgmti[imti].tok = tok;
            mod->prgmti[imti].tokRoot = mod->prgmti[imtiOuter].tokRoot;
            mod->prgmti[imti].ns = mod->prgmti[imtiOuter].ns;
            ASSERT(mod->prgmti[imti].ns);
            imti++;
        }
        else {
            PCWSTR pszAgg = rgch;
            NSAIDSYM * nsaPar = ResolveNamespaceOfClassName(&pszAgg, kaidGlobal /* aid doesn't matter */);
            if (!nsaPar)
                continue;

            NSSYM * ns = nsaPar->GetNS();
            ASSERT(ns);

            if (compiler()->getBSymmgr().FPreLoad(ns, pszAgg)) {
                ImportOneType(mod, tok);
                continue;
            }

            mod->prgmti[imti].tok = tok;
            mod->prgmti[imti].tokRoot = tok;
            mod->prgmti[imti].ns = ns;
            imti++;

            // Tell the namespace that this module has some types in it.
            // This happens by simply making sure the NSDECLSYM exists.
            GetNSDecl(ns, mod->getInputFile());
            ASSERT(ns->InAlias(compiler(), mod->GetAssemblyID()));

            ns->SetTypesUnloaded(mod->GetAssemblyID(), &compiler()->getGlobalSymAlloc());
        }
    }

    // Some may have failed so set cmti to the actual number that we kept.
    mod->cmti = imti;

    // Now sort them.
    SortModTypeInfos(mod->prgmti, mod->cmti);
}


/***************************************************************************************************
   Given an exported type specified by the token passed in, this method will first determine if it
   is a valid type forwarder and, if so, will create a FWDAGGSYM to represent it and add that sym
   to the symbol table.
***************************************************************************************************/
FWDAGGSYM *IMPORTER::ImportOneTypeForwarder(MODULESYM * mod, mdExportedType token)
{
    if (TypeFromToken(token) != mdtExportedType || token == mdExportedTypeNil)
        return NULL;

    ImportScopeModule scope(this, mod);

    DWORD flags;
    WCHAR szFull[MAX_FULLNAME_SIZE]; 
    ULONG cchFull;

    IMetaDataAssemblyImport *assemblyimport = mod->GetAssemblyImport(compiler());
    INFILESYM *infile = mod->getInputFile();

    mdToken tkAssemblyRef = mdTokenNil;
    mdToken tkTypeDef = mdTokenNil;

    // Get namespace, name, and flags for the type.
    assemblyimport->GetExportedTypeProps(
        token,              // [IN] The ExportedType for which to get the properties.
        szFull,             // [OUT] Buffer to fill with name.
        lengthof(szFull),   // [IN] Size of buffer in wide chars.
        &cchFull,           // [OUT] Actual # of wide chars in name.
        &tkAssemblyRef,     // [OUT] mdFile or mdAssemblyRef or mdExportedType.
        &tkTypeDef,         // [OUT] TypeDef token within the file.
        &flags);            // [OUT] Flags.
    CheckTruncation(cchFull, lengthof(szFull), mod);

    if (TypeFromToken(tkAssemblyRef) != mdtAssemblyRef || !(flags & tdForwarder))
        return NULL;

    FWDAGGSYM * fwd = NULL;
    PCWSTR pszAgg = szFull;
    NSAIDSYM * nsaPar = ResolveParentScope(scope, mdModuleNil, &pszAgg)->asNSAIDSYM();

    if (nsaPar) {
        ASSERT(nsaPar->GetAid() == mod->GetModuleID());
        NSDECLSYM * nsdPar = GetNSDecl(nsaPar->GetNS(), infile);

        NAME * nameAgg;
        NAME * nameWithArity;
        int cvarFromName; // Incremental arity in name.

        // Strip any arity and place name of type in name table.
        cvarFromName = StripArityFromName(pszAgg, &nameAgg, &nameWithArity);
        ASSERT(!cvarFromName == !nameWithArity);

        // Create the symbol for the new forwarder.
        fwd = compiler()->getBSymmgr().CreateFwdAgg(nameAgg, nsdPar);
        fwd->tokenImport = token;
        fwd->module = mod;
        fwd->tkAssemblyRef = tkAssemblyRef;
        fwd->cvar = cvarFromName;
    }

    SetSymInCache(scope, token, fwd);

    return fwd;
}


/***************************************************************************************************
   Import all type forwarders specified in the given module as FWDAGGSYMs.  
***************************************************************************************************/
void IMPORTER::ImportTypeForwarders(MODULESYM *mod)
{
    IMetaDataImport *pImport = mod->GetMetaImport(compiler());
    IMetaDataAssemblyImport * pmdi = mod->GetAssemblyImport(compiler());

    HCORENUM enumTypeDefs = NULL;
    ulong ctok;

    ASSERT(sizeof(ulong) == sizeof(ULONG));
    CheckHR(pmdi->EnumExportedTypes(&enumTypeDefs, NULL, 0, (ULONG *)&ctok), mod);

    CheckHR(pImport->CountEnum(enumTypeDefs, (ULONG *)&ctok), mod);

    if (!ctok) {
        pmdi->CloseEnum(enumTypeDefs);
        return;
    }

    mdToken rgtok[32];

    do {
        // Get next batch of types.
        CheckHR(pmdi->EnumExportedTypes(&enumTypeDefs, rgtok, lengthof(rgtok), (ULONG *)&ctok), mod);

        // Process each type.
        for (ulong i = 0; i < ctok; i++) {
            ImportOneTypeForwarder(mod, rgtok[i]);
        }
    } while (ctok > 0);

    pmdi->CloseEnum(enumTypeDefs);
    return;
}

/***************************************************************************************************
    The ModTypeInfos are sorted by (tokRoot, tok) so we can easily load all types with the same
    root type (outer-most type).

    Note that very often the data will already be sorted.
***************************************************************************************************/
void IMPORTER::SortModTypeInfos(ModTypeInfo * prgmti, int cmti)
{
    int i;
    int j;
    ModTypeInfo mti;

    for (;;) {
        if (cmti < 2)
            return;

        if (cmti < 10) {
            for (i = 1; i < cmti; i++) {
                if (prgmti[i - 1] <= prgmti[i])
                    continue;
                mti = prgmti[i];
                for (j = i; ; ) {
                    prgmti[j] = prgmti[j - 1];
                    if (--j == 0 || prgmti[j - 1] <= mti)
                        break;
                }
                prgmti[j] = mti;
            }
            return;
        }

        // Use QuickSort.
        i = 0;
        j = cmti - 1;
        ModTypeInfo mtiKey = prgmti[cmti / 2];

        // Partition. The loop invariant is:
        // * everything from 0 to (i-1) is < the key
        // * the key is < everything from (j+1) to (cmti-1)
        // * i < j
        for (;;) {
            while (prgmti[i] < mtiKey)
                i++;
            while (mtiKey < prgmti[j])
                j--;
            if (i >= j)
                break;

            // There shouldn't be duplicates in our data set. If there is
            // we'll be stuck in an infinite loop!
            ASSERT(prgmti[i].tok != prgmti[j].tok);
            mti = prgmti[i];
            prgmti[i] = prgmti[j];
            prgmti[j] = mti;

            ASSERT(i < j && prgmti[i] <= mtiKey && mtiKey <= prgmti[j]);
        }

        ASSERT(i == j && i < cmti && prgmti[i].tok == mtiKey.tok);

        // Now sort the two pieces....
        // Recurse on the small piece and iterate on the big one. This limits the stack depth
        // to O(log(n)).
        if (i <= cmti / 2) {
            // First piece is smaller.
            SortModTypeInfos(prgmti, i);
            prgmti += i + 1;
            cmti -= i + 1;
        }
        else {
            // Second piece is smaller.
            SortModTypeInfos(prgmti + i + 1, cmti - i - 1);
            cmti = i;
        }
    }
}


/***************************************************************************************************
    Load types in the given namespace and aid. If aid is kaidNil, don't filter on aid. If infile
    is not null, only load types from that infile.
***************************************************************************************************/
void IMPORTER::LoadTypesInNsAid(NSSYM * ns, int aid, INFILESYM * infile)
{
    // CAUTION: Make sure that this doesn't call anything that might cause recursion into here!
    if (fLoadingTypes) {
        VSFAIL("Shouldn't be recursing in loading types!");
        return;
    }

    NSDECLSYM * nsd;
    
    if (infile)
        nsd = NULL;
    else if (!(nsd = ns->DeclFirst()))
        return;

    fLoadingTypes = true;
    bool fClearThisAssembly = false;

    for (;;) {
        ASSERT(!nsd || nsd->Bag() == ns);
        int aidCur;

        if (nsd)
            infile = nsd->inputfile;

        if (!infile->isSource && (aid == kaidNil || infile->InAlias(aid)) &&
            ns->TypesUnloaded(aidCur = infile->GetAssemblyID()))
        {
            if (infile->isAddedModule) {
                // We can't clear the bit for added modules yet since there may be more than
                // one module (they share the same assembly id).
                ASSERT(aidCur == kaidThisAssembly);
                fClearThisAssembly = true;
            }
            else
                ns->ClearTypesUnloaded(aidCur);
            for (SYM * symChd = infile->firstChild; symChd; symChd = symChd->nextChild) {
                if (symChd->isMODULESYM())
                    LoadTypesInNsMod(ns, symChd->asMODULESYM());
            }
        }

        // Get the next infile.
        if (!nsd || !(nsd = nsd->DeclNext()))
            break;
    }

    if (fClearThisAssembly)
        ns->ClearTypesUnloaded(kaidThisAssembly);

    ASSERT(fLoadingTypes);
    fLoadingTypes = false;
}


/***************************************************************************************************
    Load the types in the given namespace from the given module.
***************************************************************************************************/
void IMPORTER::LoadTypesInNsMod(NSSYM * ns, MODULESYM * mod)
{
    ASSERT(fLoadingTypes);

    int imtiDst = 0;

    for (int imtiSrc = 0; imtiSrc < mod->cmti; imtiSrc++) {
        if (mod->prgmti[imtiSrc].ns == ns) {
            // We need to load this type.
            ImportOneType(mod, mod->prgmti[imtiSrc].tok);
            // Don't increment itnpDst - we don't need it anymore.
        }
        else {
            if (imtiDst < imtiSrc)
                mod->prgmti[imtiDst] = mod->prgmti[imtiSrc];
            imtiDst++;
        }
    }

    ASSERT(0 <= imtiDst && imtiDst <= mod->cmti);
    mod->cmti = imtiDst;

    ASSERT(fLoadingTypes);
}


/*
 * Import all top level types from all metadata scopes.
 */
void IMPORTER::ImportAllTypes()
{
    SETLOCATIONSTAGE(IMPORT);

    POUTFILESYM outfile;
    PINFILESYM  infile;

    outfile = compiler()->getBSymmgr().GetMDFileRoot();

    if (outfile) {
        int oldErrors = compiler()->ErrorCount();
        // Go through all the inputfile symbols
        // and do an open scope.
        for (infile = outfile->firstInfile();
             infile != NULL;
             infile = infile->nextInfile())
        {
            if (!infile->isSource) {
                OpenAssembly(infile);
            }
        }

        CErrorSuppression es;

        // if we couldn't open one of the references, stop here and don't import types
        if (compiler()->FAbortEarly(oldErrors, &es))
            return;

        // Go through all the inputfile symbols
        // and check for unification conflicts
        for (infile = outfile->firstInfile();
             infile != NULL;
             infile = infile->nextInfile())
        {
            if (!infile->isSource && !infile->isAddedModule && !infile->getBogus())
            {
                for ( INFILESYM * infileOther = outfile->firstInfile();
                    infileOther != infile;
                    infileOther = infileOther->nextInfile())
                {
                    if (infileOther->isSource || infileOther->isAddedModule || infileOther->getBogus())
                        continue;
                    if (CompareImports(infileOther, infile))
                        break;
                }
            }
        }

        // Go through all the inputfile symbols.
        // and import the types.
        for (infile = outfile->firstInfile();
             infile != NULL;
             infile = infile->nextInfile())
        {
            if (!infile->isSource && !infile->getBogus()) {
                ImportNamespaces(infile,
                    false
                    );
            }
        }
    }
}


/*
 * Given a name space, find or create a corresponding NSDECL symbol for this
 * input file.
 */
PNSDECLSYM IMPORTER::GetNSDecl(NSSYM * ns, PINFILESYM infile)
{
    ASSERT(infile);

    //
    // first search existing declarations for this namespace
    // for a declaration with this inputfile
    //
    NSDECLSYM * nsd;

    for (nsd = ns->DeclFirst(); nsd; nsd = nsd->DeclNext()) {
        if (nsd->getInputFile() == infile)
            return nsd;
    }

    // Didn't find an existing declaration for this namespace/file combo so create one.
    NSSYM * nsPar = ns->Parent();
    NSDECLSYM * nsdPar = nsPar ? GetNSDecl(nsPar, infile) : NULL;

    nsd = compiler()->getBSymmgr().CreateNamespaceDecl(ns, nsdPar, infile, NULL);
    nsd->usingClausesResolved = true;
    nsd->isDefined = true;
    if (!nsdPar) {
        ASSERT(!infile->rootDeclaration);
        infile->rootDeclaration = nsd;
    }
    ASSERT(infile->rootDeclaration);

    return nsd;
}

/*
 * Convert a fully qualified namespace string to a namespace symbol. An existing 
 * namespace symbol is returned if present, or a new namespace symbol
 * is created.  If the namespace string has errors, it returns NULL without reporting an error).
 */
PNSSYM IMPORTER::ResolveNamespace(PCWCH namespaceText, int cch)
{
    PCWCH p;
    PCWCH start;
    PNAME name;
    PNSSYM ns;

    ns = compiler()->getBSymmgr().GetRootNS();  // start at the root namespace.
    p = namespaceText;

    if (!p || !cch)
        return ns;

    for (;;) {
        ASSERT(p - namespaceText < cch);

        start = p;
        // Advance p to the end of the next segment.
        p = wcsnchr(p, L'.', cch - (p - namespaceText));

        if (p == start) {
            // Namespace can't start with . or contain ..
            return NULL;
        }

        // Find/Create a sub-namespace with this name.
        name = compiler()->namemgr->AddString(start, (int)(p - start));

        // Check for existing namespace.
        NSSYM * nsNext = compiler()->LookupGlobalSym(name, ns, MASK_NSSYM)->asNSSYM();
        if (nsNext)
            ns = nsNext;
        else
            ns = compiler()->getBSymmgr().CreateNamespace(name, ns);

        // Are we done?
        if (p - namespaceText >= cch)
            return ns;

        // Continue to the next segment.
        ++p;
    }
}

TYPESYM * IMPORTER::ResolveFullMetadataTypeName(MODULESYM * mod, PCWSTR pszClass, bool * pfIsInvalidSig)
{
    if (pfIsInvalidSig)
        *pfIsInvalidSig = false;

    DWORD ichErrorLoc;
    CComPtr<ITypeName> qtnType;
    HRESULT hr = compiler()->GetTypeNameFactory()->ParseTypeName( pszClass, &ichErrorLoc, &qtnType);
    CheckHR(hr, mod);
    if (hr == S_FALSE) {
        ASSERT(!qtnType);
        if (pfIsInvalidSig)
            *pfIsInvalidSig = true;
        return NULL;
    }

    ASSERT(!!qtnType);
    return ResolveTypeNameCore(mod, qtnType);
}

// Walks a string that was parsed into an ITypeName and produces the proper
// TYPESYM or NULL (if the type is unresolved for instance)
TYPESYM * IMPORTER::ResolveTypeNameCore(MODULESYM * mod, ITypeName * ptnType)
{
    HRESULT hr;
    BSTR bstrAssemblySpec;
    int aid = mod->GetAssemblyID();
    bool fTryMsCorLib = true;

    CheckHR(hr = ptnType->GetAssemblyName( &bstrAssemblySpec), mod);

    if (bstrAssemblySpec && SysStringLen(bstrAssemblySpec)) {
        ImportScopeModule scope(this, mod);
        NAME * nameAssemblySpec = compiler()->getNamemgr()->AddString(bstrAssemblySpec);
        SysFreeString(bstrAssemblySpec);
        aid = MapAssemblyRefToAid(nameAssemblySpec, scope, false);
        fTryMsCorLib = false;

        if (aid == kaidUnresolved || aid == kaidErrorAssem)
            return NULL;
    }
    else if (bstrAssemblySpec) {
        SysFreeString(bstrAssemblySpec);
    }

    AGGTYPESYM * ats = ResolveTypeNames(mod, aid, fTryMsCorLib, ptnType);
    if (!ats)
        return NULL;

    ats = ResolveTypeArgs(mod, ats, ptnType);
    if (!ats)
        return NULL;

    return ResolveModifiers(mod, ats, ptnType);
}

AGGTYPESYM * IMPORTER::ResolveTypeNames(MODULESYM * mod, int aid, bool fTryMsCorLib, ITypeName * ptnType)
{
    HRESULT hr;
    AGGTYPESYM * ats = NULL;
    DWORD cntName, iName = 0;
    CheckHR(hr = ptnType->GetNameCount(&cntName), mod);
    ASSERT(cntName < (DWORD)-1 && cntName > 0);
    BSTR * prgbstrName = STACK_ALLOC_ZERO(BSTR, cntName);
    CheckHR(hr = ptnType->GetNames(cntName, prgbstrName, &cntName), mod);
    ASSERT(hr == S_OK);

    BAGSYM * bagPar;
    PCWSTR pszName = prgbstrName[0];
    NSAIDSYM * nsa = ResolveNamespaceOfClassName(&pszName, aid);
    if (!nsa) {
        // This should only fail if the namespace text is invalid, but the parse succeeded
        VSFAIL("Why did ResolveNamespaceOfClassName fail?");
        goto CLEANUP;
    }
    bagPar = nsa->GetNS();
    TYPESYM * type; type = ResolveTypeName(pszName, bagPar, aid, NULL, NameResOptions::FavorNull);
    if ((!type || !type->isAGGTYPESYM()) && fTryMsCorLib) {
        aid = compiler()->GetReqPredefAgg(PT_OBJECT)->GetAssemblyID();
        type = ResolveTypeName(pszName, bagPar, aid, NULL, NameResOptions::FavorNull);
    }
    if (type && type->isAGGTYPESYM()) {
        ats = type->asAGGTYPESYM();
        bagPar = type->getAggregate();
        aid = bagPar->asAGGSYM()->GetAssemblyID();
    }
    else {
        goto CLEANUP;
    }

    for (iName = 1; iName < cntName && type && type->isAGGTYPESYM(); iName++) {
        type = ResolveTypeName(prgbstrName[iName], bagPar, aid, NULL, NameResOptions::FavorNull);
        prgbstrName[iName] = NULL;
        if (!type || !type->isAGGTYPESYM()) {
            ats = NULL;
            break;
        }
        ats = type->asAGGTYPESYM();
        bagPar = type->getAggregate();
    }

CLEANUP:
    for (DWORD x = 0; x < cntName; x++) {
        SysFreeString(prgbstrName[x]);
    }

    return ats;
}

AGGTYPESYM * IMPORTER::ResolveTypeArgs(MODULESYM * mod, AGGTYPESYM * ats, ITypeName * ptnType)
{
    HRESULT hr;
    DWORD cntTypeArg;
    CheckHR(hr = ptnType->GetTypeArgumentCount(&cntTypeArg), mod);
    ASSERT(cntTypeArg < (DWORD)-1);
    if (cntTypeArg != (DWORD)ats->typeArgsAll->size)
        return NULL;
    if (cntTypeArg == 0)
        return ats;

    ITypeName ** prgptnTypeArg = STACK_ALLOC_ZERO(ITypeName*, cntTypeArg);
    CheckHR(hr = ptnType->GetTypeArguments(cntTypeArg, prgptnTypeArg, &cntTypeArg), mod);
    ASSERT(hr == S_OK);
    TYPESYM ** prgtypeArg = STACK_ALLOC_ZERO(TYPESYM*, cntTypeArg);
    bool fFailed = false;

    for (DWORD iTypeArg = 0; iTypeArg < cntTypeArg; iTypeArg++) {
        if (!fFailed && NULL == (prgtypeArg[iTypeArg] = ResolveTypeNameCore(mod, prgptnTypeArg[iTypeArg])))
            fFailed = true;
        prgptnTypeArg[iTypeArg]->Release();
        prgptnTypeArg[iTypeArg] = NULL;
    }
    if (fFailed)
        return NULL;

    return compiler()->getBSymmgr().SubstType(ats, compiler()->getBSymmgr().AllocParams(cntTypeArg, prgtypeArg))->asAGGTYPESYM();
}

TYPESYM * IMPORTER::ResolveModifiers(MODULESYM * mod, AGGTYPESYM * ats, ITypeName * ptnType)
{
    HRESULT hr;
    DWORD cntMod;
    TYPESYM * type = ats;
    CheckHR(hr = ptnType->GetModifierLength(&cntMod), mod);
    ASSERT(cntMod < (DWORD)-1);
    if (cntMod == 0)
        return type;

    DWORD * prgdwMod = STACK_ALLOC_ZERO(DWORD, cntMod);
    CheckHR(hr = ptnType->GetModifiers(cntMod, prgdwMod, &cntMod), mod);
    ASSERT(hr == S_OK);

    for (DWORD iMod = 0; iMod < cntMod; iMod++) {
        switch (prgdwMod[iMod]) {
        case ELEMENT_TYPE_SZARRAY:
            type = compiler()->getBSymmgr().GetArray(type, 1);
            break;

        case ELEMENT_TYPE_ARRAY:
            iMod++;
            ASSERT(iMod < cntMod);
            if (iMod >= cntMod)
                return NULL;
            ASSERT(prgdwMod[iMod] != 1); // This should be an SZARRAY
            if (prgdwMod[iMod] == 0) {
                // Unknown rank array is not supported
                return NULL;
            }
            type = compiler()->getBSymmgr().GetArray(type, prgdwMod[iMod]);
            break;

        case ELEMENT_TYPE_PTR:
            type = compiler()->getBSymmgr().GetPtrType(type);
            break;

        default:
            VSFAIL("Unrecognized (but parse-able) type modifier!");
            return NULL;

        case ELEMENT_TYPE_BYREF:
            VSFAIL("We don't have a representation for byref types in the language");
            return NULL;
        }
    }

    return type;
}


NSAIDSYM * IMPORTER::ResolveNamespaceOfClassName(PCWSTR * ppsz, int aid)
{
    PCWCH prgchNS;
    int cchNS;
    PCWSTR pszBase = wcsrchr(*ppsz, L'.');

    if (pszBase) {
        prgchNS = *ppsz;
        cchNS = (int)(pszBase - *ppsz);
        *ppsz = pszBase + 1;
    }
    else {
        prgchNS = NULL;
        cchNS = 0;
    }

    // Convert namespace name into a namespace symbol.
    NSSYM * ns = ResolveNamespace(prgchNS, cchNS);
    if (!ns)
        return NULL;

    return compiler()->getBSymmgr().GetNsAid(ns, aid);
}


// Always returns either a TYPESYM or an NSAIDSYM. In theory an ERRORSYM could be returned,
// but we should never produce meta-data resulting in one.
SYM * IMPORTER::ResolveParentScope(ImportScope & scope, mdToken tkPar, PCWSTR * ppsz)
{
    int aid;

    switch (TypeFromToken(tkPar)) {
    default:
        return NULL;

    case mdtTypeDef:
        VSFAIL("Why is a type def a parent scope of a type ref?");
        return NULL;

    case mdtTypeRef:
        // Nested class. Get the parent class from the resolution scope.
        // NOTE: we don't know the arity of the parent here so can't pass it through!
        // The arity annotation is the best we can hope for.
        return ResolveTypeRef(scope, tkPar, NULL);

    case mdtAssemblyRef:
        aid = MapAssemblyRefToAid(scope, tkPar);
        break;

    case mdtAssembly:
        aid = scope.GetAssemblyID();
        break;

    case mdtModule:
        aid = scope.GetModuleID();
        break;

    case mdtModuleRef:
        aid = MapModuleRefToAid(scope, tkPar);
        if (aid == kaidUnresolved)
            return NULL;
        break;
    }

    return ResolveNamespaceOfClassName(ppsz, aid);

}


/***************************************************************************************************
    Resolves a type name to an actual type. If the class is not found, and aid is for an unresolved
    module then a new class is created.  If aid is not for an unresolved module and the class is not
    found, we return NULL. The caller should then report a warning.

    The typeArgs are used to instantiate the type. If typeArgs is NULL, the arity is matched
    with the decorated name's arity.

    The token parameter is the type def token (if known). NOTE: Use mdTypeDefNil to indicate
    that the token isn't known. Use mdTokenNil to indicate that the type should have no import
    token! (The latter is used for EnC).
***************************************************************************************************/
TYPESYM * IMPORTER::ResolveTypeName(PCWSTR className, BAGSYM * bagPar, int aid, TypeArray * typeArgs, NameResOptionsEnum nro, mdTypeDef token)
{
    ASSERT(bagPar);
    ASSERT(!bagPar->isAGGSYM() || bagPar->asAGGSYM()->InAlias(aid));
    ASSERT(aid != kaidUnresolved);

    if (aid == kaidErrorAssem) {
        ASSERT(bagPar->isNSSYM());
        if (!compiler()->options.m_fCompileSkeleton) {
            return NULL;
        }

        NAME * name = compiler()->namemgr->AddString(className);
        TYPESYM * typePar = NULL;
        TypeArray * typeArgsActual = NULL;

        if (typeArgs && typeArgs->size > 0) {
            // The parent type is encoded as the first type argument. void * indicates no parent type.
            if (!typeArgs->Item(0)->isPTRSYM()) {
                typePar = typeArgs->Item(0);
            }
            else {
                ASSERT(typeArgs->Item(0)->asPTRSYM()->baseType()->isVOIDSYM());
                ASSERT(!typePar);
            }
            if (typeArgs->size > 1) {
                // Get the the real type args.
                typeArgsActual = compiler()->getBSymmgr().AllocParams(typeArgs->size - 1, typeArgs->ItemPtr(1));
            }
        }

        if (!typePar) {
            NSAIDSYM * nsa = compiler()->getBSymmgr().GetNsAid(bagPar->asNSSYM(), kaidGlobal);
            return compiler()->getBSymmgr().GetErrorType(nsa, name, typeArgsActual);
        }

        ASSERT(bagPar == compiler()->getBSymmgr().GetRootNS());
        return compiler()->getBSymmgr().GetErrorType(typePar, name, typeArgsActual);
    }

    int cvarOuter = bagPar->isAGGSYM() ? bagPar->asAGGSYM()->typeVarsAll->size : 0;
    int cvarFromName;
    NAME * name;
    NAME * nameWithArity;

    ASSERT(!COMPILER::IsRegString(className, L"declbreak"));

    // Strip arity indication from name.
    cvarFromName = StripArityFromName(className, &name, &nameWithArity);
    ASSERT(name && !cvarFromName == !nameWithArity);

    if (typeArgs && cvarFromName > 0 && cvarFromName + cvarOuter != typeArgs->size) {
        // The arity from context (a signature) doesn't match the arity from the name.
        // Use the nameWithArity instead.
        name = nameWithArity;
        nameWithArity = NULL;
        cvarFromName = 0;
    }
    ASSERT(!typeArgs || cvarFromName + cvarOuter == typeArgs->size || cvarFromName == 0);
    ASSERT(name && !cvarFromName == !nameWithArity);

    // First time through the loop check for name.
    // Second time check for nameWithArity.
    NAME * nameMatch = name;
    bool fMdNameHasArity = (cvarFromName > 0);
    int cvarMatch;

    if (cvarFromName > 0)
        cvarMatch = cvarFromName + cvarOuter;
    else if (typeArgs)
        cvarMatch = typeArgs->size;
    else
        cvarMatch = -1;

    int aidAssem = aid;

    if (aid >= kaidMinModule) {
        SYM * sym = compiler()->getBSymmgr().GetSymForAid(aid);
        if (sym->isMODULESYM())
            aidAssem = sym->asMODULESYM()->GetAssemblyID();
        else if (sym->isOUTFILESYM())
            aidAssem = kaidThisAssembly;
        else {
            VSFAIL("Why isn't the sym a module or outfile?");
            return NULL;
        }
    }

    FWDAGGSYM * fwdFail = NULL;

    for (;;) {
        // First look for AGGSYMs.
        for (AGGSYM * aggTmp = compiler()->LookupInBagAid(nameMatch, bagPar, aidAssem, MASK_AGGSYM)->asAGGSYM();
             aggTmp;
             aggTmp = compiler()->LookupNextInAid(aggTmp, aidAssem, MASK_AGGSYM)->asAGGSYM())
        {
            ASSERT(aggTmp->InAlias(aidAssem));

            // Make sure:
            // * The TypeDef token matches (if specified).
            // * The meta-data names match.
            // * The arity is correct.
            if ((aggTmp->tokenImport == token || token == mdTypeDefNil) &&
                !aggTmp->isArityInName == !fMdNameHasArity &&
                (cvarMatch == aggTmp->typeVarsAll->size || cvarMatch < 0))
            {
                if (typeArgs)
                    return compiler()->getBSymmgr().GetInstAgg(aggTmp, typeArgs);
                return aggTmp->getThisType();
            }
        }

        // Now look for forwarders, but only if we're not matching on the TypeDef token.
        if (token == mdTypeDefNil) {
            for (FWDAGGSYM * fwdTmp = compiler()->LookupInBagAid(nameMatch, bagPar, aidAssem, MASK_FWDAGGSYM)->asFWDAGGSYM();
                fwdTmp;
                fwdTmp = compiler()->LookupNextInAid(fwdTmp, aidAssem, MASK_FWDAGGSYM)->asFWDAGGSYM())
            {
                // Check arity. For typeforwarders we only handle correct arity encoding in the name,
                // so if no type args are given and the name has no arity, only match zero arity.
                if (cvarMatch == fwdTmp->cvar || cvarMatch < 0 && fwdTmp->cvar == 0) {
                    AGGSYM * aggTmp = fwdTmp->GetAgg(compiler());
                    if (aggTmp) {
                        ASSERT(!aggTmp->IsUnresolved());
                        if (typeArgs)
                            return compiler()->getBSymmgr().GetInstAgg(aggTmp, typeArgs);
                        return aggTmp->getThisType();
                    }
                    if (!fwdFail)
                        fwdFail = fwdTmp;
                }
            }

            if (fwdFail)
                break;
        }

        // Second time, use nameWithArity
        if (!fMdNameHasArity)
            break;
        fMdNameHasArity = false;
        nameMatch = nameWithArity;

        if (typeArgs)
            cvarMatch = typeArgs->size;
        else
            cvarMatch = -1;
    }

    // Not found. If we're not in unresolved land, just return NULL.
    switch (nro) {
    case NameResOptions::FavorNull:
        return NULL;
    case NameResOptions::FavorUnres:
        break;
    default:
        if ((aid < kaidMinModule || aidAssem != kaidUnresolved) && !fwdFail)
            return NULL;
        break;
    }

    int cvar;

    // Create a symbol (used for errors) for the type.
    if (cvarFromName > 0) {
        ASSERT(!typeArgs || typeArgs->size == cvarFromName + cvarOuter);
        cvar = cvarFromName;
    }
    else if (!typeArgs) {
        // Assume no type variables. This guy is only for error reporting anyway.
        cvar = 0;
    }
    else {
        cvar = typeArgs->size - cvarOuter;
        if (cvar < 0) {
            // If this type has fewer type variables than its outer type,
            // don't bother faking it up.
            return NULL;
        }
    }

    ASSERT(cvar >= 0);

    UNRESAGGSYM * uraTmp = CreateUnresolvedAgg(nameMatch, bagPar, cvar);

    if (fwdFail) {
        uraTmp->moduleErr = fwdFail->GetModuleBreak();
        uraTmp->tokErr = fwdFail->GetAssemRefBreak();
        uraTmp->fSuppressError = fwdFail->FCycle();
    }

    if (typeArgs)
        return compiler()->getBSymmgr().GetInstAgg(uraTmp, typeArgs);
    return uraTmp->getThisType();
}


/***************************************************************************************************
    Create an unresolved AGGSYM for a type which could not be found.
***************************************************************************************************/
UNRESAGGSYM *IMPORTER::CreateUnresolvedAgg(NAME *name, BAGSYM *bagPar, int cvar)
{
    ASSERT(cvar >= 0);

    DECLSYM * declPar;
    TypeArray * typeVarsOuter;

    if (bagPar->isNSSYM()) {
        declPar = GetNSDecl(bagPar->asNSSYM(), compiler()->getBSymmgr().GetInfileForAid(kaidUnresolved));
        typeVarsOuter = BSYMMGR::EmptyTypeArray();
    }
    else {
        declPar = bagPar->asAGGSYM()->DeclOnly();
        typeVarsOuter = bagPar->asAGGSYM()->typeVarsAll;
    }

    AGGSYM * agg = compiler()->getBSymmgr().CreateAgg(name, declPar);

    if (cvar > 0) {
        // Fake up the type variables.
        TYVARSYM ** prgvar = STACK_ALLOC(TYVARSYM *, cvar);
        for (int i = 0; i < cvar; i++) {
            WCHAR sz[20];
            StringCchPrintfW(sz, lengthof(sz), L"T%u", i + typeVarsOuter->size);
            NAME * nameT = compiler()->namemgr->AddString(sz);
            TYVARSYM * var = compiler()->getBSymmgr().CreateTyVar(nameT, agg);
            ASSERT(!var->isMethTyVar);
            var->SetAccess(ACC_PRIVATE);
            var->indexTotal = i + typeVarsOuter->size;
            var->index = i;
            var->parseTree = NULL;
            compiler()->SetBounds(var, BSYMMGR::EmptyTypeArray());
            var->SetAggState(AggState::Last);
            prgvar[i] = var;
        }
        for (int i = 0; i < cvar; i++) {
            VSVERIFY(compiler()->ResolveBounds(prgvar[i], false), "ResolveBounds failed!");
            prgvar[i]->SetAggState(AggState::Last);
        }
        agg->typeVarsThis = compiler()->getBSymmgr().AllocParams(cvar, (TYPESYM **)prgvar);
        agg->typeVarsAll = compiler()->getBSymmgr().ConcatParams(typeVarsOuter, agg->typeVarsThis);
    }
    else {
        agg->typeVarsThis = BSYMMGR::EmptyTypeArray();
        agg->typeVarsAll = typeVarsOuter;
    }

    agg->SetAccess(ACC_PUBLIC);

    compiler()->getBSymmgr().CreateAggDecl(agg, declPar);
    return agg->AsUnresolved();
}


/***************************************************************************************************
    Looks for a type that is already loaded. The aid MUST be a module id. The name should be the
    imported name (not including arity decoration). This is used by ImportOneType to see if the
    type has already been loaded.
***************************************************************************************************/
AGGSYM * IMPORTER::FindAggName(NAME * name, BAGSYM * bagPar, int aid, mdTypeDef tok)
{
    ASSERT(aid >= kaidMinModule);
    ASSERT(RidFromToken(tok) != 0);
    ASSERT(bagPar && (!bagPar->isAGGSYM() || bagPar->asAGGSYM()->InAlias(aid)));

    // Make sure NOT to load any types!
    for (AGGSYM * agg = compiler()->getBSymmgr().LookupGlobalSymCore(name, bagPar, MASK_AGGSYM)->asAGGSYM();
        agg;
        agg = compiler()->getBSymmgr().LookupNextSym(agg, bagPar, MASK_AGGSYM)->asAGGSYM())
    {
        if (agg->tokenImport == tok && agg->InAlias(aid))
            return agg;
    }

    return NULL;
}




/*
 * Resolves a typeref or typedef to just a top-level class name. If it can't be resolved, or resolves to a nested class,
 * then false is returned.
 */
bool IMPORTER::GetTypeRefFullName(MODULESYM *scope, mdToken token, __out_ecount(cchBuffer) PWSTR fullnameText, ULONG cchBuffer)
{
    ASSERT(scope->GetMetaImport(compiler()));
    IMetaDataImport * metaimport = scope->GetMetaImport(compiler());
    mdToken tkResolutionScope = mdTokenNil;
    ULONG cchFullNameText = 0;
    DWORD flags;
    HRESULT hr;

    *fullnameText = 0;

    if (TypeFromToken(token) == mdtTypeRef && token != mdTypeRefNil) {
        // Get the full name of the referenced type.
        hr = metaimport->GetTypeRefProps(
                    token,                                          // typeref token
                    &tkResolutionScope,                             // resolution scope
                    fullnameText, cchBuffer, &cchFullNameText);     // Type name

        fullnameText[min(cchBuffer - 1, cchFullNameText)] = 0;
        if (FAILED(hr) || TypeFromToken(tkResolutionScope) == mdtTypeRef || TypeFromToken(tkResolutionScope) == mdtTypeDef || cchFullNameText > cchBuffer)
            return false; // failure or nested type.
    }
    else if (TypeFromToken(token) == mdtTypeDef && token != mdTypeDefNil) {
        hr = metaimport->GetTypeDefProps(token,
                fullnameText, cchBuffer, &cchFullNameText,      // Type name
                &flags,                                         // Flags
                NULL);                                          // Extends

        fullnameText[min(cchBuffer - 1, cchFullNameText)] = 0;
        if (FAILED(hr) || IsTdNested(flags) || cchFullNameText > cchBuffer)
            return false; // failure or nested type
    } else {
        return false;
    }

    return true;
}


bool IMPORTER::GetTypeRefFullNameWithOuter(IMetaDataImport * pmdi, mdToken tok, StringBldr & str)
{
    mdToken tokOuter;
    WCHAR * prgch;
    ULONG cch;
    ULONG cchT;
    DWORD flags;
    HRESULT hr;

    if (IsNilToken(tok))
        return false;

    switch (TypeFromToken(tok)) {
    default:
        return false;

    case mdtTypeRef:
        // Get the full name of the referenced type.
        hr = pmdi->GetTypeRefProps(tok, &tokOuter, NULL, 0, &cch);
        if (FAILED(hr) || !cch)
            return false;

        if (TypeFromToken(tokOuter) == mdtTypeRef || TypeFromToken(tokOuter) == mdtTypeDef) {
            if (!GetTypeRefFullNameWithOuter(pmdi, tokOuter, str))
                return false;
            str.Add(L'.');
        }

        prgch = STACK_ALLOC(WCHAR, cch + 1);

        hr = pmdi->GetTypeRefProps(tok, &tokOuter, prgch, cch + 1, &cchT);
        if (FAILED(hr) || cchT != cch)
            return false;

        str.Add(prgch, cch - 1);
        return true;

    case mdtTypeDef:
        hr = pmdi->GetTypeDefProps(tok, NULL, 0, &cch, &flags, NULL);
        if (FAILED(hr) || !cch)
            return false;

        if (IsTdNested(flags)) {
            hr = pmdi->GetNestedClassProps(tok, &tokOuter);
            if (FAILED(hr))
                return false;
            if (!GetTypeRefFullNameWithOuter(pmdi, tokOuter, str))
                return false;
            str.Add(L'.');
        }

        prgch = STACK_ALLOC(WCHAR, cch + 1);

        hr = pmdi->GetTypeDefProps(tok, prgch, cch + 1, &cchT, &flags, NULL);
        if (FAILED(hr) || cchT != cch)
            return false;

        str.Add(prgch, cch - 1);
        return true;
    }
}


TYPESYM * IMPORTER::ResolveTypeRefOrSpec(MODULESYM * mod, mdToken token,
    TypeArray * pClassTypeFormals, TypeArray * pMethTypeFormals)
{
    ImportScopeModule scope(this, mod);

    if (TypeFromToken(token) != mdtTypeSpec) {
        // A typeref or typedef so the arity should be zero (if I understand when this called).
        TYPESYM * type = ResolveTypeRefOrDef(scope, token, BSYMMGR::EmptyTypeArray());
        ASSERT(!type || !type->isAGGTYPESYM() || type->asAGGTYPESYM()->typeArgsAll->size == 0);
        return type;
    }

    ASSERT(scope.GetMetaImport());
    IMetaDataImport * metaimport = scope.GetMetaImport();
    PCCOR_SIGNATURE sig;
    ULONG sigsz;
    CheckHR(metaimport->GetTypeSpecFromToken(token, &sig, &sigsz), mod);

    return ImportSigType(scope, &sig, sig + sigsz, kfisoNone, NULL, pClassTypeFormals, pMethTypeFormals);
}


TYPESYM * IMPORTER::ResolveTypeRefOrDef(MODULESYM * mod, mdToken token, TypeArray * typeArgs)
{
    ImportScopeModule scope(this, mod);
    return ResolveTypeRefOrDef(scope, token, typeArgs);
}


TYPESYM * IMPORTER::ResolveTypeRefOrDef(ImportScope & scope, mdToken token, TypeArray * typeArgs)
{
    switch (TypeFromToken(token)) {
    case mdtTypeRef:
        return ResolveTypeRef(scope, token, typeArgs);

    case mdtTypeDef:
        return ResolveTypeDef(scope, token, typeArgs);

    default:
        return NULL;
    }
}


/*
 * Resolves a typeref to an actual class. If the assembly ref is found, but the class is not
 * then a warning is reported and NULL is returned. If the assembly ref is not found then
 * a fake class is created and no error is reported to the user until they use the fake class
 */
TYPESYM * IMPORTER::ResolveTypeRef(ImportScope & scope, mdTypeRef token, TypeArray * typeArgs)
{
    ASSERT(scope.GetMetaImport());
    ASSERT(TypeFromToken(token) == mdtTypeRef);

    if (token == mdTypeRefNil)
        return NULL;

    TYPESYM * type;
    if (GetTypeFromCache(scope, token, typeArgs, &type))
        return type;

    IMetaDataImport * metaimport = scope.GetMetaImport();
    WCHAR rgch[MAX_FULLNAME_SIZE];
    ULONG cch;

    mdToken tkPar;

    // Get the full name of the referenced type.
    CheckHR(metaimport->GetTypeRefProps(token, &tkPar, rgch, lengthof(rgch), &cch), scope);
    CheckTruncation(cch, lengthof(rgch), scope);

    PCWSTR prgch; prgch = rgch;
    SYM * symPar; symPar = ResolveParentScope(scope, tkPar, &prgch);

    if (!symPar)
        goto LDone;

    ASSERT(symPar->isTYPESYM() || symPar->isNSAIDSYM());
    int aid;

    // Resolve the type name.
    if (symPar->isAGGTYPESYM()) {
        aid = symPar->asAGGTYPESYM()->getAggregate()->GetModuleID();
        type = ResolveTypeName(prgch, symPar->asAGGTYPESYM()->getAggregate(), aid, typeArgs, NameResOptions::Normal);
    }
    else if (symPar->isNSAIDSYM()) {
        aid = symPar->asNSAIDSYM()->GetAid();

        type = ResolveTypeName(prgch, symPar->asNSAIDSYM()->GetNS(), aid, typeArgs,
            NameResOptions::Normal
            );
    }
    else {
        VSFAIL("Why are we here?");
        goto LDone;
    }

    ASSERT(aid != kaidUnresolved);

    if (type && type->isAGGTYPESYM() && type->getAggregate()->IsUnresolved()) {
        // We have a typeref that couldn't be resolved to a class. Remember the typeref token so that
        // we can give a good error message if we ever attempt to use this type.
        // ResolveTypeName just fabricated the type for us.

        // If the scope is the output file for ENC, then we shouldn't end up with an unresolved type,
        // otherwise the set of imports has changed and we shouldn't be doing an ENC!
        ASSERT(scope.GetModule());

        UNRESAGGSYM * ura = type->getAggregate()->AsUnresolved();

        if (aid < kaidMinModule) {
            MODULESYM * module = GetUnresolvedModule(scope, tkPar);
            aid = module->GetModuleID();
        }

        if (!ura->moduleRef) {
            ura->module = compiler()->getBSymmgr().GetSymForAid(aid)->asMODULESYM();
            ura->moduleRef = scope.GetModule();
            ura->tokRef = token;
            if (!ura->moduleErr) {
                ura->moduleErr = ura->moduleRef;
                ura->tokErr = ura->tokRef;
            }
        }
    }

    if (!type) {
        // The assembly reference (or outer type) was succesfully resolved
        // But this type couldn't be found.  Report a warning to the user
        // indicating possibly corrupt metadata
        if (symPar->isAGGTYPESYM()) {
            compiler()->Error(scope, WRN_MissingTypeNested, rgch, symPar);
        }
        else if (aid == kaidThisAssembly) {
            compiler()->Error(scope, WRN_MissingTypeInSource, rgch);
        }
        else {
            INFILESYM * infile = compiler()->getBSymmgr().GetInfileForAid(aid);
            ASSERT(infile->GetAssemblyID() != kaidUnresolved);
            compiler()->Error(scope, WRN_MissingTypeInAssembly, rgch, infile);
        }
    }

LDone:
    if (type && type->isAGGTYPESYM())
        SetSymInCache(scope, token, type->asAGGTYPESYM()->getAggregate());
    else
        SetSymInCache(scope, token, type);

    return type;
}


/***************************************************************************************************
    This checks the cache for a type with the given token. If the type is not found in the cache,
    it attempts to load the type. If the newly loaded type has any nested types or is itself nested,
    we force loading all the types in the same namespace and module.
***************************************************************************************************/
TYPESYM * IMPORTER::ResolveTypeDef(ImportScope & scope, mdTypeDef token, TypeArray * typeArgs)
{
    ASSERT(!fLoadingTypes);
    ASSERT(scope.GetMetaImport());
    ASSERT(TypeFromToken(token) == mdtTypeDef);

    if (token == mdTypeDefNil)
        return NULL;

    TYPESYM * type;
    if (GetTypeFromCache(scope, token, typeArgs, &type))
        return type; // NOTE: type may be NULL

    // The module will be NULL if the ImportScope is an Enc scope, in which case
    // the ImportOneType isn't necessary.
    MODULESYM * mod = scope.GetModule();
    if (!mod)
        return NULL;

    if (fLoadingTypes) {
        VSFAIL("Shouldn't be recursing in loading types!");
        return NULL;
    }

    // First load this AGGSYM (and its outer types).
    fLoadingTypes = true;
    AGGSYM * agg = ImportOneType(mod, token);

    if (!agg) {
        ASSERT(fLoadingTypes);
        fLoadingTypes = false;
        return NULL;
    }

    // Now get the outermost agg.
    while (agg->isNested())
        agg = agg->GetOuterAgg();

    // The token of agg is the root token of any nested types.
    // See if we have any to import.

    mdToken tokRoot = agg->tokenImport;
    ASSERT(RidFromToken(tokRoot) != 0);

    int imtiMin = 0;
    int imtiLim = mod->cmti;

    while (imtiMin < imtiLim) {
        int imtiMid = (imtiMin + imtiLim) / 2;
        if (mod->prgmti[imtiMid].tokRoot < tokRoot)
            imtiMin = imtiMid + 1;
        else
            imtiLim = imtiMid;
    }
    ASSERT(imtiMin == imtiLim);

    for ( ; imtiLim < mod->cmti && mod->prgmti[imtiLim].tokRoot == tokRoot; imtiLim++) {
        ImportOneType(mod, mod->prgmti[imtiLim].tok);
    }

    memmove(mod->prgmti + imtiMin, mod->prgmti + imtiLim, (mod->cmti - imtiLim) * sizeof(ModTypeInfo));
    mod->cmti -= (imtiLim - imtiMin);

    ASSERT(fLoadingTypes);
    fLoadingTypes = false;

    // Now we should be able to fetch from the cache.
    if (GetTypeFromCache(scope, token, typeArgs, &type))
        return type; // NOTE: type may be NULL
    VSFAIL("Why isn't the result in the cache now?");
    return NULL;
}


/*
 * Resolve a base class or interface name. Similar to ResolveTypeRefOrSpec,
 * but reports error on failure, and forces the base class or interface to
 * be declared.
 *
 * fRequired indicates whether it is an error if the type can't be resolved.
 * This is typically false iff symDerived is a class/struct and tokenBase came
 * from the interface list.
 */
PAGGTYPESYM IMPORTER::ResolveBaseRef(MODULESYM *scope, mdToken tokenBase, PAGGSYM symDerived, bool fRequired)
{
    ASSERT(!IsNilToken(tokenBase));
    ASSERT(scope->GetMetaImport(compiler()));

    PTYPESYM symBase;

    symBase = ResolveTypeRefOrSpec(scope, tokenBase, symDerived->typeVarsAll);

    if (!symBase || !symBase->isAGGTYPESYM()) {
        if (!fRequired)
            return NULL;

        if (TypeFromToken(tokenBase) == mdtTypeSpec) {
            compiler()->Error(ERRLOC(scope->getInputFile()), ERR_ImportBadBase, symDerived);
        }
        else {
            ASSERT(!symBase); // If symBase is found it should be an AGGTYPESYM!

            // Couldn't resolve base class. Give a good error message.
            WCHAR rgchType[MAX_FULLNAME_SIZE * 2];
            StringBldrFixed str(rgchType, lengthof(rgchType));
            WCHAR szAssem[MAX_FULLNAME_SIZE];

            szAssem[0] = 0;

            GetTypeRefFullNameWithOuter(scope->GetMetaImport(compiler()), tokenBase, str);

            GetTypeRefAssemblyName(scope, tokenBase, szAssem, lengthof(szAssem));
            szAssem[lengthof(szAssem) - 1] = 0;

            compiler()->Error(ERRLOC(scope->getInputFile()), ERR_CantImportBase, symDerived, str.Str(), szAssem);
        }

        symBase = NULL;
    }

    return symBase->asAGGTYPESYM();
}

/*
 * Given a typeref, get the name of the assembly (or module) that the typeref refers to. 
 * Used for better error reporting.
 */
void IMPORTER::GetTypeRefAssemblyName(MODULESYM *mod, mdToken token, __out_ecount(cchAssemblyName) PWSTR assemblyName, ULONG cchAssemblyName)
{
    IMetaDataImport * metaimport = mod->GetMetaImport(compiler());

    *assemblyName = 0;

    if (TypeFromToken(token) == mdtTypeRef && token != mdTypeRefNil) {

        mdToken oldToken;
        do {
            oldToken = token;
            
            // From the type ref, get it's resolution scope.
            metaimport->GetTypeRefProps(
                        token,                                                  // typeref token                    
                        &token,                                                 // resolution scope
                        NULL, 0, NULL);                                         // name

            // repeat until we get out of the typeref chain of resolution scopes.
        } while (token != oldToken && TypeFromToken(token) == mdtTypeRef && token != mdTypeRefNil);
    } else if (TypeFromToken(token) == mdtTypeDef && token != mdTypeDefNil) {
        PCWSTR sz = GetAssemblyName(mod->getInputFile());
        if (sz) {
            StringCchCopyW(assemblyName, cchAssemblyName, sz);
        }
        return;
    }
    
    // Need to deal with inputfile->assemimport being NULL which means that
    // inputfile was /addModuled not /referenced
    ULONG cchActualName = 0;
    if (TypeFromToken(token) == mdtAssemblyRef && token != mdAssemblyRefNil) {
        ImportScopeModule scope(this, mod);
        NAME * name = GetAssemblyName(scope, token);
        if (name) {
            StringCchCopyW(assemblyName, cchAssemblyName, name->text);
        }
        return;
    }
    else if (TypeFromToken(token) == mdtModuleRef && token != mdModuleRefNil) {
        metaimport->GetModuleRefProps(
            token,
            assemblyName, cchAssemblyName, &cchActualName);
    }
    else {
        ASSERT(0); // Bad resolution scope.
    }

    // null terminate
    assemblyName[min(cchActualName, cchAssemblyName - 1)] = 0;
}


/*
 * Imports an interface declared on a class or interface.
 */
PAGGTYPESYM IMPORTER::ImportInterface(MODULESYM *scope, mdInterfaceImpl tokenIntf, PAGGSYM symDerived)
{
    ASSERT(scope->GetMetaImport(compiler()));

    if (TypeFromToken(tokenIntf) != mdtInterfaceImpl || tokenIntf == mdInterfaceImplNil)
        return NULL;

    IMetaDataImport * metaimport = scope->GetMetaImport(compiler());
    mdToken tokenInterface;

    // Get typeref and flags.
    CheckHR(metaimport->GetInterfaceImplProps(tokenIntf, NULL, &tokenInterface), scope);

    if (IsNilToken(tokenInterface))
        return NULL;

    return ResolveBaseRef(scope, tokenInterface, symDerived, symDerived->IsInterface());
}

/*
 *Given type flags, get the access level. The sym is passed in just to make a determination on
 * what assembly it is in; it is not changed.
 */
ACCESS IMPORTER::AccessFromTypeFlags(uint flags, INFILESYM * infile)
{
    switch (flags & tdVisibilityMask)
    {
    case tdNotPublic:
        return ACC_INTERNAL;
    case tdPublic:
        return ACC_PUBLIC;
    case tdNestedPublic:
        return ACC_PUBLIC;
    case tdNestedPrivate:
        return ACC_PRIVATE;
    case tdNestedFamily:
        return ACC_PROTECTED;
    case tdNestedAssembly:
        return ACC_INTERNAL;
    case tdNestedFamORAssem:
        return ACC_INTERNALPROTECTED;
    case tdNestedFamANDAssem:
        // We don't support this directly. Treat as protected if in this assembly or in one that
        // gave us friend right; internal otherwise.
        if (infile->GetAssemblyID() == kaidThisAssembly || infile->InternalsVisibleTo(kaidThisAssembly))
            return ACC_PROTECTED;
        return ACC_INTERNAL;
    default:
        ASSERT(0);
        return ACC_PRIVATE;
    }
}

/*
 * From a flags field, return the access level
 */
ACCESS IMPORTER::ConvertAccessLevel(uint flags, INFILESYM * infile, bool fDontHide)
{
    ASSERT((int)fdPublic          == (int)mdPublic);
    ASSERT((int)fdPrivate         == (int)mdPrivate);
    ASSERT((int)fdFamily          == (int)mdFamily);
    ASSERT((int)fdAssembly        == (int)mdAssem);
    ASSERT((int)fdFamANDAssem     == (int)mdFamANDAssem);
    ASSERT((int)fdFamORAssem      == (int)mdFamORAssem);
    ASSERT((int)fdFieldAccessMask == (int)mdMemberAccessMask);


    flags &= mdMemberAccessMask;

    switch (flags) {
    case fdPrivate:
        return ACC_PRIVATE;
    case fdFamily:
        return ACC_PROTECTED;
    case fdPublic:
        return ACC_PUBLIC;
    case fdAssembly:
        // Treat as internal if we may have access to it. Otherwise treat it as private.
        // This is so we will never allow access to it - even if it's parent is the base
        // of an accessible type.
        if (fDontHide || infile->GetAssemblyID() == kaidThisAssembly || infile->InternalsVisibleTo(kaidThisAssembly))
            return ACC_INTERNAL;
        return ACC_PRIVATE;
    case fdFamANDAssem:
        // We don't support this directly.
        // Treat as protected if we may have access to it. Otherwise treat it as private.
        // This is so we will never allow access to it - even if it's parent is the base
        // of an accessible type.
        if (infile->GetAssemblyID() == kaidThisAssembly || infile->InternalsVisibleTo(kaidThisAssembly))
            return ACC_PROTECTED;
        if (fDontHide)
            return ACC_INTERNAL;
        return ACC_PRIVATE;
    case fdFamORAssem:
        // Treat as internal protected if it's in this assembly or in one that gave us
        // friend rights. Otherwise treat it as protected since we can't see the internal
        // part and want overrides to be protected.
        if (infile->GetAssemblyID() == kaidThisAssembly || infile->InternalsVisibleTo(kaidThisAssembly)
            )
            return ACC_INTERNALPROTECTED;
        return ACC_PROTECTED;
    default:
        return ACC_PRIVATE;
    }
}

/*
 * Uncompresses a ULONG from a signature stream. Checks for buffer overrun.
 */
inline ULONG IMPORTER::SigUncompressData(ImportScope & scope, PCCOR_SIGNATURE * sigPtr, PCCOR_SIGNATURE sigPtrEnd)
{
    if (*sigPtr >= sigPtrEnd || *sigPtr + CorSigUncompressedDataSize(*sigPtr) > sigPtrEnd)
        BogusMetadataFailure(scope);

    return CorSigUncompressData(*sigPtr);
}

/*
 * Uncompresses a token from a signature stream. Checks for buffer overrun.
 */
inline mdToken IMPORTER::SigUncompressToken(ImportScope & scope, PCCOR_SIGNATURE * sigPtr, PCCOR_SIGNATURE sigPtrEnd)
{
    if (*sigPtr >= sigPtrEnd || *sigPtr + CorSigUncompressedDataSize(*sigPtr) > sigPtrEnd)
        BogusMetadataFailure(scope);

    return CorSigUncompressToken(*sigPtr);
}

/*
 * Uncompresses a ULONG from a signature stream. Does not advance ptr. Checks for buffer overrun.
 */
inline ULONG IMPORTER::SigPeekUncompressData(MODULESYM * mod, PCCOR_SIGNATURE sigPtr, PCCOR_SIGNATURE sigPtrEnd)
{
    ImportScopeModule scope(this, mod);
    return SigUncompressData(scope, &sigPtr, sigPtrEnd);
}

/*
 * Get a BYTE froma signature stream. CHecks for buffer overrun.
 */
inline BYTE IMPORTER::SigGetByte(ImportScope & scope, PCCOR_SIGNATURE * sigPtr, PCCOR_SIGNATURE sigPtrEnd)
{
    BYTE returnValue = SigPeekByte(scope, *sigPtr, sigPtrEnd);
    *sigPtr += 1;

    return returnValue;
}

/*
 * Get a BYTE froma signature stream. CHecks for buffer overrun.
 */
inline BYTE IMPORTER::SigPeekByte(ImportScope & scope, PCCOR_SIGNATURE sigPtr, PCCOR_SIGNATURE sigPtrEnd)
{
    if (sigPtr + 1 > sigPtrEnd)
        BogusMetadataFailure(scope);

    return *sigPtr;
}

inline const void *IMPORTER::CheckBufferAccess(MODULESYM * mod, const void *buffer, size_t cbRequired, size_t cbActual)
{
    if (cbRequired > cbActual)
        BogusMetadataFailure(mod->getInputFile()->name->text);

    return buffer;
}

/*
 * Import a single signature definition type. Return NULL if we don't
 * have a corresponding type. sigPtr is updated to point just beyond
 * the end of the type.
 *
 * Note that the type returned is not necessarily declared, which
 * is what we want.
 *
 * Signatures don't distinguish between ref and out parameters,
 * so we just return the base type and return that some byref is present
 * via the returned isByref flags.
 *
 * In the case of generics this is complicated by the fact that type variables
 * (which become TYVARSYMs) must "point" directly to the TYVARSYM corresponding to
 * the declaration of the type variable.  For example, a generic class C<T> has one child
 * TYVARSYM for the declaration of T.  All uses of T inside that class must point directly
 * to (i.e. actually are) that TYVARSYM.  Similarly for methods.  As such, we pass
 * arrays corresponding to the type variables that are in scope, similar to the arrays
 * passed into SubstType.
 *
 */
PTYPESYM IMPORTER::ImportSigType(ImportScope & scope, PCCOR_SIGNATURE * sigPtr, PCCOR_SIGNATURE sigPtrEnd,
    int grfiso, int *pmodOptCount, TypeArray * typeVarsCls, TypeArray * typeVarsMeth, bool convertPinned)
{
    PCCOR_SIGNATURE sig = *sigPtr;
    TYPESYM * type;
    mdTypeRef token;
    int grfisoRec = grfiso & kfisoIncludeModOpts;

    if (sig >= sigPtrEnd) {
        BogusMetadataFailure(scope);
    }

    switch (*sig++) {
    case ELEMENT_TYPE_END:
        // Bogus.
        type = NULL;
        break;

    case ELEMENT_TYPE_VOID:
        if (grfiso & kfisoAllowVoid)
            type = compiler()->getBSymmgr().GetVoid();
        else
            type = NULL;
        break;

    case ELEMENT_TYPE_BOOLEAN:
        type = compiler()->GetReqPredefType(PT_BOOL, false);
        break;

    case ELEMENT_TYPE_CHAR:
        type = compiler()->GetReqPredefType(PT_CHAR, false);
        break;

    case ELEMENT_TYPE_U1:
        type = compiler()->GetReqPredefType(PT_BYTE, false);
        break;

    case ELEMENT_TYPE_I2:
        type = compiler()->GetReqPredefType(PT_SHORT, false);
        break;

    case ELEMENT_TYPE_I4:
        type = compiler()->GetReqPredefType(PT_INT, false);
        break;

    case ELEMENT_TYPE_I8:
        type = compiler()->GetReqPredefType(PT_LONG, false);
        break;

    case ELEMENT_TYPE_R4:
        type = compiler()->GetReqPredefType(PT_FLOAT, false);
        break;

    case ELEMENT_TYPE_R8:
        type = compiler()->GetReqPredefType(PT_DOUBLE, false);
        break;

    case ELEMENT_TYPE_STRING:
        type = compiler()->GetReqPredefType(PT_STRING, false);
        break;

    case ELEMENT_TYPE_OBJECT:
        type = compiler()->GetReqPredefType(PT_OBJECT, false);
        break;

    case ELEMENT_TYPE_I1:      
        type = compiler()->GetReqPredefType(PT_SBYTE, false);
        break;

    case ELEMENT_TYPE_U2:    
        type = compiler()->GetReqPredefType(PT_USHORT, false);
        break;

    case ELEMENT_TYPE_U4:
        type = compiler()->GetReqPredefType(PT_UINT, false);
        break;

    case ELEMENT_TYPE_U8:
        type = compiler()->GetReqPredefType(PT_ULONG, false);
        break;

    case ELEMENT_TYPE_I:
        type = compiler()->GetReqPredefType(PT_INTPTR, false);
        break;

    case ELEMENT_TYPE_U:
        type = compiler()->GetReqPredefType(PT_UINTPTR, false);
        break;

    case ELEMENT_TYPE_TYPEDBYREF:
        type = compiler()->GetOptPredefType(PT_REFANY, false);
        break;

    case ELEMENT_TYPE_VALUETYPE:
        token = SigUncompressToken(scope, &sig, sigPtrEnd);  // updates sig.
        type = ResolveTypeRefOrDef(scope, token, NULL);
        ASSERT(!type || !type->isAGGTYPESYM() || type->asAGGTYPESYM()->IsInstType());

        // Arity should be zero.
        if (type && type->isAGGTYPESYM() && type->asAGGTYPESYM()->typeArgsAll->size) {
            // Bogus signature. It should have specified type args.
            type = NULL;
        }
        break;

    case ELEMENT_TYPE_CLASS:
        // Element of class or struct type.
        token = SigUncompressToken(scope, &sig, sigPtrEnd);  // updates sig.
        type = ResolveTypeRefOrDef(scope, token, NULL);
        ASSERT(!type || !type->isAGGTYPESYM() || type->asAGGTYPESYM()->IsInstType());

        // ELEMENT_TYPE_CLASS followed by value type means the "boxed" version, which 
        // we don't support. Check for this case and return NULL.
        // Arity should be zero.
        if (type && type->isAGGTYPESYM() && (type->isStructOrEnum() || type->asAGGTYPESYM()->typeArgsAll->size)) {
            type = NULL;
        }
        break;

    case ELEMENT_TYPE_SZARRAY:
        // Single-dimensional array with 0 lower bound
        type = ImportSigType(scope, &sig, sigPtrEnd, grfisoRec, pmodOptCount, typeVarsCls, typeVarsMeth);  // Get element type.
        if (type) {
            type = compiler()->getBSymmgr().GetArray(type, 1);
        }
        break;

    case ELEMENT_TYPE_VAR: {
        int index = CorSigUncompressData(sig);
        if (index < TypeArray::Size(typeVarsCls) && 0 <= index)
            type = typeVarsCls->Item(index);
        else
            type = NULL;
        break;
    }
    case ELEMENT_TYPE_MVAR: {
        int index = CorSigUncompressData(sig);
        if (index < 0)
            type = NULL;
        else if (!typeVarsMeth)
            type = compiler()->getBSymmgr().GetStdMethTypeVar(index);
        else if (index < typeVarsMeth->size)
            type = typeVarsMeth->Item(index);
        else
            type = NULL;
        break;
    }
    case ELEMENT_TYPE_GENERICINST: {
        // Instantiated generic type
        byte b = (*sig++);
        token = SigUncompressToken(scope, &sig, sigPtrEnd);  // updates sig.
        int ctype = CorSigUncompressData(sig);
        TYPESYM ** prgtype = STACK_ALLOC(TYPESYM *, ctype);
        bool fErrors = false;
        for (int i = 0; i < ctype; i++) {
            prgtype[i] = ImportSigType(scope, &sig, sigPtrEnd, grfisoRec, pmodOptCount, typeVarsCls, typeVarsMeth);  // Get element type.
            if (!prgtype[i])
                fErrors = true;
        }
        if (fErrors) {
            type = NULL;
            break;
        }
        TypeArray * typeArgs = compiler()->getBSymmgr().AllocParams(ctype, prgtype);
        type = ResolveTypeRefOrDef(scope, token, typeArgs);

        // Translate "Nullable<T>" to "T?"
        if (ctype == 1 && type->isPredefType(PT_G_OPTIONAL))
            type = compiler()->getBSymmgr().GetNubFromNullable(type->asAGGTYPESYM());

        // ELEMENT_TYPE_CLASS followed by value type means the "boxed" version, which 
        // we don't support. Check for this case and return NULL.
        if (b == ELEMENT_TYPE_CLASS && type && type->isStructOrEnum())
            type = NULL;
        break;
    }

    case ELEMENT_TYPE_ARRAY:
        // Multi-dimensional array. We only support arrays
        // with unspecified length and zero lower bound.

        int rank, lowBound, numRanks;

        type = ImportSigType(scope, &sig, sigPtrEnd, grfisoRec, pmodOptCount, typeVarsCls, typeVarsMeth);  // Get element type.

        rank = SigUncompressData(scope, &sig, sigPtrEnd);  // Get rank.
        if (rank == 0) {
            type = NULL;
            break;
        }

        // Get sizes of each rank.
        numRanks = SigUncompressData(scope, &sig, sigPtrEnd);
        if (numRanks > 0) {
            // We don't support sizing of arrays.
            type = NULL;
            for (int i = 0; i < numRanks; ++i)
                SigUncompressData(scope, &sig, sigPtrEnd); // skip the sizes.
        }

        // Get lower bounds of each rank.
        numRanks = SigGetByte(scope, &sig, sigPtrEnd);
        while (numRanks--) {
            lowBound = SigUncompressData(scope, &sig, sigPtrEnd);
            if (lowBound != 0)
                type = NULL;     // We don't support non-zero lower bounds.
        }

        // Get the array symbol, if its an array type that we support.
        if (type)
            type = compiler()->getBSymmgr().GetArray(type, rank);
        break;

    case ELEMENT_TYPE_PTR:
        // Pointer type. Note that void * is a valid type here.
        type = ImportSigType(scope, &sig, sigPtrEnd, kfisoAllowVoid | grfisoRec, pmodOptCount, typeVarsCls, typeVarsMeth);
        if (type)
            type = compiler()->getBSymmgr().GetPtrType(type);
        break;

    case ELEMENT_TYPE_BYREF:
        // Byref param - could be ref or out, so just return indication of that.
        type = ImportSigType(scope, &sig, sigPtrEnd, grfisoRec, pmodOptCount, typeVarsCls, typeVarsMeth);
        if ((grfiso & kfisoAllowByref) && type)
            type = compiler()->getBSymmgr().GetParamModifier(type, false);
        else
            type = NULL;     // byref isn't allowed here.
        break;

    case ELEMENT_TYPE_CMOD_OPT:
        token = SigUncompressToken(scope, &sig, sigPtrEnd);  // Ignore the following optional token

        // get the 'real' type here
        type = ImportSigType(scope, &sig, sigPtrEnd, grfiso, pmodOptCount, typeVarsCls, typeVarsMeth);
        if (pmodOptCount) {
            *pmodOptCount += 1;
        }
        if ((grfiso & kfisoIncludeModOpts) && type) {
            type = scope.GetModule() ? compiler()->getBSymmgr().GetModOptType(type, token, scope.GetModule()) : NULL;
        }
        break;

    case ELEMENT_TYPE_CMOD_REQD:
        token = SigUncompressToken(scope, &sig, sigPtrEnd);  // Ignore the following optional token

        // get the 'real' type here
        ImportSigType(scope, &sig, sigPtrEnd, grfiso, pmodOptCount, typeVarsCls, typeVarsMeth);

        // We are required to understand this, since we don't, just return NULL.
        type = NULL;
        break;

    case ELEMENT_TYPE_PINNED:
        type = ImportSigType(scope, &sig, sigPtrEnd, kfisoAllowVoid | kfisoAllowByref | grfisoRec, pmodOptCount, typeVarsCls, typeVarsMeth);
        if (type && type->isPARAMMODSYM()) {
            type = compiler()->getBSymmgr().GetPtrType(type->asPARAMMODSYM()->paramType());
        }
        if (type) {
            if (!convertPinned) {
                type = compiler()->getBSymmgr().GetPinnedType(type);
            }
            else if(!type->isPTRSYM()) {
                type = NULL;
            }
        }
        break;

    default:
        // Something we don't know about.
        if (CorIsModifierElementType((CorElementType) *(sig - 1))) {
            // Consume what is modified.
            ImportSigType(scope, &sig, sigPtrEnd, kfisoAllowVoid | kfisoAllowByref | grfisoRec, pmodOptCount, typeVarsCls, typeVarsMeth);
        }
        type = NULL;
        break;
    }

    // Update the signature pointer of the called.
    *sigPtr = sig;
    return type;
}


/*
 * Import a field type from a signature. Return NULL is type is not supported.
 */
TYPESYM * IMPORTER::ImportFieldType(MODULESYM * mod, PCCOR_SIGNATURE sig, PCCOR_SIGNATURE sigEnd,
    bool * isVolatile, TypeArray * typeVarsCls)
{
    // Format of field signature is IMAGE_CEE_CS_CALLCONV_FIELD, followed by one type.
    ImportScopeModule scope(this, mod);

    if (SigGetByte(scope, &sig, sigEnd) != IMAGE_CEE_CS_CALLCONV_FIELD)
        return NULL;        // Bogus!
    *isVolatile = false;
    BYTE b = SigPeekByte(scope, sig, sigEnd);
    if (b == ELEMENT_TYPE_CMOD_REQD || b == ELEMENT_TYPE_CMOD_OPT) {
        bool isRequired = (b == ELEMENT_TYPE_CMOD_REQD);

        ++sig;
        mdToken token = SigUncompressToken(scope, &sig, sigEnd); // updates sig
        TYPESYM * typeMod = ResolveTypeRefOrDef(mod, token, NULL);

        if (typeMod && typeMod->isPredefType(PT_VOLATILEMOD)) {
            *isVolatile = true;
        }
        else if (isRequired) {
            return NULL;  // required modifier that we don't understand, can't import type at all.
        }
    }

    return ImportSigType(scope, &sig, sigEnd, kfisoNone, NULL, typeVarsCls);
}

/*
 * Import a constant value. Return false if the value can't be imported.
 *
 * constVal is the location to put the constant value, and valType is the type that the
 * constant value must be.
 *
 * constType and constValue, constLen are the COM+ metadata type and value and length (in characters).
 */
bool IMPORTER::ImportConstant(CONSTVAL * constVal, ULONG constLen, PTYPESYM valType, DWORD constType, const void * constValue)
{
    __int64 intValue;
    double floatValue;

    // Read the COM+ value into either intValue or floatValue.
    switch (constType)
    {
    case ELEMENT_TYPE_BOOLEAN:
    case ELEMENT_TYPE_I1:
        intValue = *(__int8 *)constValue; goto INTVALUE;
    case ELEMENT_TYPE_U1:
        intValue = *(unsigned __int8 *)constValue; goto INTVALUE;
    case ELEMENT_TYPE_CHAR:
    case ELEMENT_TYPE_U2:
        intValue = GET_UNALIGNED_VAL16(constValue); goto INTVALUE;
    case ELEMENT_TYPE_U4:
        intValue = GET_UNALIGNED_VAL32(constValue); goto INTVALUE;
    case ELEMENT_TYPE_U8:
        intValue = GET_UNALIGNED_VAL64(constValue); goto INTVALUE;
    case ELEMENT_TYPE_I2:
        intValue = (INT16)GET_UNALIGNED_VAL16(constValue); goto INTVALUE;
    case ELEMENT_TYPE_I4:
        intValue = (INT32)GET_UNALIGNED_VAL32(constValue); goto INTVALUE;
    case ELEMENT_TYPE_I8:
        intValue = (INT64)GET_UNALIGNED_VAL64(constValue); goto INTVALUE;
    case ELEMENT_TYPE_R4:
        {
            __int32 Value = GET_UNALIGNED_VAL32(constValue);
            floatValue = (float &)Value;
            goto FLOATVALUE;
        }
    case ELEMENT_TYPE_R8:
        {
            __int64 Value = GET_UNALIGNED_VAL64(constValue);
            floatValue = (double &) Value;
            goto FLOATVALUE;
        }
    case ELEMENT_TYPE_STRING:
    {
        // String constant.

        STRCONST * strConst;

        // Must have a string type to put it in.
        if (! valType->isPredefType(PT_STRING))
            return false;

        // Allocate memory for the string constant.
        strConst = (STRCONST *) compiler()->getGlobalSymAlloc().Alloc(sizeof(STRCONST));

        // Read the string constant. Currently this is zero terminated, and may be NULL for empty string.
        // The "null" constant is persisted as ELEMENT_TYPE_CLASS.
        if (constValue == NULL) {
            strConst->length = 0;
            strConst->text = NULL;
        }
        else {
            // looks like a cch vs. cb mixup here, But NO
            // constLen is set to the cch for string values only
            int cch = (int)constLen;
            strConst->length = cch;
            strConst->text = (WCHAR *) compiler()->getGlobalSymAlloc().Alloc(cch * sizeof(WCHAR));
            memcpy(strConst->text, constValue, cch * sizeof(WCHAR));
            SwapStringLength(strConst->text, cch);
        }

        constVal->strVal = strConst;
        return true;
    }

    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_OBJECT:
        // Only reasonable value is null.
        if (constValue != NULL && 
            constLen != 0 &&
            (constLen != 4 || GET_UNALIGNED_VAL32(constValue)))
            return false;

        if (valType->isPredefType(PT_STRING)) {
            // Strings must use STRCONST structure to hold NULL.
            STRCONST * strConst;

            // Allocate memory for the string constant.
            strConst = (STRCONST *) compiler()->getGlobalSymAlloc().Alloc(sizeof(STRCONST));
            strConst->length = 0;
            strConst->text = NULL;
            constVal->strVal = strConst;
        }

        constVal->init = 0;
        return true;

    default:
        return false;
    }

    // we never reach here.

INTVALUE:
    // An integer value was read. Coerce it to the type of integer
    // value that we were expecting.

    // NOTE: fundType is known after resolving base classes (and asserts that fact)
    VSVERIFY(compiler()->clsDeclRec.ResolveInheritanceRec(valType->getAggregate()), "ResolveInheritanceRec failed in ImportConstant!");
    switch (valType->fundType()) {
    case FT_I1:
        if (valType->isPredefType(PT_BOOL)) {
            constVal->iVal = ((intValue != 0) ? true : false);
            break;
        }
        // else FALL THROUGH to usual int case.
    case FT_U1:
    case FT_I2:
    case FT_U2:
    case FT_I4:
    case FT_U4:
        constVal->iVal = (int) intValue; break;

    case FT_I8:
    case FT_U8:
        // 64-bit constant must be allocated.
        constVal->longVal = (__int64 *) compiler()->getGlobalSymAlloc().Alloc(sizeof(__int64));
        *constVal->longVal = intValue;
        break;

    default:
        // Not a valid type to hold an integer.
        return false;
    }

    return true;

FLOATVALUE:
    // A floating point value was read. Coerce it to the type of float
    // that we were expecting.

    switch (valType->fundType()) {
    case FT_R4:
    case FT_R8:
        // 64-bit constant must be allocated.
        constVal->doubleVal = (double *) compiler()->getGlobalSymAlloc().Alloc(sizeof(double));
        *constVal->doubleVal = floatValue;
        break;

    default:
        // Not a valid type to hold a float.
        return false;
    }

    return true;
}

/*
 * Import a fielddef and create a corresponding MEMBVARSYM symbol. If we can't import
 * the field because it has attributes that we don't support and can't safely ignore,
 * we set the "isBogus" flag on the field. If we ever try to use that field, we'll give
 * an error (but if we don't use the field, the user never knows or cares).
 */
void IMPORTER::ImportField(AGGSYM * parent, AGGDECLSYM * decl, mdFieldDef tokenField)
{
    ASSERT(!parent->isMDPrivate || parent->IsStruct() || parent->IsEnum());

    MODULESYM * scope = parent->GetModule();
    ASSERT(parent->GetMetaImport(compiler()));
    IMetaDataImport * metaimport = parent->GetMetaImport(compiler());
    WCHAR fieldnameText[MAX_IDENT_SIZE];     // name of field
    ULONG cchFieldnameText;
    DWORD flags;
    PCCOR_SIGNATURE signature;
    ULONG cbSignature;
    DWORD constType;
    LPCVOID constValue;
    ULONG constLen;
    PNAME name;
    MEMBVARSYM * sym;
    PTYPESYM type;
    bool isVolatile = false;

    if (TypeFromToken(tokenField) != mdtFieldDef || tokenField == mdFieldDefNil) {
        return;
    }

    // Get properties of the field from metadata.
    CheckHR(metaimport->GetFieldProps(
        tokenField,                                             // The field for which to get props.
        NULL,                                                   // Put field's class here.
        fieldnameText, lengthof(fieldnameText), & cchFieldnameText, // Field name
        & flags,                                            // Field flags
        & signature, & cbSignature,                                 // Field signature
        & constType, & constValue, &constLen),                      // Field constant value
                scope);

    ASSERT(!COMPILER::IsRegString(fieldnameText, L"ImportField"));

    CheckTruncation(cchFieldnameText, lengthof(fieldnameText), scope);
    if (cchFieldnameText <= 1)
        return;

    // Check the access.
    ACCESS access = ConvertAccessLevel(flags, scope->getInputFile(), false);

    if (parent->isMDPrivate) {
        // We treat anything in a private type as private.
        access = ACC_PRIVATE;
    }

    // ConvertAccessLevel nukes things we can't see by returning ACC_PRIVATE.
    bool fInaccess = (access < ACC_INTERNAL);

    // Don't import inaccessible fields except in structs, and then only look at them enough to see
    // if they impact whether the type is managed.
    if (fInaccess && !parent->IsStruct())
        return;

    // Get name.
    name = compiler()->namemgr->AddString(fieldnameText, cchFieldnameText - 1);

    // Import the type of the field.
    type = ImportFieldType(scope, signature, signature + cbSignature, &isVolatile, parent->typeVarsAll);

    if (fInaccess) {
        ASSERT(parent->IsStruct());

        // We need any struct fields (other than standard known ones) for recursion checking.
        // We need instance struct fields for checking managed/unmanaged.

        if (type->isAGGTYPESYM()) {
            AGGSYM * aggT = type->getAggregate();

            if (!aggT->IsValueType()) {
                // If it's not a value type, we only care if it's an instance field.
                if (!(flags & fdStatic))
                    parent->isManagedStruct = true;
                return;
            }

            // Convert enums to their underlying type.
            if (aggT->IsEnum() && aggT->underlyingType) {
                aggT = aggT->underlyingType->getAggregate();
                ASSERT(aggT->IsStruct());
            }
            if (parent->isPredefined && parent->iPredef < PT_OBJECT) {
                // Known to be unmanaged and non-recursive.
                return;
            }
        }
        else if (type->isPTRSYM()) {
            // Pointers are unmanaged.
            return;
        }
        else {
            if (!(flags & fdStatic))
                parent->isManagedStruct = true;
            // We need to keep TYVARSYM-valued fields for struct layout errors.
            if (!type->isTYVARSYM())
                return;
        }
    }

    // Enums are a bit special. Non-static fields serve only to record the
    // underlying integral type, and are otherwise ignored. Static fields are
    // enumerators and must be of the enum type. (We change other integral ones to the
    // enum type because it's probably what the emitting compiler meant.)
    if (parent->IsEnum()) {
        if (!(flags & fdStatic)) {
            // NOTE: The underlying type is set in the resolve inheritance phase now
            ASSERT(!type || !type->isNumericType() || type->fundType() > FT_LASTINTEGRAL ||
                parent->underlyingType == type->asAGGTYPESYM());
            return;
        }

        if (type != parent->getThisType()) {
            if (type == parent->underlyingType)
                type = parent->getThisType();   // If it's the underlying type, assume it's meant to be the enum type.
            else
                type = NULL;     // Bogus type.
        }
    }

    // Declare a field. If we get a name conflict, just ignore the whole field, since
    // there's not much usefulness in report this to the user.
    // Check for conflict.
    if (compiler()->getBSymmgr().LookupAggMember(name, parent, MASK_ALL)) {
        return;  // Already declared one.
    }
    sym = compiler()->getBSymmgr().CreateMembVar(name, parent, decl);
    sym->tokenImport = tokenField;

    // Record the type.
    if (!type) {
        sym->setBogus(true);
        type = compiler()->getBSymmgr().GetErrorSym();
    }
    sym->type = type;

    //
    // Import all the attributes we are interested in at compile time
    //
    IMPORTED_CUSTOM_ATTRIBUTES attributes;
    ImportCustomAttributes(scope, NULL, &attributes, tokenField);

    sym->SetDeprecated(attributes.isDeprecated, attributes.isDeprecatedError, attributes.deprecatedString);

    if (attributes.hasCLSattribute) {
        sym->hasCLSattribute = true;
        sym->isCLS = attributes.isCLS;
    }
    if (attributes.fixedBuffer != NULL) {
        if (!type->isAGGTYPESYM())
            goto NOT_FIXED;
        compiler()->EnsureState(type);
        if (parent->IsStruct() && !type->isGenericInstance() &&
            (flags & fdStatic) == 0 && attributes.fixedBuffer->isSimpleType() &&
            !attributes.fixedBuffer->isPredefType(PT_BOOL) && !attributes.fixedBuffer->isPredefType(PT_DECIMAL)) {
            // Do some basic checking to make sure this type only has 1 field
            NAME * pnFixed = compiler()->namemgr->GetPredefName(PN_FIXEDELEMENT);
            MEMBVARSYM * fixedField = NULL;
            FOREACHCHILD(type->getAggregate(), memb) {
                switch (memb->getKind()) {
                case SK_MEMBVARSYM:
                    if (fixedField != NULL || memb->name != pnFixed ||
                        memb->asMEMBVARSYM()->type != attributes.fixedBuffer)
                        goto NOT_FIXED;
                    fixedField = memb->asMEMBVARSYM();
                    break;
                case SK_AGGTYPESYM:
                    break;
                default:
                    goto NOT_FIXED;
                }
            } ENDFOREACHCHILD;
            if (fixedField == NULL) goto NOT_FIXED;

            ASSERT(fixedField == compiler()->getBSymmgr().LookupAggMember(pnFixed, type->getAggregate(), MASK_MEMBVARSYM));
            sym->type = compiler()->getBSymmgr().GetPtrType(attributes.fixedBuffer);
            sym->fixedAgg = type->getAggregate();
            sym->constVal.iVal = attributes.fixedBufferElementCount;
        } else {
NOT_FIXED:
            sym->setBogus(true);
        }
    }

    sym->SetAccess(access);
    sym->isStatic = !!(flags & fdStatic);
    sym->isVolatile = isVolatile;

    if (parent->IsStatic() && !sym->isStatic) {
        // The class is static but has a non-static member, so treat the member as bogus.
        sym->setBogus(true);
    }

    if ((flags & fdLiteral) && constType != ELEMENT_TYPE_VOID && 
        (!sym->type->isPredefType(PT_DECIMAL))) 
    {
        // A compile time constant.
        sym->isConst = true;
        sym->isStatic = true;

        if (!sym->getBogus()) {
            // Try to import the value from COM+ metadata.
            if ( ! ImportConstant(&sym->constVal, constLen, type, constType, constValue))
                sym->setBogus(true);
        }
    }
    else if (flags & fdInitOnly) {
        // A readonly field.
        sym->isReadOnly = true;
    }

    // decimal literals are stored in a custom blob since they can't be represented MD directly
    if ((flags & fdInitOnly) && (flags & fdStatic) && sym->type->isPredefType(PT_DECIMAL)) {

        if (attributes.hasDecimalLiteral) {
            sym->constVal.decVal = (DECIMAL*) compiler()->getGlobalSymAlloc().Alloc(sizeof(DECIMAL));
            *sym->constVal.decVal = attributes.decimalLiteral;
            sym->isConst = true;
            sym->isReadOnly = false;
        }
    }

    if (parent->IsEnum() && !sym->isConst) {
        // Enum members better be read-only constants.
        sym->setBogus(true);
    }

}

/*
 * Read all the custom attributes on a members and handle them appropriately
 */
void IMPORTER::ImportCustomAttributes(MODULESYM *mod, INFILESYM * infile, IMPORTED_CUSTOM_ATTRIBUTES * attributes, mdToken token)
{
    IMetaDataImport * metaimport = mod->GetMetaImport(compiler());
    HCORENUM corenum;           // For enumerating tokens.
    HALINKENUM hEnum;           // For enumerating tokens
    mdToken attributesBuffer[32];
    ULONG cAttributes, iAttribute;
    corenum = 0;

    MEM_SET_ZERO(*attributes);
    attributes->conditionalHead = &attributes->conditionalSymbols;
    if (token == mdtAssembly) {
        CheckHR(compiler()->linker->ImportTypes(compiler()->assemID, mod->getInputFile()->mdImpFile , mod->GetIndex(), &hEnum, NULL, NULL), mod);
    }

    do {
        // Get next batch of attributes.
        if (token == mdtAssembly) 
            CheckHR(compiler()->linker->EnumCustomAttributes(hEnum, 0, attributesBuffer, lengthof(attributesBuffer), &cAttributes), mod);
        else
            CheckHR(metaimport->EnumCustomAttributes(&corenum, token, 0, attributesBuffer, lengthof(attributesBuffer), &cAttributes), mod);

        // Process each attribute.
        for (iAttribute = 0; iAttribute < cAttributes; ++iAttribute) {
            mdToken attrToken;
            const void * pvData;
            ULONG cbSize;

            CheckHR(metaimport->GetCustomAttributeProps(
                attributesBuffer[iAttribute],
                NULL,
                &attrToken,
                &pvData,
                &cbSize), mod);

            ImportOneCustomAttribute(mod, infile, attributes, attrToken, pvData, cbSize);
        }
    } while (cAttributes > 0);

    if (token == mdtAssembly) {
        CheckHR(compiler()->linker->CloseEnum(hEnum), mod);
    }
    else {
        metaimport->CloseEnum(corenum);
    }
    
    if (token != mdtAssembly)
    {
        corenum = 0;
        CheckHR(metaimport->EnumPermissionSets(&corenum, token, dclLinktimeCheck, attributesBuffer, 1, &cAttributes), mod);
        metaimport->CloseEnum(corenum);
        
        attributes->hasLinkDemand = (cAttributes != 0);
    }
}

/*
 * Handle one custom attribute read in.
 */
void IMPORTER::ImportOneCustomAttribute(MODULESYM *mod, INFILESYM * infile, IMPORTED_CUSTOM_ATTRIBUTES * attributes, mdToken attrToken, const void * pvData, ULONG cbSize)
{
    IMetaDataImport * metaimport = mod->GetMetaImport(compiler());
    mdToken typeToken;
    WCHAR ctornameText[MAX_IDENT_SIZE];     // name of field
    ULONG cchCtornameText;
    WCHAR fullNameText[MAX_FULLNAME_SIZE];
    ULONG cchFullNameText;
    PNAME typeName;
    PCCOR_SIGNATURE signature;
    ULONG cbSignature = 0;
    IMPORTEDCUSTOMATTRKIND attrKind;


    // a 0 length blob is allowed, but not for any of the attributes we are looking for
    if (cbSize == 0)
        return;

    if (TypeFromToken(attrToken) == mdtMemberRef && attrToken != mdMemberRefNil) {

        // Get name of member and signature.
        CheckHR(metaimport->GetMemberRefProps(
                    attrToken,
                    &typeToken,
                    ctornameText, lengthof(ctornameText), & cchCtornameText, // Field name
                    &signature, &cbSignature), mod);
        CheckTruncation(cchCtornameText, lengthof(ctornameText), mod);

        if (cchCtornameText == 0 || wcscmp(ctornameText, compiler()->namemgr->GetPredefName(PN_CTOR)->text) != 0)
            return ;  // Member ref didn't reference a constructor.
    } else if (TypeFromToken(attrToken) == mdtMethodDef && attrToken != mdMethodDefNil) {

        // Get name of member and signature.
        CheckHR(metaimport->GetMethodProps(
                    attrToken,
                    &typeToken,
                    ctornameText, lengthof(ctornameText), & cchCtornameText, // Field name
                    NULL,
                    &signature, &cbSignature,
                    NULL, NULL), mod);
        CheckTruncation(cchCtornameText, lengthof(ctornameText), mod);

        if (wcscmp(ctornameText, compiler()->namemgr->GetPredefName(PN_CTOR)->text) != 0)
            return ;  // Member ref didn't reference a constructor.
    }
    else
        return;  // Only MethodDefs or MemberRefs are allowed

    if (TypeFromToken(typeToken) == mdtTypeRef && typeToken != mdTypeRefNil) {
        mdToken resolutionScope;
        // Get name of type.
        CheckHR(metaimport->GetTypeRefProps(
                    typeToken,                                              // typeref token                 
                    &resolutionScope,                                       // resolution scope
                    fullNameText, lengthof(fullNameText), &cchFullNameText),// name
            mod);
        CheckTruncation(cchFullNameText, lengthof(fullNameText), mod);

        if (TypeFromToken(resolutionScope) != mdtModule && TypeFromToken(resolutionScope) != mdtModuleRef &&
             TypeFromToken(resolutionScope) != mdtAssembly && TypeFromToken(resolutionScope) != mdtAssemblyRef)
        {
            // Not looking for nested types
            return;
        }
    } else if (TypeFromToken(typeToken) == mdtTypeDef && typeToken != mdTypeDefNil) {
        DWORD flags;
        // Get name of type.
        CheckHR(metaimport->GetTypeDefProps(
                    typeToken,                          // typeref token                    
                    fullNameText, lengthof(fullNameText), &cchFullNameText,  // name
                    &flags, NULL),
            mod);
        CheckTruncation(cchFullNameText, lengthof(fullNameText), mod);

        if (IsTdNested(flags)) {
            // Not looking for any nested types
            return;
        }
    } else {
        // Unrecognized Member Ref
        return;
    }

    ASSERT(!COMPILER::IsRegString(fullNameText, L"attribute"));

    // Convert name to a NAME by looking up in hash table.
    typeName = compiler()->namemgr->LookupString(fullNameText);
    if (typeName == NULL)
        return;  // Not a known name.

    // Match type name and signature against list of predefined ones.
    attrKind = CUSTOMATTR_NONE;
    for (unsigned int i = 0; i < lengthof(g_importedAttributes); ++i) {
        if (compiler()->namemgr->GetPredefName(g_importedAttributes[i].className) == typeName &&
            (g_importedAttributes[i].cbSig < 0 ||
             ((unsigned) g_importedAttributes[i].cbSig == cbSignature &&
             memcmp(g_importedAttributes[i].sig, signature, cbSignature) == 0)))
        {
            attrKind = g_importedAttributes[i].attrKind;
            break;
        }
    }

    if (attrKind == CUSTOMATTR_NONE)
        return;         // Not a predefined custom attribute kind.

    // Make sure that data is in the correct format. Check the prolog, then
    // move beyond it.
    if (cbSize < sizeof(WORD) || GET_UNALIGNED_VAL16 (pvData) != 1) {
        return;
    }
    cbSize -= sizeof(WORD);
    pvData = (WORD *)pvData + 1;

    // We found an attribute kind that we know about. Grab any information from the binary data, and
    // apply it to the symbol.
    switch (attrKind) {
    case CUSTOMATTR_DEPRECATED_VOID:
        attributes->isDeprecated = true;
        break;

    case CUSTOMATTR_DEPRECATED_STR:
        attributes->isDeprecated = true;
        attributes->deprecatedString = ImportCustomAttrArgString(mod, & pvData, & cbSize);
        break;

    case CUSTOMATTR_DEPRECATED_STRBOOL:
        attributes->isDeprecated = true;
        attributes->deprecatedString = ImportCustomAttrArgString(mod, & pvData, & cbSize);
        attributes->isDeprecatedError = ImportCustomAttrArgBool(mod, & pvData, & cbSize);
        break;

    case CUSTOMATTR_ATTRIBUTEUSAGE_VOID:
        {
            attributes->attributeKind = catAll;
            int numberOfNamedArguments = ImportCustomAttrArgWORD(mod, &pvData, &cbSize);
            for (int i = 0; i < numberOfNamedArguments; i += 1) {
                PCWSTR name;
                TYPESYM *type;
                ImportNamedCustomAttrArg(mod, &pvData, &cbSize, &name, &type);
                if (!name || !type)
                    break;
                if (!wcscmp(name, compiler()->namemgr->GetPredefName(PN_VALIDON)->text)) {
                    attributes->allowMultiple = !!(CorAttributeTargets) ImportCustomAttrArgInt(mod, &pvData, &cbSize);
                } else if (!wcscmp(name, compiler()->namemgr->GetPredefName(PN_ALLOWMULTIPLE)->text)) {
                    attributes->allowMultiple = (ImportCustomAttrArgBYTE(mod, &pvData, &cbSize) ? true : false);
                } else if (!wcscmp(name, compiler()->namemgr->GetPredefName(PN_INHERITED)->text)) {
                    ImportCustomAttrArgBYTE(mod, &pvData, &cbSize);
                } else {
                    ASSERT(!"Unknown named argument for imported attributeusage");
                    break;
                }
            }
        }
        break;

    case CUSTOMATTR_ATTRIBUTEUSAGE_VALIDON:
        {
            attributes->attributeKind = (CorAttributeTargets) ImportCustomAttrArgInt(mod, & pvData, & cbSize);
            int numberOfNamedArguments = ImportCustomAttrArgWORD(mod, &pvData, &cbSize);
            for (int i = 0; i < numberOfNamedArguments; i += 1) {
                PCWSTR name;
                TYPESYM *type;
                ImportNamedCustomAttrArg(mod, &pvData, &cbSize, &name, &type);
                if (!name || !type)
                    break;
                if (!wcscmp(name, compiler()->namemgr->GetPredefName(PN_ALLOWMULTIPLE)->text)) {
                    attributes->allowMultiple = (ImportCustomAttrArgBYTE(mod, &pvData, &cbSize) ? true : false);
                } else if (!wcscmp(name, compiler()->namemgr->GetPredefName(PN_INHERITED)->text)) {
                    ImportCustomAttrArgBYTE(mod, &pvData, &cbSize);
                } else {
                    ASSERT(!"Unknown named argument for imported attributeusage");
                    break;
                }
            }
        }
        break;

    case CUSTOMATTR_CONDITIONAL:
        // convert the string to a name and return it
        compiler()->getBSymmgr().AddToGlobalNameList(
                compiler()->namemgr->AddString(ImportCustomAttrArgString(mod, & pvData, & cbSize)),
                &attributes->conditionalHead);
        break;

    case CUSTOMATTR_DECIMALLITERAL:
        if ((cbSize + sizeof(WORD)) == sizeof(DecimalConstantBuffer))
        {
            DecimalConstantBuffer *buffer = (DecimalConstantBuffer*)(((BYTE*)pvData)-(int)sizeof(WORD));
            DECIMAL_SCALE(attributes->decimalLiteral)   = buffer->scale;
            DECIMAL_SIGN(attributes->decimalLiteral)    = buffer->sign;
            DECIMAL_HI32(attributes->decimalLiteral)    = GET_UNALIGNED_VAL32(&buffer->hi);
            DECIMAL_MID32(attributes->decimalLiteral)   = GET_UNALIGNED_VAL32(&buffer->mid);
            DECIMAL_LO32(attributes->decimalLiteral)    = GET_UNALIGNED_VAL32(&buffer->low);
            attributes->hasDecimalLiteral = true;
        }
        break;

    case CUSTOMATTR_DEPRECATEDHACK:
        attributes->isDeprecated = true;
        break;

    case CUSTOMATTR_CLSCOMPLIANT:
        attributes->hasCLSattribute = true;
        attributes->isCLS = ImportCustomAttrArgBool(mod, & pvData, & cbSize);
        break;

    case CUSTOMATTR_PARAMS:
        attributes->isParamListArray = true;
        break;

    case CUSTOMATTR_REQUIRED:
        attributes->hasRequiredAttribute = true;
        break;

    case CUSTOMATTR_DEFAULTMEMBER2:
    case CUSTOMATTR_DEFAULTMEMBER:
        attributes->defaultMember = ImportCustomAttrArgString(mod, & pvData, & cbSize);
        break;

    case CUSTOMATTR_COCLASS:
        attributes->CoClassName = ImportCustomAttrArgString(mod, & pvData, & cbSize);
        break;

    case CUSTOMATTR_FIXEDBUFFER:
        {
            bool fInvalidSig;
            PWSTR szBufferType = ImportCustomAttrArgString(mod, & pvData, & cbSize);
            attributes->fixedBuffer = ResolveFullMetadataTypeName(mod, szBufferType, &fInvalidSig);
            attributes->fixedBufferElementCount = ImportCustomAttrArgInt(mod, & pvData, & cbSize);
            ASSERT(!fInvalidSig || attributes->fixedBuffer == NULL);
            if (attributes->fixedBuffer == NULL) {
                attributes->fixedBuffer = compiler()->GetReqPredefType(PT_OBJECT);
            }
        }
        break;

    case CUSTOMATTR_COMPILATIONRELAXATIONS:
        attributes->fCompilationRelaxations = true;
        break;

    case CUSTOMATTR_RUNTIMECOMPATIBILITY:
        {
            attributes->fRuntimeCompatibility = true;

            int numberOfNamedArguments = ImportCustomAttrArgWORD(mod, &pvData, &cbSize);
            for (int i = 0; i < numberOfNamedArguments; i += 1) {
                PCWSTR name;
                // Don't get the type, as these attributes need to be imported before the predef types are loaded
                ImportNamedCustomAttrArg(mod, &pvData, &cbSize, &name, NULL);
                if (!name)
                    break;
                if (!wcscmp(name, compiler()->namemgr->GetPredefName(PN_WRAPNONEXCEPTIONTHROWS)->text)) {
                    attributes->fWrapNonExceptionThrows = (ImportCustomAttrArgBYTE(mod, &pvData, &cbSize) ? true : false);
                } else {
                    ASSERT(!"Unknown named argument for imported RuntimeCompatibility");
                    break;
                }
            }
        }
        break;
        
    case CUSTOMATTR_FRIENDASSEMBLY:
        if (infile) {
            const WCHAR * psz = ImportCustomAttrArgString(mod, &pvData, &cbSize);
            if (psz) {
                ImportScopeModule scope(this, mod);
                NAME *nameAssemblyRef = compiler()->namemgr->AddString(psz);
                int aid = MapAssemblyRefToAid(nameAssemblyRef, scope, true);
                if (aid != kaidUnresolved && infile->GetAssemblyID() != aid) {
                    infile->AddInternalsVisibleTo(aid, &compiler()->getGlobalSymAlloc());
                    if (aid != kaidThisAssembly && infile->GetAssemblyID() != kaidThisAssembly && MatchesThisAssembly(nameAssemblyRef, scope)) {
                        compiler()->RecordAssemblyRefToOutput(nameAssemblyRef, scope.GetModule(), true);
                        infile->AddInternalsVisibleTo(kaidThisAssembly, &compiler()->getGlobalSymAlloc());
                    }
                }
                attributes->fHasFriends = true;
            }
        }
        break;


    default:
        break;
    }
}

/*
 * Import a single custom attribute argument that is a string. Updates *ppvData and *pcbSize 
 */
WCHAR * IMPORTER::ImportCustomAttrArgString(MODULESYM * mod, LPCVOID * ppvData, ULONG * pcbSize)
{
    PCCOR_SIGNATURE psigData, psigDataEnd;
    PCSTR pstrData;
    int cbString, cchString;
    WCHAR * str;

    ImportScopeModule scope(this, mod);

    // String is stored as compressed length, UTF8.
    psigData = (PCCOR_SIGNATURE) *ppvData;
    psigDataEnd  = psigData + *pcbSize;
    if (SigPeekByte(scope, psigData, psigDataEnd) == 0xFF) {
        pstrData = (PCSTR) (psigData + 1);
        str = NULL;
    }
    else {
        cbString = SigUncompressData(scope, &psigData, psigDataEnd);
        pstrData = (PCSTR) CheckBufferAccess(mod, psigData, cbString, psigDataEnd - psigData);
        cchString = UnicodeLengthOfUTF8(pstrData, cbString) + 1;
        str = (WCHAR *) compiler()->getGlobalSymAlloc().Alloc(cchString * sizeof(WCHAR));
        UTF8ToUnicode(pstrData, cbString, str, cchString);
        str[cchString - 1] = L'\0';
        pstrData += cbString;
    }

    *pcbSize -= (ULONG)((BYTE *)pstrData - (BYTE *) *ppvData);
    *ppvData = pstrData;

    return str;
}


/*
 * Import a single custom attribute argument that is a bool. Updates *ppvData and *pcbSize 
 */
bool IMPORTER::ImportCustomAttrArgBool(MODULESYM * scope, LPCVOID * ppvData, ULONG * pcbSize)
{
    return ImportCustomAttrArgBYTE(scope, ppvData, pcbSize) ? true : false;
}


/*
 * Import a single custom attribute argument that is a int32. Updates *ppvData and *pcbSize 
 */
int IMPORTER::ImportCustomAttrArgInt(MODULESYM * scope, LPCVOID * ppvData, ULONG * pcbSize)
{
    CheckBufferAccess(scope, *ppvData, sizeof(__int32), *pcbSize);

    const __int32 * pbData = (__int32 *) *ppvData;
    int i = GET_UNALIGNED_VAL32(pbData);
    pbData++;

    *ppvData = pbData;
    *pcbSize -= sizeof(__int32);

    return i;
}

/*
 * Import a single custom attribute argument that is a int16. Updates *ppvData and *pcbSize 
 */
WORD IMPORTER::ImportCustomAttrArgWORD(MODULESYM * scope, LPCVOID * ppvData, ULONG * pcbSize)
{
    CheckBufferAccess(scope, *ppvData, sizeof(WORD), *pcbSize);

    const WORD * pbData = (WORD *) *ppvData;
    WORD w = GET_UNALIGNED_VAL16(pbData);
    pbData++;

    *ppvData = pbData;
    *pcbSize -= sizeof(WORD);

    return w;
}

/*
 * Import a single custom attribute argument that is a int16. Updates *ppvData and *pcbSize 
 */
BYTE IMPORTER::ImportCustomAttrArgBYTE(MODULESYM * scope, LPCVOID * ppvData, ULONG * pcbSize)
{
    CheckBufferAccess(scope, *ppvData, sizeof(BYTE), *pcbSize);

    const BYTE * pbData = (BYTE *) *ppvData;
    BYTE b = *pbData++;
    *ppvData = pbData;
    *pcbSize -= sizeof(BYTE);

    return b;
}

static const PREDEFTYPE g_stTOptMap[] = 
{
    PT_BOOL,
    PT_CHAR,
    PT_SBYTE,
    PT_BYTE,
    PT_SHORT,
    PT_USHORT,
    PT_INT,
    PT_UINT,
    PT_LONG,
    PT_ULONG,
    PT_FLOAT,
    PT_DOUBLE,
    PT_STRING,
};

/*
 * Imports a single named custom attribute argument.
 */
void IMPORTER::ImportNamedCustomAttrArg(MODULESYM * scope, LPCVOID *ppvData, ULONG *pcbSize, PCWSTR *pname, TYPESYM **type)
{
    CorSerializationType fieldOrProp;
    fieldOrProp = (CorSerializationType) ImportCustomAttrArgBYTE(scope, ppvData, pcbSize);

    //
    // read in the type
    //
    CorSerializationType st = (CorSerializationType) ImportCustomAttrArgBYTE(scope, ppvData, pcbSize);
    if (st >= SERIALIZATION_TYPE_BOOLEAN && st <= SERIALIZATION_TYPE_STRING) {
        if (type)
            *type = compiler()->GetOptPredefType(g_stTOptMap[st], false);
    } else {
        switch (st) {
        case SERIALIZATION_TYPE_SZARRAY:
        case SERIALIZATION_TYPE_TYPE:
        case SERIALIZATION_TYPE_TAGGED_OBJECT:
        default:
                ASSERT(!"UNDONE: named custom attr arg type");
                if (type)
                    *type = NULL;
                *pname = NULL;
                return;

        case SERIALIZATION_TYPE_ENUM:
            {
                PCWSTR className;
                className = ImportCustomAttrArgString(scope, ppvData, pcbSize);
                ASSERT(!wcscmp(className, L"System.AttributeTargets"));
                if (type)
                    *type = compiler()->GetOptPredefType(PT_ATTRIBUTETARGETS, false);
            }
        }
    }

    //
    // read in the name
    //
    *pname = ImportCustomAttrArgString(scope, ppvData, pcbSize);
}

/*
 * Import a method/property ret type and params from a signature. Sets the isBogus flag
 * if the signature has a type we don't handle.
 */
void IMPORTER::ImportMethodOrPropSignature(mdMethodDef token, PCCOR_SIGNATURE sig, PCCOR_SIGNATURE sigEnd, PMETHPROPSYM sym)
{
    ASSERT(sym->GetMetaImport(compiler()));
    ASSERT(sym->parent->isAGGSYM());
    int modOptCount = 0;
    byte callConv = 0;
    bool isMethod = sym->isMETHSYM();
    TypeArray * typeVarsMeth = isMethod ? sym->asMETHSYM()->typeVars : BSYMMGR::EmptyTypeArray();

    ASSERT(typeVarsMeth);

    ImportScopeModule scope(this, sym->GetModule());

    if (isMethod && TypeFromToken(token) != mdtMethodDef || !isMethod && TypeFromToken(token) != mdtProperty)
        goto LBogus;

    if (!ImportSignature(scope, token, sig, sigEnd, &sym->retType, &sym->params, &callConv, &modOptCount, NULL,
        sym->getClass()->typeVarsAll, typeVarsMeth))
    {
LBogus:
        sym->setBogus(true);
        if (!sym->retType)
            sym->retType = compiler()->getBSymmgr().GetVoid();
        if (!sym->params)
            sym->params = BSYMMGR::EmptyTypeArray();
        return;
    }

    ASSERT(sym->params);

    // We don't support generic properties.
    if ((callConv & IMAGE_CEE_CS_CALLCONV_GENERIC) && !isMethod)
        goto LBogus;

    if ((callConv & IMAGE_CEE_CS_CALLCONV_HASTHIS) && sym->isStatic)
        goto LBogus;

    // Deal with calling convention. Must be default or varargs for methods, default or property for properties.
    switch (callConv & IMAGE_CEE_CS_CALLCONV_MASK) {
    case IMAGE_CEE_CS_CALLCONV_DEFAULT:
    case IMAGE_CEE_CS_CALLCONV_UNMGD:
        if (sym->params->size > 0 && sym->isMETHSYM()) {
            // Check for params...
            TYPESYM * typeLast = sym->params->Item(sym->params->size - 1);
            if (typeLast->isARRAYSYM() && typeLast->asARRAYSYM()->rank == 1)
                sym->isParamArray = !!IsParamArray(sym->GetModule(), token, sym->params->size);
        }
        break;

    case IMAGE_CEE_CS_CALLCONV_PROPERTY:
        if (isMethod)
            goto LBogus;
        break;

    case IMAGE_CEE_CS_CALLCONV_VARARG:
        if (!isMethod || typeVarsMeth->size || sym->getClass()->typeVarsAll->size)
            goto LBogus;
        sym->asMETHSYM()->isVarargs = true;
        break;

    default:
        // We don't support that calling convention.
        goto LBogus;
    }

    sym->modOptCount = modOptCount;
}

bool IMPORTER::ImportSignature(ImportScope & scope, mdMethodDef token, PCCOR_SIGNATURE sig, PCCOR_SIGNATURE sigEnd,
    TYPESYM ** ptypeRet, TypeArray ** pparams, byte * pcallConv, int * pcmod, int * pcvar,
    TypeArray * typeVarsCls, TypeArray * typeVarsMeth)
{
    IMetaDataImport * metaimport = scope.GetMetaImport();
    ASSERT(metaimport);

    TYPESYM ** prgtype;
    HRESULT hr;

    // Format of method signature is calling convention, followed by param count,
    // return type, then param types.

    *pcallConv = SigGetByte(scope, &sig, sigEnd);

    // Check for varargs
    bool isVarargs = ((*pcallConv & IMAGE_CEE_CS_CALLCONV_MASK) == IMAGE_CEE_CS_CALLCONV_VARARG);

    if (*pcallConv & IMAGE_CEE_CS_CALLCONV_GENERIC) {
        int cvar = CorSigUncompressData(sig);
        if (pcvar)
            *pcvar = cvar;
        if (typeVarsMeth && typeVarsMeth->size != cvar) {
            VSFAIL("Why is the method arity wrong?");
            return false;
        }
    }
    else if (pcvar)
        *pcvar = 0;

    // Get param count.
    int cParams = SigUncompressData(scope, &sig, sigEnd);
    int cParamsVarArgs = cParams + (isVarargs ? 1 : 0);

    // Get return type.
    *ptypeRet = ImportSigType(scope, &sig, sigEnd, kfisoAllowVoid, pcmod, typeVarsCls, typeVarsMeth);
    if (!*ptypeRet)
        return false;

    // Alloc some space on the stack for the param types.
    prgtype = STACK_ALLOC(TYPESYM *, cParamsVarArgs);

    // Grab the parameters.
    for (int iParam = 0; iParam < cParams; ++iParam) {
        // Get the type of this next parameter.
        TYPESYM * type = ImportSigType(scope, &sig, sigEnd, kfisoAllowByref, pcmod, typeVarsCls, typeVarsMeth);

        // check for out param
        if (type && type->isPARAMMODSYM() && type->asPARAMMODSYM()->isRef && TypeFromToken(token) == mdtMethodDef) {
            // It's a byref parameter. We need to get the param flags to determine
            // if it is ref or out calling mode.
            ULONG sequenceNum = iParam + 1;
            mdParamDef paramToken;

            // Get the param flags for this parameter.
            hr = metaimport->GetParamForMethodIndex(
                token,
                sequenceNum,
                &paramToken);

            // If a parameter record is missing, that's OK, just assume flags are zero.
            if (SUCCEEDED(hr) && TypeFromToken(paramToken) == mdtParamDef && paramToken != mdParamDefNil) {
                DWORD paramFlags;
                CheckHR(metaimport->GetParamProps(
                                    paramToken,
                                    NULL,
                                    NULL,
                                    NULL,
                                    0,
                                    NULL,
                                    &paramFlags,
                                    NULL,
                                    NULL,
                                    NULL), scope);

                // Is it a ref or out? If only pdOut is set, it's an out, otherwise a ref.
                if ((paramFlags & (pdOut | pdIn))== pdOut)
                    type = compiler()->getBSymmgr().GetParamModifier(type->asPARAMMODSYM()->paramType(), true);
            }
        }

        if (!type)
            return false;

        // Save the param type.
        prgtype[iParam] = type;
    }

    if (isVarargs)
        prgtype[cParams] = compiler()->getBSymmgr().GetArglistSym();

    // Record the signature and param count.
    *pparams = compiler()->getBSymmgr().AllocParams(cParamsVarArgs, prgtype);

    return true;
}

// This is used when we need to emit a signature with "inherited" modopts. The signature should not
// be used for anything else. This doesn't distinguish between ref and out.
void IMPORTER::ImportSignatureWithModOpts(
    MODULESYM *mod,
    PCCOR_SIGNATURE sig,
    PCCOR_SIGNATURE sigEnd,
    TYPESYM **retType,
    TypeArray ** params,
    int *pmodOptCount,
    TypeArray *pClassTypeFormals,
    TypeArray *pMethTypeFormals)
{
    int iParam;
    BYTE callConv;
    PTYPESYM * paramTypes;

    ImportScopeModule scope(this, mod);

    // Format of method signature is calling convention, followed by param count,
    // return type, then param types.

    // Deal with calling convention.
    callConv = SigGetByte(scope, &sig, sigEnd);
    if (callConv & IMAGE_CEE_CS_CALLCONV_GENERIC) {
        int cMethTypeFormals2;
        cMethTypeFormals2 = CorSigUncompressData(sig);
        ASSERT(cMethTypeFormals2 == TypeArray::Size(pMethTypeFormals));
    }

    bool isVarargs = ((callConv & IMAGE_CEE_CS_CALLCONV_MASK) == IMAGE_CEE_CS_CALLCONV_VARARG);

    // Get param count.
    int cParams = SigUncompressData(scope, &sig, sigEnd);
    int cParamsVarArgs = cParams + ((isVarargs) ? 1 : 0);

    // Get return type.
    *retType = ImportSigType(scope, &sig, sigEnd, kfisoAllowVoid | kfisoIncludeModOpts, pmodOptCount, pClassTypeFormals, pMethTypeFormals);

    // Alloc some space on the stack for the param types.
    paramTypes = STACK_ALLOC(PTYPESYM, cParamsVarArgs);

    // Grab the parameters.
    for (iParam = 0; iParam < cParams; ++iParam) {
        PTYPESYM type;

        // Get the type of this next parameter.
        type = ImportSigType(scope, &sig, sigEnd, kfisoAllowByref | kfisoIncludeModOpts, pmodOptCount, pClassTypeFormals, pMethTypeFormals);

        if (!type) {
            *params = NULL;
            return;
        }

        // We don't care whether it's ref vs. out.

        // Save the param type.
        paramTypes[iParam] = type;
    }

    if (isVarargs) {
        paramTypes[cParams] = compiler()->getBSymmgr().GetArglistSym();
    }

    // Record the signature and param count.
    *params = compiler()->getBSymmgr().AllocParams(cParamsVarArgs, paramTypes);
}


BOOL IMPORTER::IsParamArray(
    MODULESYM * scope,
    mdMethodDef methodToken,
    int iParam
    )
{
    ASSERT(scope->GetMetaImport(compiler()) && TypeFromToken(methodToken) == mdtMethodDef);
    IMetaDataImport * metaimport = scope->GetMetaImport(compiler());

    mdParamDef paramToken;
    // Get the param flags for this parameter.
    HRESULT hr = metaimport->GetParamForMethodIndex(
        methodToken,
        iParam,
        &paramToken);

    if (SUCCEEDED(hr) && TypeFromToken(paramToken) == mdtParamDef) { // if failed, could be just because parameter doesn't exist.
        IMPORTED_CUSTOM_ATTRIBUTES attributes;
        ImportCustomAttributes(scope, NULL, &attributes, paramToken);

        if (attributes.isParamListArray) {
            return true;
        }
    }
    
    return false;
}

/*
 * Import a methoddef and create a corresponding METHSYM symbol. If we can't import
 * the method because it has attributes/types that we don't support and can't safely ignore,
 * we set the "isBogus" flag on the method. If we ever try to use that method, we'll give
 * an error (but if we don't use the method, the user never knows or cares).
 */
void IMPORTER::ImportMethod(PAGGSYM parent, PAGGDECLSYM decl, mdMethodDef tokenMethod)
{
    ASSERT(!parent->isMDPrivate);

    const bool fDefineParams = true;
    MODULESYM * scope = parent->GetModule();
    ASSERT(scope->GetMetaImport(compiler()));
    IMetaDataImport * metaimport = scope->GetMetaImport(compiler());
    WCHAR methodnameText[MAX_FULLNAME_SIZE];     // name of method
    ULONG cchMethodnameText;
    DWORD flags;
    PCCOR_SIGNATURE signature;
    ULONG cbSignature;
    PNAME name;
    METHSYM * sym;

    if (TypeFromToken(tokenMethod) != mdtMethodDef || tokenMethod == mdMethodDefNil) {
        return;
    }

    // Get method properties.
    CheckHR(metaimport->GetMethodProps(
        tokenMethod,                                    // The method for which to get props.
        NULL,                                               // Put method's class here.
        methodnameText, lengthof(methodnameText), &cchMethodnameText,   // Method name
        & flags,                                            // Put flags here.
        & signature, & cbSignature,                             // Method signature
        NULL,                                                   // codeRVA
        NULL), scope);                                              // Impl. Flags
    CheckTruncation(cchMethodnameText, lengthof(methodnameText), scope);

    if (cchMethodnameText <= 1)
        return;

    // Check the access.
    bool fAbstract = !!(flags & mdAbstract);
    bool fVirtual = !!(flags & mdVirtual);
    bool fForceImport = parent->isAbstract && (fAbstract || fVirtual);
    ACCESS access = ConvertAccessLevel(flags, scope->getInputFile(), fForceImport);

    // Interfaces with non-public methods are bogus.
    if (parent->IsInterface() && access != ACC_PUBLIC)
        parent->setBogus(true);

    // ConvertAccessLevel nukes things we can't see by returning ACC_PRIVATE.
    if (access < ACC_INTERNAL && !fForceImport)
        return;

    //
    if (!(flags & mdRTSpecialName) && wcschr(methodnameText, L'.') != NULL)
        return;    

    // Interfaces in C# do not contain static methods, but can in the CLR. If we see a static
    // method in an interface, we must be importing from not C# metadata, so just ignore that method.
    if ((flags & mdStatic) && parent->IsInterface())
        return;

    name = compiler()->namemgr->AddString(methodnameText, cchMethodnameText - 1);

    // Declare a method.
    sym = compiler()->getBSymmgr().CreateMethod(name, parent, decl);
    ASSERT(sym);
    sym->tokenImport = tokenMethod;

    //
    // Import all the attributes we are interested in at compile time
    //
    IMPORTED_CUSTOM_ATTRIBUTES attributes;
    ImportCustomAttributes(scope, NULL, &attributes, tokenMethod);

    // Set attributes of the method.
    sym->SetAccess(access);

    sym->conditionalSymbols = attributes.conditionalSymbols;
    sym->SetDeprecated(attributes.isDeprecated, attributes.isDeprecatedError, attributes.deprecatedString);

    if (attributes.hasCLSattribute) {
        sym->hasCLSattribute = true;
        sym->isCLS = attributes.isCLS;
    }
    sym->hasLinkDemand = attributes.hasLinkDemand;

    sym->isStatic = (flags & mdStatic) ? true : false;
    sym->isAbstract = fAbstract;
    if (!!(flags & mdRTSpecialName) && (name == compiler()->namemgr->GetPredefName((flags & mdStatic) ? PN_STATCTOR : PN_CTOR)))
        sym->SetMethKind(MethodKind::Ctor);
    sym->isVirtual = fVirtual && !sym->isCtor() && !(flags & mdFinal);
    sym->isMetadataVirtual = fVirtual;
    sym->isOperator = false;
    sym->isHideByName = (flags & mdHideBySig) ? false : true;

    if (parent->IsStatic() && !sym->isStatic) {
        // The class is static but has a non-static member, so treat the member as bogus.
        sym->setBogus(true);
    }

    // we are importing an abstract method which
    // is not marked as virtual, currently code generation
    // keys on the isVirtual property to set the dispatch type
    // on function calls.
    //
    // if this fires, find out who generated the metadata
    // and get them to fix their metadata generation
    //
    ASSERT(sym->isVirtual || !sym->isAbstract);
    sym->isVirtual = (sym->isVirtual || sym->isAbstract) ? true : false;

    sym->isVirtual = sym->isVirtual && !sym->isStatic;


    if (fDefineParams) {
        ImportMethodPropsWorker(parent, tokenMethod, signature, cbSignature, sym);
    }

    //
    // convert special methods into operators.
    // If we don't recognize this as a C#-like operator then we just ignore the special
    // bit and make it a regular method
    //
    if ((flags & mdSpecialName) && !(flags & mdRTSpecialName)) {
        if (name == compiler()->namemgr->GetPredefName(PN_OPEXPLICITMN)) {
            sym->isOperator = true;
            sym->SetMethKind(MethodKind::ExplicitConv);

            // Add it to the conversion list.
            sym->SetConvNext(parent->convFirst);
            parent->convFirst = sym;

            // The flag is set whenever this class or any of its bases has a conversion operator.
            parent->hasConversion = true;
        }
        else if (name == compiler()->namemgr->GetPredefName(PN_OPIMPLICITMN)) {
            sym->isOperator = true;
            sym->SetMethKind(MethodKind::ImplicitConv);

            // Add it to the conversion list.
            sym->SetConvNext(parent->convFirst);
            parent->convFirst = sym;

            parent->hasConversion = true;
        }
        else {
            OPERATOR iOp = compiler()->clsDeclRec.operatorOfName(name);
            if (OP_LAST != iOp && OP_EXPLICIT != iOp && OP_IMPLICIT != iOp) {
                sym->isOperator = true;

                if (iOp == OP_EQ && sym->params->Item(0) == sym->params->Item(1) &&
                    sym->params->Item(0) == parent->getThisType())
                {
                    parent->fHasSelfEq = true;
                }
                else if (iOp == OP_NEQ && sym->params->Item(0) == sym->params->Item(1) &&
                    sym->params->Item(0) == parent->getThisType())
                {
                    parent->fHasSelfNe = true;
                }
            }
        }
    }

    if (fVirtual && !(flags & mdNewSlot) && parent->baseClass) {
        // NOTE: The isOverride bit is NOT valid until
        // AFTER the prepare stage.
        // for now just set to keep track of the !mdNewSlot bit
        // We will fix this up later in CLSDREC::setOverrideBits()
        // during the prepare stage
        sym->isOverride = true;
    }

    if (sym->isCtor() && !sym->params->size && !sym->isStatic) {
        parent->hasNoArgCtor = true;
        if (sym->GetAccess() == ACC_PUBLIC)
            parent->hasPubNoArgCtor = true;
    }

    if (fDefineParams) {
        sym->hasParamsDefined = true;
    }

    ASSERT(!sym->hasParamsDefined == !(sym->typeVars && sym->params));
}


void IMPORTER::ImportMethodPropsWorker(AGGSYM * parent, mdToken tokenMethod, PCCOR_SIGNATURE sig, ULONG cbSig, METHSYM * meth)
{
    DefineMethodTypeFormals(tokenMethod, meth, meth, &meth->typeVars);

    // Set the method signature.
    ImportMethodOrPropSignature(tokenMethod, sig, sig + cbSig, meth);

    if (meth->typeVars->size > 0)
        DefineBounds(meth);

}


void IMPORTER::ImportMethodProps(METHSYM * sym)
{

    if (!sym || sym->hasParamsDefined) {
        return;
    }

    AGGSYM * parent = sym->getClass();

    MODULESYM * scope = parent->GetModule();
    ASSERT(scope->GetMetaImport(compiler()));
    IMetaDataImport * metaimport = scope->GetMetaImport(compiler());
    PCCOR_SIGNATURE signature;
    ULONG cbSignature;
    mdToken tokenMethod = sym->tokenImport;

    // Get method properties.
    CheckHR(metaimport->GetMethodProps(
        tokenMethod,                                    // The method for which to get props.
        NULL,                                               // Put method's class here.
        NULL, 0, NULL,   // Method name
        NULL,                                            // Put flags here.
        & signature, & cbSignature,                             // Method signature
        NULL,                                                   // codeRVA
        NULL), scope);                                              // Impl. Flags
        


    ImportMethodPropsWorker(sym->getClass(), tokenMethod, signature, cbSignature, sym);

    sym->hasParamsDefined = true;

}


/*
 * Import a propertydef.
 */
void IMPORTER::ImportProperty(PAGGSYM parent, PAGGDECLSYM decl, mdProperty tokenProperty, PNAME defaultMemberName)
{
    MODULESYM * scope = parent->GetModule();
    ASSERT(scope->GetMetaImport(compiler()));
    IMetaDataImport * metaimport = scope->GetMetaImport(compiler());
    WCHAR propnameText[MAX_IDENT_SIZE];     // name of property
    ULONG cchPropnameText;
    DWORD flags;
    PCCOR_SIGNATURE signature;
    ULONG cbSignature;
    PNAME name;
    PROPSYM *sym;
    mdToken tokenSetter, tokenGetter;
    METHSYM * methSet, * methGet;

    if (TypeFromToken(tokenProperty) != mdtProperty || tokenProperty == mdPropertyNil) {
        return;
    }

    // Get property properties.
    CheckHR(metaimport->GetPropertyProps(
        tokenProperty,                                  // The method for which to get props.
        NULL,                                               // Put method's class here.
        propnameText, lengthof(propnameText), &cchPropnameText,         // Method name
        & flags,                                            // Put flags here.
        & signature, & cbSignature,                             // Method signature
                NULL, NULL, NULL,                                               // Default value
                & tokenSetter, & tokenGetter,                                   // Setter, getter,
                NULL, 0, NULL), scope);                                     // Other methods
    CheckTruncation(cchPropnameText, lengthof(propnameText), scope);

//    BASSERT(lstrcmpW(propnameText, L"ItemByIndex"));


    // In the compiler, if the name contains a '.', then we won't be able to access if,
    // and its quite probabaly a explicit property implementation. Just don't import it. 
    if (wcschr(propnameText, L'.') != NULL)
        return;

    // Get name.
    if (cchPropnameText <= 1)
        return;
    name = compiler()->namemgr->AddString(propnameText, cchPropnameText - 1);

    //
    // Import all the attributes we are interested in at compile time
    //
    IMPORTED_CUSTOM_ATTRIBUTES attributes;
    ImportCustomAttributes(scope, NULL, &attributes, tokenProperty); 

    // Declare a property. Default properties with >0 args are actually indexers.
    methSet = methGet = NULL;
    if (tokenGetter != mdMethodDefNil) {
        methGet = FindMethodDef(parent, tokenGetter);
        ImportMethodProps(methGet);
    }
    if (tokenSetter != mdMethodDefNil) {
        methSet = FindMethodDef(parent, tokenSetter);
        ImportMethodProps(methSet);
    }

    // Make sure it's accessible.
    if (!methGet && !methSet)
        return;

    if (SigPeekUncompressData(scope, signature + 1, signature + cbSignature) > 0 && (name == defaultMemberName || 
         (methGet && methGet->name == defaultMemberName) ||
         (methSet && methSet->name == defaultMemberName))) {
        sym = compiler()->getBSymmgr().CreateIndexer(name, parent, decl);
    } else {
        sym = compiler()->getBSymmgr().CreateProperty(name, parent, decl);
    }

    ASSERT(sym);
    sym->tokenImport = tokenProperty;

    // Set the method signature.
    ImportMethodOrPropSignature(tokenProperty, signature, signature + cbSignature, sym);

    // handle indexed properties
    if (sym->params->size > 0) {
        if (!sym->isIndexer()) {
            // non-default indexed property
            sym->setBogus(true);
        } else {
            for (int i = 0; i < sym->params->size; i++) {
                if (sym->params->Item(i)->isPARAMMODSYM()) {
                    // INDEXERS can't have ref or out parameters
                    sym->setBogus(true);
                    break;
                }
            }
        }
    }
    else
    {
        if (sym->isIndexer()) {
            sym->setBogus(true);
        }
    }

    // Find the accessor methods.
    if (methGet) {
        sym->methGet = methGet;

        //
        // check that the accessor is OK and it matches the property signature
        //
        if (methGet->getBogus() ||
            sym->retType != methGet->retType ||
            sym->params->size != methGet->params->size)
        {
            sym->setBogus(true);
        } else {
            if (methGet->params != sym->params) {
                // The parameters must be the same.
                // Although it is legal to allow the parameters to differ in ref/out-ness
                // all properties and indexers are bogus if they have ref or out parameters
                // so it doesn't matter
                sym->setBogus(true);
            }
            if (methGet->isParamArray)
                sym->isParamArray = true;
        }
    }

    if (methSet) {
        sym->methSet = methSet;

        //
        // check that the accessor is OK and it matches the property signature
        //
        if (methSet->getBogus() || 
            methSet->retType != compiler()->getBSymmgr().GetVoid() || 
            methSet->params->size != (sym->params->size + 1) ||
            sym->retType != methSet->params->Item(sym->params->size))
        {
            sym->setBogus(true);
        } else {
            // Can't do "if (methSet->params != sym->params)" because
            // of the set value argument
            if (!TypeArray::EqualRange(methSet->params, 0, sym->params, 0, sym->params->size)) {
                // The parameters must be the same.
                // Although it is legal to allow the parameters to differ in ref/out-ness
                // all properties and indexers are bogus if they have ref or out parameters
                // so it doesn't matter
                sym->setBogus(true);
            }

            // check for set only indexer with paramarray
            if (methSet->params->size > 1 && methSet->params->Item(methSet->params->size - 2)->isARRAYSYM() &&
                methSet->params->Item(methSet->params->size - 2)->asARRAYSYM()->rank == 1) {
                methSet->isParamArray = sym->isParamArray = !!IsParamArray(scope, methSet->tokenImport, methSet->params->size - 1);
            }
        }
    }

    // Set attributes of the property by synthesizing them from the accessors.
    // If no accessors, or accessors disagree, it is bogus.
    ASSERT(methSet || methGet);

    if (methGet) {
        // a property's accessibility is that of the more visible methysm.
        if (methSet)
            sym->SetAccess(methGet->GetAccess() > methSet->GetAccess() ? methGet->GetAccess() : methSet->GetAccess());
        else
            sym->SetAccess(methGet->GetAccess());
        sym->isStatic = methGet->isStatic;
        sym->isHideByName = methGet->isHideByName;
        if (methSet &&
            (methGet->isStatic != methSet->isStatic ||
                methGet->isHideByName != methSet->isHideByName || methSet->isParamArray != methGet->isParamArray))
        {
            sym->setBogus(true);
        }
    }
    else {
        ASSERT(methSet);
        sym->SetAccess(methSet->GetAccess());
        sym->isStatic = methSet->isStatic;
        sym->isHideByName = methSet->isHideByName;
    }

    ASSERT(ACC_PROTECTED > ACC_INTERNAL);
    if (methGet && methSet && sym->GetAccess() == ACC_PROTECTED && 
       (methGet->GetAccess() == ACC_INTERNAL || methSet->GetAccess() == ACC_INTERNAL))
    {
        // one of the accessors must be explicity more visible
        sym->setBogus(true);
    }

    //
    // only flag imported methods as accessors if the property is accesible
    //
    if (!sym->getBogus()) {
        if (sym->methGet) {
            sym->methGet->SetMethKind(MethodKind::PropAccessor);
            sym->methGet->SetProperty(sym);
        }
        if (sym->methSet) {
            sym->methSet->SetMethKind(MethodKind::PropAccessor);
            sym->methSet->SetProperty(sym);
        }
        ASSERT(!sym->useMethInstead);
    } else {
        sym->useMethInstead = (sym->methSet && !sym->methSet->getBogus()) || (sym->methGet && !sym->methGet->getBogus());
    }

    sym->SetDeprecated(attributes.isDeprecated, attributes.isDeprecatedError, attributes.deprecatedString);

    if (attributes.hasCLSattribute) {
        sym->hasCLSattribute = true;
        sym->isCLS = attributes.isCLS;
        if (methGet) {
            methGet->hasCLSattribute = true;
            methGet->isCLS = attributes.isCLS;
        }
        if (methSet) {
            methSet->hasCLSattribute = true;
            methSet->isCLS = attributes.isCLS;
        }
    }

    if (sym->IsDeprecated()) {
        if (methGet)
            methGet->CopyDeprecatedFrom(sym);
        if (methSet)
            methSet->CopyDeprecatedFrom(sym);
    }


}


/*
 * Import an eventdef.
 */
void IMPORTER::ImportEvent(PAGGSYM parent, PAGGDECLSYM decl, mdEvent tokenEvent)
{
    MODULESYM * scope = parent->GetModule();
    ASSERT(scope->GetMetaImport(compiler()));
    IMetaDataImport * metaimport = scope->GetMetaImport(compiler());
    WCHAR eventnameText[MAX_IDENT_SIZE];     // name of event
    ULONG cchEventnameText;
    PNAME name;
    EVENTSYM * event;
    DWORD flags;
    mdToken tokenEventType, tokenAdd, tokenRemove;
    PMETHSYM methAdd, methRemove;

    if (TypeFromToken(tokenEvent) != mdtEvent || tokenEvent == mdEventNil) {
        return;
    }

    // Get Event propertys
    CheckHR(metaimport->GetEventProps(
        tokenEvent,                         // [IN] event token 
        NULL,                               // [OUT] typedef containing the event declarion.    
        eventnameText,                      // [OUT] Event name 
        lengthof(eventnameText),            // [IN] the count of wchar of szEvent   
        &cchEventnameText,                  // [OUT] actual count of wchar for event's name 
        & flags,                            // [OUT] Event flags.   
        & tokenEventType,                   // [OUT] EventType class    
        & tokenAdd,                         // [OUT] AddOn method of the event  
        & tokenRemove,                      // [OUT] RemoveOn method of the event   
        NULL,                               // [OUT] Fire method of the event   
        NULL,                               // [OUT] other method of the event  
        0,                                  // [IN] size of rmdOtherMethod  
        NULL), scope);                      // [OUT] total number of other method of this event 
    CheckTruncation(cchEventnameText, lengthof(eventnameText), scope);

    //
    if (wcschr(eventnameText, L'.') != NULL)
        return;

    // Get name.
    if (cchEventnameText <= 1)
        return;
    name = compiler()->namemgr->AddString(eventnameText, cchEventnameText - 1);

    // Find the accessor methods. They must be present, and have a signature of void XXX(EventType handler);
    methAdd = methRemove = NULL;
    if (tokenAdd != mdMethodDefNil) {
        methAdd = FindMethodDef(parent, tokenAdd);
        ImportMethodProps(methAdd);
    }
    if (tokenRemove != mdMethodDefNil) {
        methRemove = FindMethodDef(parent, tokenRemove);
        ImportMethodProps(methRemove);
    }

    if (!methAdd && !methRemove)
        return;

    // Declare an event symbol.
    event = compiler()->getBSymmgr().CreateEvent(name, parent, decl);
    event->tokenImport = tokenEvent;

    // Get the event type.
    event->type = ResolveTypeRefOrSpec(scope, tokenEventType, parent->typeVarsAll);
    if (!event->type || !event->type->isAGGTYPESYM() && !event->type->isERRORSYM()) {
        event->setBogus(true);
        event->type = compiler()->getBSymmgr().GetErrorSym();
    }

    // Find the accessor methods. They must be present, and have a signature of void XXX(EventType handler);
    if (methAdd) {
        event->methAdd = methAdd;
        if (methAdd->params->size != 1 || methAdd->params->Item(0) != event->type || methAdd->retType != compiler()->getBSymmgr().GetVoid())
            event->setBogus(true);
    }
    if (methRemove) {
        event->methRemove = methRemove;
        if (methRemove->params->size != 1 || methRemove->params->Item(0) != event->type || methRemove->retType != compiler()->getBSymmgr().GetVoid())
            event->setBogus(true);
    }

    // Set attributes of the event by synthesizing from the accessors. If accessors disagree, it is bogus.
    ASSERT(methAdd || methRemove);
    if (methAdd && methRemove) {
        event->SetAccess(methAdd->GetAccess());
        event->isStatic = methAdd->isStatic;
        if (methRemove->GetAccess() != methAdd->GetAccess() || methRemove->isStatic != methAdd->isStatic ||
            !methRemove->isHideByName != !methAdd->isHideByName)
        {
            event->setBogus(true);
        }
    }
    else {
        event->SetAccess(methAdd ? methAdd->GetAccess() : methRemove->GetAccess());
        event->setBogus(true);
    }

    // If the event is OK, flags the accessors.
    if (!event->getBogus()) {
        event->methAdd->SetMethKind(MethodKind::EventAccessor);
        event->methAdd->SetEvent(event);
        event->methRemove->SetMethKind(MethodKind::EventAccessor);
        event->methRemove->SetEvent(event);
        ASSERT(!event->useMethInstead);
    } else {
        event->useMethInstead = ((event->methAdd && !event->methAdd->getBogus()) || (event->methRemove && !event->methRemove->getBogus()));
    }

    //
    // Import all the attributes we are interested in at compile time
    //
    IMPORTED_CUSTOM_ATTRIBUTES attributes;
    ImportCustomAttributes(scope, NULL, &attributes, tokenEvent);

    event->SetDeprecated(attributes.isDeprecated, attributes.isDeprecatedError, attributes.deprecatedString);

    if (attributes.hasCLSattribute) {
        event->hasCLSattribute = true;
        event->isCLS = attributes.isCLS;
    }

    if (event->IsDeprecated()) {
        if (methAdd)
            methAdd->CopyDeprecatedFrom(event);
        if (methRemove)
            methRemove->CopyDeprecatedFrom(event);
    }

}

/*
 * Given a methoddef token and a parent, find the corresponding symbol.
 * There are two possible strategies here. One, do a linear search over
 * all methods in the parent and match on the token. Two, get the
 * name of the method def and then do a regular lookup. We chose the
 * second, which should be faster, unless the metadata call to get
 * the name is really slow.
 */
PMETHSYM IMPORTER::FindMethodDef(PAGGSYM parent, mdMethodDef token)
{

    MODULESYM * scope = parent->GetModule();
    ASSERT(scope->GetMetaImport(compiler()));
    IMetaDataImport * metaimport = scope->GetMetaImport(compiler());
    WCHAR methodnameText[MAX_IDENT_SIZE];     // name of method
    ULONG cchMethodnameText;
    PNAME name;
    PMETHSYM sym;

    if (TypeFromToken(token) != mdtMethodDef || token == mdMethodDefNil) {
        return NULL;
    }

    // Get method properties.
    CheckHR(metaimport->GetMethodProps(
        token,                                          // The method for which to get props.
        NULL,                                               // Put method's class here.
        methodnameText, lengthof(methodnameText), &cchMethodnameText,   // Method name
        NULL,                                                   // Put flags here.
        NULL, NULL,                                             // Method signature
        NULL,                                                   // codeRVA
        NULL), scope);                                              // Impl. Flags
    CheckTruncation(cchMethodnameText, lengthof(methodnameText), scope);
    
    // Convert name to a NAME.
    if (cchMethodnameText <= 1)
        return NULL;
    name = compiler()->namemgr->LookupString(methodnameText, cchMethodnameText - 1);
    if (!name)
        return NULL;

    // Search the parent for methods with this name until one with a matching
    // token is found.
    sym = compiler()->getBSymmgr().LookupAggMember(name, parent, MASK_METHSYM)->asMETHSYM();
    while (sym) {
        // Found the correct one?
        if (sym->tokenImport == token)
            return sym;

        // Got to the next one with the same name.
        sym = compiler()->getBSymmgr().LookupNextSym(sym, parent, MASK_METHSYM)->asMETHSYM();
    }

    // No method with this name and token in the parent.
    return NULL;
}

void IMPORTER::GetBaseTokenAndFlags(AGGSYM *sym, AGGTYPESYM  **base, DWORD *flags)
{
    SETLOCATIONSTAGE(IMPORT);
    SETLOCATIONSYM(sym);

    ASSERT(TypeFromToken(sym->tokenImport) == mdtTypeDef);

    mdToken baseToken;

    // Get the meta-data import interface.
    IMetaDataImport * metaimport = sym->GetMetaImport(compiler());
    ASSERT(metaimport);

    CheckHR(metaimport->GetTypeDefProps(sym->tokenImport,
            NULL, 0, NULL,            // Type name
            flags,                    // Flags
            &baseToken                // Extends
           ), sym);

    if (!IsNilToken(baseToken))
        *base = ResolveBaseRef(sym->GetModule(), baseToken, sym, true);
    else
        *base = NULL;
}


void IMPORTER::GetEnumUnderlyingType(AGGSYM * agg, MODULESYM * scope)
{
    ASSERT(agg->IsEnum() && !agg->underlyingType);

    IMetaDataImport * metaimport = scope->GetMetaImport(compiler());
    HCORENUM corenum = 0;
    mdToken tokens[32];
    ULONG cTokens, iToken;

    do {
        // Get next batch of fields.
        CheckHR(metaimport->EnumFields(&corenum, agg->tokenImport, tokens, lengthof(tokens), &cTokens), agg);

        // Process each field.
        for (iToken = 0; iToken < cTokens; ++iToken) {
        
            if (TypeFromToken(tokens[iToken]) == mdtFieldDef) {

                DWORD flags;
                PCCOR_SIGNATURE signature;
                ULONG cbSignature;

                // Get properties of the field from metadata.
                CheckHR(metaimport->GetFieldProps(
                    tokens[iToken],                                         // The field for which to get props.
                    NULL,                                                   // Put field's class here.
                    NULL, NULL, NULL,                                       // Field name
                    & flags,                                                // Field flags
                    & signature, & cbSignature,                             // Field signature
                    NULL, NULL, NULL),                  // Field constant value
                            agg);

                // Enums are a bit special. Non-static fields serve only to record the
                // underlying integral type, and are otherwise ignored. Static fields are
                // enumerators and must be of the enum type. (We change other integral ones to the
                // enum type because it's probably what the emitting compiler meant.)
                if (!(flags & fdStatic)) {

                    // Import the type of the field.
                    PTYPESYM type;
                    bool dummy;
                    type = ImportFieldType(scope, signature, signature + cbSignature, &dummy, NULL);

                    // Assuming its an integral type, use it to set the
                    // enum base type.
                    if (type && type->isNumericType() && compiler()->clsDeclRec.ResolveInheritanceRec(type->getAggregate()) &&
                        type->fundType() <= FT_LASTINTEGRAL)
                    {
                        if (!agg->underlyingType) {
                            agg->underlyingType = type->asAGGTYPESYM();
                        }
                        else {
                            // Cannot have more than one non-static field used to determine underlying type
                            agg->setBogus(true);
                        }
                    }
                }
            }
        }
    } while (cTokens > 0);
    metaimport->CloseEnum(corenum);
}


/*
 * Resolves the inheritance hierarchy (and interfaces) for a type.
 */
void IMPORTER::ResolveInheritance(AGGSYM *agg)
{
    SETLOCATIONSTAGE(IMPORT);
    SETLOCATIONSYM(agg);

    IMetaDataImport * metaimport;
    DWORD flags;
    AGGTYPESYM *base = NULL;

    HCORENUM corenum;           // For enumerating tokens.
    mdToken tokens[32];
    ULONG cTokens, iToken;

    // CLSDREC::ResolveInheritanceRec should have set this. We should only be called by CLSDREC::ResolveInheritanceRec.
    ASSERT(agg->AggState() == AggState::ResolvingInheritance);

    // Get the meta-data import interface.
    MODULESYM * scope = agg->GetModule();
    metaimport = scope->GetMetaImport(compiler());
    ASSERT(metaimport);

    // Import base class. Set underlying type.
    ASSERT(!agg->baseClass);
    ASSERT(!agg->underlyingType);

    GetBaseTokenAndFlags(agg, &base, &flags);

    // The flags should match what we saw before....
    ASSERT(agg->GetAccess() == AccessFromTypeFlags(flags, agg->DeclOnly()->getInputFile()));

    if (base && !compiler()->clsDeclRec.ResolveInheritanceRec(base->getAggregate())) {
        // Detected a cycle
        if (base->getAggregate()->IsResolvingBaseClasses())
            compiler()->ErrorRef(NULL, ERR_ImportedCircularBase, base, agg);
        base->getAggregate()->setBogus(true);
        agg->setBogus(true);

        // Redirect to object.
        base = compiler()->GetReqPredefType(PT_OBJECT, false);
    }

    switch (agg->AggKind()) {
    case AggKind::Interface:
        ASSERT(IsTdInterface(flags));
        ASSERT(agg->isAbstract && !agg->isSealed);
        ASSERT(!base);
        break;

    case AggKind::Class:
        ASSERT(!agg->isAbstract == !(flags & tdAbstract));
        ASSERT(!agg->isSealed == !(flags & tdSealed));
        ASSERT(!base || !base->isPredefType(PT_VALUE) || agg->isPredefAgg(PT_ENUM));
        if (!base) {
            AGGSYM * obj = compiler()->GetReqPredefAgg(PT_OBJECT, false);
            if (agg != obj)
                base = obj->getThisType();
        }
        compiler()->SetBaseType(agg, base);
        break;

    case AggKind::Delegate:
        ASSERT(base && (base->isPredefType(PT_DELEGATE) || base->isPredefType(PT_MULTIDEL)));
        ASSERT(!agg->isAbstract == !(flags & tdAbstract));
        ASSERT(!agg->isSealed == !(flags & tdSealed));
        compiler()->SetBaseType(agg, base);

        // Note: we may morph this into a class later if we don't find the invoke method or ctor.
        // if it is marked abstract, then change it now since it is an invalid delegate
        if (agg->isAbstract)
            agg->SetAggKind(AggKind::Class);
        
        break;

    case AggKind::Struct:
        // If the enum has type parameters, we treat it as a struct.
        ASSERT(base && (base->isPredefType(PT_VALUE) || base->isPredefType(PT_ENUM) && agg->typeVarsThis->size > 0));
        ASSERT(!agg->isPredefAgg(PT_ENUM));
        ASSERT(!agg->isAbstract && agg->isSealed);
        compiler()->SetBaseType(agg, compiler()->GetReqPredefType(PT_VALUE, false));
        break;

    case AggKind::Enum:
        ASSERT(base && base->isPredefType(PT_ENUM));
        ASSERT(!agg->isAbstract && agg->isSealed);
        compiler()->SetBaseType(agg, compiler()->GetReqPredefType(PT_ENUM, false));

        GetEnumUnderlyingType(agg, scope);
        if (!agg->underlyingType) {
            // Treat it as a struct....
            agg->SetAggKind(AggKind::Struct);
        }
        break;

    default:
        break;
    }

    // Import interfaces.
    AGGTYPESYM * rgiface[8];
    int cifaceMax = lengthof(rgiface);
    AGGTYPESYM ** prgiface = rgiface;
    int ciface = 0;

    corenum = 0;
    do {
        // Get next batch of interfaces.
        CheckHR(metaimport->EnumInterfaceImpls(&corenum, agg->tokenImport, tokens, lengthof(tokens), &cTokens), agg);

        // Process each interface.
        for (iToken = 0; iToken < cTokens; ++iToken) {
            // Get the interface.
            AGGTYPESYM * symIface = ImportInterface(scope, tokens[iToken], agg);

            // Add to the interface list if interesting.
            if (symIface) {
                if (!compiler()->clsDeclRec.ResolveInheritanceRec(symIface->getAggregate())) {
                    if (base->getAggregate()->IsResolvingBaseClasses())
                        compiler()->ErrorRef(NULL, ERR_ImportedCircularBase, symIface->getAggregate(), agg);
                    continue;
                }

                if (!symIface->isInterfaceType()) {
                    continue;
                }

                if (ciface >= cifaceMax) {
                    ASSERT(ciface == cifaceMax);
                    int cifaceMaxNew = 2 * cifaceMax;
                    AGGTYPESYM ** prgifaceNew = STACK_ALLOC(AGGTYPESYM *, cifaceMaxNew);
                    memcpy(prgifaceNew, prgiface, cifaceMax * sizeof(AGGTYPESYM *));
                    prgiface = prgifaceNew;
                    cifaceMax = cifaceMaxNew;
                }
                ASSERT(ciface < cifaceMax);
                prgiface[ciface++] = symIface;
            }
        }
    } while (cTokens > 0);

    metaimport->CloseEnum(corenum);

    compiler()->SetIfaces(agg, prgiface, ciface);

    ASSERT(agg->AggState() == AggState::ResolvingInheritance);
    agg->SetAggState(AggState::Inheritance);
}


void IMPORTER::DefineBounds(PARENTSYM * sym)
{
    ASSERT(sym->isAGGSYM() || sym->isMETHSYM());

    TypeArray * typeVars;
    TypeArray * typeVarsOuter;
    TypeArray * typeVarsCls;
    TypeArray * typeVarsMeth;
    mdToken tokImport;
    class AGGSYM *agg;

    switch (sym->getKind()) {
    default:
        ASSERT(!"Bad SK in DefineBounds!");
        return;

    case SK_AGGSYM:
        agg = sym->asAGGSYM();
        
        // Advance the state.
        ASSERT(agg->HasResolvedBaseClasses());
        if (agg->AggState() >= AggState::Bounds) {
            VSFAIL("Why are we here?");
            return;
        }

        {
            AGGSYM * aggOuter = agg->GetOuterAgg();
            if (aggOuter)
                compiler()->EnsureState(aggOuter, AggState::Bounds);
        }

        agg->SetAggState(AggState::Bounds);

        typeVars = agg->typeVarsAll;
        if (typeVars->size == 0)
            return;
        typeVarsOuter = sym->parent->isAGGSYM() ? sym->parent->asAGGSYM()->typeVarsAll : BSYMMGR::EmptyTypeArray();
        ASSERT(typeVarsOuter && typeVarsOuter->size == typeVars->size - agg->typeVarsThis->size);
        typeVarsCls = typeVars;
        typeVarsMeth = NULL;
        tokImport = agg->tokenImport;
        break;

    case SK_METHSYM:
        typeVars = sym->asMETHSYM()->typeVars;
        if (typeVars->size == 0)
            return;
        typeVarsOuter = BSYMMGR::EmptyTypeArray();
        typeVarsCls = sym->asMETHSYM()->getClass()->typeVarsAll;
        typeVarsMeth = typeVars;
        tokImport = sym->asMETHSYM()->tokenImport;
        break;
    }
    ASSERT(tokImport);

    int cvar = typeVars->size;
    ASSERT(cvar > 0);

    if (!sym->hasBogus() || !sym->checkBogus()) {
        MODULESYM * scope = sym->GetModule();
        IMetaDataImport2 * metaimport = scope->GetMetaImportV2(compiler());
        ASSERT(metaimport);


        mdGenericParam * prgtokVars = STACK_ALLOC_ZERO(mdGenericParam, cvar);
        HCORENUM enumVars = 0;
        LONG ctok = 0;

        // Get the tokens.
        CheckHR(metaimport->EnumGenericParams(&enumVars, tokImport, prgtokVars, cvar, (ULONG *)&ctok), scope->getInputFile());
        metaimport->CloseEnum(enumVars);

        ASSERT(ctok == cvar);

        TYPESYM * rgtypeBnd[8];
        mdGenericParamConstraint rgtokCon[8];
        ASSERT(lengthof(rgtypeBnd) == lengthof(rgtokCon));
        int cbndMax = lengthof(rgtokCon);
        TYPESYM ** prgtypeBnd = rgtypeBnd;
        mdGenericParamConstraint * prgtokCon = rgtokCon;

        // Process each type variable.
        for (int itok = 0; itok < cvar; ++itok) {
            LONG ivar;
            DWORD flags;
            mdToken tokPar;
            WCHAR rgchName[MAX_FULLNAME_SIZE];
            ULONG cchName;

            CheckHR(metaimport->GetGenericParamProps(prgtokVars[itok], (ULONG *)&ivar, &flags, &tokPar, NULL,
                    rgchName, lengthof(rgchName), &cchName),
                scope->getInputFile());
            ASSERT(tokPar == tokImport);


            ASSERT(0 <= ivar && ivar < cvar);
            TYVARSYM * var = typeVars->ItemAsTYVARSYM(ivar);

            TypeArray * bnds;
            int ctype;

            // Get the tokens for the bounds.
            LONG cbnd = 0;
            HCORENUM enumCons = 0;

            CheckHR(metaimport->EnumGenericParamConstraints(&enumCons, prgtokVars[itok], NULL, 0, (ULONG *)&cbnd), scope->getInputFile());
            CheckHR(metaimport->CountEnum(enumCons, (ULONG *)&cbnd), scope->getInputFile());

            if (cbnd > 0) {
                if (cbnd > cbndMax) {
                    cbndMax += cbndMax;
                    if (cbndMax < cbnd)
                        cbndMax = cbnd;
                    prgtypeBnd = STACK_ALLOC_ZERO(TYPESYM *, cbndMax);
                    prgtokCon = STACK_ALLOC_ZERO(mdGenericParamConstraint, cbndMax);
                }
                CheckHR(metaimport->EnumGenericParamConstraints(&enumCons, prgtokVars[itok], prgtokCon, cbndMax, (ULONG *)&ctok), scope->getInputFile());
                ASSERT(ctok == cbnd);
            }
            metaimport->CloseEnum(enumCons);

            ctype = 0;
            for (int ibnd = 0; ibnd < cbnd; ibnd++) {
                mdGenericParam tokVar;
                mdToken tokBnd;

                CheckHR(metaimport->GetGenericParamConstraintProps(prgtokCon[ibnd], &tokVar, &tokBnd), scope->getInputFile());

                TYPESYM * type = ResolveTypeRefOrSpec(scope, tokBnd, typeVarsCls, typeVarsMeth);
                if (!type) {
                    sym->setBogus(true);
                    goto LBogus;
                }
                if (!type->GetNakedType()->isTYVARSYM())
                    compiler()->EnsureState(type, AggState::Inheritance);
                // Don't keep object in the list.
                if (!type->isPredefType(PT_OBJECT))
                    prgtypeBnd[ctype++] = type;
            }

            bnds = compiler()->getBSymmgr().AllocParams(ctype, (TYPESYM **)prgtypeBnd);

            SpecConsEnum cons = SpecCons::None;
            if (flags & gpReferenceTypeConstraint)
                cons |= SpecCons::Ref;
            if (flags & gpNotNullableValueTypeConstraint)
                cons |= SpecCons::Val;
            if (!(cons & SpecCons::Val) && (flags & gpDefaultConstructorConstraint))
                cons |= SpecCons::New;

            if (ivar < typeVarsOuter->size) {
                // Verify that the constraints match those on the outer type!
                ASSERT(var->FResolved());
                if ((uint)cons != var->cons || bnds->size != var->GetBnds()->size) {
                    sym->setBogus(true);
                    goto LBogus;
                }
                TypeArray * bndsOuter = var->GetBnds();
                if (bndsOuter != bnds) {
                    for (int i = 0; i < bnds->size; i++) {
                        if (!bndsOuter->Contains(bnds->Item(i))) {
                            sym->setBogus(true);
                            goto LBogus;
                        }
                        if (!bnds->Contains(bndsOuter->Item(i))) {
                            sym->setBogus(true);
                            goto LBogus;
                        }
                    }
                }
            }
            else {
                ASSERT(!var->GetBnds());
                compiler()->SetBounds(var, bnds);
                var->cons = cons;

                // If the interface list is bogus the sym will get set to bogus.
                if (sym->hasBogus() && sym->checkBogus())
                    goto LBogus;
            }
        }
    }
    else {
LBogus:
        // The parent is already known to be bogus, so just set all the constraints to empty.
        for (int i = typeVarsOuter->size; i < cvar; i++) {
            TYVARSYM * var = typeVars->ItemAsTYVARSYM(i);
            if (!var->GetBnds())
                compiler()->SetBounds(var, BSYMMGR::EmptyTypeArray());
        }
    }

    // Resolve all the bounds.
    for (int i = typeVarsOuter->size; i < cvar; i++) {
        TYVARSYM * var = typeVars->ItemAsTYVARSYM(i);
        VSVERIFY(compiler()->ResolveBounds(var, true), "ResolveBounds failed!");
        ASSERT(var->FResolved());
    }
}


/*
 * Given a type that previous imported via ImportAllTypes, make it
 * "declared" by importing all of its members and declaring
 * its base classes/interfaces.
 */
void IMPORTER::DefineImportedType(AGGSYM * sym)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::DefineMembers);

    SETLOCATIONSTAGE(IMPORT);
    SETLOCATIONSYM(sym);

    ASSERT(!COMPILER::IsRegString(sym->name->text, L"DefineImportedType"));

    if (sym->IsDefined())
        return;

#ifdef DEBUG
    compiler()->haveDefinedAnyType = true;
#endif


    if (!sym->HasResolvedBaseClasses()) {
        VSVERIFY(compiler()->clsDeclRec.ResolveInheritanceRec(sym), "ResolveInheritanceRec failed in DefineImportedType!");
        ASSERT(sym->HasResolvedBaseClasses());
    }

    if (sym->AggState() < AggState::Bounds) {
        DefineBounds(sym);
        ASSERT(sym->AggState() >= AggState::Bounds);
    }

    // Bogus types aren't handled at all -- don't import anything about them.
    if (sym->getBogus()) {
        sym->SetAggState(AggState::DefinedMembers);
        return;
    }

    // ResolveInheritance determined the underlyingType of an enum.
    ASSERT(!sym->IsEnum() || sym->underlyingType);

    ASSERT(sym->AggState() < AggState::DefiningMembers);
    sym->SetAggState(AggState::DefiningMembers);

    // No point importing anything for inaccessible types - except structs.
    // We need their fields for managed/unmanaged and checking recursion.
    if (sym->isMDPrivate && !sym->IsStruct()) {
        ASSERT(sym->AggState() == AggState::DefiningMembers);
        sym->SetAggState(AggState::DefinedMembers);
        return;
    }

    // Get the meta-data import interface.
    AGGDECLSYM * decl = sym->DeclOnly();
    MODULESYM * scope = sym->GetModule();
    INFILESYM * inputfile = decl->getInputFile();
    IMetaDataImport * metaimport = scope->GetMetaImport(compiler());
    ASSERT(metaimport);

    HCORENUM corenum;           // For enumerating tokens.
    mdToken tokens[32];
    ULONG cTokens, iToken;
    IMPORTED_CUSTOM_ATTRIBUTES attributes;

    //
    // Check to see if the class has any explicit interface impls
    //
    if (sym->IsStruct()) {
        corenum = 0;
        mdToken tokensBody[1];
        mdToken tokensDecl[1];
        // Get next batch of fields.
        CheckHR(metaimport->EnumMethodImpls(&corenum, sym->tokenImport, tokensBody, tokensDecl, lengthof(tokensBody), &cTokens), inputfile);

        if (cTokens) {
            sym->hasExplicitImpl = true;
        }
        metaimport->CloseEnum(corenum);
    }

    //
    // Import all the attributes we are interested in at compile time
    //
    ImportCustomAttributes(scope, NULL, &attributes, sym->tokenImport); 


    sym->setBogus(attributes.hasRequiredAttribute || (sym->hasBogus() && sym->getBogus()));
    if (!sym->IsInterface()) {
        corenum = 0;
        do {
            // Get next batch of fields.
            CheckHR(metaimport->EnumFields(&corenum, sym->tokenImport, tokens, lengthof(tokens), &cTokens), inputfile);

            // Process each field.
            for (iToken = 0; iToken < cTokens; ++iToken) {
                ImportField(sym, decl, tokens[iToken]);
            }
        } while (cTokens > 0);
        metaimport->CloseEnum(corenum);
    } else {
        if (sym->isComImport && attributes.CoClassName) {
            ASSERT(sym->underlyingType == NULL);
            sym->comImportCoClass = attributes.CoClassName;
        }
    }
    sym->hasLinkDemand = attributes.hasLinkDemand;

    // Import methods and method impls. Enums don't have them.
    if (!sym->IsEnum() && !sym->isMDPrivate) {
        corenum = 0;
        do {
            // Get next batch of methods.
            CheckHR(metaimport->EnumMethods(&corenum, sym->tokenImport, tokens, lengthof(tokens), &cTokens), inputfile);

            // Process each method.
            for (iToken = 0; iToken < cTokens; ++iToken) {
                ImportMethod(sym, decl, tokens[iToken]);
            }
        } while (cTokens > 0);
        metaimport->CloseEnum(corenum);

        // get method impls
        corenum = 0;

        mdToken tokensDecl[32];
        do {
            // get next batch of methodimpls
            CheckHR(metaimport->EnumMethodImpls(&corenum, sym->tokenImport, tokens, tokensDecl, lengthof(tokens), &cTokens), inputfile);

            for (iToken = 0; iToken < cTokens; iToken += 1) {
                // find the type token of the impl's decl
                mdToken tokenClassOfImpl;
                if (TypeFromToken(tokensDecl[iToken]) == mdtMethodDef && tokensDecl[iToken] != mdMethodDefNil)
                {
                    // Get method properties.
                    CheckHR(metaimport->GetMethodProps(
                        tokensDecl[iToken],                             // The method for which to get props.
                        &tokenClassOfImpl,                              // Put method's class here.
                        0,  0, 0,                                       // Method name
                        NULL,                                                   // Put flags here.
                        NULL, NULL,                                             // Method signature
                        NULL,                                                   // codeRVA
                        NULL), inputfile);                                              // Impl. Flags
                } else if (TypeFromToken(tokensDecl[iToken]) == mdtMemberRef && tokensDecl[iToken] != mdMemberRefNil) {
                    CheckHR(metaimport->GetMemberRefProps(tokensDecl[iToken], &tokenClassOfImpl, 0, 0, 0, 0, 0), inputfile);
                } else {
                    // Unknown method impl kind .. ignore it below
                    tokenClassOfImpl = 0;
                }

                // a method impl to a class type means the method must be an override
                TYPESYM * typeImpl = ResolveTypeRefOrDef(scope, tokenClassOfImpl, NULL);
                if (typeImpl && typeImpl->isAGGTYPESYM() && !typeImpl->isInterfaceType()) {
                    METHSYM *method;
                    method = FindMethodDef(sym, tokens[iToken]);
                    if (method && method->isVirtual)
                        method->isOverride = true;
                }
            }
        } while (cTokens > 0);
        metaimport->CloseEnum(corenum);

        // Import properties. These must be done after methods, because properties refer to methods.
        PNAME defaultMemberName = NULL;

        if (attributes.defaultMember)
            defaultMemberName = compiler()->namemgr->AddString(attributes.defaultMember);

        corenum = 0;
        do {
            // Get next batch of properties.
            CheckHR(metaimport->EnumProperties(&corenum, sym->tokenImport, tokens, lengthof(tokens), &cTokens), inputfile);

            // Process each property.
            for (iToken = 0; iToken < cTokens; ++iToken) {
                ImportProperty(sym, decl, tokens[iToken], defaultMemberName);
            }
        } while (cTokens > 0);
        metaimport->CloseEnum(corenum);

        // Import events. These must be done after methods, because events refer to methods.
        corenum = 0;
        do {
            // Get next batch of events.
            CheckHR(metaimport->EnumEvents(&corenum, sym->tokenImport, tokens, lengthof(tokens), &cTokens), inputfile);

            // Process each event.
            for (iToken = 0; iToken < cTokens; ++iToken) {
                ImportEvent(sym, decl, tokens[iToken]);
            }
        } while (cTokens > 0);
        metaimport->CloseEnum(corenum);
    }

    if (sym->IsDelegate()) {
        ASSERT(sym->baseClass && !sym->isAbstract &&
            (sym->baseClass->isPredefType(PT_DELEGATE) ||
                sym->baseClass->isPredefType(PT_MULTIDEL)));

        // We have something which looks like a delegate. Make sure that it has the correct
        // constructor and an invoke method.

        METHSYM * methInvoke = NULL;
        METHSYM * methCtor = NULL;

        FOREACHCHILD(sym, child)
            switch(child->getKind()) {
            case SK_METHSYM:
            {
                // check for constructor or invoke
                METHSYM * method = child->asMETHSYM();

                if (method->name == compiler()->namemgr->GetPredefName(PN_INVOKE)) {
                    if (!methInvoke && method->GetAccess() == ACC_PUBLIC) {
                        methInvoke = method;
                        continue;
                    }
                } else if (method->isCtor()) {
                    if (!methCtor &&
                        method->GetAccess() == ACC_PUBLIC &&
                        method->params->size == 2 &&
                        method->params->Item(0)->isPredefType(PT_OBJECT) &&
                        (method->params->Item(1)->isPredefType(PT_UINTPTR) ||
                            method->params->Item(1)->isPredefType(PT_INTPTR))) 
                    {
                        methCtor = method;
                        continue;
                    }
                }

                // found a non-delegatelike method
                break;
            }

            // other members are ignored
            default:
                break;
            }
        ENDFOREACHCHILD

        if (methInvoke && methCtor) {
            methInvoke->SetMethKind(MethodKind::Invoke);
            if (methInvoke->getBogus() || methCtor->getBogus())
                sym->setBogus(true);
        }
        else {
            // Treat it as a class.
            sym->SetAggKind(AggKind::Class);
        }
    }

    if (sym->baseClass) {
        // if there is a sealed type in the inheritance heirarchy we treat it as a bogus type in order to get an error at compile time if this is used.  If we allow it, the frameworks
        // will throw a TypeLoadException at runtime.
        if (sym->baseClass->getAggregate()->isSealed)
            sym->setBogus(true);

        sym->hasConversion |= sym->baseClass->getAggregate()->hasConversion;
    }

    sym->SetDeprecated(attributes.isDeprecated, attributes.isDeprecatedError, attributes.deprecatedString);

    if (attributes.hasCLSattribute) {
        sym->hasCLSattribute = true;
        sym->isCLS = attributes.isCLS;
    }

    if (sym->isAttribute)  {
        sym->conditionalSymbols = attributes.conditionalSymbols;

        // don't overwrite default attributeClass for security attributes
        if (attributes.attributeKind != 0) {
            sym->attributeClass = attributes.attributeKind;
            sym->isMultipleAttribute = attributes.allowMultiple;
        } else {
            AGGSYM *base = sym->baseClass->getAggregate();
            compiler()->EnsureState(base, AggState::DefinedMembers);
            sym->attributeClass = base->attributeClass;
            sym->isMultipleAttribute = base->isMultipleAttribute;
        }
    }

    ASSERT(sym->AggState() == AggState::DefiningMembers);
    sym->SetAggState(AggState::DefinedMembers);
}


class MetadataEnumerator
{
public:
    MetadataEnumerator(COMPILER *compiler, AGGSYM *sym) :
        compiler(compiler),
        sym(sym),
        corenum(0)
    {
        // check for reordering of instance fields
        ASSERT(sym->IsDefined());

        // Get the meta-data import interface.
        metaimport = sym->GetMetaImport(compiler);
        ASSERT(metaimport);
    }
    ~MetadataEnumerator()
    {
        metaimport->CloseEnum(corenum);
    }

    bool IsDone() { return cTokens == 0; }
    mdToken CurrentToken() { ASSERT(!IsDone()); return tokens[iToken]; }
    void Next() 
    {
        ASSERT (!IsDone());
        iToken += 1;
        if (iToken >= cTokens) {
            ReadTokensInternal();
        }
    }

    void Abort()
    {
        cTokens = 0;
    }

protected:
    virtual void ReadTokens() = 0;
            
    void ReadTokensInternal()
    {
        ReadTokens();
        iToken = 0;
    }
    
    COMPILER *compiler;
    AGGSYM *sym;
    
    IMetaDataImport * metaimport;
    PINFILESYM inputfile;

    HCORENUM corenum;           // For enumerating tokens.
    mdToken tokens[32];
    ULONG cTokens, iToken;
};

// If meth is NULL, we produce a list of the standard method type variables.
// Creates the type variables but doesn't set their bounds.
void IMPORTER::DefineMethodTypeFormals(mdToken methodToken, METHSYM * meth, SYM * symCtx, TypeArray ** ptypeVars)
{
    ASSERT(symCtx);

    MODULESYM * scope = symCtx->GetModule();
    IMetaDataImport2 * metaimport = scope->GetMetaImportV2(compiler());


    ASSERT(metaimport);

    // Set the type variables for the sym, but not the bounds.
    HCORENUM enumVars = 0;
    LONG cvar = 0;

    CheckHR(metaimport->EnumGenericParams(&enumVars, methodToken, NULL, 0, (ULONG *)&cvar), scope->getInputFile());
    CheckHR(metaimport->CountEnum(enumVars, (ULONG *)&cvar), scope->getInputFile());

    ASSERT(cvar >= 0);

    if (cvar == 0) {
        metaimport->CloseEnum(enumVars);
        *ptypeVars = BSYMMGR::EmptyTypeArray();
        return;
    }

    TYVARSYM ** prgvar = STACK_ALLOC_ZERO(TYVARSYM *, cvar);

    if (!meth) {
        metaimport->CloseEnum(enumVars);

        // Just use the standard method type variables.
        for (int ivar = 0; ivar < cvar; ivar++) {
            prgvar[ivar] = compiler()->getBSymmgr().GetStdMethTypeVar(ivar);
        }
        *ptypeVars = compiler()->getBSymmgr().AllocParams(cvar, (PTYPESYM *)prgvar);
        return;
    }

    // Get the tokens.
    mdGenericParam * prgtokVars = STACK_ALLOC_ZERO(mdGenericParam, cvar);
    LONG ctok;
    bool fBadList = false;

    CheckHR(metaimport->EnumGenericParams(&enumVars, methodToken, prgtokVars, cvar, (ULONG *)&ctok), scope->getInputFile());
    metaimport->CloseEnum(enumVars);

    ASSERT(ctok == cvar);
    WCHAR rgchName[MAX_FULLNAME_SIZE];

    for (int itok = 0; itok < cvar; ++itok) {
        LONG ivar;
        DWORD flags;
        mdToken tokPar;
        ULONG cchName;

        CheckHR(metaimport->GetGenericParamProps(
                    prgtokVars[itok],
                    (ULONG *)&ivar,
                    &flags,
                    &tokPar,
                    NULL,
                    rgchName,
                    lengthof(rgchName),
                    &cchName),
            scope->getInputFile());

        if (ivar < 0 || ivar >= cvar || prgvar[ivar]) {
            // Bad or duplicate index.
            fBadList = true;
            continue;
        }

        NAME * name = compiler()->namemgr->AddString(rgchName);
        TYVARSYM * var = compiler()->getBSymmgr().CreateTyVar(name, meth);
        ASSERT(var->isMethTyVar);
        var->SetAccess(ACC_PRIVATE);
        var->indexTotal = (short)ivar;
        var->index = (short)ivar;
        var->parseTree = NULL;
        prgvar[ivar] = var;
    }

    if (fBadList) {
        meth->setBogus(true);

        // Fill in blank slots.
        for (int ivar = 0; ivar < cvar; ivar++) {
            if (prgvar[ivar])
                continue;
            StringCchPrintfW(rgchName, lengthof(rgchName), L"__TyVar%d", ivar);
            NAME * name = compiler()->namemgr->AddString(rgchName);
            TYVARSYM * var = compiler()->getBSymmgr().CreateTyVar(name, meth);
            ASSERT(var->isMethTyVar);
            var->SetAccess(ACC_PRIVATE);
            var->indexTotal = (short)ivar;
            var->index = (short)ivar;
            var->parseTree = NULL;
            prgvar[ivar] = var;
        }
    }

    // Create the TypeArray.
    *ptypeVars = compiler()->getBSymmgr().AllocParams(cvar, (PTYPESYM *)prgvar);
}

/*
 * Opens an Assembly and adds all metadata files, but does
 * not add an AssemblyRef yet.
 * Returns true iff we imported the Assembly
 * Returns false if we had to 'suck-in' the file
 */
bool IMPORTER::OpenAssembly(PINFILESYM infile)
{
    ASSERT(!infile->assemimport && IsNilToken(infile->idLocalAssembly));

    mdAssembly tkAssem;

    DWORD cscope;

    CheckHR(FTL_MetadataCantOpenFile,
        compiler()->linker->ImportFile(infile->name->text, NULL, FALSE, &infile->mdImpFile, &infile->assemimport, &cscope),
        infile);

    infile->cscope = cscope;

    if (infile->assemimport == NULL) {
        ASSERT(infile->cscope == 1);
        if (!infile->isAddedModule) {
            // This is not an assembly and it wasn't added via addmodule
            compiler()->Error( NULL, ERR_ImportNonAssembly, infile->name->text);
            infile->setBogus(true);
            infile->isAddedModule = true;
            return false;
        }

        // importing into the current assembly
        ASSERT(infile->GetAssemblyID() == kaidThisAssembly);
        ASSERT(infile->InAlias(kaidThisAssembly));
        ASSERT(infile->InAlias(kaidGlobal));

        infile->idLocalAssembly = mdTokenNil;
    } else {
        if (infile->isAddedModule) {
            // This is an assembly and it was added via addmodule!
            compiler()->Error(NULL, ERR_AddModuleAssembly, infile->name->text);
            infile->setBogus(true);
            return false;
        }

        //
        // assign this imported assembly a new assembly index
        // which we'll use until we begin emitting
        //
        ASSERT(infile->GetAssemblyID() != kaidThisAssembly);

        CheckHR(infile->assemimport->GetAssemblyFromScope(&tkAssem), infile);

        if (compiler()->cmdHost) {
            // Walk all of the files in a possibly multi-file assembly
            // and report them to the host

            HCORENUM enumFiles;
            mdFile filedefs[16];
            ULONG cFiledefs, iFiledef;
            WCHAR *FileName = NULL, *filepart = NULL;
            DWORD len, cchName;
            HRESULT hr;

            cchName = (DWORD)wcslen(infile->name->text) + MAX_PATH;
            FileName = STACK_ALLOC(WCHAR, cchName);
            StringCchCopyW (FileName, cchName, infile->name->text);
            filepart = wcsrchr(FileName, L'\\');
            if (filepart)
                filepart++;
            else
                filepart = FileName;

            len = cchName - (DWORD)(filepart - FileName);
    
            // Enumeration all the Files in this assembly.
            enumFiles= 0;
            do {

                // Get next batch of files.
                hr = infile->assemimport->EnumFiles(&enumFiles, filedefs, lengthof(filedefs), &cFiledefs);
                if (FAILED(hr))
                    break;

                // Process each file.
                for (iFiledef = 0; iFiledef < cFiledefs && SUCCEEDED(hr); ++iFiledef) {
                    hr = infile->assemimport->GetFileProps( filedefs[iFiledef], filepart, len, &cchName, NULL, NULL, NULL);
                    if (FAILED(hr))
                        continue;
                    compiler()->NotifyHostOfMetadataFile(FileName);

                }
            } while (cFiledefs > 0 && SUCCEEDED(hr));
    
            infile->assemimport->CloseEnum(enumFiles);
        }
    }

    return true;
}

PCWSTR IMPORTER::GetAssemblyName(SYM * sym)
{
    INFILESYM * infile = NULL;
    CComPtr<IMetaDataAssemblyImport> assemimport;

    // Strip off ARRAYSYM, PTRSYM, etc.
    if (sym->isTYPESYM())
        sym = sym->asTYPESYM()->GetNakedType();

    // For type variables, just look at the parent.
    if (sym->isTYVARSYM())
        sym = sym->parent;

    // An AGGSYM can't be used to find an input file if it's defined in source. But
    // we don't care in that case anyway, so luckily not a problem
    if ((!sym->isAGGSYM() || !sym->asAGGSYM()->isSource) && (!sym->isAGGTYPESYM() || !sym->asAGGTYPESYM()->getAggregate()->isSource))
        infile = sym->getInputFile();

    if (!infile 
        || infile->GetAssemblyID() == kaidThisAssembly 
        )
    {
        return NULL;
    }

    if (!infile->assemblyName) {
        //
        // build the assembly name :
        //
        ASSERT(infile->assemimport != NULL);
        assemimport = infile->assemimport;

        CheckHR(GetAssemblyName(assemimport, &(infile->assemblyName), NULL, infile->assemblyVersion), infile);

    }

    return infile->assemblyName->text;
}

NAME * IMPORTER::GetAssemblyName(ImportScope & scopeSource, mdAssemblyRef tkAssemblyRef)
{
    IMetaDataAssemblyImport * assemimport = scopeSource.GetAssemblyImport();

    // get required sizes for stuff
    ULONG cbOriginator;
    ULONG cchName;
    DWORD flags;
    ASSEMBLYMETADATA data;
    MEM_SET_ZERO(data);
    CheckHR(assemimport->GetAssemblyRefProps(
        tkAssemblyRef, 
        NULL, &cbOriginator,// originator
        NULL, 0, &cchName,  // name
        &data,              // data
        NULL, NULL,         // hash
        &flags), scopeSource);

    //
    // Stack allocate and get the actual values
    //
    LPCVOID pbOriginator;
    WCHAR *szName = STACK_ALLOC(WCHAR, cchName);
    data.szLocale = data.cbLocale ? STACK_ALLOC(WCHAR, data.cbLocale) : NULL;
    CheckHR(assemimport->GetAssemblyRefProps( 
        tkAssemblyRef,
        &pbOriginator, &cbOriginator,// originator
        szName, cchName, &cchName,  // name
        &data,                      // data
        NULL, NULL,                 // hash
        NULL), scopeSource);

    //
    // Now actually format and create the NAME
    //
    NAME * nameAssemblyRef;
    CheckHR(MakeAssemblyName(szName, cchName, data, (LPBYTE)pbOriginator, cbOriginator, flags, NULL, &nameAssemblyRef), scopeSource);

    return nameAssemblyRef;
}


HRESULT IMPORTER::GetAssemblyName(IMetaDataAssemblyImport * assemimport, NAME ** nameAsNAME, BSTR * nameAsBSTR, WORD* assemblyVersion)
{

    HRESULT HR;

    if (nameAsBSTR) {
        *nameAsBSTR = NULL;
    }
    if (nameAsNAME) {
        *nameAsNAME = NULL;
    }
    mdAssembly tkAsm;
    HR = assemimport->GetAssemblyFromScope( &tkAsm);
    if (FAILED(HR)) goto LERROR;

    if (TypeFromToken(tkAsm) != mdtAssembly || tkAsm == mdAssemblyNil) {
        HR = S_OK;
        goto LERROR;
    }

    //
    // get required sizes for stuff
    //
    ASSEMBLYMETADATA data;
    ULONG cbOriginator;
    ULONG cchName;
    DWORD flags;
    MEM_SET_ZERO(data);
    HR = assemimport->GetAssemblyProps( 
        tkAsm, 
        NULL, &cbOriginator,// originator
        NULL,               // hask alg
        NULL, 0, &cchName,  // name
        &data,              // data
        &flags);
    if (FAILED(HR)) goto LERROR;
    flags |= afPublicKey; // AssemblyDefs always have the full public key (not just the token) but they don't set this bit

    //
    // Stack allocate and get the actual values
    //
    LPCVOID pbOriginator;
    WCHAR *szName; szName = STACK_ALLOC(WCHAR, cchName);
    data.szLocale = data.cbLocale ? STACK_ALLOC(WCHAR, data.cbLocale) : NULL;
    HR = assemimport->GetAssemblyProps( 
        tkAsm, 
        &pbOriginator, &cbOriginator,// originator
        NULL,               // hask alg
        szName, cchName, &cchName,  // name
        &data,              // data
        NULL);
    if (FAILED(HR)) goto LERROR;


    HR = MakeAssemblyName(szName, cchName, data, (LPBYTE)pbOriginator, cbOriginator, flags, nameAsBSTR, nameAsNAME);

    if (SUCCEEDED(HR) && assemblyVersion != NULL) {
        assemblyVersion[0] = data.usMajorVersion;
        assemblyVersion[1] = data.usMinorVersion;
        assemblyVersion[2] = data.usBuildNumber;
        assemblyVersion[3] = data.usRevisionNumber;
    }

LERROR:

    return HR;
}

// NOTE: cchName INCLUDES the null-terminator!
HRESULT IMPORTER::MakeAssemblyName(PCWSTR szName, ULONG cchName, const ASSEMBLYMETADATA & data, LPBYTE pbPublicKey, const ULONG cbPublicKey, const DWORD dwFlags, BSTR * nameAsBSTR, NAME ** nameAsNAME)
{
    HRESULT hr;
    CComPtr<IAssemblyName> pan;

    if (FAILED(hr = InitFusionAPIs()))
        return hr;

    ASSERT(wcslen(szName) + 1 == cchName);
    hr = m_pfnCreateAssemblyNameObject( &pan, NULL, 0, NULL);
    if (FAILED(hr))
        goto LERROR;

    // Name
    hr = pan->SetProperty( ASM_NAME_NAME, (LPVOID)szName, cchName * sizeof(WCHAR));
    if (FAILED(hr))
        goto LERROR;

    // Version
    hr = pan->SetProperty( ASM_NAME_MAJOR_VERSION, (LPVOID)&data.usMajorVersion, sizeof(data.usMajorVersion));
    if (FAILED(hr))
        goto LERROR;
    hr = pan->SetProperty( ASM_NAME_MINOR_VERSION, (LPVOID)&data.usMinorVersion, sizeof(data.usMinorVersion));
    if (FAILED(hr))
        goto LERROR;
    hr = pan->SetProperty( ASM_NAME_BUILD_NUMBER, (LPVOID)&data.usBuildNumber, sizeof(data.usBuildNumber));
    if (FAILED(hr))
        goto LERROR;
    hr = pan->SetProperty( ASM_NAME_REVISION_NUMBER, (LPVOID)&data.usRevisionNumber, sizeof(data.usRevisionNumber));
    if (FAILED(hr))
        goto LERROR;

    // Culture
    LPVOID szLocale; szLocale = data.szLocale;
    // NOTE: ASSEMBLYMETADATA.cbLocale seems like it is a count of bytes, but it is improplerly named
    // it really is a count of characters and should be named cchLocale!
    DWORD cchLocale; cchLocale = data.cbLocale;
    if (data.szLocale == NULL) {
        ASSERT(data.cbLocale == 0);
        szLocale = L"";
        cchLocale = lengthof(L"");
    }
    hr = pan->SetProperty( ASM_NAME_CULTURE, szLocale, cchLocale * sizeof(WCHAR));
    if (FAILED(hr))
        goto LERROR;

    // Public Key (or Token)
    if (cbPublicKey == 0)
        hr = pan->SetProperty( ASM_NAME_NULL_PUBLIC_KEY_TOKEN, NULL, 0);
    else if (!(dwFlags & afPublicKey))
        hr = pan->SetProperty( ASM_NAME_PUBLIC_KEY_TOKEN, pbPublicKey, cbPublicKey);
    else
        hr = pan->SetProperty( ASM_NAME_PUBLIC_KEY, pbPublicKey, cbPublicKey);
    if (FAILED(hr))
        goto LERROR;

    // Retargetable?
    if (dwFlags & afRetargetable) {
        BOOL fRetargetable = TRUE;
        hr = pan->SetProperty( ASM_NAME_RETARGET, &fRetargetable, sizeof(fRetargetable));
        if (FAILED(hr))
            goto LERROR;
    }

    // Now get the name
    DWORD cchDisplayName; cchDisplayName = 0;
    hr = pan->GetDisplayName( NULL, &cchDisplayName, 0); // 0 == default == ASM_DISPLAYF_VERSION | ASM_DISPLAYF_CULTURE | ASM_DISPLAYF_PUBLIC_KEY_TOKEN | ASM_DISPLAYF_RETARGET
    if (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        ASSERT(FAILED(hr));
        goto LERROR;
    }
    WCHAR * szDisplayName; szDisplayName = STACK_ALLOC(WCHAR, cchDisplayName);
    hr = pan->GetDisplayName( szDisplayName, &cchDisplayName, 0);
    if (FAILED(hr)) {
        ASSERT(hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));
        goto LERROR;
    }

    if (nameAsNAME) {
        *nameAsNAME = compiler()->namemgr->AddString(szDisplayName);
    }

    if (nameAsBSTR) {
        hr = (*nameAsBSTR = SysAllocString(szDisplayName)) ? S_OK : E_OUTOFMEMORY;
    }

LERROR:

    return hr;
}


/***************************************************************************************************
    Looks for a value in the (module, tok) -> sym cache.
    For the EE, if m_fIgnoreCache is clear, we're trying to resolve a previously unresolved type,
    so always return false.
***************************************************************************************************/
bool IMPORTER::GetTypeFromCache(ImportScope & scope, mdToken tok, TypeArray * typeArgs, TYPESYM ** ptype)
{
    SYM * symT;

    if (!GetSymFromCache(scope, tok, &symT)) {
        *ptype = NULL;
        return false;
    }

    if (!symT)
        *ptype = NULL;
    else if (symT->isAGGSYM())
        *ptype = symT->asAGGSYM()->getThisType();
    else if (symT->isTYPESYM())
        *ptype = symT->asTYPESYM();
    else {
        VSFAIL("Why are we here?");
        *ptype = NULL;
        return false;
    }

    if (!typeArgs || !*ptype)
        return true;

    if ((*ptype)->isAGGTYPESYM()) {
        AGGTYPESYM * ats = (*ptype)->asAGGTYPESYM();
        if (typeArgs == ats->typeArgsAll)
            return true;
        if (typeArgs->size != ats->typeArgsAll->size) {
            VSFAIL("Why are we here?");
            *ptype = NULL;
            return false;
        }
        *ptype = compiler()->getBSymmgr().GetInstAgg(ats->getAggregate(), typeArgs);
        return true;
    }

    if ((*ptype)->isERRORSYM()) {
        ERRORSYM * err = (*ptype)->asERRORSYM();
        ASSERT(err->parent && err->nameText && err->typeArgs);

        // Type args work differently for ERRORSYMs. The first type arg is the parent type.
        TYPESYM * typePar = NULL;
        TypeArray * typeArgsActual = NULL;

        if (typeArgs && typeArgs->size > 0) {
            // The parent type is encoded as the first type argument. void * indicates no parent type.
            if (!typeArgs->Item(0)->isPTRSYM()) {
                typePar = typeArgs->Item(0);
            }
            else {
                ASSERT(typeArgs->Item(0)->asPTRSYM()->baseType()->isVOIDSYM());
                ASSERT(!typePar);
            }
            if (typeArgs->size > 1) {
                // Get the the real type args.
                typeArgsActual = compiler()->getBSymmgr().AllocParams(typeArgs->size - 1, typeArgs->ItemPtr(1));
            }
        }

        if (typePar) {
            ASSERT(err->parent->isTYPESYM() || err->parent == compiler()->getBSymmgr().GetRootNsAid(kaidGlobal));
            *ptype = compiler()->getBSymmgr().GetErrorType(typePar, err->nameText, typeArgsActual);
        }
        else {
            NSAIDSYM * nsa = NULL;
            if (err->parent->isNSAIDSYM())
                nsa = err->parent->asNSAIDSYM();
            *ptype = compiler()->getBSymmgr().GetErrorType(nsa, err->nameText, typeArgsActual);
        }
        return true;
    }

    VSFAIL("Why are we here?");
    if (typeArgs->size) {
        *ptype = NULL;
        return false;
    }
    return true;
}


bool IMPORTER::GetSymFromCache(ImportScope & scope, mdToken tok, SYM ** psym)
{
    TokenToSymTable * ptst = compiler()->getBSymmgr().GetTokenToSymTable();

    if (!ptst->GetSymFromToken(scope.GetModule(), tok, psym))
        return false;
    ASSERT(!*psym || !(*psym)->isAGGTYPESYM());

    return true;
}


/***************************************************************************************************
    Store AGGSYMs, not AGGTYPESYMs. This allows us to defer creation of the instance AGGTYPESYM
    until first use. This saves a ton of memory because many imported types are never used.
***************************************************************************************************/
void IMPORTER::SetSymInCache(ImportScope & scope, mdToken tok, SYM * sym)
{
    ASSERT(!sym || !sym->isAGGTYPESYM());
    compiler()->getBSymmgr().GetTokenToSymTable()->SetSymForToken(scope.GetModule(), tok, sym);
}


/***************************************************************************************************
    Get an AGGSYM from the cache. Don't load any types.
***************************************************************************************************/
bool IMPORTER::GetAggFromCache(ImportScope & scope, mdTypeDef tok, AGGSYM ** pagg)
{
    ASSERT(scope.GetMetaImport());
    ASSERT(TypeFromToken(tok) == mdtTypeDef);

    if (tok == mdTypeDefNil) {
        *pagg = NULL;
        return false;
    }

    SYM * symT;
    if (!GetSymFromCache(scope, tok, &symT)) {
        *pagg = NULL;
        return false;
    }

    ASSERT(!symT || !symT->isAGGTYPESYM());

    *pagg = symT && symT->isAGGSYM() ? symT->asAGGSYM() : NULL;
    return true;
}


MODULESYM * IMPORTER::GetUnresolvedModule(ImportScope & scopeSource, mdToken token)
{
    // Create a module in the unresolved infile.
    INFILESYM * infileUnres = compiler()->getBSymmgr().GetInfileForAid(kaidUnresolved);
    ASSERT(infileUnres);

    NAME * nameUnres = compiler()->getBSymmgr().GetNameFromPtrs((UINT_PTR)scopeSource.GetModule(), (UINT_PTR)token);
    MODULESYM * moduleUnres = compiler()->LookupGlobalSym(nameUnres, infileUnres, MASK_MODULESYM)->asMODULESYM();
    if (!moduleUnres)
        moduleUnres = compiler()->getBSymmgr().CreateModule(nameUnres, infileUnres);

    return moduleUnres;
}

// Look up an AssemblyRef token and find the aid that it matches. If the assembly ref can be
// resolved, this returns the assembly id. Otherwise, it fabricates a module in the unresolved
// infile and returns the module id.
int IMPORTER::MapAssemblyRefToAid(ImportScope & scopeSource, mdAssemblyRef tkAssemblyRef)
{
    ASSERT(TypeFromToken(tkAssemblyRef) == mdtAssemblyRef);

    // Return any cached value.
    SYM * symT;
    if (GetSymFromCache(scopeSource, tkAssemblyRef, &symT))
        return symT->asNSAIDSYM()->GetAid();

    // Make sure we have the error assembly name.
    if (!m_nameErrorAssem) {
        ASSEMBLYMETADATA data;
        NAME * name = compiler()->namemgr->GetPredefName(PN_ERROR_ASSEM);
        memset(&data, 0, sizeof(data));
        // NOTE: cchName shoudl include the nul-terminator, hence the wcslen + 1.
        HRESULT hr; hr = MakeAssemblyName( name->text, (ULONG)wcslen(name->text) + 1, data, NULL, 0, 0, NULL, &m_nameErrorAssem);
        ASSERT(SUCCEEDED(hr));
    }

    // Get the ref's assembly name
    NAME * nameAssemblyRef = GetAssemblyName(scopeSource, tkAssemblyRef);
    
    int aid = (nameAssemblyRef == m_nameErrorAssem) ? kaidErrorAssem : MapAssemblyRefToAid(nameAssemblyRef, scopeSource, false);

    if (aid == kaidUnresolved) {
        // Create a module in the unresolved infile.
        MODULESYM * moduleUnres = GetUnresolvedModule(scopeSource, tkAssemblyRef);
        ASSERT(moduleUnres);

        aid = moduleUnres->GetModuleID();
    }

    ASSERT(aid != kaidUnresolved);

    SetSymInCache(scopeSource, tkAssemblyRef, compiler()->getBSymmgr().GetRootNsAid(aid));
    return aid;
}

// Use the Assembly Equivalence API to match the ref to the closest def amongst our imports
int IMPORTER::MapAssemblyRefToAid(NAME * nameAssemblyRef, ImportScope & scopeSource, bool fIsFriendAssemblyRef)
{
    // Just return any cached value (which includes failures).
    SYM * symT;
    if (compiler()->getBSymmgr().GetNameToSymTable()->GetSymFromName(nameAssemblyRef, &symT)) {
        int aid = symT->asNSAIDSYM()->GetAid();
        if (aid == kaidThisAssembly)
            compiler()->RecordAssemblyRefToOutput(nameAssemblyRef, scopeSource.GetModule(), fIsFriendAssemblyRef);
        return aid;
    }

    HRESULT hr;
    if (FAILED(hr = InitFusionAPIs()))
        return kaidUnresolved;

    // In the EE case S_FALSE means that we couldn't load the Comparison API, so we must be running
    // On a down-level platform, so fall-back to our own comparison API, which just plain isn't as robust
    ASSERT(hr == S_OK);

    OUTFILESYM * mdfileroot;
    INFILESYM * infileCompare;
    INFILESYM * infileCandidate = NULL;
    INFILESYM * infileBadVersion = NULL;
    int aid = kaidUnresolved;
    bool fPlatformUnify = false;
    CComPtr<IAssemblyName> panRef;

    mdfileroot = compiler()->getBSymmgr().GetMDFileRoot();

    // Go through all the inputfile symbols. checking for an exact match
    for (infileCompare = mdfileroot->firstInfile();
            infileCompare != NULL;
            infileCompare = infileCompare->nextInfile())
    {
        if (infileCompare->isAddedModule || infileCompare->getBogus())
            continue;

        GetAssemblyName(infileCompare);

        if (infileCompare->assemblyName == nameAssemblyRef) {
            aid = infileCompare->GetAssemblyID();
            goto FOUND;
        }
    }

    // Check if it is a valid assembly name (including partials)
    ParseAssemblyName( nameAssemblyRef->text, scopeSource, &panRef);
    if (panRef == NULL) // Couldn't be parsed, so it will be unresolved
        goto FOUND;

    // Go through all the inputfiles again looking for match
    // and keep track of any partial matches
    for (infileCompare = mdfileroot->firstInfile();
        infileCompare != NULL;
        infileCompare = infileCompare->nextInfile())
    {
        if (infileCompare->isAddedModule || infileCompare->getBogus())
            continue;

        ASSERT(infileCompare->assemblyName);
        ASSERT(panRef != NULL);
        AssemblyIdentityComparison aic(m_pfnCompareAssemblyIdentity); 
        {
            hr = aic.Compare(nameAssemblyRef->text, false, infileCompare->assemblyName->text, true);
            if (hr == HRESULT_FROM_WIN32(ERROR_INVALID_DATA) || hr == FUSION_E_INVALID_NAME) {
                compiler()->Error(scopeSource, WRN_InvalidAssemblyName, nameAssemblyRef->text);
                goto FOUND;
            }
            CheckHR(hr, scopeSource);
        }
        if (FAILED(hr))
            continue;

        if (!aic.IsEquivalent()) {
            if (aic.NonEquivalentDueToVersions() &&
                (infileBadVersion == NULL || infileBadVersion->CompareVersions( infileCompare) < 0)) {
                infileBadVersion = infileCompare;
            }
            continue;
        }

        if (aic.CouldUnify()) {
            fPlatformUnify = aic.CouldUnify(true);
            if (infileCandidate == NULL || infileCandidate->CompareVersions( infileCompare) > 0) {
                infileCandidate = infileCompare;
            }
            continue;
        }

        if (aic.IsEquivalentNoUnify()) {
            aid = infileCompare->GetAssemblyID();
            goto FOUND;
        }
            
        VSFAIL("Invalid AssemblyComparisonResult and fEquivalent combo");
    }

    if (infileCandidate != NULL) {
        // If we had an exact match, we wouldn't be here
        if (!fPlatformUnify) {
            int diff = 0;
            diff = CompareVersions( scopeSource, panRef, infileCandidate);
            if (diff == 0) {
                // partial match that unfied on version, but there's no diff?
                VSFAIL("Can't have a partial unified with the same version");
            }
            else if (diff <= 2) {
                compiler()->Error( scopeSource, WRN_UnifyReferenceMajMin, nameAssemblyRef->text, infileCandidate->assemblyName->text, ErrArgRefOnly(infileCandidate));
            }
            else {
                compiler()->Error( scopeSource, WRN_UnifyReferenceBldRev, nameAssemblyRef->text, infileCandidate->assemblyName->text, ErrArgRefOnly(infileCandidate));
            }
        }
        aid = infileCandidate->GetAssemblyID();
        goto FOUND;
    }

    if (MatchesThisAssembly(nameAssemblyRef, scopeSource)) {
        compiler()->RecordAssemblyRefToOutput(nameAssemblyRef, scopeSource.GetModule(), fIsFriendAssemblyRef);
        aid = kaidThisAssembly;
    }
    else 
        if (infileBadVersion) {
        ASSERT(!fPlatformUnify && panRef);
        ASSERT(!infileBadVersion->isBCL);
        ASSERT(scopeSource.GetModule());

        PCWSTR pszAsm = NULL;
        ASSERT(scopeSource.GetModule());
        if (scopeSource.GetModule())
            pszAsm = GetAssemblyName(scopeSource.GetModule());
        if (!pszAsm)
            pszAsm = scopeSource.GetFileName();
        compiler()->Error(scopeSource, ERR_AssemblyMatchBadVersion, pszAsm, nameAssemblyRef->text, infileBadVersion->assemblyName->text, ErrArgRefOnly(infileBadVersion));
        aid = infileBadVersion->GetAssemblyID();
    }

FOUND:
    compiler()->getBSymmgr().GetNameToSymTable()->SetSymForName(nameAssemblyRef, compiler()->getBSymmgr().GetRootNsAid(aid));
    return aid;
}

/***************************************************************************************************
    Compares just the version.  Returns 1 - 4 for the highest version part that is different
    Or 0 if the versions are the same (for partials) or is BCL
***************************************************************************************************/
int IMPORTER::CompareVersions(ImportScope & scopeSource, IAssemblyName * panRef, INFILESYM * infileCompare)
{
    ASSERT(panRef != NULL);

    for (int i = 0; i < 4; i++) {
        DWORD cb = 0;
        WORD wVer = 0;
        HRESULT hr;
        hr = panRef->GetProperty( ASM_NAME_MAJOR_VERSION + i, NULL, &cb);
        if (hr == S_OK) {
            // Not specified in a partial
            return 0;
        }
        else if (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
            CheckHR(hr, scopeSource);
        }

        cb = sizeof(WORD);
        CheckHR(hr = panRef->GetProperty( ASM_NAME_MAJOR_VERSION + i, &wVer, &cb), scopeSource);
        if (wVer != infileCompare->assemblyVersion[i]) {
            return i + 1;
        }
    }
    VSFAIL("Non partial assembly ref version exactly matches the def! How'd we get here?");
    return 0;
}


/***************************************************************************************************
    Match just the simple name. (Also generates the simple name if needed)
***************************************************************************************************/
bool IMPORTER::MatchAssemblySimpleName( IAssemblyName * asmnameRef, ImportScope & scopeSource, OUTFILESYM * manifest)
{
    if (manifest->simpleName == NULL) {
        NAME * nameOutput = PEFile::GetModuleName(manifest, compiler());
        PCWSTR pchDot = wcsrchr(nameOutput->text, L'.');

        if (pchDot == NULL)     
            manifest->simpleName = nameOutput;
        else
            manifest->simpleName = compiler()->namemgr->AddString(nameOutput->text, (int)(pchDot - nameOutput->text));
    }
    return MatchAssemblySimpleName(asmnameRef, scopeSource, manifest->simpleName);
}

bool IMPORTER::MatchAssemblySimpleName( IAssemblyName * asmnameRef, ImportScope & scopeSource, NAME *simpleName)
{
    WCHAR * pchSimpleNameRef;
    DWORD cchSimpleNameRef = 0;
    HRESULT hr; hr = asmnameRef->GetName( &cchSimpleNameRef, NULL);
    ASSERT(FAILED(hr));
    if (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
        CheckHR(hr, scopeSource);

    pchSimpleNameRef = STACK_ALLOC(WCHAR, cchSimpleNameRef); // Count should include nul terminator
    CheckHR(asmnameRef->GetName( &cchSimpleNameRef, pchSimpleNameRef), scopeSource);
    return CompareAssemblySimpleNames(pchSimpleNameRef, simpleName->text);
}

// Compares two simple names for equality using fusion
// The complexity is to ensure that the casing comparison exactly
// matches fusion.
// Interestingly the spec states that assembly name comparisons
// are case sensitive, however the code doesn't match the spec
bool IMPORTER::CompareAssemblySimpleNames(LPCWSTR assemblyRef, LPCWSTR assemblyDef)
{
    HRESULT hr; 

    // convert the def to a dummy full name by adding
    // null values for version, culture and public key
    NAME *pName = 0;
    ASSEMBLYMETADATA data;
    memset(&data, 0, sizeof(data));
    hr = MakeAssemblyName( assemblyDef, (ULONG)wcslen(assemblyDef) + 1, data, NULL, 0, 0, NULL, &pName);
    ASSERT(SUCCEEDED(hr));

    // do the comparison
    AssemblyIdentityComparison aic(m_pfnCompareAssemblyIdentity); 
    hr = aic.Compare(assemblyRef, false, pName->text, true);
    return SUCCEEDED(hr) && aic.IsEquivalent();
}


/***************************************************************************************************
    Match just the public key token . (Gets it from alink if needed)
***************************************************************************************************/
bool IMPORTER::MatchAssemblyPublicKeyToken( IAssemblyName * asmnameRef, ImportScope & scopeSource, OUTFILESYM * manifest)
{
    BYTE * pbPublicKeyTokenRef;
    DWORD cbPublicKeyTokenRef = 0;
    HRESULT hr;

    // Make sure we have our public key token
    if (manifest->cbPublicKeyToken == 0xFFFFFFFF) {
        DWORD cbManifestPublicKeyToken = 0;
        BYTE * pbManifestPublicKeyToken = NULL;

        hr = compiler()->linker->GetPublicKeyToken( compiler()->options.m_sbstrKEYFILE, compiler()->options.m_sbstrKEYNAME, NULL, &cbManifestPublicKeyToken);
        if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
            compiler()->Error( NULL, FTL_MetadataEmitFailure, compiler()->ErrHR(hr), manifest);

        if (cbManifestPublicKeyToken > 0) {
            pbManifestPublicKeyToken = (BYTE*)compiler()->getGlobalSymAlloc().Alloc(cbManifestPublicKeyToken);
            if (FAILED(hr = compiler()->linker->GetPublicKeyToken( compiler()->options.m_sbstrKEYFILE, compiler()->options.m_sbstrKEYNAME, pbManifestPublicKeyToken, &cbManifestPublicKeyToken))) 
                compiler()->Error( NULL, FTL_MetadataEmitFailure, compiler()->ErrHR(hr), manifest);
        }
        manifest->cbPublicKeyToken = cbManifestPublicKeyToken;
        manifest->pbPublicKeyToken = pbManifestPublicKeyToken;
    }

    // Let's get the public key token value
    hr = asmnameRef->GetProperty( ASM_NAME_PUBLIC_KEY_TOKEN, NULL, &cbPublicKeyTokenRef);
    if (hr == S_OK) {
        // There's either no public key token specified (includes no public key)
        // Or the name explicitly states NO public key (PublicKeyToken=NULL)?
        hr = asmnameRef->GetProperty( ASM_NAME_NULL_PUBLIC_KEY_TOKEN, NULL, NULL);
        ASSERT(hr == S_OK || hr == S_FALSE);

        // no NULL public key token matches anything
        return hr == S_FALSE || manifest->hasNoPublicKeyToken();

    }
    if (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
        CheckHR(hr, scopeSource);

    if (manifest->hasNoPublicKeyToken()) {
        // The ref has a non-NULL public key token, but we have none
        // This may be because it is specified as an attribute.  We treat it as a match, despite the mis-match in public-key tokens,
        // but will give an appropriate error/warning later in the compile when we verify the assembly refs
        return true;
    }
    if (manifest->cbPublicKeyToken != cbPublicKeyTokenRef) {
        VSFAIL("We has 2 different public key token lengths!?!?!?!");
        // I don't think this can happen, but better to be safe than sorry.
        return false;
    }

    pbPublicKeyTokenRef = STACK_ALLOC(BYTE, cbPublicKeyTokenRef);
    CheckHR(asmnameRef->GetProperty( ASM_NAME_PUBLIC_KEY_TOKEN, pbPublicKeyTokenRef, &cbPublicKeyTokenRef), scopeSource);

    // We have a non-NULL public key token, the ref has a non-NULL public key token, now we just have to compare the bits to see if they're the same
    return (memcmp( pbPublicKeyTokenRef, manifest->pbPublicKeyToken, cbPublicKeyTokenRef) == 0);
}


/***************************************************************************************************
    Match an assembly string against the name of this assembly.
    This should only be called by MapAssemblyRefToAid!
***************************************************************************************************/
bool IMPORTER::MatchesThisAssembly(NAME * nameAssemblyRef, ImportScope & scopeSource)
{
    CComPtr<IAssemblyName> asmnameRef;
    ParseAssemblyName(nameAssemblyRef->text, scopeSource, &asmnameRef);

    if (asmnameRef == NULL) // Not a valid assembly name
        return false;

    OUTFILESYM * outfile = compiler()->GetManifestOutFile();
    if (!outfile && compiler()->options.m_sbstrMODULEASSEMBLY)
    {
        NAME *name = compiler()->getNamemgr()->AddString(compiler()->options.m_sbstrMODULEASSEMBLY);
        return MatchAssemblySimpleName(asmnameRef, scopeSource, name);
    }
    else if (!outfile || outfile->isUnnamed())
        return false;

    if (!MatchAssemblySimpleName(asmnameRef, scopeSource, outfile))
        return false;

    if (!MatchAssemblyPublicKeyToken(asmnameRef, scopeSource, outfile))
        return false; // no match we're done

    return true;
}


int IMPORTER::MapModuleRefToAid(ImportScope & scopeSource, mdModuleRef tkModuleRef)
{
    ASSERT(TypeFromToken(tkModuleRef) == mdtModuleRef);

    // Just return any cached value (which includes failures)
    SYM * symT;
    if (GetSymFromCache(scopeSource, tkModuleRef, &symT))
        return symT->asNSAIDSYM()->GetAid();

    IMetaDataImport * import = scopeSource.GetMetaImport();

    //
    // get required sizes for name
    //
    ULONG cchName;
    CheckHR(import->GetModuleRefProps(tkModuleRef, NULL, 0, &cchName), scopeSource);

    //
    // Stack allocate and get the actual name
    //
    WCHAR *szName = STACK_ALLOC(WCHAR, cchName);
    CheckHR(import->GetModuleRefProps(tkModuleRef, szName, cchName, &cchName), scopeSource);
    NAME * name = compiler()->getNamemgr()->AddString(szName);

    int aid = kaidUnresolved;
    if (scopeSource.GetAssemblyID() != kaidThisAssembly) {
        ASSERT(scopeSource.GetModule());
        symT = compiler()->LookupGlobalSym(name, scopeSource.GetModule()->getInputFile(), MASK_MODULESYM);
        if (symT) {
            aid = symT->asMODULESYM()->GetModuleID();
        }
    }
    else {
        for (INFILESYM * infile = compiler()->getBSymmgr().GetMDFileRoot()->firstInfile();
             infile != NULL;
             infile = infile->nextInfile())
        {
            if (infile->isAddedModule && infile->moduleManifest->name == name) {
                aid = infile->moduleManifest->GetModuleID();
                goto LDone;
            }
        }
        // Now search the output files
        SourceOutFileIterator files;
        for (OUTFILESYM * pOutfile = files.Reset(compiler()); pOutfile != NULL; pOutfile = files.Next())
        {
            if (pOutfile->isUnnamed()) {
                // Nothing to compare against
                continue;
            }

            if (PEFile::GetModuleName(pOutfile, compiler()) == name) {
                aid = pOutfile->GetModuleID();
                break;
            }
        }
    }
LDone:

    // Cache even the failure case.
    SetSymInCache(scopeSource, tkModuleRef, compiler()->getBSymmgr().GetRootNsAid(aid));
    return aid;
}

/**************************************************************************
Compares two imports and issues appropriate errors for duplicates
if duplicates are found infile2 is 'ignored' by setting it to bogus
returns true if error was reported
**************************************************************************/
bool IMPORTER::CompareImports( INFILESYM * infile1, INFILESYM * infile2)
{
    // In the EE case we may not have access tot he CompareAssemblyIdentity API
    // But it doesn't matter since in the EE case we only see references to assemblies
    // that are actually loaded

    if (FAILED(InitFusionAPIs()))
        return false;

    AssemblyIdentityComparison aic(m_pfnCompareAssemblyIdentity);
    HRESULT hr;
    CheckHR(hr = aic.Compare(GetAssemblyName(infile1), false, GetAssemblyName(infile2), false), infile1);
    if (FAILED(hr))
        return false;

    switch (aic.GetResult()) {
    case ACR_EquivalentFXUnified:
    case ACR_EquivalentFullMatch:
        infile2->setBogus(true);
        compiler()->Error( NULL, ERR_DuplicateImport, infile1->assemblyName, ErrArgRefOnly(infile1), ErrArgRefOnly(infile2));
        return true;
    case ACR_EquivalentWeakNamed:
        infile2->setBogus(true);
        compiler()->Error( NULL, ERR_DuplicateImportSimple, infile1->assemblyName, ErrArgRefOnly(infile1), ErrArgRefOnly(infile2));
        return true;
    case ACR_NonEquivalentVersion:
    case ACR_NonEquivalent:
        // nothing to do
        break;
    case ACR_EquivalentUnified:
    default:
        VSFAIL("Unrecognized or invalid assembly comparison result!");
        break;
    }
    return false;
}

HRESULT IMPORTER::InitFusionAPIs()
{
    if (m_pfnCreateAssemblyNameObject == NULL)
    {
        HRESULT hr = GetRealProcAddress("CreateAssemblyNameObject", (void**)&m_pfnCreateAssemblyNameObject);
        if (FAILED(hr))
        {
            compiler()->Error( NULL, FTL_ComPlusInit, compiler()->ErrHR(hr));
            return hr;
        }
    }
    if (m_pfnCompareAssemblyIdentity == NULL)
    {
        HRESULT hr = GetRealProcAddress("CompareAssemblyIdentity", (void**)&m_pfnCompareAssemblyIdentity);
        if (FAILED(hr))
        {
            compiler()->Error( NULL, FTL_ComPlusInit, compiler()->ErrHR(hr));
            return hr;
        }
    }
    return S_OK;
}

// Creates a fusion IAssemblyName object from the given string, and does not report an error on failure.
HRESULT IMPORTER::ParseAssemblyNameNoError(PCWSTR szAsmName, IAssemblyName **ppAssemblyNameObj)
{
    if (ppAssemblyNameObj)
        *ppAssemblyNameObj = NULL;

    HRESULT hr;
    if (FAILED(hr = InitFusionAPIs()))
        return hr;

    CComPtr<IAssemblyName> pAssemName;

    hr = m_pfnCreateAssemblyNameObject( &pAssemName, szAsmName, CANOF_PARSE_DISPLAY_NAME, NULL);

    if (SUCCEEDED(hr) && ppAssemblyNameObj && pAssemName)
        *ppAssemblyNameObj = pAssemName.Detach();

    return hr;
}

// Creates a fusion IAssemblyName object from the given string
void IMPORTER::ParseAssemblyName(PCWSTR szAsmName, ImportScope & scopeSource, IAssemblyName ** ppan)
{
    ASSERT(ppan);

    *ppan = NULL;
    HRESULT hr;

    CComPtr<IAssemblyName> tempAssemblyName;
    if (FAILED(hr = ParseAssemblyNameNoError(szAsmName, &tempAssemblyName))) {
        if (hr == FUSION_E_INVALID_NAME || hr == E_INVALIDARG)
            compiler()->Error(scopeSource, WRN_InvalidAssemblyName, szAsmName);
        else
            CheckHR(hr, scopeSource);
    } else {
        *ppan = tempAssemblyName.Detach();
    }
}

// validate whether or not a stringized assembly name is valid
bool IMPORTER::IsValidAssemblyName(PCWSTR szAsmName)
{
    HRESULT hr = ParseAssemblyNameNoError(szAsmName, NULL);
    return (hr != FUSION_E_INVALID_NAME && hr != E_INVALIDARG);
}

// validate whether or not a strinized assembly name is a valid friend assembly reference.
// Friend assembly references cannot have verison numbers, cultures, or arch specified and if the 
// output will be signed, then the friend assembly reference must be signed as well (i.e. specify a public key)
bool IMPORTER::CheckFriendAssemblyName(BASENODE *tree, PCWSTR szAsmName, OUTFILESYM *context)
{
    CComPtr<IAssemblyName> pName;
    OUTFILESYM *outfile;
    DWORD dwSize = 0;
    HRESULT hr = ParseAssemblyNameNoError(szAsmName, &pName);

    if (!pName || FAILED(hr))
        goto L_INVALID_FRIEND;

    if (FAILED(hr = pName->GetProperty(ASM_NAME_MAJOR_VERSION, NULL, &dwSize))   ||
        FAILED(hr = pName->GetProperty(ASM_NAME_MINOR_VERSION, NULL, &dwSize))   ||
        FAILED(hr = pName->GetProperty(ASM_NAME_BUILD_NUMBER, NULL, &dwSize))    ||
        FAILED(hr = pName->GetProperty(ASM_NAME_REVISION_NUMBER, NULL, &dwSize)) ||
        FAILED(hr = pName->GetProperty(ASM_NAME_CULTURE, NULL, &dwSize))         ||
        FAILED(hr = pName->GetProperty(ASM_NAME_ARCHITECTURE, NULL, &dwSize)))
    {
        if (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
            goto L_INVALID_FRIEND;

        compiler()->Error(tree, ERR_FriendAssemblyBadArgs, szAsmName);
        return false;
    }

    // All we care about is the manifest outfile, unless we're building just modules with no manifest, 
    // then we need to check the current OUTFILESYM for the specific module.
    outfile = compiler()->GetManifestOutFile();
    if (!outfile)
        outfile = context;

    ASSERT(dwSize == 0);
    // If the output assembly has a strong name, then its friends declarations need to be signed as well.
    // For DealySign, COF_DEFAULTON is set by default for that flag even though the default should be false (see comment in optdef.h)
    // Thus, we need to specifically check for TRUE to see if it was specified on the command-line.
    if (SUCCEEDED(hr = pName->GetProperty(ASM_NAME_PUBLIC_KEY, NULL, &dwSize)) &&
        (compiler()->options.m_sbstrKEYFILE ||
         compiler()->options.m_fDELAYSIGN == TRUE ||
         compiler()->options.m_sbstrKEYNAME ||
         outfile->fHasSigningAttribute))
    {
        // If this call succeeds with an empty buffer, then the supplied
        // name doesn't have a public key token.
        compiler()->Error(tree, ERR_FriendAssemblySNReq, szAsmName);
        return false;
    } 
    
    if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
        goto L_INVALID_FRIEND;

    return true;

L_INVALID_FRIEND:
    compiler()->Error(tree, WRN_InvalidAssemblyName, szAsmName);
    return false;
}

/***************************************************************************************************
    Creates a fusion IAssemblyName object for the given output file
    Also returns the NAME*
***************************************************************************************************/
IAssemblyName * IMPORTER::GetOutputAssemblyName( PEFile * fileOutput, NAME ** pnameOutput)
{
    HRESULT hr;
    CComPtr<IMetaDataAssemblyImport> pmaiOutput;
    NAME * nameOutput;
    CComPtr<IAssemblyName> panOutput;

    *pnameOutput = NULL;
    if (FAILED(InitFusionAPIs()))
        return NULL;

    if (SUCCEEDED(hr = fileOutput->GetEmit()->QueryInterface( IID_IMetaDataAssemblyImport, (void**)&pmaiOutput)) &&
        SUCCEEDED(hr = GetAssemblyName( pmaiOutput, &nameOutput, NULL, NULL)))
    {
        hr = m_pfnCreateAssemblyNameObject( &panOutput, nameOutput->text, CANOF_PARSE_DISPLAY_NAME, NULL);
    }
    if (FAILED(hr)) {
        compiler()->Error(NULL, FTL_MetadataEmitFailure, compiler()->ErrHR(hr), fileOutput->GetOutFile()->name->text);
        return NULL;
    }

    *pnameOutput = nameOutput;    
    return panOutput.Detach();
}

/***************************************************************************************************
    Compares panOutput to nameAssemblyRef
    Gives a warning if a circular reference does not match.  If the reference is purely a friend assembly reference (i.e. InternalsVisibleTo("xxx")) then
    we give an error if the friend access is used and the references for sure do not match.  We give a warning if the friend access is used, the references
    do not match but they could unify, and we do not report anything if the friend access is never used.

    REVIEW GrantRi: Should we give unification warnings here?
***************************************************************************************************/
void IMPORTER::ConfirmMatchesThisAssembly(IAssemblyName * panOutput, NAME * nameOutput, NAME * nameAssemblyRef, SYMLIST * listModSrc, bool fIsFriendAssemblyRefOnly)
{
    ImportScopeModule scope(this, listModSrc->sym->asMODULESYM());
    AssemblyIdentityComparison aic(m_pfnCompareAssemblyIdentity);
    HRESULT hr;

    // check whether or not a friend assembly reference was used.
    bool fFriendUsage = false;
    FOREACHSYMLIST(listModSrc, mod, MODULESYM)
        if (mod->getInputFile()->fFriendAccessUsed) {
            fFriendUsage = true;
            break;
        }
    ENDFOREACHSYMLIST;

    CError * err = NULL;
    if (fFriendUsage) 
    {
        // If friend access was used, we need to use the IsEqual API in order to mimic the lookup the runtime will do at execution.
        // Friend assembly checks at runtime are not affected by policy, so the assemblies either match or they don't, and we can give an error in the case where they don't.
        // Note:  Even though the friend assembly check is not influenced by policy, it's possible that the assembly loading could be.
        CComPtr<IAssemblyName> panRef;
        CComPtr<IAssemblyName> panDef;
        ParseAssemblyName(nameAssemblyRef->text, scope, &panRef);
        ParseAssemblyName(nameOutput->text, scope, &panDef);
        ASSERT(panRef && panDef);
        HRESULT hr = S_OK;

        CheckHR(hr = panRef->IsEqual(panDef, ASM_CMPF_DEFAULT), scope);

        if (hr == S_FALSE)
            err = compiler()->MakeError(NULL, ERR_FriendRefNotEqualToThis, nameAssemblyRef, nameOutput);
    } else {
        CheckHR(hr = aic.Compare(nameAssemblyRef->text, false, nameOutput->text, true), scope);
        if (FAILED(hr)) {
            CheckHR(hr, scope);
            return;
        }

        // there is a circular reference (without using a friend assembly reference) so we give a unification warning since the assemblies could unify at runtime.
        if (!fIsFriendAssemblyRefOnly && !aic.IsEquivalentNoUnify() && aic.CouldUnify())
            err = compiler()->MakeError(NULL, WRN_AssumedMatchThis, nameAssemblyRef, nameOutput);
    } 

    if (!err)
        return;

    FOREACHSYMLIST(listModSrc, mod, MODULESYM)
        compiler()->AddLocationToError(err, ERRLOC(mod->getInputFile(), NULL));
    ENDFOREACHSYMLIST;
    compiler()->SubmitError(err);
}
    

#ifdef DEBUG
/*
 * Go through all types and declare all types imported from metadata. Used to test
 * the meta-data declaring code.
 */
void IMPORTER::DeclareAllTypes(PPARENTSYM parent)
{
    if (parent->isNSSYM()) {
        for (NSDECLSYM * decl = parent->asNSSYM()->DeclFirst(); decl; decl = decl->DeclNext()) {
            DeclareAllTypes(decl);
        }
    }
    else {
        AGGSYM * agg;

        FOREACHCHILD(parent, symChild)
            if (symChild->isNSDECLSYM()) {
                DeclareAllTypes(symChild->asNSDECLSYM());
                continue;
            }

            if (symChild->isAGGDECLSYM()) {
                if (!symChild->asAGGDECLSYM()->IsFirst())
                    continue;
                agg = symChild->asAGGDECLSYM()->Agg();
            }
            else if (symChild->isAGGSYM()) {
                agg = symChild->asAGGSYM();
            }
            else {
                continue;
            }

            if (!agg->isSource && !agg->IsPrepared() &&
                agg->DeclOnly()->getInputFile() && agg->DeclOnly()->getInputFile()->moduleManifest &&
                TypeFromToken(agg->tokenImport) == mdtTypeDef)
            {
                if (!agg->IsDefined()) 
                    DefineImportedType(agg);
                DeclareAllTypes(agg);
            }
        ENDFOREACHCHILD
    }
}
#endif //DEBUG

/***************************************************************************************************
    Implementation of virtual methods for ImportScopeModule.
***************************************************************************************************/
IMetaDataImport * ImportScopeModule::GetMetaImport()
{
    return m_mod->GetMetaImport(m_import->compiler());
}

IMetaDataAssemblyImport * ImportScopeModule::GetAssemblyImport()
{
    return m_mod->GetAssemblyImport(m_import->compiler());
}


// Wrapper class used for comparing assembly refs by calling into the fusion APIs.
AssemblyIdentityComparison::AssemblyIdentityComparison(PfnCompareAssemblyIdentity pfnCompare)
{
    ASSERT(pfnCompare);
    m_pfnCompareAssemblyIdentity = pfnCompare;
    m_fEqual = FALSE;
    m_acrResult = ACR_Unknown;
    m_fAllowUnification = false;
}

// compare two stringized assembly references.  
HRESULT AssemblyIdentityComparison::Compare(PCWSTR strAssembly1, bool fUnify1, PCWSTR strAssembly2, bool fUnify2)
{

    ASSERT(m_pfnCompareAssemblyIdentity);
    m_fAllowUnification = fUnify1 || fUnify2;

    HRESULT hr = m_pfnCompareAssemblyIdentity(strAssembly1, fUnify1, strAssembly2, fUnify2, &m_fEqual, &m_acrResult);
    
    if (hr == HRESULT_FROM_WIN32(ERROR_INVALID_DATA) || hr == FUSION_E_INVALID_NAME) {
        m_fEqual = false;
        m_acrResult = ACR_Unknown;
    }

    ASSERT(!m_fEqual || IsEquivalentNoUnify() || CouldUnify());
    return hr;
}

bool AssemblyIdentityComparison::IsEquivalentNoUnify()
{
    return ( (m_acrResult == ACR_EquivalentFullMatch)    ||        // all fields match
             (m_acrResult == ACR_EquivalentWeakNamed)    ||        // match based on weak-name, version numbers ignored
             (m_acrResult == ACR_EquivalentPartialMatch) ||
             (m_acrResult == ACR_EquivalentPartialWeakNamed) );
}

bool AssemblyIdentityComparison::CouldUnify(bool fOnlyFXUnification)
{
    bool fCouldUnify = ( (m_acrResult == ACR_EquivalentFXUnified) ||           // match based on FX-unification of version numbers
                         (m_acrResult == ACR_EquivalentPartialFXUnified) );

    if (!fOnlyFXUnification)
        fCouldUnify |= ( (m_acrResult == ACR_EquivalentUnified) ||             // match based on legacy-unification of version numbers
                         (m_acrResult == ACR_EquivalentPartialUnified) );

    return fCouldUnify;
}

bool AssemblyIdentityComparison::NonEquivalentDueToVersions()
{
    // This only returns true when the assemblies are determined to not be equal and do not unify (i.e. it does not handle version differences if assemblies unify,
    // as is the case of ACR_EquivalentUnified.
    ASSERT(!IsEquivalent() && m_fAllowUnification);
    return ( (m_acrResult == ACR_NonEquivalentVersion) ||         // all fields match except version field
             (m_acrResult == ACR_NonEquivalentPartialVersion) );
}




