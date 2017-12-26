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
// File: symmgr.h
//
// Defines the symbol manager, which manages the storage and lookup of symbols
// ===========================================================================

#ifndef __symmgr_h__
#define __symmgr_h__

/***************************************************************************************************
    Information for quickly checking whether a type name is the name of a predefined type. This
    is used by BSYMMGR::FPreLoad.
***************************************************************************************************/
struct PredefTypeNameInfo
{
    uint hash;
    NSSYM * ns;
    NAME * name;
    int cch;
};


/***************************************************************************************************
    Predefined type information.
***************************************************************************************************/
struct PredefTypeInfo
{
    PWSTR fullName;
    bool isRequired;
    bool isSimple;
    bool isNumeric;
    bool isQSimple;
    FUNDTYPE ft;
    uint et;
    AggKindEnum aggKind;
    PWSTR niceName;
    CONSTVAL zero;
    PREDEFATTR attr;
    int asize;
    int arity;
    int inmscorlib;
};


extern const PredefTypeInfo predefTypeInfo[];


/***************************************************************************************************
    The EE and type state:

    TYPESYMs and TypeArrays have two pieces of information that track their state for the EE.
    One is whether the object is "Unresolved". The second is whether the object is "Dirty".
    U(x) represents whether x is "Unresolved", D(x) represents whether x is "Dirty".

    * U(TypeArray ta) = Sum(U(T) : T in ta)
    * U(AGGTYPESYM ats) = U(ats->agg) + U(ats->typeArgsAll)
    * U(T*) = U(T) // T* represents any derived type (array, pointer, etc).
    * U(TYVARSYM var) = var->parent->isAgg && U(var->parent)

    Note that U(T) is immutable for all TYPESYMs and TypeArrays. Resolving an unresolved type sym
    produces a new type sym - it does not morph an existing type sym. Each TYPESYM and TypeArray
    caches the last thing that it was resolved to and when it was resolved. The "when" is according
    to the BSYMMGR::tsImport clock.

    * D(TypeArray ta) = Sum(D(T) : T in ta)
    * D(AGGTYPESYM ats) = U(ats) + D(ats->agg)
    * D(T*) = D(T)
    * D(TYVARSYM var) = U(var) + D(var->bounds)
    * D(AGGSYM agg) = D(agg->base) + D(agg->ifaces) + Sum(U(T) : T is used in a member of agg)

    Intuitively, dirty means that either the type is unresolved or its inheritance includes
    unresolved types or its members (inherited or direct) reference unresolved types.
    Note that a type argument being dirty does not cause the AGGTYPESYM to be dirty. If a type
    argument is unresolved, the AGGTYPESYM will be dirty though.

    Each TYPESYM and TypeArray keeps track of when it was last "cleaned" (stored in "tsDirty"
    according to the BSYMMGR::tsImport clock) and whether it is still dirty. If a object is known
    to be clean for all time (never needs cleaning again because it can't become dirty later), it
    stores ktsImportMax for its tsDirty value.
***************************************************************************************************/
const int ktsImportMax = 0x7FFFFFFF;


/***************************************************************************************************
    This holds the normalized TypeArrays. TypeArrays are used for signatures, generic type
    parameter lists, etc. This guarantees that two TypeArrays are equivalent iff their addresses
    are the same.
***************************************************************************************************/
class TypeArrayTable : public HashTableBaseSymMgr<TypeArrayTable, BSYMMGR> {
protected:
    friend class HashTableBase<TypeArrayTable>;

    struct Key {
        TYPESYM ** prgtype;
        int ctype;
        Key(int ctype, TYPESYM ** prgtype) { this->ctype = ctype; this->prgtype = prgtype; }
    };

    struct TypeArrayEntry : Entry {
        uint hash;
        TypeArray ta;
    };

    // Methods for HashTableBase.
    bool EqualEntryKey(TypeArrayEntry * ptae, Key * pkey, uint hash) {
        return hash == ptae->hash && ptae->ta.size == pkey->ctype && !memcmp(ptae->ta.ItemPtr(0), pkey->prgtype, pkey->ctype * sizeof(TYPESYM *));
    }
    uint GetEntryHash(TypeArrayEntry * ptae) { return ptae->hash; }

public:
    TypeArray * AllocTypeArray(int ctype, TYPESYM ** prgtype);
};


/***************************************************************************************************
    This maps (MODULESYM *, token) to SYM for imported symbols.
***************************************************************************************************/
class TokenToSymTable : public HashTableBaseSymMgr<TokenToSymTable, BSYMMGR> {
protected:
    friend class HashTableBase<TokenToSymTable>;

    struct Key {
        MODULESYM * module;
        mdToken tok;
        uint hash; // Store the hash to avoid recomputing it.

        Key(MODULESYM * pscope, mdToken tok);
    };

    struct EntryData : Entry {
        SYM * sym;
        Key key;
    };

    // Methods for HashTableBase.
    bool EqualEntryKey(EntryData * pend, Key * pkey, uint hash) {
        ASSERT(pkey->hash == hash);
        ASSERT(pend->key.hash == hash || pend->key.tok != pkey->tok || pend->key.module != pkey->module);
        return pend->key.tok == pkey->tok && pend->key.module == pkey->module;
    }
    uint GetEntryHash(EntryData * pend) { return pend->key.hash; }

public:
    bool GetSymFromToken(MODULESYM * pscope, mdToken tok, SYM ** psym);
    void SetSymForToken(MODULESYM * pscope, mdToken tok, SYM * sym);
};


/***************************************************************************************************
    This maps from a NAME to a SYM. This is used (for example) to map from a full assembly
    name to the INFILESYM (see import).
***************************************************************************************************/
class NameToSymTable : public HashTableBaseSymMgr<NameToSymTable, BSYMMGR> {
protected:
    friend class HashTableBase<NameToSymTable>;

    struct EntryData : Entry {
        SYM * sym;
        NAME * name;
    };

    // Methods for HashTableBase.
    bool EqualEntryKey(EntryData * pend, NAME * name, uint hash) {
        ASSERT(name->hash == hash);
        return pend->name == name;
    }
    uint GetEntryHash(EntryData * pend) { return pend->name->hash; }

public:
    bool GetSymFromName(NAME * name, SYM ** psym);
    void SetSymForName(NAME * name, SYM * sym);
};


/***************************************************************************************************
    This maps from an Aid to a SYM.
***************************************************************************************************/
class SymSet {
protected:
    MEMHEAP * m_heap;
    SYM ** m_prgsym;
    int m_csym;
    int m_csymAlloc;

public:
    SymSet(MEMHEAP * heap) {
        m_heap = heap;
        m_prgsym = NULL;
        m_csym = 0;
        m_csymAlloc = 0;
    }
    ~SymSet() {
        ASSERT(m_prgsym == NULL); // too-late to cleanup because m_mem has been freed
    }

    void Init() { }
    void Term();

    int Size() { return m_csym; }
    int AddSym(SYM * sym);
    SYM * GetSym(int isym) {
        ASSERT(0 <= isym && isym < m_csym);
        return m_prgsym[isym];
    }
};



// Used to specify whether and which type variables should be normalized.
namespace SubstTypeFlags { enum _Enum {
    NormNone = 0x00,
    NormClass = 0x01, // Replace class type variables with the normalized (standard) ones.
    NormMeth = 0x02,  // Replace method type variables with the normalized (standard) ones.
    NormAll = NormClass | NormMeth,
    DenormClass = 0x04, // Replace normalized (standard) class type variables with the given class type args.
    DenormMeth = 0x08,  // Replace normalized (standard) method type variables with the given method type args.
    DenormAll = DenormClass | DenormMeth
}; };
DECLARE_FLAGS_TYPE(SubstTypeFlags)


struct SubstContext {
    TYPESYM ** prgtypeCls;
    int ctypeCls;
    TYPESYM ** prgtypeMeth;
    int ctypeMeth;
    SubstTypeFlagsEnum grfst;

    SubstContext()
        { Init(NULL); }
    SubstContext(TypeArray *typeArgsCls, TypeArray *typeArgsMeth = NULL, SubstTypeFlagsEnum grfst = SubstTypeFlags::NormNone)
        { Init(typeArgsCls, typeArgsMeth, grfst); }
    SubstContext(AGGTYPESYM * type, TypeArray *typeArgsMeth = NULL, SubstTypeFlagsEnum grfst = SubstTypeFlags::NormNone)
        { Init(type ? type->typeArgsAll : NULL, typeArgsMeth, grfst); }
    SubstContext(TYPESYM ** prgtypeCls, int ctypeCls, TYPESYM ** prgtypeMeth, int ctypeMeth, SubstTypeFlagsEnum grfst = SubstTypeFlags::NormNone)
    {
        this->prgtypeCls = prgtypeCls;
        this->ctypeCls = ctypeCls;
        this->prgtypeMeth = prgtypeMeth;
        this->ctypeMeth = ctypeMeth;
        this->grfst = grfst;
    }

    bool FNop() { return !ctypeCls && !ctypeMeth && !(grfst & SubstTypeFlags::NormAll); }

    
    // Initializes a substitution context. Returns false iff no substitutions will ever be performed.
    __forceinline void Init(TypeArray *typeArgsCls, TypeArray *typeArgsMeth = NULL, SubstTypeFlagsEnum grfst = SubstTypeFlags::NormNone)
    {
        if (typeArgsCls) {
#ifdef DEBUG
            typeArgsCls->AssertValid();
#endif
            ctypeCls = typeArgsCls->size;
            prgtypeCls = typeArgsCls->ItemPtr(0);
        }
        else {
            ctypeCls = 0;
            prgtypeCls = NULL;
        }

        if (typeArgsMeth) {
#ifdef DEBUG
            typeArgsMeth->AssertValid();
#endif
            ctypeMeth = typeArgsMeth->size;
            prgtypeMeth = typeArgsMeth->ItemPtr(0);
        }
        else {
            ctypeMeth = 0;
            prgtypeMeth = NULL;
        }

        this->grfst = grfst;
    }
};


/* A symbol table is a helper class used by the symbol manager. There are
 * two symbol tables; a global and a local.
 */
class SYMTBL {
public:
    SYMTBL(ALLOCHOST * host, unsigned log2Buckets);
    ~SYMTBL();

    PSYM LookupSym(PNAME name, PPARENTSYM parent, symbmask_t kindmask);
    PSYM LookupNextSym(PSYM symPrev, PPARENTSYM parent, symbmask_t kindmask);
 
    void Clear();
    void Term();

    void InsertChild(PPARENTSYM parent, PSYM child);
    void ClearChildren(PPARENTSYM parent, symbmask_t kindmask);
    
private:

    // This special value markets a bucket as empty but previously occupied.
    // #define ksymDead ((SYM *)1)

    void InsertChildNoGrow(PSYM child);
    void RemoveChildFromBuckets(SYM * symOrphan);

    void GrowTable();
    unsigned Bucket(PNAME name, PPARENTSYM parent, unsigned * jump);

    PSYM * buckets;        // the buckets of the hash table.
    unsigned cBuckets;          // number of buckets
    unsigned bucketMask;        // mask, always cBuckets - 1.
    unsigned bucketShift;       // log2(cBuckets).
    unsigned cBucksUsed;        // number of buckets occupied (or marked as dead).
    ALLOCHOST * host;              // Containing host
};


/*
 * The local symbols manager
 */
class LSYMMGR 
{
public:
    LSYMMGR(ALLOCHOST * host, NRHEAP * heap);
    ~LSYMMGR();
    void Init();
    void Term();
    void DestroyLocalSymbols();
    void RemoveChildSyms(PPARENTSYM parent, symbmask_t kindmask);
    PSYM CreateLocalSym(SYMKIND symkind, PNAME name, PPARENTSYM parent);
    PSYM LookupLocalSym(PNAME name, PPARENTSYM parent, symbmask_t kindmask);
    static PSYM LookupNextSym(PSYM symPrev, PPARENTSYM parent, symbmask_t kindmask);

    void AddToLocalSymList(PSYM sym, PSYMLIST * * symLink);
    void AddToLocalNameList(PNAME name, PNAMELIST * * nameLink);
    void AddToLocalAttrList(BASENODE *attr, PARENTSYM *context, PATTRLIST * * attrLink);
    static void AddToSymList(NRHEAP *heap, PSYM sym, PSYMLIST * * symLink);


private:

    // add a symbol in the regular way into a symbol table
    static void AddChild(SYMTBL *tabl, PPARENTSYM parent, PSYM child);    

    NRHEAP *    allocLocal;
    SYMTBL      tableLocal;

    static void AddToNameList(NRHEAP *heap, PNAME name, PNAMELIST * * nameLink);
    static PSYM AllocSym(SYMKIND symkind, PNAME name, NRHEAP * allocator, int * psidLast);

    friend class BSYMMGR;
};


/*
 * The main symbol manager, restricted to functionality needed only in csee + (intersection of csee, cscomp)
 */
class BSYMMGR 
{
    friend void LSYMMGR::AddChild(SYMTBL *tabl, PPARENTSYM parent, PSYM child);
    friend PSYM LSYMMGR::AllocSym(SYMKIND symkind, PNAME name, NRHEAP * allocator, int * psidLast);
    friend void LSYMMGR::AddToSymList(NRHEAP *heap, PSYM sym, PSYMLIST * * symLink);
    friend void LSYMMGR::AddToNameList(NRHEAP *heap, PNAME name, PNAMELIST * * nameLink);
    friend void LSYMMGR::AddToLocalAttrList(BASENODE *attr, PARENTSYM *context, PATTRLIST * * nameLink);
public:

    BSYMMGR(NRHEAP * allocGlobal, ALLOCHOST * allocHost);

    ~BSYMMGR();
    void Init();
    void Term();

    bool InitPredefinedTypes();
    bool FPreLoad(NSSYM * ns, PCWCH pszAgg);

    /* Core general creation and lookup of symbols. */
    PSYM CreateGlobalSym(SYMKIND symkind, PNAME name, PPARENTSYM parent);

    SYM * LookupAggMember(NAME * name, AGGSYM * agg, symbmask_t mask);
    SYM * LookupGlobalSymCore(NAME * name, PARENTSYM * parent, symbmask_t mask);
    static SYM * LookupNextSym(SYM * symPrev, PARENTSYM * parent, symbmask_t mask);
    MISCSYM * LookupNextGlobalMiscSym(MISCSYM * prev, NAME * name, PARENTSYM * parent, MISCSYM::TYPE type);

    METHSYM * LookupInvokeMeth(AGGSYM * aggDel);

    /* Specific routines for specific symbol types. */
    PNSSYM CreateNamespace(PNAME name, PNSSYM parent);
    PALIASSYM CreateAlias(PNAME name);
    AGGSYM * CreateAgg(NAME * name, DECLSYM * declOuter);
    FWDAGGSYM * CreateFwdAgg(NAME * name, NSDECLSYM * nsd);
    AGGDECLSYM * CreateAggDecl(AGGSYM * agg, DECLSYM * declOuter);
    NSDECLSYM * CreateNamespaceDecl(NSSYM * ns, NSDECLSYM * nsdPar, INFILESYM * infile, NAMESPACENODE * parseTree);
    PMEMBVARSYM CreateMembVar(PNAME name, PAGGSYM parent, PAGGDECLSYM declaration);
    PTYVARSYM CreateTyVar(PNAME name, PPARENTSYM parent);
    PMETHSYM CreateMethod(PNAME name, PAGGSYM parent, PAGGDECLSYM declaration);
    PIFACEIMPLMETHSYM CreateIfaceImplMethod(PAGGSYM parent, PAGGDECLSYM declaration);
    PPROPSYM CreateProperty(PNAME name, PAGGSYM parent, PAGGDECLSYM declaration);
    PINDEXERSYM CreateIndexer(PNAME name, PAGGSYM parent, PAGGDECLSYM declaration);
    PEVENTSYM CreateEvent(PNAME name, PAGGSYM parent, PAGGDECLSYM declaration);
    MODULESYM * CreateModule(NAME * name, INFILESYM * infile);

    ARRAYSYM * GetArray(PTYPESYM elementType, int rank);
    AGGTYPESYM * GetInstAgg(AGGSYM * agg, AGGTYPESYM * atsOuter, TypeArray * typeArgs, TypeArray * typeArgsAll = NULL);
    AGGTYPESYM * GetInstAgg(AGGSYM * agg, TypeArray * typeArgsAll);
    PTRSYM * GetPtrType(TYPESYM * baseType);
    NUBSYM * GetNubType(TYPESYM * typeBase);
    TYPESYM * GetNubTypeOrError(TYPESYM * typeBase);
    PINNEDSYM * GetPinnedType(TYPESYM * baseType);
    PARAMMODSYM * GetParamModifier(TYPESYM * baseType, bool isOut);
    MODOPTTYPESYM * GetModOptType(TYPESYM * baseType, mdToken tokImport, MODULESYM * scope);
    MODOPTTYPESYM * GetModOptType(TYPESYM * baseType, MODOPTSYM * opt);
    NSAIDSYM * GetNsAid(NSSYM * ns, int aid);
    NSAIDSYM * GetRootNsAid(int aid)
        { return GetNsAid(rootNS, aid); }

    NUBSYM * GetNubFromNullable(AGGTYPESYM * ats);
    TYPESYM * MaybeConvertNullableToNub(TYPESYM * ats);
    TYPESYM * MaybeConvertNubToNullable(TYPESYM * nub);

    ERRORSYM * GetErrorType(PARENTSYM * symPar, NAME * nameText, TypeArray * typeArgs);

    TypeArray * AllocParams(int ctype, TYPESYM ** prgtype);
    TypeArray * ConcatParams(TypeArray * pta1, TypeArray * pta2);
    TypeArray * ConcatParams(TypeArray * pta, TYPESYM * type);
    TypeArray * ConcatParams(int ctype1, TYPESYM ** prgtype1, int ctype2, TYPESYM ** prgtype2);
    static TypeArray * EmptyTypeArray()
        { return &taEmpty; }

    /* Get special symbols */
    PVOIDSYM GetVoid()
        { return voidSym; }
    PNULLSYM GetNullType()
        { return nullType; }
    UNITSYM * GetUnitType()
        { return typeUnit; }
    ANONMETHSYM * GetAnonMethType()
        { return typeAnonMeth; }
    METHGRPSYM * GetMethGrpType()
        { return typeMethGrp; }
    PNSSYM GetRootNS()
        { return rootNS; }
    NSAIDSYM * GetGlobalNsAid()
        { return nsaGlobal; }
    PSCOPESYM GetFileRoot()
        { return fileroot; }
    POUTFILESYM GetMDFileRoot()
        { return mdfileroot; }
    PPARENTSYM GetXMLFileRoot()
        { return xmlfileroot; }
    PERRORSYM GetErrorSym()
        { return errorSym; }
    PAGGTYPESYM GetArglistSym()
        { return arglistSym; }
    PAGGTYPESYM GetNaturalIntSym()
        { return naturalIntSym; }
    PAGGSYM GetObject()    // return prefined object type
        { return GetReqPredefAgg(PT_OBJECT); }
    PAGGSYM GetNullable() // return prefined System.Nullable<> agg.
        { return GetOptPredefAgg(PT_G_OPTIONAL); }

    AGGSYM * GetReqPredefAgg(PREDEFTYPE pt); // Never returns NULL.
    AGGSYM * GetOptPredefAgg(PREDEFTYPE pt); // May return NULL.

    void ReportMissingPredefTypeError(PREDEFTYPE pt);

    int AidAlloc(SYM * sym) {
        if (sym->isINFILESYM() || sym->isEXTERNALIASSYM()) {
            return ssetAssembly.AddSym(sym) + kaidUnresolved;
        }
        ASSERT(sym->isMODULESYM() || sym->isOUTFILESYM());
        return ssetModule.AddSym(sym) + kaidMinModule;
    }

    BitSet bsetGlobalAssemblies; // Assemblies in the global alias.

    // Special Helper METHSYMs. These are found and set by FNCBIND. They are just cached here.
    AGGTYPESYM * atsDictionary;
    METHSYM * methDictionaryCtor;
    METHSYM * methDictionaryAdd;
    METHSYM * methDictionaryTryGetValue;
    METHSYM * methStringEquals;
    METHSYM * methInitArray;
    METHSYM * methStringOffset;

    // Special nullable members.
    PROPSYM * propNubValue;
    PROPSYM * propNubHasValue;
    METHSYM * methNubGetValOrDef;
    METHSYM * methNubCtor;

    CONSTVAL GetPredefZero(PREDEFTYPE pt);
    PREDEFATTR GetPredefAttr(AGGTYPESYM * type);
    PAGGTYPESYM GetObjectType()
        { return GetObject()->getThisType(); }

    PINFILESYM FindInfileSym(NAME * filename);
    PINFILESYM GetInfileForAid(int aid);
    PSYM GetSymForAid(int aid);
    
    BYTE GetElementType(PAGGTYPESYM type);
    static PCWSTR GetNiceName(PAGGSYM type);
    static PCWSTR GetNiceName(PREDEFTYPE pt);
    static int GetAttrArgSize(PREDEFTYPE pt);
    static PCWSTR GetFullName(PREDEFTYPE pt);
    static PREDEFTYPE GetPredefIndex(CorElementType et);
    static FUNDTYPE GetPredefFundType(PREDEFTYPE pt);

    TYVARSYM * GetStdMethTypeVar(int iv)
        { return stvcMethod.GetTypeVarSym(iv, this, true); }
    TYVARSYM * GetStdClsTypeVar(int iv)
        { return stvcClass.GetTypeVarSym(iv, this, false); }
    TYVARSYM * GetStdTypeVar(int iv, bool fMeth)
        { return fMeth ? stvcMethod.GetTypeVarSym(iv, this, true) : stvcClass.GetTypeVarSym(iv, this, false); }

    TYPESYM * SubstType(TYPESYM *typeSrc, SubstContext * pctx)
        { return (!pctx || pctx->FNop()) ? typeSrc : SubstTypeCore(typeSrc, pctx); }
    TYPESYM * SubstType(TYPESYM *typeSrc, TypeArray *typeArgsCls, TypeArray *typeArgsMeth = NULL, SubstTypeFlagsEnum grfst = SubstTypeFlags::NormNone);
    TYPESYM * SubstType(TYPESYM *typeSrc, AGGTYPESYM *atsCls, TypeArray *typeArgsMeth = NULL)
        { return SubstType(typeSrc, atsCls ? atsCls->typeArgsAll : NULL, typeArgsMeth); }
    TYPESYM * SubstType(TYPESYM *typeSrc, TYPESYM *typeCls, TypeArray *typeArgsMeth = NULL)
        { return SubstType(typeSrc, typeCls->isAGGTYPESYM() ? typeCls->asAGGTYPESYM()->typeArgsAll : NULL, typeArgsMeth); }
    TYPESYM * SubstType(TYPESYM *typeSrc, SubstTypeFlagsEnum grfst)
        { return SubstType(typeSrc, (TypeArray *)NULL, (TypeArray *)NULL, grfst); }

    bool SubstEqualTypes(TYPESYM *typeDst, TYPESYM *typeSrc, SubstContext * pctx);
    bool SubstEqualTypes(TYPESYM *typeDst, TYPESYM *typeSrc, TypeArray *typeArgsCls, TypeArray *typeArgsMeth = NULL, SubstTypeFlagsEnum grfst = SubstTypeFlags::NormNone);
    bool SubstEqualTypes(TYPESYM *typeDst, TYPESYM *typeSrc, AGGTYPESYM *atsCls, TypeArray *typeArgsMeth = NULL)
        { return SubstEqualTypes(typeDst, typeSrc, atsCls ? atsCls->typeArgsAll : NULL, typeArgsMeth); }
    bool SubstEqualTypes(TYPESYM *typeDst, TYPESYM *typeSrc, TYPESYM *typeCls, TypeArray *typeArgsMeth = NULL)
        { return SubstEqualTypes(typeDst, typeSrc, typeCls->isAGGTYPESYM() ? typeCls->asAGGTYPESYM()->typeArgsAll : NULL, typeArgsMeth); }

    TypeArray * SubstTypeArray(TypeArray *taSrc, SubstContext * pctx);
    TypeArray * SubstTypeArray(TypeArray *taSrc, TypeArray *typeArgsCls, TypeArray *typeArgsMeth = NULL, SubstTypeFlagsEnum grfst = SubstTypeFlags::NormNone);
    TypeArray * SubstTypeArray(TypeArray *taSrc, AGGTYPESYM *atsCls, TypeArray *typeArgsMeth = NULL)
        { return SubstTypeArray(taSrc, atsCls ? atsCls->typeArgsAll : NULL, typeArgsMeth); }
    TypeArray * SubstTypeArray(TypeArray *taSrc, TYPESYM *typeCls, TypeArray *typeArgsMeth = NULL)
        { return SubstTypeArray(taSrc, typeCls->isAGGTYPESYM() ? typeCls->asAGGTYPESYM()->typeArgsAll : NULL, typeArgsMeth); }
    TypeArray * SubstTypeArray(TypeArray *taSrc, SubstTypeFlagsEnum grfst)
        { return SubstTypeArray(taSrc, (TypeArray *)NULL, (TypeArray *)NULL, grfst); }

    bool SubstEqualTypeArrays(TypeArray *taDst, TypeArray *taSrc, SubstContext * pctx);
    bool SubstEqualTypeArrays(TypeArray *taDst, TypeArray *taSrc, TypeArray *typeArgsCls, TypeArray *typeArgsMeth = NULL,
        SubstTypeFlagsEnum grfst = SubstTypeFlags::NormNone);
    bool SubstEqualTypeArrays(TypeArray *taDst, TypeArray *taSrc, AGGTYPESYM *atsCls, TypeArray *typeArgsMeth = NULL)
        { return SubstEqualTypeArrays(taDst, taSrc, atsCls ? atsCls->typeArgsAll : NULL, typeArgsMeth); }

    bool static TypeContainsType(TYPESYM *type, TYPESYM *typeFind);
    bool static TypeContainsTyVars(TYPESYM * type, TypeArray * typeVars);
    bool static TypeContainsGenerics(TYPESYM * type);

    struct UnifyContext : SubstContext {
        // TYPESYM ** prgtypeCls;   // [in, out] The unification mapping for the class type variables. Inherited.
        // TYPESYM ** prgtypeMeth;  // [in, out] The unification mapping for the method type variables. Inherited.
        TypeArray * typeVarsCls;  // [in] The class type variables.
        TypeArray * typeVarsMeth; // [in] The method type variables.

        UnifyContext(TypeArray * typeVarsCls, TypeArray * typeVarsMeth, TYPESYM ** prgtypeCls, TYPESYM ** prgtypeMeth)
            : SubstContext(prgtypeCls, typeVarsCls->size, prgtypeMeth, typeVarsMeth->size)
        {
            this->typeVarsCls = typeVarsCls;
            this->typeVarsMeth = typeVarsMeth;
            Clear();
        }

        TYPESYM ** GetSlot(TYVARSYM * tvs); // Gets the map slot for the type var. Returns NULL if tvs is invalid.
        void Clear() {
            ASSERT(typeVarsCls);
            ASSERT(typeVarsMeth);
            memcpy(prgtypeCls, typeVarsCls->ItemPtr(0), typeVarsCls->size * sizeof(prgtypeCls[0]));
            memcpy(prgtypeMeth, typeVarsMeth->ItemPtr(0), typeVarsMeth->size * sizeof(prgtypeMeth[0]));
        }
    };
    bool UnifyTypes(TYPESYM * t1, TYPESYM * t2, UnifyContext * pctx);

    struct InferContext {
        TypeArray * typeVarsMeth; // [in] The method type variables.
        TypeArray * typeArgsCls;  // [in] The class type arguments - used to map typeSrc as we go.
        TYPESYM ** prgtypeMeth;   // [in, out] The unification mapping for the method type variables.

        TYPESYM ** GetSlot(TYVARSYM * tvs); // Gets the map slot for the type var. Returns NULL if tvs is invalid.
    };
    bool InferTypes(TYPESYM *typeSrc, TYPESYM *typeDst, InferContext *pctx);
    void static SetCanInferState(METHSYM * meth);

    int CompareTypes(TypeArray * ta1, TypeArray * ta2);

    TypeArray * BuildIfacesAll(SYM * symErr, AGGTYPESYM ** prgiface, int ciface, UnifyContext * pctx);

    bool CheckForUnifyingInstantiation(SYM *symErr, AGGTYPESYM ** prgtype, int ctype, AGGTYPESYM *iface, UnifyContext * pctx, bool * pfErrors);
    bool AddUniqueInterfaces(SYM * symErr, AGGTYPESYM ** ptypeMin, AGGTYPESYM *** pptype, AGGTYPESYM ** ptypeLim, AGGTYPESYM * iface, UnifyContext * pctx);
#ifdef DEBUG
    void DbgCheckIfaceListOrder(AGGTYPESYM * iface, TypeArray * ifacesAll, bool fCheckTypes);
#endif // DEBUG

    void AddToGlobalSymList(PSYM sym, PSYMLIST * * symLink);
    void AddToGlobalNameList(PNAME name, PNAMELIST * * nameLink);
    void AddToGlobalAttrList(BASENODE *ptr, PARENTSYM *context, PATTRLIST * * attrLink);

    BSYMHOST * host();



#ifdef DEBUG
    void DumpSymbol(PSYM sym, int indent = 0);
    void DumpType(PTYPESYM sym);
#endif



    static void AddToSymList(MEMHEAP *heap, PSYM sym, PSYMLIST * symLink);
    static void FreeSymList(MEMHEAP *heap, PSYMLIST * symLink);

    NRHEAP * getAlloc() { return allocGlobal; }
    TokenToSymTable * GetTokenToSymTable() { return &tableTokenToSym; }
    NameToSymTable * GetNameToSymTable() { return &tableNameToSym; }

    void SetAidForMsCorLib(int aid) {
        ASSERT(aidMsCorLib == kaidNil);
        aidMsCorLib = aid;
    }

    NAME * BSYMMGR::GetNameFromPtrs(UINT_PTR u1, UINT_PTR u2);

protected:

    SYMMGR * getSymmgr() { return (SYMMGR*)(void*)this;} 

protected:
    // add a symbol in the regular way into a symbol table
    static void AddChild(SYMTBL *tabl, PPARENTSYM parent, PSYM child);

    NRHEAP *    allocGlobal;
    SYMTBL      tableGlobal;

    PAGGTYPESYM arglistSym;
    PAGGTYPESYM naturalIntSym;
    PERRORSYM   errorSym;
    PVOIDSYM    voidSym;
    PNULLSYM    nullType;
    UNITSYM *   typeUnit;
    ANONMETHSYM * typeAnonMeth;
    METHGRPSYM * typeMethGrp;
    PNSSYM      rootNS;                 // The "root" (unnamed) namespace.
    NSAIDSYM *  nsaGlobal;              // global::
    PSCOPESYM   fileroot;               // All output and input file symbols rooted here.
    PredefTypeNameInfo * prgptni;
    PAGGSYM *   predefSyms;             // array of predefined symbol types.
    PAGGSYM *   arrayMethHolder;
    POUTFILESYM mdfileroot;             // The dummy output file for all imported metadata files
    PPARENTSYM  xmlfileroot;            // The dummy output file for all included XML files
    SymSet ssetAssembly;       // Map from aids to INFILESYMs and EXTERNALIASSYMs
    SymSet ssetModule;         // Map from aids to MODULESYMs and OUTFILESYMs
    int aidMsCorLib; // The assembly ID for all predefined types.
    TokenToSymTable tableTokenToSym;
    NameToSymTable tableNameToSym;

    // Standard method and class type variables, ie, !0, !1, !!0, !!1, etc.
    struct StdTypeVarColl {
        int ctvs;
        PTYVARSYM * prgptvs;

        StdTypeVarColl() {
            ctvs = 0;
            prgptvs = NULL;
        }
        PTYVARSYM GetTypeVarSym(int iv, BSYMMGR * pbsm, bool fMeth);
    };
    StdTypeVarColl stvcMethod;
    StdTypeVarColl stvcClass;

    PTYVARSYM GetStdTypeVar(int iv, StdTypeVarColl * pstvc);

    // The hash table for type arrays.
    TypeArrayTable tableTypeArrays;
    static TypeArray taEmpty;

    TYPESYM * SubstTypeCore(TYPESYM * type, SubstContext * pctx);
    bool SubstEqualTypesCore(TYPESYM * typeDst, TYPESYM * typeSrc, SubstContext * pctx);
    TYPESYM * SubstTypeSingle(TYPESYM * type, TYPESYM * typeSrc, TYPESYM * typeDst);

    bool static InferTypesEqual(TYPESYM * typeSrc, TYPESYM * typeDst, InferContext * pctx);
    bool static InferTypesTyVar(TYPESYM * typeSrc, TYVARSYM * varDst, InferContext * pctx);
    bool InferTypesAgg(TYPESYM * typeSrc, AGGTYPESYM * atsDst, InferContext * pctx);
    bool InferTypesAggCore(TYPESYM *typeSrc, AGGTYPESYM *atsDst, InferContext *pctx, bool * pfInferred);
    bool static InferTypesAggSingle(AGGTYPESYM * atsSrc, AGGTYPESYM * atsDst, InferContext * pctx, bool * pfInferred);

    PSYM AllocSym(SYMKIND symkind, PNAME name, NRHEAP * allocator);
    static PSYM AllocSymWorker(SYMKIND symkind, PNAME name, NRHEAP * allocator, int * psidLast);
    static void AddToSymList(NRHEAP *heap, PSYM sym, PSYMLIST * * symLink);
    static void AddToNameList(NRHEAP *heap, PNAME name, PNAMELIST * * nameLink);
    static void AddToAttrList(NRHEAP *heap, BASENODE *attr, PARENTSYM *context, PATTRLIST * * attrLink);

    void InitPreLoad();
    AGGSYM * FindPredefinedType(PCWSTR typeName, int aid, AggKindEnum aggKind, int arity, bool isRequired);
    AGGSYM * FindPredefinedTypeCore(NAME * name, NSSYM * ns, int aid, AggKindEnum aggKind, int arity,
        AGGSYM ** paggAmbig, AGGSYM ** paggBad);

    void Error(int id, ErrArg arg);
    void Error(int id, ErrArg arg1, ErrArg arg2);
    void Error(int id, ErrArg arg1, ErrArg arg2, ErrArg arg3);
    void ErrorRef(int id, ErrArgRef arg);
    void ErrorRef(int id, ErrArgRef arg1, ErrArgRef arg2);
    void ErrorRef(int id, ErrArgRef arg1, ErrArgRef arg2, ErrArgRef arg3);

#ifdef DEBUG

    void DumpChildren(PPARENTSYM sym, int indent);
    void DumpAccess(ACCESS acc);
    void DumpConst(PTYPESYM type, CONSTVAL * constVal);
#endif //DEBUG


};

/*
 *   This is the outersymbol manager used in the cscomp case.  It has methods not needed in the csee case.
 */
class SYMMGR : private BSYMMGR
{
public:
    SYMMGR(NRHEAP * allocGlobal);
    ~SYMMGR() {};

    BSYMMGR & getBSymmgr() { return *(SYMMGR*)this;}

    PINFILESYM CreateSourceFile(PCWSTR filename, OUTFILESYM *outfile);
    PINFILESYM CreateSynthSourceFile(PCWSTR filename, OUTFILESYM *outfile);
    PGLOBALATTRSYM CreateGlobalAttribute(PNAME name, NSDECLSYM *parent);
    POUTFILESYM CreateOutFile(PCWSTR filename, BOOL isDll, BOOL isWinApp, PCWSTR entrySym, PCWSTR resource, PCWSTR icon, PCWSTR pdbfile);
    PRESFILESYM CreateSeperateResFile(PCWSTR filename, OUTFILESYM *outfileSym, PCWSTR Ident, bool bVisible);
    PRESFILESYM CreateEmbeddedResFile(PCWSTR filename, PCWSTR Ident, bool bVisible);
    PINFILESYM CreateMDFile(PCWSTR filename, int aid, PARENTSYM *parent);
    void SetOutFileName(PINFILESYM in);

    COMPILER * compiler();

    // add a symbol into a symbol table
    void AddOrphanedChild(PPARENTSYM parent, PSYM child) {
        ASSERT(parent->asAGGSYM()->isFabricated && child->parent == NULL);
        AddChild( &tableGlobal, parent, child);
    }

#ifdef DEBUG
    PINFILESYM GetPredefInfile();

private:
    PINFILESYM  predefInputFile;        // dummy inputfile for testing purposes only

#endif //DEBUG

};

__forceinline AGGSYM * BSYMMGR::GetReqPredefAgg(PREDEFTYPE pt)
{
    ASSERT(pt >= 0 && pt < PT_COUNT);
    ASSERT(predefTypeInfo[pt].isRequired);
    ASSERT(predefSyms[pt]);
    return predefSyms[pt];
}

__forceinline AGGSYM * BSYMMGR::GetOptPredefAgg(PREDEFTYPE pt)
{
    ASSERT(pt >= 0 && pt < PT_COUNT);
    return predefSyms[pt];
}

__forceinline AGGTYPESYM * AGGTYPESYM::GetBaseClass()
{
    if (!baseType)
        baseType = getAggregate()->symmgr->SubstType(getAggregate()->baseClass, typeArgsAll)->asAGGTYPESYM();
    return baseType;
}

__forceinline TypeArray * AGGTYPESYM::GetIfacesAll()
{
    if (!ifacesAll)
        ifacesAll = getAggregate()->symmgr->SubstTypeArray(getAggregate()->ifacesAll, typeArgsAll);
    return ifacesAll;
}

#endif // __symmgr_h__
