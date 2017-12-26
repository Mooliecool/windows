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
// File: clsdrec.cpp
//
// Routines for declaring a class
// ===========================================================================
#include "stdafx.h"

// Tokens which correspond to the parser flags for the various item modifiers
// This needs to be kept in ssync with the list in nodes.h
const TOKENID CLSDREC::accessTokens[] = {
    TID_ABSTRACT,
    TID_NEW,
    TID_OVERRIDE,
    TID_PRIVATE,
    TID_PROTECTED,
    TID_INTERNAL,
    TID_PUBLIC,
    TID_SEALED,
    TID_STATIC,
    TID_VIRTUAL,
    TID_EXTERN,
    TID_READONLY,
    TID_VOLATILE,
    TID_UNSAFE,
};

//
// class level init
//
CLSDREC::CLSDREC() 
{
    m_explicitLayoutValue = -1;
    m_sequentialLayoutValue = -1;
}

// Issue an error if the given type is unsafe. This function should NOT be called if the type
// was used in an unsafe context -- this function doesn't check that we're in an unsafe context.
void CLSDREC::checkUnsafe(BASENODE * tree, TYPESYM * type, bool unsafeContext, int errCode)
{
    if (type->isUnsafe() && !unsafeContext) {
        compiler()->Error(tree, errCode);
    }
}

void CLSDREC::checkUnmanaged(BASENODE * tree, TYPESYM * type)
{
    if (!type || !type->isPTRSYM()) {
        return;
    }

    if (compiler()->CompPhase() < CompilerPhase::Prepare) {
        // It will get checked later.
        return;
    }

    type = type->asPTRSYM()->GetMostBaseType();
    ASSERT(type);

    if (compiler()->funcBRec.isManagedType(type)) {
        compiler()->ErrorRef(tree, ERR_ManagedAddr, type);
    }
}


/***************************************************************************************************
    Return whether target can be accessed from within a given place.

        symCheck  - The symbol to check access on. Typically this is a member of an AGGSYM. The
            only TYPESYM that is legal is a TYVARSYM.
        atsCheck  - The type containing symCheck. When symCheck is a member of an AGGSYM, this
            should be the particular instantiation of the AGGSYM and should NOT be NULL.
        aggWhere  - The location from which symCheck is being referenced. This may be NULL
            indicating global access (eg, inside an attribute on a top level element).
        typeThru  - The type through which symCheck is being accessed. This is used for checking
            protected access.

!!
!!IMPORTANT: IF YOU CHANGE THIS MAKE SURE THAT YOU KEEP CDirectTypeRef::IsSymbolAccessible UP TO DATE!!
!!

    Never reports an error.
***************************************************************************************************/
bool CLSDREC::CheckAccess(SYM * symCheck, AGGTYPESYM * atsCheck, SYM * symWhere, TYPESYM * typeThru)
{
    ASSERT(symCheck);
    ASSERT(!symCheck->isTYPESYM() || symCheck->isTYVARSYM());
    ASSERT(!atsCheck || symCheck->parent == atsCheck->getAggregate());
    ASSERT(!typeThru ||
        typeThru->isAGGTYPESYM() ||
        typeThru->isTYVARSYM() ||
        typeThru->isARRAYSYM() ||
        typeThru->isNUBSYM());

#ifdef DEBUG
    switch (symCheck->getKind()) {
    default:
        break;
    case SK_METHSYM:
    case SK_PROPSYM:
    case SK_FAKEMETHSYM:
    case SK_MEMBVARSYM:
    case SK_EVENTSYM:
        ASSERT(atsCheck);
        break;
    }
#endif // DEBUG

    if (!CheckAccessCore(symCheck, atsCheck, symWhere, typeThru))
        return false;

    // Check the accessibility of the return type.
    TYPESYM * type;

    switch (symCheck->getKind()) {
    default:
        return true;

    case SK_METHSYM:
    case SK_PROPSYM:
    case SK_FAKEMETHSYM:
        type = symCheck->asMETHPROPSYM()->retType;
        break;

    case SK_MEMBVARSYM:
        type = symCheck->asMEMBVARSYM()->type;
        break;

    case SK_EVENTSYM:
        type = symCheck->asEVENTSYM()->type;
        break;
    }

    // For members of AGGSYMs, atsCheck should always be specified!
    ASSERT(atsCheck);

    if (atsCheck && atsCheck->getAggregate()->isSource) {
        return true;
    }

    // Substitute on the type.
    if (atsCheck && atsCheck->typeArgsAll->size > 0)
        type = compiler()->getBSymmgr().SubstType(type, atsCheck);

    return CheckTypeAccess(type, symWhere);
}

//!!
//!!IMPORTANT: IF YOU CHANGE THIS MAKE SURE THAT YOU KEEP CDirectTypeRef::IsSymbolAccessible UP TO DATE!!
//!!
bool CLSDREC::CheckAccessCore(SYM * symCheck, AGGTYPESYM * atsCheck, SYM * symWhere, TYPESYM * typeThru)
{
    ASSERT(symCheck);
    ASSERT(!symCheck->isTYPESYM() || symCheck->isTYVARSYM());
    ASSERT(!atsCheck || symCheck->parent == atsCheck->getAggregate());
    ASSERT(!typeThru ||
        typeThru->isAGGTYPESYM() ||
        typeThru->isTYVARSYM() ||
        typeThru->isARRAYSYM() ||
        typeThru->isNUBSYM());

    switch (symCheck->GetAccess()) {
    default:
        ASSERT(!"Bad access level");
    case ACC_UNKNOWN:
        return false;

    case ACC_PUBLIC:
        return true;

    case ACC_PRIVATE:
    case ACC_PROTECTED:
        if (!symWhere)
            return false;
        break;

    case ACC_INTERNAL:
    case ACC_INTERNALPROTECTED:  // Check internal, then protected.
        if (!symWhere)
            return false;
        if (symWhere->SameAssemOrFriend(symCheck)) {
            // Log friend use
            if (symCheck->GetAssemblyID() != kaidThisAssembly && symWhere->GetAssemblyID() == kaidThisAssembly)
                compiler()->MarkUsedFriendAssemblyRef(symCheck->GetSomeInputFile());
            return true;
        }
        if (symCheck->GetAccess() != ACC_INTERNALPROTECTED)
            return false;
        break;
    }

    // Should always have atsCheck for private and protected access check.
    // We currently don't need it since access doesn't respect instantiation.
    // We just use symWhere->parent->asAGGSYM() instead.
    ASSERT(atsCheck);
    AGGSYM * aggCheck = symCheck->parent->asAGGSYM();

    // Find the inner-most enclosing AGGSYM.
    AGGSYM * aggWhere = NULL;

    for (SYM * symT = symWhere; symT; symT = symT->parent) {
        if (symT->isAGGSYM()) {
            aggWhere = symT->asAGGSYM();
            break;
        }
        if (symT->isAGGDECLSYM()) {
            aggWhere = symT->asAGGDECLSYM()->Agg();
            break;
        }
    }

    if (!aggWhere)
        return false;

    // First check for private access.
    for (AGGSYM * agg = aggWhere; agg; agg = agg->GetOuterAgg()) {
        if (agg == aggCheck)
            return true;
    }

    if (symCheck->GetAccess() == ACC_PRIVATE)
        return false;

    // Handle the protected case - which is the only real complicated one.
    ASSERT(symCheck->GetAccess() == ACC_PROTECTED || symCheck->GetAccess() == ACC_INTERNALPROTECTED);

    // Check if symCheck is in aggWhere or a base of aggWhere,
    // or in an outer agg of aggWhere or a base of an outer agg of aggWhere.

    AGGTYPESYM * atsThru = NULL;

    if (typeThru) {
        bool isStatic = true;

        switch (symCheck->getKind()) {
        case SK_MEMBVARSYM:
            isStatic = symCheck->asMEMBVARSYM()->isStatic;
            break;
        case SK_EVENTSYM:
            isStatic = symCheck->asEVENTSYM()->isStatic;
            break;
        case SK_AGGSYM:
            isStatic = true;
            break;
        default:
            isStatic = symCheck->asMETHPROPSYM()->isStatic;
            break;
        }

        if (!isStatic) {
            // Get the AGGTYPESYM through which the symbol is accessed.
            switch (typeThru->getKind()) {
            default:
                VSFAIL("Bad typeThru!");
                break;
            case SK_AGGTYPESYM:
                atsThru = typeThru->asAGGTYPESYM();
                break;
            case SK_ARRAYSYM:
                atsThru = compiler()->GetReqPredefType(PT_ARRAY);
                break;
            case SK_TYVARSYM:
                atsThru = typeThru->asTYVARSYM()->GetBaseCls();
                break;
            case SK_NUBSYM:
                atsThru = typeThru->asNUBSYM()->GetAts();
                break;
            }
        }
    }

    // Look for aggCheck among the base classes of aggWhere and outer aggs.
    for (AGGSYM * agg = aggWhere; agg; agg = agg->GetOuterAgg()) {
        ASSERT(agg != aggCheck); // We checked for this above.

        // Look for aggCheck among the base classes of agg.
        if (agg->FindBaseAgg(aggCheck)) {
            // aggCheck is a base class of agg. Check atsThru.
            // For non-static protected access to be legal, atsThru must be an instantiation of
            // agg or a type derived from an instantiation of agg. In this case
            // all that matters is that agg is in the base AGGSYM chain of atsThru. The
            // actual AGGTYPESYMs involved don't matter.
            if (!atsThru || atsThru->getAggregate()->FindBaseAgg(agg))
                return true;
        }
    }

    return false;
}


bool CLSDREC::CheckTypeAccess(TYPESYM * type, SYM * symWhere)
{
    ASSERT(type);

    // Array, Ptr, Nub, etc don't matter.
    type = type->GetNakedType(true);

    if (!type->isAGGTYPESYM()) {
        ASSERT(type->isVOIDSYM() || type->isERRORSYM() || type->isTYVARSYM());
        return true;
    }

    for (AGGTYPESYM * ats = type->asAGGTYPESYM(); ats; ats = ats->outerType) {
        if (!CheckAccessCore(ats->getAggregate(), ats->outerType, symWhere, NULL))
            return false;
    }

    TypeArray * typeArgs = type->asAGGTYPESYM()->typeArgsAll;
    for (int i = 0; i < typeArgs->size; i++) {
        if (!CheckTypeAccess(typeArgs->Item(i), symWhere))
            return false;
    }

    return true;
}


void CLSDREC::ReportAccessError(BASENODE * tree, SymWithType swtBad, SYM * symWhere, TYPESYM * typeQual)
{
    ASSERT(!CheckAccess(swtBad.Sym(), swtBad.Type(), symWhere, typeQual) ||
        !CheckTypeAccess(swtBad.Type(), symWhere));

    if (typeQual && (swtBad.Sym()->GetAccess() == ACC_PROTECTED || swtBad.Sym()->GetAccess() == ACC_INTERNALPROTECTED) &&
        CheckAccess(swtBad.Sym(), swtBad.Type(), symWhere, NULL) &&
        !CheckAccess(swtBad.Sym(), swtBad.Type(), symWhere, typeQual))
    {
        compiler()->Error(tree, ERR_BadProtectedAccess, swtBad, typeQual, symWhere);
    }
    else
        compiler()->ErrorRef(tree, ERR_BadAccess, swtBad);
}


// Normally in all of the following "created" is NULL.  However, in some cases we have to create
// a member before we can reasonably carry out the following checks.  For example, when we
// have a generic static method, we have to create the symbol for the method BEFORE we
// parse the argument types, because the argument types might refer to the type parameters.
//
// Thus, we sometimes pass in a "created" value to indicate that we might find a symbol matching the given types
// but that it's not a problem if it == created.

// This method just strictly checks for name conflicts and does not take into account method params, 
bool CLSDREC::CheckForBadMemberSimple(NAME *name, BASENODE *parseTree, AGGSYM *cls, SYM* created)
{
    // check for name same as that of parent aggregate
    // Name same as type is only an error for classes and structs
    if (name && name == cls->name && (cls->IsClass() || cls->IsStruct())) {
        compiler()->Error(parseTree, ERR_MemberNameSameAsType, name, ErrArgRefOnly(cls));
        return false;
    }

    return CheckForDuplicateSymbol(name, parseTree, cls, created);
}

bool CLSDREC::CheckForBadMember(NAME *name, SYMKIND symkind, TypeArray *params, BASENODE *parseTree, AGGSYM *cls, TypeArray *typeVarsMeth, SYM* created)
{
    ASSERT(symkind == SK_PROPSYM || symkind == SK_METHSYM);

    // check for name same as that of parent aggregate
    // Name same as type is only an error for classes and structs
    if (name && name == cls->name && (cls->IsClass() || cls->IsStruct())) {
        compiler()->Error(parseTree, ERR_MemberNameSameAsType, name, ErrArgRefOnly(cls));
        return false;
    }

    return CheckForDuplicateSymbol(name, symkind, params, parseTree, cls, typeVarsMeth, created);
}

bool CLSDREC::CheckForDuplicateSymbol(NAME *name, BASENODE *parseTree, AGGSYM *cls, SYM* created)
{
    if (name) {
        SYM* present = compiler()->getBSymmgr().LookupAggMember(name, cls, MASK_ALL);
        if (present && present != created) {
            compiler()->Error(parseTree, ERR_DuplicateNameInClass, name, cls, ErrArgRefOnly(present));
            return false;
        }
    }

    return true;
}

// Returns false if there is a duplicate, true if there is no duplicate.
bool CLSDREC::CheckForDuplicateSymbol(NAME *name, SYMKIND symkind, TypeArray *params, BASENODE *parseTree, AGGSYM *cls, TypeArray *typeVarsMeth, SYM* created)
{
    ASSERT(symkind == SK_PROPSYM || symkind == SK_METHSYM);
    ASSERT(symkind == SK_METHSYM || TypeArray::Size(typeVarsMeth) == 0);
    ASSERT(params);

    if (!name || params->HasErrors())
        return true;

    int err;
    SYM *symCur;

    for (symCur = compiler()->getBSymmgr().LookupAggMember(name, cls, MASK_ALL);
        symCur != NULL;
        symCur = compiler()->getBSymmgr().LookupNextSym(symCur, cls, MASK_ALL))
    {
        if (symCur == created) {
            // Don't count this one as a duplicate!
            continue;
        }

        if (symCur->getKind() != symkind) {
            // Different kinds of members so can't overload.
            err = ERR_DuplicateNameInClass;
            goto LError;
        }

        // Same kind of symbol.
        METHPROPSYM *mpsCur = symCur->asMETHPROPSYM();

        if (mpsCur->params->size != params->size) {
            // Different number of parameters so not a duplicate.
            continue;
        }

        if (symkind == SK_METHSYM && symCur->asMETHSYM()->typeVars->size != TypeArray::Size(typeVarsMeth)) {
            // Overloading on arity is fine.
            continue;
        }

        // Substitute method level type parameters. Note that there is no need to substitute class level type variables
        // since we're looking in the same class.
        TypeArray *paramsCur = compiler()->getBSymmgr().SubstTypeArray(mpsCur->params, (TypeArray *)NULL, typeVarsMeth);

        if (paramsCur == params) {
            err = ERR_MemberAlreadyExists;
            goto LError;
        }

        if (symkind == SK_METHSYM) {
            // Check for overloading on ref and out.
            for (int i = 0; ; i++) {
                if (i >= params->size) {
                    compiler()->Error(parseTree, ERR_OverloadRefOut, name, ErrArgRefOnly(symCur));
                    return false;
                }

                TYPESYM *t1 = params->Item(i);
                TYPESYM *t2 = paramsCur->Item(i);
                if (t1 != t2 &&
                    !(t1->isPARAMMODSYM() && t2->isPARAMMODSYM() && t1->asPARAMMODSYM()->paramType() == t2->asPARAMMODSYM()->paramType()))
                {
                    // t1 != t2 && t1 and t2 differ by more than out/ref-ness. So these signatures
                    // differ by more than out/ref-ness.
                    break;
                }
            }
        }
    }

    return true;

LError:
    if (name == compiler()->namemgr->GetPredefName(PN_CTOR) ||
        name == compiler()->namemgr->GetPredefName(PN_STATCTOR))
    {
        name = cls->name;
    }
    else if (parseTree->kind == NK_DTOR && symCur->isMETHSYM() && symCur->asMETHSYM()->isDtor()) {
        // If we have 2 destructors, report it as a duplicate destructor
        // otherwise report it as a duplicate Finalize()
        size_t temp_len = wcslen(cls->name->text) + 2;
        WCHAR* temp = STACK_ALLOC(WCHAR, temp_len);
        temp[0] = L'~';
        HRESULT hr;
        hr = StringCchCopyW(temp + 1, temp_len - 1, cls->name->text);
        ASSERT (SUCCEEDED (hr));
        name = compiler()->namemgr->AddString(temp);
    }

    compiler()->Error(parseTree, err, name, cls, ErrArgRefOnly(symCur));

    return false;
}


// Add an aggregate symbol to the given parent.  Checks for collisions and 
// displays an error if so. Does NOT declare nested types, or other aggregate members.
//
//      aggregateNode   is either a CLASSNODE or a DELEGATENODE
//      nameNode        is the NAMENODE of the aggregate to be declared
//      declPar         is the containing declaration for the new aggregate (an NSDECL or an AGGDECL)
AGGDECLSYM * CLSDREC::AddAggregate(BASENODE *aggregateNode, NAMENODE* nameNode, DECLSYM * declPar)
{
    ASSERT(aggregateNode->kind == NK_CLASS  || aggregateNode->kind == NK_DELEGATE ||
           aggregateNode->kind == NK_STRUCT || aggregateNode->kind == NK_INTERFACE ||
           aggregateNode->kind == NK_ENUM);

    ASSERT(declPar->isAGGDECLSYM() || declPar->isNSDECLSYM());
    ASSERT(declPar->GetAssemblyID() == kaidThisAssembly);

    NAME * ident = nameNode->pName;
    BAGSYM * bagPar = declPar->Bag();
    AGGDECLSYM * clsdecl = NULL;

    // Get the type variables.
    BASENODE * typars;

    switch (aggregateNode->kind) {
    case NK_CLASS:
    case NK_STRUCT:
    case NK_INTERFACE:
        typars = aggregateNode->asAGGREGATE()->pTypeParams;
        break;
    case NK_DELEGATE:
        typars = aggregateNode->asDELEGATE()->pTypeParams;
        break;
    default:
        typars = NULL;
        break;
    }

    // Count the type variables.
    int cvar = 0;

    NODELOOP(typars, TYPEBASE, var)
        cvar++;
    ENDLOOP;

    // Determine the new AggKind.
    AggKindEnum akNew;

    switch (aggregateNode->kind) {
    case NK_CLASS:
        akNew = AggKind::Class;
        break;
    case NK_STRUCT:
        akNew = AggKind::Struct;
        break;
    case NK_INTERFACE:
        akNew = AggKind::Interface;
        break;
    case NK_ENUM:
        akNew = AggKind::Enum;
        break;
    case NK_DELEGATE:
        akNew = AggKind::Delegate;
        break;
    default:
        ASSERT(!"Unrecognized aggregate parse node");
        return NULL;
    }

    // Get any conflicting bag.
    bool fMulti = false;
    AGGSYM * cls = NULL;
    NSSYM * nsPrev = NULL;
    symbmask_t mask = cvar > 0 ? MASK_AGGSYM : MASK_AGGSYM | MASK_NSSYM;

    for (BAGSYM * bag = compiler()->LookupInBagAid(ident, bagPar, kaidThisAssembly, mask)->asBAGSYM();
        bag;
        bag = compiler()->LookupNextInAid(bag, kaidThisAssembly, mask)->asBAGSYM())
    {
        // Try to match up the kind as well.
        if (bag->isNSSYM()) {
            ASSERT(!nsPrev);
            nsPrev = bag->asNSSYM();
        }
        else if (bag->asAGGSYM()->typeVarsThis->size == cvar) {
            if (bag->asAGGSYM()->AggKind() == akNew) {
                cls = bag->asAGGSYM();
                break;
            }
            else if (!cls)
                cls = bag->asAGGSYM();
        }
    }

    if (bagPar->isAGGSYM()) {
        // Nested.
        if (cls) {
            // Have an existing aggsym with same name and same or conflicting arity.
            // Might be OK if both are declared partial.
            ASSERT(cls->hasParseTree);
            fMulti = true;
        }

        // Check for same name as parent.
        if (ident == bagPar->name) {
            compiler()->Error(nameNode, ERR_MemberNameSameAsType, ident, ErrArgRefOnly(bagPar));
        }

        // There shouldn't be other non-agg members yet.
        SYM *symOther = compiler()->getBSymmgr().LookupAggMember(ident, bagPar->asAGGSYM(), MASK_ALL & ~MASK_AGGSYM);
        ASSERT(!symOther || symOther->isTYVARSYM());

        if (symOther != NULL && symOther->isTYVARSYM()) {
            compiler()->Error(nameNode, ERR_DuplicateNameInClass, ident, bagPar, ErrArgRefOnly(bagPar));
        }
    }
    else if (cls) {
        // We have multiple declarations, might be OK if all declared partial.
        // We haven't imported any added modules yet.
        ASSERT(cls->hasParseTree);
        fMulti = true;
    }
    else if (nsPrev) {
        // class is clashing with namespace.
        ASSERT(nsPrev->DeclFirst()->inputfile->isSource);
        compiler()->Error(nameNode, ERR_DuplicateNameInNS, nameNode, bagPar, ErrArgRefOnly(nsPrev->DeclFirst()));
    }

    if (fMulti) {
        // This is the second or later declaration. We already have the AGGSYM,
        // just create a new declaration, after verifying that both are marked partial.
        // First look through the old decl's for at least one marked partial.
        AGGDECLSYM * declOld = cls->DeclFirst();
        if (!declOld->isPartial) {
            // See if any decl's are partial, so we don't keep giving errors if only the
            // first is not marked partial.
            for (AGGDECLSYM * adsT = declOld; (adsT = adsT->DeclNext()); ) {
                if (adsT->isPartial) {
                    declOld = adsT;
                    break;
                }
            }
        }

        bool fPartialOk = (aggregateNode->kind == NK_CLASS || aggregateNode->kind == NK_STRUCT || aggregateNode->kind == NK_INTERFACE);
        bool fPartial = fPartialOk && (aggregateNode->flags & NF_MOD_PARTIAL);

        // Merge the types if possible.
        if (akNew != cls->AggKind() || !fPartialOk) {
            if (fPartial && declOld->isPartial) {
                // If both are marked partial, assume the user got confused on the kind.
                compiler()->ErrorRef(nameNode, ERR_PartialTypeKindConflict, cls);
            }
            else {
                compiler()->Error(nameNode,
                    bagPar->isAGGSYM() ? ERR_DuplicateNameInClass : ERR_DuplicateNameInNS,
                    nameNode, bagPar, ErrArgRefOnly(declOld));
            }
            fMulti = false;
            goto LCreateNew;
        }

        if (!fPartial && !declOld->isPartial) {
            // Neither marked partial: duplicate name error, then treat as partial.
            compiler()->Error(nameNode,
                bagPar->isAGGSYM() ? ERR_DuplicateNameInClass : ERR_DuplicateNameInNS,
                nameNode, bagPar, ErrArgRefOnly(declOld));
        }
        else if (!fPartial) {
            // New declaration should have been marked partial.
            compiler()->ErrorRef(nameNode, ERR_MissingPartial, cls);
        }
        else if (!declOld->isPartial) {
            // Existing declaration should have been marked partial.
            compiler()->ErrorRef(NULL, ERR_MissingPartial, cls, ErrArgRefOnly(nameNode));
        }

        // both new and old declarations are partial. Add the new declaration to the old symbol.
        clsdecl = compiler()->getBSymmgr().CreateAggDecl(cls, declPar);

        ASSERT(cls->AggState() == AggState::Declared);
        ASSERT(cls->AggKind() == akNew);
    }
    else {
LCreateNew:
        //
        // create new aggregate and it's declaration.
        //
        cls = compiler()->getBSymmgr().CreateAgg(ident, declPar);
        clsdecl = compiler()->getBSymmgr().CreateAggDecl(cls, declPar);
        cls->isArityInName = cvar > 0;
        cls->SetAggKind(akNew);
    }

    // Make sure the cls and clsdecl are hooked together right, and clsdecl is the last declaration.
    ASSERT(clsdecl->Agg() == cls);
    ASSERT(clsdecl->DeclNext() == NULL);

    clsdecl->parseTree = aggregateNode;

    cls->hasParseTree = true;

    //
    // check modifiers, set flags
    //
    unsigned allowableFlags = NF_MOD_PUBLIC | NF_MOD_INTERNAL;

    switch (aggregateNode->kind) {
    case NK_CLASS:
        ASSERT(cls->AggKind() == AggKind::Class);
        allowableFlags |= NF_MOD_SEALED | NF_MOD_ABSTRACT | NF_MOD_UNSAFE | NF_MOD_PARTIAL | NF_MOD_STATIC;

        // Sealed, abstract and static are additive.  Abstract can only be specified if neither of the other two flags are.
        ASSERT(fMulti || !cls->isAbstract && !cls->isSealed);
        if (aggregateNode->flags & (NF_MOD_STATIC | NF_MOD_ABSTRACT | NF_MOD_SEALED)) {
            int flags = 0;
            FOREACHAGGDECL(cls, decl)
                flags |= decl->parseTree->flags;
            ENDFOREACHAGGDECL
            if ((flags & NF_MOD_ABSTRACT) && (flags & (NF_MOD_SEALED | NF_MOD_STATIC)))
                compiler()->ErrorRef(NULL, ERR_AbstractSealedStatic, cls);

            if ((flags & NF_MOD_STATIC) && (flags & NF_MOD_SEALED))
                compiler()->ErrorRef(NULL, ERR_SealedStaticClass, cls);

            if (flags & NF_MOD_STATIC) {
                // static classes are represented as sealed abstract classes
                cls->isAbstract = true;
                cls->isSealed = true;
            }
            if (flags & NF_MOD_ABSTRACT)
                cls->isAbstract = true;
            if (flags & NF_MOD_SEALED)
                cls->isSealed = true;
        }
        break;

    case NK_STRUCT:
        ASSERT(cls->AggKind() == AggKind::Struct);
        ASSERT(!cls->isAbstract);
        cls->isSealed = true;
        allowableFlags |= NF_MOD_UNSAFE | NF_MOD_PARTIAL;
        break;

    case NK_INTERFACE:
        ASSERT(cls->AggKind() == AggKind::Interface);
        ASSERT(!cls->isSealed);
        cls->isAbstract = true;
        allowableFlags |= NF_MOD_UNSAFE | NF_MOD_PARTIAL;
        break;

    case NK_ENUM:
        ASSERT(cls->AggKind() == AggKind::Enum);
        ASSERT(!fMulti);
        cls->isSealed = true;
        ASSERT(!cls->isAbstract);
        break;

    case NK_DELEGATE:
        ASSERT(cls->AggKind() == AggKind::Delegate);
        ASSERT(!fMulti);
        cls->isSealed = true;
        ASSERT(!cls->isAbstract);
        allowableFlags |= NF_MOD_UNSAFE;
        break;

    default:
        ASSERT(!"Unrecognized aggregate parse node");
        return NULL;
    }

    if (bagPar->isAGGSYM()) {
        // nested classes can have private access
        // classes in a namespace can only have public or assembly access
        //
        // also nested classes can be marked new
        allowableFlags |= NF_MOD_NEW | NF_MOD_PRIVATE;

        // only class members can be protected
        if (bagPar->asAGGSYM()->IsClass()) {
            allowableFlags |= NF_MOD_PROTECTED;
        }
    }

    if (fMulti) {
        // Check consistency of accessibility.
        int flags = aggregateNode->flags & allowableFlags & NF_MOD_ACCESSMODIFIERS;
        if (flags) {
            int flagsPrev = 0;
            FOREACHAGGDECL(cls, decl)
                if (decl != clsdecl) {
                    int flagsDecl = decl->parseTree->flags & allowableFlags & NF_MOD_ACCESSMODIFIERS;
                    if (flagsDecl) {
                        if (flagsPrev && flagsPrev != flagsDecl) {
                            // We previously reported the error.
                            flagsPrev = 0;
                            break;
                        }
                        flagsPrev = flagsDecl;
                    }
                }
            ENDFOREACHAGGDECL
            if (flagsPrev && flags != flagsPrev) {
                // Conflicting accessibilities.
                compiler()->ErrorRef(NULL, ERR_PartialModifierConflict, cls);
            }
        }
    }

    ACCESS oldAccess = ACC_UNKNOWN;
    if (fMulti) {
        // This ASSERTs that access is not ACC_UNKNOWN ... If this is marked as a multiple definition,
        // then the previous instance should have set the access.
        oldAccess = cls->GetAccess();  
    }
                                       
    checkFlags(cls, allowableFlags, aggregateNode->flags);
    cls->SetAccess(GetAccessFromFlags(bagPar, allowableFlags, aggregateNode->flags));

    clsdecl->isPartial = (aggregateNode->flags & NF_MOD_PARTIAL & allowableFlags) != 0;

    if (fMulti && !(aggregateNode->flags & allowableFlags & NF_MOD_ACCESSMODIFIERS)) {
        // This partial didn't specify accessibility so restore the previous one.
        cls->SetAccess(oldAccess);
    }

    // unsafe if declared unsafe or container is unsafe.
    clsdecl->isUnsafe = (aggregateNode->flags & NF_MOD_UNSAFE || (declPar->isAGGDECLSYM() && declPar->asAGGDECLSYM()->isUnsafe));  

    // GENERICS: declare type parameters, but not bounds.  These come later.  The type
    // params come first because they will be needed whenever instantiating
    // generic types, e.g. when resolving inheritance.
    if (!fMulti) {
        // Usual case: first or only declaration of this type.
        DefineClassTypeVars(cls, clsdecl, typars);
    } else {
        // Make sure that the type variables agree in number and name.
        ASSERT(cvar == cls->typeVarsThis->size);
        int ivar = 0;
        bool fError = false;

        NODELOOP(typars, TYPEBASE, node)
            ASSERT(ivar < cvar);

            TYVARSYM * var = cls->typeVarsThis->ItemAsTYVARSYM(ivar++);
            BASENODE * nodeName;

            if (node->kind == NK_TYPEWITHATTR) {
                BASENODE * nodeAttr = node->asTYPEWITHATTR()->pAttr;
                ASSERT(nodeAttr);
                compiler()->getBSymmgr().AddToGlobalAttrList(nodeAttr, clsdecl, &var->attributeListTail);
                nodeName = node->asTYPEWITHATTR()->pType;
            }
            else
                nodeName = node;

            if (nodeName->kind == NK_NAMEDTYPE)
                nodeName = nodeName->asNAMEDTYPE()->pName;

            if (nodeName->kind != NK_NAME) {
                ASSERT(nodeName->IsAnyType());
                compiler()->Error(nodeName, ERR_TypeParamMustBeIdentifier);
            }
            else if (var->name != nodeName->asNAME()->pName) {
                if (!fError)
                    compiler()->ErrorRef(NULL, ERR_PartialWrongTypeParams, cls);
                fError = true;
            }
            else {
            }
        ENDLOOP;
        ASSERT(ivar == cvar);
    }

    ASSERT(cls->typeVarsThis && cls->typeVarsAll);

    cls->SetAggState(AggState::Declared);

    return clsdecl;
}


// Add a synthesised aggregate symbol to the given parent. Does NOT check for collisions.
// Does NOT declare nested types, or other aggregate members.
//
//      szBasename      is the user-supplied portion of the synthesized aggregate name
//      snk             is the particular kind of aggregate we're syntehesizing
//      declPar         is the containing declaration for the new aggregate
AGGDECLSYM * CLSDREC::AddSynthAgg(PCWSTR szBasename, SpecialNameKindEnum snk, AGGDECLSYM * declPar)
{
    NAME * ident = NULL;
    AGGSYM * cls = NULL;
    AGGDECLSYM * clsdecl = NULL;

    ident = compiler()->funcBRec.CreateSpecialName(snk, szBasename);

    // Create new aggregate and associated decl.
    cls = compiler()->getBSymmgr().CreateAgg(ident, declPar);
    clsdecl = compiler()->getBSymmgr().CreateAggDecl(cls, declPar);

    // cls->isArityInName = false;
    // cls->hasParseTree = false;
    clsdecl->parseTree = NULL;
    cls->SetAggKind(AggKind::Class);
    cls->isSealed = true;
    cls->SetAccess(ACC_PRIVATE);
    cls->isFabricated = true;
    cls->typeVarsThis = BSYMMGR::EmptyTypeArray();
    cls->typeVarsAll = declPar->Agg()->typeVarsAll;

    cls->SetAggState(AggState::Bounds);

    return clsdecl;
}


void CLSDREC::DefineClassTypeVars(AGGSYM * agg, AGGDECLSYM *decl, BASENODE * typars)
{
    int cvar = CountListNode(typars);
    TypeArray * typeVarsOuter = agg->isNested() ? agg->Parent()->asAGGSYM()->typeVarsAll : BSYMMGR::EmptyTypeArray();

    if (cvar) {
        int cvarOuter = typeVarsOuter->size;
        TYVARSYM ** prgvar = STACK_ALLOC(TYVARSYM *, cvar + cvarOuter);
        int ivar = 0;

        if (cvarOuter)
            typeVarsOuter->CopyItems(0, cvarOuter, prgvar);

        NODELOOP(typars, TYPEBASE, node)
            prgvar[ivar + cvarOuter] = CreateTypeVar(agg, node, ivar, prgvar + cvarOuter, ivar + cvarOuter);
            if (node->kind == NK_TYPEWITHATTR) {
                compiler()->getBSymmgr().AddToGlobalAttrList(node->asTYPEWITHATTR()->pAttr,
                    decl, &prgvar[ivar+cvarOuter]->attributeListTail);
            }
            ivar++;
        ENDLOOP;

        ASSERT(ivar == cvar);

        agg->typeVarsThis = compiler()->getBSymmgr().AllocParams(cvar, (TYPESYM **) prgvar + cvarOuter);
        agg->typeVarsAll = compiler()->getBSymmgr().AllocParams(cvar + cvarOuter, (TYPESYM **) prgvar);
    } else {
        agg->typeVarsThis = BSYMMGR::EmptyTypeArray();
        agg->typeVarsAll = typeVarsOuter;
    }
}


void CLSDREC::DefineMethodTypeVars(METHSYM * meth, BASENODE * typars)
{
    int cvar = CountListNode(typars);

    if (cvar) {
        TYVARSYM ** prgvar = STACK_ALLOC(TYVARSYM *, cvar);
        int ivar = 0;

            NODELOOP(typars, TYPEBASE, node)
                prgvar[ivar] = CreateTypeVar(meth, node, ivar, prgvar, ivar);
                if (node->kind == NK_TYPEWITHATTR) {
                    compiler()->getBSymmgr().AddToGlobalAttrList(node->asTYPEWITHATTR()->pAttr,
                        meth, &prgvar[ivar]->attributeListTail);
                }
                ivar++;
            ENDLOOP;
            meth->typeVars = compiler()->getBSymmgr().AllocParams(cvar, (TYPESYM **) prgvar);

    } else {
        meth->typeVars = BSYMMGR::EmptyTypeArray();
    }
}


TYVARSYM * CLSDREC::CreateTypeVar(PARENTSYM * symPar, TYPEBASENODE * tree, int ivar, TYVARSYM ** prgvar, int ivarTot)
{
    ASSERT(symPar->isAGGSYM() || symPar->isMETHSYM());
    ASSERT(ivar <= ivarTot);

    if (tree->kind == NK_TYPEWITHATTR) {
        ASSERT(tree->asTYPEWITHATTR()->pAttr);
        tree = tree->asTYPEWITHATTR()->pType;
        ASSERT(tree && tree->kind != NK_TYPEWITHATTR);
    }

    NAME * ident;
    BASENODE * nodeName = tree;

    if (nodeName->kind == NK_NAMEDTYPE)
        nodeName = nodeName->asNAMEDTYPE()->pName;

    if (nodeName->kind == NK_NAME) {
        ident = nodeName->asNAME()->pName;
        if (nodeName->flags & NF_NAME_MISSING)
            goto LDblBreak;

        for (int i = 0; i < ivar; i++) {
            if (prgvar[i]->name == ident) {
                compiler()->Error(nodeName, ERR_DuplicateTypeParameter, ident);
                goto LDblBreak;
            }
        }

        if (ident == symPar->name) {
            compiler()->Error(nodeName, ERR_TypeVariableSameAsParent, ident);
            goto LDblBreak;
        }

        for (PARENTSYM * symOuter = symPar->parent; symOuter->isAGGSYM(); symOuter = symOuter->parent) {
            for (int i = 0; i < symOuter->asAGGSYM()->typeVarsThis->size; i++) {
                if (ident == symOuter->asAGGSYM()->typeVarsThis->Item(i)->name) {
                    compiler()->Error(nodeName, WRN_TypeParameterSameAsOuterTypeParameter,
                        ident, symOuter, ErrArgRefOnly(symOuter->asAGGSYM()->typeVarsThis->Item(i)));
                    goto LDblBreak;
                }
            }
        }
LDblBreak: ;
    }
    else {
        compiler()->Error(nodeName, ERR_TypeParamMustBeIdentifier);
        // Manufacture a bogus name.
        WCHAR sz[20];
        StringCchPrintfW(sz, lengthof(sz), L"?T%d?", ivarTot);
        ident = compiler()->namemgr->AddString(sz);
    }

    TYVARSYM * var = compiler()->getBSymmgr().CreateTyVar(ident, symPar);
    ASSERT(!var->isMethTyVar == !symPar->isMETHSYM());
    var->SetAccess(ACC_PRIVATE);
    var->index = ivar;
    var->indexTotal = ivarTot;
    var->parseTree = tree;
    var->attributeListTail = &var->attributeList;

    // bounds and ifacesAll get set later.
    ASSERT(!var->GetBaseCls() && !var->GetBnds() && !var->GetIfacesAll());

    return var;
}

// Parse the bounds defined in contraints for the type variables of parent.
void CLSDREC::DefineBounds(BASENODE * tree, PARENTSYM * symPar, bool fFirst)
{
    ASSERT(symPar->isAGGDECLSYM() || symPar->isMETHSYM());

    TypeArray * typeVars;
    PARENTSYM * symTypeVars;
    PARENTSYM * symLookup;

    switch (symPar->getKind()) {
    default:
        ASSERT(!"Bad SK in DefineBounds!");
        return;

    case SK_AGGDECLSYM:
        symTypeVars = symPar->asAGGDECLSYM()->Agg();
        typeVars = symTypeVars->asAGGSYM()->typeVarsThis;
        if (typeVars->size == 0)
            return;
        symLookup = symPar->asAGGDECLSYM()->DeclPar();
        break;

    case SK_METHSYM:
        typeVars = symPar->asMETHSYM()->typeVars;
        if (typeVars->size == 0)
            return;
        symTypeVars = symPar;
        symLookup = symPar->asMETHSYM()->declaration;
        break;
    }
    ASSERT(typeVars->size > 0);
    ASSERT(symLookup && symTypeVars);

    // Clear all the bits.
    for (int i = 0; i < typeVars->size; ++i) {
        typeVars->ItemAsTYVARSYM(i)->seenWhere = false;
    }

    TYPESYM * rgtype[8];
    TYPESYM ** prgtype = rgtype;
    int ctypeMax = lengthof(rgtype);

    NODELOOP(tree, CONSTRAINT, nodeCon)
        // find type par index
        int ivar;

        for (ivar = 0; ivar < typeVars->size; ivar++) {
            if (typeVars->Item(ivar)->name == nodeCon->pName->pName)
                break;
        }
        if (ivar >= typeVars->size) {
            compiler()->Error(nodeCon->pName, ERR_TyVarNotFoundInConstraint, nodeCon->pName, symPar);
            continue;
        }

        TYVARSYM * var = typeVars->ItemAsTYVARSYM(ivar);
        bool fAdd = true;

        AGGTYPESYM * atsClsBnd = NULL;
        int ctype = 0;
        bool fIface = false; // Whether there are any interface bounds.
        SpecConsEnum cons = SpecCons::None; // Special constraints.

        if (nodeCon->flags & NF_CONSTRAINT_REFTYPE)
            cons |= SpecCons::Ref;
        if (nodeCon->flags & NF_CONSTRAINT_VALTYPE)
            cons |= SpecCons::Val;
        if ((nodeCon->flags & NF_CONSTRAINT_NEWABLE) && !(cons & SpecCons::Val))
            cons |= SpecCons::New;

        if (var->seenWhere) {
            // We've already seen a where clause for this type variable.
            compiler()->Error(nodeCon, ERR_DuplicateConstraintClause, var);
            // Keep binding but don't add to the var - for error reporting.
            fAdd = false;
        }

        // Reserve the first slot for atsClsBnd.
        prgtype[ctype++] = NULL;

        // When the value type constraint is specified, atsClsBnd is System.ValueType.
        if (cons & SpecCons::Val)
            atsClsBnd = compiler()->GetReqPredefType(PT_VALUE);

        NODELOOP (nodeCon->pBounds, TYPEBASE, nodeType)
            TYPESYM * typeBnd = TypeBind::BindTypeWithTypeVars(compiler(), nodeType, symLookup, symPar, symTypeVars);
            ASSERT(typeBnd);

            if (typeBnd->isAGGTYPESYM()) {
                AGGSYM *aggBnd = typeBnd->asAGGTYPESYM()->getAggregate();

                VSVERIFY(ResolveInheritanceRec(aggBnd), "ResolveInheritanceRec failed in DefineBounds!");

                if (compiler()->CheckForStaticClass(nodeType, NULL, typeBnd, ERR_ConstraintIsStaticClass))
                    continue;

                if (!aggBnd->IsInterface() && (!aggBnd->IsClass() || aggBnd->isSealed)) {
                    compiler()->Error(nodeType, ERR_BadBoundType, typeBnd);
                    continue;
                }

                if (aggBnd->isPredefined) {
                    PREDEFTYPE pt = (PREDEFTYPE)aggBnd->iPredef;
                    if (pt == PT_OBJECT || pt == PT_ARRAY || pt == PT_VALUE || pt == PT_ENUM || pt == PT_DELEGATE || pt == PT_MULTIDEL) {
                        compiler()->Error(nodeType, ERR_SpecialTypeAsBound, typeBnd);
                        continue;
                    }
                }
            }
            else if (!typeBnd->isTYVARSYM()) {
                if (!typeBnd->isERRORSYM())
                    compiler()->Error(nodeType, ERR_BadBoundType, typeBnd);
                continue;
            }

            if (!fAdd)
                continue;

            if (typeBnd->isClassType()) {
                // There can only be one class bound and it should come first.
                if (cons & (SpecCons::Ref | SpecCons::Val)) {
                    compiler()->Error(nodeType, ERR_RefValBoundWithClass, typeBnd);
                    if (atsClsBnd)
                        continue;
                }
                if (atsClsBnd || ctype > 1) {
                    compiler()->Error(nodeType,
                        (atsClsBnd == typeBnd) ? ERR_DuplicateBound : ERR_ClassBoundNotFirst,
                        typeBnd, var);
                    if (atsClsBnd)
                        continue;
                }
                atsClsBnd = typeBnd->asAGGTYPESYM();
            }
            else {
                ASSERT(typeBnd->isInterfaceType() || typeBnd->isTYVARSYM());

                // Make sure that we don't have a duplicate.
                for (int itype = 1; itype < ctype; itype++) {
                    if (prgtype[itype] == typeBnd) {
                        compiler()->Error(nodeType, ERR_DuplicateBound, typeBnd, var);
                        goto LNextBnd;
                    }
                }

                // Add the bound to the array.
                if (ctype >= ctypeMax) {
                    // Need more space.
                    ASSERT(ctype == ctypeMax);
                    int ctypeMaxNew = ctypeMax * 2;
                    TYPESYM ** prgtypeNew = STACK_ALLOC(TYPESYM *, ctypeMaxNew);
                    memcpy(prgtypeNew, prgtype, ctypeMax * sizeof(prgtype[0]));
                    prgtype = prgtypeNew;
                    ctypeMax = ctypeMaxNew;
                }
                ASSERT(ctype < ctypeMax);

                prgtype[ctype++] = typeBnd;

                if (typeBnd->isInterfaceType())
                    fIface = true;
            }

LNextBnd:
            ;

        ENDLOOP;

        if (!fAdd)
            continue;

        ASSERT(ctype < ctypeMax);
        TYPESYM ** prgtypeUse = prgtype;

        ASSERT(ctype > 0 && !prgtype[0]);
        if (atsClsBnd)
            prgtype[0] = atsClsBnd;
        else {
            ctype--;
            prgtypeUse++;
        }

        TypeArray * bnds = compiler()->getBSymmgr().AllocParams(ctype, prgtypeUse);
#ifdef DEBUG
        // Check that they are all distinct.
        for (int i = 1; i < bnds->size; i++) {
            for (int j = 0; j < i; j++) {
                ASSERT(bnds->Item(i) != bnds->Item(j));
            }
        }
#endif // DEBUG

        if (fFirst) {
            ASSERT(!var->GetBnds());
            var->cons = cons;
            compiler()->SetBounds(var, bnds);
        }
        else {
            // Verify that nothing has changed.
            ASSERT(symPar->isAGGDECLSYM());
            ASSERT(var->FResolved());

            if (var->cons != (uint)cons) {
LBadPartial:
                compiler()->Error(NULL, ERR_PartialWrongConstraints, ErrArgRef(symTypeVars), var);
            }
            else if (var->GetBnds() != bnds) {
                // See if one is a permutation of the other.
                if (var->GetBnds()->size != bnds->size)
                    goto LBadPartial;
                for (int i = 0; i < bnds->size; i++) {
                    if (!var->GetBnds()->Contains(bnds->Item(i)))
                        goto LBadPartial;
                }
            }
        }
        var->seenWhere = true;
    ENDLOOP;

    // For any that we didn't see, set bounds to the default.
    for (int ivar = 0; ivar < typeVars->size; ++ivar) {
        TYVARSYM * var = typeVars->ItemAsTYVARSYM(ivar);

        // All of these should have parse trees.
        ASSERT(var->parseTree);

        if (var->seenWhere)
            continue;

        if (fFirst) {
            ASSERT(!var->GetBnds());
            ASSERT(!var->FResolved());
            compiler()->SetBounds(var, BSYMMGR::EmptyTypeArray());
        }
        else {
            // Verify that nothing has changed.
            ASSERT(symPar->isAGGDECLSYM());
            ASSERT(var->FResolved());
            ASSERT(var->GetBnds() != BSYMMGR::EmptyTypeArray() || var->GetIfacesAll() == BSYMMGR::EmptyTypeArray());

            if (var->GetBnds() != BSYMMGR::EmptyTypeArray() || var->cons != SpecCons::None) {
                compiler()->Error(NULL, ERR_PartialWrongConstraints, ErrArgRef(symTypeVars), var);
            }
        }
    }

    if (fFirst) {
        // Process any type variable bounds. Check for cycles and build all recursive info.
        for (int ivar = 0; ivar < typeVars->size; ivar++) {
            VSVERIFY(compiler()->ResolveBounds(typeVars->ItemAsTYVARSYM(ivar), false),
                "ResolveBounds failed!");
        }
    }
}


void CLSDREC::CheckBoundsVisibility(SYM * sym, TypeArray * typeVars)
{
    for (int i = 0; i < typeVars->size; i++) {
        TYVARSYM * var = typeVars->ItemAsTYVARSYM(i);
        TypeArray * bnds = var->GetBnds();
        for (int j = 0; j < bnds->size; j++) {
            TYPESYM * type = bnds->Item(j);
            checkConstituentVisibility(sym, type, ERR_BadVisBound);
        }
    }
}


/***************************************************************************************************
    Make sure the constraints match.
***************************************************************************************************/
void CLSDREC::CheckImplicitImplConstraints(MethWithType mwtImpl, MethWithType mwtBase)
{
    ASSERT(mwtImpl.Meth()->typeVars && mwtBase.Meth()->typeVars && mwtImpl.Meth()->typeVars->size == mwtBase.Meth()->typeVars->size);

    if (mwtImpl.Meth()->typeVars->size == 0)
        return;

    for (int i = 0; i < mwtImpl.Meth()->typeVars->size; i++) {
        TYVARSYM * var1 = mwtImpl.Meth()->typeVars->ItemAsTYVARSYM(i);
        TYVARSYM * var2 = mwtBase.Meth()->typeVars->ItemAsTYVARSYM(i);

        if (var1->cons != var2->cons) {
LMismatch:
            compiler()->Error(NULL, ERR_ImplBadConstraints, var1, ErrArgRef(mwtImpl), var2, ErrArgRef(mwtBase));
            continue;
        }

        TypeArray * bounds1 = var1->GetBnds();
        TypeArray * bounds2 = var2->GetBnds();

        // bounds1 and bounds2 must be equal (as sets) after substition. Some members of the bounds may unify during substitution.
        // Hence, it's possible for the bounds to have a different number of items and still match, so the following tempting
        // code would be a bug: "if (bounds1->size != bounds2->size) goto LMismatch;"

        // Substitute.
        bounds1 = compiler()->getBSymmgr().SubstTypeArray(bounds1, mwtImpl.Type());
        bounds2 = compiler()->getBSymmgr().SubstTypeArray(bounds2, mwtBase.Type(), mwtImpl.Meth()->typeVars);

        // Check for the easy case.
        if (bounds1 == bounds2)
            continue;

        // Make sure bounds1 is a subset of bounds2.
        for (int j = 0; j < bounds1->size; j++) {
            TYPESYM * type = bounds1->Item(j);
            if (bounds2->Contains(type) || type->isPredefType(PT_OBJECT))
                continue;
            // See if bounds1->Item(j) is a base type of something in bounds2.
            for (int k = 0; ; k++) {
                if (k >= bounds2->size)
                    goto LMismatch;
                if (compiler()->IsBaseType(bounds2->Item(k), type))
                    break;
            }
        }

        // Make sure bounds2 is a subset of bounds1.
        for (int j = 0; j < bounds2->size; j++) {
            TYPESYM * type = bounds2->Item(j);
            if (bounds1->Contains(type) || type->isPredefType(PT_OBJECT))
                continue;
            // See if bounds2->Item(j) is a base type of something in bounds1.
            for (int k = 0; ; k++) {
                if (k >= bounds1->size)
                    goto LMismatch;
                if (compiler()->IsBaseType(bounds1->Item(k), type))
                    break;
            }
        }
    }
}


NSDECLSYM * CLSDREC::addNamespaceDeclaration(NAMENODE* name, NAMESPACENODE *parseTree, NSDECLSYM *containingDeclaration)
{
    //
    // create the namespace symbol
    //
    NSSYM *nspace = addNamespace(name, containingDeclaration);
    if (!nspace) {
        return NULL;
    }

    //
    // create the new declaration and link it in
    //
    return compiler()->getBSymmgr().CreateNamespaceDecl(
                nspace, 
                containingDeclaration,
                containingDeclaration->inputfile, 
                parseTree);
}


// add a namespace symbol to the given parent.  checks for collisions with
// classes, and displays an error if so.  returns a namespace regardless.
NSSYM * CLSDREC::addNamespace(NAMENODE * name, NSDECLSYM * parent)
{
    NAME * ident = name->pName;

    // Check for existing namespace.
    for (SYM * sym = compiler()->LookupGlobalSym(ident, parent->NameSpace(), MASK_ALL);
        sym;
        sym = sym->nextSameName)
    {
        if (sym->isNSSYM())
            return sym->asNSSYM();

        ASSERT(!sym->isAGGSYM() || !sym->asAGGSYM()->isArityInName || sym->asAGGSYM()->typeVarsThis->size > 0);
        if (sym->isAGGSYM() && sym->asAGGSYM()->InAlias(kaidThisAssembly) && !sym->asAGGSYM()->isArityInName) {
            compiler()->Error(name, ERR_DuplicateNameInNS, name, parent->NameSpace(), ErrArgRefOnly(sym));
            return NULL;
        }
    }

    // Create new namespace.
    return compiler()->getBSymmgr().CreateNamespace(ident, parent->NameSpace());
}


// declares a class or struct.  This means that this aggregate and any contained aggregates
// have their symbols entered in the symbol table. 
// the access modifier on the type is also checked, and the type
// variables get created (in AddAggregate).
void CLSDREC::declareAggregate(AGGREGATENODE * pClassTree, DECLSYM * parentDecl)
{
    ASSERT(parentDecl->isAGGDECLSYM() || parentDecl->isNSDECLSYM());

    AGGDECLSYM * clsdecl = AddAggregate(pClassTree, pClassTree->pName, parentDecl);

    if (clsdecl) {
        AGGSYM * cls = clsdecl->Agg();

        //
        // declare all nested types
        //
        //
        // parser guarantees that enums don't contain nested types
        //
        if (!cls->IsEnum() && !cls->IsDelegate()) {
            MEMBERNODE * members = pClassTree->pMembers;
            while (members) {

                SETLOCATIONNODE(members);

                if (members->kind == NK_NESTEDTYPE) {
                    if (!cls->IsInterface()) {
                        switch(members->asNESTEDTYPE()->pType->kind) {                        
                        case NK_CLASS:
                        case NK_STRUCT:
                        case NK_INTERFACE:
                        case NK_ENUM:
                            declareAggregate(members->asNESTEDTYPE()->pType->asAGGREGATE(), clsdecl);
                            break;
                        case NK_DELEGATE:
                        {
                            DELEGATENODE *delegateNode = members->asNESTEDTYPE()->pType->asDELEGATE();
                            AddAggregate(delegateNode, delegateNode->pName, clsdecl);
                            break;
                        }

                        default:
                            ASSERT(!"Unknown aggregate type");
                        }
                    } else {
                        NAMENODE * name;

                        if (members->asNESTEDTYPE()->pType->kind == NK_DELEGATE)
                            name = members->asNESTEDTYPE()->pType->asDELEGATE()->pName;
                        else
                            name = members->asNESTEDTYPE()->pType->asAGGREGATE()->pName;

                        compiler()->Error(name, ERR_InterfacesCannotContainTypes, name);
                    }
                }

                members = members->pNext;
            }
        }
    }
}


// prepare a namespace for compilation.  this merely involves preparing all
// the namespace elements.  nspace is the symbol for THIS namespace, not its
// parent...
void CLSDREC::prepareNamespace(NSDECLSYM *nsDeclaration)
{
    if (!nsDeclaration) return;

    ASSERT(nsDeclaration->isDefined);

    SETLOCATIONSYM(nsDeclaration);

    //
    // prepare members
    //
    FOREACHCHILD(nsDeclaration, elem)
        switch (elem->getKind()) {
        case SK_NSDECLSYM:
            prepareNamespace(elem->asNSDECLSYM());
            break;
        case SK_AGGDECLSYM:
            if (elem->asAGGDECLSYM()->IsFirst())
                prepareAggregate(elem->asAGGDECLSYM()->Agg());
            break;
        case SK_GLOBALATTRSYM:
            break;
        default:
            ASSERT(!"Unknown namespace member");
        }
    ENDFOREACHCHILD
}

// ensures that the using clauses for this namespace declaration
// have been resolved. This can happen between declare and define
// steps.
void CLSDREC::ensureUsingClausesAreResolved(NSDECLSYM * nsd)
{
    if (nsd->usingClausesResolved)
        return;

    nsd->usingClausesResolved = true;

    ASSERT(!nsd->usingClauses);
    SYMLIST ** plstAddList = &nsd->usingClauses;

    // First time through the loop, add all the extern aliases.
    // Second time add the normal using clauses and aliases. This is so the normal ones
    // can use the externs.
    for (bool fExtern = true; ; ) {
        SYMLIST * lstFirst = NULL;
        SYMLIST ** plstAdd = &lstFirst;

        NODELOOP(nsd->parseTree->pUsing, USING, use)
            SYM * symAdd;

            if (!use->pAlias) {
                if (fExtern)
                    continue;

                // Find the type or namespace for the using clause.
                BASENODE * nodeName = use->pName;
                symAdd = TypeBind::BindName(compiler(), nodeName, nsd);
                ASSERT(symAdd);
                if (symAdd->isERRORSYM()) {
                    // couldn't find the type/namespace
                    // error already reported
                    continue;
                }
                ASSERT(symAdd->isAGGTYPESYM() || symAdd->isNSAIDSYM());
                if (!symAdd->isNSAIDSYM()) {
                    compiler()->Error(nodeName, ERR_BadUsingNamespace, ErrArgNameNode(nodeName), ErrArgRefOnly(symAdd));
                    continue;
                }

                // Check for duplicate using clauses.
                if (lstFirst->contains(symAdd)) {
                    compiler()->Error(nodeName, WRN_DuplicateUsing, ErrArgNameNode(nodeName));
                    continue;
                }
            }
            else {
                if (!use->pName != fExtern)
                    continue;

                NAME * name = use->pAlias->pName;
                SYM * symDup = NULL;

                // Check for duplicates in externs.
                FOREACHSYMLIST(nsd->usingClauses, symT, SYM)
                    if (symT->isALIASSYM() && symT->name == name) {
                        symDup = symT;
                        break;
                    }
                ENDFOREACHSYMLIST;
                if (!symDup) {
                    // Check for duplicates in the current list.
                    FOREACHSYMLIST(lstFirst, symT, SYM)
                        if (symT->isALIASSYM() && symT->name == name) {
                            symDup = symT;
                            break;
                        }
                    ENDFOREACHSYMLIST;
                }
                if (symDup) {
                    compiler()->Error(use->pAlias, ERR_DuplicateAlias, use->pAlias, ErrArgRefOnly(symDup));
                    continue;
                }

                for (symDup = compiler()->LookupInBagAid(name, nsd->NameSpace(), kaidGlobal, MASK_ALL);
                    symDup;
                    symDup = compiler()->LookupNextInAid(symDup, kaidGlobal, MASK_ALL))
                {
                    if (symDup->isAGGSYM() && symDup->asAGGSYM()->typeVarsThis->size > 0)
                        continue;
                    if (CheckAccess(symDup, NULL, nsd, NULL))
                        break;
                }

                ALIASSYM * alias = compiler()->getBSymmgr().CreateAlias(name);
                alias->parseTree = use;
                alias->parent = nsd;
                alias->fExtern = fExtern;
                alias->symDup = symDup;

                symAdd = alias;
            }

            compiler()->compileCallback.ResolvedUsingNode(nsd, use, symAdd);
            compiler()->getBSymmgr().AddToGlobalSymList(symAdd, &plstAdd);
        ENDLOOP;

        ASSERT(!*plstAddList);
        if (lstFirst) {
            // Add what we've accumulated so far.
            *plstAddList = lstFirst;
            // Next time we should add the new list to the end of this list.
            plstAddList = plstAdd;
        }
        ASSERT(!*plstAddList);

        if (!fExtern)
            break;
        fExtern = false;
    }
}


/***************************************************************************************************
    Returns false if an error was detected that could not be fixed (faked away). For example:

    * if the AGGSYM is already having its inheritance resolved this just returns false.
    * if the AGGSYM is nested in a type having its inheritacne resolved, this resolves the
      inheritance of the inner type and returns false. In this case the error is reported on the
      inner type. The dependance can't be fixed there since the outer type is still having its
      inheritance resolved.
***************************************************************************************************/
bool CLSDREC::ResolveInheritanceRec(AGGSYM *cls)
{
    bool fRet = true;

    // Check if we're done already.
    if (cls->HasResolvedBaseClasses())
        return true;

    ASSERT(!cls->isPredefAgg(PT_OBJECT));

    // Check for cycles. The error messages are reported elsewhere.
    if (cls->IsResolvingBaseClasses())
        return false;

    SETLOCATIONSYM(cls);

    if (!cls->isSource) {
        // Imported
        ASSERT(cls->AggState() <= AggState::Declared);

        if (cls->IsUnresolved()) {
            // This class appears to be neither imported nor defined in source.
            compiler()->UndeclarableType(cls->AsUnresolved());
            ASSERT(cls->HasResolvedBaseClasses());
            return fRet;
        }

        ASSERT(cls->AggState() == AggState::Declared);

        // For imported types we need to allow an outer type to depend on an inner type, so we
        // don't advance to the resolving state until after we've resolved the outer type.

        if (cls->isNested() && !cls->Parent()->asAGGSYM()->IsResolvingBaseClasses()) {
            fRet &= ResolveInheritanceRec(cls->Parent()->asAGGSYM());
            if (cls->HasResolvedBaseClasses())
                return fRet;
        }

        ASSERT(cls->AggState() == AggState::Declared);
        cls->SetAggState(AggState::ResolvingInheritance);

        compiler()->importer.ResolveInheritance(cls);
        ASSERT(cls->HasResolvedBaseClasses());
    }
    else {
        // For source types it's an error for an outer type to depend on an inner type.
        ASSERT(cls->AggState() == AggState::Declared);
        cls->SetAggState(AggState::ResolvingInheritance);

        // Resolve our containing class.
        if (cls->isNested()) {
            if (!cls->Parent()->asAGGSYM()->IsResolvingBaseClasses())
                fRet &= ResolveInheritanceRec(cls->Parent()->asAGGSYM());
            else {
                compiler()->ErrorRef(NULL, ERR_CircularBase, cls->Parent(), cls);
                // Need to break the dependence to avoid infinite recursion elsewhere.
                fRet = false;
            }
        }

        if (cls->isFabricated) {
            ASSERT(!cls->hasParseTree);
            ASSERT(cls->ifaces && cls->ifacesAll);
            ASSERT(cls->baseClass);
        }
        else if (cls->IsEnum()) {
            // Get the underlying type. Enums can't be partial, so using DeclOnly is OK here.
            BASENODE * treeBase = cls->DeclOnly()->parseTree->asAGGREGATE()->pBases;
            PREDEFTYPE ptBase = PT_INT;

            if (treeBase) {
                if (treeBase->kind == NK_PREDEFINEDTYPE)
                    ptBase = (PREDEFTYPE)treeBase->asPREDEFINEDTYPE()->iType;
                else {
                    // Parser should have generated an error.
                    ASSERT(compiler()->ErrorCount() > 0);
                    TYPESYM * typeBase = TypeBind::BindTypeAggDeclExt(compiler(), treeBase->asANYTYPE(), cls->DeclOnly());
                    if (typeBase->isPredefined())
                        ptBase = (PREDEFTYPE)typeBase->getAggregate()->iPredef;
                }

                switch (ptBase) {
                case PT_BYTE:
                case PT_SHORT:
                case PT_INT:
                case PT_LONG:
                case PT_SBYTE:
                case PT_USHORT:
                case PT_UINT:
                case PT_ULONG:
                    break;
                default:
                    // Parser should have errored on this already.
                    ASSERT(compiler()->ErrorCount() > 0);
                    ptBase = PT_INT;
                    break;
                }
            }

            AGGSYM * aggBase = compiler()->GetReqPredefAgg(ptBase, false);

            VSVERIFY(ResolveInheritanceRec(aggBase), "ResolveInheritanceRec failed on a predefined type!");
            cls->underlyingType = aggBase->getThisType();

            // Enums should derive from "Enum".
            AGGSYM * aggEnum = compiler()->GetReqPredefAgg(PT_ENUM, false);
            VSVERIFY(ResolveInheritanceRec(aggEnum), "ResolveInheritanceRec failed on PT_ENUM!");
            compiler()->SetBaseType(cls, aggEnum->getThisType());
            compiler()->SetIfaces(cls, NULL, 0);
        }
        else if (cls->IsDelegate()) {
            // all delegates in C# are multicast
            AGGSYM * aggDel = compiler()->GetReqPredefAgg(PT_MULTIDEL, false);
            VSVERIFY(ResolveInheritanceRec(aggDel), "ResolveInheritanceRec failed on PT_MUTLIDEL!");
            compiler()->SetBaseType(cls, aggDel->getThisType());
            compiler()->SetIfaces(cls, NULL, 0);
        }
        else {
            ASSERT(cls->hasParseTree);
            // Note - we do not declare the types and other symbols 
            // involved in the constraint until
            // right at the very end when we are trying to satisfy the constraints.
            // That is, the type symbol gets fully created by the step below, but 
            // not all the aggregate types involved in the type symbol get
            // prepared/declared, because we won't need that information until 
            // we try to satisfy the constraints (e.g. then we might need to know
            // subtyping information etc.)

            // Resolve base class and implemented interfaces.
            AGGTYPESYM * rgiface[8];
            int cifaceMax = lengthof(rgiface);
            AGGTYPESYM ** prgiface = rgiface;
            int ciface = 0;

            AGGTYPESYM * rgifaceThis[8];
            int cifaceMaxThis = lengthof(rgifaceThis);
            AGGTYPESYM ** prgifaceThis = rgifaceThis;

            AGGTYPESYM * baseClass = NULL;

            // Make sure that the base is set for structs.
            if (cls->IsStruct()) {
                baseClass = compiler()->GetReqPredefType(PT_VALUE, false);
                VSVERIFY(ResolveInheritanceRec(baseClass->getAggregate()), "ResolveInheritanceRec failed on PT_VALUE!");
            }

            // Examine all the base class/interface declaration, from all partial classes, and combine
            // them, giving an error on any duplicates.
            FOREACHAGGDECL(cls, clsdecl)
                BASENODE * bases = clsdecl->parseTree->asAGGREGATE()->pBases;

                bool fFirst = cls->IsClass();
                bool fSeenBaseOnThisDecl = false;
                int cifaceThis = 0;

                for (BASENODE * list = bases; list; fFirst = false) {
                    TYPEBASENODE * base;
                    if (list->kind == NK_LIST) {
                        base = list->asLIST()->p1->asTYPEBASE();
                        list = list->asLIST()->p2;
                    }
                    else {
                        base = list->asTYPEBASE();
                        list = NULL;
                    }

                    // Search in type variables of cls first, then the containing declaration.
                    TYPESYM * typeBase = TypeBind::BindTypeAggDeclExt(compiler(), base, clsdecl);
                    ASSERT(typeBase);
                    if (typeBase->isERRORSYM()) {
                        // Already reported error in BindType....
                        continue;
                    }

                    if (!typeBase->isAGGTYPESYM()) {
                        if (typeBase->isTYVARSYM()) {
                            compiler()->Error(base, ERR_DerivingFromATyVar, ErrArgTypeNode(base));
                        } else {
                            ASSERT(!typeBase->isERRORSYM());
                            compiler()->Error(base, ERR_BadBaseType);
                        }
                        continue;
                    }

                    AGGTYPESYM * atsBase = typeBase->asAGGTYPESYM();
                    AGGSYM * aggBase = atsBase->getAggregate();

                    if (!ResolveInheritanceRec(aggBase) && !aggBase->IsResolvingBaseClasses()) {
                        // Error should have already been reported. Fix the dependency by not using this base.
                        continue;
                    }

                    if (!aggBase->IsInterface()) {
                        if (!fFirst) {
                            // Found non-interface where interface was expected.
                            if (cls->IsClass() && aggBase->IsClass()) {
                                if (!fSeenBaseOnThisDecl) {
                                    // The base class wasn't first in the list. Give an error, fix it up, and go on.
                                    compiler()->Error(base, ERR_BaseClassMustBeFirst, ErrArgTypeNode(base), ErrArgRefOnly(aggBase));
                                }
                                else {
                                    // We have multiple base classes. Ignore the second one.
                                    compiler()->Error(base, ERR_NoMultipleInheritance, cls, baseClass, ErrArgTypeNode(base), ErrArgRefOnly(aggBase));
                                    continue;
                                }
                            }
                            else {
                                // This is either a struct or an interface or the base type is not a class.
                                compiler()->Error(base, ERR_NonInterfaceInInterfaceList, ErrArgTypeNode(base), ErrArgRefOnly(aggBase));
                                continue;
                            }
                        }

                        if (aggBase->isPredefined &&
                            (aggBase->iPredef == PT_ENUM || aggBase->iPredef == PT_VALUE ||
                                aggBase->iPredef == PT_DELEGATE  || aggBase->iPredef == PT_MULTIDEL || 
                                (aggBase->iPredef == PT_ARRAY && !compiler()->IsBuildingMSCORLIB())) && 
                            !cls->isPredefined)
                        {
                            compiler()->ErrorRef(NULL, ERR_DeriveFromEnumOrValueType, cls, aggBase);
                            continue;
                        }

                        if (aggBase->IsResolvingBaseClasses()) {
                            compiler()->ErrorRef(NULL, ERR_CircularBase, cls, atsBase);
                            continue;
                        }

                        // Base class.
                        if (baseClass == atsBase) {
                            fSeenBaseOnThisDecl = true;
                            continue;
                        }

                        if (baseClass) {
                            compiler()->ErrorRef(NULL, ERR_PartialMultipleBases, cls);
                            continue;
                        }

                        if (cls->IsStatic() && !atsBase->isPredefType(PT_OBJECT)) {
                            compiler()->Error(base, ERR_StaticDerivedFromNonObject, cls, atsBase);
                            continue;
                        }

                        // Generics cannot be attributes.
                        if (cls->typeVarsAll->size && aggBase->isAttribute) {
                            compiler()->Error(base, ERR_GenericDerivingFromAttribute, ErrArgTypeNode(base));
                            // Set it anyway.
                        }

                        // Cannot derive from static or sealed types
                        if (!compiler()->CheckForStaticClass(NULL, cls, typeBase, ERR_StaticBaseClass) && aggBase->isSealed) {
                            compiler()->ErrorRef(NULL, ERR_CantDeriveFromSealedType, cls, atsBase);    
                            // Set it anyway.
                        }

                        // make sure base class is at least as visible as derived class.
                        checkConstituentVisibility(cls, atsBase, ERR_BadVisBaseClass);

                        fSeenBaseOnThisDecl = true;
                        baseClass = atsBase;
                        continue;
                    }

                    // Interfaces.
                    ASSERT(aggBase->IsInterface());
                    if (cls->IsStatic()) {
                        compiler()->ErrorRef(NULL, ERR_StaticClassInterfaceImpl, atsBase, cls);
LBadIface:
                        continue;
                    }

                    // found an interface, check that it wasn't listed twice. We check for unification
                    // later - when we build the combined list.
                    for (int i = 0; i < cifaceThis; i++) {
                        if (prgifaceThis[i] == atsBase) {
                            // Only report the error on this decl.
                            compiler()->Error(base, ERR_DuplicateInterfaceInBaseList, ErrArgTypeNode(base));
                            goto LBadIface;
                        }
                    }

                    if (aggBase->IsResolvingBaseClasses()) {
                        // Found a cycle, report error and don't add interface to list.
                        compiler()->ErrorRef(NULL, ERR_CycleInInterfaceInheritance, atsBase, cls);
                        continue;
                    }

                    // found an interface, check that it wasn't listed twice. We check for unification
                    // later - when we build the combined list.
                    for (int i = 0; ; i++) {
                        if (i >= ciface) {
                            // Add it to the total list.
                            if (ciface >= cifaceMax) {
                                ASSERT(ciface == cifaceMax);
                                int cifaceMaxNew = cifaceMax * 2;
                                AGGTYPESYM ** prgifaceNew = STACK_ALLOC(AGGTYPESYM *, cifaceMaxNew);
                                memcpy(prgifaceNew, prgiface, cifaceMax * sizeof(TYPESYM *));
                                prgiface = prgifaceNew;
                                cifaceMax = cifaceMaxNew;
                            }
                            ASSERT(ciface < cifaceMax);
                            prgiface[ciface++] = atsBase;
                            break;
                        }
                        if (prgiface[i] == atsBase) {
                            // Already in the list - don't add it to the total list.
                            break;
                        }
                    }

                    // Add it the list for this decl.
                    if (cifaceThis >= cifaceMaxThis) {
                        ASSERT(cifaceThis == cifaceMaxThis);
                        int cifaceMaxNew = cifaceMaxThis * 2;
                        AGGTYPESYM ** prgifaceNew = STACK_ALLOC(AGGTYPESYM *, cifaceMaxNew);
                        memcpy(prgifaceNew, prgifaceThis, cifaceMaxThis * sizeof(TYPESYM *));
                        prgifaceThis = prgifaceNew;
                        cifaceMaxThis = cifaceMaxNew;
                    }
                    ASSERT(cifaceThis < cifaceMaxThis);
                    prgifaceThis[cifaceThis++] = atsBase;

                    // Make sure base interface is at least as visible as derived interface (don't check
                    // for derived CLASS, though).
                    if (cls->IsInterface()) {
                        checkConstituentVisibility(cls, atsBase, ERR_BadVisBaseInterface);
                    }
                }
            ENDFOREACHAGGDECL

            if (!baseClass && cls->IsClass())
                baseClass = compiler()->GetReqPredefType(PT_OBJECT, false);

            compiler()->SetIfaces(cls, prgiface, ciface);
            compiler()->SetBaseType(cls, baseClass);
        }

        ASSERT(cls->AggState() == AggState::ResolvingInheritance);
        cls->SetAggState(AggState::Inheritance);
    }

    ASSERT(cls->ifaces && cls->ifacesAll);

    // Set inherited bits.
    if (cls->baseClass) {
        AGGSYM * aggBase = cls->baseClass->getAggregate();
        ASSERT(aggBase->HasResolvedBaseClasses());
        if (aggBase->isAttribute)
            cls->isAttribute = true;
        if (aggBase->isSecurityAttribute)
            cls->isSecurityAttribute = true;
        if (aggBase->isMarshalByRef)
            cls->isMarshalByRef = true;
    }

    return fRet;
}


void CLSDREC::ResolveInheritance(AGGSYM *cls)
{
    ASSERT(cls->AggState() == AggState::Declared || cls->AggState() == AggState::Inheritance);
    VSVERIFY(ResolveInheritanceRec(cls), "ResolveInheritanceRec failed!");
    ASSERT(cls->AggState() == AggState::Inheritance);

    //
    // resolve nested types, interfaces will have no members at this point
    //
    FOREACHCHILD(cls, elem)

        SETLOCATIONSYM(elem);

        // should only have types at this point
        switch (elem->getKind()) {
        case SK_AGGSYM:
            ASSERT(elem->asAGGSYM()->AggKind() > AggKind::Unknown && elem->asAGGSYM()->AggKind() < AggKind::Lim);
            ResolveInheritance(elem->asAGGSYM());
            break;

        case SK_AGGTYPESYM:
        case SK_TYVARSYM:
            break;

        default:
            ASSERT(!"Unknown member");
        }
    ENDFOREACHCHILD

    ASSERT(cls->AggState() == AggState::Inheritance);
}

// Resolves the inheritance hierarchy for a namespace's types.
void CLSDREC::ResolveInheritance(NSDECLSYM *nsDeclaration)
{
    if (!nsDeclaration)
        return;

    SETLOCATIONSYM(nsDeclaration);

    //
    // ensure the using clauses have been done
    // Note that this can happen before we get here
    // if a class in another namespace which is derived
    // from a class in this namespace is defined before us.
    //
    ensureUsingClausesAreResolved(nsDeclaration);

    //
    // and define contained types and namespaces...
    //
    FOREACHCHILD(nsDeclaration, elem)
        switch (elem->getKind()) {
        case SK_NSDECLSYM:
            ResolveInheritance(elem->asNSDECLSYM());
            break;
        case SK_AGGDECLSYM:
            if (elem->asAGGDECLSYM()->IsFirst())
                ResolveInheritance(elem->asAGGDECLSYM()->Agg());
            break;
        case SK_GLOBALATTRSYM:
            break;
        default:
            ASSERT(!"Unknown type");
        }
    ENDFOREACHCHILD
}

//
// define bounds for all types in a namespace Declaration
//
void CLSDREC::defineBounds(NSDECLSYM *nsDeclaration)
{
    if (!nsDeclaration)
        return;
        
    SETLOCATIONSYM(nsDeclaration);

    FOREACHCHILD(nsDeclaration, elem)
        switch (elem->getKind()) {
        case SK_NSDECLSYM:
            defineBounds(elem->asNSDECLSYM());
            break;
        case SK_AGGDECLSYM:
            if (elem->asAGGDECLSYM()->IsFirst())
                defineBounds(elem->asAGGDECLSYM()->Agg());
            break;
        case SK_GLOBALATTRSYM:
            break;
        default:
            ASSERT(!"Unknown type");
        }
    ENDFOREACHCHILD
}

//
// define bounds for this type and it's nested types
//
void CLSDREC::defineBounds(AGGSYM *cls)
{
    //
    // determine bounds for this type
    //
    if (cls->typeVarsThis->size) {
        if (cls->hasParseTree) {
            if (cls->IsDelegate()) {
                // OK to get DeclOnly, because delegates can't be partial.
                DELEGATENODE *delegateNode = cls->DeclOnly()->parseTree->asDELEGATE();
                DefineBounds(delegateNode->pConstraints, cls->DeclOnly(), true);

            } else {
                ASSERT(cls->AggKind() == AggKind::Class || cls->AggKind() == AggKind::Struct || cls->AggKind() == AggKind::Interface);
                ASSERT(!cls->isFabricated);
                
                // Handle bounds in each declaration and check that they are the same. Just call DefineBounds for
                // each. DefineBounds checks for consistency.
                bool fFirst = true;

                FOREACHAGGDECL(cls, clsdecl)
                    BASENODE * treeBnds = clsdecl->parseTree->asAGGREGATE()->pConstraints;
                    if (treeBnds) {
                        DefineBounds(treeBnds, clsdecl, fFirst);
                        fFirst = false;
                    }
                ENDFOREACHAGGDECL;
                if (fFirst) {
                    // None had constraints so call DefineBounds with a NULL constraints tree.
                    DefineBounds(NULL, cls->DeclFirst(), fFirst);
                }
            }

            CheckBoundsVisibility(cls, cls->typeVarsThis);
        } else {
            ASSERT(cls->typeVarsThis->ItemAsTYVARSYM(0)->FResolved());
        }
    }

    //
    // determine bounds for nested types
    //
    FOREACHCHILD(cls, elem)

        SETLOCATIONSYM(elem);

        // should only have types at this point
        switch (elem->getKind()) {
        case SK_AGGSYM:
            ASSERT(elem->asAGGSYM()->AggKind() > AggKind::Unknown && elem->asAGGSYM()->AggKind() < AggKind::Lim);
            defineBounds(elem->asAGGSYM());
            break;

        case SK_AGGTYPESYM:
        case SK_TYVARSYM:
            break;

        default:
            ASSERT(!"Unknown member");
        }
    ENDFOREACHCHILD
}


void CLSDREC::CheckForTypeErrors(NSDECLSYM * nsd)
{
    if (!nsd)
        return;

    SETLOCATIONSYM(nsd);

    FOREACHCHILD(nsd, elem)
        switch (elem->getKind()) {
        case SK_NSDECLSYM:
            CheckForTypeErrors(elem->asNSDECLSYM());
            break;
        case SK_AGGDECLSYM:
            if (elem->asAGGDECLSYM()->IsFirst())
                CheckForTypeErrors(elem->asAGGDECLSYM()->Agg());
            break;
        case SK_GLOBALATTRSYM:
            break;
        default:
            ASSERT(!"Unknown type");
            break;
        }
    ENDFOREACHCHILD
}

void CLSDREC::CheckForTypeErrors(AGGSYM * agg)
{
    ASSERT(agg->IsPrepared());

    if (!agg->hasParseTree)
        return;

    SETLOCATIONSYM(agg);

    // We would really like to report the errors only on the partial class that actually declared
    // the element that is in error, but there doesn't seem to be a way to find out this information,
    // so report the error on all the declarations by passing NULL as the tree.
    AGGDECLSYM * decl = agg->DeclFirst();
    BASENODE * tree = decl->DeclNext() ? NULL : decl->parseTree;

    if (agg->baseClass)
        CheckForTypeErrors(tree, agg, agg->baseClass);

    for (int i = 0; i < agg->ifaces->size; i++) {
        AGGTYPESYM * iface = agg->ifaces->Item(i)->asAGGTYPESYM();
        CheckForTypeErrors(tree, agg, iface);
    }

    for (int i = 0; i < agg->typeVarsThis->size; i++) {
        TypeArray * bnds = agg->typeVarsThis->Item(i)->asTYVARSYM()->GetBnds();
        for (int j = 0; j < bnds->size; j++) {
            CheckForTypeErrors(tree, agg, bnds->Item(j));
        }
    }

    if (agg->IsDelegate()) {
        METHSYM * invokeMethod = compiler()->getBSymmgr().LookupInvokeMeth(agg);
        CheckForTypeErrors(tree, agg, invokeMethod);
    }

    FOREACHCHILD(agg, elem)

        SETLOCATIONSYM(elem);

        switch (elem->getKind()) {
        case SK_AGGSYM:
            CheckForTypeErrors(elem->asAGGSYM());
            break;

        case SK_METHSYM:
            switch (elem->asMETHSYM()->MethKind()) {
            default:
                ASSERT(0);
            case MethodKind::None:
            case MethodKind::Ctor:
            case MethodKind::Dtor:
            case MethodKind::ExplicitConv:
            case MethodKind::ImplicitConv:
            case MethodKind::Anonymous:
                CheckForTypeErrors(elem->asMETHPROPSYM()->parseTree, elem, elem->asMETHPROPSYM());
                break;

            case MethodKind::PropAccessor:
            case MethodKind::EventAccessor:
            case MethodKind::Invoke:
                break;
            }
            break;

        case SK_PROPSYM:
            CheckForTypeErrors(elem->asMETHPROPSYM()->parseTree, elem, elem->asMETHPROPSYM());
            break;

        case SK_EVENTSYM:
            CheckForTypeErrors(elem->asEVENTSYM()->parseTree, elem, elem->asEVENTSYM()->type);
            break;

        case SK_MEMBVARSYM:
            if (!elem->asMEMBVARSYM()->isEvent)
                CheckForTypeErrors(elem->asMEMBVARSYM()->parseTree, elem, elem->asMEMBVARSYM()->type);
            break;

        default:
            break;
        }
    ENDFOREACHCHILD
}


void CLSDREC::CheckForTypeErrors(BASENODE * tree, SYM * symCtx, METHPROPSYM * mps)
{
    if (mps->isMETHSYM()) {
        // Check the type variable bounds.
        METHSYM * meth = mps->asMETHSYM();
        for (int i = 0; i < meth->typeVars->size; i++) {
            TypeArray * bnds = meth->typeVars->ItemAsTYVARSYM(i)->GetBnds();
            for (int j = 0; j < bnds->size; j++) {
                CheckForTypeErrors(tree, symCtx, bnds->Item(j));
            }
        }
    }

    // Check the return type.
    CheckForTypeErrors(tree, symCtx, mps->retType);

    // Check the parameter types.
    for (int i = 0; i < mps->params->size; i++)
        CheckForTypeErrors(tree, symCtx, mps->params->Item(i));
}


// Checks the TYPESYM for bogus, deprecated and constraints.
// If tree is NULL, symCtx is also used for reporting the error.
void CLSDREC::CheckForTypeErrors(BASENODE * tree, SYM * symCtx, TYPESYM * type)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::PostPrepare);
    compiler()->EnsureState(type);
    ASSERT(type->IsPrepared());

    TYPESYM * typeNaked = type->GetNakedType(false);

    // Convert NUBSYM to AGGTYPESYM.
    if (typeNaked->isNUBSYM()) {
        typeNaked = typeNaked->asNUBSYM()->GetAts();
        if (!typeNaked) {
            VSFAIL("Why is there a NUBSYM here when Nullable<T> doesn't exist?");
            return;
        }
    }

    if (typeNaked->isAGGTYPESYM() && typeNaked->asAGGTYPESYM()->typeArgsAll->size > 0) {
        // Check the type arguments first.
        TypeArray * typeArgs = typeNaked->asAGGTYPESYM()->typeArgsAll;
        for (int i = 0; i < typeArgs->size; i++) {
            CheckForTypeErrors(tree, symCtx, typeArgs->Item(i));
        }
    }

    if (compiler()->CheckBogus(type)) {
        if (tree == NULL)
            compiler()->ErrorRef(NULL, ERR_BogusType, ErrArgRefOnly(symCtx), type);
        else
            compiler()->ErrorRef(tree, ERR_BogusType, type);
    }
    else {
        if (type->isPTRSYM())
            checkUnmanaged(tree, type);
        // Don't report deprecated warning if any of our containing classes are deprecated.
        if (type->IsDeprecated() && !symCtx->isContainedInDeprecated()) {
            ReportDeprecated(tree, symCtx, SymWithType(type, NULL));
        }
        TypeBind::CheckConstraints(compiler(), tree, type, CheckConstraintsFlags::Outer);
    }
}


// define a namespace by binding its name to a symbol and resolving the uses
// clauses in this namespace declaration.  nspace indicates the PARENT namaspace
void CLSDREC::defineNamespace(NSDECLSYM *nsDeclaration)
{
    SETLOCATIONSYM(nsDeclaration);

    ensureUsingClausesAreResolved(nsDeclaration);
    ASSERT(nsDeclaration->usingClausesResolved);

    //
    // force binding of using aliases. Up to this point they are lazily bound
    //
    FOREACHSYMLIST(nsDeclaration->usingClauses, usingSym, SYM)
        if (usingSym->isALIASSYM()) {
            TypeBind::BindUsingAlias(compiler(), usingSym->asALIASSYM());
        }
    ENDFOREACHSYMLIST

    //
    // handle global attributes
    //
    NODELOOP(nsDeclaration->parseTree->pGlobalAttr, ATTRDECL, attr)
        declareGlobalAttribute(attr, nsDeclaration);
    ENDLOOP;

    //
    // and define contained types and namespaces...
    //
    FOREACHCHILD(nsDeclaration, elem)
        switch (elem->getKind()) {
        case SK_NSDECLSYM:
            defineNamespace(elem->asNSDECLSYM());
            break;
        case SK_AGGDECLSYM:
            if (elem->asAGGDECLSYM()->IsFirst())
                defineAggregate(elem->asAGGDECLSYM()->Agg());
            break;
        case SK_GLOBALATTRSYM:
            break;
        default:
            ASSERT(!"Unknown type");
        }
    ENDFOREACHCHILD

    nsDeclaration->isDefined = true;
}

// return access level on the item based on the given flags.  Does not do error checking, so
// callers should call CheckFlags() as well.
//
// container - parent to get default access from, is one of NSSYM, AGGSYM, or PROPSYM (for accessors).
// allowedFlags - will not set access unless the specified access is one of those allowed.
// actualFlags - provided flags
ACCESS CLSDREC::GetAccessFromFlags(SYM *container, unsigned allowedFlags, unsigned actualFlags)
{
    
    //
    NODEFLAGS protFlags = (NODEFLAGS) (actualFlags & allowedFlags & (NF_MOD_ACCESSMODIFIERS));
    switch (protFlags) {
    default:
        ASSERT(!"Invalid protection modifiers");
        // fallthrough
    case 0:
        // get default protection level from our container
        switch (container->getKind()) {
        case SK_NSSYM:
            return ACC_INTERNAL;
        case SK_AGGSYM:
            if (!container->asAGGSYM()->IsInterface())
                return ACC_PRIVATE;
            else
                return ACC_PUBLIC;
        case SK_PROPSYM:
            return container->GetAccess();

        default:
            ASSERT(!"Unknown parent");
            return ACC_PRIVATE;
        }
        break;
    case NF_MOD_INTERNAL:
        return ACC_INTERNAL;
    case NF_MOD_PRIVATE:
        return ACC_PRIVATE;
    case NF_MOD_PROTECTED:
        return ACC_PROTECTED;
    case NF_MOD_PUBLIC:
        return ACC_PUBLIC;
    case NF_MOD_PROTECTED | NF_MOD_INTERNAL:
        return ACC_INTERNALPROTECTED;
    }
}


// check the provided flags for item.
//
// item - symbol to check
// allowedFlags - what flags to allow
// actualFlags - provided flags
void CLSDREC::checkFlags(SYM * item, unsigned allowedFlags, unsigned actualFlags)
{
    //
    // if any flags are disallowed, tell which:
    //
    if (actualFlags & ~allowedFlags) {
        for (unsigned flags = 1, index = 0; flags <= NF_MOD_LAST_KWD; flags = flags << 1) {
            if (flags & actualFlags & ~allowedFlags) {
                TOKENID id = accessTokens[index];
                const TOKINFO * info = CParser::GetTokenInfo(id);

                if (item->parent->isNSSYM() && flags & (NF_MOD_PRIVATE | NF_MOD_PROTECTED)) {
                    // give a better error for namespace elements
                     compiler()->ErrorRef(NULL, ERR_NoNamespacePrivate, item);
                }
                else if (item->parent->isNSSYM() && (flags & NF_MOD_NEW)) {
                    compiler()->ErrorRef(NULL, ERR_NoNewOnNamespaceElement, item);
                }
                else {
                    compiler()->ErrorRef(NULL, ERR_BadMemberFlag, info->pszText, ErrArgRefOnly(item)); // Item is only used for location
                }
            }
            index += 1;
        }
        actualFlags &= allowedFlags;
    }

    if ((actualFlags & allowedFlags & NF_MOD_UNSAFE) && !compiler()->options.m_fUNSAFE) {
        compiler()->ErrorRef(NULL, ERR_IllegalUnsafe, item);
    }

    if (!item->isAGGSYM()) { // AddAggregate handles these for aggs
        // check for conflict with abstract and sealed modifiers
        if ((actualFlags & (NF_MOD_ABSTRACT | NF_MOD_SEALED)) == (NF_MOD_ABSTRACT | NF_MOD_SEALED)) {
            compiler()->ErrorRef(NULL, ERR_AbstractAndSealed, item);
        }

        // Check for conclict between virtual, new, static, override, abstract
        if (allowedFlags & (NF_MOD_VIRTUAL | NF_MOD_OVERRIDE | NF_MOD_NEW | NF_MOD_ABSTRACT)) {
            if ((actualFlags & NF_MOD_STATIC) && (actualFlags & (NF_MOD_VIRTUAL | NF_MOD_OVERRIDE | NF_MOD_ABSTRACT))) {
                compiler()->ErrorRef(NULL, ERR_StaticNotVirtual, item);
            } else if ((actualFlags & NF_MOD_OVERRIDE) && (actualFlags & (NF_MOD_VIRTUAL | NF_MOD_NEW))) {
                compiler()->ErrorRef(NULL, ERR_OverrideNotNew, item);
            } else if ((actualFlags & NF_MOD_ABSTRACT) && (actualFlags & NF_MOD_VIRTUAL)) {
                compiler()->ErrorRef(NULL, ERR_AbstractNotVirtual, item);
            }
            if ((actualFlags & NF_MOD_EXTERN) && (actualFlags & NF_MOD_ABSTRACT)) {
                compiler()->ErrorRef(NULL, ERR_AbstractAndExtern, item);
            }
        }
    }
}

// prepare a field for compilation by setting its constant field, if present
// and veryfing that field shadowing is explicit...
void CLSDREC::prepareFields(MEMBVARSYM *field)
{
    ASSERT(field->getClass()->isSource);

    if (!field->isEvent) {
        // Check that the hiding flags are correct.
        checkSimpleHiding(field, field->parseTree->pParent->flags);
        compiler()->CheckForStaticClass(NULL, field, field->type, ERR_VarDeclIsStaticClass);
    }

    ASSERT(!field->isUnevaled);
}

// checks if we are hiding an abstract method in a bad way
// this happens when the symNew is in an abstract class and it
// will prevent the declaration of non-abstract derived classes
void CLSDREC::CheckHiddenSymbol(SYM * symNew, SymWithType swtHid)
{
    // We are interested in hiding abstract methods in abstract classes
    if (swtHid.Sym()->isMETHSYM() && swtHid.Meth()->isAbstract && symNew->parent->asAGGSYM()->isAbstract) {
        switch (symNew->GetAccess()) {
        case ACC_INTERNAL:
            // derived classes outside this assembly will be OK
            break;
        case ACC_PUBLIC:
        case ACC_PROTECTED:
        case ACC_INTERNALPROTECTED:
            // the new symbol will always hide the abstract symbol
            compiler()->ErrorRef(NULL, ERR_HidingAbstractMethod, symNew, swtHid);
            break;
        case ACC_PRIVATE:
            // no problem since the new member won't hide the abstract method in derived classes
            break;
        default:
            // bad access
            ASSERT(0);
        }
    } else if (swtHid.Sym()->isPROPSYM()) {
        if (swtHid.Prop()->methGet) {
            CheckHiddenSymbol(symNew, SymWithType(swtHid.Prop()->methGet, swtHid.Type()));
        }
        if (swtHid.Prop()->methSet) {
            CheckHiddenSymbol(symNew, SymWithType(swtHid.Prop()->methSet, swtHid.Type()));
        }
    }
}

// list of first invalid (overflow) values for enumerator constants by type
// note that byte is unsigned, while short and int are signed
static const int enumeratorOverflowValues[] = {
    0x100,          // byte
    0x8000,         // short
    0x80000000,     // int
    0x80,           // sbyte
    0x10000,        // ushort
    0x00000000      // uint
};

// evaluate a field constant for fieldcurrent, fieldFirst is given only so that you
// know what to display in case of a circular definition error.
// returns true on success
bool CLSDREC::evaluateFieldConstant(MEMBVARSYM * fieldFirst, MEMBVARSYM * fieldCurrent)
{
    if (!fieldFirst) {
        fieldFirst = fieldCurrent;
    }

    ASSERT(fieldCurrent->isUnevaled);
    ASSERT(fieldFirst->isUnevaled);
    if (fieldCurrent->isConst) {
        // Circular field definition.
        ASSERT(fieldFirst->getClass()->IsEnum() || fieldFirst->parseTree->asVARDECL()->pArg);
        compiler()->ErrorRef(NULL, ERR_CircConstValue, fieldFirst);
        fieldCurrent->isConst = false;
        fieldCurrent->isUnevaled = false;
        return false;
    }
    // Set the flag to mean we are evaling this field...
    fieldCurrent->isConst = true;

    // const fields cannot be unsafe
    compiler()->funcBRec.setUnsafe(false);

    // and compile the parse tree:
    BASENODE *expressionTree = fieldCurrent->getConstExprTree();
    if (!expressionTree) {
        ASSERT(fieldCurrent->getClass()->IsEnum());

        //
        // we have an enum member with no expression
        //
        PREDEFTYPE fieldType = (PREDEFTYPE) fieldCurrent->type->underlyingEnumType()->getPredefType();
        
        MEMBVARSYM* previousEnumerator = fieldCurrent->GetPreviousEnumerator ();
        
        //The previous enumerator must be present and evaled. Evaluation may have resulted in 
        //an error in which case constVal may contain a NULL ptr.
        if (previousEnumerator && (!previousEnumerator->isUnevaled || evaluateFieldConstant(fieldFirst, previousEnumerator))) 
        {
            //
            // we've evaluated our previous enumerator
            // add one and check for overflow
            //

            if (fieldType == PT_LONG || fieldType == PT_ULONG) {
                ASSERT(offsetof(CONSTVAL, longVal) == offsetof(CONSTVAL, ulongVal));

                //
                // do long constants special since their values are allocated on the heap
                //
                __int64 *constVal = (__int64*) compiler()->getGlobalSymAlloc().Alloc(sizeof(__int64));

                //The previous enumerator may have had an error. 
                //There is nothing on the symbol to indicate that it evaled to error, 
                //so check the pointer here before it is used.
                if (previousEnumerator->constVal.longVal)
                    *constVal = *previousEnumerator->constVal.longVal + 1;
                else
                    *constVal = 0;

                //
                // check for overflow
                //
                if ((unsigned __int64) *constVal == (fieldType == PT_LONG ? I64(0x8000000000000000) : 0x0)) {
                    compiler()->ErrorRef(NULL, ERR_EnumeratorOverflow, fieldCurrent);
                    *constVal = 0;
                }

                fieldCurrent->constVal.longVal = constVal;
            } else {
                //
                // we've got an int, short, or byte constant. Doesn't matter if 
                // evaluation produced an error, constVal is not used as a ptr.
                //
                int constVal = previousEnumerator->constVal.iVal + 1;

                //
                // check for overflow
                //
                ASSERT(PT_BYTE == 0 && PT_SHORT == 1 && PT_INT == 2);
                ASSERT(fieldType >= PT_BYTE && fieldType <= PT_UINT);
                if (fieldType >= PT_SBYTE) {
                    ASSERT( (PT_SBYTE + 1) == PT_USHORT && (PT_USHORT + 1) == PT_UINT);
                    fieldType = (PREDEFTYPE)(fieldType - PT_SBYTE + 3);
                }
                if (constVal == enumeratorOverflowValues[fieldType]) {
                    compiler()->ErrorRef(NULL, ERR_EnumeratorOverflow, fieldCurrent);
                    constVal = 0;
                }

                fieldCurrent->constVal.iVal = constVal;
            }
        } else {
            //
            // we are the first enumerator, or we failed to evaluate our previos enumerator
            // set constVal to the appropriate type of zero
            //
            fieldCurrent->constVal = compiler()->getBSymmgr().GetPredefZero(fieldType);
        }
    } else {
        EXPR * rval;
        if (fieldCurrent == fieldFirst) {
            rval = compiler()->funcBRec.compileFirstField(fieldCurrent, expressionTree);
        } else {
            rval = compiler()->funcBRec.compileNextField(fieldCurrent, expressionTree);
        }

        //
        // check that we really got a constant value
        //
        if (rval->kind != EK_CONSTANT) {
            if (rval->kind != EK_ERROR) {  // error reported by compile already...
                //EDMAURER Give a better error message in the following case:
                //  const object x = "some_string"
                //The error here is that an implicit cast cannot be performed. All ref types
                //except strings must be initialized with null.
                if (fieldCurrent->type->IsRefType ())
                    compiler()->Error(expressionTree, ERR_NotNullConstRefField, fieldCurrent, fieldCurrent->type);
                else
                    compiler()->Error(expressionTree, ERR_NotConstantExpression, fieldCurrent);
            }
            fieldCurrent->isConst = false;
            fieldCurrent->isUnevaled = false;
            if (fieldCurrent->type->isPredefined())
                fieldCurrent->constVal = compiler()->getBSymmgr().GetPredefZero(fieldCurrent->type->getPredefType());
            else
                fieldCurrent->constVal = compiler()->getBSymmgr().GetPredefZero(PT_INT);
            return false;
        }

        // We have to copy what rval->getVal() actually points to, as well as val itself, because
        // it may be allocated with a short-lived allocator.
        fieldCurrent->constVal = rval->asCONSTANT()->getSVal();
        switch (rval->type->fundType()) {
        case FT_I8:
        case FT_U8:
            fieldCurrent->constVal.longVal = (__int64 *) compiler()->getGlobalSymAlloc().Alloc(sizeof(__int64));
            * fieldCurrent->constVal.longVal = *(rval->asCONSTANT()->getVal().longVal);
            break;

        case FT_R4:
        case FT_R8:
            fieldCurrent->constVal.doubleVal = (double *) compiler()->getGlobalSymAlloc().Alloc(sizeof(double));
            * fieldCurrent->constVal.doubleVal = *(rval->asCONSTANT()->getVal().doubleVal);
            break;

        case FT_STRUCT:
            fieldCurrent->constVal.decVal = (DECIMAL *) compiler()->getGlobalSymAlloc().Alloc(sizeof(DECIMAL));
            * fieldCurrent->constVal.decVal = *(rval->asCONSTANT()->getVal().decVal);
            break;

        case FT_REF:
            if (rval->asCONSTANT()->getSVal().strVal != NULL) {
                // Allocate memory for the string constant.
                STRCONST * strConst = (STRCONST *) compiler()->getGlobalSymAlloc().Alloc(sizeof(STRCONST));

                int cch = rval->asCONSTANT()->getSVal().strVal->length;
                strConst->length = cch;
                strConst->text = (WCHAR *) compiler()->getGlobalSymAlloc().Alloc(cch * sizeof(WCHAR));
                memcpy(strConst->text, rval->asCONSTANT()->getSVal().strVal->text, cch * sizeof(WCHAR));
                fieldCurrent->constVal.strVal = strConst;
            }
            break;
        default:
            break;
        }
    }

    compiler()->funcBRec.resetUnsafe();
    fieldCurrent->isUnevaled = false;

    if (fieldCurrent->fixedAgg) {
        fieldCurrent->isConst = false; // Fixed fields really aren't const they just get evaluated like consts
    }

    return true;
}



void CLSDREC::evaluateConstants(AGGSYM *cls)
{
    ASSERT(cls->IsDefined() && (cls->hasParseTree || cls->isFabricated));
    if (cls->isFabricated) {
        // Must be compiler synthesised
        return;
    }

    FOREACHCHILD(cls, child)
        SETLOCATIONSYM(child);

        switch (child->getKind()) {
        case SK_MEMBVARSYM:
            // Evaluate constants
            if (child->asMEMBVARSYM()->isUnevaled) {
                evaluateFieldConstant(NULL, child->asMEMBVARSYM());
                compiler()->DiscardLocalState();
            }
            ASSERT(!child->asMEMBVARSYM()->isUnevaled);
            // Fall through.
        case SK_PROPSYM:
        case SK_EVENTSYM:
            DefaultAttrBind::CompileEarly(compiler(), child);
            break;

        case SK_METHSYM:
            MethAttrBind::CompileEarly(compiler(), child->asMETHSYM());
            break;

        case SK_AGGSYM:
        case SK_AGGTYPESYM:
        case SK_TYVARSYM:
            break;

        default:
            ASSERT(!"Unknown node type");
            break;
        }
    ENDFOREACHCHILD;

    if (cls->IsClass() || cls->IsStruct() || cls->IsInterface()) {
        EarlyAggAttrBind::Compile(compiler(), cls);
    } else {
        DefaultAttrBind::CompileEarly(compiler(), cls);
    }
}


// prepares a method for compilation.  the parsetree is obtained from the
// method symbol.  verify means whether to verify that the right override/new/etc...
// flags were specified on the method...
void CLSDREC::prepareMethod(METHSYM * method)
{
    ASSERT(method->getClass()->isSource);
    ASSERT(!method->swtSlot || method->isAnyAccessor());

    if (method->isIfaceImpl) {
        return;
    }

    // Check for static parameters.
    TypeArray * params = method->params;
    for (int i = 0; i < params->size; i++) {
        compiler()->CheckForStaticClass(NULL, method, params->Item(i), ERR_ParameterIsStaticClass);
    }

    // Check for static return type.
    compiler()->CheckForStaticClass(NULL, method, method->retType, ERR_ReturnTypeIsStaticClass);

    //
    // conversions are special
    //
    if (method->isConversionOperator()) {

        prepareConversion(method);

    //
    // operators and dtors just need some simple checking
    //
    } else if (method->isOperator || method->isDtor()) {

        prepareOperator(method);
    //
    // for constructors the basic stuff(valid flags) is checked
    // in defineMethod(). The more serious stuff(base constructor calls)
    // is checked when we actually compile the method and we can
    // evaluate the arg list
    //
    // property/event accessors are handled in prepareProperty/prepareEvent by their
    // associated property/event
    //
    } else if (!method->isAnyAccessor()) {
        /*
        The rules are as follows:
        Of new, virtual, override, you can't do both override and virtual as well as
        override and new.  So that leaves:
        [new]
            - accept the method w/o any checks, regardless if base exists
        [virtual]
            - mark method as new, if base contains previous method, then complain
            about missing [new] or [override]
        [override]
            - check that base contains a virtual method
        [new virtual]
            - mark method as new, make no checks  (same as [new])
        [nothing]
            - mark method as new, check base class, give warning if previous exists
        */

        // First, find out if a method w/ that name & args exists in the baseclasses:
        AGGSYM * cls = method->getClass();
        BASENODE * tree = method->parseTree;

        //
        // check new, virtual & override flags
        //
        if (!method->IsExpImpl()) {
            // for explicit interface implementation these flags can't be set
            ASSERT(!method->swtSlot);

            if (!method->isCtor()) {
                //
                // find hidden member in a base class
                //
                if (!method->isOverride) {
                    //
                    // not an override, just do the simple checks
                    //
                    checkSimpleHiding(method, tree->flags);
                } else {
                    // We have an override method.
                    SymWithType swtHid;
                    SymWithType swtAmbig;
                    bool fNeedMethImpl;

                    if (FindSymHiddenByMethPropAgg(method, cls->baseClass, cls, &swtHid, &swtAmbig, &fNeedMethImpl)) {
                        if (!swtHid.Sym()->isMETHSYM()) {
                            // Found a non-method we will hide. We should not have 'override'.
                            method->isOverride = false;
                            compiler()->ErrorRef(NULL, ERR_CantOverrideNonFunction, method, swtHid);
                            CheckHiddenSymbol(method, swtHid);
                        }
                        else {
                            // The signatures and arity should match.
                            ASSERT(compiler()->getBSymmgr().SubstEqualTypeArrays(method->params, swtHid.Meth()->params, swtHid.Type(), method->typeVars));
                            ASSERT(swtHid.Meth()->typeVars->size == method->typeVars->size);
                            ASSERT(!swtHid.Meth()->isAnyAccessor());

                            // Found a method of same signature that we will hide. If it is an override also, set
                            // swtSlot to its swtSlot.
                            ASSERT(!swtHid.Meth()->isOverride == !swtHid.Meth()->swtSlot);
                            if (swtHid.Meth()->swtSlot) {
                                method->swtSlot.Set(
                                    swtHid.Meth()->swtSlot.Meth(),
                                    compiler()->getBSymmgr().SubstType(swtHid.Meth()->swtSlot.Type(), swtHid.Type())->asAGGTYPESYM());
                            }
                            else
                                method->swtSlot = swtHid;

                            if (fNeedMethImpl) {
                                method->fNeedsMethodImp = true;
                                method->isNewSlot = true;
                            }

                            SymWithType swtSlot = method->swtSlot;

                            if (swtAmbig)
                                compiler()->ErrorRef(NULL, ERR_AmbigOverride, ErrArgRefOnly(method), swtHid.Meth(), swtAmbig.Meth(), cls);

                            // If this is an override of Finalize on object, then give a warning...
                            if (method->name == compiler()->namemgr->GetPredefName(PN_DTOR) &&
                                method->params->size == 0 &&
                                method->typeVars->size == 0 &&
                                swtSlot.Type()->isPredefType(PT_OBJECT))
                            {
                                compiler()->Error(method->parseTree, ERR_OverrideFinalizeDeprecated);
                            }

                            // Compute the constraints.
                            ASSERT(method->typeVars && swtSlot.Meth()->typeVars && method->typeVars->size == swtSlot.Meth()->typeVars->size);
                            if (method->typeVars->size)
                                SetOverrideConstraints(method);

                            if (swtSlot.Meth()->hasBogus() && swtSlot.Meth()->checkBogus()) {
                                compiler()->ErrorRef(NULL, ERR_CantOverrideBogusMethod, method, swtSlot);
                            }

                            if (!swtHid.Meth()->isVirtual) {
                                if (swtHid.Meth()->isOverride)
                                    compiler()->ErrorRef(NULL, ERR_CantOverrideSealed, method, swtHid);
                                else
                                    compiler()->ErrorRef(NULL, ERR_CantOverrideNonVirtual, method, swtHid);
                            }

                            // Access must match.
                            if (swtSlot.Meth()->GetAccess() != method->GetAccess()) {
                                compiler()->ErrorRef(NULL, ERR_CantChangeAccessOnOverride, method,
                                    compiler()->ErrAccess(swtSlot.Meth()->GetAccess()), swtSlot);
                            }

                            // Return type must match.
                            if (!compiler()->getBSymmgr().SubstEqualTypes(method->retType, swtSlot.Meth()->retType, swtSlot.Type(), method->typeVars)) {
                                compiler()->ErrorRef(NULL, ERR_CantChangeReturnTypeOnOverride, method, swtSlot,
                                    ErrArgNoRef(compiler()->getBSymmgr().SubstType(swtSlot.Meth()->retType, swtSlot.Type(), method->typeVars)));
                            }

                            method->modOptCount = swtSlot.Meth()->modOptCount;

                            if (!method->IsDeprecated() && swtSlot.Meth()->IsDeprecated()) {
                                compiler()->ErrorRef(NULL, WRN_NonObsoleteOverridingObsolete, method, swtSlot);
                            }

                            CheckLinkDemandOnOverride(method, swtSlot);
                        }
                    } else {
                        // didn't find hidden base member
                        method->isOverride = false;
                        compiler()->ErrorRef(NULL, ERR_OverrideNotExpected, method);
                    }
                }

                //
                // check that abstract methods are in abstract classes
                //
                if (method->isAbstract && !cls->isAbstract) {
                    compiler()->ErrorRef(NULL, ERR_AbstractInConcreteClass, method, cls);
                }

                //
                // check that new virtual methods aren't in sealed classes
                //
                if (cls->isSealed && method->isVirtual && !method->isOverride) {
                    compiler()->ErrorRef(NULL, ERR_NewVirtualInSealed, method, cls);
                }
            }

            //
            // check that the method body existence matches with the abstractness
            // attribute of the method
            //
            if (method->isAbstract || method->isExternal) {
                if ((tree->other & NFEX_METHOD_NOBODY) == 0) {
                    // found an abstract method with a body
                    compiler()->ErrorRef(NULL, method->isAbstract ? ERR_AbstractHasBody : ERR_ExternHasBody, method);
                }
            } else {
                if (tree->other & NFEX_METHOD_NOBODY) {
                    // found non-abstract method without body
                    compiler()->ErrorRef(NULL, ERR_ConcreteMissingBody, method);
                }
            }

        } else {
            //
            // method is an explicit interface implementation
            //

            // get the interface
            CheckExplicitImpl(method);

            if (cls->IsStruct()) {
                cls->hasExplicitImpl = true;
            }

            //
            // must have a body
            //
            if (tree->other & NFEX_METHOD_NOBODY && !method->isExternal) {
                // found non-abstract method without body
                compiler()->ErrorRef(NULL, ERR_ConcreteMissingBody, method);
            } else if (method->isExternal && (tree->other & NFEX_METHOD_NOBODY) == 0)  {
                // found an extern method with a body
                compiler()->ErrorRef(NULL, ERR_ExternHasBody, method);
            }
        }
    }
}


/***************************************************************************************************
    Set the constraints for this override or explicit impl method from the constraints for the
    base method.
***************************************************************************************************/
void CLSDREC::SetOverrideConstraints(METHSYM * methOver)
{
    ASSERT(methOver->typeVars->size > 0);
    ASSERT(methOver->swtSlot && methOver->typeVars->size == methOver->swtSlot.Meth()->typeVars->size);

    MethWithType mwtBase = methOver->swtSlot;

    int ivar;

    for (ivar = 0; ivar < methOver->typeVars->size; ivar++) {
        TYVARSYM * varDst = methOver->typeVars->ItemAsTYVARSYM(ivar);
        TYVARSYM * varSrc = mwtBase.Meth()->typeVars->ItemAsTYVARSYM(ivar);

        varDst->cons = varSrc->cons;
        TypeArray * taBnds = varSrc->GetBnds();
        taBnds = compiler()->getBSymmgr().SubstTypeArray(taBnds, mwtBase.Type(), methOver->typeVars);
        compiler()->EnsureState(taBnds, AggState::Bounds);
        compiler()->SetBounds(varDst, taBnds, true);
        ASSERT(!varDst->FResolved());
    }

    for (ivar = 0; ivar < methOver->typeVars->size; ivar++) {
        TYVARSYM * varDst = methOver->typeVars->ItemAsTYVARSYM(ivar);
        compiler()->ResolveBounds(varDst, true);
        TYPESYM * typeBase = varDst->GetAbsoluteBaseType();
        if (varDst->IsValType() && varDst->IsRefType()) {
            ASSERT(!typeBase->IsValType() || varDst->FRefCon());
            compiler()->Error(NULL, ERR_BaseConstraintConflict, ErrArgRef(varDst), typeBase,
                typeBase->IsValType() ? L"class" : L"struct");
        }
        else if (typeBase->isNUBSYM() && (varDst->FValCon() || varDst->FRefCon())) {
            compiler()->Error(NULL, ERR_BaseConstraintConflict, ErrArgRef(varDst),
                typeBase, varDst->FRefCon() ? L"class" : L"struct");
        }
    }
}


/*
 * returns the conditional symbols for the method or what it overrides
 */
NAMELIST * CLSDREC::GetConditionalSymbols(METHSYM *method)
{
    if (!method->checkedCondSymbols && compiler()->CompPhase() >= CompilerPhase::Prepare) {
        method->checkedCondSymbols = true;
        if (method->isOverride && !method->conditionalSymbols)
            method->conditionalSymbols = method->swtSlot.Meth()->conditionalSymbols;
    }

    return method->conditionalSymbols;
}

//
//  Returns the conditional symbols for attributes, including all those inherited from base classes  
//
NAMELIST * CLSDREC::GetConditionalSymbols(AGGSYM *symAttribute)
{    
    if (!symAttribute->isAttribute) {
        ASSERT(symAttribute->conditionalSymbols == NULL);
        return NULL;
    }

    if (symAttribute->fCheckedCondSymbols)
        return symAttribute->conditionalSymbols;

    compiler()->EnsureState(symAttribute->baseClass, AggState::DefinedMembers);

    NAMELIST** pConditionalListTail = &symAttribute->conditionalSymbols;
    while(*pConditionalListTail)
        pConditionalListTail = &(*pConditionalListTail)->next;

    for (NAMELIST * list = GetConditionalSymbols(symAttribute->baseClass->getAggregate()); list; list = list->next) {
        compiler()->getBSymmgr().AddToGlobalNameList(list->name, &pConditionalListTail);
    }

    symAttribute->fCheckedCondSymbols = true;
    return symAttribute->conditionalSymbols;
}

void CLSDREC::CheckLinkDemandOnOverride(METHSYM * meth, MethWithType mwtBase)
{
    if (meth->hasLinkDemand && !mwtBase.Meth()->hasLinkDemand && !mwtBase.Meth()->getClass()->hasLinkDemand) {
        compiler()->ErrorRef(NULL, WRN_LinkDemandOnOverride, meth, mwtBase);
    }
}


TYPESYM * CLSDREC::StripNubs(TYPESYM * type, AGGSYM * clsCtx, int * pcnub)
{
    if (pcnub)
        *pcnub = 0;
    if (!type->isNUBSYM())
        return type;

    if (!clsCtx->isPredefAgg(PT_G_OPTIONAL))
        return pcnub ? type->StripNubs(pcnub) : type->StripNubs();

    // Just convert to an AGGTYPESYM.
    return type->asNUBSYM()->GetAts();
}


void CLSDREC::CheckForProtectedInSealed(SYM *member)
{
    if ((member->GetAccess() == ACC_PROTECTED || member->GetAccess() == ACC_INTERNALPROTECTED) 
        && member->parent->isAGGSYM()
        && !member->IsOverride()) {
        AGGSYM *cls = member->parent->asAGGSYM();
        if (cls->isSealed) {
            int id;
            if (cls->IsStruct())
                id = ERR_ProtectedInStruct;
            else if (cls->IsStatic())
                id = ERR_ProtectedInStatic;
            else 
                id = WRN_ProtectedInSealed;
            compiler()->ErrorRef(NULL, id, member);
        }
    }
}


// prepare checks for a user defined conversion operator
// checks that the conversion doesn't override a compiler generated conversion
// checks extern and body don't conflict
void CLSDREC::prepareConversion(METHSYM * conversion)
{
    ASSERT(conversion->getClass()->isSource);

    AGGSYM * aggConv = conversion->getClass();
    AGGTYPESYM * atsConv = aggConv->getThisType();

    if (aggConv->isPredefAgg(PT_G_OPTIONAL))
        return;

    //
    // what are we converting from/to
    //
    int cnubRet;
    int cnubArg;
    TYPESYM * typeRetBare = StripNubs(conversion->retType, conversion->getClass(), &cnubRet);
    TYPESYM * typeArgBare = StripNubs(conversion->params->Item(0), conversion->getClass(), &cnubArg);

    TYPESYM * typeOtherBare;

    if (typeRetBare == atsConv) {
        typeOtherBare = typeArgBare;
        if (cnubRet > 0) {
            // Make sure the other type isn't a base of nullable.
            AGGTYPESYM * atsNullable = conversion->retType->asNUBSYM()->GetAts();
            if (atsNullable && compiler()->IsBaseType(atsNullable, typeArgBare))
            {
                compiler()->ErrorRef(NULL, ERR_ConversionWithBase, conversion);
                goto LErrBase;
            }
        }
    } else {
        typeOtherBare = typeRetBare;
        if (atsConv != typeArgBare) {
            ASSERT(compiler()->ErrorCount() && !compiler()->FAbortEarly(0));
            return;
        }
        if (cnubArg > 0) {
            // Make sure the other type isn't a base of nullable.
            AGGTYPESYM * atsNullable = conversion->params->Item(0)->asNUBSYM()->GetAts();
            if (atsNullable && compiler()->IsBaseType(atsNullable, typeArgBare))
            {
                compiler()->ErrorRef(NULL, ERR_ConversionWithBase, conversion);
                goto LErrBase;
            }
        }
    }

    // Can't convert from/to a base or derived class or interface.
    if (typeOtherBare->isAGGTYPESYM()) {
        AGGTYPESYM * atsOther = typeOtherBare->asAGGTYPESYM();

        // Can't convert to/from an interface.
        if (atsOther->isInterfaceType()) {
            compiler()->ErrorRef(NULL, ERR_ConversionWithInterface, conversion);
        }
        else if ((aggConv->IsClass() || aggConv->IsStruct()) && atsOther->isClassType()) {
            // Check that we aren't converting to/from a base class.
            if (atsConv->FindBaseType(atsOther)) {
                compiler()->ErrorRef(NULL, ERR_ConversionWithBase, conversion);
            }
            else if (atsOther->FindBaseType(atsConv)) {
                compiler()->ErrorRef(NULL, ERR_ConversionWithDerived, conversion);
            }
        }
    }

LErrBase:

    //
    // operators don't hide
    //

    if (conversion->isExternal) {
        if ((conversion->getParseTree()->other & NFEX_METHOD_NOBODY) == 0) {
            // found an abstract method with a body
            compiler()->ErrorRef(NULL, ERR_ExternHasBody, conversion);
        }
    } else {
        if (conversion->getParseTree()->other & NFEX_METHOD_NOBODY) {
            // found non-abstract method without body
            compiler()->ErrorRef(NULL, ERR_ConcreteMissingBody, conversion);
        }
    }
}

// prepare checks for a user defined operator (not conversion operators)
// checks extern and body don't conflict
void CLSDREC::prepareOperator(METHSYM * op)
{
    ASSERT(op->getClass()->isSource);

    if (op->isExternal) {
        if ((op->getParseTree()->other & NFEX_METHOD_NOBODY) == 0) {
            // found an abstract method with a body
            compiler()->ErrorRef(NULL, ERR_ExternHasBody, op);
        }
    } else {
        if (op->getParseTree()->other & NFEX_METHOD_NOBODY) {
            // found non-abstract method without body
            compiler()->ErrorRef(NULL, ERR_ConcreteMissingBody, op);
        }
    }

    if (op->isDtor()) {
        // Check for a sealed dtor in a base type.
        SymWithType swtHid;
        AGGSYM * agg = op->getClass();

        bool fHid = FindSymHiddenByMethPropAgg(op, agg->baseClass, agg, &swtHid);
        ASSERT(fHid || agg->isPredefAgg(PT_OBJECT)); // This should hide something....

        if (fHid && swtHid.Sym()->isMETHSYM() && swtHid.Meth()->isDtor() && !swtHid.Meth()->isVirtual) {
            compiler()->ErrorRef(NULL, ERR_CantOverrideSealed, op, swtHid);
        }
    }
}

// do prepare stage for a property in a class or struct
// verifies new, override, abstract, virtual against
// inherited members.
void CLSDREC::prepareProperty(PROPSYM * property)
{
    ASSERT(property->getClass()->isSource);
    ASSERT(!property->swtSlot);

    AGGSYM * cls = property->getClass();
    if (property->isIndexer()) {
        // bind IndexerName attribute in order to resolve the actual name of the indexer.
        IndexerNameAttrBind::Compile(compiler(), property->asINDEXERSYM());
        CheckForBadMemberSimple(property->getRealName(), property->getParseTree(), cls);
    }

    // First, find out if a property w/ that name & args exists in the baseclasses:
    PROPERTYNODE * tree = property->parseTree->asANYPROPERTY();

    // pwtBase is where to look for accessor names.
    PropWithType pwtBase(NULL, NULL);

    if (property->IsExpImpl()) {
        // Property is an explicit interface implementation
        DefinePropertyAccessors(property, pwtBase);

        CheckExplicitImpl(property);
        if (property->swtSlot) {
            PROPSYM *implProp = property->swtSlot.Prop();
            prepareAccessor(property->methGet, property, implProp->methGet);
            prepareAccessor(property->methSet, property, implProp->methSet);
        }
        else {
            if (property->methGet) {
                NAME * name = createAccessorName(property->errExpImpl->nameText, L"get_");
                property->methGet->errExpImpl = compiler()->getBSymmgr().GetErrorType(property->errExpImpl->parent, name, NULL);
            }
            if (property->methSet) {
                NAME * name = createAccessorName(property->errExpImpl->nameText, L"set_");
                property->methSet->errExpImpl = compiler()->getBSymmgr().GetErrorType(property->errExpImpl->parent, name, NULL);
            }
        }
        ASSERT(!property->methGet || property->methGet->swtSlot || property->methGet->errExpImpl);
        ASSERT(!property->methSet || property->methSet->swtSlot || property->methSet->errExpImpl);
        return;
    }

    SymWithType swtHid;
    SymWithType swtAmbig;

    if (!property->isOverride) {
        checkSimpleHiding(property, tree->flags);
        CheckForProtectedInSealed(property);
        DefinePropertyAccessors(property, pwtBase);
    }
    else if (!FindSymHiddenByMethPropAgg(property, cls->baseClass, cls, &swtHid, &swtAmbig)) {
        // Didn't find a hidden base symbol to override.
        property->isOverride = false;
        compiler()->ErrorRef(NULL, ERR_OverrideNotExpected, property);
        DefinePropertyAccessors(property, pwtBase);
    }
    else if (!swtHid.Sym()->isPROPSYM()) {
        // Found a non-property we will hide. We should not have 'override'.
        property->isOverride = false;
        compiler()->ErrorRef(NULL, ERR_CantOverrideNonProperty, property, swtHid);
        CheckHiddenSymbol(property, swtHid);
        DefinePropertyAccessors(property, pwtBase);
    }
    else {
        // Found a property of same signature that we will override.
        ASSERT(!swtHid.Prop()->isOverride == !swtHid.Prop()->swtSlot);
        if (swtHid.Prop()->swtSlot) {
            property->swtSlot.Set(
                swtHid.Prop()->swtSlot.Prop(),
                compiler()->getBSymmgr().SubstType(swtHid.Prop()->swtSlot.Type(), swtHid.Type())->asAGGTYPESYM());
        }
        else
            property->swtSlot = swtHid;

        if (swtAmbig)
            compiler()->ErrorRef(NULL, ERR_AmbigOverride, ErrArgRefOnly(property), swtHid, swtAmbig, cls);

        SymWithType swtSlot = property->swtSlot;
        ASSERT(!swtSlot.Prop()->isOverride);

        pwtBase = swtSlot;

        // Check for bogus.
        if (swtSlot.Prop()->hasBogus() && swtSlot.Prop()->checkBogus()) {
            compiler()->ErrorRef(NULL, ERR_CantOverrideBogusMethod, property, swtSlot);
            // Don't look for accessors.
            pwtBase.Clear();
        }

        // Access must match.
        if (swtSlot.Prop()->GetAccess() != property->GetAccess()) {
            compiler()->ErrorRef(NULL, ERR_CantChangeAccessOnOverride, property,
                compiler()->ErrAccess(swtSlot.Prop()->GetAccess()), swtSlot);
            // Don't look for accessors.
            pwtBase.Clear();
        }

        // Return type must match.
        if (!compiler()->getBSymmgr().SubstEqualTypes(property->retType, swtSlot.Prop()->retType, swtSlot.Type())) {
            compiler()->ErrorRef(NULL, ERR_CantChangeTypeOnOverride, property, swtSlot,
                ErrArgNoRef(compiler()->getBSymmgr().SubstType(swtSlot.Prop()->retType, swtSlot.Type())));
            // Don't look for accessors.
            pwtBase.Clear();
        }

        if (!property->IsDeprecated() && swtSlot.Prop()->IsDeprecated()) {
            compiler()->ErrorRef(NULL, WRN_NonObsoleteOverridingObsolete, property, swtSlot);
        }

        DefinePropertyAccessors(property, pwtBase);

        // Check overriden get is a valid match to override.
        if (property->methGet) {
            if (pwtBase)
                CheckValidAccessorOverride(property->methGet, property, swtSlot.Prop()->methGet, ERR_NoGetToOverride);
            else
                property->methGet->isOverride = false;
        }

        // Check overridden set is a valid match to override.
        if (property->methSet) {
            if (pwtBase) 
                CheckValidAccessorOverride(property->methSet, property, swtSlot.Prop()->methSet, ERR_NoSetToOverride);
            else
                property->methSet->isOverride = false;
        }
    }

    // NOTE: inner classes can derive from outer classes so private virtual properties are OK.

    // Check that new virtual accessors aren't in sealed classes.
    if (cls->isSealed && !property->isOverride) {
        if (property->methGet && property->methGet->isVirtual) {
            compiler()->ErrorRef(NULL, ERR_NewVirtualInSealed, property->methGet, cls);
        }
        if (property->methSet && property->methSet->isVirtual) {
            compiler()->ErrorRef(NULL, ERR_NewVirtualInSealed, property->methSet, cls);
        }
    }

    // Check that abstract properties are in abstract classes.
    if (!cls->isAbstract) {
        if (property->methGet && property->methGet->isAbstract) {
            compiler()->ErrorRef(NULL, ERR_AbstractInConcreteClass, property->methGet, cls);
        }
        if (property->methSet && property->methSet->isAbstract) {
            compiler()->ErrorRef(NULL, ERR_AbstractInConcreteClass, property->methSet, cls);
        }
    }

    //
    // check that the property body existence matches with the abstractness
    // attribute of the property
    //
    prepareAccessor(property->methGet, property, NULL);
    prepareAccessor(property->methSet, property, NULL);
}

void CLSDREC::CheckValidAccessorOverride(METHSYM * methAcc, PROPSYM * prop, METHSYM * methBaseAcc, int errNone)
{
    ASSERT(prop);
    ASSERT(prop->swtSlot);
    ASSERT(methAcc && methAcc->isAnyAccessor());
    ASSERT(!prop->IsExpImpl());  // cannot be explicit impl.
    ASSERT(prop->isOverride && methAcc->isOverride);

    if (!methBaseAcc || !CheckAccess(methBaseAcc, prop->swtSlot.Type(), prop->getClass(), NULL)) {
        compiler()->ErrorRef(NULL, errNone, methAcc, prop->swtSlot);
        methAcc->isOverride = false;
        return;
    }

    methAcc->swtSlot.Set(methBaseAcc, prop->swtSlot.Type());

    // Can't change access on override.
    if (methAcc->GetAccess() != methBaseAcc->GetAccess()) {
        compiler()->ErrorRef(NULL, ERR_CantChangeAccessOnOverride, methAcc,
            compiler()->ErrAccess(methBaseAcc->GetAccess()), methAcc->swtSlot);
    }

    methAcc->modOptCount = methBaseAcc->modOptCount;
    prop->modOptCount += methAcc->modOptCount;

    // Check that accessor doesn't hide a sealed accessor....
    SymWithType swtTmp;

    AGGSYM * cls = prop->getClass();
    bool fNeedMethImpl;

    if (FindSymHiddenByMethPropAgg(methAcc, cls->baseClass, cls, &swtTmp, NULL, &fNeedMethImpl)) {
        // FindSymHiddenByMethProp filters out non-accessors (but sets fNeedMethImpl when found).
        ASSERT(swtTmp.Sym()->isMETHSYM() && swtTmp.Meth()->isAnyAccessor());
        // it must be virtual
        if (!swtTmp.Meth()->isVirtual) {
            if (swtTmp.Meth()->isOverride)
                compiler()->ErrorRef(NULL, ERR_CantOverrideSealed, methAcc, swtTmp);
            else
                compiler()->ErrorRef(NULL, ERR_CantOverrideNonVirtual, methAcc, swtTmp);
        } else {
            // Walk the inheritance tree and check that there are no sealed properties in between.
            // This will catch the case where a type in the inheritance hierarchy seals the property but that property does 
            // not implement this accessor.  In that case, sealing the property seals both accessors.

            // Note:  this is just one half of the fix, we should also emit the second accessor on sealed properties which only 
            // override one of the accessors.
            SYM *symCur = prop;
            AGGTYPESYM *typeCur = cls->getThisType();
            while ((symCur = findNextName(prop->name, &typeCur, symCur)))
            {
                if (!symCur->isPROPSYM())
                    continue;

                PROPSYM *propCur = symCur->asPROPSYM();
                if ((propCur->methGet && !propCur->methGet->isVirtual && propCur->methGet->isOverride) ||
                    (propCur->methSet && !propCur->methSet->isVirtual && propCur->methSet->isOverride))
                {
                    compiler()->ErrorRef(NULL, ERR_CantOverrideSealed, methAcc, swtTmp, propCur);
                    break;
                }

                if (propCur == swtTmp.Meth()->getProperty())
                    break;
            }
            ASSERT(symCur);  // if this is an override then we should always find the accessor it is overriding in one of the base types.

            if (fNeedMethImpl) {
                methAcc->fNeedsMethodImp = true;
                methAcc->isNewSlot = true;
            }
        }
    }

    CheckLinkDemandOnOverride(methAcc, methAcc->swtSlot);
}


void CLSDREC::prepareAccessor(METHSYM *accessor, PROPSYM *property, METHSYM *implAccessor)
{
    ASSERT(property);
    ASSERT(property->getClass()->isSource);

    if (property->name) {
        if (!accessor)
            return;

        if (accessor->isAbstract || accessor->isExternal) {
            if ((accessor->parseTree->other & NFEX_METHOD_NOBODY) == 0) {
                // found an abstract property with a body
                compiler()->ErrorRef(NULL, accessor->isAbstract ? ERR_AbstractHasBody : ERR_ExternHasBody, accessor);
            }
        } else {
            if (accessor->parseTree->other & NFEX_METHOD_NOBODY) {
                // found non-abstract property without body
                compiler()->ErrorRef(NULL, ERR_ConcreteMissingBody, accessor);
            }
        }
        return;
    }

    // Explicit interface impl.
    ASSERT(property->swtSlot);

    SymWithType swtImpl = property->swtSlot;

    if (!accessor) {
        if (implAccessor) {
            SymWithType swt(implAccessor, swtImpl.Type());
            compiler()->ErrorRef(NULL, ERR_ExplicitPropertyMissingAccessor, property, ErrArgRef(swt));
        }
        return;
    }

    accessor->isMetadataVirtual = true;

    // Must have a body.
    if ((accessor->parseTree->other & NFEX_METHOD_NOBODY) && !accessor->isExternal) {
        compiler()->ErrorRef(NULL, ERR_ConcreteMissingBody, accessor);
    } else if (accessor->isExternal && (accessor->parseTree->other & NFEX_METHOD_NOBODY) == 0) {
        // found an extern method with a body
        compiler()->ErrorRef(NULL, ERR_ExternHasBody, accessor);
    }

    // Must have same accessors as interface member.
    if (!implAccessor) {
        compiler()->ErrorRef(NULL, ERR_ExplicitPropertyAddingAccessor, accessor, swtImpl);
        NAME * name = createAccessorName(swtImpl.Sym()->name, accessor == property->methGet ? L"get_" : L"set_");
        accessor->errExpImpl = compiler()->getBSymmgr().GetErrorType(swtImpl.Type(), name, NULL);
    } else {
        accessor->swtSlot.Set(implAccessor, swtImpl.Type());
        accessor->fNeedsMethodImp = true;
    }
}

// do prepare stage for an event in a class or struct (not interface)
void CLSDREC::prepareEvent(EVENTSYM * event)
{
    ASSERT(event->getClass()->isSource);
    ASSERT(!event->ewtSlot);

    AGGSYM * cls = event->getClass();
    BASENODE * tree = event->parseTree;

    // ewtBase is where to look for accessor names.
    EventWithType ewtBase(NULL, NULL);

    // For imported types we don't really know if it's a delegate until we've imported members.
    compiler()->EnsureState(event->type, AggState::DefinedMembers);

    // Issue error if the event type is not a delegate.
    if (!event->type->isDelegateType()) {
        compiler()->ErrorRef(NULL, ERR_EventNotDelegate, event);
    }

    if (event->IsExpImpl()) {
        // event must be an explicit implementation. 
        DefineEventAccessors(event, ewtBase);

        CheckExplicitImpl(event);
        if (event->ewtSlot) {
            // Set up the accessors as explicit interfaces.
            event->methAdd->swtSlot.Set(event->ewtSlot.Event()->methAdd, event->ewtSlot.Type());
            event->methAdd->fNeedsMethodImp = true;
            event->methAdd->isMetadataVirtual = true;
            if (!event->methAdd->swtSlot) {
                NAME * name = createAccessorName(event->ewtSlot.Sym()->name, L"add_");
                event->methAdd->errExpImpl = compiler()->getBSymmgr().GetErrorType(event->ewtSlot.Type(), name, NULL);
            }
            event->methRemove->swtSlot.Set(event->ewtSlot.Event()->methRemove, event->ewtSlot.Type());
            event->methRemove->fNeedsMethodImp = true;
            event->methRemove->isMetadataVirtual = true;
            if (!event->methRemove->swtSlot) {
                NAME * name = createAccessorName(event->ewtSlot.Sym()->name, L"remove_");
                event->methRemove->errExpImpl = compiler()->getBSymmgr().GetErrorType(event->ewtSlot.Type(), name, NULL);
            }
        }
        else {
            NAME * name = createAccessorName(event->errExpImpl->nameText, L"add_");
            event->methAdd->errExpImpl = compiler()->getBSymmgr().GetErrorType(event->errExpImpl->parent, name, NULL);
            name = createAccessorName(event->errExpImpl->nameText, L"remove_");
            event->methRemove->errExpImpl = compiler()->getBSymmgr().GetErrorType(event->errExpImpl->parent, name, NULL);
        }
        ASSERT(event->methAdd->swtSlot || event->methAdd->errExpImpl);
        ASSERT(event->methRemove->swtSlot || event->methRemove->errExpImpl);
    }
    else {
        SymWithType swtHid;

        // not explicit event implementation.
        if (!event->isOverride) {
            checkSimpleHiding(event, event->getParseFlags());
            CheckForProtectedInSealed(event);
            DefineEventAccessors(event, ewtBase);
        }
        else if (!FindAnyHiddenSymbol(event->name, cls->baseClass, cls, &swtHid)) {
            // didn't find base member to override.
            event->isOverride = false;
            compiler()->ErrorRef(NULL, ERR_OverrideNotExpected, event);
            DefineEventAccessors(event, ewtBase);
        }
        else if (!swtHid.Sym()->isEVENTSYM()) {
            // Found a non-event. "override" is in error.
            event->isOverride = false;
            compiler()->ErrorRef(NULL, ERR_CantOverrideNonEvent, event, swtHid);
            DefineEventAccessors(event, ewtBase);
        }
        else {
            // Found an event that we will override.
            ASSERT(!swtHid.Event()->isOverride == !swtHid.Event()->ewtSlot);
            if (swtHid.Event()->ewtSlot) {
                event->ewtSlot.Set(
                    swtHid.Event()->ewtSlot.Event(),
                    compiler()->getBSymmgr().SubstType(swtHid.Event()->ewtSlot.Type(), swtHid.Type())->asAGGTYPESYM());
            }
            else
                event->ewtSlot = swtHid;

            EventWithType ewtSlot = event->ewtSlot;
            ewtBase = ewtSlot;

            if (ewtSlot.Event()->hasBogus() && ewtSlot.Event()->checkBogus()) {
                compiler()->ErrorRef(NULL, ERR_CantOverrideBogusMethod, event, ewtSlot);
                // Don't look for accessors.
                ewtBase.Clear();
            }
            else if (swtHid.Event() != ewtSlot.Event() && swtHid.Event()->hasBogus() && swtHid.Event()->checkBogus()) {
                compiler()->ErrorRef(NULL, ERR_CantOverrideBogusMethod, event, swtHid);
                // Don't look for accessors.
                ewtBase.Clear();
            }
            else {
                // Check that access is the same.
                if (ewtSlot.Event()->GetAccess() != event->GetAccess()) {
                    compiler()->ErrorRef(NULL, ERR_CantChangeAccessOnOverride, event,
                        compiler()->ErrAccess(ewtSlot.Event()->GetAccess()), ewtSlot);
                    // Don't look for accessors.
                    ewtBase.Clear();
                }

                // Make sure both accessors are virtual.
                if (!swtHid.Event()->methAdd->isVirtual || !swtHid.Event()->methRemove->isVirtual) {
                    if (swtHid.Event()->methAdd->isOverride || swtHid.Event()->methRemove->isOverride)
                        compiler()->ErrorRef(NULL, ERR_CantOverrideSealed, event, swtHid);
                    else
                        compiler()->ErrorRef(NULL, ERR_CantOverrideNonVirtual, event, swtHid);
                }

                if (!event->IsDeprecated() && ewtSlot.Event()->IsDeprecated()) {
                    compiler()->ErrorRef(NULL, WRN_NonObsoleteOverridingObsolete, event, ewtSlot);
                }

                // Make sure type of the event is the same.
                if (!compiler()->getBSymmgr().SubstEqualTypes(event->type, ewtSlot.Event()->type, ewtSlot.Type())) {
                    compiler()->ErrorRef(NULL, ERR_CantChangeTypeOnOverride, event, ewtSlot,
                        ErrArgNoRef(compiler()->getBSymmgr().SubstType(ewtSlot.Event()->type, ewtSlot.Type())));
                    // Don't look for accessors.
                    ewtBase.Clear();
                }
            }

            DefineEventAccessors(event, ewtBase);
            ASSERT(event->methAdd && event->methRemove);

            if (ewtBase) {
                // Set the override bits.
                SetAccessorOverride(event->methAdd, event, ewtBase.Event()->methAdd);
                SetAccessorOverride(event->methRemove, event, ewtBase.Event()->methRemove);
            }
        }
    }

    ASSERT(event->methAdd && event->methRemove);

    // Check that abstract-ness matches with whether have a body for each accessor.
    if (tree->kind != NK_VARDECL) {
        if (event->methAdd->isAbstract || event->methAdd->isExternal) {
            if ((event->methAdd->parseTree->other & NFEX_METHOD_NOBODY) == 0) {
                // found an abstract event with a body
                compiler()->ErrorRef(NULL, event->methAdd->isAbstract ? ERR_AbstractHasBody : ERR_ExternHasBody, event->methAdd);
            }
        } else {
            if (event->methAdd->parseTree->other & NFEX_METHOD_NOBODY) {
                // found non-abstract event without body
                compiler()->ErrorRef(NULL, ERR_ConcreteMissingBody, event->methAdd);
            }
        }
        if (event->methRemove->isAbstract || event->methRemove->isExternal) {
            if ((event->methRemove->parseTree->other & NFEX_METHOD_NOBODY) == 0) {
                // found an abstract event with a body
                compiler()->ErrorRef(NULL, event->methRemove->isAbstract ? ERR_AbstractHasBody : ERR_ExternHasBody, event->methRemove);
            }
        } else {
            if (event->methRemove->parseTree->other & NFEX_METHOD_NOBODY) {
                // found non-abstract event without body
                compiler()->ErrorRef(NULL, ERR_ConcreteMissingBody, event->methRemove);
            }
        }
    }

    // Check that virtual/abstractness is OK.
    if (cls->isSealed && event->methAdd->isVirtual && !event->methAdd->isOverride) {
        compiler()->ErrorRef(NULL, ERR_NewVirtualInSealed, event, cls);
    }

    if (!cls->isAbstract && event->methAdd->isAbstract) {
        compiler()->ErrorRef(NULL, ERR_AbstractInConcreteClass, event, cls);
    }
}


void CLSDREC::SetAccessorOverride(METHSYM * methAcc, EVENTSYM * evt, METHSYM * methBaseAcc)
{
    ASSERT(evt);
    ASSERT(evt->ewtSlot);
    ASSERT(methAcc && methAcc->isAnyAccessor());
    ASSERT(methBaseAcc && methBaseAcc->isAnyAccessor());
    ASSERT(!evt->IsExpImpl());  // cannot be explicit impl.
    ASSERT(evt->isOverride);

    methAcc->isOverride = true;
    methAcc->swtSlot.Set(methBaseAcc, evt->ewtSlot.Type());
    methAcc->modOptCount = methBaseAcc->modOptCount;

    AGGSYM * cls = evt->getClass();
    bool fNeedMethImpl;
    SymWithType swtTmp;
    bool fT; fT = FindSymHiddenByMethPropAgg(methAcc, cls->baseClass, cls, &swtTmp, NULL, &fNeedMethImpl);
    ASSERT(fT && swtTmp.Sym()->isMETHSYM() && swtTmp.Meth()->isAnyAccessor() &&
        (swtTmp.Meth() == methBaseAcc || swtTmp.Meth()->swtSlot.Sym() == methBaseAcc));
    if (fNeedMethImpl) {
        methAcc->fNeedsMethodImp = true;
        methAcc->isNewSlot = true;
    }
    CheckLinkDemandOnOverride(methAcc, methAcc->swtSlot);
}


// define the fields in a given field node, this involves checking that the
// flags for the decls have been specified correctly, and actually entering the
// field into the symbol table...
//
// returns true if any of the fields need initialization in the static
// constructor: i.e., are static, non-const, and have an explicit initializer (or are of struct type).
bool CLSDREC::defineFields(FIELDNODE * pFieldTree, AGGSYM * cls, AGGDECLSYM * clsdecl)
{
    ASSERT(!cls->IsEnum());

    bool isEvent = !!(pFieldTree->other & NFEX_EVENT);

    if (cls->IsInterface() && !isEvent) {
        compiler()->Error(pFieldTree->pDecls, ERR_InterfacesCantContainFields);
        return false;
    }

    TYPESYM * type = TypeBind::BindType(compiler(), pFieldTree->pType, clsdecl);
    ASSERT(type);

    if (type->isSpecialByRefType()) {
        compiler()->Error(pFieldTree->pType, ERR_FieldCantBeRefAny, type);
    }
    if (type->isVOIDSYM()) {
        compiler()->Error(pFieldTree->pType, ERR_FieldCantHaveVoidType);
    }    

    uint flags = pFieldTree->flags;
    bool disallowConst = false;

    if (pFieldTree->kind == NK_CONST && !type->CanBeConst()) {
        compiler()->Error(pFieldTree, ERR_BadConstType, type);
        disallowConst = true;
    }

    // check unsafe on the field so that we give only one error message for all vardecls
    checkUnsafe(pFieldTree, type, (clsdecl->isUnsafe || flags & NF_MOD_UNSAFE));

    bool error = true;
    bool needStaticCtor = false;
    MEMBVARSYM fakeFieldSym;   // used only for abstract events

    NODELOOP(pFieldTree->pDecls, VARDECL, vardecl)

        NAME * name = vardecl->pName->pName;

        //
        // check for name same as that of parent aggregate
        // check for conflicting field...
        //
        CheckForBadMemberSimple(name, vardecl->pName, cls);

        MEMBVARSYM * rval;
        if (cls->IsInterface() || (isEvent && (flags & (NF_MOD_ABSTRACT | NF_MOD_EXTERN)))) {
            // An abstract or extern event does not declare an underlying field. In order
            // to keep the rest of this function simpler, we declare a "fake" field on the 
            // stack and use it.
            ASSERT(isEvent);
            MEM_SET_ZERO(fakeFieldSym);
            rval = &fakeFieldSym;
            rval->name = name;
            rval->parent = cls;
            rval->setKind(SK_MEMBVARSYM);
            rval->declaration = clsdecl;
        }
        else {
            // The usual case.
            rval = compiler()->getBSymmgr().CreateMembVar(name, cls, clsdecl);
        }

        bool isFixed = !!(vardecl->flags & NF_VARDECL_ARRAY);

        rval->type = type;

        // set the value parse tree:
        rval->parseTree = vardecl;
        unsigned allowableFlags =  cls->allowableMemberAccess() | NF_MOD_UNSAFE | NF_MOD_NEW;

        if (cls->IsInterface()) { 
            // event in interface can't have initializer.
            if (vardecl->pArg) 
                compiler()->ErrorRef(NULL, ERR_InterfaceEventInitializer, rval);
        }
        else if (!isFixed) {  // ! cls->IsInterface()
            allowableFlags |= NF_MOD_STATIC;
        }

        // abstract event can't have initializer
        if (isEvent && (flags & NF_MOD_ABSTRACT) && vardecl->pArg)
            compiler()->ErrorRef(NULL, ERR_AbstractEventInitializer, rval);

        // parser guarantees this
        ASSERT(pFieldTree->kind != NK_CONST || vardecl->pArg || compiler()->ErrorCount());

        if (pFieldTree->kind == NK_CONST && vardecl->pArg && !disallowConst) {
            ASSERT(vardecl->pArg->asANYBINOP()->Op() == OP_ASSIGN);

            rval->isUnevaled = true;
            rval->isStatic = true; // consts are implicitly static.

            allowableFlags &= ~NF_MOD_UNSAFE; // consts can't be marked unsafe.

            // can't have static modifier on constant delcaration
            if (error && (flags & NF_MOD_STATIC)) {
                compiler()->ErrorRef(NULL, ERR_StaticConstant, rval);
            }

            // constants of decimal type can't be expressed in COM+ as a literal
            // so we need to initialize them in the static constructor
            if (rval->type->isPredefType(PT_DECIMAL)) {
                needStaticCtor = true;
            }
        } 
        else if (isEvent) {
            // events can be virtual, but in interfaces the modifier is implied and can't be specified directly.
            if (!cls->IsInterface()) {
                allowableFlags |= NF_MOD_VIRTUAL | NF_MOD_ABSTRACT | NF_MOD_EXTERN | NF_MOD_OVERRIDE | NF_MOD_SEALED;   
            }
        }
        else if (isFixed) {
            rval->type = compiler()->getBSymmgr().GetPtrType(type);
            rval->isReferenced = rval->isAssigned = true; // Don't do definite assignment on these guys

            if (!type->isSimpleType() || type->getPredefType() == PT_DECIMAL) {
                // Fixed sized buffers can only be bool, byte, short, int, long, char, sbyte, ushort, uint, ulong, single and double
                // Not IntPtr, UIntPtr, Decimal or any other type
                compiler()->Error(pFieldTree->pType, ERR_IllegalFixedType, type);
                // Just use int.
                type = compiler()->GetReqPredefType(PT_INT);
                rval->type = compiler()->getBSymmgr().GetPtrType(type);
            } else if (!clsdecl->isUnsafe && (flags & NF_MOD_UNSAFE) == 0) {
                ASSERT(!type->isUnsafe()); // all the fixed buffer types are 'safe'
                // But fixed buffers themselves are unsafe, and thus must be in an unsafe context
                compiler()->Error( pFieldTree, ERR_UnsafeNeeded);
            } else if (!cls->IsStruct()) {
                // Fixed sized buffers can only be in structs
                compiler()->ErrorRef(NULL, ERR_FixedNotInStruct, rval);
            }
            else if (compiler()->GetOptPredefTypeErr(PT_FIXEDBUFFER)) {
                rval->isUnevaled = true;
                rval->fixedAgg = MakeFixedBufferStruct(rval, type);
            }
        }
        else {
            // events and consts can't be readonly or volatile.
            allowableFlags |= NF_MOD_READONLY | NF_MOD_VOLATILE;
        }

        rval->isReadOnly = !!(flags & NF_MOD_READONLY);
        rval->isUnsafe = (clsdecl->isUnsafe || (flags & NF_MOD_UNSAFE));

        if (flags & NF_MOD_ABSTRACT && !isEvent)
        {
            ASSERT(!(allowableFlags & NF_MOD_ABSTRACT));
            compiler()->ErrorRef(NULL, ERR_AbstractField, rval);
            flags &= ~NF_MOD_ABSTRACT;
        }
        if (error) {
            checkFlags(rval, allowableFlags, flags);
        }
        
        rval->SetAccess(GetAccessFromFlags(cls, allowableFlags, flags));
        
        if (flags & NF_MOD_STATIC) {
            rval->isStatic = true;

            // If it had an initializer and isn't a const, 
            // then we need a static initializer.
            if (vardecl->pArg && !rval->isUnevaled)
            {
                needStaticCtor = true;
            }
        } else if (!rval->isStatic && cls->IsStruct() && vardecl->pArg && !isFixed) {
            compiler()->ErrorRef(NULL, ERR_FieldInitializerInStruct, rval);
            // Assume it should have been static. We do this for error checking.
            rval->isStatic = true;
            needStaticCtor = true;
        }

        if (flags & NF_MOD_VOLATILE) {
            if (type->isAGGTYPESYM()) {
                VSVERIFY(ResolveInheritanceRec(type->getAggregate()), "ResolveInheritanceRec failed in defineFields!");
            }

            if (!type->CanBeVolatile ())
                compiler()->Error(NULL, ERR_VolatileStruct, ErrArgRef(rval), type);
            else if (rval->isReadOnly) 
                compiler()->ErrorRef(NULL, ERR_VolatileAndReadonly, rval);
            else
                rval->isVolatile = true;
        }

        CheckForProtectedInSealed(rval);

        if (isEvent && (flags & NF_MOD_SEALED) && !(flags & NF_MOD_OVERRIDE))
        {
            compiler()->ErrorRef(NULL, ERR_SealedNonOverride, rval);
        }

        // Check that the field type is as accessible as the field itself.
        checkConstituentVisibility(rval, type, ERR_BadVisFieldType);

        // If this is an event field, define the corresponding event symbol.
        if (isEvent) {
            EVENTSYM * event = defineEvent(rval, vardecl);
            if (cls->IsInterface() || (isEvent && (flags & (NF_MOD_ABSTRACT | NF_MOD_EXTERN))))
                event->implementation = NULL;  // remove link to "fake" field symbol.
        }

        if (cls->IsStatic() && !rval->isStatic) {
            compiler()->ErrorRef(NULL, ERR_InstanceMemberInStaticClass, rval);
            rval->isStatic = true;
            if (vardecl->pArg)
                needStaticCtor = true;
        }

        // don't want to give duplicate errors on subsequent decls...
        error = false;


        ASSERT(rval->parseTree->kind == NK_VARDECL);
    ENDLOOP;

    return needStaticCtor;
}

AGGSYM * CLSDREC::MakeFixedBufferStruct(MEMBVARSYM * field, TYPESYM * type)
{
    AGGDECLSYM * clsdecl = AddSynthAgg( field->name->text, SpecialNameKind::FixedBufferStruct, field->containingDeclaration());
    AGGSYM * cls = clsdecl->Agg();
    cls->SetAggKind(AggKind::Struct);
    cls->isSealed = true;
    cls->SetAccess(ACC_PUBLIC); // other languages need to get at this guy, so make him as visible as his parent
    cls->hasExternReference = true;
    cls->isFixedBufferStruct = true;

    // Now bring the class up to the define state
    compiler()->SetBaseType(cls, compiler()->GetReqPredefType(PT_VALUE));
    compiler()->SetIfaces(cls, NULL, 0);
    defineAggregate(cls);
    cls->setBogus(false);
    cls->SetAggState(AggState::PreparedMembers);

    // FixedElementField - field to store the actual underlying array type
    NAME * pName = compiler()->namemgr->GetPredefName(PN_FIXEDELEMENT);
    MEMBVARSYM * rval = compiler()->getBSymmgr().CreateMembVar(pName, cls, clsdecl);
    rval->SetAccess(ACC_PUBLIC);
    rval->isReferenced = true;
    rval->isAssigned   = true;
    rval->isFabricated = true;
    rval->type = type;

    return cls;
}


/***************************************************************************************************
    The sym must be a property, method or event that was specified in source as an explicit
    interface member implementation. This is signalled by the sym->name == NULL. This sets the
    SymWithType member with the actual interface member being implemented (if one is found).
    If none is found, an ERRORSYM is constructed and stashed so we can manufacture the member name
    at will.
***************************************************************************************************/
void CLSDREC::CheckExplicitImpl(SYM *sym)
{
    ASSERT(!sym->name);

    NAME * name = NULL;
    BASENODE * nodeIface;
    TYPESYM * typeRet;
    TypeArray * params;
    SymWithType * pswtIface;
    AGGDECLSYM * ads;
    symbmask_t mask = sym->mask();

    switch (sym->getKind()) {
    default:
        VSFAIL("Bad sym kind in CheckExplicitImpl");
        return;

    case SK_PROPSYM:
        if (sym->asPROPSYM()->isEvent)
            mask = MASK_EVENTSYM;
        nodeIface = sym->asPROPSYM()->parseTree->asANYPROPERTY()->pName;
        goto LMethProp;

    case SK_METHSYM:
        // Accessors shouldn't come through here!
        ASSERT(!sym->asMETHSYM()->isAnyAccessor());
        nodeIface = sym->asMETHSYM()->parseTree->asMETHOD()->pName;
        sym->asMETHPROPSYM()->fNeedsMethodImp = true;
LMethProp:
        params = sym->asMETHPROPSYM()->params;
        if (params->size > 0 && sym->isPROPSYM())
            name = compiler()->namemgr->GetPredefName(PN_INDEXERINTERNAL);
        typeRet = sym->asMETHPROPSYM()->retType;
        pswtIface = &sym->asMETHPROPSYM()->swtSlot;
        ads = sym->asMETHPROPSYM()->declaration;
        ASSERT(!sym->asMETHPROPSYM()->errExpImpl);
        break;

    case SK_EVENTSYM:
        nodeIface = sym->asEVENTSYM()->parseTree->asANYPROPERTY()->pName;
        params = NULL;
        typeRet = sym->asEVENTSYM()->type;
        pswtIface = &sym->asEVENTSYM()->ewtSlot;
        ads = sym->asEVENTSYM()->declaration;
        ASSERT(!sym->asEVENTSYM()->errExpImpl);
        break;
    }

    // If the name isn't set above, get it from nodeIface.
    if (!name) {
        name = nodeIface->asDOT()->p2->asANYNAME()->pName;
        nodeIface = nodeIface->asDOT()->p1;
    }

    ASSERT(!pswtIface->Sym() && !pswtIface->Type());

    TYPESYM * typeIface = TypeBind::BindNameToType(compiler(), nodeIface, ads);
    ASSERT(typeIface);

    if (typeIface->isERRORSYM()) {
        // Interface didn't exist, error already reported.
        goto LMakeErrorSym;
    }
    if (!typeIface->isInterfaceType()) {
        compiler()->Error(nodeIface, ERR_ExplicitInterfaceImplementationNotInterface,
            ErrArgNameNode(nodeIface), ErrArgRefOnly(typeIface));
LMakeErrorSym:
        ASSERT(!*pswtIface);
        ERRORSYM * err = compiler()->getBSymmgr().GetErrorType(typeIface, name, NULL);
        switch (sym->getKind()) {
        case SK_METHSYM:
        case SK_PROPSYM:
            sym->asMETHPROPSYM()->errExpImpl = err;
            break;
        case SK_EVENTSYM:
            sym->asEVENTSYM()->errExpImpl = err;
            break;
        default:
            VSFAIL("Shouldn't be here");
            break;
        }
        return;
    }

    AGGTYPESYM * atsIface = typeIface->asAGGTYPESYM();
    AGGSYM * aggIface = atsIface->getAggregate();

    // Check that we implement the interface in question.
    if (!ads->Agg()->ifacesAll->Contains(atsIface)) {
        compiler()->ErrorRef(NULL, ERR_ClassDoesntImplementInterface, sym, atsIface);
        prepareAggregate(aggIface);
    }
    ASSERT(aggIface->IsPrepared());

    // For a method we need to map method type variables.
    TypeArray * ptaMeth = sym->isMETHSYM() ? sym->asMETHSYM()->typeVars : NULL;

    SYM * symIface;
    for (symIface = compiler()->getBSymmgr().LookupAggMember(name, aggIface, mask);
        symIface != NULL;
        symIface = compiler()->getBSymmgr().LookupNextSym(symIface, symIface->parent, mask))
    {
        ASSERT(symIface->getKind() == sym->getKind());
        if (symIface->isEVENTSYM()) {
            if (compiler()->getBSymmgr().SubstEqualTypes(typeRet, symIface->asEVENTSYM()->type, atsIface))
                break;
        }
        else {
            // Need the correct arity, param types and return type.
            if ((!symIface->isMETHSYM() || symIface->asMETHSYM()->typeVars->size == ptaMeth->size) &&
                compiler()->getBSymmgr().SubstEqualTypeArrays(params, symIface->asMETHPROPSYM()->params, atsIface, ptaMeth) &&
                compiler()->getBSymmgr().SubstEqualTypes(typeRet, symIface->asMETHPROPSYM()->retType, atsIface, ptaMeth))
            {
                break;
            }
        }
    }

    if (!symIface) {
        if (nodeIface->pParent->kind == NK_DOT)
            compiler()->Error(nodeIface->pParent, ERR_InterfaceMemberNotFound, ErrArgNameNode(nodeIface->pParent), ErrArgRefOnly(atsIface));
        else
            compiler()->Error(nodeIface, ERR_InterfaceMemberNotFound, name, ErrArgRefOnly(atsIface));
        goto LMakeErrorSym;
    }

    SymWithType swt(symIface, atsIface);

    // Check for duplicate explicit implementation.
    SYM * symDup = FindExplicitInterfaceImplementation(ads->Agg()->getThisType(), swt);
    if (symDup) {
        if (nodeIface->pParent->kind == NK_DOT) {
            compiler()->Error(nodeIface->pParent, ERR_MemberAlreadyExists,
                ErrArgNameNode(nodeIface->pParent), ads->Agg(), ErrArgRefOnly(symDup));
        } else {
            compiler()->Error(nodeIface, ERR_MemberAlreadyExists, name, ads->Agg(), ErrArgRefOnly(symDup));
        }
        goto LMakeErrorSym;
    }

    if (sym->isMETHSYM() && swt.Meth()->isAnyAccessor()) {
        sym->asMETHSYM()->errExpImpl = compiler()->getBSymmgr().GetErrorType(typeIface, name, NULL);
        compiler()->ErrorRef(NULL, ERR_ExplicitMethodImplAccessor, sym, swt);
        return;
    }

    *pswtIface = swt;

    if (symIface->hasBogus() && symIface->checkBogus()) {
        compiler()->ErrorRef(NULL, ERR_BogusExplicitImpl, sym, *pswtIface);
        return;
    }

    if (sym->isMETHSYM()) {
        if (sym->asMETHSYM()->isParamArray && !symIface->asMETHSYM()->isParamArray)
            compiler()->ErrorRef(NULL, ERR_ExplicitImplParams, sym, *pswtIface);

        ASSERT(sym->asMETHSYM()->typeVars && symIface->asMETHSYM()->typeVars &&
            sym->asMETHSYM()->typeVars->size == symIface->asMETHSYM()->typeVars->size);
        if (sym->asMETHSYM()->typeVars->size)
            SetOverrideConstraints(sym->asMETHSYM());
    }
}


// Check a "params" style signature to make sure the last argument is of correct type.
bool CLSDREC::CheckParamsType(BASENODE * tree, TYPESYM ** ptypeLast)
{
    ASSERT(ptypeLast && *ptypeLast);

    if ((*ptypeLast)->isPARAMMODSYM()) {
        compiler()->Error(tree, ERR_ParamsCantBeRefOut);
        *ptypeLast = (*ptypeLast)->asPARAMMODSYM()->paramType();
    }

    if (!(*ptypeLast)->isARRAYSYM() || (*ptypeLast)->asARRAYSYM()->rank != 1) {
        compiler()->Error(tree, ERR_ParamsMustBeArray);
        return false;
    }

    return true;
}


/***************************************************************************************************
    Create the parameter array from the nodeParams. *pfParams is an in/out parameter. This will
    be cleared if the last type is bad.
***************************************************************************************************/
void CLSDREC::DefineParameters(PARENTSYM * symCtx, BASENODE * nodeParams, bool fUnsafe, TypeArray ** pta, bool * pfParams)
{
    int ctype = CountListNode(nodeParams);
    TYPESYM ** prgtype = STACK_ALLOC(TYPESYM *, ctype);
    BASENODE * nodeLast = NULL;

    int itype = 0;
    NODELOOP(nodeParams, PARAMETER, param)
        ASSERT(itype < ctype);
        TYPESYM * type = TypeBind::BindType(compiler(), param->pType, symCtx);
        ASSERT(type);

        checkUnsafe(param->pType, type, fUnsafe);

        // Wrap it if the variable is byref.
        if (param->flags & (NF_PARMMOD_REF | NF_PARMMOD_OUT)) {
            type = compiler()->getBSymmgr().GetParamModifier(type, !!(param->flags & NF_PARMMOD_OUT));
            if (type->asPARAMMODSYM()->paramType()->isSpecialByRefType())
                compiler()->Error(param->pType, ERR_MethodArgCantBeRefAny, type);
        }

        prgtype[itype++] = type;
        nodeLast = param;
    ENDLOOP;
    ASSERT(itype == ctype);

    if (pfParams && *pfParams)
        *pfParams = ctype > 0 && CheckParamsType(nodeLast, &prgtype[ctype - 1]);

    *pta = compiler()->getBSymmgr().AllocParams(ctype, prgtype);
}


// defines a method by entering it into the symbol table, also checks for
// duplicates, and makes sure that the flags don't conflict.
METHSYM *CLSDREC::defineMethod(METHODBASENODE *pMethodTree, AGGSYM * cls, AGGDECLSYM * clsdecl)
{
    ASSERT(cls->isSource);

    NAME * name;
    NAMENODE * methodNameTree = NULL;

    bool isStaticCtor = false;

    // figure out the name...
    if (pMethodTree->kind == NK_CTOR) {
        if (cls->IsInterface()) {
            compiler()->Error(pMethodTree, ERR_InterfacesCantContainConstructors);
            return NULL;
        }

        if (cls->IsStatic() && !(pMethodTree->flags & NF_MOD_STATIC)) {
            // Static class must contain a static constructor
            compiler()->Error(pMethodTree, ERR_ConstructorInStaticClass);
        }

        if (pMethodTree->flags & NF_MOD_STATIC) {
            name = compiler()->namemgr->GetPredefName(PN_STATCTOR);
            isStaticCtor = true;
        } else {
            name = compiler()->namemgr->GetPredefName(PN_CTOR);
        }
    } else if (pMethodTree->kind == NK_DTOR) {
        if (!cls->IsClass()) {
            compiler()->Error(pMethodTree, ERR_OnlyClassesCanContainDestructors);
            return NULL;
        }

        if (cls->IsStatic()) {
            compiler()->Error(pMethodTree, ERR_DestructorInStaticClass);
        }

        name = compiler()->namemgr->GetPredefName(PN_DTOR);
    } else {
        BASENODE * nameTree = pMethodTree->asMETHOD()->pName;

        if (cls->IsStatic() && !(pMethodTree->flags & NF_MOD_STATIC)) {
            // Methods in a static class must be declared static.
            compiler()->Error(nameTree, ERR_InstanceMemberInStaticClass, ErrArgNameNode(nameTree));
        }

        if (nameTree->IsAnyName()) {
            name = nameTree->asANYNAME()->pName;
            methodNameTree = nameTree->asANYNAME();
        } else {
            ASSERT(nameTree->kind == NK_DOT);
            methodNameTree = nameTree->asDOT()->p2->asANYNAME();

            if (!cls->IsClass() && !cls->IsStruct()) {
                compiler()->Error(nameTree, ERR_ExplicitInterfaceImplementationInNonClassOrStruct, ErrArgNameNode(nameTree));
                return NULL;
            }

            // we handle explicit interface implementations in the prepare stage
            // when we have the complete list of interfaces
            name = NULL;
        }
    }

    METHSYM * rval = compiler()->getBSymmgr().CreateMethod(name, cls, clsdecl);
    rval->parseTree = pMethodTree;

    // GENERICS: declare type parameters - these may be used in the types below.
    DefineMethodTypeVars(rval, (methodNameTree && methodNameTree->kind == NK_GENERICNAME) ? methodNameTree->asGENERICNAME()->pParams : NULL);
    if (pMethodTree->kind == NK_METHOD)
        DefineBounds(pMethodTree->asMETHOD()->pConstraints, rval, true);

    rval->isUnsafe = ((pMethodTree->flags & NF_MOD_UNSAFE) || rval->containingDeclaration()->isUnsafe);

    // bind all the parameters and do some error checking
    {
        if (rval->parseTree->other & NFEX_METHOD_VARARGS) {
            // Old style varargs. Not allowed in generics or with params.
            if ((rval->parseTree->other & NFEX_METHOD_PARAMS) || rval->typeVars->size > 0 || cls->typeVarsAll->size > 0)
                compiler()->Error(rval->parseTree, ERR_BadVarargs);
            else
                rval->isVarargs = true;
        }

        bool fParams = !!(rval->parseTree->other & NFEX_METHOD_PARAMS);
        DefineParameters(rval, pMethodTree->pParms, rval->isUnsafe, &rval->params, &fParams);
        rval->isParamArray = fParams;

        if (rval->isVarargs) {
            // Tack on the extra type.
            rval->params = compiler()->getBSymmgr().ConcatParams(rval->params, compiler()->getBSymmgr().GetArglistSym());
        }
    }

    TYPESYM * retType;
    // bind the return type, or assume void for constructors...
    if (pMethodTree->pType) {
        retType = TypeBind::BindType(compiler(), pMethodTree->pType, rval);
        ASSERT(retType);

        checkUnsafe(pMethodTree->pType, retType, rval->isUnsafe);
        if (retType->isSpecialByRefType() && !cls->getThisType()->isSpecialByRefType()) { // Allow return type for themselves
            compiler()->Error(pMethodTree->pType, ERR_MethodReturnCantBeRefAny, retType);
        }

        if (rval->name == compiler()->namemgr->GetPredefName(PN_DTOR) &&
            rval->params->size == 0 &&
            rval->typeVars->size == 0 &&
            retType->isVOIDSYM())
        {
            compiler()->Error(pMethodTree, WRN_FinalizeMethod);
        }
    } else {
        ASSERT(pMethodTree->kind == NK_CTOR || pMethodTree->kind == NK_DTOR);
        retType = compiler()->getBSymmgr().GetVoid();
    }

    if (name) {
        if (cls->IsStruct() && (pMethodTree->kind == NK_CTOR) && (rval->params->size == 0) && !isStaticCtor) {
            compiler()->Error(pMethodTree, ERR_StructsCantContainDefaultContructor);

        } else {
            // for non-explicit interface implementations
            // find another method with the same signature
            CheckForBadMember(name, SK_METHSYM, rval->params, pMethodTree, cls, rval->typeVars, rval);
        }

    } else {
        //
        // method is an explicit interface implementation
        //
    }

    rval->retType = retType;
    
    if (pMethodTree->flags & NF_MOD_STATIC) {
        rval->isStatic = true;
    }

    // constructors have a much simpler set of allowed flags:
    if (pMethodTree->kind == NK_CTOR) {
        rval->SetMethKind(MethodKind::Ctor);

        //
        // NOTE:
        //
        // NF_CTOR_BASE && NF_CTOR_THIS have the same value as
        // NF_MOD_ABSTRACT && NF_MOD_NEW we mask them out so that
        // we don't get spurious conflicts with NF_MOD_STATIC
        //
        checkFlags(rval, cls->allowableMemberAccess() | NF_MOD_STATIC | NF_MOD_EXTERN | NF_MOD_UNSAFE, (NODEFLAGS) pMethodTree->flags);
        rval->SetAccess(GetAccessFromFlags(cls, cls->allowableMemberAccess(), (NODEFLAGS) pMethodTree->flags));

        if (rval->isStatic) {
            //
            // static constructors can't have explicit constructor calls,
            // access modifiers or parameters
            //
            if (pMethodTree->other & (NFEX_CTOR_BASE | NFEX_CTOR_THIS)) {
                compiler()->ErrorRef(NULL, ERR_StaticConstructorWithExplicitConstructorCall, rval);
            }
            if (pMethodTree->flags & NF_MOD_ACCESSMODIFIERS) {
                compiler()->ErrorRef(NULL, ERR_StaticConstructorWithAccessModifiers, rval);
            }
            if (rval->params->size) {
                compiler()->ErrorRef(NULL, ERR_StaticConstParam, rval);
            }
        }

        if (pMethodTree->flags & NF_MOD_EXTERN) {
            rval->isExternal = true;
        }
    } else if (pMethodTree->kind == NK_DTOR) {

        // no modifiers are allowed on destructors
        checkFlags(rval, NF_MOD_UNSAFE | NF_MOD_EXTERN, (NODEFLAGS) pMethodTree->flags);

        rval->SetAccess(ACC_PROTECTED);
        rval->SetMethKind(MethodKind::Dtor);
        rval->isVirtual = true;
        rval->isMetadataVirtual = true;
        rval->isOverride = true;

        if (pMethodTree->flags & NF_MOD_EXTERN) {
            rval->isExternal = true;
        }

    } else if (name) {
        // methods a somewhat more complicated one...
        if (pMethodTree->flags & NF_MOD_ABSTRACT || cls->IsInterface()) {
            rval->isAbstract = true;
        }
        if (pMethodTree->flags & NF_MOD_OVERRIDE) {
            rval->isOverride = true;
        }
        if ((pMethodTree->flags & NF_MOD_EXTERN) && !rval->isAbstract) {
            rval->isExternal = true;
        }
        if (pMethodTree->flags & NF_MOD_VIRTUAL ||
            rval->isOverride ||
            rval->isAbstract)
        {
            // abstract implies virtual
            rval->isVirtual = true;
            rval->isMetadataVirtual = true;
        }
        if (pMethodTree->flags & NF_MOD_SEALED) {
            if (!rval->isOverride)
                compiler()->ErrorRef(NULL, ERR_SealedNonOverride, rval);
            else {
                // Note: a sealed override is marked with isOverride=true, isVirtual=false.
                rval->isVirtual = false;
                ASSERT(rval->isMetadataVirtual);
            }
        }

        unsigned allowableFlags = cls->allowableMemberAccess();
        switch (cls->AggKind()) {
        case AggKind::Class:
            allowableFlags |= NF_MOD_ABSTRACT | NF_MOD_VIRTUAL | NF_MOD_NEW | NF_MOD_OVERRIDE | NF_MOD_SEALED | NF_MOD_STATIC | NF_MOD_UNSAFE | NF_MOD_EXTERN;
            break;
        case AggKind::Struct:
            allowableFlags |= NF_MOD_NEW | NF_MOD_OVERRIDE | NF_MOD_STATIC | NF_MOD_UNSAFE | NF_MOD_EXTERN;
            break;
        case AggKind::Interface:
            // interface members can only have new and unsafe
            allowableFlags |= NF_MOD_NEW | NF_MOD_UNSAFE;
            break;
        default:
            ASSERT(0);
        }

        checkFlags(rval, allowableFlags, (NODEFLAGS)pMethodTree->flags);
        rval->SetAccess(GetAccessFromFlags(cls, allowableFlags, (NODEFLAGS)pMethodTree->flags));

        if (rval->isVirtual && rval->GetAccess() == ACC_PRIVATE) {
            compiler()->ErrorRef(NULL, ERR_VirtualPrivate, rval);
        }

    } else {
        // explicit interface implementation
        // can't have any flags
        checkFlags(rval, NF_MOD_UNSAFE | NF_MOD_EXTERN, (NODEFLAGS) pMethodTree->flags);
        rval->SetAccess(ACC_PRIVATE);
        rval->isMetadataVirtual = true; // explicit impls need to be virtual when emitted
        if (pMethodTree->flags & NF_MOD_EXTERN)
            rval->isExternal = true;
    }

    CheckForProtectedInSealed(rval);

    ASSERT(rval->typeVars && rval->params);

    // Check return type, parameter types and constraints for correct visibility.
    checkConstituentVisibility(rval, retType, ERR_BadVisReturnType);

    int cParam = rval->params->size;
    if (rval->isVarargs) cParam--; // don't count the sentinel
    for (int i = 0; i < cParam; ++i) 
        checkConstituentVisibility(rval, rval->params->Item(i), ERR_BadVisParamType);

    if (rval->typeVars->size > 0)
        CheckBoundsVisibility(rval, rval->typeVars);

    return rval;
}


// Make the aggregate nested type, the Current, MoveNext, and Reset methods
// and the few known fields ($__current, THIS and $__state)
// more fields will get added when MoveNext is compiled
METHSYM * CLSDREC::MakeIterator(METHINFO * info)
{
    ASSERT(info->piin);

    AGGSYM     * cls = NULL;        // The class we're building up
    AGGDECLSYM * clsdecl = NULL;    // It's one and only decl (which is the same as it's method)
    int ctype;
    AGGTYPESYM * rgtype[5]; // Interface list
    TypeArray  * clsTyPars = NULL;  // Converted to use the new classes type parameters
    METHSYM    * pMoveNextMeth = NULL;  // The 'real' move next method that we'll pass out
    MEMBVARSYM * rval = NULL;       // Temp used for some fields
    NAME       * pName = NULL;      // Temp

    // Force these up front
    AGGTYPESYM * atsAble = compiler()->GetOptPredefType(PT_G_IENUMERABLE);
    AGGTYPESYM * atsAtor = compiler()->GetOptPredefType(PT_G_IENUMERATOR);

    if (!info->meth->retType->isAGGTYPESYM() ||
        info->meth->retType->HasErrors() ||
        info->meth->params->HasErrors())
    {
        return NULL;
    }

    // figure out what we need to implement
    info->piin->fGeneric = (atsAble != NULL && atsAtor != NULL);
    if (!info->piin->fGeneric) {
        // Clear these in case we got one without the other
        atsAble = NULL;
        atsAtor = NULL;
    }

    info->piin->fEnumerable = (atsAble != NULL && info->meth->retType->getAggregate() == atsAble->getAggregate() ||
        info->meth->retType->isPredefType(PT_IENUMERABLE));

    // Get a unique base name (derived from the method name)
    // to use as a prefix for all the generated classes
    PCWSTR szBasename = NULL;
    if (info->meth->name == NULL) {
        StringBldrNrHeap str(compiler()->getLocalSymHeap());
        MetaDataHelper hlpr;
        hlpr.GetExplicitImplName( info->meth, str);
        szBasename = str.Str();
    }
    else {
        szBasename = info->meth->name->text;
    }

    // Create the class
    clsdecl = AddSynthAgg(szBasename, SpecialNameKind::IteratorClass, info->meth->containingDeclaration());
    cls = clsdecl->Agg();

    // We also need to add in any possible method TyVars
    CopyMethTyVarsToClass(info->meth, cls);

    // Get the yield type by substituting method TyVars with class TyVars
    TYPESYM * typeYield = compiler()->getBSymmgr().SubstType(info->yieldType, (TypeArray *)NULL, cls->typeVarsThis);

    // Use the yield type to create type argument list for IEnumerable<T> and IEnumerator<T>
    clsTyPars = compiler()->getBSymmgr().AllocParams(1, &typeYield);

    // Get the interface list IEnumerable<T>, IEnumerable, IEnumerator<T>, IEnumerator, IDisposable
    ctype = 0;
    if (info->piin->fEnumerable) {
        if (info->piin->fGeneric)
            rgtype[ctype++] = compiler()->getBSymmgr().SubstType(atsAble, clsTyPars)->asAGGTYPESYM();
        rgtype[ctype++] = compiler()->GetReqPredefType(PT_IENUMERABLE);
    }
    if (info->piin->fGeneric)
        rgtype[ctype++] = compiler()->getBSymmgr().SubstType(atsAtor, clsTyPars)->asAGGTYPESYM();
    rgtype[ctype++] = compiler()->GetReqPredefType(PT_IENUMERATOR);
    rgtype[ctype++] = compiler()->GetReqPredefType(PT_IDISPOSABLE);
    compiler()->SetIfaces(cls, rgtype, ctype);

    // Now bring the class up to the define state
    compiler()->SetBaseType(cls, compiler()->GetReqPredefType(PT_OBJECT));
    defineAggregate(cls);
    cls->setBogus(false);
    cls->SetAggState(AggState::PreparedMembers);
    info->piin->aggIter = cls;

    if (info->piin->fEnumerable) {
        if (info->piin->fGeneric) {
            // IEnumerable<T>.GetEnumerator()
            FabricateExplicitImplMethod(PN_GETENUMERATOR, rgtype[0], clsdecl);
        }
        // IEnumerable.GetEnumerator()
        FabricateExplicitImplMethod(PN_GETENUMERATOR, compiler()->GetReqPredefType(PT_IENUMERABLE), clsdecl);
    }

    // IEnumerator.MoveNext (Give it the "MoveNext" name, but then make it an explicit impl
    pMoveNextMeth = FabricateSimpleMethod(PN_MOVENEXT, clsdecl, compiler()->GetReqPredefType(PT_BOOL));
    if (pMoveNextMeth) {
        METHSYM * methIface = compiler()->funcBRec.FindPredefMeth(NULL, PN_MOVENEXT, rgtype[ctype - 2], BSYMMGR::EmptyTypeArray());
        if (methIface) {
            pMoveNextMeth->SetAccess(ACC_PRIVATE);

            // Make a public method symbol representing a method of type retType, with a given parent.

            pMoveNextMeth->swtSlot.Set(methIface, rgtype[ctype - 2]);
            pMoveNextMeth->fNeedsMethodImp = true;
            pMoveNextMeth->isMetadataVirtual = true;
        }
    }

    if (info->piin->fGeneric) {
        info->yieldType = typeYield;

        // IEnumerator<T>.Current
        FabricateExplicitImplPropertyRO(PN_CURRENT, rgtype[ctype - 3], clsdecl);
    } else {
        ASSERT(info->yieldType == compiler()->GetReqPredefType(PT_OBJECT));
    }

    // $__current
    pName = compiler()->namemgr->GetPredefName(PN_ITERCURRENT);
    rval = compiler()->getBSymmgr().CreateMembVar(pName, cls, clsdecl);
    rval->SetAccess(ACC_PRIVATE);
    rval->isReferenced = true;
    rval->isAssigned   = true;
    rval->isFabricated = true;
    rval->type = info->yieldType;

    // IEnumerator.Reset (this will always throw)
    FabricateExplicitImplMethod(PN_RESET, rgtype[ctype - 2], clsdecl);

    // Dispose() - explicit impl
    FabricateExplicitImplMethod(PN_DISPOSE, rgtype[ctype - 1], clsdecl);

    TYPESYM * atsInt = compiler()->GetReqPredefType(PT_INT);

    // $__state - field to track state info
    pName = compiler()->namemgr->GetPredefName(PN_ITERSTATE);
    rval = compiler()->getBSymmgr().CreateMembVar(pName, cls, clsdecl);
    rval->SetAccess(ACC_PRIVATE);
    rval->isReferenced = true;
    rval->isAssigned   = true;
    rval->isFabricated = true;
    rval->type = atsInt;

    // IEnumerator.Current properties
    FabricateExplicitImplPropertyRO(PN_CURRENT, rgtype[ctype - 2], clsdecl);

    // add the .ctor
    METHSYM * ctor = FabricateSimpleMethod(PN_CTOR, clsdecl, compiler()->getBSymmgr().GetVoid());
    ctor->SetMethKind(MethodKind::Ctor);
    ctor->params = compiler()->getBSymmgr().AllocParams(1, &atsInt);

    return pMoveNextMeth;
}

// FabricateExplicitImplPropertyRO creates an Explicit Interface Implementation of a read-only property
PROPSYM * CLSDREC::FabricateExplicitImplPropertyRO(PREDEFNAME pn, AGGTYPESYM * iface, AGGDECLSYM * ads)
{
    NAME * name = compiler()->namemgr->GetPredefName(pn);
    PROPSYM * propIface = compiler()->getBSymmgr().LookupAggMember(name, iface->getAggregate(), MASK_PROPSYM)->asPROPSYM();

    if (!propIface || !propIface->methGet || propIface->params->size) {
        compiler()->ErrorRef(NULL, ERR_MissingPredefinedMember, iface, name);
        return NULL;
    }

    // Make a private property symbol pointing to the iface property, with a given parent.
    PROPSYM * prop = compiler()->getBSymmgr().CreateProperty(NULL, ads->Agg(), ads);
    prop->SetAccess(ACC_PRIVATE);
    prop->retType = compiler()->getBSymmgr().SubstType(propIface->retType, iface);
    prop->params = BSYMMGR::EmptyTypeArray();
    prop->swtSlot.Set(propIface, iface);

    // Make the accessor
    name = createAccessorName(prop->getRealName(), L"get_");
    METHSYM * methGet = compiler()->getBSymmgr().CreateMethod(name, ads->Agg(), ads);
    methGet->SetAccess(ACC_PRIVATE);
    methGet->SetMethKind(MethodKind::PropAccessor);
    methGet->SetProperty(prop);
    methGet->retType = prop->retType;
    methGet->isMetadataVirtual = true;
    methGet->isFabricated = true;
    methGet->params = BSYMMGR::EmptyTypeArray();
    methGet->typeVars = BSYMMGR::EmptyTypeArray();
    methGet->swtSlot.Set(propIface->methGet, iface);
    methGet->fNeedsMethodImp = true;

    prop->methGet = methGet;

    return prop;
}

// FabricateExplicitImplMethod creates an Explicit Interface Implementation method
METHSYM * CLSDREC::FabricateExplicitImplMethod(PREDEFNAME pn, AGGTYPESYM * iface, AGGDECLSYM * ads)
{
    METHSYM * methIface = compiler()->funcBRec.FindPredefMeth(NULL, pn, iface, BSYMMGR::EmptyTypeArray());

    if (!methIface) {
        // FindPredefMeth already reported the error....
        return NULL;
    }

    // Make sure FindPredefMeth did its job....
    ASSERT(methIface->getClass() == iface->getAggregate());
    ASSERT(methIface->typeVars->size == 0 && methIface->params->size == 0);

    // Make a public method symbol representing a method of type retType, with a given parent.
    METHSYM * meth = compiler()->getBSymmgr().CreateMethod(NULL, ads->Agg(), ads);

    meth->SetAccess(ACC_PRIVATE);
    meth->retType = compiler()->getBSymmgr().SubstType(methIface->retType, iface);
    meth->params = BSYMMGR::EmptyTypeArray();
    meth->typeVars = BSYMMGR::EmptyTypeArray();
    meth->swtSlot.Set(methIface, iface);
    meth->fNeedsMethodImp = true;
    meth->isMetadataVirtual = true;
    meth->isFabricated = true;

    return meth;
}

// FabricateSimpleMethod creates a simple method
METHSYM* CLSDREC::FabricateSimpleMethod(PREDEFNAME pn, AGGDECLSYM * ads, TYPESYM * typeRet)
{
    // Make a public method symbol representing a method of type retType name(), with a given parent.
    METHSYM * meth = compiler()->getBSymmgr().CreateMethod(compiler()->namemgr->GetPredefName(pn), ads->Agg(), ads);

    meth->SetAccess(ACC_PUBLIC);
    meth->retType = typeRet;
    meth->params = BSYMMGR::EmptyTypeArray();
    meth->typeVars = BSYMMGR::EmptyTypeArray();
    meth->isFabricated = true;

    return meth;
}

void CLSDREC::CopyMethTyVarsToClass( METHPROPSYM * pMeth, AGGSYM * cls)
{
    ASSERT(!cls->isArityInName);

    if (pMeth && pMeth->isMETHSYM() && pMeth->asMETHSYM()->typeVars->size) {
        METHSYM * pOuterMeth = pMeth->asMETHSYM();
        int cTypeFormals = pOuterMeth->typeVars->size;
        int cptsPar = cls->Parent()->asAGGSYM()->typeVarsAll->size;

        // Make a TypeArray for the TYVARs copied from the method
        TYVARSYM ** ppTypeFormals = STACK_ALLOC(TYVARSYM*, cTypeFormals);
        for (int i = 0; i < cTypeFormals; i++) {
            TYVARSYM * pOuterMethTyVar = pOuterMeth->typeVars->ItemAsTYVARSYM(i);
            ppTypeFormals[i] = CreateTypeVar(cls, pOuterMethTyVar->parseTree, 
                i, ppTypeFormals, cptsPar + i);
            ppTypeFormals[i]->attributeList = pOuterMethTyVar->attributeList;
            ppTypeFormals[i]->attributeListTail = NULL;    // setting to NULL, because nothing should be adding more nodes on this list
        }
        cls->typeVarsThis = compiler()->getBSymmgr().AllocParams(cTypeFormals, (TYPESYM **) ppTypeFormals);
        cls->typeVarsAll = compiler()->getBSymmgr().ConcatParams(cls->Parent()->asAGGSYM()->typeVarsAll, cls->typeVarsThis);

        // Now fixup the bounds to point to the class' TYVARs
        // ... and copy the special bit constraints as well
        for (int i = 0; i < cTypeFormals; i++) {
            TYVARSYM * pOuterMethTyVar = pOuterMeth->typeVars->ItemAsTYVARSYM(i);
            compiler()->SetBounds(ppTypeFormals[i],
                compiler()->getBSymmgr().SubstTypeArray(pOuterMethTyVar->GetBnds(), (TypeArray*)NULL, cls->typeVarsThis));
            ppTypeFormals[i]->cons = pOuterMethTyVar->cons;
        }
        for (int i = 0; i < cTypeFormals; i++) {
            VSVERIFY(compiler()->ResolveBounds(ppTypeFormals[i], true), "ResolveBounds failed!");
        }
        cls->isArityInName = true;
    } else {
        ASSERT(cls->typeVarsThis == BSYMMGR::EmptyTypeArray());
        ASSERT(cls->typeVarsAll == cls->Parent()->asAGGSYM()->typeVarsAll);
    }
}

// Creates all the containing class and ctor to hold an anonymous method
// pOuterMeth is the method declaring the anonymous delegate
AGGSYM * CLSDREC::CreateAnonymousMethodClass(METHSYM * methOuter)
{
    // Create the class
    AGGDECLSYM * clsdecl = AddSynthAgg(NULL, SpecialNameKind::AnonymousMethodDisplayClass, methOuter->containingDeclaration());
    AGGSYM * cls = clsdecl->Agg();
    ASSERT(cls->DeclOnly() == clsdecl);

    // We also need to add in any possible method TyVars
    CopyMethTyVarsToClass( methOuter, cls);

    compiler()->SetIfaces( cls, NULL, 0);

    compiler()->SetBaseType(cls, compiler()->GetReqPredefType(PT_OBJECT));
    defineAggregate(cls);
    cls->setBogus(false);
    cls->SetAggState(AggState::PreparedMembers);

    // .ctor
    {
        METHSYM * pCtor = FabricateSimpleMethod(PN_CTOR, clsdecl, compiler()->getBSymmgr().GetVoid());
        pCtor->SetMethKind(MethodKind::Ctor);
        ASSERT(pCtor->params == BSYMMGR::EmptyTypeArray());
        ASSERT(pCtor->typeVars == BSYMMGR::EmptyTypeArray());
    }

    return cls;
}

void CLSDREC::findEntryPoint(AGGSYM* cls)
{
    // We've found the specified class, now let's look for a Main
    METHSYM *method;

    method = compiler()->getBSymmgr().LookupAggMember(compiler()->namemgr->GetPredefName(PN_MAIN), cls, MASK_METHSYM)->asMETHSYM();

    while (method != NULL) {
        // Must be public, static, void/int Main ()
        // with no args or String[] args
        // If you change this code also change the code in EMITTER::FindEntryPointInClass (it does basically the same thing)
        if (method->isStatic && !method->isPropertyAccessor()) {
            // Check the signature.
            if ((method->retType == compiler()->getBSymmgr().GetVoid() || method->retType->isPredefType(PT_INT)) &&
                (method->params->size == 0 ||
                (method->params->size == 1 && method->params->Item(0)->isARRAYSYM() &&
                    method->params->Item(0)->asARRAYSYM()->elementType()->isPredefType(PT_STRING))))
            {
                // Make sure it's not generic.
                if (method->typeVars->size == 0 && cls->typeVarsAll->size == 0) {
                    compiler()->emitter.SetEntryPoint(method);
                }
                else {
                    compiler()->ErrorRef(NULL, WRN_MainCantBeGeneric, method);
                }
            }
            else {
                compiler()->ErrorRef(NULL, WRN_InvalidMainSig, method);
            }
        }

        SYM *next = method->nextSameName;
        method = NULL;
        while (next != NULL) {
            if (next->isMETHSYM()) {
                method = next->asMETHSYM();
                break;
            }
            next = next->nextSameName;
        }
    }
}


// defines a property by entering it into the symbol table, also checks for
// duplicates, and makes sure that the flags don't conflict.
// also adds methods for the property's accessors
void CLSDREC::defineProperty(PROPERTYNODE *propertyNode, AGGSYM *cls, AGGDECLSYM * clsdecl)
{
    TypeArray *params = NULL;


    BASENODE *nameTree = propertyNode->pName;
    BASENODE *nodeName = nameTree;
    bool isEvent = !!(propertyNode->other & NFEX_EVENT); // is it really an event?
    bool isIndexer = propertyNode->kind == NK_INDEXER;

    //
    // Get the property name
    //
    NAME *name;

    bool isExplicitImpl = (isIndexer ? !!nameTree : nameTree->kind != NK_NAME);
    if (!isExplicitImpl) {
        if (isIndexer) {
            // name defaults to "Item", may be overwritten later during prepareProperty when we bind the IndexerNameAttribute
            name = compiler()->namemgr->GetPredefName(PN_INDEXER);
        } else {
            name = nameTree->asNAME()->pName;
            params = BSYMMGR::EmptyTypeArray();
        }
    } else {
        ASSERT(isIndexer || nameTree->kind == NK_DOT);

        //
        // explicit interface implementation
        //
        if (!cls->IsClass() && !cls->IsStruct()) {
            compiler()->Error(nameTree, ERR_ExplicitInterfaceImplementationInNonClassOrStruct, ErrArgNameNode(nameTree));
            return;
        }

        // we handle explicit interface implementations in the prepare stage
        // when we have the complete list of interfaces
        name = NULL;
        params = BSYMMGR::EmptyTypeArray();

        if (!isIndexer)
            nodeName = nameTree->asDOT()->p2;
    }

    //
    // get the property's type. It can't be void.
    //
    TYPESYM * type = TypeBind::BindType(compiler(), propertyNode->pType, clsdecl);
    ASSERT(type);

    if (type->isVOIDSYM()) {
        if (isIndexer) {
            compiler()->Error(propertyNode, ERR_IndexerCantHaveVoidType);
        } else {
            if (nameTree) {
                compiler()->Error(nameTree, ERR_PropertyCantHaveVoidType, ErrArgNameNode(nameTree));
            } else {
                compiler()->Error(propertyNode, ERR_PropertyCantHaveVoidType, name);
            }
        }
    }

    if (type->isSpecialByRefType())
        compiler()->Error(propertyNode->pType, ERR_FieldCantBeRefAny, type);

    bool fUnsafe = ((propertyNode->flags & NF_MOD_UNSAFE) || clsdecl->isUnsafe);
    bool fParams;

    // define indexer parameters
    if (isIndexer) {
        fParams = !!(propertyNode->other & NFEX_METHOD_PARAMS);
        DefineParameters(clsdecl, propertyNode->pParms, fUnsafe, &params, &fParams);
        CheckForBadMember(name ? compiler()->namemgr->GetPredefName(PN_INDEXERINTERNAL) : NULL, SK_PROPSYM, params, propertyNode, cls, NULL);
    } else {
        fParams = false;
        // all we need to check is whether or not the name is valid (no parameters / type vars).
        CheckForBadMemberSimple(name, nameTree, cls);
    }

    //
    // create the symbol for the property
    //
    PROPSYM *property;
    PROPSYM fakePropSym;
    if (isEvent) {
        // A new-style event does not declare an underlying property. In order to keep
        // the rest of this function simpler, we declare a "fake" property on the stack
        // and use it.
        MEM_SET_ZERO(fakePropSym);
        property = &fakePropSym;
        property->name = name;
        property->parent = cls;
        property->declaration = clsdecl;
        property->setKind(SK_PROPSYM);
    }
    else {
        if (isIndexer) {
            property = compiler()->getBSymmgr().CreateIndexer(name, cls, clsdecl);
        } else {
            property = compiler()->getBSymmgr().CreateProperty(name, cls, clsdecl);
        }
        ASSERT(isIndexer || nodeName && nodeName->IsAnyName());
    }
    ASSERT(name || !property->getRealName());
    property->parseTree = propertyNode;
    property->retType = type;

    // indexers should have at least one paramter
    property->params = params;
    property->isParamArray = fParams;

    // indexers are the only property kind that can have param arrays
    ASSERT(!property->isParamArray || isIndexer);

    property->isUnsafe = fUnsafe;

    checkUnsafe(property->parseTree, property->retType, property->isUnsafe);

    if ((propertyNode->flags & NF_MOD_STATIC) != 0) {
        property->isStatic = true;
    }
    property->isOverride = !!(propertyNode->flags & NF_MOD_OVERRIDE);
    if ((propertyNode->flags & NF_MOD_SEALED) && !property->isOverride) {
        compiler()->ErrorRef(NULL, ERR_SealedNonOverride, property);
    }

    //
    // check the flags. Assigns access.
    //
    unsigned allowableFlags;
    if (property->name && cls->IsClass()) {
        allowableFlags = NF_MOD_ABSTRACT | NF_MOD_EXTERN | NF_MOD_VIRTUAL | NF_MOD_OVERRIDE | NF_MOD_SEALED | NF_MOD_UNSAFE; 
    } else { // interface member or explicit interface impl
        allowableFlags = NF_MOD_UNSAFE;
    }
    if (name && (cls->IsClass() || cls->IsStruct())) {
        allowableFlags |= cls->allowableMemberAccess() | NF_MOD_NEW | NF_MOD_UNSAFE;

        //
        // indexers can't be static
        //
        if (!isIndexer) {
            allowableFlags |= NF_MOD_STATIC;
        }
    } else if (cls->IsInterface()) { // interface members
        // interface members 
        // have no allowable flags
        allowableFlags |= NF_MOD_NEW;
    } else { // explicit interface impls
        ASSERT(!name);
        allowableFlags |= NF_MOD_UNSAFE | NF_MOD_EXTERN;
    }
    checkFlags(property, allowableFlags, (NODEFLAGS) propertyNode->flags);
    property->SetAccess(GetAccessFromFlags(cls, allowableFlags, propertyNode->flags));

    // Check return and parameter types for correct visibility.
    checkConstituentVisibility(property, type, isIndexer ? ERR_BadVisIndexerReturn : ERR_BadVisPropertyType);

    for (int i = 0; i < params->size; ++i) 
        checkConstituentVisibility(property, params->Item(i), ERR_BadVisIndexerParam);

    if (cls->IsStatic()) {
        if (isIndexer) {
            //Indexers are not allowed in static classes
            compiler()->ErrorRef(NULL, ERR_IndexerInStaticClass, property);
        }
        else if (!(propertyNode->flags & NF_MOD_STATIC)) {
            //Instance properties not allowed in static classes
            compiler()->ErrorRef(NULL, ERR_InstanceMemberInStaticClass, property);
        }
    }

    if (isEvent) {
        // events must have both accessors
        if (!propertyNode->pGet || !propertyNode->pSet) 
            compiler()->ErrorRef(NULL, ERR_EventNeedsBothAccessors, property);

        // If this is an event property, define the corresponding event symbol.
        EVENTSYM * event = defineEvent(property, propertyNode);
        event->implementation = NULL; // remove link to "fake" property symbol.
        ASSERT(nodeName && nodeName->IsAnyName());
    }
    else {
        // must have at least one accessor
        if (!propertyNode->pGet && !propertyNode->pSet) 
            compiler()->ErrorRef(NULL, ERR_PropertyWithNoAccessors, property);

        // disallow private virtual properties
        if (propertyNode->flags & (NF_MOD_VIRTUAL | NF_MOD_ABSTRACT | NF_MOD_OVERRIDE) && property->GetAccess() == ACC_PRIVATE)
            compiler()->ErrorRef(NULL, ERR_VirtualPrivate, property);
    }

    ASSERT(property->params);
}

// creates an internal name for user defined property accessor methods
// just prepends the name with "Get" or "Set"
PNAME CLSDREC::createAccessorName(PNAME propertyName, PCWSTR prefix)
{
    if (propertyName) {
        size_t prefixLen = wcslen(prefix);
        size_t nameBufferLen = 1 + prefixLen + wcslen(propertyName->text);
        WCHAR *nameBuffer = STACK_ALLOC(WCHAR, nameBufferLen);
        HRESULT hr;
        hr = StringCchCopyW (nameBuffer, nameBufferLen, prefix);
        ASSERT (SUCCEEDED (hr));
        hr = StringCchCopyW (
            nameBuffer + prefixLen,
            nameBufferLen - prefixLen,
            propertyName->text);
        ASSERT (SUCCEEDED (hr));
        return compiler()->namemgr->AddString(nameBuffer);
    } else {
        // explicit interface implementation accessors have no name
        // just like the properties they are contained on.
        return NULL;
    }
}

// define an event by entering it into the symbol table.
// the underlying implementation of the event, a field or property, has already been defined,
// and we do not duplicate checks performed there. We do check that the event
// is of a delegate type. The access modifier on the implementation is changed to 
// private and the event inherits the access modifier of the underlying field. The event
// accessor methods are defined.
EVENTSYM * CLSDREC::defineEvent(SYM * implementingSym, BASENODE * pTree)
{
    EVENTSYM * event;
    bool isField;
    bool isStatic;
    AGGSYM * cls;
    AGGDECLSYM * clsdecl;

    ASSERT(implementingSym->isPROPSYM() || implementingSym->isMEMBVARSYM());

#ifdef DEBUG
    bool isPropertyEvent = (pTree->kind == NK_PROPERTY);
#endif

    // An event must be implemented by a property or field symbol.
    ASSERT(implementingSym->isPROPSYM() || implementingSym->isMEMBVARSYM());
    isField = (implementingSym->isMEMBVARSYM());
    ASSERT((isField && !isPropertyEvent) || (isPropertyEvent && !isField));
    isStatic = isField ? implementingSym->asMEMBVARSYM()->isStatic : implementingSym->asPROPSYM()->isStatic;

    // Create the event symbol.
    cls = implementingSym->parent->asAGGSYM();
    clsdecl = implementingSym->containingDeclaration()->asAGGDECLSYM();
    event = compiler()->getBSymmgr().CreateEvent(implementingSym->name, cls, clsdecl);
    event->implementation = implementingSym;
    event->isStatic = isStatic;
    event->isUnsafe = implementingSym->isUnsafe();
    event->parseTree = pTree;
    if (isField) {
        event->type = implementingSym->asMEMBVARSYM()->type;
        implementingSym->asMEMBVARSYM()->isEvent = true;
    }
    else {
        event->type = implementingSym->asPROPSYM()->retType;
        implementingSym->asPROPSYM()->isEvent = true;
    }

    // The event and accessors get the access modifiers of the implementing symbol; the implementing symbol
    // becomes private.
    event->SetAccess(implementingSym->GetAccess());
    implementingSym->SetAccess(ACC_PRIVATE);

    event->isOverride = !!(event->getParseFlags() & NF_MOD_OVERRIDE);

    if (event->getParseFlags() & (NF_MOD_VIRTUAL | NF_MOD_ABSTRACT | NF_MOD_OVERRIDE) && event->GetAccess() == ACC_PRIVATE) {
        compiler()->ErrorRef(NULL, ERR_VirtualPrivate, event);
    }

    return event;
}



void CLSDREC::DefinePropertyAccessors(PROPSYM *property, PropWithType pwtBase)
{
    PROPERTYNODE *propertyNode = property->parseTree->asANYPROPERTY();

    // Overriden properties use inherited names for accessors.
    NAME * accessorName;

    // Create the get accessor.
    if (pwtBase && pwtBase.Prop()->methGet) {
        accessorName = pwtBase.Prop()->methGet->name;
    } else {
        accessorName = createAccessorName(property->getRealName(), L"get_");
    }

    if (propertyNode->pGet) {
        DefinePropertyAccessor(
            accessorName, 
            propertyNode->pGet, 
            property->params,
            property->retType,
            propertyNode->flags,
            property, 
            &property->methGet);

        bool fHasAccessModifier = !!(propertyNode->pGet->flags & NF_MOD_ACCESSMODIFIERS);
        if (fHasAccessModifier) {
            if (!propertyNode->pSet && !property->isOverride) {
                // a property must have both accessors to specify an access modifier on an accessor,
                // unless it is overriding a base accessor
                compiler()->ErrorRef(NULL, ERR_AccessModMissingAccessor, property);
            } else if (propertyNode->pSet && (propertyNode->pSet->flags & NF_MOD_ACCESSMODIFIERS)) {
                // both get and set cannot have modifiiers
                compiler()->ErrorRef(NULL, ERR_DuplicatePropertyAccessMods, property);
            }
        }
    } else if (pwtBase && pwtBase.Prop()->methGet && property->isOverride && (propertyNode->flags & NF_MOD_SEALED)) {
        CreateAccessor(&property->methGet,
            accessorName,
            NULL,
            property->params,
            property->retType,
            propertyNode->flags,
            property);
        ASSERT(property->methGet->isFabricated);
    } else {
        // According to ECMA 17.2.7, we must check for an get_XXX accessor name even if the get accessor isn't defined.
        CheckForBadMember(accessorName, SK_METHSYM, property->params, propertyNode, property->getClass(), NULL);
    }

    //
    // create set accessor
    //
    if (pwtBase.Prop() && pwtBase.Prop()->methSet) {
        accessorName = pwtBase.Prop()->methSet->name;
    } else {
        accessorName = createAccessorName(property->getRealName(), L"set_");
    }

    // build the signature for the set accessor
    PTYPESYM *paramTypes = STACK_ALLOC(PTYPESYM, property->params->size + 1);

    property->params->CopyItems(0, property->params->size, paramTypes);
    paramTypes[property->params->size] = property->retType;
    TypeArray *params = compiler()->getBSymmgr().AllocParams(property->params->size + 1, paramTypes);

    if (propertyNode->pSet) {
        DefinePropertyAccessor(
            accessorName,
            propertyNode->pSet,
            params,
            compiler()->getBSymmgr().GetVoid(),
            propertyNode->flags,
            property, 
            &property->methSet);

        bool fHasAccessModifier = !!(propertyNode->pSet->flags & NF_MOD_ACCESSMODIFIERS);
        if (fHasAccessModifier && !propertyNode->pGet && !property->isOverride) {
            // a property must have both accessors to specify an access modifier on an accessor,
            // unless it is overriding a base accessor.
            compiler()->ErrorRef(NULL, ERR_AccessModMissingAccessor, property);
        }
    } else if (pwtBase.Prop() && pwtBase.Prop()->methSet && property->isOverride && (propertyNode->flags & NF_MOD_SEALED)) {
        CreateAccessor(&property->methSet,
            accessorName,
            NULL,
            params,
            compiler()->getBSymmgr().GetVoid(),
            propertyNode->flags,
            property);
        ASSERT(property->methSet->isFabricated);
    } else {
        // According to ECMA 17.2.7, we must check for an set_XXX accessor name even if the set accessor isn't defined.
        CheckForBadMember(accessorName, SK_METHSYM, params, propertyNode, property->getClass(), NULL);
    }
}


/***************************************************************************************************
    Create an accessor for either a property or an event. The symPar parameter must be either a
    PROPSYM or an EVENTSYM, and the generated methsym will inherit flags from it.
    NOTE: The methsym is an out parameter so that the accessor can be attached to the property
    or event immediately after creation (so prop->methGet/prop->methSet/evt->methAdd/evt->methRemove
    is set immediately). This is so METHSYM::getProperty works in case of an error inside
    CreateAccessor....
***************************************************************************************************/
void CLSDREC::CreateAccessor(METHSYM ** pmeth, PNAME name, BASENODE *parseTree, TypeArray *params, PTYPESYM retType, unsigned flagsOwner, SYM *symPar)
{
    ASSERT(symPar->isPROPSYM() || symPar->isEVENTSYM());
    ASSERT(symPar->parent->isAGGSYM());

    AGGDECLSYM *aggdecl = symPar->containingDeclaration()->asAGGDECLSYM();

    if (parseTree) {
        // check for duplicate member name, or name same as class
        CheckForBadMember(name, SK_METHSYM, params, parseTree, aggdecl->Agg(), NULL);
    }

    // create accessor
    METHSYM *acc = compiler()->getBSymmgr().CreateMethod(name, aggdecl->Agg(), aggdecl);

    if (!parseTree) {
        acc->parseTree = symPar->getParseTree();
        acc->isFabricated = true;
    }
    else
        acc->parseTree = parseTree;

    if (symPar->isPROPSYM()) {
        acc->SetMethKind(MethodKind::PropAccessor);
        acc->SetProperty(symPar->asPROPSYM());
    }
    else {
        acc->SetMethKind(MethodKind::EventAccessor);
        acc->SetEvent(symPar->asEVENTSYM());
    }
    acc->retType = retType;
    acc->params = params;
    acc->typeVars = BSYMMGR::EmptyTypeArray();
    acc->hasCLSattribute = symPar->hasCLSattribute;
    acc->isCLS = symPar->isCLS;
    acc->isUnsafe = symPar->isUnsafe();
    acc->SetAccess(symPar->GetAccess());
    acc->isStatic = (flagsOwner & NF_MOD_STATIC) != 0;
    acc->isAbstract = aggdecl->Agg()->IsInterface() || (flagsOwner & NF_MOD_ABSTRACT);
    acc->isExternal = (flagsOwner & NF_MOD_EXTERN) && !acc->isAbstract;
    acc->isOverride = symPar->IsOverride();

    // Attach the accessor to the property or event. NOTE: This MUST be done before any
    // errors can be reported on the accessor (so acc->getProperty() works).
    *pmeth = acc;

    if (symPar->IsDeprecated()) {
        acc->CopyDeprecatedFrom(symPar);
    }

    if (acc->isAbstract || acc->isOverride || (flagsOwner & NF_MOD_VIRTUAL)) {
        acc->isVirtual = !(flagsOwner & NF_MOD_SEALED);
        acc->isMetadataVirtual = true;
    }

    if (symPar->isPROPSYM()) {
        acc->isParamArray = symPar->asPROPSYM()->isParamArray;
        if (symPar->asPROPSYM()->isEvent)
            acc->SetAccess(ACC_PRIVATE);
    }
}

// Defines a property accessor:  First creates the METHSYM and then sets access based on any access modifiers found on the accessor 
void CLSDREC::DefinePropertyAccessor(PNAME name, BASENODE *parseTree, TypeArray *params, PTYPESYM retType, unsigned propertyFlags, PROPSYM *property, METHSYM **accessor)
{
    ASSERT(params);
    ASSERT(property->getClass()->isSource);

    *accessor = NULL;
    AGGSYM *cls = property->getClass();

    CreateAccessor(accessor, name, parseTree, params, retType, propertyFlags, property);

    METHSYM * acc = *accessor;
    ASSERT(acc);
    ASSERT(acc->getClass() == cls);

    MethAttrBind::CompileEarly(compiler(), acc);
    
    unsigned allowableFlags = property->name ? NF_MOD_ACCESSMODIFIERS : 0;
    checkFlags(acc, allowableFlags,(NODEFLAGS) parseTree->flags);

    acc->SetAccess(GetAccessFromFlags(property, allowableFlags, (NODEFLAGS) parseTree->flags));

    bool fHasAccessModifier = !!(parseTree->flags & NF_MOD_ACCESSMODIFIERS);
    ASSERT(fHasAccessModifier || acc->GetAccess() == property->GetAccess());
    if (fHasAccessModifier) {

        // We allow private accessors on virtual properties but don't emit them as virtual.
        if (acc->GetAccess() == ACC_PRIVATE) {
            acc->isVirtual = false;
            acc->isMetadataVirtual = false;
            if (acc->isAbstract)
                compiler()->ErrorRef(NULL, ERR_PrivateAbstractAccessor, acc);
        }

        if (property->getClass()->IsInterface())
            compiler()->ErrorRef(NULL, ERR_PropertyAccessModInInterface, acc);
        else if (property->name &&
             (property->GetAccess() <= acc->GetAccess() ||                                  // Accessor visibility must be less visible than the property.
              property->GetAccess() == ACC_PROTECTED && acc->GetAccess() == ACC_INTERNAL))  // There is no explicit more/less visible ordering between protected and internal, so we do not allow that combination.
        {
            compiler()->ErrorRef(NULL, ERR_InvalidPropertyAccessMod, acc, property);
        }

        CheckForProtectedInSealed(acc);
    }
}

void CLSDREC::DefineEventAccessors(EVENTSYM *event, EventWithType ewtBase)
{
    BASENODE *pTree = event->parseTree;

    // flags: static, abstract, override, virtual, extern, sealed
    bool isPropertyEvent = (pTree->kind == NK_PROPERTY);
    unsigned flags = event->getParseFlags();
    AGGSYM *cls = NULL;
    cls = event->getClass();
    
    NAME *addName = NULL;
    NAME *removeName = NULL;
    if (ewtBase && ewtBase.Event()->methAdd && ewtBase.Event()->methAdd) {
        addName = ewtBase.Event()->methAdd->name;
        removeName = ewtBase.Event()->methRemove->name;
    }
    if (!addName || !removeName) {
        addName = createAccessorName(event->name, L"add_");
        removeName = createAccessorName(event->name, L"remove_");
    }

    TypeArray *params = compiler()->getBSymmgr().AllocParams(1, &event->type);
    // Create Accessor for Add.
    CreateAccessor(&event->methAdd,
        addName,
        isPropertyEvent ? pTree->asPROPERTY()->pSet : pTree,
        params,
        compiler()->getBSymmgr().GetVoid(),
        flags,
        event);
    ASSERT(event->methAdd);

    // Create accessor for Remove.
    CreateAccessor(&event->methRemove,
        removeName,
        isPropertyEvent ? pTree->asPROPERTY()->pGet : pTree,
        params,
        compiler()->getBSymmgr().GetVoid(),
        flags,
        event);
    ASSERT(event->methRemove);

    // Need to pick-up any LinkDemands (NOTE: event non-Property events can have CAs on the accessors)
    MethAttrBind::CompileEarly(compiler(), event->methAdd);
    MethAttrBind::CompileEarly(compiler(), event->methRemove);
}



// finds a duplicate user defined conversion operator
// or a regular member hidden by a new conversion operator
SYM *CLSDREC::findDuplicateConversion(bool conversionsOnly, TypeArray *parameterTypes, PTYPESYM returnType, PNAME name, AGGSYM* cls)
{
    SYM * present = compiler()->getBSymmgr().LookupAggMember(name, cls, MASK_ALL);
    while (present) {
        if ((!conversionsOnly && !present->isMETHSYM()) || 
            ((present->isMETHSYM()) && 
             (present->asMETHSYM()->params == parameterTypes) &&
             //
             // we have a method which matches name and parameters
             // we're looking for 2 cases:
             //

             //
             // 1) any conversion operator with matching return type
             //
             (((present->asMETHSYM()->retType == returnType) &&
                present->asMETHSYM()->isConversionOperator()) ||
             //
             // 2) a non-conversion operator, and we have an exact name match
             //
               (!present->asMETHSYM()->isConversionOperator() && 
                !conversionsOnly)))) {
            return present;
        }

        present = compiler()->getBSymmgr().LookupNextSym(present, present->parent, MASK_ALL);
    }

    return NULL;
}

#define OP(n,p,a,stmt,t,pn,e) pn,
const PREDEFNAME CLSDREC::operatorNames[]  = {
    #include "ops.h"
    };

OPERATOR CLSDREC::operatorOfName(PNAME name)
{
    unsigned i;
    for (i = 0; i < OP_LAST && ((operatorNames[i] == PN_COUNT) || (name != compiler()->namemgr->GetPredefName(operatorNames[i]))); i += 1)
    { /* nothing */ }

    return (OPERATOR) i;
}

// define a user defined conversion operator, this involves checking that the
// flags for the decls have be specified correctly, and actually entering the
// conversion into the symbol table...
//
// returns whether an operator requiring a matching operator has been seen
bool CLSDREC::defineOperator(OPERATORMETHODNODE * operatorNode, AGGSYM * cls, AGGDECLSYM * clsdecl)
{
    ASSERT(!cls->IsEnum());

    if (cls->IsInterface()) {
        compiler()->Error(operatorNode, ERR_InterfacesCantContainOperators);
        return false;
    }

    bool isConversion = false;
    bool isImplicit = false;
    PNAME name;
    PNAME otherName = NULL;

    switch (operatorNode->iOp) {
    case OP_IMPLICIT:
        isImplicit = true;
        isConversion = true;
        name = compiler()->namemgr->GetPredefName(PN_OPIMPLICITMN);
        otherName = compiler()->namemgr->GetPredefName(PN_OPEXPLICITMN);
        break;

    case OP_EXPLICIT:
        isConversion = true;
        otherName = compiler()->namemgr->GetPredefName(PN_OPIMPLICITMN);
        name = compiler()->namemgr->GetPredefName(PN_OPEXPLICITMN);
        break;

    case OP_NONE:
        // Just use the token string as the name and make it a regular method, not an operator.
        ASSERT(compiler()->ErrorCount());
        name = compiler()->namemgr->AddString(CParser::GetTokenInfo(operatorNode->tok)->pszText);
        break;

    default:
        PREDEFNAME predefName = operatorNames[operatorNode->iOp];
        ASSERT(predefName > 0 && predefName < PN_COUNT);
        name = compiler()->namemgr->GetPredefName(predefName);
        break;
    }

    AGGTYPESYM * atsThis = cls->getThisType();
    bool fUnsafe = (operatorNode->flags & NF_MOD_UNSAFE) || clsdecl->isUnsafe;

    TypeArray * params;
    DefineParameters(clsdecl, operatorNode->pParms, fUnsafe, &params, NULL);

    // Get return type.
    TYPESYM * typeRet = TypeBind::BindType(compiler(), operatorNode->pType, clsdecl);
    ASSERT(typeRet);
    checkUnsafe(operatorNode, typeRet, fUnsafe);
    TYPESYM * typeRetRaw = StripNubs(typeRet, cls);

    bool mustMatch = false;

    int ctype; ctype = params->size;
    ASSERT(ctype == 1 || ctype == 2 || operatorNode->iOp == OP_NONE);

    TypeArray * paramsRaw = params;
    if (ctype > 0 && params->Item(0)->isNUBSYM() || ctype > 1 && params->Item(1)->isNUBSYM()) {
        TYPESYM * rgtype[2];
        rgtype[0] = StripNubs(params->Item(0), cls);
        if (ctype > 1)
            rgtype[1] = StripNubs(params->Item(1), cls);
        paramsRaw = compiler()->getBSymmgr().AllocParams(ctype, rgtype);
    }

    //
    // check argument restrictions. Note that the parser has
    // already checked the number of arguments is correct.
    //
    switch (operatorNode->iOp) {

    // conversions
    case OP_IMPLICIT:
    case OP_EXPLICIT:
        ASSERT(ctype == 1);

        // Check for the identity conversion here.
        if (typeRetRaw == atsThis && paramsRaw->Item(0) == atsThis) {
            compiler()->Error(operatorNode, ERR_IdentityConversion);
        }
        // Either the source or the destination must be the containing type.
        else if (typeRetRaw != atsThis && paramsRaw->Item(0) != atsThis) {
            compiler()->Error(operatorNode, ERR_ConversionNotInvolvingContainedType);
        }
        isConversion = true;
        break;

    // unary operators
    case OP_PREINC:
    case OP_PREDEC:
        ASSERT(ctype == 1);

        // The source must be the containing type or nullable of that type. It's also bad if the source
        // is nullable and the destination isn't.
        if (paramsRaw->Item(0) != atsThis || params->Item(0)->isNUBSYM() && typeRet == atsThis)
            compiler()->Error(operatorNode, ERR_BadIncDecSignature);

        // The destination must be the containing type, nullable of that type or a derived type.
        // It's also bad if the destination is nullable and the source isn't.
        if (typeRetRaw != atsThis && !compiler()->IsBaseType(typeRetRaw, atsThis) || typeRet->isNUBSYM() && params->Item(0) == atsThis)
            compiler()->Error(operatorNode, ERR_BadIncDecRetType);
        break;

    case OP_TRUE:
    case OP_FALSE:
        ASSERT(ctype == 1);

        mustMatch = true;
        if (!typeRet->isPredefType(PT_BOOL)) {
            compiler()->Error(operatorNode, ERR_OpTFRetType);
        }
        // Fall through
    case OP_UPLUS:
    case OP_NEG:
    case OP_BITNOT:
    case OP_LOGNOT:
        ASSERT(ctype == 1);

        // The source must be the containing type
        if (paramsRaw->Item(0) != atsThis) {
            compiler()->Error(operatorNode, ERR_BadUnaryOperatorSignature);
        }
        break;

    // binary operators
    case OP_EQ:
    case OP_NEQ:
    case OP_GT:
    case OP_LT:
    case OP_GE:
    case OP_LE:
        mustMatch = true;
        // Fall through.
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_DIV:
    case OP_MOD:
    case OP_BITXOR:
    case OP_BITAND:
    case OP_BITOR:
        ASSERT(ctype == 2);

        // At least one of the parameter types must be the containing type.
        if (paramsRaw->Item(0) != atsThis && paramsRaw->Item(1) != atsThis) {
            compiler()->Error(operatorNode, ERR_BadBinaryOperatorSignature);
        }
        else if (operatorNode->iOp == OP_EQ && paramsRaw->Item(0) == paramsRaw->Item(1))
            cls->fHasSelfEq = true;
        else if (operatorNode->iOp == OP_NEQ && paramsRaw->Item(0) == paramsRaw->Item(1))
            cls->fHasSelfNe = true;

        break;

    // shift operators
    case OP_LSHIFT:
    case OP_RSHIFT:
        ASSERT(ctype == 2);

        if (paramsRaw->Item(0) != atsThis || !paramsRaw->Item(1)->isPredefType(PT_INT)) {
            compiler()->Error(operatorNode, ERR_BadShiftOperatorSignature);
        }
        break;

    case OP_NONE:
        break;

    default:
        VSFAIL("Unknown operator parse tree");
        break;
    }

    // Return type of user defined operators can't be VOID
    if (typeRetRaw == compiler()->getBSymmgr().GetVoid()) {
        compiler()->Error(operatorNode, ERR_OperatorCantReturnVoid);
    }

    // Check for duplicate and get name.
    if (isConversion) {
        //
        // For operators we check for conflicts with implicit vs. explicit as well
        // create both implicit and explicit names here because they can conflict
        // with each other.
        //
        //
        // check for name same as that of parent aggregate
        //
        if (name == cls->name) {
            compiler()->Error(operatorNode, ERR_MemberNameSameAsType, name, ErrArgRefOnly(cls));
        }

        if (!params->HasErrors() && !typeRet->HasErrors()) {
            // Check for duplicate conversion and conflicting names.
            SYM * symDup;
            if ((symDup = findDuplicateConversion(false, params, typeRet, name, cls)) ||
                (symDup = findDuplicateConversion(true, params, typeRet, otherName, cls)))
            {
                if ((symDup->isMETHSYM()) && symDup->asMETHSYM()->isConversionOperator()) {
                    compiler()->Error(operatorNode, ERR_DuplicateConversionInClass, cls, ErrArgRefOnly(symDup));
                } else {
                    compiler()->Error(operatorNode, ERR_DuplicateNameInClass, name, cls, ErrArgRefOnly(symDup));
                }
                return false;
            }
        }
    }
    else if (!CheckForBadMember(name, SK_METHSYM, params, operatorNode, cls, BSYMMGR::EmptyTypeArray())) {
        return false;
    }

    //
    // create the operator
    //
    METHSYM * meth = compiler()->getBSymmgr().CreateMethod(name, cls, clsdecl);
    meth->parseTree = operatorNode;
    // If iOp is OP_NONE - indicating an error - just make it a regular method
    meth->isOperator = (operatorNode->iOp != OP_NONE);
    ASSERT(params && params->size == ctype);
    meth->params = params;
    meth->typeVars = BSYMMGR::EmptyTypeArray();
    meth->isUnsafe = fUnsafe;
    meth->retType = typeRet;
    meth->isStatic = true;
    meth->isExternal = !!(operatorNode->flags & NF_MOD_EXTERN);

    if (isConversion) {
        if (isImplicit)
            meth->SetMethKind(MethodKind::ImplicitConv);
        else
            meth->SetMethKind(MethodKind::ExplicitConv);

        // Add it to the list of conversions operators.
        meth->SetConvNext(cls->convFirst);
        cls->convFirst = meth;

        // The flag is set if this class or any of its base classes has any conversions.
        cls->hasConversion = true;
    }

    // check flags. Conversions must be public.
    checkFlags(meth, NF_MOD_UNSAFE | NF_MOD_PUBLIC | NF_MOD_STATIC | NF_MOD_EXTERN, operatorNode->flags);

    // operators must be explicitly declared public and static
    if ((operatorNode->flags & (NF_MOD_PUBLIC | NF_MOD_STATIC)) != (NF_MOD_PUBLIC | NF_MOD_STATIC)) {
        compiler()->ErrorRef(NULL, ERR_OperatorsMustBeStatic, meth);
    }

    // Set access -- Conversions must be public.
    meth->SetAccess(ACC_PUBLIC);

    // operators are not allowed on a static class
    if (cls->IsStatic()) {
        compiler()->ErrorRef(NULL, ERR_OperatorInStaticClass, meth);
    }

    // Check constituent types
    checkConstituentVisibility(meth, typeRet, ERR_BadVisOpReturn);

    for (int i = 0; i < params->size; ++i) 
        checkConstituentVisibility(meth, params->Item(i), ERR_BadVisOpParam);

    return mustMatch;
}




/***************************************************************************************************
    Check the layout of the struct type for cycles. Return false iff there are cycles in the layout
    (indicating an error condition).

    Here is the rule that we "would like to" implement:

        Let G be the smallest directed graph such that:

        (1) ats is in G.
        (2) Whenever S is in G, the instance type of S is in G and there is a directed edge from S
            to the instance type of S.
        (3) Whenever S is in G and S has a struct valued field of type T then T is in G.
        (4) Whenever S is in G and S has a struct valued _instance_ field of type T then there is
            a (directed) edge from S to T.

        It is an error for G to be infinite or for G to contain a (directed) cycle.

    Unfortunately, the runtime disallows more:

        Let G* be the smallest directed graph such that:

        (1) ats is in G*.
        (2) Whenever S is in G*, the instance type of S is in G* and there is a directed edge from S
            to the instance type of S.
        (3) Whenever S is in G* and S has a struct valued field of type T then T is in G* and
            there is a (directed) edge from S to T.

        It is an error for G* to be infinite or for G* to contain a (directed) cycle.

    Note that the vertices of G* are the same as the vertices of G. Thus G* is infinite iff
    G is infinite. Note also that the edges of G are all edges of G*. So G* produces an
    error whenever G produces an error.

    It is actually much easier to implement G* than to implement G. This is because of the
    following facts (proof left to the reader):

        (1) There is a path from ats to any vertex of G*.
        (2) If G* is infinite then G* contains a cycle (follows from 1).
        (3) If G* contains a cycle then it contains a cycle starting (and ending) at an instance type.

    By walking the instance type edge from S before walking any other edges from S, we are
    guaranteed to detect infinite graphs in finite time (since the set of instance types is
    finite).

    We walk G* as follows:

        (1) If S is not an instance type, visit the instance type of S first.
        (2) If S is an instance type, check whether we're already processing this type
            in a calling stack frame. If so, we have a cycle in G*.
        (3) For each struct valued field of S, visit the type of the field.

    To implement G instead of G* we'd have to do the following:

        Once you find a cycle in G* determine whether it is an error:

            * If there are no static fields in the cycle then it is also a cycle in G (an error).
            * If there are static fields in the cycle but no "instance type" edges then we have
              a cycle in G* but not an infinite graph yet. So keep looking.
            * If there are static fields and instance type edges in the cycle, then G is infinite
              (an error).

    The problem is when we keep looking, we don't know when to stop and where to look.
    It's easy to end up in infinite recursion.... So we're taking the simpler approach (since
    that's all the runtime supports anyway).

    Here are some examples of the strange possibilities one can get with generics:

        // (1) Infinite recursion detectable just by checking AGGSYMs.
        A<T> { A<A<T>> }

        // (2) Infinite recursion detectable just by checking AGGSYMs.
        A<T> { B<A<T>> }
        B<T> { A<B<T>> }

        // (3) Requires tracking AGGTYPESYM and doing substitutions.
        A<T> { B<A<T>> }
        B<T> { T }

        // (4) Valid code where checking for duplicate AGGSYMs gives a false error.
        A<T> { B<B<T>> }
        B<T> { T }

        // (5) Infinite recursion not detectable by just checking AGGSYMs.
        A<T> { B<A<A<T>>> }
        B<T> { T }

    Here's an alternate statement of the algorithm:

        * If ats is an instance type, check whether we're already processing ats in a calling
          stack frame. If so, we have a cycle in G*. Note that we check for the AGGTYPESYM,
          not just AGGSYM to avoid a false error for example (4). Note that we could perform this
          check even if the ats is not an instance type, but doing so would never find any errors.
          If the type is already being checked then when we checked the instance type we would
          have seen a cycle as well.

        * For each field, get the actual field type (after substitution). If it's not a struct type
          ignore the field. If it is a struct type:

            * Check the associated instance type first. This avoids infinite recusion in examples
              (1) and (2).

            * Check the actual type. This finds cycles caused by substitution as in examples (3).
              This together with checking the instance type first catches example (5). For that
              example we first check A<T>, then recurse to B<T> which returns true, then recurse
              to B<A<A<T>>>, then recurse to A<T> (the instance type of A<A<T>>) and detect the
              error.
***************************************************************************************************/
bool CLSDREC::CheckStructLayout(AGGTYPESYM * ats, LayoutFrame * pframeOwner)
{
    ASSERT(ats && ats->isStructType());

    compiler()->EnsureState(ats);
    if (ats->isPredefined() && !ats->typeArgsAll->size)
        return true;

    AGGSYM * agg = ats->getAggregate();

    // This should always be called on the instance type before any other instantiation.
    ASSERT(agg->fLayoutChecked || ats == agg->getThisType());

    // If we haven't checked the layout yet, fLayoutError should be clear.
    ASSERT(!agg->fLayoutError || agg->fLayoutChecked);

    if (ats == agg->getThisType()) {
        if (agg->fLayoutChecked)
            return !agg->fLayoutError;

        // See if we're already checking this type.
        LayoutFrame * pframePrev = NULL;
        for (LayoutFrame * pframe = pframeOwner; pframe; pframe = pframe->pframeOwner) {
            pframe->pframeChild = pframePrev;
            pframePrev = pframe;
            if (ats == pframe->swt.ats) {
                // Found a cycle. Spit out all the fields in the chain.
                for ( ; pframe; pframe = pframe->pframeChild)
                    compiler()->Error(NULL, ERR_StructLayoutCycle, ErrArgRef(pframe->swt), pframe->atsField);
                // We'll set the fLayoutChecked and fLayoutError bits in the calling frame.
                return false;
            }
        }
    }

    bool fError = true; // Assume an error. Cleared below.

    LayoutFrame frame;
    frame.pframeOwner = pframeOwner;
    frame.swt.ats = ats;

    // Resolve the layout for all of our non-static fields.
    for (frame.swt.sym = agg->firstChild; frame.swt.sym; frame.swt.sym = frame.swt.sym->nextChild) {
        // Ignore non-fields and statics.
        if (!frame.swt.sym->isMEMBVARSYM())
            continue;

        // Get the true type and ignore non-structs.
        TYPESYM * typeField = compiler()->getBSymmgr().SubstType(frame.swt.sym->asMEMBVARSYM()->type, ats);
        if (typeField->isNUBSYM()) {
            typeField = typeField->asNUBSYM()->GetAts();
            if (!typeField)
                continue;
        }
        if (!typeField->isStructType())
            continue;

        // Static fields of the exact same type are apparently OK.
        if (frame.swt.sym->asMEMBVARSYM()->isStatic && typeField == ats)
            continue;

        frame.atsField = typeField->asAGGTYPESYM();
        typeField = frame.atsField->getAggregate()->getThisType();

        if (!CheckStructLayout(typeField->asAGGTYPESYM(), &frame))
            goto LDone;
        if (frame.atsField != typeField && !CheckStructLayout(frame.atsField, &frame))
            goto LDone;
    }

    fError = false;

LDone:
    if (ats == agg->getThisType()) {
        ASSERT(!agg->fLayoutChecked && !agg->fLayoutError);
        agg->fLayoutChecked = true;
        agg->fLayoutError = fError;
    }
    return !fError;
}


void CLSDREC::prepareInterfaceMember(METHPROPSYM * member)
{
    //
    // check that new members don't hide inherited members
    // we've built up a list of all inherited interfaces
    // search all of them for a member we will hide
    //

    // for all inherited interfaces
    checkIfaceHiding(member, member->getParseTree()->flags);

    //
    // shouldn't have a body on interface members
    //
    if (member->isMETHSYM()) {
        if ((member->asMETHSYM()->getParseTree()->other & NFEX_METHOD_NOBODY) == 0) {
            compiler()->ErrorRef(NULL, ERR_InterfaceMemberHasBody, member);
        }
    } else {
        ASSERT(member->isPROPSYM());
        PROPSYM *property = member->asPROPSYM();

        if (property->isIndexer()) {
            // bind IndexerName attribute in order to resolve the actual name of the indexer.
            IndexerNameAttrBind::Compile(compiler(), property->asINDEXERSYM());
            CheckForBadMemberSimple(property->getRealName(), property->getParseTree(), property->getClass());
        }

        PropWithType pwtBase(NULL, NULL);
        DefinePropertyAccessors(property, pwtBase);

        //
        // didn't find a conflict between this property and anything else
        // check that the accessors don't have a conflict with other methods
        //
        if (property->methGet) {
            prepareInterfaceMember(property->methGet);
        }
        if (property->methSet) {
            prepareInterfaceMember(property->methSet);
        }
    }
}





//
// does the prepare stage for an interface.
// This checks for conflicts between members in inherited interfaces
// and checks for conflicts between members in this interface
// and inherited members.
//
void CLSDREC::prepareInterface(AGGSYM *cls)
{
    ASSERT(cls->IsPreparing());

    //
    // this must be done before preparing our members
    // because they may derive from us, which requires that we
    // are prepared.
    //
    cls->setBogus(false);
    cls->SetAggState(AggState::Prepared);

    if (cls->isSource) {
        //
        // prepare members
        //
        FOREACHCHILD(cls, child)

            SETLOCATIONSYM(child);

            switch (child->getKind()) {
            case SK_METHSYM:
                if (child->asMETHSYM()->isAnyAccessor()) {
                    //
                    // accessors are handled by their property/event
                    //
                    break;
                }
                // fallthrough

            case SK_PROPSYM:
                prepareInterfaceMember(child->asMETHPROPSYM());
                break;

            case SK_TYVARSYM: 
            case SK_AGGTYPESYM:
                break;  // type based on this type.

            case SK_EVENTSYM:
                DefineEventAccessors(child->asEVENTSYM(), EventWithType(NULL, NULL));
                checkIfaceHiding(child, child->asEVENTSYM()->parseTree->pParent->flags);
                // For imported types we don't really know if it's a delegate until we've imported members. 
                compiler()->EnsureState(child->asEVENTSYM()->type, AggState::DefinedMembers);
                // Issue error if the event type is not a delegate.
                if (!child->asEVENTSYM()->type->isDelegateType()) {
                    compiler()->ErrorRef(NULL, ERR_EventNotDelegate, child->asEVENTSYM());
                }
                break;

            default:
                ASSERT(!"Unknown node type");
            }
        ENDFOREACHCHILD
    }

    cls->SetAggState(AggState::PreparedMembers);
}



//
// reports the results of a symbol hiding another symbol
//
void CLSDREC::ReportHiding(SYM *sym, SymWithType * pswtHid, unsigned flags)
{
    // Pretend like Destructors are not called "Finalize"
    if (pswtHid && (!pswtHid->Sym() || pswtHid->Sym()->isMETHSYM() && pswtHid->Meth()->isDtor())) {
        pswtHid = NULL;
    }

    //
    // check the shadowing flags are correct
    //
    if (!pswtHid) {
        if (sym->isAGGSYM()) {
            // Warn on any that have new set.
            for (AGGDECLSYM * decl = sym->asAGGSYM()->DeclFirst(); decl; decl = decl->DeclNext()) {
                if (decl->parseTree->flags & NF_MOD_NEW) {
                    compiler()->ErrorRef(NULL, WRN_NewNotRequired, decl);
                }
            }
        }
        else if (flags & NF_MOD_NEW) {
            compiler()->ErrorRef(NULL, WRN_NewNotRequired, sym);
        }
        return;
    }

    if (sym->isAGGSYM()) {
        // Warn if none have new set.
        for (AGGDECLSYM * decl = sym->asAGGSYM()->DeclFirst(); ; decl = decl->DeclNext()) {
            if (!decl) {
                compiler()->ErrorRef(NULL, WRN_NewRequired, sym, *pswtHid);
                break;
            }
            if (decl->parseTree->flags & NF_MOD_NEW)
                break;
        }
    }
    else if (!(flags & NF_MOD_NEW)) {
        compiler()->ErrorRef(NULL,
            (pswtHid->Sym()->IsVirtual() && sym->getKind() == pswtHid->Sym()->getKind() && !sym->parent->asAGGSYM()->IsInterface()) ?
                WRN_NewOrOverrideExpected : WRN_NewRequired,
            sym, *pswtHid);
    }
    if (!sym->parent->asAGGSYM()->IsInterface()) {
        CheckHiddenSymbol(sym, *pswtHid);
    }
}

//
// checks the simple hiding issues for non-override members
//
void CLSDREC::checkSimpleHiding(SYM *sym, unsigned flags)
{
    // find an accessible member with the same name in
    // a base class of our enclosing class
    AGGSYM *agg = sym->parent->asAGGSYM();
    SymWithType swtHid;

    if (sym->isMETHPROPSYM() || sym->isAGGSYM()) {
        bool fNeedsMethImpl;
        FindSymHiddenByMethPropAgg(sym, agg->baseClass, agg, &swtHid, NULL, &fNeedsMethImpl);
    }
    else {
        FindAnyHiddenSymbol(sym->name, agg->baseClass, agg, &swtHid);
    }

    // report the results
    ReportHiding(sym, &swtHid, flags);
}


void CLSDREC::checkIfaceHiding(SYM *sym, unsigned flags)
{
    if (!sym->isUserCallable())
        return;

    // Ctors don't hide and shouldn't be in interfaces anyway.
    ASSERT(!sym->isMETHSYM() || !sym->asMETHSYM()->isCtor() && !sym->asMETHSYM()->isOperator);

    AGGSYM *cls = sym->parent->asAGGSYM();
    TypeArray * ifacesAll = cls->ifacesAll;

    if (ifacesAll->size == 0) {
        // Check for gratuitous new.
        ReportHiding(sym, NULL, flags);
        return;
    }

    SYMKIND sk = sym->getKind();
    bool fIndexer = (sk == SK_PROPSYM) && sym->asPROPSYM()->isIndexer();
    TypeArray * params = (fIndexer || sk == SK_METHSYM) ? sym->asMETHPROPSYM()->params : NULL;

    // Initialize the hide bits.
    for (int i = 0; i < ifacesAll->size; i++) {
        AGGTYPESYM * iface = ifacesAll->Item(i)->asAGGTYPESYM();
        ASSERT(iface->isInterfaceType());
        iface->fAllHidden = false;
        iface->fDiffHidden = false;
    }

    // Loop through the interfaces.
    for (int itype = 0; itype < ifacesAll->size; itype++) {
        AGGTYPESYM * typeCur = ifacesAll->Item(itype)->asAGGTYPESYM();
        ASSERT(typeCur && typeCur->isInterfaceType());

        if (typeCur->fAllHidden)
            continue;

        bool fHideAll = false;
        bool fHideDiff = false;

        // Loop through the symbols.
        for (SYM * symCur = compiler()->getBSymmgr().LookupAggMember(sym->name, typeCur->getAggregate(), MASK_ALL);
            symCur != NULL;
            symCur = compiler()->getBSymmgr().LookupNextSym(symCur, typeCur->getAggregate(), MASK_ALL))
        {
            if (sk != symCur->getKind()) {
                // Some things (ctors, operators and indexers) don't interact based on name!
                ASSERT(!symCur->isMETHSYM() || !symCur->asMETHSYM()->isCtor() && !symCur->asMETHSYM()->isOperator);
                if (fIndexer || symCur->isPROPSYM() && symCur->asPROPSYM()->isIndexer())
                    continue;
                if (typeCur->fDiffHidden) {
                    // This one hides everything in base types.
                    fHideAll = true;
                    continue;
                }
            }
            else {
                switch (sk) {
                case SK_AGGSYM:
                    if (symCur->asAGGSYM()->typeVarsThis->size != sym->asAGGSYM()->typeVarsThis->size) {
                        fHideDiff = true;
                        continue;
                    }
                    break;
                case SK_METHSYM:
                    ASSERT(!symCur->asMETHSYM()->isCtor() && !symCur->asMETHSYM()->isOperator);
                    if (symCur->asMETHSYM()->typeVars->size != sym->asMETHSYM()->typeVars->size || 
                        !compiler()->getBSymmgr().SubstEqualTypeArrays(params, symCur->asMETHSYM()->params, typeCur->typeArgsAll, sym->asMETHSYM()->typeVars))
                    {
                        fHideDiff = true;
                        continue;
                    }
                    break;
                case SK_PROPSYM:
                    if (fIndexer && (!symCur->asPROPSYM()->isIndexer() ||
                        !compiler()->getBSymmgr().SubstEqualTypeArrays(params, symCur->asPROPSYM()->params, typeCur->typeArgsAll)))
                    {
                        continue;
                    }
                    break;

                default:
                    break;
                }
            }

            SymWithType swtHid(symCur, typeCur);
            ReportHiding(sym, &swtHid, flags);
            return;
        }

        // Done with the current type. Mark base interfaces appropriately.
        if (fHideAll || fHideDiff) {
            // Mark base interfaces appropriately.
            TypeArray * ifacesT = typeCur->GetIfacesAll();
            for (int i = 0; i < ifacesT->size; i++) {
                AGGTYPESYM * typeT = ifacesT->Item(i)->asAGGTYPESYM();
                ASSERT(typeT->isInterfaceType());
                if (fHideAll)
                    typeT->fAllHidden = true;
                typeT->fDiffHidden = true;
            }
        }
    }

    // Check for gratuitous new.
    ReportHiding(sym, NULL, flags);
}

// Reduces a signature to "CLS reduced form", which
//    1. Removes ref or out from all parameters in a signature.
//    2. Changes arrays of rank > 1 to rank == 1
//    3. Changes array of arrays to arrays of System.Void.
// Two methods with the same name cannot have the same CLS reduced form.
TypeArray * CLSDREC::CLSReduceSignature(TypeArray * types)
{
    int cTypes = types->size;
    PTYPESYM * newTypes = STACK_ALLOC(PTYPESYM, cTypes);

    for (int i = 0; i < cTypes; ++i) {
        PTYPESYM type = types->Item(i);
        if (type->isPARAMMODSYM()) 
            newTypes[i] = type->asPARAMMODSYM()->paramType();
        else if (type->isARRAYSYM()) {
            ARRAYSYM * arrayType = type->asARRAYSYM();
            if (arrayType->elementType()->isARRAYSYM())
                arrayType = compiler()->getBSymmgr().GetArray(compiler()->getBSymmgr().GetVoid(), 1);
            else if (arrayType->rank > 1) 
                arrayType = compiler()->getBSymmgr().GetArray(arrayType->elementType(), 1);
            newTypes[i] = arrayType;
        }
        else 
            newTypes[i] = type;
    }

    return compiler()->getBSymmgr().AllocParams(cTypes, newTypes);
}




// checks for case-insensitive collisions in
// public members of any AGGSYM. Also checks that we don't overload solely by
// ref or out.
void CLSDREC::checkCLSnaming(AGGSYM *cls)
{
    ASSERT(compiler()->AllowCLSErrors());
    ASSERT(cls && cls->hasExternalAccess());
    WCHAR buffer[2 * MAX_IDENT_SIZE];
    WCBuffer wcbuffer(buffer);
    UNITSYM * CLSroot;
    PCACHESYM temp;

    // Create a local symbol table root for this stuff - UNITSYM has no meaning here.
    // It's just a convenient parent sym type.
    CLSroot = compiler()->getLSymmgr().CreateLocalSym(SK_UNITSYM,
        compiler()->namemgr->AddString(L"$clsnamecheck$"), NULL)->asUNITSYM();

    // notice that for all these 'fake' members we're adding the sym->sym 
    // is not the actual type of the 'fake' member, but a pointer back to
    // the orignal member
    
    // add all externally visible members of interfaces
    for (int i = 0; i < cls->ifacesAll->size; i++) {
        AGGTYPESYM * base = cls->ifacesAll->Item(i)->asAGGTYPESYM();
        PAGGSYM baseAgg = base->getAggregate();
        ASSERT(baseAgg);

        FOREACHCHILD(baseAgg, member)
            if (member->hasExternalAccess() && (!member->isMETHPROPSYM() || !member->asMETHPROPSYM()->isOverride)) {
                // Add a lower-case symbol to our list (we don't care about collisions here)
                SafeToLowerCase( member->name->text, wcbuffer);
                PCACHESYM temp = compiler()->getLSymmgr().CreateLocalSym( SK_CACHESYM,
                    compiler()->namemgr->AddString( buffer), CLSroot)->asCACHESYM();
                temp->sym = member;
            }
        ENDFOREACHCHILD
    }

    // add all the externally visible members of base classes
    BASE_CLASS_TYPES_LOOP(cls->baseClass, base)
        ASSERT(base);
        if (!compiler()->isCLS_Type(cls->GetOuterAgg(), base))
            compiler()->ErrorRef(NULL, WRN_CLS_BadBase, cls, base);

        FOREACHCHILD(base->getAggregate(), member)
            if (member->hasExternalAccess() && (!member->isMETHPROPSYM() || !member->asMETHPROPSYM()->isOverride)) {
                // Add a lower-case symbol to our list (we don't care about collisions here)
                SafeToLowerCase( member->name->text, wcbuffer);
                PCACHESYM temp = compiler()->getLSymmgr().CreateLocalSym( SK_CACHESYM,
                    compiler()->namemgr->AddString( buffer), CLSroot)->asCACHESYM();
                temp->sym = member;
            }
        ENDFOREACHCHILD
    END_BASE_CLASS_TYPES_LOOP

    // Also check the underlying type of Enums
    if (cls->IsEnum() && !compiler()->isCLS_Type(cls->GetOuterAgg(), cls->underlyingType)) {
        compiler()->ErrorRef(NULL, WRN_CLS_BadBase, cls, cls->underlyingType);
    }

    // We don't check typeVarsAll becuase the outer-type will check those
    if (cls->typeVarsThis->size > 0) {
        // check bounds...
        for (int i = 0; i < cls->typeVarsThis->size; i++) {
            TYVARSYM * var = cls->typeVarsThis->ItemAsTYVARSYM(i);
            TypeArray * bnds = var->GetBnds();
            for (int j = 0; j < bnds->size; j++) {
               TYPESYM * typeBnd = bnds->Item(j);
               if (!compiler()->isCLS_Type(cls->GetOuterAgg(), typeBnd)) {
                 compiler()->ErrorRef(NULL, WRN_CLS_BadTypeVar, typeBnd);
               }
            }
        }
    }     

    FOREACHCHILD(cls, member)
        if (member->isARRAYSYM() ||
            member->isTYVARSYM() ||
            member->isAGGTYPESYM() ||
            member->isPTRSYM() ||
            member->isPARAMMODSYM() ||
            member->isPINNEDSYM() ||
            member->isFAKEMETHSYM())
            continue;

        if (!compiler()->CheckSymForCLS(member, false)) {
            if (cls->IsInterface()) {
                // CLS Compliant Interfaces can't have non-compliant members
                compiler()->ErrorRef(NULL, WRN_CLS_BadInterfaceMember, member);
            } else if (member->isMETHSYM() && member->asMETHSYM()->isAbstract) {
                // CLS Compliant types can't have non-compliant abstract members
                compiler()->ErrorRef(NULL, WRN_CLS_NoAbstractMembers, member);
            }
        } else if (member->hasExternalAccess() && (!member->isMETHPROPSYM() || !member->asMETHPROPSYM()->isOverride)) {

            SafeToLowerCase( member->name->text, wcbuffer);
            if (buffer[0] == (WCHAR)0x005F || buffer[0] == (WCHAR)0xFF3F)  // According to CLS Spec these are '_'
                compiler()->ErrorRef(NULL, WRN_CLS_BadIdentifier, member);

            PNAME nameLower = compiler()->namemgr->AddString(buffer);

            // Check for colliding names
            temp = compiler()->getLSymmgr().LookupLocalSym(nameLower, CLSroot, MASK_CACHESYM)->asCACHESYM();
            if (temp) {
                // If names are different, then they must differ only in case.
                if (member->name != temp->sym->name) 
                    compiler()->ErrorRef(NULL, WRN_CLS_BadIdentifierCase, member, temp->sym);

                // Check for colliding signatures (but don't check the accessors)
                if (member->isMETHPROPSYM() && (!member->isMETHSYM() || !member->asMETHSYM()->isAnyAccessor())) {
                    TYPESYM * unnamedArray = compiler()->getBSymmgr().GetArray(compiler()->getBSymmgr().GetVoid(), 1);
                    TypeArray * reducedSig = CLSReduceSignature(member->asMETHPROPSYM()->params);
                    bool hasUnnamedArray = reducedSig->Contains( unnamedArray);
                    while (temp) {
                        SYM * sym = temp->sym;
                        if (sym->isMETHPROPSYM() && member->asMETHPROPSYM()->params != sym->asMETHPROPSYM()->params &&
                            member->asMETHPROPSYM()->params->size == sym->asMETHPROPSYM()->params->size)
                        {
                            TypeArray * otherReducedSig = CLSReduceSignature(sym->asMETHPROPSYM()->params);
                            if (reducedSig == otherReducedSig && !hasUnnamedArray)
                            {
                                compiler()->ErrorRef(NULL, WRN_CLS_OverloadRefOut, member, sym);
                            }
                            else if (hasUnnamedArray || otherReducedSig->Contains(unnamedArray))
                            {
                                // If the reduced signatures are the same (and the non-reduced ones are different)
                                // and one of the reduced signatures contains an unnamed array, then they both do
                                // so the overload is based on the unnamed array (and possibly something else)
                                if (reducedSig != otherReducedSig) 
                                {
                                    TYPESYM ** m1 = reducedSig->ItemPtr(0);
                                    TYPESYM ** m2 = otherReducedSig->ItemPtr(0);
                                    for (int i = 0; i < reducedSig->size; i++, m1++, m2++) {
                                        if (*m1 != *m2 && (!(*m1)->isARRAYSYM() || !(*m2)->isARRAYSYM() ||
                                            (*m1 != unnamedArray && *m2 != unnamedArray))) {
                                            // The 2 types lists are different and the difference
                                            // is not based on an unnamed array
                                            goto CONTINUE;
                                        }
                                    }
                                }
                                compiler()->ErrorRef(NULL, WRN_CLS_OverloadUnnamed, member, sym);
                            }
                        }

CONTINUE:                    
                        temp = compiler()->getBSymmgr().LookupNextSym(temp, CLSroot, MASK_CACHESYM)->asCACHESYM();
                    }
                }
            } 

            // Add to the table.
            temp = compiler()->getLSymmgr().CreateLocalSym( SK_CACHESYM, nameLower, CLSroot)->asCACHESYM();
            temp->sym = member;
        }
    ENDFOREACHCHILD

    //Cleanup
    compiler()->DiscardLocalState();
}

// checks for case-insensitive collisions in
// public members of any NSSYM
void CLSDREC::checkCLSnaming(NSSYM *ns)
{
    ASSERT(compiler()->AllowCLSErrors());
    ASSERT(ns && ns->hasExternalAccess());
    WCHAR buffer[2 * MAX_IDENT_SIZE];
    WCBuffer wcbuffer(buffer);
    UNITSYM * CLSroot;
    PCACHESYM temp;

    // Create a local symbol table root for this stuff - UNITSYM has no meaning here.
    // It's just a convenient parent sym type.
    CLSroot = compiler()->getLSymmgr().CreateLocalSym(SK_UNITSYM,
        compiler()->namemgr->AddString( L"$clsnamecheck$"), NULL)->asUNITSYM();

    // notice that for all these 'fake' members we're adding the sym->sym 
    // is not the actual type of the 'fake' member, but a pointer back to
    // the orignal member

    for (NSDECLSYM * decl = ns->DeclFirst(); decl; decl = decl->DeclNext()) {
        FOREACHCHILD(decl, member)
            if (member->mask() & (MASK_GLOBALATTRSYM | MASK_ALIASSYM))
                continue;

            // If an AGGSYM isn't declared, declare it, unless it's a mere metadata reference
            // that can't be declared, in which case it should be skipped.
            // passing 'true' into CheckSymForCLS (unlike the default false) will do this for
            // for us by Declaring everything that is can be, and returning false if it bumps
            // into something that can't be declared.
            if (!member->hasExternalAccess() || !compiler()->CheckSymForCLS(member, true))
                continue;

            if (member->isAGGDECLSYM()) {
                if (!member->asAGGDECLSYM()->IsFirst())
                    continue;

                AGGSYM * agg = member->asAGGDECLSYM()->Agg();

                SafeToLowerCase( agg->name->text, wcbuffer);

                //Only warn on problems with source that is being compiled.
                if (agg->isSource && (buffer[0] == (WCHAR)0x005F || buffer[0] == (WCHAR)0xFF3F))  // According to CLS Spec these are '_'
                    compiler()->ErrorRef(NULL, WRN_CLS_BadIdentifier, agg);

                // Check for colliding names
                temp = compiler()->getLSymmgr().LookupLocalSym(
                    compiler()->namemgr->AddString( buffer), CLSroot, MASK_CACHESYM)->asCACHESYM();
                if (temp && agg->name != temp->sym->name) {
                    if (agg->isSource || (temp->sym->isAGGSYM() && temp->sym->asAGGSYM()->isSource) ||
                        (temp->sym->isNSSYM() && temp->sym->asNSSYM()->isDefinedInSource)) {
                        compiler()->ErrorRef(NULL, WRN_CLS_BadIdentifierCase, agg, temp->sym);
                    }
                } else {
                    temp = compiler()->getLSymmgr().CreateLocalSym( SK_CACHESYM,
                        compiler()->namemgr->AddString( buffer), CLSroot)->asCACHESYM();
                    temp->sym = agg;
                }
            }
            else if (member->isNSDECLSYM()) {
                NSSYM * nested = member->asNSDECLSYM()->NameSpace();

                // Only check namespaces that we haven't already visited
                if (!nested->checkingForCLS) {
                    nested->checkingForCLS = true;

                    SafeToLowerCase( nested->name->text, wcbuffer);

                    //Only warn on problems with source that is being compiled.
                    if (nested->isDefinedInSource && 
                        (buffer[0] == (WCHAR)0x005F || buffer[0] == (WCHAR)0xFF3F))  // According to CLS Spec these are '_'
                        compiler()->ErrorRef(NULL, WRN_CLS_BadIdentifier, member);

                    // Check for colliding names
                    temp = compiler()->getLSymmgr().LookupLocalSym(
                        compiler()->namemgr->AddString( buffer), CLSroot, MASK_CACHESYM)->asCACHESYM();
                    if (temp && nested->name != temp->sym->name) {
                        if (temp->sym->isNSSYM() && (temp->sym->asNSSYM()->isDefinedInSource || nested->isDefinedInSource) ||
                            temp->sym->isAGGSYM() && temp->sym->asAGGSYM()->isSource)
                        {
                            compiler()->ErrorRef(NULL, WRN_CLS_BadIdentifierCase, temp->sym, nested);
                        }
                    } else {
                        temp = compiler()->getLSymmgr().CreateLocalSym( SK_CACHESYM,
                            compiler()->namemgr->AddString( buffer), CLSroot)->asCACHESYM();
                        temp->sym = nested;
                    }
                }
            }
            else {
                ASSERT(!member->isAGGSYM());
            }
        ENDFOREACHCHILD
    }
    ns->checkedForCLS = true;

    //Cleanup
    compiler()->DiscardLocalState();
}



// prepares a class for compilation by preparing all of its elements...
// This should also verify that a class actually implements its interfaces...
// This also calls defineAggregate() for the class, resolves its inheritance,
// creates its nested types & prepares them - basically
// everything is brought up to fully-declared state except the code.
// Its type variables will already have been created.  We do not have to
// force the declaration of the types involved in the
// constraints for the type variables until we actually try to satisfy the constraints.
void CLSDREC::prepareAggregate(AGGSYM * cls)
{
    // We shouldn't call this before we're in the Prepare stage.
    ASSERT(compiler()->CompPhase() >= CompilerPhase::Prepare);

    if (cls->isFabricated) {
        ASSERT(!cls->hasParseTree);
        ASSERT(cls->IsPrepared());
        cls->setBogus(false);
        return;
    }
    ASSERT(!cls->isSource == !cls->hasParseTree);

    // If this assert fires it probably means that there's recursion in the base agg or base interface aggs.
    ASSERT(cls->AggState() != AggState::Preparing);

    if (cls->AggState() >= AggState::Preparing) {
        ASSERT(cls->AggKind() > AggKind::Unknown);
        return;
    }

    // This takes care of calling ResolveInheritanceRec if needed.
    if (!cls->hasParseTree && cls->AggState() < AggState::DefinedMembers) {
        compiler()->importer.DefineImportedType(cls);
        if (cls->AggState() < AggState::DefinedMembers) {
            return;
        }
        if (cls->AggState() >= AggState::Prepared)
            return;
    } 
    ASSERT(cls->AggKind() > AggKind::Unknown);

    ASSERT(AggState::DefinedMembers <= cls->AggState() && cls->AggState() < AggState::Preparing);
    cls->SetAggState(AggState::Preparing);

    SETLOCATIONSYM(cls);

    if (cls->baseClass) {
        // Bring the base aggregate up to prepared - not the whole AGGTYPESYM.
        prepareAggregate(cls->baseClass->getAggregate());
        // If our base class has conversion operators, we have conversion operators....
        if (cls->IsClass()) {
            cls->hasConversion |= cls->baseClass->getAggregate()->hasConversion;
        }
    }

    for (int i = 0; i < cls->ifaces->size; i++) {
        // Bring the iface aggregate up to prepared - not the whole AGGTYPESYM.
        AGGTYPESYM * iface = cls->ifaces->Item(i)->asAGGTYPESYM();
        prepareAggregate(iface->getAggregate());
    }

    if (cls->hasParseTree) {
        // Source type.

        if (cls->isNested()) {
            // Look for an accessible member with the same name in a base class of our enclosing class.
            // Check that the shadowing flags are correct.
            unsigned flags = 0;
            FOREACHAGGDECL(cls, decl)
                flags |= decl->parseTree->flags;
            ENDFOREACHAGGDECL
            checkSimpleHiding(cls, flags);
        }

        switch (cls->AggKind()) {
        default:
            ASSERT(0);
        case AggKind::Class:
        case AggKind::Struct:
            prepareClassOrStruct(cls);
            break;

        case AggKind::Delegate:
        case AggKind::Enum:
            // Nothing to prepare for enums or delegates
            cls->setBogus(false);
            cls->SetAggState(AggState::Prepared);
            break;

        case AggKind::Interface:
            prepareInterface(cls);
            break;
        }

        ASSERT(cls->IsPrepared());

    } else {
        setOverrideBits(cls);

        cls->setBogus(cls->getBogus());
        BuildOrCheckAbstractMethodsList(cls);
        cls->SetAggState(AggState::Prepared);
    }
    if (cls->isSource && (cls->IsClass() || cls->IsStruct()) && cls->getOutputFile()->entryClassName == NULL) {
        findEntryPoint(cls);
    }
}


// finds an explicit interface implementation on a class or struct
// cls is the type to look in
// swt is the interface member to look for
SYM * CLSDREC::FindExplicitInterfaceImplementation(AGGTYPESYM * ats, SymWithType swt)
{
    if (swt.Sym()->isEVENTSYM()) {
        for (EVENTSYM * evt = compiler()->getBSymmgr().LookupAggMember(NULL, ats->getAggregate(), MASK_EVENTSYM)->asEVENTSYM();
            evt;
            evt = compiler()->getBSymmgr().LookupNextSym(evt, evt->getClass(), MASK_EVENTSYM)->asEVENTSYM())
        {
            if (swt.Sym() == evt->ewtSlot.Event() && evt->IsExpImpl() &&
                compiler()->getBSymmgr().SubstEqualTypes(swt.Type(), evt->ewtSlot.Type(), ats))
            {
                return evt;
            }
        }

        return NULL;
    }

    for (METHPROPSYM * mps = compiler()->getBSymmgr().LookupAggMember(NULL, ats->getAggregate(), MASK_METHSYM | MASK_PROPSYM)->asMETHPROPSYM();
        mps;
        mps = compiler()->getBSymmgr().LookupNextSym(mps, mps->getClass(), MASK_METHSYM | MASK_PROPSYM)->asMETHPROPSYM())
    {
        if (swt.Sym() == mps->swtSlot.MethProp() && mps->IsExpImpl() &&
            compiler()->getBSymmgr().SubstEqualTypes(swt.Type(), mps->swtSlot.Type(), ats))
        {
            return mps;
        }
    }

    return NULL;
}


//
// for each method in a class, set the override bit correctly
//
void CLSDREC::setOverrideBits(AGGSYM * cls)
{
    ASSERT(!cls->isSource);

    if (!cls->baseClass)
        return;

    ASSERT(cls->baseClass->getAggregate()->IsPrepared());

    FOREACHCHILD(cls, sym)
        if (sym->isMETHSYM() && sym->asMETHSYM()->isOverride) {

            METHSYM * method = sym->asMETHSYM();
            method->isOverride = false;
            //
            // this method could be an override
            // NOTE: that we must set the isOverride flag accurately
            // because it affects:
            //      - lookup rules(override methods are ignored)
            //      - list of inherited abstract methods
            //
            SymWithType swtHid;

            if (FindSymHiddenByMethPropAgg(method, cls->baseClass, cls, &swtHid) && swtHid.Sym()->isMETHSYM()) {
                if (compiler()->getBSymmgr().SubstEqualTypes(method->retType, swtHid.Meth()->retType, swtHid.Type(), method->typeVars) &&
                    swtHid.Meth()->GetAccess() == method->GetAccess() && swtHid.Meth()->isMetadataVirtual)
                {
                    method->isOverride = true;

                    ASSERT(!swtHid.Meth()->isOverride == !swtHid.Meth()->swtSlot);
                    if (swtHid.Meth()->swtSlot) {
                        method->swtSlot.Set(
                            swtHid.Meth()->swtSlot.Meth(),
                            compiler()->getBSymmgr().SubstType(swtHid.Meth()->swtSlot.Type(), swtHid.Type())->asAGGTYPESYM());
                    }
                    else
                        method->swtSlot = swtHid;
                }

                if (swtHid.Meth()->isDtor())
                    method->SetMethKind(MethodKind::Dtor);
            }
        }
    ENDFOREACHCHILD

    // Now set the override bits on the property itself
    FOREACHCHILD(cls, sym)
        if (!sym->isPROPSYM() || sym->getBogus())
            continue;

        PROPSYM * prop = sym->asPROPSYM();
        ASSERT(!prop->isOverride);

        PROPSYM * propBase;
        AGGTYPESYM * atsBase;

        if (prop->methGet && prop->methSet) {
            if (!prop->methGet->isOverride && !prop->methSet->isOverride)
                continue;

            if (prop->methGet->isOverride != prop->methSet->isOverride ||
                !prop->methGet->swtSlot ||
                !prop->methSet->swtSlot ||
                !prop->methGet->swtSlot.Meth()->isPropertyAccessor() ||
                !prop->methSet->swtSlot.Meth()->isPropertyAccessor() ||
                (atsBase = prop->methGet->swtSlot.Type()) != prop->methSet->swtSlot.Type() ||
                (propBase = prop->methGet->swtSlot.Meth()->getProperty()) != prop->methSet->swtSlot.Meth()->getProperty())
            {
                prop->setBogus(true);
                prop->useMethInstead = true;
                continue;
            }
        }
        else {
            METHSYM * methAcc;

            if (prop->methGet)
                methAcc = prop->methGet;
            else if (prop->methSet)
                methAcc = prop->methSet;
            else {
                VSFAIL("A property without accessors?");
                continue;
            }

            if (!methAcc->isOverride)
                continue;

            if (!methAcc->swtSlot ||
                !methAcc->swtSlot.Meth()->isPropertyAccessor())
            {
                prop->setBogus(true);
                prop->useMethInstead = true;
                continue;
            }

            atsBase = methAcc->swtSlot.Type();
            propBase = methAcc->swtSlot.Meth()->getProperty();
        }

        prop->isOverride = true;
        prop->swtSlot.Set(propBase, atsBase);
    ENDFOREACHCHILD
}


// For abstract classes, build list of abstract methods.
// For non-abstract classes, report errors on inherited abstract methods.
void CLSDREC::BuildOrCheckAbstractMethodsList(AGGSYM * cls)
{
    ASSERT(!cls->abstractMethods);

    if (cls->IsInterface())
        return;

    PSYMLIST *addToList = &cls->abstractMethods;

    if (cls->isAbstract) {
        ASSERT(cls->IsClass());

        // Add all new abstract methods.
        FOREACHCHILD(cls, child)
            if (child->isMETHSYM() && child->asMETHSYM()->isAbstract) {
                compiler()->getBSymmgr().AddToGlobalSymList(child, &addToList);
            }
        ENDFOREACHCHILD
    }
    else if (!cls->isSource)
        return;

    // Deal with inherited abstract methods that we don't implement.
    // NOTE: this deals with property accessors as well.
    if (!cls->baseClass)
        return;

    AGGTYPESYM * typeMeth = cls->baseClass;
    for (SYMLIST * listMeth = typeMeth->getAggregate()->abstractMethods; listMeth; listMeth = listMeth->next) {
        METHSYM * meth = listMeth->sym->asMETHSYM();

        typeMeth = typeMeth->FindBaseType(meth->getClass());
        ASSERT(typeMeth && typeMeth->getAggregate() == meth->getClass());

        METHSYM * methImpl;

        MethWithType mwt(meth, typeMeth);

        if (!(methImpl = FindSameSignature(mwt, cls->getThisType(), true))) {
            if (!cls->isAbstract)
                compiler()->ErrorRef(NULL, ERR_UnimplementedAbstractMethod, cls, mwt);
            else
                compiler()->getBSymmgr().AddToGlobalSymList(meth, &addToList);
        }
    }
}


// prepares a class for compilation by preparing all of its elements...
// This should also verify that a class actually implements its interfaces...
void CLSDREC::prepareClassOrStruct(AGGSYM * cls)
{
    ASSERT(cls->IsPreparing());
    ASSERT(cls->isSource);
    ASSERT(cls->baseClass && cls->baseClass->getAggregate()->IsPrepared() || cls->isPredefAgg(PT_OBJECT) && !cls->baseClass);

    if (cls->isAttribute) {
        ASSERT(cls->baseClass);
        AGGSYM *base = cls->GetBaseAgg();
        ASSERT(base->attributeClass || cls->isPredefAgg(PT_ATTRIBUTE));

        // Attributes that don't have usage set should inherit it from their base class.
        if (!cls->attributeClass) {
            cls->attributeClass = base->attributeClass;
            cls->isMultipleAttribute = base->isMultipleAttribute;
        }
    }

    cls->setBogus(false);
    cls->SetAggState(AggState::Prepared);

    //
    // check that there isn't a cycle in the members
    // of this struct and other structs
    //
    if (cls->IsStruct() && !cls->fLayoutChecked)
        CheckStructLayout(cls->getThisType(), NULL);

    //
    // prepare members
    //
    FOREACHCHILD(cls, child)

        SETLOCATIONSYM(child);

        switch (child->getKind()) {
        case SK_MEMBVARSYM:
            prepareFields(child->asMEMBVARSYM());
            break;
        case SK_AGGSYM:
            //
            // need to do this after fully preparing members
            //
            break;
        case SK_TYVARSYM:
            break;
        case SK_METHSYM:
            prepareMethod(child->asMETHSYM());
            break;
        case SK_AGGTYPESYM:
            break;
        case SK_PROPSYM:
            prepareProperty(child->asPROPSYM());
            break;
        case SK_EVENTSYM:
            prepareEvent(child->asEVENTSYM());
            break;
        default:
            ASSERT(!"Unknown node type");
        }
    ENDFOREACHCHILD

    //
    // for abstract classes, build list of abstract methods
    // NOTE: must come after preparing members for abstract property accessors
    //       and before preparing nested types so they can properly check themselves
    //
    BuildOrCheckAbstractMethodsList(cls);

    //
    // prepare agggregate members
    //
    FOREACHCHILD(cls, child)

        SETLOCATIONSYM(child);

        switch (child->getKind()) {
        case SK_AGGSYM:
            prepareAggregate(child->asAGGSYM());
            break;
        default:
            break;
        }
    ENDFOREACHCHILD

    CheckInterfaceImplementations(cls);

    cls->SetAggState(AggState::PreparedMembers);
}


void CLSDREC::CheckInterfaceImplementations(AGGSYM * cls)
{
    MethWithType mwt;
    PropWithType pwt;
    EventWithType ewt;
    bool fFoundImport;
    AGGTYPESYM * atsCur;
    METHSYM * methFound;

    // Check that all interface methods are implemented.
    for (int i = 0; i < cls->ifacesAll->size; i++) {
        AGGTYPESYM * iface = cls->ifacesAll->Item(i)->asAGGTYPESYM();
        FOREACHCHILD(iface->getAggregate(), member)
            switch (member->getKind()) {
            case SK_METHSYM:
            {
                if (member->asMETHSYM()->isAnyAccessor()) {
                    //
                    // property accessor implementations are checked
                    // by their property declaration
                    // 
                    continue;
                }

                METHSYM * methFound;
                MethWithType mwtClose(NULL, NULL);

                fFoundImport = false;
                atsCur = cls->getThisType();
                mwt.Set(member->asMETHSYM(), iface);
                do {
                    // Check for explicit interface implementation.
                    methFound = FindExplicitInterfaceImplementation(atsCur, mwt)->asMETHSYM();
                    if (methFound)
                        break;

                    // Check for imported class which implements this interface.
                    if (!atsCur->getAggregate()->hasParseTree && atsCur->GetIfacesAll()->Contains(iface)) {
                        fFoundImport = true;
                        break;
                    }

                    // Check for implicit interface implementation.
                    methFound = FindSameSignature(mwt, atsCur, false);
                    if (methFound) {
                        if (!methFound->isStatic && 
                            methFound->GetAccess() == ACC_PUBLIC &&
                            compiler()->getBSymmgr().SubstType(methFound->retType, atsCur, mwt.Meth()->typeVars) ==
                                compiler()->getBSymmgr().SubstType(mwt.Meth()->retType, iface))
                        {
                            // Found a match.
                            if (methFound->isAnyAccessor()) {
                                compiler()->ErrorRef(NULL, ERR_AccessorImplementingMethod, MethWithType(methFound, atsCur), mwt, cls);
                                goto LDoneMeth;
                            }
                            if (mwt.Meth()->typeVars->size > 0) {
                                ASSERT(mwt.Meth()->typeVars->size == methFound->typeVars->size);
                                CheckImplicitImplConstraints(MethWithType(methFound, atsCur), mwt);
                            }
                            break;
                        }

                        // Found a close match, save it for error reporting
                        // and continue checking.
                        if (!mwtClose)
                            mwtClose.Set(methFound, atsCur);
                    }

                    methFound = NULL;
                    atsCur = atsCur->GetBaseClass();
                } while (atsCur);

                if (methFound) {
                    if (GetConditionalSymbols(methFound))
                        compiler()->ErrorRef(NULL, ERR_InterfaceImplementedByConditional, MethWithType(methFound, atsCur), mwt, cls);
                    if (!methFound->IsExpImpl())
                        methFound = NeedExplicitImpl(mwt, methFound, cls);

                    // If the implementing method can't be set to isMetadataVirtual
                    // then we should have added a compiler generated explicit impl in NeedExplicitImpl
                    ASSERT(methFound->isMetadataVirtual);
                }
                else if (!fFoundImport) {
                    // Didn't find an implementation.
                    if (!mwtClose)
                        compiler()->ErrorRef(NULL, ERR_UnimplementedInterfaceMember, cls, mwt);
                    else
                        compiler()->ErrorRef(NULL, ERR_CloseUnimplementedInterfaceMember, cls, mwt, mwtClose);
                }
LDoneMeth:
                break;
            }
            case SK_PROPSYM:
            {
                if (member->hasBogus() && member->checkBogus()) {
                    //
                    // don't need implementation of bogus properties
                    // just need implementation of accessors as regular methods
                    //
                    break;
                }

                bool isExplicitImpl = false;
                PROPSYM * propFound;
                PROPSYM * propClose = NULL;

                fFoundImport = false;
                atsCur = cls->getThisType();
                pwt.Set(member->asPROPSYM(), iface);
                do {
                    // Check for explicit interface implementation.
                    propFound = FindExplicitInterfaceImplementation(atsCur, pwt)->asPROPSYM();
                    if (propFound) {
                        isExplicitImpl = true;
                        break;
                    }

                    // Check for imported class which implements this interface.
                    if (!atsCur->getAggregate()->hasParseTree && atsCur->GetIfacesAll()->Contains(iface)) {
                        fFoundImport = true;
                        break;
                    }

                    // Check for implicit interface implementation.
                    propFound = findSameSignature(iface, pwt.Prop(), atsCur);
                    if (propFound) {
                        if (!propFound->isStatic && 
                            propFound->GetAccess() == ACC_PUBLIC &&
                            compiler()->getBSymmgr().SubstType(propFound->retType, atsCur) ==
                                compiler()->getBSymmgr().SubstType(pwt.Prop()->retType, iface) &&
                            !propFound->isOverride)
                        {
                            // found a match
                            break;
                        }

                        // Found a close match, save it for error reporting
                        // and continue checking.
                        if (!propClose)
                            propClose = propFound;
                        propFound = NULL;
                    }
                    else {
                        // Check for methods that collide with the accessor names.
                        if (pwt.Prop()->methGet) {
                            mwt.Set(pwt.Prop()->methGet, pwt.Type());
                            methFound = FindSameSignature(mwt, atsCur, false);
                            if (methFound) {
                                compiler()->ErrorRef(NULL, ERR_MethodImplementingAccessor, MethWithType(methFound, atsCur), mwt, cls);
                                goto LDoneProp;
                            }
                        }
                        if (pwt.Prop()->methSet) {
                            mwt.Set(pwt.Prop()->methSet, pwt.Type());
                            methFound = FindSameSignature(mwt, atsCur, false);
                            if (methFound) {
                                compiler()->ErrorRef(NULL, ERR_MethodImplementingAccessor, MethWithType(methFound, atsCur), mwt, cls);
                                goto LDoneProp;
                            }
                        }
                    }
                    atsCur = atsCur->GetBaseClass();
                } while (atsCur);

                if (!propFound && !fFoundImport) {
                    // Didn't find an implementation.
                    if (!propClose)
                        compiler()->ErrorRef(NULL, ERR_UnimplementedInterfaceMember, cls, pwt);
                    else
                        compiler()->ErrorRef(NULL, ERR_CloseUnimplementedInterfaceMember, cls, pwt, propClose);
                }
                else if (propFound) {
                    // Check that all accessors are implemented.
                    mwt.Set(member->asPROPSYM()->methGet, iface);
                    if (mwt && !propFound->methGet)
                        compiler()->ErrorRef(NULL, ERR_UnimplementedInterfaceMember, cls, mwt);
                    else if (mwt) {
                        if (!isExplicitImpl && propFound->methGet->GetAccess() != mwt.Meth()->GetAccess()) {
                            ASSERT(mwt.Meth()->GetAccess() == ACC_PUBLIC);
                            compiler()->ErrorRef(NULL, ERR_UnimplementedInterfaceAccessor, cls, mwt, propFound->methGet);
                        }
                        METHSYM *methGet;
                        methGet = NeedExplicitImpl(mwt, propFound->methGet, cls);
                        // If the implementing method can't be set to isMetadataVirtual
                        // then we should have added a compiler generated explicit impl in NeedExplicitImpl.
                        ASSERT(methGet->isMetadataVirtual);
                    }
                    mwt.Set(member->asPROPSYM()->methSet, iface);
                    if (mwt && !propFound->methSet)
                        compiler()->ErrorRef(NULL, ERR_UnimplementedInterfaceMember, cls, mwt);
                    else if (mwt) {
                        if (!isExplicitImpl && propFound->methSet->GetAccess() != mwt.Meth()->GetAccess()) {
                            ASSERT(mwt.Meth()->GetAccess() == ACC_PUBLIC);
                            compiler()->ErrorRef(NULL, ERR_UnimplementedInterfaceAccessor, cls, mwt, propFound->methSet);
                        }
                        METHSYM *methSet;
                        methSet = NeedExplicitImpl(mwt, propFound->methSet, cls);
                        // If the implementing method can't be set to isMetadataVirtual
                        // then we should have added a compiler generated explcit impl in NeedExplicitImpl.
                        ASSERT(methSet->isMetadataVirtual);
                    }
                }
LDoneProp:
                break;
            }
            case SK_EVENTSYM:
            {
                if (member->hasBogus() && member->checkBogus()) {
                    //
                    // don't need implementation of bogus events
                    // just need implementation of accessors as regular methods
                    //
                    break;
                }

                EVENTSYM * evtFound;
                EVENTSYM * evtClose = NULL;

                fFoundImport = false;
                atsCur = cls->getThisType();
                ewt.Set(member->asEVENTSYM(), iface);
                do {
                    // Check for explicit interface implementation.
                    evtFound = FindExplicitInterfaceImplementation(atsCur, ewt)->asEVENTSYM();
                    if (evtFound)
                        break;

                    // Check for imported base class which implements this interface.
                    if (!atsCur->getAggregate()->hasParseTree && atsCur->GetIfacesAll()->Contains(iface)) {
                        fFoundImport = true;
                        break;
                    }

                    // Check for implicit interface implementation.
                    evtFound = compiler()->getBSymmgr().LookupAggMember(ewt.Event()->name, atsCur->getAggregate(), MASK_EVENTSYM)->asEVENTSYM();
                    if (evtFound) {
                        if (!evtFound->isStatic && 
                            evtFound->GetAccess() == ACC_PUBLIC &&
                            compiler()->getBSymmgr().SubstType(evtFound->type, atsCur) ==
                                compiler()->getBSymmgr().SubstType(ewt.Event()->type, iface))
                        {
                            // Found a match.
                            break;
                        }

                        // Found a close match, save it for error reporting
                        // and continue checking.
                        if (!evtClose)
                            evtClose = evtFound;
                        evtFound = NULL;
                    }
                    // Check for methods that collide with the accessor names.
                    else {
                        mwt.Set(ewt.Event()->methAdd, ewt.Type());
                        methFound = FindSameSignature(mwt, atsCur, false);
                        if (methFound) {
                            compiler()->ErrorRef(NULL, ERR_MethodImplementingAccessor, MethWithType(methFound, atsCur), mwt, cls);
                            goto LDoneEvent;
                        }
                        mwt.Set(ewt.Event()->methRemove, ewt.Type());
                        methFound = FindSameSignature(mwt, atsCur, false);
                        if (methFound) {
                            compiler()->ErrorRef(NULL, ERR_MethodImplementingAccessor, MethWithType(methFound, atsCur), mwt, cls);
                            goto LDoneEvent;
                        }
                    }

                    atsCur = atsCur->GetBaseClass();
                } while (atsCur);

                if (!evtFound && !fFoundImport) {
                    // Didn't find an implementation.
                    if (!evtClose)
                        compiler()->ErrorRef(NULL, ERR_UnimplementedInterfaceMember, cls, ewt);
                    else
                        compiler()->ErrorRef(NULL, ERR_CloseUnimplementedInterfaceMember, cls, ewt, evtClose);
                }
                else if (evtFound) {
                    mwt.Set(ewt.Event()->methAdd, iface);
                    NeedExplicitImpl(mwt, evtFound->methAdd, cls);
                    mwt.Set(ewt.Event()->methRemove, iface);
                    NeedExplicitImpl(mwt, evtFound->methRemove, cls);
                }
LDoneEvent:
                break;
            }
            case SK_TYVARSYM:
                break;
            default:
                ASSERT(!"Unknown interface member");
                break;
            }
        ENDFOREACHCHILD
    }
}


//
// checks if we need a compiler generated explicit method impl
// returns the actual method implementing the interface method
//
METHSYM *CLSDREC::NeedExplicitImpl(MethWithType mwtIface, METHSYM *methImpl, AGGSYM *cls)
{
    ASSERT(mwtIface.Type() && mwtIface.Meth()->getClass() == mwtIface.Type()->getAggregate());

    CheckLinkDemandOnOverride(methImpl, mwtIface);

    if (!methImpl->IsExpImpl() &&
        (mwtIface.Meth()->modOptCount || methImpl->modOptCount ||   // differing signatures
            mwtIface.Meth()->name != methImpl->name ||              // can happen when implementing properties
            !methImpl->isMetadataVirtual && !methImpl->getInputFile()->isSource))
    {

        // This is a compiler-generated method, so it doesn't matter which class declaration we use that it was declared
        // in, hence it is OK and reasonable to use DeclFirst here.
        IFACEIMPLMETHSYM *impl = compiler()->getBSymmgr().CreateIfaceImplMethod(cls, cls->DeclFirst());

        mwtIface.Meth()->copyInto(impl, mwtIface.Type(), compiler());
        impl->SetAccess(ACC_PRIVATE);
        impl->isOverride = false;
        impl->modOptCount = mwtIface.Meth()->modOptCount;
        impl->swtSlot = mwtIface;
        impl->fNeedsMethodImp = true;
        impl->implMethod = methImpl;
        impl->parseTree = cls->DeclFirst()->parseTree; // DeclFirst is OK, see comment above.
        impl->isAbstract = false;
        impl->isMetadataVirtual = true;
        impl->declaration = cls->DeclFirst();

        return impl;
    } else {
        methImpl->isMetadataVirtual = true;
        return methImpl;
    }
}


// define a class.  this means bind its base class and implemented interface
// list as well as define the class elements.
//
// gives an error if this type is involved in a cycle in the inheritance chain
//
// NOTE: this does not work for the predefined 'object' type. </STRIP>
void CLSDREC::defineAggregate(AGGSYM *cls)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::DefineMembers);

    ASSERT(cls->AggKind() > AggKind::Unknown);
    ASSERT(cls->isSource);
    ASSERT(cls->hasParseTree || cls->isFabricated);

    // check if we're done already
    if (cls->AggState() >= AggState::DefiningMembers) {
        ASSERT(cls->AggState() >= AggState::DefinedMembers);
        return;
    }

    // object shouldn't come through here.
    ASSERT(!cls->isPredefAgg(PT_OBJECT));

#ifdef DEBUG
    compiler()->haveDefinedAnyType = true;
#endif

    // This used to call resolve inheritance. I (ShonK) don't think it needed to....
    ASSERT(cls->HasResolvedBaseClasses());

    SETLOCATIONSYM(cls);
    cls->SetAggState(AggState::DefiningMembers);

    CheckForProtectedInSealed(cls);

    //
    // do the members
    //
    if (cls->IsEnum()) {
        defineEnumMembers(cls);
    } else if (cls->IsDelegate()) {
        defineDelegateMembers(cls);
    } else if (!cls->isFabricated) {
        defineAggregateMembers(cls);
    }

    ASSERT(cls->AggState() == AggState::DefiningMembers);
    cls->SetAggState(AggState::DefinedMembers);
}


// handles the special case of bringing object up to defined state
void CLSDREC::defineObject()
{
    AGGSYM *object = compiler()->GetReqPredefAgg(PT_OBJECT, false);
    ASSERT(object);

    ASSERT(AggState::Inheritance <= object->AggState() && object->AggState() < AggState::DefiningMembers);

    SETLOCATIONSYM(object);

    if (object->isSource) {
        // we are defining object. Better not have a base class.
        FOREACHAGGDECL(object, aggdecl)
            NODELOOP(aggdecl->parseTree->asAGGREGATE()->pBases, BASE, base) 
                compiler()->ErrorRef(aggdecl->parseTree, ERR_ObjectCantHaveBases, object);
            ENDLOOP;
        ENDFOREACHAGGDECL

        object->SetAggState(AggState::DefinedMembers);
        defineAggregateMembers(object);
    } else {
        //
        // import all of its members from the
        // metadata file
        //
        compiler()->importer.DefineImportedType(object);
        ASSERT(object->IsDefined());

        ASSERT(object->DeclOnly()->getInputFile()->isBCL);

        SYM * dtor = compiler()->getBSymmgr().LookupAggMember(compiler()->namemgr->GetPredefName(PN_DTOR), object, MASK_METHSYM);
        if (dtor)
            dtor->asMETHSYM()->SetMethKind(MethodKind::Dtor);
    }
}

// define a types members, does not do base classes
// and implemented interfaces
void CLSDREC::defineAggregateMembers(AGGSYM *cls)
{
    ASSERT(cls->AggState() >= AggState::DefiningMembers);

    //
    // define nested types, interfaces will have no members at this point
    //
    FOREACHCHILD(cls, elem)

        SETLOCATIONSYM(elem);

        // should only have types at this point
        switch (elem->getKind()) {
        case SK_AGGSYM:
            ASSERT(elem->asAGGSYM()->AggKind() > AggKind::Unknown && elem->asAGGSYM()->AggKind() < AggKind::Lim);
            defineAggregate(elem->asAGGSYM());
            break;

        case SK_AGGTYPESYM: 
        case SK_TYVARSYM: 
            break;

        default:
            ASSERT(!"Unknown member");
        }
    ENDFOREACHCHILD

    //
    // define members
    //
    // We never generate a non-static constructor for structs...
    bool seenConstructor = cls->IsStruct();
    bool needStaticConstructor = false;
    bool mustMatch = false;
    
    // Iterate all members of all declarations.
    FOREACHAGGDECL(cls, clsdecl)

        MEMBERNODE * memb = clsdecl->parseTree->asAGGREGATE()->pMembers; 
        while (memb) {
        
            SETLOCATIONNODE(memb);

            switch(memb->kind) {
            case NK_DTOR:
            case NK_METHOD:
                defineMethod(memb->asANYMETHOD(), cls, clsdecl);
                break;
            case NK_CTOR:
            {
                METHSYM * method = defineMethod(memb->asCTOR(), cls, clsdecl);
                if (method) {
                    if (method->isStatic) 
                    {
                        if (!cls->IsInterface())
                            cls->hasUDStaticCtor = true;
                    } else {
                        seenConstructor = true;
                        if (method->params->size == 0) {
                            cls->hasNoArgCtor = true;
                            if (method->GetAccess() == ACC_PUBLIC)
                                cls->hasPubNoArgCtor = true;
                        }
                    }
                }
                break;
            }
            case NK_OPERATOR:
                mustMatch |= defineOperator(memb->asOPERATOR(), cls, clsdecl);
                break;
            case NK_FIELD:
                needStaticConstructor |= defineFields(memb->asFIELD(), cls, clsdecl);
                break;
            case NK_NESTEDTYPE:
                // seperate loop for nested types
                break;
            case NK_PROPERTY:
                defineProperty(memb->asPROPERTY(), cls, clsdecl);
                break;
            case NK_INDEXER:
                defineProperty(memb->asINDEXER(), cls, clsdecl);
                break;
            case NK_CONST:
                needStaticConstructor |= defineFields(memb->asCONST(), cls, clsdecl);
                break;
            case NK_PARTIALMEMBER:
                // Ignore this codesense artifact...
                break;
            default:
                ASSERT(!"Unknown node type");
            }


            memb = memb->pNext;
        }
    
    ENDFOREACHAGGDECL
    
    METHSYM * equalsMember = NULL;
    METHSYM * gethashcodeMember = NULL;
    if (!cls->IsInterface()) {
        // find public override bool Equals(object)
        for (equalsMember = compiler()->getBSymmgr().LookupAggMember(compiler()->namemgr->GetPredefName(PN_EQUALS), cls, MASK_METHSYM)->asMETHSYM();
            equalsMember;
            equalsMember = compiler()->getBSymmgr().LookupNextSym(equalsMember, cls, MASK_METHSYM)->asMETHSYM()) {

            if (equalsMember->isOverride && 
                equalsMember->GetAccess() == ACC_PUBLIC && 
                equalsMember->params->size == 1 && equalsMember->params->Item(0)->isPredefType(PT_OBJECT) &&
                equalsMember->retType->isPredefType(PT_BOOL))
            {
                break;
            }
        }

        // find public override int GetHashCode()
        for (gethashcodeMember = compiler()->getBSymmgr().LookupAggMember(compiler()->namemgr->GetPredefName(PN_GETHASHCODE), cls, MASK_METHSYM)->asMETHSYM();
            gethashcodeMember;
            gethashcodeMember = compiler()->getBSymmgr().LookupNextSym(gethashcodeMember, cls, MASK_METHSYM)->asMETHSYM()) {

            if (gethashcodeMember->isOverride && 
                gethashcodeMember->GetAccess() == ACC_PUBLIC && 
                gethashcodeMember->params->size == 0 &&
                gethashcodeMember->retType->isPredefType(PT_INT))
            {
                break;
            }
        }

        if (equalsMember && !gethashcodeMember) {
            compiler()->ErrorRef(NULL, WRN_EqualsWithoutGetHashCode, cls);
        }
    }

    if (mustMatch) {
        checkMatchingOperator(PN_OPEQUALITY, cls);
        checkMatchingOperator(PN_OPINEQUALITY, cls);
        checkMatchingOperator(PN_OPGREATERTHAN, cls);
        checkMatchingOperator(PN_OPLESSTHAN, cls);
        checkMatchingOperator(PN_OPGREATERTHANOREQUAL, cls);
        checkMatchingOperator(PN_OPLESSTHANOREQUAL, cls);
        checkMatchingOperator(PN_OPTRUE, cls);
        checkMatchingOperator(PN_OPFALSE, cls);

        if (!cls->IsInterface() && (!equalsMember || !gethashcodeMember)) {
            bool foundEqualityOp = false;
            for (METHSYM * op = compiler()->getBSymmgr().LookupAggMember(compiler()->namemgr->GetPredefName(PN_OPEQUALITY), cls, MASK_METHSYM)->asMETHSYM();
                op;
                op = compiler()->getBSymmgr().LookupNextSym(op, cls, MASK_METHSYM)->asMETHSYM()) {

                if (op->isOperator) {
                    foundEqualityOp = true;
                    break;
                }
            }

            if (!foundEqualityOp) {
                for (METHSYM * op = compiler()->getBSymmgr().LookupAggMember(compiler()->namemgr->GetPredefName(PN_OPINEQUALITY), cls, MASK_METHSYM)->asMETHSYM();
                    op;
                    op = compiler()->getBSymmgr().LookupNextSym(op, cls, MASK_METHSYM)->asMETHSYM()) {

                    if (op->isOperator) {
                        foundEqualityOp = true;
                        break;
                    }
                }
            }

            if (foundEqualityOp) {
                if (!equalsMember) {
                    compiler()->ErrorRef(NULL, WRN_EqualityOpWithoutEquals, cls);
                }
                if (!gethashcodeMember) {
                    compiler()->ErrorRef(NULL, WRN_EqualityOpWithoutGetHashCode, cls);
                }
            }
        }
    }
    
    //
    // Synthetize the static and non static default constructors if necessary...
    //
    if (!cls->IsInterface())
    {
        if (!cls->hasUDStaticCtor && needStaticConstructor)
            synthesizeConstructor(cls, true);

        if (!seenConstructor && !cls->IsStatic())
            synthesizeConstructor(cls, false);
    }
}

void CLSDREC::synthesizeConstructor(AGGSYM *cls, bool isStatic)
{
    // There are synthesize method that can be put in any declaration, so DeclFirst is OK here.
    METHSYM *method = compiler()->getBSymmgr().CreateMethod(compiler()->namemgr->GetPredefName(isStatic ? PN_STATCTOR : PN_CTOR), cls, cls->DeclFirst());
    if (isStatic)
        method->SetAccess(ACC_PRIVATE); // static ctor is always private
    else if (cls->isAbstract)
        method->SetAccess(ACC_PROTECTED); // default constructor for abstract classes is protected.
    else 
        method->SetAccess(ACC_PUBLIC);
    method->SetMethKind(MethodKind::Ctor);
    method->retType = compiler()->getBSymmgr().GetVoid();
    method->params = BSYMMGR::EmptyTypeArray();
    method->typeVars = BSYMMGR::EmptyTypeArray();
    method->parseTree = cls->DeclFirst()->parseTree; 
    method->isStatic = isStatic;

    if (!isStatic)
    {
        cls->hasNoArgCtor = true;
        if (method->GetAccess() == ACC_PUBLIC)
            cls->hasPubNoArgCtor = true;
    }
}

// Check that all operators w/ the given name have a match
void CLSDREC::checkMatchingOperator(PREDEFNAME pn, AGGSYM * cls)
{
    PREDEFNAME opposite = PN_OPFALSE;
    TOKENID oppToken = TID_FALSE;
    switch (pn) {
    case PN_OPEQUALITY: opposite = PN_OPINEQUALITY; oppToken = TID_NOTEQUAL; break;
    case PN_OPINEQUALITY: opposite = PN_OPEQUALITY; oppToken = TID_EQUALEQUAL; break;
    case PN_OPGREATERTHAN: opposite = PN_OPLESSTHAN; oppToken = TID_LESS; break;
    case PN_OPGREATERTHANOREQUAL: opposite = PN_OPLESSTHANOREQUAL; oppToken = TID_LESSEQUAL; break;
    case PN_OPLESSTHAN: opposite = PN_OPGREATERTHAN; oppToken = TID_GREATER; break;
    case PN_OPLESSTHANOREQUAL: opposite = PN_OPGREATERTHANOREQUAL; oppToken = TID_GREATEREQUAL; break;
    case PN_OPTRUE: opposite = PN_OPFALSE; oppToken = TID_FALSE; break;
    case PN_OPFALSE: opposite = PN_OPTRUE; oppToken = TID_TRUE; break;
    default:
        ASSERT(!"bad pn in checkMatchingOperator");
        break;
    }

    NAME * name = compiler()->namemgr->GetPredefName(pn);
    NAME * oppositeName = compiler()->namemgr->GetPredefName(opposite);

    for (METHSYM * original = compiler()->getBSymmgr().LookupAggMember(name, cls, MASK_METHSYM)->asMETHSYM();
        original;
        original = compiler()->getBSymmgr().LookupNextSym(original, cls, MASK_METHSYM)->asMETHSYM()) {

        if (original->isOperator) {
            for (METHSYM * match = compiler()->getBSymmgr().LookupAggMember(oppositeName, cls, MASK_METHSYM)->asMETHSYM();
                match;
                match = compiler()->getBSymmgr().LookupNextSym(match, cls, MASK_METHSYM)->asMETHSYM())
            {
                if (match->isOperator && match->typeVars->size == original->typeVars->size &&
                    compiler()->getBSymmgr().SubstEqualTypes(original->retType, match->retType, (TypeArray *)NULL, original->typeVars) &&
                    compiler()->getBSymmgr().SubstEqualTypeArrays(original->params, match->params, (TypeArray *)NULL, original->typeVars))
                {
                    goto MATCHED;
                }
            }

            PCWSTR operatorName = CParser::GetTokenInfo(oppToken)->pszText;
            ASSERT(operatorName[0]);

            compiler()->ErrorRef(NULL, ERR_OperatorNeedsMatch, original, operatorName);
            return;
        }

MATCHED:;
    }
}

// define all the enumerators in an enum type
void CLSDREC::defineEnumMembers(AGGSYM *cls)
{
    ASSERT(cls->IsEnum());
    // we do NOT need default & copy constructors

    // Enums can not be defined in multiple places, so DeclOnly is OK here.
    AGGDECLSYM * clsdecl = cls->DeclOnly();

    //
    // define members
    //
    MEMBVARSYM *previousEnumerator = NULL;
    for (ENUMMBRNODE * member = clsdecl->parseTree->asAGGREGATE()->pMembers->asENUMMBR(); 
         member;
         member = member->pNext->asENUMMBR()) {

        SETLOCATIONNODE(member);

        NAMENODE *nameNode = member->pName;
        PNAME name = nameNode->pName;

        //
        // check for name same as that of parent aggregate
        // check for conflicting enumerator name
        //
        if (!CheckForBadMemberSimple(name, nameNode, cls)) {
            continue;
        }

        // Check for same names as the reserved "value" enumerators.
        if (name == compiler()->namemgr->GetPredefName(PN_ENUMVALUE)) {
            compiler()->Error(nameNode, ERR_ReservedEnumerator, nameNode);
            continue;
        }

        //
        // create the symbol
        //
        MEMBVARSYM * rval = compiler()->getBSymmgr().CreateMembVar(name, cls, clsdecl);

        rval->type = cls->getThisType();
        rval->SetAccess(ACC_PUBLIC);

        //
        // set link to previous enumerator
        //
        rval->SetPreviousEnumerator(previousEnumerator);
        previousEnumerator = rval;

        // set the value parse tree:
        rval->parseTree = member;
    rval->declaration = clsdecl; 

        //
        // set the flags to indicate an unevaluated constant
        //
        rval->isUnevaled = true;
        rval->isStatic = true; // consts are implicitly static.
    }
}

// define all the members in a delegate type
void CLSDREC::defineDelegateMembers(AGGSYM *cls)
{
    // A delegate can be defined in only one place, so DeclOnly is OK here.
    AGGDECLSYM * clsdecl = cls->DeclOnly();

    //
    // constructor taking an object and an uintptr
    //
    {
        METHSYM *method = compiler()->getBSymmgr().CreateMethod(compiler()->namemgr->GetPredefName(PN_CTOR), cls, clsdecl);
        method->SetAccess(ACC_PUBLIC);
        method->SetMethKind(MethodKind::Ctor);
        method->retType = compiler()->getBSymmgr().GetVoid();
        method->parseTree = clsdecl->parseTree; 
        method->declaration = clsdecl; 

        PTYPESYM paramTypes[2];
        paramTypes[0] = compiler()->GetReqPredefType(PT_OBJECT, false);
        paramTypes[1] = compiler()->GetReqPredefType(PT_INTPTR, false);
        method->params = compiler()->getBSymmgr().AllocParams(2, paramTypes);
        method->typeVars = BSYMMGR::EmptyTypeArray();
    }

    BASENODE * tree = clsdecl->parseTree; 

    // Parser should enforce this.
    ASSERT(!(tree->other & NFEX_METHOD_VARARGS));

    //
    // 'Invoke' method
    //
    METHSYM * invokeMethod;
    TYPESYM * typeRet;
    {
        //
        // bind return type
        //
        DELEGATENODE * delegateNode = clsdecl->parseTree->asDELEGATE(); 
        bool unsafeContext = ((delegateNode->flags & NF_MOD_UNSAFE) || clsdecl->isUnsafe);

        typeRet = TypeBind::BindTypeAggDeclExt(compiler(), delegateNode->pType, clsdecl);
        ASSERT(typeRet);

        if (typeRet->isSpecialByRefType()) {
            compiler()->Error(delegateNode->pType, ERR_MethodReturnCantBeRefAny, typeRet);
        }

        checkUnsafe(delegateNode->pType, typeRet, unsafeContext); 

        // Bind parameter types
        TypeArray * params;
        bool fParams = !!(tree->other & NFEX_METHOD_PARAMS);
        DefineParameters(clsdecl, delegateNode->pParms, unsafeContext, &params, &fParams);

        // Check return and parameter types for correct visibility.
        checkConstituentVisibility(cls, typeRet, ERR_BadVisDelegateReturn);

        for (int i = 0; i < params->size; ++i) {
            checkConstituentVisibility(cls, params->Item(0), ERR_BadVisDelegateParam);
        }

        //
        // create virtual public 'Invoke' method
        //
        METHSYM *method = compiler()->getBSymmgr().CreateMethod(compiler()->namemgr->GetPredefName(PN_INVOKE), cls, clsdecl);
        method->SetAccess(ACC_PUBLIC);
        method->retType = typeRet;
        method->isVirtual = true;
        method->isMetadataVirtual = true;
        method->SetMethKind(MethodKind::Invoke);
        method->parseTree = clsdecl->parseTree;  
        method->params = params;
        method->isParamArray = fParams;
        method->typeVars = BSYMMGR::EmptyTypeArray();

        invokeMethod = method;
    }

    // These two types might not exist on some platforms. If they don't, then don't create a BeginInvoke/EndInvoke.
    PTYPESYM asynchResult = compiler()->GetOptPredefType(PT_IASYNCRESULT, false);
    PTYPESYM asynchCallback = compiler()->GetOptPredefType(PT_ASYNCCBDEL, false);

    if (asynchResult && asynchCallback) {
        // 'BeginInvoke' method
        int maxParams = (int)SizeAdd(invokeMethod->params->size, 2);
        int cParam = 0;
        PTYPESYM * paramTypes = STACK_ALLOC(PTYPESYM, maxParams);

        for (int i = 0; i < invokeMethod->params->size; i++) {
            paramTypes[cParam++] = invokeMethod->params->Item(i);
        }
        paramTypes[cParam++] = asynchCallback;
        paramTypes[cParam++] = compiler()->GetReqPredefType(PT_OBJECT, false);
        ASSERT(cParam <= maxParams);

        // Create virtual public 'BeginInvoke' method
        METHSYM *method = compiler()->getBSymmgr().CreateMethod(compiler()->namemgr->GetPredefName(PN_BEGININVOKE), cls, clsdecl);
        method->SetAccess(ACC_PUBLIC);
        method->retType = asynchResult;
        method->isVirtual = true;
        method->parseTree = clsdecl->parseTree; 
        method->params = compiler()->getBSymmgr().AllocParams(cParam, paramTypes);
        method->typeVars = BSYMMGR::EmptyTypeArray();

        // 'EndInvoke' method
        cParam = 0;

        for (int i = 0; i < invokeMethod->params->size; i++) {
            if (invokeMethod->params->Item(i)->isPARAMMODSYM()) {
                paramTypes[cParam++] = invokeMethod->params->Item(i);
            }
        }
        paramTypes[cParam++] = asynchResult;
        ASSERT(cParam <= maxParams);

        // Create virtual public 'EndInvoke' method
        method = compiler()->getBSymmgr().CreateMethod(compiler()->namemgr->GetPredefName(PN_ENDINVOKE), cls, clsdecl);
        method->SetAccess(ACC_PUBLIC);
        method->retType = typeRet;
        method->isVirtual = true;
        method->parseTree = clsdecl->parseTree; 
        method->params = compiler()->getBSymmgr().AllocParams(cParam, paramTypes);
        method->typeVars = BSYMMGR::EmptyTypeArray();
    }
}

// declare all the types in an input file
void CLSDREC::declareInputfile(NAMESPACENODE * parseTree, PINFILESYM inputfile)
{
    ASSERT(parseTree && inputfile->isSource && (!inputfile->rootDeclaration || !inputfile->rootDeclaration->parseTree));

    SETLOCATIONFILE(inputfile);
    SETLOCATIONNODE(parseTree);

    //
    // create the new delcaration
    //
    inputfile->rootDeclaration = compiler()->getBSymmgr().CreateNamespaceDecl(
                compiler()->getBSymmgr().GetRootNS(), 
                NULL,                               // no declaration parent
                inputfile, 
                parseTree);

    //
    // declare everything in the declaration
    //
    declareNamespace(inputfile->rootDeclaration);
}

// declare a namespace by entering it into the symbol table, and recording it in the
// parsetree...  nspace is the parent namespace.
void CLSDREC::declareNamespace(PNSDECLSYM declaration)
{
    //
    // declare members
    // 
    NODELOOP(declaration->parseTree->pElements, BASE, elem)

        SETLOCATIONNODE(elem);

        switch (elem->kind) {
        case NK_NAMESPACE:
        {
            NAMESPACENODE * nsnode = elem->asNAMESPACE();
            NSDECLSYM *newDeclaration;

            // we can not be the global namespace
            ASSERT(nsnode->pName);

            BASENODE *namenode = nsnode->pName;
            // is our name simple, or a qualified one...
            if (namenode->kind == NK_NAME) {
                newDeclaration = addNamespaceDeclaration(namenode->asNAME(), nsnode, declaration);
            } else {

                //
                // get the first component of the dotted name
                //
                BASENODE * first = namenode;
                while (namenode->asDOT()->p1->kind == NK_DOT) {
                    namenode = namenode->asDOT()->p1;
                }

                //
                // add all the namespaces in the dotted name
                //
                newDeclaration = addNamespaceDeclaration(namenode->asDOT()->p1->asNAME(), nsnode, declaration);
                while (newDeclaration) {
                    //
                    // don't add using clauses for the declaration to any but the last 
                    // namespace declaration in the list.
                    //
                    newDeclaration->usingClausesResolved = true;

                    ASSERT(namenode->kind == NK_DOT);
                    newDeclaration = addNamespaceDeclaration(namenode->asDOT()->p2->asNAME(), nsnode, newDeclaration);
                    if (!newDeclaration) {
                        break;
                    }
                    if (namenode == first) {
                        break;
                    }
                    namenode = namenode->pParent;
                }
            }
            if (newDeclaration) {
                declareNamespace(newDeclaration);
            }
            break;
        }
        case NK_CLASS:
        case NK_STRUCT:
        case NK_INTERFACE:
        case NK_ENUM:
            declareAggregate(elem->asAGGREGATE(), declaration);
            break;

        case NK_DELEGATE:
        {
            DELEGATENODE *delegateNode = elem->asDELEGATE();
            AddAggregate(delegateNode, delegateNode->pName, declaration);
            break;
        }

        default:
            ASSERT(!"unknown namespace member");
            break;
        }
    ENDLOOP;
}

/*
 * creates a global attribute symbol  for attributes on modules and assemblies
 */
void CLSDREC::declareGlobalAttribute(ATTRDECLNODE *pNode, NSDECLSYM *declaration)
{
    PGLOBALATTRSYM *attributeLocation;
    CorAttributeTargets elementKind;

    // check the global attribute's name
    if (pNode->location == AL_ASSEMBLY) {
        attributeLocation = &compiler()->assemblyAttributes;
        elementKind = catAssembly;
    } else if (pNode->location == AL_MODULE) {
        attributeLocation = &declaration->getInputFile()->getOutputFile()->attributes;
        elementKind = catModule;
    } else {
        ASSERT(pNode->location == AL_UNKNOWN);
        attributeLocation = &compiler()->unknownGlobalAttributes;
        elementKind = (CorAttributeTargets) 0;
    }

    declaration->getInputFile()->hasGlobalAttr = true;

    // create the attribute
    GLOBALATTRSYM *attr = compiler()->getCSymmgr().CreateGlobalAttribute(pNode->pNameNode->pName, declaration);
    attr->parseTree = pNode;
    attr->elementKind = elementKind;
    attr->nextAttr = *attributeLocation;

    *attributeLocation = attr;
}



/*
 * Once the "prepare" stage is done, classes must be compiled and metadata emitted
 * in three distinct stages in order to make sure that the metadata emitting is done
 * most efficiently. The stages are:
 *   EmitTypeDefs -- typedefs must be created for each aggregate types
 *   EmitMemberDefs -- memberdefs must be created for each member of aggregate types
 *   Compile -- Compile method bodies.
 * 
 * To conform to the most efficient metadata emitting scheme, each of the three stages
 * must be done in the exact same order. Furthermore, the first stage must be done
 * in an order that does base classes and interfaces before derived classes and interfaces --
 * e.g., a topological sort of the classes. So every stage must go in this same topological
 * order.
 */

// Emit typedefs for all aggregates in this namespace declaration...  
void CLSDREC::emitTypedefsNamespace(NSDECLSYM *nsDeclaration)
{
    SETLOCATIONSYM(nsDeclaration);

    //
    // emit typedefs for each aggregate type.
    //
    FOREACHCHILD(nsDeclaration, elem)
        switch (elem->getKind()) {
        case SK_NSDECLSYM:
            emitTypedefsNamespace(elem->asNSDECLSYM());
            break;
        case SK_AGGDECLSYM:
            if (elem->asAGGDECLSYM()->IsFirst())
                emitTypedefsAggregate(elem->asAGGDECLSYM()->Agg());
            break;
        case SK_GLOBALATTRSYM:
            break;
        default:
            ASSERT(!"Unknown type");
        }
    ENDFOREACHCHILD
}


// Emit typedefs for this aggregates 
void CLSDREC::emitTypedefsAggregate(AGGSYM * cls)
{
    OUTFILESYM * outputFile;
    AGGSYM * base;

    SETLOCATIONSYM(cls);

    // If we've already hit this one (because it was a base of someone earlier),
    // then nothing more to do.
    if (cls->isTypeDefEmitted) {
        ASSERT(compiler()->options.m_fNOCODEGEN || cls->tokenEmit);
        return;             // already did it.
    }

    ASSERT(!cls->tokenEmit);

    // Do base classes and base interfaces, if they are in the same output scope.
    outputFile = cls->getOutputFile();

    // Do the base class 
    base = cls->GetBaseAgg();
    if (base && base->getOutputFile() == outputFile)
        emitTypedefsAggregate(base);

    // Iterate the base interfaces.
    for (int i = 0; i < cls->ifaces->size; i++) {
        AGGTYPESYM * baseIface = cls->ifaces->Item(i)->asAGGTYPESYM();
        base = baseIface->getAggregate();
        if (base->getOutputFile() == outputFile)
            emitTypedefsAggregate(base);
    }

    // we need to do outer classes before we do the nested classes
    if (cls->Parent()->isAGGSYM() && !cls->Parent()->asAGGSYM()->isTypeDefEmitted &&
        cls->Parent()->asAGGSYM()->isFabricated == cls->isFabricated) {
        emitTypedefsAggregate(cls->Parent()->asAGGSYM());
        ASSERT(cls->isTypeDefEmitted);
        return;
    }

    // It's possible that in doing our base classes or interfaces
    // They emitted our outer class, which in turn caused us to get
    // emitted, and so we're done
    if (cls->isTypeDefEmitted) {
        ASSERT(cls->Parent()->isAGGSYM() && cls->Parent()->asAGGSYM()->isTypeDefEmitted);
        return;
    }

    // Do this aggregate.
    if (!compiler()->options.m_fNOCODEGEN) {
        compiler()->emitter.EmitAggregateDef(cls);
    }
    cls->isTypeDefEmitted = true;

    // Do child aggregates.
    FOREACHCHILD(cls, child)
        if (child->isAGGSYM()) {
            emitTypedefsAggregate(child->asAGGSYM());
        }
    ENDFOREACHCHILD
    ASSERT(compiler()->options.m_fNOCODEGEN || cls->tokenEmit || compiler()->ErrorCount());
}




// Emit memberdefs for all aggregates in this namespace...  
void CLSDREC::emitMemberdefsNamespace(NSDECLSYM *nsDeclaration)
{
    SETLOCATIONSYM(nsDeclaration);

    //
    // emit memberdefs for each aggregate type.
    //
    FOREACHCHILD(nsDeclaration, elem)
        switch (elem->getKind()) {
        case SK_NSDECLSYM:
            emitMemberdefsNamespace(elem->asNSDECLSYM());
            break;
        case SK_AGGDECLSYM:
            if (elem->asAGGDECLSYM()->IsFirst())
                emitMemberdefsAggregate(elem->asAGGDECLSYM()->Agg());
            break;
        case SK_GLOBALATTRSYM:
            break;
        default:
            ASSERT(!"Unknown type");
        }
    ENDFOREACHCHILD
}

// Emit typerefs/defs/specs for the inheritance hierarchy.
// Make sure we traverse in the same order as above, just in case.
//
// This is structured as a seperate phase because there are corner cases where
// recursion between the base class and the current class is allowed, 
// e.g. class C : IList<C> { ... }
//
void CLSDREC::emitBasesNamespace(NSDECLSYM *nsDeclaration)
{
    SETLOCATIONSYM(nsDeclaration);

    //
    // emit memberdefs for each aggregate type.
    //
    FOREACHCHILD(nsDeclaration, elem)
        switch (elem->getKind()) {
        case SK_NSDECLSYM:
            emitBasesNamespace(elem->asNSDECLSYM());
            break;
        case SK_AGGDECLSYM:
            if (elem->asAGGDECLSYM()->IsFirst())
                emitBasesAggregate (elem->asAGGDECLSYM()->Agg());
            break;
        case SK_GLOBALATTRSYM:
            break;
        default:
            ASSERT(!"Unknown type");
        }
    ENDFOREACHCHILD
}


// Emit bases for this aggregates 
void CLSDREC::emitBasesAggregate(AGGSYM * cls)
{
    OUTFILESYM * outputFile;
    AGGSYM * base;

    // If we've already hit this one (because it was a base of someone earlier),
    // then nothing more to do.
    if (cls->isBasesEmitted) return;

    SETLOCATIONSYM(cls);

    // the class should already have a token generated for it...
    ASSERT(compiler()->options.m_fNOCODEGEN || cls->tokenEmit);

    // Do base classes and base interfaces, if they are in the same output scope.
    outputFile = cls->getOutputFile();

    // Do the base class 
    base = cls->GetBaseAgg();
    if (base && base->getOutputFile() == outputFile)
        emitBasesAggregate(base);

    // Iterate the base interfaces.
    for (int i = 0; i < cls->ifaces->size; i++) {
        AGGTYPESYM * baseIface = cls->ifaces->Item(i)->asAGGTYPESYM();
        base = baseIface->getAggregate();
        if (base->getOutputFile() == outputFile)
            emitBasesAggregate(base);
    }

    // we need to do outer classes before we do the nested classes
    if (cls->Parent()->isAGGSYM() && !cls->Parent()->asAGGSYM()->isBasesEmitted &&
        cls->Parent()->asAGGSYM()->isFabricated == cls->isFabricated) {
        emitBasesAggregate(cls->Parent()->asAGGSYM());
        ASSERT(cls->isBasesEmitted);
        return;
    }

    if (cls->isBasesEmitted)
        return;

    // Do this aggregate.
    if (!compiler()->options.m_fNOCODEGEN)
        compiler()->emitter.EmitAggregateBases(cls);
    cls->isBasesEmitted = true;

    // Do child aggregates.
    FOREACHCHILD(cls, child)
        if (child->isAGGSYM()) {
            emitBasesAggregate(child->asAGGSYM());
        }
    ENDFOREACHCHILD
}



void CLSDREC::EnumMembersInEmitOrder(AGGSYM *cls, VOID *info, MEMBER_OPERATION doMember)
{
    // Constant fields go first
    FOREACHCHILD(cls, child)
        if (child->isMEMBVARSYM() && child->asMEMBVARSYM()->isConst)
        {
            (this->*doMember)(child, info);
        }
    ENDFOREACHCHILD

    // Emit the other children.
    FOREACHCHILD(cls, child)

        SETLOCATIONSYM(child);

        switch (child->getKind()) {
        case SK_MEMBVARSYM:
            if (!child->asMEMBVARSYM()->isConst) {
                (this->*doMember)(child, info);
            }
            break;
        case SK_PROPSYM:
            {
                PROPSYM *property = child->asPROPSYM();
                if (property->methSet && property->methGet)

                {
                    ASSERT(property->methSet->parseTree->kind == NK_ACCESSOR || property->methGet->parseTree->kind == NK_ACCESSOR);

                    // emit accessors in declared order
                    if (property->methSet->parseTree->kind != NK_ACCESSOR ||
                        (property->methGet->parseTree->kind == NK_ACCESSOR &&
                         property->methGet->parseTree->asACCESSOR()->tokidx < property->methSet->parseTree->asACCESSOR()->tokidx))
                    {
                        (this->*doMember)(property->methGet, info);
                        (this->*doMember)(property->methSet, info);
                    }
                    else 
                    {
                        (this->*doMember)(property->methSet, info);
                        (this->*doMember)(property->methGet, info);
                    }
                } 
                else if (property->methGet) 
                {
                    (this->*doMember)(property->methGet, info);
                } 
                else if (property->methSet) 
                {
                    (this->*doMember)(property->methSet, info);
                }
            }
            break;
        case SK_METHSYM:
            // accessors are done off of the property
            if (!child->asMETHSYM()->isAnyAccessor())
            {
                (this->*doMember)(child, info);
            }
            break;
        case SK_EVENTSYM:
            {
                // Emit accessors in declaration order. Note that if accessors weren't specified they
                // have the same parse tree and we emit Add first.
                EVENTSYM *event = child->asEVENTSYM();
                if (event->methRemove->parseTree->tokidx < event->methAdd->parseTree->tokidx) {
                    (this->*doMember)(event->methRemove, info);
                    (this->*doMember)(event->methAdd, info);
                }
                else {
                    (this->*doMember)(event->methAdd, info);
                    (this->*doMember)(event->methRemove, info);
                }
            }
            break;
        case SK_TYVARSYM: 
        case SK_AGGTYPESYM: 
            break;
        default:
            break;
        }
    ENDFOREACHCHILD

    // Properties/events must be done after methods.
    FOREACHCHILD(cls, child)
        if (child->isPROPSYM() || child->isEVENTSYM()) 
        {
            (this->*doMember)(child, info);
        }
    ENDFOREACHCHILD
}



void CLSDREC::EmitMemberdef(SYM *member, VOID *unused)
{
    SETLOCATIONSYM(member);
    switch (member->getKind())
    {
    case SK_MEMBVARSYM:
        compiler()->emitter.EmitMembVarDef  (member->asMEMBVARSYM());
        break;
    case SK_METHSYM: {
        METHSYM *meth = member->asMETHSYM();
        if (!(meth->isStatic && meth->isCompilerGeneratedCtor()))
            compiler()->emitter.EmitMethodDef (member->asMETHSYM());
        break;
    }
    case SK_PROPSYM:
        compiler()->emitter.EmitPropertyDef (member->asPROPSYM());
        break;
    case SK_EVENTSYM:
        compiler()->emitter.EmitEventDef    (member->asEVENTSYM());
        break;

    default:
        ASSERT(!"Invalid member sym");
    }
}

// Emit memberdefs for this aggregate 
//
// The ordering of items here must EXACTLY match that in compileAggregate.
void CLSDREC::emitMemberdefsAggregate(AGGSYM * cls)
{
    OUTFILESYM * outputFile;
    AGGSYM * base;

    SETLOCATIONSYM(cls);

    // If we've already hit this one (because it was a base of someone earlier),
    // then nothing more to do.
    if (cls->isMemberDefsEmitted) {
        return;             // already did it.
    }

    // If we couldn't emit the class don't try to emit the members
    // just exit.
    if (TypeFromToken(cls->tokenEmit) != mdtTypeDef || cls->tokenEmit == mdTypeDefNil) {
        return;
    }

    // Do base classes and base interfaces, if they are in the same output scope.
    outputFile = cls->getOutputFile();

    // Do the base class
    base = cls->GetBaseAgg();
    if (base && base->getOutputFile() == outputFile)
        emitMemberdefsAggregate(base);

    // Iterate the base interfaces.
    for (int i = 0; i < cls->ifaces->size; i++) {
        AGGTYPESYM * baseIface = cls->ifaces->Item(i)->asAGGTYPESYM();
        base = baseIface->getAggregate();
        if (base->getOutputFile() == outputFile)
            emitMemberdefsAggregate(base);
    }

    // To do this in the same order as the Aggregate defs
    // we need to do outer classes before we do the nested classes
    if (cls->Parent()->isAGGSYM() && !cls->Parent()->asAGGSYM()->isMemberDefsEmitted) {
        emitMemberdefsAggregate(cls->Parent()->asAGGSYM());
        ASSERT(cls->isMemberDefsEmitted);
        return;
    }

    if (cls->isMemberDefsEmitted)
        return;

    // Do any special fields of this aggregate.
    compiler()->emitter.EmitAggregateSpecialFields(cls);

    ASSERT(!cls->isMemberDefsEmitted);

    
    // Emit the children.
    EnumMembersInEmitOrder(cls, 0, &CLSDREC::EmitMemberdef);
    ASSERT(!cls->isMemberDefsEmitted);
    cls->isMemberDefsEmitted = true;

    // Do child aggregates.
    FOREACHCHILD(cls, child)
        if (child->isAGGSYM()) {
            emitMemberdefsAggregate(child->asAGGSYM());
        }
    ENDFOREACHCHILD
}

// Compile all members of this namespace...  nspace is this namespace...
void CLSDREC::compileNamespace(NSDECLSYM *nsDeclaration)
{
    SETLOCATIONSYM(nsDeclaration);

    //
    // compile members
    //
    FOREACHCHILD(nsDeclaration, elem)
        switch (elem->getKind()) {
        case SK_NSDECLSYM:
            compileNamespace(elem->asNSDECLSYM());
            break;
        case SK_AGGDECLSYM:
            ASSERT(elem->asAGGDECLSYM()->Agg()->AggKind() > AggKind::Unknown && elem->asAGGDECLSYM()->Agg()->AggKind() < AggKind::Lim);
            if (!elem->asAGGDECLSYM()->IsFirst())
                break;
            if (compiler()->options.m_fCompileSkeleton)
                CompileAggSkeleton(elem->asAGGDECLSYM()->Agg());
            else
                compileAggregate(elem->asAGGDECLSYM()->Agg());
            compiler()->funcBRec.ResetUniqueNames();
            break;
        case SK_GLOBALATTRSYM:
            break;
        default:
            ASSERT(!"Unknown type");
        }
    ENDFOREACHCHILD

    // Do CLS name checking, after compiling members so we know who is Compliant
    ASSERT(compiler()->AllowCLSErrors() || !compiler()->CheckForCLS());
    if (compiler()->CheckForCLS() &&
        nsDeclaration->hasExternalAccess() && !nsDeclaration->NameSpace()->checkedForCLS)
    {
        checkCLSnaming(nsDeclaration->NameSpace());
    }
}


// Compile the skeleton for a class. Basically we need all predefined attributes of all members.
void CLSDREC::CompileAggSkeleton(AGGSYM * agg)
{
    ASSERT(compiler()->options.m_fCompileSkeleton);
    ASSERT(agg->isSource);

    // If we've already hit this one (because it was a base of someone earlier),
    // then nothing more to do.
    if (agg->IsCompiled() || agg->isFabricated) {
        // Already did it (or shouldn't ever do it).
        return;
    }

    ASSERT(agg->hasParseTree);

    // If we couldn't emit the class don't try to compile it (unless codegen is off, of course, in which case
    // we won't have a token even if everything was good).
    if (!compiler()->options.m_fNOCODEGEN && (TypeFromToken(agg->tokenEmit) != mdtTypeDef || agg->tokenEmit == mdTypeDefNil)) {
        return;
    }

    SETLOCATIONSYM(agg);

    // Do base classes and base interfaces, if they are in the same output scope.
    OUTFILESYM * outfile = agg->getOutputFile();

    // Do the base class
    AGGSYM * aggBase = agg->GetBaseAgg();
    if (aggBase && aggBase->getOutputFile() == outfile)
        CompileAggSkeleton(aggBase);

    // Iterate the base interfaces.
    for (int i = 0; i < agg->ifaces->size; i++) {
        AGGSYM * ifaceBase = agg->ifaces->Item(i)->asAGGTYPESYM()->getAggregate();
        if (ifaceBase->getOutputFile() == outfile)
            CompileAggSkeleton(ifaceBase);
    }

    // Do outer classes before nested classes.
    if (agg->isNested() && !agg->GetOuterAgg()->IsCompiled())
        CompileAggSkeleton(agg->GetOuterAgg());

    if (agg->IsCompiled())
        return;

    AGGINFO info;
    MEM_SET_ZERO(info);

    AggAttrBind::Compile(compiler(), agg, &info);

    EnumMembersInEmitOrder(agg, &info, (MEMBER_OPERATION) &CLSDREC::CompileMemberSkeleton);

    ASSERT(AggState::Prepared <= agg->AggState() && agg->AggState() < AggState::Compiled);
    agg->SetAggState(AggState::Compiled);

    // Nested classes must be done after our state is AggState::Compiled.
    FOREACHCHILD(agg, child)
        if (child->isAGGSYM())
            CompileAggSkeleton(child->asAGGSYM());
    ENDFOREACHCHILD
}


void CLSDREC::CompileMemberSkeleton(SYM * sym, AGGINFO * pai)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::CompileMembers);

    SETLOCATIONSYM(sym);
    switch (sym->getKind())
    {
    case SK_MEMBVARSYM:
        {
            MEMBVARINFO info;
            MEM_SET_ZERO(info);
            FieldAttrBind::Compile(compiler(), sym->asMEMBVARSYM(), &info, pai);
        }
        break;

    case SK_METHSYM:
        {
            METHSYM * meth = sym->asMETHSYM();
            ASSERT(!meth->tokenImport);

            METHINFO * info = (METHINFO *)STACK_ALLOC_ZERO(byte, METHINFO::Size(meth->params->size));
            FillMethInfoCommon(meth, info, pai, true);

            SETLOCATIONSTAGE(COMPILE);
            if (!meth->getClass()->IsDelegate())
                MethAttrBind::CompileAndEmit(compiler(), meth, info->debuggerHidden);
            ParamAttrBind::CompileParamList(compiler(), info);

        }
        break;

    case SK_PROPSYM:
        PropAttrBind::Compile(compiler(), sym->asPROPSYM());
        break;

    case SK_EVENTSYM:
        DefaultAttrBind::CompileAndEmit(compiler(), sym);
        break;

    default:
        VSFAIL("Invalid member sym");
        break;
    }
}

// compile all memebers of a class, this emits the class md, and loops through its
// children...
//
// The ordering of items here must EXACTLY match that in EmitMemberDefsAggregate.
void CLSDREC::compileAggregate(AGGSYM * cls)
{
    ASSERT(cls->isSource);

    SETLOCATIONSYM(cls);

    // If we've already hit this one (because it was a base of someone earlier),
    // then nothing more to do.
    if (cls->IsCompiled() || cls->isFabricated) {
        mdToken tokEmit = cls->getTokenEmit();
        // Fabricated types need to go through attrbind to get the CompilerGeneratedAttribute emitted.
        ASSERT(tokEmit || compiler()->ErrorCount() > 0);  // we may not have an emit token if there were errors
        if (cls->isFabricated && tokEmit)
            CompilerGeneratedAttrBind::EmitAttribute(compiler(), tokEmit);
        if (cls->isFixedBufferStruct && tokEmit)
            UnsafeValueTypeAttrBind::EmitAttribute(compiler(), tokEmit);

        // Already did it (or shouldn't ever do it).
        return;
    }

    ASSERT(cls->hasParseTree);

    // If we couldn't emit the class don't try to compile it (unless codegen is off, of course, in which case
    // we won't have a token even if everything was good).
    if (!compiler()->options.m_fNOCODEGEN && (TypeFromToken(cls->tokenEmit) != mdtTypeDef || cls->tokenEmit == mdTypeDefNil)) {
        return;
    }


    // Do base classes and base interfaces, if they are in the same output scope.
    OUTFILESYM * outputFile = cls->getOutputFile();
    AGGSYM * base;

    // Do the base class
    base = cls->GetBaseAgg();
    if (base && base->getOutputFile() == outputFile)
        compileAggregate(base);

    // Iterate the base interfaces.
    for (int i = 0; i < cls->ifaces->size; i++) {
        AGGTYPESYM * baseIface = cls->ifaces->Item(i)->asAGGTYPESYM();
        base = baseIface->getAggregate();
        if (base->getOutputFile() == outputFile)
            compileAggregate(base);
    }

    // Do outer classes before nested classes
    if (cls->isNested() && !cls->GetOuterAgg()->IsCompiled())
        compileAggregate(cls->GetOuterAgg());

    if (cls->IsCompiled())
        return;

    AGGINFO info;
    MEM_SET_ZERO(info);

    AggAttrBind::Compile(compiler(), cls, &info);

    //
    // compile & emit members
    //
    EnumMembersInEmitOrder(cls, &info, (MEMBER_OPERATION) &CLSDREC::CompileMember);

    ASSERT(AggState::Prepared <= cls->AggState() && cls->AggState() < AggState::Compiled);
    cls->SetAggState(AggState::Compiled);

    // Nested classes must be done after other members.
    FOREACHCHILD(cls, child)
        if (child->isAGGSYM()) {
            SETLOCATIONSYM(child);
            compileAggregate(child->asAGGSYM());
        }
    ENDFOREACHCHILD

    // Do CLS name checking, after compiling members so we know who is Compliant
    if (compiler()->CheckForCLS() && compiler()->CheckSymForCLS(cls, false) && cls->hasExternalAccess()) {
        ASSERT(compiler()->AllowCLSErrors());
        checkCLSnaming(cls);

        if (cls->IsInterface())
        {
            //EDMAURER Enforce rule that all CLS compliant interfaces
            //derive only from CLS compliant interfaces. 
            for (int i = 0; i < cls->ifaces->size; i++) {
                AGGTYPESYM * base = cls->ifaces->Item(i)->asAGGTYPESYM();
                ASSERT(base->isInterfaceType());

                if (!compiler ()->isCLS_Type (cls, base))
                    compiler ()->Error (cls->GetSomeParseTree (), WRN_CLS_BadInterface, cls, base);
            }
        }

        if (cls->isAttribute)
        {
            bool foundCLSCtor = false;
            FOREACHCHILD(cls, child)
                if (child->isMETHSYM() && child->name == compiler()->namemgr->GetPredefName(PN_CTOR)
                    && child->hasExternalAccess() && (!child->hasCLSattribute || child->isCLS))
                {
                    METHPROPSYM *member = child->asMETHPROPSYM();
                    int i;
                    for (i = 0; i < member->params->size; i += 1)
                    {
                        if (member->params->Item(i)->isARRAYSYM() || !isAttributeType(member->params->Item(i)))
                        {
                            break;
                        }
                    }
                    if (i == member->params->size) {
                        foundCLSCtor = true;
                        break;
                    }
                }
            ENDFOREACHCHILD
            if (!foundCLSCtor) {
                compiler()->ErrorRef(NULL, WRN_CLS_BadAttributeType, cls);
            }
        }
    }

    compiler()->funcBRec.resetUnsafe();
}

void CLSDREC::CompileMember(SYM *member, AGGINFO *info)
{
    SETLOCATIONSYM(member);
    switch (member->getKind())
    {
    case SK_MEMBVARSYM:
        compileField    (member->asMEMBVARSYM(),info);
        break;
    case SK_METHSYM:
        compileMethod   (member->asMETHSYM(),   info);
        compiler()->DiscardLocalState();
        break;
    case SK_PROPSYM:
        compileProperty (member->asPROPSYM(),   info);
        break;
    case SK_EVENTSYM:
        compileEvent    (member->asEVENTSYM(),  info);
        break;

    default:
        ASSERT(!"Invalid member sym");
    }
}

// Compile all decls in a field node... all this does is emit the md...
void CLSDREC::compileField(MEMBVARSYM * field, AGGINFO * aggInfo)
{
    ASSERT(field);

    MEMBVARINFO info;
    MEM_SET_ZERO(info);
    FieldAttrBind::Compile(compiler(), field, &info, aggInfo);
    if (compiler()->CheckForCLS() && compiler()->CheckSymForCLS(field, false) && field->hasExternalAccess()) {
        if (!compiler()->isCLS_Type(field->getClass(), field->type))
            compiler()->ErrorRef(NULL, WRN_CLS_BadFieldPropType, field);
        if (field->isVolatile)
            compiler()->ErrorRef(NULL, WRN_CLS_VolatileField, field);
    }

    if (field->fixedAgg) {
        int Length = max(field->constVal.iVal, 1); // Previous errors might have set this to 0
        int size = compiler()->getBSymmgr().GetAttrArgSize(
            field->type->asPTRSYM()->baseType()->getPredefType());
        ASSERT(size == 1 || size == 2 || size == 4 || size == 8);

        // Check for overflow
        if (Length > (INT_MAX / size)) {
            // dwLength * size would be greater than what metadata can handle
            compiler()->Error(field->getConstExprTree(), ERR_FixedOverflow, Length, field->type->asPTRSYM()->baseType());
        } else {
            //Set the layout and the class
            DWORD dwLength = (DWORD)(Length * size);
            ASSERT(dwLength <= LONG_MAX);
            HRESULT hr = compiler()->curFile->GetEmit()->SetClassLayout( field->fixedAgg->tokenEmit, 0, NULL, dwLength);
            if (FAILED(hr)) {
                compiler()->Error(NULL, FTL_MetadataEmitFailure,
                    compiler()->ErrHR(hr), field->getInputFile()->getOutputFile(), ErrArgRefOnly(field));
            }
        }
    }

}


void CLSDREC::FillMethInfoCommon(METHSYM * meth, METHINFO * pinfo, AGGINFO * pai, bool fNoErrors)
{
    ASSERT(meth && pinfo);
    ASSERT(meth->parseTree);

    pinfo->meth = meth;

    if (meth->isExternal && meth->isCtor() && pai->isComimport) {
        ASSERT(meth->getClass()->IsClass() && !meth->isStatic && meth->isSysNative);
        pinfo->isMagicImpl = true;
    }

    BASENODE * nodeMeth = meth->parseTree;
    BASENODE * nodeParams = NULL;
    bool fSkipParams = false;

    switch (nodeMeth->kind) {
    case NK_METHOD:
    case NK_CTOR:
    case NK_OPERATOR:
    case NK_DTOR:
        nodeParams = nodeMeth->asANYMETHOD()->pParms;
        pinfo->nodeAttr = nodeMeth->asANYMETHOD()->pAttr;
        break;

    case NK_ACCESSOR:
        ASSERT(meth->isAnyAccessor());
        if (nodeMeth->pParent->InGroup(NG_PROPERTY)) {
            nodeParams = nodeMeth->pParent->asANYPROPERTY()->pParms;
            pinfo->nodeAttr = meth->getParseTree()->asACCESSOR()->pAttr;
        }
        break;

    // Compiler fabricated accessors
    case NK_VARDECL:  // generated for field-style event declarations
    case NK_PROPERTY: // generated for sealed property overrides that only implement one accessor
    case NK_INDEXER:
        ASSERT(meth->isAnyAccessor());
        if (nodeMeth->InGroup(NG_PROPERTY)) {
            nodeParams = nodeMeth->asANYPROPERTY()->pParms;
            pinfo->nodeAttr = nodeMeth->asANYPROPERTY()->pAttr;
        }
        break;

    case NK_DELEGATE:
        if (meth->name == compiler()->namemgr->GetPredefName(PN_INVOKE) ||
            meth->name == compiler()->namemgr->GetPredefName(PN_BEGININVOKE) ||
            meth->name == compiler()->namemgr->GetPredefName(PN_ENDINVOKE))
        {
            nodeParams = meth->getClass()->DeclOnly()->getParseTree()->asDELEGATE()->pParms;
            if (meth->name == compiler()->namemgr->GetPredefName(PN_ENDINVOKE))
                fSkipParams = true;
            if (meth->name != compiler()->namemgr->GetPredefName(PN_BEGININVOKE))
                pinfo->nodeAttr = meth->getClass()->DeclOnly()->getParseTree()->asDELEGATE()->pAttr;
        }
        break;

    case NK_STRUCT:
    case NK_CLASS:
        ASSERT(meth->isCtor() || meth->isIfaceImpl);
        break;

    default:
        VSFAIL("Bad parse tree kind in FillMethInfoCommon");
        break;
    }

    int ipin = 0;
    PARAMINFO * rgpin = pinfo->rgpin;

    if (!fSkipParams) {
        NODELOOP(nodeParams, PARAMETER, nodeParam)
            ASSERT(ipin < meth->params->size);
            rgpin[ipin].node = nodeParam;
            rgpin[ipin].nodeAttr = nodeParam->pAttr;
            rgpin[ipin++].SetName(nodeParam->pName->pName);
        ENDLOOP;
    }
    ASSERT(ipin <= meth->params->size);

    if (meth->isParamArray) {
        ASSERT(ipin > 0);
        rgpin[ipin - 1].isParamArray = true;
    }

    if (meth->isIfaceImpl) {
        ASSERT(ipin == 0);
    }
    else if (meth->isAnyAccessor()) {
        ASSERT(!meth->isEventAccessor() || meth->params->size == 1);
        if (meth->isEventAccessor() || !meth->isGetAccessor()) {
            ASSERT(ipin == meth->params->size - 1);
            ASSERT(!rgpin[ipin].node);
            rgpin[ipin].nodeAttr = pinfo->nodeAttr;
            rgpin[ipin++].SetName(compiler()->namemgr->GetPredefName(PN_VALUE));
        }
        if (nodeMeth->kind != NK_ACCESSOR) {
            // This is an auto-generated event accessor, not a user-defined one.
            if (!meth->getClass()->IsStruct() && !meth->isExternal)
                pinfo->isSynchronized = true; // synchronize this method if not on a struct.
            pinfo->noDebugInfo = true;   // Don't generate debug info for this method, since there is no source code.
        }
    }
    else if (meth->getClass()->IsDelegate()) {
        // all delegate members are implemented by the runtime
        pinfo->isMagicImpl = true;

        if (meth->isCtor()) {
            ASSERT(ipin == 0);
            ASSERT(!rgpin[ipin].node && !rgpin[ipin].nodeAttr);
            rgpin[ipin++].SetName(compiler()->namemgr->GetPredefName(PN_DELEGATECTORPARAM0));
            ASSERT(!rgpin[ipin].node && !rgpin[ipin].nodeAttr);
            rgpin[ipin++].SetName(compiler()->namemgr->GetPredefName(PN_DELEGATECTORPARAM1));
        }
        else if (meth->name == compiler()->namemgr->GetPredefName(PN_INVOKE)) {
            // Make sure there aren't any duplicates.
            if (!fNoErrors) {
                for (int i = 1; i < ipin; i++) {
                    NAME * name = rgpin[i].Name();
                    for (int j = 0; j < i; j++) {
                        if (name == rgpin[j].Name()) {
                            compiler()->Error(rgpin[i].node->pName, ERR_DuplicateParamName, name);
                            goto LDoneChecking;
                        }
                    }
                }
LDoneChecking: ;
            }
        }
        else if (meth->name == compiler()->namemgr->GetPredefName(PN_BEGININVOKE)) {
            ASSERT(ipin == meth->params->size - 2);
            ASSERT(!rgpin[ipin].node && !rgpin[ipin].nodeAttr);
            rgpin[ipin++].SetName(compiler()->namemgr->GetPredefName(PN_DELEGATEBIPARAM0));
            ASSERT(!rgpin[ipin].node && !rgpin[ipin].nodeAttr);
            rgpin[ipin++].SetName(compiler()->namemgr->GetPredefName(PN_DELEGATEBIPARAM1));
        }
        else if (meth->name == compiler()->namemgr->GetPredefName(PN_ENDINVOKE)) {
            ASSERT(ipin == 0 && fSkipParams);

            // we need the type of the invoke method:
            METHSYM * invoke = compiler()->getBSymmgr().LookupInvokeMeth(meth->getClass());

            int j = 0;
            bool fResultParam = false;
            NODELOOP(nodeParams, PARAMETER, nodeParam)
                if (invoke->params->Item(j)->isPARAMMODSYM()) {
                    ASSERT(ipin < meth->params->size - 1);
                    NAME * name = nodeParam->pName->pName;
                    if (name == compiler()->namemgr->GetPredefName(PN_DELEGATEEIPARAM0))
                        fResultParam = true;
                    rgpin[ipin].node = nodeParam;
                    rgpin[ipin].nodeAttr = nodeParam->pAttr;
                    rgpin[ipin++].SetName(name);
                }
                j++;
            ENDLOOP;

            ASSERT(j == invoke->params->size);
            ASSERT(ipin == meth->params->size - 1);
            ASSERT(!rgpin[ipin].node && !rgpin[ipin].nodeAttr);
            rgpin[ipin++].SetName(compiler()->namemgr->GetPredefName(fResultParam ? PN_DELEGATEEIPARAM0ALT : PN_DELEGATEEIPARAM0));
        }
        else {
            VSFAIL("Unknown method in delegate type");
        }
    }

    ASSERT(ipin == meth->params->size ||
        ipin == meth->params->size - 1 && meth->params->Item(ipin) == compiler()->getBSymmgr().GetArglistSym() ||
        ipin == 0 && meth->isIfaceImpl);
    pinfo->cpin = ipin;

    // Store the params in the METHINFO for downstream clients.
    // Note that for EndInvoke on a delegate type, this has the full set of parameters.
    pinfo->nodeParams = nodeParams;
}


// Compile a method...
void CLSDREC::compileMethod(METHSYM * method, AGGINFO * aggInfo)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::CompileMembers);
    ASSERT(!method->tokenImport);

    bool fEnc = compiler()->FEncBuild();

    if (fEnc && method->isExternal)
        return;

    BASENODE * tree = method->parseTree;

    ASSERT(tree == NULL || !method->getClass()->isFabricated);

    compiler()->funcBRec.setUnsafe(method->isUnsafe);

    METHINFO * info = (METHINFO *)STACK_ALLOC_ZERO(byte, METHINFO::Size(method->params->size));
    FillMethInfoCommon(method, info, aggInfo, false);

    int errNow = compiler()->ErrorCount();

    //
    // parse the method body
    //
    CComPtr<ICSInteriorTree> spIntTree;
    if (tree)
    {
        SETLOCATIONSTAGE(INTERIORPARSE);
        if ((tree->InGroup(NG_INTERIOR)) && FAILED(method->getInputFile()->pData->GetInteriorParseTree (tree, &spIntTree)))
            compiler()->Error(tree, FTL_NoMemory);     // The only possible failure
        else if (spIntTree != NULL)
        {
            CComPtr<ICSErrorContainer>  spErrors;

            // See if there were any errors in the interior parse
            if (SUCCEEDED(spIntTree->GetErrors(&spErrors)))
                controller()->ReportErrorsToHost(spErrors);
            else
                compiler()->Error(tree, FTL_NoMemory);  // Again, the only possible failure
        }
    }

    CErrorSuppression es;
    bool isStaticCGCtor;

    if (compiler()->FAbortEarly(errNow, &es))
        goto LERROR;

    isStaticCGCtor = method->isStatic && method->isCompilerGeneratedCtor();

    if (!fEnc && !isStaticCGCtor) {
        // Emit the method info.
        compiler()->emitter.EmitMethodInfo(method, info);
    }

    // Get the expr tree for method body.
    EXPR * body;

    if (method->getClass()->IsDelegate()) {
        // Delegate members are 'magic'. Their implementation is provided by the VOS.
        ASSERT(info->isMagicImpl);
        body = NULL;
    }
    else {
        SETLOCATIONSTAGE(COMPILE);

        if (!fEnc && !isStaticCGCtor)
            MethAttrBind::CompileAndEmit(compiler(), method, info->debuggerHidden);

        {
            SETLOCATIONSTAGE(BIND);
            body = compiler()->funcBRec.compileMethod(tree, info, aggInfo);

            // emit method def's for compiler generated constructors late
            if (isStaticCGCtor) {
                if (!body && compiler()->options.m_fOPTIMIZATIONS) // if optimizing don't emit an empty cctor
                    goto LERROR;
                compiler()->emitter.EmitMethodDef (method);
                if (!fEnc) {
                    // Emit the method info.
                    compiler()->emitter.EmitMethodInfo(method, info);
                    MethAttrBind::CompileAndEmit(compiler(), method, info->debuggerHidden);
                }
            }

            if (compiler()->FAbortEarly(errNow, &es))
                goto LERROR;

            // Don't bother with anonymous methods if there have been any errors at all.
            // If this changes, fix up CompileAnonMeths to handle NULL params.
            if (info->pamiFirst && !compiler()->FAbortCodeGen(0)) {
                body = compiler()->funcBRec.RewriteAnonDelegateBodies(method, info->outerScope, info->pamiFirst, body);
                CompileAnonMeths(info, tree, body);
            }

            // Iterators are a little more tolerant of errors, but they can't handle unwritten nested
            // anonymous methods.  So don't do them if #1 there were errors in this method, or #2 there
            // were anonymous methods in this method that we didn't transform because of errors
            if (info->IsIterator() && !compiler()->FAbortCodeGen(info->pamiFirst ? 0 : errNow)) {
                body = CompileIterator(tree, body, info, aggInfo);
                ASSERT(info->piin == NULL); // info->piin is local to CompileIterator
                if (!body || compiler()->FAbortEarly(errNow, &es))
                    goto LERROR;
            }
        }
    }

    // compiling the method reset the flag...
    compiler()->funcBRec.setUnsafe(method->isUnsafe);

    if (!fEnc) {

        int errorCount = compiler()->ErrorCount();
        // Emit attributes on parameters.
        ParamAttrBind::CompileParamList(compiler(), info);

        if (errorCount != compiler()->ErrorCount()) {
            ASSERT(compiler()->ErrorCount() > errorCount);
            // Mark indexer as having attribute error so we don't give the same error on the parameters for the second accessor
            if (info->meth->isPropertyAccessor() && info->meth->getProperty()->isIndexer())
                info->meth->getProperty()->fHadAttributeError = true;
            else if (info->meth->getClass()->IsDelegate())  // Mark delegates so we don't give the same errors on each fabricated method
                info->meth->getClass()->fHadAttributeError = true;
        }

        //
        // Check CLS compliance of signature (varargs, return type, and parameters)
        // NOTE: Do this AFTER compiling parameter-level attributes so that if they
        //   accidentally put the CLSCompliant attribute on a parameter, we give
        //   the warning about it being ignored/meaningless before giving the error
        //   about the non-CLS parameter.
        //
        if (compiler()->CheckForCLS() && compiler()->CheckSymForCLS(method, false) && method->hasExternalAccess()) {
            if (method->getClass()->IsDelegate()) {
                // Only check the invoke method for delegates 
                // BeginInvoke will just cause duplicate errors, and the other methods are all known OK.
                if (method->isInvoke()) {
                    CheckSigForCLS(method, method->getClass(), info->nodeParams);
                }
            } else if (!method->isPropertyAccessor() && !method->isEventAccessor()) {
                // Don't check property/event accessors because we check the property itself.
                CheckSigForCLS(method, method, info->nodeParams);
            }
        }

        if (aggInfo->isComimport && !method->isAbstract && !method->isExternal) {
            compiler()->ErrorRef(NULL, ERR_ComImportWithImpl, method, method->getClass());
        }
    }

    // Generate il only if no errors...
    if (compiler()->FAbortCodeGen(0))
        goto LERROR;


    if (!compiler()->options.m_fNOCODEGEN) {
        if (body) {
            ASSERT(!method->isAbstract && !method->isExternal);
            SETLOCATIONSTAGE(CODEGEN);
            compiler()->ilGenRec.compile(method, info, body);
        } else  {
            ASSERT(method->isAbstract || method->getClass()->IsDelegate() || method->isExternal);
        }
    }

    if (method->isExternal) {
        BASENODE * tree = method->getAttributesNode();
        if (!tree && !info->isMagicImpl && !aggInfo->isComimport) {
            // An extern method, property accessor or event with NO attributes!
            compiler()->ErrorRef(NULL, WRN_ExternMethodNoImplementation, method);
        }
    }

LERROR:

    spIntTree = NULL;

    compiler()->funcBRec.resetUnsafe();
}


EXPR * CLSDREC::CompileIterator(BASENODE * tree, EXPR * body, METHINFO * info, AGGINFO * aggInfo)
{
    ASSERT(!info->piin);
    IterInfo iin;
    memset(&iin, 0, sizeof(iin));
    info->piin = &iin;

    int cerrPrev = compiler()->ErrorCount();

    // First create the symbols
    METHSYM * iter = MakeIterator(info);

    if (!iter || compiler()->FAbortCodeGen(cerrPrev)) {
        ASSERT(compiler()->ErrorCount());
        info->piin = NULL;
        return NULL;
    }

    METHSYM * meth = iter;

    METHINFO * infoNew = (METHINFO *)STACK_ALLOC_ZERO(byte, METHINFO::Size(1));

    infoNew->InitFromIterInfo(info, meth, 0);

    // This will add all the fields for locals to both the able and etor classes
    body = compiler()->funcBRec.rewriteMoveNext(info->meth, body, infoNew);

    // Bail if there were errors
    if (compiler()->FAbortCodeGen(cerrPrev)) {
        info->piin = NULL;
        return NULL;
    }

    // So now we can emit all the metadata for the fabricated classes
    emitTypedefsAggregate(info->piin->aggIter);
    emitBasesAggregate(info->piin->aggIter);
    emitMemberdefsAggregate(info->piin->aggIter);

    // code-gen MoveNext
    ASSERT(infoNew->meth->params->size == 0);
    compileFabricatedMethod(tree, body, infoNew, aggInfo);
    compiler()->emitter.ResetIterator();

    // Now do the Current properties, IDispose.Dispose, IEnumerator.Reset,
    // IEnumerator.MoveNext (if not already done), and both IEnumerable.GetEnumerator methods
    TYPESYM * typeIDispose = compiler()->GetReqPredefType(PT_IDISPOSABLE);
    METHSYM * methGetEnumerator = NULL;
    TYPESYM * typeIEnumerator = NULL;
    typeIEnumerator = compiler()->GetReqPredefType(PT_IENUMERATOR);
    for (meth = compiler()->getBSymmgr().LookupAggMember(NULL, info->piin->aggIter, MASK_METHSYM)->asMETHSYM();
        meth;
        meth = compiler()->getBSymmgr().LookupNextSym(meth, info->piin->aggIter, MASK_METHSYM)->asMETHSYM())
    {
        // We've already code-gen'd the real MoveNext method so don't do it again
        if (meth == iter)
            continue;

        if (meth->isPropertyAccessor()) {
            infoNew->InitFromIterInfo(info, meth, 0);
            infoNew->debuggerHidden = true;
            infoNew->noDebugInfo = true;

            body = compiler()->funcBRec.makeIterCur(tree, infoNew, aggInfo);
            compileFabricatedMethod(tree, body, infoNew, aggInfo);
        }
        else if (meth->swtSlot.Type() == typeIDispose) {
            ASSERT(meth->swtSlot.Meth()->name == compiler()->getNamemgr()->GetPredefName(PN_DISPOSE));

            infoNew->InitFromIterInfo(info, meth, 0);

            body = compiler()->funcBRec.makeIterDispose(tree, infoNew, aggInfo);
            compileFabricatedMethod(tree, body, infoNew, aggInfo);
        } 
        else if (meth->retType == compiler()->getBSymmgr().GetVoid()) {
            ASSERT(meth->swtSlot.Type() == typeIEnumerator);
            ASSERT(meth->swtSlot.Meth()->name == compiler()->getNamemgr()->GetPredefName(PN_RESET));

            infoNew->InitFromIterInfo(info, meth, 0);
            infoNew->debuggerHidden = true;
            infoNew->noDebugInfo = true;

            body = compiler()->funcBRec.makeIterReset(tree, infoNew, aggInfo);
            compileFabricatedMethod(tree, body, infoNew, aggInfo);
        }
        else {
            ASSERT(meth->swtSlot.Meth()->name == compiler()->getNamemgr()->GetPredefName(PN_GETENUMERATOR));
            ASSERT(info->piin->fEnumerable);

            infoNew->InitFromIterInfo(info, meth, 0);
            infoNew->debuggerHidden = true;
            infoNew->noDebugInfo = true;

            body = compiler()->funcBRec.makeIterGetEnumerator(tree, infoNew, aggInfo, &methGetEnumerator);
            compileFabricatedMethod(tree, body, infoNew, aggInfo);
        }
    }
    compiler()->emitter.EndIterator();

    // .Ctor
    meth = compiler()->getBSymmgr().LookupAggMember(compiler()->namemgr->GetPredefName(PN_CTOR), info->piin->aggIter, MASK_METHSYM)->asMETHSYM();
    infoNew->InitFromIterInfo(info, meth, 1);
    infoNew->debuggerHidden = true;
    infoNew->noDebugInfo = true;
    infoNew->cpin = 1;
    infoNew->rgpin[0].SetName(compiler()->namemgr->GetPredefName(PN_ITERSTATE));
    body = compiler()->funcBRec.makeIterCtor(tree, infoNew, aggInfo);
    compileFabricatedMethod(NULL, body, infoNew, aggInfo);

    // dummy up a fake body for the real iterator method
    compiler()->emitter.EmitForwardedIteratorDebugInfo(info->meth, iter);
    body = compiler()->funcBRec.makeIterGet(tree, info, aggInfo);
    ASSERT(!compiler()->FAbortCodeGen(cerrPrev)); // This code should never report any errors!

    info->piin = NULL;
    info->noDebugInfo = true;
    return body;
}


void CLSDREC::CheckSigForCLS(METHSYM * method, SYM * errorSym, BASENODE * pParamTree)
{
    ASSERT(compiler()->AllowCLSErrors());
    ASSERT(errorSym == method || (errorSym->asAGGSYM()->IsDelegate() && method->isInvoke()));
    if (method->isVarargs)
        compiler()->ErrorRef(NULL, WRN_CLS_NoVarArgs, errorSym);
    if (method->retType && !compiler()->isCLS_Type(method->parent, method->retType))
        compiler()->ErrorRef(NULL, WRN_CLS_BadReturnType, errorSym);
    if (method->typeVars->size > 0) {
        // check bounds...
        for (int i = 0, n = method->typeVars->size; i < n; i++) {
            TYVARSYM * var = method->typeVars->ItemAsTYVARSYM(i);
            TypeArray * bnds = var->GetBnds();
            for (int j = 0; j < bnds->size; j++) {
               TYPESYM * typeBnd = bnds->Item(j);
               if (!compiler()->isCLS_Type(method->parent, typeBnd)) {
                 compiler()->ErrorRef(NULL, WRN_CLS_BadTypeVar, typeBnd);
               }
            }
        }
    }

    int i = 0;
    NODELOOP(pParamTree, PARAMETER, param)
        if (!compiler()->isCLS_Type(method->parent, method->params->Item(i))) {
            compiler()->Error(param, WRN_CLS_BadArgType, method->params->Item(i));
        }
        i += 1;
    ENDLOOP
    ASSERT(pParamTree == NULL || i == method->params->size);
}

PARAMINFO * CLSDREC::ReallocParams( int cntNeeded, int * maxAlreadyAlloced, PARAMINFO ** ppParams)
{
    if (cntNeeded == 0 && *maxAlreadyAlloced == 0 && *ppParams == NULL) {
        *ppParams = (PARAMINFO *)compiler()->localSymAlloc.AllocZero(0 * sizeof (PARAMINFO));
    } else if (cntNeeded > *maxAlreadyAlloced) {
        if (*maxAlreadyAlloced == 0)
            *maxAlreadyAlloced = 4;

        *maxAlreadyAlloced = max(cntNeeded * 2, *maxAlreadyAlloced * 2);
        *ppParams = (PARAMINFO *)compiler()->localSymAlloc.AllocZero(*maxAlreadyAlloced * sizeof (PARAMINFO));
    } else if (cntNeeded > 0) {
        // clear out any old junk
        memset(*ppParams, 0, sizeof(PARAMINFO) * cntNeeded);
    }

    return *ppParams;
}

// Optionally rewrites any anonymous methods contained in this method
// and emits/compiles the nested classes/methods.
void CLSDREC::CompileAnonMeths(METHINFO * infoOuter, BASENODE * tree, EXPR * body)
{
    if (!infoOuter->pamiFirst)
        return;

    ASSERT(tree);

    // This method had some anonymous delegates inside it.
    // It's already been rewritten, so just emit the metadata and compile the bodies.
    // compileFabricatedMethod will recurse into nested anonymous delegates.

    // Get the maximum number of parameters.
    int cpinMax = 0;
    AnonMethInfo * pami;

    for (pami = infoOuter->pamiFirst; pami; pami = pami->pamiNext) {
        ASSERT(pami->fSeen);
        // Update the max number of parameters.
        if (cpinMax < pami->meth->params->size)
            cpinMax = pami->meth->params->size;
    }

    // Allocate the METHINFO.
    METHINFO * methInfo = (METHINFO *)STACK_ALLOC_ZERO(byte, METHINFO::Size(cpinMax));

    AGGINFO newAggInfo;
    memset(&newAggInfo, 0, sizeof(newAggInfo));

    // Emit all the metadata for the fabricated classes and methods
    // and compile the .ctors
    FOREACHCHILD(infoOuter->meth->getClass(), child)
        // Only do un-compiled, fabricated classes
        if (!child->isAGGSYM() || !child->asAGGSYM()->isFabricated || child->asAGGSYM()->IsCompiled())
            continue;

        AGGSYM * cls = child->asAGGSYM();
        METHSYM * ctor = compiler()->getBSymmgr().LookupAggMember(compiler()->namemgr->GetPredefName(PN_CTOR), cls, MASK_METHSYM)->asMETHSYM();
        ASSERT(!ctor || (ctor->isCtor() && ctor->nextSameName == NULL));

        // If it doesn't have a ctor, then it must be a fixed buffer struct, which also means we
        // shouldn't touch it
        if (!ctor) {
            ASSERT(cls->isMemberDefsEmitted && cls->isBasesEmitted && cls->isTypeDefEmitted);
#ifdef DEBUG
            FOREACHCHILD( cls, nested)
                ASSERT(!nested->isMETHPROPSYM());
            ENDFOREACHCHILD;
#endif // DEBUG
            cls->SetAggState(AggState::Compiled);
            continue;
        }

        // If the ctor has arguments, it must be for an iterator, which means we shouldn't touch this class
        if (ctor->params->size != 0)
            continue;

        emitTypedefsAggregate(cls);
        emitBasesAggregate(cls);
        emitMemberdefsAggregate(cls);

        // It's safe to compile all the .ctors here because they're just simple .ctors
        // with no dependencies
        memset(methInfo, 0, METHINFO::Size(cpinMax));
        methInfo->meth = ctor;
        methInfo->noDebugInfo = true;

        EXPR * body = compiler()->funcBRec.makeAnonCtor(tree, methInfo, &newAggInfo);
        compileFabricatedMethod(tree, body, methInfo, &newAggInfo);
        cls->SetAggState(AggState::Compiled);
    ENDFOREACHCHILD;

    // Emit all the metadata for any anonymous methods that were pushed up to the user's class
    for (pami = infoOuter->pamiFirst; pami; pami = pami->pamiNext) {
        if (IsNilToken(pami->meth->tokenEmit)) {
            ASSERT(pami->meth->getClass()->isTypeDefEmitted);
            // an optimized anonymous method that is a member
            // of a user type that has already been emitted
            // so just emit this method (this is out of order)
            ASSERT(!pami->meth->getClass()->isFabricated);
            compiler()->emitter.EmitMethodDef(pami->meth);
        }

        if (pami->pCachedDelegate != NULL && pami->pCachedDelegate->kind == EK_FIELD) {
            MEMBVARSYM * field = pami->pCachedDelegate->asFIELD()->fwt.Field();
            ASSERT(IsNilToken( field->tokenEmit));
            ASSERT( field->getClass()->isTypeDefEmitted);
            compiler()->emitter.EmitMembVarDef( field);
            compileField(field, &newAggInfo);
        }
    }

    // Compile all the anonymous methods
    for (pami = infoOuter->pamiFirst; pami; pami = pami->pamiNext) {
        ASSERT(!pami->meth->getClass()->isFabricated || pami->meth->getClass()->IsCompiled());

        memset(methInfo, 0, METHINFO::Size(cpinMax));
        methInfo->meth = pami->meth;
        methInfo->pamiFirst = pami->pamiChild;
        methInfo->outerScope = pami->pArgs;
        methInfo->hasRetAsLeave = pami->fHasRetAsLeave;

        ANONBLOCKNODE * nodeAnon = methInfo->meth->parseTree->asANONBLOCK();
        methInfo->nodeParams = nodeAnon->pArgs;

        int i = 0;
        NODELOOP(nodeAnon->pArgs, PARAMETER, param)
            ASSERT(i < cpinMax && i < methInfo->meth->params->size);
            methInfo->rgpin[i].node = param;
            methInfo->rgpin[i++].SetName(param->pName->pName);
        ENDLOOP;
        ASSERT(i == methInfo->meth->params->size || i == 0 && nodeAnon->iClose == -1);
        methInfo->cpin = i;

        compileFabricatedMethod(tree, pami->pBodyExpr, methInfo, &newAggInfo);
    }
}

// Compile a fabricated method...
void CLSDREC::compileFabricatedMethod(BASENODE * tree, EXPR * body, METHINFO * info, AGGINFO * aggInfo)
{
    ASSERT(info->meth);

    // Either the entire class is fabricated or at least this method is an anonymous method
    ASSERT(info->meth->getClass()->isFabricated || info->meth->isAnonymous());
    ASSERT(!info->meth->tokenImport);

    // No fabricated method will ever be a param array!
    ASSERT(!info->meth->isParamArray);

    //
    // emit method info
    //
    compiler()->emitter.EmitMethodInfo(info->meth, info);

    //
    // emit attributes on the method and parameters
    // The method should get the DebuggerHiddenAttribute and the CompilerGeneratedAttribute
    // And the parameters just get their names
    //

    MethAttrBind::CompileAndEmit(compiler(), info->meth, info->debuggerHidden);
    if (info->meth->params->size)
        ParamAttrBind::CompileParamList(compiler(), info);

    // Recursively compile any nested anonymous delegates
    CompileAnonMeths(info, tree, body);

    // generate il
    if (!compiler()->options.m_fNOCODEGEN) {
        ASSERT(body);
        ASSERT(!info->meth->isAbstract && !info->meth->isExternal);
        SETLOCATIONSTAGE(CODEGEN);
        compiler()->ilGenRec.compile(info->meth, info, body);
    }
}


void CLSDREC::compileProperty(PROPSYM * property, AGGINFO * aggInfo)
{
    PropAttrBind::Compile(compiler(), property);

    PROPINFO info;
    MEM_SET_ZERO(info);

    // Accumulate parameter names for indexed properties, if any.
    if (property->params->size) {
        PARAMINFO *paramInfo = info.paramInfos = STACK_ALLOC_ZERO(PARAMINFO, property->params->size);
        int i=0;
        NODELOOP(property->getParseTree()->asANYPROPERTY()->pParms, PARAMETER, param)
            paramInfo->node = param;
            paramInfo->SetName(param->pName->pName);
            paramInfo++;

            if (compiler()->CheckForCLS() && compiler()->CheckSymForCLS(property, false) && 
                property->hasExternalAccess() && !compiler()->isCLS_Type(property->parent, property->params->Item(i))) 
            {
                compiler()->Error(param, WRN_CLS_BadArgType, property->params->Item(i));
            }
            i++;
        ENDLOOP;
    }

    if (compiler()->CheckForCLS() && compiler()->CheckSymForCLS(property, false) && property->hasExternalAccess()) {
        if (!compiler()->isCLS_Type(property->parent, property->retType)) {
            compiler()->ErrorRef(NULL, WRN_CLS_BadFieldPropType, property);
        }
    }

}

void CLSDREC::compileEvent(EVENTSYM * event, AGGINFO * aggInfo)
{
    DefaultAttrBind::CompileAndEmit(compiler(), event);

    if (compiler()->CheckForCLS() && compiler()->CheckSymForCLS(event, false) && event->hasExternalAccess() && !compiler()->isCLS_Type(event->parent, event->type))
        compiler()->ErrorRef(NULL, WRN_CLS_BadFieldPropType, event);

}


/***************************************************************************************************
    Searches the class [atsSearch] to see if it contains a method which is sufficient to implement
    [mwt]. Does not search base classes. [mwt] is typically a method in some interface.  We may be
    implementing this interface at some particular type, e.g. IList<String>, and so the required
    signature is the instantiation (i.e. substitution) of [mwt] for that instance. Similarly, the
    implementation may be provided by some base class that exists via polymorphic inheritance,
    e.g. Foo : List<String>, and so we must instantiate the parameters for each potential
    implementation. [atsSearch] may thus be an instantiated type.

    If fOverride is true, this checks for a method with swtSlot set to the particular method.
***************************************************************************************************/
METHSYM *CLSDREC::FindSameSignature(MethWithType mwt, AGGTYPESYM * atsSearch, bool fOverride)
{
    TypeArray * params = compiler()->getBSymmgr().SubstTypeArray(mwt.Meth()->params, mwt.Type());

    for (SYM * sym = compiler()->getBSymmgr().LookupAggMember(mwt.Meth()->name, atsSearch->getAggregate(), MASK_ALL);
        sym != NULL;
        sym = compiler()->getBSymmgr().LookupNextSym(sym, sym->parent, MASK_ALL))
    {
        if (!sym->isMETHSYM())
            continue;
        METHSYM * meth = sym->asMETHSYM();

        if (meth->typeVars->size != mwt.Meth()->typeVars->size ||
            !compiler()->getBSymmgr().SubstEqualTypeArrays(params, meth->params, atsSearch, mwt.Meth()->typeVars))
        {
            ASSERT(meth->swtSlot.Meth() != mwt.Meth());
            continue;
        }

        if (!fOverride ||
            meth->swtSlot.Meth() == mwt.Meth() ||
            // This condition handles when mwt.Meth() is an abstract override and meth overrides the same method.
            meth->swtSlot.Meth() == mwt.Meth()->swtSlot.Meth() && meth->swtSlot)
        {
            return meth;
        }
    }

    return NULL;
}


/*
 * See findSameSignature for methods above.
 */
PROPSYM *CLSDREC::findSameSignature(AGGTYPESYM *propAtTyp, PROPSYM *prop, AGGTYPESYM *typeToSearchIn)
{
    SYM *symbol = compiler()->getBSymmgr().LookupAggMember(prop->name, typeToSearchIn->getAggregate(), MASK_ALL);
    TypeArray *needed = compiler()->getBSymmgr().SubstTypeArray(prop->params, propAtTyp);
    while (symbol && (!symbol->isPROPSYM() || 
                      !compiler()->getBSymmgr().SubstEqualTypeArrays(needed, symbol->asPROPSYM()->params, typeToSearchIn)))
    {
        symbol = compiler()->getBSymmgr().LookupNextSym(symbol, symbol->parent, MASK_ALL);
    }
    return symbol->asPROPSYM();
}

/*
 * Searches for an accessible name in a class and its base classes
 *
 * name - the name to search for
 * classToSearchIn - the classToSearchIn
 * context - the class we are searching from
 * current - if specified, then the search looks for members in the class
 *           with the same name after current in the symbol table
 *           if specified current->parent  must equal classToSearchIn
 *
 * This method can be used to iterate over all accessible base members
 * of a given name, by updating current with the previous return value
 * and classToSearchIn with current->parent
 *
 * This method never reports any errors
 *
 * Returns NULL if there are no remaining members in a base class
 * with the given name.
 *
 */
SYM *CLSDREC::findNextAccessibleName(NAME *name, AGGTYPESYM **pTypeToSearchIn, PARENTSYM *context, SYM *current, bool bAllowAllProtected, bool ignoreSpecialMethods)
{
    PAGGTYPESYM qualifier = bAllowAllProtected ? NULL : *pTypeToSearchIn;
    while ((current = findNextName(name, pTypeToSearchIn, current)) && 
        (!CheckAccess(current, *pTypeToSearchIn, context, qualifier) || (ignoreSpecialMethods && !current->isUserCallable()))) {
    }

    return current;
}

/*
 * Searches for a name in a class and its base classes (though _not_ its interfaces!)
 *
 * name - the name to search for
 * pTypeToSearchIn - points to the typeToSearchIn, and after the call will point to
 *                   the type where the member was found, and also, coincidentally, the
 *                   information needed to continue the search if you want to search for more members. 
 *                   Normally evaluates to an AGGSYM but perhaps an
 *                   instantiated class.  May be updated if the search moves to a new
 *                   type, i.e. the base class.
 * current - if specified, then the search looks for members in the class
 *           with the same name after current in the symbol table
 *           if specified current->parent  must equal (*pTypeToSearchIn)->getAggregate()
 *
 * This method can be used to iterate over all base members
 * of a given name, by updating current with the previous return value.
 * pTypeToSearchIn will automatically be given the correct next value (i.e. the
 * containing type for current).
 *
 * This method never reports any errors
 *
 * Returns NULL if there are no remaining members in a base class
 * with the given name, in which case pTypeToSearchIn will also be set to NULL.
 *
 */
SYM *CLSDREC::findNextName(NAME *name, AGGTYPESYM **pTypeToSearchIn, SYM *current)
{
    //
    // check for next in same class
    //
    if (current) {
        ASSERT(current->parent == (*pTypeToSearchIn)->getAggregate());
        current = compiler()->getBSymmgr().LookupNextSym(current, current->parent, MASK_ALL);
        if (current)
            return current;

        //
        // didn't find any more in this class
        // start with the base class
        //
        *pTypeToSearchIn = (*pTypeToSearchIn)->GetBaseClass();
    }

    //
    // check base class
    //
    BASE_CLASS_TYPES_LOOP(*pTypeToSearchIn, typeToSearch)
        *pTypeToSearchIn = typeToSearch;
        SYM * hiddenSymbol = NULL;
        compiler()->EnsureState((*pTypeToSearchIn)->getAggregate(), AggState::DefinedMembers);
        hiddenSymbol = compiler()->getBSymmgr().LookupAggMember(name, (*pTypeToSearchIn)->getAggregate(), MASK_ALL);
        if (hiddenSymbol) {
            return hiddenSymbol;
        }
    END_BASE_CLASS_TYPES_LOOP

    return NULL;
}

//
// find an inherited member which is hidden by name
//  name      - name to find hidden member
//  typeStart - class to start the search in
//  agg       - context to search from for access checks
//  ptypeFound - out parameter indicating where the sym was found
//
bool CLSDREC::FindAnyHiddenSymbol(NAME *name, AGGTYPESYM *typeStart, AGGSYM *agg, SymWithType * pswt)
{
    for (SYM * symCur = NULL; (symCur = findNextAccessibleName(name, &typeStart, agg, symCur, true, false)) != NULL; ) {
        switch (symCur->getKind()) {
        case SK_METHSYM:
            if (symCur->asMETHSYM()->typeVars->size)
                continue;
            // Accessors don't count. They're not really there....
            if (symCur->asMETHSYM()->isAnyAccessor())
                continue;
            break;
        case SK_AGGSYM:
            if (symCur->asAGGSYM()->typeVarsThis->size)
                continue;
            break;
        default:
            break;
        }
        pswt->Set(symCur, typeStart);
        return true;
    }

    pswt->Set(NULL, NULL);
    return false;
}


bool CLSDREC::FindSymHiddenByMethPropAgg(SYM *sym, AGGTYPESYM *typeStart, AGGSYM *agg, SymWithType * pswt, SymWithType * pswtAmbig, bool * pfNeedMethodImpl)
{
    ASSERT(pswt);
    ASSERT(sym->isMETHSYM() || sym->isPROPSYM() || sym->isAGGSYM());
    ASSERT(!sym->isMETHSYM() || !sym->asMETHSYM()->isCtor());

    SYMKIND sk = sym->getKind();
    bool fIndexer = (sk == SK_PROPSYM) && sym->asPROPSYM()->isIndexer();
    TypeArray * params = (fIndexer || sk == SK_METHSYM) ? sym->asMETHPROPSYM()->params : NULL;

    AGGTYPESYM * typeCur = typeStart;
    bool fAllowDifferent = false;
    AGGTYPESYM * typeLast = NULL;
    AGGTYPESYM * typeFound = NULL;
    SYM * symFound = NULL;
    SYM * symAmbig = NULL;
    bool fNeedMethodImpl = false;

    int arity;
    switch (sk) {
    case SK_METHSYM:
        arity = sym->asMETHSYM()->typeVars->size;
        break;
    case SK_AGGSYM:
        arity = sym->asAGGSYM()->typeVarsThis->size;
        break;
    default:
        arity = 0;
        break;
    }

    for (SYM * symCur = NULL; (symCur = findNextAccessibleName(sym->name, &typeCur, agg, symCur, true, false)) != NULL; )
    {
        if (typeLast && typeLast != typeCur) {
            // typeLast contained a member that hid everything beneath it and we've past typeLast.
            break;
        }

        if (sk != symCur->getKind()) {
            // Some things (ctors, operators, indexers, accessors) don't interact based on name!
            if (fIndexer ||
                symCur->isPROPSYM() && symCur->asPROPSYM()->isIndexer() ||
                symCur->isMETHSYM() &&
                    (symCur->asMETHSYM()->isOperator || symCur->asMETHSYM()->isCtor() || symCur->asMETHSYM()->isAnyAccessor()) ||
                sym->isMETHSYM() && sym->asMETHSYM()->isAnyAccessor())
            {
                continue;
            }

            switch (symCur->getKind()) {
            case SK_METHSYM:
                if (arity == symCur->asMETHSYM()->typeVars->size)
                    break;
                continue;
            case SK_AGGSYM:
                if (arity == symCur->asAGGSYM()->typeVarsThis->size ||
                    sk == SK_METHSYM && !symCur->asAGGSYM()->typeVarsThis->size)
                {
                    break;
                }
                continue;
            default:
                if (!arity || sk == SK_METHSYM)
                    break;
                continue;
            }

            // Different kind of member is always hidden. But if fAllowDifferent is true, this member is already
            // hidden by another method/indexer/agg at a lower level than sym.
            if (fAllowDifferent) {
                // Don't look past this type since this guy hides all further down.
                typeLast = typeCur;
                continue;
            }
        }
        else {
            switch (sk) {
            case SK_AGGSYM:
                if (symCur->asAGGSYM()->typeVarsThis->size != arity)
                    continue;
                break;
            case SK_METHSYM:
                if (symCur->asMETHSYM()->isCtor() || !symCur->asMETHSYM()->isOperator != !sym->asMETHSYM()->isOperator)
                    continue;
                if (symCur->asMETHSYM()->typeVars->size != arity)
                    continue;
                if (!compiler()->getBSymmgr().SubstEqualTypeArrays(params, symCur->asMETHSYM()->params, typeCur->typeArgsAll, sym->asMETHSYM()->typeVars)) {
                    fAllowDifferent = true;
                    continue;
                }
                if (pfNeedMethodImpl && !symCur->asMETHSYM()->isAnyAccessor() != !sym->asMETHSYM()->isAnyAccessor()) {
                    fNeedMethodImpl = true;
                    continue;
                }
                break;
            case SK_PROPSYM:
                if (fIndexer && (!symCur->asPROPSYM()->isIndexer() ||
                    !compiler()->getBSymmgr().SubstEqualTypeArrays(params, symCur->asPROPSYM()->params, typeCur->typeArgsAll)))
                {
                    continue;
                }
                break;
            default:
                break;
            }
        }

        ASSERT(!symFound || symFound->isMETHPROPSYM() && typeFound == typeLast && typeFound == typeCur);
        if (!symCur->isMETHPROPSYM()) {
            symFound = symCur;
            typeFound = typeCur;
            symAmbig = NULL;
            break;
        }

        ASSERT (symCur->isMETHPROPSYM ());
        //EDMAURER If the symbol being examined this iteration has fewer MODOPTS than 
        //the previously found symbol, it is a better match. So replace symFound.

        if (!symFound || symCur->asMETHPROPSYM()->modOptCount < symFound->asMETHPROPSYM()->modOptCount) {
            ASSERT(!symAmbig || symFound);
            symFound = symCur;
            typeFound = typeCur;
            typeLast = typeCur;

            //EDMAURER If previous best symbol had an ambiguity, it doesn't matter anymore.
            symAmbig = NULL;
        }
        else if (symCur->asMETHPROPSYM()->modOptCount == symFound->asMETHPROPSYM()->modOptCount && !symAmbig)
            symAmbig = symCur;
    }

    pswt->Set(symFound, typeFound);
    if (pswtAmbig)
        pswtAmbig->Set(symAmbig, typeFound);
    if (pfNeedMethodImpl)
        *pfNeedMethodImpl = fNeedMethodImpl;

    return pswt->Sym() != NULL;
}

// Report a deprecation error on a symbol.
// If tree is NULL, refContext is used as a location to report the error.
void CLSDREC::ReportDeprecated(BASENODE * tree, PSYM refContext, SymWithType swt)
{
    ASSERT(swt.Sym()->IsDeprecated());

    if (refContext->isContainedInDeprecated())
        return;

    if (swt.Sym()->isTYPESYM()) {
        swt.Set(swt.Sym()->asTYPESYM()->GetNakedType(true), swt.Type());
    }
    PCWSTR pszDep = swt.Sym()->DeprecatedMessage();

    if (pszDep) {
        // A message is associated with this deprecated symbol: use that.
        int errId = swt.Sym()->IsDeprecatedError() ? ERR_DeprecatedSymbolStr : WRN_DeprecatedSymbolStr;

        if (!tree)
            compiler()->Error(NULL, errId, swt, pszDep, ErrArgRefOnly(refContext));
        else
            compiler()->Error(tree, errId, swt, pszDep);
    }
    else {
        // No message
        if (!tree)
            compiler()->Error(NULL, WRN_DeprecatedSymbol, swt, ErrArgRefOnly(refContext));
        else
            compiler()->Error(tree, WRN_DeprecatedSymbol, swt);
    }
}

// Check to see if sym1 is "at least as visible" as sym2.
bool CLSDREC::isAtLeastAsVisibleAs(SYM * sym1, SYM * sym2)
{
    SYM * s1parent, * s2parent;

    ASSERT(sym2 && sym2->parent && !sym2->isARRAYSYM() && !sym2->isPTRSYM() && !sym2->isPARAMMODSYM() && !sym2->isAGGTYPESYM());

    // quick check -- everything is at least as visible as private 
    if (sym2->GetAccess() == ACC_PRIVATE)
        return true;

    if (sym1->isTYPESYM()) {
        if (sym1->asTYPESYM()->HasErrors()) {
            // Error syms have already generated an error - don't generate another.
            return true;
        }

LRecurse:
        // If sym1 is a pointer, array, or byref type, convert to underlying type. 
        switch (sym1->getKind()) {
        case SK_ARRAYSYM:
        case SK_PTRSYM:
        case SK_NUBSYM:
        case SK_PARAMMODSYM:
        case SK_MODOPTTYPESYM:
            sym1 = sym1->parent->asTYPESYM();
            goto LRecurse;
        case SK_VOIDSYM:
            // void is completely visible.
            return true;
        case SK_TYVARSYM:
            // in the current model tyvar's are completely visible.
            // This may not be what we desire in the long run - it is
            // possible to imagine that a tyvar is private, in the sense
            // that the generic class must promise not to reveal anything
            // more about it than the user of the generic class already knows.
            return true; 
        case SK_AGGTYPESYM:
            // GENERICS: check visibility of argument types (e.g. [String] in List<String>), as
            // well as the root type which we check via the loop.
            // Check all type parameters here (including ones from outer types).
            // aggregates are then checked below
            for (int i = 0; i < sym1->asAGGTYPESYM()->typeArgsAll->size; i++) { 
                if (!isAtLeastAsVisibleAs(sym1->asAGGTYPESYM()->typeArgsAll->Item(i), sym2))
                    return false;
            }
            sym1 = sym1->asAGGTYPESYM()->getAggregate();
            break;
        default:
            break;
        }
    }

    // Algorithm: The only way that sym1 is NOT at least as visible as sym2
    // is if it has a access restriction that sym2 does not have. So, we simply
    // go up the parent chain on sym1. For each access modifier found, we check
    // to see that the same access modifier, or a more restrictive one, is somewhere
    // is sym2's parent chain.

    for (SYM * s1 = sym1; !s1->isNSSYM(); s1 = s1->parent)
    {
        ACCESS acc1 = s1->GetAccess();

        if (acc1 != ACC_PUBLIC) {
            bool asRestrictive = false;

            for (SYM * s2 = sym2; !s2->isNSSYM(); s2 = s2->parent)
            {
                ACCESS acc2 = s2->GetAccess();

                switch (acc1) {
                case ACC_INTERNAL:
                    // If s2 is private or internal, and within the same assembly as s1,
                    // then this is at least as restrictive as s1's internal. 
                    if ((acc2 == ACC_PRIVATE || acc2 == ACC_INTERNAL) && s2->SameAssemOrFriend(s1))
                        asRestrictive = true;
                    break;

                case ACC_PROTECTED:
                    s1parent = s1->parent;

                    if (acc2 == ACC_PRIVATE) {
                        // if s2 is private and within s1's parent or within a subclass of s1's parent,
                        // then this is at least as restrictive as s1's protected. 
                        for (s2parent = s2->parent; !s2parent->isNSSYM(); s2parent = s2parent->parent)
                            if (compiler()->IsBaseAggregate(s2parent->asAGGSYM(), s1parent->asAGGSYM()))
                                asRestrictive = true;
                    }
                    else if (acc2 == ACC_PROTECTED) {
                        // if s2 is protected, and it's parent is a subclass (or the same as) s1's parent
                        // then this is at least as restrictive as s1's protected
                        if (compiler()->IsBaseAggregate(s2->parent->asAGGSYM(), s1parent->asAGGSYM()))
                            asRestrictive = true;
                    }
                    break;
                        
                case ACC_INTERNALPROTECTED:
                    s1parent = s1->parent;

                    if (acc2 == ACC_PRIVATE) {
                        // if s2 is private and within a subclass of s1's parent,
                        // or withing the same assembly as s1
                        // then this is at least as restrictive as s1's internal protected. 
                        if (s2->SameAssemOrFriend(s1))
                            asRestrictive = true;
                        else {
                            for (s2parent = s2->parent; !s2parent->isNSSYM(); s2parent = s2parent->parent)
                                if (compiler()->IsBaseAggregate(s2parent->asAGGSYM(), s1parent->asAGGSYM()))
                                    asRestrictive = true;
                        }
                    }
                    else if (acc2 == ACC_INTERNAL) {
                        // If s2 is in the same assembly as s1, then this is more restrictive
                        // than s1's internal protected.
                        if (s2->SameAssemOrFriend(s1))
                            asRestrictive = true;
                    }
                    else if (acc2 == ACC_PROTECTED) {
                        // if s2 is protected, and it's parent is a subclass (or the same as) s1's parent
                        // then this is at least as restrictive as s1's internal protected
                        if (compiler()->IsBaseAggregate(s2->parent->asAGGSYM(), s1parent->asAGGSYM()))
                            asRestrictive = true;
                    }
                    else if (acc2 == ACC_INTERNALPROTECTED) {
                        // if s2 is internal protected, and it's parent is a subclass (or the same as) s1's parent
                        // and its in the same assembly and s1, then this is at least as restrictive as s1's protected
                        if (s2->SameAssemOrFriend(s1) && compiler()->IsBaseAggregate(s2->parent->asAGGSYM(), s1parent->asAGGSYM()))
                            asRestrictive = true;
                    }
                    break;

                case ACC_PRIVATE:
                    if (acc2 == ACC_PRIVATE) {
                        // if s2 is private, and it is withing s1's parent, then this is at
                        // least as restrictive of s1's private.
                        s1parent = s1->parent;
                        for (s2parent = s2->parent; !s2parent->isNSSYM(); s2parent = s2parent->parent)
                            if (s2parent == s1parent)
                                asRestrictive = true;
                    }
                    break;

                default:
                    ASSERT(0);
                    break;
                }

            }

            if (! asRestrictive)
                return false;  // no modifier on sym2 was as restrictive as s1
        }
    }

    return true;
}

// Check a symbol and a constituent symbol to make sure the constituent is at least 
// as visible as the main symbol. If not, report an error of the given code.
void CLSDREC::checkConstituentVisibility(SYM * main, SYM * constituent, int errCode)
{
    if (!isAtLeastAsVisibleAs(constituent, main)) {
        compiler()->ErrorRef(NULL, errCode, main, constituent);
    }
}

// Get the value for the specific field in the LayoutKind enum.  First time through we look this up, then cache the result for any subsequent queries.
int CLSDREC::GetLayoutKindValue(PREDEFNAME pnLayoutKind)
{
    ASSERT((pnLayoutKind == PN_EXPLICIT) || (pnLayoutKind == PN_SEQUENTIAL));

    // first check for the cached value
    int val = 0;

    if (pnLayoutKind == PN_EXPLICIT) {
        if (m_explicitLayoutValue >= 0)
            return m_explicitLayoutValue;
        val = 2;
    }
    else if (pnLayoutKind == PN_SEQUENTIAL) {
        if (m_sequentialLayoutValue >= 0)
            return m_sequentialLayoutValue;
        val = 0;
    }

    // Otherwise we need to look it up
    AGGTYPESYM * cls = compiler()->GetOptPredefType(PT_LAYOUTKIND);
    ASSERT(cls);

    NAME * name = compiler()->namemgr->GetPredefName(pnLayoutKind);
    MEMBVARSYM * fld = compiler()->getBSymmgr().LookupAggMember(name, cls->getAggregate(), MASK_MEMBVARSYM)->asMEMBVARSYM();

    if (!fld || !fld->isConst)
        compiler()->Error(NULL, ERR_MissingPredefinedMember, cls->name->text, name->text);
    else {
        ASSERT(fld->constVal.iVal >= 0);
        val = fld->constVal.iVal;
    }

    // cache the result
    if (pnLayoutKind == PN_EXPLICIT)
        m_explicitLayoutValue = val;
    else if (pnLayoutKind == PN_SEQUENTIAL)
        m_sequentialLayoutValue = val;

    return val;
}

