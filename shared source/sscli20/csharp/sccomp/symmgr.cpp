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
// File: symmgr.cpp
//
// Routines for storing and handling symbols.
// ===========================================================================


#include "stdafx.h"

// This special value markets a bucket (in SYMTBL) as available but previously occupied.
// This is so we know to keep looking past this bucket.
#define ksymDead ((SYM *)1)

/*
 * Static array of info about the predefined types.
 */
#define PREDEFTYPEDEF(id, name, isRequired, isSimple, isNumeric, kind, ft, et, nicenm, zero, qspec, asize, st, attr, arity, inmscorlib) \
                     { L##name, isRequired, isSimple, isNumeric, qspec, ft, et, AggKind::kind, nicenm, { { (INT_PTR)(zero) } }, attr, asize, arity, inmscorlib },

const static double doubleZero = 0;
const static __int64 longZero = 0;
#if BIGENDIAN
const static DECIMAL decimalZero = { { { 0 } }, 0 };
#else   // BIGENDIAN
const static DECIMAL decimalZero = { 0, { { 0 } } };
#endif  // BIGENDIAN

const PredefTypeInfo predefTypeInfo[] =
{
    #include "predeftype.h"
};

TypeArray BSYMMGR::taEmpty;

/***************************************************************************************************
    Gets the one and only TypeArray for this array of types.
***************************************************************************************************/
TypeArray * TypeArrayTable::AllocTypeArray(int ctype, TYPESYM ** prgtype)
{
    ASSERT(ctype > 0 && prgtype != NULL);

    Key key(ctype, prgtype);
    uint hash = NAMEMGR::HashString((WCHAR *)prgtype, ctype * sizeof(prgtype[0]) / sizeof(WCHAR));

    Entry ** ppenIns;
    TypeArrayEntry * ptae = FindEntry<TypeArrayEntry, Key *>(&key, hash, &ppenIns);

    if (!ptae) {
        // Typearray not found; create new and add it to the table.
        ptae = (TypeArrayEntry *)m_pmgr->getAlloc()->AllocZero(sizeof(TypeArrayEntry) + ctype * sizeof(TYPESYM *));

        // Fill in the new typearray structure and copy in the array.
        ptae->hash = hash;
        ptae->ta.size = ctype;

        ASSERT(!ptae->ta.fHasErrors && !ptae->ta.fUnres);

        // Copy the types and set array wide information.
        TYPESYM ** ptype = ptae->ta.ItemPtr(0);
        for (int itype = 0; itype < ctype; itype++, ptype++) {
            ASSERT(prgtype[itype]);
            *ptype = prgtype[itype];
            if ((*ptype)->HasErrors())
                ptae->ta.fHasErrors = true;
            if ((*ptype)->IsUnresolved())
                ptae->ta.fUnres = true;
        }
        InsertEntry<TypeArrayEntry>(ptae, ppenIns);
    }

    // Return the type array.
    ASSERT(hash == ptae->hash);
    ASSERT(!memcmp(ptae->ta.ItemPtr(0), prgtype, ctype * sizeof(TYPESYM *)));

    return &ptae->ta;
}


/***************************************************************************************************
    Init the key, including setting the hash value.
***************************************************************************************************/
TokenToSymTable::Key::Key(MODULESYM * module, mdToken tok)
{
    this->module = module;
    this->tok = tok;
    hash = HashUInt((UINT_PTR)this->module) + HashUInt((UINT_PTR)this->tok);
}




/***************************************************************************************************
    Get the SYM for tok in (infile, iscope). Return false iff it's never been set.
***************************************************************************************************/
bool TokenToSymTable::GetSymFromToken(MODULESYM * pscope, mdToken tok, SYM ** psym)
{
    Key key(pscope, tok);

    EntryData * pend = FindEntry<EntryData, Key *>(&key, key.hash);

    if (!pend) {
        *psym = NULL;
        return false;
    }


    *psym = pend->sym;
    return true;
}


/***************************************************************************************************
    Set the SYM for tok in (infile, iscope). Asserts that there isn't already a mapping.
    The sym may be NULL indicating that there was a previous failure identifying the sym.
***************************************************************************************************/
void TokenToSymTable::SetSymForToken(MODULESYM * pscope, mdToken tok, SYM * sym)
{
    Key key(pscope, tok);

    Entry ** ppenIns;
    EntryData * pend = FindEntry<EntryData, Key *>(&key, key.hash, &ppenIns);

    if (!pend) {
        pend = (EntryData *)m_pmgr->getAlloc()->AllocZero(sizeof(EntryData));

        // Fill in the new entry and insert it.
        pend->key = key;
        InsertEntry<EntryData>(pend, ppenIns);
    }

    ASSERT(!pend->sym || !"Why is the sym being changed for this (module, tok)?");
    pend->sym = sym;
}


/***************************************************************************************************
    Get the SYM for name. Return false iff it's never been set.
***************************************************************************************************/
bool NameToSymTable::GetSymFromName(NAME * name, SYM ** psym)
{
    EntryData * pend = FindEntry<EntryData, NAME *>(name, name->hash);

    if (!pend) {
        *psym = NULL;
        return false;
    }


    *psym = pend->sym;
    return true;
}


/***************************************************************************************************
    Set the SYM for name. Asserts that there isn't already a mapping.
    The sym may be NULL indicating that there was a previous failure identifying the sym.
***************************************************************************************************/
void NameToSymTable::SetSymForName(NAME * name, SYM * sym)
{
    Entry ** ppenIns;
    EntryData * pend = FindEntry<EntryData, NAME *>(name, name->hash, &ppenIns);

    if (!pend) {
        pend = (EntryData *)m_pmgr->getAlloc()->AllocZero(sizeof(EntryData));

        // Fill in the new entry and insert it.
        pend->name = name;
        InsertEntry<EntryData>(pend, ppenIns);
    }

    ASSERT(!pend->sym || !"Why is the sym being changed for this name?");
    pend->sym = sym;
}



/*
 * Given a symbol, get its parse tree
 */
BASENODE * SYM::getParseTree()
{
    switch (kind) {
    case SK_METHSYM:
    case SK_PROPSYM:
        return asMETHPROPSYM()->parseTree;
    case SK_MEMBVARSYM:
        return asMEMBVARSYM()->parseTree;
    case SK_TYVARSYM:
        return asTYVARSYM()->parseTree;
    case SK_EVENTSYM:
        return asEVENTSYM()->parseTree;
    case SK_AGGDECLSYM:
        return asAGGDECLSYM()->parseTree;
    case SK_NSDECLSYM:
        return asNSDECLSYM()->parseTree;
    case SK_ALIASSYM:
        return asALIASSYM()->parseTree;
    case SK_GLOBALATTRSYM:
        return asGLOBALATTRSYM()->parseTree;
    case SK_LOCVARSYM:
        return asLOCVARSYM()->declTree;

    case SK_ERRORSYM:
        return NULL;

    case SK_NSSYM:
    case SK_NSAIDSYM:
        return NULL;

    case SK_LABELSYM:
        return NULL;

    case SK_PTRSYM:
    case SK_NUBSYM:
    case SK_ARRAYSYM:
    case SK_PINNEDSYM:
    case SK_PARAMMODSYM:
    case SK_MODOPTTYPESYM:
    case SK_AGGTYPESYM:
    case SK_AGGSYM:  // an AGGSYM could have multiple parse trees, due to partial classes!
    default:
        // should never call this with any other type
        ASSERT(false);
        return NULL;
    }
}

/*
 * Given a symbol, get its parse tree
 */
BASENODE * SYM::GetSomeParseTree()
{
    switch (kind) {
    case SK_METHSYM:
    case SK_PROPSYM:
        return asMETHPROPSYM()->parseTree;
    case SK_MEMBVARSYM:
        return asMEMBVARSYM()->parseTree;
    case SK_TYVARSYM:
        return asTYVARSYM()->parseTree;
    case SK_EVENTSYM:
        return asEVENTSYM()->parseTree;
    case SK_AGGDECLSYM:
        return asAGGDECLSYM()->parseTree;
    case SK_NSDECLSYM:
        return asNSDECLSYM()->parseTree;
    case SK_ALIASSYM:
        return asALIASSYM()->parseTree;

    case SK_AGGSYM:
        return asAGGSYM()->DeclFirst()->parseTree;

    case SK_LOCVARSYM:
        return asLOCVARSYM()->declTree;

    default:
        if (this->parent)
            return this->parent->GetSomeParseTree();
        return NULL;
    }
}

/*
 * Given a symbol, returns the address of its metadata token for emitting
 * or NULL if this symbol is not emitted
 */
mdToken *SYM::getTokenEmitPosition()
{
    switch (kind) {
    case SK_METHSYM:
        return &asMETHSYM()->tokenEmit;

    case SK_PROPSYM:
        return &asPROPSYM()->tokenEmit;

    case SK_MEMBVARSYM:
        return &asMEMBVARSYM()->tokenEmit;

    case SK_AGGSYM:
        return &asAGGSYM()->tokenEmit;

    case SK_EVENTSYM:
        return &asEVENTSYM()->tokenEmit;

    default:
        ;
    }

    return NULL;
}

/*
 * Given a symbol, returns the address of its metadata token for emitting
 * or NULL if this symbol is not emitted
 */
mdToken SYM::getTokenEmit()
{
    switch (kind) {
    case SK_METHSYM:
        return asMETHSYM()->tokenEmit;

    case SK_PROPSYM:
        return asPROPSYM()->tokenEmit;

    case SK_MEMBVARSYM:
        return asMEMBVARSYM()->tokenEmit;

    case SK_TYVARSYM:
        return mdTokenNil;

    case SK_EVENTSYM:
        return asEVENTSYM()->tokenEmit;

    case SK_AGGSYM:
        return asAGGSYM()->tokenEmit;

    case SK_AGGDECLSYM:
        return asAGGDECLSYM()->Agg()->tokenEmit;

    default:
        ASSERT(!"Bad Symbol type");
    }

    return mdTokenNil;
}

mdToken SYM::getTokenImport()
{
    switch (kind) {
    case SK_AGGSYM:
        return asAGGSYM()->tokenImport;

    case SK_METHSYM:
        return asMETHSYM()->tokenImport;

    case SK_PROPSYM:
        return asPROPSYM()->tokenImport;

    case SK_MEMBVARSYM:
        return asMEMBVARSYM()->tokenImport;

    case SK_EVENTSYM:
        return asEVENTSYM()->tokenImport;

    case SK_TYVARSYM:
        return mdTokenNil;

    case SK_AGGDECLSYM:
        return asAGGDECLSYM()->Agg()->tokenImport;

    default:
        return mdTokenNil;
    }
}

BASENODE * SYM::getAttributesNode()
{
    switch (kind) {
    case SK_METHSYM:
        return asMETHSYM()->getAttributesNode();

    case SK_AGGDECLSYM:
        return asAGGDECLSYM()->getAttributesNode();

    case SK_MEMBVARSYM:
        return asMEMBVARSYM()->getAttributesNode();

    case SK_TYVARSYM:
        return asTYVARSYM()->getAttributesNode();

    case SK_PROPSYM:
        return asPROPSYM()->getAttributesNode();

    case SK_EVENTSYM:
        return asEVENTSYM()->getAttributesNode();

    default:
        ASSERT(!"Bad Symbol Type");
        return 0;
    }
}

bool    SYM::isContainedInDeprecated() const
{
    if (this == 0)
        return false;

    if (this->isDeprecated)
        return true;

    if (parent && (parent->isAGGSYM()))
        return parent->isContainedInDeprecated();

    return false;
}


void PARENTSYM::AddToChildList(SYM * sym)
{
    ASSERT(sym && this);

    // If parent is set it should be set to us!
    ASSERT(!sym->parent || sym->parent == this);
    // There shouldn't be a nextChild.
    ASSERT(!sym->nextChild);

    if (!psymAttachChild) {
        ASSERT(!firstChild);
        psymAttachChild = &firstChild;
    }
    else {
#ifdef DEBUG
    // Validate our chain.
        SYM ** psym;
        int count = 100; // Limited the length of chain that we'll run.
        for (psym = &firstChild; *psym && --count; )
            psym = &(*psym)->nextChild;
        ASSERT(psymAttachChild == psym || !count);
#endif
    }

    *psymAttachChild = sym;
    psymAttachChild = &sym->nextChild;
    *psymAttachChild = NULL;
    sym->parent = this;
}


void PARENTSYM::RemoveFromChildList(SYM * sym)
{
    ASSERT(sym && sym->parent == this);

    for (SYM ** psym = &firstChild; *psym; psym = &(*psym)->nextChild) {
        if (*psym == sym) {
            // Remove it from the list
            *psym = sym->nextChild;

            // psymAttachChild should point to the sym's nextChild field iff the sym
            // is at the end of the list.
            ASSERT((psymAttachChild == &sym->nextChild) == !*psym);
            if (!*psym)
                psymAttachChild = psym;

            // Update the sym so it has no siblings or parent.
            sym->nextChild = NULL;
            sym->parent = NULL;

            return;
        }
    }
    ASSERT(!"sym not found in RemoveFromChildList!");
}


// Compare to PARENTSYM::AddToChildList
void BAGSYM::AddDecl(DECLSYM * decl, NRHEAP * heap)
{
    ASSERT(decl && this);
    ASSERT(this->isNSSYM() || this->isAGGSYM());
    ASSERT(decl->isNSDECLSYM() || decl->isAGGDECLSYM());
    ASSERT(!this->isNSSYM() == !decl->isNSDECLSYM());

    // If parent is set it should be set to us!
    ASSERT(!decl->bag || decl->bag == this);
    // There shouldn't be a declNext.
    ASSERT(!decl->declNext);

    if (!pdeclAttach) {
        ASSERT(!declFirst);
        pdeclAttach = &declFirst;
    }
    else {
#ifdef DEBUG
    // Validate our chain.
        DECLSYM ** pdecl;
        for (pdecl = &declFirst; *pdecl; )
            pdecl = &(*pdecl)->declNext;
        ASSERT(pdeclAttach == pdecl);
#endif
    }

    *pdeclAttach = decl;
    pdeclAttach = &decl->declNext;
    *pdeclAttach = NULL;
    decl->bag = this;

    if (decl->isNSDECLSYM())
        decl->asNSDECLSYM()->Bag()->DeclAdded(decl->asNSDECLSYM(), heap);
}



void NSSYM::DeclAdded(NSDECLSYM * decl, NRHEAP * heap)
{
    ASSERT(decl->Bag() == this);
    ASSERT(this->pdeclAttach == &decl->declNext);

    INFILESYM * infile = decl->getInputFile();

    if (infile->isSource) {
        bsetFilter.SetBit(kaidGlobal, heap);
        bsetFilter.SetBit(kaidThisAssembly, heap);
    }
    else {
        infile->UnionAliasFilter(bsetFilter, heap);
    }
}

bool TYPESYM::CanBeVolatile ()
{
    FUNDTYPE ft = fundType();

    bool result = (ft == FT_REF || 
                    ft == FT_PTR ||
                    ft == FT_R4 ||
                    ft == FT_I4 || 
                    ft == FT_U4 ||
                    ft == FT_I2 || 
                    ft == FT_U2 ||
                    ft == FT_I1 || 
                    ft == FT_U1 ||
                    (ft == FT_VAR && asTYVARSYM()->IsRefType()));

    if (!result && isPredefined ())
    {
        PREDEFTYPE pt = getPredefType();
        result = (pt == PT_INTPTR || pt == PT_UINTPTR);
    }

    return result;
}

AGGTYPESYM * AGGSYM::getThisType()
{
    if (!atsInst) {
        ASSERT(typeVarsThis == typeVarsAll || isNested());
        atsInst = symmgr->GetInstAgg(this, isNested() ? GetOuterAgg()->getThisType() : NULL,
            typeVarsThis, typeVarsAll);
    }
    ASSERT(typeVarsThis->size == atsInst->typeArgsThis->size);
    return atsInst;
}


AGGTYPESYM * AGGTYPESYM::FindBaseType(AGGSYM * agg)
{
    for (AGGTYPESYM * ats = this; ats; ats = ats->GetBaseClass()) {
        if (ats->getAggregate() == agg)
            return ats;
    }
    return NULL;
}


void AGGSYM::InitFromOuterDecl(DECLSYM * declOuter)
{
    infile = declOuter->getInputFile();
    isSource = infile->isSource;
}


bool AGGSYM::FindBaseAgg(AGGSYM * agg)
{
    for (AGGSYM * aggT = this; aggT; aggT = aggT->GetBaseAgg()) {
        if (aggT == agg)
            return true;
    }
    return false;
}

bool AGGSYM::IsCLRAmbigStruct()
{
    if (!this->isPredefined)
        return this->IsEnum();

    switch (getThisType()->getPredefType()) {
    case PT_BYTE:
    case PT_SHORT:
    case PT_INT:
    case PT_LONG:
    case PT_CHAR:
    case PT_BOOL:
    case PT_SBYTE:
    case PT_USHORT:
    case PT_UINT:
    case PT_ULONG:
    case PT_INTPTR:
    case PT_UINTPTR:
    case PT_FLOAT:
    case PT_DOUBLE:
    case PT_TYPEHANDLE:
    case PT_FIELDHANDLE:
    case PT_METHODHANDLE:
    case PT_ARGUMENTHANDLE:
        return true;
    default:
        return false;
    }
}


/*
 *  Given a type forwarder, resolve it so that we know which AGGSYM in which assembly it points to.
 *  After resolving the type forwarder, the target assembly ID and AGGSYM will point to the final valid AGGSYM, 
 *  so in the case where multiple FWDAGGSYMs are chained together, once the FWDAGGSYM is resolved, there will be no context of the path 
 *  used to get to the final AGGSYM (aside from the assemblyRef token, which could be used to start back at the beginning).
 */
void FWDAGGSYM::Resolve(COMPILER * compiler)
{
    ASSERT(!m_aggRes && !m_moduleBreak);

    ImportScopeModule scope(&compiler->importer, module); 
    int aid = compiler->importer.MapAssemblyRefToAid(scope, tkAssemblyRef);
    ASSERT(aid != kaidUnresolved); // MapAssemblyRefToAid will create a fake module if the assembly does not exist.
    ASSERT(!this->InAlias(aid));

    if (m_fResolving) {
        // This is being resolved so this must be a cycle in the type forwarder list.
        // Thus, m_aggRes should not be set.
        compiler->Error(NULL, ERR_CycleInTypeForwarder, this->name, ErrArg(module));
        m_moduleBreak = module;
        m_tokBreak = tkAssemblyRef;
        m_fCycle = true;
    }
    // Type forwarders always point to assemblies, if the target AID is a module, then it means MapAssemblyRefToAid
    // created one for us because the actual assembly does not exist.
    else if (aid < kaidMinModule) {
        m_fResolving = true;

        FWDAGGSYM * fwdTmp = NULL;
        for (SYM * sym = compiler->LookupInBagAid(this->name, this->parent->asNSSYM(), aid, MASK_AGGSYM | MASK_FWDAGGSYM);
            sym;
            sym = compiler->LookupNextInAid(sym, aid, MASK_AGGSYM | MASK_FWDAGGSYM))
        {
            ASSERT(sym->isAGGSYM() || sym->isFWDAGGSYM());
            // check arity to make sure this is the correct one.
            if (sym->isAGGSYM() && sym->asAGGSYM()->typeVarsThis->size == this->cvar) {
                m_aggRes = sym->asAGGSYM();
                break;
            }
            if (sym->isFWDAGGSYM() && !fwdTmp && sym->asFWDAGGSYM()->cvar == this->cvar)
                fwdTmp = sym->asFWDAGGSYM();
        }

        if (!m_aggRes && fwdTmp) {
            // Recursively resolve the forwarder.
            m_aggRes = fwdTmp->GetAgg(compiler);
            if (!m_aggRes) {
                // Propogate the error information.
                m_moduleBreak = fwdTmp->m_moduleBreak;
                m_tokBreak = fwdTmp->m_tokBreak;
                m_fCycle = fwdTmp->m_fCycle;
            }
        }

        ASSERT(m_fResolving);
        m_fResolving = false;
    }

    if (!m_aggRes && !m_moduleBreak) {
        m_moduleBreak = module;
        m_tokBreak = tkAssemblyRef;
    }

    ASSERT(m_aggRes || m_moduleBreak);

}


/***************************************************************************************************
    Get the AGGSYM that the FWDAGGSYM resolves to.
***************************************************************************************************/
AGGSYM * FWDAGGSYM::GetAgg(COMPILER * compiler)
{
    if (!m_aggRes && !m_moduleBreak)
        Resolve(compiler);
    return m_aggRes;
}


void METHPROPSYM::copyInto(METHPROPSYM * mpsDst, AGGTYPESYM * typeSrc, COMPILER * compiler)
{
    SYM::copyInto(mpsDst);
    mpsDst->declaration = declaration;

    mpsDst->firstChild = NULL;
    mpsDst->psymAttachChild = &mpsDst->firstChild;

    mpsDst->isStatic = isStatic;
    mpsDst->retType = typeSrc ? compiler->getBSymmgr().SubstType(retType, typeSrc) : retType;
    mpsDst->params = typeSrc ? compiler->getBSymmgr().SubstTypeArray(params, typeSrc) : params;
    mpsDst->isParamArray = isParamArray;
    mpsDst->modOptCount = modOptCount;
}


unsigned int EVENTSYM::getParseFlags()
{
    if (!parseTree) return 0;

    if (parseTree->kind == NK_PROPERTY)
        return parseTree->flags;
    else
        return parseTree->asVARDECL()->pDecl->flags;
}

BASENODE * MEMBVARSYM::getAttributesNode()
{
    BASENODE *attr;
    BASENODE *parseTree = getParseTree();

    if (parseTree == NULL)
        return NULL;
    else if (parseTree->kind == NK_ENUMMBR) {
        // enumerators currently don't have attributes
        attr = parseTree->asENUMMBR()->pAttr;
    } else {

        BASENODE * fieldTree = parseTree->asVARDECL()->pParent;
        while (fieldTree->kind == NK_LIST) {
            fieldTree = fieldTree->pParent;
        }
        attr = fieldTree->asANYFIELD()->pAttr;
    }

    return attr;
}

BASENODE * PROPSYM::getAttributesNode()
{
    BASENODE *parseTree = getParseTree();
    if (parseTree != NULL)
        return parseTree->asANYPROPERTY()->pAttr;
    else
        return NULL;
}

BASENODE * EVENTSYM::getAttributesNode()
{
    BASENODE * parseTree = getParseTree();
    BASENODE * attributes;

    // parseTree could be field or property parse tree.
    if (parseTree == NULL)
        return NULL;
    else if (parseTree->kind == NK_VARDECL) {
        BASENODE * fieldTree = parseTree->asVARDECL()->pParent;
        while (fieldTree->kind == NK_LIST) {
            fieldTree = fieldTree->pParent;
        }
        attributes = fieldTree->asANYFIELD()->pAttr;
    }
    else if (parseTree->kind == NK_PROPERTY) {
        attributes = parseTree->asPROPERTY()->pAttr;
    }
    else {
        ASSERT(!"BadEvent nodes");
        attributes = 0;
    }

    return attributes;
}

BASENODE * METHSYM::getAttributesNode()
{
    BASENODE * tree = getParseTree();
    if (tree == NULL)
        return NULL;

    switch (tree->kind)
    {
    case NK_METHOD:
    case NK_CTOR:
    case NK_OPERATOR:
    case NK_DTOR:
        return tree->asANYMETHOD()->pAttr;

    case NK_ACCESSOR:
        ASSERT(this->isAnyAccessor());
        return tree->asACCESSOR()->pAttr;

    case NK_VARDECL:
        {
            // simple event declaration auto-generated accessors.
            ASSERT(this->isEventAccessor());
            BASENODE * fieldTree = parseTree->asVARDECL()->pParent;
            while (fieldTree->kind == NK_LIST) {
                fieldTree = fieldTree->pParent;
            }
            return fieldTree->asANYFIELD()->pAttr;
        }

    case NK_INDEXER:
    case NK_PROPERTY:
        // This only happens when there is no parse tree for an event accessor (error case),
        // or when a property is a sealed override but only implements one accessor.
        // In this case we create the accessor anyway and give it the property parse tree.
        // Just ignore any attributes on the property.
        ASSERT(this->isAnyAccessor() && this->isFabricated);
        return NULL;

    default:
        ASSERT(this->isCompilerGeneratedCtor() || this->isIfaceImpl || this->getClass()->IsDelegate() || this->isAnonymous());
        return NULL;
    }
}



/*
 * Given a symbol, returns its element kind for attribute purpose
 */
CorAttributeTargets AGGSYM::getElementKind()
{
    switch (AggKind()) {
    case AggKind::Interface:
        return catInterface;
    case AggKind::Enum:
        return catEnum;
    case AggKind::Class:
        return catClass;
    case AggKind::Struct:
        return catStruct;
    case AggKind::Delegate:
        return catDelegate;
    default:
        ASSERT(!"Bad aggsym");
    }

    return (CorAttributeTargets)0;
}

/*
 * Given a symbol, returns its element kind for attribute purpose
 */
CorAttributeTargets SYM::getElementKind()
{
    switch (kind) {
    case SK_METHSYM:
        return (asMETHSYM()->isCtor() ? catConstructor : catMethod);

    case SK_PROPSYM:
        return catProperty;

    case SK_MEMBVARSYM:
        return catField;

    case SK_TYVARSYM:
        return catGenericParameter;

    case SK_AGGTYPESYM:
        ASSERT(!"Bad Symbol type: SK_AGGTYPESYM"); // GENERICS - fix me
        return catField;
       
    case SK_EVENTSYM:
        return catEvent;

    case SK_AGGSYM:
        return asAGGSYM()->getElementKind();

    case SK_AGGDECLSYM:
        return asAGGDECLSYM()->Agg()->getElementKind();

    case SK_GLOBALATTRSYM:
        return this->asGLOBALATTRSYM()->elementKind;

    default:
        ASSERT(!"Bad Symbol type");
    }

    return (CorAttributeTargets) 0;
}

// true for a&b in namespace a.b.c {}
bool NSDECLSYM::isDottedDeclaration()
{
    return  this->parseTree->pName &&
           (this->parseTree->pName->kind == NK_DOT) &&
            this->firstChild && 
           (this->firstChild->isNSDECLSYM()) &&
           (this->firstChild->asNSDECLSYM()->parseTree == this->parseTree);
}  

/*
 * returns the output file for this aggregate
 */
OUTFILESYM * AGGSYM::getOutputFile()
{
    // This is safe, since all partial parts of a class must be in the same output file.
    return DeclFirst()->getInputFile()->getOutputFile();
}

/*
 * returns the assembly id for the declaration of this symbol
 */
int SYM::GetAssemblyID()
{
    switch(this->kind) {
    case SK_METHSYM:
    case SK_PROPSYM:
    case SK_MEMBVARSYM:
    case SK_EVENTSYM:
    case SK_TYVARSYM:
    case SK_AGGTYPESYM:
        return parent->asAGGSYM()->GetAssemblyID();

    case SK_PTRSYM:
    case SK_NUBSYM:
    case SK_ARRAYSYM:
    case SK_PINNEDSYM:
    case SK_PARAMMODSYM:
    case SK_MODOPTTYPESYM:
        return parent->GetAssemblyID();

    case SK_AGGDECLSYM:
        return this->asAGGDECLSYM()->GetAssemblyID();
    case SK_AGGSYM:
        return this->asAGGSYM()->GetAssemblyID();
    case SK_NSDECLSYM:
        return this->asNSDECLSYM()->GetAssemblyID();
    case SK_INFILESYM:
        return this->asINFILESYM()->GetAssemblyID();
    case SK_MODULESYM:
        return this->asMODULESYM()->getInputFile()->GetAssemblyID();
    case SK_EXTERNALIASSYM:
        return this->asEXTERNALIASSYM()->GetAssemblyID();

    case SK_NSSYM:
    case SK_NSAIDSYM:
    default:
        // Should never call this with any other kind.
        VSFAIL("GetAssemblyID called on bad sym kind");
        return kaidNil;
    }
}

/*
 * returns the assembly id for the declaration of this symbol
 */
bool SYM::InternalsVisibleTo(int aid)
{
    switch (this->kind) {
    case SK_METHSYM:
    case SK_PROPSYM:
    case SK_MEMBVARSYM:
    case SK_EVENTSYM:
    case SK_TYVARSYM:
    case SK_AGGTYPESYM:
        return parent->asAGGSYM()->InternalsVisibleTo(aid);

    case SK_PTRSYM:
    case SK_NUBSYM:
    case SK_ARRAYSYM:
    case SK_PINNEDSYM:
    case SK_PARAMMODSYM:
    case SK_MODOPTTYPESYM:
        return parent->InternalsVisibleTo(aid);

    case SK_AGGDECLSYM:
        return this->asAGGDECLSYM()->InternalsVisibleTo(aid);
    case SK_AGGSYM:
        return this->asAGGSYM()->InternalsVisibleTo(aid);
    case SK_NSDECLSYM:
        return this->asNSDECLSYM()->InternalsVisibleTo(aid);
    case SK_INFILESYM:
        return this->asINFILESYM()->InternalsVisibleTo(aid);
    case SK_MODULESYM:
        return this->asMODULESYM()->getInputFile()->InternalsVisibleTo(aid);

    case SK_EXTERNALIASSYM:
    case SK_NSSYM:
    case SK_NSAIDSYM:
    default:
        // Should never call this with any other kind.
        VSFAIL("InternalsVisibleTo called on bad sym kind");
        return false;
    }
}


bool SYM::SameAssemOrFriend(SYM * sym)
{
    int aid = this->GetAssemblyID();
    return aid == sym->GetAssemblyID() || sym->InternalsVisibleTo(aid);
}


bool SYM::IsDeprecated()
{
    switch (this->kind) {
    case SK_AGGTYPESYM:
        return this->asAGGTYPESYM()->getAggregate()->IsDeprecated();
    case SK_ARRAYSYM:
    case SK_PTRSYM:
    case SK_PINNEDSYM:
    case SK_PARAMMODSYM:
    case SK_MODOPTSYM:
    case SK_MODOPTTYPESYM:
        if (this->parent)
            return this->parent->IsDeprecated();
    default:
        return this->isDeprecated;
    }
}


bool SYM::IsDeprecatedError()
{
    switch (this->kind) {
    case SK_AGGTYPESYM:
        return this->asAGGTYPESYM()->getAggregate()->IsDeprecatedError();
    case SK_ARRAYSYM:
    case SK_PTRSYM:
    case SK_PINNEDSYM:
    case SK_PARAMMODSYM:
    case SK_MODOPTSYM:
    case SK_MODOPTTYPESYM:
        if (this->parent)
            return this->parent->IsDeprecatedError();
    default:
        return this->isDeprecatedError;
    }
}


PCWSTR SYM::DeprecatedMessage()
{
    switch (this->kind) {
    case SK_METHSYM:
    case SK_PROPSYM:
        return this->asMETHPROPSYM()->PszDepMsg();
    case SK_EVENTSYM:
        return this->asEVENTSYM()->PszDepMsg();
    case SK_MEMBVARSYM:
        return this->asMEMBVARSYM()->PszDepMsg();
    case SK_AGGSYM:
        return this->asAGGSYM()->PszDepMsg();
    case SK_AGGTYPESYM:
        return this->asAGGTYPESYM()->getAggregate()->PszDepMsg();
    case SK_ARRAYSYM:
    case SK_PTRSYM:
    case SK_PINNEDSYM:
    case SK_PARAMMODSYM:
    case SK_MODOPTSYM:
    case SK_MODOPTTYPESYM:
        if (this->parent)
            return this->parent->DeprecatedMessage();
    default:
        return NULL;
    }
}


void SYM::SetDeprecated(bool fDep, bool fDepError, PCWSTR pszDep)
{
    switch (this->kind) {
    default:
        return;
    case SK_METHSYM:
    case SK_PROPSYM:
        this->asMETHPROPSYM()->SetDepMsg(pszDep);
        break;
    case SK_EVENTSYM:
        this->asEVENTSYM()->SetDepMsg(pszDep);
        break;
    case SK_MEMBVARSYM:
        this->asMEMBVARSYM()->SetDepMsg(pszDep);
        break;
    case SK_AGGSYM:
        this->asAGGSYM()->SetDepMsg(pszDep);
        break;
    }
    this->isDeprecated = fDep;
    this->isDeprecatedError = fDepError;
}


/*
 * returns the allowable access modifiers on members of this aggregate symbol
 */
unsigned AGGSYM::allowableMemberAccess()
{
    switch (this->AggKind()) {
    case AggKind::Class:
        return NF_MOD_ACCESSMODIFIERS;
    case AggKind::Struct:
        return NF_MOD_ACCESSMODIFIERS;
    case AggKind::Interface:
        return 0;
    default:
        ASSERT(0);
        return 0;
    }
}

/*
 * returns the inputfile where a symbol was declared.
 *
 * returns NULL for namespaces because they can be declared
 * in multiple files.
 */
INFILESYM * SYM::getInputFile()
{
    switch (kind)
    {
    case SK_NSSYM:
    case SK_NSAIDSYM:
        // namespaces don't have input files
        // call with a NSDECLSYM instead
        ASSERT(0);
        return NULL;

    case SK_NSDECLSYM:
        return this->asNSDECLSYM()->inputfile;

    case SK_AGGSYM: 
    {
        AGGSYM * aggsym = this->asAGGSYM();
        if (!aggsym->isSource)
            return aggsym->DeclOnly()->getInputFile();

        // Because an AGGSYM that isn't metadata can be defined across multiple
        // files, getInputFile isn't a reasonable operation.
        ASSERT(0);
        return NULL;
    }

    case SK_AGGTYPESYM:
        return ((SYM*)this->asAGGTYPESYM()->getAggregate())->getInputFile();

    case SK_AGGDECLSYM:
        return this->asAGGDECLSYM()->getInputFile();

    case SK_TYVARSYM:
        if (this->parent->isAGGSYM()) {
            // Because an AGGSYM that isn't metadata can be defined across multiple
            // files, getInputFile isn't a reasonable operation.
            ASSERT(0);
            return NULL;
        }
        else if (this->parent->isMETHSYM())
            return this->parent->asMETHSYM()->getInputFile();
        ASSERT(0); 
        break;

    case SK_MEMBVARSYM:
        return this->asMEMBVARSYM()->containingDeclaration()->getInputFile();
    case SK_FAKEMETHSYM:
        return this->asFAKEMETHSYM()->containingDeclaration()->getInputFile();
    case SK_METHSYM:
        return this->asMETHSYM()->containingDeclaration()->getInputFile();
    case SK_PROPSYM:
        return this->asPROPSYM()->containingDeclaration()->getInputFile();
    case SK_EVENTSYM:
        return this->asEVENTSYM()->containingDeclaration()->getInputFile();

    case SK_ALIASSYM:
        return this->parent->asNSDECLSYM()->getInputFile();

    case SK_PTRSYM:
    case SK_NUBSYM:
    case SK_ARRAYSYM:
    case SK_PINNEDSYM:
    case SK_PARAMMODSYM:
    case SK_MODOPTTYPESYM:
        return parent->asTYPESYM()->getInputFile();

    case SK_GLOBALATTRSYM:
        return parent->getInputFile();
    case SK_MODOPTSYM:
        return this->asMODOPTSYM()->GetModule()->getInputFile();

    case SK_OUTFILESYM:
        return asOUTFILESYM()->firstInfile();

    case SK_NULLSYM:
    case SK_VOIDSYM:
        return NULL;

    case SK_INFILESYM:
        return this->asINFILESYM();
    case SK_MODULESYM:
        return asMODULESYM()->getInputFile();

    default:
        ASSERT(0);
        break;
    }

    return NULL;
}


/***************************************************************************************************
    Return some input file for the symbol. Doesn't matter which. This returns NULL for some
    symbol kinds, but doesn't assert for any.
***************************************************************************************************/
INFILESYM * SYM::GetSomeInputFile()
{
    switch (kind)
    {
    case SK_INFILESYM:
        return this->asINFILESYM();

    case SK_MODULESYM:
        return this->asMODULESYM()->getInputFile();

    case SK_OUTFILESYM:
        return this->asOUTFILESYM()->firstInfile();

    case SK_NSDECLSYM:
        return this->asNSDECLSYM()->inputfile;

    case SK_AGGSYM:
        return this->asAGGSYM()->DeclFirst()->getInputFile();

    case SK_AGGDECLSYM:
        return this->asAGGDECLSYM()->getInputFile();

    case SK_MEMBVARSYM:
        return this->asMEMBVARSYM()->containingDeclaration()->getInputFile();
    case SK_FAKEMETHSYM:
        return this->asFAKEMETHSYM()->containingDeclaration()->getInputFile();
    case SK_METHSYM:
        return this->asMETHSYM()->containingDeclaration()->getInputFile();
    case SK_PROPSYM:
        return this->asPROPSYM()->containingDeclaration()->getInputFile();
    case SK_EVENTSYM:
        return this->asEVENTSYM()->containingDeclaration()->getInputFile();

    default:
        if (this->parent)
            return this->parent->GetSomeInputFile();
        return NULL;
    }
}

/*
 * returns the inputfile scope where a symbol was declared.
 *
 */
MODULESYM * SYM::GetModule() const
{
    switch (kind)
    {
    case SK_AGGSYM:
        return const_cast<SYM *>(this)->asAGGSYM()->GetModule();

    case SK_TYVARSYM:
        if (this->parent->isAGGSYM())
            return this->parent->asAGGSYM()->GetModule();
        else if (this->parent->isMETHSYM())
            return this->parent->asMETHSYM()->GetModule();
        ASSERT(0); 
        return NULL;

    case SK_MEMBVARSYM:
    case SK_AGGTYPESYM:
    case SK_METHSYM:
    case SK_PROPSYM:
    case SK_EVENTSYM:
        return this->parent->asAGGSYM()->GetModule();

    case SK_MODOPTSYM:
        return const_cast<SYM*>(this)->asMODOPTSYM()->GetModule();

    default:
        ASSERT(0);
        return NULL;
    }
}

IMetaDataImport2 * SYM::GetMetaImportV2(COMPILER *compiler)
{
    return this->GetModule()->GetMetaImportV2(compiler);
}

IMetaDataImport * SYM::GetMetaImport(COMPILER *compiler)
{
    return this->GetModule()->GetMetaImport(compiler);
}

/* Returns if the symbol is virtual.
 */
bool SYM::IsVirtual()
{
    switch (kind) {
    case SK_METHSYM:
        return this->asMETHSYM()->isVirtual;
    case SK_EVENTSYM:
        return this->asEVENTSYM()->methAdd && this->asEVENTSYM()->methAdd->isVirtual;
    case SK_PROPSYM:
        return (this->asPROPSYM()->methGet && this->asPROPSYM()->methGet->isVirtual) || 
               (this->asPROPSYM()->methSet && this->asPROPSYM()->methSet->isVirtual);
    default:
        return false;
    }
}

bool SYM::IsOverride()
{
    switch (kind) {
    case SK_METHSYM:
    case SK_PROPSYM:
        return this->asMETHPROPSYM()->isOverride;
    case SK_EVENTSYM:
        return this->asEVENTSYM()->isOverride;
    default:
        return false;
    }
}

bool SYM::IsHideByName()
{
    switch (kind) {
    case SK_METHSYM:
    case SK_PROPSYM:
        return this->asMETHPROPSYM()->isHideByName;
    case SK_EVENTSYM:
        return this->asEVENTSYM()->methAdd && this->asEVENTSYM()->methAdd->isHideByName;
    default:
        return true;
    }
}

// Returns the virtual that this sym overrides (if IsOverride() is true), NULL otherwise.
SYM * SYM::SymBaseVirtual()
{
    switch (kind) {
    case SK_METHSYM:
    case SK_PROPSYM:
        return this->asMETHPROPSYM()->swtSlot.Sym();
    case SK_EVENTSYM:
        return this->asEVENTSYM()->ewtSlot.Event();
    default:
        return false;
    }
}


/*
 * returns the containing declaration of a symbol or NULL if there isn't one.
 * this will be an AGGDECLSYM for members of a type, or a NSDECLSYM for members of a namespace.
 */
DECLSYM * SYM::GetDecl()
{
    switch (kind) {
    case SK_NSSYM:
    case SK_NSAIDSYM:
        return NULL;

    case SK_AGGTYPESYM:
    case SK_AGGSYM:
        // an AGGSYM doesn't have a containing declaration, due to partial classes.
        return NULL;

    case SK_NSDECLSYM:
    case SK_AGGDECLSYM:
        return asDECLSYM()->DeclPar();

    case SK_TYVARSYM:
        if (this->parent->isMETHSYM())
            return this->parent->asMETHSYM()->containingDeclaration();
        // an AGGSYM doesn't have a containing declaration, due to partial classes.
        return NULL;

    case SK_MEMBVARSYM:
        return asMEMBVARSYM()->containingDeclaration();

    case SK_METHSYM:
    case SK_PROPSYM:
        return asMETHPROPSYM()->containingDeclaration();

    case SK_EVENTSYM:
        return asEVENTSYM()->containingDeclaration();

    case SK_GLOBALATTRSYM:
        return parent->asNSDECLSYM();

    default:
        return NULL;
    }
}

bool SYM::isUnsafe()
{
    if (this->isMEMBVARSYM())
        return this->asMEMBVARSYM()->isUnsafe;
    else if (this->isMETHPROPSYM())
        return this->asMETHPROPSYM()->isUnsafe;
    else if (this->isAGGDECLSYM())
        return this->asAGGDECLSYM()->isUnsafe;
    else if (this->isEVENTSYM())
        return this->asEVENTSYM()->isUnsafe;
    else {
        ASSERT(!"Undefined unsafe check");
        return false;
    }
}

/*
 * is this a compiler generated constructor
 */
bool METHSYM::isCompilerGeneratedCtor()
{
    BASENODE* parseTree = getParseTree();
    if (parseTree)
        return this->isCtor() && (parseTree->kind == NK_CLASS || parseTree->kind == NK_STRUCT);
    else
        return false;
}


/*
 * returns true if this property is a get accessor
 */
bool METHSYM::isGetAccessor()
{
    ASSERT(this->isPropertyAccessor()); 
    PROPSYM *property = getProperty();
    if (property)
        return (this == property->methGet); 

    ASSERT(!"cannot find property for accessor");
    return false;
}

/*
 * returns event. Only valid to call is isEvent is true.
 */
EVENTSYM *PROPSYM::getEvent(BSYMMGR * symmgr)
{
    ASSERT(this->isEvent);

    EVENTSYM * event;
    event = symmgr->LookupAggMember(this->name, this->getClass(), MASK_EVENTSYM)->asEVENTSYM();
    if (event && event->implementation == this)
        return event;
    return NULL;
}

/*
 * returns event. Only valid to call is isEvent is true.
 */
EVENTSYM *MEMBVARSYM::getEvent(BSYMMGR * symmgr)
{
    ASSERT(this->isEvent);

    EVENTSYM * event;
    event = symmgr->LookupAggMember(this->name, this->getClass(), MASK_EVENTSYM)->asEVENTSYM();
    if (event && event->implementation == this)
        return event;
    else 
        return NULL;
}


/*
 * returns the parse node for the type's attributes
 */
BASENODE * AGGDECLSYM::getAttributesNode()
{
    if (parseTree == NULL) {
        ASSERT(Agg()->isFabricated);
        return NULL;
    }
    return (parseTree->kind == NK_DELEGATE ? parseTree->asDELEGATE()->pAttr : parseTree->asAGGREGATE()->pAttr);
}


/*
 * Given a symbol, determine its fundemental type. This is the type that indicate how the item
 * is stored and what instructions are used to reference if. The fundemental types are:
 *   one of the integral/float types (includes enums with that underlying type)
 *   reference type
 *   struct/value type
 */
FUNDTYPE TYPESYM::fundType()
{

    switch (this->getKind()) {

    case SK_AGGTYPESYM:
    {
        AGGSYM * sym = this->asAGGTYPESYM()->getAggregate();
        ASSERT(sym->HasResolvedBaseClasses() || (sym->isPredefined && sym->iPredef < PT_OBJECT));

        // Treat enums like their underlying types.
        if (sym->IsEnum()) {
            sym = sym->underlyingType->getAggregate();
            ASSERT(sym->IsStruct());
        }

        if (sym->IsStruct()) {
            // Struct type could be predefined (int, long, etc.) or some other struct.
            if (sym->isPredefined)
                return predefTypeInfo[sym->iPredef].ft;
            return FT_STRUCT;
        }
        return FT_REF;  // Interfaces, classes, delegates are reference types.
    }

    case SK_TYVARSYM:  
        return FT_VAR; 

    case SK_ARRAYSYM:
    case SK_NULLSYM:
        return FT_REF;

    case SK_PTRSYM:
        return FT_PTR;

    case SK_NUBSYM:
        return FT_STRUCT;

    default:
        return FT_NONE;
    }
}

/*
 * returns true if the type is any struct type
 */
bool TYPESYM::isStructType()
{
    return this->isAGGTYPESYM() && this->getAggregate()->IsStruct() || this->isNUBSYM();
}

/* 
 * returns the base of the expression tree for this initializer
 * ie. the entire assignment expression
 */
BASENODE *MEMBVARSYM::getBaseExprTree()
{
    ASSERT(!this->getClass()->IsEnum());
    return this->parseTree->asVARDECL()->pArg;
}

/*
 * returns the constant expression tree(after the =) or null
 */
BASENODE *MEMBVARSYM::getConstExprTree()
{
    if (this->getClass()->IsEnum()) {
        return this->parseTree->asENUMMBR()->pValue;
    } else if (this->fixedAgg) {
        return getBaseExprTree();
    } else {
        ASSERT(this->isConst);
        return getBaseExprTree()->asBINOP()->p2;
    }
}

/*
 * returns TRUE if a preprocessor symbol is defined
 */
bool INFILESYM::isSymbolDefined(NAME *symbol)
{
    return !!((CSourceData *)pData)->GetModule ()->IsSymbolDefined(symbol);
}


void INFILESYM::SetAssemblyID(int aid, NRHEAP * heap)
{
    ASSERT(!this->aid);
    ASSERT(kaidThisAssembly <= aid && aid < kaidMinModule);

    this->aid = aid;
    bsetFilter.SetBit(aid, heap);
    if (aid == kaidThisAssembly)
        bsetFilter.SetBit(kaidGlobal, heap);
}


void INFILESYM::AddInternalsVisibleTo(int aid, NRHEAP * heap)
{
    ASSERT(0 <= aid && aid < kaidMinModule);

    // NOTE: No need to keep track for this assembly.
    ASSERT(this->aid > kaidThisAssembly);

    bsetFriend.SetBit(aid, heap);
}


void INFILESYM::AddToAlias(int aid, NRHEAP * heap)
{
    ASSERT(0 <= aid && aid < kaidMinModule);

    // NOTE: Anything in this assembly should not be added to other aliases!
    ASSERT(this->aid > kaidThisAssembly);
    ASSERT(bsetFilter.TestBit(this->aid));

    // If this assert fires, then AddToAlias is being called too late
    // or UnionAliasFilter is being called too early.
    ASSERT(!fUnionCalled);

    bsetFilter.SetBit(aid, heap);
}


void INFILESYM::UnionAliasFilter(BitSet & bsetDst, NRHEAP * heap)
{
    bsetDst.Union(bsetFilter, heap);
#ifdef DEBUG
    fUnionCalled = true;
#endif
}


/*
 * returns true if this symbol is a normal symbol visible to the user
 */
bool SYM::isUserCallable()
{
    switch (kind) {
    case SK_METHSYM:
        return this->asMETHSYM()->isUserCallable();
    default:
        break;
    }

    return true;
}


void TYVARSYM::SetBaseTypes(TYPESYM * typeBaseAbs, AGGTYPESYM * atsBaseCls)
{
    this->typeBaseAbs = typeBaseAbs;
    this->atsBaseCls = atsBaseCls;
}


/*
 * Returns the matching EXF_PARAM flags for a type.  This tells whether this is
 * a ref or an out param...
 */
//int __fastcall TYPESYM::getParamFlags() {
//    if (isPARAMMODSYM()) {
//        ASSERT(EXF_REFPARAM << 1 == EXF_OUTPARAM);
//        return (EXF_REFPARAM << (int)asPARAMMODSYM()->isOut);
//    } else {
//        return 0;
//    }
//}

/*
 * returns true if this list contains sym
 * NOTE that the this pointer may be NULL
 */
bool SYMLIST::contains(PSYM sym)
{
    FOREACHSYMLIST(this, p, SYM)
        if (p == sym) {
            return true;
        }
    ENDFOREACHSYMLIST

    return false;
}

void SymSet::Term()
{
    if (m_prgsym)
        m_heap->Free(m_prgsym);
    m_prgsym = NULL;
    m_csymAlloc = 0;
    m_csym = 0;
}


int SymSet::AddSym(SYM * sym)
{
    ASSERT(m_heap);
    ASSERT(!m_csymAlloc == !m_prgsym);
    ASSERT(0 <= m_csym && m_csym <= m_csymAlloc);

    // Grow if necessary.
    if (m_csym >= m_csymAlloc) {
        if (!m_csymAlloc) {
            ASSERT(!m_prgsym && !m_csym);
            m_csymAlloc = 64;
            m_prgsym = (SYM**)m_heap->Alloc(sizeof(SYM *) * m_csymAlloc);
        }
        else {
            m_csymAlloc *= 2;
            m_prgsym = (SYM **)m_heap->Realloc(m_prgsym, sizeof(SYM *) * m_csymAlloc);
        }
    }

    ASSERT(m_csym < m_csymAlloc);

    m_prgsym[m_csym] = sym;

    return m_csym++;
}


/* 
 * Constructor.
 */
SYMTBL::SYMTBL(ALLOCHOST * host, unsigned log2Buckets)
{
    this->host = host;
    buckets = NULL;
    cBucksUsed = cBuckets = 0;
    bucketShift = log2Buckets;
}


/* 
 * Destructor 
 */
SYMTBL::~SYMTBL()
{
    Term();
}

/*
 * Hash a name and parent to get a bucket number and jump amount. The jump
 * amount must be odd.
 */
__forceinline unsigned SYMTBL::Bucket(PNAME name, PPARENTSYM parent, unsigned * pjump)
{
    unsigned hash, iBucket, jump;

    // Create a hash value from the name and parent of the symbol.
    hash = HashUInt((UINT_PTR)name) + HashUInt((UINT_PTR)parent);

    // Get bucket and jump amount. Jump amount must be odd so that
    // all symbols in the table are hit.
    iBucket = hash & bucketMask;
    jump = hash >> bucketShift;
    while (!(jump & 1)) {
        jump >>= 1;
        if (jump == 0)
            jump = 1;
    }

    *pjump = jump;
    return iBucket;
}

/*
 * Helper routine to place a child into the hash table. This hash table is
 * organized to enable quick resolving of mapping a (name, parent) pair
 * to a child symbol. If multiple child symbols of a parent have the same
 * name, they are all chained together, so we need only find the first one.
 * We place children in a hash table with the hash function a hashing of the 
 * NAME and PARENT addresses. We use double hashing to handle collisions
 * (the second hash providing the "jump"), and double the size of the table
 * when it becomes 75% full.
 */
void SYMTBL::InsertChildNoGrow(PSYM child)
{
    ASSERT(4 * cBucksUsed < 3 * cBuckets); // Caller should have grown the bucket array.
    // Note that child->nextSameName may be non-NULL when we're called by GrowTable.

    PNAME name = child->name;
    PPARENTSYM parent = child->parent;
    unsigned iBucket, jump;

    // Get bucket and jump amount.
    iBucket = Bucket(name, parent, &jump);

    SYM ** psymAdd = NULL; // Where to add the new child.
    SYM ** psym;

    // Search the table until the correct name or an empty bucket is found.
    for ( ; *(psym = &buckets[iBucket]); iBucket = (iBucket + jump) & bucketMask) {
        ASSERT(*psym == ksymDead || !(*psym)->isDead);
        if (*psym == ksymDead) {
            if (!psymAdd)
                psymAdd = psym;
            continue;
        }

        if ((*psym)->name == name && (*psym)->parent == parent) {
            // Link onto the end of the symbol chain here.
            while (*psym) {
                ASSERT(*psym != ksymDead && !(*psym)->isDead);
                psym = &(*psym)->nextSameName;
            }
            *psym = child;
            return;
        }
    }

    // No bucket has this parent/name pair.
    if (psymAdd) {
        // We're replacing a dead slot with a SYM so don't increment cBucksUsed.
        ASSERT(*psymAdd == ksymDead);
        *psymAdd = child;
    }
    else {
        // We're filling an empty bucket so increment cBucksUsed.
        cBucksUsed++;
        *psym = child;
    }
}

/* 
 * Add a named symbol to a parent scope, for later lookup.
 */
void SYMTBL::InsertChild(PPARENTSYM parent, PSYM child)
{
    ASSERT(child->nextSameName == NULL);
    ASSERT(!child->parent || child->parent == parent);
    child->parent = parent;

    // Is the table more than 3/4 full? Grow the table if so.
    if (cBucksUsed * 4 >= cBuckets * 3) {
        GrowTable();
    }

    // Place the child into the hash table.
    InsertChildNoGrow(child);
}

/*
 * Remove a symbol from SYM table and nextSameName list.
 * Also sets the isDead bit
 * Note: this does not remove any children of the SYM
 */
void SYMTBL::RemoveChildFromBuckets(SYM * symOrphan)
{
    // Because of the hash table type that we use, we can't cause a bucket to become empty.
    // If removing this SYM would empty an entire bucket, we leave it instead. Insert
    // knows from the isDead bit that the bucket is available.
    ASSERT(!symOrphan->isDead && symOrphan->parent);

    NAME * name = symOrphan->name;
    PARENTSYM * par = symOrphan->parent;
    uint jump;
    SYM ** psym;

    for (uint iBucket = Bucket(name, par, &jump); *(psym = &buckets[iBucket]); iBucket = (iBucket + jump) & bucketMask) {
        ASSERT(*psym == ksymDead || !(*psym)->isDead);
        if (*psym != ksymDead && (*psym)->name == name && (*psym)->parent == par) {
            for ( ; *psym; psym = &(*psym)->nextSameName) {
                ASSERT(!(*psym)->isDead);
                if (*psym == symOrphan) {
                    *psym = symOrphan->nextSameName;
                    symOrphan->nextSameName = NULL;

                    // If we emptied a slot, mark it as dead. In either case, don't decrement cBucksUsed.
                    if (!*psym)
                        *psym = ksymDead;
                    symOrphan->isDead = true;
                    return;
                }
            }
            break;
        }
    }

    ASSERT(!"sym not found in RemoveFromBuckets!");
    symOrphan->isDead = true;
}

/*
 * Remove all child symbols of parent that match kindmask.
 * Note: this does not remove any nested children
 */
void SYMTBL::ClearChildren(PPARENTSYM parent, symbmask_t kindmask)
{
    SYM **psym = &parent->firstChild;

    while (*psym) {
        if ((*psym)->mask() & kindmask) {
            SYM * symBad = *psym;
            ASSERT(symBad && symBad->parent == parent);
            RemoveChildFromBuckets(symBad);
            *psym = (*psym)->nextChild;
            symBad->nextChild = NULL;
            symBad->parent = NULL;
        }
        else {
            psym = &(*psym)->nextChild;
        }
    }

    parent->psymAttachChild = psym;
}

/* 
 * Look up a symbol by name and parent, filtering by mask.
 */
PSYM SYMTBL::LookupSym(PNAME name, PPARENTSYM parent, symbmask_t kindmask)
{
    unsigned iBucket, jump;
    PSYM sym;

    if (!buckets)
        return NULL;        // No bucket table yet.

    // Get bucket and jump amount.
    iBucket = Bucket(name, parent, &jump);

    // Search the table until the correct name or an empty bucket is found.
    for ( ; (sym = buckets[iBucket]); iBucket = (iBucket + jump) & bucketMask) {
        ASSERT(sym == ksymDead || !sym->isDead);
        if (sym != ksymDead && sym->name == name && sym->parent == parent) {
            do {
                ASSERT(!sym->isDead);
                if (kindmask & sym->mask())
                    return sym;
                sym = sym->nextSameName;
            } while (sym);

            return NULL;
        }
    }

    return NULL;
}


/* 
 * Clear the table of symbols.
 */
void SYMTBL::Clear()
{
    // Clear the table.
    memset(buckets, 0, cBuckets * sizeof(PSYM));
    cBucksUsed = 0;
}


/*
 * Terminate the table. Free the bucket array.
 */
void SYMTBL::Term()
{
    // Free the bucket table.
    if (buckets) {
        size_t cb = cBuckets * sizeof(PSYM);
        if (cb < PAGEHEAP::pageSize)
            cb = PAGEHEAP::pageSize;
        host->GetPageHeap()->FreePages(ProtectedEntityFlags::Other, buckets, cb);
        buckets = NULL;
    }
}

/* 
 * Grow the table. This could either be the initial allocation of
 * the table, or a doubling in size.
 */
void SYMTBL::GrowTable()
{
    PSYM * bucketsPrev;        // the buckets of the old hash table.
    unsigned cBucketsPrev = 0; // number of buckets in old table
    size_t cb;

    // Hold onto the old table.
    bucketsPrev = buckets;
    if (buckets) {
        cBucketsPrev = cBuckets;
        ++bucketShift;              // double size.
    }

    // Allocate a new empty table of size 2^bucketShift
    cBuckets = (1 << bucketShift);
    bucketMask = cBuckets - 1;
    cb = cBuckets * sizeof(PSYM);
    if (cb < PAGEHEAP::pageSize)
        cb = PAGEHEAP::pageSize;
    buckets = (PSYM *) host->GetPageHeap()->AllocPages(cb);
    if (!buckets)
        host->NoMemory();  // won't return.
    memset(buckets, 0, cb);

    // Redistribute the old table into the new table.
    if (bucketsPrev) {
        cBucksUsed = 0; // InsertChildNoGrow we count the ones we use.

        // Re-add the symbols to the new table.
        for (unsigned i = 0; i < cBucketsPrev; ++i) {
            PSYM sym = bucketsPrev[i];
            if (sym && sym != ksymDead)
                InsertChildNoGrow(sym);
        }

        // Free the old table.
        cb = cBucketsPrev * sizeof(PSYM);
        if (cb < PAGEHEAP::pageSize)
            cb = PAGEHEAP::pageSize;
        host->GetPageHeap()->FreePages(ProtectedEntityFlags::Other, bucketsPrev, cb);
    }
}


/*
 * Constructor.
 */
SYMMGR::SYMMGR(NRHEAP * allocGlobal) : BSYMMGR(allocGlobal, compiler())
{
#ifdef DEBUG
    predefInputFile = NULL;
#endif
}

BSYMMGR::BSYMMGR(NRHEAP * allocGlobal, ALLOCHOST * allocHost
    ) :
    tableGlobal(allocHost, 13),  // Initial global size: 8192 buckets.
    ssetAssembly(allocHost->GetStandardHeap()),
    ssetModule(allocHost->GetStandardHeap())
{
    // Fill in the rest of taEmpty.
    ASSERT(!taEmpty.size);
    ASSERT(!taEmpty.tok);

    taEmpty.SetAggState(AggState::Last);

    this->allocGlobal = allocGlobal;

    predefSyms = NULL;
    prgptni = NULL;

    voidSym = NULL;
    nullType = NULL;
    errorSym = NULL;
    arglistSym = NULL;
    naturalIntSym = NULL;
    rootNS = NULL;
    nsaGlobal = NULL;
    fileroot = NULL;
    mdfileroot = NULL;
    xmlfileroot = NULL;
    aidMsCorLib = kaidNil;
    atsDictionary = NULL;
    methDictionaryCtor = NULL;
    methDictionaryAdd = NULL;
    methDictionaryTryGetValue = NULL;
    methStringEquals = NULL;
    methInitArray = NULL;
    methStringOffset = NULL;

    propNubValue = NULL;
    propNubHasValue = NULL;
    methNubGetValOrDef = NULL;
    methNubCtor = NULL;

    bsetGlobalAssemblies.SetBit(kaidThisAssembly, allocGlobal);

}

LSYMMGR::LSYMMGR(ALLOCHOST * host, NRHEAP * heap)
: tableLocal(host, 7)     // Initial local size: 128 buckets.
{
    //this->host = host;
    this->allocLocal = heap;
}

/* 
 * Destructor
 */
BSYMMGR::~BSYMMGR()
{
    Term();
    ASSERT(taEmpty.size == 0 && taEmpty.tok == 0);
}

/* 
 * Destructor
 */
LSYMMGR::~LSYMMGR()
{
    Term();
}

/*
 * returns a constant value of zero for a predefined type
 */
CONSTVAL BSYMMGR::GetPredefZero(PREDEFTYPE pt)
{
    ASSERT(pt >= 0 && pt < PT_COUNT);
    return FetchAtIndex(predefTypeInfo, pt).zero;
}

/* 
 * Initialize a bunch of pre-defined symbols and such.
 */



void BSYMMGR::Init()
{
    tableTypeArrays.Init(this);
    tableTokenToSym.Init(this);
    tableNameToSym.Init(this);
    ssetAssembly.Init();
    ssetModule.Init();


    // 'void' and 'null' are special types with their own symbol kind.
    errorSym = CreateGlobalSym(SK_ERRORSYM, NULL, NULL)->asERRORSYM();
    voidSym = CreateGlobalSym(SK_VOIDSYM, NULL, NULL)->asVOIDSYM();
    nullType = CreateGlobalSym(SK_NULLSYM, NULL, NULL)->asNULLSYM();
    typeUnit = CreateGlobalSym(SK_UNITSYM, NULL, NULL)->asUNITSYM();
    typeAnonMeth = CreateGlobalSym(SK_ANONMETHSYM, NULL, NULL)->asANONMETHSYM();
    typeMethGrp = CreateGlobalSym(SK_METHGRPSYM, NULL, NULL)->asMETHGRPSYM();

#define InitType(t) \
    t->SetAccess(ACC_PUBLIC); \
    t->SetAggState(AggState::PreparedMembers)

    InitType(errorSym);
    errorSym->fHasErrors = true;

    InitType(voidSym);
    InitType(nullType);
    InitType(typeUnit);
    InitType(typeAnonMeth);
    InitType(typeMethGrp);

#undef InitType

    // create the varargs type symbol:
    AGGSYM *arglistAgg = CreateGlobalSym(SK_AGGSYM, host()->getNamemgr()->GetPredefName(PN_ARGLIST), NULL)->asAGGSYM();
    arglistAgg->symmgr = this;
    arglistAgg->SetAggState(AggState::PreparedMembers);
    arglistAgg->isSealed = true;
    arglistAgg->SetAccess(ACC_PUBLIC);
    arglistAgg->setBogus(false);
    arglistAgg->ifaces = BSYMMGR::EmptyTypeArray();
    arglistAgg->ifacesAll = BSYMMGR::EmptyTypeArray();
    arglistAgg->typeVarsThis = BSYMMGR::EmptyTypeArray();
    arglistAgg->typeVarsAll = BSYMMGR::EmptyTypeArray();
    arglistSym = arglistAgg->getThisType();

    // create the natural int type symbol:
    AGGSYM * naturalIntSymAgg = CreateGlobalSym(SK_AGGSYM, host()->getNamemgr()->GetPredefName(PN_NATURALINT), NULL)->asAGGSYM();
    naturalIntSymAgg->symmgr = this;
    naturalIntSymAgg->SetAggState(AggState::PreparedMembers);
    naturalIntSymAgg->isSealed = true;
    naturalIntSymAgg->SetAccess(ACC_PUBLIC);
    naturalIntSymAgg->setBogus(false);
    naturalIntSymAgg->ifaces = BSYMMGR::EmptyTypeArray();
    naturalIntSymAgg->ifacesAll = BSYMMGR::EmptyTypeArray();
    naturalIntSymAgg->typeVarsThis = BSYMMGR::EmptyTypeArray();
    naturalIntSymAgg->typeVarsAll = BSYMMGR::EmptyTypeArray();
    naturalIntSym = naturalIntSymAgg->getThisType();

    // Some root symbols.
    PNAME emptyName = host()->getNamemgr()->AddString(L"");
    rootNS = CreateNamespace(emptyName, NULL);  // Root namespace
    nsaGlobal = GetNsAid(rootNS, kaidGlobal);
    fileroot = CreateGlobalSym(SK_SCOPESYM, NULL, NULL)->asSCOPESYM();  // Root of file symbols.
    mdfileroot = CreateGlobalSym(SK_OUTFILESYM, emptyName, fileroot)->asOUTFILESYM();
    xmlfileroot = CreateGlobalSym(SK_SCOPESYM, NULL, NULL)->asSCOPESYM();  // Root of predefined included XML files

    INFILESYM * infileUnres = CreateGlobalSym(SK_INFILESYM, emptyName, NULL)->asINFILESYM();
    infileUnres->isSource = false;
    infileUnres->SetAssemblyID(kaidUnresolved, allocGlobal);
    infileUnres->idLocalAssembly = mdTokenNil;
    int isym; isym = ssetAssembly.AddSym(infileUnres);
    ASSERT(isym == 0);

    InitPreLoad();
}


/***************************************************************************************************
    Compare methof for sorting the array of PredefTypeNameInfo's by hash value.
***************************************************************************************************/
int __cdecl ComparePredefTypeNameInfo(const void * pv1, const void * pv2)
{
    const PredefTypeNameInfo * p1 = (const PredefTypeNameInfo *)pv1;
    const PredefTypeNameInfo * p2 = (const PredefTypeNameInfo *)pv2;
    return (p1->hash < p2->hash) ? -1 : p1->hash > p2->hash;
}


/***************************************************************************************************
    Build the data structures needed to make FPreLoad fast.
    Make sure the namespaces are created. Compute and sort hashes of the NSSYM * value and type
    name (sans arity indicator).
***************************************************************************************************/
void BSYMMGR::InitPreLoad()
{
    ASSERT(!prgptni);

    prgptni = (PredefTypeNameInfo *)allocGlobal->Alloc(PT_COUNT * sizeof(prgptni[0]));

    for (int i = 0; i < PT_COUNT; ++i) {
        NSSYM * nsCur = GetRootNS();
        NAME * name;
        int cch;

        for (PCWCH pchCur = predefTypeInfo[i].fullName; ; pchCur++) {
            // Get the next name component.
            PCWCH pchMin = pchCur;
            while (*pchCur && *pchCur != L'.')
                pchCur++;

            cch = (int)(pchCur - pchMin);
            name = host()->getNamemgr()->AddString(pchMin, cch);

            if (!*pchCur) {
                // This is the last component. Handle it special below.
                break;
            }

            // This assumes all predefined types are not nested.
            NSSYM * nsNext = LookupGlobalSymCore(name, nsCur, MASK_NSSYM)->asNSSYM();
            if (!nsNext)
                nsNext = CreateNamespace(name, nsCur);
            nsCur = nsNext;
        }

        nsCur->fHasPredefs = true;
        prgptni[i].ns = nsCur;
        prgptni[i].name = name;
        prgptni[i].cch = cch;
        prgptni[i].hash = name->hash + HashUInt((UINT_PTR)nsCur);
    }

    // Now sort by hash value.
    qsort(prgptni, PT_COUNT, sizeof(PredefTypeNameInfo), &ComparePredefTypeNameInfo);
}


/***************************************************************************************************
    The importer calls this to determine whether a type needs to be loaded immediately. This is
    so predefined types are all loaded before InitPredefTypes is called.
***************************************************************************************************/
bool BSYMMGR::FPreLoad(NSSYM * ns, PCWCH pszAgg)
{
    if (!ns->fHasPredefs)
        return false;

    PCWCH pchLim;

    for (pchLim = pszAgg; *pchLim && *pchLim != '`' && *pchLim != '!'; pchLim++)
        ;

    int cch = (int)(pchLim - pszAgg);
    uint hash = NAMEMGR::HashString(pszAgg, cch) + HashUInt((UINT_PTR)ns);

    int iMin, iLim;
    for (iMin = 0, iLim = PT_COUNT; iMin < iLim; ) {
        int iMid = (iMin + iLim) / 2;
        if (prgptni[iMid].hash < hash)
            iMin = iMid + 1;
        else
            iLim = iMid;
    }

    ASSERT(iMin >= PT_COUNT || prgptni[iMin].hash >= hash);
    ASSERT(iMin == 0 || prgptni[iMin - 1].hash < hash);

    for ( ; iMin < PT_COUNT && prgptni[iMin].hash == hash; iMin++) {
        if (ns == prgptni[iMin].ns && cch == prgptni[iMin].cch &&
            !wcsncmp(pszAgg, prgptni[iMin].name->text, cch))
        {
            return true;
        }
    }

    return false;
}


/***************************************************************************************************
    Initialize the predefined types.
    
    If we haven't found mscorlib yet, we first look for System.Object in kaidGlobal and set
    aidMsCorLib to the assembly containing it.

    We look in both aidMsCorLib and kaidGlobal for all predefined types. If the type isn't in
    aidMsCorLib but is in kaidGlobal we produce a warning and use the one we found. If we find
    the type in aidMsCorLib and find another type with the same fully qualified name in kaidGlobal,
    we warn and use the one in aidMsCorLib.

    Returns true if all of the required types are found, false otherwise
***************************************************************************************************/
bool BSYMMGR::InitPredefinedTypes()
{
    ASSERT(!predefSyms);

    PAGGSYM sym;

    if (aidMsCorLib == kaidNil) {
        // If we haven't found mscorlib yet, first look for System.Object. Then use its assembly as
        // the location for all other pre-defined types.
        AGGSYM * aggObj = FindPredefinedType(predefTypeInfo[PT_OBJECT].fullName, kaidGlobal, AggKind::Class, 0, true);
        if (!aggObj)
            return false;
        aidMsCorLib = aggObj->GetAssemblyID();
    }

    if (aidMsCorLib != kaidThisAssembly)
        GetInfileForAid(aidMsCorLib)->isBCL = true;

    bool fAllRequiredFound = true;
        predefSyms = (PAGGSYM *) allocGlobal->AllocZero(sizeof(PAGGSYM) * PT_COUNT);

    for (int i = 0; i < PT_COUNT; ++i) {
        const PredefTypeInfo * ppti = &predefTypeInfo[i];


        sym = FindPredefinedType(ppti->fullName, aidMsCorLib, ppti->aggKind, ppti->arity, false);
        if (!sym) {
            // Not found in mscorlib. Look in all of global.
            sym = FindPredefinedType(ppti->fullName, kaidGlobal, ppti->aggKind, ppti->arity, ppti->isRequired);
            if (sym && ppti->inmscorlib) {
                // Warn that the type isn't where expected.
                ErrArg arg;
                if (aidMsCorLib == kaidThisAssembly)
                    arg = ErrArgIds(IDS_ThisAssembly);
                else {
                    arg = ErrArg(GetInfileForAid(aidMsCorLib));
                    ASSERT(arg.sym);
                }
                Error(WRN_UnexpectedPredefTypeLoc, sym, arg, sym->DeclFirst()->getInputFile());
            }
        }

        if (sym) {
            ASSERT(sym->AggKind() == ppti->aggKind && sym->typeVarsAll->size == ppti->arity);

            sym->isPredefined = true;
            sym->iPredef = i;
            ASSERT(sym->iPredef == (unsigned)i); // Assert that the bitfield is large enough.
            sym->fSkipUDOps = i <= PT_ENUM && i != PT_INTPTR && i != PT_UINTPTR;
        }
        else if (ppti->isRequired) {
            // We still want to report all of the missing required types!
            fAllRequiredFound = false;
        }

        predefSyms[i] = sym;
    }
    if (!fAllRequiredFound) {
        // Don't bother continuing -- we're dead in the water.
        return false;
    }

    // set up the root of the attribute hierarchy
    sym = GetReqPredefAgg(PT_ATTRIBUTE);
    sym->isAttribute = true;

    // need to set this up because the attribute class has the attribute usage attribute on it
    sym = GetOptPredefAgg(PT_ATTRIBUTEUSAGE);
    if (sym) sym->attributeClass = catClass;
    if (sym) sym->isAttribute = true;

    sym = GetOptPredefAgg(PT_ATTRIBUTETARGETS);
    if (sym) sym->SetAggKind(AggKind::Enum);

    sym = GetOptPredefAgg(PT_CONDITIONAL);
    if (sym) sym->isAttribute = true;
    if (sym) sym->attributeClass = catMethod;

    sym = GetOptPredefAgg(PT_OBSOLETE);
    if (sym) sym->isAttribute = true;
    if (sym) sym->attributeClass = catAll;

    sym = GetOptPredefAgg(PT_CLSCOMPLIANT);
    if (sym) sym->isAttribute = true;
    if (sym) sym->attributeClass = (CorAttributeTargets)(catAssembly | catClassMembers);

    // set up the root of the security attribute hierarchy
    sym = GetOptPredefAgg(PT_SECURITYATTRIBUTE);
    if (sym) sym->isSecurityAttribute = true;

    // set up the root of for marshalbyref types.
    sym = GetOptPredefAgg(PT_MARSHALBYREF);
    if (sym) sym->isMarshalByRef = true;
    sym = GetOptPredefAgg(PT_CONTEXTBOUND);
    if (sym) sym->isMarshalByRef = true;

    return true;
}



/*
 * finds an existing declaration for a predefined type
 * returns NULL on failure. If isRequired is true, an error message is also given.
 */
AGGSYM * BSYMMGR::FindPredefinedType(PCWSTR pszType, int aid, AggKindEnum aggKind, int arity, bool isRequired)
{
    ASSERT(*pszType); // Shouldn't be the empty string!

    NSSYM * nsCur = GetRootNS();
    NAME * name;

    for (const WCHAR * pchCur = pszType; ; pchCur++) {
        // Get the next name component.
        const WCHAR * pchMin = pchCur;
        while (*pchCur && *pchCur != L'.')
            pchCur++;

        name = host()->getNamemgr()->AddString(pchMin, (int)(pchCur - pchMin));

        if (!*pchCur) {
            // This is the last component. Handle it special below.
            break;
        }

        // This assumes all predefined types are not nested.
        nsCur = LookupGlobalSymCore(name, nsCur, MASK_NSSYM)->asNSSYM();
        if (!nsCur || !nsCur->InAlias(this, aid)) {
            // Didn't find the namespace in this aid.
            if (isRequired)
                Error(ERR_PredefinedTypeNotFound, pszType);
            return NULL;
        }
    }

    AGGSYM * aggAmbig;
    AGGSYM * aggBad;
    AGGSYM * aggFound = FindPredefinedTypeCore(name, nsCur, aid, aggKind, arity, &aggAmbig, &aggBad);

    if (!aggFound) {
        // Didn't find the AGGSYM.
        if (aggBad && (isRequired || aid == kaidGlobal && aggBad->isSource))
            ErrorRef(ERR_PredefinedTypeBadType, aggBad);
        else if (isRequired)
            Error(ERR_PredefinedTypeNotFound, pszType);
        return NULL;
    }

    if (!aggAmbig && aid != kaidGlobal) {
        // Look in kaidGlobal to make sure there isn't a conflicting one.
        AGGSYM * agg2 = FindPredefinedTypeCore(name, nsCur, kaidGlobal, aggKind, arity, &aggAmbig, NULL);
        ASSERT(agg2);
        if (agg2 != aggFound)
            aggAmbig = agg2;
    }

    if (aggAmbig) {
        Error(WRN_MultiplePredefTypes, pszType, aggFound->DeclFirst()->getInputFile()->name);
    }

    return aggFound;
}


AGGSYM * BSYMMGR::FindPredefinedTypeCore(NAME * name, NSSYM * ns, int aid, AggKindEnum aggKind, int arity,
    AGGSYM ** paggAmbig, AGGSYM ** paggBad)
{
    AGGSYM * aggFound = NULL;
    if (paggAmbig)
        *paggAmbig = NULL;
    if (paggBad)
        *paggBad = NULL;

    for (AGGSYM * aggCur = LookupGlobalSymCore(name, ns, MASK_AGGSYM)->asAGGSYM();
        aggCur;
        aggCur = LookupNextSym(aggCur, ns, MASK_AGGSYM)->asAGGSYM())
    {
        if (!aggCur->InAlias(aid) || aggCur->typeVarsAll->size != arity)
            continue;
        if (aggCur->AggKind() != aggKind) {
            if (paggBad && !*paggBad)
                *paggBad = aggCur;
            continue;
        }
        if (aggFound) {
            ASSERT(paggAmbig && !*paggAmbig);
            *paggAmbig = aggCur;
            break;
        }
        aggFound = aggCur;
        if (!paggAmbig)
            break;
    }

    return aggFound;
}


void BSYMMGR::ReportMissingPredefTypeError(PREDEFTYPE pt)
{
    ASSERT(0 <= pt && pt < PT_COUNT && !predefSyms[pt] && !predefTypeInfo[pt].isRequired);

    Error(ERR_PredefinedTypeNotFound, FetchAtIndex(predefTypeInfo,pt).fullName);
}

static void ReleaseXMLDocuments (PPARENTSYM symRoot)
{
}


/*
 * Free all memory associated with the symbol manager.
 */
void BSYMMGR::Term()
{

    ReleaseXMLDocuments(xmlfileroot);
    xmlfileroot = NULL;
    
    tableGlobal.Term();
    tableNameToSym.Term();
    tableTokenToSym.Term();
    tableTypeArrays.Term();
    ssetAssembly.Term();
    ssetModule.Term();
}

void LSYMMGR::Term()
{
    tableLocal.Term();
}


/*
 * Clear the local table in preperation for the next fuction
 */
void LSYMMGR::DestroyLocalSymbols()
{
    tableLocal.Clear();
}

/*
 * Clear the local table of any SK_CACHESYMs
 */
void LSYMMGR::RemoveChildSyms(PPARENTSYM parent, symbmask_t kindmask)
{
    tableLocal.ClearChildren(parent, kindmask);
}


/*
 * Helper routine to allocator a symbol structor from the given heap,
 * and assign the given name (if appropriate). This code is kind of
 * repetitive, but its hard to do it in a generic way with the new
 * operator. It's fast, which is most important.
 */
__forceinline PSYM BSYMMGR::AllocSymWorker(SYMKIND symkind, PNAME name, NRHEAP * allocator, int * psidLast)
{
    SYM * sym;

    switch (symkind) {

#define SYMBOLDEF(kind, global, local) \
    case SK_ ## kind: \
        sym = new(allocator, psidLast) kind; \
        sym->name = name; \
        sym->fHasSid = !!psidLast; \
        break;
#include "symkinds.h"

    default:
        // Illegal symbol kind. This shouldn't happen.
        ASSERT(0);
        return NULL;
    }

    sym->setKind(symkind);
    return sym;
}


__forceinline PSYM BSYMMGR::AllocSym(SYMKIND symkind, PNAME name, NRHEAP * allocator)
{
    
    PSYM sym = AllocSymWorker(symkind, name, allocator, NULL);


    return sym;

}


/* 
 * Add a named symbol to a parent scope, for later lookup.
 */
void BSYMMGR::AddChild(SYMTBL *table, PPARENTSYM parent, PSYM child)
{
    ASSERT(child->nextSameName == NULL);

    parent->AddToChildList(child);

    table->InsertChild(parent, child);
}

/* 
 * The main routine for creating a global symbol and putting it into the
 * symbol table under a particular parent. Either name or parent can
 * be NULL.
 */
PSYM BSYMMGR::CreateGlobalSym(SYMKIND symkind, PNAME name, PPARENTSYM parent)
{
    PSYM sym;

    // Only some symbol kinds are valid as global symbols. Validate.
#ifdef DEBUG
    switch (symkind) {
#define SYMBOLDEF(name, global, local) case SK_ ## name: if (global) break; goto LFail;
#include "symkinds.h"

    default:
LFail:
        VSFAIL("Bad symkind in CreateGlobalSym");
        break;
    }
#endif // DEBUG

    // Allocate the symbol from the global allocator and fill in the name member.
    sym = AllocSym(symkind, name, allocGlobal);
    ASSERT(!sym->isLocal);

    if (parent) {
        // Set the parent element of the child symbol.
        AddChild(&tableGlobal, parent, sym);
    }

    return sym;
}

/* 
 * The main routine for creating a local symbol and putting it into the
 * symbol table under a particular parent. Either name or parent can
 * be NULL.
 */
PSYM LSYMMGR::CreateLocalSym(SYMKIND symkind, PNAME name, PPARENTSYM parent)
{
    PSYM sym;

    // Only some symbol kinds are valid as local symbols. Validate.
#ifdef DEBUG
    switch (symkind) {
#define SYMBOLDEF(name, global, local) case SK_ ## name: if (local) break; goto LFail;
#include "symkinds.h"

    default:
LFail:
        VSFAIL("Bad symkind in CreateLocalSym");
        break;
    }
#endif // DEBUG

    // Allocate the symbol from the local allocator and fill in the name member.
    sym = AllocSym(symkind, name, allocLocal, NULL);
    sym->isLocal = true;

    if (parent) {
        // Set the parent element of the child symbol.
        AddChild(&tableLocal, parent, sym);
    }

    return sym;
}


/*
 * The main routine for looking up a global symbol by name. It's possible for
 * there to be more that one symbol with a particular name in a particular
 * parent; if you want to check for more, then use LookupNextSym.
 *
 * kindmask filters the result by particular symbol kinds.
 *
 * returns NULL if no match found.
 */
PSYM BSYMMGR::LookupGlobalSymCore(PNAME name, PPARENTSYM parent, symbmask_t kindmask)
{
    return tableGlobal.LookupSym(name, parent, kindmask);
}


/******************************************************************************
    Find a member of the given AGGSYM. This never has to be concerned about
    filtering on aid.
******************************************************************************/
SYM * BSYMMGR::LookupAggMember(NAME * name, AGGSYM * agg, symbmask_t mask)
{
    return tableGlobal.LookupSym(name, agg, mask);
}


/***************************************************************************************************
    Find the invoke method of the delegate AGGSYM.
***************************************************************************************************/
METHSYM * BSYMMGR::LookupInvokeMeth(AGGSYM * aggDel)
{
    ASSERT(aggDel->AggKind() == AggKind::Delegate);

    for (SYM * sym = LookupAggMember(host()->getNamemgr()->GetPredefName(PN_INVOKE), aggDel, MASK_ALL);
        sym;
        sym = sym->nextSameName)
    {
        if (sym->isMETHSYM() && sym->asMETHSYM()->isInvoke())
            return sym->asMETHSYM();
    }

    return NULL;
}


/*
 * The main routine for looking up a local symbol by name. It's possible for
 * there to be more than one symbol with a particular name in a particular
 * parent; if you want to check for more, then use LookupNextSym.
 *
 * kindmask filters the result by particular symbol kinds.
 *
 * returns NULL if no match found.
 */
PSYM LSYMMGR::LookupLocalSym(PNAME name, PPARENTSYM parent, symbmask_t kindmask)
{
    ASSERT(name);       // name can't be NULL.

    return tableLocal.LookupSym(name, parent, kindmask);
}


/*
 * Look up the next symbol with the same name and parent.
 */
PSYM BSYMMGR::LookupNextSym(PSYM sym, PPARENTSYM parent, symbmask_t kindmask)
{
    ASSERT(sym->parent == parent);

    sym = sym->nextSameName;
    ASSERT(!sym || sym->parent == parent);

    // Keep traversing the list of symbols with same name and parent.
    while (sym) {
        if (kindmask & sym->mask())
            return sym;

        sym = sym->nextSameName;
        ASSERT(!sym || sym->parent == parent);
    }
    return NULL;
}


/*
 * Specific creation functions.
 * These functions create specific kinds of symbols.
 */


/*
 * Create a namespace symbol.
 */
PNSSYM BSYMMGR::CreateNamespace(PNAME name, PNSSYM parent)
{
    // Caller should make sure the namespace doesn't already exist.
    ASSERT(LookupGlobalSymCore(name, parent, MASK_NSSYM) == NULL);

    //
    // create and initialize the symbol.
    //
    // NOTE: we don't use CreateGlobalSym() because we don't want
    //       to link the new SYM into its parent's list of children
    NSSYM *ns = AllocSym(SK_NSSYM, name, allocGlobal)->asNSSYM();
    ns->SetAccess(ACC_PUBLIC);    // namespaces all have public access
    if (parent) {
        tableGlobal.InsertChild(parent, ns);
    }

    return ns;
}

/*
 * Create a namespace declaration symbol.
 *
 * nspace is the namespace for the declaration to be created.
 * parent is the containing declaration for the new declaration.
 * parseTree is the parseTree fro the new declaration.
 */
NSDECLSYM * BSYMMGR::CreateNamespaceDecl(NSSYM * ns, NSDECLSYM * nsdPar, INFILESYM * infile, NAMESPACENODE * parseTree)
{
    ASSERT(infile);
    // Input file must match parent's input file if we have a parent.
    ASSERT(!nsdPar || infile && nsdPar->inputfile == infile);
    // Namespace parent must match declaration's container.
    ASSERT(nsdPar && nsdPar->NameSpace() && nsdPar->NameSpace() == ns->Parent() ||
           !nsdPar && !ns->Parent() && ns == GetRootNS());

    // Create and initialize the symbol.
    //
    // NOTE: we don't use CreateGlobalSym() because we don't want
    //       to insert the new SYM into the global lookup table.
    //       This requires us to do a bunch of stuff manually.
    //
    NSDECLSYM * nsd = AllocSym(SK_NSDECLSYM, ns->name, allocGlobal)->asNSDECLSYM();
    nsd->inputfile = infile;
    nsd->parseTree = parseTree;

    // Link into the parent's child list at the end.
    ns->AddDecl(nsd, allocGlobal);
    if (nsdPar)
        nsdPar->AddToChildList(nsd);

    // Set some bits on the namespace.
    if (infile->isSource)
        ns->isDefinedInSource = true;

    return nsd;
}


/*
 * Create a symbol for an aggregate type: class, struct, interface, or enum.
 */
AGGSYM * BSYMMGR::CreateAgg(NAME * name, DECLSYM * declOuter)
{
    ASSERT(declOuter->isNSDECLSYM() || declOuter->isAGGDECLSYM());

    BAGSYM * bag = declOuter->Bag();

    // Create the new symbol.
    AGGSYM * aggNew;

    if (declOuter->GetAssemblyID() == kaidUnresolved) {
        // Unresolved aggs need extra storage.
        SYM * sym = CreateGlobalSym(SK_UNRESAGGSYM, name, bag);
        sym->setKind(SK_AGGSYM);
        aggNew = sym->asAGGSYM();
    }
    else {
        aggNew = CreateGlobalSym(SK_AGGSYM, name, bag)->asAGGSYM();
    }
    aggNew->symmgr = this;

    aggNew->InitFromOuterDecl(declOuter);

    ASSERT(!declOuter->isAGGDECLSYM() || declOuter->asAGGDECLSYM()->Agg()->isSource == aggNew->isSource);

    return aggNew;
}

/*
 * Create a symbol for a forwarded aggregate type.
 */
FWDAGGSYM * BSYMMGR::CreateFwdAgg(NAME * name, NSDECLSYM * nsd)
{
    ASSERT(nsd->GetAssemblyID() != kaidUnresolved);
    ASSERT(!nsd->getInputFile()->isSource);

    FWDAGGSYM * fwd = CreateGlobalSym(SK_FWDAGGSYM, name, nsd->NameSpace())->asFWDAGGSYM();
    fwd->infile = nsd->getInputFile();

    return fwd;
}


AGGDECLSYM * BSYMMGR::CreateAggDecl(AGGSYM * agg, DECLSYM * declOuter)
{
    ASSERT(agg && declOuter);

    AGGDECLSYM * declNew = AllocSym(SK_AGGDECLSYM, agg->name, allocGlobal)->asAGGDECLSYM();
    declOuter->AddToChildList(declNew);
    agg->AddDecl(declNew, allocGlobal);

    return declNew;
}

/*
 * Create a symbol for a using alias clause.
 */
PALIASSYM BSYMMGR::CreateAlias(PNAME name)
{
    // Create the new symbol.
    return CreateGlobalSym(SK_ALIASSYM, name, NULL)->asALIASSYM();
}

/*
 * Create a symbol for a global attribute.
 */
PGLOBALATTRSYM SYMMGR::CreateGlobalAttribute(PNAME name, NSDECLSYM *parent)
{
    // Create the new symbol.
    return CreateGlobalSym(SK_GLOBALATTRSYM, name, parent)->asGLOBALATTRSYM();
}

/*
 * Create a symbol for an member variable.
 */
PMEMBVARSYM BSYMMGR::CreateMembVar(PNAME name, PAGGSYM parent, PAGGDECLSYM declaration)
{
    // Create the new symbol.
    PMEMBVARSYM sym = CreateGlobalSym(SK_MEMBVARSYM, name, parent)->asMEMBVARSYM();
    sym->declaration = declaration;
    sym->iIteratorLocal = -1;
    return sym;
}


/*
 * Create a symbol for a type parameter declaration and/or use of that 
 * declaration.  The parent is the class or method that is parameterized
 * by the type parameter.
 */
PTYVARSYM BSYMMGR::CreateTyVar(PNAME name, PPARENTSYM parent)
{
    // Create the new symbol.
    TYVARSYM * ptvs = CreateGlobalSym(SK_TYVARSYM, name, parent)->asTYVARSYM();
    ptvs->isMethTyVar = parent && parent->isMETHSYM();
    ASSERT(!ptvs->fHasErrors);
    ptvs->fUnres = (parent && parent->isAGGSYM() && parent->asAGGSYM()->IsUnresolved());
    return ptvs;
}

/*
 * Create a symbol for a method. Does not check for existing symbols
 * because methods are assumed to be overloadable.
 */
PMETHSYM BSYMMGR::CreateMethod(PNAME name, PAGGSYM parent, PAGGDECLSYM declaration)
{
    // Create the new symbol.
    PMETHSYM sym = CreateGlobalSym(SK_METHSYM, name, parent)->asMETHSYM();
    sym->declaration = declaration;
    return sym;
}

PIFACEIMPLMETHSYM BSYMMGR::CreateIfaceImplMethod(PAGGSYM parent, PAGGDECLSYM declaration)
{
    IFACEIMPLMETHSYM *sym = (IFACEIMPLMETHSYM*) CreateGlobalSym(SK_IFACEIMPLMETHSYM, NULL, parent);
    sym->setKind(SK_METHSYM); // these syms really want to be methods + a little bit
    sym->isIfaceImpl = true;
    sym->declaration = declaration;
    return sym;
}

/* 
 * Create a symbol for an property. Does not check for existing symbols
 * because properties are assumed to be overloadable.
 */
PPROPSYM BSYMMGR::CreateProperty(PNAME name, PAGGSYM parent, PAGGDECLSYM declaration)
{
    // Create the new symbol.
    PPROPSYM sym = CreateGlobalSym(SK_PROPSYM, name, parent)->asPROPSYM();
    sym->declaration = declaration;
    return sym;
}


/*
 * Create a symbol for an property. Does not check for existing symbols
 * because properties are assumed to be overloadable.
 */
PINDEXERSYM BSYMMGR::CreateIndexer(PNAME name, PAGGSYM parent, PAGGDECLSYM declaration)
{
    // Create the new symbol.
    PINDEXERSYM indexer = (INDEXERSYM*) CreateGlobalSym(SK_INDEXERSYM, name ? host()->getNamemgr()->GetPredefName(PN_INDEXERINTERNAL) : NULL, parent);
    indexer->realName = name;
    indexer->setKind(SK_PROPSYM);
    indexer->isOperator = true;
    indexer->declaration = declaration;

    return indexer;
}

/*
 * Create an event symbol. Does not check for existing symbols.
 */
PEVENTSYM BSYMMGR::CreateEvent(PNAME name, PAGGSYM parent, PAGGDECLSYM declaration)
{
    PEVENTSYM sym = CreateGlobalSym(SK_EVENTSYM, name, parent)->asEVENTSYM();
    sym->declaration = declaration;
    return sym;
}


MODULESYM * BSYMMGR::CreateModule(NAME * name, INFILESYM * infile)
{
    MODULESYM * module = CreateGlobalSym(SK_MODULESYM, name, infile)->asMODULESYM();
    int aid = AidAlloc(module);
    module->SetModuleID(aid);

    return module;
}


/*
 * Handling "derived types".
 *
 * There are a set of types that are "derived" types, in that they
 * are derived from standard types. Pointer types and array types are
 * the obvious examples. We don't want to allocate new symbols for these
 * every time we come across them. We handle these by using the parent
 * lookup mechanism do the work for us, we use the "base" or "element" type of the
 * derived type being the parent type, with a special weird name for looking
 * up the derived type by.
 */


/***************************************************************************************************
    Fill in the state tracking information for a "derived" type from the information in the parent.
***************************************************************************************************/
void TYPESYM::InitFromParent()
{
    ASSERT(!isAGGTYPESYM());
    TYPESYM * typePar = parent->asTYPESYM();

    this->fHasErrors = typePar->HasErrors();
    this->fUnres = typePar->IsUnresolved();
}


/*
 * Create or return an existing array symbol. We use the lookup mechanism
 * to find unique array symbols efficiently. The parent of an array symbol
 * is the element type, and the name is "[X<n+1>", where the second character
 * has the rank.
 */

PARRAYSYM BSYMMGR::GetArray(PTYPESYM elementType, int args)
{
    WCHAR nameString[4];
    PNAME name;
    ARRAYSYM *sym;

    ASSERT(args > 0 && args < 32767);

    switch (args) {
    case 1: case 2:
        name = host()->getNamemgr()->GetPredefName((PREDEFNAME) (PN_ARRAY0 + args));
        break;
        // fall through
    default:
        nameString[0] = L'[';
        nameString[1] = L'X';
        nameString[2] = args + 1;
        nameString[3] = L'\0';
        name = host()->getNamemgr()->AddString(nameString);
    }

    // See if we already have a array symbol of this element type and rank.
    sym = LookupGlobalSymCore(name, elementType, MASK_ARRAYSYM)->asARRAYSYM();
    if (! sym) {
        // No existing array symbol. Create a new one.
        sym = CreateGlobalSym(SK_ARRAYSYM, name, elementType)->asARRAYSYM();
        sym->rank = args;
        sym->InitFromParent();
    }
    else {
        ASSERT(sym->fHasErrors == elementType->HasErrors());
        ASSERT(sym->fUnres == elementType->IsUnresolved());
    }

    ASSERT(sym->rank == args);
    ASSERT(sym->elementType() == elementType);

    return sym;
}

/*
 * Create or return an existing pointer symbol. The parent of a pointer symbol
 * is the base type, and the name is "*"
 */
PPTRSYM BSYMMGR::GetPtrType(PTYPESYM baseType)
{
    PPTRSYM sym;
    PNAME namePtr = host()->getNamemgr()->GetPredefName(PN_PTR);

    // See if we already have a pointer symbol of this base type.
    sym = LookupGlobalSymCore(namePtr, baseType, MASK_PTRSYM)->asPTRSYM();
    if (! sym) {
        // No existing array symbol. Create a new one.
        sym = CreateGlobalSym(SK_PTRSYM, namePtr, baseType)->asPTRSYM();
        sym->InitFromParent();
    }
    else {
        ASSERT(sym->fHasErrors == baseType->HasErrors());
        ASSERT(sym->fUnres == baseType->IsUnresolved());
    }

    ASSERT(sym->baseType() == baseType);

    return sym;
}


/***************************************************************************************************
    Get / create the NUBSYM for the given base type. The base type is the parent of the NUBSYM.
***************************************************************************************************/
NUBSYM * BSYMMGR::GetNubType(TYPESYM * typeBase)
{
    NAME * name = host()->getNamemgr()->GetPredefName(PN_NUB);

    // See if we already have a pointer symbol of this base type.
    NUBSYM * nub = LookupGlobalSymCore(name, typeBase, MASK_NUBSYM)->asNUBSYM();
    if (!nub) {
        nub = CreateGlobalSym(SK_NUBSYM, name, typeBase)->asNUBSYM();
        nub->InitFromParent();
        nub->symmgr = this;
    }
    else {
        ASSERT(nub->fHasErrors == typeBase->HasErrors());
        ASSERT(nub->fUnres == typeBase->IsUnresolved());
        ASSERT(nub->symmgr == this);
    }

    ASSERT(nub->baseType() == typeBase);

    return nub;
}


TYPESYM * BSYMMGR::GetNubTypeOrError(TYPESYM * typeBase)
{
    if (!GetNullable()) {
        ReportMissingPredefTypeError(PT_G_OPTIONAL);
        TypeArray * ta = AllocParams(1, &typeBase);
        NAME * name = host()->getNamemgr()->LookupString(L"Nullable");
        PARENTSYM * symPar = LookupGlobalSymCore(host()->getNamemgr()->LookupString(L"System"), rootNS, MASK_NSSYM)->asPARENTSYM();
        return GetErrorType(symPar, name, ta);
    }

    return GetNubType(typeBase);
}


/***************************************************************************************************
    Get the equivalent Nullable<T> for the given T?.
    WARNING: This may return NULL if the Nullable predefined type is not found!
***************************************************************************************************/
AGGTYPESYM * NUBSYM::GetAts()
{
    if (!this->ats) {
        AGGSYM * aggNullable = this->symmgr->GetNullable();
        if (!aggNullable) {
            this->symmgr->ReportMissingPredefTypeError(PT_G_OPTIONAL);
            return NULL;
        }

        TYPESYM * typePar = this->parent->asTYPESYM();
        TypeArray * ta = this->symmgr->AllocParams(1, &typePar);
        this->ats = this->symmgr->GetInstAgg(aggNullable, ta);
    }

    return this->ats;
}


/***************************************************************************************************
    Get the equivalent T? for a Nullable<T>.
***************************************************************************************************/
NUBSYM * BSYMMGR::GetNubFromNullable(AGGTYPESYM * ats)
{
    ASSERT(ats->isPredefType(PT_G_OPTIONAL));
    return GetNubType(ats->typeArgsAll->Item(0));
}

TYPESYM * BSYMMGR::MaybeConvertNullableToNub(TYPESYM * ts)
{
    if (ts->isAGGTYPESYM() && ts->asAGGTYPESYM()->isPredefType(PT_G_OPTIONAL)) {
        return GetNubFromNullable(ts->asAGGTYPESYM());
    } else {
        return ts;
    }
}

TYPESYM * BSYMMGR::MaybeConvertNubToNullable(TYPESYM * ts)
{
    if (ts->isNUBSYM()) {
        return ts->asNUBSYM()->GetAts();
    } else {
        return ts;
    }
}


NAME * BSYMMGR::GetNameFromPtrs(UINT_PTR u1, UINT_PTR u2)
{
    // We have to make sure none of the characters is zero, since NAME's don't store their length -
    // they rely on null termination.
    ASSERT(sizeof(UINT_PTR) % sizeof(WCHAR) == 0);
    const int cchPtr = sizeof(UINT_PTR) / sizeof(WCHAR);
    WCHAR rgchName[2 * cchPtr + 1];

    *reinterpret_cast<UINT_PTR *>(rgchName) = u1;
    *reinterpret_cast<UINT_PTR *>(rgchName + cchPtr) = u2;

    int cchRaw = cchPtr * 2;
    while (cchRaw > 0 && rgchName[cchRaw - 1] == 0)
        cchRaw--;

    WCHAR chExtra = 1;
    for (int ich = cchRaw; --ich >= 0; ) {
        chExtra = (chExtra << 1) | rgchName[ich] & 1;
        rgchName[ich] |= 1;
    }

    rgchName[cchRaw] = (chExtra << 1) | 1;
    return host()->getNamemgr()->AddString(rgchName, cchRaw + 1);
}


/***************************************************************************************************
    Create / fetch an instantiated aggregate, eg, List<string>. The parent is the AGGSYM. The
    name is a merge of the outer type and type args. The resulting AGGTYPESYM is NOT placed
    in the child list of the AGGSYM.
***************************************************************************************************/
AGGTYPESYM * BSYMMGR::GetInstAgg(AGGSYM * agg, AGGTYPESYM * atsOuter, TypeArray * typeArgs, TypeArray * typeArgsAll)
{
    ASSERT(this && agg->symmgr == this);
    ASSERT(!atsOuter && (!agg->Parent() || !agg->isNested()) || atsOuter->getAggregate() == agg->Parent());

    if (!typeArgs)
        typeArgs = &taEmpty;

    ASSERT(agg->typeVarsThis->size == typeArgs->size);
    ASSERT(!typeArgsAll || agg->typeVarsAll->size == typeArgsAll->size);

    NAME * name = GetNameFromPtrs((UINT_PTR)typeArgs, (UINT_PTR)atsOuter);
    ASSERT(name != NULL);

    AGGTYPESYM * ats = LookupAggMember(name, agg, MASK_AGGTYPESYM)->asAGGTYPESYM();
    if (!ats) {
        ats = CreateGlobalSym(SK_AGGTYPESYM, name, NULL)->asAGGTYPESYM();

        // Set the parent and add it to the hash table, but not to the child list.
        ats->parent = agg;
        tableGlobal.InsertChild(agg, ats);

        ats->typeArgsThis = typeArgs;
        ats->outerType = atsOuter;
        ASSERT(!ats->fConstraintsChecked && !ats->fConstraintError);

        // build list of all type parameters including ones from outer types
        if (typeArgsAll) {
            // The caller gave us pArgsAll. Assert they did it right.
            ASSERT(atsOuter || typeArgsAll == typeArgs);
            ASSERT(!atsOuter || typeArgsAll == ConcatParams(atsOuter->typeArgsAll, typeArgs));
            ats->typeArgsAll = typeArgsAll;
        }
        else if (atsOuter && atsOuter->typeArgsAll->size > 0) {
            ats->typeArgsAll = ConcatParams(atsOuter->typeArgsAll, ats->typeArgsThis);
        }
        else {
            ats->typeArgsAll = ats->typeArgsThis;
        }

        ats->fHasErrors = ats->typeArgsAll->HasErrors();
        ats->fUnres = ats->typeArgsAll->IsUnresolved() || agg->IsUnresolved();
    }
    else {
        ASSERT(ats->fHasErrors == ats->typeArgsAll->HasErrors());
        ASSERT(ats->fUnres == (ats->typeArgsAll->IsUnresolved() || agg->IsUnresolved()));
    }

    ASSERT(ats->getAggregate() == agg);
    ASSERT(ats->typeArgsThis && ats->typeArgsAll);
    ASSERT(ats->typeArgsThis == typeArgs);
    ASSERT(!typeArgsAll || ats->typeArgsAll == typeArgsAll);

    return ats;
}


AGGTYPESYM * BSYMMGR::GetInstAgg(AGGSYM * agg, TypeArray * typeArgsAll)
{
    ASSERT(typeArgsAll && typeArgsAll->size == agg->typeVarsAll->size);

    if (!typeArgsAll->size)
        return agg->getThisType();

    AGGSYM * aggOuter = agg->GetOuterAgg();

    if (!aggOuter)
        return GetInstAgg(agg, NULL, typeArgsAll, typeArgsAll);

    int cvarOuter = aggOuter->typeVarsAll->size;
    ASSERT(cvarOuter <= typeArgsAll->size);

    TypeArray * typeArgsOuter = AllocParams(cvarOuter, typeArgsAll->ItemPtr(0));
    TypeArray * typeArgsInner = AllocParams(agg->typeVarsThis->size, typeArgsAll->ItemPtr(cvarOuter));
    AGGTYPESYM * atsOuter = GetInstAgg(aggOuter, typeArgsOuter);

    return GetInstAgg(agg, atsOuter, typeArgsInner, typeArgsAll);
}


/*
 * Create or return an existing pinned symbol. The parent of a pinned symbol
 * is the base type, and the name is "@"
 */
PPINNEDSYM BSYMMGR::GetPinnedType(PTYPESYM baseType)
{
    PPINNEDSYM sym;

    PNAME namePinned = host()->getNamemgr()->GetPredefName(PN_PINNED);

    // See if we already have a pointer symbol of this base type.
    sym = LookupGlobalSymCore(namePinned, baseType, MASK_PINNEDSYM)->asPINNEDSYM();
    if (! sym) {
        // No existing array symbol. Create a new one.
        sym = CreateGlobalSym(SK_PINNEDSYM, namePinned, baseType)->asPINNEDSYM();
        sym->InitFromParent();
    }
    else {
        ASSERT(sym->fHasErrors == baseType->HasErrors());
        ASSERT(sym->fUnres == baseType->IsUnresolved());
    }

    ASSERT(sym->baseType() == baseType);

    return sym;
}

/*
 * Create or return an param modifier symbol. This symbol represents the
 * type of a ref or out param.
 */
PPARAMMODSYM BSYMMGR::GetParamModifier(PTYPESYM paramType, bool isOut)
{
    PNAME name = host()->getNamemgr()->GetPredefName(isOut ? PN_OUTPARAM : PN_REFPARAM);
    PPARAMMODSYM sym;

    // See if we already have a parammod symbol of this base type.
    sym = LookupGlobalSymCore(name, paramType, MASK_PARAMMODSYM)->asPARAMMODSYM();
    if (! sym) {
        // No existing parammod symbol. Create a new one.
        sym = CreateGlobalSym(SK_PARAMMODSYM, name, paramType)->asPARAMMODSYM();
        if (isOut)
            sym->isOut = true;
        else
            sym->isRef = true;
        sym->InitFromParent();
    }
    else {
        ASSERT(sym->fHasErrors == paramType->HasErrors());
        ASSERT(sym->fUnres == paramType->IsUnresolved());
    }

    ASSERT(sym->paramType() == paramType);

    return sym;
}


MODOPTTYPESYM * BSYMMGR::GetModOptType(TYPESYM * baseType, mdToken tokImport, MODULESYM * scope)
{
    NAME * name = GetNameFromPtrs(tokImport, 0);
    ASSERT(name != NULL);

    MODOPTSYM * opt = LookupGlobalSymCore(name, scope, MASK_MODOPTSYM)->asMODOPTSYM();
    if (!opt) {
        // No existing parammod symbol. Create a new one.
        opt = CreateGlobalSym(SK_MODOPTSYM, name, scope)->asMODOPTSYM();
        opt->tokImport = tokImport;
        opt->tokEmit = 0;
    }

    return GetModOptType(baseType, opt);
}


MODOPTTYPESYM * BSYMMGR::GetModOptType(TYPESYM * baseType, MODOPTSYM * opt)
{
    NAME * name = GetNameFromPtrs((UINT_PTR)opt, 0);
    ASSERT(name != NULL);

    MODOPTTYPESYM * type = LookupGlobalSymCore(name, baseType, MASK_MODOPTTYPESYM)->asMODOPTTYPESYM();
    if (!type) {
        // No existing parammod symbol. Create a new one.
        type = CreateGlobalSym(SK_MODOPTTYPESYM, name, baseType)->asMODOPTTYPESYM();
        type->opt = opt;
        type->InitFromParent();
    }
    else {
        ASSERT(type->fHasErrors == baseType->HasErrors());
        ASSERT(type->fUnres == baseType->IsUnresolved());
    }

    ASSERT(type->baseType() == baseType);

    return type;
}


NSAIDSYM * BSYMMGR::GetNsAid(NSSYM * ns, int aid)
{
    NAME * name = GetNameFromPtrs(aid, 0);
    ASSERT(name != NULL);

    NSAIDSYM * nsa = LookupGlobalSymCore(name, ns, MASK_NSAIDSYM)->asNSAIDSYM();
    if (!nsa) {
        // Create a new one.
        nsa = CreateGlobalSym(SK_NSAIDSYM, name, ns)->asNSAIDSYM();
        nsa->aid = aid;
    }

    ASSERT(nsa->GetNS() == ns);

    return nsa;
}


ERRORSYM * BSYMMGR::GetErrorType(PARENTSYM * symPar, NAME * nameText, TypeArray * typeArgs)
{
    ASSERT(nameText);
    ASSERT(!symPar || symPar->isNSAIDSYM() || symPar->isTYPESYM());
    if (!symPar)
        symPar = GetRootNsAid(kaidGlobal);
    if (!typeArgs)
        typeArgs = EmptyTypeArray();

    NAME * name = GetNameFromPtrs((UINT_PTR)nameText, (UINT_PTR)typeArgs);
    ASSERT(name != NULL);

    ERRORSYM * err = LookupGlobalSymCore(name, symPar, MASK_ERRORSYM)->asERRORSYM();
    if (!err) {
        // No existing error symbol. Create a new one.
        err = CreateGlobalSym(SK_ERRORSYM, name, symPar)->asERRORSYM();
        err->fHasErrors = true;
        err->nameText = nameText;
        err->typeArgs = typeArgs;
    }
    else {
        ASSERT(err->fHasErrors);
        ASSERT(err->nameText == nameText);
        ASSERT(err->typeArgs == typeArgs);
    }
    ASSERT(!err->fUnres);

    return err;
}


/*
 * Sets the filename of an output file to that
 * of the given input file
 */
void SYMMGR::SetOutFileName(PINFILESYM in)
{
    WCHAR filename[MAX_PATH], *filePart;
    PCWSTR pszExt;
    OUTFILESYM * outfile = in->getOutputFile();

    if (!outfile->isDll)
        pszExt = L".exe";
    else if (outfile->isManifest)
        pszExt = L".dll";
    else
        pszExt = L".netmodule";

    if (FAILED(StringCchCopyW(filename, lengthof(filename), in->name->text)) ||
        !PEFile::ReplaceFileExtension(filename, lengthof(filename), pszExt)) 
    {
        compiler()->Error(NULL, ERR_OutputFileNameTooLong, filename);
        return;
    }
    
    // point to the file part.
    filePart = PathFindFileNameW(filename);

    WCHAR buffer[MAX_PATH];
    WCBuffer wcbuffer(buffer);
    DWORD len = 0;

    len = GetCanonFilePath(filePart, wcbuffer, false);
    if (len == 0) {
        compiler()->Error(NULL, ERR_OutputFileNameTooLong, filePart);
        in->getOutputFile()->name = host()->getNamemgr()->AddString(filePart);
        return;
    }

    in->getOutputFile()->name = host()->getNamemgr()->AddString(buffer);
}

/*
 * Create a symbol representing an output file. All output files
 * are placed as children of the "fileroot" symbol.
 */
POUTFILESYM SYMMGR::CreateOutFile(PCWSTR filename, BOOL isDll, BOOL isWinApp, PCWSTR entryClass, PCWSTR resource, PCWSTR icon, PCWSTR pdbfile)
{
    if (!filename)
        filename = L"*";
    PNAME name = host()->getNamemgr()->AddString(filename);
    POUTFILESYM rval = CreateGlobalSym(SK_OUTFILESYM, name, fileroot)->asOUTFILESYM();
    rval->isResource = false; // by default
    rval->isDll = isDll ? true : false;
    rval->isConsoleApp = !isWinApp;
    rval->idFile = (mdFile)0;
    rval->idModRef = (mdModuleRef)0;
    rval->multiEntryReported = false;   // Always starts off as false
    if (resource) {
        rval->makeResFile = false;
        rval->resourceFile = allocGlobal->AllocStr(resource);
        ASSERT(!icon);
    } else {
        rval->makeResFile = true;
        if (icon)
            rval->iconFile = allocGlobal->AllocStr(icon);
        else
            ASSERT(rval->iconFile == NULL);
    }

    if (entryClass && !isDll)
        rval->entryClassName = allocGlobal->AllocStr(entryClass);
    else
        ASSERT(rval->entryClassName == NULL);

    ASSERT(rval->entrySym == NULL);
    rval->cbPublicKeyToken = 0xFFFFFFFF; // meaning uninitialized

    if (pdbfile)
        rval->pszPDBFile = allocGlobal->AllocStr(pdbfile);
    else
        ASSERT(rval->pszPDBFile == NULL);

    compiler()->cOutputFiles += 1;
    return rval;
}


/*
 * Create a symbol representing a resource file.  If bEmbed is true
 * it becomes a child of the default output file, otherwise a new
 * output file is created for itself
 */
PRESFILESYM SYMMGR::CreateEmbeddedResFile(PCWSTR filename, PCWSTR Ident, bool bVisible)
{
    RESFILESYM *resfile = CreateSeperateResFile(filename, mdfileroot, Ident, bVisible);
    resfile->isEmbedded = true;
    return resfile;
}

/*
 * Create a symbol representing a resource file.  If bEmbed is true
 * it becomes a child of the default output file, otherwise a new
 * output file is created for itself
 */
PRESFILESYM SYMMGR::CreateSeperateResFile(PCWSTR filename, OUTFILESYM *outfileSym, PCWSTR Ident, bool bVisible)
{
    PNAME name;
    PRESFILESYM resfileSym;
    POUTFILESYM pOutfile;

    name = host()->getNamemgr()->AddString(Ident);
    ASSERT(outfileSym);

    // Check for duplicates
    for (pOutfile = fileroot->firstChild->asOUTFILESYM(); pOutfile != NULL; pOutfile = pOutfile->nextOutfile()) {
        if (LookupGlobalSymCore( name, pOutfile, MASK_RESFILESYM))
            return NULL;
    }

    // Create the input file symbol.
    resfileSym = CreateGlobalSym(SK_RESFILESYM, name, outfileSym)->asRESFILESYM();
    resfileSym->filename = allocGlobal->AllocStr( filename);
    resfileSym->isVis = bVisible;

    compiler()->NotifyHostOfBinaryFile(filename);

    return resfileSym;
}

/*
 * Create a symbol representing an imported metadata input file. 
 * All imported metadata input files are children of the MDFileRoot
 */
PINFILESYM SYMMGR::CreateMDFile(PCWSTR filename, int aid, PARENTSYM *parent)
{
    // kaidNil means allocate the next aid.
    ASSERT(aid == kaidNil || aid >= kaidThisAssembly);

    PNAME name;
    PINFILESYM infileSym;

    name = host()->getNamemgr()->AddString(filename);

    infileSym = LookupGlobalSymCore(name, parent, MASK_INFILESYM)->asINFILESYM();
    if (infileSym && !infileSym->isSource && infileSym->isAddedModule == (aid == kaidThisAssembly)) {
        // If we have a match then just return it.
        ASSERT(aid == kaidNil || aid == infileSym->GetAssemblyID());
        return infileSym;
    }

    // Create the input file symbol.
    infileSym = CreateGlobalSym(SK_INFILESYM, name, parent)->asINFILESYM();
    ASSERT(!infileSym->isSource);

    if (aid == kaidNil)
        aid = AidAlloc(infileSym);

    infileSym->SetAssemblyID(aid, allocGlobal);

    infileSym->idLocalAssembly = mdTokenNil;
    infileSym->isAddedModule = (aid == kaidThisAssembly);
    ASSERT(!infileSym->hasModuleCLSattribute);

    compiler()->NotifyHostOfMetadataFile(filename);

    infileSym->getOutputFile()->cInputFiles += 1;
    compiler()->cInputFiles += 1;

    return infileSym;
}

/*
 * Create a symbol representing an synthetized input file (used if #line has a source file name)
 */
PINFILESYM SYMMGR::CreateSynthSourceFile(PCWSTR filename, OUTFILESYM *outfile)
{
    PNAME name;
    PINFILESYM infileSym;

    name = host()->getNamemgr()->AddString(filename);

    // Create the input file symbol.
    infileSym = CreateGlobalSym(SK_SYNTHINFILESYM, name, outfile)->asANYINFILESYM();

    return infileSym;
}

/*
 * Create a symbol representing an input file, which creates
 * a given output file. All input files are placed as children of
 * their output files.
 */
PINFILESYM SYMMGR::CreateSourceFile(PCWSTR filename, OUTFILESYM *outfile)
{
    PNAME name;
    PINFILESYM infileSym;

    name = host()->getNamemgr()->AddString(filename);
    outfile->cInputFiles += 1;
    compiler()->cInputFiles += 1;

    // Create the input file symbol.
    infileSym = CreateGlobalSym(SK_INFILESYM, name, outfile)->asINFILESYM();
    infileSym->isSource = true;
    infileSym->idLocalAssembly = (mdAssembly) mdTokenNil;
    ASSERT(!infileSym->hasModuleCLSattribute);

    infileSym->SetAssemblyID(kaidThisAssembly, allocGlobal);

    return infileSym;
}

/*
 * Find the INFILESYM with a given name.
 */
PINFILESYM BSYMMGR::FindInfileSym(NAME * filename)
{
    POUTFILESYM pOutfile;

    // Check all outfile sym.
    for (pOutfile = fileroot->firstChild->asOUTFILESYM(); pOutfile != NULL; pOutfile = pOutfile->nextOutfile()) {
        SYM * sym = LookupGlobalSymCore( filename, pOutfile, MASK_INFILESYM);
        if (sym)
            return sym->asINFILESYM();
    }

    // This is much slower, but does case-insensitive, which is needed in some cases.
    for (pOutfile = fileroot->firstChild->asOUTFILESYM(); pOutfile != NULL; pOutfile = pOutfile->nextOutfile()) {
        for (PINFILESYM pInfile = pOutfile->firstInfile(); pInfile != NULL; pInfile = pInfile->nextInfile()) {
        if (CompareNoCase(pInfile->name->text, filename->text) == 0)
            return pInfile;
        }
    }

    return NULL;
}




PINFILESYM BSYMMGR::GetInfileForAid(int aid)
{
    ASSERT(aid > kaidThisAssembly);
    SYM * sym = GetSymForAid(aid);
    if (!sym)
        return NULL;
    if (sym->isINFILESYM())
        return sym->asINFILESYM();
    if (sym->isMODULESYM())
        return sym->asMODULESYM()->getInputFile();
    return NULL;
}


PSYM BSYMMGR::GetSymForAid(int aid)
{
    if (aid >= kaidMinModule) {
        SYM * sym = ssetModule.GetSym(aid - kaidMinModule);
        ASSERT(sym);
        ASSERT(sym->isMODULESYM() || sym->isOUTFILESYM());
        ASSERT(!sym->isMODULESYM() || sym->asMODULESYM()->GetModuleID() == aid);
        ASSERT(!sym->isOUTFILESYM() || sym->asOUTFILESYM()->GetModuleID() == aid);
        return sym;
    }

    if (aid >= kaidUnresolved) {
        SYM * sym = ssetAssembly.GetSym(aid - kaidUnresolved);
        ASSERT(sym);
        ASSERT(sym->isINFILESYM() || sym->isEXTERNALIASSYM());
        ASSERT(sym->GetAssemblyID() == aid);
        return sym;
    }

    ASSERT(aid >= 0);
    return NULL;
}


/***************************************************************************************************
    Get one of the required predefined AGGSYMs. Optionally calls EnsureState.
***************************************************************************************************/
AGGSYM * COMPILER::GetReqPredefAgg(PREDEFTYPE pt, bool fEnsureState)
{
    AGGSYM * agg = getBSymmgr().GetReqPredefAgg(pt);
    ASSERT(agg);

    if (fEnsureState && agg && !agg->isSource)
        EnsureState(agg);

    return agg;
}


AGGTYPESYM * COMPILER::GetReqPredefType(PREDEFTYPE pt, bool fEnsureState)
{
    AGGSYM * agg = getBSymmgr().GetReqPredefAgg(pt);
    if (!agg) {
        VSFAIL("Required predef type missing");
        return NULL;
    }

    AGGTYPESYM * ats = agg->getThisType();
    if (fEnsureState && !agg->isSource)
        EnsureState(ats);
    return ats;
}


/***************************************************************************************************
    Get one of the optional predefined AGGSYMs. Optionally calls EnsureState if the agg exists.
***************************************************************************************************/
AGGSYM * COMPILER::GetOptPredefAgg(PREDEFTYPE pt, bool fEnsureState)
{
    AGGSYM * agg = getBSymmgr().GetOptPredefAgg(pt);

    if (fEnsureState && agg && !agg->isSource)
        EnsureState(agg);

    return agg;
}


AGGTYPESYM * COMPILER::GetOptPredefType(PREDEFTYPE pt, bool fEnsureState)
{
    AGGSYM * agg = getBSymmgr().GetOptPredefAgg(pt);
    if (!agg)
        return NULL;

    AGGTYPESYM * ats = agg->getThisType();
    if (fEnsureState && !agg->isSource)
        EnsureState(ats);
    return ats;
}


/***************************************************************************************************
    Get one of the optional predefined AGGSYMs. If the agg doesn't exist, generates an error.
    Optionally calls EnsureState if the agg exists.
***************************************************************************************************/
AGGSYM * COMPILER::GetOptPredefAggErr(PREDEFTYPE pt, bool fEnsureState)
{
    ASSERT(pt >= 0 && pt < PT_COUNT);

    AGGSYM * agg = getBSymmgr().GetOptPredefAgg(pt);
    if (!agg) {
        getBSymmgr().ReportMissingPredefTypeError(pt);
        return NULL;
    }

    if (fEnsureState && !agg->isSource)
        EnsureState(agg);
    return agg;
}


AGGTYPESYM * COMPILER::GetOptPredefTypeErr(PREDEFTYPE pt, bool fEnsureState)
{
    AGGSYM * agg = getBSymmgr().GetOptPredefAgg(pt);
    if (!agg) {
        getBSymmgr().ReportMissingPredefTypeError(pt);
        return NULL;
    }

    AGGTYPESYM * ats = agg->getThisType();
    if (fEnsureState && !agg->isSource)
        EnsureState(ats);
    return ats;
}


/*
 * Is this type a particular predefined type?
 */
bool TYPESYM::isPredefType(PREDEFTYPE pt)
{
    if (this == NULL)
        return false;
    if (this->isAGGTYPESYM())
        return this->asAGGTYPESYM()->getAggregate()->iPredef == (unsigned)pt && this->asAGGTYPESYM()->getAggregate()->isPredefined;
    return (this->isVOIDSYM() && pt == PT_VOID);
}

/*
 *  A few types are considered "simple" types for purposes of conversions and so on. They
 *  are the fundemental types the compiler knows about for operators and conversions.
 */
bool TYPESYM::isSimpleType()
{
    return (this->isPredefined() &&
            predefTypeInfo[this->getPredefType()].isSimple);
}

bool TYPESYM::isSimpleOrEnum()
{
    return isSimpleType() || isEnumType();
}

bool TYPESYM::isSimpleOrEnumOrStringOrPtr()
{
    return isSimpleType() || this->isPredefType(PT_STRING) || this->isEnumType() || this->isPTRSYM();
}

bool TYPESYM::isSimpleOrEnumOrStringOrAnyPtr()
{
    return isSimpleType() || this->isPredefType(PT_STRING) || this->isPTRSYM() || this->isPredefType(PT_INTPTR) || this->isPredefType(PT_UINTPTR) || this->isEnumType();
}

bool TYPESYM::isSimpleOrEnumOrString() 
{
    return isSimpleType() || this->isPredefType(PT_STRING) || this->isEnumType();
}

bool TYPESYM::isPointerLike()
{
    return isPTRSYM() || this->isPredefType(PT_INTPTR) || this->isPredefType(PT_UINTPTR);
}

bool TYPESYM::isDelegateType()
{
    return (this->isAGGTYPESYM() && this->getAggregate()->IsDelegate());
}

bool TYPESYM::isInterfaceType()
{
    return (this->isAGGTYPESYM() && this->getAggregate()->IsInterface());
}

bool TYPESYM::isClassType()
{
    return (this->isAGGTYPESYM() && this->getAggregate()->IsClass());
}


bool TYPESYM::isQSimpleType()
{
    return (this->isPredefined() &&
            predefTypeInfo[this->getPredefType()].isQSimple);
}


/* 
 *  A few types are considered "numeric" types. They
 *  are the fundemental number types the compiler knows about for 
 *  operators and conversions.
 */
bool TYPESYM::isNumericType()
{
    return (this->isPredefined() &&
            predefTypeInfo[this->getPredefType()].isNumeric);
}

/* 
 *  byte, ushort, uint, ulong, and enums of the above
 */
bool TYPESYM::isUnsigned()
{
    if (this->isAGGTYPESYM()) {
        AGGTYPESYM *sym = this->asAGGTYPESYM();
        if (sym->isEnumType()) {
            sym = sym->underlyingEnumType();
        }
        if (sym->isPredefined()) {
            PREDEFTYPE pt = sym->getPredefType();
            return pt == PT_UINTPTR || pt == PT_BYTE || (pt >= PT_USHORT && pt <= PT_ULONG);
        } else {
            return false;
        }
    } else {
        return this->isPTRSYM();
    }
}


bool COMPILER::IsCLSAccessible(AGGSYM *context, TYPESYM* type) {
    if (!context || !type->isAGGTYPESYM()) 
        return true;
    AGGSYM *agg = type->getAggregate();
    if (agg->GetAccess() != ACC_PROTECTED && agg->GetAccess() != ACC_INTERNALPROTECTED)
        return true;
    AGGTYPESYM *atsType = type->asAGGTYPESYM();
    AGGTYPESYM *atsOuter = atsType->outerType;
    if (atsOuter->typeArgsAll->size <= 0) 
        return true;
    AGGSYM *aggOuter = atsOuter->getAggregate();
    // does type appear in context's derivation chain?
    while (context != NULL) {
        if (context == aggOuter) {
            return getBSymmgr().SubstEqualTypeArrays(context->typeVarsAll, atsOuter->typeArgsAll, (SubstContext*)NULL);
        }
        else {
            AGGTYPESYM *atsMatch = context->getThisType()->FindBaseType(aggOuter);
            if (atsMatch != NULL)
                return getBSymmgr().SubstEqualTypeArrays(atsMatch->typeArgsAll, atsOuter->typeArgsAll, (SubstContext*)NULL);
        }
        if (!context->parent || !context->parent->isAGGSYM())
            return false;
        context = context->parent->asAGGSYM();
    }
    return false;
}

/*
 * True if a type is not listed as a non-CLS type
 * See spec.
 */
bool COMPILER::isCLS_Type(SYM *context, TYPESYM * type)
{
    if (type->isQSimpleType() || type->isPTRSYM())
        return false;

    if (type->isPredefined())
        return (type->getPredefType() != PT_REFANY && type->getPredefType() != PT_UINTPTR);

    if (type->isARRAYSYM()) {
        // arrays of arrays are CLS compliant
        TYPESYM * elementType = type->asARRAYSYM()->elementType();
        return isCLS_Type(context, elementType);
    }

    if (type->isPARAMMODSYM())
        return isCLS_Type(context, type->asPARAMMODSYM()->paramType());

    if (type->isVOIDSYM())
        return true;

    if (type->isNUBSYM())
        return isCLS_Type(context, type->asNUBSYM()->baseType());

    if (context && context->isAGGSYM() && !IsCLSAccessible(context->asAGGSYM(), type))
        return false;

    if (type->isAGGTYPESYM()) {
        AGGTYPESYM *ats = type->asAGGTYPESYM();
        if (ats->typeArgsThis->size > 0) {
            for (int i = 0, n = ats->typeArgsThis->size; i < n; i++) {
                TYPESYM *arg = ats->typeArgsThis->Item(i);
                if (!this->isCLS_Type(context, arg)) return false;
            }
        }

        return CheckSymForCLS(type->asAGGTYPESYM()->getAggregate(), false);
    }

    return CheckSymForCLS(type, false);
}

// Strips off ARRAYSYM, PARAMMODSYM, PTRSYM, PINNEDSYM and optionally NUBSYM and returns the result.
TYPESYM * TYPESYM::GetNakedType(bool fStripNub)
{
    if (!this)
        return NULL;

    for (TYPESYM * type = this; ; ) {
        switch (type->getKind()) {
        default:
            return type;

        case SK_NUBSYM:
            if (!fStripNub)
                return type;
            // Fall through.
        case SK_ARRAYSYM:
        case SK_PARAMMODSYM:
        case SK_MODOPTTYPESYM:
        case SK_PTRSYM:
        case SK_PINNEDSYM:
            type = type->parent->asTYPESYM();
            break;
        }
    }
}


/*
 * Is this type System.TypedReference or System.ArgIterator?
 * (used for errors becase these types can't go certain places)
 */
bool TYPESYM::isSpecialByRefType()
{
    if (this == NULL)
        return false;
    else if (this->isPredefined())
        return this->getPredefType() == PT_REFANY
            || this->getPredefType() == PT_ARGITERATOR
            || this->getPredefType() == PT_ARGUMENTHANDLE;
    else
        return false;
}

/*
 * true iff this symbol should be CLS compliant based on it's attributes
 * and the attributes of it's declaration scope
 */
bool COMPILER::CheckSymForCLS(SYM * sym, bool FailIfCantBeDeclared)
{
    ASSERT(sym && !sym->isNSSYM());
    if (sym->isERRORSYM() || sym->isVOIDSYM())
        return true;

    if (sym->isNSDECLSYM()) {
        // Since there are no attributes on namespaces
        // skip directly to the 'global' assembly attribute.
        // If there is no assembly level attribute and we are
        // doing CLS checking, assume it should be Compliant
//GET_ASSEMBLY:
        INFILESYM *in = sym->getInputFile();
        return in->hasCLSattribute && in->isCLS;
    }
    
    if (sym->isAGGSYM()) {
        EnsureState(sym->asAGGSYM(), AggState::DefinedMembers);
        if (FailIfCantBeDeclared && !CanAggsymBeDeclared(sym->asAGGSYM()))
            return false;
        EnsureState(sym->asAGGSYM(), AggState::Prepared);
    }
    // If the type can't be declared then we shouldn't have even gotten into CLS checking
    ASSERT (!sym->isAGGSYM() || sym->asAGGSYM()->IsPrepared());

    if (sym->hasCLSattribute)
        return sym->isCLS;

    // For AGGSYM, just use the first declaration of the class, since all must be equivalent.
    PARENTSYM * temp;
    if (sym->isAGGSYM())
        temp = sym->asAGGSYM()->DeclFirst()->DeclPar();
    else if (sym->isAGGDECLSYM())
        temp = sym->asAGGDECLSYM()->Agg();
    else 
        temp = sym->parent;

    return CheckSymForCLS(temp, FailIfCantBeDeclared);
}


// Using the same logic as ForceAggStates, determine if
// declaring the type could be expected to be something that
// could succeed. This returns false only if a type was referenced
// by metadata, but wasn't defined by metadata or source.
bool COMPILER::CanAggsymBeDeclared(PAGGSYM sym)
{
    if (sym->IsPrepared())
        return true;
    if (sym->hasParseTree)
        return true;
    if (sym->isSource)
        return true;
    if (TypeFromToken(sym->tokenImport) == mdtTypeDef)
        return true;
        
    return false;        
}

void COMPILER::UndeclarableType(UNRESAGGSYM * ura)
{
    if (!ura->fSuppressError) {
        // type was imported, but we have no definition for it

        LOCATION * location = this->location;

        //
        // find last spot which wasn't imported metadata
        //
        BASENODE *node = NULL;
        INFILESYM * file = NULL;
        if (this->location) {
            while (location && location->getFile() && !location->getFile()->isSource) {
                location = location->getPrevious();
            }

            if (location) {
                node = location->getNode();
                file = location->getFile();
            }
        }

        // Get the assembly this would be found in.
        WCHAR assemblyName[MAX_FULLNAME_SIZE];
        assemblyName[0] = '\0';
        importer.GetTypeRefAssemblyName(ura->moduleErr, ura->tokErr, assemblyName, lengthof(assemblyName));

        //
        // try and generate a good error message for it
        //
        CError  *pError = MakeError(node && file ? node : NULL, ERR_NoTypeDef, ErrArgRef(ura), assemblyName);

        //
        // dump all intervening import symbols on the location stack
        // to show the dependency chain which caused the importing of the unknown symbol
        //
        LOCATION *currentLocation = this->location;
        SYM *currentSymbol = NULL;
        while (currentLocation != location) {
            SYM *topSymbol = currentLocation->getSymbol();
            if (topSymbol && topSymbol != currentSymbol) {
                currentSymbol = topSymbol;
                if (currentSymbol != ura) {
                    AddRelatedSymLoc(pError, topSymbol);
                }
            }
            currentLocation = currentLocation->getPrevious();
        }

        // Submit the error
        SubmitError (pError);
    }

    //
    // fake up the type so that it looks reasonable
    // for the rest of the compile
    //
    ASSERT(ura->AggState() == AggState::None);
    ura->SetAggState(AggState::PreparedMembers);
    ura->SetAggKind(AggKind::Class);
    if (!ura->isPredefAgg(PT_OBJECT)) {
        ASSERT(!ura->baseClass && !ura->ifaces && !ura->ifacesAll);
        SetBaseType(ura, GetReqPredefType(PT_OBJECT, false));
        SetIfaces(ura, NULL, 0);
    }
}



/***************************************************************************************************
    This attempts to bring the type up to at least the indicated agg state. What this means depends
    on the kind of type we're dealing with:

      * For pointers, arrays, etc where there is a single constituent, the state of the type is the
        state of the constituent.
      * For an AGGTYPESYM, the state of the type is the state of the AGGSYM. This does not check the
        states of type arguments! This avoid infinite recursion when a type variable is used as a
        type argument in one of its bounds. It also fulfills the general rule that EnsureState
        propogates to base types but not type arguments.
      * For a TYVARSYM, the state of the type is the min of the state of each AGGTYPESYM used as
        a bound.

    Type variables and aggs depend on base types.
    AGGSYMs and TypeArrays are handled by overloads of ForceAggStates.
***************************************************************************************************/
void COMPILER::ForceAggStates(TYPESYM * type, AggStateEnum aggStateMin)
{
    ASSERT(FBelow(type, aggStateMin));
    ASSERT(aggStateMin <= this->aggStateMax);

    switch (type->getKind()) {
    default:
        break;

    case SK_PTRSYM:
    case SK_PARAMMODSYM:
    case SK_MODOPTTYPESYM:
    case SK_PINNEDSYM:
    case SK_ARRAYSYM:
        EnsureState(type->parent->asTYPESYM(), aggStateMin);
        break;

    case SK_NUBSYM:
        {
            AGGTYPESYM * ats = type->asNUBSYM()->GetAts();
            if (ats)
                EnsureState(ats, aggStateMin);
        }
        break;

    case SK_AGGTYPESYM:
        EnsureState(type->asAGGTYPESYM()->getAggregate(), aggStateMin);
        break;

    case SK_ERRORSYM:
        if (type->parent->isTYPESYM())
            EnsureState(type->parent->asTYPESYM(), aggStateMin);
        break;

    case SK_TYVARSYM:
        // NOTE: This may change var->bnds!
        if (type->parent) {
            ASSERT(!type->parent || type->parent->isAGGSYM() || type->parent->isMETHSYM());
            EnsureState(
                type->parent->isAGGSYM() ? type->parent->asAGGSYM() : type->parent->asMETHSYM()->getClass(),
                aggStateMin);
        }

        if (type->asTYVARSYM()->GetBnds())
            EnsureState(type->asTYVARSYM()->GetBnds(), aggStateMin);
        break;
    }

    ComputeAggState(type);
    ASSERT(type->AggState() >= aggStateMin);
}


void COMPILER::ForceAggStates(AGGSYM * agg, AggStateEnum aggStateMin)
{
    ASSERT(FBelow(agg, aggStateMin));
    ASSERT(aggStateMin <= this->aggStateMax);
    if (agg->isSource) {
        ASSERT(!"Shouldn't be forcing a source agg state!");
        return;
    }

    if (agg->IsUnresolved()) {
        UndeclarableType(agg->AsUnresolved());
        return;
    }


    // These all take care of base types and interfaces as well.
    if (aggStateMin <= AggState::Inheritance) {
        // They're asking for at most inheritance.
        VSVERIFY(clsDeclRec.ResolveInheritanceRec(agg), "ResolveInheritanceRec failed ForceAggStates!");
    }
    else if (aggStateMin <= AggState::Bounds) {
        // They're asking for at most bounds.
        VSVERIFY(clsDeclRec.ResolveInheritanceRec(agg), "ResolveInheritanceRec failed ForceAggStates!");
        importer.DefineBounds(agg);
    }
    else if (aggStateMin <= AggState::DefinedMembers) {
        // They're asking for at most members.
        // Note that this takes care of inheritance.
        importer.DefineImportedType(agg);
    }
    else {
        // Anything beyond members means prepare.
        // Note that this takes care of inheritance and members.
        prepareAggregate(agg);
    }

}


/***************************************************************************************************
    Just call EnsureState on all of the types.
***************************************************************************************************/
void COMPILER::ForceAggStates(TypeArray * ta, AggStateEnum aggStateMin)
{
    ASSERT(ta->size > 0);
    ASSERT(FBelow(ta, aggStateMin));

    for (int i = 0; i < ta->size; i++)
        EnsureState(ta->Item(i), aggStateMin);

    ComputeAggState(ta);
}




void COMPILER::ComputeAggState(TYPESYM * type)
{
    TYPESYM * typeSrc;

    switch (type->getKind()) {
    case SK_NULLSYM:
    case SK_VOIDSYM:
    case SK_UNITSYM:
    case SK_ANONMETHSYM:
    case SK_METHGRPSYM:
        type->SetAggState(AggState::Last);
        break;

    case SK_PTRSYM:
    case SK_PARAMMODSYM:
    case SK_MODOPTTYPESYM:
    case SK_PINNEDSYM:
    case SK_ARRAYSYM:
        typeSrc = type->parent->asTYPESYM();
        type->SetAggState(typeSrc->AggState());
        break;

    case SK_NUBSYM:
        {
            AGGTYPESYM * ats = type->asNUBSYM()->GetAts();
            if (ats) {
                type->SetAggState(ats->AggState());
            }
            else {
                type->SetAggState(AggState::Last);
            }
        }
        break;

    case SK_AGGTYPESYM:
        {
            AGGTYPESYM * ats = type->asAGGTYPESYM();
            AGGSYM * agg = ats->getAggregate();


            if (agg->AggState() > ats->AggState()) {
                ats->fConstraintsChecked = false;
                ats->fConstraintError = false;
            }
            ats->SetAggState(agg->AggState());
        }
        break;

    case SK_ERRORSYM:
        if (!type->parent) {
            type->SetAggState(AggState::Last);
        }
        else {
            ERRORSYM * err = type->asERRORSYM();
            ASSERT(err->parent && err->nameText && err->typeArgs);

            if (err->parent->isTYPESYM()) {
                err->SetAggState(err->parent->asTYPESYM()->AggState());
            }
            else {
                err->SetAggState(AggState::Last);
            }
        }
        break;

    case SK_TYVARSYM:
        if (!type->parent) {
            // This should be a standard TYVARSYM used for emitting.
            ASSERT(type->asTYVARSYM()->parseTree == NULL);
            type->SetAggState(AggState::Last);
        }
        else {
            ASSERT(type->parent->isAGGSYM() || type->parent->isMETHSYM());
            TYVARSYM * var = type->asTYVARSYM();
            TypeArray * bnds = var->GetBnds(); // May be NULL.

            var->SetAggState(AggState::Last);


            if (!bnds) {
                ASSERT(var->parent->isAGGSYM() && var->parent->asAGGSYM()->AggState() < AggState::Bounds);
                if (var->AggState() > AggState::Declared)
                    var->SetAggState(AggState::Declared);
            }
            else if (var->AggState() > bnds->AggState())
                var->SetAggState(bnds->AggState());
        }
        break;

    default:
        ASSERT(!"unknown type");
        break;
    }
}


void COMPILER::ComputeAggState(TypeArray * ta)
{
    ta->SetAggState(AggState::Last);

    for (int i = 0; i < ta->size; i++) {
        TYPESYM * type = ta->Item(i);
        if (ta->AggState() > type->AggState())
            ta->SetAggState(type->AggState());
    }
}


/***************************************************************************************************
    Lookup in the given parent. If symPar is an NSSYM or AGGSYM and mask includes MASK_AGGSYM, this
    makes sure all relevant types are loaded.
***************************************************************************************************/
SYM * COMPILER::LookupGlobalSym(NAME * name, PARENTSYM * symPar, symbmask_t mask)
{
    if (symPar->isNSSYM() && symPar->asNSSYM()->AnyTypesUnloaded() && (mask & MASK_AGGSYM) &&
        this->compPhase > CompilerPhase::ImportTypes)
    {
        // Load the imported types in this namespace (all assemblies).
        importer.LoadTypesInNsAid(symPar->asNSSYM(), kaidNil, NULL);
    }

    return getBSymmgr().LookupGlobalSymCore(name, symPar, mask);
}


/***************************************************************************************************
    Lookup in the given bag, restricting to the given aid. Aid should be an assembly id or extern
    alias id - NOT a module id.
***************************************************************************************************/
SYM * COMPILER::LookupInBagAid(NAME * name, BAGSYM * bag, int aid, symbmask_t mask)
{
    ASSERT(aid < kaidMinModule);

    if (!bag->InAlias(this, aid))
        return NULL;

    if (bag->isNSSYM() && bag->asNSSYM()->AnyTypesUnloaded() && (mask & MASK_AGGSYM) &&
        this->compPhase > CompilerPhase::ImportTypes)
    {
        EnsureTypesInNsAid(bag->asNSSYM(), aid);
    }

    for (SYM * sym = getBSymmgr().LookupGlobalSymCore(name, bag, mask);
        sym;
        sym = sym->nextSameName)
    {
        if (!(sym->mask() & mask))
            continue;
        switch (sym->getKind()) {
        case SK_AGGSYM:
            if (sym->asAGGSYM()->InAlias(aid))
                return sym;
            break;
        case SK_NSSYM:
            if (sym->asNSSYM()->InAlias(this, aid))
                return sym;
            break;
        case SK_FWDAGGSYM:
            if (sym->asFWDAGGSYM()->InAlias(aid))
                return sym;
            break;
        default:
            return sym;
        }
    }

    return NULL;
}


/***************************************************************************************************
    Lookup the next symbol in the given aid.
***************************************************************************************************/
SYM * COMPILER::LookupNextInAid(SYM * sym, int aid, symbmask_t mask)
{
    ASSERT(aid < kaidMinModule);
    ASSERT(sym);

    if (!sym)
        return NULL;
    while ((sym = sym->nextSameName) != NULL) {
        if (!(sym->mask() & mask))
            continue;
        switch (sym->getKind()) {
        case SK_AGGSYM:
            if (sym->asAGGSYM()->InAlias(aid))
                return sym;
            break;
        case SK_NSSYM:
            if (sym->asNSSYM()->InAlias(this, aid))
                return sym;
            break;
        case SK_FWDAGGSYM:
            if (sym->asFWDAGGSYM()->InAlias(aid))
                return sym;
            break;
        default:
            return sym;
        }
    }
    return NULL;
}


/***************************************************************************************************
    Make sure all the types in the given (ns, aid) pair are loaded. Aid should be an assembly id
    or extern alias id, NOT a module id.
***************************************************************************************************/
void COMPILER::EnsureTypesInNsAid(NSSYM * ns, int aid)
{
    ASSERT(aid < kaidMinModule);

    SYM * symAid;
    INFILESYM * infile = NULL;

    switch (aid) {
    case kaidThisAssembly:
        if (!ns->TypesUnloaded(aid))
            return;
        break;
    case kaidGlobal:
        if (!ns->AnyTypesUnloaded(getBSymmgr().bsetGlobalAssemblies))
            return;
        break;
    default:
        symAid = getBSymmgr().GetSymForAid(aid);
        if (!symAid)
            return;
        if (symAid->isINFILESYM()) {
            ASSERT(symAid->asINFILESYM()->GetAssemblyID() == aid);
            if (!ns->TypesUnloaded(aid))
                return;
            infile = symAid->asINFILESYM();
        }
        else if (symAid->isEXTERNALIASSYM()) {
            // Enumerate assemblies and check for being in aid.
            if (!ns->AnyTypesUnloaded(symAid->asEXTERNALIASSYM()->bsetAssemblies))
                return;
        }
        break;
    }

    importer.LoadTypesInNsAid(ns, aid, infile);
}


/*---------------------------------------------------------------------------------------
    Get the standard type variable (eg, !0, !1, or !!0, !!1).

        iv is the index.
        pbsm is the containing symbol manager
        fMeth designates whether this is a method type var or class type var

    The standard class type variables are useful during emit, but not for type comparison
    when binding. The standard method type variables are useful during binding for
    signature comparison.
---------------------------------------------------------------------------------------*/
PTYVARSYM BSYMMGR::StdTypeVarColl::GetTypeVarSym(int iv, BSYMMGR * pbsm, bool fMeth)
{
    ASSERT(iv >= 0 && iv < 0x0001000);
    if (iv >= ctvs) {
        // Grow the list.
        int ctvsNew = ctvs * 2;
        if (ctvsNew < 8)
            ctvsNew = 8;
        if (ctvsNew <= iv)
            ctvsNew = iv + 1;

        // Allocate the new table and copy over the old values.
        PTYVARSYM * prgptvsNew = (PTYVARSYM *)pbsm->getAlloc()->AllocZero(ctvsNew * sizeof(PTYVARSYM));
        if (ctvs)
            memcpy(prgptvsNew, prgptvs, sizeof(PTYVARSYM *) * ctvs);
        prgptvs = prgptvsNew;
        ctvs = ctvsNew;
    }

    PTYVARSYM * pptvs = prgptvs + iv;
    if (!*pptvs) {
        // Allocate the new TYVARSYM.
        *pptvs = pbsm->CreateGlobalSym(SK_TYVARSYM, NULL, NULL)->asTYVARSYM();
        PTYVARSYM ptvs = *pptvs;
        ptvs->isMethTyVar = fMeth;
        ptvs->SetAccess(ACC_PRIVATE);
        ptvs->index = (short)iv;
        ptvs->indexTotal = (short)iv;
    }
    ASSERT(*pptvs);

    return *pptvs;
}

//
// Substitute an instantiation through a type, generating a new type.
//
// Either or both of ppClassTypeArgs and ppMethTypeArgs can be NULL or empty, which indicates that we want the
// identity substitution for that collection of type variables, e.g. T -> T, U -> U.
//
// If non-null, the arrays must be of the appropriate size, and specify the substitution for that range of type parameters.
//
TYPESYM * BSYMMGR::SubstType(TYPESYM *typeSrc, TypeArray *typeArgsCls, TypeArray *typeArgsMeth, SubstTypeFlagsEnum grfst)
{
    if (!typeSrc)
        return NULL;

    SubstContext ctx(typeArgsCls, typeArgsMeth, grfst);

    return ctx.FNop() ? typeSrc : SubstTypeCore(typeSrc, &ctx);
}


TypeArray * BSYMMGR::SubstTypeArray(TypeArray *taSrc, SubstContext * pctx)
{
    if (TypeArray::Size(taSrc) == 0 || !pctx || pctx->FNop())
        return taSrc;

    TYPESYM ** prgpts = STACK_ALLOC(TYPESYM *, taSrc->size);
    for (int ipts = 0; ipts < taSrc->size; ipts++) { 
        prgpts[ipts] = SubstTypeCore(taSrc->Item(ipts), pctx);
    }
    return AllocParams(taSrc->size, prgpts);
}


TypeArray * BSYMMGR::SubstTypeArray(TypeArray *taSrc, TypeArray *typeArgsCls, TypeArray *typeArgsMeth, SubstTypeFlagsEnum grfst)
{
    if (TypeArray::Size(taSrc) == 0)
        return taSrc;

    SubstContext ctx(typeArgsCls, typeArgsMeth, grfst);

    if (ctx.FNop())
        return taSrc;

    TYPESYM ** prgpts = STACK_ALLOC(PTYPESYM, taSrc->size);
    for (int ipts = 0; ipts < taSrc->size; ipts++) { 
        prgpts[ipts] = SubstTypeCore(taSrc->Item(ipts), &ctx);
    }
    return AllocParams(taSrc->size, prgpts);
}


TYPESYM * BSYMMGR::SubstTypeCore(TYPESYM * type, SubstContext * pctx)
{
    TYPESYM *typeSrc;
    TYPESYM *typeDst;

    switch (type->getKind()) {
    default:
        ASSERT(0);
        // fall through by design...

    case SK_NULLSYM:
    case SK_VOIDSYM: 
    case SK_UNITSYM:
    case SK_METHGRPSYM:
    case SK_ANONMETHSYM:
        return type;

    case SK_PARAMMODSYM:
        typeDst = SubstTypeCore(typeSrc = type->asPARAMMODSYM()->paramType(), pctx);
        return (typeDst == typeSrc) ? type : GetParamModifier(typeDst, type->asPARAMMODSYM()->isOut);

    case SK_MODOPTTYPESYM:
        typeDst = SubstTypeCore(typeSrc = type->asMODOPTTYPESYM()->baseType(), pctx);
        return (typeDst == typeSrc) ? type : GetModOptType(typeDst, type->asMODOPTTYPESYM()->opt);

    case SK_ARRAYSYM:
        typeDst = SubstTypeCore(typeSrc = type->asARRAYSYM()->elementType(), pctx);
        return (typeDst == typeSrc) ? type : GetArray(typeDst, type->asARRAYSYM()->rank);

    case SK_PTRSYM:
        typeDst = SubstTypeCore(typeSrc = type->asPTRSYM()->baseType(), pctx);
        return (typeDst == typeSrc) ? type : GetPtrType(typeDst);

    case SK_NUBSYM:
        typeDst = SubstTypeCore(typeSrc = type->asNUBSYM()->baseType(), pctx);
        return (typeDst == typeSrc) ? type : GetNubType(typeDst);

    case SK_PINNEDSYM:
        typeDst = SubstTypeCore(typeSrc = type->asPINNEDSYM()->baseType(), pctx);
        return (typeDst == typeSrc) ? type : GetPinnedType(typeDst);

    case SK_AGGTYPESYM:
        if (type->asAGGTYPESYM()->typeArgsAll->size > 0) {
            AGGTYPESYM * ats = type->asAGGTYPESYM();

            TypeArray * typeArgs = SubstTypeArray(ats->typeArgsAll, pctx);
            if (ats->typeArgsAll != typeArgs)
                return GetInstAgg(ats->getAggregate(), typeArgs);
        }
        return type;

    case SK_ERRORSYM:
        if (type->parent) {
            ERRORSYM * err = type->asERRORSYM();
            ASSERT(err->parent && err->nameText && err->typeArgs);

            PARENTSYM * symPar = err->parent;
            if (symPar->isTYPESYM())
                symPar = SubstTypeCore(symPar->asTYPESYM(), pctx);
            TypeArray * typeArgs = SubstTypeArray(err->typeArgs, pctx);
            if (typeArgs != err->typeArgs || symPar != err->parent)
                return GetErrorType(symPar, err->nameText, typeArgs);
        }
        return type;

    case SK_TYVARSYM:
        {
            TYVARSYM *tvs = type->asTYVARSYM();
            int index = tvs->indexTotal;
            if (tvs->isMethTyVar) {
                if ((pctx->grfst & SubstTypeFlags::DenormMeth) && tvs->parent)
                    return type;
                ASSERT(tvs->index == tvs->indexTotal);
                ASSERT(!pctx->prgtypeMeth || index < pctx->ctypeMeth);
                return index < pctx->ctypeMeth ? pctx->prgtypeMeth[index] :
                    ((pctx->grfst & SubstTypeFlags::NormMeth) ? GetStdMethTypeVar(index) : type);
            }
            if ((pctx->grfst & SubstTypeFlags::DenormClass) && tvs->parent)
                return type;
            return index < pctx->ctypeCls ? pctx->prgtypeCls[index] :
                ((pctx->grfst & SubstTypeFlags::NormClass) ? GetStdClsTypeVar(index) : type);
        }
    }
}


bool BSYMMGR::SubstEqualTypes(TYPESYM *typeDst, TYPESYM *typeSrc, SubstContext * pctx)
{
    if (typeDst == typeSrc) {
        ASSERT(typeDst == SubstType(typeSrc, pctx));
        return true;
    }

    return pctx && !pctx->FNop() && SubstEqualTypesCore(typeDst, typeSrc, pctx);
}


bool BSYMMGR::SubstEqualTypes(TYPESYM *typeDst, TYPESYM *typeSrc, TypeArray *typeArgsCls, TypeArray *typeArgsMeth, SubstTypeFlagsEnum grfst)
{
    if (typeDst == typeSrc) {
        ASSERT(typeDst == SubstType(typeSrc, typeArgsCls, typeArgsMeth, grfst));
        return true;
    }

    SubstContext ctx(typeArgsCls, typeArgsMeth, grfst);

    return !ctx.FNop() && SubstEqualTypesCore(typeDst, typeSrc, &ctx);
}


// Test equality of two type arrays after substituting in the second.
bool BSYMMGR::SubstEqualTypeArrays(TypeArray *taDst, TypeArray *taSrc, SubstContext * pctx)
{
    // Handle the simple common cases first.
    if (taDst == taSrc) {
        ASSERT(taDst == SubstTypeArray(taSrc, pctx));
        return true;
    }
    if (TypeArray::Size(taDst) != TypeArray::Size(taSrc))
        return false;
    if (!TypeArray::Size(taDst))
        return true;

    if (!pctx || pctx->FNop())
        return false;

    for (int i = 0; i < taDst->size; i++) {
        if (!SubstEqualTypesCore(taDst->Item(i), taSrc->Item(i), pctx))
            return false;
    }

    return true;
}


bool BSYMMGR::SubstEqualTypeArrays(TypeArray *taDst, TypeArray *taSrc, TypeArray *typeArgsCls, TypeArray *typeArgsMeth, SubstTypeFlagsEnum grfst)
{
    // Handle the simple common cases first.
    if (taDst == taSrc) {
        ASSERT(taDst == SubstTypeArray(taSrc, typeArgsCls, typeArgsMeth, grfst));
        return true;
    }
    if (TypeArray::Size(taDst) != TypeArray::Size(taSrc))
        return false;
    if (!TypeArray::Size(taDst))
        return true;

    SubstContext ctx(typeArgsCls, typeArgsMeth, grfst);

    if (ctx.FNop())
        return false;

    for (int i = 0; i < taDst->size; i++) {
        if (!SubstEqualTypesCore(taDst->Item(i), taSrc->Item(i), &ctx))
            return false;
    }

    return true;
}


bool BSYMMGR::SubstEqualTypesCore(TYPESYM *typeDst, TYPESYM *typeSrc, SubstContext *pctx)
{
LRecurse: // Label used for "tail" recursion.

    if (typeDst == typeSrc) {
        ASSERT(typeDst == SubstTypeCore(typeSrc, pctx));
        return true;
    }

    switch (typeSrc->getKind()) {
    default:
        ASSERT(!"Bad SYM kind in SubstEqualTypesCore");
        return false;

    case SK_NULLSYM:
    case SK_VOIDSYM:
    case SK_UNITSYM:
        // There should only be a single instance of these.
        ASSERT(typeDst->getKind() != typeSrc->getKind());
        return false;

    case SK_ARRAYSYM:
        if (typeDst->getKind() != SK_ARRAYSYM || typeDst->asARRAYSYM()->rank != typeSrc->asARRAYSYM()->rank)
            return false;
        goto LCheckBases;

    case SK_PARAMMODSYM:
        if (typeDst->getKind() != SK_PARAMMODSYM || typeDst->asPARAMMODSYM()->isOut != typeSrc->asPARAMMODSYM()->isOut)
            return false;
        goto LCheckBases;

    case SK_MODOPTTYPESYM:
        // NOTE: This is not 100% correct because the same modopt in two different imports is represented by
        // two different MODOPTSYMs. This shouldn't affect anything. In fact I doubt this code will ever be executed.
        if (typeDst->getKind() != SK_MODOPTTYPESYM || typeDst->asMODOPTTYPESYM()->opt != typeSrc->asMODOPTTYPESYM()->opt)
            return false;
        goto LCheckBases;

    case SK_PTRSYM:
    case SK_NUBSYM:
        if (typeDst->getKind() != typeSrc->getKind())
            return false;
LCheckBases:
        typeSrc = typeSrc->parent->asTYPESYM();
        typeDst = typeDst->parent->asTYPESYM();
        goto LRecurse;

    case SK_AGGTYPESYM:
        if (typeDst->getKind() != SK_AGGTYPESYM)
            return false;

        { // BLOCK
            AGGTYPESYM *atsSrc = typeSrc->asAGGTYPESYM();
            AGGTYPESYM *atsDst = typeDst->asAGGTYPESYM();

            if (atsSrc->getAggregate() != atsDst->getAggregate())
                return false;

            ASSERT(atsSrc->typeArgsAll->size == atsDst->typeArgsAll->size);

            // All the args must unify.
            for (int i = 0; i < atsSrc->typeArgsAll->size; i++) {
                if (!SubstEqualTypesCore(atsDst->typeArgsAll->Item(i), atsSrc->typeArgsAll->Item(i), pctx))
                    return false;
            }
        }
        return true;

    case SK_ERRORSYM:
        if (!typeDst->isERRORSYM() || !typeSrc->parent || !typeDst->parent)
            return false;

        {
            ERRORSYM * errSrc = typeSrc->asERRORSYM();
            ERRORSYM * errDst = typeDst->asERRORSYM();
            ASSERT(errSrc->parent && errSrc->nameText && errSrc->typeArgs);
            ASSERT(errDst->parent && errDst->nameText && errDst->typeArgs);

            if (errSrc->nameText != errDst->nameText || errSrc->typeArgs->size != errDst->typeArgs->size)
                return false;

            if (errSrc->parent != errDst->parent) {
                if (!errSrc->parent->isTYPESYM() != !errDst->parent->isTYPESYM())
                    return false;
                if (errSrc->parent->isTYPESYM() && !SubstEqualTypesCore(errDst->parent->asTYPESYM(), errSrc->parent->asTYPESYM(), pctx))
                    return false;
            }

            // All the args must unify.
            for (int i = 0; i < errSrc->typeArgs->size; i++) {
                if (!SubstEqualTypesCore(errDst->typeArgs->Item(i), errSrc->typeArgs->Item(i), pctx))
                    return false;
            }
        }
        return true;

    case SK_TYVARSYM:
        { // BLOCK
            TYVARSYM *tvs = typeSrc->asTYVARSYM();
            int index = tvs->indexTotal;

            if (tvs->isMethTyVar) {
                if ((pctx->grfst & SubstTypeFlags::DenormMeth) && tvs->parent) {
                    // typeDst == typeSrc was handled above.
                    ASSERT(typeDst != typeSrc);
                    return false;
                }
                ASSERT(tvs->index == tvs->indexTotal);
                ASSERT(!pctx->prgtypeMeth || tvs->indexTotal < pctx->ctypeMeth);
                if (index < pctx->ctypeMeth)
                    return typeDst == pctx->prgtypeMeth[index];
                if (pctx->grfst & SubstTypeFlags::NormMeth)
                    return typeDst == GetStdMethTypeVar(index);
            }
            else {
                if ((pctx->grfst & SubstTypeFlags::DenormClass) && tvs->parent) {
                    // typeDst == typeSrc was handled above.
                    ASSERT(typeDst != typeSrc);
                    return false;
                }
                ASSERT(!pctx->prgtypeCls || tvs->indexTotal < pctx->ctypeCls);
                if (index < pctx->ctypeCls)
                    return typeDst == pctx->prgtypeCls[index];
                if (pctx->grfst & SubstTypeFlags::NormClass)
                    return typeDst == GetStdClsTypeVar(index);
            }
        }
        return false;
    }
}


// Search within type for instances of typeFind and replace them with typeReplace.
PTYPESYM BSYMMGR::SubstTypeSingle(TYPESYM *type, TYPESYM *typeFind, TYPESYM *typeReplace)
{
    if (type == typeFind)
        return typeReplace;

    TYPESYM *typeSrc;
    TYPESYM *typeDst;

    switch (type->getKind()) {
    default:
        ASSERT(0);
        return NULL;

    case SK_NULLSYM:
    case SK_VOIDSYM: 
    case SK_UNITSYM:
        return type;

    case SK_PARAMMODSYM:
        typeDst = SubstTypeSingle(typeSrc = type->asPARAMMODSYM()->paramType(), typeFind, typeReplace);
        return (typeDst == typeSrc) ? type : GetParamModifier(typeDst, type->asPARAMMODSYM()->isOut);

    case SK_MODOPTTYPESYM:
        typeDst = SubstTypeSingle(typeSrc = type->asMODOPTTYPESYM()->baseType(), typeFind, typeReplace);
        return (typeDst == typeSrc) ? type : GetModOptType(typeDst, type->asMODOPTTYPESYM()->opt);

    case SK_ARRAYSYM:
        typeDst = SubstTypeSingle(typeSrc = type->asARRAYSYM()->elementType(), typeFind, typeReplace);
        return (typeDst == typeSrc) ? type : GetArray(typeDst, type->asARRAYSYM()->rank);

    case SK_PTRSYM:
        typeDst = SubstTypeSingle(typeSrc = type->asPTRSYM()->baseType(), typeFind, typeReplace);
        return (typeDst == typeSrc) ? type : GetPtrType(typeDst);

    case SK_NUBSYM:
        typeDst = SubstTypeSingle(typeSrc = type->asNUBSYM()->baseType(), typeFind, typeReplace);
        return (typeDst == typeSrc) ? type : GetNubType(typeDst);

    case SK_PINNEDSYM:
        typeDst = SubstTypeSingle(typeSrc = type->asPINNEDSYM()->baseType(), typeFind, typeReplace);
        return (typeDst == typeSrc) ? type : GetPinnedType(typeDst);

    case SK_AGGTYPESYM:
        if (type->asAGGTYPESYM()->typeArgsAll->size > 0) {
            AGGTYPESYM * ats = type->asAGGTYPESYM();

            TYPESYM ** prgtype = STACK_ALLOC(TYPESYM *, ats->typeArgsAll->size);
            for (int i = 0; i < ats->typeArgsAll->size; i++)
                prgtype[i] = SubstTypeSingle(ats->typeArgsAll->Item(i), typeFind, typeReplace);
            TypeArray * typeArgs = AllocParams(ats->typeArgsAll->size, prgtype);
            if (typeArgs != ats->typeArgsAll)
                return GetInstAgg(ats->getAggregate(), typeArgs);
        }
        return type;

    case SK_ERRORSYM:
        if (type->parent) {
            ERRORSYM * err = type->asERRORSYM();
            ASSERT(err->parent && err->nameText && err->typeArgs);

            PARENTSYM * symPar = err->parent;
            if (symPar->isTYPESYM())
                symPar = SubstTypeSingle(symPar->asTYPESYM(), typeFind, typeReplace);

            TypeArray * typeArgs = err->typeArgs;
            if (typeArgs->size > 0) {
                TYPESYM ** prgtype = STACK_ALLOC(TYPESYM *, typeArgs->size);
                for (int i = 0; i < typeArgs->size; i++)
                    prgtype[i] = SubstTypeSingle(typeArgs->Item(i), typeFind, typeReplace);
                typeArgs = AllocParams(typeArgs->size, prgtype);
            }
            if (typeArgs != err->typeArgs || symPar != err->parent)
                return GetErrorType(symPar, err->nameText, typeArgs);
        }
        return type;

    case SK_TYVARSYM:
        return type;
    }
}


// Find the unification mapping slot for the given type variable.
// Returns NULL if the type variable isn't a known one.
TYPESYM ** BSYMMGR::UnifyContext::GetSlot(TYVARSYM *tvs)
{
    if (tvs->isMethTyVar) {
        if (tvs->indexTotal >= typeVarsMeth->size ||
            tvs != typeVarsMeth->Item(tvs->indexTotal))
        {
            return NULL;
        }
        return prgtypeMeth + tvs->indexTotal;
    }

    if (tvs->indexTotal >= typeVarsCls->size ||
        tvs != typeVarsCls->Item(tvs->indexTotal))
    {
        return NULL;
    }
    return prgtypeCls + tvs->indexTotal;
}


bool BSYMMGR::UnifyTypes(TYPESYM *t1, TYPESYM *t2, UnifyContext *pctx)
{
    // First substitute using the current unifications.
    t1 = SubstType(t1, pctx);
    t2 = SubstType(t2, pctx);

// Note: skipping the SubstType calls when we goto LRecurse is an optimization.
// They're not needed because we never modify the mapping before jumping here.
LRecurse: // Label used for "tail" recursion.

#ifdef DEBUG
    // t1 and t2 should not contain any type variables that are already mapped.
    for (int i = 0; i < pctx->typeVarsCls->size; i++) {
        TYVARSYM * var = pctx->typeVarsCls->ItemAsTYVARSYM(i);
        if (pctx->prgtypeCls[i] != var) {
            ASSERT(!TypeContainsType(t1, var));
            ASSERT(!TypeContainsType(t2, var));
        }
    }
    for (int i = 0; i < pctx->typeVarsMeth->size; i++) {
        TYVARSYM * var = pctx->typeVarsMeth->ItemAsTYVARSYM(i);
        if (pctx->prgtypeMeth[i] != var) {
            ASSERT(!TypeContainsType(t1, var));
            ASSERT(!TypeContainsType(t2, var));
        }
    }
#endif // DEBUG

    if (t1 == t2)
        return true;

    if (t2->isTYVARSYM()) {
        // Swap them.
        TYPESYM *t = t2;
        t2 = t1;
        t1 = t;
    }

    // The only way t2 is a type variable is if both are type variables.
    ASSERT(!t2->isTYVARSYM() || t1->isTYVARSYM());

    switch (t1->getKind()) {
    default:
        ASSERT(!"Bad SYM kind in UnifyTypes");
        return false;

    case SK_NULLSYM:
    case SK_VOIDSYM:
        // There should only be a single instance of these.
        ASSERT(t2->getKind() != t1->getKind());
        return false;

    case SK_PARAMMODSYM:
        // If byref doesn't match up, we don't unify. Note that unification does
        // not distinguish between out and ref (since the CLR can't) so we don't
        // require that t1->asPARAMMODSYM()->isOut == t2->asPARAMMODSYM()->isOut.
    case SK_NUBSYM:
    case SK_PTRSYM:
    case SK_ARRAYSYM:
        if (t2->getKind() != t1->getKind() ||
            t1->isARRAYSYM() && t2->asARRAYSYM()->rank != t1->asARRAYSYM()->rank)
        {
            return false;
        }
        t1 = t1->parent->asTYPESYM();
        t2 = t2->parent->asTYPESYM();
        goto LRecurse;

    case SK_AGGTYPESYM:
        if (t2->getKind() != SK_AGGTYPESYM)
            return false;

        { // BLOCK
            AGGTYPESYM *ats1 = t1->asAGGTYPESYM();
            AGGTYPESYM *ats2 = t2->asAGGTYPESYM();

            if (ats1->getAggregate() != ats2->getAggregate())
                return false;

            ASSERT(ats1->typeArgsAll->size == ats2->typeArgsAll->size);

            // All the args must unify.
            for (int i = 0; i < ats1->typeArgsAll->size; i++) {
                if (!UnifyTypes(ats1->typeArgsAll->Item(i), ats2->typeArgsAll->Item(i), pctx))
                    return false;
            }
        }
        return true;

    case SK_ERRORSYM:
        if (!t2->isERRORSYM() || !t1->parent || !t2->parent)
            return false;

        {
            ERRORSYM * err1 = t1->asERRORSYM();
            ERRORSYM * err2 = t2->asERRORSYM();
            ASSERT(err1->parent && err1->nameText && err1->typeArgs);
            ASSERT(err2->parent && err2->nameText && err2->typeArgs);

            if (err1->nameText != err2->nameText || err1->typeArgs->size != err2->typeArgs->size)
                return false;

            if (err1->parent != err2->parent) {
                if (!err1->parent->isTYPESYM() != !err2->parent->isTYPESYM())
                    return false;
                if (err1->parent->isTYPESYM() && !UnifyTypes(err1->parent->asTYPESYM(), err2->parent->asTYPESYM(), pctx))
                    return false;
            }

            // All the args must unify.
            for (int i = 0; i < err1->typeArgs->size; i++) {
                if (!UnifyTypes(err1->typeArgs->Item(i), err2->typeArgs->Item(i), pctx))
                    return false;
            }
        }
        return true;

    case SK_TYVARSYM:
        { // BLOCK
            TYPESYM ** ptype1; ptype1 = pctx->GetSlot(t1->asTYVARSYM());
            ASSERT(ptype1);

            // Since we substituted at the top, t1 should always be unmapped.
            ASSERT(*ptype1 == t1);

            switch (t2->getKind()) {
            default:
                ASSERT(!"Bad SYM kind in UnifyTypes");
                return false;

            case SK_NULLSYM:
            case SK_VOIDSYM:
            case SK_PARAMMODSYM:
            case SK_PTRSYM:
                // Can't assign these to a type variable.
                return false;

            case SK_ERRORSYM:
                if (!t2->parent)
                    return false;
                break;

            case SK_NUBSYM:
            case SK_ARRAYSYM:
            case SK_AGGTYPESYM:
                break;

            case SK_TYVARSYM:
                // Since we substituted at the top, t2 should always be unmapped.
                ASSERT(t2 == *pctx->GetSlot(t2->asTYVARSYM()));
                break;
            }

            // If t1 is in t2 then we can't unify - we have no recursive types.
            if (TypeContainsType(t2, t1))
                return false;

            // Eliminate t1 from the mappings. In particular, this will set *ptype1 to t2.
            for (int i = 0; i < pctx->typeVarsCls->size; i++) {
                pctx->prgtypeCls[i] = SubstTypeSingle(pctx->prgtypeCls[i], t1, t2);
            }
            for (int i = 0; i < pctx->typeVarsMeth->size; i++) {
                pctx->prgtypeMeth[i] = SubstTypeSingle(pctx->prgtypeMeth[i], t1, t2);
            }
            ASSERT(*ptype1 == t2);
#ifdef DEBUG
            // Any type variables that are mapped should not appear anywhere in the mappings.
            for (int i = 0; i < pctx->typeVarsCls->size; i++) {
                TYVARSYM * var = pctx->typeVarsCls->ItemAsTYVARSYM(i);
                if (pctx->prgtypeCls[i] != var) {
                    for (int j = 0; j < pctx->typeVarsCls->size; j++) {
                        ASSERT(!TypeContainsType(pctx->prgtypeCls[j], var));
                    }
                    for (int j = 0; j < pctx->typeVarsMeth->size; j++) {
                        ASSERT(!TypeContainsType(pctx->prgtypeMeth[j], var));
                    }
                }
            }
            for (int i = 0; i < pctx->typeVarsMeth->size; i++) {
                TYVARSYM * var = pctx->typeVarsMeth->ItemAsTYVARSYM(i);
                if (pctx->prgtypeMeth[i] != var) {
                    for (int j = 0; j < pctx->typeVarsCls->size; j++) {
                        ASSERT(!TypeContainsType(pctx->prgtypeCls[j], var));
                    }
                    for (int j = 0; j < pctx->typeVarsMeth->size; j++) {
                        ASSERT(!TypeContainsType(pctx->prgtypeMeth[j], var));
                    }
                }
            }
#endif // DEBUG
        }
        return true;
    }
}


/***************************************************************************************************
    Determine which set of types is more specific. Returns +1 if ta1 is more specific, -1 if ta2
    is more specific or 0 if neither.

    IF YOU CHANGE THIS METHOD BE SURE TO UPDATE CMethodMemberRef::CompareSignature IN THE LANGAUGE SERVICE!!!
***************************************************************************************************/
int BSYMMGR::CompareTypes(TypeArray * ta1, TypeArray * ta2)
{
    //IF YOU CHANGE THIS METHOD BE SURE TO UPDATE CMethodMemberRef::CompareSignature IN THE LANGAUGE SERVICE!!!

    if (ta1 == ta2)
        return 0;
    if (ta1->size != ta2->size) {
        // The one with more parameters is more specific.
        return ta1->size > ta2->size ? +1 : -1;
    }

    int nTot = 0;

    for (int i = 0; i < ta1->size; i++) {
        TYPESYM * type1 = ta1->Item(i);
        TYPESYM * type2 = ta2->Item(i);
        int nParam = 0;

LAgain:
        if (type1->getKind() != type2->getKind()) {
            if (type1->isTYVARSYM())
                nParam = -1;
            else if (type2->isTYVARSYM())
                nParam = +1;
        }
        else {
            switch (type1->getKind()) {
            default:
                ASSERT(!"Bad kind in CompareTypes");
                break;
            case SK_TYVARSYM:
            case SK_ERRORSYM:
                break;

            case SK_PTRSYM:
            case SK_PARAMMODSYM:
            case SK_ARRAYSYM:
            case SK_NUBSYM:
                type1 = type1->parent->asTYPESYM();
                type2 = type2->parent->asTYPESYM();
                goto LAgain;

            case SK_AGGTYPESYM:
                nParam = CompareTypes(type1->asAGGTYPESYM()->typeArgsAll, type2->asAGGTYPESYM()->typeArgsAll);
                break;
            }
        }

        if (nParam) {
            if (!nTot)
                nTot = nParam;
            else if (nParam != nTot)
                return 0;
        }
    }

    return nTot;

    //IF YOU CHANGE THIS METHOD BE SURE TO UPDATE CMethodMemberRef::CompareSignature IN THE LANGAUGE SERVICE!!!
}


void BSYMMGR::SetCanInferState(METHSYM * meth)
{
    ASSERT(meth->cisCanInfer == METHSYM::cisMaybe);

    // Determine whether we'll ever be able to infer.
    for (int ivar = 0; ivar < meth->typeVars->size; ivar++) {
        TYVARSYM * var = meth->typeVars->ItemAsTYVARSYM(ivar);

        // See if type var is used in a parameter.
        for (int ipar = 0; ; ipar++) {
            if (ipar >= meth->params->size) {
                // This type variable is not in any parameters.
                meth->cisCanInfer = METHSYM::cisNo;
                return;
            }
            if (TypeContainsType(meth->params->Item(ipar), var))
                break;
        }
    }

    // All type variables are used in a parameter.
    meth->cisCanInfer = METHSYM::cisYes;
}


// Find the inference mapping slot for the given type variable.
// Returns NULL if the type variable isn't a known one.
TYPESYM ** BSYMMGR::InferContext::GetSlot(TYVARSYM *tvs)
{
    if (!tvs->isMethTyVar || tvs->indexTotal >= typeVarsMeth->size ||
            tvs != typeVarsMeth->Item(tvs->indexTotal))
    {
        return NULL;
    }
    return prgtypeMeth + tvs->indexTotal;
}


// If typeDst doesn't contain method type variables this just returns true.
bool BSYMMGR::InferTypes(TYPESYM *typeSrc, TYPESYM *typeDst, InferContext *pctx)
{
    // NOTE: We substitute class type variables in typeDst with the types in pctx->typeArgsCls.

    // NOTE: We can't short circuit and test for typeSrc == typeDst here since
    // if there are method type variables in typeDst, we still need to map those to
    // themselves. This is so code like "void F<T>(T t) { F(t); }" will properly
    // infer that the type arg for the recursive call is <T>.

    // At the top level, the inference succeeds if typeDst doesn't contain any
    // method type variables. This is not true at recursive levels (handled by InferTypesEqual).
    if (!TypeContainsTyVars(typeDst, pctx->typeVarsMeth))
        return true;

    // Strip out / ref.
    if (typeDst->getKind() == SK_PARAMMODSYM)
        typeDst = typeDst->asPARAMMODSYM()->paramType();

LAgain:
    // Check for null or bad source types.
    switch (typeSrc->getKind()) {
    default:
        ASSERT(!"Bad src SYM kind in InferTypes");
        return false;

    case SK_PTRSYM:
        // Can't bind type variables to these.
        return false;

    case SK_PARAMMODSYM:
        typeSrc = typeSrc->asPARAMMODSYM()->paramType();
        goto LAgain;

    case SK_METHGRPSYM:
    case SK_ANONMETHSYM:
    case SK_NULLSYM:
        // We don't want to prohibit inference. We just can't bind a type variable
        // to these types. If the type variable is bound to a type through another
        // parameter, verifyArgs() will take care of the conversion.
        return true;

    case SK_ERRORSYM:
    case SK_ARRAYSYM:
    case SK_NUBSYM:
    case SK_AGGTYPESYM:
    case SK_TYVARSYM:
        break;
    }

LRecurse:
    ASSERT(
        typeSrc->isAGGTYPESYM() ||
        typeSrc->isARRAYSYM() ||
        typeSrc->isNUBSYM() ||
        typeSrc->isTYVARSYM() ||
        typeSrc->isERRORSYM());

    switch (typeDst->getKind()) {
    default:
        ASSERT(!"Bad dst SYM kind in InferTypes");
        return false;

    case SK_TYVARSYM:
        return InferTypesTyVar(typeSrc, typeDst->asTYVARSYM(), pctx);

    case SK_AGGTYPESYM:
        if (typeSrc->isERRORSYM())
            return false;
        return InferTypesAgg(typeSrc, typeDst->asAGGTYPESYM(), pctx);

    case SK_ERRORSYM:
        return InferTypesEqual(typeSrc, typeDst, pctx);

    case SK_NUBSYM:
    case SK_ARRAYSYM:
        if (typeSrc->getKind() != typeDst->getKind() ||
            typeSrc->isARRAYSYM() && typeSrc->asARRAYSYM()->rank != typeDst->asARRAYSYM()->rank)
        {
            return false;
        }

        typeDst = typeDst->parent->asTYPESYM();
        typeSrc = typeSrc->parent->asTYPESYM();

        switch (typeSrc->getKind()) {
        default:
            ASSERT(!"Bad src SYM kind in InferTypes");
            return false;

        case SK_PTRSYM:
            return false;

        case SK_ERRORSYM:
        case SK_ARRAYSYM:
        case SK_NUBSYM:
        case SK_AGGTYPESYM:
        case SK_TYVARSYM:
            break;
        }
        goto LRecurse;
    }
}


// This is used when matching type args for agg type syms. It requires typeSrc to match typeDst
// without any conversion. This assumes that typeSrc and typeDst came from type arguments.
bool BSYMMGR::InferTypesEqual(TYPESYM *typeSrc, TYPESYM *typeDst, InferContext *pctx)
{
LRecurse:
    ASSERT(
        typeSrc->isAGGTYPESYM() ||
        typeSrc->isARRAYSYM() ||
        typeSrc->isNUBSYM() ||
        typeSrc->isTYVARSYM() ||
        typeSrc->isERRORSYM());

    switch (typeDst->getKind()) {
    default:
        ASSERT(!"Bad SYM kind in InferTypesEqual");
        return false;

    case SK_TYVARSYM:
        return InferTypesTyVar(typeSrc, typeDst->asTYVARSYM(), pctx);

    case SK_AGGTYPESYM:
        if (!typeSrc->isAGGTYPESYM() || typeSrc->asAGGTYPESYM()->getAggregate() != typeDst->asAGGTYPESYM()->getAggregate())
            return false;
        for (int i = 0; i < typeSrc->asAGGTYPESYM()->typeArgsAll->size; i++) {
            if (!InferTypesEqual(typeSrc->asAGGTYPESYM()->typeArgsAll->Item(i), typeDst->asAGGTYPESYM()->typeArgsAll->Item(i), pctx))
                return false;
        }
        return true;

    case SK_ERRORSYM:
        if (!typeSrc->isERRORSYM() || !typeDst->parent || !typeSrc->parent)
            return false;

        {
            ERRORSYM * errSrc = typeSrc->asERRORSYM();
            ERRORSYM * errDst = typeDst->asERRORSYM();
            ASSERT(errSrc->parent && errSrc->nameText && errSrc->typeArgs);
            ASSERT(errDst->parent && errDst->nameText && errDst->typeArgs);

            if (errSrc->nameText != errDst->nameText || errSrc->typeArgs->size != errDst->typeArgs->size)
                return false;
            if (!errSrc->parent->isTYPESYM() != !errDst->parent->isTYPESYM())
                return false;
            if (errSrc->parent->isTYPESYM() && !InferTypesEqual(errSrc->parent->asTYPESYM(), errDst->parent->asTYPESYM(), pctx))
                return false;
            for (int i = 0; i < errSrc->typeArgs->size; i++) {
                if (!InferTypesEqual(errSrc->typeArgs->Item(i), errDst->typeArgs->Item(i), pctx))
                    return false;
            }
        }
        return true;

    case SK_ARRAYSYM:
    case SK_NUBSYM:
        if (typeSrc->getKind() != typeDst->getKind() ||
            typeSrc->isARRAYSYM() && typeSrc->asARRAYSYM()->rank != typeDst->asARRAYSYM()->rank)
        {
            return false;
        }

        typeDst = typeDst->parent->asTYPESYM();
        typeSrc = typeSrc->parent->asTYPESYM();

        switch (typeSrc->getKind()) {
        default:
            ASSERT(!"Bad src SYM kind in InferTypesEqual");
            return false;

        case SK_PTRSYM:
            return false;

        case SK_ERRORSYM:
        case SK_ARRAYSYM:
        case SK_NUBSYM:
        case SK_AGGTYPESYM:
        case SK_TYVARSYM:
            break;
        }
        goto LRecurse;
    }
}


bool BSYMMGR::InferTypesTyVar(TYPESYM * typeSrc, TYVARSYM * varDst, InferContext * pctx)
{
    ASSERT(
        typeSrc->isAGGTYPESYM() ||
        typeSrc->isARRAYSYM() ||
        typeSrc->isNUBSYM() ||
        typeSrc->isTYVARSYM() ||
        typeSrc->isERRORSYM());

    if (!varDst->isMethTyVar) {
        ASSERT((unsigned)varDst->indexTotal < (unsigned)pctx->typeArgsCls->size);
        return (typeSrc == pctx->typeArgsCls->Item(varDst->indexTotal));
    }

    if (!typeSrc->parent)
        return false;

    TYPESYM ** ptype = pctx->GetSlot(varDst);
    ASSERT(ptype);

    if (*ptype) {
        // See if the inference is consistent.
        return typeSrc == *ptype;
    }

    // Only assign an array if its base type is an agg, tyvar or error (not ptr, for example).
    if (typeSrc->isARRAYSYM()) {
        TYPESYM * type = typeSrc->asARRAYSYM()->GetMostBaseType();
        if (!type->isAGGTYPESYM() && !type->isNUBSYM() && !type->isTYVARSYM() && !type->isERRORSYM())
            return false;
    }

    // Make the inference.
    *ptype = typeSrc;
    return true;
}


// Called only by InferTypes to handle agg inference. This is complicated by requiring that there
// be a unique inference.
bool BSYMMGR::InferTypesAgg(TYPESYM *typeSrc, AGGTYPESYM *atsDst, InferContext *pctx)
{
    // If typeDst is A<...> we need to look for an instantiation of A to which typeSrc
    // is implicitly convertible (ie, a base type or interface).
    // If there is more than one such instantiation, the inferences need to be consistent.
    // If there is no such instantiation then inferencing fails.

    // Since atsDst contains a method type variable, it can't be object.
    // Thus we don't have to worry about implicit convertibility of an interface
    // to object.
    ASSERT(!atsDst->isPredefType(PT_OBJECT));

    if (typeSrc->isAGGTYPESYM() || typeSrc->isTYVARSYM()) {
        bool fFound = false;
        return InferTypesAggCore(typeSrc, atsDst, pctx, &fFound) && fFound;
    }

    if (!typeSrc->isARRAYSYM() || atsDst->typeArgsAll->size != 1 || !atsDst->isPredefined())
        return false;

    switch (atsDst->getPredefType()) {
    case PT_G_IENUMERABLE:
    case PT_G_ICOLLECTION:
    case PT_G_ILIST: 
        return InferTypes(typeSrc->asARRAYSYM()->elementType(), atsDst->typeArgsAll->Item(0), pctx);
    default:
        return false;
    }
}


// WARNING: The return result of this function carries different semantics than that of the other
// Infer* functions.
//
// This infers the values of method type variables in atsDst from the type args in atsSrc.
// * If the inference fails, this returns true and doesn't modify *pfInferred or pctx->prgtypeMeth.
// * If the inference succeeds, pctx->prgtypeMeth is updated with the results.
//   * If the update is consistent this returns true. In this case *pfInferred is set to true and 
//     pctx->prgtypeMeth is updated with any new inference information.
//   * If the update is inconsistent with the existing contents of pctx->prgtypeMeth, this returns false.
//     In this case, *pfInferred and pctx->prgtypeMeth may have been modified.
bool BSYMMGR::InferTypesAggCore(TYPESYM *typeSrc, AGGTYPESYM *atsDst, InferContext *pctx, bool * pfInferred)
{
    ASSERT(typeSrc->isAGGTYPESYM() || typeSrc->isTYVARSYM());

    if (typeSrc->isAGGTYPESYM()) {
        AGGTYPESYM * atsSrc = typeSrc->asAGGTYPESYM();

        if (atsSrc->getAggregate() == atsDst->getAggregate()) {
            if (!InferTypesAggSingle(atsSrc, atsDst, pctx, pfInferred))
                return false;
        }
        else if (atsDst->isInterfaceType()) {
            // Check to see if atsSrc or one of its base classes implements some instantiation of atsDst.
            for (AGGTYPESYM * ats = atsSrc; ats != NULL; ats = ats->GetBaseClass()) {
                TypeArray * ifaces = ats->GetIfacesAll();
                for (int j = 0; j < ifaces->size; j++) {
                    AGGTYPESYM * iface = ifaces->Item(j)->asAGGTYPESYM();
                    if (iface->getAggregate() == atsDst->getAggregate() && 
                        !InferTypesAggSingle(iface, atsDst, pctx, pfInferred))
                    {
                        return false;
                    }
                }
            }
        }
        else if (!atsSrc->isInterfaceType() && !atsDst->getAggregate()->isSealed) {
            // Check to see if atsSrc derives from some instantiation of atsDst.
            for (AGGTYPESYM * ats = atsSrc; (ats = ats->GetBaseClass()) != NULL; ) {
                if (ats->getAggregate() == atsDst->getAggregate() &&
                    !InferTypesAggSingle(ats, atsDst, pctx, pfInferred))
                {
                    return false;
                }
            }
        }
    }
    else if (typeSrc->isTYVARSYM()) {
        TypeArray * bnds = typeSrc->asTYVARSYM()->GetBnds();

        for (int i = 0; i < bnds->size; i++) {
            TYPESYM * bnd = bnds->Item(i);
            if ((bnd->isAGGTYPESYM() || bnd->isTYVARSYM()) && !InferTypesAggCore(bnd, atsDst, pctx, pfInferred))
                return false;
        }
    }

    return true;
}


// WARNING: The return result of this function carries different semantics than that of the other
// Infer* functions.
//
// This requires atsSrc->getAggregate() == atsDst->getAggregate().
// This infers the values of method type variables in atsDst from the type args in atsSrc.
// * If the inference fails, this returns true and doesn't modify *pfInferred or pctx->prgtypeMeth.
// * If the inference succeeds, pctx->prgtypeMeth is updated with the results.
//   * If the update is consistent this returns true. In this case *pfInferred is set to true and 
//     pctx->prgtypeMeth is updated with any new inference information.
//   * If the update is inconsistent with the existing contents of pctx->prgtypeMeth, this returns false.
//     In this case, *pfInferred and pctx->prgtypeMeth may have been modified.
bool BSYMMGR::InferTypesAggSingle(AGGTYPESYM * atsSrc, AGGTYPESYM * atsDst, InferContext * pctx, bool * pfInferred)
{
    ASSERT(atsSrc->getAggregate() == atsDst->getAggregate());
    ASSERT(atsDst->typeArgsAll->size == atsSrc->typeArgsAll->size);

    // We need to start with an empty inference context.
    InferContext ctx = *pctx;
    ctx.prgtypeMeth = STACK_ALLOC_ZERO(TYPESYM *, pctx->typeVarsMeth->size);

    for (int i = 0; i < atsSrc->typeArgsAll->size; i++) {
        if (!InferTypesEqual(atsSrc->typeArgsAll->Item(i), atsDst->typeArgsAll->Item(i), &ctx)) {
            // Don't touch pfInferred or pctx->prgtypeMeth! Just return true to indicate that nothing
            // inconsistent was found.
            return true;
        }
    }

    // Inferencing succeeded. Now make sure the results are consistent.
    for (int i = 0; i < ctx.typeVarsMeth->size; i++) {
        if (!pctx->prgtypeMeth[i]) {
            pctx->prgtypeMeth[i] = ctx.prgtypeMeth[i];
        }
        else if (pctx->prgtypeMeth[i] != ctx.prgtypeMeth[i] && ctx.prgtypeMeth[i]) {
            // Inconsistent.
            return false;
        }
    }

    *pfInferred = true;
    return true;
}


// Looks for typeFind in type. Returns true iff it appears.
bool BSYMMGR::TypeContainsType(TYPESYM *type, TYPESYM *typeFind)
{
LRecurse: // Label used for "tail" recursion.

    if (type == typeFind)
        return true;

    switch (type->getKind()) {
    default:
        ASSERT(!"Bad SYM kind in TypeContainsType");
        return false;

    case SK_NULLSYM:
    case SK_VOIDSYM:
    case SK_UNITSYM:
        // There should only be a single instance of these.
        ASSERT(typeFind->getKind() != type->getKind());
        return false;

    case SK_ARRAYSYM:
    case SK_NUBSYM:
    case SK_PARAMMODSYM:
    case SK_MODOPTTYPESYM:
    case SK_PTRSYM:
        type = type->parent->asTYPESYM();
        goto LRecurse;

    case SK_AGGTYPESYM:
        { // BLOCK
            AGGTYPESYM * ats = type->asAGGTYPESYM();

            for (int i = 0; i < ats->typeArgsAll->size; i++) {
                if (TypeContainsType(ats->typeArgsAll->Item(i), typeFind))
                    return true;
            }
        }
        return false;

    case SK_ERRORSYM:
        if (type->parent) {
            ERRORSYM * err = type->asERRORSYM();
            ASSERT(err->parent && err->nameText && err->typeArgs);

            for (int i = 0; i < err->typeArgs->size; i++) {
                if (TypeContainsType(err->typeArgs->Item(i), typeFind))
                    return true;
            }
            if (err->parent->isTYPESYM()) {
                type = err->parent->asTYPESYM();
                goto LRecurse;
            }
        }
        return false;

    case SK_TYVARSYM:
        return false;
    }
}


// If typeVars is empty, determines whether the type contains any type variables at all.
// If typeVars is non-empty, determines whether the type contains any of the type
// variables in "typeVars". The type variables in typeVars must be a "complete set",
// meaning that each one live in its "indexTotal" slot. That is,
// typeVars->ItemAsTYVARSYM(i)->indexTotal == i must be true for all i.
bool BSYMMGR::TypeContainsTyVars(TYPESYM * type, TypeArray * typeVars)
{
LRecurse: // Label used for "tail" recursion.
    switch (type->getKind()) {
    default:
        ASSERT(!"Bad SYM kind in TypeContainsTyVars");
        return false;

    case SK_NULLSYM:
    case SK_VOIDSYM:
    case SK_UNITSYM:
    case SK_METHGRPSYM:
        return false;

    case SK_ARRAYSYM:
    case SK_NUBSYM:
    case SK_PARAMMODSYM:
    case SK_MODOPTTYPESYM:
    case SK_PTRSYM:
    case SK_PINNEDSYM:
        type = type->parent->asTYPESYM();
        goto LRecurse;

    case SK_AGGTYPESYM:
        { // BLOCK
            AGGTYPESYM * ats = type->asAGGTYPESYM();

            for (int i = 0; i < ats->typeArgsAll->size; i++) {
                if (TypeContainsTyVars(ats->typeArgsAll->Item(i), typeVars))
                    return true;
            }
        }
        return false;

    case SK_ERRORSYM:
        if (type->parent) {
            ERRORSYM * err = type->asERRORSYM();
            ASSERT(err->parent && err->nameText && err->typeArgs);

            for (int i = 0; i < err->typeArgs->size; i++) {
                if (TypeContainsTyVars(err->typeArgs->Item(i), typeVars))
                    return true;
            }
            if (err->parent->isTYPESYM()) {
                type = err->parent->asTYPESYM();
                goto LRecurse;
            }
        }
        return false;

    case SK_TYVARSYM:
        if (typeVars && typeVars->size > 0) {
            int ivar = type->asTYVARSYM()->indexTotal;
            return ivar < typeVars->size && type == typeVars->Item(ivar);
        }
        return true;
    }
}


// Determines whether the type contains any generic types - either constructed types
// or type variables.
bool BSYMMGR::TypeContainsGenerics(TYPESYM * type)
{
LRecurse: // Label used for "tail" recursion.
    switch (type->getKind()) {
    default:
        ASSERT(!"Bad SYM kind in TypeContainsGenerics");
        return false;

    case SK_NULLSYM:
    case SK_VOIDSYM:
        return false;

    case SK_ARRAYSYM:
    case SK_PARAMMODSYM:
    case SK_MODOPTTYPESYM:
    case SK_PTRSYM:
    case SK_PINNEDSYM:
        type = type->parent->asTYPESYM();
        goto LRecurse;

    case SK_NUBSYM:
        return true;

    case SK_AGGTYPESYM:
        return type->asAGGTYPESYM()->typeArgsAll->size > 0;

    case SK_ERRORSYM:
        if (type->parent) {
            ERRORSYM * err = type->asERRORSYM();
            ASSERT(err->parent && err->nameText && err->typeArgs);

            if (err->typeArgs->size > 0)
                return true;
            if (err->parent->isTYPESYM()) {
                type = err->parent->asTYPESYM();
                goto LRecurse;
            }
        }
        return false;

    case SK_TYVARSYM:
        return true;
    }
}


/*
 * Add a sym to a symbol list. The memory for the list is allocated from
 * the global symbol area, so this is appropriate only for global symbols.
 *
 * The calls should pass a pointer to a local that's initialized to
 * point to the PSYMLIST that's the head of the list.
 */
void BSYMMGR::AddToGlobalSymList(PSYM sym, PSYMLIST * * symLink)
{
    ASSERT(!sym->isLocal);
    AddToSymList(allocGlobal, sym, symLink);
}


/*
 * Add a sym to a symbol list. The memory for the list is allocated from
 * the local symbol area, so this is appropriate only for local symbols.
 *
 * The calls should pass a pointer to a local that's initialized to
 * point to the PSYMLIST that's the head of the list.
 */
void LSYMMGR::AddToLocalSymList(PSYM sym, PSYMLIST * * symLink)
{
    AddToSymList(allocLocal, sym, symLink);
}


/*
 * Add a sym to a symbol list. The memory for the list is allocated from
 * the provided heap.
 *
 * The calls should pass a pointer to a local that's initialized to
 * point to the PSYMLIST that's the head of the list.
 */
void BSYMMGR::AddToSymList(NRHEAP *heap, PSYM sym, PSYMLIST ** symLink)
{
    PSYMLIST newList;

    // Allocate and fill in new list element.
    newList = (SYMLIST *) heap->Alloc(sizeof(SYMLIST));
    newList->sym = sym;
    newList->next = **symLink;

    // List onto end of list, and update the link value.
    **symLink = newList;
    *symLink = & newList->next;
}


/*
 * Add a name to a symbol list. The memory for the list is allocated from
 * the global symbol area, so this is appropriate only for global symbols.
 *
 * The calls should pass a pointer to a local that's initialized to
 * point to the PNAMELIST that's the head of the list.
 */
void BSYMMGR::AddToGlobalNameList(PNAME name, PNAMELIST * * nameLink)
{
    AddToNameList(allocGlobal, name, nameLink);
}

/*
 * Add a name to a symbol list. The memory for the list is allocated from
 * the local symbol area, so this is appropriate only for local symbols.
 *
 * The calls should pass a pointer to a local that's initialized to
 * point to the PNAMELIST that's the head of the list.
 */
void LSYMMGR::AddToLocalNameList(PNAME name, PNAMELIST * * nameLink)
{
    AddToNameList(allocLocal, name, nameLink);
}


/*
 * Add a name to a symbol list. The memory for the list is allocated from
 * the provided heap.
 *
 * The calls should pass a pointer to a local that's initialized to
 * point to the PNAMELIST that's the head of the list.
 */
void BSYMMGR::AddToNameList(NRHEAP *heap, PNAME name, PNAMELIST * * nameLink)
{
    PNAMELIST newList;

    ASSERT(**nameLink == NULL);

    // Allocate and fill in new list element.
    newList = (NAMELIST *) heap->Alloc(sizeof(NAMELIST));
    newList->name = name;
    newList->next = NULL;

    // List onto end of list, and update the link value.
    **nameLink = newList;
    *nameLink = & newList->next;
}

/*
 * Add an attribute node to a list. The memory for the list is allocated from
 * the global symbol area.

 * The calls should pass a pointer to a local that's initialized to
 * point to the PATTRLIST that's the head of the list.
 */
void BSYMMGR::AddToGlobalAttrList(BASENODE *node, PARENTSYM *context, PATTRLIST * * attrLink)
{
    AddToAttrList(allocGlobal, node, context, attrLink);
}

/*
 * Add an attribute to a symbol list. The memory for the list is allocated from
 * the local symbol area.
 *
 * The calls should pass a pointer to a local that's initialized to
 * point to the PATTRLIST that's the head of the list.
 */
void LSYMMGR::AddToLocalAttrList(BASENODE *node, PARENTSYM *context, PATTRLIST * * attrLink)
{
    return BSYMMGR::AddToAttrList(allocLocal, node, context, attrLink);
}


/*
 * Add an attribute  to a list. The memory for the list is allocated from
 * the provided heap.
 *
 * The calls should pass a pointer to a local that's initialized to
 * point to the PATTRLIST that's the head of the list.
 */
void BSYMMGR::AddToAttrList(NRHEAP *heap, BASENODE *node, PARENTSYM *context, PATTRLIST * * attrLink)
{
    PATTRLIST attrList;

    ASSERT(**attrLink == NULL);

    // Allocate and fill in new list element.
    attrList = (ATTRLIST *) heap->Alloc(sizeof(ATTRLIST));
    attrList->attr = node;
    attrList->context = context;
    attrList->fHadError = false;
    attrList->next = NULL;

    // List onto end of list, and update the link value.
    **attrLink = attrList;
    *attrLink = & attrList->next;
}


/*
 * Add a sym to a symbol list. The memory for the list is allocated from
 * the provided heap.
 *
 * The calls should pass a pointer to a local that's initialized to
 * point to the PSYMLIST that's the head of the list.
 */
void BSYMMGR::AddToSymList(MEMHEAP *heap, PSYM sym, PSYMLIST * symLink)
{
    PSYMLIST newList;

    // Allocate and fill in new list element.
    newList = (SYMLIST *) heap->Alloc(sizeof(SYMLIST));
    newList->sym = sym;
    newList->next = *symLink;

    // Link onto start of list.
    *symLink = newList;
}


/*
 * Frees memory used by a symbol list, but not the symbols.
 */
void BSYMMGR::FreeSymList(MEMHEAP *heap, PSYMLIST * symLink)
{
    PSYMLIST list = *symLink;

    while (list != NULL) {
        PSYMLIST next = list->next;
        heap->Free(list);
        list = next;        
    }

    *symLink = NULL;
}


/*
 * Get the COM+ signature element type from an aggregate type. 
 */
BYTE BSYMMGR::GetElementType(PAGGTYPESYM type)
{
    if (type->isPredefined()) {
        BYTE et = predefTypeInfo[type->getPredefType()].et;
        if (et != ELEMENT_TYPE_END)
            return et;
    }

    // Not a special type. Either a value or reference type.
    if (type->fundType() == FT_REF)
        return ELEMENT_TYPE_CLASS;
    else 
        return ELEMENT_TYPE_VALUETYPE;
}

/*
 * Some of the predefined types have built-in names, like "int"
 * or "string" or "object". This return the nice name if one
 * exists; otherwise NULL is returned.
 */
PCWSTR BSYMMGR::GetNiceName(PAGGSYM type)
{
    if (type->isPredefined) 
        return GetNiceName((PREDEFTYPE) type->iPredef);
    else
        return NULL;
}

/*
 * Some of the predefined types have built-in names, like "int"
 * or "string" or "object". This return the nice name if one
 * exists; otherwise NULL is returned.
 */
PCWSTR BSYMMGR::GetNiceName(PREDEFTYPE pt)
{
    return predefTypeInfo[pt].niceName;
}

int BSYMMGR::GetAttrArgSize(PREDEFTYPE pt)
{
    return predefTypeInfo[pt].asize;
}

FUNDTYPE BSYMMGR::GetPredefFundType(PREDEFTYPE pt)
{
    return predefTypeInfo[pt].ft;
}

PCWSTR BSYMMGR::GetFullName(PREDEFTYPE pt)
{
    return predefTypeInfo[pt].fullName;
}

PREDEFTYPE BSYMMGR::GetPredefIndex(CorElementType et)
{
    switch (et) {
        case ELEMENT_TYPE_BOOLEAN:
            return PT_BOOL;
        case ELEMENT_TYPE_CHAR:
            return PT_CHAR;
        case ELEMENT_TYPE_U1:
            return PT_BYTE;
        case ELEMENT_TYPE_I2:
            return PT_SHORT;
        case ELEMENT_TYPE_I4:
            return PT_INT;
        case ELEMENT_TYPE_I8:
            return PT_LONG;
        case ELEMENT_TYPE_R4:
            return PT_FLOAT;
        case ELEMENT_TYPE_R8:
            return PT_DOUBLE;
        case ELEMENT_TYPE_STRING:
            return PT_STRING;
        case ELEMENT_TYPE_OBJECT:
            return PT_OBJECT;
        case ELEMENT_TYPE_I1:      
            return PT_SBYTE;
        case ELEMENT_TYPE_U2:    
            return PT_USHORT;
        case ELEMENT_TYPE_U4:
            return PT_UINT;
        case ELEMENT_TYPE_U8:
            return PT_ULONG;
        case ELEMENT_TYPE_I:
            return PT_INTPTR;
        case ELEMENT_TYPE_U:
            return PT_UINTPTR;
        case ELEMENT_TYPE_TYPEDBYREF:
            return PT_REFANY;
        default:
            return (PREDEFTYPE) UNDEFINEDINDEX;
    }
}


/* 
 * Determine if a class/struct/interface (base) is a base of 
 * another class/struct/interface (derived), considering only the
 * head aggregate names. Object is NOT considered
 * a base of an interface but is considered as base of a struct.  This
 * is used for visibility rules only.
 */
bool COMPILER::IsBaseAggregate(PAGGSYM derived, PAGGSYM base)
{
    ASSERT(!derived->IsEnum() && !base->IsEnum());

    EnsureState(derived, AggState::Inheritance);
    EnsureState(base, AggState::Inheritance);

    if (derived == base)
        return true;      // identity.

    if (base->IsInterface()) {
        // Search the direct and indirect interfaces via ifacesAll, going up the base chain...

        while (derived) {
            for (int i = 0; i < derived->ifacesAll->size; i++) {
                AGGTYPESYM * iface = derived->ifacesAll->Item(i)->asAGGTYPESYM();
                if (iface->getAggregate() == base)
                    return true;
            }
            derived = derived->GetBaseAgg();
        }

        return false;
    }

    // base is a class. Just go up the base class chain to look for it.

    while (derived->baseClass) {
        derived = derived->baseClass->getAggregate();
        if (derived == base)
            return true;
    }

    return false;
}


/***************************************************************************************************
    Determine if a class/struct/interface (atsBase) is a base of another class/struct/interface
    (atsDer). Object IS considered a base of an interface or struct.  This operation takes into
    account "generic inheritance", e.g. class Foo<T> : Baz<List<T>>
***************************************************************************************************/
bool COMPILER::IsBaseType(AGGTYPESYM * atsDer, AGGTYPESYM * atsBase)
{
    EnsureState(atsDer, AggState::Inheritance);
    EnsureState(atsBase, AggState::Inheritance);

    // One nice optimization to have would be to just return false if the type is marked as sealed.  
    // This is not possible because in the refactoring scenario we may have code that derives from sealed types.

    // ResolveInheritance will give an error if we derive from a sealed type, for types defined in source.
    // Imported types that are derived from a sealed type are treated as bogus.  
    // IsBaseType(), then, will simply return wether or not it is specified as the base type, not whether or not it is a _valid_ base type.


    // The test for object is more than an optimization. It makes this return true
    // when atsDer is an interface.
    if (atsDer == atsBase || atsBase->isPredefType(PT_OBJECT))
        return true;

    if (atsBase->getAggregate()->IsInterface()) {
        // Search the direct and indirect interfaces via ifacesAll, going up the base chain...
        while (atsDer) {
            TypeArray * ifacesAll = atsDer->GetIfacesAll();
            for (int i = 0; i < ifacesAll->size; i++) {
                if (ifacesAll->Item(i)->asAGGTYPESYM() == atsBase)
                    return true;
            }
            atsDer = atsDer->GetBaseClass();
        }

        return false;
    }

    // Base is a class. Just go up the base class chain to look for it.
    while ((atsDer = atsDer->GetBaseClass()) != NULL) {
        if (atsDer == atsBase)
            return true;
    }

    return false;
}


/***************************************************************************************************
    Returns true if typeDer has typeBase as a base type. Implemented interfaces are considered
    to be base types. Also, array covariance is considered: object[] is considered to be a base
    type of string[].
***************************************************************************************************/
bool COMPILER::IsBaseType(TYPESYM * typeDer, TYPESYM * typeBase)
{
    AGGSYM * aggIList;

LAgain:
    EnsureState(typeDer, AggState::Inheritance);
    EnsureState(typeBase, AggState::Inheritance);

    switch (typeDer->getKind()) {
    case SK_AGGTYPESYM:
        if (typeDer == typeBase)
            return true;
        return typeBase->isAGGTYPESYM() && IsBaseType(typeDer->asAGGTYPESYM(), typeBase->asAGGTYPESYM());

    case SK_TYVARSYM:
        if (typeDer == typeBase || typeBase->isPredefType(PT_OBJECT))
            return true;
        {
            // Object is handled above.
            TypeArray * bnds = typeDer->asTYVARSYM()->GetBnds();
            for (int i = 0; i < bnds->size; i++) {
                if (IsBaseType(bnds->Item(i), typeBase))
                    return true;
            }
        }
        return false;

    case SK_NUBSYM:
        if (typeDer == typeBase)
            return true;
        typeDer = typeDer->asNUBSYM()->GetAts();
        if (typeDer)
            goto LAgain;
        return false;

    case SK_ARRAYSYM:
        if (typeDer == typeBase)
            return true;

        // Handle IList<T> and its base interfaces.
        // NOTE: This code assumes that any base interface of IList<T> has arity one
        // and is instantiated at T. When this code was written (May, 2004) we had
        //     IList<T> : ICollection<T> : IEnumerable<T>
        // so this assumption is/was true.
        if (typeDer->asARRAYSYM()->rank == 1 &&
            typeBase->isInterfaceType() &&
            typeBase->asAGGTYPESYM()->typeArgsAll->size == 1 &&
            (aggIList = GetOptPredefAgg(PT_G_ILIST, false)) != NULL &&
            IsBaseAggregate(aggIList, typeBase->asAGGTYPESYM()->getAggregate()))
        {
            typeDer = typeDer->asARRAYSYM()->elementType();
            typeBase = typeBase->asAGGTYPESYM()->typeArgsAll->Item(0);
            if (typeDer == typeBase)
                return true;
            // Recurse if typeDer is a reference type. Note: If both types are type variables and
            // typeDer has typeBase as a constraint, this will work even if typeBase->IsRefType()
            // returns false....
            if (!typeDer->IsRefType())
                return false;
        }
        else if (typeBase->isARRAYSYM()) {
            if (typeDer->asARRAYSYM()->rank != typeBase->asARRAYSYM()->rank)
                return false;
            typeDer = typeDer->asARRAYSYM()->elementType();
            typeBase = typeBase->asARRAYSYM()->elementType();
            // Recurse if typeDer is a reference type. Note: If both types are type variables and
            // typeDer has typeBase as a constraint, this will work even if typeBase->IsRefType()
            // returns false....
            if (!typeDer->IsRefType())
                return false;
        }
        else
            typeDer = GetReqPredefType(PT_ARRAY, false);

        // Tail recursion.
        goto LAgain;

    default:
        return false;
    }
}


/***************************************************************************************************
    Allocate a type array; used to represent a parameter list.
    We use a hash table to make sure that allocating the same type array
    twice returns the same value. This does two things:
    1) Save a lot of memory.
    2) Make it so parameter lists can be compared by a simple pointer comparison.
    3) Allow us to associate a token with each signature for faster metadata emit.
***************************************************************************************************/
TypeArray * BSYMMGR::AllocParams(int ctype, TYPESYM ** prgtype)
{
    ASSERT(prgtype || !ctype);
    if (!ctype) {
        // We have one standard empty TypeArray. It's not in the hash table.
        return &taEmpty;
    }

    return tableTypeArrays.AllocTypeArray(ctype, prgtype);
}


TypeArray * BSYMMGR::ConcatParams(int ctype1, TYPESYM ** prgtype1, int ctype2, TYPESYM ** prgtype2)
{
    int ctype = ctype1 + ctype2;
    TYPESYM ** prgtype = STACK_ALLOC(TYPESYM *, ctype);

    memcpy(prgtype, prgtype1, ctype1 * sizeof(TYPESYM *));
    memcpy(prgtype + ctype1, prgtype2, ctype2 * sizeof(TYPESYM *));

    return AllocParams(ctype, prgtype);
}


TypeArray * BSYMMGR::ConcatParams(TypeArray * pta, TYPESYM * type)
{
    if (!TypeArray::Size(pta))
        return AllocParams(1, &type);
    return ConcatParams(pta->size, pta->ItemPtr(0), 1, &type);
}


TypeArray * BSYMMGR::ConcatParams(TypeArray * pta1, TypeArray * pta2)
{
    if (!TypeArray::Size(pta1)) {
        if (!TypeArray::Size(pta2))
            return &taEmpty;
        return pta2;
    }
    if (!TypeArray::Size(pta2))
        return pta1;

    return ConcatParams(pta1->size, pta1->ItemPtr(0), pta2->size, pta2->ItemPtr(0));
}


/***************************************************************************************************
    Set the baseClass field of the AGGSYM.
***************************************************************************************************/
void COMPILER::SetBaseType(AGGSYM *cls, AGGTYPESYM *baseClass)
{
    ASSERT(!cls->baseClass);
    ASSERT(!baseClass || baseClass->getAggregate()->HasResolvedBaseClasses());
    ASSERT(baseClass || cls->IsInterface() || cls->isPredefAgg(PT_OBJECT));

    cls->baseClass = baseClass;
}


/***************************************************************************************************
    Set the interfaces of the AGGSYM.
***************************************************************************************************/
void COMPILER::SetIfaces(AGGSYM *agg, AGGTYPESYM ** prgiface, int ciface)
{
    TypeArray * ifaces = getBSymmgr().AllocParams(ciface, (TYPESYM **)prgiface);
    SetIfaces(agg, ifaces);
}


/***************************************************************************************************
    Set the interfaces of the AGGSYM.

    Builds the full list of supported interfaces for a type. Also checks for conflicting members
    between interfaces in the list, and all their inherited interfaces.

    It is important that we form the lists in a given order.  Basically, an interface may not be
    preceeded by any interfaces that it descends from.  So, we always add to the front of the list.
***************************************************************************************************/
void COMPILER::SetIfaces(AGGSYM *agg, TypeArray * ifaces)
{
    ASSERT(ifaces);
    ASSERT(agg->typeVarsThis);
    ASSERT(agg->typeVarsAll);

    ASSERT(!agg->ifacesAll);
    ASSERT(!agg->ifaces);

    int ciface = ifaces->size;
    TypeArray * ifacesAll;

    if (ciface == 0) {
        ASSERT(ifaces == BSYMMGR::EmptyTypeArray());
        ifacesAll = ifaces;
    }
    else if (agg->typeVarsAll->size == 0) {
        ASSERT(agg->typeVarsThis->size == 0);
        ifacesAll = getBSymmgr().BuildIfacesAll(agg, (AGGTYPESYM **)ifaces->ItemPtr(0), ciface, NULL);
    }
    else {
        BSYMMGR::UnifyContext ctx(agg->typeVarsAll, BSYMMGR::EmptyTypeArray(), STACK_ALLOC_ZERO(TYPESYM *, agg->typeVarsAll->size), NULL);
        ifacesAll = getBSymmgr().BuildIfacesAll(agg, (AGGTYPESYM **)ifaces->ItemPtr(0), ciface, &ctx);
    }

    agg->ifaces = ifaces;
    agg->ifacesAll = ifacesAll;

}




/***************************************************************************************************
    Set the bounds of the type variable to the given set. This just sets the bnds field. After
    all siblings TYVARSYMs have their bounds set, call ResolveBounds on each.
***************************************************************************************************/
void COMPILER::SetBounds(TYVARSYM * var, TypeArray * bnds, bool fReset)
{
    ASSERT(bnds);

    ASSERT(!var->GetBnds() || fReset);
    ASSERT(!var->FResolved() || fReset);
    ASSERT(!var->fResolving || fReset);

    TYPESYM ** prgtype = STACK_ALLOC(TYPESYM *, bnds->size);
    int ctype = 0;

    // Filter out dups and System.Object.
    for (int i = 0; i < bnds->size; i++) {
        // Don't record System.Object.
        if (bnds->Item(i)->isPredefType(PT_OBJECT))
            continue;

        for (int j = ctype; ; ) {
            if (j == 0) {
                // The type doesn't match a previous type so record it.
                prgtype[ctype++] = bnds->Item(i);
                break;
            }
            j--;
            if (bnds->Item(i) == prgtype[j]) {
                // It matches a previous type so don't record it.
                break;
            }
        }
    }

    if (ctype < bnds->size)
        bnds = getBSymmgr().AllocParams(ctype, prgtype);


    var->SetBnds(bnds);
}


/***************************************************************************************************
    Searches an array of AGGTYPESYMs for a particular ats. Returns true if found.
***************************************************************************************************/
static bool FindAts(AGGTYPESYM * ats, AGGTYPESYM ** pats, AGGTYPESYM ** patsLim)
{
    for ( ; pats < patsLim; pats++) {
        if (*pats == ats)
            return true;
    }
    return false;
}


/***************************************************************************************************
    Recursively compute the ifacesAll and atsCls members of a type variable.
    This may recurse since a type variable may have another type variable as a bound.
***************************************************************************************************/
bool COMPILER::ResolveBounds(TYVARSYM * var, bool fInherited)
{
    ASSERT(var);
    ASSERT(var->GetBnds());

    if (var->FResolved()) {
        ASSERT(!var->fResolving);
        return true;
    }

    if (var->fResolving) {
        // Circularity. Caller should report an error.
        return false;
    }

    var->fResolving = true;

    bool fValType = false;
    TypeArray * bnds = var->GetBnds();
    AGGTYPESYM * atsBaseCls = GetReqPredefType(PT_OBJECT, false);
    TYPESYM * typeBaseAbs = atsBaseCls;

    // Resolve base type vars, compute the max number of interfaces and
    // determine atsCls - the most derived base class. Also filter out any
    // bounds that cause cycles in the bound hierarchy.
    TYPESYM ** prgtype = STACK_ALLOC(TYPESYM *, bnds->size);
    int ctype = 0;
    int cifaceMax = 0;

    for (int i = 0; i < bnds->size; i++) {
        TYPESYM * typeBnd = bnds->Item(i);
        AGGTYPESYM * atsBase = NULL; // Effective base class.
        TYPESYM * typeBase = NULL; // Other base type, like object[], int, etc.

        switch (typeBnd->getKind()) {
        case SK_AGGTYPESYM:
            ASSERT(typeBnd->getAggregate()->HasResolvedBaseClasses());

            switch (typeBnd->getAggregate()->AggKind()) {
            case AggKind::Interface:
                cifaceMax += typeBnd->asAGGTYPESYM()->GetIfacesAll()->size + 1;
                prgtype[ctype++] = typeBnd;
                continue;
            case AggKind::Class:
                typeBase = atsBase = typeBnd->asAGGTYPESYM();
                break;
            case AggKind::Delegate:
                ASSERT(fInherited);
                typeBase = atsBase = typeBnd->asAGGTYPESYM();
                break;
            case AggKind::Struct:
                // Effective base class is System.ValueType.
                ASSERT(fInherited);
                atsBase = GetReqPredefType(PT_VALUE, false);
                typeBase = typeBnd;
                break;
            case AggKind::Enum:
                // Effective base class is System.Enum.
                ASSERT(fInherited);
                atsBase = GetReqPredefType(PT_ENUM, false);
                typeBase = typeBnd;
                break;
            default:
                VSFAIL("Bad AggKind");
                typeBase = typeBnd;
                break;
            }
            ASSERT(!atsBase || atsBase->isClassType());
            break;

        case SK_TYVARSYM:
            {
                TYVARSYM * varBnd = typeBnd->asTYVARSYM();
                if (!ResolveBounds(varBnd, fInherited)) {
                    ASSERT(!varBnd->GetIfacesAll());
                    ErrorRef(NULL, ERR_CircularConstraint, varBnd, var);
                    continue;
                }
                ASSERT(varBnd->FResolved());
                if (varBnd->FValCon()) {
                    if (!fInherited) {
                        ErrorRef(NULL, ERR_ConWithValCon, var, varBnd);
                        continue;
                    }
                    fValType = true;
                }
                cifaceMax += varBnd->GetIfacesAll()->size;
                atsBase = varBnd->GetBaseCls();
                typeBase = varBnd->GetAbsoluteBaseType();
            }
            break;

        case SK_NUBSYM:
            // This should only happen if we're computing bounds automagically....
            ASSERT(fInherited);
            atsBase = GetReqPredefType(PT_VALUE, false);
            typeBase = typeBnd;
            break;

        case SK_ARRAYSYM:
            // This should only happen if we're computing bounds automagically....
            ASSERT(fInherited);
            atsBase = GetReqPredefType(PT_ARRAY, false);
            typeBase = typeBnd;
            break;

        default:
            // Some other type. This should only happen if we're computing bounds
            // automagically....
            ASSERT(fInherited);
            typeBase = typeBnd;
            break;
        }
        prgtype[ctype++] = typeBnd;

        ASSERT(typeBase && (!atsBase || IsBaseType(typeBase, atsBase)));

        if (!IsBaseType(typeBaseAbs, typeBase)) {
            if (!IsBaseType(typeBase, typeBaseAbs))
                Error(NULL, ERR_BaseConstraintConflict, ErrArgRef(var), typeBase, typeBaseAbs);
            else if (atsBase && !IsBaseType(atsBase, atsBaseCls)) {
                VSFAIL("Bad logic - typeBase derives from typeBaseAbs, but atsBase doesn't derive from atsBaseCls");
            }
            else {
                typeBaseAbs = typeBase;
                if (atsBase)
                    atsBaseCls = atsBase;
            }
        }
        else if (atsBase && !IsBaseType(atsBaseCls, atsBase)) {
            VSFAIL("Bad logic - typeBaseAbs derives from typeBase, but atsBaseCls doesn't derive from atsBase");
        }
    }

    // If circularity is detected, our actual set of bounds is less than
    // the original set.
    ASSERT(0 <= ctype && ctype <= bnds->size);
    if (ctype < bnds->size) {
        bnds = getBSymmgr().AllocParams(ctype, prgtype);
        var->SetBnds(bnds);
    }

    ASSERT(IsBaseType(typeBaseAbs, atsBaseCls));
    var->SetBaseTypes(typeBaseAbs, atsBaseCls);
    ASSERT(!var->fHasRefBnd && !var->fHasValBnd);

    if (fValType || typeBaseAbs->IsValType())
        var->fHasValBnd = true;
    if (typeBaseAbs->IsRefType()) {
        if (!typeBaseAbs->isPredefined())
            var->fHasRefBnd = true;
        else {
            PREDEFTYPE pt = typeBaseAbs->getPredefType();
            var->fHasRefBnd = (pt != PT_OBJECT && pt != PT_VALUE && pt != PT_ENUM);
        }
    }

    TypeArray * ifacesAll;

    if (cifaceMax > 0) {
        AGGTYPESYM ** pifaceMin = STACK_ALLOC(AGGTYPESYM *, cifaceMax);
        AGGTYPESYM ** pifaceLim = pifaceMin + cifaceMax;
        AGGTYPESYM ** piface = pifaceLim;

        // Always add to the front so process the interfaces in the reverse order.
        for (int i = bnds->size; --i >= 0; ) {
            TYPESYM * typeBnd = bnds->Item(i);

            if (typeBnd->isAGGTYPESYM()) {
                AGGTYPESYM * atsBnd = typeBnd->asAGGTYPESYM();
                if (!atsBnd->getAggregate()->IsInterface())
                    continue;

                // If atsBnd is already in this array then the interface has already been seen
                // so all of its ifaces have also been seen.
                if (FindAts(atsBnd, piface, pifaceLim))
                    continue;
                ifacesAll = atsBnd->GetIfacesAll();
            }
            else if (typeBnd->isTYVARSYM()) {
                ASSERT(typeBnd->asTYVARSYM()->FResolved());
                ifacesAll = typeBnd->asTYVARSYM()->GetIfacesAll();
            }
            else {
                ASSERT(fInherited);
                continue;
            }

            // Add everything in ifacesAll.
            for (int j = ifacesAll->size; --j >= 0; ) {
                AGGTYPESYM * atsChild = ifacesAll->Item(j)->asAGGTYPESYM();
                ASSERT(atsChild->isInterfaceType());
                if (!FindAts(atsChild, piface, pifaceLim)) {
                    ASSERT(pifaceMin < piface);
                    *--piface = atsChild;
                }
            }

            if (typeBnd->isAGGTYPESYM()) {
                ASSERT(!FindAts(typeBnd->asAGGTYPESYM(), piface, pifaceLim));
                ASSERT(pifaceMin < piface);
                *--piface = typeBnd->asAGGTYPESYM();
            }
        }

        ifacesAll = getBSymmgr().AllocParams((int)(pifaceLim - piface), (TYPESYM **)piface);
    }
    else {
        ifacesAll = BSYMMGR::EmptyTypeArray();
    }

    var->SetIfacesAll(ifacesAll);
    var->fResolving = false;

    ASSERT(var->FResolved());

    var->SetAggState(AggState::None);
    ComputeAggState(var);

    return true;
}


TypeArray * BSYMMGR::BuildIfacesAll(SYM * symErr, AGGTYPESYM ** prgiface, int ciface, UnifyContext * pctx)
{
    if (ciface == 0)
        return EmptyTypeArray();

    TypeArray * ifacesAll;
    bool fErrors = false;

    // Get an upper bound on the number of interfaces.
    int cifaceMax = ciface;

    for (int i = 0; i < ciface; i++) {
        AGGTYPESYM * iface = prgiface[i];
        ASSERT(iface->isInterfaceType());
        ASSERT(iface->getAggregate()->HasResolvedBaseClasses());
        cifaceMax += iface->GetIfacesAll()->size;
    }

    AGGTYPESYM ** pifaceMin = STACK_ALLOC(AGGTYPESYM *, cifaceMax);
    AGGTYPESYM ** pifaceLim = pifaceMin + cifaceMax;
    AGGTYPESYM ** piface = pifaceLim;

    // Always add to the front so process the interfaces in the reverse order.
    for (int i = ciface; --i >= 0; ) {
        AGGTYPESYM * iface = prgiface[i];
        fErrors |= !AddUniqueInterfaces(symErr, pifaceMin, &piface, pifaceLim, iface, pctx);
        ASSERT(pifaceMin <= piface && piface <= pifaceLim);
    }

    int cifaceAll = (int)(pifaceLim - piface);
    ifacesAll = AllocParams(cifaceAll, (TYPESYM **)piface);

#ifdef DEBUG
    // Check that we indeed added them all and in the correct order!
    for (int i = 0; i < ciface; i++) {
        DbgCheckIfaceListOrder(prgiface[i], ifacesAll, !fErrors);
    }
#endif

    return ifacesAll;
}


//
// Builds the recursive closure of an interface list.  For example, 
//    interface A<T> { }
//    interface B<T> : A<A<T>> { }
//    interface C : B<String>, A<String>, B<A<String>> { }
//
//  C implements B<String>, A<A<String>>, A<String>, B<A<String>>, A<A<A<String>>>
// The substitution step is used as always when accessing information about a base class or
// interface which may be specialized to a particular type.
// Returns false if there were some errors.
bool BSYMMGR::AddUniqueInterfaces(SYM * symErr, AGGTYPESYM ** ptypeMin, AGGTYPESYM *** pptype, AGGTYPESYM ** ptypeLim, AGGTYPESYM * iface, UnifyContext * pctx)
{
    ASSERT(ptypeMin <= *pptype && *pptype <= ptypeLim);

    // See if it's already there.
    bool fErrors = false;

    if (CheckForUnifyingInstantiation(symErr, *pptype, (int)(ptypeLim - *pptype), iface, pctx, &fErrors))
        return !fErrors;

    // Add the children.
    TypeArray * ifacesAll = iface->GetIfacesAll();
    for (int i = ifacesAll->size; --i >= 0; ) {
        AGGTYPESYM * child = ifacesAll->Item(i)->asAGGTYPESYM();
        ASSERT(child->isInterfaceType());
        if (!CheckForUnifyingInstantiation(symErr, *pptype, (int)(ptypeLim - *pptype), child, pctx, &fErrors)) {
            ASSERT(ptypeMin < *pptype);
            *--(*pptype) = child;
        }
    }

    // Add the interface.
    ASSERT(ptypeMin < *pptype);
    *--(*pptype) = iface;

    return !fErrors;
}


// *pfErrors is [in / out]. This routine never sets it to false.
// Returns true iff there was a duplicate or conflict.
bool BSYMMGR::CheckForUnifyingInstantiation(SYM *symErr, AGGTYPESYM ** prgtype, int ctype, AGGTYPESYM *iface, UnifyContext * pctx, bool * pfErrors)
{
    // Check for more than one instantiation of same interface. Allow distinct intstantiations as long as they can never unify.
    for (int i = 0; i < ctype; i++) {
        AGGTYPESYM * type = prgtype[i];

        if (type == iface)
            return true;

        if (pctx && type->getAggregate() == iface->getAggregate()) {
            // The aggs are the same but the types aren't, so the agg MUST be generic.
            ASSERT(type->getAggregate()->typeVarsAll->size > 0);
            ASSERT(pctx->typeVarsCls->size > 0 || pctx->typeVarsMeth->size);

            pctx->Clear();
            if (UnifyTypes(type, iface, pctx)) {
                if (!*pfErrors) {
                    if (symErr->isAGGSYM() && symErr->asAGGSYM()->isSource || symErr->isTYVARSYM()) {
                        Error(ERR_UnifyingInterfaceInstantiations, ErrArgRef(symErr), iface, type);
                    } else {
                        symErr->setBogus(true);
                    }
                    *pfErrors = true;
                }
                // Add it anyway.
                return false;
            }
        }
    }

    return false;
}


#ifdef DEBUG
void BSYMMGR::DbgCheckIfaceListOrder(AGGTYPESYM * iface, TypeArray * ifaces, bool fCheckTypes)
{
    ASSERT(iface->isInterfaceType());

    // Make sure iface is in the list.
    bool fFound = false;
    for (int i = 0; i < ifaces->size; i++) {
        AGGTYPESYM * temp = ifaces->Item(i)->asAGGTYPESYM();
        if (temp == iface) {
            if (fFound) {
                ASSERT(!"iface is in the list twice!");
                return;
            }
            if (temp != iface && fCheckTypes) {
                ASSERT(!"wrong instantiation of the interface!");
                return;
            }
            fFound = true;
        }
    }
    if (!fFound) {
        ASSERT(!"iface is not in the list!");
        return;
    }

    // Make sure all the base interfaces are in the list and after iface.
    TypeArray * ifacesAll = iface->GetIfacesAll();
    for (int i = 0; i < ifacesAll->size; i++) {
        AGGTYPESYM * base = ifacesAll->Item(i)->asAGGTYPESYM();
        bool fBefore = true;
        fFound = false;

        for (int j = 0; j < ifaces->size; j++) {
            AGGTYPESYM * temp = ifaces->Item(j)->asAGGTYPESYM();
            if (temp == base) {
                if (fFound) {
                    ASSERT(!"base is in the list twice!");
                    return;
                }
                if (fBefore) {
                    ASSERT(!"base is before iface!");
                    return;
                }
                if (temp != base && fCheckTypes) {
                    ASSERT(!"wrong instantiation of the base interface!");
                    return;
                }
                fFound = true;
            }
            else if (temp == iface) {
                ASSERT(fBefore);
                fBefore = false;
            }
        }
        if (!fFound) {
            ASSERT(!"base is not in the list!");
            return;
        }
    }
}
#endif


bool COMPILER::CheckBogusCore(SYM * sym, bool fNoEnsure, bool * pfUndeclared)
{
    if (!sym)
        return false;
    if (sym->hasBogus())
        return sym->checkBogus();

    bool fBogus = false;
    bool fUndeclared = false;

    switch (sym->getKind()) {
    case SK_PROPSYM:
    case SK_METHSYM:
    case SK_FAKEMETHSYM:
        {
            METHPROPSYM * meth = sym->asMETHPROPSYM();

            fBogus = CheckBogusCore(meth->retType, fNoEnsure, &fUndeclared);

            // We need to check the parameters as well.
            if (meth->params) {
                for (int i = 0; !fBogus && i < meth->params->size; i++) {
                    fBogus |= CheckBogusCore(meth->params->Item(i), fNoEnsure, &fUndeclared);
                }
            }
            else {
                // Params should only be NULL if fNoEnsure is set, in which case we don't want
                // to call setBogus(false)!
                ASSERT(fNoEnsure);
                fUndeclared = true;
            }
        }
        break;

    case SK_PARAMMODSYM:
    case SK_MODOPTTYPESYM:
    case SK_PTRSYM:
    case SK_ARRAYSYM:
    case SK_NUBSYM:
    case SK_PINNEDSYM:
        fBogus = CheckBogusCore(sym->parent->asTYPESYM(), fNoEnsure, &fUndeclared);
        break;

    case SK_EVENTSYM:
        fBogus = CheckBogusCore(sym->asEVENTSYM()->type, fNoEnsure, &fUndeclared);
        break;

    case SK_MEMBVARSYM:
        fBogus = CheckBogusCore(sym->asVARSYM()->type, fNoEnsure, &fUndeclared);
        break;

    case SK_ERRORSYM:
        sym->setBogus(false);
        break;

    case SK_AGGTYPESYM:
        fBogus = CheckBogusCore(sym->asAGGTYPESYM()->getAggregate(), fNoEnsure, &fUndeclared);
        for (int i = 0; !fBogus && i < sym->asAGGTYPESYM()->typeArgsAll->size; i++) {
            fBogus |= CheckBogusCore(sym->asAGGTYPESYM()->typeArgsAll->Item(i), fNoEnsure, &fUndeclared);
        }
        break;

    case SK_TYVARSYM:  
    case SK_VOIDSYM:
    case SK_NULLSYM:
    case SK_UNITSYM:
    case SK_LOCVARSYM:
        sym->setBogus(false);
        break;

    case SK_AGGSYM:
        if (!fNoEnsure)
            EnsureState(sym->asAGGSYM());
        fUndeclared = !sym->asAGGSYM()->IsPrepared();
        fBogus = sym->hasBogus() && sym->checkBogus();
        break;

    case SK_SCOPESYM:
    case SK_ANONSCOPESYM:
    case SK_NSSYM:
    case SK_NSDECLSYM:
    default:
        ASSERT(!"CheckBogus with invalid Symbol kind");
        sym->setBogus(false);
        break;
    }

    if (!fUndeclared || fBogus) {
        // Only set this if everything is declared or
        // at least 1 declared thing is bogus
        sym->setBogus(fBogus);
    }

    if (pfUndeclared)
        *pfUndeclared |= fUndeclared;

    return sym->hasBogus() && sym->checkBogus();
}


// Generates an error for static classes
void COMPILER::ReportStaticClassError(BASENODE * tree, SYM * symCtx, TYPESYM * type, int err)
{
    if (symCtx)
        compiler()->Error(tree, err, type, ErrArgRef(symCtx));
    else
        compiler()->Error(tree, err, type);
}


// Generate an error if type is static.
bool COMPILER::CheckForStaticClass(BASENODE * tree, SYM * symCtx, TYPESYM * type, int err)
{
    if (!type->isStaticClass())
        return false;

    ReportStaticClassError(tree, symCtx, type, err);
    return true;
}


void BSYMMGR::Error(int id, ErrArg arg)
{
    host()->ErrorLocArgs(NULL, id, 1, &arg);
}


void BSYMMGR::Error(int id, ErrArg arg1, ErrArg arg2)
{
    ErrArg rgarg[2] = { arg1, arg2 };
    host()->ErrorLocArgs(NULL, id, 2, rgarg);
}

void BSYMMGR::Error(int id, ErrArg arg1, ErrArg arg2, ErrArg arg3)
{
    ErrArg rgarg[3] = { arg1, arg2, arg3 };
    host()->ErrorLocArgs(NULL, id, 3, rgarg);
}

void BSYMMGR::ErrorRef(int id, ErrArgRef arg)
{
    host()->ErrorLocArgs(NULL, id, 1, &arg);
}

void BSYMMGR::ErrorRef(int id, ErrArgRef arg1, ErrArgRef arg2)
{
    ErrArg rgarg[2] = { arg1, arg2 };
    host()->ErrorLocArgs(NULL, id, 2, rgarg);
}

void BSYMMGR::ErrorRef(int id, ErrArgRef arg1, ErrArgRef arg2, ErrArgRef arg3)
{
    ErrArg rgarg[3] = { arg1, arg2, arg3 };
    host()->ErrorLocArgs(NULL, id, 3, rgarg);
}

#ifdef DEBUG
/****************************************************************************
 * Symbol dumping routines.
 */



/*
 * Print <indent" spaces to stdout.
 */
static void PrintIndent(int indent)
{
    for (int i = 0; i < indent; ++i)
        wprintf(L" ");
}

/*
 * Dump the children of a symbol to stdout.
 */
void BSYMMGR::DumpChildren(PPARENTSYM sym, int indent)
{
    FOREACHCHILD(sym, symChild)
        if (symChild->isNSSYM() || symChild->isMEMBVARSYM() ||
            symChild->isMETHSYM() || symChild->isPROPSYM() ||
            symChild->isAGGDECLSYM() || symChild->isAGGSYM() || symChild->isERRORSYM() ||
            symChild->isNSDECLSYM() || symChild->isTYVARSYM() ||
            symChild->isLOCVARSYM() || symChild->isSCOPESYM() || symChild->isANONSCOPESYM())
        {
            DumpSymbol(symChild, indent + 2);
        }
    ENDFOREACHCHILD
}

/*
 * Dump a text representation of an access level to stdout
 */
void BSYMMGR::DumpAccess(ACCESS acc)
{
    switch (acc) {
    case ACC_PRIVATE:
        wprintf(L"private "); break;
    case ACC_INTERNAL:
        wprintf(L"internal "); break;
    case ACC_PROTECTED:
        wprintf(L"protected "); break;
    case ACC_INTERNALPROTECTED:
        wprintf(L"internal protected "); break;
    case ACC_PUBLIC:
        wprintf(L"public "); break;
    default:
        wprintf(L"/* unknown access */"); break;
    }
}

/* 
 * Dump a text representation of a constant value
 */
void BSYMMGR::DumpConst(PTYPESYM type, CONSTVAL * constVal)
{
    if (type->isPredefType(PT_BOOL)) {
        wprintf(L"%s", constVal->iVal ? L"true" : L"false");
        return;
    }   
    else if (type->isPredefType(PT_STRING)) {
        STRCONST * strConst = constVal->strVal;
        wprintf(L"\"");
        for (int i = 0; i < strConst->length; ++i) {
            wprintf(L"%lc", strConst->text[i]);
        }
        wprintf(L"\"");
        return;
    }

    switch (type->fundType())
    {
    case FT_I1: case FT_I2: case FT_I4:
        wprintf(L"%d", constVal->iVal);
        break;
    case FT_U1: case FT_U2: case FT_U4:
        wprintf(L"%u", constVal->uiVal);
        break;
    case FT_I8:
        wprintf(L"%I64d", *constVal->longVal);
        break;
    case FT_U8:
        wprintf(L"%I64u", *constVal->ulongVal);
        break;
    case FT_R4:
    case FT_R8:
        wprintf(L"%g", *constVal->doubleVal);
        break;
    case FT_REF:
        ASSERT(constVal->init == 0);
        wprintf(L"null");
        break;
    case FT_STRUCT:
        wprintf(L"<struct>");
        break;
    default:
        ASSERT(0);
    }
}

/* 
 * Dump a text representation of a symbol to stdout.
 */
void BSYMMGR::DumpSymbol(PSYM sym, int indent)
{
    int i;

    if (sym == NULL) {
        wprintf(L"*NULL*");
        return;
    }

    switch (sym->getKind()) {
    case SK_NSSYM:
        {
            for (NSDECLSYM * decl = sym->asNSSYM()->DeclFirst(); decl; decl = decl->DeclNext()) {
                DumpSymbol(decl, indent);
            }
        }
        break;
    case SK_NSDECLSYM:
        PrintIndent(indent);
        // We gave the NSDECL the same name as the NS.
        wprintf(L"namespace %ls {\n", sym->name->text);

        DumpChildren(sym->asPARENTSYM(), indent);

        PrintIndent(indent);
        wprintf(L"}\n");
        break;

    case SK_AGGDECLSYM:
        if (!sym->asAGGDECLSYM()->IsFirst())
            return;

        sym = sym->asAGGDECLSYM()->Agg();
        // Fall through.
    case SK_AGGSYM:
        PrintIndent(indent);

        DumpAccess(sym->asAGGSYM()->GetAccess());

        switch (sym->asAGGSYM()->AggKind()) {
        case AggKind::Class:
            wprintf(L"class");
            break;
        case AggKind::Interface:
            wprintf(L"interface");
            break;
        case AggKind::Enum:
            wprintf(L"enum");
            break;
        case AggKind::Struct:
            wprintf(L"struct");
            break;
        case AggKind::Delegate:
            wprintf(L"delegate");
            break;
        default:
            wprintf(L"*unknown*");
            break;
        }

        if (sym->asAGGSYM()->typeVarsThis->size > 0) {
            wprintf(L" %ls<", sym->name->text);
            AGGSYM * agg = sym->asAGGSYM();
            for (int i = 0; i < agg->typeVarsThis->size; i++) {
                if (i > 0)
                    wprintf( L", ");
                DumpSymbol(agg->typeVarsThis->Item(i));
            }
            wprintf(L">");
        } else
            wprintf(L" %ls ", sym->name->text);

        // Base class.
        if (sym->asAGGSYM()->baseClass) {
            wprintf(L": ");
            if (sym->asAGGSYM()->IsEnum())
                DumpType(sym->asAGGSYM()->underlyingType);
            else
                DumpType(sym->asAGGSYM()->baseClass);
        }
        if (TypeArray::Size(sym->asAGGSYM()->ifaces) > 0) {
            if (sym->asAGGSYM()->baseClass)
                wprintf(L", ");
            else
                wprintf(L": ");

            TypeArray * ifaces = sym->asAGGSYM()->ifaces;
            for (int i = 0; i < ifaces->size; i++) {
                if (i > 0)
                    wprintf(L", ");
                DumpType(ifaces->Item(i));
            }
        }

        wprintf(L"\n");
        PrintIndent(indent);
        wprintf(L"{\n");

        DumpChildren(sym->asPARENTSYM(), indent);

        PrintIndent(indent);
        wprintf(L"}\n\n");
        break;

    case SK_MEMBVARSYM:
        PrintIndent(indent);
        DumpAccess(sym->asMEMBVARSYM()->GetAccess());

        if (sym->asMEMBVARSYM()->isConst)
            wprintf(L"const ");
        else {
            if (sym->asMEMBVARSYM()->isStatic)
                wprintf(L"static ");
            if (sym->asMEMBVARSYM()->isReadOnly)
                wprintf(L"readonly ");
        }        

        if (sym->asMEMBVARSYM()->fixedAgg) {
            wprintf(L"fixed ");
            DumpType(sym->asVARSYM()->type->asPTRSYM()->baseType());
        } else {
            DumpType(sym->asVARSYM()->type);
        }

        wprintf(L" %ls", sym->asVARSYM()->name->text);

        if (sym->asMEMBVARSYM()->isConst && !sym->asVARSYM()->type->isERRORSYM()) {
            wprintf(L" = ");
            DumpConst(sym->asVARSYM()->type, & sym->asMEMBVARSYM()->constVal);
        } else if (sym->asMEMBVARSYM()->fixedAgg) {
            wprintf(L"[");
            AGGSYM *aggInt = GetReqPredefAgg(PT_INT);
            if (aggInt && aggInt->getThisType())
                DumpConst(aggInt->getThisType(), & sym->asMEMBVARSYM()->constVal);
            wprintf(L"]");
        }

        wprintf(L";");

        if (sym->asMEMBVARSYM()->getBogus())
            wprintf(L"  /* UNSUPPORTED or BOGUS */");

        wprintf(L"\n");
        break;

    case SK_TYVARSYM:
        PrintIndent(indent);
        wprintf(L"%ls ", sym->name->text);
        if (sym->asTYVARSYM() != NULL) {
            wprintf(L": (");
            TypeArray * bnds = sym->asTYVARSYM()->GetBnds();
            if (bnds) {
                for (int i = 0; i < bnds->size; i++) {
                    if (i > 0)
                        wprintf(L", ");
                    DumpType(bnds->Item(i));
                }
            }
            wprintf(L")");
        }
        break;

    case SK_AGGTYPESYM:
        PrintIndent(indent);
        DumpType(sym->asAGGTYPESYM());
        wprintf(L"<...>\n");
        break;

    case SK_LOCVARSYM:
        PrintIndent(indent);
        DumpType(sym->asVARSYM()->type);
        wprintf(L" %ls;\n", sym->asVARSYM()->name->text);
        break;

    case SK_METHSYM:
        PrintIndent(indent);
        DumpAccess(sym->asMETHSYM()->GetAccess());

        if (sym->asMETHSYM()->isStatic)
            wprintf(L"static ");
        if (sym->asMETHSYM()->isAbstract)
            wprintf(L"abstract ");
        if (sym->asMETHSYM()->isVirtual)
            wprintf(L"virtual ");

        if (sym->asMETHSYM()->isCtor()) {
            wprintf(L"%ls", sym->asMETHSYM()->getClass()->name->text);
        }
        else {
            DumpType(sym->asMETHSYM()->retType);
            wprintf(L" %ls", sym->asMETHSYM()->name ? sym->asMETHSYM()->name->text : L"impl");
        }
        if (sym->asMETHSYM()->typeVars->size)
            wprintf(L"<...>");
        wprintf(L"(");

        for (i = 0; i < sym->asMETHSYM()->params->size; ++i) {
            if (i != 0)
                wprintf(L", ");
            DumpType(sym->asMETHSYM()->params->Item(i));
        }
        wprintf(L");");

        if (sym->asMETHSYM()->getBogus())
            wprintf(L"  /* UNSUPPORTED or BOGUS */");
        wprintf(L"\n");

        break;

    case SK_PROPSYM:
        PrintIndent(indent);
        DumpAccess(sym->asPROPSYM()->GetAccess());

        if (sym->asPROPSYM()->isStatic)
            wprintf(L"static ");

        DumpType(sym->asPROPSYM()->retType);
        wprintf(L" %ls", sym->name ? sym->name->text : L"impl");

        if (sym->asPROPSYM()->params->size > 0)
            wprintf(L"[");

        for (i = 0; i < sym->asPROPSYM()->params->size; ++i) {
            if (i != 0)
                wprintf(L", ");
            DumpType(sym->asPROPSYM()->params->Item(i));
        }
        if (sym->asPROPSYM()->params->size > 0)
            wprintf(L"]");

        if (sym->asPROPSYM()->getBogus())
            wprintf(L"  /* UNSUPPORTED or BOGUS */");
        wprintf(L"\n");

        PrintIndent(indent);
        wprintf(L"{ ");
        if (sym->asPROPSYM()->methGet)
            wprintf(L"get { %ls } ", sym->asPROPSYM()->methGet->name ? sym->asPROPSYM()->methGet->name->text : L"getter");
        if (sym->asPROPSYM()->methSet)
            wprintf(L"set { %ls } ", sym->asPROPSYM()->methSet->name ? sym->asPROPSYM()->methSet->name->text : L"setter");
        wprintf(L"}\n");

        break;

    case SK_ERRORSYM:
        wprintf(L"*error*");
        break;

    case SK_INFILESYM:
        if (sym->asINFILESYM()->isSource)
            wprintf(L"source file %ls", sym->asINFILESYM()->name->text);
        else
            wprintf(L"metadata file %ls", sym->asINFILESYM()->name->text);
        break;

    case SK_SCOPESYM:
        PrintIndent(indent);
        wprintf(L"scope %p {\n", sym);
        DumpChildren(sym->asPARENTSYM(), indent);
        PrintIndent(indent);
        wprintf(L"}\n");
        break;

    case SK_ANONSCOPESYM:
        PrintIndent(indent);
        wprintf(L"anon-scope %p\n", sym);
        DumpSymbol(sym->asANONSCOPESYM()->scope, indent);
        break;

    case SK_CACHESYM:
    case SK_LABELSYM:
    case SK_OUTFILESYM:
    default:
        ASSERT(0);
        break;
    }
}

/* 
 * Dump a text representation of a type to stdout.
 */

void BSYMMGR::DumpType(PTYPESYM sym)
{
    int i, rank;

    if (sym == NULL) {
        wprintf(L"*NULL*");
        return;
    }

    switch (sym->getKind()) {
    case SK_ARRAYSYM:
    {
        // Brackets go the reverse way that would be logical, so we need
        // to get down to the first non-array element type.
        PTYPESYM elementType = sym;
        while (elementType->isARRAYSYM())
            elementType = elementType->asARRAYSYM()->elementType();
        DumpType(elementType);

        do {
            rank = sym->asARRAYSYM()->rank;

            if (rank == 0) {
                wprintf(L"[?]");
            }
            else if (rank == 1)
                wprintf(L"[]");
            else
            {
                // known rank > 1
                wprintf(L"[*");
                for (i = rank; i > 1; --i) {
                    wprintf(L",*");
                }
                wprintf(L"]");
            }
        } while ((sym = sym->asARRAYSYM()->elementType())->isARRAYSYM());
        break;
    }

    case SK_PTRSYM:
        DumpType(sym->asPTRSYM()->baseType());
        wprintf(L" *");
        break;

    case SK_NUBSYM:
        DumpType(sym->asNUBSYM()->baseType());
        wprintf(L" ?");
        break;

    case SK_PARAMMODSYM:
        if (sym->asPARAMMODSYM()->isOut)
            wprintf(L"out ");
        else
            wprintf(L"ref ");
        DumpType(sym->asPARAMMODSYM()->paramType());
        break;

    case SK_VOIDSYM:
        wprintf(L"void");
        break;

    case SK_NULLSYM:
        wprintf(L"null");
        break;

    case SK_UNITSYM:
        wprintf(L"unit");
        break;

    case SK_ANONMETHSYM:
        wprintf(L"<anonymous method>");
        break;

    case SK_METHGRPSYM:
        wprintf(L"<method group>");
        break;

    case SK_TYVARSYM:
        wprintf(L"%ls", sym->name->text);
        break;

    case SK_AGGTYPESYM:
        if (sym->asAGGTYPESYM()->isPredefined()) {
            PREDEFTYPE pt = sym->asAGGTYPESYM()->getPredefType();
            PCWCH psz = GetNiceName(pt);
            if (psz) {
                wprintf(L"%ls", psz);
                break;
            }
        }
        wprintf(L"%ls", sym->getAggregate()->name->text);

        if (sym->asAGGTYPESYM()->typeArgsThis->size > 0)
            wprintf(L"{inst-type}");
        break;

    case SK_ERRORSYM:
        wprintf(L"*error*");
        break;

    default:
        ASSERT(0);
        break;
    }




}


PINFILESYM SYMMGR::GetPredefInfile()
{
    if (!predefInputFile) {
        // Create a bogus inputfile
        // we use the bogus assemly id 0

        predefInputFile = CreateSourceFile(L"", mdfileroot);
        predefInputFile->rootDeclaration = CreateNamespaceDecl(
                    rootNS, 
                    NULL,                               // no declaration parent
                    predefInputFile, 
                    NULL);                              // no parse tree
    }
    return predefInputFile;
}

#endif //DEBUG


PSYM LSYMMGR::LookupNextSym(PSYM symPrev, PPARENTSYM parent, symbmask_t kindmask) {
    return BSYMMGR::LookupNextSym(symPrev, parent, kindmask);
};

__forceinline void LSYMMGR::AddChild(SYMTBL *tabl, PPARENTSYM parent, PSYM child) {
    return BSYMMGR::AddChild(tabl, parent, child);
};



__forceinline PSYM LSYMMGR::AllocSym(SYMKIND symkind, PNAME name, NRHEAP * allocator, int * psidLast) {
    return BSYMMGR::AllocSymWorker(symkind, name, allocator, psidLast);
};

__forceinline void LSYMMGR::AddToSymList(NRHEAP *heap, PSYM sym, PSYMLIST * * symLink) {
    return BSYMMGR::AddToSymList(heap, sym, symLink);
};


__forceinline void LSYMMGR::AddToNameList(NRHEAP *heap, PNAME name, PNAMELIST * * nameLink) {
    return BSYMMGR::AddToNameList(heap, name, nameLink);
};



MISCSYM * BSYMMGR::LookupNextGlobalMiscSym(MISCSYM * prev, NAME * name, PARENTSYM * parent, MISCSYM::TYPE type)
{
    if (prev) {
AGAIN:
        prev = LookupNextSym(prev, parent, MASK_MISCSYM)->asMISCSYM();
    } else {
        prev = LookupGlobalSymCore(name, parent, MASK_MISCSYM)->asMISCSYM();
    }
    if (prev) {
        if (prev->miscKind == type) {
            return prev;
        }
        goto AGAIN;
    }
    return NULL;
}


PREDEFATTR BSYMMGR::GetPredefAttr(AGGTYPESYM * type)
{
    AGGSYM * agg = type->getAggregate();
    if (!agg->isPredefined)
        return PA_COUNT;
    return predefTypeInfo[agg->iPredef].attr;
}


