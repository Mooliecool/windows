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
// File: typebind.cpp
//
// Type lookup and binding.
// ===========================================================================

#include "stdafx.h"

/***************************************************************************************************
    Initialize the TypeBind instance.
***************************************************************************************************/
void TypeBind::Init(COMPILER * cmp, SYM * symCtx, SYM * symStart, SYM * symAccess, SYM * symTypeVars, TypeBindFlagsEnum flags)
{
    // symAccess may be NULL (indicating that only publics are accessible).
    ASSERT(cmp && symCtx && symStart);
    ASSERT(!symTypeVars || symTypeVars->isMETHSYM() || symTypeVars->isAGGSYM());

    m_cmp = cmp;
    m_symCtx = symCtx;
    m_symStart = symStart;
    m_symAccess = symAccess;
    m_symTypeVars = symTypeVars;
    m_flags = flags;
    m_fUsingAlias = false;
    DebugOnly(m_fValid = true);

    // In define members or before, we shouldn't check deprecated.
    if (cmp->AggStateMax() < AggState::Prepared)
        m_flags = m_flags | TypeBindFlags::NoDeprecated;
}


/***************************************************************************************************
    Bind a dotted name, a single name (generic or not), or an alias qualified name.
***************************************************************************************************/
SYM * TypeBind::BindNameCore(BASENODE * node)
{
    ASSERT(m_fValid);
    ASSERT(node);

    switch (node->kind) {
    case NK_DOT:
        return BindDottedNameCore(node->asANYBINOP(), node->asANYBINOP()->p2->asANYNAME()->pName);
    case NK_NAME:
    case NK_GENERICNAME:
    case NK_OPENNAME:
        return BindSingleNameCore(node->asANYNAME(), node->asANYNAME()->pName);
    case NK_ALIASNAME:
        return BindAliasNameCore(node->asALIASNAME());
    default:
        VSFAIL("Bad node kind in BindNameCore");
        return NULL;
    }
}


/***************************************************************************************************
    Bind a dotted name or a single name (generic or not). This is only called by
    BindAttributeTypeCore so it doesn't need to deal with OPENNAME and ALIASNAME.
***************************************************************************************************/
SYM * TypeBind::BindNameCore(BASENODE * node, NAME * name)
{
    ASSERT(m_fValid);
    ASSERT(name);

    switch (node->kind) {
    case NK_DOT:
        return BindDottedNameCore(node->asANYBINOP(), name);
    case NK_NAME:
    case NK_GENERICNAME:
        return BindSingleNameCore(node->asANYNAME(), name);
    default:
        VSFAIL("Bad node kind in BindNameCore");
        return NULL;
    }
}


/***************************************************************************************************
    Bind a dotted name, a single name (generic or not), or an alias qualified name (A::B<...>) to
    a type.
***************************************************************************************************/
TYPESYM * TypeBind::BindNameToTypeCore(BASENODE * node)
{
    ASSERT(m_fValid);
    ASSERT(node);

    SYM * sym;

    switch (node->kind) {
    case NK_DOT:
        sym = BindDottedNameCore(node->asANYBINOP(), node->asANYBINOP()->p2->asANYNAME()->pName);

        if (sym != NULL && 
            sym->isTYPESYM() && 
            !sym->isERRORSYM() &&
            compiler()->compileCallback.CheckForNameSimplification())
        {
            if (sym->asTYPESYM()->isPredefined() && predefTypeInfo[sym->asTYPESYM()->getPredefType()].niceName != NULL)
            {
                compiler()->compileCallback.CanSimplifyNameToPredefinedType(m_symCtx->getInputFile()->pData, node, sym->asTYPESYM());
            }
            else
            {
                NAMENODE* pLastName = node->asANYBINOP()->p2->asANYNAME();
                if (sym == BindSingleNameCore(pLastName, pLastName->pName))
                { 
                    compiler()->compileCallback.CanSimplifyName(m_symCtx->getInputFile()->pData, node->asDOT());
                }
            }
        }
        break;
    case NK_NAME:
    case NK_OPENNAME:
    case NK_GENERICNAME:
        sym = BindSingleNameCore(node->asANYNAME(), node->asANYNAME()->pName);

        if (sym != NULL && 
            sym->isTYPESYM() && 
            !sym->isERRORSYM() &&
            sym->asTYPESYM()->isPredefined() &&
            compiler()->compileCallback.CheckForNameSimplification())
        {
            compiler()->compileCallback.CanSimplifyNameToPredefinedType(m_symCtx->getInputFile()->pData, node, sym->asTYPESYM());
        }

        break;
    case NK_ALIASNAME:
        sym = BindAliasNameCore(node->asALIASNAME());
        break;
    default:
        VSFAIL("Bad node kind in BindNameToTypeCore");
        return NULL;
    }

    ASSERT(sym || FAllowMissing());
    if (!sym || sym->isTYPESYM())
        return sym->asTYPESYM();

    ASSERT(sym->isNSAIDSYM());
    if (!FSuppressErrors())
        compiler()->Error(node, ERR_BadSKknown, sym, ErrArgSymKind(sym), SK_AGGSYM);

    // Construct the error sym.
    switch (node->kind) {
    default:
        VSFAIL("Unexpected node kind in BindNameToTypeCore");
    case NK_DOT:
        break;

    case NK_NAME:
    case NK_ALIASNAME:
        return compiler()->getBSymmgr().GetErrorType(NULL, node->asSingleName()->pName, NULL);
    }

    // Construct the parent NSAIDSYM.
    NSAIDSYM * nsa = sym->asNSAIDSYM();
    ASSERT(nsa->GetNS()->Parent());
    NSAIDSYM * nsaPar = compiler()->getBSymmgr().GetNsAid(nsa->GetNS()->Parent(), nsa->GetAid());

    return compiler()->getBSymmgr().GetErrorType(nsaPar, nsa->GetNS()->name, NULL);
}


/***************************************************************************************************
    Bind a single name (generic or not).
***************************************************************************************************/
SYM * TypeBind::BindSingleNameCore(NAMENODE * node, NAME * name, TypeArray * typeArgs)
{
    ASSERT(m_fValid);
    ASSERT(node);

    SYM * sym;

    if (!typeArgs)
        typeArgs = BindTypeArgsCore(node);
    ASSERT(typeArgs);

    ClearErrorInfo();

    if (m_fUsingAlias) {
        // We're binding a using alias.
        ASSERT(m_symStart->isNSDECLSYM());
        ASSERT(!m_symTypeVars);

        NSDECLSYM * nsd = m_symStart->asNSDECLSYM();
        sym = SearchNamespacesCore(nsd, node, name, typeArgs);
    }
    else {
        sym = NULL;
        SYM * symSearch = m_symStart;
        SYM * symTypeVars = m_symTypeVars;

        if ((m_flags & TypeBindFlags::CallPreHook) && (sym = PreLookup(name, typeArgs)) != NULL)
            goto LDone;

        // Search the type variables in the method
        if (symSearch->isMETHSYM()) {
            ASSERT(!symTypeVars || symTypeVars == symSearch);
            symTypeVars = symSearch;
            symSearch = symSearch->asMETHSYM()->declaration;
        }


        // check for type parameters while resolving base types, where clauses, XML comments, etc.
        if (symTypeVars && (sym = SearchTypeVarsCore(symTypeVars, name, typeArgs)) != NULL)
            goto LDone;

        // search class
        if (symSearch->isAGGDECLSYM()) {
            AGGSYM * agg = symSearch->asAGGDECLSYM()->Agg();
            // Include outer types and type vars.
            if ((sym = SearchClassCore(agg->getThisType(), name, typeArgs, true)) != NULL)
                goto LDone;
        }

        while (!symSearch->isNSDECLSYM()) {
            symSearch = symSearch->containingDeclaration();
        }
        sym = SearchNamespacesCore(symSearch->asNSDECLSYM(), node, name, typeArgs);
    }

LDone:
    ReportErrors(node, name, NULL, typeArgs, &sym);

    return sym;
}


/***************************************************************************************************
    Bind the alias name. Returns an NSAIDSYM or AGGTYPESYM (the latter only after reporting an
    error). Doesn't call ClearErrorInfo or ReportErrors.
***************************************************************************************************/
SYM * TypeBind::BindAliasNameCore(NAMENODE * node)
{
    ASSERT(m_fValid);
    ASSERT(node->kind == NK_ALIASNAME && node->pName);

    SYM * sym;
    NAME * name = node->pName;

    if (node->flags & NF_GLOBAL_QUALIFIER) {
        sym = compiler()->getBSymmgr().GetGlobalNsAid();
        return sym;
    }

    SYM * symSearch = m_symStart;

    while (!symSearch->isNSDECLSYM())
        symSearch = symSearch->containingDeclaration();

    for (NSDECLSYM * nsd = symSearch->asNSDECLSYM(); nsd; nsd = nsd->DeclPar()) {
        // Check the using aliases.
        compiler()->clsDeclRec.ensureUsingClausesAreResolved(nsd);

        FOREACHSYMLIST(nsd->usingClauses, symUse, SYM)
            if (symUse->name != name || !symUse->isALIASSYM() || !FAliasAvailable(symUse->asALIASSYM()))
                continue;

            sym = BindUsingAlias(compiler(), symUse->asALIASSYM());
            if (!sym) 
                continue;

            //Found something within this using.  it is not "unused"
            compiler()->compileCallback.BoundToUsing(nsd, symUse);

            switch (sym->getKind()) {
            case SK_ERRORSYM:
                return sym;
            case SK_NSAIDSYM:
                return sym;
            case SK_AGGTYPESYM:
                // Should have used . not ::
                if (FSuppressErrors())
                    return compiler()->getBSymmgr().GetErrorType(NULL, name, NULL);
                // Can't use type aliases with ::.
                compiler()->ErrorRef(node, ERR_ColColWithTypeAlias, symUse->asALIASSYM());
                return sym;
            default:
                VSFAIL("Alias resolved to bad kind");
                break;
            }
        ENDFOREACHSYMLIST
    }

    if (FAllowMissing())
        return NULL;

    if (!FSuppressErrors())
        compiler()->Error(node, ERR_AliasNotFound, name);

    sym = compiler()->getBSymmgr().GetErrorType(NULL, name, NULL);

    return sym;
}


/***************************************************************************************************
    Bind the type arguments on a generic name.
***************************************************************************************************/
TypeArray * TypeBind::BindTypeArgsCore(NAMENODE * node)
{
    ASSERT(m_fValid);

    int ctype;
    bool fUnit;

    switch (node->kind) {
    default:
        return BSYMMGR::EmptyTypeArray();

    case NK_GENERICNAME:
        ctype = CountListNode(node->asGENERICNAME()->pParams);
        fUnit = false;
        break;

    case NK_OPENNAME:
        ctype = node->asOPENNAME()->carg;
        fUnit = true;
        break;
    }

    ASSERT(ctype > 0);
    TYPESYM ** prgtype = STACK_ALLOC(TYPESYM *, ctype);

    if (fUnit) {
        TYPESYM * typeUnit = compiler()->getBSymmgr().GetUnitType();
        for (int itype = 0; itype < ctype; itype++)
            prgtype[itype] = typeUnit;
    }
    else {
        // Turn off AllowMissing when binding type parameters.
        TypeBindFlagsEnum flagsOrig = m_flags;
        m_flags = m_flags & ~TypeBindFlags::AllowMissing;

        int itype = 0;
        NODELOOP(node->asGENERICNAME()->pParams, TYPEBASE, arg)
            // bind the type, and wrap it if the variable is byref
            TYPESYM * type = BindTypeCore(arg);
            ASSERT(type);
            prgtype[itype++] = type;
        ENDLOOP;
        ASSERT(itype == ctype);

        m_flags = flagsOrig;
    }

    // get the formal (and unique) param array for the given types...
    return compiler()->getBSymmgr().AllocParams(ctype, prgtype);
}


/***************************************************************************************************
    Bind a dotted name.
***************************************************************************************************/
SYM * TypeBind::BindDottedNameCore(BINOPNODE * node, NAME * name)
{
    ASSERT(m_fValid);
    ASSERT(node && node->kind == NK_DOT);

    SYM * symLeft = BindNameCore(node->p1);
    if (!symLeft)
        return NULL;

    return BindRightSideCore(symLeft, node, node->p2->asANYNAME(), name);
}


/***************************************************************************************************
    Bind the right identifier of a dotted name.
***************************************************************************************************/
SYM * TypeBind::BindRightSideCore(SYM * symLeft, BASENODE * nodePar, NAMENODE * nodeName, NAME * name, TypeArray * typeArgs)
{
    ASSERT(m_fValid);

    if (!typeArgs)
        typeArgs = BindTypeArgsCore(nodeName);
    ASSERT(typeArgs);

    ClearErrorInfo();

    SYM * sym;

    switch (symLeft->getKind()) {
    case SK_ERRORSYM:
        sym = compiler()->getBSymmgr().GetErrorType(symLeft->asERRORSYM(), name, typeArgs);
        break;

    case SK_NUBSYM:
        symLeft = symLeft->asNUBSYM()->GetAts();
        // The parser shouldn't allo T?.x - only Nullable<T>.x - so we know Nullable
        // exists.
        ASSERT(symLeft);
        ASSERT(symLeft->isAGGTYPESYM());
        // Fall through.
    case SK_AGGTYPESYM:
        {
            AGGTYPESYM * ats = symLeft->asAGGTYPESYM();
            sym = SearchClassCore(ats, name, typeArgs, false); // no outer and no type vars
        }
        break;

    case SK_NSAIDSYM:
        sym = SearchSingleNamespaceCore(symLeft->asNSAIDSYM(), nodeName, name, typeArgs);
        break;

    case SK_TYVARSYM:
        // Can't lookup in a type variable.
        if (!FSuppressErrors())
            compiler()->Error(nodePar, ERR_LookupInTypeVariable, symLeft);
        // Note: don't fall through and report more errors 
        sym = compiler()->getBSymmgr().GetErrorType(symLeft->asPARENTSYM(), name, typeArgs);
        return sym;

    default:
        VSFAIL("Bad symbol kind in BindRightSideCore");
        return NULL;
    }

    ReportErrors(nodeName, name, symLeft, typeArgs, &sym);

    return sym;
}


/***************************************************************************************************
    Bind a TYPEBASENODE to a type.
***************************************************************************************************/
TYPESYM * TypeBind::BindTypeCore(TYPEBASENODE * node)
{
    ASSERT(m_fValid);
    TYPESYM * type;

    switch (node->kind) {
    case NK_PREDEFINEDTYPE:
        if (node->asPREDEFINEDTYPE()->iType == PT_VOID)
            type = compiler()->getBSymmgr().GetVoid();
        else {
            PREDEFTYPE pt = (PREDEFTYPE)node->asPREDEFINEDTYPE()->iType;
            type = compiler()->GetOptPredefType(pt, false);
            if (!type) {
                // If decimal is missing we'll get here. Use the nice name in the error symbol.
                if (!FSuppressErrors())
                    compiler()->getBSymmgr().ReportMissingPredefTypeError(pt);
                NAME * name = compiler()->namemgr->LookupString(compiler()->getBSymmgr().GetNiceName(pt));
                type = compiler()->getBSymmgr().GetErrorType(NULL, name, NULL);
            }
        }
        break;

    case NK_ARRAYTYPE: 
        {
            TYPESYM * typeElem = BindTypeCore(node->asARRAYTYPE()->pElementType);
            if (!typeElem)
                return NULL;
            if (!FSuppressErrors()) {
                if (typeElem->isSpecialByRefType())
                    compiler()->Error(node, ERR_ArrayElementCantBeRefAny, typeElem);
                if (compiler()->AggStateMax() >= AggState::DefinedMembers)
                    compiler()->CheckForStaticClass(node, NULL, typeElem, ERR_ArrayOfStaticClass);
            }

            int rank = node->asARRAYTYPE()->iDims;
            ASSERT(rank > 0);
            type = compiler()->getBSymmgr().GetArray(typeElem, rank);
        }
        break;

    case NK_NAMEDTYPE: 
        type = BindNameToTypeCore(node->asNAMEDTYPE()->pName);
        break;

    case NK_OPENTYPE:
        type = BindNameToTypeCore(node->asOPENTYPE()->pName);
        ASSERT(!type || type->isERRORSYM() || type->isAGGTYPESYM() && type->asAGGTYPESYM()->typeArgsAll->size > 0);
        break;

    case NK_POINTERTYPE:
        {
            TYPESYM * typeInner = BindTypeCore(node->asPOINTERTYPE()->pElementType);
            if (!typeInner)
                return NULL;
            type = compiler()->getBSymmgr().GetPtrType(typeInner);
            if (!typeInner->isPTRSYM())
                compiler()->clsDeclRec.checkUnmanaged(node, type);
        }
        break;

    case NK_NULLABLETYPE:
        {
            TYPESYM * typeInner = BindTypeCore(node->asNULLABLETYPE()->pElementType);
            if (!typeInner)
                return NULL;

            // Convert to NUBSYM.
            type = compiler()->getBSymmgr().GetNubTypeOrError(typeInner);
            if (type->isNUBSYM() && compiler()->CompPhase() >= CompilerPhase::EvalConstants)
                CheckConstraints(compiler(), node, type->asNUBSYM()->GetAts(), CheckConstraintsFlags::None);
        }
        break;

    case NK_TYPEWITHATTR:
        compiler()->Error(node, ERR_AttrOnTypeArg);
        type = BindTypeCore(node->asTYPEWITHATTR()->pType);
        break;

    default:
        VSFAIL("BAD type node kind");
        return NULL;
    }

    return type;
}


/***************************************************************************************************
    Search a class for the name (as a nested type).

    If fOuterAndTypeVars is true, this searches first for type variables, then for nested types in
    this class and its base classes, then for nested types in outer types and base classes of the
    outer types.

    If fOuterAndTypeVars is false, this searches for nested types just in this type and its
    base classes.
***************************************************************************************************/
SYM * TypeBind::SearchClassCore(AGGTYPESYM * ats, NAME * name, TypeArray * typeArgs, bool fOuterAndTypeVars)
{
    ASSERT(m_fValid);
    ASSERT(typeArgs);

    // Check this class and all classes that this class is nested in.
    for ( ; ats; ats = ats->outerType) {
        AGGSYM * agg = ats->getAggregate();

        if (agg->AggState() < AggState::Prepared) {
            if (compiler()->AggStateMax() >= AggState::Prepared)
                compiler()->EnsureState(agg);
            else if (agg->AggState() < AggState::Inheritance) {
                // Note: we cannot assert that this succeeds. If it fails, we're in ResolveInheritanceRec
                // and it will be detected by ResolveInheritanceRec (eventually).
                compiler()->clsDeclRec.ResolveInheritanceRec(agg);
            }
        }

        if (fOuterAndTypeVars) {
            SYM * sym = SearchTypeVarsCore(ats->getAggregate(), name, typeArgs);
            if (sym)
                return sym;
        }

        // Check this class and all base classes.
        for (AGGTYPESYM * atsCur = ats; atsCur; atsCur = atsCur->GetBaseClass()) {
            AGGSYM * aggCur = atsCur->getAggregate();
            for (SYM * sym = compiler()->getBSymmgr().LookupAggMember(name, aggCur, MASK_ALL); sym; sym = sym->nextSameName) {
                if (sym->isAGGSYM()) {
                    // Arity must be checked before access!
                    SYM * symRet = sym;
                    CheckArity(&symRet, typeArgs, atsCur);
                    CheckAccess(&symRet);
                    if (symRet)
                        return symRet;
                }
                else if (!m_symBadKind && !sym->isTYVARSYM()) {
                    m_symBadKind = sym;
                }
            }
        }

        if (!fOuterAndTypeVars)
            return NULL;
    }

    return NULL;
}


/***************************************************************************************************
    Searches for the name among the type variables of symOwner.
***************************************************************************************************/
TYVARSYM * TypeBind::SearchTypeVarsCore(SYM * symOwner, NAME * name, TypeArray * typeArgs)
{
    ASSERT(m_fValid);
    ASSERT(symOwner->isMETHSYM() || symOwner->isAGGSYM());
    ASSERT(typeArgs);

    TYVARSYM * var = compiler()->LookupGlobalSym(name, symOwner->asPARENTSYM(), MASK_TYVARSYM)->asTYVARSYM();
    if (var)
        CheckArity((SYM **)&var, typeArgs, NULL);

    return var;
}


/***************************************************************************************************
    Searches for the name among the members of a single (filtered) namespace. Returns either an
    NSAIDSYM, AGGTYPESYM or ERRORSYM.
***************************************************************************************************/
SYM * TypeBind::SearchSingleNamespaceCore(NSAIDSYM * nsa, BASENODE * node, NAME * name, TypeArray * typeArgs, bool fAggOnly)
{
    SYM * sym = SearchSingleNamespaceCore(nsa->GetNS(), nsa->GetAid(), node, name, typeArgs, fAggOnly);
    if (!sym || sym->isERRORSYM())
        return sym;
    if (sym->isNSSYM())
        return compiler()->getBSymmgr().GetNsAid(sym->asNSSYM(), nsa->GetAid());
    // Translate "Nullable<T>" to "T?".
    if (sym->asAGGTYPESYM()->isPredefType(PT_G_OPTIONAL) &&
        !sym->asAGGTYPESYM()->typeArgsAll->Item(0)->isUNITSYM())
    {
        return compiler()->getBSymmgr().GetNubFromNullable(sym->asAGGTYPESYM());
    }
    return sym->asAGGTYPESYM();
}


enum {
    krankNone,
    krankTypeImp,       // Public type in an imported assembly.
    krankNamespaceImp,  // Namespace used in imported assemblies.
    krankNamespaceThis, // Namespace used in this assembly.
    krankTypeThis,      // Type in source or an added module (this assembly).
};


/***************************************************************************************************
    Private method for SearchSingleNamespaceCore. Determines what kind of SYM we're dealing with
    for ambiguity error / warning reporting. Returns a value from the enum above.
    Note that this is only called on NSSYMs and AGGTYPESYMs where the AGGSYM is NOT nested.
***************************************************************************************************/
int TypeBind::RankSym(SYM * sym)
{
    ASSERT(sym->isNSSYM() || sym->isAGGTYPESYM());

    if (sym->isNSSYM()) {
        if (sym->asNSSYM()->InAlias(compiler(), kaidThisAssembly))
            return krankNamespaceThis;
        return krankNamespaceImp;
    }

    AGGSYM * agg = sym->asAGGTYPESYM()->getAggregate();
    ASSERT(!agg->isNested());

    if (agg->isSource || agg->InAlias(kaidThisAssembly))
        return krankTypeThis;

    // CheckAccess should have dealt with private types already.
    ASSERT(agg->GetAccess() > ACC_INTERNAL || agg->GetAccess() == ACC_INTERNAL && agg->InternalsVisibleTo(kaidThisAssembly));
    return krankTypeImp;
}


/***************************************************************************************************
    Searches for the name among the members of a single (filtered) namespace. Returns either an
    NSSYM (not NSAIDSYM) or AGGTYPESYM.
***************************************************************************************************/
SYM * TypeBind::SearchSingleNamespaceCore(NSSYM * ns, int aid, BASENODE * node, NAME * name, TypeArray * typeArgs, bool fAggOnly)
{
    ASSERT(m_fValid);
    ASSERT(typeArgs);

    symbmask_t mask = fAggOnly ? MASK_AGGSYM : MASK_NSSYM | MASK_AGGSYM;

    SYM * symBest = NULL;
    int rankBest = krankNone;
    SYM * symAmbig = NULL;
    int rankAmbig = krankNone;

    for (BAGSYM * bag = compiler()->LookupInBagAid(name, ns, aid, mask)->asBAGSYM();
        bag;
        bag = compiler()->LookupNextInAid(bag, aid, mask)->asBAGSYM())
    {
        SYM * symNew = bag;

        // Arity must be checked before access!
        CheckArity(&symNew, typeArgs, NULL);
        CheckAccess(&symNew);

        if (!symNew)
            continue;

        ASSERT(symNew->isNSSYM() || symNew->isAGGTYPESYM());

        // If it's ambiguous, at least one of them should be a type.
        ASSERT(!symBest || symBest->isAGGTYPESYM() || symNew->isAGGTYPESYM());

        int rankNew = RankSym(symNew);

        if (rankNew > rankBest) {
            symAmbig = symBest;
            rankAmbig = rankBest;
            symBest = symNew;
            rankBest = rankNew;
        }
        else if (rankNew > rankAmbig) {
            symAmbig = symNew;
            rankAmbig = rankNew;
        }
    }

    ASSERT(rankBest >= rankAmbig);

    if (!symAmbig)
        return symBest;

    ASSERT(symBest && rankBest == RankSym(symBest));
    ASSERT(symAmbig && rankAmbig == RankSym(symAmbig));

    // Handle errors and warnings.
    // NOTE: Representing (rankBest,rankAmbig) as a pair (x,y), the following should be true:
    // * If (x,y) is a warning then (z,y) must be a warning for any z >= x.
    // * If (x,y) is a warning then (x,z) must be a warning for any z <= y.

    int err;

    switch (rankAmbig) {
    case krankTypeImp:
        if (FSuppressErrors()) {
            if (rankBest >= krankNamespaceThis)
                return symBest;
            goto LRetErrorSym;
        }

        switch (rankBest) {
        case krankTypeImp:
            err = ERR_SameFullNameAggAgg;
            break;
        case krankNamespaceImp:
            err = ERR_SameFullNameNsAgg;
            break;
        case krankNamespaceThis:
            err = WRN_SameFullNameThisNsAgg;
            break;
        case krankTypeThis:
            err = WRN_SameFullNameThisAggAgg;
            break;
        default:
            VSFAIL("Bad rank");
            err = ERR_SameFullNameAggAgg;
            break;
        }
        break;

    case krankNamespaceImp:
        ASSERT(rankBest == krankTypeThis);
        if (FSuppressErrors())
            return symBest;
        err = WRN_SameFullNameThisAggNs;
        break;

    case krankNamespaceThis:
        ASSERT(rankBest == krankTypeThis);
        if (FSuppressErrors())
            goto LRetErrorSym;
        err = ERR_SameFullNameThisAggThisNs;
        break;

    case krankTypeThis:
        ASSERT(rankBest == krankTypeThis);
        if (FSuppressErrors()) {
LRetErrorSym:
            NSAIDSYM * nsa = compiler()->getBSymmgr().GetNsAid(ns, aid);
            return compiler()->getBSymmgr().GetErrorType(nsa, name, typeArgs);
        }
        compiler()->ErrorRef(node, ERR_SameFullNameThisAggThisAgg,
            ErrArgAggKind(symBest->asAGGTYPESYM()), symBest,
            ErrArgAggKind(symAmbig->asAGGTYPESYM()), symAmbig);
        return symBest;

    default:
        VSFAIL("Bad rank");
        err = ERR_SameFullNameAggAgg;
        break;
    }

    DECLSYM * declBest;
    DECLSYM * declAmbig;

    if (symBest->isAGGTYPESYM())
        declBest = (DECLSYM *)symBest->asAGGTYPESYM()->getAggregate()->DeclFirst();
    else {
        declBest = symBest->asNSSYM()->DeclFirst();
        if (rankBest == krankNamespaceThis) {
            while (!declBest->getInputFile()->InAlias(kaidThisAssembly) && declBest->DeclNext())
                declBest = declBest->DeclNext();
        }
    }

    if (symAmbig->isAGGTYPESYM())
        declAmbig = (DECLSYM *)symAmbig->asAGGTYPESYM()->getAggregate()->DeclFirst();
    else {
        declAmbig = symAmbig->asNSSYM()->DeclFirst();
        if (rankAmbig == krankNamespaceThis) {
            while (!declAmbig->getInputFile()->InAlias(kaidThisAssembly) && declAmbig->DeclNext())
                declAmbig = declAmbig->DeclNext();
        }
    }

    compiler()->ErrorRef(node, err,
        declBest->getInputFile()->name, declBest,
        declAmbig->getInputFile()->name, declAmbig);

    return symBest;
}


/***************************************************************************************************
    Searches for the name in the using aliases of the given namespace declaration. If there is an
    ambiguity and SuppressErrors is set, this returns ERRORSYM. If there is an ambiguity and
    SuppressErrors is not set, this reports an error and returns the best symbol found.
***************************************************************************************************/
SYM * TypeBind::SearchUsingAliasesCore(NSDECLSYM * nsd, BASENODE * node, NAME * name, TypeArray * typeArgs)
{
    ASSERT(m_fValid);
    ASSERT(typeArgs);
    ASSERT(!m_fUsingAlias || m_symStart->isNSDECLSYM());

    if (typeArgs->size && m_symBadArity)
        return NULL;

    compiler()->clsDeclRec.ensureUsingClausesAreResolved(nsd);

    FOREACHSYMLIST(nsd->usingClauses, symUse, SYM)
        if (symUse->name != name || !symUse->isALIASSYM() || !FAliasAvailable(symUse->asALIASSYM()))
            continue;

        ALIASSYM * alias = symUse->asALIASSYM();
        SYM * sym = BindUsingAlias(compiler(), alias);
        if (!sym)
            continue;

        if (typeArgs->size) {
            m_symBadArity = alias;
            return NULL;
        }

        //Found something within this using.  it is not "unused"
        compiler()->compileCallback.BoundToUsing(nsd, symUse);

        if (alias->symDup) {
            // Error - alias conflicts with other element of the namespace.
            if (FSuppressErrors())
                return compiler()->getBSymmgr().GetErrorType(NULL, name, NULL);
            compiler()->Error(node, ERR_ConflictAliasAndMember, name, alias->symDup->parent, ErrArgRefOnly(alias->symDup));
        }
        return sym;
    ENDFOREACHSYMLIST

    return NULL;
}


/***************************************************************************************************
    Searches for the name in the using clauses of the given namespace declaration. Checks only
    normal using clauses - not aliases. If there is an ambiguity and SuppressErrors is set, this
    returns ERRORSYM. If there is an ambiguity and SuppressErrors is not set, this reports an error
    and returns the best symbol found.
***************************************************************************************************/
SYM * TypeBind::SearchUsingClausesCore(NSDECLSYM * nsd, BASENODE * node, NAME * name, TypeArray * typeArgs)
{
    ASSERT(m_fValid);
    ASSERT(typeArgs);
    ASSERT(!m_fUsingAlias || m_symStart->isNSDECLSYM());

    // Don't check using namespaces when binding aliases in this decl.
    if (m_fUsingAlias && m_symStart == nsd)
        return NULL;

    compiler()->clsDeclRec.ensureUsingClausesAreResolved(nsd);

    SYM * symRet = NULL;

    FOREACHSYMLIST(nsd->usingClauses, symUse, SYM)
        if (!symUse->isNSAIDSYM())
            continue;

        // Only search for types.
        SYM * sym = SearchSingleNamespaceCore(symUse->asNSAIDSYM(), node, name, typeArgs, true);
        if (!sym)
            continue;

        //Found something within this using.  it is not "unused"
        compiler()->compileCallback.BoundToUsing(nsd, symUse);

        if (sym->isERRORSYM())
            return sym;

        // Check for ambiguity between different using namespaces.
        if (symRet) {
            if (FSuppressErrors())
                return compiler()->getBSymmgr().GetErrorType(NULL, name, NULL);

            // After reporting the error just run with the first one.
            compiler()->ErrorRef(node, ERR_AmbigContext, name, symRet, sym);
            return symRet;
        }

        symRet = sym;
    ENDFOREACHSYMLIST

    return symRet;
}


/******************************************************************************
    Searches for the name in a namespace declaration and containing
    declarations. This searches using clauses as well.
******************************************************************************/
SYM * TypeBind::SearchNamespacesCore(NSDECLSYM * nsd, BASENODE * node, NAME * name, TypeArray * typeArgs)
{
    ASSERT(m_fValid);
    ASSERT(typeArgs);

    for ( ; nsd; nsd = nsd->DeclPar()) {
        SYM * sym;

        // Check the using aliases and extern aliases.
        sym = SearchUsingAliasesCore(nsd, node, name, typeArgs);
        if (sym)
            return sym;

        // Check the namespace.
        sym = SearchSingleNamespaceCore(compiler()->getBSymmgr().GetNsAid(nsd->NameSpace(), kaidGlobal), node, name, typeArgs);
        if (sym)
            return sym;

        // Check the using clauses for this declaration.
        sym = SearchUsingClausesCore(nsd, node, name, typeArgs);
        if (sym)
            return sym;
    }

    return NULL;
}


/***************************************************************************************************
    Searches for an alias with the given name in a namespace declaration and containing
    declarations. Reports an error if not found.
***************************************************************************************************/
SYM * TypeBind::SearchNamespacesForAliasCore(NSDECLSYM * nsd, BASENODE * node, NAME * name)
{
    ASSERT(m_fValid);
    ASSERT(!m_fUsingAlias); // This doesn't check for extern vs regular.

    for ( ; nsd; nsd = nsd->DeclPar()) {
        // Check the using aliases.
        compiler()->clsDeclRec.ensureUsingClausesAreResolved(nsd);

        FOREACHSYMLIST(nsd->usingClauses, symUse, SYM)
            if (symUse->name != name || !symUse->isALIASSYM())
                continue;

            SYM * sym = BindUsingAlias(compiler(), symUse->asALIASSYM());
            if (sym) {

                //Found something within this using.  it is not "unused"
                compiler()->compileCallback.BoundToUsing(nsd, symUse);

                return sym;
            }
        ENDFOREACHSYMLIST
    }

    if (FAllowMissing())
        return NULL;

    if (!FSuppressErrors())
        compiler()->Error(node, ERR_AliasNotFound, name);

    return compiler()->getBSymmgr().GetErrorType(NULL, name, NULL);
}


/***************************************************************************************************
    Check access and bogosity of the given symbol. Sets *psym to NULL if it's not accessible or
    is bogus. Updates m_symInaccess and m_symBogus as appropriate.

    Generally this should be called after CheckArity so only SYMs with the correct arity are put
    in m_symInaccess and m_symBogus. ReportErrors may return m_symInaccess or m_symBogus after
    reporting the errors!
***************************************************************************************************/
void TypeBind::CheckAccess(SYM ** psym)
{
    ASSERT(m_fValid);
    ASSERT(psym);

    SYM * sym = *psym;

    if (!sym || sym->isNSSYM())
        return;

    ASSERT(sym->isAGGTYPESYM());

    AGGTYPESYM * atsCheck = NULL;

    // CLSDREC::CheckAccess wants an AGGSYM and containing AGGTYPESYM.
    if (sym->isAGGTYPESYM()) {
        atsCheck = sym->asAGGTYPESYM();
        sym = atsCheck->getAggregate();
        atsCheck = atsCheck->outerType;
    }

    if (!compiler()->clsDeclRec.CheckAccess(sym, atsCheck, m_symAccess, NULL)) {
        if (!m_symInaccess)
            m_symInaccess = *psym; // Set the original.
        *psym = NULL;
        return;
    }

    if (!(m_flags & TypeBindFlags::NoBogusCheck) && compiler()->CheckBogus(sym)) {
        if (!m_symBogus)
            m_symBogus = *psym; // Set the original.
        *psym = NULL;
    }
}


/***************************************************************************************************
    Check the arity of the given symbol. If the arity doesn't match, sets *psym to NULL. If the
    arity does match (and it's a type), sets *psym to the AGGTYPESYM.

    Call this before CheckAccess!
***************************************************************************************************/
void TypeBind::CheckArity(SYM ** psym, TypeArray * typeArgs, AGGTYPESYM * typeOuter)
{
    ASSERT(m_fValid);
    ASSERT(psym);
    ASSERT(typeArgs);

    SYM * sym = *psym;

    if (!sym)
        return;

    ASSERT(sym->isAGGSYM() || sym->isNSSYM() || sym->isTYVARSYM());

    switch (sym->getKind()) {
    case SK_AGGSYM:
        if (typeArgs->size != sym->asAGGSYM()->typeVarsThis->size) {
            if (!m_symBadArity)
                m_symBadArity = sym;
            *psym = NULL;
            return;
        }
        *psym = compiler()->getBSymmgr().GetInstAgg(sym->asAGGSYM(), typeOuter, typeArgs);
        break;

    default:
        if (typeArgs->size) {
            if (!m_symBadArity)
                m_symBadArity = sym;
            *psym = NULL;
        }
        break;
    }
}


/***************************************************************************************************
    Static method to check for type errors - like deprecated, constraints, etc.
***************************************************************************************************/
void TypeBind::CheckType(COMPILER * cmp, BASENODE * node, TYPESYM * type, SYM * symCtx, TypeBindFlagsEnum flags)
{
    if (!type || type->isERRORSYM())
        return;

    if (cmp->AggStateMax() >= AggState::Prepared)
        cmp->EnsureState(type);

    if (flags & TypeBindFlags::SuppressErrors)
        return;

    if (type->isAGGTYPESYM() || type->isNUBSYM()) {
        if (cmp->CompPhase() >= CompilerPhase::EvalConstants)
            CheckConstraints(cmp, node, type, CheckConstraintsFlags::None);
    }
    if (type->IsDeprecated() && !(flags & TypeBindFlags::NoDeprecated)) {
        cmp->clsDeclRec.ReportDeprecated(node, symCtx, SymWithType(type, NULL));
    }
    if (type->isPredefType(PT_SYSTEMVOID)) {
        cmp->Error(node, ERR_SystemVoid);
    }
}


/***************************************************************************************************
    Report the errors found. May morph *psym from NULL to non-NULL after reporting an error.
    If the compiler's AggStateMax is at least Prepared then this prepares any type that is returned.
***************************************************************************************************/
void TypeBind::ReportErrors(BASENODE * node, NAME * name, SYM * symLeft, TypeArray * typeArgs, SYM ** psym)
{
    ASSERT(m_fValid);
    ASSERT(node && name && psym);
    ASSERT(!symLeft || symLeft->isAGGTYPESYM() || symLeft->isNSAIDSYM() || symLeft->isERRORSYM());
    ASSERT(!m_symInaccess || m_symInaccess != m_symBadKind);

    if (FSuppressErrors()) {
        if (!*psym && (m_flags & TypeBindFlags::AllowInaccessible)) {
            if (m_symInaccess)
                *psym = m_symInaccess;
            else if (m_symBogus)
                *psym = m_symBogus;
        }
        if (!*psym && FAllowMissing())
            return;
        goto LDone;
    }

    if (*psym) {
        if ((*psym)->isTYPESYM())
            CheckType(compiler(), node, (*psym)->asTYPESYM(), m_symCtx, m_flags);
        // CheckType already called EnsureState so just return.
        return;
    }

    if (FAllowMissing())
        return;

    if (m_symInaccess) {
        // found an inaccessible name or an uncallable name
        if (!m_symInaccess->isUserCallable()) {
            compiler()->Error(node, ERR_CantCallSpecialMethod, name, ErrArgRefOnly(m_symInaccess));
        }
        else {
            compiler()->Error(node, ERR_BadAccess, m_symInaccess);
        }
        *psym = m_symInaccess;
    }
    else if (m_symBogus) {
        compiler()->ErrorRef(node, ERR_BogusType, m_symBogus);
        *psym = m_symBogus;
    }
    else if (m_symBadKind || m_symBadArity) {
        if (m_symBadKind) {
            compiler()->Error(node, ERR_BadSKknown, m_symBadKind, ErrArgSymKind(m_symBadKind), SK_AGGSYM);
        }

        if (m_symBadArity) {
            if (m_symBadArity->isAGGSYM()) {
                int cvar = m_symBadArity->asAGGSYM()->typeVarsThis->size;
                compiler()->ErrorRef(node, cvar > 0 ? ERR_BadArity : ERR_HasNoTypeVars,
                    m_symBadArity, ErrArgSymKind(m_symBadArity), cvar);
            }
            else {
                compiler()->ErrorRef(node, ERR_TypeArgsNotAllowed, m_symBadArity, ErrArgSymKind(m_symBadArity));
            }
        }
    }
    // Didn't find anything at all.
    else if (symLeft) {
        if (symLeft == compiler()->getBSymmgr().GetGlobalNsAid()) {
            compiler()->Error(node, ERR_GlobalSingleTypeNameNotFound, name);
        }
        else {
            int err = symLeft->isNSAIDSYM() ?
                ERR_DottedTypeNameNotFoundInNS : ERR_DottedTypeNameNotFoundInAgg;
            compiler()->Error(node, err, name, symLeft);
        }
    }
    else {
        compiler()->Error(node, ERR_SingleTypeNameNotFound, name, m_symCtx);
    }

LDone:
    if (!*psym)
        *psym = compiler()->getBSymmgr().GetErrorType(symLeft->asPARENTSYM(), name, typeArgs);
    else if ((*psym)->isTYPESYM() && compiler()->AggStateMax() >= AggState::Prepared && !(m_flags & TypeBindFlags::AvoidEnsureState))
        compiler()->EnsureState((*psym)->asTYPESYM());
}


/***************************************************************************************************
    Search a single namespace for the name.
***************************************************************************************************/
SYM * TypeBind::SearchSingleNamespace(COMPILER * cmp, BASENODE * node, NAME * name, TypeArray * typeArgs,
    NSAIDSYM * nsa, SYM * symAccess, TypeBindFlagsEnum flags)
{
    TypeBind tb(cmp, nsa->GetNS(), nsa->GetNS(), symAccess, NULL, flags);
    tb.ClearErrorInfo();

    SYM * sym = tb.SearchSingleNamespaceCore(nsa, node, name, typeArgs);

    tb.ReportErrors(node, name, nsa, typeArgs, &sym);

    return sym;
}


/***************************************************************************************************
    Search a namespace declaration and its containing declarations (including using claues) for
    the name. This is an instance method.
***************************************************************************************************/
SYM * TypeBind::SearchNamespacesInst(COMPILER * cmp, BASENODE * node, NAME * name, TypeArray * typeArgs,
    DECLSYM * decl, SYM * symAccess, TypeBindFlagsEnum flags)
{
    ASSERT(decl->isAGGDECLSYM() || decl->isNSDECLSYM());

    // Find the NSDECL
    DECLSYM * declStart;
    for (declStart = decl; !declStart->isNSDECLSYM(); declStart = declStart->DeclPar())
        ;

    Init(cmp, decl, declStart, symAccess, NULL, flags);
    ClearErrorInfo();
    return SearchNamespacesCore(declStart->asNSDECLSYM(), node, name, typeArgs);
}


/***************************************************************************************************
    Static method to search a namespace declaration and its containing declarations (including
    using claues) for the name.
***************************************************************************************************/
SYM * TypeBind::SearchNamespaces(COMPILER * cmp, BASENODE * node, NAME * name, TypeArray * typeArgs,
    NSDECLSYM * nsd, SYM * symAccess, TypeBindFlagsEnum flags)
{
    TypeBind tb(cmp, nsd, nsd, symAccess, NULL, flags);
    tb.ClearErrorInfo();

    SYM * sym = tb.SearchNamespacesCore(nsd, node, name, typeArgs);

    tb.ReportErrors(node, name, NULL, typeArgs, &sym);

    return sym;
}


/***************************************************************************************************
    Static method to search a namespace declaration and its containing declarations for a
    using/extern alias.
***************************************************************************************************/
SYM * TypeBind::SearchNamespacesForAlias(COMPILER * cmp, BASENODE * node, NAME * name,
    NSDECLSYM * nsd, TypeBindFlagsEnum flags)
{
    TypeBind tb(cmp, nsd, nsd, nsd, NULL, flags);
    tb.ClearErrorInfo();

    SYM * sym = tb.SearchNamespacesForAliasCore(nsd, node, name);

    // SearchNamespaceForAliasCore already reported an error if the alias wasn't found.
    // Don't set the node sym. It's already been set to the alias sym.
    // tb.ReportErrors(node, name, NULL, NULL, &sym);

    return sym;
}


/***************************************************************************************************
    Bind the given node to an NSAIDSYM or AGGTYPESYM. The node should be an NK_DOT, NK_NAME,
    NK_GENERICNAME or NK_ALIASQUALNAME.
***************************************************************************************************/
SYM * TypeBind::BindName(COMPILER * cmp, BASENODE * node, SYM * symCtx, TypeBindFlagsEnum flags)
{
    ASSERT(symCtx->isNSDECLSYM() || symCtx->isAGGDECLSYM() || symCtx->isMETHSYM());

    TypeBind tb(cmp, symCtx, symCtx, symCtx, NULL, flags);
    return tb.BindNameCore(node);
}


/***************************************************************************************************
    Bind the given node to a type. If the name resolves to something other than a type, reports an
    error (if !SuppressErrors) and returns ERRORSYM. The node should be an NK_DOT, NK_NAME,
    NK_GENERICNAME or NK_ALIASQUALNAME (the latter will always produce an error).
***************************************************************************************************/
TYPESYM * TypeBind::BindNameToType(COMPILER * cmp, BASENODE * node, SYM * symCtx, TypeBindFlagsEnum flags)
{
    ASSERT(symCtx->isNSDECLSYM() || symCtx->isAGGDECLSYM() || symCtx->isMETHSYM());

    TypeBind tb(cmp, symCtx, symCtx, symCtx, NULL, flags);
    return tb.BindNameToTypeCore(node);
}


/***************************************************************************************************
    Bind the given node to a type.
***************************************************************************************************/
TYPESYM * TypeBind::BindType(COMPILER * cmp, TYPEBASENODE * node, SYM * symCtx, TypeBindFlagsEnum flags)
{
    ASSERT(symCtx->isNSDECLSYM() || symCtx->isAGGDECLSYM() || symCtx->isMETHSYM());

    TypeBind tb(cmp, symCtx, symCtx, symCtx, NULL, flags);
    return tb.BindTypeCore(node);
}


/***************************************************************************************************
    Bind the given node to a type. Only the "exterior" of the agg is in scope - type variables are
    in scope but members are not.
***************************************************************************************************/
TYPESYM * TypeBind::BindTypeAggDeclExt(COMPILER * cmp, TYPEBASENODE * node, AGGDECLSYM * ads, TypeBindFlagsEnum flags)
{
    TypeBind tb(cmp, ads, ads->DeclPar(), ads, ads->Agg(), flags);
    return tb.BindTypeCore(node);
}


/***************************************************************************************************
    Bind the given node to a type. The search starts with the type variables of symTypeVars, then
    continues with symStart.
***************************************************************************************************/
TYPESYM * TypeBind::BindTypeWithTypeVars(COMPILER * cmp, TYPEBASENODE * node, SYM * symStart, SYM * symAccess, SYM * symTypeVars,
    TypeBindFlagsEnum flags)
{
    TypeBind tb(cmp, symStart, symStart, symAccess, symTypeVars, flags);
    return tb.BindTypeCore(node);
}


/***************************************************************************************************
    Bind the generic args of the given NAMENODE to a type array. If the NAMENODE is not generic,
    return the empty type array (not NULL). Returns NULL iff SuppressErrors is specified and
    there were errors in binding one or more type argument.
***************************************************************************************************/
TypeArray * TypeBind::BindTypeArgs(COMPILER * cmp, NAMENODE * node, SYM * symCtx, TypeBindFlagsEnum flags)
{
    if (node->kind != NK_GENERICNAME)
        return BSYMMGR::EmptyTypeArray();
    TypeBind tb(cmp, symCtx, symCtx, symCtx, NULL, flags);
    return tb.BindTypeArgsCore(node);
}


/***************************************************************************************************
    Bind the node as an attribute type. The resulting types actually name may have "Attribute"
    appended to the last identifier.
***************************************************************************************************/
TYPESYM * TypeBind::BindAttributeType(COMPILER * cmp, BASENODE * node, SYM * symCtx, TypeBindFlagsEnum flags)
{
    TypeBind tb(cmp, symCtx, symCtx, symCtx, NULL, flags);
    return tb.BindAttributeTypeCore(node);
}


/***************************************************************************************************
    Return the identifier in node concatenated with "Attribute". May return NULL indicating that
    appending Attribute is not appropriate (because the identifier was specified with the literal
    prefix @).
***************************************************************************************************/
NAME * TypeBind::AppendAttrSuffix(NAMENODE * node)
{
    if (node->flags & NF_NAME_LITERAL)
        return NULL;

    size_t cch = wcslen(node->pName->text) + wcslen(L"Attribute") + 1;
    PWSTR psz = STACK_ALLOC(WCHAR, cch);
    StringCchCopyW(psz, cch, node->pName->text);
    StringCchCatW (psz, cch, L"Attribute");
    return compiler()->namemgr->AddString(psz);
}


/***************************************************************************************************
    Determines whether the sym is an attribute type. This calls EnsureState before checking.
***************************************************************************************************/
bool TypeBind::IsAttributeType(SYM * sym)
{
    if (!sym || !sym->isAGGTYPESYM())
        return false;
    AGGSYM * agg = sym->asAGGTYPESYM()->getAggregate();
    compiler()->EnsureState(agg);
    return agg->isAttribute;
}


/***************************************************************************************************
    Bind the node as an attribute type. The resulting type's actual name may have "Attribute"
    appended to the last identifier.
***************************************************************************************************/
TYPESYM * TypeBind::BindAttributeTypeCore(BASENODE * node)
{
    // ALIASNAME and OPENNAME shouldn't come through here.
    ASSERT(node->kind == NK_DOT || node->kind == NK_NAME || node->kind == NK_GENERICNAME);
    ASSERT(!FSuppressErrors());

    NAMENODE * nodeName;

    switch (node->kind) {
    case NK_DOT:
        nodeName = node->asDOT()->p2->asNAME();
        break;
    case NK_NAME:
        nodeName = node->asNAME();
        break;
    default:
        VSFAIL("Bad node kind in BindAttributeTypeCore");
        return NULL;
    }

    // lookup both names but don't report errors when doing so
    NAME * nameShort = nodeName->pName;
    NAME * nameLong = AppendAttrSuffix(nodeName);

    // Suppress the errors for the initial lookup.
    TypeBindFlagsEnum flagsOrig = m_flags;
    m_flags = flagsOrig | TypeBindFlags::AllowMissing;

    SYM * symShort = BindNameCore(node, nameShort);
    SYM * symLong = nameLong ? BindNameCore(node, nameLong) : NULL;

    // Reset the SuppressErrors bit.
    m_flags = flagsOrig;

    if (FAllowMissing() && !symShort && !symLong)
        return NULL;

    // Check results.
    bool fShort = IsAttributeType(symShort);
    bool fLong =  IsAttributeType(symLong);
    TYPESYM * typeRet;

    if (fShort == fLong) {
        if (fShort) {
            compiler()->ErrorRef(nodeName, ERR_AmbigousAttribute, nameShort, symShort, symLong);
            // Use the short one.
            typeRet = symShort->asAGGTYPESYM();
            goto LDone;
        }

        if (symShort && symShort->isERRORSYM()) {
            typeRet = symShort->asERRORSYM();
            goto LDone;
        }
        if (symLong && symLong->isERRORSYM()) {
            typeRet = symLong->asERRORSYM();
            goto LDone;
        }

        // Need to generate at least one error.
        if (!symShort && !symLong)
            BindNameCore(node, nameShort); // Generate missing error.
        else {
            if (symShort)
                compiler()->ErrorRef(nodeName, ERR_NotAnAttributeClass, symShort);
            if (symLong)
                compiler()->ErrorRef(nodeName, ERR_NotAnAttributeClass, symLong);
        }
        typeRet = compiler()->getBSymmgr().GetErrorType(NULL, nameShort, NULL);
    }
    else
        typeRet = fShort ? symShort->asAGGTYPESYM() : symLong->asAGGTYPESYM();

LDone:
    return typeRet;
}


/***************************************************************************************************
    Return the type or namespace that the using alias references. Resolves the alias if it hasn't
    been resolved yet. May return NULL indicating that there was an error resolving the alias.
    Can only return an NSAIDSYM or AGGTYPESYM.
***************************************************************************************************/
SYM * TypeBind::BindUsingAlias(COMPILER * cmp, ALIASSYM *alias)
{
    if (!alias->hasBeenBound) {
        if (alias->fExtern) {
            // Extern alias.
            ASSERT(!alias->parseTree->asUSING()->pName);

            EXTERNALIASSYM * ext = cmp->LookupGlobalSym(alias->name,
                cmp->GetExternAliasContainer(), MASK_EXTERNALIASSYM)->asEXTERNALIASSYM();
            if (!ext)
                cmp->Error(alias->parseTree, ERR_BadExternAlias, alias);
            else
                alias->sym = ext->nsa;
        }
        else {
            NSDECLSYM * nsdPar = alias->parent->asNSDECLSYM();
            BASENODE * nodeRight = alias->parseTree->asUSING()->pName;
            ASSERT(nodeRight);

            // Right hand side of alias isn't bound yet. Check enclosing namespace (but not using clauses)
            // then parent namespace declarations.

            TypeBind tb(cmp, nsdPar, nsdPar, nsdPar, NULL, TypeBindFlags::None);
            tb.m_fUsingAlias = true;

            alias->sym = tb.BindNameCore(nodeRight);

            ASSERT(!alias->sym || alias->sym->isNSAIDSYM() || alias->sym->isAGGTYPESYM() || alias->sym->isERRORSYM());
        }
        alias->hasBeenBound = true;
    }

    return alias->sym;
}


/***************************************************************************************************
    Check the constraints of any type arguments in the given TYPESYM.
***************************************************************************************************/
bool TypeBind::CheckConstraints(COMPILER * cmp, BASENODE * tree, TYPESYM * type, CheckConstraintsFlagsEnum flags)
{
    type = type->GetNakedType();

    if (type->isNUBSYM()) {
        TYPESYM * typeT = type->asNUBSYM()->GetAts();
        if (typeT)
            type = typeT;
        else
            type = type->GetNakedType(true);
    }

    if (!type->isAGGTYPESYM())
        return true;

    AGGTYPESYM * ats = type->asAGGTYPESYM();

    if (ats->typeArgsAll->size == 0) {
        // Common case: there are no type vars, so there are no constraints.
        ats->fConstraintsChecked = true;
        ats->fConstraintError = false;
        return true;
    }

    if (ats->fConstraintsChecked) {
        // Already checked.
        if (!ats->fConstraintError || (flags & CheckConstraintsFlags::NoDupErrors)) {
            // No errors or no need to report errors again.
            return !ats->fConstraintError;
        }
    }

    TypeArray *typeVars = ats->getAggregate()->typeVarsThis;
    TypeArray *typeArgsThis = ats->typeArgsThis;
    TypeArray *typeArgsAll = ats->typeArgsAll;

    ASSERT(typeVars->size == typeArgsThis->size);

    cmp->EnsureState(ats);
    cmp->EnsureState(typeVars);
    cmp->EnsureState(typeArgsAll);
    cmp->EnsureState(typeArgsThis);

    if (ats->AggState() < AggState::DefinedMembers || typeVars->AggState() < AggState::DefinedMembers ||
        typeArgsAll->AggState() < AggState::DefinedMembers || typeArgsThis->AggState() < AggState::DefinedMembers)
    {
        // Too early to check anything.
        ASSERT(cmp->AggStateMax() < AggState::DefinedMembers);
        return true;
    }

    if (!ats->fConstraintsChecked) {
        ats->fConstraintsChecked = true;
        ats->fConstraintError = false;
    }

    // Check the outer type first. If CheckConstraintsFlags::Outer is not specified and the
    // outer type has already been checked then don't bother checking it.
    if (ats->outerType && ((flags & CheckConstraintsFlags::Outer) || !ats->outerType->fConstraintsChecked)) {
        CheckConstraints(cmp, tree, ats->outerType, flags);
        ats->fConstraintError |= ats->outerType->fConstraintError;
    }

    if (typeVars->size > 0)
        ats->fConstraintError |= !CheckConstraintsCore(cmp, tree, ats->getAggregate(), typeVars, typeArgsThis, typeArgsAll, NULL, (flags & CheckConstraintsFlags::NoErrors) );

    // Now check type args themselves.
    for (int i = 0; i < typeArgsThis->size; i++) {
        TYPESYM * arg = typeArgsThis->Item(i)->GetNakedType(true);
        if (arg->isAGGTYPESYM() && !arg->asAGGTYPESYM()->fConstraintsChecked) {
            CheckConstraints(cmp, tree, arg->asAGGTYPESYM(), flags | CheckConstraintsFlags::Outer);
            if (arg->asAGGTYPESYM()->fConstraintError)
                ats->fConstraintError = true;
        }
    }

    // Nullable should have the value type constraint!
    ASSERT(!ats->isPredefType(PT_G_OPTIONAL) || typeVars->ItemAsTYVARSYM(0)->FValCon());
    return !ats->fConstraintError;
}


/***************************************************************************************************
    Check the constraints on the method instantiation.
***************************************************************************************************/
void TypeBind::CheckMethConstraints(COMPILER * cmp, BASENODE * tree, MethWithInst mwi)
{
    ASSERT(mwi.Meth() && mwi.Type() && mwi.TypeArgs());
    ASSERT(mwi.Meth()->typeVars->size == mwi.TypeArgs()->size);
    ASSERT(mwi.Type()->getAggregate() == mwi.Meth()->getClass());

    if (mwi.TypeArgs()->size > 0) {
        cmp->EnsureState(mwi.Meth()->typeVars);
        cmp->EnsureState(mwi.TypeArgs());

        ASSERT(mwi.Meth()->typeVars->AggState() >= AggState::DefinedMembers);
        ASSERT(mwi.TypeArgs()->AggState() >= AggState::DefinedMembers);

        CheckConstraintsCore(cmp, tree, mwi.Meth(), mwi.Meth()->typeVars, mwi.TypeArgs(),
            mwi.Type()->typeArgsAll, mwi.TypeArgs(), CheckConstraintsFlags::None);
    }
}


/***************************************************************************************************
    Check whether typeArgs satisfies the constraints of typeVars. The typeArgsCls and typeArgsMeth
    are used for substitution on the bounds. The tree and symErr are used for error reporting.
***************************************************************************************************/
bool TypeBind::CheckConstraintsCore(COMPILER * cmp, BASENODE * tree, SYM * symErr,
    TypeArray * typeVars, TypeArray * typeArgs, TypeArray * typeArgsCls, TypeArray * typeArgsMeth, 
    CheckConstraintsFlagsEnum flags)
{
    ASSERT(typeVars->size == typeArgs->size);
    ASSERT(typeVars->size > 0);
    ASSERT(typeVars->AggState() >= AggState::DefinedMembers);
    ASSERT(typeArgs->AggState() >= AggState::DefinedMembers);
    ASSERT(flags == CheckConstraintsFlags::None || flags == CheckConstraintsFlags::NoErrors);

    bool fReportErrors = !(flags & CheckConstraintsFlags::NoErrors);
    bool fError = false;

    for (int i = 0; i < typeVars->size; i++) {
        // Empty bounds should be set to object.
        TYVARSYM * var = typeVars->ItemAsTYVARSYM(i);
        ASSERT(var->FResolved());
        TYPESYM * arg = typeArgs->Item(i);

        if (arg->isUNITSYM())
            continue;

        if (arg->isERRORSYM()) {
            // Error should have been reported previously.
            fError = true;
            continue;
        }

        if (cmp->CheckBogus(arg)) {
            if (fReportErrors)
                cmp->ErrorRef(tree, ERR_BogusType, arg);
            fError = true;
            continue;
        }

        if (arg->isPTRSYM() || arg->isSpecialByRefType()) {
            if (fReportErrors)
                cmp->Error(tree, ERR_BadTypeArgument, arg);
            fError = true;
            continue;
        }

        if (arg->isStaticClass()) {
            if (fReportErrors)
                cmp->ReportStaticClassError(tree, NULL, arg, ERR_GenericArgIsStaticClass);
            fError = true;
            continue;
        }

        if (var->FRefCon() && !arg->IsRefType()) {
            if (fReportErrors)
                cmp->ErrorRef(tree, ERR_RefConstraintNotSatisfied, symErr, ErrArgNoRef(var), arg);
            fError = true;
        }

        TypeArray * bnds = cmp->getBSymmgr().SubstTypeArray(var->GetBnds(), typeArgsCls, typeArgsMeth);
        int itypeMin = 0;

        if (var->FValCon()) {
            if (!arg->IsValType() || arg->isNUBSYM()) {
                if (fReportErrors)
                    cmp->ErrorRef(tree, ERR_ValConstraintNotSatisfied, symErr, ErrArgNoRef(var), arg);
                fError = true;
            }

            // Since FValCon() is set it is redundant to check System.ValueType as well.
            if (bnds->size && bnds->Item(0)->isPredefType(PT_VALUE))
                itypeMin = 1;
        }

        for (int j = itypeMin; j < bnds->size; j++) {
            TYPESYM * typeBnd = bnds->Item(j);
            if (!SatisfiesBound(cmp, arg, typeBnd)) {
                if (fReportErrors) {
                    cmp->Error(tree, ERR_GenericConstraintNotSatisfied,
                        ErrArgRef(symErr), ErrArg(typeBnd, ErrArgFlags::Unique), var, ErrArgRef(arg, ErrArgFlags::Unique));
                }
                fError = true;
            }
        }

        // Check the newable constraint.
        if (!var->FNewCon() || arg->IsValType())
            continue;

        if (arg->isClassType()) {
            AGGSYM * agg = arg->asAGGTYPESYM()->getAggregate();
            if (agg->hasPubNoArgCtor && !agg->isAbstract)
                continue;
        }
        else if (arg->isTYVARSYM() && arg->asTYVARSYM()->FNewCon()) {
            continue;
        }

        if (fReportErrors)
            cmp->ErrorRef(tree, ERR_NewConstraintNotSatisfied, symErr, ErrArgNoRef(var), arg);
        fError = true;
    }

    return !fError;
}


/***************************************************************************************************
    Determine whether the arg type satisfies the typeBnd constraint. Note that typeBnd could be
    just about any type (since we added naked type parameter constraints).
***************************************************************************************************/
bool TypeBind::SatisfiesBound(COMPILER * cmp, TYPESYM * arg, TYPESYM * typeBnd)
{
    if (typeBnd == arg)
        return true;

    switch (typeBnd->getKind()) {
    default:
        ASSERT(0);
        return false;

    case SK_PTRSYM:
    case SK_ERRORSYM:
        return false;

    case SK_ARRAYSYM:
    case SK_TYVARSYM:
        break;

    case SK_NUBSYM:
        typeBnd = typeBnd->asNUBSYM()->GetAts();
        if (!typeBnd)
            return true;
        break;

    case SK_AGGTYPESYM:
        break;
    }

    ASSERT(typeBnd->isAGGTYPESYM() || typeBnd->isTYVARSYM() || typeBnd->isARRAYSYM());

    switch (arg->getKind()) {
    default:
        ASSERT(0);
        return false;

    case SK_ERRORSYM:
    case SK_PTRSYM:
        return false;

    case SK_NUBSYM:
        arg = arg->asNUBSYM()->GetAts();
        if (!arg)
            return true;
        // Fall through.
    case SK_TYVARSYM:
    case SK_ARRAYSYM: // IsBaseType handles IList<T>....
    case SK_AGGTYPESYM:
        return cmp->IsBaseType(arg, typeBnd);
    }
}
