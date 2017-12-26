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
// File: nullable.cpp
//
// Routines for nullable lifting.
// ===========================================================================

#include "stdafx.h"


/***************************************************************************************************
    Return true iff the method is the nullable ctor taking one parameter.
***************************************************************************************************/
bool FUNCBREC::IsNubCtor(METHSYM * meth)
{
    return meth && meth->getClass()->isPredefAgg(PT_G_OPTIONAL) && meth->params->size == 1 &&
        meth->params->Item(0)->isTYVARSYM() && meth->isCtor();
}


/***************************************************************************************************
    Return true iff the expr is an invocation of the nullable ctor.
***************************************************************************************************/
bool FUNCBREC::IsNubCtor(EXPR * expr)
{
    // Note: If the constructor call is used to init an object, we can't strip it off!
    return expr->kind == EK_CALL && !expr->asCALL()->object && IsNubCtor(expr->asCALL()->mwi.Meth());
}


/***************************************************************************************************
    If the expr is new T?(t) reduce it to t.
***************************************************************************************************/
EXPR * FUNCBREC::StripNubCtor(EXPR * expr)
{
    while (IsNubCtor(expr)) {
        expr = expr->asCALL()->args;
        ASSERT(expr && expr->kind != EK_LIST);
    }
    return expr;
}


/***************************************************************************************************
    Make sure the HasValue property of System.Nullable<T> is appropriate (and return it).
***************************************************************************************************/
PROPSYM * FUNCBREC::EnsureNubHasValue(BASENODE * tree)
{
    PROPSYM * prop = compiler()->getBSymmgr().propNubHasValue;

    if (!prop) {
        AGGSYM * aggNub = compiler()->GetOptPredefAggErr(PT_G_OPTIONAL, true);
        if (!aggNub)
            return NULL;
        NAME * name = compiler()->namemgr->GetPredefName(PN_HASVALUE);

        prop = compiler()->getBSymmgr().LookupAggMember(name, aggNub, MASK_PROPSYM)->asPROPSYM();
        if (!prop || prop->isStatic || prop->GetAccess() != ACC_PUBLIC ||
            prop->params->size || !prop->retType->isPredefType(PT_BOOL) || !prop->methGet)
        {
            compiler()->Error(tree, ERR_MissingPredefinedMember, aggNub, name);
            return NULL;
        }
        compiler()->getBSymmgr().propNubHasValue = prop;
    }

    return prop;
}


/***************************************************************************************************
    Create an expr for exprSrc.HasValue where exprSrc->type is a NUBSYM. If fCheckTrue is false,
    invert the result.
***************************************************************************************************/
EXPR * FUNCBREC::BindNubHasValue(BASENODE * tree, EXPR * exprSrc, bool fCheckTrue)
{
    ASSERT(exprSrc && exprSrc->type->isNUBSYM());

    TYPESYM * typeBool = GetReqPDT(PT_BOOL);

    // When exprSrc is a null, the result is false
    if (exprSrc->GetConst())
        return AddSideEffects(tree, newExprConstant(tree, typeBool, ConstValInit(!fCheckTrue)), exprSrc, true, true);

    // For new T?(x), the answer is true.
    if (IsNubCtor(exprSrc))
        return AddSideEffects(tree, newExprConstant(tree, typeBool, ConstValInit(fCheckTrue)), StripNubCtor(exprSrc), true, true);

    AGGTYPESYM * ats = exprSrc->type->asNUBSYM()->GetAts();
    if (!ats)
        return newError(tree, typeBool);
    compiler()->EnsureState(ats);

    PROPSYM * prop = EnsureNubHasValue(tree);
    if (!prop)
        return newError(tree, typeBool);

    CheckFieldUse(exprSrc);

    EXPRPROP * exprRes = newExpr(tree, EK_PROP, typeBool)->asPROP();

    exprRes->pwtSlot.Set(prop, ats);
    exprRes->mwtGet.Set(prop->methGet, ats);
    exprRes->args = NULL;
    exprRes->object = exprSrc;

    if (fCheckTrue)
        return exprRes;

    return newExprBinop(tree, EK_LOGNOT, exprRes->type, exprRes, NULL);
}


/***************************************************************************************************
    Create an expr for exprSrc.Value where exprSrc->type is a NUBSYM.
***************************************************************************************************/
EXPR * FUNCBREC::BindNubValue(BASENODE * tree, EXPR * exprSrc)
{
    ASSERT(exprSrc && exprSrc->type->isNUBSYM());

    // For new T?(x), the answer is x.
    if (IsNubCtor(exprSrc)) {
        ASSERT(exprSrc->asCALL()->args && exprSrc->asCALL()->args->kind != EK_LIST);
        return exprSrc->asCALL()->args;
    }

    TYPESYM * typeBase = exprSrc->type->asNUBSYM()->baseType();
    AGGTYPESYM * ats = exprSrc->type->asNUBSYM()->GetAts();
    if (!ats)
        return newError(tree, typeBase);
    compiler()->EnsureState(ats);

    PROPSYM * prop = compiler()->getBSymmgr().propNubValue;

    if (!prop) {
        NAME * name = compiler()->namemgr->GetPredefName(PN_CAP_VALUE);

        prop = compiler()->getBSymmgr().LookupAggMember(name, ats->getAggregate(), MASK_PROPSYM)->asPROPSYM();
        if (!prop || prop->isStatic || prop->GetAccess() != ACC_PUBLIC ||
            prop->params->size || !prop->retType->isTYVARSYM() || !prop->methGet)
        {
            compiler()->Error(tree, ERR_MissingPredefinedMember, ats, name);
            return newError(tree, typeBase);
        }
        compiler()->getBSymmgr().propNubValue = prop;
    }

    CheckFieldUse(exprSrc);

    EXPRPROP * exprRes = newExpr(tree, EK_PROP, typeBase)->asPROP();

    exprRes->pwtSlot.Set(prop, ats);
    exprRes->mwtGet.Set(prop->methGet, ats);
    exprRes->args = NULL;
    exprRes->object = exprSrc;

    return exprRes;
}


/***************************************************************************************************
    Make sure the HasValue property of System.Nullable<T> is appropriate (and return it).
***************************************************************************************************/
METHSYM * FUNCBREC::EnsureNubGetValOrDef(BASENODE * tree)
{
    METHSYM * meth = compiler()->getBSymmgr().methNubGetValOrDef;

    if (!meth) {
        AGGSYM * aggNub = compiler()->GetOptPredefAggErr(PT_G_OPTIONAL, true);
        if (!aggNub)
            return NULL;
        NAME * name = compiler()->namemgr->GetPredefName(PN_GET_VALUE_OR_DEF);

        for (SYM * sym = compiler()->getBSymmgr().LookupAggMember(name, aggNub, MASK_ALL); ; sym = sym->nextSameName) {
            if (!sym) {
                compiler()->Error(tree, ERR_MissingPredefinedMember, aggNub, name);
                return NULL;
            }
            if (sym->isMETHSYM()) {
                meth = sym->asMETHSYM();
                if (meth->params->size == 0 && meth->typeVars->size == 0 && meth->retType->isTYVARSYM() &&
                    !meth->isStatic && meth->GetAccess() == ACC_PUBLIC)
                {
                    break;
                }
            }
        }
        compiler()->getBSymmgr().methNubGetValOrDef = meth;
    }

    return meth;
}


/***************************************************************************************************
    Create an expr for exprSrc.GetValueOrDefault() where exprSrc->type is a NUBSYM.
***************************************************************************************************/
EXPR * FUNCBREC::BindNubGetValOrDef(BASENODE * tree, EXPR * exprSrc)
{
    ASSERT(exprSrc && exprSrc->type->isNUBSYM());

    TYPESYM * typeBase = exprSrc->type->asNUBSYM()->baseType();

    // If exprSrc is null, just return the appropriate default value.
    if (exprSrc->GetConst())
        return AddSideEffects(tree, newExprZero(tree, typeBase), exprSrc, true, true);

    // For new T?(x), the answer is x.
    if (IsNubCtor(exprSrc)) {
        ASSERT(exprSrc->asCALL()->args && exprSrc->asCALL()->args->kind != EK_LIST);
        return exprSrc->asCALL()->args;
    }

    AGGTYPESYM * ats = exprSrc->type->asNUBSYM()->GetAts();
    if (!ats)
        return newError(tree, typeBase);
    compiler()->EnsureState(ats);

    METHSYM * meth = EnsureNubGetValOrDef(tree);
    if (!meth)
        return newError(tree, typeBase);

    CheckFieldUse(exprSrc);

    EXPRCALL * exprRes = newExpr(tree, EK_CALL, typeBase)->asCALL();

    exprRes->mwi.Set(meth, ats, BSYMMGR::EmptyTypeArray());
    exprRes->args = NULL;
    exprRes->object = exprSrc;

    return exprRes;
}


/***************************************************************************************************
    Create an expr for new T?(exprSrc) where T is exprSrc->type.
***************************************************************************************************/
EXPR * FUNCBREC::BindNubNew(BASENODE * tree, EXPR * exprSrc)
{
    ASSERT(exprSrc);

    NUBSYM * nub = compiler()->getBSymmgr().GetNubType(exprSrc->type);

    AGGTYPESYM * ats = nub->GetAts();
    if (!ats)
        return newError(tree, nub);
    compiler()->EnsureState(ats);

    METHSYM * meth = compiler()->getBSymmgr().methNubCtor;

    if (!meth) {
        NAME * name = compiler()->namemgr->GetPredefName(PN_CTOR);

        for (SYM * sym = compiler()->getBSymmgr().LookupAggMember(name, ats->getAggregate(), MASK_ALL); ; sym = sym->nextSameName) {
            if (!sym) {
                compiler()->Error(tree, ERR_MissingPredefinedMember, ats, name);
                return newError(tree, nub);
            }
            if (sym->isMETHSYM()) {
                meth = sym->asMETHSYM();
                if (meth->params->size == 1 && meth->params->Item(0)->isTYVARSYM() && meth->GetAccess() == ACC_PUBLIC)
                    break;
            }
        }
        compiler()->getBSymmgr().methNubCtor = meth;
    }

    EXPRCALL * exprRes = newExpr(tree, EK_CALL, nub)->asCALL();

    exprRes->mwi.Set(meth, ats, BSYMMGR::EmptyTypeArray());
    exprRes->args = exprSrc;
    exprRes->object = NULL;
    exprRes->flags |= EXF_NEWOBJCALL | EXF_CANTBENULL;

    return exprRes;
}


/***************************************************************************************************
    Fill in the given slot (iexpr) of the NubInfo appropriately. If exprSrc is a constant (possibly
    with side effects), put the side effects in nin.rgexprPre[iexpr] and the constant value in
    nin.rgexprVal[iexpr]. Otherwise, construct an expr to save the value of exprSrc in a temp,
    an expr to load the temp and an expr to free the temp. Store these in the appropriate places
    in the NubInfo. If fLift is true, the value saved in the temp will have at most one level
    of Nullable.
***************************************************************************************************/
bool FUNCBREC::BindNubSave(EXPR * exprSrc, NubInfo & nin, int iexpr, bool fLift)
{
    ASSERT(0 <= iexpr && iexpr <= 1);
    ASSERT(
        !nin.rgexprPre[iexpr] &&
        !nin.rgexprPst[iexpr] &&
        !nin.rgexprVal[iexpr] &&
        !nin.rgexprTmp[iexpr] &&
        !nin.rgexprCnd[iexpr]);

    // If we're lifting the expr and it's a new T?(t), just operate on t. The caller should have
    // already taken care of calling StripNubCtor!
    ASSERT(!exprSrc->type->isNUBSYM() || !fLift || !IsNubCtor(exprSrc));

    // Check for an EK_CONSTANT or EK_ZEROINIT inside EK_SEQUENCE and EK_SEQREV exprs.
    EXPR * exprConst = exprSrc->GetConst();
    if (exprConst) {
        ASSERT(exprConst->kind == EK_CONSTANT || exprConst->kind == EK_ZEROINIT);
        if (exprConst != exprSrc) {
            // Keep the side effects.
            nin.rgexprPre[iexpr] = exprSrc;
        }
        if (!exprConst->type->isNUBSYM() || !fLift)
            nin.rgexprTmp[iexpr] = exprConst;
        else
            nin.rgexprTmp[iexpr] = newExprZero(exprSrc->tree, exprConst->type->StripAllButOneNub());
        return false;
    }

    if (exprSrc->type->isNUBSYM() && fLift) {
        while (exprSrc->type->parent->isNUBSYM())
            exprSrc = BindNubGetValOrDef(exprSrc->tree, exprSrc);
    }

    if (exprSrc->kind == EK_LDTMP) {
        // The thing is already in a temp, no need to put it in another.
        nin.rgexprPre[iexpr] = exprSrc;
        nin.rgexprTmp[iexpr] = exprSrc;
        return true;
    }

    // Create the temp.
    EXPRSTTMP * exprStTmp = newExpr(exprSrc->tree, EK_STTMP, exprSrc->type)->asSTTMP();
    EXPRLDTMP * exprLdTmp = newExpr(exprSrc->tree, EK_LDTMP, exprSrc->type)->asLDTMP();
    EXPRFREETMP * exprFreeTmp = newExpr(exprSrc->tree, EK_FREETMP, exprSrc->type)->asFREETMP();

    exprStTmp->src = exprSrc;
    exprStTmp->flags |= EXF_ASSGOP;
    exprLdTmp->tmp = exprStTmp;
    exprFreeTmp->tmp = exprStTmp;
    exprFreeTmp->flags |= EXF_ASSGOP;

    nin.rgexprPre[iexpr] = exprStTmp;
    nin.rgexprPst[iexpr] = exprFreeTmp;
    nin.rgexprTmp[iexpr] = exprLdTmp;

    return true;
}


/***************************************************************************************************
    If the given slot has a temp associated with it, constructs a reverse sequence for loading
    the value and freeing the temp. Otherwise, just returns the value expr.
***************************************************************************************************/
EXPR * FUNCBREC::BindNubFetchAndFree(BASENODE * tree, NubInfo & nin, int iexpr)
{
    EXPR * expr = nin.rgexprTmp[iexpr];
    ASSERT(expr);

    if (!nin.rgexprPst[iexpr])
        return expr;

    return AddSideEffects(tree, expr, nin.rgexprPst[iexpr], false, true);
}


/***************************************************************************************************
    Fill in the NubInfo for a unary or binary operator lifting.
***************************************************************************************************/
void FUNCBREC::BindNubCondValBin(BASENODE * tree, EXPR * expr1, EXPR * expr2, NubInfo & nin, LiftFlagsEnum grflt)
{
    ASSERT(grflt & (LiftFlags::Lift1 | LiftFlags::Lift2));
    ASSERT(expr1);
    ASSERT(expr2 || (grflt & LiftFlags::Lift1));

    nin.Init();

    bool rgfLift[2];
    rgfLift[0] = !!(grflt & LiftFlags::Lift1);
    rgfLift[1] = expr2 && !!(grflt & LiftFlags::Lift2);

    EXPR * rgexpr[2];
    rgexpr[0] = rgfLift[0] ? StripNubCtor(expr1) : expr1;
    rgexpr[1] = rgfLift[1] ? StripNubCtor(expr2) : expr2;

    if ((!rgfLift[0] || !rgexpr[0]->type->isNUBSYM()) && (!rgfLift[1] || !rgexpr[1]->type->isNUBSYM())) {
        // All lifted params can't be null so we don't need temps.
        nin.rgexprVal[0] = rgexpr[0];
        nin.rgexprVal[1] = rgexpr[1];
        nin.fActive = true;
        nin.fAlwaysNonNull = true;
        return;
    }

    // Optimization: if they are the same local then we only need one temp.
    if (rgexpr[1] && rgexpr[0]->kind == EK_LOCAL && rgexpr[1]->kind == EK_LOCAL &&
        rgexpr[0]->asLOCAL()->local == rgexpr[1]->asLOCAL()->local &&
        rgfLift[0] == rgfLift[1])
    {
        BindNubSave(rgexpr[0], nin, 0, rgfLift[0]);
        ASSERT(nin.rgexprTmp[0]->kind == EK_LDTMP);
        nin.rgexprTmp[1] = nin.rgexprTmp[0];
        nin.fSameTemp = true;
    }
    else {
        BindNubSave(rgexpr[0], nin, 0, rgfLift[0]);
        if (rgexpr[1])
            BindNubSave(rgexpr[1], nin, 1, rgfLift[1]);
    }

    for (int iexpr = 0; iexpr < 2 && rgexpr[iexpr]; iexpr++) {
        if (!rgfLift[iexpr] || !nin.rgexprTmp[iexpr]->type->isNUBSYM()) {
            nin.rgexprVal[iexpr] = nin.rgexprTmp[iexpr];
            continue;
        }
        nin.rgexprVal[iexpr] = BindNubGetValOrDef(tree, nin.rgexprTmp[iexpr]);
        ASSERT(!nin.rgexprVal[iexpr]->type->isNUBSYM());
        if (nin.FConst(iexpr))
            nin.rgfNull[iexpr] = nin.fAlwaysNull = true;
        else if (nin.fSameTemp && iexpr)
            nin.rgexprCnd[iexpr] = nin.rgexprCnd[0];
        else
            nin.rgexprCnd[iexpr] = BindNubHasValue(tree, nin.rgexprTmp[iexpr]);
    }

    if (!nin.fAlwaysNull) {
        if (!nin.rgexprCnd[0])
            nin.exprCnd = nin.rgexprCnd[1];
        else if (!nin.rgexprCnd[1] || nin.fSameTemp)
            nin.exprCnd = nin.rgexprCnd[0];
        else
            nin.exprCnd = newExprBinop(tree, EK_BITAND, GetReqPDT(PT_BOOL), nin.rgexprCnd[0], nin.rgexprCnd[1]);
    }
    else {
        // One of the operands is null so the result will always be null and we
        // don't need the temps.
        if (nin.rgexprPre[0] && nin.rgexprPre[0]->kind == EK_STTMP)
            nin.rgexprPre[0] = nin.rgexprPre[0]->asSTTMP()->src;
        if (nin.rgexprPre[1] && nin.rgexprPre[1]->kind == EK_STTMP)
            nin.rgexprPre[1] = nin.rgexprPre[1]->asSTTMP()->src;
        nin.rgexprPst[0] = NULL;
        nin.rgexprPst[1] = NULL;
    }

    nin.fActive = true;
    ASSERT(nin.exprCnd || nin.fAlwaysNull);
}


/***************************************************************************************************
    Combine the condition and value.
***************************************************************************************************/
EXPR * FUNCBREC::BindNubOpRes(BASENODE * tree, NUBSYM * nubNull, TYPESYM * typeDst, EXPR * exprVal, NubInfo & nin, bool fWarnOnNull)
{
    if (nin.FAlwaysNull() && fWarnOnNull)
        compiler()->Error(tree, WRN_AlwaysNull, nubNull);

    return BindNubOpRes(tree, typeDst, exprVal,
        newExprZero(tree, typeDst->isNUBSYM() ? typeDst : nubNull), nin);
}


/***************************************************************************************************
    Combine the condition and value.
***************************************************************************************************/
EXPR * FUNCBREC::BindNubOpRes(BASENODE * tree, TYPESYM * typeDst, EXPR * exprVal, EXPR * exprNull, NubInfo & nin)
{
    EXPR * exprRes;

    exprNull = mustConvert(exprNull, typeDst);
    exprVal = mustConvert(exprVal, typeDst);

    if (nin.FAlwaysNonNull()) {
        // Don't need exprNull and there aren't any temps.
        exprRes = exprVal;
    }
    else if (nin.FAlwaysNull()) {
        // Don't need exprVal but do need side effects.
        exprRes = BindNubAddTmps(tree, exprNull, nin);
    }
    else {
        ASSERT(nin.exprCnd);
        exprRes = BindQmark(tree, nin.exprCnd, mustConvert(exprVal, typeDst), exprNull);
        exprRes = BindNubAddTmps(tree, exprRes, nin);
    }

    return exprRes;
}


/***************************************************************************************************
    Add exprSide to exprBase as a side effect. If fPre is true, exprSide is evaluated before
    exprBase. If exprSide is NULL or contains no side effects, just returns exprBase.
***************************************************************************************************/
EXPR * FUNCBREC::AddSideEffects(BASENODE * tree, EXPR * exprBase, EXPR * exprSide, bool fPre, bool fForceNonConst)
{
    if (!exprSide)
        return exprBase;
    if (!exprSide->hasSideEffects(compiler())) {
        // Make sure it's not an lvalue or constant (unless exprSide is a constant and !fForceNonConst).
        if (!(exprBase->flags & EXF_LVALUE) && (exprBase->kind != EK_CONSTANT || exprSide->kind == EK_CONSTANT && !fForceNonConst))
            return exprBase;
        return newExprBinop(tree, EK_SEQUENCE, exprBase->type, newExpr(tree, EK_NOOP, getVoidType()), exprBase);
    }
    if (fPre)
        return newExprBinop(tree, EK_SEQUENCE, exprBase->type, exprSide, exprBase);
    return newExprBinop(tree, EK_SEQREV, exprBase->type, exprBase, exprSide);
}


/***************************************************************************************************
    If exprBase is a constant or lvalue, sequence it with EK_NOOP so it doesn't appear to be either.
***************************************************************************************************/
EXPR * FUNCBREC::EnsureNonConstNonLvalue(BASENODE * tree, EXPR * exprBase)
{
    if (!(exprBase->flags & EXF_LVALUE) && exprBase->kind != EK_CONSTANT)
        return exprBase;
    return newExprBinop(tree, EK_SEQUENCE, exprBase->type, newExpr(tree, EK_NOOP, getVoidType()), exprBase);
}


/***************************************************************************************************
    Return an expr for "new bool?(fT)".
***************************************************************************************************/
EXPR * FUNCBREC::BindNubConstBool(BASENODE * tree, bool fT)
{
    EXPR * expr = newExprConstant(tree, GetReqPDT(PT_BOOL), ConstValInit(fT));
    return BindNubNew(tree, expr);
}


/***************************************************************************************************
    Constructs an expr for "exprCond ? exprLeft : exprRight". If fInvert is true, reverses
    exprLeft and exprRight.
    REVIEW ShonK: Optimize BindQmark when the condition is a constant (with possible side effects).
***************************************************************************************************/
EXPR * FUNCBREC::BindQmark(BASENODE * tree, EXPR * exprCond, EXPR * exprLeft, EXPR * exprRight, bool fInvert)
{
    ASSERT(exprCond->type->isPredefType(PT_BOOL));
    ASSERT(exprLeft->type == exprRight->type);

    EXPR * exprColon;

    if (fInvert)
        exprColon = newExprBinop(tree, EK_BINOP, NULL, exprRight, exprLeft);
    else
        exprColon = newExprBinop(tree, EK_BINOP, NULL, exprLeft, exprRight);

    return newExprBinop(tree, EK_QMARK, exprLeft->type, exprCond, exprColon);
}


/***************************************************************************************************
    Combines the pre and post expressions of the NubInfo with exprRes. The pre and post exprs
    are typically to store values to temps and free the temps.
***************************************************************************************************/
EXPR * FUNCBREC::BindNubAddTmps(BASENODE * tree, EXPR * exprRes, NubInfo & nin)
{
    if (nin.rgexprPst[1])
        exprRes = newExprBinop(tree, EK_SEQREV, exprRes->type, exprRes, nin.rgexprPst[1]);
    if (nin.rgexprPst[0])
        exprRes = newExprBinop(tree, EK_SEQREV, exprRes->type, exprRes, nin.rgexprPst[0]);

    if (nin.rgexprPre[1])
        exprRes = newExprBinop(tree, EK_SEQUENCE, exprRes->type, nin.rgexprPre[1], exprRes);
    if (nin.rgexprPre[0])
        exprRes = newExprBinop(tree, EK_SEQUENCE, exprRes->type, nin.rgexprPre[0], exprRes);

    return exprRes;
}


/***************************************************************************************************
    Called by bindImplicitConversion when the destination type is Nullable<T>. The following
    conversions are handled by this method:

    * For S in { object, ValueType, interfaces implemented by underlying type} there is an explicit
      unboxing conversion S => T?
    * System.Enum => T? there is an unboxing conversion if T is an enum type
    * null => T? implemented as default(T?)

    * Implicit T?* => T?+ implemented by either wrapping or calling GetValueOrDefault the
      appropriate number of times.
    * If imp/exp S => T then imp/exp S => T?+ implemented by converting to T then wrapping the
      appropriate number of times.
    * If imp/exp S => T then imp/exp S?+ => T?+ implemented by calling GetValueOrDefault (m-1) times
      then calling HasValue, producing a null if it returns false, otherwise calling Value,
      converting to T then wrapping the appropriate number of times.

    The 3 rules above can be summarized with the following recursive rules:

    * If imp/exp S => T? then imp/exp S? => T? implemented as
      qs.HasValue ? (T?)(qs.Value) : default(T?)
    * If imp/exp S => T then imp/exp S => T? implemented as new T?((T)s)

    This method also handles calling bindUserDefinedConverion. This method does NOT handle
    the following conversions:

    * Implicit boxing conversion from S? to { object, ValueType, Enum, ifaces implemented by S }. (Handled by bindImplicitConversion.)
    * If imp/exp S => T then explicit S?+ => T implemented by calling Value the appropriate number
      of times. (Handled by bindExplicitConversion.)

    The recursive equivalent is:

    * If imp/exp S => T and T is not nullable then explicit S? => T implemented as qs.Value

    Some nullable conversion are NOT standard conversions. In particular, if S => T is implicit
    then S? => T is not standard. Similarly if S => T is not implicit then S => T? is not standard.
***************************************************************************************************/
bool FUNCBREC::BindNubConversion(BASENODE * tree, EXPR * exprSrc, TYPESYM * typeSrc, NUBSYM * nubDst, EXPR ** pexprDst, uint flags)
{
    // This code assumes that STANDARD and ISEXPLICIT are never both set.
    // bindUserDefinedConversion should ensure this!
    ASSERT(~flags & (STANDARD | ISEXPLICIT));

    ASSERT(!exprSrc || exprSrc->type == typeSrc);
    ASSERT(!pexprDst || exprSrc);
    ASSERT(typeSrc != nubDst); // bindImplicitConversion should have taken care of this already.

    AGGTYPESYM * atsDst = nubDst->GetAts();
    if (!atsDst)
        return false;

    // Check for the unboxing conversion. This takes precedence over the wrapping conversions.
    if (compiler()->IsBaseType(nubDst->baseType(), typeSrc) && !FWrappingConv(typeSrc, nubDst)) {
        // These should be different! Fix the caller if typeSrc is an AGGTYPESYM of Nullable.
        ASSERT(atsDst != typeSrc);

        // typeSrc is a base type of the destination nullable type so there is an explicit
        // unboxing conversion.
        if (!(flags & ISEXPLICIT))
            return false;
        return bindSimpleCast(tree, exprSrc, nubDst, pexprDst, EXF_UNBOX);
    }

    int cnubDst;
    int cnubSrc;
    TYPESYM * typeDstBase = nubDst->StripNubs(&cnubDst);
    TYPESYM * typeSrcBase = typeSrc->StripNubs(&cnubSrc);

    bool (FUNCBREC::*pfn)(BASENODE *, EXPR *, TYPESYM *, TYPESYM *, EXPR **, uint) =
        (flags & ISEXPLICIT) ? &FUNCBREC::bindExplicitConversion : &FUNCBREC::bindImplicitConversion;

    if (cnubSrc == 0) {
        ASSERT(typeSrc == typeSrcBase);

        // The null type can be implicitly converted to T? as the default value.
        if (typeSrc->isNULLSYM()) {
            if (pexprDst)
                *pexprDst = AddSideEffects(tree, newExprZero(tree, nubDst), exprSrc, true, true);
            return true;
        }

        EXPR * exprTmp = exprSrc;

        // If there is an implicit/explicit S => T then there is an implicit/explicit S => T?
        if (typeSrc == typeDstBase || (this->*pfn)(tree, exprSrc, typeSrc, typeDstBase, pexprDst ? &exprTmp : NULL, flags | NOUDC)) {
            if (pexprDst) {
                // typeSrc is not nullable so just wrap the required number of times.
                for (int i = 0; i < cnubDst; i++)
                    exprTmp = BindNubNew(tree, exprTmp);
                ASSERT(exprTmp->type == nubDst);
                *pexprDst = exprTmp;
            }
            return true;
        }

        // No builtin conversion. Maybe there is a user defined conversion....
        return !(flags & NOUDC) && bindUserDefinedConversion(tree, exprSrc, typeSrc, nubDst, pexprDst, !(flags & ISEXPLICIT));
    }

    // Both are Nullable so there is only a conversion if there is a conversion between the base types.
    // That is, if there is an implicit/explicit S => T then there is an implicit/explicit S?+ => T?+.
    if (typeSrcBase != typeDstBase && !(this->*pfn)(tree, NULL, typeSrcBase, typeDstBase, NULL, flags | NOUDC)) {
        // No builtin conversion. Maybe there is a user defined conversion....
        return !(flags & NOUDC) && bindUserDefinedConversion(tree, exprSrc, typeSrc, nubDst, pexprDst, !(flags & ISEXPLICIT));
    }

    if (pexprDst) {
        // We need to go all the way down to the base types, do the conversion, then come all the way back up.
        EXPR * exprVal;
        NubInfo nin;

        BindNubCondValBin(tree, exprSrc, NULL, nin);
        exprVal = nin.Val(0);

        ASSERT(exprVal->type == typeSrcBase);

        if (!(this->*pfn)(tree, exprVal, exprVal->type, typeDstBase, &exprVal, flags | NOUDC)) {
            VSFAIL("bind(Im|Ex)plicitConversion failed unexpectedly");
            return false;
        }

        for (int i = 0; i < cnubDst; i++)
            exprVal = BindNubNew(tree, exprVal);
        ASSERT(exprVal->type == nubDst);

        *pexprDst = BindNubOpRes(tree, nubDst, nubDst, exprVal, nin);
    }

    return true;
}
