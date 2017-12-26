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
// File: fncpbind.cpp
//
// Routines for analyzing the bound body of a function
// ===========================================================================

#include "stdafx.h"

/***************************************************************************************************
    This does the following:
    * Warn on unreferenced variables.
    * Do reachability analysis and warn on unreachable statements.
    * Determine whether the method (and anon methods) have any returns out of try-catch blocks.
    * Realize gotos and error on untargetable ones.
    * Warn on untargeted user defined labels.
    * Error on any label that shadows another label.
    * Check definite assignment and error on bad variable use.
    * Check definite assignment of out parameters.
    * Error if a method returning a value has a reachable end.
    * Realize string concatenations (EK_CONCAT).
    * Mark locals that need to be hoisted for anonymous methods.
    * Mark AnonMethInfos that are in reachable code.

    This assumes that the reachability is already set for all anon meth bodies. This should be
    done when the anon method expr tree is created (in bindAnonymousMethod).

    This does the definite assignment analysis on the main method and all anonymous methods. This
    should be called before anything is "rewritten" (for anon meths or iterators).
***************************************************************************************************/
void FUNCBREC::PostBindCompile(EXPRBLOCK * block)
{
    if (unreferencedVarCount)
        ReportUnreferencedVars(pOuterScope);

    if (info->yieldType)
        CheckForIteratorErrors();

    // Binding linked these in reverse order.
    ReverseAnonMeths(&m_pamiFirst);

    // NOTE: Reachability for an anon meth is set when the anon meth is bound.
    ReachabilityChecker reach(compiler());

    reach.SetReachability(block, true);
    info->hasRetAsLeave = reach.HasRetAsLeave();

    // For error reporting, realize all remaining gotos.
    for (EXPRGOTO * gtCur = gotos; gtCur; gtCur = gtCur->prev)
        reach.RealizeGoto(gtCur, false);

    // Now that all gotos have been resolved, proceed to unreferened labels:
    EXPRLOOP(userLabelList, exprLab)
        EXPRLABEL * lab = exprLab->asANYLABEL();
        if (lab->kind == EK_LABEL) {
            if (!(lab->flags & EXF_LABELREFERENCED))
                compiler()->Error(lab->tree, WRN_UnreferencedLabel);
            for (SCOPESYM * scope = lab->scope; (scope = scope->parent->asSCOPESYM()) != NULL; ) {
                SYM * sym = compiler()->getLSymmgr().LookupLocalSym(lab->label->name, scope, MASK_LABELSYM);
                if (sym) {
                    compiler()->Error(lab->tree, ERR_LabelShadow, lab->label->name);
                    break;
                }
            }
        }
    ENDLOOP;

    // This does all of the following:
    // * Check definite assignment.
    // * Realize string concatenation.
    // * Mark locals that need to be hoisted for anonymous methods (see ScanLocal).
    // * Mark AnonMethInfos that are in reachable code.
    // Since this does more than just definite assignment analysis, we always do this,
    // even if there are no uninited vars.
    FlowChecker::CheckDefiniteAssignment(compiler(), bindCallback.GetTarget(), allocator,
        block, info, m_pamiFirst, uninitedVarCount);
}


/***************************************************************************************************
    Check for iterator specific errors:
    * no out/ref parameters
    * no unsafe code
    * no varargs
***************************************************************************************************/
void FUNCBREC::CheckForIteratorErrors()
{
    ASSERT(info->yieldType);

    bool fUnsafeError = false;

    for (int i = 0; i < info->cpin; i++) {
        PARAMETERNODE * nodeParam = info->rgpin[i].node;
        if (nodeParam) {
            if (nodeParam->flags & (NF_PARMMOD_REF | NF_PARMMOD_OUT))
                compiler()->Error(nodeParam, ERR_BadIteratorArgType);
            if (info->meth->params->Item(i)->isUnsafe()) {
                fUnsafeError = true;
                compiler()->Error(nodeParam, ERR_UnsafeIteratorArgType);
            }
        }
    }

    // No varargs
    if (info->meth->isVarargs)
        compiler()->ErrorRef(NULL, ERR_VarargsIterator, info->meth);

    BASENODE * node = info->meth->getParseTree();

    if (!node->InGroup(NG_METHOD)) {
        // Get unsafe from the property node, not accessor node.
        node = node->pParent;
    }

    if (node->flags & NF_MOD_UNSAFE) {
        // You can't put unsafe right on the iterator declaration
        compiler()->ErrorRef(NULL, ERR_IllegalInnerUnsafe, info->meth);
    }
    else if (info->meth->isUnsafe && info->yieldType->isUnsafe()) {
        compiler()->ErrorRef(NULL, ERR_UnsafeIteratorArgType, info->meth);
    }
    else if (info->unsafeTree && !fUnsafeError) {
        // You can't have unsafe code blocks (we do this check here due to ordering issues)
        compiler()->Error(info->unsafeTree, ERR_IllegalInnerUnsafe);
    }
}


/***************************************************************************************************
    The anon meth info lists are created in reverse order. This puts them back in source order.
    This is just so any errors get reported in a natural order.
***************************************************************************************************/
void FUNCBREC::ReverseAnonMeths(AnonMethInfo ** ppami)
{
    AnonMethInfo * pamiCur = *ppami;
    *ppami = NULL;

    while (pamiCur) {
        if (pamiCur->pamiChild)
            ReverseAnonMeths(&pamiCur->pamiChild);
        AnonMethInfo * pamiNext = pamiCur->pamiNext;
        pamiCur->pamiNext = *ppami;
        *ppami = pamiCur;
        pamiCur = pamiNext;
    }
}


/***************************************************************************************************
    Realize the given string concatenation.
***************************************************************************************************/
void FUNCBREC::RealizeStringConcat(EXPRCONCAT * expr)
{
    BOOL allStrings = true;  // Are all arguments to the concat strings?
    AGGTYPESYM * typeString = GetReqPDT(PT_STRING);

    ASSERT(expr->flags & EXF_UNREALIZEDCONCAT);

    expr->flags &= ~EXF_UNREALIZEDCONCAT;

    // Scan the list of things being concatinated to see if they are 
    // all strings or not.
    EXPRLOOP(expr->list, item) 
        if (item->type != typeString) {
            allStrings = false;
            break;
        }
    ENDLOOP;
    // We compile this to 4 different possibilities, depending on the arguments:
    // 1. <= 4 string arguments.
    //     string.Concat(string, string) or string.Concat(string, string, string)
    // 2. > 4 string arguments
    //     string.Concat(string[])
    // 3. <= 3 arguments, at least one not a string
    //     string.Concat(object, object) or string.Concat(object, object, object)
    // 4. > 3 arguments, at least one not a string
    //     string.Concat(object[])

    EXPR * args;
    if (expr->count > (allStrings ? 4U : 3U)) {
        // Create array from arguments.
        args = newExpr(NULL, EK_ARRINIT, compiler()->getBSymmgr().GetArray(GetReqPDT(allStrings ? PT_STRING : PT_OBJECT), 1));
        args->asARRINIT()->args = expr->list;
        args->asARRINIT()->dimSize = expr->count;
        args->asARRINIT()->dimSizes = &(args->asARRINIT()->dimSize);
        args->flags |= (expr->flags & EXF_PUSH_OP_FIRST);
    } else {
        args = expr->list;
    }

    // Call static method String.Concat.
    EXPR * call = BindPredefMethToArgs(expr->tree, PN_STRCONCAT, typeString, NULL, args);

    expr->list = call;
}


/***************************************************************************************************
    Warn on unused locals.
***************************************************************************************************/
void FUNCBREC::ReportUnreferencedVars(SCOPESYM * scope)
{
    for (SYM * symCur = scope->firstChild; symCur; symCur = symCur->nextChild) {
        switch (symCur->getKind()) {
        case SK_LOCVARSYM:
            if (!symCur->asLOCVARSYM()->slot.isReferenced && !symCur->asLOCVARSYM()->slot.isParam) {
                compiler()->Error(symCur->asLOCVARSYM()->declTree,
                    symCur->asLOCVARSYM()->slot.isReferencedAssg ? WRN_UnreferencedVarAssg : WRN_UnreferencedVar, symCur);
            }
            break;    
        case SK_SCOPESYM:
            ReportUnreferencedVars(symCur->asSCOPESYM());
            break;
        case SK_ANONSCOPESYM:
            ReportUnreferencedVars(symCur->asANONSCOPESYM()->scope);
            break;
        default:
            break;
        }
    }
}


/***************************************************************************************************
    Returns true iff the local is definitely assigned according to the given bitset. If a local
    has no definite assignment slot (when jbit is zero), it is always assigned. If pfPartial is
    not null and the local is not completely definitely assigned, *pfPartial is set to indicate
    whether any fields of the local are definitely assigned.
***************************************************************************************************/
bool FUNCBREC::IsLocalDefinitelyAssigned(LOCVARSYM *local, BitSet & bset, bool * pfPartial)
{
    if (pfPartial)
        *pfPartial = false;

    int ibit = local->slot.JbitDefAssg() - 1;
    if (ibit < 0)
        return true;

    int cbit = FlowChecker::GetCbit(compiler(), local->type);

    if (bset.TestAllRange(ibit, ibit + cbit))
        return true;

    if (pfPartial)
        *pfPartial = cbit > 1 && bset.TestAnyRange(ibit, ibit + cbit);
    return false;
}


/***************************************************************************************************
    Scan the main method body and all anonymous methods. This is the main entry point.
***************************************************************************************************/
void FlowChecker::ScanAll(EXPRBLOCK * block, AnonMethInfo * pamiFirst)
{
    ASSERT(m_info);

    BitSet bsetCur(m_cvarUninit, m_heap);
    BitSet bsetErr(m_cvarUninit, m_heap);

    m_pami = NULL;
    m_tsFinallyScanCur = 0;
    m_tsFinallyScanPrev = 0;

    SCOPESYM * scopeParams = m_info->outerScope;

    // Get the LOCVARSYM for "this" if we need to check definite assignment for it.
    m_locThis = compiler()->getLSymmgr().LookupLocalSym(compiler()->namemgr->GetPredefName(PN_THIS), scopeParams, MASK_LOCVARSYM)->asLOCVARSYM();
    if (m_locThis && m_locThis->slot.JbitDefAssg() == 0)
        m_locThis = NULL;
    BuildOutParamList(scopeParams);

    if (ScanBlock(block, bsetCur, bsetErr))
        ReportReturnNeeded(block, m_info->meth->parseTree, bsetCur);

    // Do definite assignment analysis on the anon methods.
    ScanAnonMeths(pamiFirst, bsetCur, bsetErr);
}


/***************************************************************************************************
    Build a SYMLIST for the out parameters, given m_scopeParams.
***************************************************************************************************/
void FlowChecker::BuildOutParamList(SCOPESYM * scopeParams)
{
    m_listOutParams = NULL;
    SYMLIST ** plistLast = &m_listOutParams;
    for (SYM * sym = scopeParams->firstChild; sym; sym = sym->nextChild) {
        int ibitMin;
        if (sym->isLOCVARSYM() && (ibitMin = sym->asLOCVARSYM()->slot.JbitDefAssg() - 1) >= 0 && !sym->asLOCVARSYM()->isThis)
            compiler()->getLSymmgr().AddToLocalSymList(sym, &plistLast);
    }
}


/***************************************************************************************************
    Scan the anonymous methods. The bitsets are just passed in to share the memory.
***************************************************************************************************/
void FlowChecker::ScanAnonMeths(AnonMethInfo * pami, BitSet & bsetCur, BitSet & bsetErr)
{
    for ( ; pami; pami = pami->pamiNext) {
        if (!pami->bsetEnter.FInited())
            bsetCur.SetBitRange(0, m_cvarUninit, m_heap);
        else
            bsetCur.Set(pami->bsetEnter, m_heap);
        bsetCur.ClearBitRange(pami->jbitMin - 1, pami->jbitLim - 1);

        m_pami = pami;
        m_tsFinallyScanCur = 0;
        m_tsFinallyScanPrev = 0;
        m_locThis = NULL;
        BuildOutParamList(m_pami->pArgs);

        if (ScanBlock(pami->pBodyExpr, bsetCur, bsetErr))
            ReportReturnNeeded(pami->pBodyExpr, pami->tree, bsetCur);
        ASSERT(m_pami == pami);

        ScanAnonMeths(pami->pamiChild, bsetCur, bsetErr);
    }
}


/***************************************************************************************************
    Report an error if the end of the block is reachable and the method returns a value.
    Sets the EXF_NEEDSRET flag as appropriate. Checks that all out parameters are assigned for
    methods with void return type.
***************************************************************************************************/
void FlowChecker::ReportReturnNeeded(EXPRBLOCK * block, BASENODE * tree, BitSet & bsetCur)
{
    if (!block || !block->FReachableEnd())
        return;

    if (m_pami) {
        if (m_pami->typeRet && m_pami->typeRet->isPredefType(PT_VOID)) {
            CheckOutParams(tree, bsetCur);
            block->flags |= EXF_NEEDSRET;
        }
        else if (m_pami->pDelegateType)
            compiler()->Error(tree, ERR_AnonymousReturnExpected, m_pami->pDelegateType);
        return;
    }

    if (m_info->meth->retType && m_info->meth->retType->isPredefType(PT_VOID) || m_info->IsIterator()) {
        CheckOutParams(tree, bsetCur);
        block->flags |= EXF_NEEDSRET;
    }
    else
        compiler()->Error(tree, ERR_ReturnExpected, m_info->meth);
}


/***************************************************************************************************
    Make sure all out parameters (including "this" for a struct ctor) are definitely assigned.
***************************************************************************************************/
void FlowChecker::CheckOutParams(BASENODE * tree, BitSet & bsetCur)
{
    int ibitMin;

    if (m_locThis && (ibitMin = m_locThis->slot.JbitDefAssg() - 1) >= 0 &&
        !bsetCur.TestAllRange(ibitMin, ibitMin + GetCbit(m_locThis->type)))
    {
        TYPESYM * typeThis = m_locThis->type;
        if (typeThis->isNUBSYM())
            typeThis = typeThis->asNUBSYM()->GetAts();

        ASSERT(typeThis->asAGGTYPESYM()->IsInstType());

        // Determine which fields of this were unassigned, and report an error for each.
        for (SYM * sym = typeThis->getAggregate()->firstChild; sym; sym = sym->nextChild) {
            if (!sym->isMEMBVARSYM() || sym->asMEMBVARSYM()->isStatic || sym->asMEMBVARSYM()->fixedAgg)
                continue;
            int ibitMinChd = ibitMin + GetIbit(sym->asMEMBVARSYM(), typeThis->asAGGTYPESYM());
            int ibitLimChd = ibitMinChd + GetCbit(compiler()->getBSymmgr().SubstType(sym->asMEMBVARSYM()->type, typeThis->asAGGTYPESYM()));
            if (!bsetCur.TestAllRange(ibitMinChd, ibitLimChd))
                compiler()->Error(tree, ERR_UnassignedThis, sym);
        }
    }

    FOREACHSYMLIST(m_listOutParams, loc, LOCVARSYM)
        if ((ibitMin = loc->slot.JbitDefAssg() - 1) >= 0 &&
            !bsetCur.TestAllRange(ibitMin, ibitMin + GetCbit(loc->type)))
        {
            compiler()->Error(tree, ERR_ParamUnassigned, loc);
        }
    ENDFOREACHSYMLIST;
}


/***************************************************************************************************
    Scan a list of statements. In general, the ending bsetCur is garbage, so don't use it!
    NOTE:  This may scan stuff outside the statement list if there are jumps out of the containing
    block (or other structure).
***************************************************************************************************/
void FlowChecker::ScanStmts(EXPRSTMT * stmt, BitSet & bsetCur, BitSet & bsetErr)
{
    if (!stmt) {
        VSFAIL("Why is ScanStmts being called with NULL?");
        bsetCur.Trash();
        return;
    }

    EXPRLABEL * labTop = NULL;
    ScanStmtsInner(stmt, &labTop, bsetCur, bsetErr);

    while (labTop) {
        EXPRLABEL * labCur = EXPRLABEL::PopFromStack(&labTop);

        // No switch labels should come through here.
        ASSERT(labCur->kind == EK_LABEL);
        bsetCur.Set(labCur->bsetEnter, m_heap);
        if (!labCur->stmtNext)
            ScanEndOfChain(labCur, bsetCur);
        else
            ScanStmtsInner(labCur->stmtNext, &labTop, bsetCur, bsetErr);
    }
    bsetCur.Trash();
}


/***************************************************************************************************
    Scan a list of statements. In general, the ending bsetCur is garbage, so don't use it!
    NOTE:  This may scan stuff outside the statement list if there are jumps out of the containing
    block (or other structure).
***************************************************************************************************/
void FlowChecker::ScanStmtsInner(EXPRSTMT * stmt, EXPRLABEL ** plabTop, BitSet & bsetCur, BitSet & bsetErr)
{
    ASSERT(stmt && plabTop);

    for (;;) {
        ASSERT(stmt && stmt->FReachable());

        switch (stmt->kind) {
        default:
        case EK_HANDLER:
            VSFAIL("Bad stmt expr kind");
            return;

        case EK_DELIM:
            ASSERT(stmt->FReachableEnd());
            if (!m_tsFinallyScanCur) {
                if (!stmt->asDELIM()->bset.FInited())
                    stmt->asDELIM()->bset.Set(bsetCur, m_heap);
                else
                    stmt->asDELIM()->bset.Intersect(bsetCur);
            }
            break;
        case EK_NOOP:
        case EK_DEBUGNOOP:
            ASSERT(stmt->FReachableEnd());
            break;
        case EK_DECL:
            ASSERT(stmt->FReachableEnd());
            ScanExpr(stmt->asDECL()->init, bsetCur, bsetErr);
            break;
        case EK_STMTAS:
            ASSERT(stmt->FReachableEnd());
            ScanExpr(stmt->asSTMTAS()->expression, bsetCur, bsetErr);
            break;
        case EK_THROW:
            ASSERT(!stmt->FReachableEnd());
            // NOTE: The resulting bitset is garbage!
            ScanExpr(stmt->asTHROW()->object, bsetCur, bsetErr);
            return;
        case EK_BLOCK:
            // The resulting bitset is the continuation bitset (on true return).
            if (!ScanBlock(stmt->asBLOCK(), bsetCur, bsetErr))
                return;
            break;
        case EK_GOTOIF:
            // The resulting bitset is the continuation bitset (on true return).
            if (!ScanGotoIf(stmt->asGOTOIF(), plabTop, bsetCur, bsetErr))
                return;
            break;
        case EK_GOTO:
            ASSERT(!stmt->FReachableEnd());
            if (!ScanGoto(stmt->asGOTO(), bsetCur, bsetErr))
                return;
            stmt = stmt->asGOTO()->label;
            ASSERT(stmt->FReachable());
            continue;
        case EK_LABEL:
            // The resulting bitset is the continuation bitset (on true return).
            if (!ScanLabel(stmt->asLABEL(), bsetCur))
                return;
            break;
        case EK_RETURN:
            // The resulting bitset is correct.
            ScanExpr(stmt->asRETURN()->object, bsetCur, bsetErr);
            if (!(stmt->flags & EXF_ASFINALLYLEAVE) || ScanThroughFinallys(stmt, bsetCur, bsetErr))
                CheckOutParams(stmt->tree, bsetCur);
            break;
        case EK_TRY:
            ScanTry(stmt->asTRY(), bsetCur, bsetErr);
            break;
        case EK_SWITCH:
            // The resulting bitset just reflects the switch expression. The end of the switch is
            // reachable iff it doesn't have a default, so this is the correct continuation set.
            ScanSwitch(stmt->asSWITCH(), bsetCur, bsetErr);
            break;
        case EK_SWITCHLABEL:
            // Can get here via a goto.
            while (!stmt->asSWITCHLABEL()->statements) {
                stmt = stmt->stmtNext;
                if (!stmt || stmt->kind != EK_SWITCHLABEL) {
                    VSFAIL("Shouldn't get here!");
                    return;
                }
                ASSERT(stmt->FReachable());
            }

            // The resulting bitset is the continuation bitset (on true return).
            if (!ScanLabel(stmt->asSWITCHLABEL(), bsetCur))
                return;
            stmt = stmt->asSWITCHLABEL()->statements;
            ASSERT(stmt);
            continue;
        }

        if (!stmt->FReachableEnd())
            return;
        if (!stmt->stmtNext)
            break;
        stmt = stmt->stmtNext;
    }

    ScanEndOfChain(stmt, bsetCur);
}


/***************************************************************************************************
    After a statement with no "next" (but reachable end) is scanned this is called to update the
    parent's information. If the parent is a block, the exit bitset is set to bsetCur.
***************************************************************************************************/
void FlowChecker::ScanEndOfChain(EXPRSTMT * stmt, BitSet & bsetCur)
{
    // We come here when the end of stmt is reachable and stmt is the end of a chain.
    // The bitset needs to be propogated to the parent.
    ASSERT(stmt && !stmt->stmtNext && stmt->FReachableEnd());

    EXPRSTMT * stmtPar = stmt->stmtPar;
    if (!stmtPar)
        return;

    switch (stmtPar->kind) {
    default:
    case EK_SWITCH:
    case EK_HANDLER:
    case EK_TRY:
        VSFAIL("Shouldn't get here!");
        break;
    case EK_BLOCK:
        // We should be processing the parent somewhere up the stack.
        ASSERT(stmtPar->flags & EXF_MARKING);
        stmtPar->asBLOCK()->bsetExit.Set(bsetCur, m_heap);
        break;
    case EK_SWITCHLABEL:
        // Should have already reported an error.
        break;
    }
}


/***************************************************************************************************
    Scan a block. On exit, bsetCur is the exit bitset of the block. If the end of the block is not
    reachable, this is all 1's. Returns true iff the end is reachable.
    NOTE:  This may scan stuff outside the block if there are jumps out of the block.
***************************************************************************************************/
bool FlowChecker::ScanBlock(EXPRBLOCK * block, BitSet & bsetCur, BitSet & bsetErr)
{
    //If there is no block, let the end be reachable.  This can happen when
    //working with a malformed parse tree while refactoring.
    if (!block)
        return true;

    ASSERT(block->FReachable());
    if (!block->statements) {
        ASSERT(block->FReachableEnd());
        return true;
    }

    ASSERT(!(block->flags & EXF_MARKING));
    block->flags |= EXF_MARKING;
    block->bsetExit.SetBitRange(0, m_cvarUninit, m_heap);
    ScanStmts(block->statements, bsetCur, bsetErr);
    // Set the current bitset to whatever was stored on the block,
    // NOT what came back from the recursive call (which is garbage).
    bsetCur.Set(block->bsetExit, m_heap);
    ASSERT(block->flags & EXF_MARKING);
    block->flags &= ~EXF_MARKING;

    return block->FReachableEnd();
}


/***************************************************************************************************
    Scan a try-finally or try-catch. On exit, bsetCur is the exit bitset if the end of the try-blah
    is reachable. If the end is not reachable, bsetCur is garbage.
    NOTE:  This may scan stuff outside the try-blah if there are jumps out of the try.
    Nevertheless, the return bitset is accurate (if the end of the try-blah is reachable).
***************************************************************************************************/
void FlowChecker::ScanTry(EXPRTRY * tr, BitSet & bsetCur, BitSet & bsetErr)
{
    BitSet bsetSave(bsetCur, m_heap);

    if (tr->flags & EXF_ISFINALLY) {
        if (!ScanBlock(tr->tryblock, bsetCur, bsetErr))
            bsetCur.ClearAll();
        if (ScanBlock(tr->handlers->asBLOCK(), bsetSave, bsetErr))
            bsetCur.Union(bsetSave, m_heap);
    }
    else {
        BitSet bsetTmp;

        // ScanBlock sets all bits if the end of the block is not reachable.
        ScanBlock(tr->tryblock, bsetCur, bsetErr);

        ASSERT((tr->tryblock && tr->tryblock->FReachableEnd()) || bsetCur.TestAllRange(0, m_cvarUninit));

        // All handlers are reachable.
        STMTLOOP(tr->handlers, stmtChd)
            bsetTmp.Set(bsetSave, m_heap);
            if (ScanBlock(stmtChd->asHANDLER()->handlerBlock, bsetTmp, bsetErr))
                bsetCur.Intersect(bsetTmp);
        ENDLOOP;
    }
}


/***************************************************************************************************
    Scan a switch statement. On return bsetCur just reflects the switch expression, no contained
    statements.
    NOTE:  This typically scans stuff outside the switch since case sections often end with a jump
    out of the switch.
***************************************************************************************************/
void FlowChecker::ScanSwitch(EXPRSWITCH * sw, BitSet & bsetCur, BitSet & bsetErr)
{
    // For a constant, only a matching switch label is reachable. We put a goto before the switch
    // to the correct label. Scanning from the goto should have already hit the target and closure.
    ASSERT(sw->arg->kind != EK_CONSTANT);

    ScanExpr(sw->arg, bsetCur, bsetErr);
    BitSet bsetTmp;

    // All switch labels are reachable.
    STMTLOOP(sw->bodies, stmtChd)
        EXPRSWITCHLABEL * lab = stmtChd->asSWITCHLABEL();
        if (lab->statements) {
            bsetTmp.Set(bsetCur, m_heap);
            if (ScanLabel(lab, bsetTmp))
                ScanStmts(lab->statements, bsetTmp, bsetErr);
        }
    ENDLOOP;
}


/***************************************************************************************************
    Scan a goto. Returns true iff the target of the goto should be scanned.
    NOTE: On false return bsetCur is garbage, so don't use it!
***************************************************************************************************/
bool FlowChecker::ScanGoto(EXPRGOTO * gt, BitSet & bsetCur, BitSet & bsetErr)
{
    ASSERT(gt->FReachable() && !gt->FReachableEnd());
    ASSERT(!(gt->flags & EXF_UNREALIZEDGOTO) || (gt->flags & EXF_BADGOTO));
    if ((gt->flags & EXF_BADGOTO) ||
        (gt->flags & EXF_ASFINALLYLEAVE) && !ScanThroughFinallys(gt, bsetCur, bsetErr))
    {
        bsetCur.Trash();
        return false;
    }
    ASSERT(gt->flags & EXF_GOTONOTBLOCKED);
    ASSERT(gt->label);
    return true;
}


/***************************************************************************************************
    Scan a goto-if and target of the goto. Returns false if the fall-through is not possible.
    When this returns true, bsetCur is set to the continuation set.
    NOTE: If this returns false, bsetCur is garbage, so don't use it!
***************************************************************************************************/
bool FlowChecker::ScanGotoIf(EXPRGOTOIF * gtif, EXPRLABEL ** plabTop, BitSet & bsetCur, BitSet & bsetErr)
{
    BitSet bsetTrue(bsetCur, m_heap);
    BitSet bsetFalse(bsetCur, m_heap);

    ScanExpr(gtif->condition, bsetCur, bsetErr, &bsetTrue, &bsetFalse, gtif->sense);
    if (!gtif->FNeverJumps()) {
        // NOTE: GotoIf never jumps out of try blocks.
        ASSERT(gtif->label && gtif->label->kind == EK_LABEL);
        if (ScanLabel(gtif->label, bsetTrue))
            gtif->label->PushOnStack(plabTop);
        if (gtif->FAlwaysJumps()) {
            ASSERT(!gtif->FReachableEnd());
            bsetCur.Trash();
            return false;
        }
    }
    bsetCur.Set(bsetFalse, m_heap);
    return true;
}


/***************************************************************************************************
    This handles both EK_LABEL and EK_SWITCHLABEL. Scan a label but NOT the subsequent statements.
    If the subsequent statement list should be scanned, this returns true (and bsetCur is valid on
    return). If the subsequent statement list should not be scanned, this returns false and bsetCur
    is garbarge on return. If the label is already in a label stack, this returns false (after
    updating the label's bsetEnter).
***************************************************************************************************/
bool FlowChecker::ScanLabel(EXPRLABEL * lab, BitSet & bsetCur)
{
    ASSERT(lab->FReachable());

    if (!lab->bsetEnter.FInited() || lab->tsFinallyScan != m_tsFinallyScanCur)
        lab->bsetEnter.Set(bsetCur, m_heap);
    else if (lab->bsetEnter.FIntersectChanged(bsetCur))
        bsetCur.Set(lab->bsetEnter, m_heap);
    else {
        bsetCur.Trash();
        return false;
    }

    lab->tsFinallyScan = m_tsFinallyScanCur;
    return !lab->InStack();
}


/***************************************************************************************************
    Scans the finallys executed by the given GOTO or RETURN. Returns false iff a finally blocks the
    jump (because the end of the finally is unreachable). On exit, bsetCur is the resulting bitset,
    which may be larger than the input bitset since the finallys may assign some additional locals.
***************************************************************************************************/
bool FlowChecker::ScanThroughFinallys(EXPRSTMT * stmt, BitSet & bsetCur, BitSet & bsetErr)
{
    ASSERT(stmt->kind == EK_GOTO ||
        stmt->kind == EK_RETURN && !(stmt->flags & EXF_RETURNISYIELD));
    ASSERT(stmt->FReachable());

    int tsFinallyScanSave = m_tsFinallyScanCur;
    m_tsFinallyScanCur = ++m_tsFinallyScanPrev;
    SCOPESYM * scopeSrc;
    SCOPESYM * scopeDst;

    if (stmt->kind == EK_GOTO) {
        scopeSrc = stmt->asGOTO()->currentScope;
        scopeDst = stmt->asGOTO()->targetScope;
    }
    else {
        scopeSrc = stmt->asRETURN()->currentScope;
        scopeDst = NULL;
    }

    for (SCOPESYM * scope = scopeSrc; scope != scopeDst; scope = scope->parent->asSCOPESYM()) {
        if (!scope->GetFinallyScope())
            continue;

        EXPRBLOCK * block = scope->GetFinallyScope()->GetBlock();
        ASSERT(block->FReachable());
        if (!ScanBlock(block, bsetCur, bsetErr)) {
            ASSERT(stmt->flags & EXF_FINALLYBLOCKED);
            m_tsFinallyScanCur = tsFinallyScanSave;
            return false;
        }
    }

    ASSERT(!(stmt->flags & EXF_FINALLYBLOCKED));
    m_tsFinallyScanCur = tsFinallyScanSave;
    return true;
}


/***************************************************************************************************
    Scan a conditional expression that needs to process a true-set and false-set. The resulting
    bitset is the intersection of the resulting true-set and false-set.
***************************************************************************************************/
void FlowChecker::ScanExprCond(EXPR * expr, BitSet & bsetCur, BitSet & bsetErr)
{
    BitSet bsetTrue(bsetCur, m_heap);
    BitSet bsetFalse(bsetCur, m_heap);

    ScanExpr(expr, bsetCur, bsetErr, &bsetTrue, &bsetFalse, true);

    // Intersect the true and false sets.
    bsetCur.Set(bsetTrue, m_heap);
    bsetCur.Intersect(bsetFalse);
}


/***************************************************************************************************
    Scan an expression with a true-set and false-set.
***************************************************************************************************/
void FlowChecker::ScanExpr(EXPR * expr, BitSet & bsetCur, BitSet & bsetErr,
    BitSet * pbsetTrue, BitSet * pbsetFalse, bool fSense)
{
    ASSERT(pbsetTrue && pbsetFalse);

LRepeat:
    if (!expr)
        return;

    if (m_pccb)
        m_pccb->ProcessExpression(expr, bsetCur);

    if (!fSense) {
        Swap(pbsetTrue, pbsetFalse);
        fSense = true; 
    }

    switch (expr->kind) {
    default:
        ScanExpr(expr, bsetCur, bsetErr);
        break;

    case EK_LOGNOT:
        expr = expr->asBIN()->p1;
        fSense = !fSense;
        goto LRepeat;

    case EK_CONSTANT:
        if (!expr->asCONSTANT()->getVal().iVal)
            Swap(pbsetTrue, pbsetFalse);
        pbsetTrue->Set(bsetCur, m_heap);
        pbsetFalse->SetBitRange(0, m_cvarUninit, m_heap);
        return;
    case EK_LOGOR:
        ASSERT(fSense);
        fSense = false;
        Swap(pbsetTrue, pbsetFalse);
        // Fall through.
    case EK_LOGAND: 
        ScanExpr(expr->asBIN()->p1, bsetCur, bsetErr, pbsetTrue, pbsetFalse, fSense);
        {
            BitSet bsetFalse2(*pbsetTrue, m_heap);
            bsetCur.Set(*pbsetTrue, m_heap);
            ScanExpr(expr->asBIN()->p2, bsetCur, bsetErr, pbsetTrue, &bsetFalse2, fSense);
            pbsetFalse->Intersect(bsetFalse2);
        }
        return;

    case EK_QMARK:
        {
            EXPR * op1 = expr->asBIN()->p1;
            EXPR * op2 = expr->asBIN()->p2;
            EXPR * op3 = op2->asBIN()->p2;
            op2 = op2->asBIN()->p1;

            // We have op1 ? op2 : op3
            ScanExpr(op1, bsetCur, bsetErr, pbsetTrue, pbsetFalse, true);
            bsetCur.Set(*pbsetTrue, m_heap);
            BitSet bsetFalse2(bsetCur, m_heap);
            ScanExpr(op2, bsetCur, bsetErr, pbsetTrue, &bsetFalse2, true);
            bsetCur.Set(*pbsetFalse, m_heap);
            BitSet bsetTrue2(bsetCur, m_heap);
            ScanExpr(op3, bsetCur, bsetErr, &bsetTrue2, pbsetFalse, true);
            pbsetTrue->Intersect(bsetTrue2);
            pbsetFalse->Intersect(bsetFalse2);
        }
        return;
    }

    pbsetTrue->Set(bsetCur, m_heap);
    pbsetFalse->Set(bsetCur, m_heap);
}


/***************************************************************************************************
    Scan an expression without a true-set and false-set.
***************************************************************************************************/
void FlowChecker::ScanExpr(EXPR * expr, BitSet & bsetCur, BitSet & bsetErr)
{
LRepeat:
    if (!expr)
        return;

    // Callback for refactoring.
    if (m_pccb)
        m_pccb->ProcessExpression(expr, bsetCur);

    switch (expr->kind) {
    case EK_LDTMP:
    case EK_FREETMP:
    case EK_NOOP:
    case EK_DELIM:
    case EK_MULTIGET:
    case EK_WRAP:
    case EK_FUNCPTR:
    case EK_TYPEOF:
    case EK_SIZEOF:
    case EK_CONSTANT:
    case EK_ERROR:
        break;

    // These are simple ones that use tail recursion.
    case EK_LOGNOT:
        expr = expr->asBIN()->p1;
        goto LRepeat;
    case EK_ARRINIT:
        expr = expr->asARRINIT()->args;
        goto LRepeat;
    case EK_CAST:
        expr = expr->asCAST()->p1;
        goto LRepeat;
    case EK_CONCAT:
        if (expr->flags & EXF_UNREALIZEDCONCAT) {
            compiler()->funcBRec.RealizeStringConcat(expr->asCONCAT());
        }
        expr = expr->asCONCAT()->list;
        goto LRepeat;
    case EK_MULTI:
        ScanExpr(expr->asMULTI()->left, bsetCur, bsetErr);
        expr = expr->asMULTI()->op;
        goto LRepeat;
    case EK_STTMP:
        expr = expr->asSTTMP()->src;
        goto LRepeat;
    case EK_PROP:
        ScanExpr(expr->asPROP()->object, bsetCur, bsetErr);
        expr = expr->asPROP()->args;
        goto LRepeat;
    case EK_USERLOGOP:
        ScanExpr(expr->asUSERLOGOP()->opX, bsetCur, bsetErr);
        expr = expr->asUSERLOGOP()->callOp->asCALL()->args->asBIN()->p2;
        goto LRepeat;
    case EK_DBLQMARK:
        ScanExpr(expr->asDBLQMARK()->exprTest, bsetCur, bsetErr);
        expr = expr->asDBLQMARK()->exprElse;
        goto LRepeat;

    case EK_ZEROINIT:
        expr = expr->asZEROINIT()->p1;
        if (expr)
            ScanAssign(expr, NULL, bsetCur, bsetErr, false);
        break;

    case EK_LOGOR:
    case EK_LOGAND: 
    case EK_QMARK:
        // These need to propogate true-set and false-set information.
        ScanExprCond(expr, bsetCur, bsetErr);
        break;

    case EK_ANONMETH:
        {
            AnonMethInfo * pami = expr->asANONMETH()->pInfo;
            // Save the current state
            if (!pami->bsetEnter.FInited())
                pami->bsetEnter.Set(bsetCur, m_heap);
            else
                pami->bsetEnter.Intersect(bsetCur);
            pami->fSeen = true;
        }
        break;

    case EK_LOCAL:
        ScanLocal(expr->asLOCAL(), bsetCur, bsetErr);
        break;

    case EK_CALL:
        // This assigns to the object being called on.
        if (expr->flags & (EXF_NEWSTRUCTASSG | EXF_IMPLICITSTRUCTASSG))
            ScanAssign(expr->asCALL()->object, expr->asCALL()->args, bsetCur, bsetErr, false);
        else {
            ScanExpr(expr->asCALL()->object, bsetCur, bsetErr);
            ScanExpr(expr->asCALL()->args, bsetCur, bsetErr);
        }
        if (expr->flags & EXF_HASREFPARAM) {
            // Look over the args again and assign to any out params...
            EXPRLOOP(expr->asCALL()->args, arg) 
                if (arg->type->isPARAMMODSYM()) {
                    MarkAsAlias(arg);
                    ScanAssign(arg, NULL, bsetCur, bsetErr, false);
                }
            ENDLOOP;
        }
        break;

    case EK_FIELD:
        if (ScanField(expr->asFIELD(), bsetCur, bsetErr)) {
            expr = expr->asFIELD()->object;
            goto LRepeat;
        }
        break;

    case EK_ADDR:
        ASSERT(expr->asBIN()->p2 == NULL);
        MarkAsAlias(expr->asBIN()->p1);
        ScanAssign(expr->asBIN()->p1, expr->asBIN()->p2, bsetCur, bsetErr, true);
        break;

    case EK_ASSG:
        ScanAssign(expr->asBIN()->p1, expr->asBIN()->p2, bsetCur, bsetErr, false);
        break;

    default:
        if (!(expr->flags & EXF_BINOP)) {
            ASSERT(compiler()->ErrorCount() > 0);
            break;
        }
        ScanExpr(expr->asBIN()->p1, bsetCur, bsetErr);
        expr = expr->asBIN()->p2;
        goto LRepeat;
    }
}


/***************************************************************************************************
    Process a local usage (not assignment). Mark the local as used (and whether it needs to be
    hoisted) and check that it is definitely assigned.
***************************************************************************************************/
void FlowChecker::ScanLocal(EXPRLOCAL * exprLoc, BitSet & bsetCur, BitSet & bsetErr)
{
    int ibitMin;
    int ibitLim;

    MarkAsUsed(exprLoc, true);

    LOCVARSYM * local = exprLoc->local;
    ASSERT(local && !local->isConst);

    // Out params get assigned at the conclusion of the call expr.
    if ((ibitMin = local->slot.JbitDefAssg() - 1) >= 0 &&
        (!exprLoc->type->isPARAMMODSYM() || exprLoc->type->asPARAMMODSYM()->isRef) &&
        !bsetCur.TestAllRange(ibitMin, ibitLim = ibitMin + GetCbit(local->type)) &&
        !bsetErr.TestAllRange(ibitMin, ibitLim))
    {
        bsetErr.SetBitRange(ibitMin, ibitLim, m_heap);
        if (local->isThis)
            compiler()->Error(exprLoc->tree, ERR_UseDefViolationThis);
        else if (local->slot.isParam && local->slot.isRefParam)
            compiler()->Error(exprLoc->tree, ERR_UseDefViolationOut, local);
        else
            compiler()->Error(exprLoc->tree, ERR_UseDefViolation, local);
    }
}


/***************************************************************************************************
    Scan a field on use (not assignment). Mark any base local as used (and whether it should be
    hoisted). Check for definite assignment. Return true iff the object should be scanned.
***************************************************************************************************/
bool FlowChecker::ScanField(EXPRFIELD * exprFld, BitSet & bsetCur, BitSet & bsetErr)
{
    int ibitMin;
    int ibitLim;

    bool fMarked = MarkAsUsed(exprFld->object, true);

    if ((ibitMin = exprFld->offset - 1) >= 0) {
        if (!(exprFld->flags & EXF_MEMBERSET) &&
            !bsetCur.TestAllRange(ibitMin, ibitLim = ibitMin + GetCbit(
                compiler()->getBSymmgr().SubstType(exprFld->fwt.Field()->type, exprFld->fwt.Type()))) &&
            !bsetErr.TestAllRange(ibitMin, ibitLim))
        {
            bsetErr.SetBitRange(ibitMin, ibitLim, m_heap);
            compiler()->Error(exprFld->tree, ERR_UseDefViolationField, exprFld->fwt.Field()->name);
        }
        return false;
    }

    return !exprFld->fwt.Field()->fixedAgg || !fMarked;
}


/***************************************************************************************************
    Rips through any EK_FIELDs and EK_INDIRs looking for a base EK_LOCAL. Tracks usage of the local
    in anon meths. If fValUsed is true, marks the local as used.
    Returns true iff it finds a base local.
***************************************************************************************************/
bool FlowChecker::MarkAsUsed(EXPR * expr, bool fValUsed)
{
    if (!fValUsed && !m_pami)
        return false;

LRepeat:
    if (!expr)
        return false;
    switch (expr->kind) {
    default:
        return false;
    case EK_INDIR:
        ASSERT(!expr->asBIN()->p2);
        expr = expr->asBIN()->p1;
        goto LRepeat;
    case EK_FIELD:
        expr = expr->asFIELD()->object;
        goto LRepeat;
    case EK_LOCAL:
        break;
    }

    LOCVARSYM * local = expr->asLOCAL()->local;
    ASSERT(local && !local->isConst);

    if (fValUsed)
        local->slot.SetUsed(true);

    // If we're inside an anonymous method block, we need to mark any
    // locals that get used so they can be moved from the local scope
    // to a heap allocated object for the delegate to access
    SCOPESYM * scope;
    if (m_pami && local->fUsedInAnonMeth &&
        (scope = local->parent->asSCOPESYM())->nestingOrder < m_pami->pArgs->nestingOrder)
    {
        // The local is outside this anon meth so need to hoist it.
        // We checked for legality during bindToLocal.
        local->fHoistForAnonMeth = true;

        // Every outer anonymous delegate between where it's used and where
        // it's declared must be made non-static, and moved to the $locals class
        for (AnonMethInfo * pami = m_pami; pami && pami->pArgs->nestingOrder > scope->nestingOrder; pami = pami->pamiOuter) {
            if (local->isThis)
                pami->fUsesThis = true;
            else
                pami->fUsesLocals = true;
            if (pami->pScope->nestingOrder < scope->nestingOrder)
                pami->pScope = scope;
        }
    }

    return true;
}


/***************************************************************************************************
    Sets the appropriate definite assignment bits, if any.
***************************************************************************************************/
void FlowChecker::ScanAssign(EXPR * expr, EXPR * val, BitSet & bsetCur, BitSet & bsetErr, bool fValUsed)
{
    int jbit;
    TYPESYM * type;

    switch (expr->kind) {
    case EK_LOCAL:
        MarkAsUsed(expr, fValUsed);
        ScanExpr(val, bsetCur, bsetErr);
        jbit = expr->asLOCAL()->local->slot.JbitDefAssg();
        if (!jbit)
            return;
        type = expr->asLOCAL()->local->type;
        break;

    case EK_FIELD:
        jbit = expr->asFIELD()->offset;
        if (jbit) {
            // Shouldn't scan the expr, just the value.
            MarkAsUsed(expr, fValUsed);
            ScanExpr(val, bsetCur, bsetErr);
            type = compiler()->getBSymmgr().SubstType(expr->asFIELD()->fwt.Field()->type, expr->asFIELD()->fwt.Type());
            break;
        }
        // Fall through.
    default:
        ScanExpr(expr, bsetCur, bsetErr);
        ScanExpr(val, bsetCur, bsetErr);
        return;
    }

    int ibit = jbit - 1;
    bsetCur.SetBitRange(ibit, ibit + GetCbit(type), m_heap);
}


/***************************************************************************************************
    Mark a local as aliased.
***************************************************************************************************/
void FlowChecker::MarkAsAlias(EXPR * expr)
{
    if (expr && expr->kind == EK_LOCAL)
        expr->asLOCAL()->local->slot.aliasPossible = true;
}


/***************************************************************************************************
    Returns the number of bits used to represent the type when doing definite assignment analysis.
    For non-structs this is always 1. For structs it is the sum of the cbits of the instance field
    types. Empty structs have cbit 0. Fixed size buffers have cbit 0.
***************************************************************************************************/
int FlowChecker::GetCbit(COMPILER * comp, TYPESYM * type)
{
    AGGTYPESYM * ats;

    switch (type->getKind()) {
    default:
        return 1;
    case SK_AGGTYPESYM:
        ats = type->asAGGTYPESYM();
        break;
    case SK_NUBSYM:
        ats = type->asNUBSYM()->GetAts();
        if (!ats)
            return 1;
        break;
    }

    if (ats->FCbitDefAssgSet()) {
        ASSERT(ats->AggState() >= AggState::DefinedMembers);
        return ats->GetCbitDefAssg();
    }

    // Make sure we have the fields!
    comp->EnsureState(ats);
    ASSERT(ats->AggState() >= AggState::DefinedMembers);

    if (!ats->isStructType() ||
        ats->isPredefined() && BSYMMGR::GetPredefFundType(ats->getPredefType()) != FT_STRUCT ||
        ats->getAggregate()->fLayoutError)
    {
        ats->SetCbitDefAssg(1);
        return 1;
    }

    int cbit = 0;
    bool fSubst = !ats->IsInstType();

    if (fSubst) {
        // Make sure the instance type has been done, so we can compare ibit values.
        AGGTYPESYM * atsInst = ats->GetInstType();
        if (!atsInst->FCbitDefAssgSet())
            GetCbit(comp, atsInst);
    }

    for (SYM * sym = ats->getAggregate()->firstChild; sym; sym = sym->nextChild) {
        if (!sym->isMEMBVARSYM())
            continue;
        MEMBVARSYM * fld = sym->asMEMBVARSYM();
        if (fld->isStatic || fld->fixedAgg)
            continue;

        TYPESYM * typeFld = fld->type;
        if (!fSubst)
            fld->SetIbitInst(cbit);
        else {
            typeFld = comp->getBSymmgr().SubstType(typeFld, ats);
            if (cbit != fld->GetIbitInst())
                fld->fIbitVaries = true;
        }

        cbit += GetCbit(comp, typeFld);
    }

    if (!cbit && ats->isPredefined())
        cbit = 1;
    ats->SetCbitDefAssg(cbit);

    return cbit;
}


/***************************************************************************************************
    Get the bit position for the given field within the given type. The type MUST be an AGGTYPESYM
    for the AGGSYM containing the fld.
***************************************************************************************************/
int FlowChecker::GetIbit(COMPILER * comp, MEMBVARSYM * fld, AGGTYPESYM * ats)
{
    ASSERT(ats->getAggregate() == fld->getClass());
    ASSERT(ats->isStructType());
    ASSERT(!fld->isStatic && !fld->fixedAgg);

    // This should already be set, but just in case we assert it here
    // and call FlowChecker::GetCbit instead of AGGTYPESYM::GetCbitDefAssg.
    ASSERT(ats->FCbitDefAssgSet());

    if (GetCbit(comp, ats) == 1) {
        // We can get here for a variety of reasons.
        ASSERT(
            ats->isPredefined() && BSYMMGR::GetPredefFundType(ats->getPredefType()) != FT_STRUCT ||
            ats->getAggregate()->fLayoutError ||
            fld->GetIbitInst() == 0 ||
            (fld->GetIbitInst() == 1 && GetCbit(comp, comp->getBSymmgr().SubstType(fld->type, ats)) == 0));
        return 0;
    }

    if (ats->IsInstType() || !fld->fIbitVaries)
        return fld->GetIbitInst();


    int ibit = 0;

    for (SYM * sym = ats->getAggregate()->firstChild; sym; sym = sym->nextChild) {
        if (sym == fld) {
            ASSERT(ibit + GetCbit(comp, comp->getBSymmgr().SubstType(fld->type, ats)) <= ats->GetCbitDefAssg());
            return ibit;
        }

        if (!sym->isMEMBVARSYM())
            continue;
        MEMBVARSYM * fld = sym->asMEMBVARSYM();
        if (fld->isStatic || fld->fixedAgg)
            continue;

        TYPESYM * typeFld = comp->getBSymmgr().SubstType(fld->type, ats);
        ibit += GetCbit(comp, typeFld);
        ASSERT(ibit <= ats->GetCbitDefAssg());
    }

    ASSERT(ibit == ats->GetCbitDefAssg());
    VSFAIL("Why didn't we find the field?");
    return 0;
}


/***************************************************************************************************
    This:
    1)  Clears all reachability flags and sets parent information for the statements
        in the given block.
    2)  Sets reachability information by scanning.
    3)  Optionally reports warnings on unreachable code.
***************************************************************************************************/
void ReachabilityChecker::SetReachability(EXPRBLOCK * block, bool fReportWarnings)
{
    ASSERT(!block->stmtPar);
    ASSERT(!block->stmtNext);

    m_fHasRetAsLeave = false;
    block->ClearReachable();
    SetParents(block, block->statements);
    MarkReachable(block);

    if (fReportWarnings) {
        bool fReachable = true;
        ReportUnreachable(block, &fReachable);
    }
    // DumpStmts(block, 0);
}


/***************************************************************************************************
    Sets the parents information for the statement forest and clears all the reachability states.
***************************************************************************************************/
void ReachabilityChecker::SetParents(EXPRSTMT * stmtPar, EXPRSTMT * stmtFirst)
{
    for (EXPRSTMT * stmt = stmtFirst; stmt; stmt = stmt->stmtNext) {
        stmt->ClearReachable();
        stmt->stmtPar = stmtPar;
        switch (stmt->kind) {
        case EK_BLOCK:
            SetParents(stmt, stmt->asBLOCK()->statements);
            break;
        case EK_SWITCH:
            SetParents(stmt, stmt->asSWITCH()->bodies);
            break;
        case EK_SWITCHLABEL:
            SetParents(stmt, stmt->asSWITCHLABEL()->statements);
            break;
        case EK_HANDLER:
            SetParents(stmt, stmt->asHANDLER()->handlerBlock);
            break;
        case EK_TRY:
            SetParents(stmt, stmt->asTRY()->tryblock);
            SetParents(stmt, stmt->asTRY()->handlers);
            break;
        default:
            break;
        }
    }
}


/***************************************************************************************************
    Mark the given statement as subsequent ones as reachable. When a statement previously marked
    reachable is encountered, this returns.
***************************************************************************************************/
void ReachabilityChecker::MarkReachable(EXPRSTMT * stmt)
{
    if (!stmt)
        return;

    EXPRLABEL * labTop = NULL;
    MarkReachableInner(stmt, &labTop);
    while (labTop) {
        EXPRLABEL * labCur = EXPRLABEL::PopFromStack(&labTop);

        // No switch labels should come through here.
        ASSERT(labCur->kind == EK_LABEL);
        if (!labCur->FReachable())
            MarkReachableInner(labCur, &labTop);
    }
}


/***************************************************************************************************
    Mark the given statement as subsequent ones as reachable. When a statement previously marked
    reachable is encountered, this returns.
***************************************************************************************************/
void ReachabilityChecker::MarkReachableInner(EXPRSTMT * stmt, EXPRLABEL ** plabTop)
{
    ASSERT(stmt && plabTop);

    for (;;) {
        if (stmt->FReachable())
            return;

        ASSERT(!stmt->FReachableEnd());

        switch (stmt->kind) {
        default:
        case EK_HANDLER:
            VSFAIL("Bad stmt expr kind");
            return;
        case EK_DELIM:
        case EK_NOOP:
        case EK_DEBUGNOOP:
        case EK_DECL:
        case EK_STMTAS:
        case EK_LABEL:
            stmt->SetReachableEnd();
            break;
        case EK_THROW:
            stmt->SetReachable();
            return;

        case EK_RETURN:
            // Yield return has a reachable end. Other forms don't.
            if ((stmt->flags & EXF_RETURNISYIELD) && stmt->asRETURN()->object) {
                stmt->SetReachableEnd();
                // Mark the SCOPESYMs.
                for (SCOPESYM * scope = stmt->asRETURN()->currentScope; 
                    scope && !(scope->scopeFlags & SF_HASYIELDRETURN);
                    scope = scope->parent->asSCOPESYM())
                {
                    scope->scopeFlags |= SF_HASYIELDRETURN;
                }
            }
            else
                stmt->SetReachable();

            if (stmt->flags & EXF_ASLEAVE) {
                m_fHasRetAsLeave = true;

                // Mark whether the return is blocked by a finally.
                if (stmt->flags & EXF_ASFINALLYLEAVE) {
                    ASSERT(!(stmt->flags & EXF_FINALLYBLOCKED));
                    for (EXPRSTMT * stmtPar = stmt->stmtPar; stmtPar; stmtPar = stmtPar->stmtPar) {
                        if (stmtPar->kind == EK_TRY && (stmtPar->flags & EXF_ISFINALLY) && stmtPar->asTRY()->IsFinallyBlocked()) {
                            stmt->flags |= EXF_FINALLYBLOCKED;
                            break;
                        }
                    }
                }
            }
            break;

        case EK_BLOCK:
            if (!MarkBlock(stmt->asBLOCK()))
                return;
            break;

        case EK_GOTO:
            stmt->SetReachable();
            // Resolve and continue from the target.
            if (RealizeGoto(stmt->asGOTO(), true)) {
                ASSERT(stmt->flags & EXF_GOTONOTBLOCKED);
                stmt = stmt->asGOTO()->label;
                continue;
            }
            ASSERT(stmt->flags & (EXF_BADGOTO | EXF_FINALLYBLOCKED));
            break;

        case EK_GOTOIF:
            if (stmt->asGOTOIF()->FNeverJumps()) {
                // Just like a STMTAS.
                stmt->SetReachableEnd();
                RealizeGoto(stmt->asGOTOIF(), true);
            }
            else if (stmt->asGOTOIF()->FAlwaysJumps()) {
                // Just like a GOTO.
                stmt->SetReachable();
                // Resolve and continue from the target.
                if (RealizeGoto(stmt->asGOTOIF(), true)) {
                    stmt = stmt->asGOTOIF()->label;
                    continue;
                }
                ASSERT(stmt->flags & (EXF_BADGOTO | EXF_FINALLYBLOCKED));
            }
            else {
                stmt->SetReachableEnd();
                // Resolve and add the target to the label list.
                if (RealizeGoto(stmt->asGOTOIF(), true)) {
                    // Don't mark the label's statement list now, but instead add it to the
                    // label list. This avoids nasty unbounded recursion.
                    EXPRLABEL * labCur = stmt->asGOTOIF()->label;
                    if (!labCur->FReachable() && !labCur->InStack())
                        labCur->PushOnStack(plabTop);
                }
            }
            break;

        case EK_SWITCH:
            // Only a matching switch label is reachable. We put a goto before the switch
            // to the label. Scanning from the goto should have already hit the target and closure.
            ASSERT(stmt->asSWITCH()->arg->kind != EK_CONSTANT);

            if (!(stmt->flags & EXF_HASDEFAULT))
                stmt->SetReachableEnd();
            else
                stmt->SetReachable();

            // All switch labels are reachable.
            ASSERT(!(stmt->flags & EXF_MARKING));
            stmt->flags |= EXF_MARKING;
            STMTLOOP(stmt->asSWITCH()->bodies, stmtChd)
                // These don't affect the reachable state of the switch since
                // they all jump out of the switch to the break label.
                MarkReachable(stmtChd);
            ENDLOOP;
            ASSERT(stmt->flags & EXF_MARKING);
            stmt->flags &= ~EXF_MARKING;
            break;

        case EK_SWITCHLABEL:
            stmt->SetReachable();
            if (stmt->asSWITCHLABEL()->statements) {
                ASSERT(!(stmt->flags & EXF_MARKING));
                stmt->flags |= EXF_MARKING;
                MarkReachable(stmt->asSWITCHLABEL()->statements);
                ASSERT(!stmt->FReachableEnd());
                ASSERT(stmt->flags & EXF_MARKING);
                stmt->flags &= ~EXF_MARKING;
            }
            else if (!stmt->stmtNext) {
                VSFAIL("We should have put a NOOP in here!");
                compiler()->Error(stmt->tree, ERR_SwitchFallThrough, stmt->asSWITCHLABEL()->label);
                return;
            }
            else {
                // Just part of a switch label group. "Fall through" is legal.
                stmt->SetReachableEnd();
            }
            break;

        case EK_TRY:
            if (stmt->flags & EXF_ISFINALLY) {
                stmt->SetReachableEnd();
                // Must do the finally block first, since its reachability affects
                // return statements and gotos inside the try block.
                if (!MarkBlock(stmt->asTRY()->handlers->asBLOCK()))
                    stmt->ClearReachableEnd();
                if (!MarkBlock(stmt->asTRY()->tryblock))
                    stmt->ClearReachableEnd();
            }
            else {
                stmt->SetReachable();
                if (MarkBlock(stmt->asTRY()->tryblock))
                    stmt->SetReachableEnd();

                // All handlers are reachable.
                STMTLOOP(stmt->asTRY()->handlers, stmtChd)
                    if (MarkBlock(stmtChd->asHANDLER()->handlerBlock)) {
                        stmtChd->SetReachableEnd();
                        stmt->SetReachableEnd();
                    }
                    else
                        stmtChd->SetReachable();
                ENDLOOP;
            }
            break;
        }

        if (!stmt->FReachableEnd())
            return;

        if (!stmt->stmtNext)
            break;
        stmt = stmt->stmtNext;
    }

    // We come here when the end of stmt is reachable and stmt is the end of a chain.
    // The reachability needs to be propogated to the parent.
    ASSERT(stmt && !stmt->stmtNext && stmt->FReachableEnd());

    EXPRSTMT * stmtPar = stmt->stmtPar;
    if (!stmtPar)
        return;

    // We should be processing the parent somewhere up the stack.
    ASSERT(stmtPar->flags & EXF_MARKING);

    switch (stmtPar->kind) {
    default:
    case EK_SWITCH:
    case EK_HANDLER:
    case EK_TRY:
        VSFAIL("Shouldn't get here!");
        break;
    case EK_BLOCK:
        stmtPar->SetReachableEnd();
        break;
    case EK_SWITCHLABEL:
        compiler()->Error(stmtPar->tree, ERR_SwitchFallThrough, stmtPar->asSWITCHLABEL()->label);
        break;
    }
}


/***************************************************************************************************
    Mark the block and contents as reachable. The end reachability is marked by the last child.
***************************************************************************************************/
bool ReachabilityChecker::MarkBlock(EXPRBLOCK * block)
{
    //If there is no block, let its end be reachable. This can happen when
    //working with a malformed parse tree while refactoring.
    if (!block)
        return true;

    // If there are no statements, the end is reachable. Otherwise, the
    // recursive call to MarkReachable will set the reachability of the
    // end of the block.
    if (!block->statements) {
        block->SetReachableEnd();
        return true;
    }

    block->SetReachable();

    ASSERT(!(block->flags & EXF_MARKING));
    block->flags |= EXF_MARKING;
    MarkReachable(block->statements);
    ASSERT(block->flags & EXF_MARKING);
    block->flags &= ~EXF_MARKING;

    return block->FReachableEnd();
}


/***************************************************************************************************
    Make sure the goto is realized. Also checks for jumping out of an anonymous method or finally
    block. If fFull is true, also makes sure the goto is marked with either EXF_FINALLYBLOCKED or
    EXF_GOTONOTBLOCKED. If fFull is false, this is being called just to report errors so checking
    whether a finally blocks the goto should not be done.
***************************************************************************************************/
bool ReachabilityChecker::RealizeGoto(EXPRGOTO * gt, bool fFull)
{
    if (gt->flags & (EXF_BADGOTO | EXF_FINALLYBLOCKED)) {
        ASSERT(!(gt->flags & EXF_UNREALIZEDGOTO) || (gt->flags & EXF_BADGOTO));
        return false;
    }
    if (gt->flags & EXF_GOTONOTBLOCKED) {
        ASSERT(!(gt->flags & EXF_UNREALIZEDGOTO));
        return true;
    }

    if (gt->flags & EXF_UNREALIZEDGOTO) {
        LABELSYM * target = NULL;

        if (gt->flags & EXF_GOTOCASE) {
            ASSERT(gt->targetScope);
            target = compiler()->getLSymmgr().LookupLocalSym(gt->labelName, gt->targetScope, MASK_LABELSYM)->asLABELSYM();
        }
        else {
            for (SCOPESYM * scope = gt->currentScope; scope; scope = scope->parent->asSCOPESYM()) {
                target = compiler()->getLSymmgr().LookupLocalSym(gt->labelName, scope, MASK_LABELSYM)->asLABELSYM();
                if (target) {
                    gt->targetScope = scope;
                    break;
                }
            }
        }

        if (!target) {
            compiler()->Error(gt->tree, ERR_LabelNotFound, gt->labelName);
            gt->flags |= EXF_UNREALIZEDGOTO | EXF_BADGOTO;
            return false;
        }
        gt->label = target->labelExpr;
        gt->flags &= ~EXF_UNREALIZEDGOTO;
        gt->label->flags |= EXF_LABELREFERENCED;
    }
    ASSERT(!(gt->flags & (EXF_BADGOTO | EXF_UNREALIZEDGOTO)));

    ASSERT(!gt->targetScope == !gt->currentScope);
    if (!gt->targetScope || !gt->currentScope) {
        // Not a user entered goto. Comes from an if, for, while, etc.
        gt->flags |= EXF_GOTONOTBLOCKED;
        return true;
    }

    // Check for jumping out of anon methods and finally blocks.
    for (SCOPESYM * scope = gt->currentScope; scope != gt->targetScope; scope = scope->parent->asSCOPESYM()) {
        ASSERT(scope);
        if (scope->scopeFlags & (SF_FINALLYSCOPE | SF_DELEGATESCOPE)) {
            compiler()->Error(gt->tree,
                (scope->scopeFlags & SF_FINALLYSCOPE) ? ERR_BadFinallyLeave : ERR_BadDelegateLeave);
            gt->flags |= EXF_BADGOTO;
            return false;
        }
        if (scope->scopeFlags & (SF_TRYSCOPE | SF_CATCHSCOPE)) {
            gt->flags |= EXF_ASLEAVE;
            if (scope->GetFinallyScope())
                gt->flags |= EXF_ASFINALLYLEAVE;
        }
    }
    ASSERT(!(gt->flags & (EXF_BADGOTO | EXF_UNREALIZEDGOTO)));

    // Determine whether there is a blocking finally.
    if (!(gt->flags & EXF_ASFINALLYLEAVE)) {
        gt->flags |= EXF_GOTONOTBLOCKED;
        return true;
    }

    if (!fFull)
        return false;

    for (EXPRSTMT * stmtPar = gt->stmtPar; stmtPar != gt->label->stmtPar; stmtPar = stmtPar->stmtPar) {
        if (!stmtPar) {
            VSFAIL("Why didn't we find the labels parent?");
            break;
        }
        if (stmtPar->kind == EK_TRY && (stmtPar->flags & EXF_ISFINALLY) && stmtPar->asTRY()->IsFinallyBlocked()) {
            gt->flags |= EXF_FINALLYBLOCKED;
            return false;
        }
    }

    gt->flags |= EXF_GOTONOTBLOCKED;
    return true;
}


/***************************************************************************************************
    Report warnings on unreachable code.
***************************************************************************************************/
void ReachabilityChecker::ReportUnreachable(EXPRSTMT * stmt, bool * pfReachable)
{
    for ( ; stmt; stmt = stmt->stmtNext) {
        // See if the state is changing.
        if (!*pfReachable != !stmt->FReachable() && stmt->tree && !(stmt->flags & EXF_GENERATEDSTMT)) {
            switch (stmt->kind) {
            default:
                VSFAIL("Bad stmt expr kind");
            case EK_BLOCK:
            case EK_DELIM:
            case EK_NOOP:
            case EK_DEBUGNOOP:
            case EK_LABEL:
            case EK_SWITCH:
            case EK_SWITCHLABEL:
                // These don't really matter.
                break;
            case EK_STMTAS:
            case EK_DECL:
            case EK_RETURN:
            case EK_THROW:
            case EK_GOTO:
            case EK_TRY:
            case EK_HANDLER:
                // If we went from reachable to unreachable, warn.
                if (*pfReachable)
                    compiler()->Error(stmt->tree, WRN_UnreachableCode);
                *pfReachable = !*pfReachable;
                break;
            case EK_GOTOIF:
                if (*pfReachable) {
                    BASENODE * errNode = stmt->tree;
                    if (errNode && errNode->kind == NK_DO && errNode->asDO()->pExpr) {
                        errNode = errNode->asDO()->pExpr;
                    }
                    compiler()->Error(errNode, WRN_UnreachableCode);
                }
                *pfReachable = !*pfReachable;
                break;
            }
        }

        switch (stmt->kind) {
        case EK_BLOCK:
            ReportUnreachable(stmt->asBLOCK()->statements, pfReachable);
            break;
        case EK_SWITCH:
            ReportUnreachable(stmt->asSWITCH()->bodies, pfReachable);
            break;
        case EK_SWITCHLABEL:
            ReportUnreachable(stmt->asSWITCHLABEL()->statements, pfReachable);
            break;
        case EK_HANDLER:
            ReportUnreachable(stmt->asHANDLER()->handlerBlock, pfReachable);
            break;
        case EK_TRY:
            ReportUnreachable(stmt->asTRY()->tryblock, pfReachable);
            ReportUnreachable(stmt->asTRY()->handlers, pfReachable);
            break;
        default:
            break;
        }
    }
}


/***************************************************************************************************
    Dump the statement hierarchy and reachability info to stdout.
***************************************************************************************************/
void ReachabilityChecker::DumpStmts(EXPRSTMT * stmtFirst, int ctab)
{
    for (EXPRSTMT * stmt = stmtFirst; stmt; stmt = stmt->stmtNext) {
        for (int itab = 0; itab < ctab; itab++)
            wprintf(L"  ");
        switch (stmt->kind) {
        default:
            VSFAIL("Bad stmt expr kind");
            return;
        case EK_STMTAS:
            wprintf(L"STMTAS");
            break;
        case EK_DELIM:
            wprintf(L"DELIM");
            break;
        case EK_NOOP:
            wprintf(L"NOOP");
            break;
        case EK_DEBUGNOOP:
            wprintf(L"DEBUGNOOP");
            break;
        case EK_DECL:
            wprintf(L"DECL");
            break;
        case EK_LABEL:
            wprintf(L"LABEL");
            break;
        case EK_RETURN:
            wprintf(L"RETURN");
            break;
        case EK_BLOCK:
            wprintf(L"BLOCK");
            break;
        case EK_THROW:
            wprintf(L"THROW");
            break;
        case EK_GOTO:
            wprintf(L"GOTO");
            break;
        case EK_GOTOIF:
            wprintf(L"GOTOIF");
            break;
        case EK_SWITCH:
            wprintf(L"SWITCH");
            break;
        case EK_SWITCHLABEL:
            wprintf(L"SWITCHLABEL(%s)", stmt->asSWITCHLABEL()->label->name->text);
            break;
        case EK_TRY:
            wprintf(L"TRY");
            break;
        case EK_HANDLER:
            wprintf(L"HANDLER");
            break;
        }
        switch (stmt->flags & (EXF_UNREACHABLEBEGIN | EXF_UNREACHABLEEND)) {
        case 0:
            wprintf(L" Begin|End");
            break;
        case EXF_UNREACHABLEBEGIN:
            wprintf(L" ** End **");
            break;
        case EXF_UNREACHABLEEND:
            wprintf(L" Begin");
            break;
        case EXF_UNREACHABLEBEGIN | EXF_UNREACHABLEEND:
            break;
        }
        wprintf(L"\n");

        switch (stmt->kind) {
        case EK_BLOCK:
            DumpStmts(stmt->asBLOCK()->statements, ctab + 1);
            break;
        case EK_SWITCH:
            DumpStmts(stmt->asSWITCH()->bodies, ctab + 1);
            break;
        case EK_SWITCHLABEL:
            DumpStmts(stmt->asSWITCHLABEL()->statements, ctab + 1);
            break;
        case EK_HANDLER:
            DumpStmts(stmt->asHANDLER()->handlerBlock, ctab + 1);
            break;
        case EK_TRY:
            DumpStmts(stmt->asTRY()->tryblock, ctab + 1);
            DumpStmts(stmt->asTRY()->handlers, ctab + 1);
            break;
        default:
            break;
        }
    }
}
