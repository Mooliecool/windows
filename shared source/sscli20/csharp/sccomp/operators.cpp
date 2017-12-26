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
// File: operators.cpp
//
// Routines for operator overload resolution.
// ===========================================================================


#include "stdafx.h"

/***************************************************************************************************
    These are the predefined binary operator signatures

        (object,    object)     :                   == !=
        (string,    string)     :                   == !=
        (string,    string)     :       +
        (string,    object)     :       +
        (object,    string)     :       +

        (int,       int)        : * / % + - << >>   == != < > <= >= & | ^
        (uint,      uint)       : * / % + -         == != < > <= >= & | ^
        (long,      long)       : * / % + -         == != < > <= >= & | ^
        (ulong,     ulong)      : * / % + -         == != < > <= >= & | ^
        (uint,      int)        :           << >>
        (long,      int)        :           << >>
        (ulong,     int)        :           << >>

        (float,     float)      : * / % + -         == != < > <= >=
        (double,    double)     : * / % + -         == != < > <= >=
        (decimal,   decimal)    : * / % + -         == != < > <= >=

        (bool,      bool)       :                   == !=           & | ^ && ||

        (Sys.Del,   Sys.Del)    :                   == !=

        // Below here the types cannot be represented entirely by a PREDEFTYPE.
        (delegate,  delegate)   :       + -         == !=

        (enum,      enum)       :         -         == != < > <= >= & | ^
        (enum,      under)      :       + -
        (under,     enum)       :       +

        (ptr,       ptr)        :         -
        (ptr,       int)        :       + -
        (ptr,       uint)       :       + -
        (ptr,       long)       :       + -
        (ptr,       ulong)      :       + -
        (int,       ptr)        :       +
        (uint,      ptr)        :       +
        (long,      ptr)        :       +
        (ulong,     ptr)        :       +

        (void*,     void*)      :                   == != < > <= >=

    There are the predefined unary operator signatures:

        int     : + -   ~
        uint    : +     ~
        long    : + -   ~
        ulong   : +     ~

        float   : + -   
        double  : + - 
        decimal : + - 

        bool    :     !

        // Below here the types cannot be represented entirely by a PREDEFTYPE.
        enum    :       ~
        ptr     :         * 

    Note that pointer operators cannot be lifted over nullable.
***************************************************************************************************/

FUNCBREC::BinOpSig FUNCBREC::g_rgbos[] = {
    { PT_INT,       PT_INT,         BinOpMask::Integer,     8,  &FUNCBREC::BindIntBinOp,   OpSigFlags::Value },
    { PT_UINT,      PT_UINT,        BinOpMask::Integer,     7,  &FUNCBREC::BindIntBinOp,   OpSigFlags::Value },
    { PT_LONG,      PT_LONG,        BinOpMask::Integer,     6,  &FUNCBREC::BindIntBinOp,   OpSigFlags::Value },
    { PT_ULONG,     PT_ULONG,       BinOpMask::Integer,     5,  &FUNCBREC::BindIntBinOp,   OpSigFlags::Value },

    // These two are errors.
    { PT_ULONG,     PT_LONG,        BinOpMask::Integer,     4,  NULL,            OpSigFlags::Value },
    { PT_LONG,      PT_ULONG,       BinOpMask::Integer,     3,  NULL,            OpSigFlags::Value },

    { PT_FLOAT,     PT_FLOAT,       BinOpMask::Real,        1,  &FUNCBREC::BindRealBinOp,  OpSigFlags::Value },
    { PT_DOUBLE,    PT_DOUBLE,      BinOpMask::Real,        0,  &FUNCBREC::BindRealBinOp,  OpSigFlags::Value },
    { PT_DECIMAL,   PT_DECIMAL,     BinOpMask::Real,        0,  &FUNCBREC::BindDecBinOp,   OpSigFlags::Value },

    { PT_STRING,    PT_STRING,      BinOpMask::Equal,       0,  &FUNCBREC::BindStrCmpOp,   OpSigFlags::Reference },

    { PT_STRING,    PT_STRING,      BinOpMask::Add,         2,  &FUNCBREC::BindStrBinOp,   OpSigFlags::Reference },
    { PT_STRING,    PT_OBJECT,      BinOpMask::Add,         1,  &FUNCBREC::BindStrBinOp,   OpSigFlags::Reference },
    { PT_OBJECT,    PT_STRING,      BinOpMask::Add,         0,  &FUNCBREC::BindStrBinOp,   OpSigFlags::Reference },

    { PT_INT,       PT_INT,         BinOpMask::Shift,       3,  &FUNCBREC::BindShiftOp,    OpSigFlags::Value },
    { PT_UINT,      PT_INT,         BinOpMask::Shift,       2,  &FUNCBREC::BindShiftOp,    OpSigFlags::Value },
    { PT_LONG,      PT_INT,         BinOpMask::Shift,       1,  &FUNCBREC::BindShiftOp,    OpSigFlags::Value },
    { PT_ULONG,     PT_INT,         BinOpMask::Shift,       0,  &FUNCBREC::BindShiftOp,    OpSigFlags::Value },

    { PT_BOOL,      PT_BOOL,        BinOpMask::BoolNorm,    0,  &FUNCBREC::BindBoolBinOp,  OpSigFlags::Value },
    { PT_BOOL,      PT_BOOL,        BinOpMask::Logical,     0,  &FUNCBREC::BindBoolBinOp,  OpSigFlags::Convert },
    { PT_BOOL,      PT_BOOL,        BinOpMask::Bitwise,     0,  &FUNCBREC::BindBoolBitwiseOp, OpSigFlags::BoolBit },
};


FUNCBREC::UnaOpSig FUNCBREC::g_rguos[] = {
    { PT_INT,       UnaOpMask::Signed,      7,  &FUNCBREC::BindIntUnaOp },
    { PT_UINT,      UnaOpMask::Unsigned,    6,  &FUNCBREC::BindIntUnaOp },
    { PT_LONG,      UnaOpMask::Signed,      5,  &FUNCBREC::BindIntUnaOp },
    { PT_ULONG,     UnaOpMask::Unsigned,    4,  &FUNCBREC::BindIntUnaOp },

    // This is NOT a bug! We want unary minus to bind to "operator -(ulong)" and then we
    // produce an error (since there is no pfn). We can't let - bind to a floating point type,
    // since they lose precision. See the language spec. 
    { PT_ULONG,     UnaOpMask::Minus,       3,  NULL },

    { PT_FLOAT,     UnaOpMask::Real,        1,  &FUNCBREC::BindRealUnaOp },
    { PT_DOUBLE,    UnaOpMask::Real,        0,  &FUNCBREC::BindRealUnaOp },
    { PT_DECIMAL,   UnaOpMask::Real,        0,  &FUNCBREC::BindDecUnaOp },

    { PT_BOOL,      UnaOpMask::Bool,        0,  &FUNCBREC::BindBoolUnaOp },

    // Increment and decrement operators. These are special cased.
    { PT_SBYTE,     UnaOpMask::IncDec,     10,  NULL },
    { PT_BYTE,      UnaOpMask::IncDec,      9,  NULL },
    { PT_SHORT,     UnaOpMask::IncDec,      8,  NULL },
    { PT_USHORT,    UnaOpMask::IncDec,      7,  NULL },
    { PT_INT,       UnaOpMask::IncDec,      6,  NULL },
    { PT_UINT,      UnaOpMask::IncDec,      5,  NULL },
    { PT_LONG,      UnaOpMask::IncDec,      4,  NULL },
    { PT_ULONG,     UnaOpMask::IncDec,      3,  NULL },
    { PT_FLOAT,     UnaOpMask::IncDec,      1,  NULL },
    { PT_DOUBLE,    UnaOpMask::IncDec,      0,  NULL },
    { PT_DECIMAL,   UnaOpMask::IncDec,      0,  NULL },
    { PT_CHAR,      UnaOpMask::IncDec,      0,  NULL },
};


static PREDEFTYPE rgptIntOp[] = { PT_INT, PT_UINT, PT_LONG, PT_ULONG };


/***************************************************************************************************
    Set the values of the BinOpFullSig from the given info.
***************************************************************************************************/
void FUNCBREC::BinOpFullSig::Set(TYPESYM * type1, TYPESYM * type2, PfnBindBinOp pfn,
    OpSigFlagsEnum grfos, LiftFlagsEnum grflt)
{
    this->pt1 = PREDEFTYPE(-1);
    this->pt2 = PREDEFTYPE(-1);
    this->grfbom = BinOpMask::None;
    this->cbosSkip = 0;
    this->pfn = pfn;
    this->grfos = grfos;
    this->type1 = type1;
    this->type2 = type2;
    this->grflt = grflt;
}


/***************************************************************************************************
    Set the values of the BinOpFullSig from the given BinOpSig. The FUNCBREC is needed to get
    the predefined types. Returns true iff the predef types are found.
***************************************************************************************************/
bool FUNCBREC::BinOpFullSig::Set(FUNCBREC * fnc, BinOpSig & bos)
{
    *(BinOpSig *)this = bos;
    ASSERT(pt1 >= 0);
    type1 = fnc->GetOptPDT(pt1);
    type2 = fnc->GetOptPDT(pt2);
    this->grflt = LiftFlags::None;
    return type1 && type2;
}


/***************************************************************************************************
    Set the values of the UnaOpFullSig from the given info.
***************************************************************************************************/
void FUNCBREC::UnaOpFullSig::Set(TYPESYM * type, PfnBindUnaOp pfn, LiftFlagsEnum grflt)
{
    this->pt = PREDEFTYPE(-1);
    this->grfuom = UnaOpMask::None;
    this->cuosSkip = 0;
    this->pfn = pfn;
    this->type = type;
    this->grflt = grflt;
}


/***************************************************************************************************
    Set the values of the UnaOpFullSig from the given UnaOpSig. The FUNCBREC is needed to get
    the predefined type. Returns true iff the predef type is found.
***************************************************************************************************/
bool FUNCBREC::UnaOpFullSig::Set(FUNCBREC * fnc, UnaOpSig & uos)
{
    *(UnaOpSig *)this = uos;
    ASSERT(pt >= 0);
    type = fnc->GetOptPDT(pt);
    this->grflt = LiftFlags::None;
    return type != NULL;
}


/***************************************************************************************************
    This handles binding binary operators by first checking for user defined operators, then
    applying overload resolution to the predefined operators. It handles lifting over nullable.
***************************************************************************************************/
EXPR * FUNCBREC::BindStdBinOp(BASENODE * tree, EXPRKIND ek, EXPR * arg1, EXPR * arg2, bool * pfUserDef, NubInfo * pnin)
{
#ifdef DEBUG
    static bool fCheckedBetter;

    if (!fCheckedBetter) {
        for (int i = 0; i <= NUM_EXT_TYPES; i++) {
            ASSERT(g_mpptptBetter[i][i] == 0);
            for (int j = 0; j < i; j++) {
                ASSERT(g_mpptptBetter[i][j] != 0 && g_mpptptBetter[j][i] != 0);
                ASSERT(g_mpptptBetter[i][j] + g_mpptptBetter[j][i] == 3 ||
                    g_mpptptBetter[i][j] == 3 && g_mpptptBetter[j][i] == 3);
                ASSERT(!GetOptPDT(PREDEFTYPE(i)) || !GetOptPDT(PREDEFTYPE(j)) ||
                    (!canConvert(GetOptPDT(PREDEFTYPE(i)), GetOptPDT(PREDEFTYPE(j)), NOUDC) || g_mpptptBetter[i][j] == 1) &&
                    (!canConvert(GetOptPDT(PREDEFTYPE(j)), GetOptPDT(PREDEFTYPE(i)), NOUDC) || g_mpptptBetter[j][i] == 1));
            }
        }
        fCheckedBetter = true;
    }
#endif

    ASSERT(arg1 && arg2);

    uint flags = 0;
    BinOpArgInfo info;

    if (pnin)
        pnin->Init();
    if (pfUserDef)
        *pfUserDef = false;

    // Get the binary operator kind and flags.
    if (!GetBokAndFlags(ek, &info.bok, &flags))
        return badOperatorTypesError(tree, arg1, arg2);

    info.arg1 = arg1;
    info.arg2 = arg2;
    info.type1 = arg1->type;
    info.type2 = arg2->type;
    info.typeRaw1 = info.type1->StripNubs();
    info.typeRaw2 = info.type2->StripNubs();
    info.grfbom = BinOpMaskEnum(1 << info.bok);

    compiler()->EnsureState(info.type1);
    compiler()->EnsureState(info.type2);

    info.pt1 = info.type1->isPredefined() ? info.type1->getPredefType() : PT_COUNT;
    info.pt2 = info.type2->isPredefined() ? info.type2->getPredefType() : PT_COUNT;
    info.ptRaw1 = info.typeRaw1->isPredefined() ? info.typeRaw1->getPredefType() : PT_COUNT;
    info.ptRaw2 = info.typeRaw2->isPredefined() ? info.typeRaw2->getPredefType() : PT_COUNT;

    const int kcbofsMaxExtra = 10; // This is the maximum number of non-predef signatures we'll have.
    const int kcbofsMax = lengthof(g_rgbos) + kcbofsMaxExtra;
    BinOpFullSig rgbofs[kcbofsMax];
    int cbofs = 0;
    int ibofsBest = -1;
    unsigned int ibos;
    unsigned int ibosMinLift;
    int ibofs;

    /***************************************************************************************************
        Find all applicable operator signatures.
        First check for special ones (delegate, enum, pointer) and check for user defined ops.
    ***************************************************************************************************/

    if (info.pt1 > PT_ULONG || info.pt2 > PT_ULONG) {
        EXPR * expr;

        // Check for user defined.
        switch (info.bok) {
        case BinOpKind::Logical:
            // Logical operators cannot be overloaded, but use the bitwise overloads.
            expr = BindUDBinop(tree, (EXPRKIND)(ek - EK_LOGAND + EK_BITAND), arg1, arg2, true);
            if (expr) {
                if (pfUserDef)
                    *pfUserDef = true;
                return BindUserBoolOp(tree, ek, expr);
            }
            break;
        default:
            expr = BindUDBinop(tree, ek, arg1, arg2);
            if (expr) {
                if (pfUserDef)
                    *pfUserDef = true;
                return expr;
            }
            break;
        }

        // Delegate types and enum types are special in that they carry a set
        // of "predefined" operators.
        if ((info.type1->isDelegateType() || info.type2->isDelegateType()) &&
                (info.grfbom & BinOpMask::Delegate) && GetDelBinOpSigs(rgbofs, &cbofs, info) ||
            (info.typeRaw1->isEnumType() || info.typeRaw2->isEnumType()) &&
                GetEnumBinOpSigs(rgbofs, &cbofs, info) ||
            (info.type1->isPTRSYM() || info.type2->isPTRSYM()) &&
                GetPtrBinOpSigs(rgbofs, &cbofs, info) ||
            info.bok == BinOpKind::Equal && GetRefEqualSigs(rgbofs, &cbofs, info))
        {
            // We have an exact match.
            ASSERT(cbofs > 0);
            ibofsBest = cbofs - 1;
            goto LMatch;
        }
    }
    ASSERT(cbofs <= kcbofsMaxExtra);

    /***************************************************************************************************
        Now check for standard ones (numeric, string, etc) and their lifted forms.
    ***************************************************************************************************/

    ibosMinLift = compiler()->FCanLift() ? 0 : lengthof(g_rgbos);
    for (ibos = 0; ibos < lengthof(g_rgbos); ibos++) {
        BinOpSig & bos = g_rgbos[ibos];
        if (!(bos.grfbom & info.grfbom))
            continue;

        TYPESYM * typeSig1 = GetOptPDT(bos.pt1);
        TYPESYM * typeSig2 = GetOptPDT(bos.pt2);
        if (!typeSig1 || !typeSig2)
            continue;

        ConvKindEnum cv1 = GetConvKind(info.pt1, bos.pt1);
        ConvKindEnum cv2 = GetConvKind(info.pt2, bos.pt2);
        LiftFlagsEnum grflt = LiftFlags::None;

        switch (cv1) {
        default:
            VSFAIL("Shouldn't happen!");
        case ConvKind::None:
            continue;
        case ConvKind::Explicit:
            if (arg1->kind != EK_CONSTANT)
                continue;
            // Need to try to convert.
        case ConvKind::Unknown:
            if (canConvert(arg1, typeSig1))
                break;
            if (ibos < ibosMinLift || !(bos.grfos & OpSigFlags::CanLift))
                continue;
            ASSERT(typeSig1->IsValType());

            typeSig1 = compiler()->getBSymmgr().GetNubType(typeSig1);
            if (!canConvert(arg1, typeSig1))
                continue;
            switch (GetConvKind(info.ptRaw1, bos.pt1)) {
            default:
                grflt = grflt | LiftFlags::Convert1;
                break;
            case ConvKind::Implicit:
            case ConvKind::Identity:
                grflt = grflt | LiftFlags::Lift1;
                break;
            }
            break;
        case ConvKind::Implicit:
            break;
        case ConvKind::Identity:
            if (cv2 == ConvKind::Identity && rgbofs[cbofs].Set(this, bos)) {
                // Exact match.
                ibofsBest = cbofs++;
                goto LMatch;
            }
            break;
        }

        switch (cv2) {
        default:
            VSFAIL("Shouldn't happen!");
        case ConvKind::None:
            continue;
        case ConvKind::Explicit:
            if (arg2->kind != EK_CONSTANT)
                continue;
            // Need to try to convert.
        case ConvKind::Unknown:
            if (canConvert(arg2, typeSig2))
                break;
            if (ibos < ibosMinLift || !(bos.grfos & OpSigFlags::CanLift))
                continue;
            ASSERT(typeSig2->IsValType());

            typeSig2 = compiler()->getBSymmgr().GetNubType(typeSig2);
            if (!canConvert(arg2, typeSig2))
                continue;
            switch (GetConvKind(info.ptRaw2, bos.pt2)) {
            default:
                grflt = grflt | LiftFlags::Convert2;
                break;
            case ConvKind::Implicit:
            case ConvKind::Identity:
                grflt = grflt | LiftFlags::Lift2;
                break;
            }
            break;
        case ConvKind::Identity:
        case ConvKind::Implicit:
            break;
        }

        if (grflt) {
            // We have a lifted signature.
            rgbofs[cbofs].Set(typeSig1, typeSig2, bos.pfn, bos.grfos, grflt);
            cbofs++;

            // NOTE: Can't skip any if we use a lifted signature because the
            // type might convert to int? and to long (but not to int) in which
            // case we should get an ambiguity. But we can skip the lifted ones....
            ibosMinLift = ibos + bos.cbosSkip + 1;
        }
        else {
            // Record it as applicable and skip accordingly.
            if (rgbofs[cbofs].Set(this, bos))
                cbofs++;
            ibos += bos.cbosSkip;
        }
    }
    ASSERT(cbofs <= kcbofsMax);

    if (!cbofs) {
        // Check for x == null where x is a nullable.
        if (info.bok == BinOpKind::Equal) {
            EXPR * exprRes;
            if (info.type1->isNUBSYM() && info.type2->isNULLSYM()) {
                exprRes = BindNubHasValue(tree, info.arg1, ek == EK_NE);
                return AddSideEffects(tree, exprRes, info.arg2, false, true);
            }
            if (info.type1->isNULLSYM() && info.type2->isNUBSYM()) {
                exprRes = BindNubHasValue(tree, info.arg2, ek == EK_NE);
                return AddSideEffects(tree, exprRes, info.arg1, true, true);
            }
        }
        return badOperatorTypesError(tree, arg1, arg2);
    }

    ibofsBest = 0;
    if (cbofs == 1)
        goto LMatch;

    // Determine which is best.
    for (ibofs = 1; ibofs < cbofs; ibofs++) {
        if (ibofsBest < 0)
            ibofsBest = ibofs;
        else {
            int nT = WhichBofsIsBetter(rgbofs[ibofsBest], rgbofs[ibofs], info.type1, info.type2);
            if (nT == 0)
                ibofsBest = -1;
            else if (nT > 0)
                ibofsBest = ibofs;
        }
    }
    if (ibofsBest < 0) {
        // Ambigous.
        return ambiguousOperatorError(tree, arg1, arg2);
    }

    // Verify that our answer works.
    for (ibofs = 0; ibofs < cbofs; ibofs++) {
        if (ibofs == ibofsBest)
            continue;
        if (WhichBofsIsBetter(rgbofs[ibofsBest], rgbofs[ibofs], info.type1, info.type2) >= 0)
            return ambiguousOperatorError(tree, arg1, arg2);
    }

LMatch:
    ASSERT(cbofs <= kcbofsMax);
    ASSERT(ibofsBest < cbofs);

    BinOpFullSig & bofs = rgbofs[ibofsBest];

    if (!bofs.pfn)
        return badOperatorTypesError(tree, arg1, arg2);

    EXPR * expr1 = arg1;
    EXPR * expr2 = arg2;

    if (!bofs.grflt || !(bofs.grfos & OpSigFlags::AutoLift)) {
        if (bofs.grfos & OpSigFlags::Convert) {
            expr1 = mustConvert(expr1, bofs.type1);
            expr2 = mustConvert(expr2, bofs.type2);
        }
        return (this->*bofs.pfn)(tree, ek, flags, expr1, expr2);
    }

    // Have to lift.
    ASSERT(bofs.type1->isNUBSYM() || bofs.type2->isNUBSYM());

    EXPR * exprRes;
    NubInfo nin;

    if (!(bofs.grflt & LiftFlags::Lift1))
        expr1 = mustConvert(expr1, bofs.type1);
    if (!(bofs.grflt & LiftFlags::Lift2))
        expr2 = mustConvert(expr2, bofs.type2);

    BindNubCondValBin(tree, expr1, expr2, nin, LiftFlags::LiftBoth);
    ASSERT(nin.fActive);

    if (info.bok == BinOpKind::Equal)
        return BindNubEqOpCore(tree, ek, flags, expr1, expr2, bofs, nin);

    if (info.bok == BinOpKind::Compare)
        return BindNubCmpOpCore(tree, ek, flags, bofs, nin);

    expr1 = nin.Val(0);
    expr2 = nin.Val(1);

    if (bofs.grflt & LiftFlags::Lift1)
        expr1 = mustConvert(expr1, bofs.type1->asNUBSYM()->baseType());
    if (bofs.grflt & LiftFlags::Lift2)
        expr2 = mustConvert(expr2, bofs.type2->asNUBSYM()->baseType());

    // We need to substitute a dummy expr for GetValOrDef(null) - which was optimized to
    // a default value - so the bofs.pfn doesn't issue bogus errors - like can't divide
    // by zero (for x / null). We do however want to issue errors for any non-null args
    // (for null / 0).
    if (nin.rgfNull[0]) {
        ASSERT(nin.FAlwaysNull());
        expr1 = newExpr(expr1->tree, EK_NOOP, expr1->type);
    }
    if (nin.rgfNull[1]) {
        ASSERT(nin.FAlwaysNull());
        expr2 = newExpr(expr2->tree, EK_NOOP, expr2->type);
    }

    // Bind the value expression.
    exprRes = (this->*bofs.pfn)(tree, ek, flags, expr1, expr2);

    ASSERT(!exprRes->type->isNUBSYM());
    if (pnin) {
        *pnin = nin;
        return exprRes;
    }

    NUBSYM * nubDst = compiler()->getBSymmgr().GetNubType(exprRes->type);
    return BindNubOpRes(tree, nubDst, nubDst, exprRes, nin, true);
}


/***************************************************************************************************
    Handle predefined nullable equality operators. These are special because the return type
    is not lifted and null compares equal to null.
***************************************************************************************************/
EXPR * FUNCBREC::BindNubEqOpCore(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * expr1, EXPR * expr2,
    BinOpFullSig & bofs, NubInfo & nin)
{
    ASSERT(nin.fActive);

    EXPR * exprRes;

    if (nin.FAlwaysNull()) {
        // At least one of them is a constant null.
        TYPESYM * typeBool = GetReqPDT(PT_BOOL);

        if (!bofs.type1->isNUBSYM() || !bofs.type2->isNUBSYM()) {
            // One is null and the other is not nullable.
            compiler()->Error(tree, WRN_NubExprIsConstBool, ek == EK_NE ? PN_TRUE : PN_FALSE,
                nin.rgfNull[0] ? bofs.type2 : bofs.type1,
                nin.rgfNull[0] ? bofs.type1 : bofs.type2);
            exprRes = newExprConstant(tree, typeBool, ConstValInit(ek == EK_NE));
            exprRes = AddSideEffects(tree, exprRes, expr2, true, true);
            return AddSideEffects(tree, exprRes, expr1, true, true);
        }

        if (nin.rgfNull[0] && nin.rgfNull[1]) {
            // Both are constant nulls.
            exprRes = newExprConstant(tree, typeBool, ConstValInit(ek == EK_EQ));
            exprRes = AddSideEffects(tree, exprRes, expr2, true, true);
            return AddSideEffects(tree, exprRes, expr1, true, true);
        }

        // Generate seq(a, !b.HasValue) or rev(!a.HasValue, b).
        exprRes = BindNubHasValue(tree, nin.rgfNull[0] ? expr2 : expr1, ek == EK_NE);
        return AddSideEffects(tree, exprRes, nin.rgfNull[0] ? expr1 : expr2, nin.rgfNull[0], true);
    }

    if (nin.fSameTemp) {
        // The answer is (ek == EK_EQ). There aren't any side effects.
        return newExprConstant(tree, GetReqPDT(PT_BOOL), ConstValInit(ek == EK_EQ));
    }

    // Generate (tmp1 = a).GetValOrDef() == (tmp2 = b).GetValOrDef() && tmp1.HasValue == tmp2.HasValue
    EXPR * exprT = nin.PreVal(0);
    if (exprT->type->isNUBSYM())
        expr1 = BindNubGetValOrDef(tree, exprT);
    else {
        ASSERT(!nin.rgexprCnd[0]);
        nin.rgexprPst[0] = NULL;
    }

    exprT = nin.PreVal(1);
    if (exprT->type->isNUBSYM())
        expr2 = BindNubGetValOrDef(tree, exprT);
    else {
        ASSERT(!nin.rgexprCnd[1]);
        nin.rgexprPst[1] = NULL;
    }

    if (bofs.grflt & LiftFlags::Lift1)
        expr1 = mustConvert(expr1, bofs.type1->asNUBSYM()->baseType());
    if (bofs.grflt & LiftFlags::Lift2)
        expr2 = mustConvert(expr2, bofs.type2->asNUBSYM()->baseType());

    // Bind the value expression.
    exprRes = (this->*bofs.pfn)(tree, ek, flags, expr1, expr2);

    ASSERT(exprRes->type->isPredefType(PT_BOOL));

    if (nin.FAlwaysNonNull())
        return exprRes;

    ASSERT(nin.exprCnd);
    ASSERT((nin.exprCnd->kind == EK_BITAND) == (nin.rgexprCnd[0] && nin.rgexprCnd[1]));

    if (nin.exprCnd->kind == EK_BITAND)
        nin.exprCnd->kind = ek;
    else if (ek == EK_NE)
        nin.exprCnd = newExprBinop(tree, EK_LOGNOT, exprRes->type, nin.exprCnd, NULL);
    exprRes = newExprBinop(tree, ek == EK_NE ? EK_LOGOR : EK_LOGAND, exprRes->type, exprRes, nin.exprCnd);

    // Don't call BindNubAddTmps since we've already used the rgexprPre values.
    exprRes = AddSideEffects(tree, exprRes, nin.rgexprPst[0], false, true);
    return AddSideEffects(tree, exprRes, nin.rgexprPst[1], false, true);
}


/***************************************************************************************************
    Handle predefined nullable comparison operators. These are special because the return type
    is not lifted.
***************************************************************************************************/
EXPR * FUNCBREC::BindNubCmpOpCore(BASENODE * tree, EXPRKIND ek, uint flags, BinOpFullSig & bofs, NubInfo & nin)
{
    ASSERT(nin.fActive);

    EXPR * exprRes;
    EXPR * expr1 = nin.Val(0);
    EXPR * expr2 = nin.Val(1);

    if (bofs.grflt & LiftFlags::Lift1)
        expr1 = mustConvert(expr1, bofs.type1->asNUBSYM()->baseType());
    if (bofs.grflt & LiftFlags::Lift2)
        expr2 = mustConvert(expr2, bofs.type2->asNUBSYM()->baseType());

    if (nin.FAlwaysNull()) {
        compiler()->Error(tree, WRN_CmpAlwaysFalse, nin.TmpVal(nin.rgfNull[0] ? 0 : 1)->type);
        exprRes = newExprConstant(tree, GetReqPDT(PT_BOOL), ConstValInit(false));
    }
    else {
        // Bind the value expression.
        exprRes = (this->*bofs.pfn)(tree, ek, flags, expr1, expr2);
        ASSERT(exprRes->type->isPredefType(PT_BOOL));
        if (nin.FAlwaysNonNull())
            return exprRes;
        exprRes = newExprBinop(tree, EK_LOGAND, exprRes->type, exprRes, nin.exprCnd);
    }

    return BindNubAddTmps(tree, exprRes, nin);
}


/***************************************************************************************************
    Get the special signatures when at least one of the args is a delegate instance.
    Returns true iff an exact signature match is found.
***************************************************************************************************/
bool FUNCBREC::GetDelBinOpSigs(BinOpFullSig * prgbofs, int * pcbofs, BinOpArgInfo & info)
{
    ASSERT(info.grfbom & BinOpMask::Delegate);
    ASSERT(info.type1->isDelegateType() || info.type2->isDelegateType());

    // Don't allow comparison with an anonymous method. It's just too weird.
    if ((info.grfbom & BinOpMask::Equal) && (info.type1->isANONMETHSYM() || info.type2->isANONMETHSYM()))
        return false;

    // No conversions needed. Determine the lifting. This is the common case.
    if (info.type1 == info.type2) {
        prgbofs[(*pcbofs)++].Set(info.type1, info.type2, &FUNCBREC::BindDelBinOp, OpSigFlags::Reference);
        return true;
    }

    TYPESYM * typeDel;

    // Conversions needed. Since there are no conversions between distinct delegate types
    // we never need to check both cases.
    if (info.type1->isDelegateType() ?
            canConvert(info.arg2, typeDel = info.type1) :
            canConvert(info.arg1, typeDel = info.type2))
    {
        prgbofs[(*pcbofs)++].Set(typeDel, typeDel, &FUNCBREC::BindDelBinOp, OpSigFlags::Reference);
    }
    ASSERT(typeDel->isDelegateType());

    // Might be ambiguous so return false.
    return false;
}


/***************************************************************************************************
    Utility method to determine whether arg1 is convertible to typeDst, either in a regular
    scenario or lifted scenario. Sets *pgrflt, *ptypeSig1 and *ptypeSig2 accordingly.
***************************************************************************************************/
bool FUNCBREC::CanConvertArg1(BinOpArgInfo & info, TYPESYM * typeDst, LiftFlagsEnum * pgrflt,
    TYPESYM ** ptypeSig1, TYPESYM ** ptypeSig2)
{
    ASSERT(!typeDst->isNUBSYM());

    if (canConvert(info.arg1, typeDst))
        *pgrflt = LiftFlags::None;
    else {
        if (!compiler()->FCanLift())
            return false;
        typeDst = compiler()->getBSymmgr().GetNubType(typeDst);
        if (!canConvert(info.arg1, typeDst))
            return false;
        *pgrflt = LiftFlags::Convert1;
    }
    *ptypeSig1 = typeDst;

    if (info.type2->isNUBSYM()) {
        *pgrflt = *pgrflt | LiftFlags::Lift2;
        *ptypeSig2 = compiler()->getBSymmgr().GetNubType(info.typeRaw2);
    }
    else
        *ptypeSig2 = info.typeRaw2;

    return true;
}


/***************************************************************************************************
    Same as CanConvertArg1 but with the indices interchanged!
***************************************************************************************************/
bool FUNCBREC::CanConvertArg2(BinOpArgInfo & info, TYPESYM * typeDst, LiftFlagsEnum * pgrflt,
    TYPESYM ** ptypeSig1, TYPESYM ** ptypeSig2)
{
    ASSERT(!typeDst->isNUBSYM());

    if (canConvert(info.arg2, typeDst))
        *pgrflt = LiftFlags::None;
    else {
        if (!compiler()->FCanLift())
            return false;
        typeDst = compiler()->getBSymmgr().GetNubType(typeDst);
        if (!canConvert(info.arg2, typeDst))
            return false;
        *pgrflt = LiftFlags::Convert2;
    }
    *ptypeSig2 = typeDst;

    if (info.type1->isNUBSYM()) {
        *pgrflt = *pgrflt | LiftFlags::Lift1;
        *ptypeSig1 = compiler()->getBSymmgr().GetNubType(info.typeRaw1);
    }
    else
        *ptypeSig1 = info.typeRaw1;

    return true;
}


/***************************************************************************************************
    Record the appropriate binary operator full signature from the given BinOpArgInfo. This assumes
    that any NUBSYM valued args should be lifted.
***************************************************************************************************/
void FUNCBREC::RecordBinOpSigFromArgs(BinOpFullSig * prgbofs, int * pcbofs, BinOpArgInfo & info, PfnBindBinOp pfn)
{
    LiftFlagsEnum grflt = LiftFlags::None;
    TYPESYM * typeSig1;
    TYPESYM * typeSig2;

    if (info.type1 != info.typeRaw1) {
        ASSERT(info.type1->isNUBSYM());
        grflt = grflt | LiftFlags::Lift1;
        typeSig1 = compiler()->getBSymmgr().GetNubType(info.typeRaw1);
    }
    else
        typeSig1 = info.typeRaw1;

    if (info.type2 != info.typeRaw2) {
        ASSERT(info.type2->isNUBSYM());
        grflt = grflt | LiftFlags::Lift2;
        typeSig2 = compiler()->getBSymmgr().GetNubType(info.typeRaw2);
    }
    else
        typeSig2 = info.typeRaw2;

    prgbofs[(*pcbofs)++].Set(typeSig1, typeSig2, pfn, OpSigFlags::Value, grflt);
}


/***************************************************************************************************
    Get the special signatures when at least one of the args is an enum.
***************************************************************************************************/
bool FUNCBREC::GetEnumBinOpSigs(BinOpFullSig * prgbofs, int * pcbofs, BinOpArgInfo & info)
{
    ASSERT(info.typeRaw1->isEnumType() || info.typeRaw2->isEnumType());

    // (enum,      enum)       :         -         == != < > <= >= & | ^
    // (enum,      under)      :       + -
    // (under,     enum)       :       +
    TYPESYM * typeSig1;
    TYPESYM * typeSig2;
    LiftFlagsEnum grflt;

    // Look for the no conversions cases. Still need to determine the lifting. These are the common case.
    if (info.typeRaw1 == info.typeRaw2) {
        if (!(info.grfbom & BinOpMask::Enum))
            return false;
        RecordBinOpSigFromArgs(prgbofs, pcbofs, info, &FUNCBREC::BindEnumBinOp);
        return true;
    }

    if (info.typeRaw1->isEnumType() ?
            info.typeRaw2 == info.typeRaw1->underlyingEnumType() && (info.grfbom & BinOpMask::EnumUnder) :
            info.typeRaw1 == info.typeRaw2->underlyingEnumType() && (info.grfbom & BinOpMask::UnderEnum))
    {
        RecordBinOpSigFromArgs(prgbofs, pcbofs, info, &FUNCBREC::BindEnumBinOp);
        return true;
    }

    // Now deal with the conversion cases. Since there are no conversions from enum types to other
    // enum types we never need to do both cases.
    if (info.typeRaw1->isEnumType() ?
            ((info.grfbom & BinOpMask::Enum) && CanConvertArg2(info, info.typeRaw1, &grflt, &typeSig1, &typeSig2) ||
                (info.grfbom & BinOpMask::EnumUnder) &&
                    CanConvertArg2(info, info.typeRaw1->underlyingEnumType(), &grflt, &typeSig1, &typeSig2)) :
            ((info.grfbom & BinOpMask::Enum) && CanConvertArg1(info, info.typeRaw2, &grflt, &typeSig1, &typeSig2) ||
                (info.grfbom & BinOpMask::EnumUnder) &&
                    CanConvertArg1(info, info.typeRaw2->underlyingEnumType(), &grflt, &typeSig1, &typeSig2)))
    {
        prgbofs[(*pcbofs)++].Set(typeSig1, typeSig2, &FUNCBREC::BindEnumBinOp, OpSigFlags::Value, grflt);
    }

    return false;
}


/***************************************************************************************************
    Get the special signatures when at least one of the args is a pointer. Since pointers can't be
    type arguments, a nullable pointer is illegal, so no sense trying to lift any of these.

    NOTE: We don't filter out bad operators on void pointers since BindPtrBinOp gives better
    error messages than the operator overload resolution does.
***************************************************************************************************/
bool FUNCBREC::GetPtrBinOpSigs(BinOpFullSig * prgbofs, int * pcbofs, BinOpArgInfo & info)
{
    ASSERT(info.type1->isPTRSYM() || info.type2->isPTRSYM());

    // (ptr,       ptr)        :         -
    // (ptr,       int)        :       + -
    // (ptr,       uint)       :       + -
    // (ptr,       long)       :       + -
    // (ptr,       ulong)      :       + -
    // (int,       ptr)        :       +
    // (uint,      ptr)        :       +
    // (long,      ptr)        :       +
    // (ulong,     ptr)        :       +
    // (void*,     void*)      :                   == != < > <= >=

    // Check the common case first.
    if (info.type1->isPTRSYM() && info.type2->isPTRSYM()) {
        if (info.grfbom & BinOpMask::VoidPtr) {
            prgbofs[(*pcbofs)++].Set(info.type1, info.type2, &FUNCBREC::BindPtrCmpOp, OpSigFlags::None);
            return true;
        }
        if (info.type1 == info.type2 && (info.grfbom & BinOpMask::Ptr)) {
            prgbofs[(*pcbofs)++].Set(info.type1, info.type2, &FUNCBREC::BindPtrBinOp, OpSigFlags::None);
            return true;
        }
        return false;
    }

    TYPESYM * typeT;

    if (info.type1->isPTRSYM()) {
        if (info.type2->isNULLSYM()) {
            if (!(info.grfbom & BinOpMask::VoidPtr))
                return false;
            prgbofs[(*pcbofs)++].Set(info.type1, info.type1, &FUNCBREC::BindPtrCmpOp, OpSigFlags::Convert);
            return true;
        }
        if (!(info.grfbom & BinOpMask::PtrNum))
            return false;

        for (unsigned int i = 0; i < lengthof(rgptIntOp); i++) {
            if (canConvert(info.arg2, typeT = GetReqPDT(rgptIntOp[i]))) {
                prgbofs[(*pcbofs)++].Set(info.type1, typeT, &FUNCBREC::BindPtrBinOp, OpSigFlags::Convert);
                return true;
            }
        }
        return false;
    }

    ASSERT(info.type2->isPTRSYM());
    if (info.type1->isNULLSYM()) {
        if (!(info.grfbom & BinOpMask::VoidPtr))
            return false;
        prgbofs[(*pcbofs)++].Set(info.type2, info.type2, &FUNCBREC::BindPtrCmpOp, OpSigFlags::Convert);
        return true;
    }
    if (!(info.grfbom & BinOpMask::NumPtr))
        return false;

    for (unsigned i = 0; i < lengthof(rgptIntOp); i++) {
        if (canConvert(info.arg1, typeT = GetReqPDT(rgptIntOp[i]))) {
            prgbofs[(*pcbofs)++].Set(typeT, info.type2, &FUNCBREC::BindPtrBinOp, OpSigFlags::Convert);
            return true;
        }
    }
    return false;
}


/***************************************************************************************************
    See if standard reference equality applies. Make sure not to return true if another == operator
    may be applicable and better (or ambiguous)! This also handles == on System.Delegate, since
    it has special rules as well.
***************************************************************************************************/
bool FUNCBREC::GetRefEqualSigs(BinOpFullSig * prgbofs, int * pcbofs, BinOpArgInfo & info)
{
    ASSERT(info.grfbom == BinOpMask::Equal);

    if (info.type1 != info.typeRaw1 || info.type2 != info.typeRaw2)
        return false;

    bool fRet = false;

    TYPESYM * type1 = info.type1;
    TYPESYM * type2 = info.type2;
    TYPESYM * typeObj = GetReqPDT(PT_OBJECT);
    TYPESYM * typeCls = NULL;

    if (type1->isNULLSYM() && type2->isNULLSYM()) {
        typeCls = typeObj;
        fRet = true;
        goto LRecord;
    }

    // Check for: operator ==(System.Delegate, System.Delegate).
    TYPESYM * typeDel; typeDel = GetReqPDT(PT_DELEGATE);

    if (canConvert(info.arg1, typeDel) && canConvert(info.arg2, typeDel) &&
        !type1->isDelegateType() && !type2->isDelegateType())
    {
        prgbofs[(*pcbofs)++].Set(typeDel, typeDel, &FUNCBREC::BindDelBinOp, OpSigFlags::Convert);
    }

    // The reference type equality operators only handle reference types.
    FUNDTYPE ft1; ft1 = type1->fundType();
    FUNDTYPE ft2; ft2 = type2->fundType();

    switch (ft1) {
    default:
        return false;
    case FT_REF:
        break;
    case FT_VAR:
        if (type1->asTYVARSYM()->IsValType() || (!type1->asTYVARSYM()->IsRefType() && !type2->isNULLSYM()))
            return false;
        type1 = type1->asTYVARSYM()->GetBaseCls();
        break;
    }
    if (type2->isNULLSYM()) {
        fRet = true;
        // We don't need to determine the actual best type since we're
        // returning true - indicating that we've found the best operator.
        typeCls = typeObj;
        goto LRecord;
    }

    switch (ft2) {
    default:
        return false;
    case FT_REF:
        break;
    case FT_VAR:
        if (type2->asTYVARSYM()->IsValType() || (!type2->asTYVARSYM()->IsRefType() && !type1->isNULLSYM()))
            return false;
        type2 = type2->asTYVARSYM()->GetBaseCls();
        break;
    }
    if (type1->isNULLSYM()) {
        fRet = true;
        // We don't need to determine the actual best type since we're
        // returning true - indicating that we've found the best operator.
        typeCls = typeObj;
        goto LRecord;
    }

    if (!canCast(type1, type2, NOUDC) && !canCast(type2, type1, NOUDC))
        return false;

    if (type1->isInterfaceType() || type1->isPredefType(PT_STRING) || compiler()->IsBaseType(type1, typeDel))
        type1 = typeObj;
    else if (type1->isARRAYSYM())
        type1 = GetReqPDT(PT_ARRAY);
    else if (!type1->isClassType())
        return false;

    if (type2->isInterfaceType() || type2->isPredefType(PT_STRING) || compiler()->IsBaseType(type2, typeDel))
        type2 = typeObj;
    else if (type2->isARRAYSYM())
        type2 = GetReqPDT(PT_ARRAY);
    else if (!type2->isClassType())
        return false;

    ASSERT(type1->isClassType() && !type1->isPredefType(PT_STRING) && !type1->isPredefType(PT_DELEGATE));
    ASSERT(type2->isClassType() && !type2->isPredefType(PT_STRING) && !type2->isPredefType(PT_DELEGATE));

    if (compiler()->IsBaseType(type2, type1))
        typeCls = type1;
    else if (compiler()->IsBaseType(type1, type2))
        typeCls = type2;

LRecord:
    prgbofs[(*pcbofs)++].Set(typeCls, typeCls, &FUNCBREC::BindRefCmpOp, OpSigFlags::None);
    return fRet;
}


/***************************************************************************************************
    Determined which predefined type is better relative to a given type. It is assumed that
    the given type is implicitly convertible to both of the predefined types (possibly via
    a user defined conversion, method group conversion, etc).
***************************************************************************************************/
BetterTypeEnum FUNCBREC::WhichTypeIsBetter(PREDEFTYPE pt1, PREDEFTYPE pt2, TYPESYM * typeGiven)
{
    if (pt1 == pt2)
        return BetterType::Same;
    if (typeGiven->isPredefType(pt1))
        return BetterType::Left;
    if (typeGiven->isPredefType(pt2))
        return BetterType::Right;
    if (pt1 <= NUM_EXT_TYPES && pt2 <= NUM_EXT_TYPES)
        return BetterTypeEnum(g_mpptptBetter[pt1][pt2]);
    if (pt2 == PT_OBJECT && pt1 < PT_COUNT)
        return BetterType::Left;
    if (pt1 == PT_OBJECT && pt2 < PT_COUNT)
        return BetterType::Right;
    return WhichTypeIsBetter(GetOptPDT(pt1), GetOptPDT(pt2), typeGiven);
}


/***************************************************************************************************
    Determined which type is better relative to a given type. It is assumed that the given type
    (or its associated expression) is implicitly convertible to both of the types (possibly via
    a user defined conversion, method group conversion, etc).
***************************************************************************************************/
BetterTypeEnum FUNCBREC::WhichTypeIsBetter(TYPESYM * type1, TYPESYM * type2, TYPESYM * typeGiven)
{
    ASSERT(type1 && type2);
    if (type1 == type2)
        return BetterType::Same;
    if (typeGiven == type1)
        return BetterType::Left;
    if (typeGiven == type2)
        return BetterType::Right;

    bool f12 = canConvert(type1, type2);
    bool f21 = canConvert(type2, type1);
    if (f12 != f21)
        return f12 ? BetterType::Left : BetterType::Right;

    if (!type1->isNUBSYM() || !type2->isNUBSYM() ||
        !type1->asNUBSYM()->baseType()->isPredefined() ||
        !type2->asNUBSYM()->baseType()->isPredefined())
    {
        return BetterType::Neither;
    }

    PREDEFTYPE pt1 = type1->asNUBSYM()->baseType()->getPredefType();
    PREDEFTYPE pt2 = type2->asNUBSYM()->baseType()->getPredefType();

    if (pt1 <= NUM_EXT_TYPES && pt2 <= NUM_EXT_TYPES)
        return BetterTypeEnum(g_mpptptBetter[pt1][pt2]);

    return BetterType::Neither;
}


/***************************************************************************************************
    Determine which BinOpSig is better for overload resolution.
    Better means: at least as good in all params, and better in at least one param.

    Better w/r to a param means:
    1) same type as argument
    2) implicit conversion from this one's param type to the other's param type
    Because of user defined conversion operators this relation is not transitive.

    Returns negative if ibos1 is better, positive if ibos2 is better, 0 if neither.
***************************************************************************************************/
int FUNCBREC::WhichBofsIsBetter(BinOpFullSig & bofs1, BinOpFullSig & bofs2, TYPESYM * type1, TYPESYM * type2)
{
    BetterTypeEnum bt1;
    BetterTypeEnum bt2;

    if (bofs1.FPreDef() && bofs2.FPreDef()) {
        // Faster to compare predefs.
        bt1 = WhichTypeIsBetter(bofs1.pt1, bofs2.pt1, type1);
        bt2 = WhichTypeIsBetter(bofs1.pt2, bofs2.pt2, type2);
    }
    else {
        bt1 = WhichTypeIsBetter(bofs1.type1, bofs2.type1, type1);
        bt2 = WhichTypeIsBetter(bofs1.type2, bofs2.type2, type2);
    }

    int res = 0;

    switch (bt1) {
    default:
        VSFAIL("Shouldn't happen");
    case BetterType::Same:
    case BetterType::Neither:
        break;
    case BetterType::Left:
        res--;
        break;
    case BetterType::Right:
        res++;
        break;
    }

    switch (bt2) {
    default:
        VSFAIL("Shouldn't happen");
    case BetterType::Same:
    case BetterType::Neither:
        break;
    case BetterType::Left:
        res--;
        break;
    case BetterType::Right:
        res++;
        break;
    }

    return res;
}


/***************************************************************************************************
    Bind a standard unary operator. Takes care of user defined operators, predefined operators
    and lifting over nullable.
***************************************************************************************************/
EXPR * FUNCBREC::BindStdUnaOp(BASENODE * tree, OPERATOR op, EXPR * arg)
{
    ASSERT(arg);

    EXPRKIND ek;
    UnaOpKindEnum uok;
    uint flags = 0;

    switch (op) {
    case OP_UPLUS:
        uok = UnaOpKind::Plus;
        ek = EK_UPLUS;
        break;
    case OP_NEG:
        if (checked.normal)
            flags |= EXF_CHECKOVERFLOW;
        uok = UnaOpKind::Minus;
        ek = EK_NEG;
        break;
    case OP_BITNOT:
        uok = UnaOpKind::Tilde;
        ek = EK_BITNOT;
        break;
    case OP_LOGNOT:
        uok = UnaOpKind::Bang;
        ek = EK_LOGNOT;
        break;
    case OP_POSTINC:
        flags |= EXF_ISPOSTOP;
    case OP_PREINC:
        if (checked.normal)
            flags |= EXF_CHECKOVERFLOW;
        uok = UnaOpKind::IncDec;
        ek = EK_ADD;
        break;
    case OP_POSTDEC:
        flags |= EXF_ISPOSTOP;
    case OP_PREDEC:
        if (checked.normal)
            flags |= EXF_CHECKOVERFLOW;
        uok = UnaOpKind::IncDec;
        ek = EK_SUB;
        break;
    default:
        VSFAIL("Bad op");
        return badOperatorTypesError(tree, arg, NULL);
    }

    UnaOpMaskEnum fuom = UnaOpMaskEnum(1 << uok);
    TYPESYM * type = arg->type;
    TYPESYM * typeRaw = type->StripNubs();
    compiler()->EnsureState(type);

    PREDEFTYPE pt = type->isPredefined() ? type->getPredefType() : PT_COUNT;
    PREDEFTYPE ptRaw = typeRaw->isPredefined() ? typeRaw->getPredefType() : PT_COUNT;

    const int kcuofsMax = lengthof(g_rguos) + 5;
    UnaOpFullSig rguofs[kcuofsMax];
    int cuofs = 0;
    int iuofsBest = -1;
    unsigned int iuos;
    unsigned int iuosMinLift;
    int iuofs;

    /***************************************************************************************************
        Find all applicable operator signatures.
        First check for special ones (enum, ptr) and check for user defined ops.
    ***************************************************************************************************/

    if (ptRaw > PT_ULONG) {
        // Enum types are special in that they carry a set of "predefined" operators (~ and inc/dec).
        if (typeRaw->isEnumType()) {
            if (fuom & (UnaOpMask::Tilde | UnaOpMask::IncDec)) {
                // We have an exact match.
                LiftFlagsEnum grflt = LiftFlags::None;
                TYPESYM * typeSig = type;

                if (typeSig->isNUBSYM()) {
                    if (typeSig->asNUBSYM()->baseType() != typeRaw)
                        typeSig = compiler()->getBSymmgr().GetNubType(typeRaw);
                    grflt = LiftFlags::Lift1;
                }

                rguofs[iuofsBest = cuofs++].Set(typeSig,
                    uok == UnaOpKind::Tilde ? &FUNCBREC::BindEnumUnaOp : NULL, grflt);
                goto LMatch;
            }
        }
        else if (uok == UnaOpKind::IncDec) {
            // Check for pointers
            if (type->isPTRSYM()) {
                rguofs[iuofsBest = cuofs++].Set(type, NULL, LiftFlags::None);
                goto LMatch;
            }

            // Check for user defined inc/dec
            EXPRMULTIGET * exprGet = newExpr(arg->tree, EK_MULTIGET, arg->type)->asMULTIGET();

            EXPR * exprVal = bindUDUnop(tree, (EXPRKIND) (ek - EK_ADD + EK_INC), exprGet);
            if (exprVal) {
                EXPRMULTI * exprMulti = newExpr(tree, EK_MULTI, arg->type)->asMULTI();
                exprMulti->left = arg;
                exprMulti->flags |= EXF_ASSGOP | flags;
                exprGet->multi = exprMulti;

                if (!exprVal->type->isERRORSYM() && exprVal->type != arg->type)
                    exprVal = mustConvert(exprVal, arg->type);

                exprMulti->op = exprVal;
                return exprMulti;
            }
            // Try for a predefined increment operator.
        }
        else {
            // Check for user defined.
            EXPR * expr = bindUDUnop(tree, ek, arg);
            if (expr)
                return expr;
        }
    }

    // Find applicable signatures....
    iuosMinLift = compiler()->FCanLift() ? 0 : lengthof(g_rguos);
    for (iuos = 0; iuos < lengthof(g_rguos); iuos++) {
        UnaOpSig & uos = g_rguos[iuos];
        if (!(uos.grfuom & fuom))
            continue;

        ConvKindEnum cv = GetConvKind(pt, g_rguos[iuos].pt);
        TYPESYM * typeSig = NULL;

        switch (cv) {
        default:
            VSFAIL("Shouldn't happen!");
        case ConvKind::None:
            continue;
        case ConvKind::Explicit:
            if (arg->kind != EK_CONSTANT)
                continue;
            // Need to try to convert.
        case ConvKind::Unknown:
            if (canConvert(arg, typeSig = GetOptPDT(uos.pt)))
                break;
            if (iuos < iuosMinLift)
                continue;
            typeSig = compiler()->getBSymmgr().GetNubType(typeSig);
            if (!canConvert(arg, typeSig))
                continue;
            break;
        case ConvKind::Implicit:
            break;
        case ConvKind::Identity:
            if (rguofs[cuofs].Set(this, uos)) {
                // Exact match.
                iuofsBest = cuofs++;
                goto LMatch;
            }
            break;
        }

        if (typeSig && typeSig->isNUBSYM()) {
            // Need to use a lifted signature.
            LiftFlagsEnum grflt = LiftFlags::None;

            switch (GetConvKind(ptRaw, uos.pt)) {
            default:
                grflt = grflt | LiftFlags::Convert1;
                break;
            case ConvKind::Implicit:
            case ConvKind::Identity:
                grflt = grflt | LiftFlags::Lift1;
                break;
            }

            rguofs[cuofs].Set(typeSig, uos.pfn, grflt);
            cuofs++;

            // NOTE: Can't skip any if we use the lifted signature because the
            // type might convert to int? and to long (but not to int) in which
            // case we should get an ambiguity. But we can skip the lifted ones....
            iuosMinLift = iuos + uos.cuosSkip + 1;
        }
        else {
            // Record it as applicable and skip accordingly.
            if (rguofs[cuofs].Set(this, uos))
                cuofs++;
            iuos += uos.cuosSkip;
        }
    }

    if (!cuofs)
        return badOperatorTypesError(tree, arg, NULL);

    iuofsBest = 0;
    if (cuofs == 1)
        goto LMatch;

    // Determine which is best.
    for (iuofs = 1; iuofs < cuofs; iuofs++) {
        if (iuofsBest < 0)
            iuofsBest = iuofs;
        else {
            int nT = WhichUofsIsBetter(rguofs[iuofsBest], rguofs[iuofs], type);
            if (nT == 0)
                iuofsBest = -1;
            else if (nT > 0)
                iuofsBest = iuofs;
        }
    }
    if (iuofsBest < 0) {
        // Ambigous.
        return ambiguousOperatorError(tree, arg, NULL);
    }

    // Verify that our answer works.
    for (iuofs = 0; iuofs < cuofs; iuofs++) {
        if (iuofs == iuofsBest)
            continue;
        if (WhichUofsIsBetter(rguofs[iuofsBest], rguofs[iuofs], type) >= 0)
            return ambiguousOperatorError(tree, arg, NULL);
    }

LMatch:
    ASSERT(cuofs < kcuofsMax);
    ASSERT(iuofsBest < cuofs);

    UnaOpFullSig & uofs = rguofs[iuofsBest];

    if (!uofs.pfn) {
        if (uok == UnaOpKind::IncDec)
            return BindIncOp(tree, ek, flags, arg, uofs);
        return badOperatorTypesError(tree, arg, NULL);
    }

    EXPR * expr = arg;
    NubInfo nin;

    if (uofs.grflt) {
        // Have to lift.
        ASSERT(uofs.type->isNUBSYM());
        if (!(uofs.grflt & LiftFlags::Lift1))
            expr = mustConvert(expr, uofs.type);

        BindNubCondValBin(tree, expr, NULL, nin);
        ASSERT(nin.fActive);

        expr = nin.Val(0);

        if (uofs.grflt & LiftFlags::Lift1)
            expr = mustConvert(expr, uofs.type->asNUBSYM()->baseType());
    }
    else
        expr = mustConvert(expr, uofs.type);

    EXPR * exprRes = (this->*uofs.pfn)(tree, ek, flags, expr);

    if (uofs.grflt) {
        ASSERT(nin.fActive);

        exprRes = BindNubNew(tree, exprRes);
        exprRes = BindNubOpRes(tree, exprRes->type->asNUBSYM(), exprRes->type, exprRes, nin, true);
    }

    return exprRes;
}


/***************************************************************************************************
    Determine which UnaOpSig is better for overload resolution.
    Returns negative if iuos1 is better, positive if iuos2 is better, 0 if neither.
***************************************************************************************************/
int FUNCBREC::WhichUofsIsBetter(UnaOpFullSig & uofs1, UnaOpFullSig & uofs2, TYPESYM * typeArg)
{
    BetterTypeEnum bt;

    if (uofs1.FPreDef() && uofs2.FPreDef()) {
        // Faster to compare predefs.
        bt = WhichTypeIsBetter(uofs1.pt, uofs2.pt, typeArg);
    }
    else {
        bt = WhichTypeIsBetter(uofs1.type, uofs2.type, typeArg);
    }

    switch (bt) {
    default:
        VSFAIL("Shouldn't happen");
    case BetterType::Same:
    case BetterType::Neither:
        return 0;
    case BetterType::Left:
        return -1;
    case BetterType::Right:
        return +1;
    }
}


/***************************************************************************************************
    Handles binding a predefined operator that is implemented as a "user-defined" operator.
    For example, decimal ops and delegate equality ops.
***************************************************************************************************/
EXPR * FUNCBREC::BindPredefOpAsUD(BASENODE * tree, EXPRKIND ek, EXPR * arg1, EXPR * arg2)
{
    ASSERT(!arg2 || arg1->type == arg2->type);

    TYPESYM * type = arg1->type;
    ASSERT(type->isPredefined());

    NAME * name = ekName(ek);
    ASSERT(name);

    MemberLookup mem;
    if (!mem.Lookup(compiler(), type, NULL, parentDecl, name, 0, MemLookFlags::Operator)) {
        mem.ReportErrors(tree);
        return newError(tree, NULL);
    }
    ASSERT(mem.SymFirst()->isMETHSYM() && mem.SymFirst()->asMETHSYM()->isOperator);
    mem.ReportWarnings(tree);

    EXPRMEMGRP * grp = newExprMemGrp(tree, tree, mem);
    EXPR * args = arg2 ? newExprBinop(NULL, EK_LIST, NULL, arg1, arg2) : arg1;

    return BindGrpToArgs(tree, 0, grp, args);
}


/***************************************************************************************************
    Handles standard binary integer based operators.
***************************************************************************************************/
EXPR * FUNCBREC::BindIntBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2)
{
    ASSERT(arg1->type->isPredefined() && arg2->type->isPredefined() && arg1->type->getPredefType() == arg2->type->getPredefType());
    return BindIntOp(tree, ek, flags, arg1, arg2, arg1->type->getPredefType());
}


/***************************************************************************************************
    Handles standard unary integer based operators.
***************************************************************************************************/
EXPR * FUNCBREC::BindIntUnaOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg)
{
    ASSERT(arg->type->isPredefined());
    return BindIntOp(tree, ek, flags, arg, NULL, arg->type->getPredefType());
}


/***************************************************************************************************
    Handles standard binary floating point (float, double) based operators.
***************************************************************************************************/
EXPR * FUNCBREC::BindRealBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2)
{
    ASSERT(arg1->type->isPredefined() && arg2->type->isPredefined() && arg1->type->getPredefType() == arg2->type->getPredefType());
    return bindFloatOp(tree, ek, flags, arg1, arg2);
}


/***************************************************************************************************
    Handles standard unary floating point (float, double) based operators.
***************************************************************************************************/
EXPR * FUNCBREC::BindRealUnaOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg)
{
    ASSERT(arg->type->isPredefined());
    return bindFloatOp(tree, ek, flags, arg, NULL);
}


/***************************************************************************************************
    Handles standard increment and decrement operators.
***************************************************************************************************/
EXPR * FUNCBREC::BindIncOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg, UnaOpFullSig & uofs)
{
    ASSERT(ek == EK_ADD || ek == EK_SUB);
    if (!checkLvalue(arg))
        return newError(tree, arg->type);

    TYPESYM * typeRaw = uofs.type->StripNubs();

    // The type of the 1.
    TYPESYM * typeOne = typeRaw;
    if (typeOne->isEnumType())
        typeOne = typeOne->underlyingEnumType();

    EXPRMULTIGET * exprGet = newExpr(arg->tree, EK_MULTIGET, arg->type)->asMULTIGET();
    exprGet->flags |= EXF_ASSGOP;

    NubInfo nin;
    EXPR * exprVal = exprGet;

    if (uofs.grflt) {
        // Have to lift.
        ASSERT(uofs.type->isNUBSYM());
        if (!(uofs.grflt & LiftFlags::Lift1))
            exprVal = mustConvert(exprVal, uofs.type);

        BindNubCondValBin(tree, exprVal, NULL, nin);
        ASSERT(nin.fActive);

        exprVal = nin.Val(0);

        if (uofs.grflt & LiftFlags::Lift1)
            exprVal = mustConvert(exprVal, uofs.type->asNUBSYM()->baseType());
    }
    else {
        ASSERT(!uofs.type->isNUBSYM());
        exprVal = mustConvert(exprVal, uofs.type);
    }

    CONSTVAL cv;
    FUNDTYPE ft;
    TYPESYM * typeTmp = typeRaw;

    switch (ft = typeRaw->fundType()) {
    default:
        ASSERT(typeRaw->isPredefType(PT_DECIMAL));
        exprVal = BindPredefOpAsUD(tree, ek == EK_ADD ? EK_INC : EK_DEC, exprVal, NULL);
        break;
    case FT_PTR:
        cv.iVal = 1;
        exprVal = BindPtrBinOp(tree, ek, flags, exprVal, newExprConstant(tree, GetReqPDT(PT_INT), cv));
        break;
    case FT_I1:
    case FT_I2:
    case FT_U1:
    case FT_U2:
        typeTmp = GetReqPDT(PT_INT);
        // Fall through.
    case FT_I4:
    case FT_U4:
        cv.iVal = 1;
        goto LScalar;
    case FT_I8:
    case FT_U8:
        cv.longVal = (__int64 *) allocator->Alloc(sizeof(__int64));
        *cv.longVal = (__int64)1;
        goto LScalar;
    case FT_R4:
    case FT_R8:
        flags &= ~EXF_CHECKOVERFLOW;
        cv.doubleVal = (double*) allocator->Alloc(sizeof(double));
        *cv.doubleVal = 1.0;
LScalar:
        exprVal = newExprBinop(tree, ek, typeTmp, exprVal, newExprConstant(tree, typeOne, cv));
        exprVal->flags |= flags;
        if (typeTmp != typeRaw)
            exprVal = mustCast(exprVal, typeRaw, NOUDC);
        break;
    }
    ASSERT(!exprVal->type->isNUBSYM());

    EXPRMULTI * exprMulti = newExpr(tree, EK_MULTI, arg->type)->asMULTI();
    exprGet->multi = exprMulti;
    exprMulti->left = arg;
    exprMulti->flags |= EXF_ASSGOP | flags;

    // Convert to the destination type.
    if (!nin.fActive)
        exprVal = mustConvert(exprVal, arg->type);
    else {
        // The operator is lifted.
        EXPR * exprNull = newExprZero(tree, uofs.type);

        // First convert to the nullable type, then the arg type.
        exprVal = mustConvert(exprVal, uofs.type);
        exprVal = mustConvert(exprVal, arg->type);

        // If exprRes is an error expr, just use it - don't try to cast the exprNull and
        // risk a duplicate error message.
        exprNull = exprVal->isOK() ? mustCast(exprNull, arg->type) : exprVal;
        exprVal = BindNubOpRes(tree, arg->type, exprVal, exprNull, nin);
    }

    exprMulti->op = exprVal;

    return exprMulti;
}


/***************************************************************************************************
    Handles standard binary decimal based operators.
***************************************************************************************************/
EXPR * FUNCBREC::BindDecBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2)
{
    ASSERT(arg1->type->isPredefType(PT_DECIMAL) && arg2->type->isPredefType(PT_DECIMAL));

    TYPESYM * typeDec = GetOptPDT(PT_DECIMAL);
    ASSERT(typeDec);

    EXPR * argConst1 = arg1->GetConst();
    EXPR * argConst2 = arg2->GetConst();


    if (argConst1) {
        if (argConst2)
            goto LBothConst;
        DECIMAL dec = * argConst1->asCONSTANT()->getVal().decVal;

        // Convert 1 + d to inc(d) and -1 + d to dec(d)
        if (ek == EK_ADD &&
            DECIMAL_LO32(dec) == 1 && DECIMAL_SCALE(dec) == 0 &&
            (DECIMAL_MID32(dec) | DECIMAL_HI32(dec)) == 0)
        {
            ek = (DECIMAL_SIGN(dec) == DECIMAL_NEG) ? EK_DEC : EK_INC;
            return AddSideEffects(tree, BindPredefOpAsUD(tree, ek, arg2, NULL), arg1, true);
        }
    }
    else if (argConst2) {
        DECIMAL dec = * argConst2->asCONSTANT()->getVal().decVal;

        // Convert d + 1 to inc(d) and d - 1 to dec(d)
        if ((ek == EK_ADD || ek == EK_SUB) &&
            DECIMAL_LO32(dec) == 1 && DECIMAL_SCALE(dec) == 0 &&
            (DECIMAL_MID32(dec) | DECIMAL_HI32(dec)) == 0)
        {
            if (DECIMAL_SIGN(dec) == DECIMAL_NEG)
                ek = (EXPRKIND)(EK_ADD + EK_SUB - ek);
            return BindPredefOpAsUD(tree, (EXPRKIND)(ek - EK_ADD + EK_INC), AddSideEffects(tree, arg1, arg2, false), NULL);
        }
    }

    return BindPredefOpAsUD(tree, ek, arg1, arg2);

LBothConst:
    DECIMAL dec1;
    DECIMAL dec2;       
    DECIMAL decRes;
    bool fRes = false;
    bool fBool = false;
    HRESULT hr = NOERROR;

    dec1 = * argConst1->asCONSTANT()->getVal().decVal;
    dec2 = * argConst2->asCONSTANT()->getVal().decVal;

    // Do the operation.
    switch (ek) {
    case EK_ADD: hr = VarDecAdd(&dec1, &dec2, &decRes); break;
    case EK_SUB: hr = VarDecSub(&dec1, &dec2, &decRes); break;
    case EK_MUL: hr = VarDecMul(&dec1, &dec2, &decRes); break;
    case EK_DIV:
        if ((DECIMAL_HI32(dec2) | DECIMAL_LO32(dec2) | DECIMAL_MID32(dec2)) == 0) {
            compiler()->Error(tree, ERR_IntDivByZero);
            return newError(tree, typeDec);
        }

        hr = VarDecDiv(&dec1, &dec2, &decRes); 
        break;

    case EK_MOD:
    {

        /* n % d = n - d * truncate(n/d) */
        DECIMAL decDiv, decTrunc, decProd;

        if ((DECIMAL_HI32(dec2) | DECIMAL_LO32(dec2) | DECIMAL_MID32(dec2)) == 0) {
            compiler()->Error(tree, ERR_IntDivByZero);
            return newError(tree, typeDec);
        }

        hr = VarDecDiv(&dec1, &dec2, &decDiv);
        if (SUCCEEDED(hr) &&
            SUCCEEDED(hr = VarDecFix(&decDiv, &decTrunc)) &&
            SUCCEEDED(hr = VarDecMul(&decTrunc, &dec2, &decProd)))
        {
            hr = VarDecSub(&dec1, &decProd, &decRes);
        }
        break;
    }

    default:
        fBool = true;
        hr = VarDecCmp(&dec1, &dec2);
        ASSERT(SUCCEEDED(hr));
        switch (ek) {
        default:
            VSFAIL("Bad ek");
        case EK_EQ: fRes = (hr == (HRESULT)VARCMP_EQ); break;
        case EK_NE: fRes = (hr != (HRESULT)VARCMP_EQ); break;
        case EK_LE: fRes = (hr == (HRESULT)VARCMP_LT || hr == (HRESULT)VARCMP_EQ); break;
        case EK_LT: fRes = (hr == (HRESULT)VARCMP_LT); break;
        case EK_GE: fRes = (hr == (HRESULT)VARCMP_GT || hr == (HRESULT)VARCMP_EQ); break;
        case EK_GT: fRes = (hr == (HRESULT)VARCMP_GT); break;
        }
        break;
    }

    if (FAILED(hr)) {
        compiler()->Error(tree, ERR_DecConstError, compiler()->ErrHR(hr));
        return newError(tree, fBool ? GetReqPDT(PT_BOOL) : typeDec);
    }

    // Allocate the result node.
    CONSTVAL cv;
    EXPR * exprRes;

    if (fBool) {
        cv.iVal = fRes;
        exprRes = newExprConstant(tree, GetReqPDT(PT_BOOL), cv);
    }
    else {
        cv.decVal = (DECIMAL *) allocator->Alloc(sizeof(DECIMAL));
        *cv.decVal = decRes;
        exprRes = newExprConstant(tree, typeDec, cv);
    }

    exprRes = AddSideEffects(tree, exprRes, arg2, true);
    return AddSideEffects(tree, exprRes, arg1, true);
}


/***************************************************************************************************
    Handles standard unary decimal based operators.
***************************************************************************************************/
EXPR * FUNCBREC::BindDecUnaOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg)
{
    ASSERT(arg->type->isPredefType(PT_DECIMAL));
    ASSERT(ek == EK_NEG || ek == EK_UPLUS);

    TYPESYM * typeDec = GetOptPDT(PT_DECIMAL);
    ASSERT(typeDec);

    EXPR * argConst = arg->GetConst();

    if (ek == EK_UPLUS)
        return argConst ? arg : newExprBinop(tree, ek, typeDec, arg, NULL);

    if (!argConst) {
        return BindPredefOpAsUD(tree, ek, arg, NULL);
    }

    DECIMAL dec = *argConst->asCONSTANT()->getVal().decVal;
    DECIMAL_SIGN(dec) ^= DECIMAL_NEG;

    // Allocate the result node.
    CONSTVAL cv;

    cv.decVal = (DECIMAL *)allocator->Alloc(sizeof(DECIMAL));
    *cv.decVal = dec;

    EXPR * exprRes = newExprConstant(tree, typeDec, cv);
    return AddSideEffects(tree, exprRes, arg, true);
}


/***************************************************************************************************
    Handles string concatenation.
***************************************************************************************************/
EXPR * FUNCBREC::BindStrBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2)
{
    ASSERT(ek == EK_ADD);
    ASSERT(arg1->type->isPredefType(PT_STRING) || arg2->type->isPredefType(PT_STRING));
    return bindStringConcat(tree, arg1, arg2);
}


/***************************************************************************************************
    Bind a shift operator: <<, >>. These can have integer or long first operands,
    and second operand must be int.
***************************************************************************************************/
EXPR * FUNCBREC::BindShiftOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2)
{
    ASSERT(ek == EK_LSHIFT || ek == EK_RSHIFT);

    ASSERT(arg1->type->isPredefined());
    ASSERT(arg2->type->isPredefType(PT_INT));

    TYPESYM * typeOp = arg1->type;
    PREDEFTYPE ptOp = arg1->type->getPredefType();

    ASSERT(ptOp == PT_INT || ptOp == PT_UINT || ptOp == PT_LONG || ptOp == PT_ULONG);

    CONSTVAL cv;
    cv.iVal = (ptOp == PT_LONG || ptOp == PT_ULONG) ? 0x3f : 0x1f;

    EXPR * exprRes;

    EXPR * argConst2 = arg2->GetConst();
    if (argConst2) {
        cv.iVal = argConst2->asCONSTANT()->getVal().iVal & cv.iVal;
        if (cv.iVal == 0)
            return AddSideEffects(tree, arg1, arg2, false);
        arg2 = AddSideEffects(arg2->tree, newExprConstant(arg2->tree, arg2->type, cv), arg2, true);
    }
    else {
        arg2 = newExprBinop(arg2->tree, EK_BITAND, arg2->type, arg2, newExprConstant(NULL, arg2->type, cv));
    }

    EXPR * argConst1; argConst1 = arg1->GetConst();
    if (argConst1 && argConst1->isZero(false))
        return AddSideEffects(tree, arg1, arg2, false);

    if (!argConst1 || !argConst2)
        return newExprBinop(tree, ek, typeOp, arg1, arg2);

    ASSERT(arg1->type == typeOp);
    ASSERT(arg2->type->isPredefType(PT_INT));

    // Both args are constant so compute the result.
    int cbit = cv.iVal;

    // Fill in the CONSTVAL.
    if (ptOp == PT_LONG || ptOp == PT_ULONG) {
        ASSERT(0 <= cbit && cbit < 0x40);
        unsigned __int64 u1 = *argConst1->asCONSTANT()->getVal().ulongVal;

        cv.ulongVal = (unsigned __int64 *) allocator->Alloc(sizeof(unsigned __int64));

        switch (ek) {
        case EK_LSHIFT: 
            *cv.ulongVal = u1 << cbit;
            break;
        case EK_RSHIFT:
            *cv.ulongVal = (ptOp == PT_LONG) ? (unsigned __int64)((__int64)u1 >> cbit) : (u1 >> cbit);
            break;
        default:
            VSFAIL("Unknown op");
            *cv.ulongVal = 0;
            break;
        }
    }
    else {
        ASSERT(0 <= cbit && cbit < 0x20);
        uint u1 = argConst1->asCONSTANT()->getVal().uiVal;

        switch (ek) {
        case EK_LSHIFT: 
            cv.uiVal = u1 << cbit;
            break;
        case EK_RSHIFT:
            cv.uiVal = (ptOp == PT_INT) ? (uint)((int)u1 >> cbit) : (u1 >> cbit);
            break;
        default:
            VSFAIL("Unknown op");
            cv.uiVal = 0;
            break;
        }
    }

    exprRes = newExprConstant(tree, GetReqPDT(ptOp), cv);
    exprRes = AddSideEffects(tree, exprRes, arg2, true);
    return AddSideEffects(tree, exprRes, arg1, true);
}


/***************************************************************************************************
    Bind a bool binary operator: ==, !=, &&, ||, &, |, ^. If both operands are constant, the
    result will be a constant also.
***************************************************************************************************/
EXPR * FUNCBREC::BindBoolBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2)
{
    ASSERT(arg1->type->isPredefType(PT_BOOL) && arg2->type->isPredefType(PT_BOOL));

    // Get the result type and operand type.
    TYPESYM * typeBool = GetReqPDT(PT_BOOL);

    // For optimizations and for better definite assignment and unreachable code analysis,
    // we determine whether the result values of op1 and op2 are constants. The ops may
    // have side effects even though the result values are constant, eg, (F() | true).
    // We walk past any EK_SEQUENCE nodes to check for a constant result. If one or both
    // results are constant, the result may be constant (or optimizable) but any side effects
    // still need to be performed (and short-circuited in the case of && and ||).

#define NOT(a) newExprBinop(tree, EK_LOGNOT, typeBool, (a), NULL)
#define VAL(a) newExprConstant(tree, typeBool, ConstValInit(a))

    // Determine if arg1 has a constant value.
    // Strip off EK_SEQUENCE for constant checking.
    EXPR * argConst1 = arg1->GetConst();
    EXPR * argConst2 = arg2->GetConst();

    if (!argConst1 && !argConst2) {
        // General case - no optimizations.
        return newExprBinop(tree, ek, typeBool, arg1, arg2);
    }

    bool f1 = argConst1 && argConst1->asCONSTANT()->getVal().iVal;
    bool f2 = argConst2 && argConst2->asCONSTANT()->getVal().iVal;

    int nConst = (argConst1 ? 1 : 0) | (argConst2 ? 2 : 0);
    ASSERT(1 <= nConst && nConst <= 3);

    bool fInvert = false;
    EXPR * exprRes;

    switch (ek) {
    default:
        VSFAIL("Shouldn't happen");
        return badOperatorTypesError(tree, arg1, arg2, typeBool);

    case EK_BITXOR: // Same as NE.
    case EK_NE:
        fInvert = true; // Fall through.
    case EK_EQ:
        switch (nConst) {
        case 1: // arg1
            exprRes = (f1 ^ fInvert) ? arg2 : NOT(arg2);
            return AddSideEffects(tree, exprRes, arg1, true);
        case 2: // arg2
            exprRes = (f2 ^ fInvert) ? arg1 : NOT(arg1);
            return AddSideEffects(tree, exprRes, arg2, false);
        case 3: // Both
            exprRes = VAL(!(f1 ^ f2 ^ fInvert));
            exprRes = AddSideEffects(tree, exprRes, arg2, true);
            return AddSideEffects(tree, exprRes, arg1, true);
        }
        break;

    case EK_BITOR:
        fInvert = true; // Fall through.
    case EK_BITAND:
        switch (nConst) {
        case 1: // arg1
            return (f1 ^ fInvert) ?
                AddSideEffects(tree, arg2, arg1, true) :
                AddSideEffects(tree, arg1, arg2, false);
        case 2: // arg2
            return (f2 ^ fInvert) ?
                AddSideEffects(tree, arg1, arg2, false) :
                AddSideEffects(tree, arg2, arg1, true);
        case 3: // Both
            exprRes = VAL(fInvert ? (f1 | f2) : (f1 & f2));
            exprRes = AddSideEffects(tree, exprRes, arg2, true);
            return AddSideEffects(tree, exprRes, arg1, true);
        }
        break;

    case EK_LOGOR:
        fInvert = true; // Fall through.
    case EK_LOGAND:
        switch (nConst) {
        case 1: // arg1
            if (f1 ^ fInvert)
                return AddSideEffects(tree, arg2, arg1, true);
            // Since these are the short-circuiting operators we don't add side effects for arg2!
            // Warn if we're dropping code that has side effects.
            if (arg2->hasSideEffects(compiler()))
                compiler()->Error(arg2->tree, WRN_UnreachableExpr);
            // Since arg2 isn't a constant, we need to make the result a non-constant (and non-lvalue).
            return EnsureNonConstNonLvalue(tree, arg1);

        case 2: // arg2
            if (arg2->hasSideEffects(compiler()))
                exprRes = newExprBinop(tree, ek, typeBool, arg1, arg2);
            else
                exprRes = arg1;
            if (!(f2 ^ fInvert)) {
                // Result is always f2, but execution of f2 is conditioned on f1.
                exprRes = AddSideEffects(tree, VAL(f2), exprRes, true, true);
            }
            return EnsureNonConstNonLvalue(tree, exprRes);

        case 3: // Both
            if (f1 ^ fInvert) {
                exprRes = VAL(f2);
                exprRes = AddSideEffects(tree, exprRes, arg2, true);
                return AddSideEffects(tree, exprRes, arg1, true);
            }

            // Since these are the short-circuiting operators we don't add side effects for arg2!
            // Warn if we're dropping code that has side effects.
            if (arg2->hasSideEffects(compiler()))
                compiler()->Error(arg2->tree, WRN_UnreachableExpr);
            // If either isn't a constant, we need to make the result a non-constant.
            if (arg1->kind == EK_CONSTANT && arg2->kind == EK_CONSTANT)
                return arg1;
            return EnsureNonConstNonLvalue(tree, arg1);
        }
        break;
    }

#undef NOT
#undef VAL

    VSFAIL("Shouldn't be here");
    return newError(tree, typeBool);
}


/***************************************************************************************************
    Bind a bitwise bool binary operator: &, |. This handles the nullable cases directly and
    forwards the non-nullable case to BindBoolBinOp.
***************************************************************************************************/
EXPR * FUNCBREC::BindBoolBitwiseOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * expr1, EXPR * expr2)
{
    ASSERT(ek == EK_BITAND || ek == EK_BITOR);
    ASSERT(expr1->type->isPredefType(PT_BOOL) || expr1->type->isNUBSYM() && expr1->type->asNUBSYM()->baseType()->isPredefType(PT_BOOL));
    ASSERT(expr2->type->isPredefType(PT_BOOL) || expr2->type->isNUBSYM() && expr2->type->asNUBSYM()->baseType()->isPredefType(PT_BOOL));

    if (!expr1->type->isNUBSYM() && !expr2->type->isNUBSYM())
        return BindBoolBinOp(tree, ek, flags, expr1, expr2);

    if (expr1->type->isNUBSYM())
        expr1 = StripNubCtor(expr1);
    if (expr2->type->isNUBSYM())
        expr2 = StripNubCtor(expr2);

    if (!expr1->type->isNUBSYM() && !expr2->type->isNUBSYM()) {
        // We stripped away all the NUBSYMs. The result is simply a wrap
        // of the normal result.
        return BindNubNew(tree, BindBoolBinOp(tree, ek, flags, expr1, expr2));
    }

    EXPR * exprConst;
    EXPR * exprCond;
    EXPR * exprRes;
    int iexpr;

    // Get the result type.
    TYPESYM * typeBool = GetReqPDT(PT_BOOL);
    TYPESYM * typeRes = compiler()->getBSymmgr().GetNubType(typeBool);

    EXPR * rgexpr[2];
    rgexpr[0] = expr1;
    rgexpr[1] = expr2;

    bool fInvert = false;

    NubInfo nin;
    nin.Init();

    switch (ek) {
    default:
        VSFAIL("Shouldn't happen");
        return badOperatorTypesError(tree, rgexpr[0], rgexpr[1], typeRes);

    case EK_BITAND:
        /***************************************************************************************************
            When only b is nullable, generate:
                rev(a, tmp = b) ? tmp : new bool?(false);
            When only a is nullable, generate:
                seq(tmp = a, b) ? tmp : new bool?(false);
            When both a and b are nullable, generate the equivalent of:
                tmp1 = a;
                tmp2 = b;
                tmp1.GetValOrDef() || !(tmp2.GetValOrDef() || tmp1.HasValue) ? tmp2 : tmp1;
        ***************************************************************************************************/
        fInvert = true;
        // Fall through.
    case EK_BITOR:
        /***************************************************************************************************
            When only b is nullable, generate:
                rev(a, tmp = b) ? new bool?(true) : tmp;
            When only a is nullable, generate:
                seq(tmp = a, b) ? new bool?(true) : tmp;
            When both a and b are nullable, generate the equivalent of:
                tmp1 = a;
                tmp2 = b;
                tmp1.GetValOrDef() || !(tmp2.GetValOrDef() || tmp1.HasValue) ? tmp1 : tmp2;
        ***************************************************************************************************/
        for (iexpr = 0; iexpr < 2; iexpr++) {
            if (rgexpr[iexpr]->type->isNUBSYM())
                continue;

            exprConst = rgexpr[iexpr]->GetConst();
            if (exprConst) {
                if (!exprConst->asCONSTANT()->getVal().iVal ^ fInvert) {
                    // seq(a, b);
                    return AddSideEffects(tree, rgexpr[1 - iexpr], rgexpr[iexpr], !iexpr, true);
                }

                // seq(a, b, new bool?(!fInvert));
                exprRes = AddSideEffects(tree, BindNubConstBool(tree, !fInvert), rgexpr[1], true, true);
                return AddSideEffects(tree, exprRes, rgexpr[0], true, true);
            }

            BindNubSave(rgexpr[1 - iexpr], nin, 1 - iexpr, true);
            exprRes = AddSideEffects(tree, rgexpr[iexpr], nin.rgexprPre[1 - iexpr], !!iexpr, true);
            exprRes = BindQmark(tree, exprRes, BindNubConstBool(tree, !fInvert), nin.rgexprTmp[1 - iexpr], fInvert);
            return AddSideEffects(tree, exprRes, nin.rgexprPst[1 - iexpr], false, true);
        }

        // Both are nullable.
        ASSERT(rgexpr[0]->type->isNUBSYM() && rgexpr[1]->type->isNUBSYM());

        bool rgfNull[2];
        rgfNull[0] = !BindNubSave(rgexpr[0], nin, 0, true);
        rgfNull[1] = !BindNubSave(rgexpr[1], nin, 1, true);

        if (rgfNull[0] == rgfNull[1]) {
            if (rgfNull[0]) {
                // Both are null, so result is null.
                // seq(a, b);
                return AddSideEffects(tree, rgexpr[1], rgexpr[0], true, true);
            }

            EXPR * exprLeft = BindNubGetValOrDef(tree, nin.rgexprTmp[1]);
            EXPR * exprRight = BindNubHasValue(tree, nin.rgexprTmp[0]);
            exprRight = newExprBinop(tree, EK_LOGOR, typeBool, exprLeft, exprRight);
            exprRight = newExprBinop(tree, EK_LOGNOT, typeBool, exprRight, NULL);
            exprLeft = BindNubGetValOrDef(tree, nin.rgexprTmp[0]);
            exprLeft = AddSideEffects(tree, exprLeft, nin.rgexprPre[1], true, true);
            exprLeft = AddSideEffects(tree, exprLeft, nin.rgexprPre[0], true, true);
            exprCond = newExprBinop(tree, EK_LOGOR, typeBool, exprLeft, exprRight);
            iexpr = 0;
        }
        else {
            iexpr = !!rgfNull[0];

            // When iexpr == 1, rgexpr[0] is null. Evaluate as: seq(tmp1 = a, tmp2 = b).GetValOrDef() ? tmp2 : tmp1;
            // Note that tmp1 will not actually be code-gened.
            exprCond = AddSideEffects(tree, nin.rgexprPre[iexpr], nin.rgexprPre[1 - iexpr], !!iexpr, true);
            exprCond = BindNubGetValOrDef(tree, exprCond);
        }
        return BindQmark(tree, exprCond, BindNubFetchAndFree(tree, nin, iexpr), BindNubFetchAndFree(tree, nin, 1 - iexpr), fInvert);
    }
}


/***************************************************************************************************
    Handles boolean unary operater (!).
***************************************************************************************************/
EXPR * FUNCBREC::BindBoolUnaOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg)
{
    ASSERT(arg->type->isPredefType(PT_BOOL));
    ASSERT(ek == EK_LOGNOT);

    // Get the result type and operand type.
    TYPESYM * typeBool = GetReqPDT(PT_BOOL);

    // Determine if arg has a constant value.
    // Strip off EK_SEQUENCE for constant checking.

    EXPR * argConst = arg->GetConst();

    if (!argConst)
        return newExprBinop(tree, EK_LOGNOT, typeBool, arg, NULL);

    bool fRes = !!argConst->asCONSTANT()->getVal().iVal;
    return AddSideEffects(tree, newExprConstant(tree, typeBool, ConstValInit(!fRes)), arg, true);
}


/***************************************************************************************************
    Handles string equality.
***************************************************************************************************/
EXPR * FUNCBREC::BindStrCmpOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2)
{
    ASSERT(ek == EK_EQ || ek == EK_NE);
    ASSERT(arg1->type->isPredefType(PT_STRING) && arg2->type->isPredefType(PT_STRING));

    if (arg1->kind == EK_CONSTANT && arg2->kind == EK_CONSTANT) {
        // Both strings are constants. Compare values at compile time.
        bool fRes;

        STRCONST * str1 = arg1->asCONSTANT()->getSVal().strVal;
        STRCONST * str2 = arg2->asCONSTANT()->getSVal().strVal;
        if (!str1 || !str2)
            fRes = (str1 == str2);
        else
            fRes = (str1->length == str2->length && !memcmp(str1->text, str2->text, str1->length * sizeof(WCHAR)));

        if (ek == EK_NE)
            fRes = !fRes;

        return newExprConstant(tree, GetReqPDT(PT_BOOL), ConstValInit(fRes));
    }

    if (arg1->isNull() || arg2->isNull())
        return BindRefCmpOp(tree, ek, flags, arg1, arg2);

    return BindPredefOpAsUD(tree, ek, arg1, arg2);
}


/***************************************************************************************************
    Handles reference equality operators. Type variables come through here.
***************************************************************************************************/
EXPR * FUNCBREC::BindRefCmpOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2)
{
    ASSERT(ek == EK_EQ || ek == EK_NE);

    // Check if maybe you are making a mistake... :)
    if (arg1->type != arg2->type) {
        if ((arg2->kind != EK_CONSTANT || arg2->asCONSTANT()->getSVal().strVal) && HasSelfCompare(arg1->type, ek))
            compiler()->Error(tree, WRN_BadRefCompareRight, arg1->type);
        else if ((arg1->kind != EK_CONSTANT || arg1->asCONSTANT()->getSVal().strVal) && HasSelfCompare(arg2->type, ek))
            compiler()->Error(tree, WRN_BadRefCompareLeft, arg2->type);
    }

    if (arg1->kind == EK_CONSTANT && arg2->kind == EK_CONSTANT) {
        // Both references are constants. Compare values at compile time. Only null vs. non-null can possibly matter here.
        bool fRes = (!arg1->asCONSTANT()->getSVal().strVal && !arg2->asCONSTANT()->getSVal().strVal);
        if (ek == EK_NE)
            fRes = !fRes;
        return newExprConstant(tree, GetReqPDT(PT_BOOL), ConstValInit(fRes));
    }

    // Must box type variables for the verifier.
    arg1 = mustConvert(arg1, GetReqPDT(PT_OBJECT), NOUDC);
    arg2 = mustConvert(arg2, GetReqPDT(PT_OBJECT), NOUDC);

    return newExprBinop(tree, ek, GetReqPDT(PT_BOOL), arg1, arg2);
}


/***************************************************************************************************
    Handles delegate binary operators.
***************************************************************************************************/
EXPR * FUNCBREC::BindDelBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2)
{
    if (ek == EK_EQ || ek == EK_NE) {
        if (arg1->isNull() || arg2->isNull())
            return BindRefCmpOp(tree, ek, flags, arg1, arg2);

        ASSERT(arg1->type == arg2->type && (arg1->type->isDelegateType() || arg1->type->isPredefType(PT_DELEGATE)));
        arg1 = mustConvert(arg1, GetReqPDT(PT_DELEGATE), NOUDC);
        arg2 = mustConvert(arg2, GetReqPDT(PT_DELEGATE), NOUDC);

        return BindPredefOpAsUD(tree, ek, arg1, arg2);
    }

    ASSERT(ek == EK_ADD || ek == EK_SUB);
    ASSERT(arg1->type == arg2->type && arg1->type->isDelegateType());

    // Construct argument list from the two delegates.
    EXPR * args = newExprBinop(NULL, EK_LIST, getVoidType(), arg1, arg2);

    // Find and bind the Delegate.Combine or Delegate.Remove call.
    EXPR * expr = BindPredefMethToArgs(tree, (ek == EK_ADD ? PN_COMBINE : PN_REMOVE), GetReqPDT(PT_DELEGATE), NULL, args);

    // Cast the result to the delegate type.
    return mustCastCore(expr, arg1->type, tree);
}


/***************************************************************************************************
    Handles enum binary operators.
***************************************************************************************************/
EXPR * FUNCBREC::BindEnumBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2)
{
    ASSERT(arg1->type->isEnumType() || arg2->type->isEnumType());

    AGGTYPESYM * type1 = arg1->type->asAGGTYPESYM();
    AGGTYPESYM * type2 = arg2->type->asAGGTYPESYM();

    AGGTYPESYM * typeEnum = type1->isEnumType() ? type1 : type2;

    ASSERT(type1 == typeEnum || type1 == typeEnum->underlyingEnumType());
    ASSERT(type2 == typeEnum || type2 == typeEnum->underlyingEnumType());

    AGGTYPESYM * typeDst = typeEnum;

    switch (ek) {
    case EK_BITAND:
    case EK_BITOR:
    case EK_BITXOR:
        ASSERT(type1 == type2);
        break;

    case EK_ADD:
        ASSERT(type1 != type2);
        break;

    case EK_SUB:
        if (type1 == type2)
            typeDst = typeEnum->underlyingEnumType();
        break;

    default:
        ASSERT(EK_RELATIONAL_MIN <= ek && ek <= EK_RELATIONAL_MAX);
        typeDst = GetReqPDT(PT_BOOL);
        break;
    }

    PREDEFTYPE ptOp;

    switch (typeEnum->fundType()) {
    default:
        // Promote all smaller types to int.
        ptOp = PT_INT;
        break;
    case FT_U4:
        ptOp = PT_UINT;
        break;
    case FT_I8:
        ptOp = PT_LONG;
        break;
    case FT_U8:
        ptOp = PT_ULONG;
        break;
    }

    TYPESYM * typeOp = GetReqPDT(ptOp);
    arg1 = mustCast(arg1, typeOp, NOUDC);
    arg2 = mustCast(arg2, typeOp, NOUDC);

    EXPR * exprRes = BindIntOp(tree, ek, flags, arg1, arg2, ptOp);

    if (!exprRes->isOK())
        return newError(tree, typeDst);

    if (exprRes->type != typeDst) {
        ASSERT(!typeDst->isPredefType(PT_BOOL));
        exprRes = mustCast(exprRes, typeDst, NOUDC);
    }

    return exprRes;
}


/***************************************************************************************************
    Handles enum unary operator (~).
***************************************************************************************************/
EXPR * FUNCBREC::BindEnumUnaOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg)
{
    ASSERT(arg->type->isEnumType());
    ASSERT(ek == EK_BITNOT);

    PREDEFTYPE ptOp;
    TYPESYM * typeEnum = arg->type;

    switch (typeEnum->fundType()) {
    default:
        // Promote all smaller types to int.
        ptOp = PT_INT;
        break;
    case FT_U4:
        ptOp = PT_UINT;
        break;
    case FT_I8:
        ptOp = PT_LONG;
        break;
    case FT_U8:
        ptOp = PT_ULONG;
        break;
    }

    TYPESYM * typeOp = GetReqPDT(ptOp);
    arg = mustCast(arg, typeOp, NOUDC);

    EXPR * exprRes = BindIntOp(tree, ek, flags, arg, NULL, ptOp);

    if (!exprRes->isOK())
        return newError(tree, typeEnum);

    CHECKEDCONTEXT checked(this, false);
    exprRes = mustCast(exprRes, typeEnum, NOUDC);
    checked.restore(this);

    return exprRes;
}


/***************************************************************************************************
    Handles pointer binary operators (+ and -).
***************************************************************************************************/
EXPR * FUNCBREC::BindPtrBinOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2)
{
    ASSERT(arg1->type->isPTRSYM() || arg2->type->isPTRSYM());
    ASSERT(ek == EK_ADD || ek == EK_SUB);

    EXPR * expr;

    checkUnsafe(tree);

    if (ek == EK_SUB && arg1->type == arg2->type) {
        if (arg1->type->asPTRSYM()->baseType()->isVOIDSYM()) {
            compiler()->Error(tree, ERR_VoidError);
            return newError(tree, GetReqPDT(PT_LONG));
        }

        // Substract and divide by the size of the underlying type...
        expr = newExprBinop(tree, EK_SUB, GetReqPDT(PT_INTPTR), arg1, arg2); // the type here must be signed...
        expr = newExprBinop(tree, EK_DIV, GetReqPDT(PT_INTPTR), expr, bindSizeOf(tree, arg1->type->parent->asTYPESYM()));
        EXPRCAST * exprCast = newExpr(tree, EK_CAST, GetReqPDT(PT_LONG))->asCAST();
        exprCast->p1 = expr;

        return exprCast;
    }

    EXPR * exprNum;
    EXPR * exprPtr;

    if (arg1->type->isPTRSYM()) {
        exprNum = arg2;
        exprPtr = arg1;
    }
    else {
        ASSERT(ek == EK_ADD);
        exprNum = arg1;
        exprPtr = arg2;
    }

    ASSERT(exprNum->type->isPredefined());

    if (exprPtr->type->asPTRSYM()->baseType()->isVOIDSYM()) {
        compiler()->Error(tree, ERR_VoidError);
        return newError(tree, exprPtr->type);
    }

    EXPR * exprSize = bindSizeOf(tree, exprPtr->type->parent->asTYPESYM());
    expr = bindPtrAddMul(tree, exprPtr, exprNum, exprSize, arg1->type->isPTRSYM(), ek);

    if (expr->kind == ek)
        expr->flags |= flags;   

    return expr;
}


/***************************************************************************************************
    Handles pointer comparison operators.
***************************************************************************************************/
EXPR * FUNCBREC::BindPtrCmpOp(BASENODE * tree, EXPRKIND ek, uint flags, EXPR * arg1, EXPR * arg2)
{
    ASSERT(arg1->type->isPTRSYM() && arg2->type->isPTRSYM());

    EXPR * expr;

    checkUnsafe(tree);

    // this is just a simple direct comparison...
    expr = newExprBinop(tree, ek, GetReqPDT(PT_BOOL), arg1, arg2);
    expr->flags |= flags;
    return expr;
}


/***************************************************************************************************
    Given a binary operator EXPRKIND, get the BinOpKind and flags.

    REVIEW ShonK: Use a lookup table of some sort. It'd work best if we had the OPERATOR instead
    of EXPRKIND.
***************************************************************************************************/
bool FUNCBREC::GetBokAndFlags(EXPRKIND ek, BinOpKindEnum * pbok, uint * pflags)
{
    uint flags = 0;

    switch (ek) {
    case EK_ADD:
        if (checked.normal)
            flags |= EXF_CHECKOVERFLOW;
        *pbok = BinOpKind::Add;
        break;
    case EK_SUB:
        if (checked.normal)
            flags |= EXF_CHECKOVERFLOW;
        *pbok = BinOpKind::Sub;
        break;
    case EK_DIV:
    case EK_MOD:
        flags |= EXF_ASSGOP;
    case EK_MUL:
        if (checked.normal)
            flags |= EXF_CHECKOVERFLOW;
        *pbok = BinOpKind::Mul;
        break;
    case EK_BITAND:
    case EK_BITOR:
        *pbok = BinOpKind::Bitwise;
        break;
    case EK_BITXOR:
        *pbok = BinOpKind::BitXor;
        break;
    case EK_LSHIFT:
    case EK_RSHIFT:
        *pbok = BinOpKind::Shift;
        break;
    case EK_LOGOR: 
    case EK_LOGAND:
        *pbok = BinOpKind::Logical;
        break;
    case EK_LT:
    case EK_LE:
    case EK_GT:
    case EK_GE:
        *pbok = BinOpKind::Compare;
        break;
    case EK_EQ:
    case EK_NE:
        *pbok = BinOpKind::Equal;
        break;
    default:
        VSFAIL("Bad ek");
        return false;
    }

    *pflags = flags;
    return true;
}


// This table indicates for predefined types through object which are better for the purposes of overload resolution.
// 0 means they're the same, 1 means the left index is better, 2 means the right, 3 means neither. These values MUST
// match the values of the BetterType enum.
const byte FUNCBREC::g_mpptptBetter[NUM_EXT_TYPES + 1][NUM_EXT_TYPES + 1] = {
//          BYTE    SHORT   INT     LONG    FLOAT   DOUBLE  DECIMAL CHAR    BOOL    SBYTE   USHORT  UINT    ULONG   IPTR     UIPTR    OBJECT
/* BYTE   */{0,     1,      1,      1,      1,      1,      1,      3,      3,      2,      1,      1,      1,      3,       3,       1},
/* SHORT  */{2,     0,      1,      1,      1,      1,      1,      3,      3,      2,      1,      1,      1,      3,       3,       1},
/* INT    */{2,     2,      0,      1,      1,      1,      1,      2,      3,      2,      2,      1,      1,      3,       3,       1},
/* LONG   */{2,     2,      2,      0,      1,      1,      1,      2,      3,      2,      2,      2,      1,      3,       3,       1},
/* FLOAT  */{2,     2,      2,      2,      0,      1,      3,      2,      3,      2,      2,      2,      2,      3,       3,       1},
/* DOUBLE */{2,     2,      2,      2,      2,      0,      3,      2,      3,      2,      2,      2,      2,      3,       3,       1},
/* DECIMAL*/{2,     2,      2,      2,      3,      3,      0,      2,      3,      2,      2,      2,      2,      3,       3,       1},
/* CHAR   */{3,     3,      1,      1,      1,      1,      1,      0,      3,      3,      1,      1,      1,      3,       3,       1},
/* BOOL   */{3,     3,      3,      3,      3,      3,      3,      3,      0,      3,      3,      3,      3,      3,       3,       1},
/* SBYTE  */{1,     1,      1,      1,      1,      1,      1,      3,      3,      0,      1,      1,      1,      3,       3,       1},
/* USHORT */{2,     2,      1,      1,      1,      1,      1,      2,      3,      2,      0,      1,      1,      3,       3,       1},
/* UINT   */{2,     2,      2,      1,      1,      1,      1,      2,      3,      2,      2,      0,      1,      3,       3,       1},
/* ULONG  */{2,     2,      2,      2,      1,      1,      1,      2,      3,      2,      2,      2,      0,      3,       3,       1},
/* IPTR   */{3,     3,      3,      3,      3,      3,      3,      3,      3,      3,      3,      3,      3,      0,       3,       1},
/* UIPTR  */{3,     3,      3,      3,      3,      3,      3,      3,      3,      3,      3,      3,      3,      3,       0,       1},
/* OBJECT */{2,     2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,       2,       0}
};


