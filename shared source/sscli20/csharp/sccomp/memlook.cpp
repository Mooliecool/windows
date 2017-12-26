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
// File: memlook.cpp
//
// Member lookup
// ===========================================================================

#include "stdafx.h"

/***************************************************************************************************
    Lookup must be called before anything else can be called.

    typeSrc - Must be an AGGTYPESYM or TYVARSYM.
    obj - the expression through which the member is being accessed. This is used for accessibility
        of protected members and for constructing a MEMGRP from the results of the lookup.
        It is legal for obj to be an EK_CLASS, in which case it may be used for accessibility, but
        will not be used for MEMGRP construction.
    symWhere - the symbol from with the name is being accessed (for checking accessibility).
    name - the name to look for.
    arity - the number of type args specified. Only members that support this arity are found.
        Note that when arity is zero, all methods are considered since we do type argument
        inferencing.

    flags - See MemLookFlags.
        TypeVarsAllowed only applies to the most derived type (not base types).
***************************************************************************************************/
bool MemberLookup::Lookup(COMPILER * compiler, TYPESYM * typeSrc, EXPR * obj, PARENTSYM * symWhere,
    NAME * name, int arity, MemLookFlagsEnum flags)
{
    ASSERT((flags & ~MemLookFlags::All) == 0);
    ASSERT(!obj || obj->type);
    ASSERT(typeSrc->isAGGTYPESYM() || typeSrc->isTYVARSYM());

    // Clear out the results.
    memset(this, 0, sizeof(*this));

#ifdef DEBUG
    m_fValid = true;
#endif

    m_prgtype = m_rgtypeStart;
    m_ctypeMax = lengthof(m_rgtypeStart);

    // Save the inputs for error handling, etc.
    m_compiler = compiler;
    m_typeSrc = typeSrc;
    m_obj = obj && obj->kind != EK_CLASS ? obj : NULL;
    m_symWhere = symWhere;
    m_name = name;
    m_arity = arity;
    m_flags = flags;

    if (m_flags & MemLookFlags::BaseCall)
        m_typeQual = NULL;
    else if (m_flags & MemLookFlags::Ctor)
        m_typeQual = m_typeSrc;
    else if (obj)
        m_typeQual = obj->type;
    else
        m_typeQual = NULL;

    // Determine what to search.
    AGGTYPESYM * typeCls1 = NULL;
    AGGTYPESYM * typeIface = NULL;
    TypeArray * ifaces = BSYMMGR::EmptyTypeArray();
    AGGTYPESYM * typeCls2 = NULL;

    // We need to do EnsureState before fetching the ifaces and cls bound because
    // these may change as a result (in the EE anyway).

    if (typeSrc->isTYVARSYM()) {
        // This may be a little paranoid, but may be necessary for the EE....
        ASSERT(typeSrc->parent->isAGGSYM() || typeSrc->parent->isMETHSYM());
        AGGSYM * aggPar = typeSrc->parent->isAGGSYM() ? typeSrc->parent->asAGGSYM() : typeSrc->parent->asMETHSYM()->getClass();
        compiler->EnsureState(aggPar);
    }

    compiler->EnsureState(typeSrc);

    if (typeSrc->isTYVARSYM()) {
        ASSERT(!(m_flags & (MemLookFlags::Ctor | MemLookFlags::NewObj | MemLookFlags::Operator | MemLookFlags::BaseCall | MemLookFlags::TypeVarsAllowed)));
        m_flags &= ~MemLookFlags::TypeVarsAllowed;
        ifaces = typeSrc->asTYVARSYM()->GetIfacesAll();
        typeCls1 = typeSrc->asTYVARSYM()->GetBaseCls();
        if (ifaces->size && typeCls1->isPredefType(PT_OBJECT))
            typeCls1 = NULL;
    }
    else if (!typeSrc->isInterfaceType()) {
        typeCls1 = typeSrc->asAGGTYPESYM();
    }
    else {
        ASSERT(typeSrc->isInterfaceType());
        ASSERT(!(m_flags & (MemLookFlags::Ctor | MemLookFlags::NewObj | MemLookFlags::Operator | MemLookFlags::BaseCall)));
        typeIface = typeSrc->asAGGTYPESYM();
        ifaces = typeIface->GetIfacesAll();
    }

    if (typeIface || ifaces->size)
        typeCls2 = compiler->GetReqPredefType(PT_OBJECT);

    // Search the class first (except possibly object).
    if (!typeCls1 || LookupInClass(typeCls1, &typeCls2)) {
        // Search the interfaces.
        if ((typeIface || ifaces->size) && LookupInInterfaces(typeIface, ifaces) && typeCls2) {
            // Search object last.
            ASSERT(typeCls2 && typeCls2->isPredefType(PT_OBJECT));
            LookupInClass(typeCls2, NULL);
        }
    }

    ASSERT(!m_swtFirst == !m_ctype && (!m_swtFirst || m_prgtype[0] == m_swtFirst.Type()));
    ASSERT(m_fMulti == (m_swtFirst && (m_swtFirst.Sym()->isMETHSYM() ||
        m_swtFirst.Sym()->isPROPSYM() && m_swtFirst.Prop()->isIndexer())));

    return !FError();
}


/***************************************************************************************************
    Another match was found. Increment the count of syms and add the type to our list if it's not
    already there.
***************************************************************************************************/
void MemberLookup::RecordType(AGGTYPESYM * type, SYM * sym)
{
    ASSERT(type && sym);
    ASSERT(!m_swtFirst || m_swtFirst.Sym()->getKind() == sym->getKind());

    // This type shouldn't already be there.
    if (m_ctype == 0 || m_prgtype[m_ctype - 1] != type) {
        // Make sure there is enough room.
        ASSERT(m_ctype <= m_ctypeMax);
        if (m_ctype == m_ctypeMax) {
            // NOTE: We'd like to allocate this on the stack but we can't!
            AGGTYPESYM ** prgtypeNew = (AGGTYPESYM **)compiler()->getLocalSymHeap()->Alloc(sizeof(TYPESYM *) * m_ctypeMax * 2);
            memcpy(prgtypeNew, m_prgtype, m_ctypeMax * sizeof(m_prgtype[0]));
            m_ctypeMax *= 2;
            m_prgtype = prgtypeNew;
        }

        // Add the type.
        ASSERT(m_ctype < m_ctypeMax);
        m_prgtype[m_ctype++] = type;
    }

    // Now record the sym....

    m_csym++;

    // If it is first, record it.
    if (!m_swtFirst) {
        m_swtFirst.Set(sym, type);
        ASSERT(m_csym == 1);
        ASSERT(m_ctype == 1 && m_prgtype[0] == type);
        m_fMulti = sym->isMETHSYM() || sym->isPROPSYM() && sym->asPROPSYM()->isIndexer();
    }
}


/******************************************************************************
    Search just the given type (not any bases). Returns true iff it finds
    something (which will have been recorded by RecordType).

    *pfHideByName is set to true iff something was found that hides all
    members of base types (eg, a hidebyname method).
******************************************************************************/
bool MemberLookup::SearchSingleType(AGGTYPESYM * typeCur, bool * pfHideByName)
{
    bool fFoundSome = false;

    *pfHideByName = false;

    // Make sure this type is accessible. It may not be due to private inheritance
    // or friend assemblies.
    bool fInaccess = !compiler()->clsDeclRec.CheckTypeAccess(typeCur, m_symWhere);
    if (fInaccess && (m_csym || m_swtInaccess))
        return false;

    // Loop through symbols.
    SYM * symCur = NULL;
    for (symCur = compiler()->getBSymmgr().LookupAggMember(m_name, typeCur->getAggregate(), MASK_ALL);
        symCur != NULL;
        symCur = compiler()->getBSymmgr().LookupNextSym(symCur, typeCur->getAggregate(), MASK_ALL))
    {
        // Check for arity.
        switch (symCur->getKind()) {
        case SK_METHSYM:
            // For non-zero arity, only methods of the correct arity are considered.
            // For zero arity, don't filter out any methods since we do type argument
            // inferencing.
            if (m_arity && symCur->asMETHSYM()->typeVars->size != m_arity)
                goto LBadArity;
            break;

        case SK_AGGSYM:
            // For types, always filter on arity.
            if (symCur->asAGGSYM()->typeVarsThis->size != m_arity)
                goto LBadArity;
            break;

        case SK_TYVARSYM:
            if (!(m_flags & MemLookFlags::TypeVarsAllowed))
                continue;
            // Fall through
        default:
            // All others are only considered when arity is zero.
            if (m_arity) {
LBadArity:
                if (!m_swtBadArity)
                    m_swtBadArity.Set(symCur, typeCur);
                continue;
            }
            break;
        }

        // Check for user callability.
        if (symCur->IsOverride() && !symCur->IsHideByName())
            continue;
        if ((m_flags & MemLookFlags::UserCallable) && symCur->isMETHPROPSYM() && !symCur->asMETHPROPSYM()->isUserCallable()) {
            if (!m_swtInaccess)
                m_swtInaccess.Set(symCur, typeCur);
            continue;
        }

        if (fInaccess || !compiler()->clsDeclRec.CheckAccess(symCur, typeCur, m_symWhere, m_typeQual)) {
            // Not accessible so get the next sym.
            if (!m_swtInaccess)
                m_swtInaccess.Set(symCur, typeCur);
            if (fInaccess)
                return false;
            continue;
        }

        // Make sure that whether we're seeing a ctor, operator, or indexer is consistent with the flags.
        if (!(m_flags & MemLookFlags::Ctor) != (!symCur->isMETHSYM() || !symCur->asMETHSYM()->isCtor()) ||
            !(m_flags & MemLookFlags::Operator) != (!symCur->isMETHSYM() || !symCur->asMETHSYM()->isOperator) ||
            !(m_flags & MemLookFlags::Indexer) != (!symCur->isPROPSYM() || !symCur->asPROPSYM()->isIndexer()))
        {
            if (!m_swtBad)
                m_swtBad.Set(symCur, typeCur);
            continue;
        }

        if (!symCur->isMETHSYM() && !(m_flags & MemLookFlags::Indexer) && compiler()->CheckBogus(symCur)) {
            // A bogus member - we can't use these, so only record them for error reporting.
            if (!m_swtBogus)
                m_swtBogus.Set(symCur, typeCur);
            continue;
        }

        // We have a visible symbol.
        fFoundSome = true;

        if (m_swtFirst) {
            ASSERT(m_ctype > 0);

            if (!typeCur->isInterfaceType()) {
                // Non-interface case.
                ASSERT(m_fMulti || typeCur == m_prgtype[0]);
                if (!m_fMulti) {
                    if (m_swtFirst.Sym()->isMEMBVARSYM() && symCur->isEVENTSYM()
                        // This is not a problem for the compiler because the field is only
                        // accessible in the scope in whcih it is declared,
                        // but in the EE we ignore accessibility...
                        && m_swtFirst.Field()->isEvent
                        )
                    {
                        // m_swtFirst is just the field behind the event symCur so ignore symCur.
                        continue;
                    }
                    goto LAmbig;
                }
                if (m_swtFirst.Sym()->getKind() != symCur->getKind()) {
                    if (typeCur == m_prgtype[0])
                        goto LAmbig;
                    // This one is hidden by the first one. This one also hides any more in base types.
                    *pfHideByName = true;
                    continue;
                }
            }
            else if (!m_fMulti) {
                if (/* !compiler()->options.fLookupHack ||*/ !symCur->isMETHSYM())
                    goto LAmbig;
                m_swtAmbigWarn = m_swtFirst;
                // Erase previous results so we'll record this method as the first.
                m_ctype = 0;
                m_csym = 0;
                m_swtFirst.Clear();
                m_swtAmbig.Clear();
            }
            else if (m_swtFirst.Sym()->getKind() != symCur->getKind()) {
                if (!typeCur->fDiffHidden) {
                    if (/*!compiler()->options.fLookupHack ||*/ !m_swtFirst.Sym()->isMETHSYM())
                        goto LAmbig;
                    if (!m_swtAmbigWarn)
                        m_swtAmbigWarn.Set(symCur, typeCur);
                }
                // This one is hidden by another. This one also hides any more in base types.
                *pfHideByName = true;
                continue;
            }
        }

        RecordType(typeCur, symCur);

        if (symCur->isMETHPROPSYM() && symCur->asMETHPROPSYM()->isHideByName)
            *pfHideByName = true;

        // We've found a symbol in this type but need to make sure there aren't any conflicting
        // syms here, so keep searching the type.
    }

    ASSERT(!fInaccess || !fFoundSome);

    return fFoundSome;

LAmbig:
    // Ambiguous!
    if (!m_swtAmbig)
        m_swtAmbig.Set(symCur, typeCur);
    *pfHideByName = true;
    return true;
}


/******************************************************************************
    Lookup in a class and its bases (until *ptypeEnd is hit).
    
    ptypeEnd [in/out] - *ptypeEnd should be either NULL or object. If we find
        something here that would hide members of object, this sets *ptypeEnd
        to NULL.

    Returns true when searching should continue to the interfaces.
******************************************************************************/
bool MemberLookup::LookupInClass(AGGTYPESYM * typeStart, AGGTYPESYM ** ptypeEnd)
{
    ASSERT(!m_swtFirst || m_fMulti);
    ASSERT(typeStart && !typeStart->isInterfaceType() && (!ptypeEnd || typeStart != *ptypeEnd));

    AGGTYPESYM * typeEnd = ptypeEnd ? *ptypeEnd : NULL;
    AGGTYPESYM * typeCur;

    // Loop through types. Loop until we hit typeEnd (object or NULL).
    for (typeCur = typeStart; typeCur != typeEnd && typeCur; typeCur = typeCur->GetBaseClass()) {
        ASSERT(!typeCur->isInterfaceType());
        compiler()->EnsureState(typeCur);

        bool fHideByName = false;

        SearchSingleType(typeCur, &fHideByName);
        m_flags &= ~MemLookFlags::TypeVarsAllowed;

        if (m_swtFirst && !m_fMulti) {
            // Everything below this type and in interfaces is hidden.
            return false;
        }

        if (fHideByName) {
            // This hides everything below it and in object, but not in the interfaces!
            if (ptypeEnd)
                *ptypeEnd = NULL;
            // Return true to indicate that it's ok to search additional types.
            return true;
        }

        if (m_flags & MemLookFlags::Ctor) {
            // If we're looking for a constructor, don't check base classes or interfaces.
            return false;
        }
    }

    ASSERT(typeCur == typeEnd);
    return true;
}


/******************************************************************************
    Returns true if searching should continue to object.
******************************************************************************/
bool MemberLookup::LookupInInterfaces(AGGTYPESYM * typeStart, TypeArray * types)
{
    ASSERT(!m_swtFirst || m_fMulti);
    ASSERT(!typeStart || typeStart->isInterfaceType());
    ASSERT(typeStart || types->size);
    ASSERT(!(m_flags & (MemLookFlags::Ctor | MemLookFlags::Operator | MemLookFlags::BaseCall)));

    // Clear all the hidden flags. Anything found in a class hides any other
    // kind of member in all the interfaces.
    if (typeStart) {
        typeStart->fAllHidden = false;
        typeStart->fDiffHidden = (m_swtFirst != NULL);
    }

    for (int i = 0; i < types->size; i++) {
        AGGTYPESYM * type = types->Item(i)->asAGGTYPESYM();
        ASSERT(type->isInterfaceType());
        type->fAllHidden = false;
        type->fDiffHidden = !!m_swtFirst;
    }

    if (typeStart)
        compiler()->EnsureState(typeStart);
    if (types)
        compiler()->EnsureState(types);

    bool fHideObject = false;
    AGGTYPESYM * typeCur = typeStart;
    int itypeNext = 0;

    if (!typeCur) {
        typeCur = types->Item(itypeNext++)->asAGGTYPESYM();
    }
    ASSERT(typeCur);

    // Loop through the interfaces.
    for (;;) {
        ASSERT(typeCur && typeCur->isInterfaceType());

        bool fHideByName = false;

        if (!typeCur->fAllHidden && SearchSingleType(typeCur, &fHideByName)) {
            fHideByName |= !m_fMulti;

            // Mark base interfaces appropriately.
            TypeArray * ifaces = typeCur->GetIfacesAll();
            for (int i = 0; i < ifaces->size; i++) {
                AGGTYPESYM * type = ifaces->Item(i)->asAGGTYPESYM();
                ASSERT(type->isInterfaceType());
                if (fHideByName)
                    type->fAllHidden = true;
                type->fDiffHidden = true;
            }

            // If we hide all base types, that includes object!
            if (fHideByName)
                fHideObject = true;
        }
        m_flags &= ~MemLookFlags::TypeVarsAllowed;

        if (itypeNext >= types->size)
            return !fHideObject;

        // Substitution has already been done.
        typeCur = types->Item(itypeNext++)->asAGGTYPESYM();
    }
}


/******************************************************************************
    Return a type array holding all the types that contain results. Overload
    resolution uses this.
******************************************************************************/
TypeArray * MemberLookup::GetAllTypes()
{
    ASSERT(m_fValid);
    return compiler()->getBSymmgr().AllocParams(m_ctype, (TYPESYM **)m_prgtype);
}


/***************************************************************************************************
    It's only valid to call this when there is no error and the first element found is an
    AGGSYM. The size of typeArgs must match the arity.
***************************************************************************************************/
AGGTYPESYM * MemberLookup::SymFirstAsAts(TypeArray * typeArgs)
{
    ASSERT(typeArgs);
    ASSERT(m_fValid);
    ASSERT(m_swtFirst && m_swtFirst.Sym()->isAGGSYM() &&
        m_swtFirst.Sym()->asAGGSYM()->typeVarsThis->size == m_arity);
    ASSERT(m_arity == typeArgs->size);
    ASSERT(m_prgtype[0] && m_prgtype[0]->getAggregate() == m_swtFirst.Sym()->asAGGSYM()->GetOuterAgg());

    return compiler()->getBSymmgr().GetInstAgg(m_swtFirst.Sym()->asAGGSYM(), m_prgtype[0], typeArgs);
}


/******************************************************************************
    Reports errors. Only call this if FError() is true.
******************************************************************************/
void MemberLookup::ReportErrors(BASENODE * tree)
{
    ASSERT(m_fValid);
    ASSERT(FError());

    // Report error.
    // NOTE: If the definition of FError changes, this code will need to change.
    ASSERT(!m_swtFirst || m_swtAmbig);

    if (m_swtFirst) {
        // Ambiguous lookup.
        compiler()->ErrorRef(tree, ERR_AmbigMember, m_swtFirst, m_swtAmbig);
    }
    else if (m_typeSrc->isDelegateType() && m_name == compiler()->namemgr->GetPredefName(PN_INVOKE)) {
        compiler()->Error(tree && tree->kind == NK_DOT ? tree->asANYBINOP()->p2 : tree, ERR_DontUseInvoke);
    }
    else if (m_swtInaccess) {
        if (!m_swtInaccess.Sym()->isUserCallable() && (m_flags & MemLookFlags::UserCallable))
            compiler()->Error(tree, ERR_CantCallSpecialMethod, m_swtInaccess);
        else
            compiler()->clsDeclRec.ReportAccessError(tree, m_swtInaccess, m_symWhere, m_typeQual);
    }
    else if (m_flags & MemLookFlags::Ctor) {
            compiler()->Error(tree, ERR_NoConstructors, m_typeSrc->getAggregate());
    }
    else if (m_flags & MemLookFlags::Operator) {
        compiler()->Error(tree, ERR_NoSuchMember, m_typeSrc, m_name);
    }
    else if (m_flags & MemLookFlags::Indexer) {
        compiler()->Error(tree, ERR_BadIndexLHS, m_typeSrc);
    }
    else if (m_swtBad) {
        compiler()->Error(tree, ERR_CantCallSpecialMethod, m_swtBad);
    }
    else if (m_swtBogus) {
        ReportBogus(tree, m_swtBogus);
    }
    else if (m_swtBadArity) {
        int cvar;

        switch (m_swtBadArity.Sym()->getKind()) {
        case SK_METHSYM:
            ASSERT(m_arity);
            cvar = m_swtBadArity.Sym()->asMETHSYM()->typeVars->size;
            compiler()->ErrorRef(tree, cvar > 0 ? ERR_BadArity : ERR_HasNoTypeVars,
                m_swtBadArity, ErrArgSymKind(m_swtBadArity.Sym()), cvar);
            break;
        case SK_AGGSYM:
            cvar = m_swtBadArity.Sym()->asAGGSYM()->typeVarsThis->size;
            compiler()->ErrorRef(tree, cvar > 0 ? ERR_BadArity : ERR_HasNoTypeVars,
                m_swtBadArity, ErrArgSymKind(m_swtBadArity.Sym()), cvar);
            break;
        default:
            ASSERT(m_arity);
            compiler()->funcBRec.ReportTypeArgsNotAllowedError(tree, m_arity, m_swtBadArity, ErrArgSymKind(m_swtBadArity.Sym()));
            break;
        }
    }
    else {
        compiler()->Error(tree, ERR_NoSuchMember, m_typeSrc, m_name);
    }
}


void MemberLookup::ReportBogus(BASENODE * tree, SymWithType swt)
{
    ASSERT(swt.Sym()->hasBogus() && swt.Sym()->checkBogus());

    METHSYM * meth1;
    METHSYM * meth2;

    switch (swt.Sym()->getKind()) {
    case SK_EVENTSYM:
        if (swt.Event()->useMethInstead) {
            meth1 = swt.Event()->methAdd;
            meth2 = swt.Event()->methRemove;
            goto LUseAccessors;
        }
        break;

    case SK_PROPSYM:
        if (swt.Prop()->useMethInstead) {
            meth1 = swt.Prop()->methGet;
            meth2 = swt.Prop()->methSet;
LUseAccessors:
            if (meth1 && meth2) {
                compiler()->Error(tree, ERR_BindToBogusProp2, swt.Sym()->name->text,
                    SymWithType(meth1, swt.Type()), SymWithType(meth2, swt.Type()),
                    ErrArgRefOnly(swt.Sym()));
                return;
            }
            if (meth1 || meth2) {
                compiler()->Error(tree, ERR_BindToBogusProp1, swt.Sym()->name->text,
                    SymWithType(meth1 ? meth1 : meth2, swt.Type()),
                    ErrArgRefOnly(swt.Sym()));
                return;
            }
            VSFAIL("useMethInstead is set, but there are no accessors to use?");
        }
        break;

    case SK_METHSYM:
        if (swt.Meth()->name == compiler()->namemgr->GetPredefName(PN_INVOKE) && swt.Meth()->getClass()->IsDelegate()) {
            swt.Set(swt.Type() ? (SYM *)swt.Type() : swt.Meth()->getClass(), NULL);
        }
        break;

    default:
        if (swt.Sym()->isTYPESYM()) {
            compiler()->Error(tree, ERR_BogusType, swt);
            return;
        }
        break;
    }

    // Generic bogus error.
    compiler()->ErrorRef(tree, ERR_BindToBogus, swt);
}


/******************************************************************************
    Reports warnings after a successful lookup. Only call this if FError() is
    false.
******************************************************************************/
void MemberLookup::ReportWarnings(BASENODE * tree)
{
    ASSERT(m_fValid);
    ASSERT(!FError());

    if (m_swtAmbigWarn) {
        // Ambiguous lookup.
        compiler()->ErrorRef(tree, WRN_AmbigLookupMeth, m_swtFirst, m_swtAmbigWarn);
    }
}


/******************************************************************************
    Fills in a member group from the results of the lookup. Only call this
    if the result is a method group or indexer group. Assume no type arguments.
******************************************************************************/
void MemberLookup::FillGroup(EXPRMEMGRP * grp)
{
    ASSERT(m_fValid); 
    ASSERT(!FError());

    // This should only be called if we found a method group or indexer group.
    ASSERT(m_fMulti && m_swtFirst && (m_swtFirst.Sym()->isMETHSYM() || m_swtFirst.Sym()->isPROPSYM() && m_swtFirst.Prop()->isIndexer()));

    grp->name = m_name;
    grp->typeArgs = BSYMMGR::EmptyTypeArray();
    grp->sk = m_swtFirst.Sym()->getKind();
    grp->typePar = m_typeSrc;
    grp->mps = NULL;
    grp->object = m_obj;
    grp->types = compiler()->getBSymmgr().AllocParams(m_ctype, (TYPESYM **)m_prgtype);
    grp->flags |= m_flags;
}

