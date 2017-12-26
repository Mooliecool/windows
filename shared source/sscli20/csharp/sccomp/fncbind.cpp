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
// File: fncbind.cpp
//
// Routines for binding the body of a function
// ===========================================================================

#include "stdafx.h"
#include "prefixassert.h"

/*
 * Define the simple type conversions table.
 */

#define CONV_KIND_ID   (byte)ConvKind::Identity // Identity conversion
#define CONV_KIND_IMP  (byte)ConvKind::Implicit // Implicit conversion
#define CONV_KIND_EXP  (byte)ConvKind::Explicit // Explicit conversion
#define CONV_KIND_NONE (byte)ConvKind::None     // No conversion
#define CONV_KIND_MASK 0x0F     // Mask for above.
#define CONV_KIND_USEUDC 0x40   // Use a "user defined" operator function to make the conversion.

#define ID  CONV_KIND_ID      // shorthands for defining the table
#define IMP CONV_KIND_IMP
#define EXP CONV_KIND_EXP
#define NO  CONV_KIND_NONE
#define UDC CONV_KIND_USEUDC
const byte FUNCBREC::simpleTypeConversions[NUM_SIMPLE_TYPES][NUM_SIMPLE_TYPES] = {
//        to: BYTE    I2      I4      I8      FLT     DBL     DEC     CHAR    BOOL    SBYTE     U2      U4      U8
/* from */
/* BYTE */ {   ID    ,IMP    ,IMP    ,IMP    ,IMP    ,IMP    ,IMP|UDC,EXP    ,NO ,       EXP    ,IMP    ,IMP    ,IMP    },
/*   I2 */ {  EXP    ,ID     ,IMP    ,IMP    ,IMP    ,IMP    ,IMP|UDC,EXP    ,NO ,       EXP    ,EXP    ,EXP    ,EXP    },
/*   I4 */ {  EXP    ,EXP    ,ID     ,IMP    ,IMP    ,IMP    ,IMP|UDC,EXP    ,NO ,       EXP    ,EXP    ,EXP    ,EXP    },
/*   I8 */ {  EXP    ,EXP    ,EXP    ,ID     ,IMP    ,IMP    ,IMP|UDC,EXP    ,NO ,       EXP    ,EXP    ,EXP    ,EXP    },
/*  FLT */ {  EXP    ,EXP    ,EXP    ,EXP    ,ID     ,IMP    ,EXP|UDC,EXP    ,NO ,       EXP    ,EXP    ,EXP    ,EXP    },
/*  DBL */ {  EXP    ,EXP    ,EXP    ,EXP    ,EXP    ,ID     ,EXP|UDC,EXP    ,NO ,       EXP    ,EXP    ,EXP    ,EXP    },
/*  DEC */ {  EXP|UDC,EXP|UDC,EXP|UDC,EXP|UDC,EXP|UDC,EXP|UDC,ID     ,EXP|UDC,NO ,       EXP|UDC,EXP|UDC,EXP|UDC,EXP|UDC},
/* CHAR */ {  EXP    ,EXP    ,IMP    ,IMP    ,IMP    ,IMP    ,IMP|UDC,ID     ,NO ,       EXP    ,IMP    ,IMP    ,IMP    },
/* BOOL */ {  NO     ,NO     ,NO     ,NO     ,NO     ,NO     ,NO     ,NO     ,ID ,       NO     ,NO     ,NO     ,NO     },
/*SBYTE */ {  EXP    ,IMP    ,IMP    ,IMP    ,IMP    ,IMP    ,IMP|UDC,EXP    ,NO ,       ID     ,EXP    ,EXP    ,EXP    },
/*   U2 */ {  EXP    ,EXP    ,IMP    ,IMP    ,IMP    ,IMP    ,IMP|UDC,EXP    ,NO ,       EXP    ,ID     ,IMP    ,IMP    },
/*   U4 */ {  EXP    ,EXP    ,EXP    ,IMP    ,IMP    ,IMP    ,IMP|UDC,EXP    ,NO ,       EXP    ,EXP    ,ID     ,IMP    },
/*   U8 */ {  EXP    ,EXP    ,EXP    ,EXP    ,IMP    ,IMP    ,IMP|UDC,EXP    ,NO ,       EXP    ,EXP    ,EXP    ,ID     },
};
#undef ID
#undef IMP
#undef EXP
#undef UDC

static const WCHAR szOriginalPrefix[] = L"<>3__";
static const int cchOriginalPrefix = lengthof(szOriginalPrefix) - 1;

static PREDEFTYPE rgptIntOp[] = { PT_INT, PT_UINT, PT_LONG, PT_ULONG };

// return the given predefined type (including void)
#if _MSC_VER
__forceinline
#endif
AGGTYPESYM * FUNCBREC::GetReqPDT(PREDEFTYPE pt)
{
    ASSERT(pt != PT_VOID);  // use getVoidType()
    return compiler()->GetReqPredefType(pt, true);
}

AGGTYPESYM * FUNCBREC::GetOptPDT(PREDEFTYPE pt)
{
    ASSERT(pt != PT_VOID);  // use getVoidType()
    return compiler()->GetOptPredefTypeErr(pt, true);
}

__forceinline AGGSYM * FUNCBREC::getPDO()
{
    return compiler()->GetReqPredefAgg(PT_OBJECT);
}

__forceinline AGGTYPESYM * FUNCBREC::getPDOT()
{
    return compiler()->GetReqPredefType(PT_OBJECT);
}

// Constructor, this gets called only at compiler init time...
FUNCBREC::FUNCBREC()
{
    pOuterScope = NULL;
    pCurrentScope = NULL;
    parentAgg = NULL;
    parentDecl = NULL;
    thisPointer = NULL;
    inFieldInitializer = false;
    allocator = &(compiler()->localSymAlloc);
    pFSym = NULL;
    pFOrigSym = NULL;
    pMSym = NULL;
    info = NULL;
    m_pamiFirst = NULL;
    m_pamiCur = NULL;
    uniqueNameIndex = 0;
    outerThisPointer = NULL;
    taClsVarsForMethVars = NULL;
    unsafeState = UNSAFESTATES_Unknown;
    exprSwitchCur = NULL;
}

// Report a deprecation error on a symbol.
void FUNCBREC::ReportDeprecated(BASENODE * tree, SymWithType swt)
{
    ASSERT(swt.Sym()->IsDeprecated());

    if (!pMSym || !pMSym->IsDeprecated()) {
        compiler()->clsDeclRec.ReportDeprecated(tree, parentAgg, swt);
    }
}


// compile a method.  pAMNode can point to a METHODNODE, a CTORNODE, or a
// CLASSNODE if compiling a synthetized constructor...
EXPR * FUNCBREC::compileMethod(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo, ICompileCallback *pBindCallback)
{
    ASSERT(info->meth);

#if DEBUG
    {
        if (info->meth->name && COMPILER::IsRegString(info->meth->name->text, L"Method")) {
            if (COMPILER::IsRegString(info->meth->getClass()->name->text, L"Class")) {
                ASSERT(!"Compilation breakpoint hit");
            }
        }
    }
#endif

    // Setup special member for refactoring callback
    bindCallback.Init(pBindCallback);

    insideTryOfCatch = false;

    InitMethod(info, info->meth->parseTree, classInfo);

    setUnsafe(info->meth->isUnsafe);

    checked.normal = compiler()->GetCheckedMode();
    checked.constant = true;

    errorsBeforeBind = compiler()->ErrorCount();

    EXPR * rval;

    switch (info->meth->MethKind()) {
    case MethodKind::Ctor:
        rval = bindConstructor(info);
        break;
    case MethodKind::Dtor:
        rval = bindDestructor(info);
        break;
    case MethodKind::PropAccessor:
        rval = BindPropertyAccessor(info);
        break;
    case MethodKind::EventAccessor:
        rval = bindEventAccessor(info);
        break;
    default:
        rval = info->meth->isIfaceImpl ? bindIfaceImpl(info) : bindMethod(info);
        break;
    }

    {
        CErrorSuppression es;

        if ((bindCallback.ForcePostBind() || !compiler()->FAbortEarly(errorsBeforeBind, &es)) && rval && rval->isOK()) {
            SETLOCATIONSTAGE(SCAN);
            bindCallback.StartPostBinding(rval->asBLOCK());
            PostBindCompile(rval->asBLOCK());
            // PostBindCompile marked all the anon methods that are actually used
            // (in reachable code), so filter out all the others.
            if (m_pamiFirst) {
                // Filter out the unused ones.
                FixUpAnonMethInfoLists(&m_pamiFirst);
            }
            bindCallback.EndPostBinding(rval->asBLOCK());
        }
    }

    info->pamiFirst = m_pamiFirst;

    this->info = NULL;
    this->m_pamiFirst = NULL;
    this->m_pamiCur = NULL;

    resetUnsafe();

    return rval;
}

// rewrite the Iterator method as a valid MoveNext method
// This should never report any errors.
// It will create fields for all locals (and emit all parameters for aggregate)
EXPR * FUNCBREC::rewriteMoveNext(METHSYM * pOuterMeth, EXPR * pIterExpr, METHINFO * info)
{
    ASSERT(info->piin);

    SETLOCATIONSTAGE(TRANSFORM);

    // Fix the METHINFO to reflect the real MoveNext method
    this->info = info;

    IteratorRewriteInfo irwInfo;
    NAME * pName = NULL;
    AGGSYM * saveParentAgg = parentAgg;
    DECLSYM * saveParentDecl = parentDecl;

    pMSym = info->meth;
    localCount = 0;

    // We need to blow away the locals from the current method and move them to the class
    // this also does parameters
    pCatchScope = pTryScope = pFinallyScope = pCurrentScope = pOuterScope = info->outerScope;
    parentAgg = irwInfo.aggInnerClass = info->meth->getClass();
    parentDecl = info->meth->containingDeclaration();
    taClsVarsForMethVars = parentAgg->getThisType()->typeArgsThis;
    int cIteratorLocals = MoveLocalsToIter(info->outerScope, info->piin->aggIter, 0); // Count up the iterator locals
    compiler()->emitter.BeginIterator(cIteratorLocals);

    //re-point these guys
    MEMBVARSYM * hoistedThis = compiler()->getBSymmgr().LookupAggMember(compiler()->namemgr->GetPredefName(PN_HOISTEDTHIS), parentAgg, MASK_MEMBVARSYM)->asMEMBVARSYM();
    LOCVARSYM * otherThisPointer = compiler()->getLSymmgr().LookupLocalSym(compiler()->namemgr->GetPredefName(PN_THIS), info->outerScope, MASK_LOCVARSYM)->asLOCVARSYM();
    while (otherThisPointer) {
        if (hoistedThis != NULL) {
            otherThisPointer->movedToField = hoistedThis;
        } else {
            otherThisPointer->movedToField = NULL;
            hoistedThis = MoveLocalToField(otherThisPointer, info->piin->aggIter, info->piin->aggIter->typeVarsThis, false);
        }
        otherThisPointer = otherThisPointer->nextSameName->asLOCVARSYM();
    }

    initThisPointer();
    irwInfo.exprLocalThis = bindThisImplicit(NULL);
    pName = compiler()->namemgr->GetPredefName(PN_ITERSTATE);
    irwInfo.exprState = MakeFieldAccess( irwInfo.exprLocalThis, pName);
    pName = compiler()->namemgr->GetPredefName(PN_ITERCURRENT);
    irwInfo.exprCurrent = MakeFieldAccess( irwInfo.exprLocalThis, pName);
    irwInfo.exprLabelFailRet = MakeFreshLabel();
    irwInfo.iNextState = IteratorStates::FirstUnusedState;
    irwInfo.iFinallyState = IteratorStates::RunningIEnumerator;

    // Now re-write the EXPR tree to get a valid MoveNext (done recursively)
    EXPR* body = pIterExpr;
    RecurseAndRewriteExprTree(&body, &irwInfo);
    ASSERT(body->asBLOCK()->statements != NULL);
    body->flags &= ~EXF_NEEDSRET;

    EXPRSWITCHLABEL * pSwLab = MakeSwitchLabel(MakeIntConst(IteratorStates::NotStartedIEnumerator), pCurrentScope);
    EXPRLABEL* pStartLab = MakeFreshLabel();
    pSwLab->statements = MakeGoto(NULL, pStartLab, EXF_NODEBUGINFO);
    pSwLab = MakeSwitchLabel(NULL, pCurrentScope);
    pSwLab->statements = MakeGoto(NULL, irwInfo.exprLabelFailRet->asLABEL(), EXF_NODEBUGINFO);

    // Now put together the switch block that will either jump after the last yield
    // or jump to the end
    EXPRSTMT * pPrologue = NULL;
    StmtListBldr bldr(&pPrologue);
    bldr.Add(MakeIteratorSwitch(pCurrentScope, &irwInfo, pStartLab, irwInfo.exprLabelFailRet->asLABEL()));
    bldr.Add(pStartLab);
    bldr.Add(MakeAssignment(irwInfo.exprState, MakeIntConst(IteratorStates::RunningIEnumerator)));
    bldr.Add(body->asBLOCK());
    bldr.Add(irwInfo.exprLabelFailRet);
    bldr.Add(MakeReturn(pCurrentScope, MakeBoolConst(false), info->piin->disposeBody ? EXF_ASLEAVE : 0));

    body = newExprBlock(NULL);
    body->asBLOCK()->statements = pPrologue;

    if (info->piin->disposeBody) {
        ASSERT(info->hasYieldAsLeave);

        // Make a try/fault block to call Dispose
        EXPRTRY * tryExpr = newExpr(NULL, EK_TRY, NULL)->asTRY();
        tryExpr->flags |= EXF_ISFINALLY | EXF_ISFAULT;
        tryExpr->tryblock = body->asBLOCK();

        EXPR * call = BindPredefMethToArgs( NULL, PN_DISPOSE, GetReqPDT(PT_IDISPOSABLE), bindThisImplicit(NULL), NULL);

        EXPRSTMT * stmt = MakeStmt(NULL, call, EXF_NODEBUGINFO);
        body = newExprBlock(NULL);
        body->asBLOCK()->statements = stmt;

        tryExpr->handlers = body->asBLOCK();
        body = newExprBlock(NULL);
        body->asBLOCK()->statements = tryExpr;

        info->hasRetAsLeave = true;
    }
    pMSym = pOuterMeth;

    parentAgg = saveParentAgg;
    parentDecl = saveParentDecl;

    this->info = NULL;
    this->taClsVarsForMethVars = NULL;

    return body;
}


// Using a LOCVARSYM as a template, construct a MEMBVARSYM in cls
// Will automatically resolve duplicates (since 2 locals in different scopes can have the same name)
MEMBVARSYM * FUNCBREC::MoveLocalToField( LOCVARSYM * local, AGGSYM * aggDest, TypeArray * pSubstTypeVars, bool bAllowDupNames, NAME * pMembName)
{
    MEMBVARSYM * rval = NULL;

    if (!pMembName) {
        // If this guy has already been moved, keep the same name
        if (local->movedToField) {
            ASSERT(local->type->isFabricated() || local->slot.isParam);
            pMembName = local->movedToField->name;
        } else if (local->isThis) {
            pMembName = compiler()->namemgr->GetPredefName(PN_HOISTEDTHIS);
        } else if (bAllowDupNames && !local->fIsCompilerGenerated) {
            // We don't want to re-mangle an already mangled name
            pMembName = CreateSpecialName( SpecialNameKind::HoistedIteratorLocal, local->name->text);
        } else {
            pMembName = local->name;
        }
    }

    // The name should not already exist in the class
    ASSERT(NULL == compiler()->getBSymmgr().LookupAggMember( pMembName, aggDest, MASK_ALL));
    // The class that we move this to should be completely compiler-generated
    ASSERT(aggDest->isFabricated);

    rval = compiler()->getBSymmgr().CreateMembVar(pMembName, aggDest, aggDest->DeclOnly());
    rval->SetAccess(ACC_PUBLIC);
    rval->isReferenced = rval->isAssigned = true;

    if (TypeArray::Size(pSubstTypeVars)) {
        rval->type = compiler()->getBSymmgr().SubstType(local->type, (TypeArray*)NULL, pSubstTypeVars);
    } else
        rval->type = local->type;

    if (!local->movedToField)
        local->movedToField = rval;

    ASSERT(!local->IsAssumedPinned());

    return rval;
}

// Recursively move locals from the local scope to a fabricated class.
// Only move used locals/parameters into the class. After moving, orphan the locals.
int FUNCBREC::MoveLocalsToIter( SCOPESYM * pScope, AGGSYM * aggIter, int cIteratorLocals)
{
    SYM ** ppNextChild = &pScope->firstChild;
    bool bParam = (pScope == pOuterScope);
    // If this scope doesn't have a yield return then we don't need to hoist
    // but we still need to substitute type parameters (don't forget that parameters are always hoisted)
    bool fHoist = !!(pScope->scopeFlags & SF_HASYIELDRETURN) || bParam;
    WCHAR szNameBuffer[MAX_IDENT_LEN + cchOriginalPrefix + 1];
    HRESULT hr;

    if (!fHoist && !TypeArray::Size(taClsVarsForMethVars)) {
        // No hoisting and nothing to substitute, so we're done
        return cIteratorLocals;
    }

    ASSERT(!bParam || cIteratorLocals == 0);

    if (bParam && info->piin->fEnumerable) {
        hr = StringCchCopyW( szNameBuffer, lengthof(szNameBuffer), szOriginalPrefix);
        ASSERT(SUCCEEDED(hr));
    }

    ASSERT(aggIter->isFabricated);
    ASSERT(info->piin);

    FOREACHCHILD( pScope, inner)
        *ppNextChild = inner;
        if (inner->isLOCVARSYM()) {
            LOCVARSYM * local = inner->asLOCVARSYM();
            if (!local->slot.IsUsed() || !bParam && local->fHoistForAnonMeth && !local->isCatch) {
                // It's not used or only used in an anonymous method, so NUKE it!
                goto ORPHAN_CHILD;
            }

            if (!fHoist) {
                ASSERT(TypeArray::Size(taClsVarsForMethVars) > 0);
                local->type = compiler()->getBSymmgr().SubstType( local->type, (TypeArray*)NULL, taClsVarsForMethVars);
            }
            else {
                // If you get an ASSERT here, that means somehow
                // We didn't report an error when the address was taken of a local
                // inside an iterator!!!!
                ASSERT(!local->IsAssumedPinned());
                ASSERT(bParam || local->movedToField == NULL ||
                    (local->type->isAGGTYPESYM() && local->type->asAGGTYPESYM()->getAggregate()->isFabricated));

                // Add an appropriately named member to our "$local" class
                MEMBVARSYM * memb = MoveLocalToField( local, aggIter, taClsVarsForMethVars, !bParam);
                local->movedToField = memb;

                if (bParam && info->piin->fEnumerable && (!local->isThis || memb->type->isStructOrEnum())) {
                    ASSERT(local->slot.isParam);
                    hr = StringCchCopyW( szNameBuffer + cchOriginalPrefix, lengthof(szNameBuffer) - cchOriginalPrefix, memb->name->text);
                    ASSERT(SUCCEEDED(hr));
                    NAME * nameCopy = compiler()->getNamemgr()->AddString( szNameBuffer);
                    MEMBVARSYM * membCopy = compiler()->getBSymmgr().CreateMembVar(nameCopy, aggIter, aggIter->DeclOnly());
                    membCopy->SetAccess(ACC_PUBLIC);
                    membCopy->isReferenced = membCopy->isAssigned = true;
                    membCopy->type = memb->type;
                    memb->SetOriginalCopy(membCopy);
                }

                local->fIsIteratorLocal = true;
                if (bParam) {
                    // Parameters are always live so don't need to keep the locals
                    goto ORPHAN_CHILD;
                }

                memb->iIteratorLocal = cIteratorLocals++;
            }
        }
        else if (inner->isSCOPESYM()) {
            // recurse (nested scopes never have parameters)
            cIteratorLocals = MoveLocalsToIter( inner->asSCOPESYM(), aggIter, cIteratorLocals);
        }
        ppNextChild = &inner->nextChild;
ORPHAN_CHILD:
        ;
    ENDFOREACHCHILD;

    // Terminate the list.
    *ppNextChild = NULL;

    return cIteratorLocals;
}


void FUNCBREC::FixUpAnonMethInfoLists(AnonMethInfo ** ppami)
{
    for (AnonMethInfo * pamiCur = *ppami; pamiCur; ) {
        AnonMethInfo * pamiNext = pamiCur->pamiNext;
        if (pamiCur->fSeen) {
            FixUpAnonMethInfoLists(&pamiCur->pamiChild);
            *ppami = pamiCur;
            ppami = &pamiCur->pamiNext;
        }
        else {
            // For safety, make sure orphaned ones don't point to valid ones.
            pamiCur->pamiOuter = NULL;
            pamiCur->pamiNext = NULL;
        }
        pamiCur = pamiNext;
    }
    *ppami = NULL;
}


// Rewrite method bodies that contain anonymous methods
// Mostly just moves locals from local scope to $local of type pDelClass
// and changes EK_LOCAL to EK_FIELD (recurses through the EK_ANONMETH exprs in rewriteAnonMeth)
// and substitutes types to go from the method type arguments to the class type arguments
EXPR * FUNCBREC::RewriteAnonDelegateBodies(METHSYM * meth, SCOPESYM * scpArg, AnonMethInfo * pamiFirst, EXPR * body)
{
    SETLOCATIONSTAGE(TRANSFORM);
    SETLOCATIONSYM(meth);

    SCOPESYM  * pOldCurScope = pCurrentScope;
    SCOPESYM  * pOldOutScope = pOuterScope;
    AGGSYM    * pOldParentAgg = parentAgg;
    DECLSYM * pOldParentDecl = parentDecl;
    AnonymousMethodRewriteInfo amrwInfo(meth, scpArg, pamiFirst);

    pCurrentScope = pOuterScope = scpArg;
    parentAgg = meth->getClass();
    parentDecl = meth->containingDeclaration();
    RecurseAndRewriteExprTree(&body, &amrwInfo);

    pCurrentScope = pOldCurScope;
    pOuterScope = pOldOutScope;
    parentAgg = pOldParentAgg;
    parentDecl = pOldParentDecl;

    return body;
}

// Construct $local and copy in all parameters
// Call this for each scope that has hoisted locals
// Will add all init expressions to the end of the expr list and pass back out the new end
void FUNCBREC::InitAnonDelegateLocals(SCOPESYM * pArgScope, StmtListBldr & bldr, TypeArray * amClsTypeVars)
{
    // If this scope doesn't have any hoisted locals (indicated by a NULL pasi)
    // or the exprLoc is merely an outer <this> pointer (for anonymous methods
    // hoisted into the user's class or an outer scope display class) there's nothing to do
    if (!pArgScope || !pArgScope->pasi || !pArgScope->pasi->exprLoc ||
        (pArgScope->pasi->exprLoc->flags & EXF_IMPLICITTHIS))
    {
        VSFAIL("Why was InitAnonDelegateLocals called?");
        return;
    }

    EXPRCALL * expr;
    EXPR * pObject = NULL;

    expr = createConstructorCall(NULL, NULL, pArgScope->pasi->exprLoc->type->asAGGTYPESYM(), NULL, NULL, MemLookFlags::NewObj)->asCALL();
    bldr.Add(MakeAssignment(pArgScope->pasi->exprLoc, expr));

    // Copy all outer $locals to 'flatten' the derefence time
    // but only if we have a anonymous method that will block
    // accessing the locals directly
    bool bHasMethods = false;
    for (AnonMethInfo * pami = pArgScope->pasi->pamiFirst; pami; pami = pami->pamiNext) {
        if (pami->pScope == pArgScope) {
            bHasMethods = true;
            break;
        }
    }
    if (bHasMethods) {
        EXPRLOCAL * pLocals = pArgScope->pasi->exprLoc;
        AGGSYM * aggLocal = pLocals->type->asAGGTYPESYM()->getAggregate();

        // But don't copy the argscope because it is a duplicate of an inner scope
        for (SCOPESYM *pCur = pArgScope->parent->asSCOPESYM(); pCur && pCur->parent; pCur = pCur->parent->asSCOPESYM()) {
            if (!pCur->pasi || (pCur->scopeFlags & SF_DELEGATESCOPE) || !pCur->pasi->aggHoist->isFabricated)
                continue;

            EXPRLOCAL * pCurLocals = pCur->pasi->exprLoc;
            EXPR * src = NULL;

            if (pCur->nestingOrder >= pOuterScope->nestingOrder) {
                src = pCurLocals;
            } else if (pObject == NULL) {
                // If thisPointer is not marked as a param, we're in an nested anon method that
                // is static. Hence there's no more display classes to hoist.
                if (!thisPointer || !thisPointer->slot.isParam)
                    break;
                src = bindThisImplicit(NULL);
                if (src->type->asAGGTYPESYM()->getAggregate()->isFabricated) {
                    if (src->type->getAggregate() != pCurLocals->type->getAggregate()) {
                        // skip any extra scopes that exist between my scope (pArgScope) and the one associated with my this pointer.
#if DEBUG
                       // verify that we will eventually find the correct hoisted scope
                        bool willFindScope = false;
                        for (SCOPESYM *sc = pCur; sc && sc->parent; sc = sc->parent->asSCOPESYM()) {
                            if (sc->pasi && sc->pasi->exprLoc && sc->pasi->exprLoc->type->getAggregate() == src->type->getAggregate()) {
                                willFindScope = true;
                                break;
                            }
                        }
                        ASSERT(willFindScope);
#endif
                       continue;
                    }
                    pObject = src;
                }
                else {
                    // The outer anonymous method was optimistically placed on the user's class
                    // so it has no outer display classes that need to be flattened
                    break;
                }
            } else {
                ASSERT(pObject->type->asAGGTYPESYM()->getAggregate()->isFabricated);
                if (!pCurLocals->local->movedToField) {
                    // We're in a nested anonymous method, if the outer display class hasn't already been
                    // hoisted that must mean it's not needed, so ignore it
                    continue;
                }
                src = MakeFieldAccess(pObject, pCurLocals->local->movedToField->name);
                if (!src) {
                    // We're in a nested anonymous method, if the outer display class hasn't already been
                    // hoisted into our parent that must mean it's not needed, so ignore it
                    continue;
                }
            }

            // 2 anonymous methods inside two levels of outer anonymous methods will cause the same field to get hoisted twice
            NAME * name = NULL;
            if (pCurLocals->local->movedToField)
                name = pCurLocals->local->movedToField->name;
            else if (pCurLocals->flags & EXF_IMPLICITTHIS) {
                // Get the name from the original outer local!
                for (SCOPESYM * scp = pCur->parent->asSCOPESYM(); scp; scp = scp->parent->asSCOPESYM()) {
                    if (scp->pasi && scp->pasi->aggHoist == pCur->pasi->aggHoist &&
                        !(scp->pasi->exprLoc->flags & EXF_IMPLICITTHIS))
                    {
                        name = scp->pasi->exprLoc->local->name;
                        break;
                    }
                }
                ASSERT(name);
            }
            else
                name = pCurLocals->local->name;

            if (!name || !compiler()->getBSymmgr().LookupAggMember(name, aggLocal, MASK_ALL)) {
                MoveLocalToField(pCurLocals->local, aggLocal, amClsTypeVars, false, name);
                name = pCurLocals->local->movedToField->name;

                EXPR * dest = MakeFieldAccess(pLocals, name);
                ASSERT(dest);

                bldr.Add(MakeAssignment(dest, src));
            }
        }
    }
}

EXPR * FUNCBREC::makeAnonCtor(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo)
{
    ASSERT(info->meth && info->meth->params->size == 0);

    InitMethod(info, pAMNode, classInfo);

    EXPRBLOCK * exprBlock = newExprBlock(NULL);
    exprBlock->flags |= EXF_NEEDSRET;
    StmtListBldr bldr(&exprBlock->statements);

    // can't call createBaseConstructorCall because it sucks in the arguments from pTree if pAMSym is a CTOR
    // It also does some unneeded checks, so we just put the important part here.
    bldr.Add(MakeStmt(pTree,
        createConstructorCall(pTree, NULL, parentAgg->baseClass, bindThisImplicit(pTree), NULL, MemLookFlags::BaseCall),
        EXF_NODEBUGINFO));

    return exprBlock;
}


EXPR * FUNCBREC::makeIterCtor(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo)
{
    ASSERT(info->meth);

    InitMethod(info, pAMNode, classInfo);
    DeclareMethodParameters(info);

    EXPRBLOCK * exprBlock = newExprBlock(NULL);
    exprBlock->flags |= EXF_NEEDSRET;
    StmtListBldr bldr(&exprBlock->statements);

    // can't call createBaseConstructorCall because it sucks in the arguments from pTree if pAMSym is a CTOR
    // It also does some unneeded checks, so we just put the important part here.
    bldr.Add(MakeStmt(pTree,
        createConstructorCall(pTree, NULL, parentAgg->baseClass, bindThisImplicit(pTree), NULL, MemLookFlags::BaseCall),
        EXF_NODEBUGINFO));

    // Now we need to pass in all the parameters
    FOREACHCHILD(pOuterScope, child)
        if (child->isLOCVARSYM() && child != thisPointer) {
            ASSERT(child->asLOCVARSYM()->slot.isParam);
            EXPR * exprLHS = MakeFieldAccess( bindThisImplicit(NULL), child->name, EXF_MEMBERSET | EXF_LVALUE);
            EXPR * exprRHS = bindToLocal( NULL, child->asLOCVARSYM(), BIND_RVALUEREQUIRED);
            bldr.Add(MakeAssignment(exprLHS, exprRHS));
        }
    ENDFOREACHCHILD;

    return exprBlock;
}

EXPR * FUNCBREC::makeIterGet(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo)
{
    ASSERT(info->piin);

    InitMethod(info, pAMNode, classInfo);

    ASSERT(!info->meth->getClass()->isFabricated);
    DeclareMethodParameters(info);

    EXPRBLOCK * exprBlock = newExprBlock(NULL);
    AGGTYPESYM * atsIter = compiler()->getBSymmgr().SubstType(info->piin->aggIter->getThisType(),
            compiler()->getBSymmgr().ConcatParams(info->meth->getClass()->typeVarsAll, info->meth->typeVars))->asAGGTYPESYM();

    exprBlock->statements = NULL;
    StmtListBldr bldr(&exprBlock->statements);

    createNewScope();
    pCurrentBlock = exprBlock;
    pCurrentBlock->scopeSymbol = pCurrentScope;
    LOCVARSYM *local = declareVar( NULL, CreateSpecialName(SpecialNameKind::IteratorInstance), atsIter);
    local->fIsCompilerGenerated = true;
    local->slot.hasInit = true;
    local->slot.isReferenced = true;
    local->slot.SetUsed(true);
    EXPR * exprLocal = bindToLocal( NULL, local, BIND_MEMBERSET);
    EXPR * exprCtor = createConstructorCall(pTree, NULL, atsIter, NULL,
        MakeIntConst( info->piin->fEnumerable ? IteratorStates::UnusedIEnumerable : IteratorStates::NotStartedIEnumerator), MemLookFlags::NewObj);
    bldr.Add(MakeAssignment(exprLocal, exprCtor));
    exprLocal = bindToLocal( NULL, local, BIND_RVALUEREQUIRED);

    // Now we need to pass in all the parameters
    FOREACHCHILD(pOuterScope, child)
        if (child->isLOCVARSYM()) {
            ASSERT(child->asLOCVARSYM()->slot.isParam);
            NAME * name = child->asLOCVARSYM()->isThis ? compiler()->namemgr->GetPredefName(PN_HOISTEDTHIS) : child->name;
            EXPR * exprLHS = MakeFieldAccess( exprLocal, name, EXF_MEMBERSET | EXF_LVALUE);

            if (exprLHS != NULL) {
                // Sometimes, they just aren't hoisted because they're not used
                if (exprLHS->asFIELD()->fwt.Field()->isHoistedParameter) {
                    ASSERT(info->piin->fEnumerable);
                    exprLHS->asFIELD()->fwt.sym = exprLHS->asFIELD()->fwt.Field()->GetOriginalCopy();
                }
                else {
                    // Don't need a copy of the this pointer because it can't change
                    ASSERT(child == thisPointer || !child->asLOCVARSYM()->slot.IsUsed());
                }

                EXPR * exprRHS = bindToLocal( NULL, child->asLOCVARSYM(), BIND_RVALUEREQUIRED);
                bldr.Add(MakeAssignment(exprLHS, exprRHS));
            }
        }
    ENDFOREACHCHILD;

    EXPR * exprCast = mustCastCore( exprLocal, info->meth->retType, NULL, 0);
    bldr.Add(MakeReturn(pOuterScope, exprCast));
    closeScope();

    return exprBlock;
}

EXPR * FUNCBREC::makeIterGetEnumerator(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo, METHSYM ** pmethGetEnumerator)
{
    ASSERT(info->piin);

    InitMethod(info, pAMNode, classInfo);

    ASSERT(info->cpin == 0 && info->meth->params->size == 0);

    EXPRBLOCK * exprBlock = newExprBlock(NULL);
    pCurrentBlock = exprBlock;

    StmtListBldr bldr(&exprBlock->statements);

    if (*pmethGetEnumerator == NULL) {
        // Save this for later
        (*pmethGetEnumerator) = info->meth;
        createNewScope();
        pCurrentBlock->scopeSymbol = pCurrentScope;
        AGGTYPESYM * atsIter = info->meth->getClass()->getThisType();
        LOCVARSYM *local = declareVar( NULL, CreateSpecialName(SpecialNameKind::IteratorInstance), atsIter);
        local->fIsCompilerGenerated = true;
        local->slot.hasInit = true;
        local->slot.isReferenced = true;
        local->slot.SetUsed(true);
        EXPR * exprLocal = bindToLocal( NULL, local, BIND_MEMBERSET);
        EXPR * exprArgs = NULL;
        EXPR ** pexprArgLast = &exprArgs;
        newList( MakeFieldAccess( bindThisImplicit(NULL), compiler()->namemgr->GetPredefName(PN_ITERSTATE), EXF_MEMBERSET | EXF_LVALUE), &pexprArgLast);
        exprArgs->setType(compiler()->getBSymmgr().GetParamModifier(exprArgs->type, false));
        newList( MakeIntConst(IteratorStates::NotStartedIEnumerator), &pexprArgLast);
        newList( MakeIntConst(IteratorStates::UnusedIEnumerable), &pexprArgLast);

        EXPR * exprCmpExch = BindPredefMethToArgs( NULL, PN_COMPAREEXCHANGE, GetOptPDT(PT_INTERLOCKED), NULL, exprArgs);
        EXPR * exprCompare = newExprBinop( NULL, EK_NE, GetReqPDT(PT_BOOL), exprCmpExch, MakeIntConst(IteratorStates::UnusedIEnumerable));
        EXPRLABEL * exprElseLabel = MakeFreshLabel();
        EXPRLABEL * exprEndIfLabel = MakeFreshLabel();
        bldr.Add(MakeGotoIf(NULL, exprCompare, exprElseLabel, true));

        bldr.Add(MakeAssignment(exprLocal, bindThisImplicit(NULL)));
        bldr.Add(MakeGoto(NULL, exprEndIfLabel, EXF_NODEBUGINFO));
        bldr.Add(exprElseLabel);

        EXPR * exprCtorCall = createConstructorCall(pTree, NULL, atsIter, NULL, MakeIntConst(IteratorStates::NotStartedIEnumerator), MemLookFlags::NewObj);
        bldr.Add(MakeAssignment(exprLocal, exprCtorCall));
        exprLocal = bindToLocal( NULL, local, BIND_RVALUEREQUIRED);
        MEMBVARSYM * hoistedThis = compiler()->getBSymmgr().LookupAggMember( compiler()->getNamemgr()->GetPredefName(PN_HOISTEDTHIS), atsIter->getAggregate(), MASK_MEMBVARSYM)->asMEMBVARSYM();
        if (hoistedThis != NULL) {
            EXPR * LHS = MakeFieldAccess( exprLocal, hoistedThis);
            EXPR * RHS = MakeFieldAccess( bindThisImplicit(NULL), hoistedThis);
            bldr.Add(MakeAssignment(LHS, RHS));
        }
        bldr.Add(exprEndIfLabel);

        // Now we need to pass in all the parameters
        FOREACHCHILD(parentAgg, child)
            if (child->isMEMBVARSYM() && child->asMEMBVARSYM()->isHoistedParameter) {
                EXPR * LHS = MakeFieldAccess( exprLocal, child->asMEMBVARSYM());
                EXPR * RHS = MakeFieldAccess( bindThisImplicit(NULL), child->asMEMBVARSYM()->GetOriginalCopy());
                bldr.Add(MakeAssignment(LHS, RHS));
            }
        ENDFOREACHCHILD;

        EXPR * exprCast = mustCastCore( exprLocal, info->meth->retType, NULL, 0);
        bldr.Add(MakeReturn(pOuterScope, exprCast));
        closeScope();
    }
    else {
        // We've alreayd code-gen'd one GetEnumerator, so just call it and cast the result
        EXPRCALL * call = newExpr(NULL, EK_CALL, (*pmethGetEnumerator)->retType)->asCALL();
        call->object = bindThisImplicit(NULL);
        call->mwi.Set((*pmethGetEnumerator), call->object->type->asAGGTYPESYM(), NULL);
        call->args = NULL;
        bldr.Add(MakeReturn(pOuterScope, mustCastCore(call, info->meth->retType, NULL, 0)));
    }

    return exprBlock;
}

EXPR * FUNCBREC::makeIterCur(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo)
{
    InitMethod(info, pAMNode, classInfo);

    ASSERT(info->cpin == 0 && info->meth->params->size == 0);

    NAME * pName = compiler()->namemgr->GetPredefName(PN_ITERCURRENT);
    EXPRBLOCK * pBlock = newExprBlock(NULL);
    EXPR * pValue = MakeFieldAccess(bindThisImplicit(NULL), pName);
    pValue = mustCastCore(pValue, info->meth->retType, NULL, 0);
    pBlock->statements = MakeReturn(pOuterScope, pValue);
    return pBlock;
}

EXPR * FUNCBREC::makeIterReset(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo)
{
    InitMethod(info, pAMNode, classInfo);

    ASSERT(info->cpin == 0 && info->meth->params->size == 0);

    EXPRBLOCK * pBlock = newExprBlock(NULL);
    AGGTYPESYM * ehType = GetOptPDT(PT_NOTSUPPORTEDEXCEPTION);
    pBlock->statements = newExpr(EK_THROW)->asTHROW();
    if (ehType != NULL)
        pBlock->statements->asTHROW()->object = createConstructorCall(pTree, NULL, ehType, NULL, NULL, MemLookFlags::NewObj);
    pBlock->flags |= EXF_NEEDSRET;
    return pBlock;
}

void EraseDebugInfo(SCOPESYM * scope)
{
    scope->debugBlockStart = NULL;
    scope->debugBlockEnd = NULL;
    scope->debugOffsetStart = 0;
    scope->debugOffsetEnd = 0;

    for (SYM * sym = scope->firstChild; sym; sym = sym->nextChild) {
        switch (sym->getKind()) {
        case SK_LOCVARSYM:
            {
            LOCVARSYM * loc = sym->asLOCVARSYM();
            loc->debugBlockFirstUsed = NULL;
            loc->debugOffsetFirstUsed = 0;
            loc->slot.type = NULL;
            if (!(scope->scopeFlags & SF_DISPOSESCOPE))
                loc->slot.SetUsed(false);
            }
            break;
        case SK_SCOPESYM:
            EraseDebugInfo(sym->asSCOPESYM());
            break;
        case SK_ANONSCOPESYM:
        case SK_CACHESYM:
        case SK_LABELSYM:
            break;
        default:
            VSFAIL("Unexpected sym kind");
            break;
        }
    }
}

EXPR * FUNCBREC::makeIterDispose(BASENODE * pAMNode, METHINFO * info, AGGINFO * classInfo)
{
    // Find the locals scope so we can stitch it into the new method
    // for debug info on locals
    SCOPESYM * scopeLocalsMoveNext;
    {
        SYM * sym = info->outerScope->firstChild;
        // Clear out everything except SCOPESYMs
        SYM ** pnextChild = (SYM**)&scopeLocalsMoveNext;
        do {
            if (sym->isSCOPESYM()) {
                *pnextChild = sym;
                pnextChild = &sym->nextChild;
            }
            sym = sym->nextChild;
        } while (sym);
        *pnextChild = NULL;
        ASSERT(scopeLocalsMoveNext);
        ASSERT(scopeLocalsMoveNext->nestingOrder == 1);
    }

    InitMethod(info, pAMNode, classInfo);

    ASSERT(info->cpin == 0 && info->meth->params->size == 0);

    EXPRBLOCK * pBlock = newExprBlock(NULL);
    ASSERT(pOuterScope->firstChild->asLOCVARSYM()->isThis && pOuterScope->firstChild->nextChild == NULL);
    pOuterScope->firstChild->nextChild = pBlock->scopeSymbol = scopeLocalsMoveNext;
    pBlock->statements = info->piin->disposeBody;
    pBlock->flags |= EXF_NEEDSRET;

    EraseDebugInfo(scopeLocalsMoveNext);

    return pBlock;
}

bool FUNCBREC::IteratorRewriteInfo::RewriteFunc(FUNCBREC *funcBRec, EXPR ** expr)
{
    return funcBRec->RewriteIteratorFunc(expr, this);
}

bool FUNCBREC::AnonymousMethodRewriteInfo::RewriteFunc(FUNCBREC *funcBRec, EXPR ** expr)
{
    return funcBRec->RewriteAnonymousMethodFunc(expr, this);
}

// Recursively walks the expr tree, making type substitutions and rewriting the tree using the provided
// call-back function pointer
void FUNCBREC::RecurseAndRewriteExprTree(EXPR ** pexpr, RewriteInfo * rwInfo) {
    ASSERT(pexpr);
    ASSERT(rwInfo);

    if (!*pexpr)
        return;

LRepeat:
    EXPRSTMT * stmtNext = NULL;

    if ((*pexpr)->kind < EK_StmtLim) {
        // Process each in isolation.
        stmtNext = (*pexpr)->asSTMT()->stmtNext;
        (*pexpr)->asSTMT()->stmtNext = NULL;
        // An unreachable switch means that the switch expression is a constant, so
        // at least one of the switch labels is reachable. Thus we need to process the switch.
        if (!(*pexpr)->asSTMT()->FReachable() && (*pexpr)->kind != EK_SWITCH) {
            *pexpr = stmtNext;
            if (!*pexpr)
                return;
            goto LRepeat;
        }
    }

    // Call out to do 2 things:
    //  * transform this expr if needed
    //  * find out if we need to recurse into this expr (post-transform)
    if (rwInfo->RewriteFunc(this, pexpr)) {
        // Substitue all the types
        // NOTE: the order of type substitution is based on
        // the declaration order, so expr->type is last
        if (TypeArray::Size(taClsVarsForMethVars)) {
            switch ((*pexpr)->kind) {
            default:
                ASSERT((*pexpr)->kind > EK_COUNT);
                ASSERT((*pexpr)->flags & EXF_BINOP);
                goto WALK_TYPE_EXPRBINOP;

#define DECLARE_EXPR( name, base)            case EK_ ## name : goto WALK_TYPE_EXPR ## name; WALK_BASE_TYPE_EXPR ## name : \
                                                    goto WALK_TYPE_ ## base; WALK_TYPE_EXPR ## name :
#define DECLARE_EXPR_NO_EK( name, base)      WALK_BASE_TYPE_EXPR ## name : goto WALK_TYPE_ ## base; WALK_TYPE_EXPR ## name :
#define CHILD_EXPR( name, expr_type)            // nop
#define CHILD_EXPR_NO_RECURSE( name, expr_type) // nop
#define NEXT_EXPR( name, expr_type)             // nop
#define CHILD_SYM( name, sym_type)              // nop
// TYPESYM::asTYPESYM() returns void to prevent source code from calling it
// But here we have a legitimate case, so we call PARENTSYM::asTYPESYM(), the compiler should just
// optiimize it down to a nop in retail and an ASSERT in debug
#define CHILD_TYPESYM( name, sym_type)          (static_cast<THIS_TYPE(EXPR)*>(*pexpr))->name = compiler()->getBSymmgr().SubstType( \
                                                    (static_cast<THIS_TYPE(EXPR)*>(*pexpr))->name, (TypeArray*)NULL, \
                                                    taClsVarsForMethVars)->PARENTSYM::as ## sym_type ();
#define CHILD_TYPEARRAY( name)                  (static_cast<THIS_TYPE(EXPR)*>(*pexpr))->name = compiler()->getBSymmgr().SubstTypeArray( \
                                                    (static_cast<THIS_TYPE(EXPR)*>(*pexpr))->name, (TypeArray*)NULL, taClsVarsForMethVars);
#define CHILD_OTHER( name, type)                // nop
#define CHILD_OTHER_SZ( name, type, size)       // nop
#define CHILD_WITHINST( name, type)             (static_cast<THIS_TYPE(EXPR)*>(*pexpr))->name.typeArgs = compiler()->getBSymmgr().SubstTypeArray( \
                                                    (static_cast<THIS_TYPE(EXPR)*>(*pexpr))->name.typeArgs, (TypeArray*)NULL, taClsVarsForMethVars); \
                                                (static_cast<THIS_TYPE(EXPR)*>(*pexpr))->name.ats = compiler()->getBSymmgr().SubstType( \
                                                    (static_cast<THIS_TYPE(EXPR)*>(*pexpr))->name.ats, \
                                                    (TypeArray*)NULL, taClsVarsForMethVars)->asAGGTYPESYM();
#define CHILD_WITHTYPE( name, type)             (static_cast<THIS_TYPE(EXPR)*>(*pexpr))->name.ats = compiler()->getBSymmgr().SubstType( \
                                                    (static_cast<THIS_TYPE(EXPR)*>(*pexpr))->name.ats, \
                                                    (TypeArray*)NULL, taClsVarsForMethVars)->asAGGTYPESYM();
#define END_EXPR()                              goto THIS_TYPE(WALK_BASE_TYPE_EXPR);

#include "exprnodes.h"
            }
WALK_TYPE_EXPR:
            (*pexpr)->setType(compiler()->getBSymmgr().SubstType((*pexpr)->type, (TypeArray*)NULL, taClsVarsForMethVars));
        }

        // Recursively rewrite all the nested expr trees
        // NOTE: the order of type is based on the declaration order,
        // not the semmantic order
        switch ((*pexpr)->kind) {
        default:
            ASSERT((*pexpr)->kind > EK_COUNT);
            ASSERT((*pexpr)->flags & EXF_BINOP);
            goto WALK_EXPRBINOP;

#define DECLARE_EXPR( name, base)            case EK_ ## name : goto WALK_EXPR ## name; WALK_BASE_EXPR ## name : goto WALK_ ## base; WALK_EXPR ## name :
#define DECLARE_EXPR_NO_EK( name, base)      WALK_BASE_EXPR ## name : goto WALK_ ## base; WALK_EXPR ## name :
#define CHILD_EXPR( name, expr_type)            RecurseAndRewriteExprTree((EXPR **)&(static_cast<THIS_TYPE(EXPR)*>(*pexpr))->name, rwInfo);
#define CHILD_EXPR_NO_RECURSE( name, expr_type) // nop
#define NEXT_EXPR( name, expr_type)             // nop
#define CHILD_SYM( name, sym_type)              // nop
#define CHILD_TYPESYM( name, sym_type)          // nop
#define CHILD_TYPEARRAY( name)                  // nop
#define CHILD_OTHER( name, type)                // nop
#define CHILD_OTHER_SZ( name, type, size)       // nop
#define CHILD_WITHINST( name, type)             // nop
#define CHILD_WITHTYPE( name, type)             // nop
#define END_EXPR()                              goto THIS_TYPE(WALK_BASE_EXPR);

#include "exprnodes.h"
        }
WALK_EXPR:
        ; // no nested exprs in EXPR node
    }

    if (stmtNext) {
        while (*pexpr)
            pexpr = (EXPR **)&(*pexpr)->asSTMT()->stmtNext;
        *pexpr = stmtNext;
        goto LRepeat;
    }
}

void SetDisposeScopeFlagInner(SCOPESYM * scope)
{
    FOREACHCHILD( scope, child)
        if (child->isSCOPESYM()) {
            child->asSCOPESYM()->scopeFlags |= SF_DISPOSESCOPE;
            SetDisposeScopeFlagInner(child->asSCOPESYM());
        }
    ENDFOREACHCHILD
}

void SetDisposeScopeFlag(EXPRBLOCK * block)
{
    SCOPESYM * scope = block->scopeSymbol;
    while (!scope) {
        block = block->owningBlock;
        scope = block->scopeSymbol;
        ASSERT(scope);
    }

    for (SCOPESYM * scopeOuter = scope; scopeOuter; scopeOuter = scopeOuter->parent->asSCOPESYM())
        scopeOuter->scopeFlags |= SF_DISPOSESCOPE;

    SetDisposeScopeFlagInner(scope);
}

// Function called by RecurseAndRewriteExprTree to do the actual rewriting of important EXPRs
// for rewriting of iterator bodies (happens after anonymous methods)
bool FUNCBREC::RewriteIteratorFunc( EXPR ** /* in, out */ pexpr, IteratorRewriteInfo * irwInfo)
{
    switch ((*pexpr)->kind) {
    case EK_ANONMETH: case EK_MEMGRP:
        ASSERT(!"Illegal expr kind in transform stage!");
        return false;
    case EK_LOCAL:
        {
            EXPRLOCAL* q = (*pexpr)->asLOCAL();

            // Parameters or locals in a scope with a (nested) yield return that are used or the this pointer
            // have been hoisted and need to be rewritten
            if ((q->local->slot.isParam || !!(q->local->declarationScope()->scopeFlags & SF_HASYIELDRETURN)) &&
                (q->local->slot.IsUsed() || (!!(q->flags & EXF_IMPLICITTHIS) && q->local != thisPointer)))
            {
                // An anonymous method might have hoisted it already
                if (q->local->movedToField->getClass() != info->piin->aggIter) {
                    *pexpr = MakeFieldAccess(irwInfo->exprLocalThis, q->local->movedToField->name,
                        (q->flags & EXF_LVALUE) ? (EXF_LVALUE | EXF_MEMBERSET) : 0);
                } else {
                    *pexpr = MakeFieldAccess(irwInfo->exprLocalThis, q->local->movedToField,
                        (q->flags & EXF_LVALUE) ? (EXF_LVALUE | EXF_MEMBERSET) : 0);
                }
                ASSERT(*pexpr);
                if (q->type->isPARAMMODSYM()) {
                    (*pexpr)->setType(compiler()->getBSymmgr().GetParamModifier((*pexpr)->type, q->type->asPARAMMODSYM()->isOut));
                }
                return false;
            }
        }
        break;
    case EK_RETURN:
        {
            EXPRRETURN* q = (*pexpr)->asRETURN();
            ASSERT(q->flags & EXF_RETURNISYIELD);

            EXPRSTMT * stmtFirst = NULL;
            StmtListBldr bldr(&stmtFirst);

            if (q->object) {
                RecurseAndRewriteExprTree(&q->object, irwInfo);

                EXPRCONSTANT * exprNewStateValue = MakeIntConst(irwInfo->iNextState++);

                bldr.Add(MakeAssignment(q->tree, irwInfo->exprCurrent, q->object));
                bldr.Add(MakeAssignment(irwInfo->exprState, exprNewStateValue));
                // If any yield return is in a try-finally, then all the iterator code is put in a try-fault (to ensure
                // the Dispose method is invoked, since the Dispose method contains the user's finally code).
                bldr.Add(MakeReturn(pOuterScope, MakeBoolConst(true), info->hasYieldAsLeave ? EXF_ASLEAVE : 0));
                EXPRSWITCHLABEL * pLab = MakeSwitchLabel(exprNewStateValue, pOuterScope);
                if (!q->FReachable()) {
                    // This yield is not reachable, so don't make anything after it reachable
                    // by sending the switch to the failure case.
                    pLab->statements = MakeGoto(NULL, irwInfo->exprLabelFailRet, EXF_NODEBUGINFO);
                } else {
                    EXPRLABEL * next = MakeFreshLabel();
                    pLab->statements = MakeGoto(NULL, next, EXF_NODEBUGINFO);
                    bldr.Add(next);
                    pCurrentScope->scopeFlags |= SF_HASYIELDRETURN;
                }
                bldr.Add(MakeAssignment(irwInfo->exprState, MakeIntConst(irwInfo->iFinallyState)));

                if (pCurrentScope != pOuterScope) {
                    // We're inside a finally, so create a switch label for the dispose
                    MakeSwitchLabel(exprNewStateValue, pCurrentScope);
                }
            } else {
                EXPRSTMT * stmt = MakeGoto((*pexpr)->tree, irwInfo->exprLabelFailRet, EXF_NODEBUGINFO);
                if (pCurrentScope != pOuterScope)
                    stmt->flags |= EXF_ASLEAVE;

                if (irwInfo->iFinallyState != IteratorStates::RunningIEnumerator) {
                    // If we're inside a try/finally we need to call
                    // Dispose before returning
                    ASSERT(pCurrentScope != pOuterScope);
                    EXPR * call = BindPredefMethToArgs((*pexpr)->tree, PN_DISPOSE, GetReqPDT(PT_IDISPOSABLE), irwInfo->exprLocalThis, NULL);
                    pCurrentScope->scopeFlags |= SF_HASYIELDBREAK;
                    ASSERT(call->kind == EK_CALL);
                    bldr.Add(MakeStmt((*pexpr)->tree, call));
                }
                else
                    stmt->flags &= ~EXF_NODEBUGINFO;

                bldr.Add(stmt);
            }
            (*pexpr) = stmtFirst;
        }
        return false; // Do not recurse (we already handled the one case of returning a value)
    case EK_TRY:
        {
            EXPRTRY* q = (*pexpr)->asTRY();
            // Save the outer finally state
            int outerFinallyState = irwInfo->iFinallyState;
            // and create a new state for the nested finally
            int innerFinallyState = irwInfo->iFinallyState = irwInfo->iNextState++;
            SCOPESYM * pOldCurrent = pCurrentScope;
            createNewScope();
            SCOPESYM * pFinallyScope = pCurrentScope;
            pCurrentScope->scopeFlags |= SF_SWITCHSCOPE;
            EXPRSTMT * pPrev = info->piin->disposeBody;
            info->piin->disposeBody = NULL;
            EXPRBLOCK * pTry = q->tryblock;
            RecurseAndRewriteExprTree((EXPR**)&pTry, irwInfo);
            ASSERT(pTry->kind == EK_BLOCK);
            // Now restore to the outer Finally state now that we're done with the try body
            irwInfo->iFinallyState = outerFinallyState;
            EXPRSTMT * pHandler = q->handlers;
            RecurseAndRewriteExprTree((EXPR **)&pHandler, irwInfo);
            pCurrentScope = pOldCurrent;
            if (pCurrentScope != pOuterScope) // Propigate out this flag
                pCurrentScope->scopeFlags |= (pFinallyScope->scopeFlags & (SF_HASYIELDBREAK | SF_HASYIELDRETURN));
            if (!(pFinallyScope->scopeFlags & (/*SF_HASYIELDBREAK | */ SF_HASYIELDRETURN))) {
                // No yields in this try-block, so leave it alone;
                info->piin->disposeBody = pPrev;
                q->tryblock = pTry;
                q->handlers = pHandler;
            } else {
                // There was a yield, so we need to save the finally block
                EXPRSTMT * pList = NULL;
                StmtListBldr bldr(&pList);

                // Set the $__state on entry of the try block (This stuff only goes into the MoveNext method)
                EXPRCONSTANT* pTryState = MakeIntConst(innerFinallyState);
                bldr.Add(MakeAssignment(irwInfo->exprState, pTryState));
                bldr.AddList(pTry->statements);
                pTry->statements = pList;
                pList = NULL;
                bldr.Init(&pList);

                // Reset it on entry of the finally block (This stuff goes into MoveNext and Dispose)
                EXPRCONSTANT * pFinState = MakeIntConst(outerFinallyState);
                bldr.Add(MakeAssignment(irwInfo->exprState, pFinState));
                bldr.AddList(pHandler->asBLOCK()->statements);
                SetDisposeScopeFlag(pHandler->asBLOCK());
                pHandler->asBLOCK()->statements = pList;
                pList = NULL;
                bldr.Clear();

                // Test to skip over the finally (This only goes into the Dispose method)
                EXPRLABEL * pGoInto = MakeFreshLabel();
                EXPRLABEL * pSkip = MakeFreshLabel();
                EXPRSWITCHLABEL * pSwLab = MakeSwitchLabel(pTryState, pFinallyScope);
                pSwLab = MakeSwitchLabel(NULL, pFinallyScope);
                EXPRTRY * pDispose = newExpr(EK_TRY)->asTRY();
                ASSERT(q->flags & EXF_ISFINALLY);
                pDispose->tryblock = newExprBlock(NULL);
                pDispose->tryblock->statements = NULL;
                pDispose->tryblock->scopeSymbol = q->tryblock->scopeSymbol;
                // Now put together the switch block that will either jump into or over the try block
                // which might contain nested try/finally stuff
                bldr.Init(&pDispose->tryblock->statements);
                bldr.Add(pGoInto);
                bldr.AddList(info->piin->disposeBody);
                bldr.Clear();

                // q->tryblock now consists of switch label targets and any nested EH stuff
                pDispose->tryblock->scopeSymbol = pTry->scopeSymbol;
                pDispose->handlers = pHandler;
                pDispose->flags = q->flags;
                pDispose->tree = q->tree;
                pDispose->setType(q->type);

                // For dispose
                // switch, try/finally, pSkip
                info->piin->disposeBody = NULL;
                bldr.Init(&info->piin->disposeBody);
                bldr.AddList(pPrev);
                bldr.Add(MakeIteratorSwitch(pFinallyScope, irwInfo, pGoInto, pSkip));
                bldr.Add(pDispose);
                bldr.Add(pSkip);
                bldr.Clear();

                // Replace with a flattened try/finally for the MoveNext method.
                *pexpr = NULL;
                bldr.Init((EXPRSTMT **)pexpr);
                bldr.Add(pTry);
                bldr.Add(pHandler);
            }

            // Already rewritten inner stuff so no need to recurse
            return false;
        }
    case EK_WRAP:
        {
            EXPRWRAP* q = (*pexpr)->asWRAP();
            ASSERT(!q->type->isPARAMMODSYM() && (!q->expr || !q->expr->type->isPARAMMODSYM()));
            if (q->flags & EXF_REPLACEWRAP) {
                // This wrapped expression should be replaced with the contained expr.
                *pexpr = q->expr;
                return false;
            }

            if ((q->flags & EXF_WRAPASTEMP) && (q->containingScope->scopeFlags & SF_HASYIELDRETURN)) {
                // We only need to hoist the wrap if there's a yield in this scope
                NAME* pName = CreateSpecialName(SpecialNameKind::HoistedWrap);
                EXPR * pField = MakeFieldAccess(irwInfo->exprLocalThis, pName, EXF_LVALUE);
                if (pField == NULL) {
                    // Add the 'wrap' field if it doesn't already exist
                    // We use the pointer value to keep it unique
                    MEMBVARSYM * memb = compiler()->getBSymmgr().CreateMembVar(pName, irwInfo->aggInnerClass,
                        irwInfo->aggInnerClass->DeclOnly());
                    memb->SetAccess(ACC_PUBLIC);
                    memb->isReferenced = memb->isAssigned = true;
                    memb->type = compiler()->getBSymmgr().SubstType(q->type, (TypeArray*)NULL, taClsVarsForMethVars);
                    pField = MakeFieldAccess(irwInfo->exprLocalThis, pName, EXF_LVALUE);
                }
                q->expr = pField;
                q->flags |= EXF_REPLACEWRAP;
                *pexpr = pField;
                return false;
            }
            else if (q->expr && q->expr->kind == EK_WRAP && (q->expr->flags & EXF_REPLACEWRAP)) {
                // There is no longer a temp to free.
                *pexpr = newExpr(NULL, EK_NOOP, NULL);
                return false;
            }
            break;
        }
    default:
        CheckForNonvirtualAccessFromClosure(*pexpr);
        break;
    }
    return true;
} // RewriteIteratorFunc

// Only call this after binding the outermost block
// and pass in the scope symbol for that block
// This will push all anonymous methods in this method
// above that down to the given scope.
void FUNCBREC::CorrectAnonMethScope(SCOPESYM* pTrueOutermostScope)
{
    AnonMethInfo * pami;

    for (pami = (m_pamiCur ? m_pamiCur->pamiChild : m_pamiFirst); pami; pami = pami->pamiNext) {
        if (pami->pScope == pOuterScope || pami->pScope->nestingOrder < pTrueOutermostScope->nestingOrder)
            pami->pScope = pTrueOutermostScope;
    }
}

// Returns true if any local (besides <this>) have fHoistForAnonMeth set.
// Also for locals not used in anonymous methods (meaning they don't need to be hoisted)
// We must perform substitution on their types to go from method-based type parameters to class-based ones
bool FUNCBREC::SubstAndTestLocalUsedInAnon(SCOPESYM * pScope)
{
    bool fHasHoistedLocal = false;
    FOREACHCHILD(pScope, child)
        if (child->isLOCVARSYM()) {
            if (child->asLOCVARSYM()->fHoistForAnonMeth) {
                if (!fHasHoistedLocal && !child->asLOCVARSYM()->isThis) {
                    if (TypeArray::Size(taClsVarsForMethVars))
                        fHasHoistedLocal = true;
                    else
                        return true;
                }
            } else if (TypeArray::Size(taClsVarsForMethVars)) {
                child->asLOCVARSYM()->type = compiler()->getBSymmgr().SubstType(child->asLOCVARSYM()->type, (TypeArray*)NULL, taClsVarsForMethVars);
            }
        }
    ENDFOREACHCHILD;

    return fHasHoistedLocal;
}

// Recursively rewrites a block and hoists any locals at this scope if needed
// Also creates and initializes the local cached delegates for the Anonymous Methods at this
// block/scope level
EXPR * FUNCBREC::RewriteLocalsBlock(EXPRBLOCK * q, AnonymousMethodRewriteInfo * amrwInfo)
{

    // Set needsLocals to true if we need a local display class for hoisted locals in this scope
    bool needsLocals = false;
    // If we hoisted <this>, but we're not the outer-most block, clear the hoisting so sibling
    // blocks don't get confused
    bool clearHoistedThis = false;
    SCOPESYM * pScope = q->scopeSymbol;

    // All of the expressions stuffed into this list
    // go before any user code in this block, but be careful because
    // they are added AFTER rewriting the block, they must be created using proper
    // <this> pointers and field references!
    EXPRSTMT * list = NULL;
    StmtListBldr bldr(&list);
    EXPRSTMT * stmtInitThisFld = NULL;

    if (pScope != NULL) {
        // figure out if there are any locals that need to be hoisted in this scope
        needsLocals = SubstAndTestLocalUsedInAnon(pScope);
        // if there's no inner anonymous method, we have no need for a local display class
        ASSERT(amrwInfo->pamiFirst || !needsLocals);

        if (amrwInfo->pamiFirst) {
            // Set if this is the outer-most block and so we need to check for used arguments
            bool alsoCheckArgsScope = !q->owningBlock;

            if (!needsLocals && alsoCheckArgsScope) {
                // this is the outermost scope so we also have to check for usage of arguments
                needsLocals = SubstAndTestLocalUsedInAnon(amrwInfo->pArgScope);
            }

            // Set setLocals to true if !needsLocals, but we have an Anonymous Method at this scope
            // This should only happen if this is the outermost block (meaning we couldn't cache any higher up)
            bool setLocals = false;
            bool fUsesThis = false; // Do any of these methods need a <this> pointer?
            AnonMethInfo * pami;

            for (pami = amrwInfo->pamiFirst; pami; pami = pami->pamiNext) {
                ASSERT(pami->fSeen);
                if (pami->pScope != pScope)
                    continue;

                if (!pami->fUsesLocals && !pami->fUsesThis && amrwInfo->pAMSym->typeVars->size == 0) {
                    ASSERT(q->owningBlock == NULL); // if it really doesn't use anything, we should have pushed top the outermost block
                    // We're going to cache this anonymous method as a static member
                    AGGTYPESYM * atsDel = compiler()->getBSymmgr().SubstType(pami->pDelegateType, (TypeArray *)NULL, taClsVarsForMethVars)->asAGGTYPESYM();
                    NAME * pName = CreateSpecialName(SpecialNameKind::CachedDelegateInstance);
                    MEMBVARSYM * membCache = compiler()->getBSymmgr().CreateMembVar( pName, NULL, NULL);
                    EXPRFIELD * exprField = newExpr(NULL, EK_FIELD, atsDel)->asFIELD();
                    DebugOnly(exprField->fCheckedMarshalByRef = true);

                    membCache->parent = amrwInfo->pAMSym->getClass();
                    membCache->declaration = amrwInfo->pAMSym->declaration;
                    membCache->type = atsDel;
                    membCache->isStatic = true;
                    membCache->isReferenced = true;
                    membCache->isAssigned = true;
                    membCache->isFabricated = true;
                    membCache->SetAccess(ACC_PRIVATE);
                    exprField->fwt.Set(membCache, amrwInfo->pAMSym->getClass()->getThisType());
                    exprField->object = NULL;
                    exprField->flags = EXF_LVALUE;

                    pami->pCachedDelegate = exprField;
                }
                else if (pami->pArgs->parent != pami->pScope) {
                    AGGTYPESYM * atsDel = compiler()->getBSymmgr().SubstType(pami->pDelegateType, (TypeArray *)NULL, taClsVarsForMethVars)->asAGGTYPESYM();
                    NAME * pName = CreateSpecialName(SpecialNameKind::CachedDelegateInstance);
                    LOCVARSYM * pLocal = compiler()->getLSymmgr().CreateLocalSym( SK_LOCVARSYM, pName, pScope)->asLOCVARSYM();
                    pLocal->fIsCompilerGenerated = true;
                    pLocal->slot.hasInit = true;
                    pLocal->slot.isReferenced = true;
                    pLocal->slot.SetUsed(true);
                    pLocal->type = atsDel;

                    // Init it to null
                    bldr.Add(MakeAssignment(
                        pami->pCachedDelegate = MakeLocal(NULL, pLocal, true),
                        bindNull(NULL)));
                }

                if (!needsLocals && !setLocals) {
                    ASSERT(q->owningBlock == NULL);
                    setLocals = true;
                }

                // Keep track if any of the anonymous methods need an outer <this> pointer
                fUsesThis |= pami->fUsesThis;
            }

            if (needsLocals || setLocals) {
                pScope->pasi = (AnonScopeInfo *)allocator->Alloc(sizeof(AnonScopeInfo));
                pScope->pasi->pamiFirst = amrwInfo->pamiFirst;
                if (needsLocals) {
                    // Create the class and $locals for the hoisted variables and instance anonymous methods
                    pScope->pasi->aggHoist = compiler()->clsDeclRec.CreateAnonymousMethodClass(amrwInfo->pAMSym);
                    NAME * pName = CreateSpecialName(SpecialNameKind::DisplayClassInstance);
                    AGGTYPESYM * localsType = compiler()->getBSymmgr().SubstType( pScope->pasi->aggHoist->getThisType(),
                        compiler()->getBSymmgr().ConcatParams( amrwInfo->pAMSym->getClass()->typeVarsAll, amrwInfo->pAMSym->typeVars) )->asAGGTYPESYM();
                    LOCVARSYM * pLocal = compiler()->getLSymmgr().CreateLocalSym( SK_LOCVARSYM, pName, pScope)->asLOCVARSYM();
                    pLocal->fIsCompilerGenerated = true;
                    pLocal->slot.hasInit = true;
                    pLocal->slot.isReferenced = true;
                    pLocal->slot.SetUsed(true);
                    pLocal->type = localsType;
                    pScope->pasi->exprLoc = MakeLocal(NULL, pLocal, true);

                    // If this is the outermethod, then taClsVarsForMethVars isn't set yet
                    // and we can't just set it here, because there's still EXPR's in the outer method to process
                    TypeArray * nestedClsTypeVars = taClsVarsForMethVars;
                    if (nestedClsTypeVars == NULL) {
                        AGGSYM * cls = pScope->pasi->aggHoist;
                        // loop 'outwards' until we find the outermost fabricated type
                        // then we know it's typeVarsThis must match the containing method's typeVars
                        // which are the ones we want to subsitite with
                        while (cls->Parent()->asAGGSYM()->isFabricated)
                            cls = cls->Parent()->asAGGSYM();
                        nestedClsTypeVars = cls->typeVarsThis;
                    }

                    // This will add the .ctor call and 'flatten' any outer locals
                    InitAnonDelegateLocals(pScope, bldr, nestedClsTypeVars);

                    // Move the locals to the new class
                    SYM ** prevChild = &pScope->firstChild;
                    FOREACHCHILD(pScope, child)
                        if (child->isLOCVARSYM() && child->asLOCVARSYM()->fHoistForAnonMeth) {
                            MEMBVARSYM * field = MoveLocalToField( child->asLOCVARSYM(), pScope->pasi->aggHoist, nestedClsTypeVars, false);
                            if (!child->asLOCVARSYM()->isCatch) {
                                // By not updating prevChild we're effectively orphaning these
                                continue;
                            }
                            // Catch variables need to be copied in manually (and left in the user's class)
                            bldr.Add(MakeAssignment(
                                MakeFieldAccess(pScope->pasi->exprLoc, field),
                                MakeLocal(NULL, child->asLOCVARSYM(), false)));
                        }

                        *prevChild = child;
                        prevChild = &child->nextChild;
                        ASSERT(child->name == NULL || child->nextSameName == NULL);
                    ENDFOREACHCHILD;
                    *prevChild = NULL;
                    pScope->psymAttachChild = prevChild;

                    pScope->pasi->exprLoc->setType(localsType);

                    if (alsoCheckArgsScope) {
                        // this is the outermost block so we also have to copy used arguments
                        FOREACHCHILD(amrwInfo->pArgScope, child)
                            if (child->isLOCVARSYM() && child->asLOCVARSYM()->fHoistForAnonMeth) {
                                ASSERT(child->asLOCVARSYM()->slot.isParam);
                                LOCVARSYM * local = child->asLOCVARSYM();
                                MEMBVARSYM * field = MoveLocalToField( local, pScope->pasi->aggHoist, nestedClsTypeVars, false);
                                EXPRSTMT * stmtInit = MakeAssignment(
                                    MakeFieldAccess(pScope->pasi->exprLoc, field),
                                    MakeLocal(NULL, local, false));
                                if (local->isThis) {
                                    ASSERT(!stmtInitThisFld);
                                    stmtInitThisFld = stmtInit;
                                }
                                else
                                    bldr.Add(stmtInit);
                            }
                        ENDFOREACHCHILD;
                        amrwInfo->pArgScope->pasi = pScope->pasi;
                    }

                    if (outerThisPointer && fUsesThis && outerThisPointer->movedToField == NULL) {
                        ASSERT(outerThisPointer->fHoistForAnonMeth);
                        // No outer block hoisted <this> (most likely because they didn't have any locals to hoist).
                        // Hoist <this> for the rewrite of this scope. The hoisting then gets cleared after the rewrite
                        // (so sibling scopes don't use our hoisting).
                        //
                        clearHoistedThis = true;

                        ASSERT(outerThisPointer->slot.isParam);
                        MEMBVARSYM * field = MoveLocalToField( outerThisPointer, pScope->pasi->aggHoist, NULL, false);
                        bldr.Add(MakeAssignment(MakeFieldAccess(pScope->pasi->exprLoc, field), bindThisImplicit(NULL)));
                        outerThisPointer->parent->asSCOPESYM()->pasi = pScope->pasi;
                    }
                } else {
                    // Use the this pointer and class for this method
                    pScope->pasi->aggHoist = amrwInfo->pAMSym->getClass();
                    // This is sometimes null (for static methods)
                    pScope->pasi->exprLoc = bindThisImplicit(NULL)->asLOCAL();
                }
            }

            // Sets the anonymous method parent to be the newly generated class or pAMSym's class.
            for (pami = amrwInfo->pamiFirst; pami; pami = pami->pamiNext) {
                ASSERT(pami->fSeen);
                if (pami->pScope != pScope)
                    continue;

                ASSERT(!pami->meth);
                if (pami->fUsesLocals) {
                    // We should have created a locals class!
                    ASSERT(pScope->pasi->aggHoist->isFabricated);

                    // Need to attach to this locals class.
                    pami->meth = compiler()->getBSymmgr().CreateMethod(pami->name, pScope->pasi->aggHoist, pScope->pasi->aggHoist->DeclOnly());
                    pami->meth->SetMethKind(MethodKind::Anonymous);
                    pami->meth->SetAccess(ACC_PUBLIC);
                    pami->meth->typeVars = BSYMMGR::EmptyTypeArray();
                    pami->thisPointer->type = pScope->pasi->aggHoist->getThisType();
                }
                else {
                    // Need to attach to the next outer method's class.
                    if (amrwInfo->pAMSym->getClass()->isFabricated) {
                        pami->meth = compiler()->getBSymmgr().CreateMethod(pami->name, amrwInfo->pAMSym->getClass(), amrwInfo->pAMSym->getClass()->DeclOnly());
                        // Type variables were moved to the locals class.
                        pami->meth->typeVars = BSYMMGR::EmptyTypeArray();
                    }
                    else {
                        // We don't want to add it to the child list or symbol table.
                        pami->meth = compiler()->getBSymmgr().CreateMethod(pami->name, NULL, NULL);
                        pami->meth->parent = amrwInfo->pAMSym->getClass();
                        pami->meth->declaration = amrwInfo->pAMSym->containingDeclaration();
                        pami->meth->typeVars = amrwInfo->pAMSym->typeVars;
                    }
                    pami->meth->SetMethKind(MethodKind::Anonymous);
                    pami->meth->SetAccess(ACC_PRIVATE);
                    pami->meth->isStatic = !pami->fUsesThis;
                    pami->thisPointer->type = amrwInfo->pAMSym->getClass()->getThisType();

                    //
                    ASSERT(!amrwInfo->pAMSym->isStatic || pami->meth->isStatic); // if the outer method is static we'd better be!
                    if (pami->meth->isStatic || amrwInfo->pAMSym->getClass()->IsStruct() || pami->fInCtorPreamble) {
                        ASSERT(!pami->fInCtorPreamble || q->owningBlock == NULL);
                        pami->meth->isStatic = true;

                        // remove the nested 'this pointer' because the method is static
                        ASSERT(pami->pArgs->firstChild == pami->thisPointer);
                        pami->pArgs->RemoveFromChildList(pami->thisPointer);

                        pami->thisPointer->slot.isParam = false;
                        pami->thisPointer->slot.hasInit = false;
                    }
                }
                pami->meth->parseTree = pami->tree;

                // Find the correct type variables. They're the ones that are on the outermost fabricated type.
                TypeArray * taAgg = pami->meth->getClass()->typeVarsAll;
                AGGSYM * aggPar;
                if (taAgg->size && (aggPar = taAgg->Item(taAgg->size - 1)->parent->asAGGSYM())->isFabricated) {
                    TypeArray * taSubst = aggPar->typeVarsThis;
                    // All type variables with fabricated parent should belong to the same fabricated class.
                    ASSERT(taSubst->size == taAgg->size ||
                        taSubst->size < taAgg->size && !taAgg->Item(taAgg->size - taSubst->size - 1)->parent->asAGGSYM()->isFabricated);
                    pami->meth->retType = compiler()->getBSymmgr().SubstType(pami->typeRet, (TypeArray*)NULL, taSubst);
                    pami->meth->params = compiler()->getBSymmgr().SubstTypeArray(pami->params, (TypeArray*)NULL, taSubst);
                }
                else {
                    pami->meth->retType = pami->typeRet;
                    pami->meth->params = pami->params;
                }
            }
        }
    }

    // Now for the normal rewriting
    RecurseAndRewriteExprTree((EXPR **)&q->statements, amrwInfo);

    // If we hoisted the this pointer at an inner scope, un-hoist so siblings don't try to use it.
    if (clearHoistedThis) {
        outerThisPointer->movedToField = NULL;
        outerThisPointer->parent->asSCOPESYM()->pasi = NULL;
    }

    // Now combine with the init expressions.
    ASSERT(!(q->flags & EXF_CTORPREAMBLE) || q->owningBlock && !q->owningBlock->owningBlock);

    // If we're in a ctor that has a preamble (field initializers and/or base ctor invocation),
    // stmtInitThisFld needs to go AFTER the preamble.
    if (!stmtInitThisFld)
        ;
    else if (q->statements && q->statements->kind == EK_BLOCK && (q->statements->flags & EXF_CTORPREAMBLE)) {
        stmtInitThisFld->stmtNext = q->statements->stmtNext;
        q->statements->stmtNext = stmtInitThisFld;
    }
    else
        bldr.Add(stmtInitThisFld);
    bldr.Add(q);

    if (!list->stmtNext) {
        ASSERT(list->kind == EK_BLOCK);
        return list->asBLOCK();
    }

    EXPRBLOCK * blockRet = newExprBlock(NULL);
    blockRet->statements = list;
    blockRet->owningBlock = q->owningBlock;
    q->owningBlock = blockRet;
    blockRet->flags |= (q->flags & EXF_NEEDSRET);
    q->flags &= ~EXF_NEEDSRET;

    return blockRet;
}

// This rewrites an Anonymous Methods in 2 ways
// It recurses into the Anonymous Method code and rewrites that
// It also transforms the EXPRANONMETH into a real expression something like
// this pseudo-code: ((cache != NULL ? null : cache = new Delegate(AnonymousMethod)), cache)
// We use the EK_SEQUENCE expr to prevent some extra dups and branches
// The generated IL looks something like this:
//      LDLOC   (cache)
//      BRTRUE  AlreadySetLabel
//      LDLOC   ($locals)
//      LDFTN   AnonymousMethod
//      NEWOBJ  Delegate
//      STLOC   (cache)
//      ; in /o- we get an extraneous "BR AlreadySetLabel" here
//  AlreadSetLabel:
//      LDLOC   (cache)
EXPR * FUNCBREC::rewriteAnonMethExpr(EXPRANONMETH * q)
{
    METHSYM * pDelMeth = q->pInfo->meth;
    TypeArray * taOuter = taClsVarsForMethVars;

    ASSERT(pDelMeth->getClass());
    ASSERT(q->pInfo->thisPointer->type == pDelMeth->getClass()->getThisType());

    // If this is the outermost anonymous method, then set the class type variables to use for substitution
    ASSERT(!pDelMeth->getClass()->isFabricated || pDelMeth->getClass()->isNested());
    if (pDelMeth->getClass()->isFabricated && taClsVarsForMethVars == NULL) {
        ASSERT(!pDelMeth->getClass()->GetOuterAgg()->isFabricated);
        taClsVarsForMethVars = pDelMeth->getClass()->typeVarsThis;
    }

    // recursively rewrite the inner EXPR tree (and any inner anonymous methods)
    {
        // push in a new thisPointer
        LOCVARSYM * pOldThis = thisPointer;
        MEMBVARSYM * outerThisMoved = NULL;
        AnonScopeInfo * pasiOuter = NULL;

        thisPointer = q->pInfo->thisPointer;
        if (outerThisPointer && outerThisPointer->fHoistForAnonMeth && !pDelMeth->getClass()->isFabricated) {
            ASSERT(!q->pInfo->fUsesLocals);
            outerThisMoved = outerThisPointer->movedToField;
            pasiOuter = outerThisPointer->parent->asSCOPESYM()->pasi;
            outerThisPointer->movedToField = NULL;
            outerThisPointer->parent->asSCOPESYM()->pasi = NULL;
        }

        q->pInfo->pBodyExpr = RewriteAnonDelegateBodies(pDelMeth, q->pInfo->pArgs, q->pInfo->pamiChild, q->pInfo->pBodyExpr)->asBLOCK();

        // and pop the thisPointer
        thisPointer = pOldThis;
        if (outerThisPointer && outerThisPointer->fHoistForAnonMeth && !pDelMeth->getClass()->isFabricated) {
            outerThisPointer->movedToField = outerThisMoved;
            outerThisPointer->parent->asSCOPESYM()->pasi = pasiOuter;
        }
    }

    taClsVarsForMethVars = taOuter;

    EXPR * obj = NULL;
    EXPRFUNCPTR* funcptr = newExpr(NULL, EK_FUNCPTR, getVoidType())->asFUNCPTR();

    // Now create the cached delegate access/init expression
    // It will replace the EXPRANONMETH so ILGEN will never see this EXPR type
    if (pDelMeth->getClass()->isFabricated) {
        // Anonymous methods get processed/transformed before iterators, so this fabricated class
        // will never be an iterator, and always a display class
        EXPR * exprLoc = q->pInfo->pScope->pasi->exprLoc;
        AGGTYPESYM * atsLoc = exprLoc->type->asAGGTYPESYM();
        if (pDelMeth->getClass() == atsLoc->getAggregate())
            funcptr->mwi.Set(pDelMeth, atsLoc, BSYMMGR::EmptyTypeArray());
        else {
#ifdef DEBUG
            for (BAGSYM * bagTmp = atsLoc->getAggregate(); bagTmp != pDelMeth->getClass(); bagTmp = bagTmp->BagPar()) {
                if (bagTmp == NULL) {
                    VSFAIL("Expected nested type!");
                    break;
                }
            }
#endif
            funcptr->mwi.Set(pDelMeth,
                compiler()->getBSymmgr().SubstType(pDelMeth->getClass()->getThisType(), atsLoc->typeArgsAll, NULL)->asAGGTYPESYM(),
                BSYMMGR::EmptyTypeArray());
            exprLoc = bindThisImplicit(NULL);
        }

        if (pDelMeth->isStatic) {
            funcptr->object = NULL;
            obj = bindNull(NULL);
        }
        else {
            obj = funcptr->object = exprLoc;
        }
    } else {
        // Call on the non-fabricated class.
        ASSERT(q->pInfo->thisPointer->type == parentAgg->getThisType());
        ASSERT(thisPointer == NULL || q->pInfo->thisPointer->type == thisPointer->type);
        funcptr->mwi.Set(pDelMeth, parentAgg->getThisType(), pMSym->typeVars);
        if (pDelMeth->isStatic) {
            funcptr->object = NULL;
            obj = bindNull(NULL);
        }
        else {
            obj = funcptr->object = bindThisImplicit(NULL);
            if (parentAgg->IsStruct()) {
                obj = mustCast(obj, GetReqPDT(PT_OBJECT));
            }
        }
    }

    ASSERT(funcptr->mwi.Meth() && funcptr->mwi.Type() && funcptr->mwi.TypeArgs());
    ASSERT(funcptr->mwi.TypeArgs()->size == q->pInfo->meth->typeVars->size);

    AGGTYPESYM * delType = compiler()->getBSymmgr().SubstType(q->pInfo->pDelegateType, (TypeArray*)NULL, taClsVarsForMethVars)->asAGGTYPESYM();
    EXPRCALL * call = newExpr(NULL, EK_CALL, delType)->asCALL();
    call->args = newExprBinop(NULL, EK_LIST, getVoidType(), obj, funcptr);
    call->object = NULL;
    call->mwi.Set(FindDelegateCtor(delType, q->tree), delType, NULL);
    ASSERT(call->mwi.Meth() != NULL);
    call->flags |= EXF_NEWOBJCALL | EXF_CANTBENULL;

    if (q->pInfo->pCachedDelegate) {
        ASSERT(delType == q->pInfo->pCachedDelegate->type);
        EXPR* compare = newExprBinop( NULL, EK_NE, GetReqPDT(PT_BOOL), q->pInfo->pCachedDelegate, bindNull(NULL));
        EXPR* temp = newExprBinop( NULL, EK_ASSG, delType, q->pInfo->pCachedDelegate, call);
        temp->flags |= EXF_ASSGOP;
        temp = newExprBinop( NULL, EK_BINOP, getVoidType(), NULL, temp);
        temp = newExprBinop( q->tree, EK_QMARK, delType, compare, temp);
        return newExprBinop( q->tree, EK_SEQUENCE, delType, temp, q->pInfo->pCachedDelegate);
    } else {
        return call;
    }
}

// Function called by RecurseAndRewriteExprTree to do the actual rewriting of important EXPRs
// for rewrite anonymous delegates outers and bodies by changing locals access into appropriate EK_FIELDs
bool FUNCBREC::RewriteAnonymousMethodFunc( EXPR ** /* in, out */ expr, AnonymousMethodRewriteInfo * amrwInfo)
{
    switch ((*expr)->kind) {
    case EK_BLOCK:
        *expr = RewriteLocalsBlock( (*expr)->asBLOCK(), amrwInfo);
        return false;
    case EK_ANONMETH:
        *expr = rewriteAnonMethExpr((*expr)->asANONMETH());
        return false;
    case EK_LOCAL:
        {
            EXPRLOCAL* q = (*expr)->asLOCAL();

            // if it wasn't hoisted, leave it alone
            if (!q->local->fHoistForAnonMeth)
                return true;

            // Find if this local is defined inside our scopes, or some outer scope
            // We always treat <this> as if it was defined outside everything
            if (q->local->parent->asSCOPESYM()->nestingOrder >= amrwInfo->pArgScope->nestingOrder &&
                !q->local->isThis)
            {
                ASSERT(q->local->movedToField);
                // A real or anonymous method accessing an inner local that has been hoisted
                // so we now have an extra level of indirection.
                DWORD dwFlags = q->flags & EXF_MASK_ANY;
                if (dwFlags & EXF_LVALUE)
                    dwFlags |= EXF_MEMBERSET;
                EXPR * pObject = MakeFieldAccess(q->local->parent->asSCOPESYM()->pasi->exprLoc, q->local->movedToField, dwFlags);
                if (q->type->isPARAMMODSYM()) {
                    pObject->setType(compiler()->getBSymmgr().GetParamModifier(pObject->type, q->type->asPARAMMODSYM()->isOut));
                }
                *expr = pObject;
            } else {
                // An anonymous method accessing a hoisted local defined in an outer scope (including parameters and <this>)
                // Or a real method accessing <this>

                // If optimizations allowed this method to be parented by a user class, then the only outer local
                // it could be accessing is <this> in which case, we don't need to transform it, but we do need to adjust
                // which thisPointer LOCVARSYM it uses
                // Or if this method isn't an anonymous method, then it must be an iterator
                // of which the <this> access will get transformed later.
                if (!amrwInfo->pAMSym->getClass()->isFabricated) {
                    ASSERT(q->local->name == compiler()->namemgr->GetPredefName(PN_THIS) && q->local->isThis);
                    ASSERT(!amrwInfo->pAMSym->isStatic);
                    q->local = thisPointer;
                    return false;
                }

                EXPR * pObject = bindThisImplicit(NULL);
                ASSERT(pObject->type == amrwInfo->pAMSym->getClass()->getThisType());

                if (pObject->type == q->type) {
                    // This happens because of how we share the exprLoc
                    ASSERT(q->type->getAggregate()->isFabricated);
                    return false;
                }

                DWORD dwFlags = q->flags & EXF_MASK_ANY;
                if (dwFlags & EXF_LVALUE)
                    dwFlags |= EXF_MEMBERSET;

                ASSERT(q->local->movedToField);
                // If the local is not in this display class
                // it must be in an outer one, which requires exactly one more level of indirection
                if (amrwInfo->pAMSym->getClass() != q->local->movedToField->getClass()) {
                    LOCVARSYM * locPar = q->local->parent->asSCOPESYM()->pasi->exprLoc->local;
                    pObject = MakeFieldAccess(pObject, locPar->name, dwFlags);
                }

                // Now pObject is either '<this>' or '<this>.someField' where someField is a flattened
                // outer $locals that has the hoisted variable
                ASSERT(pObject->type->getAggregate() == q->local->movedToField->getClass());
                pObject = MakeFieldAccess( pObject, q->local->movedToField, dwFlags);
                if (q->type->isPARAMMODSYM()) {
                    pObject->setType(compiler()->getBSymmgr().GetParamModifier(pObject->type, q->type->asPARAMMODSYM()->isOut));
                }
                *expr = pObject;
            }
            return false;
        }
    default:
        //'RewriteAnonymousMethodFunc' is called to process the entire body of a method that contains
        //an anonymous method, not just the body of the anonymous method. Before emitting
        //warnings related to non-virtual calls to virt functions within closures, confirm 
        //that the access is actually from within the method that is put on the closure.
        if (amrwInfo->pAMSym->isAnonymous ())
            CheckForNonvirtualAccessFromClosure(*expr);
        break;
    }

    return true;
} // RewriteAnonymousMethodFunc

void FUNCBREC::CheckForNonvirtualAccessFromClosure(EXPR* expr)
{
    //if this is left around in EE case, ILGENREC::callAsVirt needs to be moved 
    //to a file that is built with the EE.
    switch (expr->kind) {
    case EK_FUNCPTR:
        {
            EXPRFUNCPTR * funcptr = expr->asFUNCPTR();
            if (funcptr->mwi.Meth()->isVirtual &&
                (funcptr->mwi.Meth()->getClass()->isSealed || funcptr->object->type->fundType() != FT_REF ||
                (funcptr->flags & EXF_BASECALL))) {
                compiler()->Error(funcptr->tree, WRN_NonVirtualCallFromClosure, funcptr->mwi.Meth());
            }
            break;
        }
    case EK_CALL:
        {
            EXPRCALL *call = expr->asCALL();
            METHSYM * func = call->mwi.Meth();
            EXPR * object = call->object;
            bool fConstrained = !!(call->flags & EXF_CONSTRAINED);
            bool isBaseCall = !!(call->flags & EXF_BASECALL);
            
            if (func->isVirtual && !fConstrained && !ILGENREC::callAsVirtual(func, object, isBaseCall) &&
                object->type->IsRefType ()) {
                compiler()->Error(call->tree, WRN_NonVirtualCallFromClosure, func);
            }
            break;
        }
    case EK_PROP:
        {
            EXPRPROP *prop = expr->asPROP();
            PROPSYM * propSlot = prop->pwtSlot.Prop();
            EXPR * object = prop->object;
            bool fConstrained = !!(prop->flags & EXF_CONSTRAINED);
            bool isBaseCall = !!(prop->flags & EXF_BASECALL);

            MethWithType mwt = prop->mwtGet ? prop->mwtGet : prop->mwtSet;
            if (mwt.Meth()->isVirtual && !fConstrained && !ILGENREC::callAsVirtual(mwt.Meth(), object, isBaseCall)) {
                compiler()->Error(prop->tree, WRN_NonVirtualCallFromClosure, propSlot);
            }
            break;
        }
    default:
        break;
    }
}



// Bind a const initializer. The value of the initializer must
// be implicitly converted to the constant type.
EXPR * FUNCBREC::bindConstInitializer(MEMBVARSYM * pAFSym, BASENODE * tree)
{
    CHECKEDCONTEXT checked(this, compiler()->GetCheckedMode());
    this->checked.constant = true;

	EXPR *rval = NULL;
	if (tree->kind == NK_ARRAYINIT && pAFSym->type->isARRAYSYM()) {
        // special case binding of array initializer here.  It is not legal to have an initializer for a
        // constant array, but we need to avoid generating the error deep inside bindExpr and instead
        // allow the caller to generate the appropriate error when it discovers that the initializer
        // expression is not actually constant.
        rval = bindArrayInit(tree->asARRAYINIT(), pAFSym->type->asARRAYSYM(), NULL);
	}
	else {
        // Just bind the expression...
        rval = bindExpr(tree);
    }

    checked.restore(this);

    // Convert to the correct destination type
    if (pAFSym->parent->isAGGSYM() && pAFSym->getClass()->IsEnum()) {
        ASSERT(pAFSym->type == pAFSym->getClass()->getThisType());  // enumerator must be of correct type.

        // Enumerator fields are handled somewhat special. The initializer
        // must be convertable to the *underlying* type of the enum.
        rval = mustConvert(rval, pAFSym->type->underlyingType());
    }
    else if (pAFSym->fixedAgg) {
        // Fixed-sized buffer length.
        rval = mustConvert(rval, GetReqPDT(PT_INT));
        if (rval->kind == EK_CONSTANT && rval->asCONSTANT()->getI64Value() <= 0)  {
            compiler()->Error(tree, ERR_InvalidFixedArraySize);
            // No need to cascade errors
            rval->asCONSTANT()->getVal().iVal = 0;
        }
    }
    else {
        // Regular const field.
        rval = mustConvert(rval, pAFSym->type);
    }

    return rval;
}

// called to compile the value of a constant field.  tree here points to
// the expression being assigned to the field...
EXPR * FUNCBREC::compileFirstField(MEMBVARSYM * pAFSym, BASENODE * tree)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);

    // Save and restore state.
    FncBindState state(this);
    MEMBVARSYM * pFOrigSymOld = pFOrigSym;

    pCatchScope = pTryScope = pFinallyScope = pSwitchScope = pOuterScope = NULL;
    pCurrentScope = compiler()->getLSymmgr().CreateLocalSym(SK_SCOPESYM, NULL, NULL)->asSCOPESYM();
    pCurrentScope->nestingOrder = 1;

    ASSERT(pAFSym->isUnevaled);
    InitField(pAFSym);
    pFOrigSym = pFSym;

    // Just bind the initializer...
    EXPR * rval = bindConstInitializer(pAFSym, tree);

    // Restore the previous field.
    pFOrigSym = pFOrigSymOld;

    return rval;
}

// called to compile a constant field which was referenced during the compilation
// of another constant field...
EXPR * FUNCBREC::compileNextField(MEMBVARSYM * pAFSym, BASENODE * tree)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);

    // Save and restore state.
    FncBindState state(this);

    InitField(pAFSym);

    // Just bind the initializer...
    EXPR * rval = bindConstInitializer(pAFSym, tree);

    return rval;
}

// Initialize the expr generator for the eval of a field
// constant, if another constant is being evaled already...
void FUNCBREC::InitField(MEMBVARSYM * field)
{
    ASSERT(field);
    pMSym = NULL;
    pFSym = field;
    pTree = NULL; // pTree is only used for function members.
    pClassInfo = NULL;
    parentAgg = pFSym->getClass();
    parentDecl = pFSym->containingDeclaration();
    btfFlags = pFSym->isContainedInDeprecated() ? TypeBindFlags::NoDeprecated : TypeBindFlags::None;
}


void FUNCBREC::InitMethod(METHINFO * infoMeth, BASENODE * tree, AGGINFO * infoCls)
{
    ASSERT(infoMeth && infoMeth->meth && tree && infoCls);
    ASSERT(
        tree->kind == NK_CLASS || tree->kind == NK_STRUCT || tree->kind == NK_METHOD || tree->kind == NK_CTOR ||
        tree->kind == NK_ACCESSOR || tree->kind == NK_OPERATOR || tree->kind == NK_DTOR  ||
        tree->kind == NK_VARDECL || tree->kind == NK_PROPERTY || tree->kind == NK_INDEXER);
    ASSERT(tree->kind != NK_CLASS || infoMeth->meth->isCtor() || infoMeth->meth->isIfaceImpl);
    ASSERT(tree->kind != NK_VARDECL && tree->kind != NK_PROPERTY || infoMeth->meth->isAnyAccessor());

    pMSym = infoMeth->meth;
    pFSym = NULL;
    taClsVarsForMethVars = NULL;
    pTree = tree;
    pClassInfo = infoCls;
    info = infoMeth;

    parentAgg = pMSym->getClass();
    parentDecl = pMSym->containingDeclaration();

    btfFlags = pMSym->isContainedInDeprecated() ? TypeBindFlags::NoDeprecated : TypeBindFlags::None;

    pOuterScope = compiler()->getLSymmgr().CreateLocalSym(SK_SCOPESYM, NULL, NULL)->asSCOPESYM();
    pOuterScope->nestingOrder = 0;
    infoMeth->outerScope = pOuterScope;
    pCatchScope = pTryScope = pFinallyScope = pCurrentScope = pOuterScope;
    pSwitchScope = NULL;

    initLabels.breakLabel = NULL;
    initLabels.contLabel = NULL;
    loopLabels = &initLabels;

    uninitedVarCount = 0;
    unreferencedVarCount = 0;
    finallyNestingCount = 0;
    localCount = 0;
    m_pamiFirst = NULL;
    m_pamiCur = NULL;
    gotos = NULL;

    userLabelList = NULL;
    pUserLabelList = &userLabelList;

    m_fForwardGotos = false;

    initThisPointer();

    pCurrentBlock = NULL;
    lastNode = NULL;
}


// Declare the variable of a given name and type in the current scope.
LOCVARSYM * FUNCBREC::declareVar(BASENODE * tree, NAME * ident, TYPESYM * type
#ifdef DEBUG
                                 , bool fSkipScopeCheck
#endif
                                 )
{
    checkUnsafe(tree, type);

    SYM * rval = compiler()->getLSymmgr().LookupLocalSym(ident, pCurrentScope, MASK_LOCVARSYM);
    if (rval) {
        compiler()->Error(tree, ERR_LocalDuplicate, ident);
        return NULL;
    }

    localCount ++;

    if (localCount > 0xffff) {
        compiler()->Error(tree, ERR_TooManyLocals);
    }

    // Anonymous methods and iterators don't like locals in scopes that
    // don't belong to blocks!
    ASSERT(fSkipScopeCheck || pCurrentScope == pCurrentBlock->scopeSymbol);

    rval = compiler()->getLSymmgr().CreateLocalSym(SK_LOCVARSYM, ident, pCurrentScope);
    rval->asLOCVARSYM()->type = type;
    rval->asLOCVARSYM()->declTree = tree;

    storeInCache(tree, ident, rval, NULL, true);

    return rval->asLOCVARSYM();
}



// Declare the given parameter in the outermost scope.
// Checks for duplicate parameter names.
inline LOCVARSYM * FUNCBREC::declareParam(NAME *ident, TYPESYM *type, unsigned flags, BASENODE *parseTree, SCOPESYM * pScope)
{
    ASSERT(type);

    if (pScope == NULL)
        pScope = pOuterScope;

    checkUnsafe(parseTree, type);

    SYM * sym = compiler()->getLSymmgr().LookupLocalSym(ident, pScope , MASK_LOCVARSYM | MASK_CACHESYM);
    if (sym) {
        // Try to report the error on the name, not the type
        if (parseTree && parseTree->kind == NK_PARAMETER)
            parseTree = parseTree->asPARAMETER()->pName;

        if (sym->isCACHESYM()) {
            ASSERT(sym->asCACHESYM()->sym->isTYVARSYM() && sym->asCACHESYM()->sym->parent == pMSym);
            compiler()->Error(parseTree, ERR_LocalSameNameAsTypeParam, ident);
        }
        else {
            compiler()->Error(parseTree, ERR_DuplicateParamName, ident);
        }
        return NULL;
    }

    LOCVARSYM * rval = addParam(ident, type, flags, pScope);
    rval->declTree = parseTree;

    return rval;
}

// adds a parameter to the outermost scope
// does not do duplicate parameter error checking
LOCVARSYM * FUNCBREC::addParam(PNAME ident, TYPESYM *type, uint flags, SCOPESYM * pScope)
{
    flags &= (NF_PARMMOD_OUT | NF_PARMMOD_REF);
    ASSERT(!type->isPARAMMODSYM() || !flags);

    if (type->isPARAMMODSYM()) {
        flags = (type->asPARAMMODSYM()->isOut ? NF_PARMMOD_OUT : NF_PARMMOD_REF);
        type = type->asPARAMMODSYM()->paramType();
    }

    if (pScope == NULL)
        pScope = pOuterScope;

    LOCVARSYM * rval = compiler()->getLSymmgr().CreateLocalSym(SK_LOCVARSYM, ident, pScope)->asLOCVARSYM();
    rval->slot.hasInit = true;
    rval->slot.isParam = true;
    rval->slot.isReferenced = true;     // Set this so we never report unreferenced parameters

    if (flags) {
        rval->slot.isRefParam = true;
        rval->slot.SetUsed(true);
        if (flags & NF_PARMMOD_OUT) {
            rval->slot.hasInit = false;
            rval->slot.SetJbitDefAssg(uninitedVarCount + 1);
            uninitedVarCount += FlowChecker::GetCbit(compiler(), type);
        }
    }

    rval->type = type;
    return rval;
}


// Bind the given type assuming the current namespace and class context and method.
// This also calls EnsureState on the type.
TYPESYM * FUNCBREC::bindType(TYPEBASENODE * tree)
{
    TYPESYM * type = TypeBind::BindType(compiler(), tree, contextForTypeBinding(), btfFlags);
    if (type)
        compiler()->EnsureState(type);

    return type;
}

// Store the given symbol in the local cache of symbols.
// Check for parent indicates whether to check that no cache entry exists which
// denotes a symbol bound in a parent scope, which we would be rebinding now (which
// is illegal)  It is possible to optimize away this check if we know that
// no symbol was found which was visible in our scope, since visiblity would
// imply that it had to be bound in a parent scope.
// We know the absence of this visibility if a cache lookup for the symbol
// failed, which is what happens when this function is called from bindName(...)
void FUNCBREC::storeInCache(BASENODE * tree, NAME * name, SYM * sym, TypeArray * types, bool checkForParent)
{
    ASSERT((sym->isLOCVARSYM() || sym->isAGGTYPESYM() || sym->isNSAIDSYM()) == !types);

    if (!pOuterScope)
        return;

    CACHESYM * cacheEntry = NULL;
    SYM * prev = compiler()->getLSymmgr().LookupLocalSym(name, pOuterScope, MASK_CACHESYM | MASK_LOCVARSYM);
    if (prev) {
        int id;

        // If we have a previous cache entry, then the current one might clash and
        // not be valid, so we need to check:
        if (prev->isLOCVARSYM()) {
            // trying to rebind the name of a param, this is ALWAYS a nono:
            id = IDS_PARENT;
            goto DISPERROR;
        }
        cacheEntry = prev->asCACHESYM();

        prev = cacheEntry->sym;

        // if the cached name already denotes the same symbol, there can be no conflict...
        if (prev == sym) {
            // Note that it is impossible at this point for sym's scope to be a
            // child of prev's scope.  This is because if it were so then prev would
            // be visible in this scope and would have been found on a cache lookup,
            // since this is only called on a lookup failure, except in the case of
            // Local decls.  However, in the case of local decls it is impossible for
            // the decl to be already in the cache since it is being declared.
            ASSERT(cacheEntry->types == types);
            ASSERT(!checkForParent);
            cacheEntry->flags = NameCacheFlags::None;
            cacheEntry->scope = pCurrentScope;
            return;
        }

        // first check whether previous gives a meaning to the name in our
        // current scope...  this means that there exists a parent/child
        // relationship between previous' scope and the current scope...

        SCOPESYM * current;
        if (checkForParent
#if DEBUG
            || true
#endif
            ) {
            // Does the previous definition occur in a parent scope the thereby
            // contribute a meaning to our scope?
            current = pCurrentScope;
            unsigned prevOrdinal = cacheEntry->scope->nestingOrder;

            if (prevOrdinal == 0) {
                ASSERT(prev->isTYVARSYM() && prev->parent == pMSym);
                goto LInScope;
            }

            ASSERT(prevOrdinal > 0);
            // we only need to check up to the same tree height, obviously a
            // parent of our scope has to have a lower ordinal
            while (current->nestingOrder >= prevOrdinal) {
                if (current == cacheEntry->scope) {
LInScope:
                    // ok, the previous definition is visible in this scope, and so cannot
                    // be overriden:
                    ASSERT(checkForParent);
                    // The failure of the above assert means sombody called this
                    // w/ checkForParent turned off w/o doing a cache lookup
                    // on this name first...
                    id = IDS_PARENT;
                    goto DISPERROR;
                }
                current = current->parent->asSCOPESYM();
            }
        }

        // Does the previous definition occur in a child scope of the current one?
        current = cacheEntry->scope;
        // Obviously, pCurrentScope->nestingOrder is always at least 1 since a 0
        // would imply arguments scope. So, the loop will break before current
        // becomes NULL...
        while (current->nestingOrder >= pCurrentScope->nestingOrder) {
            if (current == pCurrentScope) {
                // ok, the current definition would override a previous one and be
                // visible in the scope of the previous one, so in that scope
                // the name could have two possible meanings... this is also
                // illegal...
                id = IDS_CHILD;
                goto DISPERROR;
            }
            current = current->parent->asSCOPESYM();
        }

        // ok, the definitions would not clash, so we just need to rebind the
        // cache entry:

        // [The reason this works is that we assume that we bind the parsetree
        //  in lexical order, so that all child scopes are bound before doing
        //  any code in parent scopes that follows the child scope in question...]

        goto MAKENEWBINDING;

DISPERROR:
        if (sym->isLOCVARSYM()) {
            if (prev->isTYVARSYM() && prev->parent == pMSym)
                compiler()->Error(tree, ERR_LocalSameNameAsTypeParam, name);
            else
                compiler()->Error(tree, ERR_LocalIllegallyOverrides, name, ErrArgIds(id));
        } else {
            compiler()->Error(tree, ERR_NameIllegallyOverrides, sym, name, ErrArgIds(id));
        }

        // For IDS_CHILD, make the binding anyway.
        if (id != IDS_CHILD)
            return;

    } else {
        // A cache miss
        cacheEntry = compiler()->getLSymmgr().CreateLocalSym(SK_CACHESYM, name, pOuterScope)->asCACHESYM();
    }

MAKENEWBINDING:
    cacheEntry->sym = sym;
    cacheEntry->types = types;
    cacheEntry->flags = NameCacheFlags::None;
    cacheEntry->scope = pCurrentScope;
}


void FUNCBREC::storeFlagsInCache(NAME * name, NameCacheFlagsEnum flags)
{
    if (!pOuterScope)
        return;

    SYM * prev = compiler()->getLSymmgr().LookupLocalSym(name, pOuterScope, MASK_CACHESYM | MASK_LOCVARSYM);

    // No can do for parameters.
    if (prev && prev->isCACHESYM()) {
        ASSERT(prev->asCACHESYM()->sym->isPROPSYM() || prev->asCACHESYM()->sym->isLOCVARSYM() || prev->asCACHESYM()->sym->isMEMBVARSYM());
        prev->asCACHESYM()->flags = flags;
    }
}



/***************************************************************************************************
    This finds a predefined method with the given name and parameter types in the given type. The
    method must be in the given type - not in a base type.
***************************************************************************************************/
METHSYM * FUNCBREC::FindPredefMeth(BASENODE * tree, PREDEFNAME pn, AGGTYPESYM * type, TypeArray * params, bool fReportError, MemLookFlagsEnum flags)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);
    ASSERT(params);

    // caused by GetOptPDT failing on non-required types
    if (!type)
        return NULL;

    NAME * name = compiler()->namemgr->GetPredefName(pn);

    if (pn == PN_CTOR)
        flags |= MemLookFlags::Ctor;

    MemberLookup mem;
    if (!mem.Lookup(compiler(), type, NULL, parentDecl, name, 0, flags) ||
        !mem.SymFirst()->isMETHSYM() ||
        mem.SymFirst()->asMETHSYM()->getClass() != type->getAggregate())
    {
        if (fReportError)
            compiler()->Error(tree, ERR_MissingPredefinedMember, type, name);
        return NULL;
    }
    mem.ReportWarnings(tree);

    EXPRMEMGRP * grp = newExprMemGrp(tree, NULL, mem);

    // Only look in the given type.
    grp->typePar = type;
    grp->types = compiler()->getBSymmgr().AllocParams(1, (TYPESYM **)&type);

    MethWithInst mwi;
    MethWithInst mwiAmbig;

    BindGrpToParams(&mwi, tree, grp, params, fReportError, &mwiAmbig);

    ASSERT(!mwi ||
        mwi.Type() == type &&
            compiler()->getBSymmgr().SubstEqualTypeArrays(params, mwi.Meth()->params, type) &&
            mwi.Meth()->typeVars->size == 0);

    if (mwiAmbig && !fReportError)
        return NULL;

    return mwi.Meth();
}


SYM * FUNCBREC::lookupCachedName(NAMENODE * name, symbmask_t mask, TypeArray ** ptypes, NameCacheFlagsEnum * pflags)
{
    TypeArray * types = NULL;
    unsigned rvalOrdinal;
    SCOPESYM * current = NULL;
    NameCacheFlagsEnum flags = NameCacheFlags::None;
    *ptypes = NULL;
    *pflags = NameCacheFlags::None;

    if (!pOuterScope)
        return NULL;

    SYM * rval = compiler()->getLSymmgr().LookupLocalSym(
        name->pName, pOuterScope, (mask & MASK_LOCVARSYM) | MASK_CACHESYM);

    if (!rval)
        return NULL;

    if (rval->isLOCVARSYM()) { // this is a param...
        goto FOUNDVISIBLE;
    }

    SCOPESYM * cacheScope;
    cacheScope = rval->asCACHESYM()->scope;
    types = rval->asCACHESYM()->types;
    flags = rval->asCACHESYM()->flags;
    rval = rval->asCACHESYM()->sym;

    // Check that the definition is visible in our scope:
    // (ie, it occurs in our parent, or in our scope)

    rvalOrdinal = cacheScope->nestingOrder;

    ASSERT(rvalOrdinal > 0 || rvalOrdinal == 0 && rval->isTYVARSYM() && rval->asTYVARSYM()->isMethTyVar);
    // Note that the ordinal of rval is at least 1 except for method type variables, since a 0 implies the
    // outer scope and only params and type variables are found there.
    if (!rvalOrdinal)
        goto FOUNDVISIBLE;

    // Note that the ordinal of rval is at least 1 now so the loop will break before current is null.
    current = pCurrentScope;
    while (current->nestingOrder >= rvalOrdinal) {
        if (current == cacheScope) {
FOUNDVISIBLE:
            // ok, this definition is visible and defines the meaning for the name
            // is it of the right type?
            if (rval->mask() & mask) {
                *ptypes = types;
                *pflags = flags;
                return rval;
            }
            ErrorBadSK(name, SymWithType(rval, types && types->size == 1 ? types->Item(0)->asAGGTYPESYM() : NULL), mask);
            return compiler()->getBSymmgr().GetErrorType(NULL, name->pName, NULL);
        }
        current = current->parent->asSCOPESYM();
    }

    // no parent relation, or no visible definition:
    return NULL;

}

// Display an error based on the fact that the given symbol is not of the
// expected kind...
void FUNCBREC::ErrorBadSK(BASENODE * tree, SymWithType swt, symbmask_t mask, int bindFlags)
{
    ASSERT(!(swt.Sym()->mask() & mask));

    // First check for the bad cast error.
    if (bindFlags & BIND_MAYBECONFUSEDNEGATIVECAST) {
        // This could be a case where the user wrote (E)-1 instead of (E)(-1) where E is a
        // type that a negative number could be cast to. If it is of this form, report
        // an additional hint to the user.
        AGGTYPESYM * ats = NULL;
        if (swt.Sym()->isAGGSYM()) {
            ats = swt.Sym()->asAGGSYM()->getThisType();
        } else if (swt.Sym()->isAGGTYPESYM()) {
            ats = swt.Sym()->asAGGTYPESYM();
        }

        if (ats && (ats->isEnumType() || ats->isNumericType() || ats->isPredefType(PT_OBJECT) || ats->getAggregate()->hasConversion)) {
            // Look 'up' the parse tree for the OP_SUB that set the flag
            BASENODE * parent = tree->pParent;
            while (parent && parent->kind != NK_BINOP && parent->Op() != OP_SUB) {
                parent = parent->pParent;
            }
            if (!parent)
                parent = tree;

            compiler()->Error(parent, ERR_PossibleBadNegCast);
        }
    }

    SYMKIND expected;
    if ((mask & (MASK_LOCVARSYM)) &&
        !(mask & (MASK_AGGTYPESYM)) &&
        !(mask & (MASK_NSAIDSYM)) &&
        !(mask & (MASK_EVENTSYM)))
    {
        expected = SK_LOCVARSYM;
    }
    else if (mask == (MASK_MEMBVARSYM | MASK_PROPSYM))
        expected = SK_LOCVARSYM;
    else if (mask == MASK_METHSYM)
        expected = SK_METHSYM;
    else if (mask == MASK_MEMBVARSYM)
        expected = SK_MEMBVARSYM;
    else if (mask == MASK_AGGSYM)
        expected = SK_AGGSYM;
    else
        expected = (SYMKIND)0;

    if (swt.Sym()->isEVENTSYM() && (mask & MASK_MEMBVARSYM)) {
        if (swt.Event()->implementation)
            compiler()->Error(tree, ERR_BadEventUsage, swt, swt.Event()->getClass());
        else
            compiler()->Error(tree, ERR_BadEventUsageNoField, swt);
    }
    else if (expected) {
        // if this fires, then we should bring back CS654
        ASSERT(!(swt.Sym()->isMETHSYM() && (expected == SK_LOCVARSYM || expected == SK_MEMBVARSYM)));
        compiler()->Error(tree, ERR_BadSKknown, swt, ErrArgSymKind(swt.Sym()), expected);
    } else {
        compiler()->Error(tree, ERR_BadSKunknown, swt, ErrArgSymKind(swt.Sym()));
    }
}


// try to bind to a constant field.
// returns true on success.
bool FUNCBREC::bindUnevaledConstantField(MEMBVARSYM * field)
{
    ASSERT(field->isUnevaled);
    ASSERT(!pMSym);

    // Save and restore state.
    FncBindState state(this);

    // the below might fail, in which case we will bind to a non-constant
    // expression, and eventually return such, which will cause us to
    // give an approporiate error...
    return compiler()->clsDeclRec.evaluateFieldConstant(pFOrigSym, field);
}

// this determines whether the expression as an object of a prop or field is an lvalue
bool __fastcall FUNCBREC::objectIsLvalue(EXPR * object)
{
    return (
        !object || // statics are always lvalues

        isThisPointer(object) || // the this pointer's fields or props are lvalues

        ((object->flags & EXF_LVALUE) && (object->kind != EK_PROP)) ||
        // things marked as lvalues have props/fields which are lvalues, with one exception:  props of structs
        // do not have fields/structs as lvalues

        !object->type->isStructOrEnum()
        // non-struct types are lvalues (such as non-struct method returns)
        );
}


void __fastcall FUNCBREC::AdjustMemberObj(BASENODE * tree, SymWithType swt, EXPR ** pobj, bool * pfConstrained)
{
    // Assert that the type is present and is an instantiation of the member's parent.
    ASSERT(swt.Type() && swt.Type()->getAggregate() == swt.Sym()->parent->asAGGSYM());

    if (pfConstrained)
        *pfConstrained = false;

    bool isStatic;

    switch (swt.Sym()->getKind()) {
    case SK_MEMBVARSYM:
        isStatic = swt.Field()->isStatic;
        break;
    case SK_EVENTSYM:
        isStatic = swt.Event()->isStatic;
        break;
    default:
        isStatic = swt.MethProp()->isStatic;
        break;
    }

    if (isStatic) {
        // Generate an error iff:
        // * There is an object.
        // * The object isn't implicit this.
        // * We're not in the weird E.M case where the object could be substituted with the type.
        if (!*pobj || ((*pobj)->flags & EXF_IMPLICITTHIS) && (*pobj)->kind == EK_LOCAL)
            ;
        else if ((*pobj)->flags & EXF_SAMENAMETYPE)
            ;
        else
            compiler()->ErrorRef(tree, ERR_ObjectProhibited, swt);
        *pobj = NULL;
        return;
    }

    if (swt.Sym()->isMETHSYM() && swt.Meth()->isCtor())
        return;

    if (!*pobj) {
        if (inFieldInitializer && !pMSym->isStatic && parentAgg == swt.Sym()->parent)
            compiler()->ErrorRef(tree, ERR_FieldInitRefNonstatic, swt); // give better error message for common mistake                                
        else if (m_pamiCur && !pMSym->isStatic && parentAgg == swt.Sym()->parent && parentAgg->IsStruct())
            compiler()->Error(tree, ERR_ThisStructNotInAnonMeth);
        else
            compiler()->ErrorRef(tree, ERR_ObjectRequired, swt);
        return;
    }

    TYPESYM *typeObj = (*pobj)->type;
    TYPESYM *typeTmp;

    if (typeObj->isNUBSYM() && (typeTmp = typeObj->asNUBSYM()->GetAts()) != NULL && typeTmp != swt.Type())
        typeObj = typeTmp;

    if (typeObj->isTYVARSYM() || typeObj->isAGGTYPESYM()) {
        AGGSYM *aggCalled = NULL;
        aggCalled = swt.Sym()->parent->asAGGSYM();
        ASSERT(swt.Type()->getAggregate() == aggCalled);

        if ((*pobj)->kind == EK_FIELD && !(*pobj)->asFIELD()->fwt.Field()->isAssigned && !swt.Sym()->isMEMBVARSYM() &&
            typeObj->isStructType() && !typeObj->isPredefined())
        {
            (*pobj)->asFIELD()->fwt.Field()->isAssigned = true;
        }

        if (pfConstrained &&
            (typeObj->isTYVARSYM() ||
                typeObj->isStructType() && swt.Type()->IsRefType() && swt.Sym()->IsVirtual()))
        {
            // For calls on type parameters or virtual calls on struct types (not enums),
            // use the constrained prefix.
            *pfConstrained = true;
        }

        EXPR * objNew = tryConvert(*pobj, swt.Type(), NOUDC);

        // This check ensures that we do not bind to methods in an outerclass
        // which are visible, but whose this pointer is of an incorrect type...
        // ... also handles case of calling an object method on a RefAny value.
        // WE don't give a great message for this, but it'll do.
        if (!objNew)
        {
            if (!(*pobj)->type->isSpecialByRefType())
                compiler()->Error(tree, ERR_WrongNestedThis, swt.Type(), (*pobj)->type);
            else
                compiler()->Error(tree, ERR_NoImplicitConv, (*pobj)->type, swt.Type());
        }
        *pobj = objNew;
    }
}


void FUNCBREC::verifyMethodArgs(EXPR * call)
{
    ASSERT(call->kind == EK_CALL || call->kind == EK_PROP);

    EXPR **argsPtr = call->getArgsPtr();
    SymWithType swt = call->GetSymWithType();
    METHPROPSYM * mp = swt.Sym()->asMETHPROPSYM();
    TypeArray * typeArgs = call->kind == EK_CALL ? call->asCALL()->mwi.TypeArgs() : NULL;

    int paramCount = mp->params->size;
    TypeArray * params = mp->params;
    int iDst = 0;
    bool markTypeFromExternCall = mp->isFMETHSYM() && mp->asFMETHSYM()->isExternal;

    if (mp->isFMETHSYM() && mp->asFMETHSYM()->isVarargs) {
        paramCount--; // we don't care about the vararg sentinel
    }

    bool argListSeen = false;

    EXPR ** indir = NULL;

    if (!*argsPtr) {
        if (mp->isParamArray) goto FIXUPPARAMLIST;
        return;
    }

    while (true) {
        // this will splice the optional arguments into the list
        if (argsPtr[0]->kind == EK_ARGLIST) {
            if (argListSeen) {
                compiler()->Error(argsPtr[0]->tree, ERR_IllegalArglist);
            }
            argsPtr[0] = argsPtr[0]->asBIN()->p1;
            argListSeen = true;
            if (!argsPtr[0]) break;
            continue;
        }

        if (argsPtr[0]->kind == EK_LIST) {
            indir = &(argsPtr[0]->asBIN()->p1);
        } else {
            indir = argsPtr;
        }

        if (indir[0]->type->isPARAMMODSYM()) {
            call->flags |= EXF_HASREFPARAM;
            if (paramCount) paramCount--;
            if (markTypeFromExternCall)
                SetExternalRef(indir[0]->type);
        } else if (paramCount) {
            if (paramCount == 1 && indir != argsPtr && mp->isParamArray) {
                // we arrived at the last formal, and we have more than one actual, so
                // we need to put the rest in an array...
                goto FIXUPPARAMLIST;
            }
            TYPESYM *substDestType = compiler()->getBSymmgr().SubstType(params->Item(iDst), swt.Type(), typeArgs);
            EXPR * rval = tryConvert(indir[0], substDestType);
            if (!rval) {
                // the last arg failed to fix up, so it must fixup into the array element
                // (we will be passing a 1 element array...)
                ASSERT(mp->isParamArray);
                goto FIXUPPARAMLIST;
            }
            ASSERT(rval);
            indir[0] = rval;
            paramCount--;
        }
        // note that destype might not be valid if we are in varargs, but then we won't ever use it...
        iDst++;

        if (indir == argsPtr) {
            if (paramCount && mp->isParamArray) {
                // we run out of actuals, but we still have formals, so this is an empty array being passed
                // into the last param...
                indir = NULL;
                goto FIXUPPARAMLIST;
            }
            break;
        }

        argsPtr = &(argsPtr[0]->asBIN()->p2);
    }

    return;

FIXUPPARAMLIST:

    // we need to create an array and put it as the last arg...
    TYPESYM * arrayType = compiler()->getBSymmgr().SubstType(mp->params->Item(mp->params->size - 1), swt.Type(), typeArgs);
    TYPESYM * elemType = arrayType->asARRAYSYM()->elementType();

    // Use an EK_ARRINIT even in the empty case so empty param arrays in attributes work.
    EXPRARRINIT * arrayInit = newExpr(NULL, EK_ARRINIT, arrayType)->asARRINIT();
    arrayInit->dimSizes = &(arrayInit->dimSize);

    if (!argsPtr[0] || !indir) {
        arrayInit->dimSize = 0;
        arrayInit->args = NULL;
        if (!argsPtr[0]) {
            argsPtr[0] = arrayInit;
        } else {
            argsPtr[0] = newExprBinop(NULL, EK_LIST, NULL, argsPtr[0], arrayInit);
        }
    } else {
        EXPR ** lastArg = argsPtr;
        unsigned count = 0;

        while (true) {
            if (argsPtr[0]->kind == EK_LIST) {
                indir = &(argsPtr[0]->asBIN()->p1);
            } else {
                indir = argsPtr;
            }

            count++;
            EXPR * rval = tryConvert(indir[0], elemType);
            ASSERT(rval);

            indir[0] = rval;

            if (indir == argsPtr) {
                break;
            }

            argsPtr = &(argsPtr[0]->asBIN()->p2);
        }

        arrayInit->dimSize = count;
        arrayInit->args = lastArg[0];
        lastArg[0] = arrayInit;
    }
}

// Binds a call to a method, return type is an error or an EXPRCALL.
//
// tree      - ParseTree for error messages
// object    - object to call method on
// pmwi      - Meth to bind to. This will be morphed when we remap to an override.
// args      - arguments
// exprFlags - Flags to put on the generated expr
//
EXPR * FUNCBREC::BindToMethod(BASENODE * tree, EXPR *object, MethWithInst mwi, EXPR *args, MemLookFlagsEnum flags)
{
    ASSERT(mwi.Sym() && mwi.Sym()->isMETHSYM() && (!mwi.Meth()->isOverride || mwi.Meth()->isHideByName));

    // Get the conditionals before mapping to an override.
    NAMELIST * list = compiler()->clsDeclRec.GetConditionalSymbols(mwi.Meth());

    bool fConstrained;
    PostBindMeth(tree, !!(flags & MemLookFlags::BaseCall), &mwi, &object, &fConstrained);

    TYPESYM * typeRet = NULL;
    if ((flags & (MemLookFlags::Ctor | MemLookFlags::NewObj)) == (MemLookFlags::Ctor | MemLookFlags::NewObj)) {
        typeRet = mwi.Type();
    } else {
        typeRet = compiler()->getBSymmgr().SubstType(mwi.Meth()->retType, mwi.Type(), mwi.TypeArgs());
    }

    EXPRCALL *expr = newExpr(tree, EK_CALL, typeRet)->asCALL();
    expr->mwi = mwi;
    expr->args = args;
    expr->object = object;

    // Set the return type and flags for constructors.
    if (flags & MemLookFlags::Ctor) {
        if (flags & MemLookFlags::NewObj) {
            expr->flags |= EXF_NEWOBJCALL | EXF_CANTBENULL;
        }
        else {
            ASSERT(expr->type == getVoidType());
        }
    }

    if (flags & MemLookFlags::BaseCall)
        expr->flags |= EXF_BASECALL;
    else if (fConstrained && object) {
        // Use the constrained prefix.
        expr->flags |= EXF_CONSTRAINED;
    }

    verifyMethodArgs(expr);
    compiler()->EnsureState(expr->type);

    if (list) {
        INFILESYM * infile = NULL;
        if (pMSym)
            infile = pMSym->getInputFile();
        else if (pFSym)
            infile = pFSym->getInputFile();
        if (list && infile) {
            for (;;) {
                if (infile->isSymbolDefined(list->name))
                    break;
                list = list->next;
                if (!list) {
                    // Turn this call into a no-op
                    return newExpr(tree, EK_NOOP, getVoidType());
                }
            }
        }
    }

    return expr;
}

EXPR * FUNCBREC::BindToProperty(BASENODE * tree, EXPR * object, PropWithType pwt, int bindFlags, EXPR * args, AGGTYPESYM * typeOther)
{
    ASSERT(pwt.Sym() && pwt.Sym()->isPROPSYM() && pwt.Type() && pwt.Prop()->getClass() == pwt.Type()->getAggregate());
    ASSERT(!args == !pwt.Prop()->isIndexer());
    ASSERT(pwt.Prop()->params->size == 0 || pwt.Prop()->isIndexer());
    ASSERT(!typeOther || !pwt.Prop()->isIndexer() && typeOther->getAggregate() == pwt.Prop()->retType->getAggregate());

    bool fConstrained;
    MethWithType mwtGet;
    MethWithType mwtSet;

    PostBindProp(tree, !!(bindFlags & BIND_BASECALL), pwt, &object, &fConstrained, &mwtGet, &mwtSet);

    TYPESYM * typeRet = compiler()->getBSymmgr().SubstType(pwt.Prop()->retType, pwt.Type());
    ASSERT(typeOther == typeRet || !typeOther);

    if (object && !object->isOK())
        return newError(tree, typeRet);

    // if we are doing a get on this thing, and there is no get, and
    // most imporantly, we are not leaving the arguments to be bound by the array index
    // then error...
    if (bindFlags & BIND_RVALUEREQUIRED) {
        if (!mwtGet) {
            if (typeOther)
                goto LReturnType;
            compiler()->ErrorRef(tree, ERR_PropertyLacksGet, pwt);
        } else if ((bindFlags & BIND_BASECALL) && mwtGet.Meth()->isAbstract) {
            // if the get exists, but is abstract, forbid the call as well...
            if (typeOther)
                goto LReturnType;
            compiler()->Error(tree, ERR_AbstractBaseCall, pwt);
        } else if (!compiler()->clsDeclRec.CheckAccess(mwtGet.Meth(), mwtGet.Type(), parentDecl, NULL)) {
            // if the get exists, but is not accessible, give an error.
            if (typeOther) {
LReturnType:
                return newExpr(tree, EK_CLASS, typeOther);
            }

            compiler()->ErrorRef(tree, ERR_InaccessibleGetter, pwt);
        }
    }

    EXPRPROP * rval = newExpr(tree, EK_PROP, typeRet)->asPROP();

    rval->pwtSlot = pwt;
    rval->mwtGet = mwtGet;
    rval->mwtSet = mwtSet;
    rval->args = args;
    rval->object = object;

    ASSERT(EXF_BASECALL == BIND_BASECALL);
    if (EXF_BASECALL & bindFlags)
        rval->flags |= EXF_BASECALL;
    else if (fConstrained && object) {
        // Use the constrained prefix.
        rval->flags |= EXF_CONSTRAINED;
    }

    if (rval->args)
        verifyMethodArgs(rval);

    if (mwtSet && objectIsLvalue(rval->object))
        rval->flags |= EXF_LVALUE;
    if (typeOther)
        rval->flags |= EXF_SAMENAMETYPE;

    if (rval->type)
        compiler()->EnsureState(rval->type);

    return rval;
}

// Construct the EXPR node which corresponds to a field expression
// for a given field and object pointer.
EXPR * FUNCBREC::BindToField(BASENODE * tree, EXPR * object, FieldWithType fwt, int bindFlags)
{
    ASSERT(fwt.Type() && fwt.Field()->getClass() == fwt.Type()->getAggregate());

    EXPR * expr;
    EXPR * objOrig = object;

    if (fwt.Field()->IsDeprecated()) {
        ReportDeprecated(tree, fwt);
    }

    TYPESYM * fieldType = compiler()->getBSymmgr().SubstType(fwt.Field()->type, fwt.Type());

    if (object && !object->isOK())
        return newError(tree, fieldType);

    AdjustMemberObj(tree, fwt, &object, NULL);
    if (fwt.Field()->isUnevaled && !bindUnevaledConstantField(fwt.Field())) {
        return newError(tree, fieldType);
    }

    CheckFieldUse(objOrig);

    if (bindFlags & BIND_RVALUEREQUIRED) {
        fwt.Field()->isReferenced = true;
    }

    if (fwt.Field()->isConst) {
        // Special enum rule: if we're evaluating an enumerator initializer, and the constant is any other enumerator,
        // it's treated as a constant of it's underlying type.
        if (!pMSym && pFSym && parentAgg->IsEnum() && fieldType->isEnumType()) {
            fieldType = fieldType->underlyingType();
        }

        expr = newExprConstant(tree, fieldType, fwt.Field()->constVal);
    } else {
        expr = newExpr(tree, EK_FIELD, fieldType);

        int jbitPar;
        if (object && (jbitPar = object->getOffset()) && object->type->isStructType() && !fwt.Field()->fixedAgg)
            expr->asFIELD()->offset = jbitPar + FlowChecker::GetIbit(compiler(), fwt);
        else
            expr->asFIELD()->offset = 0;

        expr->asFIELD()->fwt = fwt;
        expr->asFIELD()->object = object;
        ASSERT(BIND_MEMBERSET == EXF_MEMBERSET);
        expr->flags |= (bindFlags & BIND_MEMBERSET);
        if ((object && object->type->isPTRSYM()) || objectIsLvalue(object)) {
            expr->flags |= EXF_LVALUE;

            // Exception: a readonly field is not an lvalue unless we're in the constructor/static constructor appropriate
            // for the field.
            if (fwt.Field()->isReadOnly) {
                if (parentAgg == NULL ||
                    !pMSym || !pMSym->isCtor() ||
                    fwt.Field()->getClass() != parentAgg ||
                    pMSym->isStatic != fwt.Field()->isStatic ||
                    (object != NULL && !isThisPointer(object)) ||
                    m_pamiCur)
                {
                    expr->flags &= ~EXF_LVALUE;
                }
            }
        }
    }

    if (expr->type)
        compiler()->EnsureState(expr->type);

    if (expr->kind == EK_FIELD && fwt.Field()->fixedAgg) {
        ASSERT(fwt.Field()->getClass()->IsStruct());

        LOCVARSYM * local = NULL;
        bool needsFixing = !isFixedExpression(expr, &local);
        if (!objectIsLvalue(object)) {
            compiler()->Error(tree, ERR_FixedNeedsLvalue);
        }
        else if (needsFixing != !!(bindFlags & BIND_FIXEDVALUE)) {
            compiler()->Error(tree, !needsFixing ? ERR_FixedNotNeeded : ERR_FixedBufferNotFixed);
        }

        if (!needsFixing && local) {
            // a local doesn't need fixing unless it might get hoisted into an anonymous method
            if (local->fUsedInAnonMeth) {
                if (local->nodeAnonMethUse)
                    compiler()->Error(tree, ERR_LocalCantBeFixedAndHoisted, local, ErrArgRefOnly(local->nodeAnonMethUse));
                else
                    compiler()->Error(tree, ERR_LocalCantBeFixedAndHoisted, local);
            }
            else {
                ASSERT(tree != NULL);
                if (local->nodeAddrTaken == NULL)
                    local->nodeAddrTaken = tree;
            }
        }

        // Change the field access into an EK_ADDR of the ((FixedBufferType)field).FixedElementField
        TYPESYM * type = expr->type;
        DWORD flags = expr->flags;
        NAME * pnNestedField = compiler()->namemgr->GetPredefName(PN_FIXEDELEMENT);

        expr->setType(fwt.Field()->fixedAgg->getThisType());
        DebugOnly(expr->asFIELD()->fCheckedMarshalByRef = true);

        MEMBVARSYM * nestedField = compiler()->getBSymmgr().LookupAggMember(pnNestedField, fwt.Field()->fixedAgg, MASK_MEMBVARSYM)->asMEMBVARSYM();
        ASSERT(nestedField && nestedField->type->isSimpleType());
        ASSERT(nestedField->type == type->asPTRSYM()->baseType());
        EXPRFIELD * fixedExpr = newExpr(tree, EK_FIELD, nestedField->type)->asFIELD(); // don't need to do substitution, since a it's a simple type
        DebugOnly(fixedExpr->fCheckedMarshalByRef = true);
        fixedExpr->fwt.Set(nestedField, expr->type->asAGGTYPESYM());
        fixedExpr->flags = flags;
        fixedExpr->object = expr;
        fixedExpr->offset = 0;

        EXPR * addr = newExprBinop(tree, EK_ADDR, type, fixedExpr, NULL);
        addr->flags |= (flags & ~EXF_LVALUE);
        ASSERT((addr->flags & EXF_LVALUE) == 0);
        expr = addr;
    }

    return expr;
}

// Construct the EXPR node which corresponds to an event expression
// for a given event and object pointer.
EXPR * FUNCBREC::BindToEvent(BASENODE * tree, EXPR * obj, EventWithType ewt, int bindFlags)
{
    ASSERT(ewt.Type() && ewt.Event()->getClass() == ewt.Type()->getAggregate());

    EventWithType ewtOrig = ewt;
    EXPR * objOrig = obj;

    // If it is virtual, find a remap of the method to something more specific.  This
    // may alter where the method is found.
    if ((bindFlags & BIND_BASECALL) && obj)
        RemapToOverride(&ewt, obj->type);

    AdjustMemberObj(tree, ewt, &obj, NULL);

    CheckFieldUse(objOrig);

    if (ewtOrig.Event()->IsDeprecated())
        ReportDeprecated(tree, ewtOrig);
    else if (ewt.Event()->IsDeprecated())
        ReportDeprecated(tree, ewt);

    TYPESYM * typeEvt = compiler()->getBSymmgr().SubstType(ewt.Event()->type, ewt.Type());
    compiler()->EnsureState(typeEvt);

    if (obj && !obj->isOK())
        return newError(tree, typeEvt);

    EXPREVENT * expr = newExpr(tree, EK_EVENT, typeEvt)->asEVENT();
    expr->object = obj;
    expr->ewt = ewt;

    ASSERT(EXF_BASECALL == BIND_BASECALL);
    expr->flags = (EXF_BASECALL & bindFlags);

    return expr;
}


AGGTYPESYM * FUNCBREC::CheckForTypeSameName(NAMENODE * node, SYM * sym, NameCacheFlagsEnum flags)
{
    ASSERT(!(flags & NameCacheFlags::TypeSameName) || !(flags & NameCacheFlags::NoTypeSameName));

    if ((flags & NameCacheFlags::NoTypeSameName) || node->kind == NK_GENERICNAME)
        return NULL;

    TYPESYM * type;

    switch (sym->getKind()) {
    default:
        ASSERT(0);
        return NULL;
    case SK_LOCVARSYM:
        type = sym->asLOCVARSYM()->type;
        break;
    case SK_MEMBVARSYM:
        type = sym->asMEMBVARSYM()->type;
        break;
    case SK_PROPSYM:
        type = sym->asPROPSYM()->retType;
        break;
    }

    if (!type->isAGGTYPESYM())
        return NULL;
    if (flags & NameCacheFlags::TypeSameName)
        return type->asAGGTYPESYM();

    SYM * symSame = TypeBind::BindName(compiler(), node, contextForTypeBinding(),
        btfFlags | TypeBindFlags::SuppressErrors | TypeBindFlags::NoBogusCheck | TypeBindFlags::AvoidEnsureState);
    if (symSame != type) {
        storeFlagsInCache(node->pName, flags | NameCacheFlags::NoTypeSameName);
        return NULL;
    }

    storeFlagsInCache(node->pName, flags | NameCacheFlags::TypeSameName);
    return type->asAGGTYPESYM();
}

// Bind the unqualified name.  mask details the allowed return types
bool FUNCBREC::BindNameToSym(SymWithType * pswt, TypeArray ** ptypeArgs, TypeArray ** ptypes, AGGTYPESYM ** ptypeOther,
    NAMENODE *name, symbmask_t mask, int bindFlags)
{
    ASSERT(pswt && ptypeArgs && ptypes && ptypeOther);
    ASSERT(!(mask & MASK_AGGTYPESYM) == !(mask & MASK_NUBSYM));

    MemberLookup mem;
    AGGSYM * aggError = NULL; // The agg in which we found something useful for error reporting....

    NAME * ident = name->pName;
    TypeArray *typesMem = NULL;
    AGGTYPESYM *typeOther = NULL;
    NameCacheFlagsEnum ncf = NameCacheFlags::None;
    TypeArray *typeArgs;
    SYM * rval = NULL;

    pswt->Clear();

    // bind type parameters
    typeArgs = TypeBind::BindTypeArgs(compiler(), name, contextForTypeBinding(), btfFlags);
    ASSERT(typeArgs);

    if (name->flags & NF_NAME_MISSING) {
        ASSERT(compiler()->ErrorCount() > 0);
        return false;
    }

    if (!typeArgs->size) {
        rval = lookupCachedName(name, MASK_ALL, &typesMem, &ncf);
        if (rval) {
            pswt->Set(rval, TypeArray::Size(typesMem) == 1 ? typesMem->Item(0)->asAGGTYPESYM() : NULL);
            goto LHaveIt;
        }
    }



    ASSERT(ncf == NameCacheFlags::None);

    // We got a cache miss.

    // See if it is a member in the current agg or an outer class.
    // The idea is to then later report an error if the types of the "this" pointer don't match up.
    for (BAGSYM * cls = parentAgg; cls && cls->isAGGSYM(); cls = cls->asAGGSYM()->Parent()) {
        AGGTYPESYM *clsThisType = cls->asAGGSYM()->getThisType();
        if (!mem.Lookup(compiler(), clsThisType, NULL, parentDecl, ident, typeArgs->size, MemLookFlags::UserCallable | MemLookFlags::TypeVarsAllowed)) {
            if (!mem.SymFirst()) {
                if (!aggError && mem.HasIntelligentErrorInfo())
                    aggError = cls->asAGGSYM();
                continue;
            }
            // Report the ambiguity then just use the first one.
            mem.ReportErrors(name);
        }
        else
            mem.ReportWarnings(name);

        if (mem.SymFirst()->isAGGSYM()) {
            pswt->Set(mem.SymFirstAsAts(typeArgs), NULL);
            TypeBind::CheckConstraints(compiler(), name, pswt->Sym()->asAGGTYPESYM(), CheckConstraintsFlags::None);
        }
        else {
            typesMem = mem.GetAllTypes();
            pswt->Set(mem.SymFirst(), typesMem->size == 1 ? typesMem->Item(0)->asAGGTYPESYM() : NULL);
        }

        // Only store in the cache if the arity is zero.
        if (!typeArgs->size)
            storeInCache(name, ident, pswt->Sym(), typesMem);
        goto LHaveIt;
    }

    // We didn't find anything in the class search. Do a namespace search.
    {
        ASSERT(!pswt->Sym());

        TypeBind tb;
        SYM * symRes = tb.SearchNamespacesInst(compiler(), name, ident, typeArgs, parentDecl, contextForTypeBinding(), btfFlags);

        if (!symRes) {
            if (aggError) {
                // Deal with errors from the class search. Redo the lookup and report the errors.
                if (!mem.Lookup(compiler(), aggError->getThisType(), NULL, parentDecl, ident, typeArgs->size, MemLookFlags::UserCallable | MemLookFlags::TypeVarsAllowed)) {
                    mem.ReportErrors(name);
                    return false;
                }
                ASSERT(!"Why didn't we find a class error?");
            }

            if (!tb.HasIntelligentErrorInfo()) {
                // tb.ReportErrors can't give a good message so we try to.
                if (typeArgs->size && (rval = lookupCachedName(name, MASK_LOCVARSYM | MASK_TYVARSYM, &typesMem, &ncf)) != NULL)
                    ReportTypeArgsNotAllowedError(name, typeArgs->size, rval, ErrArgSymKind(rval));
                else
                    compiler()->Error(name, ERR_NameNotInContext, ident);
                return false;
            }
        }

        tb.ReportErrors(name, ident, NULL, typeArgs, &symRes);
        if (!symRes || symRes->isERRORSYM())
            return false;

        pswt->Set(symRes, NULL);
    }

    // Don't store the result in the cache if it is generic or if we're refactoring.
    // The former is because we can't cache the arity information. The latter is so we correctly map
    // using aliases                                         
    if (!typeArgs->size)
        storeInCache(name, name->pName, pswt->Sym(), NULL);

LHaveIt:
    ASSERT(typeArgs->size == 0 || pswt->Sym()->isMETHSYM() || pswt->Sym()->isAGGTYPESYM() || pswt->Sym()->isNUBSYM());

    switch (pswt->Sym()->getKind()) {
    case SK_TYVARSYM:
        ASSERT(!typesMem || typesMem->size == 1);
        ASSERT(!!pswt->Sym()->asTYVARSYM()->isMethTyVar == !typesMem);
        break;

    case SK_NUBSYM:
        ASSERT(!typesMem);
        ASSERT(typeArgs->size == 1 && typeArgs->Item(0) == pswt->Sym()->parent);
        break;

    case SK_AGGTYPESYM:
        // Binding to a using alias may map arity 0 to non-zero arity.
        ASSERT(!typesMem);
        ASSERT(typeArgs == pswt->Sym()->asAGGTYPESYM()->typeArgsThis || !typeArgs->size);
        break;

    case SK_NSAIDSYM:
        ASSERT(!typesMem);
        break;

    case SK_LOCVARSYM:
        ASSERT(!typesMem);
        if (mask & MASK_AGGTYPESYM)
            typeOther = CheckForTypeSameName(name, pswt->Sym(), ncf);
        break;

    case SK_PROPSYM:
        ASSERT(pswt->Type());
        if (mask & MASK_AGGTYPESYM)
            typeOther = CheckForTypeSameName(name, pswt->Sym(), ncf);
        break;

    case SK_MEMBVARSYM:
        ASSERT(pswt->Type());
        if ((mask & MASK_AGGTYPESYM) && !pswt->Sym()->asMEMBVARSYM()->isEvent)
            typeOther = CheckForTypeSameName(name, pswt->Sym(), ncf);
        break;

    case SK_EVENTSYM:
        ASSERT(pswt->Type());
        break;

    case SK_METHSYM:
        ASSERT(typeArgs->size == pswt->Sym()->asMETHSYM()->typeVars->size || !typeArgs->size);
        ASSERT(typesMem && typesMem->size >= 1);
        pswt->ats = NULL;
        break;

    default:
        ASSERT(!"bad kind");
        return false;
    }

    if (!(pswt->Sym()->mask() & mask)) {
        if (!typeOther || !(mask & MASK_AGGTYPESYM)) {
            ErrorBadSK(name, *pswt, mask, bindFlags);
            pswt->Clear();
            return false;
        }

        // Use the type.
        pswt->Set(typeOther, NULL);
        typesMem = NULL;
        typeOther = NULL;
    }

    *ptypeArgs = typeArgs;
    *ptypes = typesMem;

    if (typeOther)
        compiler()->EnsureState(typeOther);
    *ptypeOther = typeOther;

    if (pswt->Sym()->isTYPESYM())
        compiler()->EnsureState(pswt->Sym()->asTYPESYM());

    return true;
}


void FUNCBREC::ReportTypeArgsNotAllowedError(BASENODE * node, int arity, ErrArgRef argName, ErrArgRef argKind)
{
    // See if this might be because of the generic ambiguity issue: F(a, b, c<d, e>(f), g)
    if (arity <= 1 || !node || !node->pParent) {
LNormal:
        compiler()->ErrorRef(node, ERR_TypeArgsNotAllowed, argName, argKind);
        return;
    }

    BASENODE * nodePrev = node;
    BASENODE * nodeT = nodePrev->pParent;

    if (nodeT->kind == NK_DOT) {
        if (nodeT->asDOT()->p2 != nodePrev)
            goto LNormal;
        nodeT = (nodePrev = nodeT)->pParent; // Move up.
    }
    if (!nodeT || nodeT->kind != NK_CALL || nodeT->asCALL()->p1 != nodePrev)
        goto LNormal;

    // This name is the name part of a CALL. Make sure the entire call is itself
    // an argument to a call, new or deref.
    do {
        nodeT = (nodePrev = nodeT)->pParent; // Move up.
    } while (nodeT && nodeT->kind == NK_LIST);

    if (!nodeT)
        goto LNormal;

    switch (nodeT->kind) {
    default:
        goto LNormal;
    case NK_CALL:
        if (nodeT->asCALL()->p2 != nodePrev)
            goto LNormal;
        break;
    case NK_NEW:
        if (nodeT->asNEW()->pArgs != nodePrev)
            goto LNormal;
        break;
    case NK_DEREF:
        if (nodeT->asDEREF()->p2 != nodePrev)
            goto LNormal;
        break;
    }

    // Yup. Give more specific and verbose error.
    compiler()->ErrorRef(node, ERR_TypeArgsNotAllowedAmbig, argName, argKind);
}




// Bind the unqualified name.  mask details the allowed return types
EXPR * FUNCBREC::bindName(NAMENODE *name, symbmask_t mask, int bindFlags)
{
    // TYVARSYMs can't be bound to an expression.
    ASSERT(!(mask & MASK_TYVARSYM));

    EXPR      *thisExpr;
    TypeArray *typeArgs;
    TypeArray *types;
    AGGTYPESYM * typeOther;
    SymWithType swt;


    if (!BindNameToSym(&swt, &typeArgs, &types, &typeOther, name, mask, bindFlags))
    {
        return newError(name, NULL);
    }

    ASSERT(swt.Sym()->mask() & mask);

    EXPR * expr = NULL;

    // First handle the kinds that don't need "this".
    switch (swt.Sym()->getKind()) {
    case SK_NSAIDSYM:
        ASSERT(!types);
        expr = newExpr(name, EK_NSPACE, getVoidType());
        expr->asNSPACE()->nsa = swt.Sym()->asNSAIDSYM();
        goto LDone;

    case SK_NUBSYM:
    case SK_AGGTYPESYM:
        ASSERT(!types);
        expr = newExpr(name, EK_CLASS, swt.Sym()->asTYPESYM());
        goto LDone;

    case SK_LOCVARSYM:
        ASSERT(!types);
        expr = bindToLocal(name, swt.Sym()->asLOCVARSYM(), bindFlags);
        if (typeOther)
            expr->flags |= EXF_SAMENAMETYPE;
        goto LDone;

    default:
        break;
    }

    // Now bind "this" and handle the kinds that might need "this".
    thisExpr = bindThisImplicit(name);

    switch (swt.Sym()->getKind()) {
    case SK_PROPSYM:
        ASSERT(types && types->size == 1 && types->Item(0) == swt.Type());
        ASSERT(!swt.Prop()->isIndexer());
        if (typeOther && !swt.Prop()->isStatic &&
            (!thisExpr || !canConvert(thisExpr->type, swt.Type(), NOUDC)))
        {
            // Use the type.
            expr = newExpr(name, EK_CLASS, typeOther);
            break;
        }
        expr = BindToProperty(name, thisExpr, swt, bindFlags, NULL, typeOther);
        break;

    case SK_MEMBVARSYM:
        ASSERT(types && types->size == 1 && types->Item(0) == swt.Type());
        if (typeOther && !swt.Field()->isStatic &&
            (!thisExpr || !canConvert(thisExpr->type, swt.Type(), NOUDC)))
        {
            // Use the type.
            expr = newExpr(name, EK_CLASS, typeOther);
            break;
        }
        expr = BindToField(name, thisExpr, swt, bindFlags);
        if (typeOther)
            expr->flags |= EXF_SAMENAMETYPE;
        break;

    case SK_EVENTSYM:
        ASSERT(types && types->size == 1 && types->Item(0) == swt.Type());
        expr = BindToEvent(name, thisExpr, swt, bindFlags);
        break;

    case SK_METHSYM:
        ASSERT(types && types->size >= 1);
        expr = newExpr(name, EK_MEMGRP, compiler()->getBSymmgr().GetMethGrpType());
        expr->asMEMGRP()->nodeName = name;
        expr->asMEMGRP()->name = name->pName;
        expr->asMEMGRP()->typeArgs = typeArgs;
        expr->asMEMGRP()->sk = SK_METHSYM;
        expr->asMEMGRP()->typePar = NULL;
        expr->asMEMGRP()->mps = NULL;
        expr->asMEMGRP()->object = thisExpr;
        expr->asMEMGRP()->types = types;
        expr->asMEMGRP()->flags |= EXF_USERCALLABLE;
        break;

    default:
        VSFAIL("bad kind");
        return newError(name, NULL);
    }

LDone:
    ASSERT(expr);
    if (expr->type)
        checkUnsafe(name, expr->type);

    return expr;
}




EXPR * FUNCBREC::bindToLocal(BASENODE * tree, LOCVARSYM * local, int bindFlags)
{
    EXPR * expr;

    if (local->isConst) {
        expr = newExprConstant(tree, local->type, local->constVal);
    } else {
        {
            expr = MakeLocal(tree, local, !local->slot.isPinned && !local->isNonWriteable || (bindFlags & BIND_USINGVALUE));
            ASSERT(BIND_MEMBERSET == EXF_MEMBERSET);
            expr->flags |= (bindFlags & BIND_MEMBERSET);
        }
    }

    if ((bindFlags & BIND_RVALUEREQUIRED)) {
        ASSERT(local->slot.JbitDefAssg() || local->slot.hasInit || local->isConst);
        if (!local->slot.isReferenced) {
            local->slot.isReferenced = true;
            unreferencedVarCount--;
        }
    }

    // If we're inside an anonymous delegate block, we need to mark any
    // locals that get used so they can be moved from the local scope
    // to a heap allocated object for the delegate to access
    if (m_pamiCur && !local->isConst) {
        SYM * pScope = pCurrentScope;
        bool used = true;
        // Only mark this local if it comes from a scope outside
        // the delegate. So if we don't find it's parent between
        // pCurrentScope and the inner-most delegate scope mark it.
        while (pScope && pScope->isSCOPESYM()) {
            if (pScope == local->parent) {
                used = false;
                break;
            } else if ((pScope->asSCOPESYM()->scopeFlags & SF_KINDMASK) == SF_DELEGATESCOPE) {
                break;
            }
            pScope = pScope->parent;
        }
        if (used) {
            if (local->slot.isParam && local->slot.isRefParam) {
                // Illegal to use ref or out args in anonymous delegate blocks
                compiler()->Error(tree, ERR_AnonDelegateCantUse, local->name->text);
                return newError(tree, local->type);
            }

            // Mark it as used (unless the use is illegal)
            if (local->IsAssumedPinned())
                compiler()->Error(tree, ERR_LocalCantBeFixedAndHoisted, local, ErrArgRefOnly(local->nodeAddrTaken));
            else {
                ASSERT(tree != NULL);
                local->fUsedInAnonMeth = true;
                if (!local->nodeAnonMethUse)
                    local->nodeAnonMethUse = tree;
            }
        }
    }

    return expr;
}


inline bool __fastcall FUNCBREC::hasThisPointer()
{
    return pMSym && !pMSym->isStatic && this->thisPointer;
}


inline bool __fastcall FUNCBREC::isThisPointer(EXPR * expr)
{
    return expr->kind == EK_LOCAL && expr->asLOCAL()->local == thisPointer;
}


// Binds the actual this pointer of the method.
// In the EE this always returns the runtime this pointer,
// even in the case of anonymous methods
EXPR * FUNCBREC::bindRealThis(BASENODE * tree, int flags)
{
    LOCVARSYM * relevantThisPointer;
    relevantThisPointer = thisPointer;

    EXPR * thisExpr;
    if (!relevantThisPointer) {
        return NULL;
    }

    TYPESYM * typeThis = parentAgg->getThisType();

    if (parentAgg->isPredefAgg(PT_G_OPTIONAL))
        typeThis = compiler()->getBSymmgr().GetNubFromNullable(typeThis->asAGGTYPESYM());

    thisExpr = newExprCore(tree, EK_LOCAL, typeThis, flags, EK_LOCAL);
    thisExpr->asLOCAL()->local = relevantThisPointer;
    if (parentAgg->IsStruct()) {
        thisExpr->flags |= EXF_LVALUE;
    }
    if (m_pamiCur) {
        thisExpr->asLOCAL()->local->fUsedInAnonMeth = true;
        if (!thisExpr->asLOCAL()->local->nodeAnonMethUse)
            thisExpr->asLOCAL()->local->nodeAnonMethUse = tree;
    }

    return thisExpr;
}

// Return an expression that refers to the this pointer. Returns null
// if no this is available. See bindThis is error reporting is required.
// If this is an anonymous method in the EE, then bindThisCore
// will return an expr which corresponds to the field which holds the
// semantically correct this pointer, not the real this pointer
EXPR * FUNCBREC::bindThisCore(PBASENODE tree, int flags)
{
    EXPR * thisExpr;


    if (!thisPointer) {
        return NULL;
    }

    AGGSYM *root = parentAgg;

    if (m_pamiCur && root->IsStruct()) {
        // Anonymous methods cannot access the 'this' pointer inside structs
        return NULL;
    }


    thisExpr = bindRealThis(tree, flags);

    return thisExpr;
}


// Return an expression that refers to the this pointer. Prints appropriate
// error if this is not available. See bindThisExpr if no error is desired.
EXPR * FUNCBREC::bindThisExplicit(BASENODE * tree)
{
    EXPR * thisExpr = bindThisCore(tree, EXF_CANTBENULL);

    if (!thisExpr) {
        if (pMSym && pMSym->isStatic)
            compiler()->Error(tree, ERR_ThisInStaticMeth);
        else if (pMSym && !pMSym->isStatic && m_pamiCur && parentAgg->IsStruct())
            compiler()->Error(tree, ERR_ThisStructNotInAnonMeth);
        else
            compiler()->Error(tree, ERR_ThisInBadContext); // 'this' isn't available for some other reason.

        return newError(tree, parentAgg ? parentAgg->getThisType() : NULL);
    }

    ASSERT(!(thisExpr->flags & EXF_IMPLICITTHIS));

    return thisExpr;
}


// Creates a new scopes as a child of the current scope.  You should always call
// this instead of creating the scope by hand as this correctly increments the
// scope nesting order...
void FUNCBREC::createNewScope()
{
    SCOPESYM * rval = compiler()->getLSymmgr().CreateLocalSym(SK_SCOPESYM, NULL, pCurrentScope)->asSCOPESYM();
    rval->nestingOrder = pCurrentScope->nestingOrder + 1;
    pCurrentScope = rval;
}

void FUNCBREC::closeScope()
{
    pCurrentScope->tree = lastNode;
    pCurrentScope = pCurrentScope->parent->asSCOPESYM();
}


void FUNCBREC::initThisPointer()
{
    if (!pMSym->isStatic) {
        // Enter the this pointer:
        thisPointer = compiler()->getLSymmgr().CreateLocalSym(SK_LOCVARSYM, compiler()->namemgr->GetPredefName(PN_THIS), pOuterScope)->asLOCVARSYM();
        thisPointer->isThis = true;
        thisPointer->SetAccess(ACC_PUBLIC);
        thisPointer->type = parentAgg->getThisType();
        if (parentAgg->isPredefAgg(PT_G_OPTIONAL))
            thisPointer->type = compiler()->getBSymmgr().GetNubFromNullable(thisPointer->type->asAGGTYPESYM());
        thisPointer->slot.isParam = true;
        thisPointer->slot.hasInit = true;
        if (parentAgg->IsStruct()) {
            thisPointer->slot.isRefParam = true;
            if (pMSym->isCtor()) {
                thisPointer->slot.SetJbitDefAssg(uninitedVarCount + 1);
                uninitedVarCount += FlowChecker::GetCbit(compiler(), parentAgg->getThisType());
                thisPointer->slot.hasInit = false;
            }
        }
    } else {
        thisPointer = NULL;
    }

    outerThisPointer = thisPointer;
}


void FUNCBREC::addTyVarsToScope(SCOPESYM * scope)
{
    // First put the method type variables in the cache.
    for (int i = 0; i < pMSym->typeVars->size; i++) {
        TYVARSYM * var = pMSym->typeVars->ItemAsTYVARSYM(i);
        CACHESYM * sym = compiler()->getLSymmgr().CreateLocalSym(SK_CACHESYM, var->name, scope)->asCACHESYM();
        sym->sym = var;
        sym->types = NULL;
        sym->scope = scope;
    }
}


// Create declarations for method parameters
void FUNCBREC::DeclareMethodParameters(METHINFO * info, EXPR ** params)
{
    ASSERT(info->cpin == info->meth->params->size ||
        info->cpin == info->meth->params->size - 1 &&
            info->meth->params->Item(info->meth->params->size - 1) == compiler()->getBSymmgr().GetArglistSym());

    if (params)
        *params = NULL;

    for (int i = 0; i < info->cpin; i++) {
        PARAMETERNODE * node = info->rgpin[i].node;
        TYPESYM * type = info->meth->params->Item(i);

        LOCVARSYM * sym = declareParam(info->rgpin[i].Name(), type, 0, node ? node : pTree);

        if (params)
            newList(MakeLocal(NULL, sym, true), &params);
    }
}


// compile the code for a method
EXPR * FUNCBREC::bindMethod(METHINFO * info)
{
    addTyVarsToScope(pOuterScope);
    DeclareMethodParameters(info);
    return bindMethOrPropBody(pTree->asANYMETHOD()->pBody);
}

// binds a property accessor
EXPR *FUNCBREC::BindPropertyAccessor(METHINFO * info)
{
    if (pTree->kind != NK_PROPERTY && pTree->kind != NK_INDEXER) {
        // checkUnsafe() is called on the property in defineProperty(), so don't check the accessors.
        unsafeErrorGiven = true;
        DeclareMethodParameters(info);
        unsafeErrorGiven = false;

        return bindMethOrPropBody(pTree->asACCESSOR()->pBody);
    }

    // This is the case where a sealed property only implemented one accessor, so the compiler generated another
    // which should call the override.
    ASSERT(info->meth->swtSlot && info->meth->isFabricated && !info->meth->isStatic && info->meth->IsOverride());

    info->noDebugInfo = true;   // Don't generate debug info for this method, since there is no source code.

    EXPR * args = NULL;
    DeclareMethodParameters(info, &args);
    EXPR *obj = bindBase(NULL);

    MethWithInst mwi(info->meth->swtSlot.Meth(), info->meth->swtSlot.Type(), BSYMMGR::EmptyTypeArray());   
    RemapToOverride(&mwi, obj->type);

    if (mwi.Meth()->isAbstract) {
        compiler()->ErrorRef(NULL, ERR_UnimplementedAbstractMethod, info->meth->getClass(), mwi);
        return NULL;
    }

    TYPESYM * typeRet = compiler()->getBSymmgr().SubstType(mwi.Meth()->retType, mwi.Type(), mwi.TypeArgs());
    EXPRCALL *call = newExpr(info->meth->parseTree, EK_CALL, typeRet)->asCALL();
    call->mwi = mwi;
    call->args = args;
    call->object = obj;
    call->flags |= EXF_BASECALL;

    EXPRBLOCK * block = newExprBlock(NULL);
    if (info->meth->retType == getVoidType()) {
        // implicit return
        block->statements = MakeStmt(NULL, call);
        block->flags |= EXF_NEEDSRET;
    } else {
        // explicit return
        EXPRRETURN * returnExpr;
        returnExpr = newExpr(NULL, EK_RETURN, NULL)->asRETURN();
        returnExpr->object = call;
        block->statements = returnExpr;
    }

    return block;
}

// binds an event accessor
EXPR *FUNCBREC::bindEventAccessor(METHINFO * info)
{
    LOCVARSYM * param;
    EVENTSYM * event;
    BASENODE * tree = info->meth->parseTree;
    bool wasUnreferenced = false;

    //
    // declare single parameter called "handler".
    //
    ASSERT(info->cpin == 1 && info->rgpin[0].Name() == compiler()->namemgr->GetPredefName(PN_VALUE));
    param = declareParam(info->rgpin[0].Name(), info->meth->params->Item(0), 0 /* no REF or OUT flags */, pTree);

    if (info->meth->isAbstract || info->meth->isExternal)
        return NULL;            // in interface.

    // Get the associated event.
    event = info->meth->getEvent();

    if (!event->implementation) {
        if (pTree->kind != NK_ACCESSOR) {
            // This can happen in error cases.
            ASSERT(pTree->kind == NK_PROPERTY && (pTree->other & NFEX_EVENT));
            return NULL;
        }
        return bindMethOrPropBody(pTree->asACCESSOR()->pBody);
    }

    SymWithType swtImpl(event->implementation, parentAgg->getThisType());

    // The code for this is either:
    //    event = Delegate.Combine(event, handler)
    //    event = Delegate.Remove(event, handler)
    // wheren event is the member field or property, and handler is the argument.

    EXPR * op1, * op2;   // the two arguments to Combine/Remove

    // Should not be any user-provided code for this event.
    ASSERT(pTree->kind != NK_ACCESSOR);

    // Get current value of event.
    if (swtImpl.Sym()->isMEMBVARSYM()) {
        if (!swtImpl.Field()->isReferenced)
            wasUnreferenced = true;
        op1 = BindToField(tree, bindThisImplicit(tree), swtImpl);
    }
    else {
        op1 = BindToProperty(tree, bindThisImplicit(tree), swtImpl);
    }

    // get "handler" parameter
    op2 = MakeLocal(tree, param, false);
    param->slot.SetUsed(true);

    // Construct argument list from the two
    EXPR * args = newExprBinop(tree, EK_LIST, getVoidType(), op1, op2);

    // Find and bind the Delegate.Combine or Delegate.Remove call.
    EXPR * call = BindPredefMethToArgs(tree, (info->meth == event->methAdd) ? PN_COMBINE : PN_REMOVE,
        GetReqPDT(PT_DELEGATE), NULL, args);
    if (!call->isOK())
        return newError(tree, NULL);
    ASSERT(call->kind == EK_CALL);

    // Cast the result to the delegate type.
    EXPR * cast = mustCast(call, event->type);

    // assign results of the call back to the property/field
    EXPR * lvalue = swtImpl.Sym()->isMEMBVARSYM() ?
        BindToField(tree, bindThisImplicit(tree), swtImpl) :
        BindToProperty(tree, bindThisImplicit(tree), swtImpl);

    // Wrap it all in a statement and then a block, and add implicit return.
    EXPRBLOCK * block = newExprBlock(NULL);
    block->statements = MakeAssignment(tree, lvalue, cast);
    block->flags |= EXF_NEEDSRET;

    if (wasUnreferenced) {
        // Any references made while binding this event accessor is not considered a reference to the event field.
        swtImpl.Field()->isReferenced = false;
    }

    return block;
}

EXPR * FUNCBREC::bindIfaceImpl(METHINFO *info)
{

    ASSERT(info->cpin == 0);

    static const WCHAR szFakeParamNameTemplate[] = L"p$%d";

    info->noDebugInfo = true;   // Don't generate debug info for this method, since there is no source code.
    setUnsafe(true); // Don't generate unsafe warnings

    // declare parameters
    for (int i = 0; i < info->meth->params->size; i++) {
        NAME * name = CreateName(szFakeParamNameTemplate, i);
        declareParam(name, info->meth->params->Item(i), 0, NULL);
    }

    // The code for this is call of the real implementation

    EXPR * args = NULL;
    EXPR ** pargs = &args;
    bool hasRefParam = false;
    for (int i = 0; i < info->meth->params->size; i++) {
        NAME * name = CreateName(szFakeParamNameTemplate, i);
        EXPRLOCAL * expr = newExpr(NULL, EK_LOCAL, info->meth->params->Item(i))->asLOCAL();
        LOCVARSYM *local = compiler()->getLSymmgr().LookupLocalSym(
            name, pOuterScope, MASK_LOCVARSYM)->asLOCVARSYM();

        if (expr->type->isPARAMMODSYM()) {
            hasRefParam = true;
        }
        expr->local = local;
        expr->flags = EXF_LVALUE;
        if (!local->slot.isReferenced) {
            local->slot.isReferenced = true;
            unreferencedVarCount--;
        }
        local->slot.SetUsed(true);
        newList(expr, &pargs);
    }

    EXPR *obj = bindThisImplicit(NULL);
    AGGTYPESYM * methodInType = parentAgg->getThisType();
    METHSYM * meth = info->meth->asIFACEIMPLMETHSYM()->implMethod;

    methodInType = methodInType->FindBaseType(meth->getClass());
    ASSERT(methodInType && methodInType->getAggregate() == meth->getClass());

    ASSERT(info->meth->getClass() == parentAgg); // No need to substitute.

    TYPESYM *rtyp = info->meth->retType;

    EXPRCALL * call = newExpr(NULL, EK_CALL, rtyp)->asCALL();
    call->mwi.Set(meth, methodInType, info->meth->typeVars);
    call->object = obj;
    call->args = args;
    if (hasRefParam)
        call->flags |= EXF_HASREFPARAM;

    // Wrap it all in a block, figure out the return statement
    EXPRBLOCK * block = newExprBlock(NULL);
    if (rtyp == getVoidType()) {
        block->statements = MakeStmt(NULL, call);
        block->flags |= EXF_NEEDSRET;       // implicit return statement
    } else {

        // explicit return statement
        EXPRRETURN * returnExpr;
        returnExpr = newExpr(NULL, EK_RETURN, NULL)->asRETURN();
        returnExpr->object = call;
        block->statements = returnExpr;
    }

    resetUnsafe();
    return block;
}

// binds the body of a method or property accessor
// the FUNCBREC must already be set up with the this ptr and arguments
// declared.
EXPRBLOCK *FUNCBREC::bindMethOrPropBody(BLOCKNODE *body)
{
    if (!body) {
        // Abstract method or extreme parse error.
        return NULL;
    }

    EXPRBLOCK * block = bindBlock(body);

    CorrectAnonMethScope(block->scopeSymbol);

    return block;
}


// compile the code for a constructor, this also deals with field inits and
// synthetized bodies
EXPR * FUNCBREC::bindConstructor(METHINFO * info)
{
    EXPRSTMT * list = NULL;
    StmtListBldr bldr(&list);

    //
    // determine if we are calling this class or a base class
    // Note that the parse tree may be a CTORNODE for explicit constructors
    // or its a CLASSNODE for implicit constructors
    //
    ASSERT(pTree->kind == NK_CTOR || pTree->kind == NK_CLASS || pTree->kind == NK_STRUCT);
    bool isThisCall;
    bool hasFieldInits = false;
    if (pTree->kind == NK_CTOR) {
        // parser should never generate both flags
        ASSERT((pTree->other & (NFEX_CTOR_THIS | NFEX_CTOR_BASE)) != (NFEX_CTOR_THIS | NFEX_CTOR_BASE));

        isThisCall = !!(pTree->other & NFEX_CTOR_THIS);
    } else {
        // implicit constructors never call a this constructor
        isThisCall = false;
    }

    //
    // static constructors always generate field initializers
    //
    // non-static constructors generate field initializers
    // if we don't explicitly call another contructor
    // on this class
    //
    createNewScope();
    if ((pMSym->isStatic || !isThisCall) && !pMSym->isExternal) {
        // bind field inits
        BindInitializers(pMSym->isStatic, bldr);
        hasFieldInits = (list != NULL);
        if (pMSym->isStatic && pMSym->isCompilerGeneratedCtor() && !hasFieldInits) {
            closeScope();
            return NULL;
        }
    }

    //
    // declare the constructor arguments
    //
    // Note that this must come after initializers
    // are done, otherwise constructor arguments will be available
    // to field initializers. Oops.
    //
    DeclareMethodParameters(info);

    //
    // for compiler generated constructors of comimport classes
    //
    if (pMSym->isExternal) {
        return NULL;
    }

    // Error tolerance.
    if (pTree->kind == NK_CTOR && !pTree->asCTOR()->pBody)
        return NULL;

    //
    // generate the call to the base class constructor
    //
    if (!pMSym->isStatic) {
        if (parentAgg->IsStruct()) {
            if (pTree->kind == NK_CTOR) {
                CALLNODE * nodeCall = pTree->asCTOR()->pThisBaseCall;
                if (pTree->other & NFEX_CTOR_BASE) {
                    // Structs can never call a base constructor.
                    compiler()->Error(nodeCall, ERR_StructWithBaseConstructorCall, pMSym);
                    // bind the args and toss them.
                    bindExpr(nodeCall->p2, BIND_RVALUEREQUIRED | BIND_ARGUMENTS);
                }
                else if (pTree->other & NFEX_CTOR_THIS) {
                    if (!nodeCall->p2) {
                        // No args means default value.
                        EXPRZEROINIT * expr = newExpr(nodeCall, EK_ZEROINIT, parentAgg->getThisType())->asZEROINIT();
                        expr->p1 = bindThisImplicit(nodeCall);
                        bldr.Add(MakeStmt(nodeCall, expr));
                    }
                    else {
                        bldr.Add(createBaseConstructorCall(true));
                    }
                }
            }
        }
        else if (!parentAgg->baseClass && !isThisCall) {
            // We're compiling constructor for object
            ASSERT(parentAgg->isPredefAgg(PT_OBJECT));

            //
            // constructor call for object is attempting
            // to call a base class constrcutor ...
            //
            if ((pTree->kind == NK_CTOR) && (pTree->other & NFEX_CTOR_BASE)) {
                compiler()->ErrorRef(NULL, ERR_ObjectCallingBaseConstructor, parentAgg);
            }
        }
        else {
            // Generate constructor call for all classes
            EXPRSTMT * baseCall = createBaseConstructorCall(isThisCall);
            if (pTree->kind != NK_CTOR && baseCall)
                baseCall->flags |= EXF_NODEBUGINFO;
            bldr.Add(baseCall);
        }
    }

    for (AnonMethInfo * pami = m_pamiFirst; pami; pami = pami->pamiNext)
        pami->fInCtorPreamble = true;

    EXPRBLOCK * blockPreamble;

    if (list) {
        blockPreamble = newExprBlock(NULL);
        blockPreamble->statements = list;
        list = NULL;
        bldr.Init(&list);
        blockPreamble->flags |= EXF_CTORPREAMBLE;
        bldr.Add(blockPreamble);
    }
    else
        blockPreamble = NULL;

    EXPRBLOCK * blockBody;

    // if we got a body, bind it...
    if (pTree->kind == NK_CTOR) {
        blockBody = bindBlock(pTree->asCTOR()->pBody);
        bldr.Add(blockBody);
        ASSERT(blockBody->scopeSymbol);
    }
    else {
        // No user-supplied body, so no debug info please.
        // Unless we had some field initializers
        info->noDebugInfo = !hasFieldInits;
        blockBody = NULL;
    }

    // Correct the scopes on any anonymous methods in the base call, field initializers
    // and body.
    CorrectAnonMethScope(pCurrentScope);

    //
    // if we have a default constructor for a struct with no
    // members, then we must force an empty function to be generated
    // because folks will want to call it ...
    // Same for Object's default constructor
    //
    if (!list && pMSym->isCtor() && !pMSym->isStatic &&
        (parentAgg->IsStruct() ||
        (!isThisCall && !parentAgg->baseClass && pTree->kind != NK_CTOR)))
    {
        bldr.Add(newExpr(pTree, EK_RETURN, NULL)->asRETURN());
    }

    if (list && (list->kind != EK_BLOCK || list->stmtNext || m_pamiFirst)) {
        EXPRBLOCK * block = newExprBlock(NULL);
        block->statements = list;
        block->scopeSymbol = pCurrentScope;
        if (blockPreamble)
            blockPreamble->owningBlock = block;
        if (blockBody)
            blockBody->owningBlock = block;
        list = block;
    }

    closeScope();

    return list;
}


// compile the code for a destructor
//
EXPR * FUNCBREC::bindDestructor(METHINFO * info)
{
    ASSERT(pTree->kind == NK_DTOR);

    EXPRSTMT * list = NULL;
    StmtListBldr bldr(&list);

    //
    // for compiler generated constructors of comimport classes
    //
    if (pMSym->isExternal) {
        return NULL;
    }

    // Error tolerance.
    if (!pTree->asDTOR()->pBody)
        return NULL;

    //
    // call base class destructor if we have one
    //
    SYM * baseDestructor = NULL;
    AGGTYPESYM *typeDtorBase = parentAgg->baseClass;

    if (typeDtorBase) {
        //
        // find a destructor in a base class, ignoring everything
        // which doesn't look like a destructor
        //
        while ((baseDestructor = compiler()->clsDeclRec.findNextName(pMSym->name, &typeDtorBase, baseDestructor)) &&
               (!baseDestructor->isMETHSYM() ||
                (baseDestructor->asMETHSYM()->GetAccess() != ACC_PROTECTED && baseDestructor->asMETHSYM()->GetAccess() != ACC_INTERNALPROTECTED) ||
                baseDestructor->asMETHSYM()->params->size != 0 ||
                baseDestructor->asMETHSYM()->retType != getVoidType()))
        {
            // Nothing.
        }
    }

    if (baseDestructor) {
        EXPRTRY * tryExpr = newExpr(NULL, EK_TRY, NULL)->asTRY();

        tryExpr->flags |= EXF_ISFINALLY;

        finallyNestingCount ++;

        EXPR * body = bindBlock(pTree->asDTOR()->pBody, SF_TRYSCOPE);

        if (body) {
            ASSERT(body->asBLOCK()->scopeSymbol);
            CorrectAnonMethScope(body->asBLOCK()->scopeSymbol);
        }

        tryExpr->tryblock = body->asBLOCK();

        finallyNestingCount --;

        //
        // generate the this pointer from our first param
        //
        if (thisPointer) {
            EXPR * thisExpression = bindThisImplicit(pTree);

            //
            // create the call expression
            //
            EXPRCALL * baseCall;
            baseCall = newExpr(pTree, EK_CALL, getVoidType())->asCALL();
            baseCall->object = thisExpression;
            baseCall->mwi.Set(baseDestructor->asMETHSYM(), typeDtorBase, NULL);
            baseCall->args = NULL;
            baseCall->flags |= EXF_BASECALL;

            tryExpr->handlers = newExprBlock(NULL);
            tryExpr->handlers->asBLOCK()->statements = MakeStmt(pTree, baseCall, EXF_LASTBRACEDEBUGINFO);
            bldr.Add(tryExpr);
        }

    } else {
        EXPRBLOCK * body = bindBlock(pTree->asDTOR()->pBody);
        if (body) {
            ASSERT(body->asBLOCK()->scopeSymbol);
            CorrectAnonMethScope(body->asBLOCK()->scopeSymbol);
        }
        bldr.Add(body);
    }

    // wrap it all in a block...
    if (list && (list->kind != EK_BLOCK || list->stmtNext != NULL)) {
        EXPRBLOCK * block = newExprBlock(NULL);
        block->statements = list;
        list = block;
    }

    return list;
}


// Bind the given block by binding its statement list..
EXPRBLOCK * FUNCBREC::bindBlock(BASENODE * tree, int scopeFlags, SCOPESYM ** scope, SCOPESYM * scopeExisting)
{
    if (!scopeExisting)
        createNewScope();
    if (scope) *scope = pCurrentScope;
    pCurrentScope->scopeFlags = scopeFlags;

    SCOPESYM ** pSaved = NULL;
    SCOPESYM * oldScope = NULL;

    pCurrentBlock = newExprBlock(tree);
    pCurrentBlock->scopeSymbol = pCurrentScope;

    switch (scopeFlags & SF_KINDMASK) {
    case SF_CATCHSCOPE: pSaved = &pCatchScope; break;
    case SF_TRYSCOPE: pSaved = &pTryScope; break;
    case SF_FINALLYSCOPE: pSaved = &pFinallyScope; pCurrentScope->SetBlock(pCurrentBlock); break;
    case SF_SWITCHSCOPE: pSaved = &pSwitchScope; break;
    case SF_NONE: break;
    default:
        ASSERT(!"bad sf type");
    }

    if (pSaved) {
        oldScope = *pSaved;
        *pSaved = pCurrentScope;
    }

    STATEMENTNODE * stms = tree->kind == NK_BLOCK ? tree->asBLOCK()->pStatements : tree->asANYSTATEMENT();

    StmtListBldr bldr(&pCurrentBlock->statements);

    while (stms) {
        BindStatement(stms, bldr);
        stms = stms->pNext;
    }

    EXPRBLOCK * rval = pCurrentBlock;
    pCurrentBlock = rval->owningBlock;

    closeScope();

    if (pSaved) {
        *pSaved = oldScope;
    }

    SetNodeExpr(tree, rval);
    return rval;
}

// Bind an individial statement
void FUNCBREC::BindStatement(STATEMENTNODE *sn, StmtListBldr & bldr)
{
    if (bindCallback.IsStartStatementNode(sn)) {
        EXPRDELIM * delim = newExprDelim(sn, DELIM_START);
        bldr.Add(delim);
        bindCallback.DelimCreated(delim);
    }
    BindStatementWorker(sn, bldr);
    if (bindCallback.IsEndStatementNode(sn)) {
        EXPRDELIM * delim = newExprDelim(sn, DELIM_END);
        bldr.Add(delim);
        bindCallback.DelimCreated(delim);
    }
}

void FUNCBREC::BindStatementWorker(STATEMENTNODE *sn, StmtListBldr & bldr)
{
    BASENODE * tree = sn;
    EXPR * rval;

    SETLOCATIONNODE(sn);

    //  give one error per statement at most...
    unsafeErrorGiven = false;

AGAIN:

    if (!tree)
        return;

#if DEBUG
    POSDATA pd;
    {
        if (tree) {
            if (pMSym) {
                pMSym->getInputFile()->pData->GetSingleTokenPos(tree->tokidx, &pd);
            } else if (pFSym) {
                pFSym->getInputFile()->pData->GetSingleTokenPos(tree->tokidx, &pd);
            }
        }
    }
#endif

    // Anonymous methods and iterators don't like locals in scopes that
    // don't belong to blocks. We have an assert in declareVar, but we
    // want this one here to catch more cases.
    ASSERT(pCurrentScope == pCurrentBlock->scopeSymbol);

    lastNode = sn;

    switch (tree->kind) {
    case NK_BLOCK:
        bldr.Add(bindBlock(tree->asBLOCK()));
        return;
    case NK_LABEL:
        BindLabel(tree->asLABEL(), bldr);
        return;
    case NK_UNSAFE:
         BindUnsafe(tree->asUNSAFE(), bldr);
         return;
    case NK_CHECKED:
        BindChecked(tree->asCHECKED(), bldr);
        return;
    case NK_EMPTYSTMT:
        return;
    default:
        break;
    }

    switch (tree->kind) {
    default:
        // This expression is not a legal statement.
        compiler()->Error(tree, ERR_IllegalStatement);
        // We call bindExpr for refactoring and error reporting.
        rval = bindExpr(tree);
        return;

    case NK_NAME: case NK_GENERICNAME: case NK_DOT:
    case NK_OP: case NK_CALL: case NK_DEREF: case NK_BINOP: case NK_UNOP:
    case NK_NEW:
        rval = bindExpr(tree, BIND_RVALUEREQUIRED | BIND_STMTEXPRONLY);
        bldr.Add(SetNodeStmt(sn, MakeStmt(sn, rval)));
        return;

    case NK_EXPRSTMT:
        tree = tree->asEXPRSTMT()->pArg;
        goto AGAIN;

    case NK_BREAK:
        BindBreakOrContinue(tree, true, bldr);
        return;
    case NK_CONTINUE:
        BindBreakOrContinue(tree, false, bldr);
        return;
    case NK_YIELD:
        BindYield(tree->asYIELD(), bldr);
        return;
    case NK_RETURN:
        BindReturn(tree->asRETURN(), bldr);
        return;
    case NK_DECLSTMT:
        BindVarDecls(tree->asDECLSTMT(), bldr);
        return;
    case NK_IF:
        BindIf(tree->asIF(), bldr);
        return;
    case NK_DO:
        BindWhileOrDo(tree->asDO(), false, bldr);
        return;
    case NK_WHILE:
        BindWhileOrDo(tree->asWHILE(), true, bldr);
        return;
    case NK_FOR:
        BindFor(tree->asFOR(), bldr);
        return;
    case NK_GOTO:
        BindGoto(tree->asGOTO(), bldr);
        return;
    case NK_SWITCH:
        BindSwitch(tree->asSWITCH(), bldr);
        return;
    case NK_TRY:
        BindTry(tree->asTRY(), bldr);
        return;
    case NK_THROW:
        BindThrow(tree->asTHROW(), bldr);
        return;
    case NK_LOCK:
        BindLock(tree->asLOCK(), bldr);
        return;
    }
}


EXPRCONSTANT * FUNCBREC::VerifySwitchLabel(BASENODE * tree, TYPESYM * type, bool fGoto)
{
    ASSERT(tree);

    bool fNullable = type && type->isNUBSYM();
    if (fNullable)
        type = type->StripNubs();

    EXPR * rval = bindExpr(tree);

    if (fNullable && rval->isNull()) {
        rval = mustConvert(rval, compiler()->getBSymmgr().GetNubType(type));
        ASSERT(rval->kind == EK_ZEROINIT || !rval->isOK());
        // Use generic null.
        rval = bindNull(NULL);
    }
    else {
        if (!fGoto) {
            ASSERT(type);
            rval = mustConvert(rval, type);
        }
        else if (type) {
            FUNDTYPE ft = rval->type->fundType();
            if (ft > FT_I8 && ft != FT_REF) {
                // A float constant or something equally bad...
                compiler()->Error(tree, ERR_IntegralTypeValueExpected);
                return NULL;
            }

            EXPR * exprT = tryConvert(rval, type, NOUDC);
            if (!exprT) {
                exprT = mustCast(rval, type, NOUDC);
                if (!exprT->isOK())
                    return NULL;
                if (exprT->kind == EK_CONSTANT)
                    compiler()->Error(tree, WRN_GotoCaseShouldConvert, type);
            }
            rval = exprT;
        }

        if (rval->kind != EK_CONSTANT) {
            if (rval->isOK())
                compiler()->Error(tree, ERR_ConstantExpected);
            return NULL;
        }

        // Switch on U8 is treated like switch on I8
        if (rval->type->fundType() == FT_U8)
            rval->setType(GetReqPDT(PT_LONG));
    }

    return rval->asCONSTANT();
}


// bind a goto statement...
void FUNCBREC::BindGoto(EXPRSTMTNODE * tree, StmtListBldr & bldr)
{
    EXPRGOTO * expr = newExpr(tree, EK_GOTO, NULL)->asGOTO();

    if (tree->flags & NF_GOTO_CASE) {
        EXPRCONSTANT * key = NULL;
        if (tree->pArg)
            key = VerifySwitchLabel(tree->pArg, exprSwitchCur ? exprSwitchCur->arg->type : NULL, true);
        if (!pSwitchScope) {
            compiler()->Error(expr->tree, ERR_InvalidGotoCase);
            return;
        }
        if (!key && tree->pArg) {
            // Treat it like a break.
            EXPRLABEL * target = loopLabels->breakLabel;
            expr->label = target;
            expr->targetScope = target->scope;
        }
        else {
            // We don't check validity now, but during the def-use pass
            expr->flags |= EXF_GOTOCASE | EXF_UNREALIZEDGOTO;
            expr->labelName = getSwitchLabelName(key);
            expr->targetScope = pSwitchScope;
        }
    }
    else {
        // We don't check validity now, but during the def-use pass
        expr->labelName = tree->pArg->asNAME()->pName;
        expr->flags |= EXF_UNREALIZEDGOTO;
        m_fForwardGotos = true;
    }

    expr->currentScope = pCurrentScope;
    expr->prev = gotos;
    gotos = expr;

    bldr.Add(SetNodeStmt(tree, expr));
}

void FUNCBREC::BindUnsafe(LABELSTMTNODE * tree, StmtListBldr & bldr)
{
    if (!compiler()->options.m_fUNSAFE) {
        compiler()->Error(tree, ERR_IllegalUnsafe);
    }

    if (tree != NULL && info->unsafeTree == NULL)
        info->unsafeTree = tree;

    UNSAFESTATES oldUnsafe = unsafeState;

    setUnsafe(true);

    BindStatement(tree->pStmt, bldr);

    setUnsafeState(oldUnsafe);
}


void FUNCBREC::BindChecked(LABELSTMTNODE * tree, StmtListBldr & bldr)
{
    CHECKEDCONTEXT checked(this, !(tree->flags & NF_UNCHECKED));

    BindStatement(tree->pStmt, bldr);

    checked.restore(this);
}

// bind a label statement...
void FUNCBREC::BindLabel(LABELSTMTNODE * tree, StmtListBldr & bldr)
{
    NAME * ident = tree->pLabel->asNAME()->pName;

    LABELSYM * label = compiler()->getLSymmgr().LookupLocalSym(ident, pCurrentScope, MASK_LABELSYM)->asLABELSYM();

    if (label)
        compiler()->Error(tree, ERR_DuplicateLabel, ident, ErrArgRefOnly(label->labelExpr->tree));

    label = compiler()->getLSymmgr().CreateLocalSym(SK_LABELSYM, ident, pCurrentScope)->asLABELSYM();

    EXPRLABEL * lab = newExprLabel();
    lab->label = label;
    lab->tree = tree;
    label->labelExpr = lab;

    bldr.Add(SetNodeStmt(tree, lab));

    // store it in our list so that we know where to find unreferenced or
    // unreachable labels...
    newList(lab, &pUserLabelList);

    BindStatement(tree->pStmt, bldr);
}

// bind the initializers for fields (either static or instance based...)
void FUNCBREC::BindInitializers(bool isStatic, StmtListBldr & bldr)
{
    ASSERT(parentAgg == pMSym->getClass());
    AGGSYM * cls = parentAgg;

    // we will need this later...
    LOCVARSYM * thisPointer = this->thisPointer;
    DECLSYM * parentDeclOld = parentDecl;

    EXPRSTMT * stmtFirst = NULL;
    StmtListBldr bldrTmp(&stmtFirst);
    bool allAreZero = true;

    //
    // for all class members
    //
    FOREACHCHILD(cls, child)
        if (!child->isMEMBVARSYM())
            continue;

        MEMBVARSYM * memb = child->asMEMBVARSYM();

        // Only compile initializers that match the constructor's staticness
        // but not fixed buffers or const fields (unless they're const decimal
        // which must be inited at runtime)
        if (memb->isStatic != isStatic)
            continue;
        if (memb->isConst && !memb->type->isPredefType(PT_DECIMAL))
            continue;
        if (memb->fixedAgg || (memb->parseTree->flags & NF_VARDECL_ARRAY))
            continue;

        EXPR *item = NULL;

        UNSAFESTATES oldUnsafe = unsafeState;
        setUnsafe(memb->isUnsafe);
        unsafeErrorGiven = false;
        parentDecl = memb->containingDeclaration();

        checkUnsafe(memb->getParseTree(), memb->type);

        //
        // check for explicit initializer
        //
        if (memb->getBaseExprTree()) {
            ASSERT(!cls->IsStruct() || isStatic);
            //
            // we have an explicit field initializer
            //
            ASSERT(!memb->isUnevaled);
            BINOPNODE * nodeAsg = memb->getBaseExprTree()->asBINOP();

            // This gives us the lefthand side, ie the field...
            // Need to do this inline because we may be binding to a
            // constant decimal, which won't come out right in bindExpr
            //
            EXPR * op1;
            op1 = newExpr(nodeAsg->p1, EK_FIELD, memb->type);
            DebugOnly(op1->asFIELD()->fCheckedMarshalByRef = true);
            SetNodeExpr(nodeAsg->p1, op1);

            op1->asFIELD()->offset = 0;
            if (isStatic)
                op1->asFIELD()->object = 0;
            else
                op1->asFIELD()->object = bindThisImplicit(nodeAsg->p1);
            op1->asFIELD()->fwt.Set(memb, parentAgg->getThisType());
            op1->flags |= EXF_LVALUE;

            this->thisPointer = NULL;
            inFieldInitializer = true;
            // Hide the this pointer for the duration of the right side...

            EXPR * op2 = bindPossibleArrayInit(nodeAsg->p2, memb->type);

            inFieldInitializer = false;
            this->thisPointer = thisPointer;

            item = SetNodeExpr(nodeAsg, bindAssignment(nodeAsg, op1, op2));

            // optimize away default initializers
            // be sure to check p2 from item, and not op2 as there may be a cast
            if (compiler()->options.m_fOPTIMIZATIONS && (item->kind == EK_ASSG) &&
                item->asBIN()->p2->isZero(true))
            {
                // go ahead and append if static... statics are only optimized away if all initializers are zero
                if (!isStatic) {
                    ASSERT(item->asBIN()->p2->kind == EK_CONSTANT);
                    item = NULL;
                }
            }
            else {
                allAreZero = false;
            }
        }

        setUnsafeState(oldUnsafe);
        parentDecl = parentDeclOld;

        if (item) {
            //
            // if we generated an initializer, wrap it up in
            // a statement and add it to the list
            // Use the field node for the parse tree, for the first field in each list
            BASENODE *pNode = memb->getParseTree();
            if (pNode && pNode->pParent) {
                if (pNode->pParent->kind == NK_FIELD)
                    pNode = pNode->pParent;
                else if (pNode->pParent->kind == NK_LIST && pNode == pNode->pParent->asLIST()->p1 && pNode->pParent->pParent && pNode->pParent->pParent->kind == NK_FIELD)
                    pNode = pNode->pParent->pParent;
            }
            bldrTmp.Add(MakeStmt(pNode, item));
        }
    ENDFOREACHCHILD

    // the valid names available when we are binding instance constructors
    // is different for the initializers and the constructor body.
    //
    // this can lead to problems when we have a non-static field with
    // an initializer referencing static member and an instance constructor
    // with a parameter with the same name as the static member.
    //
    // The field initializer binds the name to the static field, then when we
    // want to declare the parameter the name cache already contains
    // a conflicting entry referencing the static member.
    //
    compiler()->getLSymmgr().RemoveChildSyms( pOuterScope, MASK_CACHESYM);

    // append items if they aren't all initialized to zero
    if (!allAreZero)
        bldr.AddList(stmtFirst);
}

//
// creates a constructor call expr
//
// does overload resolution and access checks on the constructor
//
// tree         - parse tree to do error reporting
// typ          - class whose constructor we're calling (GENERICS: perhaps an instantiated generic class)
// arguments    - the constructor's argument list
// isNew
//      true    - generate a call for a new<type> expression
//                return type is the type of the class being constructed
//      false   - generate a call to the constructor only - no new
//                return type is void
//
EXPR * FUNCBREC::createConstructorCall(BASENODE * tree, BASENODE * nodeCtor, AGGTYPESYM * type, EXPR * thisExpression, EXPR *args, MemLookFlagsEnum flags)
{
    // Only legal flags are EXF_NEWOBJCALL and EXF_BASECALL, but they can't both be set.
    ASSERT(!(flags & ~(MemLookFlags::NewObj | MemLookFlags::BaseCall)) && (~flags & (MemLookFlags::NewObj | MemLookFlags::BaseCall)));

    TYPESYM * typeRet = (flags & MemLookFlags::NewObj) ? type : getVoidType();
    MemberLookup mem;

    if (!mem.Lookup(compiler(), type, thisExpression, parentDecl, compiler()->namemgr->GetPredefName(PN_CTOR), 0, flags | MemLookFlags::Ctor)) {
        mem.ReportErrors(tree);
        return newError(tree, typeRet);
    }
    mem.ReportWarnings(tree);

    // Create the method group expression.
    EXPRMEMGRP * grp = newExprMemGrp(tree, nodeCtor, mem);

    EXPR * exprRes = BindGrpToArgs(tree, 0, grp, args);
    if (!exprRes->isOK())
        return newError(tree, typeRet);

    ASSERT(exprRes->type == typeRet);

    if (type->fundType() <= FT_LASTNUMERIC && args && args->kind != EK_LIST && args->type == type) {
        ASSERT(flags & MemLookFlags::NewObj);
        exprRes = exprRes->asCALL()->args;
    }

    return exprRes;
}


//
EXPRSTMT * FUNCBREC::createBaseConstructorCall(bool isThisCall)
{
    // check that the FNCBREC is setup the way we expect
    ASSERT(pMSym->getClass() == parentAgg);

    //
    // the class to call the base constructor on is
    // either our base class, or our own class
    //
    AGGTYPESYM * typeToCallConstructorOn;
    if (isThisCall) {
        typeToCallConstructorOn = parentAgg->getThisType();
    } else {
        typeToCallConstructorOn = parentAgg->baseClass;
    }

    //
    // Evaluate the arguments for our base constructor call.
    // this will do the right thing when we have no explicit
    // base constructor call: it will generate the same expr
    // as an argument list with no arguments
    //
    // NOTE: we remove the availability of the this pointer
    //       when evaluating the arguments since our base class
    //       hasn't been initialized yet
    //
    BASENODE * nodeMeth;
    EXPR *arguments = NULL;
    if (pTree->kind == NK_CTOR) {

        // Create a new scope so that the names in the args don't interfere w/ the body
        createNewScope();
        pCurrentScope->scopeFlags |= SF_ARGSCOPE;

        // check the parse tree is as expected
        ASSERT(!(pTree->asCTOR()->pThisBaseCall && !(pTree->other & (NFEX_CTOR_THIS | NFEX_CTOR_BASE))));
        ASSERT((pTree->other & (NFEX_CTOR_THIS | NFEX_CTOR_BASE)) != (NFEX_CTOR_THIS | NFEX_CTOR_BASE));
        ASSERT(!pTree->asCTOR()->pThisBaseCall || pTree->asCTOR()->pThisBaseCall->p1);

        LOCVARSYM * thisPointer = this->thisPointer;
        this->thisPointer = NULL;
        if (pTree->asCTOR()->pThisBaseCall)
        {
            arguments = bindExpr(pTree->asCTOR()->pThisBaseCall->p2, BIND_RVALUEREQUIRED | BIND_ARGUMENTS);
        }
        this->thisPointer = thisPointer;

        closeScope();

        nodeMeth = pTree->asCTOR()->pThisBaseCall;

    } else {
        // This case can happen with anonymous methods, iterators, or compiler-generated default constructors!
        arguments = NULL;
        nodeMeth = NULL;
    }

    //
    // generate the this pointer from our first param
    //
    ASSERT(thisPointer);
    EXPR * exprThis = bindThisImplicit(pTree);

    //
    // create the constructor call. Doing overload resolution and access checks.
    //
    EXPR *exprCall = createConstructorCall(pTree, nodeMeth, typeToCallConstructorOn, exprThis, arguments,
        isThisCall ? MemLookFlags::None : MemLookFlags::BaseCall);
    if (!exprCall->isOK())
        return NULL;

    if (isThisCall && parentAgg->IsStruct()) {
        exprCall->flags |= EXF_NEWSTRUCTASSG;
    }
    else {
        ASSERT(!isThisCall == !!(exprCall->flags & EXF_BASECALL));
    }

    //
    // check for single step recursive constructor calls
    //
    if ((exprCall->kind == EK_CALL) && (exprCall->asCALL()->mwi.Meth() == pMSym)) {
        compiler()->ErrorRef(NULL, ERR_RecursiveConstructorCall, pMSym);
    }

    //
    // wrap this all in a statement...
    //
    return MakeStmt(pTree, exprCall);
}


// bind a break or continue label...
void FUNCBREC::BindBreakOrContinue(BASENODE * tree, bool asBreak, StmtListBldr & bldr)
{
    EXPRGOTO * expr;
    EXPRLABEL * target;

    target = asBreak ? loopLabels->breakLabel : loopLabels->contLabel;
    if (!target) {
        compiler()->Error(tree, ERR_NoBreakOrCont);
        return;
    }
    // both break and continue are always forward jumps...
    expr = MakeGoto(tree, target);
    expr->targetScope = target->scope;
    expr->currentScope = pCurrentScope;

    expr->prev = gotos;
    gotos = expr;
    bldr.Add(SetNodeStmt(tree, expr));
}


// Bind the expression
EXPR * FUNCBREC::bindExpr(BASENODE *tree, int bindFlags)
{


#if DEBUG
    POSDATA pd;
    {
        if (tree && parentDecl) {
            parentDecl->getInputFile()->pData->GetSingleTokenPos(tree->tokidx, &pd);
        }
    }
#endif

    if (!tree) return NULL;

    // Remember if we are restriction to statement-expressions only. Don't propagate this value
    // to sub-expressions.
    symbmask_t typesMask = 0;
    symbmask_t methodMask = MASK_METHSYM;
    ASSERT(!(bindFlags & (BIND_METHODNOTOK | BIND_TYPEOK)));
    bool stmtExprOnly = !!(bindFlags & BIND_STMTEXPRONLY);
    bindFlags &= ~BIND_STMTEXPRONLY;

    lastNode = tree;

    int mods = tree->flags & (NF_PARMMOD_REF | NF_PARMMOD_OUT);

    if (mods & NF_PARMMOD_OUT) {
        bindFlags &= ~ BIND_RVALUEREQUIRED;
        bindFlags |= BIND_MEMBERSET;
    }

    EXPR * rval;
    switch(tree->kind) {
    case NK_ANONBLOCK:
         if (stmtExprOnly)
            compiler()->Error(tree, ERR_IllegalStatement);
        rval = bindAnonymousMethod( tree->asANONBLOCK());
        break;
    case NK_NEW:
        rval = bindNew(tree->asNEW(), stmtExprOnly);
        break;
    case NK_CONSTVAL:
        {
         if (stmtExprOnly)
            compiler()->Error(tree, ERR_IllegalStatement);
        CONSTVAL val = tree->asCONSTVAL()->val;
        TYPESYM * type = GetOptPDT(tree->PredefType());
        if (!type) // decimal could be missing
            return newError(tree, NULL);

        rval = newExprConstant(tree, type, val);
        rval->flags |= EXF_LITERALCONST;
        }
        break;
    case NK_CALL:
    case NK_DEREF:
    case NK_BINOP:
        if (stmtExprOnly && ! opCanBeStatement[tree->Op()])
            compiler()->Error(tree, ERR_IllegalStatement);

        rval = bindBinop(tree->asANYBINOP(), bindFlags);
        break;
    case NK_UNOP:
        if (stmtExprOnly && ! opCanBeStatement[tree->Op()])
            compiler()->Error(tree, ERR_IllegalStatement);

        rval = bindUnop(tree->asUNOP(), bindFlags);
        break;
    case NK_LIST: {
        rval = NULL;
        EXPR ** prval = &rval;
        if (stmtExprOnly)
            bindFlags |= BIND_STMTEXPRONLY;
        NODELOOP(tree, BASE, elem)
            EXPR * item = bindExpr(elem, bindFlags);
            newList(item, &prval);
        ENDLOOP;
        break;
                  }
    case NK_GENERICNAME:
    case NK_NAME:
        if (stmtExprOnly)
            compiler()->Error(tree, ERR_IllegalStatement);

        rval = bindName(tree->asANYNAME(), MASK_LOCVARSYM | MASK_MEMBVARSYM | MASK_PROPSYM | typesMask | methodMask, bindFlags);
        // since we could have bound to a constant expression, we do not
        // mark this as an lvalue here, but rather let bindName do it...
        break;
    case NK_ALIASNAME:
        if (stmtExprOnly)
            compiler()->Error(tree, ERR_IllegalStatement);
        rval = bindAliasName(tree->asALIASNAME());
        break;
    case NK_DOT:
        if (stmtExprOnly)
            compiler()->Error(tree, ERR_IllegalStatement);
        rval = bindDot(tree->asDOT(), MASK_MEMBVARSYM | MASK_PROPSYM | typesMask | methodMask, bindFlags);
        break;
    case NK_ARRAYINIT:
		compiler()->Error(tree, ERR_ArrayInitInBadPlace);
		bindArrayInit(tree->asARRAYINIT(), compiler()->getBSymmgr().GetArray(GetReqPDT(PT_OBJECT), 1), NULL);
		rval = newError(tree, NULL);
        break;
    case NK_OP:
        if (stmtExprOnly && ! opCanBeStatement[tree->Op()])
            compiler()->Error(tree, ERR_IllegalStatement);

        switch(tree->Op()) {
        case OP_NULL:
            rval = bindNull(tree);
            break;

        case OP_TRUE:
        case OP_FALSE:
            rval = newExprConstant(tree, GetReqPDT(PT_BOOL), ConstValInit(tree->Op() == OP_TRUE));
            rval->flags |= EXF_LITERALCONST;
            break;

        case OP_THIS:
            rval = bindThisExplicit(tree);
            break;

        case OP_ARGS:
            if (pMSym && pMSym->isVarargs) {
                AGGTYPESYM * atsArgHandle = GetOptPDT(PT_ARGUMENTHANDLE);
                if (atsArgHandle != NULL)
                    rval = newExprBinop(tree, EK_ARGS, atsArgHandle, NULL, NULL);
                else
                    rval = newError(tree, NULL);
            } else {
                compiler()->Error(tree, ERR_ArgsInvalid);
                rval = newError(tree, GetOptPDT(PT_ARGUMENTHANDLE));
            }
            break;

        case OP_BASE:
            compiler()->Error(tree, ERR_BaseIllegal);
            rval = newError(tree, NULL);
            break;

        default:
            ASSERT(compiler()->ErrorCount());
            rval = newError(tree, NULL);
            break;
        }
        break;
    case NK_PREDEFINEDTYPE:
    case NK_ARRAYTYPE:
    case NK_POINTERTYPE:
    case NK_NULLABLETYPE:
    case NK_NAMEDTYPE:
        {
            TYPESYM * type = bindType(tree->asANYTYPE());
            ASSERT(type);
            rval = newExpr(tree, EK_CLASS, type);
        }
        break;
    case NK_ARROW:
         if (stmtExprOnly)
            compiler()->Error(tree, ERR_IllegalStatement);
        rval = bindDot(tree->asARROW(), MASK_MEMBVARSYM | MASK_PROPSYM | methodMask, bindFlags);
        break;
    default:
        ASSERT(compiler()->ErrorCount() > 0 || !"unknown expression");
        return newError(tree, NULL);
    }

    if (mods) {
        if (!checkLvalue(rval, false))
            return SetNodeExpr(tree, rval->isOK() ? newError(tree, rval->type) : rval);

        // do not give this error if we also failed the lvalue check...
        if (rval->kind == EK_PROP && (rval->flags & EXF_LVALUE)) {
            compiler()->Error(tree, ERR_RefProperty);
        }
        CheckFieldRef(rval);
        noteReference(rval);
        rval->setType(compiler()->getBSymmgr().GetParamModifier(rval->type, !!(mods & NF_PARMMOD_OUT)));
    }

    if (rval->type) {
        checkUnsafe(tree, rval->type);
        compiler()->EnsureState(rval->type);
    }

    return SetNodeExpr(tree, rval);
}


EXPR * FUNCBREC::bindNull(BASENODE * tree)
{
    return newExprConstant(tree, compiler()->getBSymmgr().GetNullType(), ConstValInit());
}


/***************************************************************************************************
    Binds the ?? operator. This works for either nullables or reference types.
***************************************************************************************************/
EXPR * FUNCBREC::BindValOrDefOp(BASENODE * tree, EXPR * arg1, EXPR * arg2)
{
    NubInfo nin;
    TYPESYM * typeRes;
    EXPR * exprLeft;
    EXPR * exprT;
    EXPR * exprRight = arg2;
    EXPR * exprLeftConst = NULL;

    if (arg1->type->isNUBSYM()) {
        BindNubCondValBin(tree, arg1, NULL, nin);
        ASSERT(nin.fActive);
        exprLeft = nin.Val(0);
        ASSERT(exprLeft->type == arg1->type->StripNubs());
    }
    else if (arg1->type->IsRefType()) {
        ASSERT(!nin.fActive);
        exprLeftConst = arg1->GetConst();
        exprLeft = exprLeftConst ? arg1 : newExprWrap(arg1, TK_SHORTLIVED);
    }
    else if (arg1->isOK())
        return badOperatorTypesError(tree, arg1, arg2);
    else
        return newError(tree, NULL);

    ASSERT(nin.fActive == (exprLeft->type != arg1->type));

    // arg1 is of type A?* and arg2 is of type B. The rules are:
    // * if arg2 => A then typeRes = A
    // * else if arg2 => A?* then typeRes = A?*
    // * else if A => B then typeRes = B
    // * else error.
    if (!!(exprT = tryConvert(exprRight, typeRes = exprLeft->type)))
        exprRight = exprT;
    else if (nin.fActive && !!(exprT = tryConvert(exprRight, typeRes = arg1->type))) {
        exprLeft = mustConvert(nin.TmpVal(0), typeRes);
        exprRight = exprT;
    }
    else if (!!(exprT = tryConvert(exprLeft, typeRes = exprRight->type)))
        exprLeft = exprT;
    else if (arg1->isOK() && arg2->isOK())
        return badOperatorTypesError(tree, arg1, arg2, exprLeft->type);
    else
        return newError(tree, NULL);

    ASSERT(exprRight->type == typeRes && exprLeft->type == typeRes);

    if (nin.fActive)
        return BindNubOpRes(tree, typeRes, exprLeft, exprRight, nin);

    if (exprLeftConst)
        return !exprLeftConst->isNull() ? exprLeft : AddSideEffects(tree, exprRight, exprLeft, true, true);

    EXPRDBLQMARK * exprRes = newExpr(tree, EK_DBLQMARK, typeRes)->asDBLQMARK();
    exprRes->exprTest = arg1;
    exprRes->exprConv = exprLeft;
    exprRes->exprElse = exprRight;

    return exprRes;
}


/***************************************************************************************************
    Add the AnonMethInfo to the list in *ppamiHead. Note that we always add at the head of the
    list. Once we're done with the initial binding, we reverse the list. This simplifies list
    maintenance, yet we can still process things in source order.
***************************************************************************************************/
void FUNCBREC::AddAnonMethInfo(AnonMethInfo ** ppamiHead, AnonMethInfo * pami)
{
    ASSERT(pami && !pami->fSeen);
    pami->pamiNext = *ppamiHead;
    *ppamiHead = pami;
}

/*********************************************************************************************
Here's where we start anonymous methods.
Each METHSYM keeps a list of AnonMethInfo structs that contain information about
each anonymous method.  Currently no attempts is made to find duplicate anonymous methods.
Durings the bind stage we determine if the anonymous method uses any outer locals/parameters.
That determines the scope at which the anonymous method's delegate will be cached at and where
the actual method lives.  We start by setting the pScope to pOuterScope, and then rachet it
down to inner scopes as locals are used.  So that when the body of the anonymous method is
finished binding pScope is the outermost scope at which it can be cached/created.  At the
end of binding a method any anonymous methods still sitting in pOuterScope have to be manually
moved into the real outerscope (pOuterScope is the argument scope).  After binding the body
various conversion should be applied that determine the delegate type.  During post-bind the
anonymous method body is checked for accessing the <this> pointer and thus we finally have
all the information needed for the transformation.

The transformation stage creates all the needed global symbols (display classes, methods,
static cache fields, hoisted fields) and some of the local symbols (locals instances of the
display classes, local cached instantiated delegates, etc.).  As each block is entered, the
appropriate symbols are created as well as any initialization code (create the display class,
pass in any hoisted parameters, null-init the caches).  When each EK_ANONMETH expr is
reached during the expr tree walk it is changed into a test-and-init expression on the
cached delegate.  Once we've finished walking the expr tree, we're all done rewriting.
Iterator transformations, if any, happen at this point (so they never see EK_ANONMETH). Then
each of the nested classes and no-longer-anonymous methods each get compiled using the info
stored in the AnonMethInfo struct.

How and Where we cache:
 * Even though static methods are slower to invoke (through delegates) than instance methods
   we will make anonymous methods static if they don't use <this> or any locals.
 * If no locals are used the anonymous method is a private member of the user's class
 * If no locals are used, and <this> is not used, and the outer method does not have
   type parameters, then the delegate is cached as a static field of the user's class.
 * If caching scope is the same as the use scope for the anonymous method we don't bother
   to cache it and just recreate the delegate each time.
 * Otherwise we create a local variable of the delegate type in the caching scope and
   initialize it to null on entry of that scope.
 * If the anonymous method is cached (either as a static field or a local), then each use
   becomes a test-and-init-and-use.
*********************************************************************************************/
EXPR * FUNCBREC::bindAnonymousMethod(ANONBLOCKNODE * tree)
{
    AnonMethInfo * pami = NULL;

    if (pMSym == NULL) {
        compiler()->Error(tree, ERR_AnonMethNotAllowed);

        // Create arg scope (to be symmetric with non-error case)
        createNewScope();
    }
    else {
        // Create the method symbol, but don't attach it anyplace, yet
        pami = (AnonMethInfo *)compiler()->localSymAlloc.AllocZero(sizeof(AnonMethInfo));
        pami->tree = tree;
        pami->pamiOuter = m_pamiCur;

        if (pMSym->name == NULL) {
            StringBldrNrHeap str(allocator);
            MetaDataHelper hlpr;
            hlpr.GetExplicitImplName(pMSym, str);
            pami->name = CreateSpecialName(SpecialNameKind::AnonymousMethod, str.Str());
        }
        else {
            pami->name = CreateSpecialName(SpecialNameKind::AnonymousMethod, pMSym->name->text);
        }

        AddAnonMethInfo(m_pamiCur ? &m_pamiCur->pamiChild : &m_pamiFirst, pami);

        // Set the scope to create the cached delegate (and $locals) to the outermost possible,
        // then as variables are bound, this will ratchet down to the inner-most scope with
        // a used local
        pami->pScope = pOuterScope; // This is just a marker until we know the real scope

        ANONSCOPESYM * ascp = compiler()->getLSymmgr().CreateLocalSym(SK_ANONSCOPESYM, NULL, pCurrentScope)->asANONSCOPESYM();

        // We need to verify that there are names for every parameter
        // Introduce a new scope for those names (this scope will become the outerscope
        // of the delegate method)
        // Can't call createNewScope because we don't want this new scope to show up as a child
        pami->pArgs = compiler()->getLSymmgr().CreateLocalSym(SK_SCOPESYM, NULL, NULL)->asSCOPESYM();
        pami->pArgs->parent = pCurrentScope;
        pami->pArgs->nestingOrder = pCurrentScope->nestingOrder + 1;
        pCurrentScope = pami->pArgs;
        ascp->scope = pCurrentScope;

        // This will eventually be a top-level scope!
        pami->pArgs->scopeFlags |= SF_DELEGATESCOPE;

        pami->jbitMin = uninitedVarCount + 1;

        // Create the nested <this> pointer in case we need it later
        // Use a dummy type for now until we know the real type
        LOCVARSYM * nestedThis = addParam( compiler()->namemgr->GetPredefName(PN_THIS), getVoidType(), 0, pCurrentScope);
        nestedThis->slot.SetUsed(true);
        nestedThis->isThis = true;
        pami->thisPointer = nestedThis;
    }

    // -1 here as a token index indicates that the user did not specify any parameters
    // instead of tree->pArgs == NULL which means they specified an empty parameter list "()"
    // For anonymous methods with no user-specified parameters we leave this NULL so it will
    // match any delegate signature parameters (which is different than the delegate with no args)
    TypeArray * params = NULL;
    if (tree->iClose != -1) {
        // The user had some parameters
        ASSERT(!(tree->other & NFEX_METHOD_VARARGS) && !(tree->other & NFEX_METHOD_PARAMS) );

        // Pass in the outer method as a context so we get it's type parameters (we just re-use it's symbols)
        compiler()->clsDeclRec.DefineParameters(contextForTypeBinding(), tree->pArgs, false, &params, NULL);

        // Now add all of the declared method arguments to the scope
        int p = 0;
        NODELOOP(tree->pArgs, PARAMETER, param)
            TYPESYM * type = params->Item(p++);
            LOCVARSYM * arg = declareParam(param->pName->pName, type, 0, param, pCurrentScope);
            if (arg) {
                // Because we put the parameters at an inner scope, we have to put them in the outer cache to be found
                storeInCache(param, arg->name, arg, NULL, true);
            }
        ENDLOOP;
    }

    if (pami != NULL)
        pami->params = params;

    EXPR * rval = BindAnonymousMethodInner(tree, pami);

    if (pami != NULL)
        pami->jbitLim = uninitedVarCount + 1;

    closeScope();

    return rval;
}

EXPR * FUNCBREC::BindAnonymousMethodInner(ANONBLOCKNODE * tree, AnonMethInfo * pami)
{
    EXPR * rval = NULL;

    // Save the scopes so we don't think we're inside anything (because we aren't when inside the AM block)
    SCOPESYM * pOuterTryScope = pTryScope;
    SCOPESYM * pOuterCatchScope = pCatchScope;
    SCOPESYM * pOuterFinallyScope = pFinallyScope;
    pCatchScope = pTryScope = pFinallyScope = pOuterScope;

    if (!pami) {
        // Bind the block as if it were inline'd code
        bindBlock(tree->pBody);
        // Then ignore it
        rval = newError(tree, NULL);
    }
    else {
        ASSERT(m_pamiCur == pami->pamiOuter);

        m_pamiCur = pami;

        createNewScope();

        // Bind the block as if it were inline'd code
        pami->pBodyExpr = bindMethOrPropBody(tree->pBody);

        ReachabilityChecker reach(compiler());
        reach.SetReachability(pami->pBodyExpr, true);
        pami->fHasRetAsLeave = reach.HasRetAsLeave();

        ASSERT(m_pamiCur == pami);
        m_pamiCur = pami->pamiOuter;

        pami->pBodyExpr->owningBlock = NULL;

        closeScope();

        rval = newExpr(tree, EK_ANONMETH, compiler()->getBSymmgr().GetAnonMethType());
        rval->asANONMETH()->pInfo = pami;
    }

    // Restore everything
    pTryScope = pOuterTryScope;
    pCatchScope = pOuterCatchScope;
    pFinallyScope = pOuterFinallyScope;

    return rval;
}

METHSYM * FUNCBREC::FindDelegateCtor(AGGTYPESYM * type, BASENODE * tree, bool fReportErrors)
{
    METHSYM * methCtor = NULL;
    TYPESYM * rgtype[2];
    TypeArray * ta;

    ASSERT(type->isDelegateType());

    rgtype[0] = compiler()->GetReqPredefType(PT_OBJECT);
    rgtype[1] = compiler()->GetReqPredefType(PT_INTPTR);
    ta = compiler()->getBSymmgr().AllocParams(2, rgtype);

    methCtor = FindPredefMeth(tree, PN_CTOR, type, ta, false);
    if (!methCtor) {
        rgtype[1] = compiler()->GetReqPredefType(PT_UINTPTR);
        ta = compiler()->getBSymmgr().AllocParams(2, rgtype);
        methCtor = FindPredefMeth(tree, PN_CTOR, type, ta, false);
        if (!methCtor && fReportErrors) {
            compiler()->Error(tree, ERR_BadDelegateConstructor, type->getAggregate());
        }
    }

    return methCtor;
}

EXPR * FUNCBREC::bindDelegateNew(AGGTYPESYM * type, NEWNODE * tree)
{
    ASSERT(type->isDelegateType());

    EXPR * expr;

    // First check to see if it's an anonymous method
    ASSERT(tree->pArgs);
    if (tree->pArgs->kind == NK_ANONBLOCK) {

        expr = bindAnonymousMethod(tree->pArgs->asANONBLOCK());
        if (!expr->isOK())
            goto LError;
        return mustConvert(expr, type);
    }

    // Next, find the function we're taking the address of:
    expr = bindMethodName(tree->pArgs);
    if (expr->isOK() && BindGrpConversion(tree, expr->asMEMGRP(), type, &expr, true))
        return expr;

LError:
    return newError(tree, type);
}


// bind the new keyword...
EXPR * FUNCBREC::bindNew(NEWNODE * tree, bool stmtExprOnly)
{
    EXPR * rval = NULL;
    EXPR * args;
    AGGTYPESYM * coclassType;
    TYPESYM * type = bindType(tree->pType);
    ASSERT(type);

    if (tree->flags & NF_NEW_STACKALLOC) {
        ASSERT(tree->pType->kind == NK_POINTERTYPE);
        ASSERT(type->isPTRSYM());
        rval = bindLocAlloc(tree, type->asPTRSYM());
        goto CHECKSTMTONLY;
    }

    // arrays are done separately...
    if (type->isARRAYSYM()) {
        rval = bindArrayNew(type->asARRAYSYM(), tree);
        goto CHECKSTMTONLY;
    }

    if (type->isDelegateType() && tree->pArgs != NULL) {
        // this could be a special new which takes a func pointer:
        rval = bindDelegateNew(type->asAGGTYPESYM(), tree);
CHECKSTMTONLY:
        if (stmtExprOnly) {
            compiler()->Error(tree, ERR_IllegalStatement);
        }

        return rval;
    }

    // first the args...
    args = bindExpr(tree->pArgs, BIND_RVALUEREQUIRED | BIND_ARGUMENTS);

    if (type->isERRORSYM())
        goto LERROR;

    if (type->isTYVARSYM()) {
        if (!type->asTYVARSYM()->FCanNew()) {
            compiler()->Error(tree, ERR_NoNewTyvar, type);
            goto LERROR;
        }
        if (args)
            compiler()->Error(tree, ERR_NewTyvarWithArgs, type);

        // The type variable has the new() constraint.
        return bindNewTyVar(tree, type->asTYVARSYM());
    }

    if (type->isNUBSYM()) {
        if (!args)
            return newExprZero(tree, type);

        if (args->kind == EK_LIST) {
            int carg = 1;
            for (EXPR * exprT = args; exprT && exprT->kind == EK_LIST; exprT = exprT->asBIN()->p2)
                carg++;
            compiler()->Error(tree, ERR_BadArgCount, type, carg);
            return newError(tree, type);
        }

        args = mustConvert(args, type->asNUBSYM()->baseType());
        if (!args->isOK())
            return newError(tree, type);
        return BindNubNew(tree, args);
    }

    if(!type->isAGGTYPESYM())
        goto LERROR;

    // Parameterless struct constructor calls == zero init
    if (!args && type->isStructOrEnum() && (!type->getAggregate()->hasNoArgCtor || type->isSimpleType())) {
        return newExprZero(tree, type);
    }

    coclassType = type->asAGGTYPESYM();

    if (type->getAggregate()->isAbstract) {
        if (type->isInterfaceType() && type->getAggregate()->comImportCoClass ) {
            AGGSYM * agg = type->getAggregate();
            if (agg->underlyingType) {
                // The coclass has already been resolved
                if (agg->underlyingType == agg->getThisType()) {
                    // This means the coclass was not imported, an error was already given
                    // when we first discovered this, so don't give another one
                    goto LERROR;
                }

                // Check accessibility.
                if (!compiler()->clsDeclRec.CheckTypeAccess(agg->underlyingType, parentDecl)) {
                    compiler()->Error(tree->pType, ERR_BadAccess, agg->underlyingType);
                    goto LERROR;
                }

                // just new the coclass instead of the interface
                coclassType = agg->underlyingType;
            } else {

                bool fIsInvalidSig;
                TYPESYM * type = compiler()->importer.ResolveFullMetadataTypeName(agg->GetModule(), agg->comImportCoClass, &fIsInvalidSig);

                if (type && type->isAGGTYPESYM()) {
                    agg->underlyingType = type->asAGGTYPESYM();

                    // Check accessibility.
                    if (!compiler()->clsDeclRec.CheckTypeAccess(type, parentDecl)) {
                        compiler()->Error(tree->pType, ERR_BadAccess, type);
                        goto LERROR;
                    }

                    // Check decprecated, constraints, etc.
                    TypeBind::CheckType(compiler(), tree->pType, type, parentDecl, btfFlags);

                    // CheckType doesn't do bogus.
                    if (compiler()->CheckBogus(type))
                        compiler()->ErrorRef(tree->pType, ERR_BogusType, type);

                    coclassType = type->asAGGTYPESYM();
                } else {
                    if (fIsInvalidSig)
                        compiler()->Error(tree->pType, ERR_BadCoClassSig, agg->comImportCoClass, agg);
                    else
                        compiler()->Error(tree->pType, ERR_MissingCoClass, agg->comImportCoClass, agg);
                    // Set the coclass to this, so we don't give more errors on this same interface
                    agg->underlyingType = agg->getThisType();
                    goto LERROR;
                }
            }
        }

        if (coclassType->getAggregate()->IsStatic()) {
            compiler()->Error(tree, ERR_InstantiatingStaticClass, type);
            goto LERROR;
        }
        if (coclassType->getAggregate()->isAbstract) {
            compiler()->Error(tree, ERR_NoNewAbstract, type);
            goto LERROR;
        }
    }

    // now find any of that classes constructors
    rval = createConstructorCall(tree, tree, coclassType, NULL, args, MemLookFlags::NewObj);
    if (coclassType != type)
        rval = mustCast(rval, type);

    return rval;

LERROR:
    return newError(tree, type);
}


EXPR * FUNCBREC::bindNewTyVar(NEWNODE * tree, TYVARSYM * var)
{
    ASSERT(var->FCanNew());

    if (var->IsValType())
        return newExprZero(tree, var);

    TypeArray * typeArgs = compiler()->getBSymmgr().AllocParams(1, (TYPESYM **)&var);

    EXPR * exprObj = BindPredefMethToArgs(NULL, PN_CREATEINSTANCE, GetOptPDT(PT_ACTIVATOR), NULL, NULL, typeArgs);
    if (!exprObj->isOK())
        return newError(tree, var);
    if (exprObj->type != var) {
        VSFAIL("Why didn't Activator.CreateInstance return the correct type?");
        exprObj = mustConvert(exprObj, var, NOUDC);
        if (!exprObj->isOK())
            return newError(tree, var);
    }

    if (var->IsRefType())
        return exprObj;

    // Produce T.default == null ? exprObj : T.default
    EXPR * exprTest = newExprBinop(tree, EK_EQ, GetReqPDT(PT_BOOL), bindNull(NULL),
        mustConvert(newExprZero(tree, var), GetReqPDT(PT_OBJECT), NOUDC));

    EXPR * exprColon = newExprBinop(tree, EK_BINOP, NULL, exprObj, newExprZero(tree, var));
    return newExprBinop(tree, EK_QMARK, var, exprTest, exprColon);
}


void FUNCBREC::bindArrayInitList(UNOPNODE * tree, TYPESYM * elemType, int rank, int * size, EXPR *** ppList, int * totCount, int * constCount,
    bool * pfSideEffects)
{
    int count = 0;

    NODELOOP(tree->p1, BASE, item)
        count++;
        if (rank == 1) {
            EXPR * expr = mustConvert(bindExpr(item), elemType);
            EXPR * exprConst = expr->GetConst();
            if (exprConst) {
                if (!exprConst->isZero(true)) {
                    (*constCount)++;
                }
                if (expr->hasSideEffects(compiler()))
                    *pfSideEffects = true;
            } else {
                (*totCount)++;
            }
            newList(expr, ppList);
        } else {
            if (item->kind == NK_ARRAYINIT && item->asARRAYINIT()->p1) {
                bindArrayInitList(item->asARRAYINIT(), elemType, rank - 1, size + 1, ppList, totCount, constCount, pfSideEffects);
            } else {
                compiler()->Error(item, ERR_InvalidArray);
            }
        }
    ENDLOOP;

    if (size[0] != -1) {
        if (size[0] != count) {
            compiler()->Error(tree, ERR_InvalidArray);
        }
    } else {
        size[0] = count;
    }
}


EXPR * FUNCBREC::bindArrayInit(UNOPNODE * tree, ARRAYSYM * type, EXPR * args)
{

    EXPRARRINIT * rval = newExpr(tree, EK_ARRINIT, type)->asARRINIT();
    rval->flags |= EXF_CANTBENULL;

    TYPESYM * elemType = type->elementType();

    int * pointer;
    int * pointerEnd;

    if (type->rank > 1) {
        rval->dimSizes = (int*) allocator->Alloc(SizeMul(type->rank, sizeof(int)));
    } else {
        rval->dimSizes = &(rval->dimSize);
    }
    pointer = rval->dimSizes;
    pointerEnd = pointer + type->rank;

    if (args) {
        EXPRLOOP(args, arg)
            if (pointer == pointerEnd) {
                compiler()->Error(arg->tree, ERR_InvalidArray);
                break;
            }
            if (arg->kind != EK_CONSTANT) {
                compiler()->Error(arg->tree, ERR_ConstantExpected);
                *pointer = -1;
            } else {
                *pointer = arg->asCONSTANT()->getVal().iVal;
            }
            pointer++;
        ENDLOOP;
        if (pointer != pointerEnd) {
            compiler()->Error(args->tree, ERR_InvalidArray);
        }
    }

    memset(pointer, -1, (pointerEnd - pointer) * sizeof (int));

    rval->args = NULL;
    EXPR ** pList = &(rval->args);

    bool constant = elemType->isSimpleType() && compiler()->getBSymmgr().GetAttrArgSize(elemType->getPredefType()) > 0;
    int totCount = 0; // the count of non-constants
    int constCount = 0; // the count of non-zero constants
    bool fSideEffects = false;

    bindArrayInitList(tree, elemType, type->rank, rval->dimSizes, &pList, &totCount, &constCount, &fSideEffects);

    if (rval->dimSizes[0] == 0 && type->rank > 1) {
        compiler()->Error(tree, ERR_InvalidArray);
    }

    if (constant && (constCount > 2) && (constCount * 3 >= totCount) && info != NULL && !compiler()->FEncBuild()) {
        rval->flags |= (totCount == 0) && !fSideEffects ? (EXF_ARRAYALLCONST | EXF_ARRAYCONST) : EXF_ARRAYCONST;
        if (!compiler()->getBSymmgr().methInitArray) {
            TYPESYM * rgtype[2];
            rgtype[0] = GetReqPDT(PT_ARRAY);
            rgtype[1] = GetReqPDT(PT_FIELDHANDLE);
            compiler()->getBSymmgr().methInitArray =
                FindPredefMeth(tree, PN_INITIALIZEARRAY, GetOptPDT(PT_RUNTIMEHELPERS), compiler()->getBSymmgr().AllocParams(2, rgtype));
        }
    }

    return SetNodeExpr(tree, rval);
}

// Check an expression for a negative constant value, and issue error/warning if so.
void FUNCBREC::checkNegativeConstant(BASENODE * tree, EXPR * expr, int id)
{
    if (expr->kind == EK_CONSTANT && expr->type->fundType() != FT_U8 && expr->asCONSTANT()->getI64Value() < 0)
        compiler()->Error(tree, id);
}


// bind an array creation expression...
EXPR * FUNCBREC::bindArrayNew(ARRAYSYM * type, NEWNODE * tree)
{
    EXPR * args = NULL;
    EXPR ** pArgs = &args;
    EXPR * temp, *expr;
    TYPESYM * destType;
    int count = 0;

    AGGTYPESYM * intType = GetReqPDT(PT_INT);

    NODELOOP(tree->pArgs, BASE, arg)
        count ++;
        expr = bindExpr(arg);
        if (expr && expr->isOK()) {
            destType = chooseArrayIndexType(tree, expr); // use int, long, uint, or ulong?
            if (!destType) {
                // using int as the type will allow us to give a better error...
                destType = intType;
            }
            temp = mustConvert(expr, destType);
            checkNegativeConstant(arg, temp, ERR_NegativeArraySize);

            if (destType != intType) {
                expr = newExpr(expr->tree, EK_CAST, destType);
                expr->asCAST()->flags |= EXF_INDEXEXPR;
                expr->asCAST()->p1 = temp;
            } else {
                expr = temp;
            }
        }

        newList(expr, &pArgs);
    ENDLOOP;

    if (tree->pInit)
        return bindArrayInit(tree->pInit->asARRAYINIT(), type, args);

    if (count != type->rank) {
        compiler()->Error(tree, ERR_BadIndexCount, type->rank);
    }

    if (args && args->isZero(false) && count == 1 && args->type == intType) {
        // For "new foo[0]" use EK_ARRINIT as if they did "new foo[0] { }".
        EXPRARRINIT * rval = newExpr(tree, EK_ARRINIT, type)->asARRINIT();
        rval->dimSizes = &(rval->dimSize);
        rval->dimSize = 0;
        rval->args = NULL;
        rval->flags |= EXF_CANTBENULL;
        return rval;
    }

    EXPR * rval = newExprBinop(tree, EK_NEWARRAY, type, args, NULL);
    rval->flags |= EXF_CANTBENULL;

    return rval;
}

EXPR * FUNCBREC::bindPossibleArrayInit(BASENODE * tree, TYPESYM * destinationType, int bindFlags)
{
    if (!tree)
        return NULL;

    if (tree->kind != NK_ARRAYINIT)
        return bindExpr(tree, bindFlags | BIND_RVALUEREQUIRED);

    if (destinationType->isARRAYSYM())
        return bindArrayInit(tree->asARRAYINIT(), destinationType->asARRAYSYM(), NULL);

    // Not an array type.
    compiler()->Error(tree, ERR_ArrayInitToNonArrayType);
    ARRAYSYM * typeT = compiler()->getBSymmgr().GetArray(GetReqPDT(PT_OBJECT), 1);
    bindArrayInit(tree->asARRAYINIT(), typeT, NULL);
    return newError(tree, destinationType);
}

EXPR * FUNCBREC::bindPossibleArrayInitAssg(BINOPNODE * tree, TYPESYM * type, int bindFlags)
{
    if (!tree) {
        return NULL;
    }

    EXPR * op1 = bindExpr(tree->p1, BIND_MEMBERSET | (bindFlags & BIND_USINGVALUE));
    EXPR * op2 = bindPossibleArrayInit(tree->p2, type, bindFlags);
    if (!op1->isOK() || !op2->isOK())
        return SetNodeExpr(tree, newError(tree, type));

    return SetNodeExpr(tree, bindAssignment(tree, op1, op2));
}

// Bind a return statement...  Check that the value is of the correct type...
void FUNCBREC::BindReturn(EXPRSTMTNODE * tree, StmtListBldr & bldr)
{
    // First, see if inside an iterator or a finally block:
    if (!m_pamiCur) {
        if (info->IsIterator())
            compiler()->Error(tree, ERR_ReturnInIterator);
        if (!info->nodeRet)
            info->nodeRet = tree;
    }
    if (pFinallyScope != pOuterScope)
        compiler()->Error(tree, ERR_BadFinallyLeave);

    EXPR * arg = bindExpr(tree->pArg);
    TYPESYM * retType = NULL;

    if (arg && !arg->isOK())
        return;

    retType = (m_pamiCur || !pMSym) ? NULL : pMSym->retType;

    if (retType != NULL) {
        // Can't do this check if we're in an anonymous method with an unknown return type

        if (retType == getVoidType()) {
            if (arg) {
                compiler()->Error(tree, ERR_RetNoObjectRequired, pMSym);
                return;
            }
        } else {
            if (!arg) {
                compiler()->Error(tree, ERR_RetObjectRequired, retType);
                return;
            }

            checkUnsafe(tree->pArg, retType);
            arg = mustConvert(arg, retType);
        }
    }

    EXPRRETURN * rval;
    rval = newExpr(tree, EK_RETURN, NULL)->asRETURN();
    rval->object = arg;

    if (pTryScope != pOuterScope || pCatchScope != pOuterScope) {
        rval->currentScope = pCurrentScope;
        rval->flags |= EXF_ASLEAVE;
        if (finallyNestingCount)
            rval->flags |= EXF_ASFINALLYLEAVE;
        info->hasRetAsLeave = true;
    }

    if (m_pamiCur) {
        ASSERT(retType == NULL);
        // We're in an anonymous method block and we don't know the return type yet, so save this for later
        if (!m_pamiCur->pexprListRet)
            m_pamiCur->pexprListRet = &m_pamiCur->listReturns;
        newList(rval, &m_pamiCur->pexprListRet);
    }

    bldr.Add(SetNodeStmt(tree, rval));
}

// Bind a yield statement...  Check that the value is of the correct type...
void FUNCBREC::BindYield(EXPRSTMTNODE * tree, StmtListBldr & bldr)
{
    bool fYield = true;

    if (m_pamiCur) {
        // Can't have a yield inside an anonymous method
        compiler()->Error(tree, ERR_YieldInAnonMeth);
        fYield = false;
    }
    else if (!info->IsIterator()) {
        ASSERT(!info->yieldType);

        // First, check the location and set the yieldType
        if (pMSym->retType && pMSym->retType->isAGGTYPESYM() && pMSym->retType->getAggregate()->isPredefined) {
            switch (pMSym->retType->getAggregate()->iPredef) {
            case PT_IENUMERATOR:
            case PT_IENUMERABLE:
                // Non-generic
                info->yieldType = getPDOT();
                break;
            case PT_G_IENUMERABLE:
            case PT_G_IENUMERATOR:
                // The yield type is the first type-parameter of the generic return type
                ASSERT(pMSym->retType->asAGGTYPESYM()->typeArgsAll->size == 1);
                info->yieldType = pMSym->retType->asAGGTYPESYM()->typeArgsAll->Item(0);
                break;
            }
        }

        if (!info->yieldType) {
            compiler()->Error(pMSym->parseTree, ERR_BadIteratorReturn, pMSym, pMSym->retType);
            info->yieldType = getPDOT();
        }

        ASSERT(info->IsIterator());
        if (info->nodeRet)
            compiler()->Error(info->nodeRet, ERR_ReturnInIterator);
    }

    // Can't have a yield in a finally clause
    // Can't have a yield <expr> in any part of try statement that has a catch clause
    if (pFinallyScope != pOuterScope) {
        compiler()->Error(tree, ERR_BadYieldInFinally);
    } else if (insideTryOfCatch && tree->pArg != NULL) {
        compiler()->Error(tree, ERR_BadYieldInTryOfCatch);
    } else if (pCatchScope != pOuterScope && tree->pArg != NULL) {
        compiler()->Error(tree, ERR_BadYieldInCatch);
    } else if (finallyNestingCount && tree->pArg && fYield) {
        info->hasYieldAsLeave = true;
    }

    EXPR * arg = NULL;
    if (tree->pArg) {
        arg = bindExpr(tree->pArg);
        if (!arg->isOK())
            return;
        if (fYield) {
            checkUnsafe(tree->pArg, info->yieldType);
            arg = mustConvert(arg, info->yieldType);
        }
    }

    if (!fYield)
        return;

    EXPRRETURN * rval;
    rval = newExpr(tree, EK_RETURN, NULL)->asRETURN();
    rval->currentScope = pCurrentScope;
    rval->object = arg;
    rval->flags |= EXF_RETURNISYIELD;

    bldr.Add(SetNodeStmt(tree, rval));
}


void FUNCBREC::BindThrow(EXPRSTMTNODE * tree, StmtListBldr & bldr)
{
    EXPRTHROW * rval = newExpr(tree, EK_THROW, NULL)->asTHROW();

    if (tree->pArg) {
        EXPR * expr = bindExpr(tree->pArg);
        if (!expr->isOK())
            return;
        EXPR * exprT = tryConvert(expr, GetReqPDT(PT_EXCEPTION), NOUDC);
        if (!exprT)
            compiler()->Error(tree->pArg, ERR_BadExceptionType);
        else
            expr = exprT;
        rval->object = expr;
    } else {
        rval->object = NULL;
        // find out if we are enclosed in a catch scope...

        bool foundFinally = false;
        SCOPESYM * scope = pCurrentScope;
        do {
            if (scope->scopeFlags & SF_DELEGATESCOPE) {
                scope = NULL;
                break;
            }
            if (scope->scopeFlags & SF_CATCHSCOPE) break;
            if (scope->scopeFlags & SF_FINALLYSCOPE) {
                foundFinally = true;
            }
            scope = scope->parent->asSCOPESYM();
        } while (scope);

        if (!scope)
            compiler()->Error(tree, ERR_BadEmptyThrow);
        else if (foundFinally)
            compiler()->Error(tree, ERR_BadEmptyThrowInFinally);
    }

    bldr.Add(SetNodeStmt(tree, rval));
}

void FUNCBREC::BindTry(TRYSTMTNODE * tree, StmtListBldr & bldr)
{
    EXPRTRY * rval = newExpr(tree, EK_TRY, NULL)->asTRY();
    rval->handlers = NULL;
    StmtListBldr bldrHandlers(&rval->handlers);
    bool oldInsideTryOfCatch = insideTryOfCatch;

    if (!(tree->flags & NF_TRY_CATCH)) {
        finallyNestingCount ++;
    } else {
        insideTryOfCatch = true;
    }

    SCOPESYM * tryScope;
    if (tree->pBlock) {
        rval->tryblock = bindBlock(tree->pBlock, SF_TRYSCOPE, &tryScope);
    } else {
        // Ensure that the EXPRTRY is well-formed so that downstream code can assume so.
        rval->tryblock = newExprBlock(NULL);
        tryScope = NULL;
    }

    // Restore this now that we're 'outside' the try block
    insideTryOfCatch = oldInsideTryOfCatch;

    if (tree->flags & NF_TRY_CATCH) {
        NODELOOP(tree->pCatch, CATCH, handler)
            EXPRHANDLER * expr = newExpr(handler, EK_HANDLER, NULL)->asHANDLER();
            lastNode = handler;
            expr->param = NULL;

            SCOPESYM * scopeCatch = NULL;
            if (handler->pType) {
                lastNode = handler->pType;
                TYPESYM * type =  bindType(handler->pType->asANYTYPE());
                ASSERT(type);
                if (!type->isERRORSYM()) {
                    if (!canConvert(type, compiler()->GetReqPredefType(PT_EXCEPTION), NOUDC)) {
                        compiler()->Error(handler->pType, ERR_BadExceptionType);
                    } else {
                        expr->setType(type);
                        // need to check whether this would even be reachable...
                        STMTLOOP(rval->handlers, hand)
                            EXPRHANDLER * prevH = hand->asHANDLER();
                            if (prevH->type && canConvert(type, prevH->type, NOUDC)) {
                                compiler()->Error(handler->pType, ERR_UnreachableCatch, prevH->type);
                                break;
                            }
                        ENDLOOP;
                    }
                    if (handler->pName) {
                        unreferencedVarCount++;
                        lastNode = handler->pName;
                        createNewScope();
                        scopeCatch = pCurrentScope;
                        LOCVARSYM * var = declareVarNoScopeCheck(handler, handler->pName->pName, type);
                        if (var) {
                            expr->param = var;
                            var->slot.hasInit = true;
                            var->isCatch = true;
                        }
                    }
                }
            } else {
                if (compiler()->WrapNonExceptionThrows()) {
                    // need to check whether this would even be reachable...
                    STMTLOOP(rval->handlers, hand)
                        EXPRHANDLER * prevH = hand->asHANDLER();
                        if (prevH->type && prevH->type->isPredefType(PT_EXCEPTION)) {
                            compiler()->Error(handler, WRN_UnreachableGeneralCatch);
                            break;
                        }
                    ENDLOOP;
                }
                expr->setType(compiler()->GetReqPredefType(PT_OBJECT));
            }

            expr->handlerBlock = bindBlock(handler->pBlock, SF_CATCHSCOPE, NULL, scopeCatch);
            bldrHandlers.Add(SetNodeStmt(handler, expr));
        ENDLOOP;
    }
    else {
        // finally...
        finallyNestingCount--;

        if (tree->pCatch) {
            SCOPESYM* newscope = NULL;
            rval->handlers = bindBlock(tree->pCatch->asBLOCK(), SF_FINALLYSCOPE, &newscope);
            if (tryScope)
                tryScope->SetFinallyScope(newscope);
        }
        else
            rval->handlers = newExprBlock(NULL);

        rval->flags |= EXF_ISFINALLY;
    }

    bldr.Add(SetNodeStmt(tree, rval));
}

// Assumes pDest is at least 2 chars wide
static WCHAR * EscapeChar(__out_ecount(2) WCHAR * pDest, WCHAR src)
{
    switch (src) {
    case L'\0':
        *(pDest++) = L'\\';
        *(pDest++) = L'0';
        break;
    case L'\'':
        *(pDest++) = L'\\';
        *(pDest++) = L'\'';
        break;
    case L'\"':
        *(pDest++) = L'\\';
        *(pDest++) = L'\"';
        break;
    case L'\\':
        *(pDest++) = L'\\';
        *(pDest++) = L'\\';
        break;
    case L'\a':
        *(pDest++) = L'\\';
        *(pDest++) = L'a';
        break;
    case L'\b':
        *(pDest++) = L'\\';
        *(pDest++) = L'b';
        break;
    case L'\f':
        *(pDest++) = L'\\';
        *(pDest++) = L'f';
        break;
    case L'\n':
        *(pDest++) = L'\\';
        *(pDest++) = L'n';
        break;
    case L'\r':
        *(pDest++) = L'\\';
        *(pDest++) = L'r';
        break;
    case L'\t':
        *(pDest++) = L'\\';
        *(pDest++) = L't';
        break;
    case L'\v':
        *(pDest++) = L'\\';
        *(pDest++) = L'v';
        break;
    default:
        *(pDest++) = src;
        break;
    }

    return pDest;
}

// Generates a string equivalent of StringCchPrintfW("case '%s' (0x%X):", pchSrc, iVal)
// If this is a string, then we use double quotes to surround the string and leave off the hex value.
NAME* FUNCBREC::GenerateSwitchLabelName(EXPRCONSTANT * expr)
{
    bool fIsChar = expr->type->isPredefType(PT_CHAR);
    WCHAR *pchSrc = NULL;
    int cchSrc;

    WCHAR szHex[256];
    int cchHex = 0;
    if (fIsChar) {
        // 2 escaped characters and a null
        pchSrc = (WCHAR*) STACK_ALLOC_ZERO(WCHAR,3);
        EscapeChar( pchSrc, (WCHAR)expr->getVal().iVal);
        cchSrc = (int)wcslen(pchSrc);

        // calculate the hex value as well
        StringCchPrintfW(szHex, lengthof(szHex), L" (0x%X)", expr->getVal().iVal);
        cchHex = (int)wcslen(szHex);
    } else {
        pchSrc = expr->getSVal().strVal->text;
        cchSrc = (int)expr->getSVal().strVal->length;
    }

    WCHAR * pchLimSrc = pchSrc + cchSrc;
    WCHAR bufferW[256];
    const WCHAR szPre[] = L"case ";
    const WCHAR chColon = L':';
    WCHAR chQuote = (fIsChar ? L'\'' : L'\"');

    // sizeof and lengthof include the trailing null
    int cchPre = lengthof(szPre) - 1;
    int cchTot = 2 * cchSrc + cchPre + (fIsChar ? cchHex : 0) + 4; // 4 = 2*quote + colon + null

    WCHAR * prgch = ((unsigned)cchTot <= lengthof(bufferW) ? bufferW : (WCHAR*) allocator->Alloc(SizeMul(cchTot, sizeof(WCHAR))));
    WCHAR * pchDst = prgch;

    memcpy(pchDst, szPre, cchPre * sizeof(WCHAR));
    pchDst += cchPre;

    // append quote
    *(pchDst++) = chQuote;

    while (pchSrc < pchLimSrc) {
        WCHAR * pchPrev; pchPrev = pchDst;
        pchDst = EscapeChar(pchDst, *pchSrc++);
        ASSERT(pchDst - pchPrev <= 2);
    }

    // append quote
    *(pchDst++) = chQuote;

    if (fIsChar) {
        memcpy(pchDst, szHex, cchHex * sizeof(WCHAR));
        pchDst += cchHex;
    }

    // append colon
    *(pchDst++) = chColon;

    // null terminate
    *pchDst = 0;

    ASSERT(pchDst < prgch + cchTot);
    return compiler()->namemgr->AddString(prgch);
}

NAME * FUNCBREC::getSwitchLabelName(EXPRCONSTANT * expr)
{
    // This returns "default:" which relies on the : to guarantee uniqueness...
    if (!expr)
        return compiler()->namemgr->GetPredefName(PN_DEFAULT_CASE);

    if (expr->isNull())
        return compiler()->namemgr->AddString(L"case null:");

    if (expr->type->isPredefType(PT_STRING) || expr->type->isPredefType(PT_CHAR))
        return GenerateSwitchLabelName(expr);

    WCHAR bufferW[256];
    HRESULT hr;

    if (expr->type->isPredefType(PT_BOOL)) {
        // The space and the : below guarantee uniqueness...
        hr = StringCchPrintfW(bufferW, lengthof(bufferW), L"case %s:", expr->getVal().iVal ? L"true" : L"false");
    }
    else {
        // The space and the : below guarantee uniqueness...
        hr = StringCchPrintfW(bufferW, lengthof(bufferW), L"case %I64d:", expr->getI64Value());
    }

    ASSERT(SUCCEEDED(hr));
    return compiler()->namemgr->AddString(bufferW);
}


int FUNCBREC::compareSwitchLabels(const void *l1, const void *l2)
{
    EXPRSWITCHLABEL * e1 = *(EXPRSWITCHLABEL**)l1;
    EXPRSWITCHLABEL * e2 = *(EXPRSWITCHLABEL**)l2;

    // Eveything coming through here should have a type, and should either be a null or an integral constant.
    ASSERT(!e1->key || e1->key->kind == EK_CONSTANT && e1->key->type);
    ASSERT(!e2->key || e2->key->kind == EK_CONSTANT && e2->key->type);

    if (e1 == e2)
        return 0;

    // default goes last and null goes 2nd to last.
    if (!e1->key)
        return !!e2->key;
    if (!e2->key)
        return -1;

    if (e1->key->isNull())
        return !e2->key->isNull();
    if (e2->key->isNull())
        return -1;

    __int64 v1 = e1->key->asCONSTANT()->getI64Value();
    __int64 v2 = e2->key->asCONSTANT()->getI64Value();
    if (v1 < v2) return -1;
    if (v2 < v1) return 1;
    return 0;
}


void FUNCBREC::initForNonHashtableSwitch(BASENODE * tree)
{
    if (!compiler()->getBSymmgr().methStringEquals) {
        TYPESYM * rgtype[2];
        rgtype[0] = GetReqPDT(PT_STRING);
        rgtype[1] = rgtype[0];

        compiler()->getBSymmgr().methStringEquals =
            FindPredefMeth(tree, PN_OPEQUALITY, GetReqPDT(PT_STRING), compiler()->getBSymmgr().AllocParams(2, rgtype), true, MemLookFlags::Operator);
    }
}

void FUNCBREC::initForHashtableSwitch(BASENODE * tree, EXPRSWITCH * expr)
{

    if (!compiler()->getBSymmgr().atsDictionary) {
        TYPESYM * rgtype[2];

        rgtype[0] = GetReqPDT(PT_STRING);
        rgtype[1] = GetReqPDT(PT_INT);

        TypeArray * taDictInst = compiler()->getBSymmgr().AllocParams(2, rgtype);


        AGGSYM * aggDict = compiler()->GetOptPredefAgg(PT_G_DICTIONARY);
        ASSERT(aggDict && "This should have been checked before...");
        if (!aggDict) return;

        AGGTYPESYM * atsDict = compiler()->getBSymmgr().GetInstAgg(aggDict, taDictInst);
        if (!atsDict) return;

        rgtype[0] = GetReqPDT(PT_INT);

        compiler()->getBSymmgr().methDictionaryCtor =
            FindPredefMeth(tree, PN_CTOR, GetOptPDT(PT_G_DICTIONARY), compiler()->getBSymmgr().AllocParams(1, rgtype), false);
        if (!compiler()->getBSymmgr().methDictionaryCtor)
            return;


        TypeArray * taDict = aggDict->typeVarsThis;
        
        compiler()->getBSymmgr().methDictionaryAdd =
            FindPredefMeth(tree, PN_ADD, GetOptPDT(PT_G_DICTIONARY), taDict, false);
        if (!compiler()->getBSymmgr().methDictionaryAdd)
            return;

        rgtype[0] = taDict->Item(0);
        rgtype[1] = compiler()->getBSymmgr().GetParamModifier(taDict->Item(1), true);

        compiler()->getBSymmgr().methDictionaryTryGetValue =
            FindPredefMeth(tree, PN_TRYGETVALUE, GetOptPDT(PT_G_DICTIONARY), compiler()->getBSymmgr().AllocParams(2, rgtype), false);
        if (!compiler()->getBSymmgr().methDictionaryTryGetValue)
            return;

        compiler()->getBSymmgr().atsDictionary = atsDict;

    }

    expr->flags |= EXF_HASHTABLESWITCH;
}


void FUNCBREC::BindSwitch(SWITCHSTMTNODE * tree, StmtListBldr & bldr)
{
    EXPRSWITCH * rval = newExpr(tree, EK_SWITCH, NULL)->asSWITCH();
    rval->hashtableToken = 0;
    rval->nullLabel = NULL;

    rval->arg = bindExpr(tree->pExpr);

    bool isConstant = rval->arg->kind == EK_CONSTANT;
    CONSTVAL val;
    EXPRGOTO * constantGoto = NULL;
    if (isConstant) {
        val = rval->arg->asCONSTANT()->getSVal();
        constantGoto = newExpr(tree, EK_GOTO, NULL)->asGOTO();
        // This is the one place that a goto may jump to a label that is nested
        // deeper than the goto. When ResolveGoto attempts to determine whether
        // the goto is blocked by a finally, it will ASSERT in this case. To avoid
        // this issue (and for efficiency) we set GOTONOTBLOCKED here.
        constantGoto->flags |= EXF_GOTONOTBLOCKED;
    }

    TYPESYM * stringType = GetReqPDT(PT_STRING);
    TYPESYM * charType = GetReqPDT(PT_CHAR);

    TYPESYM * type = rval->arg->type;
    compiler()->EnsureState(type);
    TYPESYM * typeNonNub = type->StripNubs();
    compiler()->EnsureState(typeNonNub);

    if (type != stringType && typeNonNub->fundType() > FT_LASTINTEGRAL) {
        int cconvMatch = 0;
        int cconvMatchLift = 0;
        TYPESYM * typeDst = NULL;
        TYPESYM * typeDstLift = NULL;

        if (typeNonNub->isAGGTYPESYM() && typeNonNub->getAggregate()->hasConversion) {
            for (AGGTYPESYM * typeCur = typeNonNub->asAGGTYPESYM(); typeCur && typeCur->getAggregate()->hasConversion; typeCur = typeCur->GetBaseClass()) {
                AGGSYM * aggCur = typeCur->getAggregate();

                for (METHSYM * convCur = aggCur->convFirst; convCur; convCur = convCur->ConvNext()) {
                    ASSERT(convCur->params->size == 1);
                    ASSERT(convCur->getClass() == aggCur);

                    if (!convCur->isImplicit())
                        continue;

                    // Get the substituted src and dst types.
                    TYPESYM * typeFrom = compiler()->getBSymmgr().SubstType(convCur->params->Item(0), typeCur);
                    bool fLift = type->isNUBSYM() && !typeFrom->isNUBSYM();

                    TYPESYM * typeTo = compiler()->getBSymmgr().SubstType(convCur->retType, typeCur);
                    TYPESYM * typeToNonNub = typeTo->StripNubs();

                    if ((typeToNonNub->isNumericType() && typeToNonNub->fundType() <= FT_LASTINTEGRAL) || typeToNonNub == stringType || typeToNonNub == charType) {
                        if (fLift) {
                            cconvMatchLift++;
                            typeDstLift = typeTo;
                        }
                        else {
                            cconvMatch++;
                            typeDst = typeTo;
                        }
                    }
                }
            }
        }

        if (!cconvMatch) {
            cconvMatch = cconvMatchLift;
            typeDst = typeDstLift;
        }

        if (cconvMatch != 1) {
            compiler()->Error(tree->pExpr, ERR_IntegralTypeValueExpected);
            typeNonNub = type = compiler()->getBSymmgr().GetErrorSym();
        } else {
            rval->arg = mustConvert(rval->arg, typeDst);
            type = rval->arg->type;
            typeNonNub = type->StripNubs();
            compiler()->EnsureState(typeNonNub);
        }
    }

    if (typeNonNub == GetReqPDT(PT_BOOL) && compiler()->options.IsECMA1Mode()) {
        compiler()->Error(tree->pExpr, ERR_NonECMAFeature, ErrArgIds(IDS_FeatureSwitchOnBool));
    }

    if (typeNonNub != type) {
        // Make sure we can get the HasValue and GetValueOrDefault members (for ilgen).
        EnsureNubHasValue(tree);
        EnsureNubGetValOrDef(tree);
    }

    // ok, create labels for all case statements:

    SCOPESYM * prevSwitch = pSwitchScope;
    createNewScope();
    pSwitchScope = pCurrentScope;
    pCurrentScope->scopeFlags |= SF_SWITCHSCOPE;
    EXPRBLOCK * block = newExprBlock(NULL);
    block->scopeSymbol = pCurrentScope;
    pCurrentBlock = block;

    LOOPLABELS * prev = loopLabels;
    LOOPLABELS ll(this);
    loopLabels->contLabel = prev->contLabel;
    rval->breakLabel = loopLabels->breakLabel;

    int count = 0;
    NODELOOP(tree->pCases, CASE, codeLab)
        NODELOOP(codeLab->pLabels, BASE, caseLab)
            count++;
        ENDLOOP;
    ENDLOOP;

    if (type == stringType) {
        if (count >= 7 && compiler()->GetOptPredefType(PT_G_DICTIONARY) && !compiler()->FEncBuild()) {
            initForHashtableSwitch(tree, rval);
        } else {
            initForNonHashtableSwitch(tree);
        }
    }

    EXPRSWITCHLABEL ** labArray = (EXPRSWITCHLABEL**) allocator->Alloc(SizeMul(count, sizeof(EXPRSWITCHLABEL*)));
    rval->labels = labArray;

    EXPRSWITCHLABEL * labelList = NULL;
    StmtListBldr bldrLabels((EXPRSTMT **)&labelList);

    SwitchPusher sp(this, rval);

    NODELOOP(tree->pCases, CASE, codeLab)

        NAME * labelName = NULL;
        EXPRSWITCHLABEL * label = NULL;

        NODELOOP(codeLab->pLabels, BASE, caseLab)
            EXPRCONSTANT * key;
            if (caseLab->asCASELABEL()->p1) {
                key = VerifySwitchLabel(caseLab->asCASELABEL()->p1, type, false);
                if (!key) {
                    count--;
                    continue;
                }
            } else {
                rval->flags |= EXF_HASDEFAULT;
                key = NULL;
            }
            labelName = getSwitchLabelName(key);
            LABELSYM * sym = compiler()->getLSymmgr().LookupLocalSym(labelName, pCurrentScope, MASK_LABELSYM)->asLABELSYM();
            if (sym) {
                compiler()->Error(caseLab->asCASELABEL()->p1, ERR_DuplicateCaseLabel, labelName,
                    ErrArgRefOnly(sym->labelExpr->tree->asCASELABEL()->p1));
            } else {
                sym = compiler()->getLSymmgr().CreateLocalSym(SK_LABELSYM, labelName, pCurrentScope)->asLABELSYM();
            }
            label = newExpr(caseLab, EK_SWITCHLABEL, NULL)->asSWITCHLABEL();
            if (key && key->isNull()) {
                rval->nullLabel = label;
            }
            label->key = key;
            if (isConstant) {
                if (!key) {
                    if (!constantGoto->label) {
                        constantGoto->label = label;
                    }
                } else if (key->asCONSTANT()->isEqual(rval->arg->asCONSTANT())) {
                    constantGoto->label = label;
                }
                newList(label, &pUserLabelList);
            }
            label->label = sym;
            sym->labelExpr = label;
            *labArray = label;
            labArray ++;
            bldrLabels.Add(SetNodeStmt(caseLab, label));
        ENDLOOP;

        // Bind the statements even if there isn't a valid label.
        EXPRSTMT * stmtTmp = NULL;
        StmtListBldr bldrTmp(label ? &label->statements : &stmtTmp);

        STATEMENTNODE * stms = codeLab->pStmts;

        while (stms) {
            BindStatement(stms, bldrTmp);
            stms = stms ->pNext;
        }

        if (!label)
            continue;

        if (!label->statements)
            label->statements = newExpr(codeLab->pStmts, EK_NOOP, NULL)->asNOOP();
    ENDLOOP;

    if (type != stringType) {
        qsort(rval->labels, count, sizeof(EXPR*), &FUNCBREC::compareSwitchLabels);
    } else if (rval->flags & EXF_HASDEFAULT) {
        // have to move the default to the last position...
        for (int cc = 0; cc < count; cc++) {
            if (!rval->labels[cc]->key) {
                EXPRSWITCHLABEL * def = rval->labels[cc];
                int jj;
                for (jj = cc + 1; jj < count; jj++) {
                    rval->labels[jj - 1] = rval->labels[jj];
                }
                rval->labels[jj-1] = def;
                break;
            }
        }
    }

    rval->bodies = labelList;
    rval->labelCount = count;

    loopLabels = prev;
    closeScope();
    pSwitchScope = prevSwitch;

    ASSERT(block->statements == NULL);
    StmtListBldr bldrTmp(&block->statements);

    if (isConstant) {
        if (!constantGoto->label) {
            constantGoto->label = ll.breakLabel;
        }
        bldrTmp.Add(constantGoto);
    }
    bldrTmp.Add(SetNodeStmt(tree, rval));
    bldrTmp.Add(ll.breakLabel);
    pCurrentBlock = pCurrentBlock->owningBlock;

    bldr.Add(block);
}


void FUNCBREC::OnLockOrDisposeEnter(EXPR * arg)
{
    if (arg->kind == EK_LOCAL) {
        arg->asLOCAL()->local->isLockOrDisposeTargetCount++;
    }
}
void FUNCBREC::OnLockOrDisposeExit(EXPR * arg)
{
    if (arg->kind == EK_LOCAL) {
        ASSERT(arg->asLOCAL()->local->isLockOrDisposeTargetCount > 0);
        arg->asLOCAL()->local->isLockOrDisposeTargetCount--;
    }
}
void FUNCBREC::OnPossibleAssignmentToArg(BASENODE * tree, EXPR * arg)
{
    if (arg->kind == EK_LOCAL) {
        LOCVARSYM * local = arg->asLOCAL()->local;
        ASSERT(local->isLockOrDisposeTargetCount >= 0);
        if (local->isLockOrDisposeTargetCount) {
            compiler()->Error(tree, WRN_AssignmentToLockOrDispose, local);
        }
    }
}



void FUNCBREC::BindLock(LOOPSTMTNODE * tree, StmtListBldr & bldr)
{
    ASSERT(tree && tree->pExpr);
    EXPR * arg, * orig= arg = bindExpr(tree->pExpr);
    EXPR * wrap = NULL;
    EXPRTRY * tryExpr = NULL;

    if (arg->isOK()) {
        switch (arg->type->fundType()) {
        case FT_VAR:
            // Box first so we enter and exit the same object.
            arg = mustConvert(arg, getPDOT(), NOUDC);
            // Fall through.
        case FT_REF:
            wrap = newExprWrap(arg, TK_LOCK);
            wrap->flags |= EXF_WRAPASTEMP;
            arg = newExprBinop(arg->tree, EK_SAVE, arg->type, arg, wrap);

            tryExpr = newExpr(tree, EK_TRY, NULL)->asTRY();
            tryExpr->flags |= EXF_ISFINALLY;
            break;

        default:
            compiler()->Error(tree, ERR_LockNeedsReference, arg->type);
            arg = newError(arg->tree, NULL);
            break;
        }

    }

    finallyNestingCount ++;
    OnLockOrDisposeEnter(orig);

    EXPR * body = bindBlock(tree->pStmt, SF_TRYSCOPE | SF_LAZYFINALLY);

    OnLockOrDisposeExit(orig);
    finallyNestingCount --;

    if (!arg->isOK())
        return;

    tryExpr->tryblock = body->asBLOCK();

    EXPR * call = BindPredefMethToArgs(tree, PN_EXIT, GetOptPDT(PT_CRITICAL), NULL, wrap);
    if (!call->isOK())
        return;

    body = newExprBlock(tree);
    body->asBLOCK()->statements = MakeStmt(NULL, call, EXF_NODEBUGINFO);
    createNewScope();
    pCurrentScope->scopeFlags = SF_FINALLYSCOPE;
    body->asBLOCK()->scopeSymbol = pCurrentScope;
    pCurrentScope->SetBlock(body->asBLOCK());
    closeScope();

    tryExpr->handlers = body->asBLOCK();

    call = BindPredefMethToArgs(tree, PN_ENTER, GetOptPDT(PT_CRITICAL), NULL, arg);
    if (!call->isOK())
        return;

    bldr.Add(MakeStmt(tree, call));
    bldr.Add(SetNodeStmt(tree, tryExpr));
}

void FUNCBREC::BindUsingDecls(FORSTMTNODE * tree, StmtListBldr & bldr)
{
    createNewScope();
    pCurrentBlock = newExprBlock(tree);
    pCurrentBlock->scopeSymbol = pCurrentScope;

    if (tree->pInit) {
        DECLSTMTNODE * decls = tree->pInit->asDECLSTMT();
        ASSERT(decls->flags & NF_USING_DECL);
        EXPRSTMT * stmtFirst = NULL;
        StmtListBldr bldrTmp(&stmtFirst);
        BindVarDecls(decls, bldrTmp);
        bldrTmp.Init(&pCurrentBlock->statements);
        BindUsingDeclsCore(tree, stmtFirst, bldrTmp);
    } else {
        EXPR * expr = bindExpr(tree->pExpr);
        if (expr->isOK()) {
            OnLockOrDisposeEnter(expr);
            StmtListBldr bldrTmp(&pCurrentBlock->statements);
            BindUsingDeclsCore(tree, expr, bldrTmp);
            OnLockOrDisposeExit(expr);
        }
    }

    EXPRBLOCK * rval = pCurrentBlock;
    pCurrentBlock = rval->owningBlock;

    closeScope();

    bldr.Add(SetNodeStmt(tree, rval));
}

void FUNCBREC::BindUsingDeclsCore(FORSTMTNODE * tree, EXPR * exprFirst, StmtListBldr & bldr)
{
    EXPR * wrap = NULL;
    EXPR * exprNext = NULL;

    if (exprFirst->kind == EK_DECL) {
        exprNext = exprFirst->asSTMT()->stmtNext;
        exprFirst->asSTMT()->stmtNext = NULL;
        bldr.Add(exprFirst->asSTMT());
    }
    else if (!canConvert(exprFirst, GetReqPDT(PT_IDISPOSABLE), NOUDC)) {
        // We'll report the error later....
        wrap = exprFirst;
    }
    else {
        wrap = newExprWrap(exprFirst, TK_USING);
        wrap->flags |= EXF_WRAPASTEMP;
        wrap->asWRAP()->doNotFree = true;
        wrap->asWRAP()->needEmptySlot = true;
        bldr.Add(MakeStmt(tree, bindAssignment(tree, wrap, exprFirst)));
    }

    EXPRTRY * tryExpr = newExpr(tree, EK_TRY, NULL)->asTRY();

    tryExpr->flags |= EXF_ISFINALLY;

    finallyNestingCount ++;

    createNewScope();
    pCurrentScope->scopeFlags = SF_TRYSCOPE;
    pCurrentBlock = newExprBlock(tree);
    tryExpr->tryblock = pCurrentBlock;
    SCOPESYM * oldScope = pTryScope;
    pTryScope = pCurrentScope;
    pCurrentBlock->scopeSymbol = pCurrentScope;

    StmtListBldr bldrTmp(&pCurrentBlock->statements);

    if (exprNext)
        BindUsingDeclsCore(tree, exprNext, bldrTmp);
    else
        BindStatement(tree->pStmt, bldrTmp);
    bldrTmp.Clear();

    pCurrentBlock = pCurrentBlock->owningBlock;
    closeScope();
    pTryScope = oldScope;

    finallyNestingCount--;

    tryExpr->handlers = newExprBlock(tree);
    createNewScope();
    pCurrentScope->scopeFlags = SF_FINALLYSCOPE;
    tryExpr->handlers->asBLOCK()->scopeSymbol = pCurrentScope;
    pCurrentScope->SetBlock(tryExpr->handlers->asBLOCK());
    closeScope();

    EXPR * object = wrap ? wrap : MakeLocal(NULL, exprFirst->asDECL()->sym, false);

    // otherwise, cast to IDisposable to call the explicit interface
    EXPR * iDispObject = tryConvert(object, GetReqPDT(PT_IDISPOSABLE), NOUDC);
    if (!iDispObject) {
        // must be implicitly convertable to IDisposable.
        if (!object->type->isERRORSYM())
            compiler()->Error(tree, ERR_NoConvToIDisp, object->type);
        return;
    }

    EXPRSTMT * exitList = NULL;
    bldrTmp.Init(&exitList);

    EXPRLABEL * pLabel = NULL;

    if (!object->type->IsValType() || object->type->isNUBSYM()) {
        // we must check for null...
        pLabel = newExprLabel();

        EXPR *cond;
        if (object->type->isNUBSYM()) {
            cond = BindNubHasValue(NULL, object);
            bldrTmp.Add(MakeGotoIf(NULL,
                cond,
                pLabel, false));
        } else {
            // Need to box type variables for the null test.
            EXPR * obj = object->type->isTYVARSYM() ? mustConvertCore(object, GetReqPDT(PT_OBJECT), tree, NOUDC) : object;
            cond = newExprBinop(NULL, EK_EQ, GetReqPDT(PT_BOOL), obj, bindNull(NULL));
            bldrTmp.Add(MakeGotoIf(NULL,
                cond,
                pLabel, true));
        }
    }

    AGGTYPESYM *disposeMMethodInType = GetReqPDT(PT_IDISPOSABLE);
    METHSYM *disposeM = FindPredefMeth(tree, PN_DISPOSE, disposeMMethodInType, BSYMMGR::EmptyTypeArray());
    if (!disposeM) {
        // either getPDT or FindPredefMeth should have given the error
        ASSERT(compiler()->ErrorCount() > 0);
        return;
    }

    ASSERT(disposeM && disposeMMethodInType);
    EXPRCALL *callDispose = BindToMethod(tree, iDispObject, MethWithInst(disposeM, disposeMMethodInType), NULL)->asCALL();
    if (object->type->isTYVARSYM() || object->type->IsNonNubValType())
        callDispose->flags |= EXF_CONSTRAINED;

    bldrTmp.Add(MakeStmt(NULL, callDispose, EXF_NODEBUGINFO));
    if (pLabel)
        bldrTmp.Add(pLabel);
    if (wrap && wrap->kind == EK_WRAP)
        bldrTmp.Add(MakeStmt(NULL, newExprWrap(wrap, TK_USING)));

    tryExpr->handlers->asBLOCK()->statements = exitList;

    bldr.Add(tryExpr);
}


// Searches for a valid method to match either the GetEnumerator or Dispose patterns
// This will not report errors, but will give warnings if the pattern is not matched.
//
// tree -- the parse-tree for error reporting
// name -- id to match (either PN_GETENUMERATOR or PN_DISPOSE
// type -- the type to look in
// expr -- the object
// typeRet -- the return type to match (may be NULL)
// idsPattern -- a string resource id for the pattern name (for error messages)
EXPRCALL * FUNCBREC::BindPatternToMethod(BASENODE *tree, PREDEFNAME pn, TYPESYM *type, EXPR *exprObj, TYPESYM * typeRet, int idsPattern)
{
    ASSERT(type);

    NAME *name = compiler()->namemgr->GetPredefName(pn);

    // First check if the object has a public dispose method, if so use the method directly
    // instead of casting to an interface...
    MemberLookup mem;
    if (!type->isAGGTYPESYM() && !type->isTYVARSYM())
        return NULL;

    if (!mem.Lookup(compiler(), type, exprObj, parentDecl, name, 0, MemLookFlags::UserCallable) || !mem.SymFirst()->isMETHSYM())
    {
        if (mem.SymFirst())
            compiler()->Error(tree, WRN_PatternBadSignature, type, ErrArgIds(idsPattern), ErrArgRef(mem.SymFirst()));
        return NULL;
    }
    mem.ReportWarnings(tree);

    EXPRMEMGRP * grp = newExprMemGrp(tree, NULL, mem);
    ASSERT(grp->sk == SK_METHSYM);      // must be a method

    MethWithInst mwiAmbig;
    MethWithInst mwiRes;

    if (!BindGrpToParams(&mwiRes, tree, grp, BSYMMGR::EmptyTypeArray(), false, &mwiAmbig)) {
        compiler()->Error(tree, WRN_PatternBadSignature, type, ErrArgIds(idsPattern), ErrArgRef(mem.SymFirst()));
        return NULL;
    }
    if (mwiAmbig) {
        compiler()->Error(tree, WRN_PatternIsAmbiguous, type, ErrArgIds(idsPattern), ErrArgRef(mwiRes), ErrArgRef(mwiAmbig));
        return NULL;
    }
    if (typeRet && mwiRes.Meth()->retType != typeRet) {
        compiler()->Error(tree, WRN_PatternBadSignature, type, ErrArgIds(idsPattern), ErrArgRef(mwiRes));
        return NULL;
    }

    METHSYM *meth = mwiRes.Meth();
    ASSERT(meth->params->size == 0 && meth->typeVars->size == 0);
    if (meth->isStatic || meth->GetAccess() != ACC_PUBLIC) {
        compiler()->Error(tree, WRN_PatternStaticOrInaccessible, type, ErrArgIds(idsPattern), ErrArgRef(mwiRes));
        return NULL;
    }

    EXPR *callRes = BindToMethod(tree, grp->object, mwiRes, NULL);

    // if the method had conditional symbols on it, then it will get turned into a no-op and a method that does not exist should not meet the pattern.
    // Thus, we need the check to verify this is actually an EXPRCALL
    if (!callRes || !callRes->isOK() || callRes->kind != EK_CALL)
        return NULL;

    return callRes->asCALL();
}


void FUNCBREC::BindFixedDecls(FORSTMTNODE * tree, StmtListBldr & bldr)
{
    EXPRTRY * tryExpr = newExpr(tree, EK_TRY, NULL)->asTRY();

    EXPRGOTO * oldGotos = gotos;

    tryExpr->flags |= EXF_ISFINALLY;

    finallyNestingCount ++;

    createNewScope();
    pCurrentScope->scopeFlags = SF_TRYSCOPE | SF_LAZYFINALLY;
    pCurrentBlock = newExprBlock(tree);
    tryExpr->tryblock = pCurrentBlock;
    SCOPESYM * oldScope = pTryScope;
    SCOPESYM * scopeOfCurrentTry = pTryScope = pCurrentScope;
    pCurrentBlock->scopeSymbol = pCurrentScope;

    EXPRSTMT * stmtFirst = NULL;
    StmtListBldr bldrTmp(&stmtFirst);

    DECLSTMTNODE * decls = tree->pInit->asDECLSTMT();
    ASSERT(decls->flags & NF_FIXED_DECL);
    BindVarDecls(decls, bldrTmp);

    // Walk the tree and replace any fixed strings with temporaries
    TYPESYM * stringTyp = GetReqPDT(PT_STRING);
    int cstmtStr = 0;
    int cstmtTot = 0;

    for (EXPRSTMT * stmt = stmtFirst; stmt; stmt = stmt->stmtNext) {
        cstmtTot++;
        if (stmt->kind != EK_DECL || !stmt->asDECL()->sym)
            continue;
        EXPRDECL * decl = stmt->asDECL();
        if (!decl->init || decl->init->kind != EK_ASSG || !decl->init->asBIN()->p2)
            continue;
        bool hadCast = false;
        EXPR * stringExpr = decl->init->asBIN()->p2;
        if (stringExpr->kind != EK_ADDR && stringExpr->kind == EK_CAST && stringExpr->asCAST()->p1 && stringExpr->asCAST()->p1->kind == EK_ADDR) {
            stringExpr = stringExpr->asCAST()->p1;
            hadCast = true;
        }
        if (stringExpr->kind == EK_ADDR) {
            stringExpr = stringExpr->asBIN()->p1;
        }
        if (stringExpr->type == stringTyp) {
            // what we have is:
            // charptr = &stringexpr
            // we replace this with
            // wrap = stringexpr, charptr = &wrap
            EXPRWRAP * wrap = newExprWrap(stringExpr, (TEMP_KIND)(TK_FIXED_STRING_0 + cstmtStr));
            cstmtStr++;

            wrap->setType(compiler()->getBSymmgr().GetPinnedType(stringTyp));
            wrap->pinned = true;
            wrap->doNotFree = true;
            EXPRBINOP * assignment = newExprBinop(NULL, EK_ASSG, stringTyp, wrap, stringExpr);
            assignment->flags |= EXF_ASSGOP;
            EXPRBINOP * sequence = newExprBinop(NULL, EK_SEQUENCE, NULL, assignment, decl->init);
            if (hadCast) {
                sequence->asBIN()->p2->asBIN()->p2 = decl->init->asBIN()->p2->asCAST()->p1;
            }
            sequence->asBIN()->p2->asBIN()->p2->asBIN()->p1 = wrap;
            sequence->asBIN()->p2->asBIN()->p2->flags |= EXF_ASSGOP;
            decl->init = sequence;
            decl->sym->slot.isPinned = false;
        }
    }

    BindStatement(tree->pStmt, bldrTmp);
    pCurrentBlock->statements = stmtFirst;
    bldrTmp.Clear();

    pCurrentBlock = pCurrentBlock->owningBlock;

    closeScope();
    pTryScope = oldScope;

    finallyNestingCount--;

    tryExpr->handlers = newExprBlock(tree);

    // Add the code which nulls out the fixed locals...
    bldrTmp.Init(&tryExpr->handlers->asBLOCK()->statements);
    for (EXPRSTMT * stmt = stmtFirst; stmt; stmt = stmt->stmtNext) {
        // Only need to loop over the original decls.
        if (--cstmtTot < 0)
            break;
        if (stmt->kind != EK_DECL || !stmt->asDECL()->sym || !stmt->asDECL()->sym->type)
            continue;
        EXPRDECL * decl = stmt->asDECL();
        EXPR * target = NULL;
        if (decl->sym->slot.isPinned)
            target = MakeLocal(NULL, decl->sym, true);
        else if (decl->init && decl->init->kind == EK_SEQUENCE)
            target = decl->init->asBIN()->p1->asBIN()->p1->asWRAP();

        if (target) {
            EXPR * value = tryConvert(bindNull(NULL), target->kind == EK_WRAP ? stringTyp : target->type);
            if (value) {
                bldrTmp.Add(MakeAssignment(target, value));
                if (target->kind == EK_WRAP)
                    bldrTmp.Add(MakeStmt(NULL, newExprWrap(target, target->asWRAP()->tempKind)));
            }
        }
    }
    bldrTmp.Clear();

    // see if we can eliminate the try-finally
    // this is ok if we meet all the following conditions:
    // we are not enclosed in a try-catch block
    // the body does not contain any gotos to outside

    bool canEliminate = true;

    SCOPESYM * scope = pTryScope;
    while (scope) {
        if ((scope->scopeFlags & (SF_TRYSCOPE | SF_LAZYFINALLY)) == SF_TRYSCOPE) {
            canEliminate = false;
            break;
        }
        scope = scope->parent->asSCOPESYM();
    }

    if (canEliminate) {
        EXPRGOTO * newGotos = gotos;
        while (newGotos != oldGotos) {
            if ((newGotos->flags & EXF_GOTOCASE) || !(newGotos->flags & EXF_UNREALIZEDGOTO)) {
                if (newGotos->targetScope->nestingOrder < scopeOfCurrentTry->nestingOrder) {
                    canEliminate = false;
                    break;
                }
            } else {
                // this goto is not yet realized and we don't know where its going, so it could be going outside
                canEliminate = false;
                break;
            }
            newGotos = newGotos->prev;
        }
    }

    if (canEliminate) {
        tryExpr->flags |= EXF_REMOVEFINALLY;
    }

    bldr.Add(SetNodeStmt(tree, tryExpr));
}

struct ARRAYFOREACHSLOT {
    EXPRWRAP * limit;
    EXPRWRAP * current;
    EXPRLABEL * body;
    EXPRLABEL * test;

};


void FUNCBREC::BindForEachArray(FORSTMTNODE * tree, EXPR * array, StmtListBldr & bldr)
{
    ASSERT(array->type->isARRAYSYM());
    ASSERT(array->type->asARRAYSYM()->rank != 0);

    int rank;
    int curRank;
    rank = array->type->asARRAYSYM()->rank;

    ARRAYFOREACHSLOT * slots;
    slots = STACK_ALLOC(ARRAYFOREACHSLOT, rank);

    EXPRWRAP * arrayTemp;
    arrayTemp = newExprWrap(array, TK_FOREACH_ARRAY);
    arrayTemp->flags |= EXF_WRAPASTEMP;
    arrayTemp->doNotFree = true;

    bldr.Add(MakeAssignment(tree->pExpr, arrayTemp, array));

    TYPESYM * intType;
    intType = GetReqPDT(PT_INT);

    TYPESYM * oneIntArg[1];
    oneIntArg[0] = intType;
    TypeArray * oneIntArgList;
    oneIntArgList = compiler()->getBSymmgr().AllocParams(1, oneIntArg);

    if (rank == 1) {
        // we do not prefetch the array limit as that results in worse code...
        slots[0].limit = NULL;
    } else {
        // First initialize locals to the maximum sizes for each dimension
        for (curRank = 0; curRank < rank; curRank++) {
            EXPRWRAP * limit = NULL;
            limit = newExprWrap( NULL, (TEMP_KIND)(TK_FOREACH_ARRAYLIMIT_0 + curRank));
            limit->setType(intType);
            limit->doNotFree = true;
            limit->flags = EXF_WRAPASTEMP;

            slots[curRank].limit = limit;

            EXPR * arg = newExprConstant(NULL, intType, ConstValInit(curRank));
            EXPR * callGetLimit = BindPredefMethToArgs(tree, PN_GETUPPERBOUND, GetReqPDT(PT_ARRAY), arrayTemp, arg);

            bldr.Add(MakeAssignment(tree->pExpr, limit, callGetLimit), EXF_GENERATEDSTMT);
        }
    }

    for (curRank = 0; curRank < rank; curRank++) {
        EXPRWRAP * current = newExprWrap( NULL, (TEMP_KIND)(TK_FOREACH_ARRAYINDEX_0 + curRank));
        current->setType(intType);
        current->doNotFree = true;
        current->flags = EXF_WRAPASTEMP;

        slots[curRank].current = current;

        if (rank > 1) {
            EXPR * arg = newExprConstant(NULL, intType, ConstValInit(curRank));
            EXPR * callGetLimit = BindPredefMethToArgs(tree, PN_GETLOWERBOUND, GetReqPDT(PT_ARRAY), arrayTemp, arg);

            bldr.Add(MakeAssignment(tree->pExpr, current, callGetLimit), EXF_GENERATEDSTMT);
        }
        else {
            bldr.Add(MakeAssignment(tree->pExpr, current, newExprConstant(tree, intType, ConstValInit(0))), EXF_GENERATEDSTMT);
        }

        EXPRLABEL * test = newExprLabel();
        slots[curRank].test = test;
        bldr.Add(MakeGoto(NULL, test, EXF_NODEBUGINFO));

        EXPRLABEL * body = newExprLabel();
        slots[curRank].body = body;
        bldr.Add(body);
    }

    EXPRSTMT * init = NULL;
    StmtListBldr bldrTmp(&init);
    BindVarDecls(tree->pInit->asDECLSTMT(), bldrTmp);
    if (!init || init->kind != EK_DECL) {
        BindStatement(tree->pStmt, bldr);
        return;
    }

    LOCVARSYM * local = init->asDECL()->sym;

    EXPR * indexList;
    EXPR ** pIndexList;
    indexList = NULL;
    pIndexList = &indexList;

    for (curRank = 0; curRank < rank; curRank ++) {
        newList(slots[curRank].current, &pIndexList);
    }

    EXPR * arrayIndexing;
    arrayIndexing = newExprBinop(tree, EK_ARRINDEX, array->type->asARRAYSYM()->elementType(), arrayTemp, indexList);
    arrayIndexing->flags |= EXF_LVALUE | EXF_ASSGOP;

    bldr.Add(MakeAssignment(tree->pInit,
        MakeLocal(tree, local, true),
        mustCast(arrayIndexing, local->type)), EXF_GENERATEDSTMT);

    local->isNonWriteable = true;
    local->isForeach = true;
    ASSERT(!local->slot.isReferenced);
    local->slot.isReferenced = true;
    unreferencedVarCount--;

    BindStatement(tree->pStmt, bldr);

    local->isNonWriteable = false;

    bldr.Add(loopLabels->contLabel);

    for (curRank = rank - 1; curRank >= 0; curRank--) {
        EXPRSTMT * assignment = MakeAssignment(tree, slots[curRank].current,
            newExprBinop(tree, EK_ADD, intType, slots[curRank].current, newExprConstant(tree, intType, ConstValInit(1))));
        if (curRank == rank - 1)
            assignment->flags |= EXF_NODEBUGINFO;   // To get better breakpoint behavior, innermost increment has no debug info. See VS7:343108.
        bldr.Add(assignment, EXF_GENERATEDSTMT);
        bldr.Add(slots[curRank].test, EXF_GENERATEDSTMT);
        bldr.Add(MakeGotoIf(tree, 
            newExprBinop(tree, rank == 1 ? EK_LT : EK_LE, GetReqPDT(PT_BOOL), slots[curRank].current, rank == 1 ? newExprBinop(tree, EK_ARRLEN, intType, arrayTemp, NULL)->asEXPR() : slots[curRank].limit->asEXPR()),
            slots[curRank].body, true, EXF_GENERATEDSTMT));
    }

    bldr.Add(loopLabels->breakLabel);

    // this will free the temporaries
    for (curRank = 0; curRank < rank; curRank++) {
        bldr.Add(MakeStmt(NULL, newExprWrap(slots[curRank].current, TK_SHORTLIVED)));
        if (rank != 1)
            bldr.Add(MakeStmt(NULL, newExprWrap(slots[curRank].limit, TK_SHORTLIVED)));
    }

    bldr.Add(MakeStmt(NULL, newExprWrap(arrayTemp, TK_FOREACH_ARRAY)));
}


void FUNCBREC::BindForEachString(FORSTMTNODE * tree, EXPR * string, StmtListBldr & bldr)
{
    ASSERT(string->type == GetReqPDT(PT_STRING));

    EXPRWRAP * stringTemp;
    stringTemp = newExprWrap(string, TK_FOREACH_ARRAY);
    stringTemp->flags |= EXF_WRAPASTEMP;
    stringTemp->doNotFree = true;

    bldr.Add(MakeAssignment(tree->pExpr, stringTemp, string));

    TYPESYM * intType = GetReqPDT(PT_INT);

    TYPESYM * oneIntArg[1];
    oneIntArg[0] = intType;
    compiler()->getBSymmgr().AllocParams(1, oneIntArg);

    EXPR * callGetLength = BindPredefMethToArgs(tree, PN_GETLENGTH, GetReqPDT(PT_STRING), stringTemp, NULL);

    EXPRWRAP * current = newExprWrap( NULL, TK_FOREACH_ARRAYINDEX_0);
    current->setType(intType);
    current->doNotFree = true;
    current->flags = EXF_WRAPASTEMP;

    bldr.Add(MakeAssignment(tree->pExpr, current, newExprConstant(tree, intType, ConstValInit(0))), EXF_GENERATEDSTMT);

    EXPRLABEL * test = newExprLabel();
    bldr.Add(MakeGoto(NULL, test, EXF_NODEBUGINFO));

    EXPRLABEL * labBody = newExprLabel();
    bldr.Add(labBody);

    EXPRSTMT * init = NULL;
    StmtListBldr bldrTmp(&init);
    BindVarDecls(tree->pInit->asDECLSTMT(), bldrTmp);
    if (!init || init->kind != EK_DECL) {
        BindStatement(tree->pStmt, bldr);
        return;
    }

    LOCVARSYM * local = init->asDECL()->sym;

    METHSYM * getChars;

    bool fError = false;
    MemberLookup mem;
    if (!mem.Lookup(compiler(), string->type, string, parentDecl, compiler()->namemgr->GetPredefName(PN_INDEXERINTERNAL), 0, MemLookFlags::Indexer)) {
        mem.ReportErrors(tree);
        fError = true;
    }
    else {
        mem.ReportWarnings(tree);
        ASSERT(mem.SymFirst() && mem.SymFirst()->isPROPSYM() && mem.SymFirst()->asPROPSYM()->isIndexer());

        getChars = mem.SymFirst()->asPROPSYM()->methGet;
        ASSERT(getChars && getChars->params->size == 1 && getChars->params->Item(0) == intType);

        EXPRCALL * callGetChars = newExpr(tree, EK_CALL, NULL)->asCALL();
        callGetChars->mwi.Set(getChars, mem.Type(0), NULL);
        callGetChars->args = current;
        callGetChars->object = stringTemp;
        callGetChars->setType(getChars->retType);

        bldr.Add(MakeAssignment(tree->pInit,
            MakeLocal(tree, local, true),
            mustCast(callGetChars, local->type)), EXF_GENERATEDSTMT);
    }

    local->isNonWriteable = true;
    local->isForeach = true;
    ASSERT(!local->slot.isReferenced);
    local->slot.isReferenced = true;
    unreferencedVarCount--;

    BindStatement(tree->pStmt, bldr);

    local->isNonWriteable = false;

    if (fError)
        return;

    bldr.Add(loopLabels->contLabel);

    // To get better breakpoint behavior, increment has no debug info. See VS7:343108.
    bldr.Add(MakeAssignment(tree, current, newExprBinop(tree, EK_ADD, intType, current, newExprConstant(tree, intType, ConstValInit(1)))),
        EXF_NODEBUGINFO | EXF_GENERATEDSTMT);

    bldr.Add(test);
    bldr.Add(MakeGotoIf(tree,
        newExprBinop(tree, EK_LT, GetReqPDT(PT_BOOL), current, callGetLength),
        labBody, true, EXF_GENERATEDSTMT));
    bldr.Add(loopLabels->breakLabel);
    bldr.Add(MakeStmt(NULL, newExprWrap(current, TK_SHORTLIVED)));
    bldr.Add(MakeStmt(NULL, newExprWrap(stringTemp, TK_FOREACH_ARRAY)));
}


void FUNCBREC::BindForEach(FORSTMTNODE * tree, StmtListBldr & bldr)
{
    EXPRDEBUGNOOP * nopExpr = newExpr(tree, EK_DEBUGNOOP, NULL)->asDEBUGNOOP();
    nopExpr->flags |= EXF_USEORIGDEBUGINFO;
    bldr.Add(nopExpr);

    EXPRTRY * tryExpr = newExpr(tree, EK_TRY, NULL)->asTRY();
    tryExpr->flags |= EXF_ISFINALLY;
    tryExpr->handlers = newExprBlock(tree);

    finallyNestingCount++;

    createNewScope();
    pCurrentScope->scopeFlags = SF_TRYSCOPE;
    pCurrentBlock = newExprBlock(tree);
    tryExpr->tryblock = pCurrentBlock;
    SCOPESYM * oldScope = pTryScope;
    pTryScope = pCurrentScope;
    pCurrentBlock->scopeSymbol = pCurrentScope;

    EXPR * enumerator = NULL;
    StmtListBldr bldrTmp(&pCurrentBlock->statements);
    BindForEachInner(tree, &enumerator, bldr, bldrTmp);
    bldrTmp.Clear();

    pCurrentBlock = pCurrentBlock->owningBlock;
    closeScope();
    pTryScope = oldScope;

    finallyNestingCount--;

    // Now, do we need a finally block?
    if (enumerator) {
        ASSERT(enumerator->type->isAGGTYPESYM() || enumerator->type->isTYVARSYM());

        bool fRemoveFinally;
        bldrTmp.Init(&tryExpr->handlers->asBLOCK()->statements);
        if (!BindForEachFinally(tree, enumerator, bldrTmp, &fRemoveFinally))
            return;
        bldrTmp.Clear();

        if (fRemoveFinally)
            tryExpr->flags |= EXF_REMOVEFINALLY;
        else {
            createNewScope();
            pCurrentScope->scopeFlags = SF_FINALLYSCOPE;
            tryExpr->handlers->asBLOCK()->scopeSymbol = pCurrentScope;
            pCurrentScope->SetBlock(tryExpr->handlers->asBLOCK());
            closeScope();
        }

        bldr.Add(tryExpr);
        bldr.Add(MakeStmt(tree->pInit, newExprWrap(enumerator, TK_SHORTLIVED), EXF_GENERATEDSTMT));
    }
    // this was either an error, or an array foreach...
    else if (tryExpr->tryblock->statements) {
        tryExpr->flags |= EXF_REMOVEFINALLY;
        bldr.Add(tryExpr);
    }
}

// BindForEachFinally() -- binds the finally clause on a foreach statement if there is one.
//  We first check for the dispose pattern, and if that does not exist then we check for IDisposable interface, and as a
//  last resort we check at runtime.
//
//  tree -- parsetree for error messages
//  enumerator -- the EXPR for the enumerator
//  pList -- [In/Out] List of statements to append to
//  pfRemoveFinally -- [Out] can we optimize away the finally clause?
bool FUNCBREC::BindForEachFinally(BASENODE *tree, EXPR *enumerator, StmtListBldr & bldr, bool *pfRemoveFinally)
{
    ASSERT(enumerator);
    *pfRemoveFinally = false;

    TYPESYM *typeEnum = enumerator->type;
    bool isStruct = typeEnum->isStructType();

    EXPR * dispObject = enumerator;
    bool fMustCheckAtRuntime = false;
    bool fHasInterface = false;
    bool fRemoveFinally = false;

    AGGTYPESYM *typDisp = GetReqPDT(PT_IDISPOSABLE);
    ASSERT(typDisp);

    if (typeEnum->isPredefType(PT_IENUMERATOR)) {
        fMustCheckAtRuntime = true;
    } else {
        if (canConvert(typeEnum, typDisp, NOUDC)) {
            fHasInterface = true;
        } else if (isStruct || typeEnum->isAGGTYPESYM() && typeEnum->getAggregate()->isSealed) {
            fRemoveFinally = true;
        } else {
            fMustCheckAtRuntime = true;
        }
    }

    // must be exactly one of the above
    ASSERT((fMustCheckAtRuntime ^ fHasInterface ^ fRemoveFinally) && !(fMustCheckAtRuntime && fHasInterface && fRemoveFinally));
    ASSERT(typDisp);

    if (fRemoveFinally) {
        // no dispose to call, so get rid of the try/finally mechanism
        *pfRemoveFinally = true;
        return true;
    }
    ASSERT(fMustCheckAtRuntime || fHasInterface);
    if (fMustCheckAtRuntime) {
        ASSERT(!isStruct);

        // if we haven't found a public dispose method, and it does not implement IDisposable
        // then we have to check at runtime
        EXPRTYPEOF * typeofExpr = newExpr(tree, EK_TYPEOF, GetReqPDT(PT_TYPE))->asTYPEOF();
        typeofExpr->sourceType = typDisp;
        typeofExpr->method = NULL;

        EXPR * exprAs = newExprBinop(tree, EK_AS, typDisp, enumerator, typeofExpr);
        EXPRWRAP * disp = newExprWrap(exprAs, TK_SHORTLIVED);
        disp->flags |= EXF_WRAPASTEMP;
        disp->doNotFree = true;

        EXPRSTMT * stmt = MakeAssignment(tree, disp, exprAs);
        stmt->flags |= EXF_NODEBUGINFO;

        bldr.Add(stmt);
        isStruct = false;

        dispObject = disp;
    }

    // Call Dispose on the interface
    METHSYM *meth = FindPredefMeth(tree, PN_DISPOSE, typDisp, BSYMMGR::EmptyTypeArray());
    dispObject = mustConvertCore(dispObject, typDisp, tree, NOUDC);

    EXPR * callDispose = BindToMethod(tree, dispObject, MethWithInst(meth, typDisp), NULL);
    if (!callDispose || callDispose->kind != EK_CALL)
        return false;
    if (typeEnum->isTYVARSYM() && !fMustCheckAtRuntime || isStruct)
        callDispose->flags |= EXF_CONSTRAINED;

    // we have a dispose method, now generate the EXPR and add it to the list
    ASSERT(callDispose && dispObject);

    EXPRLABEL *label = NULL;
    if (!isStruct) {
        // we also need to check for null before calling...
        label = newExprLabel();

        // Need to box type variables for the null test.
        EXPR * obj = dispObject->type->isTYVARSYM() ? mustConvertCore(dispObject, GetReqPDT(PT_OBJECT), tree, NOUDC) : dispObject;

        bldr.Add(MakeGotoIf(tree,
            newExprBinop(tree, EK_EQ, GetReqPDT(PT_BOOL), obj, bindNull(NULL)),
            label, true, EXF_NODEBUGINFO));
    }

    bldr.Add(MakeStmt(tree, callDispose, EXF_NODEBUGINFO));
    if (dispObject->kind == EK_WRAP && dispObject->asWRAP()->expr->kind == EK_AS)
        bldr.Add(MakeStmt(tree, newExprWrap(dispObject, TK_SHORTLIVED)));
    if (label)
        bldr.Add(label);

    return true;
}


// Returns the implemented interface or NULL if there is none
// It always reports an error before returning NULL
// if there are multiple generic interfaces, it reports ERR_MultipleIEnumOfT
// if badMember is -1, it reports ERR_BadGetEnumerator
// otherwise it reports ERR_ForEachMissingMember
AGGTYPESYM * FUNCBREC::HasIEnumerable(TYPESYM * collection, BASENODE * tree, TYPESYM * badType, PREDEFNAME badMember)
{
    AGGTYPESYM * atsIfaceCandidate = NULL;
    // First try the generic interfaces
    AGGSYM * clsGEnum = compiler()->GetOptPredefAgg(PT_G_IENUMERABLE);
    TypeArray * ifacesAll = NULL;
    AGGTYPESYM * atsBase = NULL;

    // If generics don't exist or the type isn't an AGGTYPESYM
    // then we can't check the interfaces (and base-class interfaces)
    // for IEnumerable<T> so immediately try the non-generic IEnumerable
    if (clsGEnum == NULL)
        goto NO_GENERIC;

    if (collection->isAGGTYPESYM()) {
        if (collection->getAggregate() == clsGEnum || collection->isPredefType(PT_IENUMERABLE)) {
            ASSERT(!"IEnumerable/ator types are bad!");
            goto LERROR;
        }

        ifacesAll = collection->asAGGTYPESYM()->GetIfacesAll();
        atsBase = collection->asAGGTYPESYM()->GetBaseClass();
    }
    else if (collection->isTYVARSYM()) {
        // Note: we'll search the interface list before the class constraint, but it doesn't matter
        // since we require a unique instantiation of IEnumerable<T>.
        // Note: The pattern search will usually find the interface constraint - but if the class
        // constraint has a non-public or non-applicable or non-method GetEnumerator, the interfaces
        // are hidden in which case we will find them here.
        ifacesAll = collection->asTYVARSYM()->GetIfacesAll();
        atsBase = collection->asTYVARSYM()->GetBaseCls();
    }
    else
        goto NO_GENERIC;

    ASSERT(ifacesAll);

    // If the type implements exactly one instantiation of
    // IEnumerable<T> then it's the one.
    //
    // If it implements none then try the non-generic interface.
    //
    // If it implements more than one, then it's an error.
    //
    // Search the direct and indirect interfaces via ifacesAll, going up the base chain...
    // Work up the base chain
    for (;;) {
        // Now work across all the interfaces
        for (int i = 0; i < ifacesAll->size; i++) {
            AGGTYPESYM * iface = ifacesAll->Item(i)->asAGGTYPESYM();
            if (iface->getAggregate() == clsGEnum) {
                if (atsIfaceCandidate == NULL) {
                    // First implementation
                    atsIfaceCandidate = iface;
                } else if (iface != atsIfaceCandidate) {
                    // If this really is a different instantiation report an error
                    compiler()->Error(tree, ERR_MultipleIEnumOfT, ErrArgRef(collection), clsGEnum->getThisType());
                    return NULL;
                }
            }
        }
        // Check the base class.
        if (!atsBase)
            break;
        ifacesAll = atsBase->GetIfacesAll();
        atsBase = atsBase->GetBaseClass();
    }

    // Report the one and only generic interface
    if (atsIfaceCandidate) {
        ASSERT(canConvert(collection, atsIfaceCandidate, NOUDC));
        return atsIfaceCandidate;
    }

NO_GENERIC:
    if (collection->isPredefType(PT_IENUMERABLE)) {
        VSFAIL("Why didn't IEnumerator match the pattern?");
        goto LERROR;
    }

    // No errors, no generic interfaces, try the non-generic interface
    atsIfaceCandidate = GetReqPDT(PT_IENUMERABLE);
    if (canConvert(collection, atsIfaceCandidate, NOUDC)) {
        return atsIfaceCandidate;
    }

LERROR:
    compiler()->Error(tree, ERR_ForEachMissingMember, collection, badType, badMember);

    return NULL;
}

void FUNCBREC::BindForEachInner(FORSTMTNODE * tree, EXPR ** enumerator, StmtListBldr & bldrInit, StmtListBldr & bldr)
{
    // Note that the caller has prepared a block and scope for us already, so we just fill in the
    // statements into that block
    LOOPLABELS * prev = loopLabels;
    LOOPLABELS ll(this);

    BindForEachInnerCore(tree, enumerator, bldrInit, bldr);

    loopLabels = prev;
}


void FUNCBREC::BindForEachInnerCore(FORSTMTNODE * tree, EXPR ** enumeratorExpr, StmtListBldr & bldrInit, StmtListBldr & bldr)
{
    *enumeratorExpr = NULL;

    EXPR * callGetEnum = NULL;
    EXPR * callMoveNext = NULL;
    EXPR * callCurrent = NULL;
    EXPRLABEL * labelAgain = NULL;
    TYPESYM * typeEnum = NULL;
    EXPRWRAP * exprAtor = NULL;

    // Note that the caller has prepared a block and scope for us already, so we just fill in the
    // statements into that block

    MemberLookup mem;
    MethWithInst mwiAmbig;
    MethWithInst mwiT;

    // This is cleared below if we don't encounter any errors.
    bool fError = true;

    EXPR * collection = bindExpr(tree->pExpr);
    if (collection->isNull()) {
        compiler()->Error(tree, ERR_NullNotValid);
    }
    else if (collection->isOK()) {
        TYPESYM * type; type = collection->type;

        if (type->isARRAYSYM()) {
            BindForEachArray(tree, collection, bldr);
            return;
        }
        if (type->isPredefType(PT_STRING)) {
            BindForEachString(tree, collection, bldr);
            return;
        }

        if (!type->isAGGTYPESYM() && !type->isTYVARSYM()) {
            if (type->isANONMETHSYM() || type->isMETHGRPSYM()) {
                compiler()->Error(tree, ERR_AnonMethGrpInForEach, type);
            }
            else {
                compiler()->Error(tree, ERR_ForEachMissingMember, type, type, PN_GETENUMERATOR);
            }
            goto LError;
        }

        // First check for the pattern.
        callGetEnum = BindPatternToMethod(tree, PN_GETENUMERATOR, type, collection, NULL, IDS_Collection);
        if (!callGetEnum) {
            // There is no pattern, so we try the interface
            AGGTYPESYM * typeIAble = HasIEnumerable(type, tree, type, PN_GETENUMERATOR);
            if (!typeIAble)
                goto LError;
            callGetEnum = BindPredefMethToArgs(tree, PN_GETENUMERATOR, typeIAble, collection, NULL);
            if (!callGetEnum->isOK())
                goto LError;
        }

        typeEnum = callGetEnum->type;

        if (!typeEnum->isAGGTYPESYM() && !typeEnum->isTYVARSYM()) {
LBadRetType:
            compiler()->ErrorRef(tree, ERR_BadGetEnumerator, typeEnum, callGetEnum->asCALL()->mwi);
            goto LError;
        }

        compiler()->EnsureState(typeEnum);
        if (typeEnum->IsDeprecated()) {
            ReportDeprecated(tree, SymWithType(typeEnum, NULL));
        }

        exprAtor = newExprWrap(callGetEnum, TK_FOREACH_GETENUM);
        exprAtor->flags |= EXF_WRAPASTEMP;
        exprAtor->doNotFree = true;
        *enumeratorExpr = exprAtor;

        // Handle the MoveNext method.
        NAME * name; name = compiler()->namemgr->GetPredefName(PN_MOVENEXT);

        if (!mem.Lookup(compiler(), typeEnum, exprAtor, parentDecl, name, 0, MemLookFlags::None)) {
            mem.ReportErrors(tree);
            goto LBadRetType;
        }
        if (!mem.SymFirst()->isMETHSYM())
            goto LBadRetType;
        mem.ReportWarnings(tree);

        EXPRMEMGRP * grp; grp = newExprMemGrp(tree, NULL, mem);

        if (!BindGrpToParams(&mwiT, tree, grp, BSYMMGR::EmptyTypeArray(), false, &mwiAmbig))
            goto LBadRetType;
        if (mwiAmbig) {
            compiler()->Error(tree, ERR_AmbigCall, mwiT, mwiAmbig);
            goto LBadRetType;
        }
        if (!mwiT.Meth()->retType->isPredefType(PT_BOOL) || mwiT.Meth()->GetAccess() != ACC_PUBLIC || mwiT.Meth()->isStatic)
            goto LBadRetType;

        callMoveNext = BindToMethod(tree, grp->object, mwiT, NULL);
        if (!callMoveNext->isOK())
            goto LBadRetType;

        // Handle the Current property.
        name = compiler()->namemgr->GetPredefName(PN_CURRENT);
        if (!mem.Lookup(compiler(), typeEnum, exprAtor, parentDecl, name, 0, MemLookFlags::None)) {
            mem.ReportErrors(tree);
            goto LBadRetType;
        }
        if (!mem.SymFirst()->isPROPSYM() || mem.SymFirst()->GetAccess() != ACC_PUBLIC || mem.SymFirst()->asPROPSYM()->isStatic)
            goto LBadRetType;
        mem.ReportWarnings(tree);
        callCurrent = BindToProperty(tree, exprAtor, mem.SwtFirst(), BIND_RVALUEREQUIRED);
        if (!callCurrent->isOK())
            goto LBadRetType;

        fError = false;
    }

LError:
    EXPRSTMT ** pstmtNext = bldrInit.PstmtNext();
    BindVarDecls(tree->pInit->asDECLSTMT(), bldrInit);
    EXPR * init = *pstmtNext;

    if (!init || init->kind != EK_DECL) {
        ASSERT(!init);
        fError = true;
        BindStatement(tree->pStmt, bldr);
        return;
    }

    LOCVARSYM * local = init->asDECL()->sym;

    if (!fError) {
        bldrInit.Add(MakeAssignment(tree->pExpr, exprAtor, callGetEnum));
        bldr.Add(MakeGoto(NULL, loopLabels->contLabel));
        bldr.Add(labelAgain = newExprLabel());
        bldr.Add(MakeAssignment(tree->pInit, MakeLocal(tree, local, true), mustCast(callCurrent, local->type)), EXF_GENERATEDSTMT);
    }

    local->isNonWriteable = true;
    local->isForeach = true;
    ASSERT(!local->slot.isReferenced);
    local->slot.isReferenced = true;
    unreferencedVarCount--;

    BindStatement(tree->pStmt, bldr);

    local->isNonWriteable = false;

    if (fError)
        return;

    bldr.Add(loopLabels->contLabel);
    bldr.Add(MakeGotoIf(tree, mustConvertCore(callMoveNext, GetReqPDT(PT_BOOL), tree),
        labelAgain, true, EXF_GENERATEDSTMT));
    bldr.Add(loopLabels->breakLabel);
}


// bind a for statement... the result is usually several expr statements
void FUNCBREC::BindFor(FORSTMTNODE * tree, StmtListBldr & bldr)
{
    if (tree->flags & NF_FOR_FOREACH) {
        BindForEach(tree, bldr);
        return;
    }

    if (tree->flags & NF_FIXED_DECL) {
        BindFixedDecls(tree, bldr);
        return;
    }

    if (tree->flags & NF_USING_DECL) {
        BindUsingDecls(tree, bldr);
        return;
    }

    // a for statament introduces a new scope, and to get the debug info right,
    // we need to introduce a new block new in the expression list also.

    createNewScope();
    pCurrentBlock = newExprBlock(tree);
    pCurrentBlock->scopeSymbol = pCurrentScope;

    StmtListBldr bldrInner(&pCurrentBlock->statements);

    LOOPLABELS * prev = loopLabels;
    LOOPLABELS ll(this);

    // this looks like:

    // [init]
    // goto labelTest
    // label1
    // [body]
    // labelContinue
    // [inc]
    // labelTest
    // gotoif test, label1
    // labelBreak

    if (tree->pInit) {
        if (tree->pInit->kind == NK_DECLSTMT)
            BindVarDecls(tree->pInit->asDECLSTMT(), bldrInner);
        else
            bldrInner.Add(MakeStmt(tree->pInit, bindExpr(tree->pInit, BIND_RVALUEREQUIRED | BIND_STMTEXPRONLY)));
    }

    EXPR * test = tree->pExpr ?
        bindBooleanValue(tree->pExpr, bindExpr(tree->pExpr)) :
        newExprConstant(tree, GetReqPDT(PT_BOOL), ConstValInit(true));
    EXPR * incr = bindExpr(tree->pInc, BIND_RVALUEREQUIRED | BIND_STMTEXPRONLY);
    EXPRLABEL * labelTest = newExprLabel();

    bldrInner.Add(MakeGoto(tree, labelTest, EXF_NODEBUGINFO));

    EXPRLABEL * label1 = newExprLabel();
    bldrInner.Add(label1);

    BindStatement(tree->pStmt, bldrInner);

    loopLabels = prev;

    bldrInner.Add(ll.contLabel);
    if (incr)
        bldrInner.Add(MakeStmt(tree->pInc, incr));
    bldrInner.Add(labelTest);

    if (test)
        bldrInner.Add(MakeGotoIf(tree->pExpr, test, label1, true));
    else
        bldrInner.Add(MakeGoto(tree->pExpr, label1));

    bldrInner.Add(ll.breakLabel);

    EXPRBLOCK * rval = pCurrentBlock;
    pCurrentBlock = rval->owningBlock;

    closeScope();

    bldr.Add(SetNodeStmt(tree, rval));
}


// bind a while or do loop...
void FUNCBREC::BindWhileOrDo(LOOPSTMTNODE * tree, bool asWhile, StmtListBldr & bldr)
{
    LOOPLABELS * prev = loopLabels;
    LOOPLABELS ll(this);  // The constructor sets up the links and creates the labels...

    EXPR * cond = NULL;

    // the result looks like:

    // goto labelCont   [optional] if a while loop
    // label2
    // [body]
    // labelCont
    // gotoif cond, label2
    // labelBreak

    if (asWhile) {
        cond = bindBooleanValue(tree->pExpr, bindExpr(tree->pExpr));
        bldr.Add(MakeGoto(tree, ll.contLabel, EXF_NODEBUGINFO));
    }

    EXPRLABEL * label2 = newExprLabel();
    bldr.Add(label2);
    BindStatement(tree->pStmt, bldr);
    bldr.Add(ll.contLabel);
    if (!asWhile)
        cond = bindBooleanValue(tree->pExpr, bindExpr(tree->pExpr));
    bldr.Add(MakeGotoIf(tree, cond, label2, true));
    bldr.Add(ll.breakLabel);

    loopLabels = prev;
}


EXPR * FUNCBREC::bindBooleanValue(BASENODE * tree, EXPR * op)
{
    ASSERT(tree && op);
    TYPESYM * typeBool = GetReqPDT(PT_BOOL);

    if (!op->isOK())
        return newError(tree, typeBool);

    // Give a warning for something like "if (x = false)"
    if (op->kind == EK_ASSG) {
        EXPR * rhs = op->asBIN()->p2;
        if (rhs->kind == EK_CONSTANT && rhs->type->isPredefType(PT_BOOL) && (rhs->flags & EXF_LITERALCONST))
            compiler()->Error(tree, WRN_IncorrectBooleanAssg);
    }

    EXPR * exprRes = tryConvert(op, typeBool);
    if (!exprRes) {
        // Check for operator true.
        EXPR * exprSrc = bindUDUnop(tree, EK_TRUE, op);
        if (!exprSrc || !(exprRes = tryConvert(exprSrc, typeBool))) {
            // This will give an error.
            exprRes = mustConvert(op, typeBool);
        }
    }
    ASSERT(exprRes && (exprRes->type == typeBool || !exprRes->isOK()));

    return exprRes;
}


// Bind an if statement.  (morphs it into gotoifs and gotos...)
void FUNCBREC::BindIf(IFSTMTNODE * tree, StmtListBldr & bldr)
{
    // gotoif !cond, label1
    // [ifArm]
    // goto label2  [optional]
    // label1
    // [elseArm]    [optional]
    // label2       [optional]

    EXPRLABEL * label1 = newExprLabel();

    bldr.Add(MakeGotoIf(tree, bindBooleanValue(tree->pExpr, bindExpr(tree->pExpr)), label1, false));
    BindStatement(tree->pStmt, bldr);

    if (!tree->pElse)
        bldr.Add(label1);
    else {
        EXPRLABEL * label2 = newExprLabel();
        bldr.Add(MakeGoto(NULL, label2));
        bldr.Add(label1);
        BindStatement(tree->pElse, bldr);
        bldr.Add(label2);
    }
}


EXPR * FUNCBREC::bindUDUnop(BASENODE * tree, EXPRKIND ek, EXPR * arg)
{
    NAME * name = ekName(ek);
    ASSERT(name);

    TYPESYM * typeSrc = arg->type;

LAgain:
    compiler()->EnsureState(typeSrc);
    switch (typeSrc->getKind()) {
    case SK_NUBSYM:
        typeSrc = typeSrc->StripNubs();
        goto LAgain;
    case SK_TYVARSYM:
        typeSrc = typeSrc->asTYVARSYM()->GetBaseCls();
        goto LAgain;
    case SK_AGGTYPESYM:
        if (!typeSrc->isClassType() && !typeSrc->isStructType() || typeSrc->asAGGTYPESYM()->getAggregate()->fSkipUDOps)
            return NULL;
        break;
    default:
        return NULL;
    }

    ArgInfos * info = (ArgInfos *)_alloca(sizeof(ArgInfos) + sizeof(EXPR*) * 1);

    info->carg = 1;
    FillInArgInfoFromArgList(info, arg);

    METHLIST * pmethFirst = NULL;
    METHLIST ** ppmethAdd = &pmethFirst;
    METHSYM * methCur = NULL;
    AGGTYPESYM * atsCur = typeSrc->asAGGTYPESYM();

    for (;;) {
        // Find the next operator.
        methCur = (methCur == NULL) ?
            compiler()->getBSymmgr().LookupAggMember(name, atsCur->getAggregate(), MASK_METHSYM)->asMETHSYM() :
            compiler()->getBSymmgr().LookupNextSym(methCur, atsCur->getAggregate(), MASK_METHSYM)->asMETHSYM();

        if (!methCur) {
            // Find the next type.
            // If we've found some applicable methods in a class then we don't need to look any further.
            if (pmethFirst)
                break;
            atsCur = atsCur->GetBaseClass();
            if (!atsCur)
                break;
            continue;
        }

        // Only look at operators with 1 args.
        if (!methCur->isOperator || methCur->params->size != 1)
            continue;
        ASSERT(methCur->typeVars->size == 0);

        TypeArray * paramsCur = compiler()->getBSymmgr().SubstTypeArray(methCur->params, atsCur);
        TYPESYM * typeParam = paramsCur->Item(0);
        NUBSYM * nubParam;
        METHLIST * pmeth = NULL;

        if (canConvert(arg, typeParam)) {
            pmeth = STACK_ALLOC(METHLIST, 1);
            pmeth->mpwi.Set(methCur, atsCur, BSYMMGR::EmptyTypeArray());
            pmeth->ctypeLift = 0;
            pmeth->fExpanded = false;
            pmeth->params = paramsCur;
        }
        else if (compiler()->FCanLift() && typeParam->IsNonNubValType() &&
            compiler()->getBSymmgr().SubstType(methCur->retType, atsCur)->IsNonNubValType() &&
            canConvert(arg, nubParam = compiler()->getBSymmgr().GetNubType(typeParam)))
        {
            pmeth = STACK_ALLOC(METHLIST, 1);
            pmeth->mpwi.Set(methCur, atsCur, BSYMMGR::EmptyTypeArray());
            pmeth->ctypeLift = 1;
            pmeth->fExpanded = false;
            pmeth->params = compiler()->getBSymmgr().AllocParams(1, (TYPESYM **)&nubParam);
        }

        if (pmeth) {
            // Link it in.
            ASSERT(!*ppmethAdd);
            *ppmethAdd = pmeth;
            pmeth->next = NULL;
            ppmethAdd = &pmeth->next;
        }
    }

    if (!pmethFirst)
        return NULL;

    METHLIST * pmethAmbig1;
    METHLIST * pmethAmbig2;
    METHLIST * pmethBest = FindBestMethod(pmethFirst, info, &pmethAmbig1, &pmethAmbig2);

    if (!pmethBest) {
        // No winner, so its an ambigous call...
        compiler()->Error(tree, ERR_AmbigCall, pmethAmbig1->mpwi, pmethAmbig2->mpwi);
        return newError(tree, NULL);
    }

    if (compiler()->CheckBogus(pmethBest->mpwi.Meth())) {
        compiler()->ErrorRef(tree, ERR_BindToBogus, pmethBest->mpwi);
        return newError(tree, NULL);
    }

    if (pmethBest->mpwi.Meth()->IsDeprecated())
        ReportDeprecated(tree, pmethBest->mpwi);

    NubInfo nin;
    EXPR * exprVal = arg;
    TYPESYM * typePrm = pmethBest->params->Item(0);
    TYPESYM * typeRaw = typePrm;

    if (pmethBest->ctypeLift) {
        // This is a lifted user defined operator.
        typeRaw = typePrm->StripNubs();

        if (!arg->type->isNUBSYM() || !canConvert(arg->type->StripNubs(), typeRaw, NOUDC)) {
            // Convert then lift.
            exprVal = mustConvert(arg, typePrm);
        }

        ASSERT(exprVal->type->isNUBSYM());
        BindNubCondValBin(tree, exprVal, NULL, nin);
        ASSERT(nin.fActive);

        exprVal = nin.Val(0);
        ASSERT(!exprVal->type->isNUBSYM());
    }

    exprVal = mustConvert(exprVal, typeRaw);

    EXPRCALL * call = newExpr(tree, EK_CALL,
        compiler()->getBSymmgr().SubstType(pmethBest->mpwi.Meth()->retType, pmethBest->mpwi.Type()))->asCALL();
    call->object = NULL;
    call->mwi = pmethBest->mpwi;
    call->args = exprVal;

    verifyMethodArgs(call);

    EXPR * exprRes = call;

    if (pmethBest->ctypeLift) {
        // The lifted case.
        TYPESYM * typeRes = exprRes->type;
        if (!typeRes->isNUBSYM())
            typeRes = compiler()->getBSymmgr().GetNubType(typeRes);

        exprRes = BindNubOpRes(tree, typeRes->asNUBSYM(), typeRes, exprRes, nin, true);
    }

    return exprRes;
}


/*
 * Get the name of an operator for error reporting purposes.
 */
PCWSTR FUNCBREC::opName(OPERATOR op)
{
    return CParser::GetTokenInfo((TOKENID) CParser::GetOperatorInfo(op)->iToken)->pszText;
}

/*
 * Report a bad operator types error to the user.
 */
EXPR * FUNCBREC::badOperatorTypesError(BASENODE * tree, EXPR * op1, EXPR * op2, TYPESYM * typeErr)
{
    PCWSTR strOp = opName(tree->Op());

    while (op1->kind == EK_WRAP && op1->asWRAP()->expr)
        op1 = op1->asWRAP()->expr;

    if (op2) {
        while (op2->kind == EK_WRAP && op2->asWRAP()->expr)
            op2 = op2->asWRAP()->expr;
        if (!op1->type->isERRORSYM() && !op2->type->isERRORSYM())
            compiler()->Error(tree, ERR_BadBinaryOps, strOp, op1->type, op2->type);
    }
    else if (!op1->type->isERRORSYM())
        compiler()->Error(tree, ERR_BadUnaryOp, strOp, op1->type);

    return newError(tree, typeErr);
}

/*
 * Report an ambiguous operator types error.
 */
EXPR * FUNCBREC::ambiguousOperatorError(BASENODE * tree, EXPR * op1, EXPR * op2)
{
    // Bad arg types - report error to user.
    if (op2)
        compiler()->Error(tree, ERR_AmbigBinaryOps, opName(tree->Op()), op1->type, op2->type);
    else
        compiler()->Error(tree, ERR_AmbigUnaryOp, opName(tree->Op()), op1->type);
    return newError(tree, NULL);
}

/* Check for a integral comparison operation that can't be right. If one operand
 * is a constant, the other operand has been cast from a smaller integration type, and the
 * constant isn't in the range of the smaller type, the comparison is vacuous, and will
 * always be true or false.
 */
void FUNCBREC::checkVacuousIntegralCompare(BASENODE * tree, EXPR * castOp, EXPR * constOp)
{
    ASSERT(castOp->kind == EK_CAST);
    ASSERT(constOp->kind == EK_CONSTANT);

    do {
        // Ensure this is a cast from one integral/enum type to another.
        if (castOp->flags & (EXF_BOX | EXF_UNBOX))
            break;

        if (!castOp->type->isAGGTYPESYM()) break;
        if (!castOp->asCAST()->p1->type->isAGGTYPESYM()) break;

        AGGTYPESYM * tpDest = castOp->type->underlyingType()->asAGGTYPESYM();
        AGGTYPESYM * tpSrc = castOp->asCAST()->p1->type->underlyingType()->asAGGTYPESYM();

        if (! tpSrc->isSimpleType() || ! tpDest->isSimpleType())
            break;

        // If the cast is a narrowing cast, then it could change the
        // value of the expression, so this test isn't valid                                
        unsigned convertKind = simpleTypeConversions[tpSrc->getPredefType()][tpDest->getPredefType()];
        if ((convertKind & CONV_KIND_MASK) == CONV_KIND_EXP)
            break;

        // The cast meets all the requirements, so check if the constant is in
        // range of the original integral type of the casted expression.
        if (! isConstantInRange(constOp->asCONSTANT(), tpSrc)) {
            compiler()->Error(tree, WRN_VacuousIntegralComp, tpSrc);
            break;
        }

        castOp = castOp->asCAST()->p1;
    } while (castOp->kind == EK_CAST);
}


/***************************************************************************************************
    Convert and constant fold an expression involving I4, U4, I8 or U8 operands. The operands are
    assumed to be already converted to the correct types.
***************************************************************************************************/
EXPR * FUNCBREC::BindIntOp(BASENODE * tree, EXPRKIND kind, uint flags, EXPR * op1, EXPR * op2, PREDEFTYPE ptOp)
{
    ASSERT(
        EK_RELATIONAL_MIN <= kind && kind <= EK_RELATIONAL_MAX ||
        EK_ARITH_MIN <= kind && kind <= EK_ARITH_MAX ||
        EK_BIT_MIN <= kind && kind <= EK_BIT_MAX);
    ASSERT(ptOp == PT_INT || ptOp == PT_UINT || ptOp == PT_LONG || ptOp == PT_ULONG);

    TYPESYM * typeOp = GetReqPDT(ptOp);
    ASSERT(typeOp);
    ASSERT(op1 && op1->type == typeOp);
    ASSERT(!op2 || op2->type == typeOp);
    ASSERT(!op2 == (kind == EK_NEG || kind == EK_UPLUS || kind == EK_BITNOT));

    bool fLong = (ptOp == PT_LONG || ptOp == PT_ULONG);

    // Check for constants.
    EXPR * opConst1 = op1->GetConst();
    EXPR * opConst2 = op2 ? op2->GetConst() : NULL;

    // Zero as second operand
    if (opConst2 && opConst2->isZero(false)) {
        switch (kind) {
        case EK_ADD:
        case EK_SUB:
        case EK_BITOR:
        case EK_BITXOR:
            // Result is op1.
            return AddSideEffects(tree, op1, op2, false);
        case EK_DIV:
        case EK_MOD:
            // Integer division by zero -- an error.
            compiler()->Error(tree, ERR_IntDivByZero);
            return newError(tree, typeOp);
        case EK_MUL:
        case EK_BITAND:
            // Result is zero.
            return AddSideEffects(tree, op2, op1, true);
        default:
            break;
        }
    }

    // Zero as first operand
    if (opConst1 && opConst1->isZero(false)) {
        switch (kind) {
        case EK_ADD:
        case EK_BITOR:
        case EK_BITXOR:
            // These are no-ops.
            return AddSideEffects(tree, op2, op1, true);
        case EK_DIV:
        case EK_MOD:
            if (!opConst2) {
                // Since op2 isn't constant, if op2 is zero at runtime an exception
                // should be thrown.
                break;
            }
            // Fall through
        case EK_MUL:
        case EK_BITAND:
            // Result is zero.
            return AddSideEffects(tree, op1, op2, false);
        case EK_SUB:
            if ((ptOp == PT_INT || ptOp == PT_LONG) && !(flags & EXF_CHECKOVERFLOW)) {
                // Convert (0 - x) to -x.
                kind = EK_NEG;
                op1 = AddSideEffects(tree, op2, op1, true);
                opConst1 = op1->GetConst();
                op2 = NULL;
                opConst2 = NULL;
            }
            break;
        default:
            break;
        }
    }

    // Fold operation if both args are constant.
    if (opConst1 && (!op2 || opConst2)) {
        return fLong ?
            FoldConstI8Op(tree, kind, op1, opConst1, op2, opConst2, ptOp) :
            FoldConstI4Op(tree, kind, op1, opConst1, op2, opConst2, ptOp);
    }

    TYPESYM * typeDest = typeOp;

    switch (kind) {
    case EK_MUL:
        if (opConst1 && (!fLong && opConst1->asCONSTANT()->getVal().iVal == 1 || fLong && *opConst1->asCONSTANT()->getVal().longVal == 1))
            return AddSideEffects(tree, op2, op1, true);
        if (opConst2 && (!fLong && opConst2->asCONSTANT()->getVal().iVal == 1 || fLong && *opConst2->asCONSTANT()->getVal().longVal == 1))
            return AddSideEffects(tree, op1, op2, false);
        break;

    case EK_NEG:
        switch (ptOp) {
        case PT_INT:
        case PT_LONG:
            if (flags & EXF_CHECKOVERFLOW) {
                op2 = op1;
                op1 = newExprZero(tree, typeOp);
                kind = EK_SUB;
            }
            break;
        case PT_UINT:
            if (op1->type->fundType() == FT_U4) {
                typeDest = GetReqPDT(PT_LONG);
                op1 = mustConvertCore(op1, typeDest, tree, NOUDC);
            }
            break;
        case PT_ULONG:
            return badOperatorTypesError(tree, op1, op2);
        default:
            break;
        }
        break;

    case EK_BITOR:
        {
            // Want to warn about code like:
            //        int hi = 1;
            //        int lo = -1;
            //        long value = (((long)hi) << 32) | lo;   // bad
            FUNDTYPE ft1 = FT_NONE;
            FUNDTYPE ft2 = FT_NONE;

            if (op1->kind == EK_CAST) {
                ft1 = op1->asCAST()->p1->type->fundType();
                if (ft1 != FT_I1 && ft1 != FT_I2 && (ft1 != FT_I4 || !fLong))
                    ft1 = FT_NONE;
            }
            if (op2->kind == EK_CAST) {
                ft2 = op2->asCAST()->p1->type->fundType();
                if (ft2 != FT_I1 && ft2 != FT_I2 && (ft2 != FT_I4 || !fLong))
                    ft2 = FT_NONE;
            }
            if (ft1 == ft2)
                break;

            // Don't error if one is a constant within the range of the other.
            if (opConst1 || opConst2) {
                __int64 ln;
                __int64 lnLim;

                if (opConst1) {
                    ASSERT(ft1 == FT_NONE);
                    ln = opConst1->asCONSTANT()->getI64Value();
                    ft1 = ft2;
                }
                else {
                    ASSERT(ft2 == FT_NONE);
                    ln = opConst2->asCONSTANT()->getI64Value();
                }

                // See if ln is in the range of ft1.
                switch (ft1) {
                case FT_I1:
                    lnLim = 0x80;
                    break;
                case FT_I2:
                    lnLim = 0x8000;
                    break;
                default:
                    lnLim = I64(0x0000000080000000);
                    break;
                }
                if (ln >= -lnLim && ln < lnLim)
                    break;
            }

            compiler()->Error(tree, WRN_BitwiseOrSignExtend);
        }
        break;

    default:
        if (EK_RELATIONAL_MIN <= kind && kind <= EK_RELATIONAL_MAX) {
            typeDest = GetReqPDT(PT_BOOL);

            // Give warning if comparing to a constant that isn't in range.  E.g., int i; i < 0x80000000000.
            if (opConst1 && op2->kind == EK_CAST) {
                checkVacuousIntegralCompare(tree, op2, opConst1);
            }
            else if (opConst2 && op1->kind == EK_CAST) {
                checkVacuousIntegralCompare(tree, op1, opConst2);
            }
        }
        break;
    }

    // Allocate the result expression.
    EXPR * exprRes = newExprBinop(tree, kind, typeDest, op1, op2);
    exprRes->flags |= flags;
    ASSERT(!(exprRes->flags & EXF_LVALUE));
    return exprRes;
}


EXPR * FUNCBREC::FoldConstI4Op(BASENODE * tree, EXPRKIND kind, EXPR * op1, EXPR * opConst1, EXPR * op2, EXPR * opConst2, PREDEFTYPE ptOp)
{
    ASSERT(ptOp == PT_INT || ptOp == PT_UINT);
    ASSERT(opConst1->kind == EK_CONSTANT);
    ASSERT(op1->type->isPredefType(ptOp) && op1->type == opConst1->type);
    ASSERT(!op2 && !opConst2 ||
        op2 && opConst2 && opConst2->kind == EK_CONSTANT && op1->type == op2->type && op1->type == opConst2->type);

    bool fSigned = (ptOp == PT_INT);

    // Get the operands
    uint u1 = opConst1->asCONSTANT()->getVal().uiVal;
    uint u2 = opConst2 ? opConst2->asCONSTANT()->getVal().uiVal : 0;
    uint uRes;

    ASSERT(sizeof(uint) == 4);

    // The sign bit.
    const uint uSign = 0x80000000;

#define ENSURE_CHECKED(s, u) \
    if (checked.constant && (fSigned ? !(s) : !(u))) \
        compiler()->Error(tree, ERR_CheckedOverflow); \
    else (void)0
#define ENSURE_CHECKED1(s) \
    if (checked.constant && !(s)) \
        compiler()->Error(tree, ERR_CheckedOverflow); \
    else (void)0

    // Do the operation.
    switch (kind) {
    case EK_ADD:
        uRes = u1 + u2;
        // For signed, we want either sign(u1) != sign(u2) or sign(u1) == sign(uRes).
        // For unsigned, the result should be at least as big as either operand (if it's bigger than
        // one, it will be bigger than the other as well).
        ENSURE_CHECKED(((u1 ^ u2) | (u1 ^ uRes ^ uSign)) & uSign, uRes >= u1);
        break;

    case EK_SUB:
        uRes = u1 - u2;
        // For signed, we want either sign(u1) == sign(u2) or sign(u1) == sign(uRes).
        // For unsigned, the result should be no bigger than the first operand.
        ENSURE_CHECKED(((u1 ^ u2 ^ uSign) | (u1 ^ uRes ^ uSign)) & uSign, uRes <= u1);
        break;

    case EK_MUL:
        // Multiply mod 2^32 doesn't depend on signed vs unsigned.
        uRes = u1 * u2;
        // Note that divide depends on signed-ness.
        // For signed, the first check detects 0x80000000 / 0xFFFFFFFF == 0x80000000.
        // This test needs to come first to avoid an integer overflow exception - yes we get this
        // in native code.
        ENSURE_CHECKED((u2 != uRes || u1 == 1) && (int)uRes / (int)u1 == (int)u2, uRes / u1 == u2);
        break;

    case EK_DIV:
        ASSERT(u2 != 0); // Caller should have handled this.
        if (!fSigned)
            uRes = u1 / u2;
        else if (u2 != (uint)~0)
            uRes = (uint)((int)u1 / (int)u2);
        else {
            uRes = (uint)-(int)u1;
            ENSURE_CHECKED1(u1 != uSign);
        }
        break;

    case EK_MOD:
        ASSERT(u2 != 0); // Caller should have handled this.
        if (!fSigned)
            uRes = u1 % u2;
        else if (u2 != (uint)~0)
            uRes = (uint)((int)u1 % (int)u2);
        else
            uRes = 0;
        break;

    case EK_NEG:
        if (!fSigned) {
            // Special case: a unary minus promotes a uint to a long
            CONSTVAL cv;
            cv.longVal = (__int64 *) allocator->Alloc(sizeof(__int64));
            *cv.longVal = -(__int64)u1;
            return newExprConstant(tree, GetReqPDT(PT_LONG), cv);
        }

        uRes = (uint)-(int)u1;
        ENSURE_CHECKED1(u1 != uSign);
        break;

    case EK_UPLUS:   uRes = u1; break;
    case EK_BITAND:  uRes = u1 & u2; break;
    case EK_BITOR:   uRes = u1 | u2; break;
    case EK_BITXOR:  uRes = u1 ^ u2; break;
    case EK_BITNOT:  uRes = ~u1; break;
    case EK_EQ:      uRes = (u1 == u2); break;
    case EK_NE:      uRes = (u1 != u2); break;
    case EK_LE:      uRes = fSigned ? (int)u1 <= (int)u2 : u1 <= u2; break;
    case EK_LT:      uRes = fSigned ? (int)u1 <  (int)u2 : u1 <  u2; break;
    case EK_GE:      uRes = fSigned ? (int)u1 >= (int)u2 : u1 >= u2; break;
    case EK_GT:      uRes = fSigned ? (int)u1 >  (int)u2 : u1 >  u2; break;
    default:
        VSFAIL("Unknown op");
        uRes = 0;
        break;
    }

#undef ENSURE_CHECKED
#undef ENSURE_CHECKED1

    TYPESYM * typeDest = GetOptPDT(EK_RELATIONAL_MIN <= kind && kind <= EK_RELATIONAL_MAX ? PT_BOOL : ptOp);
    ASSERT(typeDest);

    // Allocate the result node.
    EXPR * exprRes = newExprConstant(tree, typeDest, ConstValInit(uRes));
    exprRes = AddSideEffects(tree, exprRes, op2, true);
    return AddSideEffects(tree, exprRes, op1, true);
}


EXPR * FUNCBREC::FoldConstI8Op(BASENODE * tree, EXPRKIND kind, EXPR * op1, EXPR * opConst1, EXPR * op2, EXPR * opConst2, PREDEFTYPE ptOp)
{
    ASSERT(ptOp == PT_LONG || ptOp == PT_ULONG);
    ASSERT(opConst1->kind == EK_CONSTANT);
    ASSERT(op1->type->isPredefType(ptOp) && op1->type == opConst1->type);
    ASSERT(!op2 && !opConst2 ||
        op2 && opConst2 && opConst2->kind == EK_CONSTANT && op1->type == op2->type && op1->type == opConst2->type);

    bool fSigned = (ptOp == PT_LONG);

    // Get the operands
    unsigned __int64 u1 = *opConst1->asCONSTANT()->getVal().longVal;
    unsigned __int64 u2 = opConst2 ? *opConst2->asCONSTANT()->getVal().longVal : 0;
    unsigned __int64 uRes = 0;
    bool fRes = false;

    ASSERT(sizeof(unsigned __int64) == 8);

    // The sign bit.
    const unsigned __int64 uSign = UI64(0x8000000000000000);

#define ENSURE_CHECKED(s, u) \
    if (checked.constant && (fSigned ? !(s) : !(u))) \
        compiler()->Error(tree, ERR_CheckedOverflow); \
    else (void)0
#define ENSURE_CHECKED1(s) \
    if (checked.constant && !(s)) \
        compiler()->Error(tree, ERR_CheckedOverflow); \
    else (void)0

    // Do the operation.
    switch (kind) {
    case EK_ADD:
        uRes = u1 + u2;
        // For signed, we want either sign(u1) != sign(u2) or sign(u1) == sign(uRes).
        // For unsigned, the result should be at least as big as either operand (if it's bigger than
        // one, it will be bigger than the other as well).
        ENSURE_CHECKED(((u1 ^ u2) | (u1 ^ uRes ^ uSign)) & uSign, uRes >= u1);
        break;

    case EK_SUB:
        uRes = u1 - u2;
        // For signed, we want either sign(u1) == sign(u2) or sign(u1) == sign(uRes).
        // For unsigned, the result should be no bigger than the first operand.
        ENSURE_CHECKED(((u1 ^ u2 ^ uSign) | (u1 ^ uRes ^ uSign)) & uSign, uRes <= u1);
        break;

    case EK_MUL:
        // Multiply mod 2^32 doesn't depend on signed vs unsigned.
        uRes = u1 * u2;
        // Note that divide depends on signed-ness.
        // For signed, the first check detects 0x8000000000000000 / 0xFFFFFFFFFFFFFFFF == 0x8000000000000000.
        ENSURE_CHECKED((u2 != uRes || u1 == 1) && (__int64)uRes / (__int64)u1 == (__int64)u2, uRes / u1 == u2);
        break;

    case EK_DIV:
        ASSERT(u2 != 0); // Caller should have handled this.
        if (!fSigned)
            uRes = u1 / u2;
        else if (u2 != ~((unsigned __int64)0))
            uRes = (unsigned __int64)((__int64)u1 / (__int64)u2);
        else {
            uRes = (unsigned __int64)-(__int64)u1;
            ENSURE_CHECKED1(u1 != uSign);
        }
        break;

    case EK_MOD:
        ASSERT(u2 != 0); // Caller should have handled this.
        if (!fSigned)
            uRes = u1 % u2;
        else if (u2 != ~((unsigned __int64)0))
            uRes = (unsigned __int64)((__int64)u1 % (__int64)u2);
        else
            uRes = 0;
        break;

    case EK_NEG:
        if (!fSigned) {
            //You can't do this!
            return badOperatorTypesError(tree, op1, op2);
        }

        uRes = (unsigned __int64)-(__int64)u1;
        ENSURE_CHECKED1(u1 != uSign);
        break;

    case EK_UPLUS:   uRes = u1; break;
    case EK_BITAND:  uRes = u1 & u2; break;
    case EK_BITOR:   uRes = u1 | u2; break;
    case EK_BITXOR:  uRes = u1 ^ u2; break;
    case EK_BITNOT:  uRes = ~u1; break;
    case EK_EQ:      fRes = (u1 == u2); break;
    case EK_NE:      fRes = (u1 != u2); break;
    case EK_LE:      fRes = fSigned ? (__int64)u1 <= (__int64)u2 : u1 <= u2; break;
    case EK_LT:      fRes = fSigned ? (__int64)u1 <  (__int64)u2 : u1 <  u2; break;
    case EK_GE:      fRes = fSigned ? (__int64)u1 >= (__int64)u2 : u1 >= u2; break;
    case EK_GT:      fRes = fSigned ? (__int64)u1 >  (__int64)u2 : u1 >  u2; break;
    default:
        VSFAIL("Unknown op");
        uRes = 0;
        break;
    }

#undef ENSURE_CHECKED
#undef ENSURE_CHECKED1

    // Allocate the result node.
    TYPESYM * typeDest;
    CONSTVAL cv;

    if (EK_RELATIONAL_MIN <= kind && kind <= EK_RELATIONAL_MAX) {
        cv.iVal = fRes;
        typeDest = GetReqPDT(PT_BOOL);
    }
    else {
        cv.ulongVal = (unsigned __int64 *) allocator->Alloc(sizeof(unsigned __int64));
        *cv.ulongVal = uRes;
        typeDest = GetOptPDT(ptOp);
        ASSERT(typeDest);
    }

    // Allocate the result node.
    EXPR * exprRes = newExprConstant(tree, typeDest, cv);
    exprRes = AddSideEffects(tree, exprRes, op2, true);
    return AddSideEffects(tree, exprRes, op1, true);
}


/*
 * Bind an float/double operator: +, -, *, /, %, <, >, <=, >=, ==, !=. If both operations are constants, the result
 * will be a constant also. op2 can be null for a unary operator. The operands are assumed
 * to be already converted to the correct type.
 */
// We have an intentional divide by 0 there, so disable the warning...
#if _MSC_VER
#pragma warning( disable : 4723 )
#endif
EXPR * FUNCBREC::bindFloatOp(BASENODE * tree, EXPRKIND kind, unsigned flags, EXPR * op1, EXPR * op2)
{
    ASSERT(
        EK_RELATIONAL_MIN <= kind && kind <= EK_RELATIONAL_MAX ||
        EK_ARITH_MIN <= kind && kind <= EK_ARITH_MAX);
    ASSERT(!op2 || op1->type == op2->type);
    ASSERT(op1->type->isPredefType(PT_FLOAT) || op1->type->isPredefType(PT_DOUBLE));

    EXPR * exprRes;
    EXPR * opConst1 = op1->GetConst();
    EXPR * opConst2 = op2 ? op2->GetConst() : NULL;

    // Check for constants and fold them.
    if (opConst1 && (!op2 || opConst2)) {
        // Get the operands
        double d1 = * opConst1->asCONSTANT()->getVal().doubleVal;
        double d2 = opConst2 ? * opConst2->asCONSTANT()->getVal().doubleVal : 0.0;
        double result = 0;      // if isBoolResult is false
        bool result_b = false;  // if isBoolResult is true

        // Do the operation.
        switch (kind) {
        case EK_ADD:  result = d1 + d2; break;
        case EK_SUB:  result = d1 - d2; break;
        case EK_MUL:  result = d1 * d2; break;
        case EK_DIV:  result = d1 / d2; break;
        case EK_NEG:  result = -d1; break;
        case EK_UPLUS: result = d1; break;
        case EK_MOD:
            if (!_finite(d2) && !_isnan(d2)) {
                if (!_finite(d1) && !_isnan(d1)) {
                    double d = 0.0;
                    result = 0.0 / d;
                } else {
                    result = d1;
                }
            } else {
                result = fmod(d1, d2);
            }
            break;
        case EK_EQ:   result_b = (d1 == d2); break;
        case EK_NE:   result_b = (d1 != d2); break;
        case EK_LE:   result_b = (d1 <= d2); break;
        case EK_LT:   result_b = (d1 < d2);  break;
        case EK_GE:   result_b = (d1 >= d2); break;
        case EK_GT:   result_b = (d1 > d2);  break;
        default:      ASSERT(0); result = 0.0;  // unexpected operation.
        }

        TYPESYM * typeDest;
        CONSTVAL cv;

        // Allocate the result node.
        if (EK_RELATIONAL_MIN <= kind && kind <= EK_RELATIONAL_MAX) {
            cv.iVal = result_b;
            typeDest = GetReqPDT(PT_BOOL);
        }
        else {
            cv.doubleVal = (double *) allocator->Alloc(sizeof(double));

            // NaN has some implementation defined bits that differ between platforms.
            // Normalize it to produce identical images accross all platforms
            if (_isnan(result)) {
                * (unsigned __int64 *) cv.doubleVal = UI64(0xFFF8000000000000);
                _ASSERTE(_isnan(* cv.doubleVal));
            }
            else {
                * cv.doubleVal = result;
            }

            typeDest = op1->type;
        }
        exprRes = newExprConstant(tree, typeDest, cv);
        exprRes = AddSideEffects(tree, exprRes, op2, true);
        exprRes = AddSideEffects(tree, exprRes, op1, true);
    }
    else {
        // Allocate the result expression.
        TYPESYM * typeDest = (EK_RELATIONAL_MIN <= kind && kind <= EK_RELATIONAL_MAX) ? GetReqPDT(PT_BOOL) : op1->type;

        exprRes = newExprBinop(tree, kind, typeDest, op1, op2);
        flags &= ~EXF_CHECKOVERFLOW;
        exprRes->flags |= flags;
    }

    return exprRes;
}

#if _MSC_VER
#pragma warning( default : 4723 )
#endif

bool FUNCBREC::HasSelfCompare(TYPESYM * type, EXPRKIND kind)
{

    if (!type || !type->isAGGTYPESYM())
        return false;

    AGGSYM * agg = type->getAggregate();

    if (!agg->fSelfCmpValid) {
        compiler()->EnsureState(agg);
        if (!agg->fHasSelfEq)
            agg->fHasSelfEq = HasSelfCompare(agg->baseClass, EK_EQ);
        if (!agg->fHasSelfEq)
            agg->fHasSelfNe = HasSelfCompare(agg->baseClass, EK_NE);
        agg->fSelfCmpValid = true;
    }

    return (kind == EK_EQ) ? agg->fHasSelfEq : agg->fHasSelfNe;
}


EXPR * FUNCBREC::BindUserBoolOp(BASENODE * tree, EXPRKIND kind, EXPR * call)
{
    ASSERT(kind == EK_LOGAND || kind == EK_LOGOR);

    if (call->kind != EK_CALL) {
        ASSERT(call->kind == EK_ERROR);
        return call;
    }

    TYPESYM * typeRet = call->type;

    ASSERT(call->asCALL()->mwi.Meth()->params->size == 2);
    if (!compiler()->getBSymmgr().SubstEqualTypes(typeRet, call->asCALL()->mwi.Meth()->params->Item(0), typeRet) ||
        !compiler()->getBSymmgr().SubstEqualTypes(typeRet, call->asCALL()->mwi.Meth()->params->Item(1), typeRet))
    {
        compiler()->Error(tree, ERR_BadBoolOp, call->asCALL()->mwi);
        return newError(tree, typeRet);
    }

    ASSERT(call->asCALL()->args->asBIN()->p1->type == typeRet);
    ASSERT(call->asCALL()->args->asBIN()->p2->type == typeRet);

    EXPR * expr1 = call->asCALL()->args->asBIN()->p1;
    EXPR * exprWrap = expr1;

    exprWrap = newExprWrap(expr1, TK_SHORTLIVED);
    call->asCALL()->args->asBIN()->p1 = exprWrap;

    EXPR * callT = bindUDUnop(tree, EK_TRUE, exprWrap);
    EXPR * callF = bindUDUnop(tree, EK_FALSE, exprWrap);

    if (!callT || !callF) {
        compiler()->Error(tree, ERR_MustHaveOpTF, typeRet);
        return newError(tree, typeRet);
    }

    callT = mustConvert(callT, GetReqPDT(PT_BOOL));
    callF = mustConvert(callF, GetReqPDT(PT_BOOL));

    EXPRUSERLOGOP * rval = newExpr(tree, EK_USERLOGOP, typeRet)->asUSERLOGOP();
    rval->opX = expr1;
    rval->callTF = kind == EK_LOGAND ? callF : callT;
    rval->callOp = call;
    rval->flags |= EXF_ASSGOP;

    return rval;
}


/*
 * Bind a constant cast to or from decimal. Return null if cast can't be done.
 */
EXPR * FUNCBREC::bindDecimalConstCast(BASENODE * tree, TYPESYM * destType, TYPESYM * srcType, EXPRCONSTANT * src)
{
    TYPESYM * typeDecimal = compiler()->GetOptPredefType(PT_DECIMAL);
    HRESULT hr = NOERROR;
    CONSTVAL cv;

    if (!typeDecimal)
        return NULL;

    if (destType == typeDecimal) {
        // Casting to decimal.

        FUNDTYPE ftSrc = srcType->fundType();
        DECIMAL result;

        switch (ftSrc) {

        case FT_I1:
        case FT_I2:
        case FT_I4:
            hr = VarDecFromI4(src->getVal().iVal, &result); break;
        case FT_U1:
        case FT_U2:
        case FT_U4:
            hr = VarDecFromUI4(src->getVal().uiVal, &result); break;
        case FT_R4:
            hr = VarDecFromR4((float) (* src->getVal().doubleVal), &result); break;
        case FT_R8:
            hr = VarDecFromR8(* src->getVal().doubleVal, &result); break;
        case FT_U8:
        case FT_I8:
            /* I8,U8 -> decimal. No API for this, do it manually. */
            __int64 srcLong;

            srcLong = *src->getVal().longVal;

            DECIMAL_SCALE(result) = 0;
            DECIMAL_SIGN(result) = 0;
            if (srcLong < 0 && ftSrc == FT_I8) { // Only worry about the sign if it is signed
                DECIMAL_SIGN(result) |= DECIMAL_NEG;
                srcLong = -srcLong;
            }

            DECIMAL_MID32(result) = (ULONG)(srcLong>>32);
            DECIMAL_LO32(result) = (ULONG)srcLong;
            DECIMAL_HI32(result) = 0;
            break;

        default:
            hr = E_FAIL;  // Not supported cast.
        }

        if (FAILED(hr))
            return NULL;

        cv.decVal = (DECIMAL *) allocator->Alloc(sizeof(DECIMAL));
        * cv.decVal = result;
        return newExprConstant(tree, typeDecimal, cv);
    }

    if (srcType == typeDecimal) {
        // Casting from decimal
        DECIMAL decTrunc;

        FUNDTYPE ftDest = destType->fundType();

        if (ftDest != FT_R4 && ftDest != FT_R8) {
            // First truncate to integer.
            hr = VarDecFix(src->getVal().decVal, &decTrunc);
        }
        if (SUCCEEDED(hr)) {
            switch (ftDest) {
            case FT_I1:
                CHAR c;
                hr = VarI1FromDec(&decTrunc, &c);
                cv.iVal = c;
                break;
            case FT_U1:
                BYTE b;
                hr = VarUI1FromDec(&decTrunc, &b);
                cv.uiVal = b;
                break;
            case FT_I2:
                SHORT s;
                hr = VarI2FromDec(&decTrunc, &s);
                cv.iVal = s;
                break;
            case FT_U2:
                USHORT u;
                hr = VarUI2FromDec(&decTrunc, &u);
                cv.uiVal = u;
                break;
            case FT_I4:
                LONG i;
                hr = VarI4FromDec(&decTrunc, &i);
                cv.iVal = i;
                break;
            case FT_U4:
                ULONG ui;
                hr = VarUI4FromDec(&decTrunc, &ui);
                cv.uiVal = ui;
                break;
            case FT_I8:
                /* convert decimal to long -- no API for this (sigh) */
                if (SUCCEEDED(hr)) {
                    ASSERT(DECIMAL_SCALE (decTrunc) == 0);

                    // Are we out of range of a long. The most negative long makes this test more complex than you would think.
                    if (DECIMAL_HI32(decTrunc) != 0 ||
                        ((DECIMAL_MID32(decTrunc) & 0x80000000) != 0 &&
                        (DECIMAL_MID32(decTrunc) != 0x80000000 || DECIMAL_LO32(decTrunc) != 0 || DECIMAL_SIGN(decTrunc))))
                    {
                        hr = E_FAIL;  // Out of range.
                    }
                    else {
                        __int64 val64 = ((ULONGLONG)DECIMAL_MID32(decTrunc) << 32) | DECIMAL_LO32(decTrunc);
                        if (DECIMAL_SIGN(decTrunc)) {
                            val64 = -val64;
                        }
                        cv.longVal = (__int64 *) allocator->Alloc(sizeof(__int64));
                        *cv.longVal = val64;
                    }
                }
                break;

            case FT_U8:
                /* convert decimal to ulong -- no API for this (sigh) */

                if (SUCCEEDED(hr)) {
                    ASSERT(DECIMAL_SCALE(decTrunc) == 0);

                    // Are we out of range of a ulong.
                    if (DECIMAL_HI32(decTrunc) != 0 || DECIMAL_SIGN(decTrunc))
                    {
                        hr = E_FAIL;  // Out of range.
                    }
                    else {
                        cv.ulongVal = (unsigned __int64 *) allocator->Alloc(sizeof(unsigned __int64));
                        *cv.ulongVal = ((ULONGLONG)DECIMAL_MID32(decTrunc) << 32) | DECIMAL_LO32(decTrunc);
                    }
                }
                break;

            case FT_R4:
                float f;
                hr = VarR4FromDec(src->getVal().decVal, &f);
                cv.doubleVal = (double *) allocator->Alloc(sizeof(double));
                * cv.doubleVal = f;
                break;

            case FT_R8:
                double d;
                hr = VarR8FromDec(src->getVal().decVal, &d);
                cv.doubleVal = (double *) allocator->Alloc(sizeof(double));
                * cv.doubleVal = d;
                break;

            default:
                hr = E_FAIL;  // Not supported cast.
            }
        }

        if (FAILED(hr))
            return NULL;

        return newExprConstant(tree, destType, cv);
    }

    return NULL;
}


// A false return means not to process the expr any further - it's totally out
// of place. For example - a method group or an anonymous method.
bool FUNCBREC::checkLvalue(EXPR * expr, bool isAssignment)
{
    if (!expr->isOK())
        return false;

    if (expr->flags & EXF_LVALUE) {
        // otherwise, even if it is an lvalue, it may be an abstract prop call, which is also illegal...
        if (expr->kind == EK_PROP) {
            if ((expr->flags & EXF_BASECALL) && expr->asPROP()->mwtSet.Meth()->isAbstract)
                compiler()->Error(expr->tree, ERR_AbstractBaseCall, expr->asPROP()->mwtSet);
            else if (!compiler()->clsDeclRec.CheckAccess(expr->asPROP()->mwtSet.Meth(), expr->asPROP()->mwtSet.Type(), parentDecl, NULL)) {
                // Not accessible so give an error
                compiler()->Error(expr->tree, ERR_InaccessibleSetter, expr->asPROP()->pwtSlot);
            }
        }
        markFieldAssigned(expr);
        OnPossibleAssignmentToArg(expr->tree, expr);
        return true;
    }

    switch (expr->kind) {
    case EK_PROP:
        if (!isAssignment) {
            // passing a property as ref or out
            compiler()->Error(expr->tree, ERR_RefProperty);
            return true;
        }
        if (!expr->asPROP()->mwtSet) {
            // Assigning to a property without a setter.
            compiler()->Error(expr->tree, ERR_AssgReadonlyProp, expr->asPROP()->pwtSlot);
            return true;
        }
        break;

    case EK_ARRLEN:
        if (!isAssignment) {
            // passing a property as ref or out
            compiler()->Error(expr->tree, ERR_RefProperty);
        }
        else {
            // Special case, the length property of an array
            compiler()->Error(expr->tree, ERR_AssgReadonlyProp,
                compiler()->getBSymmgr().LookupAggMember(compiler()->namemgr->GetPredefName(PN_LENGTH),
                    compiler()->GetReqPredefAgg(PT_ARRAY), MASK_ALL));
        }
        return true;

    case EK_ANONMETH:
    case EK_CONSTANT:
        compiler()->Error(expr->tree, isAssignment ? ERR_AssgLvalueExpected : ERR_RefLvalueExpected);
        return false;

    case EK_MEMGRP:
        compiler()->Error(expr->tree, isAssignment ? ERR_AssgReadonlyLocalCause : ERR_RefReadonlyLocalCause,
            expr->asMEMGRP()->name, ErrArgIds(IDS_MethodGroup));
        return false;

    default:
        break;
    }

    bool isNested = false; // Did we recurse on a field or property to give a better error

    // We have a lvalue failure. Was the reason because this field
    // was marked readonly? Give special messages for this case..
    for (EXPR * walk = expr; ; isNested = true) {
        if (walk->kind == EK_LOCAL) {
            int cause = 0;
            if (walk->asLOCAL()->local->isForeach)
                cause = IDS_FOREACHLOCAL;
            else if (walk->asLOCAL()->local->isUsing)
                cause = IDS_USINGLOCAL;
            else if (walk->asLOCAL()->local->isFixed)
                cause = IDS_FIXEDLOCAL;

            if (cause) {
                static const int ReadOnlyLocalWCauseErrors [] = {
                    ERR_RefReadonlyLocalCause,
                    ERR_AssgReadonlyLocalCause,
                    ERR_RefReadonlyLocal2Cause,
                    ERR_AssgReadonlyLocal2Cause };
                int err = ReadOnlyLocalWCauseErrors[isNested << 1 | (isAssignment & 1)];

                compiler()->Error(expr->tree, err, walk->asLOCAL()->local->name, ErrArgIds(cause));
            } else {
                static const int ReadOnlyLocalErrors [] = {
                    ERR_RefReadonlyLocal,
                    ERR_AssgReadonlyLocal,
                    ERR_RefReadonlyLocal2,
                    ERR_AssgReadonlyLocal2 };
                int err = ReadOnlyLocalErrors[isNested << 1 | (isAssignment & 1)];

                compiler()->Error(expr->tree, err, walk->asLOCAL()->local->name);
            }
            return true;
        }

        EXPR * object = NULL;
        bool isReadOnly = false;
        bool isStatic = false;

        if (walk->kind == EK_PROP) {
            ASSERT(walk->asPROP()->mwtSet);
            object = walk->asPROP()->object;
        } else if (walk->kind == EK_FIELD) {
            isReadOnly = walk->asFIELD()->fwt.Field()->isReadOnly;
            isStatic = walk->asFIELD()->fwt.Field()->isStatic;
            if (!isStatic)
                object = walk->asFIELD()->object;
        }

        if (isReadOnly) {
            if (isNested) {
                static const int ReadOnlyNestedErrors [] = {
                    ERR_RefReadonly2,
                    ERR_AssgReadonly2,
                    ERR_RefReadonlyStatic2,
                    ERR_AssgReadonlyStatic2 };
                int err = ReadOnlyNestedErrors[isStatic << 1 | (isAssignment & 1)];

                compiler()->Error(expr->tree, err, walk->asFIELD()->fwt);
            } else {
                static const int ReadOnlyErrors [] = {
                    ERR_RefReadonly,
                    ERR_AssgReadonly,
                    ERR_RefReadonlyStatic,
                    ERR_AssgReadonlyStatic };
                int err = ReadOnlyErrors[isStatic << 1 | (isAssignment & 1)];

                compiler()->Error(expr->tree, err);
            }
            return true;
        }

        if (object && object->type->isStructOrEnum()) {
            if (object->kind == EK_CALL || object->kind == EK_PROP) {
                // assigning to RHS of method or property getter returning a value-type on the stack or
                // passing RHS of method or property getter returning a value-type on the stack, as ref or out
                compiler()->Error(object->tree, ERR_ReturnNotLValue, object->GetSymWithType());
                return true;
            }
            if (object->kind == EK_CAST) {
                // An unboxing conversion.
                compiler()->Error(object->tree, ERR_UnboxNotLValue);
                return true;
            }
        }

        // everything else
        if (object && !(object->flags & EXF_LVALUE) && (walk->kind == EK_FIELD || (!isNested && walk->kind == EK_PROP))) {
            ASSERT(object->type->isStructOrEnum());
            walk = object;
        }
        else {
            compiler()->Error(expr->tree, isAssignment ? ERR_AssgLvalueExpected : ERR_RefLvalueExpected);
            return true;
        }
    }
}

// Sets the isAssigned bit
void FUNCBREC::markFieldAssigned(EXPR * expr)
{
    if (expr->kind == EK_FIELD && (expr->flags & EXF_LVALUE)) {
        EXPRFIELD *field;

        do {
            field = expr->asFIELD();
            field->fwt.Field()->isAssigned = true;
            expr = field->object;
        } while (field->fwt.Field()->getClass()->IsStruct() && !field->fwt.Field()->isStatic && expr && expr->kind == EK_FIELD);
    }
}


void FUNCBREC::checkUnsafe(BASENODE * tree, TYPESYM * type, int errCode, ErrArg *pArg)
{
    ASSERT((errCode != ERR_SizeofUnsafe) || pArg);
    if (type == NULL || type->isUnsafe()) {
        if (!isUnsafeContext() && !unsafeErrorGiven) {
            unsafeErrorGiven = true;
            if (pArg)
                compiler()->Error(tree, errCode, *pArg);
            else
                compiler()->Error(tree, errCode);
        } else if (info != NULL && info->unsafeTree == NULL && tree != NULL) {
            // If this is unsafe code and we didn't already report an error
            // save this location for later in case this method turns out to
            // be an iterator
            info->unsafeTree = tree;
        }
    }
}


// Taking the reference of a field is suspect if the type is marshalbyref.
// Also checks for passing volatile field by-ref.
void FUNCBREC::CheckFieldRef(EXPR * expr)
{
    if (!expr || expr->kind != EK_FIELD)
        return;

    EXPRFIELD * exprFld = expr->asFIELD();

    // We ignore it if it's static or (an optional cast of) the this pointer.
    if (exprFld->fwt.Field()->getClass()->isMarshalByRef && !exprFld->fwt.Field()->isStatic && !isCastOptThis(exprFld->object))
        compiler()->Error(exprFld->tree, WRN_ByRefNonAgileField, exprFld->fwt, ErrArgRefOnly(exprFld->fwt.Field()->getClass()));
    DebugOnly(exprFld->fCheckedMarshalByRef = true);

    if (exprFld->fwt.Field()->isVolatile)
        compiler()->Error(exprFld->tree, WRN_VolatileByRef, exprFld->fwt);
}


bool FUNCBREC::CheckFieldUse(EXPR * expr, bool fReport)
{
    if (!expr || expr->kind != EK_FIELD)
        return true;

    EXPRFIELD * exprFld = expr->asFIELD();

    DebugOnly(exprFld->fCheckedMarshalByRef = true);

    // We ignore it if it's static or (an optional cast of) the this pointer.
    // It must also be something we could use a ldflda instruction on -
    // a valuetype or type variable.
    if (exprFld->fwt.Field()->getClass()->isMarshalByRef && !exprFld->fwt.Field()->isStatic && !isCastOptThis(exprFld->object) &&
        (exprFld->type->isStructOrEnum() || exprFld->type->isTYVARSYM()))
    {
        if (fReport)
            compiler()->Error(exprFld->tree, WRN_CallOnNonAgileField, exprFld->fwt, ErrArgRefOnly(exprFld->fwt.Field()->getClass()));
        return false;
    }

    return true;
}


inline void FUNCBREC::noteReference(EXPR * op)
{

    MEMBVARSYM * field;

    if (op->kind == EK_LOCAL && !op->asLOCAL()->local->slot.isReferenced) {
        op->asLOCAL()->local->slot.isReferenced = true;
        unreferencedVarCount--;
    } else if (op->kind == EK_FIELD && !(field = op->asFIELD()->fwt.Field())->isReferenced) {
        field->isReferenced = true;
    }
}

EXPR * FUNCBREC::bindPtrToString(BASENODE * tree, EXPR * string)
{
    TYPESYM * typeRet = compiler()->getBSymmgr().GetPtrType(GetReqPDT(PT_CHAR));

    if (!compiler()->getBSymmgr().methStringOffset) {
        NAME * name = compiler()->namemgr->GetPredefName(PN_OFFSETTOSTRINGDATA);
        AGGTYPESYM * helpers = GetOptPDT(PT_RUNTIMEHELPERS);
        if (helpers == NULL)
            return newError(tree, typeRet);

        MemberLookup mem;
        if (!mem.Lookup(compiler(), helpers, NULL, parentDecl, name, 0, MemLookFlags::UserCallable) ||
            !mem.SymFirst()->isPROPSYM() || !mem.SymFirst()->asPROPSYM()->methGet)
        {
            compiler()->Error(tree, ERR_MissingPredefinedMember, helpers, name);
            return newError(tree, typeRet);
        }
        mem.ReportWarnings(tree);
        compiler()->getBSymmgr().methStringOffset = mem.SymFirst()->asPROPSYM()->methGet;
    }

    CheckFieldUse(string);
    return newExprBinop(tree, EK_ADDR, typeRet, string, NULL);
}


EXPR * FUNCBREC::BindPtrToArray(BASENODE * tree, EXPRLOCAL * exprLoc, EXPR * array)
{
    TYPESYM * typeElem = array->type->asARRAYSYM()->elementType();
    TYPESYM * typePtrElem = compiler()->getBSymmgr().GetPtrType(typeElem);

    // element must be unmanaged...
    if (isManagedType(typeElem)) {
        compiler()->Error(tree, ERR_ManagedAddr, typeElem);
    }

    SetExternalRef(typeElem);

    EXPR * test = NULL;
    EXPR * wrapArray = newExprWrap(array, TK_SHORTLIVED);
    wrapArray->asWRAP()->doNotFree = true;
    EXPR * save = newExprBinop(tree, EK_SAVE, array->type, array, wrapArray);
    EXPR * nullTest = newExprBinop(tree, EK_NE, GetReqPDT(PT_BOOL), save, newExprConstant(tree, wrapArray->type, ConstValInit(0)));
    EXPR * lenTest;

    if (array->type->asARRAYSYM()->rank == 1) {
        EXPR * len = newExprBinop(tree, EK_ARRLEN, GetReqPDT(PT_INT), wrapArray, NULL);
        lenTest = newExprBinop(tree, EK_NE, GetReqPDT(PT_BOOL), len, newExprConstant(tree, GetReqPDT(PT_INT), ConstValInit(0)));
    }
    else {
        EXPR * call = BindPredefMethToArgs(NULL, PN_GETLENGTH, GetReqPDT(PT_ARRAY), wrapArray, NULL);
        lenTest = newExprBinop(tree, EK_NE, GetReqPDT(PT_BOOL), call, newExprConstant(tree, GetReqPDT(PT_INT), ConstValInit(0)));
    }

    test = newExprBinop(tree, EK_LOGAND, GetReqPDT(PT_BOOL), nullTest, lenTest);

    EXPR * list = NULL;
    EXPR ** pList = &list;
    for (int cc = 0; cc < array->type->asARRAYSYM()->rank; cc++) {
        newList(newExprConstant(tree, GetReqPDT(PT_INT), ConstValInit(0)), &pList);
    }
    ASSERT(list);

    noteReference(exprLoc);

    EXPR * exprAddr = newExprBinop(tree, EK_ADDR, typePtrElem, newExprBinop(tree, EK_ARRINDEX, typeElem, wrapArray, list), NULL);
    exprAddr->flags |= EXF_ADDRNOCONV;
    exprAddr = mustConvert(exprAddr, exprLoc->type, NOUDC);
    exprAddr = newExprBinop(tree, EK_ASSG, exprLoc->type, exprLoc, exprAddr);
    exprAddr->flags |= EXF_ASSGOP;
    exprAddr = newExprBinop(NULL, EK_SEQREV, exprLoc->type, exprAddr, newExprWrap(wrapArray, TK_SHORTLIVED)); // free the temp

    EXPR * exprNull = newExprZero(NULL, exprLoc->type);
    exprNull = newExprBinop(NULL, EK_ASSG, exprLoc->type, exprLoc, exprNull);
    exprNull->flags |= EXF_ASSGOP;

    EXPR * exprRes = newExprBinop(NULL, EK_BINOP, NULL, exprAddr, exprNull);
    return newExprBinop(tree, EK_QMARK, exprLoc->type, test, exprRes);
}


bool FUNCBREC::localDeclaredOutsideTry(LOCVARSYM * local)
{
    SCOPESYM * declarationScope = local->declarationScope();

    for (SCOPESYM * currentScope = pCurrentScope; (declarationScope != currentScope) && currentScope; currentScope = currentScope->parent->asSCOPESYM()) {
        if (currentScope->scopeFlags & SF_TRYSCOPE) {
            return true;
        }
    }
    return false;
}

// check if two expressions are actually referring to the same local variable
bool IsSameLocalOrField(EXPR* expr1, EXPR* expr2) {
    if (expr1->kind != expr2->kind)
        return false;

    if (expr1->kind == EK_LOCAL)
        return expr1->asLOCAL()->local == expr2->asLOCAL()->local;
    if (expr1->kind == EK_FIELD) {
        EXPRFIELD * fld1 = expr1->asFIELD();
        EXPRFIELD * fld2 = expr2->asFIELD();
        // Must be the same field on the same type and the objects must either
        // both be null or be the same.
        return fld1->fwt.Field() == fld2->fwt.Field() &&
            fld1->fwt.Type() == fld2->fwt.Type() &&
            !fld1->object == !fld2->object && (!fld1->object || IsSameLocalOrField(fld1->object, fld2->object));
    }
    return false;
}

/*
 * Bind the simple assignment operator =.
 */
EXPR * FUNCBREC::bindAssignment(BASENODE * tree, EXPR * op1, EXPR * op2, bool allowExplicit)
{
    EXPR * expr;
    bool fOp2NotAddrOp = false;
    bool fOp2WasCast = false;

    if (IsSameLocalOrField(op1, op2)) {
        compiler()->Error(tree, WRN_AssignmentToSelf);
    }
    if (op1->kind != EK_LOCAL || !op1->asLOCAL()->local->slot.mustBePinned) {
        if (!checkLvalue(op1))
            return newError(tree, op1->type);
    } else {
        op1->asLOCAL()->local->slot.mustBePinned = false;
        op1->asLOCAL()->local->slot.isPinned = true;
        if (op2->type->isARRAYSYM())
            return BindPtrToArray(tree, op1->asLOCAL(), op2);
        if (op2->type == GetReqPDT(PT_STRING))
            op2 = bindPtrToString(tree, op2);
        else if (op2->kind == EK_ADDR)
            op2->flags |= EXF_ADDRNOCONV;
        else if (op2->kind != EK_ERROR) {
            fOp2NotAddrOp = true;
            fOp2WasCast = (op2->kind == EK_CAST);
        }
    }

    BASENODE * op2Tree = op2->tree ? op2->tree : tree;
    {
        op2 = mustConvertCore(op2, op1->type, op2Tree);
    }

    if (op2->isOK() && fOp2NotAddrOp) {
        // Only report these errors if the convert succeeded
        if (fOp2WasCast) {
            compiler()->Error(tree, ERR_BadCastInFixed);
        } else {
            compiler()->Error(tree, ERR_FixedNotNeeded);
        }
    }

    EXPR * exprVal2 = op2->GetSeqVal();

    if (exprVal2->kind == EK_CONSTANT || exprVal2->kind == EK_ZEROINIT ||
        exprVal2->type->isNUBSYM() && exprVal2->kind == EK_CALL &&
            exprVal2->asCALL()->mwi.Meth() == compiler()->getBSymmgr().methNubCtor &&
            exprVal2->asCALL()->args->GetConst())
    {
        // A constant, zero-init, or nullable constant, possibly with side effects.
        if (op1->kind == EK_LOCAL)
            op1->asLOCAL()->local->slot.isReferencedAssg = true;
    }
    else
        noteReference(op1);

    // Note: we can't do these transformations if op2 contains side effects other
    // than the zero-init or call (when exprVal2 contains other things sequenced in).
    if (op2->kind == EK_CALL && (op2->flags & EXF_NEWOBJCALL) && op2->type->isStructOrEnum() && op1->kind == EK_LOCAL &&
        !op1->asLOCAL()->local->slot.isRefParam && !localDeclaredOutsideTry(op1->asLOCAL()->local))
    {
        ASSERT(!op2->asCALL()->object);
        op2->asCALL()->object = op1;
        op2->setType(op1->type);
        op2->flags &= ~EXF_NEWOBJCALL;
        op2->flags |= EXF_IMPLICITSTRUCTASSG;
        expr = op2;
    }
    else if (op2->kind == EK_ZEROINIT && !op2->asZEROINIT()->p1 &&
        op1->kind != EK_PROP && (op1->kind != EK_ARRINDEX || !op1->type->isTYVARSYM()) &&
        CheckFieldUse(op1, false))
    {
        op2->asZEROINIT()->p1 = op1;
        expr = op2;
    }
    else
    {
        expr = newExprBinop(tree, EK_ASSG, op1->type, op1, op2);
        expr->flags |= EXF_ASSGOP;
    }

    return expr;
}


EXPR * FUNCBREC::bindIndexer(BASENODE * tree, EXPR * object, EXPR * args, int bindFlags)
{
    NAME * name;
    TYPESYM * type = object->type;

    if (!type->isAGGTYPESYM() && !type->isTYVARSYM()) {
        compiler()->Error(tree, ERR_BadIndexLHS, type);
        return newError(tree, NULL);
    }

    name = compiler()->namemgr->GetPredefName(PN_INDEXERINTERNAL);

    MemberLookup mem;
    if (!mem.Lookup(compiler(), type, object, parentDecl, name, 0,
        (bindFlags & BIND_BASECALL) ? (MemLookFlags::BaseCall | MemLookFlags::Indexer) : MemLookFlags::Indexer))
    {
        mem.ReportErrors(tree);
        return newError(tree, NULL);
    }
    mem.ReportWarnings(tree);

    ASSERT(mem.SymFirst()->isPROPSYM() && mem.SymFirst()->asPROPSYM()->isIndexer());

    EXPRMEMGRP * grp = newExprMemGrp(tree, tree, mem);
    return BindGrpToArgs(tree, bindFlags, grp, args);
}


EXPR * FUNCBREC::bindBase(BASENODE * tree)
{
    EXPR * rval = bindThisCore(tree, EXF_CANTBENULL);
    if (!rval) {
        if (pMSym && pMSym->isStatic)
            compiler()->Error(tree, ERR_BaseInStaticMeth);
        else
            compiler()->Error(tree, ERR_BaseInBadContext); // 'this' isn't available for some other reason.
        return newError(tree, parentAgg ? parentAgg->baseClass : NULL);
    }

    ASSERT(!(rval->flags & EXF_IMPLICITTHIS));

    TYPESYM * type = rval->type;
    if (type->isNUBSYM()) {
        type = type->asNUBSYM()->GetAts();
        ASSERT(type);
    }

    AGGTYPESYM * baseType = type->asAGGTYPESYM()->GetBaseClass();
    if (baseType) {
        rval = tryConvert(rval, baseType);
        ASSERT(rval);
    } else {
        compiler()->Error(tree, ERR_NoBaseClass);
    }

    return rval;
}


EXPR * FUNCBREC::bindSizeOf(BASENODE * tree, TYPESYM * type)
{
    if (type->isEnumType()) {
        type = type->underlyingEnumType();
    }
    CONSTVAL cv;
    cv.iVal = 0;
    if (type->isSimpleType())
        cv.iVal = compiler()->getBSymmgr().GetAttrArgSize(type->getPredefType());
    if (type->isPredefType(PT_DECIMAL))
        cv.iVal = 16;

    if (cv.iVal > 0) {
        return newExprConstant(tree, GetReqPDT(PT_INT), cv);
    }

    ErrArg arg(type);
    checkUnsafe(tree, NULL, ERR_SizeofUnsafe, &arg);
    EXPRSIZEOF * rval = newExpr(tree, EK_SIZEOF, GetReqPDT(PT_INT))->asSIZEOF();
    rval->sourceType = type;

    return rval;
}


EXPR * FUNCBREC::bindCastToNatural(EXPR * original)
{
    AGGTYPESYM * atsNaturalInt = GetReqPDT(original->type->isUnsigned() ? PT_UINTPTR : PT_INTPTR);
    if (atsNaturalInt == NULL)
        return newError(original->tree, NULL);

    EXPR * temp = newExpr(original->tree, EK_CAST, atsNaturalInt);
    temp->asCAST()->p1 = original;
    return temp;
}


EXPR * FUNCBREC::bindPtrAddMul(BASENODE * tree, EXPR * ptr, EXPR * times, EXPR * size, bool ptrFirst, EXPRKIND kind)
{
    EXPR * mulResult = bindPtrMul(tree, times, size);
    EXPR * temp = mulResult;
    while (temp && temp->kind == EK_CAST) {
        temp = temp->asCAST()->p1;
    }
    if (temp && temp->isZero(false)) {
        return ptr;
    }
    if (mulResult->isOK() && (mulResult->type->isPredefType(PT_LONG) || mulResult->type->isPredefType(PT_ULONG))) {
        mulResult = bindCastToNatural(mulResult);
    }
    if (ptrFirst) {
        return newExprBinop(tree, kind, ptr->type, ptr, mulResult);
    } else {
        return newExprBinop(tree, kind, ptr->type, mulResult, ptr);
    }
}

EXPR * FUNCBREC::bindPtrMul(BASENODE * tree, EXPR * times, EXPR * size)
{
    if (times) {
        if (times->type->isPredefType(PT_ULONG)) {
            size = mustCast(size, times->type);
        }
        EXPR * mulResult = BindStdBinOp(tree, EK_MUL, times, size);
        if (mulResult->kind == EK_MUL && !mulResult->type->isPredefType(PT_LONG) && !mulResult->type->isPredefType(PT_ULONG)) {
            // this multiplication needs to be done as natural ints, but since a int * natint = natint,
            // we only need to promote one side
            mulResult->asBIN()->p1 = bindCastToNatural(mulResult->asBIN()->p1);
            mulResult->setType(mulResult->asBIN()->p1->type);
            return mulResult;
        } else if (mulResult->kind == EK_CONSTANT) {
            return bindCastToNatural(mulResult);
        } else {
            return mulResult;
        }
    } else {
        return bindCastToNatural(size);
    }
}


EXPR * FUNCBREC::bindPtrArrayIndexer(BINOPNODE * tree, EXPR * opArr, EXPR * opIndex)
{

    ASSERT(opArr->type->isPTRSYM());

    TYPESYM * type = opArr->type->asPTRSYM()->baseType();

    if (opIndex->kind == EK_LIST) {
        compiler()->Error(tree, ERR_PtrIndexSingle);
        return newError(tree, type);
    }
    if (type == this->getVoidType()) {
        compiler()->Error(tree, ERR_VoidError);
        return newError(tree, NULL);
    }

    TYPESYM * typeIndex = chooseArrayIndexType(tree, opIndex);

    if (!typeIndex) {
        // using int as the type will allow us to give a better error...
        typeIndex = GetReqPDT(PT_INT);
    }

    opIndex = mustConvert(opIndex, typeIndex);
    if (!opIndex->isOK())
        return newError(tree, type);

    EXPR * size = bindSizeOf(tree, type);

    EXPR * add = bindPtrAddMul(tree, opArr, opIndex, size);
    EXPR * rval = newExprBinop(tree, EK_INDIR, type, add, NULL);
    rval->flags |= EXF_LVALUE | EXF_ASSGOP;

    return rval;
}

TYPESYM * FUNCBREC::chooseArrayIndexType(BASENODE * tree, EXPR * args)
{
    // first, select the allowable types
    for (unsigned int ipt = 0; ipt < lengthof(rgptIntOp); ipt++) {
        TYPESYM * type = GetReqPDT(rgptIntOp[ipt]);
        EXPRLOOP(args, arg)
            if (!canConvert(arg, type)) {
                goto NEXTI;
            }
        ENDLOOP;
        return type;
NEXTI:;
    }

    return NULL;
}


/*
 * Bind an array being dereferenced by one or more indexes: a[x], a[x,y], ...
 */
EXPR * FUNCBREC::bindArrayIndex(BINOPNODE * tree, int bindFlags)
{
    EXPR * expr;
    EXPR * op1;
    EXPR * op2;

    // base[foobar] is actually valid...
    if (tree->p1->kind == NK_OP && tree->p1->Op() == OP_BASE) {
        op1 = SetNodeExpr(tree->p1, bindBase(tree->p1));
        op2 = bindExpr(tree->p2);
        if (!op1->isOK() || !op2->isOK())
            return newError(tree, NULL);
        expr = bindIndexer(tree, op1, op2, bindFlags | BIND_BASECALL);
        expr->flags |= EXF_BASECALL;
        return expr;
    }

    op1 = bindExpr(tree->p1);
    op2 = bindExpr(tree->p2);
    if (!op1->isOK() || !op2->isOK())
        return newError(tree, NULL);

    if (tree->p1 && tree->p1->kind == NK_UNOP && tree->p1->Op() == OP_DEFAULT &&
        (op1->type->IsRefType() || op1->type->isPTRSYM()))
    {
        compiler()->Error( tree, WRN_DotOnDefault, op1->type);
    }

    TYPESYM * intType = GetReqPDT(PT_INT);

    // Array indexing must occur on an array type.
    compiler()->EnsureState(op1->type);
    if (!op1->type->isARRAYSYM()) {
        if (op1->type->isPTRSYM()) {
            return bindPtrArrayIndexer(tree, op1, op2);
        }
        return bindIndexer(tree, op1, op2, bindFlags);
    }
    ARRAYSYM * arrayType = op1->type->asARRAYSYM();

    // Check the rank of the array against the number of indices provided, and
    // convert the indexes to ints

    TYPESYM * destType = chooseArrayIndexType(tree, op2);

    if (!destType) {
        // using int as the type will allow us to give a better error...
        destType = intType;
    }

    int rank = arrayType->rank;

    EXPR * temp;
    EXPR ** next = &op2;
    EXPR ** first;
    int cIndices = 0;

AGAIN:
    if (next[0]->kind == EK_LIST) {
        first = &(next[0]->asBIN()->p1);
        next = &(next[0]->asBIN()->p2);
LASTONE:
        cIndices++;
        temp = mustConvert(*first, destType);
        if (rank == 1) {
            checkNegativeConstant(first[0]->tree, temp, WRN_NegativeArrayIndex);
        }
        if (destType == intType) {
            *first = temp;
        } else {
            first[0] = newExpr(first[0]->tree, EK_CAST, destType);
            first[0]->asCAST()->flags |= EXF_INDEXEXPR;
            first[0]->asCAST()->p1 = temp;
        }
        if (first != next) goto AGAIN;
    } else {
        first = next;
        goto LASTONE;
    }

    if (cIndices != rank) {
        compiler()->Error(tree, ERR_BadIndexCount, rank);
        return newError(tree, arrayType->elementType());
    }

    // Allocate a new expression, the type is the element type of the array.
    // Array index operations are always lvalues.
    expr = newExprBinop(tree, EK_ARRINDEX, arrayType->elementType(), op1, op2);
    expr->flags |= EXF_LVALUE | EXF_ASSGOP;

    return expr;
}

void EXPRCONSTANT::realizeStringConstant()
{
    if (!list) return;

    unsigned totLen = 0;
    if (val.strVal) {
        totLen = val.strVal->length;
    }
    EXPRLOOP(list, item)
        EXPRCONSTANT * econst = item->asCONSTANT();
        ASSERT(!econst->list);
        if (econst->val.strVal) {
            totLen += econst->val.strVal->length;
        }
    ENDLOOP;

    STRCONST * str = (STRCONST*) allocator->Alloc(sizeof(STRCONST));

    str->text = (WCHAR*) allocator->Alloc(sizeof(WCHAR) * totLen);
    str->length = totLen;
    WCHAR * ptr = str->text;
    if (val.strVal) {
        memcpy(ptr, val.strVal->text, val.strVal->length * sizeof(WCHAR));
        ptr += val.strVal->length;
    }
    EXPRLOOP(list, item)
        EXPRCONSTANT * econst = item->asCONSTANT();
        if (econst->val.strVal) {
            memcpy(ptr, econst->val.strVal->text, econst->val.strVal->length * sizeof(WCHAR));
            ptr += econst->val.strVal->length;
        }
    ENDLOOP;
    val.strVal = str;

    list = NULL;
    pList = &list;
}


EXPR * FUNCBREC::bindStringConcat(BASENODE * tree, EXPR * op1, EXPR * op2)
{
    EXPR  * firstB, * lastA;

    if (op1->kind == EK_CONCAT) {
        lastA = *(op1->asCONCAT()->pList);
    } else {
        lastA = op1;
    }

    if (op2->kind == EK_CONCAT) {
        firstB = op2->asCONCAT()->list->asBIN()->p1;
    } else {
        firstB = op2;
    }

    if (lastA->kind == EK_CONSTANT && firstB->kind == EK_CONSTANT) {

        EXPR * second;
        if (firstB->asCONSTANT()->list) {
            // we need to convert it to a BINOP list
            second = newExprBinop(NULL, EK_LIST, NULL, firstB, firstB->asCONSTANT()->list);
            firstB->asCONSTANT()->list = NULL;
            firstB->asCONSTANT()->pList = &(firstB->asCONSTANT()->list);
        } else {
            second = firstB;
        }

        newList(second, &(lastA->asCONSTANT()->pList));
        lastA->asCONSTANT()->allocator = allocator;
        if (second->kind == EK_LIST) {
            lastA->asCONSTANT()->pList = &(second->asBIN()->p2);
        }

        if (firstB == op2) { // if second was an item, and not a list
            return op1; // just return the first...
        }

        if (op2->asCONCAT()->count == 2) {
            // now this list is really an item...
            op2 = firstB = op2->asCONCAT()->list->asBIN()->p2;
        } else {
            op2->asCONCAT()->list = op2->asCONCAT()->list->asBIN()->p2;
            op2->asCONCAT()->count --;
        }
    }

    if (firstB == op2) { // ???? + item
        if (lastA != op1) { // LIST + ????
            newList(op2, &op1->asCONCAT()->pList);
            op1->asCONCAT()->count += 1;
            return op1;
        } else { // item + ????
            EXPRCONCAT * rval = newExpr(tree, EK_CONCAT, GetReqPDT(PT_STRING))->asCONCAT();
            rval->list = op1;
            rval->pList = &rval->list;
            newList(op2, &rval->pList);
            rval->count = 2;
            rval->flags |= EXF_UNREALIZEDCONCAT;
            return rval;
        }
    } else { // ???? + LIST
        if (lastA != op1) { // LIST + ????
            newList(op2->asCONCAT()->list, &op1->asCONCAT()->pList);
            op1->asCONCAT()->pList = op2->asCONCAT()->pList;
            op1->asCONCAT()->count += op2->asCONCAT()->count;
            return op1;
        } else { // item + ????
            op2->asCONCAT()->list = newExprBinop(NULL, EK_LIST, NULL, op1, op2->asCONCAT()->list);
            op2->asCONCAT()->count ++;
            return op2;
        }
    }
}


// Bind the "is" operator.
EXPR * FUNCBREC::bindIs(BINOPNODE * tree, EXPR * op1)
{
    ASSERT(tree->Op() == OP_IS);

    BASENODE * node2 = tree->p2;
    TYPESYM * type2 = bindType(node2->asANYTYPE());

    TYPESYM * typeBool = GetReqPDT(PT_BOOL);
    TYPESYM * type1 = op1->type;

    if (type2->isERRORSYM() || !op1->isOK())
        return newError(tree, typeBool);

    if (type1->isPTRSYM() || type2->isPTRSYM()) {
        compiler()->Error(tree, ERR_PointerInAsOrIs);
        return newError(tree, typeBool);
    }

    // If the left operand is the constant value null, the answer is always false.
    if (!op1->isNull())  {
        // First check whether type1 is always a type2.
        if (FIsSameType(type1, type2) || FImpRefConv(type1, type2) || 
            FBoxingConv(type1, type2) || FWrappingConv(type1, type2)) {
            // The result is op1 != null.
            if (type1->isNUBSYM())
                return BindNubHasValue(tree, op1);
            if (!type1->IsValType()) {
                if (!type1->IsRefTypeInVerifier()) {
                    // We have to box for the verifier, even if we know it's a reference.
                    op1 = mustConvert(op1, GetReqPDT(PT_OBJECT), NOUDC);
                }
                return newExprBinop(tree, EK_NE, GetReqPDT(PT_BOOL), op1, bindNull(NULL));
            }
            // type1 can't be null. Hence the value is always true.
            compiler()->Error(tree, WRN_IsAlwaysTrue, type2);
            return AddSideEffects(tree, newExprConstant(tree, typeBool, ConstValInit(true)), op1, true, true);
        }
        
        if (FUnwrappingConv(type1, type2)) {
            return BindNubHasValue(tree, op1);
        }
        // explicit reference type conversion, unboxing conversion or open types
        if (FExpRefConv(type1, type2) || FUnboxingConv(type1, type2) ||
            (BSYMMGR::TypeContainsTyVars(type1, NULL) || BSYMMGR::TypeContainsTyVars(type2, NULL))) {
            // here the language spec states a dynamic test is required
            // however we check for a few more cases where we know that no conversion is possible
            if (!(type1->IsValType() && type2->isClassType() && !type2->isPredefType(PT_ENUM))) {
                if (!type1->IsRefTypeInVerifier()) {
                    // We have to box for the verifier, even if we know its a reference.
                    op1 = mustConvert(op1, GetReqPDT(PT_OBJECT), NOUDC);
                }
                EXPR * expr2 = newExpr(node2, EK_TYPEOF, GetReqPDT(PT_TYPE));
                expr2->asTYPEOF()->sourceType = type2;
                expr2->asTYPEOF()->method = NULL;
                return newExprBinop(tree, EK_IS, typeBool, op1, expr2);
            }
        }
    }

    // We can't cast from op1 to type2, so the answer is false.
    compiler()->Error(tree, WRN_IsAlwaysFalse, type2);
    return AddSideEffects(tree, newExprConstant(tree, typeBool, ConstValInit(false)), op1, true, true);
}


// Bind "as" operator.
EXPR * FUNCBREC::bindAs(BINOPNODE * tree, EXPR * op1)
{
    ASSERT(tree->Op() == OP_AS);

    BASENODE * node2 = tree->p2;
    TYPESYM * type2 = bindType(node2->asANYTYPE());

    TYPESYM * type1 = op1->type;

    if (type2->isERRORSYM() || !op1->isOK())
        return newError(tree, type2);

    if (type1->isPTRSYM() || type2->isPTRSYM()) {
        compiler()->Error(tree, ERR_PointerInAsOrIs);
        return newError(tree, type2);
    }

    if (!type2->IsRefType() && !type2->isNUBSYM()) {
        if (type2->isTYVARSYM())
            compiler()->Error(tree, ERR_AsWithTypeVar, type2);
        else
            compiler()->Error(tree, ERR_AsMustHaveReferenceType, type2);
        return newError(tree, type2);
    }

    if (FIsSameType(type1, type2) || FImpRefConv(type1, type2) || 
        FBoxingConv(type1, type2) || FWrappingConv(type1, type2)) {
        if (op1->isNull()) {
            compiler()->Error(tree, WRN_AlwaysNull, type2);
            return newExprZero(tree, type2);
        }

        return mustConvertCore(op1, type2, tree, NOUDC);
    }
    
    if (FExpRefConv(type1, type2) || FUnboxingConv(type1, type2) ||
        BSYMMGR::TypeContainsTyVars(type1, NULL) || BSYMMGR::TypeContainsTyVars(type2, NULL)) {
        if (op1->isNull()) {
            return newExprZero(tree, type2);
        }

        // check for case we know is always false
        if (op1->isNull() || !canCast(op1, type2, NOUDC) && type1->IsValType() && type2->isClassType() && (!type1->isTYVARSYM() || !type2->isPredefType(PT_ENUM))) {
            compiler()->Error(tree, WRN_AlwaysNull, type2);
            return AddSideEffects(tree, newExprZero(tree, type2), op1, true, true);
        }

        if (!type1->IsRefTypeInVerifier()) {
            // Need to box.
            op1 = mustConvert(op1, GetReqPDT(PT_OBJECT), NOUDC);
        }

        EXPR * expr2 = newExpr(node2, EK_TYPEOF, GetReqPDT(PT_TYPE));
        expr2->asTYPEOF()->sourceType = type2;
        expr2->asTYPEOF()->method = NULL;

        if (!type2->isTYVARSYM()) {
            return newExprBinop(tree, EK_AS, type2, op1, expr2);
        } else {
            EXPR * exprRes = newExprBinop(tree, EK_AS, GetReqPDT(PT_OBJECT), op1, expr2);
            bindSimpleCast(tree, exprRes, type2, &exprRes, EXF_FORCE_UNBOX);
            return exprRes;
        }
    }

    // There is no explicit reference or unboxing conversion from op1 to type2, and neither is an open type,
    // report an error.
    compiler()->Error(tree, ERR_NoExplicitBuiltinConv, ErrArg(type1, ErrArgFlags::Unique), ErrArg(type2, ErrArgFlags::Unique));
    return AddSideEffects(tree, newExprZero(tree, type2), op1, true, true);
}


EXPR * FUNCBREC::bindRefValue(BINOPNODE * tree)
{
    EXPR * rany = bindExpr(tree->p1);
    TYPESYM * type = bindType(tree->p2->asANYTYPE());
    ASSERT(type);

    AGGTYPESYM * atsRefAny = GetOptPDT(PT_REFANY);
    if (!rany->isOK() || type->isERRORSYM() || !atsRefAny)
        return newError(tree, type);

    rany = mustConvert(rany, atsRefAny);

    EXPR * rval = newExprBinop(tree, EK_VALUERA, type, rany, newExpr(EK_TYPEOF));
    rval->asBIN()->p2->asTYPEOF()->sourceType = type;

    rval->flags |= EXF_ASSGOP | EXF_LVALUE;

    return rval;
}

// Bind an event add (+=) or event remove (-=) expression. Becomes
// a call to the adder or remover accessor.
EXPR * FUNCBREC::bindEventAccess(BASENODE * tree, bool isAdd, EXPREVENT * exprEvent, EXPR * exprRHS)
{
    // Convert RHS to the type of the event.
    EVENTSYM * eventSym = exprEvent->ewt.Event();
    exprRHS = mustConvert(exprRHS, exprEvent->type);

    // Get the correct accessor to call.
    METHSYM * accessor = isAdd ? eventSym->methAdd : eventSym->methRemove;

    if ((exprEvent->flags & EXF_BASECALL) && accessor->isAbstract) {
        compiler()->Error(tree, ERR_AbstractBaseCall, exprEvent->ewt);
    }

    // Call the accessor.
    EXPRCALL * call = newExpr(tree, EK_CALL, getVoidType())->asCALL();
    call->object = exprEvent->object;
    call->flags |= (exprEvent->flags & EXF_BASECALL);
    call->mwi.Set(accessor, exprEvent->ewt.Type(), NULL);
    call->args = exprRHS;

    return call;
}


// Bind a binary expression (or statement...)
EXPR * FUNCBREC::bindBinop(BINOPNODE * tree, int bindFlags)
{
    ASSERT(tree->IsAnyBinaryOperator());

    OPERATOR op = tree->Op();
    EXPR * op1;
    EXPR * op2;

    switch (op) {
    case OP_CALL:
        return bindCall(tree->asCALL(), bindFlags);

    case OP_REFVALUE:
        return bindRefValue(tree);

    case OP_ASSIGN:
        ASSERT(tree->p1 && tree->p2);
        // Simple assignment.
        op1 = bindExpr(tree->p1, BIND_MEMBERSET);
        op2 = bindExpr(tree->p2);
        ASSERT(op1 && op2);
        if (!op1->isOK() || !op2->isOK())
            return newError(tree, op1->type);
        return bindAssignment(tree, op1, op2);

    case OP_DEREF:
        // Array indexing
        return bindArrayIndex(tree, bindFlags);

    case OP_CAST:
        {
            // p1 is the type to cast to. p2 is the thing to cast.
            TYPESYM * typeDest = bindType(tree->p1->asANYTYPE());
            return mustCastCore(bindExpr(tree->p2, BIND_RVALUEREQUIRED | (bindFlags & BIND_FIXEDVALUE)), typeDest, tree);
        }

    case OP_ADDEQ:
    case OP_SUBEQ:
        {
            ASSERT(tree->p1 && tree->p2);
            // These are special if LHS is an event. Bind the LHS, but allow events. If we get an
            // event, then process specially, otherwise, bind +=, -= as usual.
            BASENODE * p1 = tree->p1;

            // A METHSYM isn't really valid here, but we'll report a better error message by allowing it through.
            if (p1->IsAnyName())
                op1 = SetNodeExpr(p1, bindName(p1->asANYNAME(), MASK_METHSYM | MASK_EVENTSYM | MASK_MEMBVARSYM | MASK_LOCVARSYM | MASK_PROPSYM));
            else if (p1->kind == NK_DOT)
                op1 = SetNodeExpr(p1, bindDot(p1->asDOT(), MASK_METHSYM | MASK_EVENTSYM | MASK_MEMBVARSYM | MASK_PROPSYM));
            else
                op1 = bindExpr(p1);

            if (op1->kind == EK_EVENT) {
                op2 = bindExpr(tree->p2);
                ASSERT(op1->isOK());
                if (!op2->isOK())
                    return newError(tree, op1->type);
                return bindEventAccess(tree, (op == OP_ADDEQ), op1->asEVENT(), op2);
            }
        }
        // LHS isn't an event; go into regular +=, -= processing.
        goto BOUND_OP1;

    default:
        break;
    }

    // Then normal binop expressions where both children are always bound first:
    if (op == OP_SUB)
        op1 = bindExpr(tree->p1, BIND_RVALUEREQUIRED | BIND_MAYBECONFUSEDNEGATIVECAST);
    else
        op1 = bindExpr(tree->p1);

BOUND_OP1:

    if (op == OP_IS)
        return bindIs(tree, op1);

    if (op == OP_AS)
        return bindAs(tree, op1);

    op2 = bindExpr(tree->p2);
    if (!op1->isOK() || !op2->isOK())
        return newError(tree, NULL);

    EXPRKIND ek = OP2EK[op];

    switch (ek) {
        case EK_LT:
        case EK_LE:
        case EK_GT:
        case EK_GE:
        case EK_EQ:
        case EK_NE:
            if (IsSameLocalOrField(op1, op2)) {
                compiler()->Error(tree, WRN_ComparisonToSelf);
            }
            break;
        default:
            break;
    }

    if (ek != EK_COUNT) {
        if (ek > EK_MULTIOFFSET) {
            ek = (EXPRKIND) (ek - EK_MULTIOFFSET);
            return bindMultiOp(tree, ek, op1, op2);
        }
        return BindStdBinOp(tree, ek, op1, op2);
    }

    switch(op) {
    case OP_QUESTION:
        return bindQMark(tree, op1, op2->asBINOP());
    case OP_COLON:
        return newExprBinop(tree, EK_BINOP, NULL, op1, op2);
    case OP_VALORDEF:
        return BindValOrDefOp(tree, op1, op2);
    default:
        break;
    }

    compiler()->Error(tree, ERR_UnimplementedOp, opName(op));
    return newError(tree, NULL);
}

EXPR * FUNCBREC::bindQMark(BASENODE * tree, EXPR * op1, EXPRBINOP * op2)
{
    ASSERT(op2->kind == EK_BINOP && op1->isOK());

    op1 = bindBooleanValue(op1->tree ? op1->tree : tree, op1);

    EXPR * T = op2->asBIN()->p1;
    EXPR * S = op2->asBIN()->p2;

    ASSERT(S->isOK() && T->isOK());

    EXPR * rval;
    EXPR * opConst1;

    if (T->type != S->type || T->type->IsNeverSameType()) {
        bool T1 = canConvert(T, S->type);
        bool S1 = canConvert(S, T->type);
        if (T1) {
            if (!S1) {
                op2->asBIN()->p1 = mustConvert(T, S->type);
            } else {
                compiler()->Error(op2->tree, ERR_AmbigQM, T->type, S->type);
                goto LERROR;
            }
        } else { // ! T1
            if (S1) {
                op2->asBIN()->p2 = mustConvert(S, T->type);
            } else {
                // Don't report the error if one of the types is an unknown error type
                if ((!T->type->isERRORSYM() || T->type->parent) && (!S->type->isERRORSYM() || S->type->parent)) {
                    compiler()->Error(op2->tree, ERR_InvalidQM,
                        ErrArg(T->type, ErrArgFlags::Unique), ErrArg(S->type, ErrArgFlags::Unique));
                }
                else
                    ASSERT(compiler()->ErrorCount());
                goto LERROR;
            }
        }
    }

    opConst1 = op1->GetConst();
    if (opConst1) {
        if (opConst1->asCONSTANT()->getVal().iVal) {
            // controlling bool is constant true.
            rval = AddSideEffects(tree, op2->asBIN()->p1, op1, true);
            if (op2->asBIN()->p2 && op2->asBIN()->p2->isOK() && op2->asBIN()->p2->tree)
                compiler()->Error(op2->asBIN()->p2->tree, WRN_UnreachableExpr);
        }
        else {
            // controlling bool is constant false.
            rval = AddSideEffects(tree, op2->asBIN()->p2, op1, true);
            if (op2->asBIN()->p1 && op2->asBIN()->p1->isOK() && op2->asBIN()->p1->tree)
                compiler()->Error(op2->asBIN()->p1->tree, WRN_UnreachableExpr);
        }
    }
    else {
        // Usual case: controlling bool is not a constant.
        TYPESYM * type = op2->asBIN()->p1->type;
        ASSERT(type == op2->asBIN()->p2->type);

        rval = newExprBinop(tree, EK_QMARK, type, op1, op2);
    }

    return rval;

LERROR:

    return newError(tree, NULL);
}


bool FUNCBREC::isCastOrExpr(EXPR * search, EXPR * target)
{
AGAIN:
    if (search == target) return true;

    if (search && search->kind == EK_CAST) {
        search = search->asCAST()->p1;
        goto AGAIN;
    }

    return false;
}


inline bool FUNCBREC::isCastOptThis(EXPR * search)
{
    while (search && search->kind == EK_CAST) {
        search  = search->asCAST()->p1;
    }
    return (search && isThisPointer(search));
}

EXPR * FUNCBREC::bindMultiOp(BASENODE * tree, EXPRKIND ek, EXPR * op1, EXPR * op2)
{
    if (!checkLvalue(op1))
        return newError(tree, op1->type);

    EXPRMULTIGET * exprGet = newExpr(op1->tree, EK_MULTIGET, op1->type)->asMULTIGET();
    exprGet->flags |= EXF_ASSGOP;

    bool fUserDef;
    NubInfo nin;
    EXPR * exprVal = BindStdBinOp(tree, ek, exprGet, op2, &fUserDef, &nin);

    ASSERT(!fUserDef || !nin.fActive);
    if (!nin.fActive)
        exprVal->flags |= EXF_PUSH_OP_FIRST;

    EXPRMULTI * exprMulti = newExpr(tree, EK_MULTI, op1->type)->asMULTI();
    exprGet->multi = exprMulti;
    exprMulti->left = op1;
    exprMulti->flags |= EXF_ASSGOP;

    // Convert to the destination type.
    EXPR * exprRes;

    /***************************************************************************************************
        These conversion rules are bogus - they allow stuff that really shouldn't be allowed
        and prohibit other things that should be allowed. Eg,
        * char += char, char *= char, etc are allowed but shouldn't be.
        * char += 1 is not allowed but should be.
        * ptr += ptr, ptr -= ptr are allowed but shouldn't be.
        * byte <<= 1 can throw in a checked context.

        The proper fix would be for +(byte, byte) to be defined and available when doing compound
        assignment (but not available otherwise). This leads to a much cleaner implementation that
        is naturally extensible to nullable as well.
    ***************************************************************************************************/

    if (fUserDef) {
        ASSERT(!nin.fActive);
        exprRes = mustConvert(exprVal, op1->type);
    }
    else if (!nin.fActive) {
        exprRes = tryConvert(exprVal, op1->type);
        if (!exprRes) {
            exprRes = mustCast(exprVal, op1->type);
            if (ek != EK_LSHIFT && ek != EK_RSHIFT)
                mustConvert(op2, op1->type);
        }
    }
    else {
        // The operator is lifted.
        ASSERT(!exprVal->type->isNUBSYM());
        NUBSYM * nubDst = compiler()->getBSymmgr().GetNubType(exprVal->type);
        EXPR * exprNull = newExprZero(tree, nubDst);

        // First convert to the nullable type.
        exprVal = mustConvert(exprVal, nubDst);

        // Now go through the goofy conversion tests.
        exprRes = tryConvert(exprVal, op1->type);
        if (!exprRes) {
            exprRes = mustCast(exprVal, op1->type);
            if (ek != EK_LSHIFT && ek != EK_RSHIFT)
                mustConvert(op2, op1->type);
        }

        // If exprRes is an error expr, just use it - don't try to cast the exprNull and
        // risk a duplicate error message.
        exprNull = exprRes->isOK() ? mustCast(exprNull, op1->type) : exprRes;

        if (nin.FAlwaysNull() && exprNull->isOK())
            compiler()->Error(tree, WRN_AlwaysNull, nubDst);
        exprRes = BindNubOpRes(tree, op1->type, exprRes, exprNull, nin);
    }

    exprMulti->op = exprRes;

    return exprMulti;
}


EXPR * FUNCBREC::bindSizeOf(UNOPNODE * tree)
{
    TYPESYM * type = bindType(tree->p1->asANYTYPE());
    ASSERT(type);
    if (type->isERRORSYM())
        return newError(tree, GetReqPDT(PT_INT));

    if (isManagedType(type))
        compiler()->Error(tree->p1, ERR_ManagedAddr, type);

    return bindSizeOf(tree, type);
}





bool FUNCBREC::fixupTypeOfExpr(BASENODE * tree, EXPRTYPEOF * expr)
{
    PMETHSYM meth;
    TYPESYM * param;
    TypeArray * paramList;
    param = GetReqPDT(PT_TYPEHANDLE);
    paramList = compiler()->getBSymmgr().AllocParams(1, &param);
    meth = FindPredefMeth(tree, PN_GETTYPEFROMHANDLE, GetReqPDT(PT_TYPE), paramList);

    if (meth) {
        expr->method = meth;
        return true;
    }

    return false;
}


EXPR * FUNCBREC::bindTypeOf(UNOPNODE * tree)
{
    BASENODE * op = tree->p1;

    TYPESYM * type = bindType(op->asANYTYPE());
    ASSERT(type);
    if (type->isERRORSYM())
        goto LERROR;


    if (type == getVoidType())
        type = GetReqPDT(PT_SYSTEMVOID);

    EXPRTYPEOF *rval;
    rval = newExpr(tree, EK_TYPEOF, GetReqPDT(PT_TYPE))->asTYPEOF();
    rval->sourceType = type;
    if (op->kind == NK_OPENTYPE) {
        ASSERT(type->isAGGTYPESYM() && type->asAGGTYPESYM()->typeArgsAll->size > 0 &&
            type->asAGGTYPESYM()->typeArgsAll->Item(type->asAGGTYPESYM()->typeArgsAll->size - 1)->isUNITSYM());
        rval->flags |= EXF_OPENTYPE;
    }

    if (!fixupTypeOfExpr(tree, rval)) goto LERROR;

    rval->flags |= EXF_CANTBENULL;

    return rval;

LERROR:
    return newError(tree, GetReqPDT(PT_TYPE));
}


EXPR * FUNCBREC::bindDefault(UNOPNODE * tree)
{
    BASENODE * op = tree->p1;
    TYPESYM * type;

    type = bindType(op->asANYTYPE());
    ASSERT(type);
    if (type->isERRORSYM())
        return newError(tree, type);

    return newExprZero(tree, type);
}


EXPR * FUNCBREC::bindMakeRefAny(BASENODE * tree, EXPR * op)
{
    AGGTYPESYM * atsRefAny = GetOptPDT(PT_REFANY);

    if (!checkLvalue(op))
        return newError(tree, atsRefAny);

    if (op->type->isSpecialByRefType()) {
        compiler()->Error(op->tree, ERR_MethodArgCantBeRefAny, op->type);
        return newError(tree, atsRefAny);
    }

    CheckFieldRef(op);

    if (!atsRefAny)
        return newError(tree, NULL);

    return newExprBinop(tree, EK_MAKERA, atsRefAny, op, NULL);
}

EXPR * FUNCBREC::bindRefType(BASENODE * tree, EXPR * op)
{
    AGGTYPESYM * atsRefAny = GetOptPDT(PT_REFANY);
    TYPESYM * typeType = GetReqPDT(PT_TYPE);

    if (!atsRefAny)
        return newError(tree, typeType);
    op = mustConvert(op, atsRefAny);
    if (!op->isOK())
        return newError(tree, typeType);

    EXPR * rval = newExpr(tree, EK_TYPEOF, typeType)->asTYPEOF();

    // this is a dummy, we will instead use the type of the refany to load...
    rval->asTYPEOF()->sourceType = GetReqPDT(PT_OBJECT);

    if (!fixupTypeOfExpr(tree, rval->asTYPEOF()))
        return newError(tree, typeType);

    rval = newExprBinop(tree, EK_TYPERA, typeType, op, rval);

    return rval;
}


EXPR * FUNCBREC::bindCheckedExpr(UNOPNODE * tree, int bindFlags)
{
    CHECKEDCONTEXT checked(this, tree->Op() == OP_CHECKED);

    EXPR * rval = bindExpr(tree->p1, bindFlags);

    checked.restore(this);

    return rval;

}


// Bind a unary expression
EXPR * FUNCBREC::bindUnop(UNOPNODE * tree, int bindFlags)
{
    ASSERT(tree->kind == NK_UNOP && tree->p1);

    OPERATOR op = tree->Op();

    switch(op) {
    case OP_TYPEOF:
        return bindTypeOf(tree);
    case OP_SIZEOF:
        return bindSizeOf(tree);
    case OP_DEFAULT:
        return bindDefault(tree);
    case OP_CHECKED:
    case OP_UNCHECKED:
        return bindCheckedExpr(tree, bindFlags);
    default:
        break;
    }

    // Bind the child node.
    EXPR * op1 = bindExpr(tree->p1, BIND_RVALUEREQUIRED | (bindFlags & BIND_MAYBECONFUSEDNEGATIVECAST));
    ASSERT(op1);

    if (!op1->isOK())
        return newError(tree, NULL);

    switch (op)
    {
    case OP_PAREN:
        // Parenthesises expression is ignored.
        return op1;

    case OP_UPLUS:
    case OP_NEG:
    case OP_BITNOT:
    case OP_LOGNOT:
    case OP_POSTDEC:
    case OP_PREDEC:
    case OP_POSTINC:
    case OP_PREINC:
        return BindStdUnaOp(tree, op, op1);

    case OP_MAKEREFANY: return bindMakeRefAny(tree, op1);
    case OP_REFTYPE: return bindRefType(tree, op1);
    case OP_INDIR:   return bindPtrIndirection(tree, op1);
    case OP_ADDR:    return bindPtrAddr(tree, op1, bindFlags);

    default:
        compiler()->Error(tree, ERR_UnimplementedOp, opName(op));
        return newError(tree, NULL);
    }
}

bool FUNCBREC::isManagedType(TYPESYM * type)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);
    compiler()->EnsureState(type);

    if (type->isVOIDSYM())
        return false;

    AGGSYM * aggT;

    switch (type->fundType()) {
    case FT_NONE:
    case FT_REF:
    case FT_VAR:
        return true;

    case FT_STRUCT:
        if (type->isNUBSYM())
            return true;

        aggT = type->getAggregate();
        ASSERT(!aggT->isManagedStruct || !aggT->isUnmanagedStruct);

        // See if we already know.
        if (aggT->isManagedStruct || aggT->isUnmanagedStruct)
            return aggT->isManagedStruct;

        // Generics are always managed.
        if (aggT->typeVarsAll->size > 0) {
            aggT->isManagedStruct = true;
            return true;
        }

        for (SYM * ps = aggT->firstChild; ps; ps = ps->nextChild) {
            if (ps->isMEMBVARSYM() && !ps->asMEMBVARSYM()->isStatic && isManagedType(ps->asMEMBVARSYM()->type)) {
                aggT->isManagedStruct = true;
                return true;
            }
        }
        aggT->isUnmanagedStruct = true;
        return false;

    default:
        return false;
    }
}

// prop returns, array indexes and refanys are not addressable
// This means that you can't say &X in either a fixed statement or outside of it.
// If foo is an array you can never say &(foo[4]), not even in a fixed statement...
// [Note, however, that you can say &(foo[3].la) in a fixed statement...
// If foo is a fixed buffer this code should not get called because it is not
// a valid LVALUE, and thus not even eligible for taking the address of.
// It will ASSERT and return false.
bool FUNCBREC::isAddressable(EXPR * expr)
{

    switch (expr->kind) {
    case EK_PROP:
    case EK_VALUERA:
        return false;
    case EK_ARRINDEX:
    case EK_FIELD:
    case EK_LOCAL:
    case EK_INDIR:
        return true;
    default:
        ASSERT(!"bad lvalue expr");
        return false;
    }

}

bool FUNCBREC::isFixedExpression(EXPR * expr, LOCVARSYM ** psymLocal)
{
    if (psymLocal)
        *psymLocal = NULL;

    for (;;) {
        switch (expr->kind) {
        case EK_FIELD:
            {
                if (expr->asFIELD()->fwt.Field()->isStatic) return false;
                EXPR * object = expr->asFIELD()->object;
                if (!object) return false;
                if (!(object->flags & EXF_LVALUE)) return false;
                compiler()->EnsureState(object->type);
                if (object->type->fundType() == FT_REF) return false;
                expr = object;
                continue;
            }
        case EK_LOCAL:
            {
                LOCVARSYM * local = expr->asLOCAL()->local;
                if (psymLocal) *psymLocal = local;
                if (local->slot.isParam) {
                    return !local->slot.isRefParam;
                }
                return local != thisPointer;
            }
        case EK_LOCALLOC:
        case EK_INDIR:
            return true;
        default:
            return false;
        }
    }
}


EXPR * FUNCBREC::bindLocAlloc(NEWNODE * tree, PTRSYM * type)
{
    checkUnsafe(tree);

    if (pCatchScope != pOuterScope || pFinallyScope != pOuterScope) {
        compiler()->Error(tree, ERR_StackallocInCatchFinally);
    }

    EXPR * arg = bindExpr(tree->pArgs);
    if (!arg) {
        ASSERT(compiler()->ErrorCount());
        return newError(tree, type);
    }

    arg = mustConvert(arg, GetReqPDT(PT_INT));

    checkNegativeConstant(tree->pArgs, arg, ERR_NegativeStackAllocSize);

    arg = newExprBinop(NULL, EK_MUL, GetReqPDT(PT_INT), bindSizeOf(NULL, type->baseType()), arg);

    if (checked.normal)
        arg->flags |= EXF_CHECKOVERFLOW;

    EXPR * rval = newExprBinop(tree, EK_LOCALLOC, type, arg, NULL);

    rval->flags |= EXF_ASSGOP;

    return rval;
}


EXPR * FUNCBREC::bindPtrAddr(UNOPNODE * tree, EXPR * op, int bindFlags)
{
    checkUnsafe(tree);

    switch (op->kind) {
    case EK_ANONMETH:
    case EK_MEMGRP:
        compiler()->Error(tree, ERR_InvalidAddrOp);
        return newError(tree, NULL);
    case EK_FIELD:
        CheckFieldRef(op);
        markFieldAssigned(op);
        break;
    default:
        break;
    }


    LOCVARSYM * local = NULL;
    bool needsFixing = !isFixedExpression(op, &local);
    bool fSkipFixedErrors = false;

    if (isManagedType(op->type)) {
        compiler()->Error(tree, ERR_ManagedAddr, op->type);
        // Prevent reporting cascading errors
        fSkipFixedErrors = true;
    }

    if (op->isOK() && !(op->flags & EXF_LVALUE) || !isAddressable(op)) {
        compiler()->Error(tree, op->kind == EK_LOCAL ? ERR_AddrOnReadOnlyLocal : ERR_InvalidAddrOp);
        // Prevent reporting cascading errors
        fSkipFixedErrors = true;
    }

    if (!fSkipFixedErrors && needsFixing != !!(bindFlags & BIND_FIXEDVALUE)) {
        compiler()->Error(tree, !needsFixing ? ERR_FixedNotNeeded : ERR_FixedNeeded);
    }

    if (!fSkipFixedErrors && !needsFixing && local != NULL) {
        // a local doesn't need fixing unless it might get hoisted into an anonymous delegate
        if (local->fUsedInAnonMeth) {
            if (local->nodeAnonMethUse)
                compiler()->Error(tree, ERR_LocalCantBeFixedAndHoisted, local, ErrArgRefOnly(local->nodeAnonMethUse));
            else
                compiler()->Error(tree, ERR_LocalCantBeFixedAndHoisted, local);
        }
        else {
            ASSERT(tree != NULL);
            if (local->nodeAddrTaken == NULL)
                local->nodeAddrTaken = tree;
        }
    }

    SetExternalRef(op->type);


    TYPESYM * desiredType = compiler()->getBSymmgr().GetPtrType(op->type);
    EXPR * rval = newExprBinop(tree, EK_ADDR, desiredType, op, NULL);


    return rval;
}

EXPR * FUNCBREC::bindPtrIndirection(UNOPNODE * tree, EXPR * op)
{

    // In the EE:
    // class Foo ...
    // Foo f = ...
    // &f is the address of the local f (its location on the stack(
    // *f is the address of the object referenced by f (the actual hidden value of f)
    // &f has type Foo** which means it can be indirected once to get Foo*
    // *f has type Foo* which means that it can not be indirected
    // *f is not an lvalue
    // &f is not an lvalue
    // you can say *&f which gives you Foo*
    // you cannot say &*f as *f is not an lvalue


    if (!op->type->isPTRSYM()) {
        compiler()->Error(tree, ERR_PtrExpected);
        return newError(tree, NULL);
    }

    TYPESYM * typeBase = op->type->asPTRSYM()->baseType();

    if (typeBase == getVoidType()) {
        compiler()->Error(tree, ERR_VoidError);
        return newError(tree, typeBase);
    }

    EXPR * rval = newExprBinop(tree, EK_INDIR, typeBase, op, NULL);
    rval->flags |= EXF_ASSGOP | EXF_LVALUE;
    return rval;
}


// bind a list of variable declarations  newLast is set if
// the result consists of multuiple EXPR statatments
void FUNCBREC::BindVarDecls(DECLSTMTNODE * tree, StmtListBldr & bldr)
{
    TYPESYM * type = bindType(tree->pType);
    ASSERT(type);

    compiler()->CheckForStaticClass(tree, NULL, type, ERR_VarDeclIsStaticClass);

    unsigned flags = tree->flags;
    if (flags & NF_FIXED_DECL && !type->isPTRSYM()) {
        compiler()->Error(tree, ERR_BadFixedInitType);
    }
    else if (flags & NF_CONST_DECL && !type->CanBeConst()) {
        compiler()->Error(tree, ERR_BadConstType, type);
        flags &= ~(NF_CONST_DECL);
    }

    EXPRDECL * declFirst = NULL;

    NODELOOP(tree->pVars, VARDECL, decl)
        EXPRDECL * expr = bindVarDecl(decl, type, flags & (NF_CONST_DECL | NF_FIXED_DECL | NF_USING_DECL));
        if (!declFirst)
            declFirst = expr;
        bldr.Add(expr);
    ENDLOOP;

    if (declFirst) {
        // For debug info associate the NK_DECLSTMT with the first variable declaration
        declFirst->tree = tree;
    }

    SetNodeExpr(tree, declFirst);
}


// Bind a single variable declaration
EXPRDECL * FUNCBREC::bindVarDecl(VARDECLNODE * tree, TYPESYM * type, unsigned flags)
{
    bool isConst = !!(flags & NF_CONST_DECL);
    bool isFixed = !!(flags & NF_FIXED_DECL);
    bool isUsing = !!(flags & NF_USING_DECL);

    ASSERT(!(isConst && isFixed));
    ASSERT(!(isConst && isUsing));
    ASSERT(!(isUsing && isFixed));

    int cbit = 0;
    LOCVARSYM * sym = declareVar(tree, tree->pName->pName, type);
    ASSERT(!sym || sym->type == type);

    if (sym) {
        if (isFixed) {
            sym->slot.mustBePinned = true;
            sym->isNonWriteable = true;
            sym->isFixed = true;
        } else if (isUsing) {
            sym->isNonWriteable = true;
            sym->isUsing = true;
        }

        sym->declTree = tree;
        ASSERT(!sym->slot.isReferenced);
        unreferencedVarCount++;

        sym->slot.SetJbitDefAssg(uninitedVarCount + 1);
        cbit = FlowChecker::GetCbit(compiler(), sym->type);
        uninitedVarCount += cbit;
    }

    EXPR * init = bindPossibleArrayInitAssg(tree->pArg->asBINOP(), type, isFixed ? BIND_FIXEDVALUE : (isUsing ? BIND_USINGVALUE : 0));

    if (!sym)
        return NULL;

    if (isFixed || isUsing) {
        sym->slot.hasInit = true;
        if (!init) {
            compiler()->Error(tree, ERR_FixedMustInit);
        }
    } else if (isConst && init) {
        if (init->kind == EK_ASSG) {
            init = init->asBIN()->p2;
        }
        if (init->kind == EK_CONSTANT) {
            sym->isConst = true;
            sym->constVal = init->asCONSTANT()->getSVal();
            init = NULL;
        }
        else {
            if (init->kind != EK_ERROR) {
                BASENODE * err = tree->pArg;
                if (err && err->kind == NK_BINOP) {
                    ASSERT(err->Op() == OP_ASSIGN);
                    err = err->asBINOP()->p2;
                }
                //EDMAURER Give a better error message in the following case:
                //  const object x = "some_string"
                //The error here is that an implicit cast cannot be performed. All ref types
                //except strings must be initialized with null.
                if (sym->type->IsRefType ())
                    compiler()->Error(err, ERR_NotNullConstRefField, sym, sym->type);
                else
                    compiler()->Error(err, ERR_NotConstantExpression, sym);
            }
            sym->slot.hasInit = true;
        }
    } else if (init && !pSwitchScope && !m_fForwardGotos) {
        sym->slot.hasInit = true;
    }

    // If we don't need the bits after all, "deallocate" them.
    if (!sym->slot.isReferenced && (sym->slot.hasInit || sym->isConst)) {
        if (sym->slot.JbitDefAssg() == uninitedVarCount - cbit + 1)
            uninitedVarCount -= cbit;
        sym->slot.SetJbitDefAssg(0);
    }

    EXPRDECL * rval = newExpr(tree, EK_DECL, getVoidType())->asDECL();

    rval->sym = sym;
    rval->init = init;

    SetNodeExpr(tree, rval);
    return rval;
}


/*
 * Create a cast node with the given expression flags. Always returns true. If pexprDest is NULL,
 * just return true.
 */
bool FUNCBREC::bindSimpleCast(BASENODE * tree, EXPR * exprSrc, TYPESYM * typeDest, EXPR ** pexprDest, unsigned exprFlags)
{
    if (!pexprDest)
        return true;

    // If the source is a constant, and cast is really simple (no change in fundemental
    // type, no flags), then create a new constant node with the new type instead of
    // creating a cast node. This allows compile-time constants to be easily recognized.
    EXPR * exprConst = exprSrc->GetConst();

    if (exprConst && exprFlags == 0 &&
        exprSrc->type->fundType() == typeDest->fundType() &&
        (!exprSrc->type->isPredefType(PT_STRING) || exprConst->asCONSTANT()->getSVal().strVal == NULL))
    {
        EXPRCONSTANT * expr = newExprConstant(tree, typeDest, exprConst->asCONSTANT()->getSVal());
        *pexprDest = AddSideEffects(tree, expr, exprSrc, true);
        return true;
    }

    // Just alloc a new node and fill it in.
    EXPRCAST * expr = newExpr(tree, EK_CAST, typeDest)->asCAST();
    expr->p1 = exprSrc;
    expr->flags = exprFlags;
    if (checked.normal) {
        expr->flags |= EXF_CHECKOVERFLOW;
    }
    *pexprDest = expr;
    ASSERT(expr->p1);
    return true;
}

bool FUNCBREC::bindAnonMethConversion(BASENODE * tree, EXPR * exprSrc, TYPESYM * typeDest, EXPR ** pexprDest, bool fReportErrors)
{
    ASSERT(exprSrc && exprSrc->type->isANONMETHSYM() && exprSrc->kind == EK_ANONMETH);

    if (pexprDest)
        *pexprDest = NULL;

    if (!typeDest->isDelegateType()) {
        if (fReportErrors)
            compiler()->Error(tree, ERR_AnonMethToNonDel, typeDest);
        return false;
    }

    if (!exprSrc || exprSrc->kind != EK_ANONMETH)
        return false;

    AGGTYPESYM * type = typeDest->asAGGTYPESYM();

    METHSYM * invoke = compiler()->getBSymmgr().LookupInvokeMeth(type->getAggregate());
    if (!invoke || !invoke->isInvoke()) {
        return false;
    }

    AnonMethInfo * pami = exprSrc->asANONMETH()->pInfo;
    TypeArray * invokeParams = compiler()->getBSymmgr().SubstTypeArray(invoke->params, type);
    TYPESYM * invokeRetType = compiler()->getBSymmgr().SubstType(invoke->retType, type);
    bool result = true;

    if (pami->params) {
        // Check parameter lists if the user gave one
        if (pami->params != invokeParams) {
            // the error case, parameter lists don't match exactly
            if (!fReportErrors)
                return false;

            compiler()->Error(tree, ERR_CantConvAnonMethParams, type);
            result = false;

            if (pami->params->size != invokeParams->size) {
                compiler()->Error(exprSrc->tree, ERR_BadDelArgCount, type, pami->params->size);
            }
            else {
                int p = 0;
                bool bReportedError = false;
                NODELOOP(exprSrc->tree->asANONBLOCK()->pArgs, PARAMETER, param)
                    TYPESYM * from = pami->params->Item(p);
                    TYPESYM * to = invokeParams->Item(p);
                    p++;
                    if (from != to) {
                        TYPESYM * fromStripped = from->isPARAMMODSYM() ? from->asPARAMMODSYM()->paramType() : from;
                        TYPESYM * toStripped = to->isPARAMMODSYM() ? to->asPARAMMODSYM()->paramType() : to;
                        if (fromStripped == toStripped) {
                            if (toStripped != to) {
                                compiler()->Error(param, ERR_BadParamRef, p, to->asPARAMMODSYM()->isOut ? L"out" : L"ref");
                                bReportedError = true;
                            } else {
                                // the argument is decorated, but doesn't needs a 'ref' or 'out'
                                compiler()->Error(param, ERR_BadParamExtraRef, p, from->asPARAMMODSYM()->isOut ? L"out" : L"ref");
                                bReportedError = true;
                            }
                        } else {
                            compiler()->Error(param, ERR_BadParamType, p, ErrArg(from, ErrArgFlags::Unique), ErrArg(to, ErrArgFlags::Unique));
                            bReportedError = true;
                        }
                    }
                ENDLOOP;
                ASSERT(bReportedError);
            }
        }
    } else {
        // The user gave no parameter list so this AM is compatible with any signature containing no out parameters
        for (int p = 0; p < invokeParams->size;) {
            TYPESYM * to = invokeParams->Item(p);
            p++;
            if (to->isPARAMMODSYM() && to->asPARAMMODSYM()->isOut) {
                if (!fReportErrors)
                    return false;

                if (result) {
                    // only report this once
                    compiler()->Error(tree, ERR_CantConvAnonMethNoParams, type);
                    result = false;
                }

                // They need to add an 'out' if they want to use this signature
                compiler()->Error(tree, ERR_BadParamRef, p, L"out");
            }
        }
    }

    // Check (and possibly cast) return types
    bool returnResult = true;
    if (invokeRetType == getVoidType()) {
        // delegate returns void, so there must be either no return statements, or only return statements with no value/object
        EXPRLOOP(pami->listReturns, pReturn)
            if (pReturn->asRETURN()->object) {
                if (!fReportErrors) {
                    return false;
                }

                if (returnResult)
                    compiler()->Error(tree, ERR_CantConvAnonMethReturns, type);
                // return non-empty
                compiler()->Error(tree, ERR_RetNoObjectRequired, type);
                returnResult = false;
            }
        ENDLOOP;
    } else if (pami->listReturns) {
        // delegate returns a value, check that all the return statements are IMPLICITLY convertable
        EXPRLOOP(pami->listReturns, pReturn)
            if (pReturn->asRETURN()->object == NULL || !canConvert(pReturn->asRETURN()->object, invokeRetType)) {
                if (!fReportErrors) {
                    return false;
                }
                if (returnResult)
                    compiler()->Error(tree, ERR_CantConvAnonMethReturns, type);
                if (pReturn->asRETURN()->object)
                    mustConvert(pReturn->asRETURN()->object, invokeRetType);
                else
                    compiler()->Error(tree, ERR_RetObjectRequired, invoke->retType);
                returnResult = false;
            }
        ENDLOOP;
        if (returnResult && pami->pBodyExpr->FReachableEnd()) {
            if (!fReportErrors)
                return false;
            compiler()->Error(pami->tree, ERR_AnonymousReturnExpected, typeDest);
            returnResult = false;
        }
    } else if (pami->pBodyExpr->FReachableEnd()) {
        // Non-void returning delegate, and the anonymous method has no return statements and
        // a reachable end.
        if (!fReportErrors)
            return false;
        compiler()->Error(pami->tree, ERR_AnonymousReturnExpected, typeDest);
        returnResult = false;
    }

    if (result && returnResult && pexprDest) {
        (*pexprDest) = exprSrc;
        (*pexprDest)->setType(type);
        ASSERT(!pami->pDelegateType);
        pami->pDelegateType = type;
        if (!pami->params)
            pami->params = invokeParams;
        pami->typeRet = invokeRetType;

        SetNodeExpr(tree, *pexprDest);

        // Now cast all the return expressions
        if (invokeRetType != getVoidType()) {
            EXPRLOOP(pami->listReturns, pReturn)
                pReturn->asRETURN()->object = mustConvert(pReturn->asRETURN()->object, invokeRetType);
                ASSERT(pReturn->asRETURN()->object && pReturn->asRETURN()->object->isOK());
            ENDLOOP;
        }
    }

    return (result && returnResult);
}


/*
 * Check to see if an integral constant is within range of a integral destination type.
 */
bool FUNCBREC::isConstantInRange(EXPRCONSTANT * exprSrc, TYPESYM * typeDest, bool realsOk)
{
    FUNDTYPE ftSrc = exprSrc->type->fundType();
    FUNDTYPE ftDest = typeDest->fundType();

    if (ftSrc > FT_LASTINTEGRAL || ftDest > FT_LASTINTEGRAL) {
        if (!realsOk) {
            return false;
        } else if (ftSrc > FT_LASTNUMERIC || ftDest > FT_LASTNUMERIC) {
            return false;
        }
    }

    // if converting to a float type, this always suceeds...
    if (ftDest > FT_LASTINTEGRAL) {
        return true;
    }

    // if converting from float to an integral type, we need to check whether it fits
    if (ftSrc > FT_LASTINTEGRAL) {
        double dvalue = *(exprSrc->asCONSTANT()->getVal().doubleVal);

        if (!_finite(dvalue)) {
            return false;
        }

        switch (ftDest) {
        case FT_I1: if (dvalue > -0x81 && dvalue < 0x80) return true;  break;
        case FT_I2: if (dvalue > -0x8001 && dvalue < 0x8000) return true;  break;
        case FT_I4: if (dvalue > I64(-0x80000001) && dvalue < I64(0x80000000)) return true;  break;
        case FT_I8:
            // this code is shared by fjitdef.h
            if (dvalue < I64(-0x7000000000000000)) {
                double tmp = dvalue - I64(-0x7000000000000000);
                if (tmp > I64(-0x7000000000000000) &&
                    (__int64)tmp > I64(-0x1000000000000001)) {
                    return true;
                }
            }
            else {
                if (dvalue > I64(0x7000000000000000)) {
                    double tmp = dvalue - I64(0x7000000000000000);
                    if (tmp < I64(0x7000000000000000) &&
                        (__int64)tmp < I64(0x1000000000000000)) {
                        return true;
                    }
                }
                else {
                    return true;
                }
            }
            break;
        case FT_U1: if (dvalue > -1 && dvalue < 0x800) return true;  break;
        case FT_U2: if (dvalue > -1 && dvalue < 0x80000) return true;  break;
        case FT_U4: if (dvalue > -1 && dvalue < I64(0x800000000)) return true;  break;
        case FT_U8:
            // this code is shared by fjitdef.h
            if (dvalue < UI64(0xF000000000000000)) {
                if (dvalue > -1) {
                    return true;
                }
            }
            else {
                double tmp = dvalue - UI64(0xF000000000000000);
                if (tmp < I64(0x7000000000000000) &&
                    (__int64)tmp < I64(0x1000000000000000)) {
                    return true;
                }
            }
            break;
        default: break;
        }
        return false;
    }

    __int64 value = exprSrc->asCONSTANT()->getI64Value();

    // U8 src is unsigned, so deal with values > MAX_LONG here.
    if (ftSrc == FT_U8) {
        if (ftDest == FT_U8)
            return true;
        if (value < 0)  // actually > MAX_LONG.
            return false;
    }

    switch (ftDest) {
    case FT_I1: if (value >= -128 && value <= 127) return true;  break;
    case FT_I2: if (value >= -0x8000 && value <= 0x7fff) return true;  break;
    case FT_I4: if (value >= I64(-0x80000000) && value <= I64(0x7fffffff)) return true;  break;
    case FT_I8: return true;
    case FT_U1: if (value >= 0 && value <= 0xff) return true;  break;
    case FT_U2: if (value >= 0 && value <= 0xffff) return true;  break;
    case FT_U4: if (value >= 0 && value <= I64(0xffffffff)) return true;  break;
    case FT_U8: if (value >= 0) return true;    break;
    default:
        break;
    }

    return false;
}

#if defined(_MSC_VER)
#pragma optimize("p", on)  // consistent floating point.
#endif // defined(_MSC_VER)

/*
 * Fold a constant cast. Returns true if the constant could be folded.
 */
bool FUNCBREC::bindConstantCast(BASENODE * tree, EXPR * exprSrc, TYPESYM * typeDest, EXPR ** pexprDest, bool * checkFailure)
{
    __int64 valueInt = 0;
    double valueFlt = 0;
    FUNDTYPE ftSrc = exprSrc->type->fundType();
    FUNDTYPE ftDest = typeDest->fundType();
    bool srcIntegral = (ftSrc <= FT_LASTINTEGRAL);

    EXPRCONSTANT * constSrc = exprSrc->GetConst()->asCONSTANT();
    ASSERT(constSrc);

    bool explicitConversion = *checkFailure;

    *checkFailure = false;

    if (ftSrc == FT_STRUCT || ftDest == FT_STRUCT) {
        // Do constant folding involving decimal constants.
        EXPR * expr = bindDecimalConstCast(tree, typeDest, exprSrc->type, constSrc);

        if (!expr)
            return false;
        if (pexprDest)
            *pexprDest = AddSideEffects(tree, expr, exprSrc, true);
        return true;
    }

    if (explicitConversion && checked.constant && !isConstantInRange(constSrc, typeDest, true)) {
        *checkFailure = true;
        return false;
    }

    if (!pexprDest)
        return true;

    // Get the source constant value into valueInt or valueFlt.
    if (srcIntegral)
        valueInt = constSrc->getI64Value();
    else
        valueFlt = * constSrc->getVal().doubleVal;

    // Convert constant to the destination type, truncating if necessary.
    // valueInt or valueFlt contains the result of the conversion.
    switch (ftDest) {
    case FT_I1:
        if (!srcIntegral)  valueInt = (__int64) valueFlt;
        valueInt = (signed char) (valueInt & 0xFF);
        break;
    case FT_I2:
        if (!srcIntegral)  valueInt = (__int64) valueFlt;
        valueInt = (signed short) (valueInt & 0xFFFF);
        break;
    case FT_I4:
        if (!srcIntegral)  valueInt = (__int64) valueFlt;
        valueInt = (signed int) (valueInt & 0xFFFFFFFF);
        break;
    case FT_I8:
        if (!srcIntegral)  valueInt = (__int64) valueFlt;
        break;
    case FT_U1:
        if (!srcIntegral)  valueInt = (__int64) valueFlt;
        valueInt = (unsigned char) (valueInt & 0xFF);
        break;
    case FT_U2:
        if (!srcIntegral)  valueInt = (__int64) valueFlt;
        valueInt = (unsigned short) (valueInt & 0xFFFF);
        break;
    case FT_U4:
        if (!srcIntegral)  valueInt = (__int64) valueFlt;
        valueInt = (unsigned int) (valueInt & 0xFFFFFFFF);
        break;
    case FT_U8:
        if (!srcIntegral)  {
            valueInt = (unsigned __int64) valueFlt;
            // code below stolen from jit...
            const double two63  = 2147483648.0 * 4294967296.0;
            if (valueFlt < two63) {
                valueInt = (__int64)valueFlt;
            } else {
                valueInt = ((__int64)( valueFlt - two63)) + I64(0x8000000000000000);
            }
        }
        break;
    case FT_R4:
    case FT_R8:
        if (srcIntegral) {
            if (ftSrc == FT_U8)
                valueFlt = (double) (unsigned __int64) valueInt;
            else
                valueFlt = (double) valueInt;
        }
        if (ftDest == FT_R4) {
            // Force to R4 precision/range.
            float f;
            RoundToFloat(valueFlt, &f);
            valueFlt = f;
        }
        break;
    default:
        ASSERT(0);
        break;
    }

    // Create a new constant with the value in "valueInt" or "valueFlt".
    {
        CONSTVAL cv;
        if (ftDest == FT_U4)
            cv.uiVal = (unsigned int) valueInt;
        else if (ftDest <= FT_LASTNONLONG)
            cv.iVal = (int) valueInt;
        else if (ftDest <= FT_LASTINTEGRAL) {
            cv.longVal = (__int64 *) allocator->Alloc(sizeof(__int64));
            * cv.longVal = valueInt;
        }
        else {
            cv.doubleVal = (double *) allocator->Alloc(sizeof(double));
            * cv.doubleVal = valueFlt;
        }
        EXPRCONSTANT * expr = newExprConstant(tree, typeDest, cv);
        *pexprDest = AddSideEffects(tree, expr, exprSrc, true);
    }

    return true;
}

#if defined(_MSC_VER)
#pragma optimize("", on)  // restore default optimizations
#endif // defined(_MSC_VER)


/***************************************************************************************************
    This is a helper method for bindUserDefinedConversion. "Compares" two types relative to a
    base type and indicates which is "closer" to base. fImplicit(1|2) specifies whether there is a
    standard implicit conversion from base to type(1|2). If fImplicit(1|2) is false there should
    be a standard explicit conversion from base to type(1|2). The partial ordering used is as
    follows:

    * If exactly one of fImplicit(1|2) is true then the corresponding type is closer.
    * Otherwise if there is a standard implicit conversion in neither direction or both directions
      then neither is closer.
    * Otherwise if both of fImplicit(1|2) are true:
        * If there is a standard implicit conversion from type(1|2) to type(2|1) then type(1|2)
          is closer.
        * Otherwise neither is closer.
    * Otherwise both of fImplicit(1|2) are false and:
        * If there is a standard implicit conversion from type(1|2) to type(2|1) then type(2|1)
          is closer.
        * Otherwise neither is closer.

    The return value is -1 if type1 is closer, +1 if type2 is closer and 0 if neither is closer.
***************************************************************************************************/
int FUNCBREC::CompareSrcTypesBased(TYPESYM * type1, bool fImplicit1, TYPESYM * type2, bool fImplicit2)
{
    ASSERT(type1 != type2);

    if (fImplicit1 != fImplicit2)
        return fImplicit1 ? -1 : +1;

    bool fCon1 = canConvert(type1, type2, NOUDC);
    bool fCon2 = canConvert(type2, type1, NOUDC);

    if (fCon1 == fCon2)
        return 0;

    return (fImplicit1 == fCon1) ? -1 : +1;
}


/***************************************************************************************************
    This is a helper method for bindUserDefinedConversion. "Compares" two types relative to a
    base type and indicates which is "closer" to base. fImplicit(1|2) specifies whether there is a
    standard implicit conversion from type(1|2) to base. If fImplicit(1|2) is false there should
    be a standard explicit conversion from type(1|2) to base. The partial ordering used is as
    follows:

    * If exactly one of fImplicit(1|2) is true then the corresponding type is closer.
    * Otherwise if there is a standard implicit conversion in neither direction or both directions
      then neither is closer.
    * Otherwise if both of fImplicit(1|2) are true:
        * If there is a standard implicit conversion from type(1|2) to type(2|1) then type(2|1)
          is closer.
        * Otherwise neither is closer.
    * Otherwise both of fImplicit(1|2) are false and:
        * If there is a standard implicit conversion from type(1|2) to type(2|1) then type(1|2)
          is closer.
        * Otherwise neither is closer.

    The return value is -1 if type1 is closer, +1 if type2 is closer and 0 if neither is closer.
***************************************************************************************************/
int FUNCBREC::CompareDstTypesBased(TYPESYM * type1, bool fImplicit1, TYPESYM * type2, bool fImplicit2)
{
    ASSERT(type1 != type2);

    if (fImplicit1 != fImplicit2)
        return fImplicit1 ? -1 : +1;

    bool fCon1 = canConvert(type1, type2, NOUDC);
    bool fCon2 = canConvert(type2, type1, NOUDC);

    if (fCon1 == fCon2)
        return 0;

    return (fImplicit1 == fCon1) ? +1 : -1;
}


// Used by bindUserDefinedConversion
struct UdConvInfo {
    MethWithType mwt;
    bool fSrcImplicit;
    bool fDstImplicit;
};


/***************************************************************************************************
    Binds a user-defined conversion. The parameters to this procedure are the same as
    bindImplicitConversion, except the last: implicitOnly - only consider implicit conversions.

    This is a helper routine for bindImplicitConversion and bindExplicitConversion.

    It's non trivial to get this right in the presence of generics. e.g.

        class D<B,C> {
            static implicit operator B (D<B,C> x) { ... }
        }

        class E<A> : D<List<A>, A> { }

        E<int> x;
        List<int> y = x;

    The locals below would have the following values:

        typeList->sym: D<List<A>, A>
        typeCur: E<int>
        typeConv = subst(typeList->sym, typeCur)
                 = subst(D<List<!0>, !0>, <int>) = D<List<int>, int>

        retType: B
        typeTo = subst(retType, typeConv)
               = subst(!0, <List<int>, int>) = List<int>
        params->Item(0): D<B,C>
        typeFrom = subst(params->Item(0), typeConv)
                 = subst(D<!0,!1>, <List<int>, int>)
                 = D<List<int>, int> = typeConv

    REVIEW ShonK: Consider caching the results of this. Performing this over and over again
    could be a big time waster.

    For lifting over nullable:
    * Look in the most base types for the conversions (not in System.Nullable).
    * We only lift if both the source type and destination type are nullable and the input
      or output of the conversion is not a nullable.
    * When we lift we count the number of types (0, 1, 2) that need to be lifted.
      A conversion that needs fewer lifts is better than one that requires more (if the lifted
      forms have identical signatures).
***************************************************************************************************/
bool FUNCBREC::bindUserDefinedConversion(BASENODE * tree, EXPR * exprSrc, TYPESYM * typeSrc, TYPESYM * typeDst, EXPR ** pexprDst, bool fImplicitOnly)
{
    ASSERT(!exprSrc || exprSrc->type == typeSrc);

    // If either type is an interface we should never employ a UD conversion.
    if (!typeSrc || !typeDst || typeSrc->isInterfaceType() || typeDst->isInterfaceType())
        return false;

    TYPESYM * typeSrcBase = typeSrc->StripNubs();
    TYPESYM * typeDstBase = typeDst->StripNubs();

    bool fLiftSrc = typeSrcBase != typeSrc;
    bool fLiftDst = typeDstBase != typeDst;
    bool fDstHasNull = fLiftDst || typeDst->IsRefType() || typeDst->isPTRSYM();

    AGGTYPESYM * rgats[2];
    int cats = 0;

    // This will be true if it must be the case that either the operator is implicit
    // or the from-type of the operator must be the same as the source type.
    // This is true when the source type is a type variable.
    bool fImplicitOrExactSrc = fImplicitOnly;

    // This flag will be true if we should ignore the IntPtr/UIntPtr -> int/uint conversion
    // in favor of the IntPtr/UIntPtr -> long/ulong conversion.
    bool fIntPtrOverride2 = false;

    // Get the list of operators from the source.
    if (typeSrcBase->isTYVARSYM()) {
        AGGTYPESYM * atsBase = typeSrcBase->asTYVARSYM()->GetBaseCls();
        if (atsBase->getAggregate()->hasConversion)
            rgats[cats++] = atsBase;

        // If an implicit conversion exists from the class bound to typeDst, then
        // an implicit conversion exists from typeSrc to typeDst. An explicit from
        // the class bound to typeDst doesn't buy us anything.
        // We can still use an explicit conversion that has this type variable (or
        // nullable of it) as its from-type.
        fImplicitOrExactSrc = true;
    }
    else if (typeSrcBase->isAGGTYPESYM() && typeSrcBase->getAggregate()->hasConversion) {
        rgats[cats++] = typeSrcBase->asAGGTYPESYM();
        fIntPtrOverride2 = typeSrcBase->isPredefType(PT_INTPTR) || typeSrcBase->isPredefType(PT_UINTPTR);
    }

    // Get the list of operators from the destination.
    if (typeDstBase->isTYVARSYM()) {
        // If an explicit conversion exists from typeSrc to the class bound, then
        // an explicit conversion exists from typeSrc to typeDst. An implicit is no better
        // than an explicit.
        AGGTYPESYM * atsBase;
        if (!fImplicitOnly && (atsBase = typeDstBase->asTYVARSYM()->GetBaseCls())->getAggregate()->hasConversion)
            rgats[cats++] = atsBase;
    }
    else if (typeDstBase->isAGGTYPESYM()) {
        if (typeDstBase->getAggregate()->hasConversion)
            rgats[cats++] = typeDstBase->asAGGTYPESYM();
        if (fIntPtrOverride2 && !typeDstBase->isPredefType(PT_LONG) && !typeDstBase->isPredefType(PT_ULONG))
            fIntPtrOverride2 = false;
    }
    else
        fIntPtrOverride2 = false;

    // If there are no user defined conversions, we're done.
    if (!cats)
        return false;

    UdConvInfo rguciRaw[8];
    UdConvInfo * prguci = rguciRaw;
    int cuciMax = lengthof(rguciRaw);
    int cuci = 0;

    TYPESYM * typeBestSrc = NULL;
    TYPESYM * typeBestDst = NULL;
    bool fBestSrcExact = false;
    bool fBestDstExact = false;
    int iuciBestSrc = -1;
    int iuciBestDst = -1;

    TYPESYM * typeFrom;
    TYPESYM * typeTo;

    // In the first pass if we find types that are non-comparable, keep one of the types and keep going.
    // In the second pass, we verify that the types we ended up with are indeed minimal and find the one valid conversion.
    for (int iats = 0; iats < cats; iats++) {
        for (AGGTYPESYM * atsCur = rgats[iats]; atsCur && atsCur->getAggregate()->hasConversion; atsCur = atsCur->GetBaseClass()) {
            AGGSYM * aggCur = atsCur->getAggregate();

            // We need to ship with an RTM bug that allows non-standard conversions with these guys.
            bool fIntPtrStandard = (aggCur->isPredefined && (aggCur->iPredef == PT_INTPTR || aggCur->iPredef == PT_UINTPTR || aggCur->iPredef == PT_DECIMAL));

            for (METHSYM * convCur = aggCur->convFirst; convCur; convCur = convCur->ConvNext()) {
                ASSERT(convCur->params->size == 1);
                ASSERT(convCur->getClass() == aggCur);

                if (fImplicitOnly && !convCur->isImplicit())
                    continue;
                if (compiler()->CheckBogus(convCur))
                    continue;

                // Get the substituted src and dst types.
                typeFrom = compiler()->getBSymmgr().SubstType(convCur->params->Item(0), atsCur);
                typeTo = compiler()->getBSymmgr().SubstType(convCur->retType, atsCur);

                bool fNeedImplicit = fImplicitOnly;

                // If fImplicitOrExactSrc is set then it must be the case that either the conversion
                // is implicit or the from-type must be the src type (modulo nullables).
                if (fImplicitOrExactSrc && !fNeedImplicit && typeFrom->StripNubs() != typeSrcBase) {
                    if (!convCur->isImplicit())
                        continue;
                    fNeedImplicit = true;
                }

                {
                    FUNDTYPE ftFrom;
                    FUNDTYPE ftTo;

                    if ((ftTo = typeTo->fundType()) <= FT_LASTNUMERIC && ftTo > FT_NONE &&
                        (ftFrom = typeFrom->fundType()) <= FT_LASTNUMERIC && ftFrom > FT_NONE)
                    {
                        continue;
                    }
                }

                // Ignore the IntPtr/UIntPtr -> int/uint conversion in favor of
                // the IntPtr/UIntPtr -> long/ulong conversion.
                if (fIntPtrOverride2 && (typeTo->isPredefType(PT_INT) || typeTo->isPredefType(PT_UINT)))
                    continue;

                // Lift the conversion if needed.
                if ((fLiftSrc & (fDstHasNull | !fNeedImplicit)) && typeFrom->IsNonNubValType())
                    typeFrom = compiler()->getBSymmgr().GetNubType(typeFrom);
                if (fLiftDst && typeTo->IsNonNubValType())
                    typeTo = compiler()->getBSymmgr().GetNubType(typeTo);

                // Check for applicability.
                bool fFromImplicit = exprSrc ? canConvert(exprSrc, typeFrom, STANDARDANDNOUDC) : canConvert(typeSrc, typeFrom, STANDARDANDNOUDC);
                if (!fFromImplicit && (fNeedImplicit ||
                    !canConvert(typeFrom, typeSrc, STANDARDANDNOUDC) &&
                    // We allow IntPtr and UIntPtr to use non-standard explicit casts as long as they don't involve pointer types.
                    // This is because the framework uses it and RTM allowed it.
                    (!fIntPtrStandard || typeSrc->isPTRSYM() || typeFrom->isPTRSYM() || !canCast(typeSrc, typeFrom, NOUDC))))
                {
                    continue;
                }
                bool fToImplicit = canConvert(typeTo, typeDst, STANDARDANDNOUDC);
                if (!fToImplicit && (fNeedImplicit ||
                    !canConvert(typeDst, typeTo, STANDARDANDNOUDC) &&
                    // We allow IntPtr and UIntPtr to use non-standard explicit casts as long as they don't involve pointer types.
                    // This is because the framework uses it and RTM allowed it.
                    (!fIntPtrStandard || typeDst->isPTRSYM() || typeTo->isPTRSYM() || !canCast(typeTo, typeDst, NOUDC))))
                {
                    continue;
                }

                // The conversion is applicable so it affects the best types.
                if (cuci >= cuciMax) {
                    ASSERT(cuci == cuciMax);
                    UdConvInfo * prguciNew = STACK_ALLOC(UdConvInfo, cuciMax + cuciMax);
                    memcpy(prguciNew, prguci, cuci * sizeof(UdConvInfo));
                    cuciMax += cuciMax;
                    prguci = prguciNew;
                }
                ASSERT(cuci < cuciMax);
                int iuciCur = cuci++;

                prguci[iuciCur].mwt.Set(convCur, atsCur);
                prguci[iuciCur].fSrcImplicit = fFromImplicit;
                prguci[iuciCur].fDstImplicit = fToImplicit;

                if (!fBestSrcExact) {
                    if (typeFrom == typeSrc) {
                        ASSERT(!typeBestSrc == !typeBestDst); // If typeBestSrc is null then typeBestDst should be null.
                        ASSERT(fFromImplicit);
                        typeBestSrc = typeFrom;
                        iuciBestSrc = iuciCur;
                        fBestSrcExact = true;
                    }
                    else if (!typeBestSrc) {
                        ASSERT(iuciBestSrc == -1);
                        typeBestSrc = typeFrom;
                        iuciBestSrc = iuciCur;
                    }
                    else if (typeBestSrc != typeFrom) {
                        ASSERT(0 <= iuciBestSrc && iuciBestSrc < iuciCur);
                        int n = CompareSrcTypesBased(typeBestSrc, prguci[iuciBestSrc].fSrcImplicit, typeFrom, fFromImplicit);
                        if (n > 0) {
                            typeBestSrc = typeFrom;
                            iuciBestSrc = iuciCur;
                        }
                    }
                }

                if (!fBestDstExact) {
                    if (typeTo == typeDst) {
                        ASSERT(fToImplicit);
                        typeBestDst = typeTo;
                        iuciBestDst = iuciCur;
                        fBestDstExact = true;
                    }
                    else if (!typeBestDst) {
                        ASSERT(iuciBestDst == -1);
                        typeBestDst = typeTo;
                        iuciBestDst = iuciCur;
                    }
                    else if (typeBestDst != typeTo) {
                        ASSERT(0 <= iuciBestDst && iuciBestDst < iuciCur);
                        int n = CompareDstTypesBased(typeBestDst, prguci[iuciBestDst].fDstImplicit, typeTo, fToImplicit);
                        if (n > 0) {
                            typeBestDst = typeTo;
                            iuciBestDst = iuciCur;
                        }
                    }
                }
            }
        }
    }

    ASSERT(!typeBestSrc == !typeBestDst);
    if (!typeBestSrc) {
        ASSERT(iuciBestSrc == -1 && iuciBestDst == -1);
        return false;
    }

    ASSERT(0 <= iuciBestSrc && iuciBestSrc < cuci);
    ASSERT(0 <= iuciBestDst && iuciBestDst < cuci);

    int ctypeLiftBest = 3; // Bigger than any legal value on purpose.
    int iuciBest = -1;
    int iuciAmbig = -1;

    // In the second pass, we verify that the types we ended up with are indeed minimal and find the one valid conversion.
    for (int iuci = 0; iuci < cuci; iuci++) {
        UdConvInfo & uci = prguci[iuci];

        // Get the substituted src and dst types.
        typeFrom = compiler()->getBSymmgr().SubstType(uci.mwt.Meth()->params->Item(0), uci.mwt.Type());
        typeTo = compiler()->getBSymmgr().SubstType(uci.mwt.Meth()->retType, uci.mwt.Type());

        int ctypeLift = 0;

        // Lift the conversion if needed.
        if (fLiftSrc && typeFrom->IsNonNubValType()) {
            typeFrom = compiler()->getBSymmgr().GetNubType(typeFrom);
            ctypeLift++;
        }
        if (fLiftDst && typeTo->IsNonNubValType()) {
            typeTo = compiler()->getBSymmgr().GetNubType(typeTo);
            ctypeLift++;
        }

        if (typeFrom == typeBestSrc && typeTo == typeBestDst) {
            // Record the matching conversions.
            if (ctypeLiftBest > ctypeLift) {
                // This one is better.
                iuciBest = iuci;
                iuciAmbig = -1;
                ctypeLiftBest = ctypeLift;
                continue;
            }

            if (ctypeLiftBest < ctypeLift) {
                // Current answer is better.
                continue;
            }

            // Ambiguous at this lifting level. This only guarantees an error if the
            // lifting level is zero.
            if (iuciAmbig < 0) {
                iuciAmbig = iuci;
                if (ctypeLift == 0) {
                    // No point continuing. We have an error.
                    break;
                }
            }
            continue;
        }

        ASSERT(typeFrom != typeBestSrc || typeTo != typeBestDst);

        // Verify that the best types are indeed best. Must NOT compare if the best type is exact.
        // This is not just an efficiency issue. With nullables there are types that are implicitly
        // convertible to each other (eg, int? and int??) and hence not distinguishable by CompareXxxTypesBase.
        if (!fBestSrcExact && typeFrom != typeBestSrc) {
            int n = CompareSrcTypesBased(typeBestSrc, prguci[iuciBestSrc].fSrcImplicit, typeFrom, uci.fSrcImplicit);
            ASSERT(n <= 0);
            if (n >= 0) {
                if (!pexprDst)
                    return true;
                iuciBestDst = iuci;
                goto LAmbig;
            }
        }
        if (!fBestDstExact && typeTo != typeBestDst) {
            int n = CompareDstTypesBased(typeBestDst, prguci[iuciBestDst].fDstImplicit, typeTo, uci.fDstImplicit);
            ASSERT(n <= 0);
            if (n >= 0) {
                if (!pexprDst)
                    return true;
                iuciBestDst = iuci;
                goto LAmbig;
            }
        }
    }

    if (!pexprDst)
        return true;

    if (iuciBest < 0)
        goto LAmbig;
    if (iuciAmbig >= 0) {
        iuciBestSrc = iuciBest;
        iuciBestDst = iuciAmbig;
LAmbig:
        ASSERT(0 <= iuciBestSrc && iuciBestSrc < cuci);
        ASSERT(0 <= iuciBestDst && iuciBestDst < cuci);
        compiler()->Error(tree, ERR_AmbigUDConv, prguci[iuciBestSrc].mwt, prguci[iuciBestDst].mwt, typeSrc, typeDst);
        *pexprDst = newError(tree, typeDst);
        return true;
    }

    MethWithInst mwiBest(prguci[iuciBest].mwt.Meth(), prguci[iuciBest].mwt.Type(), NULL);

    ASSERT(ctypeLiftBest <= 2);
    if (mwiBest.Meth()->IsDeprecated() && tree) {
        ReportDeprecated(tree, mwiBest);
    }

    typeFrom = compiler()->getBSymmgr().SubstType(mwiBest.Meth()->params->Item(0), mwiBest.Type());
    typeTo = compiler()->getBSymmgr().SubstType(mwiBest.Meth()->retType, mwiBest.Type());

    EXPR * exprFrom = exprSrc;
    NubInfo nin;

    if (ctypeLiftBest > 0 && !typeFrom->isNUBSYM() && fDstHasNull) {
        // Need to lift over the null.
        ASSERT(fLiftSrc || fLiftDst);
        BindNubCondValBin(tree, exprSrc, NULL, nin);
        ASSERT(nin.fActive);
        exprFrom = nin.Val(0);
    }
    else
        ASSERT(!nin.fActive);

    exprFrom = mustCastCore(exprFrom, typeFrom, tree, NOUDC);
    ASSERT(exprFrom);

    EXPRCALL * exprCall = newExpr(tree, EK_CALL, typeTo)->asCALL();
    exprCall->args = exprFrom;
    exprCall->object = NULL;
    exprCall->mwi = mwiBest;

    EXPR * exprDst = mustCastCore(exprCall, typeDst, tree, NOUDC);
    ASSERT(exprDst);

    if (nin.fActive)
        exprDst = BindNubOpRes(tree, typeDst, exprDst, newExprZero(tree, typeDst), nin);

    *pexprDst = exprDst;

    return true;
}


/*
 * bindImplicitConversion
 *
 * This is a complex routine with complex parameters. Generally, this should
 * be called through one of the helper methods that insulates you
 * from the complexity of the interface. This routine handles all the logic
 * associated with implicit conversions.
 *
 * exprSrc - the expression being converted. Can be NULL if only type conversion
 *           info is being supplied.
 * typeSrc - type of the source
 * typeDest - type of the destination
 * exprDest - returns an expression of the src converted to the dest. If NULL, we
 *            only care about whether the conversion can be attempted, not the
 *            expression tree.
 * flags    - flags possibly customizing the conversions allowed. E.g., can suppress
 *            user-defined conversions.
 *
 * returns true if the conversion can be made, false if not.
 */
bool FUNCBREC::bindImplicitConversion(BASENODE * tree, EXPR * exprSrc, TYPESYM * typeSrc, TYPESYM * typeDest, EXPR ** pexprDest, unsigned flags)
{
    // Can't convert to or from the error type.
    if (!typeSrc || !typeDest || typeDest->IsNeverSameType())
        return false;

    ASSERT(typeSrc && typeDest);                            // types must be supplied.
    ASSERT(exprSrc == NULL || typeSrc == exprSrc->type);    // type of source should be correct if source supplied
    ASSERT(pexprDest == NULL || exprSrc != NULL);           // need source expr to create dest expr

    // Make sure both types are declared.
    compiler()->EnsureState(typeSrc);
    compiler()->EnsureState(typeDest);

    switch (typeDest->getKind()) {
    case SK_ERRORSYM:
        ASSERT(typeDest->parent);
        if (typeSrc != typeDest)
            return false;
        if (pexprDest)
            *pexprDest = exprSrc;
        return true;
    case SK_NULLSYM:
        // Can only convert to the null type if src is null.
        if (!typeSrc->isNULLSYM())
            return false;
        if (pexprDest)
            *pexprDest = exprSrc;
        return true;
    case SK_ANONMETHSYM:
    case SK_METHGRPSYM:
        VSFAIL("Something is wrong with TYPESYM::IsNeverSameType()");
        return false;
    case SK_VOIDSYM:
        return false;
    default:
        break;
    }

    if (typeSrc->isERRORSYM()) {
        ASSERT(!typeDest->isERRORSYM());
        return false;
    }

    // Does the trivial conversion exist?
    if (typeSrc == typeDest &&
        (!(flags & ISEXPLICIT) || (!typeSrc->isPredefType(PT_FLOAT) && !typeSrc->isPredefType(PT_DOUBLE))))
    {
        if (pexprDest)
            *pexprDest = exprSrc;
        return true;
    }

    if (typeDest->isNUBSYM())
        return BindNubConversion(tree, exprSrc, typeSrc, typeDest->asNUBSYM(), pexprDest, flags);

    // Boxing converions from T? to object, ValueType and the interfaces supported by T....
    if (typeSrc->isNUBSYM()) {
        AGGTYPESYM * atsNub = typeSrc->asNUBSYM()->GetAts();
        if (!atsNub)
            return false;
        if (atsNub == typeDest) {
            if (pexprDest)
                *pexprDest = exprSrc;
            return true;
        }
        if (compiler()->IsBaseType(typeSrc->asNUBSYM()->baseType(), typeDest) && !FUnwrappingConv(typeSrc,typeDest))
            return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, EXF_BOX);
        return !(flags & NOUDC) && bindUserDefinedConversion(tree, exprSrc, typeSrc, typeDest, pexprDest, true);
    }

    if (flags & ISEXPLICIT)
        flags |= NOUDC;

    // Get the fundamental types of destination.
    FUNDTYPE ftDest = typeDest->fundType();
    ASSERT(ftDest != FT_NONE || typeDest->isPARAMMODSYM());

    switch (typeSrc->getKind()) {
    default:
        ASSERT(0);
        break;

    case SK_METHGRPSYM:
        return BindGrpConversion(tree, exprSrc->asMEMGRP(), typeDest, pexprDest, false);
    case SK_ANONMETHSYM:
        return bindAnonMethConversion(tree, exprSrc, typeDest, pexprDest);

    case SK_VOIDSYM:
    case SK_ERRORSYM:
    case SK_PARAMMODSYM:
        return false;

    case SK_NULLSYM:
        // null type can be implicitly converted to any reference type or pointer type or type variable with reference-type constraint.
        if (ftDest != FT_REF && ftDest != FT_PTR &&
            (ftDest != FT_VAR || !typeDest->asTYVARSYM()->IsRefType()) &&
            // null is convertible to System.Nullable<T>.
            !typeDest->isPredefType(PT_G_OPTIONAL))
        {
            break;
        }
        if (pexprDest) {
            ASSERT(exprSrc);
            *pexprDest = AddSideEffects(tree, newExprZero(tree, typeDest), exprSrc, true);
        }
        return true;

    case SK_ARRAYSYM:
        /*
        * Handle array conversions.
        * An array type can be implicitly converted to some other array types, Object,
        * or Array or some base class/interface that Array handles. Also, IList<U> whenever
        * there is an implicit reference conversion from the element type to U and the rank is 1.
        */
        if (compiler()->IsBaseType(typeSrc, typeDest)) {
            uint grfex = 0;

            if ((typeDest->isARRAYSYM() ||
                typeDest->isInterfaceType() &&
                    typeDest->asAGGTYPESYM()->typeArgsAll->size == 1 &&
                    typeDest->asAGGTYPESYM()->typeArgsAll->Item(0) != typeSrc->asARRAYSYM()->elementType()) &&
                (compiler()->getBSymmgr().TypeContainsTyVars(typeSrc, NULL) ||
                    compiler()->getBSymmgr().TypeContainsTyVars(typeDest, NULL)))
            {
                grfex = EXF_REFCHECK;
            }
            return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, grfex);
        }
        break;

    case SK_PTRSYM:
        /*
         * Handle pointer conversions.
         * A pointer can be implicitly converted to void *. That's it.
         */
        if (typeDest->isPTRSYM() && typeDest->asPTRSYM()->baseType() == getVoidType())
            return bindSimpleCast(tree, exprSrc, typeDest, pexprDest);
        break;

    case SK_TYVARSYM:
        /*
         * Handle implicit conversions for type variables
         * A type variable can be implicitly converted to its bounds (perhaps by boxing - will use generalized "box" instruction)
         */
        {
            // Check the effective base class first.
            TYPESYM * typeTmp = typeSrc->asTYVARSYM()->GetBaseCls();
            TypeArray * bnds = typeSrc->asTYVARSYM()->GetBnds();
            int itype = -1;

            for(;;) {
                ASSERT(!typeTmp->IsValType());
                if (canConvert(typeTmp, typeDest, flags | NOUDC)) {
                    if (!pexprDest)
                        ;
                    else if (typeDest->isTYVARSYM()) {
                        // For a type var destination we need to cast to object then to the other type var.
                        EXPR * exprT;
                        bindSimpleCast(tree, exprSrc, GetReqPDT(PT_OBJECT), &exprT, EXF_FORCE_BOX);
                        bindSimpleCast(tree, exprT, typeDest, pexprDest, EXF_FORCE_UNBOX);
                    }
                    else
                        bindSimpleCast(tree, exprSrc, typeDest, pexprDest, EXF_FORCE_BOX);
                    return true;
                }
LNext:
                if (++itype >= bnds->size)
                    break;
                typeTmp = bnds->Item(itype);
                if (!typeTmp->isInterfaceType() && !typeTmp->isTYVARSYM())
                    goto LNext;
            }
        }
        break;

    case SK_AGGTYPESYM: {
        // GENERICS: The case for constructed types is very similar to types with
        // no parameters. The parameters are irrelevant for most of the conversions
        // below. They could be relevant if we had user-defined conversions on
        // generic types.
        AGGSYM * aggSrc = typeSrc->asAGGTYPESYM()->getAggregate();

        // TypeReference and ArgIterator can't be boxed (or converted to anything else)
        if (typeSrc->isSpecialByRefType())
            return false;

        if (aggSrc->IsEnum()) {
            /*
             * Handle enum conversions.
             */
            if (typeDest->isAGGTYPESYM() && compiler()->IsBaseType(typeSrc->asAGGTYPESYM(), typeDest->asAGGTYPESYM())) {
                return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, EXF_BOX | EXF_CANTBENULL);
            }
            break;
        }

        if (typeDest->isEnumType()) {
            // No implicit conversion TO enum except the identity conversion
            // or literal '0'.
            // NB: this is NOT a standard conversion
            // Note: Don't use GetConst here since the conversion only applies to bona-fide compile time constants.
            if (aggSrc->iPredef != PT_BOOL && exprSrc && exprSrc->isZero(false) && (exprSrc->flags & EXF_LITERALCONST) && !(flags & STANDARD)) {
                if (pexprDest) {
                    *pexprDest = newExprConstant(tree, typeDest, compiler()->getBSymmgr().GetPredefZero(typeDest->underlyingEnumType()->getPredefType()));
                    SetNodeExpr(exprSrc->tree, *pexprDest);
                }
                return true;
            }
            break;
        }

        if (aggSrc->getThisType()->isSimpleType() && typeDest->isSimpleType()) {
            /*
             * Handle conversions between simple types. (int->long, float->double, etc...)
             */

            ASSERT(aggSrc->isPredefined && typeDest->isPredefined());
            int ptSrc = aggSrc->iPredef;
            int ptDest = typeDest->getPredefType();
            unsigned convertKind;
            bool fConstShrinkCast = false;

            ASSERT(ptSrc < NUM_SIMPLE_TYPES && ptDest < NUM_SIMPLE_TYPES);

            // Note: Don't use GetConst here since the conversion only applies to bona-fide compile time constants.
            if (exprSrc && exprSrc->kind == EK_CONSTANT &&
                ((ptSrc == PT_INT && ptDest != PT_BOOL && ptDest != PT_CHAR) ||
                 (ptSrc == PT_LONG && ptDest == PT_ULONG)) &&
                isConstantInRange(exprSrc->asCONSTANT(), typeDest))
            {
                // Special case (CLR 6.1.6): if integral constant is in range, the conversion is a legal implicit conversion.
                convertKind = CONV_KIND_IMP;
                fConstShrinkCast = pexprDest && (simpleTypeConversions[ptSrc][ptDest] & CONV_KIND_MASK) != CONV_KIND_IMP;
            } else if (ptSrc == ptDest) {
                // Special case: precision limiting casts to float or double
                ASSERT(ptSrc == PT_FLOAT || ptSrc == PT_DOUBLE);
                ASSERT(flags & ISEXPLICIT);
                convertKind = CONV_KIND_IMP;
            } else {
                convertKind = simpleTypeConversions[ptSrc][ptDest];
                ASSERT((convertKind & CONV_KIND_MASK) != CONV_KIND_ID);  // identity conversion should have been handled at first.
            }

            if ((convertKind & CONV_KIND_MASK) == CONV_KIND_IMP) {
                // An implicit conversion exists. Do the conversion.
                if (exprSrc->GetConst()) {
                    // Fold the constant cast if possible.
                    bool checkFailure = false;
                    if (bindConstantCast(tree, exprSrc, typeDest, pexprDest, &checkFailure)) {
                        ASSERT(!checkFailure);
                        // If we cast down in size because the value was in range, then map that
                        // node to the new value, so ExtractMethod doesn't introduce an error.
                        if (fConstShrinkCast)
                            SetNodeExpr(exprSrc->tree, *pexprDest);
                        return true;  // else, don't fold and use a regular cast, below.
                    }
                    ASSERT(!checkFailure); // since this is an implicit cast, it can never fail the check...
                }

                if (convertKind & CONV_KIND_USEUDC) {
                    if (!pexprDest) return true;
                    // According the language, this is a standard conversion, but it is implemented
                    // through a user-defined conversion. Because it's a standard conversion, we don't
                    // test the NOUDC flag here.
                    return bindUserDefinedConversion(tree, exprSrc, typeSrc, typeDest, pexprDest, true);
                }
                return bindSimpleCast(tree, exprSrc, typeDest, pexprDest);
            }

            // No break here, continue testing for derived to base conversions below.
        }

        /*
         * Handle struct, class and interface conversions. Delegates are handled just
         * like classes here. A class, struct, or interface is implicitly convertable
         * to a base class or interface.
         */
        if (typeDest->isAGGTYPESYM() && compiler()->IsBaseType(typeSrc->asAGGTYPESYM(), typeDest->asAGGTYPESYM())) {
            if (aggSrc->IsStruct() && ftDest == FT_REF)
                return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, EXF_BOX | EXF_CANTBENULL);
            else
                return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, exprSrc ? (exprSrc->flags & EXF_CANTBENULL) : 0);
        }
        break;
    }
    }

    // No built-in conversion was found. Maybe a user-defined conversion?
    if (!(flags & NOUDC)) {
        return bindUserDefinedConversion(tree, exprSrc, typeSrc, typeDest, pexprDest, true);
    }

    // No conversion was found.
    return false;
}


/***************************************************************************************************
    Determine whether there is an implicit reference conversion from typeSrc to typeDst. This is
    when the source is a reference type and the destination is a base type of the source. Note
    that typeDst->IsRefType() may still return false (when both are type parameters).
***************************************************************************************************/
bool FUNCBREC::FImpRefConv(TYPESYM * typeSrc, TYPESYM * typeDst)
{
    return typeSrc->IsRefType() && compiler()->IsBaseType(typeSrc, typeDst);
}


/***************************************************************************************************
    Determine whether there is an explicit or implicit reference conversion (or identity conversion)
    from typeSrc to typeDst. This is when:

    * Both src and dst are reference types and there is a builtin explicit conversion from
      src to dst.
    * Or src is a reference type and dst is a base type of src (in which case the conversion is
      implicit as well).
    * Or dst is a reference type and src is a base type of dst.

    The latter two cases can happen with type variables even though the other type variable is not
    a reference type.
***************************************************************************************************/
bool FUNCBREC::FExpRefConv(TYPESYM * typeSrc, TYPESYM * typeDst)
{
    if (typeSrc->IsRefType()) {
        if (typeDst->IsRefType())
            return canCast(typeSrc, typeDst, NOUDC);
        return compiler()->IsBaseType(typeSrc, typeDst);
    }
    return typeDst->IsRefType() && compiler()->IsBaseType(typeDst, typeSrc);
}

/***************************************************************************************************
    Determine whether two types are the same type.
    Always returns false if the types are error, anonymous method, or method group
***************************************************************************************************/
bool FUNCBREC::FIsSameType(TYPESYM * typeSrc, TYPESYM * typeDst)
{
    return typeSrc == typeDst && !typeSrc->IsNeverSameType();
}

/***************************************************************************************************
    Determines whether there is a boxing conversion from typeSrc to typeDest
***************************************************************************************************/
bool FUNCBREC::FBoxingConv(TYPESYM * typeSrc, TYPESYM * typeDst)
{
    return (typeSrc->IsValType() || (typeSrc->isTYVARSYM() && !typeSrc->IsRefType())) && typeDst->IsRefType() && canConvert(typeSrc, typeDst, NOUDC);
}

/***************************************************************************************************
    Determines whether there is an unboxing conversion from typeSrc to typeDest
***************************************************************************************************/
bool FUNCBREC::FUnboxingConv(TYPESYM * typeSrc, TYPESYM * typeDst)
{
    return FBoxingConv(typeDst, typeSrc);
}

/***************************************************************************************************
    Determines whether there is a wrapping conversion from typeSrc to typeDest
***************************************************************************************************/
bool FUNCBREC::FWrappingConv(TYPESYM * typeSrc, TYPESYM * typeDst)
{
    return typeDst->isNUBSYM() && typeSrc == typeDst->asNUBSYM()->baseType();
}

/***************************************************************************************************
    Determines whether there is a unwrapping conversion from typeSrc to typeDest
***************************************************************************************************/
bool FUNCBREC::FUnwrappingConv(TYPESYM * typeSrc, TYPESYM * typeDst)
{
    return FWrappingConv(typeDst, typeSrc);
}


// returns true if an implicit conversion exists from source type to dest type. flags is an optional parameter.
bool FUNCBREC::canConvert(TYPESYM * src, TYPESYM * dest, unsigned flags)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);
    return bindImplicitConversion(NULL, NULL, src, dest, NULL, flags);
}

// returns true if a implicit conversion exists from source expr to dest type. flags is an optional parameter.
bool FUNCBREC::canConvert(EXPR * expr, TYPESYM * dest, unsigned flags)
{
    return bindImplicitConversion(NULL, expr, expr->type, dest, NULL, flags);
}

// performs an implicit conversion if it's possible. otherwise displays an error. flags is an optional parameter.
EXPR * FUNCBREC::mustConvertCore(EXPR * expr, TYPESYM * dest, BASENODE * tree, unsigned flags)
{
    EXPR * exprResult;

    if (bindImplicitConversion(tree, expr, expr->type, dest, &exprResult, flags)) {
        // Conversion works.
        return exprResult;
    }

    if (expr->isOK() && !dest->isERRORSYM()) {
        // don't report cascading error.

        // For certain situations, try to give a better error.

        FUNDTYPE ftSrc = expr->type->fundType();
        FUNDTYPE ftDest = dest->fundType();

        if (expr->kind == EK_CONSTANT &&
            expr->type->isSimpleType() && dest->isSimpleType())
        {
            if ((ftSrc == FT_I4 && (ftDest <= FT_LASTNONLONG || ftDest == FT_U8)) ||
                (ftSrc == FT_I8 && ftDest == FT_U8))
            {
                // Failed because value was out of range. Report nifty error message.
                WCHAR value[40];
                StringCchPrintfW (value, lengthof(value), L"%I64d", expr->asCONSTANT()->getI64Value());

                compiler()->Error(tree, ERR_ConstOutOfRange, value, dest);
                return newError(tree, dest);
            }
            else if (ftSrc == FT_R8 && (expr->flags & EXF_LITERALCONST) &&
                (dest->isPredefType(PT_FLOAT) || dest->isPredefType(PT_DECIMAL)))
            {
                // Tried to assign a literal of type double (the default) to a float or decimal. Suggest use
                // of a 'F' or 'M' suffix.
                compiler()->Error(tree, ERR_LiteralDoubleCast, dest->isPredefType(PT_DECIMAL) ? L"M" : L"F", dest);
                return newError(tree, dest);
            }
        }

        if (expr->type->isNULLSYM() && dest->fundType() != FT_REF) {
            compiler()->Error(tree, dest->isTYVARSYM() ? ERR_TypeVarCantBeNull : ERR_ValueCantBeNull, dest);
        }
        else if (expr->kind == EK_MEMGRP) {
            BindGrpConversion(tree, expr->asMEMGRP(), dest, NULL, true);
        }
        else if (expr->type->isANONMETHSYM()) {
            bindAnonMethConversion(tree, expr, dest, NULL, true);
        }
        else if (canCast(expr->type, dest, flags)) {
            // can't convert, but explicit exists.
            compiler()->Error(tree, ERR_NoImplicitConvCast, ErrArg(expr->type, ErrArgFlags::Unique), ErrArg(dest, ErrArgFlags::Unique));
        }
        else {
            // Generic "can't convert" error.
            compiler()->Error(tree, ERR_NoImplicitConv, ErrArg(expr->type, ErrArgFlags::Unique), ErrArg(dest, ErrArgFlags::Unique));
        }
    }

    return newError(tree, dest);
}

// performs an implicit conversion if its possible. otherwise returns null. flags is an optional parameter.
// Only call this if you are ALWAYS going to use the returned result (and you're not just going to test and
// possibly throw away the result)
// If the conversion is possible it will modify an Anonymous Method expr thus changing results of
// future conversions.  It will also produce possible binding errors for method goups.
EXPR * FUNCBREC::tryConvert(EXPR * expr, TYPESYM * dest, unsigned flags)
{
    EXPR * exprResult;

    if (bindImplicitConversion(expr->tree, expr, expr->type, dest, &exprResult, flags)) {
        // Conversion works.
        return exprResult;
    }

    return NULL;
}


/*
 * bindExplicitConversion
 *
 * This is a complex routine with complex parameter. Generally, this should
 * be called through one of the helper methods that insulates you
 * from the complexity of the interface. This routine handles all the logic
 * associated with explicit conversions.
 *
 * Note that this function calls bindImplicitConversion first, so the main
 * logic is only concerned with conversions that can be made explicitly, but
 * not implicitly.
 */
bool FUNCBREC::bindExplicitConversion(BASENODE * tree, EXPR * exprSrc, TYPESYM * typeSrc, TYPESYM * typeDest, EXPR * * pexprDest, unsigned flags)
{
    // To test for a standard conversion, call canConvert(exprSrc, typeDest, STANDARDANDNOUDC) and canConvert(typeDest, typeSrc, STANDARDANDNOUDC).
    ASSERT(!(flags & STANDARD));

    // All implicit conversions are explicit conversions. Don't try user-defined conversions now because we'll
    // try them again later.
    if (bindImplicitConversion(tree, exprSrc, typeSrc, typeDest, pexprDest, flags | ISEXPLICIT))
        return true;

    if (!typeSrc || !typeDest || typeSrc->isERRORSYM() || typeDest->isERRORSYM() || typeDest->IsNeverSameType())
        return false;

    if (typeDest->isNUBSYM()) {
        // This is handled completely by bindImplicitConversion.
        return false;
    }

    if (typeSrc->isNUBSYM()) {
        // If S and T are value types and there is a builtin conversion from S => T then there is an
        // explicit conversion from S? => T that throws on null.
        if (typeDest->IsValType() && bindExplicitConversion(tree, NULL, typeSrc->StripNubs(), typeDest, NULL, flags | NOUDC)) {
            if (pexprDest) {
                while (exprSrc->type->isNUBSYM())
                    exprSrc = BindNubValue(tree, exprSrc);
                ASSERT(exprSrc->type == typeSrc->StripNubs());
                if (!bindExplicitConversion(tree, exprSrc, exprSrc->type, typeDest, pexprDest, flags | NOUDC)) {
                    VSFAIL("bindExplicitConversion failed unexpectedly");
                    return false;
                }
            }
            return true;
        }

        return !(flags & NOUDC) && bindUserDefinedConversion(tree, exprSrc, typeSrc, typeDest, pexprDest, false);
    }

    AGGSYM * aggIList;

    if (typeSrc->isARRAYSYM() && typeSrc->asARRAYSYM()->rank == 1 &&
        typeDest->isInterfaceType() &&
        typeDest->asAGGTYPESYM()->typeArgsAll->size == 1 &&
        (aggIList = compiler()->GetOptPredefAgg(PT_G_ILIST)) != NULL &&
        compiler()->IsBaseAggregate(aggIList, typeDest->asAGGTYPESYM()->getAggregate()))
    {
        TYPESYM * typeArr = typeSrc->asARRAYSYM()->elementType();
        TYPESYM * typeLst = typeDest->asAGGTYPESYM()->typeArgsAll->Item(0);

        if (FExpRefConv(typeArr, typeLst))
            return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, EXF_REFCHECK);
    }

    // if we were casting an integral constant to another constant type,
    // then, if the constant were in range, then the above call would have succeeded.

    // But it failed, and so we know that the constant is not in range

    switch (typeDest->getKind()) {
    default:
        ASSERT(0);
        // Fall through.
    case SK_VOIDSYM:
        return false; // Can't convert to a method group or anon method.

    case SK_NULLSYM:
        return false;  // Can never convert TO the null type.

    case SK_TYVARSYM:
        // NOTE: for the flags, we have to use EXF_FORCE_UNBOX (not EXF_REFCHECK) even when
        // we know that the type is a reference type. The verifier expects all code for
        // type parameters to behave as if the type parameter is a value type.
        // The jitter should be smart about it....
        if (typeSrc->isInterfaceType() || canConvert(typeDest, typeSrc, NOUDC)) {
            // There is an explicit, possibly unboxing, conversion from Object or any interface to
            // a type variable. This will involve a type check and possibly an unbox.
            // There is an explicit conversion from non-interface X to the type var iff there is an
            // implicit conversion from the type var to X.
            if (typeSrc->isTYVARSYM()) {
                // Need to box first before unboxing.
                EXPR * exprT;
                bindSimpleCast(tree, exprSrc, GetReqPDT(PT_OBJECT), &exprT, EXF_FORCE_BOX);
                exprSrc = exprT;
            }
            return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, EXF_FORCE_UNBOX);
        }
        break;

    case SK_ARRAYSYM:
        /*
         * Handle conversions to arrays.
         * An array type can be explicitly converted to some other array types. Object, and Array
         * and their interface can be explicitly converted to any array type.
         */
        if (typeSrc->isARRAYSYM()) {
            // Handle array -> array conversions. Ranks must match (or cast from unknown rank),
            // and element type must be same or built-in explicit reference conversion.
            PARRAYSYM arraySrc = typeSrc->asARRAYSYM();
            PARRAYSYM arrayDest = typeDest->asARRAYSYM();
            PTYPESYM elementSrc = arraySrc->elementType();
            PTYPESYM elementDest = arrayDest->elementType();

            if (arraySrc->rank != arrayDest->rank)
                break;  // Ranks do not match.

            if (FExpRefConv(elementSrc, elementDest)) {
                // Element types are compatible.
                return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, EXF_REFCHECK);
            }

            break;
        }

        if (typeDest->asARRAYSYM()->rank == 1 &&
            typeSrc->isInterfaceType() &&
            typeSrc->asAGGTYPESYM()->typeArgsAll->size == 1 &&
            (aggIList = compiler()->GetOptPredefAgg(PT_G_ILIST)) != NULL &&
            compiler()->IsBaseAggregate(aggIList, typeSrc->asAGGTYPESYM()->getAggregate()))
        {
            TYPESYM * typeArr = typeDest->asARRAYSYM()->elementType();
            TYPESYM * typeLst = typeSrc->asAGGTYPESYM()->typeArgsAll->Item(0);

            ASSERT(!typeArr->IsNeverSameType());
            if (typeArr == typeLst || FExpRefConv(typeArr, typeLst))
                return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, EXF_REFCHECK);
        }

        if (canConvert(GetReqPDT(PT_ARRAY), typeSrc, NOUDC)) {
            // The above if checks for src==Array, object or an interface Array implements.
            return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, EXF_REFCHECK);
        }

        break;

    case SK_PTRSYM:
        /*
         * Handle pointer conversions.
         *
         * Any pointer can be explicitly converted to any other pointer.
         */
        if (typeSrc->isPTRSYM() || typeSrc->fundType() <= FT_LASTINTEGRAL && typeSrc->isNumericType())
            return bindSimpleCast(tree, exprSrc, typeDest, pexprDest);
        break;

    case SK_AGGTYPESYM: {
        AGGSYM * aggDest = typeDest->asAGGTYPESYM()->getAggregate();

        // TypeReference and ArgIterator can't be boxed (or converted to anything else)
        if (typeSrc->isSpecialByRefType())
            return false;

        if (typeSrc->isEnumType() && !aggDest->isPredefAgg(PT_DECIMAL)) {
            /*
             * Handle explicit conversion from enum.
             * enums can explicitly convert to any numeric type or any other enum.
             */
            if (aggDest->getThisType()->isNumericType() || aggDest->IsEnum() || (aggDest->isPredefined && aggDest->iPredef == PT_CHAR)) {
                if (exprSrc->GetConst()) {
                    bool checkFailure = true;
                    if (bindConstantCast(tree, exprSrc, typeDest, pexprDest, &checkFailure))
                        return true;
                    if (checkFailure) {
                        return false;
                    }
                }
                return bindSimpleCast(tree, exprSrc, typeDest, pexprDest);
            }
            break;
        }

        if (aggDest->IsEnum() && !typeSrc->isPredefType(PT_DECIMAL)) {
            /*
             * Handle conversions to enum.
             * Object or numeric types can explicitly convert to enums.
             * However, this is not considered on a par to a user-defined
             * conversion.
             */
            if (typeSrc->isNumericType() || (typeSrc->isPredefined() && typeSrc->getPredefType() == PT_CHAR)) {
                // Transform constant to constant.
                if (exprSrc->GetConst()) {
                    bool checkFailure = true;
                    if (bindConstantCast(tree, exprSrc, typeDest, pexprDest, &checkFailure))
                        return true;
                    if (checkFailure) {
                        return false;
                    }
                }
                return bindSimpleCast(tree, exprSrc, typeDest, pexprDest);
            }
            else if (typeSrc->isPredefined() &&
                (typeSrc->isPredefType(PT_OBJECT) || typeSrc->isPredefType(PT_VALUE) || typeSrc->isPredefType(PT_ENUM)))
                return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, EXF_UNBOX);
            break;
        }

        if ((typeSrc->isSimpleType() && typeDest->isSimpleType()) ||
            (typeSrc->isEnumType() && aggDest->isPredefAgg(PT_DECIMAL)) ||
            (typeSrc->isPredefType(PT_DECIMAL) && aggDest->IsEnum()))
        {

            bool wasEnum = false;
            AGGSYM * oldAggDest = aggDest;
            TYPESYM * oldTypeDest = typeDest;

            if (!typeSrc->isSimpleType()) {
                ASSERT(typeSrc->isEnumType());
                wasEnum = true;
                typeSrc = typeSrc->underlyingType();
                // Need to first cast the source expr to its underlying type.
                if (exprSrc) {
                    EXPR * exprT;
                    bindSimpleCast(tree, exprSrc, typeSrc, &exprT);
                    exprSrc = exprT;
                }
            } else if (!typeDest->isSimpleType()) {
                ASSERT(aggDest->IsEnum());
                wasEnum = true;
                typeDest = typeDest->underlyingType();
                aggDest = typeDest->getAggregate();
            }

            /*
             * Handle conversions between simple types. (int->long, float->double, etc...)
             */
            ASSERT(typeSrc->isPredefined() && aggDest->isPredefined);
            int ptSrc = typeSrc->getPredefType();
            int ptDest = aggDest->iPredef;

            ASSERT(ptSrc < NUM_SIMPLE_TYPES && ptDest < NUM_SIMPLE_TYPES);

            unsigned convertKind = simpleTypeConversions[ptSrc][ptDest];
            ASSERT((convertKind & CONV_KIND_MASK) != CONV_KIND_ID);   // identity conversion should have been handled at first.
            ASSERT((convertKind & CONV_KIND_MASK) != CONV_KIND_IMP || wasEnum);  // implicit conversion should have been handled earlier.

            if ((convertKind & CONV_KIND_MASK) == CONV_KIND_EXP || ((convertKind & CONV_KIND_MASK) == CONV_KIND_IMP && wasEnum)) {
                // An explicit conversion exists.

                if (exprSrc->GetConst()) {
                    // Fold the constant cast if possible.
                    bool checkFailure = true;
                    if (bindConstantCast(tree, exprSrc, oldTypeDest, pexprDest, &checkFailure))
                        return true;  // else, don't fold and use a regular cast, below.
                    if (checkFailure && !(flags & CHECKOVERFLOW)) {
                        return false;
                    }
                }

                if (convertKind & CONV_KIND_USEUDC) {
                    if (!pexprDest) return true;
                    // According the language, this is a standard conversion, but it is implemented
                    // through a user-defined conversion. Because it's a standard conversion, we don't
                    // test the NOUDC flag here.
                    bool ok = bindUserDefinedConversion(tree, exprSrc, typeSrc, typeDest, pexprDest, false);
                    // if we are converting Decimal to Enum, we just got a conversion from Decimal to
                    // underlying type, so we need to upcast to the Enum type
                    if (ok && oldAggDest->IsEnum()) {
                        return bindSimpleCast(tree, *pexprDest, oldTypeDest, pexprDest, false);
                    }
                    return ok;
                }
                else {
                    return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, (flags & CHECKOVERFLOW) ? EXF_CHECKOVERFLOW : 0);
                }
            }

            // No break here, continue testing for derived to base conversions below.
        }

        /*
         * Handle struct, class and interface conversions. A class, struct, or interface is explicity convertable
         * to a derived class or interface.
         */
        if (typeSrc->isAGGTYPESYM()) {
            AGGSYM * aggSrc = typeSrc->asAGGTYPESYM()->getAggregate();

            if (compiler()->IsBaseType(typeDest->asAGGTYPESYM(), typeSrc->asAGGTYPESYM())) {
                if (aggDest->IsStruct() && aggSrc->getThisType()->fundType() == FT_REF)
                    return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, EXF_UNBOX);
                else
                    return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, EXF_REFCHECK | (exprSrc ? (exprSrc->flags & EXF_CANTBENULL) : 0));
            }

            /* A non-sealed class can cast to any interface, any interface can cast to a non-sealed class,
             * and any interface can cast to any other interface.
             */
            if ((aggSrc->IsClass() && !aggSrc->isSealed && aggDest->IsInterface()) ||
                (aggSrc->IsInterface() && aggDest->IsClass() && !aggDest->isSealed) ||
                (aggSrc->IsInterface() && aggDest->IsInterface()))
            {
                return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, EXF_REFCHECK | (exprSrc ? (exprSrc->flags & EXF_CANTBENULL) : 0));
            }
        }

        if (typeSrc->isPTRSYM() && typeDest->fundType() <= FT_LASTINTEGRAL && typeDest->isNumericType())
            return bindSimpleCast(tree, exprSrc, typeDest, pexprDest);

        if (typeSrc->isVOIDSYM()) {
            // No conversion is allowed to or from a void type (user defined or otherwise)
            // This is most likely the result of a failed anonymous method or member group conversion
            return false;
        }

        if (typeSrc->isTYVARSYM()
            && aggDest->IsInterface()
            ) {
            // Explicit conversion of type variables to interfaces.
            return bindSimpleCast(tree, exprSrc, typeDest, pexprDest, EXF_FORCE_BOX | EXF_REFCHECK);
        }
        break;
    }}

    // No built-in conversion was found. Maybe a user-defined conversion?
    if (!(flags & NOUDC)) {
        return bindUserDefinedConversion(tree, exprSrc, typeSrc, typeDest, pexprDest, false);
    }

    return false;
}

// returns true if an explicit conversion exists from source type to dest type. flags is an optional parameter.
bool FUNCBREC::canCast(TYPESYM * src, TYPESYM * dest, unsigned flags)
{
    return bindExplicitConversion(NULL, NULL, src, dest, NULL, flags);
}

// returns true if a explicit conversion exists from source expr to dest type. flags is an optional parameter.
bool FUNCBREC::canCast(EXPR * expr, TYPESYM * dest, unsigned flags)
{
    return bindExplicitConversion(NULL, expr, expr->type, dest, NULL, flags);
}

// performs an explicit conversion if its possible. otherwise displays an error. flags is an optional parameter.
EXPR * FUNCBREC::mustCastCore(EXPR * expr, TYPESYM * dest, BASENODE * tree, unsigned flags)
{
    EXPR * exprResult;

    compiler()->CheckForStaticClass(tree, NULL, dest, ERR_ConvertToStaticClass);

    if (bindExplicitConversion(tree, expr, expr->type, dest, &exprResult, flags)) {
        // Conversion works.
        return exprResult;
    }

    if (expr->isOK() && dest && !dest->isERRORSYM())  {// don't report cascading error.

        // For certain situations, try to give a better error.

        EXPR * exprConst = expr->GetConst();
        if (exprConst && checked.constant &&
            expr->type->isSimpleType() && (dest->isSimpleType() || dest->isEnumType()))
        {
            // check if we failed because we are in checked mode...
            CHECKEDCONTEXT checked(this, false);
            bool okNow = bindExplicitConversion(tree, expr, expr->type, dest, NULL, flags | NOUDC);
            checked.restore(this);

            if (!okNow) goto CANTCONVERT;

            // Failed because value was out of range. Report nifty error message.
            HRESULT hr = E_FAIL;
            WCHAR value[256];
            if (expr->type->fundType() <= FT_LASTINTEGRAL) {
                if (expr->type->isUnsigned())
                    hr = StringCchPrintfW (value, lengthof(value), L"%I64u", exprConst->asCONSTANT()->getI64Value());
                else
                    hr = StringCchPrintfW (value, lengthof(value), L"%I64d", exprConst->asCONSTANT()->getI64Value());
            } else {
                hr = StringCchPrintfW (value, lengthof(value), L"%g", *(exprConst->asCONSTANT()->getVal().doubleVal));
            }
            ASSERT (SUCCEEDED (hr));

            compiler()->Error(tree, ERR_ConstOutOfRangeChecked, value, dest);
        }
        else if (expr->type->isNULLSYM() && dest->fundType() != FT_REF) {
            compiler()->Error(tree, ERR_ValueCantBeNull, dest);
        }
        else if (expr->kind == EK_MEMGRP) {
            BindGrpConversion(tree, expr->asMEMGRP(), dest, NULL, true);
        }
        else if (expr->type->isANONMETHSYM()) {
            bindAnonMethConversion(tree, expr, dest, NULL, true);
        }
        else {
CANTCONVERT:
            // Generic "can't convert" error.
            compiler()->Error(tree, ERR_NoExplicitConv,
                ErrArg(expr->type, ErrArgFlags::Unique), ErrArg(dest, ErrArgFlags::Unique));
        }
    }

    return newError(tree, dest);
}

// performs an explicit conversion if its possible. otherwise returns null. flags is an optional parameter.
// Only call this if you are ALWAYS going to use the returned result (and you're not just going to test and
// possibly throw away the result)
// If the conversion is possible it will modify an Anonymous Method expr thus changing results of
// future conversions.  It will also produce possible binding errors for method goups.
EXPR * FUNCBREC::tryCast(EXPR * expr, TYPESYM * dest, BASENODE * tree, unsigned flags)
{
    EXPR * exprResult;

    if (bindExplicitConversion(tree, expr, expr->type, dest, &exprResult, flags)) {
        // Conversion works.
        return exprResult;
    }

    return NULL;
}


// Bind the dot operator.  mask indicates allowed return values
EXPR * FUNCBREC::bindDot(BINOPNODE * tree, symbmask_t mask, int bindFlags)
{
    // The only valid masks are:
    // MASK_PROPSYM where we want a property
    // MASK_METHSYM where we want a method name, and we return an EXPRGRP
    // MASK_AGGTYPESYM | MASK_NUBSYM where we want a class name, and we return an EXPRCLASS
    // MASK_NSSSYM where we want a namespace, and we return an EXPRNSPACE
    // MASK_MEMBVARSYM where we want a field


    ASSERT(mask);
    ASSERT(!(mask & ~(MASK_EVENTSYM | MASK_METHSYM | MASK_AGGTYPESYM | MASK_NUBSYM | MASK_NSAIDSYM | MASK_MEMBVARSYM | MASK_PROPSYM)));
    ASSERT(!(mask & MASK_AGGTYPESYM) == !(mask & MASK_NUBSYM));
    ASSERT(tree->p1 && tree->p2);

    BASENODE * nodeLeft = tree->p1;
    NAMENODE * nodeName = tree->p2->asANYNAME();
    bool fBase = false;

    EXPR * exprLeft;

    const symbmask_t maskLeft = MASK_AGGTYPESYM | MASK_NUBSYM | MASK_NSAIDSYM | MASK_MEMBVARSYM | MASK_PROPSYM;
    if (tree->kind == NK_ARROW) {
        exprLeft = bindExpr(nodeLeft);
        if (!exprLeft->isOK())
            goto LHaveLeft;
        if (!exprLeft->type->isPTRSYM()) {
            compiler()->Error(tree, ERR_PtrExpected);
            // Treat the -> like a .
            goto LHaveLeft;
        }
        exprLeft = newExprBinop(exprLeft->tree, EK_INDIR, exprLeft->type->asPTRSYM()->baseType(), exprLeft, NULL);
        exprLeft->flags |= EXF_ASSGOP | EXF_LVALUE;
        goto LHaveLeft;
    }

    switch (nodeLeft->kind) {
    case NK_DOT:
        // the left side is also a dot...
        exprLeft = SetNodeExpr(nodeLeft, bindDot(nodeLeft->asDOT(), maskLeft, BIND_RVALUEREQUIRED | bindFlags));
        break;
    case NK_NAME:
    case NK_GENERICNAME:
        exprLeft = SetNodeExpr(nodeLeft, bindName(nodeLeft->asANYNAME(), maskLeft | MASK_LOCVARSYM, BIND_RVALUEREQUIRED | bindFlags));
        break;
    case NK_ALIASNAME:
        exprLeft = SetNodeExpr(nodeLeft, bindAliasName(nodeLeft->asALIASNAME()));
        break;
    case NK_OP:
        if (nodeLeft->Op() == OP_BASE) {
            exprLeft = SetNodeExpr(nodeLeft, bindBase(nodeLeft));
            fBase = true;
            break;
        }
        // Fall through.
    default:
        exprLeft = bindExpr(nodeLeft);
        if (nodeLeft->kind == NK_UNOP && nodeLeft->Op() == OP_DEFAULT &&
            (exprLeft->type->IsRefType() || exprLeft->type->isPTRSYM()))
        {
            compiler()->Error( tree, WRN_DotOnDefault, exprLeft->type);
        }
        break;
    }

LHaveLeft:
    ASSERT(exprLeft);
    // bind type parameters
    TypeArray * typeArgs = TypeBind::BindTypeArgs(compiler(), nodeName, contextForTypeBinding(), btfFlags);
    ASSERT(typeArgs);

    NAME * name = nodeName->pName;
    TYPESYM * typeLeft = NULL;
    TypeArray * typesGroup = BSYMMGR::EmptyTypeArray();
    SymWithType swtRight;

    if (exprLeft->kind == EK_NSPACE) {
        NSAIDSYM * nsaLeft = exprLeft->asNSPACE()->nsa;

        SYM * symRight = TypeBind::SearchSingleNamespace(compiler(), nodeName, name, typeArgs, nsaLeft, parentDecl);
        ASSERT(symRight);
        if (symRight->isERRORSYM())
            return newError(tree, symRight->asERRORSYM());

        // SearchSingleNamespace should have ensured the arity is correct.
        ASSERT(symRight->isNSAIDSYM() && typeArgs->size == 0 ||
            symRight->isAGGTYPESYM() && symRight->asAGGTYPESYM()->getAggregate()->typeVarsThis->size == typeArgs->size ||
            symRight->isNUBSYM() && typeArgs->size == 1);
        swtRight.Set(symRight, NULL);
    }
    else {
        typeLeft = exprLeft->type;
        if (!typeLeft || typeLeft->isERRORSYM())
            return newError(tree, NULL);

        compiler()->EnsureState(typeLeft);

        if (typeLeft->isARRAYSYM()) {
            // If typeLeft->isARRAYSYM() is true then fBase must be false.
            ASSERT(!fBase);

            // Length property on rank 1 arrays is converted into the ldlen instruction.
            if (name == compiler()->namemgr->GetPredefName(PN_LENGTH) && typeLeft->asARRAYSYM()->rank == 1 && exprLeft->kind != EK_CLASS) {
                if (typeArgs->size)
                    ReportTypeArgsNotAllowedError(nodeName, typeArgs->size, name, SK_PROPSYM);
                EXPR * rval = newExprBinop(tree, EK_ARRLEN, GetReqPDT(PT_INT), exprLeft, NULL);
                rval->flags |= EXF_ASSGOP;
                return rval;
            }

            // All other members come from System.Array.
            typeLeft = GetReqPDT(PT_ARRAY);
            compiler()->EnsureState(typeLeft);
        }
        else if (typeLeft->isNUBSYM()) {
            ASSERT(!fBase);

            TYPESYM * atsLeft = typeLeft->asNUBSYM()->GetAts();
            ASSERT(atsLeft);
            if (atsLeft) {
                typeLeft = atsLeft;
                compiler()->EnsureState(typeLeft);
            }
        }

        if (!typeLeft->isAGGTYPESYM() && !typeLeft->isTYVARSYM()) {
            if (!typeLeft->isERRORSYM())
                compiler()->Error(tree, ERR_BadUnaryOp, L".", typeLeft);
            return newError(tree, NULL);
        }

        // Look for members.
        MemberLookup mem;

        if (!mem.Lookup(compiler(), typeLeft, exprLeft, parentDecl, name, typeArgs->size,
            fBase ? (MemLookFlags::BaseCall | MemLookFlags::UserCallable) : MemLookFlags::UserCallable))
        {
            mem.ReportErrors(nodeName);
            return newError(tree, NULL);
        }
        mem.ReportWarnings(nodeName);

        if (mem.SymFirst()->isAGGSYM()) {
            swtRight.Set(mem.SymFirstAsAts(typeArgs), NULL);
            TypeBind::CheckConstraints(compiler(), tree, swtRight.Sym()->asAGGTYPESYM(), CheckConstraintsFlags::None);

            // Can only reference type names through their containing type.
            if (exprLeft->kind != EK_CLASS && exprLeft->isOK()) {
                // This could be the weird E.M case where E can be either the class name or a prop / field / variable.
                if (!(exprLeft->flags & EXF_SAMENAMETYPE)) {
                    compiler()->Error(nodeName, ERR_BadTypeReference, nodeName, swtRight);
                    return newError(nodeName, swtRight.Sym()->asAGGTYPESYM());
                }
            }
        }
        else {
            swtRight = mem.SwtFirst();

            if (mem.SymFirst()->isMETHSYM()) {
                typesGroup = mem.GetAllTypes();
            }
            else {
                ASSERT(typeArgs->size == 0);
                ASSERT(!mem.FMultiKind() && mem.TypeCount() == 1);
            }
        }
    }

    ASSERT(swtRight && !swtRight.Sym()->isAGGSYM()); // should never be an AGGSYM (AGGTYPESYM instead).
    if (!(swtRight.Sym()->mask() & mask)) {
        ErrorBadSK(nodeName, swtRight, mask, bindFlags);
        return newError(nodeName, NULL);
    }

    EXPR * rval;
    EXPR * object = (exprLeft->kind == EK_CLASS) ? NULL : exprLeft;

    switch (swtRight.Sym()->getKind()) {
    case SK_MEMBVARSYM:
        ASSERT(exprLeft->kind != EK_NSPACE);
        rval = BindToField(tree, object, swtRight, bindFlags);
        break;

    case SK_PROPSYM:
        ASSERT(exprLeft->kind != EK_NSPACE);
        rval = BindToProperty(tree, object, swtRight, fBase ? (bindFlags | BIND_BASECALL) : bindFlags);
        break;

    case SK_EVENTSYM:
        ASSERT(exprLeft->kind != EK_NSPACE);
        rval = BindToEvent(tree, object, swtRight, fBase ? (bindFlags | BIND_BASECALL) : bindFlags);
        break;

    case SK_NSAIDSYM:
        rval = newExpr(tree, EK_NSPACE, NULL);
        rval->asNSPACE()->nsa = swtRight.Sym()->asNSAIDSYM();
        break;

    case SK_NUBSYM:
    case SK_AGGTYPESYM:
        rval = newExpr(tree, EK_CLASS, swtRight.Sym()->asTYPESYM());
        compiler()->EnsureState(swtRight.Sym()->asTYPESYM());
        if (swtRight.Sym()->IsDeprecated())
            ReportDeprecated(tree, swtRight);
        break;

    case SK_METHSYM:
        rval = newExpr(tree, EK_MEMGRP, compiler()->getBSymmgr().GetMethGrpType());
        rval->asMEMGRP()->nodeName = nodeName;
        rval->asMEMGRP()->name = name;
        rval->asMEMGRP()->sk = SK_METHSYM;
        rval->asMEMGRP()->typeArgs = typeArgs;
        rval->asMEMGRP()->typePar = typeLeft;
        rval->asMEMGRP()->mps = NULL;
        rval->asMEMGRP()->object = object;
        rval->asMEMGRP()->types = typesGroup;
        rval->flags |= EXF_USERCALLABLE;

        if (fBase) {
            rval->flags |= EXF_BASECALL;
        }
        break;

    default:
        ASSERT(!"bad kind");
        rval = NULL;
    }

    if (rval && rval->type) {
        checkUnsafe(tree, rval->type);
    }

    return rval;
}


EXPR * FUNCBREC::bindAliasName(NAMENODE * tree)
{
    ASSERT(tree->kind == NK_ALIASNAME);

    SYM * sym = TypeBind::BindName(compiler(), tree, contextForTypeBinding(), btfFlags);
    ASSERT(sym);
    if (sym->isERRORSYM())
        return newError(tree, sym->asERRORSYM());

    EXPR * expr;

    switch (sym->getKind()) {
    case SK_NSAIDSYM:
        expr = newExpr(tree, EK_NSPACE, getVoidType());
        expr->asNSPACE()->nsa = sym->asNSAIDSYM();
        break;

    case SK_AGGTYPESYM:
        expr = newExpr(tree, EK_CLASS, sym->asAGGTYPESYM());
        break;

    default:
        VSFAIL("bad kind");
        return newError(tree, NULL);
    }

    ASSERT(expr);
    return expr;
}


EXPR * FUNCBREC::bindMethodName(BASENODE * tree)
{
    EXPR * expr;
    BASENODE * nodeName;

    if (tree->IsAnyName()) {
        expr = SetNodeExpr(tree, bindName(tree->asANYNAME(), MASK_METHSYM | MASK_MEMBVARSYM | MASK_LOCVARSYM | MASK_PROPSYM));
        nodeName = tree->asANYNAME();
    } else if (tree->kind == NK_DOT || tree->kind == NK_ARROW) {
        expr = SetNodeExpr(tree, bindDot(tree->asANYBINOP(), MASK_METHSYM | MASK_MEMBVARSYM | MASK_PROPSYM));
        nodeName = tree->asANYBINOP()->p2->asANYNAME();
    } else {
        expr = bindExpr(tree);
        nodeName = tree;
    }

    if (!expr->isOK())
        return expr;

    if (expr->kind == EK_MEMGRP && expr->asMEMGRP()->sk == SK_METHSYM)
        return expr->asMEMGRP();

    if (expr->type && expr->type->isDelegateType()) {
        // Note: don't call SetNodeExpr on the MEMGRP. We want the delegate expression to be bound
        // to the name node, not the MEMGRP.
        TYPESYM * type = expr->type;

        if (tree->kind == NK_UNOP && tree->Op() == OP_DEFAULT)
        {
            compiler()->Error( tree->pParent, WRN_DotOnDefault, type);
        }

        // Construct the method group.
        EXPRMEMGRP * grp = newExpr(tree, EK_MEMGRP, compiler()->getBSymmgr().GetMethGrpType())->asMEMGRP();
        grp->nodeName = nodeName;
        grp->name = compiler()->namemgr->GetPredefName(PN_INVOKE);
        grp->typeArgs = BSYMMGR::EmptyTypeArray();
        grp->sk = SK_METHSYM;
        grp->typePar = type->asAGGTYPESYM();
        grp->mps = compiler()->getBSymmgr().LookupInvokeMeth(type->asAGGTYPESYM()->getAggregate());
        grp->object = expr;
        grp->types = NULL;
        grp->flags |= EXF_DELEGATE;

        return grp;
    }

    // Not a delegate, so to try to get a better error message, rebind, allowing only a method.
    if (tree->IsAnyName()) {
        expr = bindName(tree->asANYNAME(), MASK_METHSYM);
    }
    else if (tree->kind == NK_DOT) {
        expr = bindDot(tree->asDOT(), MASK_METHSYM);
    }
    else {
        // No need to do this again!
        // expr = bindExpr(tree);
    }
    if (!expr->isOK())
        return expr;

    // Some probably that rebinding didn't catch, so use the generic error message.
    compiler()->Error(tree, ERR_MethodNameExpected);
    return newError(tree, NULL);
}


// This finds and binds a method with the given name to the args provided.
EXPR * FUNCBREC::BindPredefMethToArgs(BASENODE * tree, PREDEFNAME id, AGGTYPESYM * type, EXPR * obj, EXPR * args, TypeArray * typeArgs)
{
    NAME * name = compiler()->namemgr->GetPredefName(id);
    MemberLookup mem;

    // Caused by missing type in GetOptPDT()
    if (type == NULL)
        return newError(tree, NULL);

    if (!mem.Lookup(compiler(), type, obj, parentDecl, name, TypeArray::Size(typeArgs), MemLookFlags::None) || !mem.SymFirst()->isMETHSYM()) {
        compiler()->Error(tree, ERR_MissingPredefinedMember, type, name);
        return newError(tree, NULL);
    }
    mem.ReportWarnings(tree);

    EXPRMEMGRP * grp = newExprMemGrp(tree, NULL, mem);
    if (typeArgs)
        grp->typeArgs = typeArgs;

    return BindGrpToArgs(tree, 0, grp, args);
}


// paramsMeth may be either meth->params or an expanded version of meth->params (in the case when meth->isParamArray is true).
// argsMatch are the arguments being supplied to the call. This uses the argument types to infer type variable values.
bool FUNCBREC::InferTypeArgs(METHSYM * meth, AGGTYPESYM * type, TypeArray * paramsMeth, ArgInfos * argsMatch, TypeArray ** ptypeArgs)
{
    ASSERT(meth->typeVars->size > 0);
    ASSERT(meth->isParamArray || meth->params == paramsMeth);

    if (paramsMeth->size == 0 || meth->cisCanInfer == METHSYM::cisNo) {
        return false;
    }

    ASSERT(argsMatch);
    ASSERT(argsMatch->carg == paramsMeth->size);

    int ctype = meth->typeVars->size;
    BSYMMGR::InferContext ctx;

    // Fill in the Infer Context with an empty mapping.
    ctx.typeVarsMeth = meth->typeVars;
    ctx.prgtypeMeth = STACK_ALLOC_ZERO(TYPESYM *, ctype);
    ctx.typeArgsCls = type->typeArgsAll;

    compiler()->EnsureState(paramsMeth);

    for (int i = 0; i < paramsMeth->size; i++) {

        // InferTypes requires the inheritance to be resolved.
        compiler()->EnsureState(argsMatch->types->Item(i));

        if (!compiler()->getBSymmgr().InferTypes(argsMatch->types->Item(i), paramsMeth->Item(i), &ctx))
            return false;
    }

    for (int i = 0; i < ctype; i++) {
        if (!ctx.prgtypeMeth[i]) {
            // The inference succeeded but not all type variables were mapped.
            // This can happen if not all type variables appear in the signature or if
            // some of the args are null. Generally meth->cisCanInfer should be set to cisNo or cisMaybe
            // in the former case, but this can still happen if this is a param method and
            // we're in the zero extra args case.
            if (meth->cisCanInfer == METHSYM::cisMaybe)
                BSYMMGR::SetCanInferState(meth);
            return false;
        }
    }

    *ptypeArgs = compiler()->getBSymmgr().AllocParams(ctype, ctx.prgtypeMeth);
    meth->cisCanInfer = METHSYM::cisYes;
    return true;
}


// Given a method group or indexer group, bind it to the arguments for an invocation.
bool FUNCBREC::BindGrpToArgsCore(MethPropWithInst * pmpwi, BASENODE * tree, int bindFlags, EXPRMEMGRP * grp,
    ArgInfos * args, AGGTYPESYM * atsDelegate, bool fReportErrors, MethPropWithInst * pmpwiAmbig)
{
    ASSERT(pmpwi);
    ASSERT(fReportErrors || pmpwiAmbig);
    ASSERT(grp->sk == SK_METHSYM || grp->sk == SK_PROPSYM && (grp->flags & EXF_INDEXER));

    // We need the EXPRs for error reporting for non-delegates
    ASSERT(atsDelegate != NULL || args->fHasExprs);

    NAME * name = grp->name;
    TypeArray * typeArgs = grp->typeArgs;
    symbmask_t mask = (symbmask_t)1 << grp->sk;

    // Only methods should have type args.
    ASSERT(mask == MASK_METHSYM || typeArgs->size == 0);
    pmpwi->Clear();
    if (pmpwiAmbig)
        pmpwiAmbig->Clear();

    TYPESYM * typeQual;

    if (grp->flags & EXF_BASECALL)
        typeQual = NULL;
    else if (grp->flags & EXF_CTOR)
        typeQual = grp->typePar;
    else if (grp->object)
        typeQual = grp->object->type;
    else
        typeQual = NULL;

    TypeArray * types = grp->types;
    if (!types)
        types = BSYMMGR::EmptyTypeArray();
    ASSERT(types->size > 0 || grp->typePar && !grp->typePar->isInterfaceType());

    AGGTYPESYM * typeCur;
    int itypeCur = 0;

    if (types->size > 0) {
        typeCur = types->Item(itypeCur)->asAGGTYPESYM();
        // Clear all the hidden flags.
        for (int i = 0; i < types->size; i++) {
            types->Item(i)->asAGGTYPESYM()->fAllHidden = false;
        }
    }
    else {
        // If the full set of types isn't given, we better be in a class.
        // For the interface and type variable case, MemberLookup.Lookup should have been called.
        ASSERT(grp->typePar->isAGGTYPESYM() && !grp->typePar->isInterfaceType());
        typeCur = grp->typePar->asAGGTYPESYM();
    }

    ASSERT(!grp->mps || grp->mps->getClass() == typeCur->getAggregate());

    METHLIST * pmethFirst = NULL;  // List of method matches
    METHLIST ** ppmethAdd = &pmethFirst;

    int cargBest = -1;
    MethPropWithInst mpwiBest(NULL, NULL, NULL);
    MethPropWithInst mpwiInaccess(NULL, NULL, NULL);
    MethPropWithInst mpwiBogus(NULL, NULL, NULL);
    MethPropWithInst mpwiParamTypeConstraints(NULL, NULL, NULL);
    MethWithType mwtCantInfer(NULL, NULL);
    MethWithType mwtBadArity(NULL, NULL);
    TypeArray * paramsBest = NULL;
    METHPROPSYM * mpsCur = NULL;
    TypeArray * paramsCur = NULL;
    bool fExpanded = false;

    SymWithType rgswtWrongCount[20];
    const int cswtMaxWrongCount = lengthof(rgswtWrongCount);
    int cswtWrongCount = 0;

    // Look for candidates.
    for (;;) {
LNextSym:
        // Deal with params.
        if (!(bindFlags & BIND_NOPARAMS) && mpsCur && mpsCur->isParamArray && !fExpanded && args->carg >= paramsCur->size - 1) {
            ASSERT(!pmethFirst || pmethFirst->mpwi.MethProp() != mpsCur);
            // Construct the expanded params.
            paramsCur = GetExpandedParams(mpsCur->params, args->carg);
            fExpanded = true;
            ASSERT(paramsCur != mpsCur->params);
            goto LHaveSym;
        }

LNextSymNoParams:
        fExpanded = false;

        // Find the next sym.
        for (;;) {
            mpsCur = (mpsCur == NULL) ?
                compiler()->getBSymmgr().LookupAggMember(name, typeCur->getAggregate(), mask)->asMETHPROPSYM() :
                compiler()->getBSymmgr().LookupNextSym(mpsCur, typeCur->getAggregate(), mask)->asMETHPROPSYM();

            if (!mpsCur) {
                // Find the next type.
                // If we've found some applicable methods in a class then we don't need to look any further.
                if (pmethFirst && !pmethFirst->mpwi.Type()->isInterfaceType())
                    goto LListDone;
                if (types->size > 0) {
                    for (;;) {
                        if (++itypeCur >= types->size)
                            goto LListDone;
                        typeCur = types->Item(itypeCur)->asAGGTYPESYM();
                        if (!typeCur->fAllHidden)
                            break;
                    }
                }
                else {
                    ASSERT(!pmethFirst);
                    typeCur = typeCur->GetBaseClass();
                    if (!typeCur)
                        goto LListDone;
                }
                continue;
            }

            if (grp->mps && mpsCur != grp->mps)
                continue;

            // Make sure that whether we're seeing a ctor is consistent with the flag.
            // The only properties we handle are indexers.
            if (mask == MASK_METHSYM && (
                    !(grp->flags & EXF_CTOR) != !mpsCur->asMETHSYM()->isCtor() ||
                    !(grp->flags & EXF_OPERATOR) != !mpsCur->asMETHSYM()->isOperator) ||
                mask == MASK_PROPSYM && !mpsCur->asPROPSYM()->isIndexer())
            {
                // Get the next symbol.
                continue;
            }

            paramsCur = mpsCur->params;

            if (isMethPropCallable(mpsCur, (grp->flags & EXF_USERCALLABLE) != 0))
                break;
        }

        if (paramsCur->size != args->carg) {
            if (cswtWrongCount < cswtMaxWrongCount && (!mpsCur->isParamArray || args->carg < paramsCur->size - 1))
                rgswtWrongCount[cswtWrongCount++].Set(mpsCur, typeCur);
            continue;
        }

LHaveSym:
        TypeArray * typeArgsCur;

        // Get the type args.
        if (mpsCur->isMETHSYM() && mpsCur->asMETHSYM()->typeVars->size != typeArgs->size) {
            // Can't infer if some type args are specified.
            if (typeArgs->size > 0) {
                if (!mwtBadArity)
                    mwtBadArity.Set(mpsCur->asMETHSYM(), typeCur);
                goto LNextSymNoParams;
            }
            ASSERT(mpsCur->asMETHSYM()->typeVars->size > 0);

            // Try to infer.
            if (!InferTypeArgs(mpsCur->asMETHSYM(), typeCur, paramsCur, args, &typeArgsCur)) {
                if (!mwtCantInfer)
                    mwtCantInfer.Set(mpsCur->asMETHSYM(), typeCur);
                continue;
            }
        }
        else
            typeArgsCur = typeArgs;

        bool fCanAccess = compiler()->clsDeclRec.CheckAccess(mpsCur, typeCur, parentDecl, typeQual);
        if (!fCanAccess && (pmethFirst || mpwiInaccess)) {
            // We'll never use this one for error reporting anyway, so just skip it.
            goto LNextSymNoParams;
        }

        bool fBogus = fCanAccess && compiler()->CheckBogus(mpsCur);
        if (fBogus && (pmethFirst || mpwiInaccess || mpwiBogus)) {
            // We'll never use this one for error reporting anyway, so just skip it.
            goto LNextSymNoParams;
        }

        if (args->carg) {
            paramsCur = compiler()->getBSymmgr().SubstTypeArray(paramsCur, typeCur, typeArgsCur);
            for (int ivar = 0; ivar < args->carg; ivar++) {
                TYPESYM * var = paramsCur->Item(ivar);
                if (!TypeBind::CheckConstraints(compiler(), tree, var, CheckConstraintsFlags::NoErrors)) {
                    mpwiParamTypeConstraints.Set(mpsCur, typeCur, typeArgsCur);
                    goto LNextSym;
                }
            }                
            
            for (int ivar = 0; ivar < args->carg; ivar++) {
                TYPESYM * var = paramsCur->Item(ivar);

                if (args->fHasExprs ? !canConvert(args->prgexpr[ivar], var) : !canConvert(args->types->Item(ivar), var)) {
                    if (ivar > cargBest) {
                        cargBest = ivar;
                        mpwiBest.Set(mpsCur, typeCur, typeArgsCur);
                        paramsBest = paramsCur;
                    } else if (ivar == cargBest && args->types->Item(ivar) != var) { // this is to eliminate the paranoid case of types that are equal but can't convert (think ERRORSYM != ERRORSYM)
                        // See if they just differ in out / ref.
                        TYPESYM * argStripped = args->types->Item(ivar)->isPARAMMODSYM() ?
                            args->types->Item(ivar)->asPARAMMODSYM()->paramType() : args->types->Item(ivar);
                        TYPESYM * varStripped = var->isPARAMMODSYM() ? var->asPARAMMODSYM()->paramType() : var;
                        if (argStripped == varStripped) {
                            mpwiBest.Set(mpsCur, typeCur, typeArgsCur);
                            paramsBest = paramsCur;
                        }
                    }
                    goto LNextSym;
                }
            }
        }

        // We know we have the right number of arguments and they are all convertable.
        if (!fCanAccess) {
            // In case we never get an accessible method, this will allow us to give
            // a better error...
            ASSERT(!mpwiInaccess);
            mpwiInaccess.Set(mpsCur, typeCur, typeArgsCur);
        }
        else if (fBogus) {
            // In case we never get a good method, this will allow us to give
            // a better error...
            ASSERT(!mpwiBogus);
            mpwiBogus.Set(mpsCur, typeCur, typeArgsCur);
        }
        else {
            // This is a plausible method / property to call.
            METHLIST * pmeth = STACK_ALLOC(METHLIST, 1);

            // Link it in at the end of the list.
            ASSERT(!*ppmethAdd);
            *ppmethAdd = pmeth;
            pmeth->next = NULL;
            ppmethAdd = &pmeth->next;

            pmeth->mpwi.Set(mpsCur, typeCur, typeArgsCur);
            pmeth->ctypeLift = 0;
            pmeth->params = paramsCur;
            pmeth->fExpanded = fExpanded;

            if (typeCur->isInterfaceType()) {
                // Mark base interfaces.
                TypeArray * ifaces = typeCur->GetIfacesAll();
                for (int i = 0; i < ifaces->size; i++) {
                    AGGTYPESYM * type = ifaces->Item(i)->asAGGTYPESYM();
                    ASSERT(type->isInterfaceType());
                    type->fAllHidden = true;
                }

                // Mark object.
                AGGTYPESYM * typeObject = GetReqPDT(PT_OBJECT);
                typeObject->fAllHidden = true;
            }
        }

        // Don't look at the expanded form.
        goto LNextSymNoParams;
    }

LListDone:
    // We looked at all the evidence, and we come to render the verdict:
    METHLIST * pmethBest;

    if (!pmethFirst)
        goto LError;

   if (!pmethFirst->next) {
        // We found the single best method to call.
        pmethBest = pmethFirst;
    }
    else {
        METHLIST * pmethAmbig1;
        METHLIST * pmethAmbig2;

        pmethBest = FindBestMethod(pmethFirst, args, &pmethAmbig1, &pmethAmbig2);

        if (!pmethBest) {
            // Arbitrarily use the first one, but make sure to report errors or give the ambiguous one
            // back to the caller.
            pmethBest = pmethAmbig1;
            if (pmpwiAmbig)
                *pmpwiAmbig = pmethAmbig2->mpwi;

            if (fReportErrors) {
                if (pmethAmbig1->params != pmethAmbig2->params ||
                    pmethAmbig1->mpwi.MethProp()->params->size != pmethAmbig2->mpwi.MethProp()->params->size ||
                    pmethAmbig1->mpwi.TypeArgs() != pmethAmbig2->mpwi.TypeArgs() ||
                    pmethAmbig1->mpwi.Type() != pmethAmbig2->mpwi.Type() ||
                    pmethAmbig1->mpwi.MethProp()->params == pmethAmbig2->mpwi.MethProp()->params)
                {
                    compiler()->Error(tree, ERR_AmbigCall, pmethAmbig1->mpwi, pmethAmbig2->mpwi);
                }
                else {
                    // The two signatures are identical so don't use the type args in the error message.
                    compiler()->Error(tree, ERR_AmbigCall, pmethAmbig1->mpwi.MethProp(), pmethAmbig2->mpwi.MethProp());
                }
            }
        }
    }

    // This is the "success" exit path.
    ASSERT(pmethBest);
    mpwiBest = pmethBest->mpwi;

    *pmpwi = mpwiBest;
    if (!fReportErrors)
        return true;

    // used for Methods and Indexers
    ASSERT(grp->sk == SK_METHSYM || grp->sk == SK_PROPSYM && (grp->flags & EXF_INDEXER));
    ASSERT(grp->typeArgs->size == 0 || grp->sk == SK_METHSYM);

    // if this is a binding to finalize on object, then complain:
    if (mpwiBest.MethProp()->name == compiler()->namemgr->GetPredefName(PN_DTOR) &&
        mpwiBest.MethProp()->getClass()->isPredefAgg(PT_OBJECT))
    {
        if (grp->flags & EXF_BASECALL) {
            compiler()->Error(tree, ERR_CallingBaseFinalizeDeprecated);
        }
        else {
            compiler()->Error(tree, ERR_CallingFinalizeDepracated);
        }
    }

    ASSERT(!(grp->flags & EXF_USERCALLABLE) || mpwiBest.MethProp()->isUserCallable());

    if (grp->sk == SK_METHSYM) {
        ASSERT(mpwiBest.MethProp()->isMETHSYM());

        if (mpwiBest.TypeArgs()->size > 0) {
            // Check method type variable constraints.
            TypeBind::CheckMethConstraints(compiler(), tree, mpwiBest);
        }
    }

    return true;

LError:
    if (!fReportErrors)
        return false;

    ASSERT(!pmethFirst);
    if (mpwiInaccess) {
        // We might have called this, but it is inaccesable...
        compiler()->clsDeclRec.ReportAccessError(tree, mpwiInaccess, parentDecl, typeQual);
        return false;
    }
    if (mpwiBogus) {
        // We might have called this, but it is bogus...
        compiler()->ErrorRef(tree, ERR_BindToBogus, mpwiBogus);
        return false;
    }

    bool fUseDelegateErrors = false;
    NAME * nameErr = name;

    if (grp->object && grp->object->type && grp->object->type->isDelegateType() && grp->name == compiler()->namemgr->GetPredefName(PN_INVOKE)) {
        ASSERT(!mpwiBest || mpwiBest.MethProp()->getClass()->IsDelegate());
        ASSERT(!mpwiBest || mpwiBest.Type()->getAggregate()->IsDelegate());
        fUseDelegateErrors = true;
        nameErr = grp->object->type->getAggregate()->name;
    }

    if (mpwiBest) {
        // Best matching overloaded method 'name' had some invalid arguments.
        if (atsDelegate) {
            compiler()->ErrorRef(tree, ERR_MethDelegateMismatch, nameErr, atsDelegate, mpwiBest);
            return false;
        }

        if (fUseDelegateErrors)
            compiler()->Error(tree, ERR_BadDelArgTypes, mpwiBest.Type()); // Point to the Delegate, not the Invoke method
        else
            compiler()->Error(tree, ERR_BadArgTypes, mpwiBest);

        // Argument X: cannot convert type 'Y' to type 'Z'
        for (int ivar = 0; ivar < args->carg; ivar++) {
            TYPESYM * var = paramsBest->Item(ivar);

            if (!canConvert(args->prgexpr[ivar], var)) {
                // See if they just differ in out / ref.
                TYPESYM * argStripped = args->types->Item(ivar)->isPARAMMODSYM() ?
                    args->types->Item(ivar)->asPARAMMODSYM()->paramType() : args->types->Item(ivar);
                TYPESYM * varStripped = var->isPARAMMODSYM() ? var->asPARAMMODSYM()->paramType() : var;
                if (argStripped == varStripped) {
                    if (varStripped != var) {
                        compiler()->Error(args->prgexpr[ivar]->tree, ERR_BadArgRef, ivar + 1, var->asPARAMMODSYM()->isOut ? L"out" : L"ref");
                    }
                    else {
                        // the argument is decorated, but doesn't needs a 'ref' or 'out'
                        compiler()->Error(args->prgexpr[ivar]->tree, ERR_BadArgExtraRef, ivar + 1, args->types->Item(ivar)->asPARAMMODSYM()->isOut ? L"out" : L"ref");
                    }
                }
                else {
                    compiler()->Error(args->prgexpr[ivar]->tree, ERR_BadArgType, ivar + 1,
                        ErrArg(args->types->Item(ivar), ErrArgFlags::Unique), ErrArg(var, ErrArgFlags::Unique));
                }
            }
        }
    }
    else if (mwtCantInfer) {
        compiler()->Error(tree, ERR_CantInferMethTypeArgs, mwtCantInfer);
    }
    else if (mwtBadArity) {
        int cvar = mwtBadArity.Meth()->typeVars->size;
        compiler()->ErrorRef(tree, cvar > 0 ? ERR_BadArity : ERR_HasNoTypeVars, mwtBadArity, ErrArgSymKind(mwtBadArity.Meth()), args->carg);
    }
    else if (mpwiParamTypeConstraints) {
        // This will always report an error
        TypeBind::CheckMethConstraints(compiler(), tree, mpwiParamTypeConstraints);
    }
    else {
        CError * err = NULL;

        if (atsDelegate) {
            err = compiler()->MakeError(tree, ERR_MethDelegateMismatch, nameErr, atsDelegate);
            compiler()->AddRelatedSymLoc(err, atsDelegate);
        }
        else {
            if (grp->flags & EXF_CTOR) {
                nameErr = grp->typePar->isTYVARSYM() ? grp->typePar->name : grp->typePar->getAggregate()->name;
            }

            err = compiler()->MakeError(tree, fUseDelegateErrors ? ERR_BadDelArgCount : ERR_BadArgCount, nameErr, args->carg);
        }

        for (int i = 0; i < cswtWrongCount; i++) {
            if (compiler()->clsDeclRec.CheckAccess(rgswtWrongCount[i].Sym(), rgswtWrongCount[i].Type(), parentDecl, typeQual)) {
                compiler()->AddRelatedSymLoc(err, rgswtWrongCount[i].Sym());
            }
        }
        compiler()->SubmitError(err);
    }

    return false;
}

void FUNCBREC::FillInArgInfoFromArgList(ArgInfos *argInfo, EXPR * args)
{
    TYPESYM ** prgtype = STACK_ALLOC(TYPESYM*, argInfo->carg);
    argInfo->fHasExprs = true;

    int iarg = 0;
    for (EXPR * list = args; list; iarg++) {
        EXPR * arg;
        if (list->kind == EK_LIST) {
            arg = list->asBIN()->p1;
            list = list->asBIN()->p2;
        }
        else {
            arg = list;
            list = NULL;
        }
        prgtype[iarg] = arg->type;
        argInfo->prgexpr[iarg] = arg;
    }
    ASSERT(iarg == argInfo->carg);
    argInfo->types = compiler()->getBSymmgr().AllocParams(iarg, prgtype);
}


// Given a method group or indexer group, bind it to the arguments for an invocation.
EXPR * FUNCBREC::BindGrpToArgs(BASENODE * tree, int bindFlags, EXPRMEMGRP * grp, EXPR * args)
{
    ASSERT(grp->sk == SK_METHSYM || grp->sk == SK_PROPSYM && (grp->flags & EXF_INDEXER));

    // Count the args.
    ArgInfos * argInfo;
    int carg = 0;
    if (args) {
        for (EXPR * list = args; list; ) {
            carg++;

            EXPR * arg;

            if (list->kind == EK_LIST) {
                arg = list->asBIN()->p1;
                list = list->asBIN()->p2;
            }
            else {
                arg = list;
                list = NULL;
            }

            // If an arg has a generic ERRORSYM for its type, bail out.
            if (arg->type->isERRORSYM() && !arg->type->parent)
                return newError(tree, NULL);
        }
    }
    argInfo = (ArgInfos*)_alloca(SizeAdd(sizeof(ArgInfos), SizeMul(carg, sizeof(EXPR *))));
    argInfo->carg = carg;
    FillInArgInfoFromArgList(argInfo, args);

    MethPropWithInst mpwiBest;
    if (!BindGrpToArgsCore(&mpwiBest, tree, bindFlags, grp, argInfo, NULL, true, NULL))
        return newError(tree, NULL);

    EXPR * exprRes;

    if (grp->sk == SK_PROPSYM) {
        ASSERT(grp->flags & EXF_INDEXER);

        ASSERT(EXF_BASECALL == BIND_BASECALL);
        exprRes = BindToProperty(tree, grp->object, mpwiBest, (bindFlags | (grp->flags & EXF_BASECALL)), args);
    }
    else {
        ASSERT(grp->sk == SK_METHSYM);
        exprRes = BindToMethod(tree, grp->object, mpwiBest, args, (MemLookFlagsEnum)grp->flags);
    }

    return exprRes;
}


/***************************************************************************************************
    Given a method group, find one with the correct parameter types. This does NOT do type argument
    inferencing.
***************************************************************************************************/
bool FUNCBREC::BindGrpToParams(MethPropWithInst * pmpwi, BASENODE * tree, EXPRMEMGRP * grp,
    TypeArray * params, bool fReportErrors, MethPropWithInst * pmpwiAmbig, TYPESYM * typeRet)
{
    ASSERT(pmpwi);
    ASSERT(grp->sk == SK_METHSYM || grp->sk == SK_PROPSYM);
    ASSERT(fReportErrors || pmpwiAmbig);

    NAME * name = grp->name;
    TypeArray * typeArgs = grp->typeArgs;
    symbmask_t mask = (symbmask_t)1 << grp->sk;

    // Only methods should have type args.
    ASSERT(mask == MASK_METHSYM || typeArgs->size == 0);

    TYPESYM * typeQual;

    if (grp->flags & EXF_BASECALL)
        typeQual = NULL;
    else if (grp->flags & EXF_CTOR)
        typeQual = grp->typePar;
    else if (grp->object)
        typeQual = grp->object->type;
    else
        typeQual = NULL;

    TypeArray * types = grp->types;
    if (!types)
        types = BSYMMGR::EmptyTypeArray();
    ASSERT(types->size > 0 || !grp->typePar->isInterfaceType());

    AGGTYPESYM * typeCur;
    int itypeCur = 0;

    if (types->size > 0) {
        typeCur = types->Item(itypeCur)->asAGGTYPESYM();
        // Clear all the hidden flags.
        for (int i = 0; i < types->size; i++) {
            types->Item(i)->asAGGTYPESYM()->fAllHidden = false;
        }
    }
    else {
        // If the full set of types isn't given, we better be in a class.
        // For the interface and type variable case, MemberLookup.Lookup should have been called.
        ASSERT(grp->typePar->isAGGTYPESYM() && !grp->typePar->isInterfaceType());
        typeCur = grp->typePar->asAGGTYPESYM();
    }

    ASSERT(!grp->mps || grp->mps->getClass() == typeCur->getAggregate());

    ArgInfos argParam;
    argParam.carg = params->size;
    argParam.types = params;
    argParam.fHasExprs = false;

    MethPropWithInst mpwiFound(NULL, NULL, NULL);
    MethPropWithInst mpwiInaccess(NULL, NULL, NULL);
    MethPropWithInst mpwiBogus(NULL, NULL, NULL);
    MethPropWithInst mpwiAmbig(NULL, NULL, NULL);
    MethWithType mwtBadArity(NULL, NULL);

    SymWithType rgswtWrong[20];
    const int cswtMaxWrong = lengthof(rgswtWrong);
    int cswtWrongCount = 0;
    int cswtWrongSig = 0;
    bool fBaseHidden = false;

    // Look for matches.
    for (METHPROPSYM * mpsCur = NULL; ; ) {
        // Find the next sym.
        mpsCur = (mpsCur == NULL) ?
            compiler()->getBSymmgr().LookupAggMember(name, typeCur->getAggregate(), mask)->asMETHPROPSYM() :
            compiler()->getBSymmgr().LookupNextSym(mpsCur, typeCur->getAggregate(), mask)->asMETHPROPSYM();

        if (!mpsCur) {
            // Find the next type.
            if (fBaseHidden) {
                // Something in the current class has hidden everything in base classes and interfaces.
                ASSERT(!typeCur->isInterfaceType());
                break;
            }
            if (mpwiFound && !mpwiFound.Type()->isInterfaceType()) {
                // We've found some applicable methods in a class so we don't need to look any further.
                break;
            }
            if (types->size > 0) {
                for (;;) {
                    if (++itypeCur >= types->size)
                        goto LSearchDone;
                    typeCur = types->Item(itypeCur)->asAGGTYPESYM();
                    if (!typeCur->fAllHidden)
                        break;
                }
            }
            else {
                ASSERT(!mpwiFound);
                typeCur = typeCur->GetBaseClass();
                if (!typeCur)
                    break;
            }
            continue;
        }

        if (grp->mps && mpsCur != grp->mps)
            continue;

        // Make sure that whether we're seeing a ctor is consistent with the flag.
        // The only properties we handle are indexers.
        if (mask == MASK_METHSYM && (
                !(grp->flags & EXF_CTOR) != !mpsCur->asMETHSYM()->isCtor() ||
                !(grp->flags & EXF_OPERATOR) != !mpsCur->asMETHSYM()->isOperator) ||
            mask == MASK_PROPSYM && !mpsCur->asPROPSYM()->isIndexer())
        {
            // Get the next symbol.
            continue;
        }

        if (!isMethPropCallable(mpsCur, (grp->flags & EXF_USERCALLABLE) != 0))
            continue;

        if (params->size != mpsCur->params->size) {
            if (cswtWrongCount < cswtMaxWrong && cswtWrongSig == 0)
                rgswtWrong[cswtWrongCount++].Set(mpsCur, typeCur);
            continue;
        }

        if (mpsCur->isMETHSYM() && mpsCur->asMETHSYM()->typeVars->size != typeArgs->size) {
            if (!mwtBadArity)
                mwtBadArity.Set(mpsCur->asMETHSYM(), typeCur);
            continue;
        }

        if (!compiler()->getBSymmgr().SubstEqualTypeArrays(params, mpsCur->params, typeCur, typeArgs) ||
             typeRet && typeRet != compiler()->getBSymmgr().SubstType(mpsCur->retType, typeCur, typeArgs))
        {
            if (cswtWrongSig < cswtMaxWrong)
                rgswtWrong[cswtWrongSig++].Set(mpsCur, typeCur);
            continue;
        }

        if (!compiler()->clsDeclRec.CheckAccess(mpsCur, typeCur, parentDecl, typeQual)) {
            if (!mpwiInaccess && !mpwiFound)
                mpwiInaccess.Set(mpsCur, typeCur, typeArgs);
            continue;
        }
        if (compiler()->CheckBogus(mpsCur)) {
            if (!mpwiBogus && !mpwiFound)
                mpwiBogus.Set(mpsCur, typeCur, typeArgs);
            continue;
        }

        if (!mpwiFound) {
            mpwiFound.Set(mpsCur, typeCur, typeArgs);
            ASSERT(!mpwiAmbig);
        }
        else if (mpwiFound.MethProp()->modOptCount != mpsCur->modOptCount) {
            if (mpwiFound.MethProp()->modOptCount > mpsCur->modOptCount) {
                // mpsCur is better.
                mpwiFound.Set(mpsCur, typeCur, typeArgs);
                mpwiAmbig.Clear();
            }
        }
        else if (!mpwiAmbig) {
            // Ambiguous.
            mpwiAmbig.Set(mpsCur, typeCur, typeArgs);
        }

        if (!typeCur->isInterfaceType()) {
            // This one hides all possibilities in base classes and in all interfaces.
            fBaseHidden = true;
        }
        else {
            // This one hides all possibilities in base interfaces so mark them.
            TypeArray * ifaces = typeCur->GetIfacesAll();
            for (int i = 0; i < ifaces->size; i++) {
                AGGTYPESYM * type = ifaces->Item(i)->asAGGTYPESYM();
                ASSERT(type->isInterfaceType());
                type->fAllHidden = true;
            }

            // Mark object.
            AGGTYPESYM * typeObject = GetReqPDT(PT_OBJECT);
            typeObject->fAllHidden = true;
        }
    }

LSearchDone:
    if (pmpwiAmbig)
        *pmpwiAmbig = mpwiAmbig;

    if (fReportErrors) {
        if (!mpwiFound) {
            ASSERT(!mpwiAmbig);

            if (mpwiInaccess) {
                // We might have called this, but it is inaccesible...
                compiler()->clsDeclRec.ReportAccessError(tree, mpwiInaccess, parentDecl, typeQual);
            }
            else if (mpwiBogus) {
                // We might have called this, but it is bogus...
                compiler()->ErrorRef(tree, ERR_BindToBogus, mpwiBogus);
            }
            else if (mwtBadArity && cswtWrongSig == 0) {
                int cvar = mwtBadArity.Meth()->typeVars->size;
                compiler()->ErrorRef(tree, cvar > 0 ? ERR_BadArity : ERR_HasNoTypeVars, mwtBadArity, ErrArgSymKind(mwtBadArity.Meth()), cvar);
            }
            else {
                CError * err = compiler()->MakeError(tree, cswtWrongSig > 0 ? ERR_WrongSignature : ERR_BadArgCount, name, params->size);

                int cswt = cswtWrongSig > 0 ? cswtWrongSig : cswtWrongCount;

                // Report possible matches (same name and is accesible).
                for (int i = 0; i < cswt; i++) {
                    if (compiler()->clsDeclRec.CheckAccess(rgswtWrong[i].Sym(), rgswtWrong[i].Type(), parentDecl, typeQual)) {
                        compiler()->AddRelatedSymLoc(err, rgswtWrong[i].Sym());
                    }
                }
                compiler()->SubmitError(err);
            }
        }
        else if (mpwiAmbig) {
            compiler()->Error(tree, ERR_AmbigMember, mpwiFound, mpwiAmbig);
            // Return one of them.
        }
    }

    *pmpwi = mpwiFound;
    return (bool)mpwiFound;
}


/***************************************************************************************************
    Convert a method group to a delegate type.

    NOTE: Currently it is not well defined when there is an implicit conversion from a method
    group to a delegate type. There are several possibilities. On the two extremes are:

    (1) (Most permissive) When there is at least one applicable method in the method group.

    (2) (Most restrictive) When all of the following are satisified:
        * Overload resolution does not produce an error
        * The method's parameter types don't require any conversions other than implicit reference
          conversions.
        * The method's return type is compatible.
        * The method's constraints are satisified.
        * The method is not conditional.

    For (1), it may be the case that an error is produced whenever the conversion is actually used.
    For example, if the result of overload resolution is ambiguous or if the result of overload
    resolution is a method with the wrong return result or with unsatisfied constraints.

    For (2), the intent is that if the answer is yes, then an error is never produced.

    Note that (2) is not monotone: adding a method to the method group may cause the answer
    to go from yes to no. This has a very odd effect in certain situations:

    Suppose:
        * I1 and J1 are interfaces with I1 : J1.
        * I2, J2 and K2 are interfaces with I2 : J2, K2.
        * Di is a delegate type with signature void Di(Ii).
        * A method group named F contains F(D1(I1)) and F(D2(I2)).
        * There is another method group named M containing a subset of:
            void M(J1)
            void M(J2)
            void M(K2)

    Under any of the definitions we're considering:

        * If M is { M(J1), M(J2) } then F(M) is an error (ambiguous between F(D1) and F(D2)).
        * If M is { M(J1), M(K2) } then F(M) is an error (ambiguous between F(D1) and F(D2)).
        * If M is { M(J2), M(K2) } then F(M) is an error (M -> D2 is ambiguous).

    If M is { M(J1), M(J2), M(K2) } what should F(M) be? It seems logical for F(M) to be ambiguous
    in this case as well. However, under definition (2), there is no implicit conversion from M
    to D2 (since overload resolution is ambiguous). Thus F(M) is unambiguously taken to mean
    F(D1) applied to M(J1). Note that the user has just made the situation more ambiguous by having
    all three methods in the method group, but we ignore this additional ambiguity and pick a
    winner (rather arbitrarily).

    We currently implement (1). The spec needs to be tightened up.
    REVIEW ShonK: Fix the spec.
***************************************************************************************************/
bool FUNCBREC::BindGrpConversion(BASENODE * tree, EXPRMEMGRP * grp, TYPESYM * typeDst, EXPR ** pexprDst, bool fReportErrors)
{
    if (pexprDst)
        *pexprDst = NULL;

    if (!typeDst->isDelegateType()) {
        if (fReportErrors)
            compiler()->Error(tree, ERR_MethGrpToNonDel, grp->name, typeDst);
        return false;
    }

    AGGTYPESYM * type = typeDst->asAGGTYPESYM();

    METHSYM * methCtor;
    METHSYM * methInvoke;

    methCtor = FindDelegateCtor(type, tree, fReportErrors);
    if (!methCtor)
        return false;

    // Now, find the invoke function on the delegate.
    methInvoke = compiler()->getBSymmgr().LookupInvokeMeth(type->getAggregate());
    ASSERT(methInvoke && methInvoke->isInvoke());

    TypeArray * params = compiler()->getBSymmgr().SubstTypeArray(methInvoke->params, type);
    TYPESYM * typeRet = compiler()->getBSymmgr().SubstType(methInvoke->retType, type);
    ArgInfos argParam;
    argParam.carg = params->size;
    argParam.types = params;
    argParam.fHasExprs = false;

    // Next, verify that the function has a suitable type for the invoke method.
    MethWithInst mwiWrap;
    MethWithInst mwiAmbig;

    if (compiler()->options.IsECMA1Mode()) {
        if (!BindGrpToParams(&mwiWrap, tree, grp, params, fReportErrors, &mwiAmbig, typeRet))
            return false;
    }
    else {
        if (!BindGrpToArgsCore(&mwiWrap, tree, BIND_NOPARAMS, grp, &argParam, type, fReportErrors, &mwiAmbig))
            return false;
    }

    // From here on we should only return true.
    if (!fReportErrors && !pexprDst)
        return true;

    // Note: We report errors below even if fReportErrors is false. Note however that we only
    // get here if pexprDst is non-null and we'll return true even if we report an error, so this
    // is really the only chance we'll get to report the error.
    bool fError = (bool)mwiAmbig;

    if (mwiAmbig && !fReportErrors) {
        // Report the ambiguity, since BindGrpToArgsCore didn't.
        compiler()->Error(tree, ERR_AmbigCall, mwiWrap, mwiAmbig);
    }

    TYPESYM * typeRetReal = compiler()->getBSymmgr().SubstType(mwiWrap.Meth()->retType, mwiWrap.Type(), mwiWrap.TypeArgs());
    if (typeRet != typeRetReal && !FImpRefConv(typeRetReal, typeRet)) {
        compiler()->ErrorRef(tree, ERR_BadRetType, mwiWrap, typeRetReal);
        fError = true;
    }

    TypeArray * paramsReal = compiler()->getBSymmgr().SubstTypeArray(mwiWrap.Meth()->params, mwiWrap.Type(), mwiWrap.TypeArgs());
    if (paramsReal != params) {
        for (int i = 0; i < paramsReal->size; i++) {
            TYPESYM * param = params->Item(i);
            TYPESYM * paramReal = paramsReal->Item(i);

            if (param != paramReal && !FImpRefConv(param, paramReal)) {
                compiler()->ErrorRef(tree, ERR_MethDelegateMismatch, mwiWrap, typeDst);
                fError = true;
                break;
            }
        }
    }

    EXPR * obj = grp->object;

    PostBindMeth(tree, !!(grp->flags & EXF_BASECALL), &mwiWrap, &obj, NULL);
    ASSERT(mwiWrap.Meth()->getKind() == SK_METHSYM);

    if (mwiWrap.TypeArgs()->size > 0) {
        // Check method type variable constraints.
        TypeBind::CheckMethConstraints(compiler(), tree, mwiWrap);
    }

    if (compiler()->clsDeclRec.GetConditionalSymbols(mwiWrap.Meth()) != NULL) {
        compiler()->ErrorRef(tree, ERR_DelegateOnConditional, mwiWrap);
    }

    // Warn if the new binding rules found something different from the old binding rules.
    // Again we specifically ignore fReportErrors because we need to report the warning
    // if we're going to succeed the bind
    if (paramsReal != params && !fError && !compiler()->options.IsECMA1Mode()) {
        MethWithInst mwiOld;
        MethWithInst mwiT;

        if (BindGrpToParams(&mwiOld, tree, grp, params, false, &mwiT))
            compiler()->Error(tree, WRN_DelegateNewMethBind, typeDst, ErrArgRef(mwiWrap), ErrArgRef(mwiOld));
    }

    if (!pexprDst)
        return true;

    EXPRFUNCPTR * funcPtr;
    funcPtr = newExpr(grp->tree, EK_FUNCPTR, getVoidType())->asFUNCPTR();
    funcPtr->mwi = mwiWrap;
    funcPtr->flags |= (grp->flags & EXF_BASECALL);

    if (!mwiWrap.Meth()->isStatic) {
        if (mwiWrap.Meth()->getClass()->isPredefAgg(PT_G_OPTIONAL)) {
            compiler()->Error(tree, ERR_DelegateOnNullable, mwiWrap);
        }
        funcPtr->object = obj;
        if (obj && obj->type->fundType() != FT_REF) {
            // Must box the object before creating a delegate to it.
            obj = mustConvert(obj, GetReqPDT(PT_OBJECT));
        }
    } else {
        funcPtr->object = NULL;
        obj = bindNull(NULL);
    }

    EXPRCALL * call = newExpr(grp->tree, EK_CALL, type)->asCALL();
    call->args = newExprBinop(NULL, EK_LIST, getVoidType(), obj, funcPtr);
    call->object = NULL;
    call->mwi.Set(methCtor, type, NULL);
    call->flags |= EXF_NEWOBJCALL | EXF_CANTBENULL;

    *pexprDst = call;

    // If we actually create the destination expr, then set the node sym and expr.
    if (!(grp->flags & EXF_DELEGATE)) {
        SetNodeExpr(grp->tree, call);
    }

    return true;
}


void FUNCBREC::PostBindMeth(BASENODE * tree, bool fBaseCall, MethWithInst * pmwi, EXPR ** pobj, bool * pfConstrained)
{
    MethWithInst mwiOrig = *pmwi;
    EXPR * objOrig = *pobj;

    // If it is virtual, find a remap of the method to something more specific.  This
    // may alter where the method is found.
    if (*pobj && (fBaseCall || (*pobj)->type->isSimpleType() || (*pobj)->type->isSpecialByRefType()))
        RemapToOverride(pmwi, (*pobj)->type);

    AdjustMemberObj(tree, *pmwi, pobj, pfConstrained);
    CheckFieldUse(objOrig);

    if (fBaseCall && pmwi->Meth()->isAbstract)
        compiler()->Error(tree, ERR_AbstractBaseCall, *pmwi);

    if (mwiOrig.Meth()->IsDeprecated())
        ReportDeprecated(tree, mwiOrig);
    else if (pmwi->Meth()->IsDeprecated())
        ReportDeprecated(tree, *pmwi);

    if (pmwi->Meth()->retType) {
        compiler()->EnsureState(pmwi->Meth()->retType);
        checkUnsafe(tree, pmwi->Meth()->retType);
        bool fCheckParams = false;

        if (pmwi->Meth()->isExternal) {
            fCheckParams = true;
            SetExternalRef(pmwi->Meth()->retType);
        }

        // We need to check unsafe on the parameters as well, since we cannot check in conversion.
        TypeArray * params = pmwi->Meth()->params;
        compiler()->EnsureState(params);

        for (int i = 0; i < params->size; i++) {
            // This is an optimization: don't call this in the vast majority of cases
            TYPESYM * type = params->Item(i);

            if (type->isUnsafe()) {
                checkUnsafe(tree, type);
            }
            if (fCheckParams && type->isPARAMMODSYM())
                SetExternalRef(type);
        }
    }
}


void FUNCBREC::PostBindProp(BASENODE * tree, bool fBaseCall, PropWithType pwt, /*in/out*/ EXPR ** pobj,
    /*out*/ bool * pfConstrained, /*out*/ MethWithType * pmwtGet, /*out*/ MethWithType * pmwtSet)
{
    ASSERT(pmwtGet);
    ASSERT(pmwtSet);

    EXPR * objOrig = *pobj;

    // Get the accessors.
    if (pwt.Prop()->methGet)
        pmwtGet->Set(pwt.Prop()->methGet, pwt.Type());
    else
        pmwtGet->Clear();
    if (pwt.Prop()->methSet)
        pmwtSet->Set(pwt.Prop()->methSet, pwt.Type());
    else
        pmwtSet->Clear();

    // If it is virtual, find a remap of the method to something more specific.  This
    // may alter where the accessors are found.
    if (fBaseCall && *pobj) {
        if (*pmwtGet)
            RemapToOverride(pmwtGet, (*pobj)->type);
        if (*pmwtSet)
            RemapToOverride(pmwtSet, (*pobj)->type);
    }

    if (*pmwtGet && (!*pmwtSet || pmwtSet->Type() == pmwtGet->Type() || compiler()->IsBaseType(pmwtGet->Type(), pmwtSet->Type())))
        AdjustMemberObj(tree, *pmwtGet, pobj, pfConstrained);
    else if (*pmwtSet)
        AdjustMemberObj(tree, *pmwtSet, pobj, pfConstrained);
    else
        AdjustMemberObj(tree, pwt, pobj, pfConstrained);

    CheckFieldUse(objOrig);

    if (pwt.Prop()->IsDeprecated())
        ReportDeprecated(tree, pwt);
    else if (*pmwtGet && pmwtGet->Meth()->getProperty()->IsDeprecated())
        ReportDeprecated(tree, PropWithType(pmwtGet->Meth()->getProperty(), pmwtGet->Type()));
    else if (*pmwtSet && pmwtSet->Meth()->getProperty()->IsDeprecated())
        ReportDeprecated(tree, PropWithType(pmwtSet->Meth()->getProperty(), pmwtSet->Type()));

    if (pwt.Prop()->retType) {
        compiler()->EnsureState(pwt.Prop()->retType);
        checkUnsafe(tree, pwt.Prop()->retType);
    }
}


void FUNCBREC::SetExternalRef(TYPESYM * type)
{
    AGGSYM * agg = type->GetNakedAgg();
    if (!agg || agg->hasExternReference)
        return;

    agg->hasExternReference = true;
    FOREACHCHILD(agg, sym)
        if (sym->isMEMBVARSYM())
            SetExternalRef(sym->asMEMBVARSYM()->type);
    ENDFOREACHCHILD
}


// Bind a function call
EXPR * FUNCBREC::bindCall(CALLNODE * tree, int bindFlags)
{
    ASSERT(tree->Op() == OP_CALL);
    EXPR * args;

    if (tree->p1->kind == NK_OP && tree->p1->Op() == OP_ARGS) {
        if (!(bindFlags & BIND_ARGUMENTS)) {
            compiler()->Error(tree, ERR_IllegalArglist);
        }
        args = bindExpr(tree->p2, BIND_RVALUEREQUIRED | BIND_ARGUMENTS);
        if (args && !args->isOK())
            return newError(tree, compiler()->getBSymmgr().GetArglistSym());
        return newExprBinop(tree, EK_ARGLIST, compiler()->getBSymmgr().GetArglistSym(), args, NULL);
    }

    EXPR * expr = bindMethodName(tree->p1);
    args = bindExpr(tree->p2, BIND_RVALUEREQUIRED | BIND_ARGUMENTS);

    if (!expr->isOK())
        return newError(tree, NULL);

    EXPRMEMGRP * grp = expr->asMEMGRP();
    ASSERT(grp->sk == SK_METHSYM);

    return BindGrpToArgs(tree, bindFlags, grp, args);
}


// This table is used to implement the last set of 'better' conversion rules
// when there are no implicit conversions between T1(down) and T2 (across)
// Use all the simple types plus 1 more for Object
// See CLR section 7.4.1.3
static const byte betterConversionTable[NUM_EXT_TYPES + 1][NUM_EXT_TYPES + 1] = {
//          BYTE    SHORT   INT     LONG    FLOAT   DOUBLE  DECIMAL CHAR    BOOL    SBYTE   USHORT  UINT    ULONG   IPTR     UIPTR    OBJECT
/* BYTE   */{0,     0,      0,      0,      0,      0,      0,      0,      0,      2,      0,      0,      0,      0,       0,       0},
/* SHORT  */{0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      1,      1,      1,      0,       0,       0},
/* INT    */{0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      1,      1,      0,       0,       0},
/* LONG   */{0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      1,      0,       0,       0},
/* FLOAT  */{0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,       0,       0},
/* DOUBLE */{0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,       0,       0},
/* DECIMAL*/{0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,       0,       0},
/* CHAR   */{0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,       0,       0},
/* BOOL   */{0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,       0,       0},
/* SBYTE  */{1,     0,      0,      0,      0,      0,      0,      0,      0,      0,      1,      1,      1,      0,       0,       0},
/* USHORT */{0,     2,      0,      0,      0,      0,      0,      0,      0,      2,      0,      0,      0,      0,       0,       0},
/* UINT   */{0,     2,      2,      0,      0,      0,      0,      0,      0,      2,      0,      0,      0,      0,       0,       0},
/* ULONG  */{0,     2,      2,      2,      0,      0,      0,      0,      0,      2,      0,      0,      0,      0,       0,       0},
/* IPTR   */{0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,       0,       0},
/* UIPTR  */{0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,       0,       0},
/* OBJECT */{0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,       0,       0}
};


//
//
int FUNCBREC::WhichMethodIsBetter(METHLIST * node1, METHLIST * node2, ArgInfos * args)
{
    MethPropWithInst mpwi1 = node1->mpwi;
    MethPropWithInst mpwi2 = node2->mpwi;

    // Substitutions should have already been done on these!
    TypeArray *pta1 = node1->params;
    TypeArray *pta2 = node2->params;

    ASSERT(pta1->size == pta2->size && args->carg == pta1->size);

    if (pta1 == pta2) {
        // Same signatures. If the have different lifting numbers, the smaller number wins.
        // Otherwise, if one is generic and the other isn't then the non-generic wins.
        // Otherwise, if one is expanded and the other isn't then the non-expanded wins.
        // Otherwise, if one has fewer modopts than the other then it wins.
        if (node1->ctypeLift != node2->ctypeLift)
            return node1->ctypeLift < node2->ctypeLift ? +1 : -1;

        // Non-generic wins.
        if (mpwi1.TypeArgs()->size != 0) {
            if (mpwi2.TypeArgs()->size == 0)
                return -1;
        }
        else if (mpwi2.TypeArgs()->size != 0)
            return +1;

        // Non-expanded wins
        if (node1->fExpanded) {
            if (!node2->fExpanded)
                return -1;
        }
        else if (node2->fExpanded)
            return +1;

        // See if one's parameter types (un-instantiated) are more specific.
        int nT = compiler()->getBSymmgr().CompareTypes(mpwi1.MethProp()->params, mpwi2.MethProp()->params);
        if (nT)
            return nT;

        // Fewer modopts wins.
        if (mpwi1.MethProp()->modOptCount != mpwi2.MethProp()->modOptCount)
            return mpwi1.MethProp()->modOptCount < mpwi2.MethProp()->modOptCount ? +1 : -1;

        // Bona-fide tie.
        return 0;
    }

    bool a2b, b2a;
    int better = 0;

    for (int i = 0; i < args->carg; i++)
    {
        TYPESYM *arg = args->types->Item(i);
        TYPESYM *p1 = pta1->Item(i);
        TYPESYM *p2 = pta2->Item(i);

        if (p1 == p2)
            continue;

        // Check exact matches:
        if (arg == p1)
            goto BETM1;
        if (arg == p2)
            goto BETM2;

        a2b = canConvert(p1, p2);
        b2a = canConvert(p2, p1);

        if (!(a2b ^ b2a) &&
            p1->isPredefined() && p2->isPredefined() &&
            p1->getPredefType() <= PT_OBJECT && p2->getPredefType() <= PT_OBJECT) {
            // See CLR section 7.4.1.3
            int c = betterConversionTable[p1->getPredefType()][p2->getPredefType()];
            if (c == 1)
                goto BETM1;
            else if (c == 2)
                goto BETM2;
        }

        if (a2b && !b2a) {
BETM1:
            if (better < 0) {
                return 0;
            } else {
                better = 1;
                continue;
            }
        }
        if (b2a && !a2b) {
BETM2:
            if (better > 0) {
                return 0;
            } else {
                better = -1;
            }
        }
    }

    return better;
}


EXPR * FUNCBREC::BindUDBinop(BASENODE * tree, EXPRKIND ek, EXPR * arg1, EXPR * arg2, bool fDontLift)
{
    NAME * name = ekName(ek);
    ASSERT(name);

    AGGTYPESYM * rgats[2];
    int cats = 0;

    for (int i = 0; i < 2; i++) {
        TYPESYM * type = (i == 0) ? arg1->type : arg2->type;
LAgain:
        compiler()->EnsureState(type);
        switch (type->getKind()) {
        case SK_NUBSYM:
            type = type->StripNubs();
            goto LAgain;
        case SK_TYVARSYM:
            type = type->asTYVARSYM()->GetBaseCls();
            goto LAgain;
        case SK_AGGTYPESYM:
            if ((type->isClassType() || type->isStructType()) && !type->asAGGTYPESYM()->getAggregate()->fSkipUDOps) {
                PREFAST_ASSUME(cats < 2, "This will be hit no more than twice per the loop termination condition");
                rgats[cats++] = type->asAGGTYPESYM();
            }
            break;
        default:
            break;
        }
    }

    if (!cats)
        return NULL;

    if (cats == 2 && rgats[0] == rgats[1]) {
        // Common case: they're the same.
        cats = 1;
    }

    EXPR * args = newExprBinop(NULL, EK_LIST, NULL, arg1, arg2);
    ArgInfos * info = (ArgInfos*)_alloca(sizeof(ArgInfos) + sizeof(EXPR*) * 2);

    info->carg = 2;
    FillInArgInfoFromArgList(info, args);

    bool fEqOp = false;
    bool fRelOp = false;

    switch (ek) {
    case EK_EQ:
    case EK_NE:
        fEqOp = true;
        break;
    case EK_GT:
    case EK_GE:
    case EK_LT:
    case EK_LE:
        fRelOp = true;
        break;
    default:
        break;
    }

    METHLIST * pmethFirst = NULL;
    METHLIST ** ppmethAdd = &pmethFirst;
    bool fFoundSomeInType = false;
    int iats = 0;
    METHSYM * methCur = NULL;
    AGGTYPESYM * atsCur = rgats[0];

    for (;;) {
        if (pmethFirst && iats > 0 && atsCur == rgats[0]) {
            // We're searching the 2nd type hierarchy and we've hit
            // the type in the 1st hieararchy where we found some. So we're done.
            break;
        }

        // Find the next operator.
        methCur = (methCur == NULL) ?
            compiler()->getBSymmgr().LookupAggMember(name, atsCur->getAggregate(), MASK_METHSYM)->asMETHSYM() :
            compiler()->getBSymmgr().LookupNextSym(methCur, atsCur->getAggregate(), MASK_METHSYM)->asMETHSYM();

        if (!methCur) {
            // Find the next type.
            // If we've found some applicable methods in a class then we don't need to look any further.
            if (!fFoundSomeInType) {
                atsCur = atsCur->GetBaseClass();
                if (atsCur)
                    continue;
            }

            // Get the next type.
            if (++iats >= cats) {
                // Done with types.
                break;
            }
            atsCur = rgats[iats];
            fFoundSomeInType = false;
            continue;
        }

        // Only look at operators with 2 args.
        if (!methCur->isOperator || methCur->params->size != 2)
            continue;
        ASSERT(methCur->typeVars->size == 0);

        TypeArray * paramsCur = compiler()->getBSymmgr().SubstTypeArray(methCur->params, atsCur);
        METHLIST * pmeth = NULL;
        bool fConv1;

        if ((fConv1 = canConvert(arg1, paramsCur->Item(0))) && canConvert(arg2, paramsCur->Item(1))) {
            pmeth = STACK_ALLOC(METHLIST, 1);
            pmeth->ctypeLift = 0;
            pmeth->params = paramsCur;
        }
        else {
            // Check whether any lifted operator is applicable.
            if (!compiler()->FCanLift() || fDontLift)
                continue;
            if (!paramsCur->Item(0)->IsNonNubValType() ||
                !paramsCur->Item(1)->IsNonNubValType())
            {
                continue;
            }

            // If the return type isn't a value type we can't lift.
            TYPESYM * typeRet = compiler()->getBSymmgr().SubstType(methCur->retType, atsCur);
            if (!typeRet->IsNonNubValType())
                continue;

            if (fEqOp && (!typeRet->isPredefType(PT_BOOL) || paramsCur->Item(0) != paramsCur->Item(1)))
                continue;
            if (fRelOp && !typeRet->isPredefType(PT_BOOL))
                continue;

            TYPESYM * rgtype[2];

            rgtype[0] = compiler()->getBSymmgr().GetNubType(paramsCur->Item(0));
            rgtype[1] = compiler()->getBSymmgr().GetNubType(paramsCur->Item(1));

            if (!fConv1 && !canConvert(arg1, rgtype[0]) || !canConvert(arg2, rgtype[1]))
                continue;

            pmeth = STACK_ALLOC(METHLIST, 1);
            pmeth->ctypeLift = 2;
            pmeth->params = compiler()->getBSymmgr().AllocParams(2, rgtype);
        }
        pmeth->mpwi.Set(methCur, atsCur, BSYMMGR::EmptyTypeArray());
        pmeth->fExpanded = false;

        // Link it in.
        ASSERT(!*ppmethAdd);
        *ppmethAdd = pmeth;
        pmeth->next = NULL;
        ppmethAdd = &pmeth->next;

        fFoundSomeInType = true;
        rgats[iats] = atsCur;
    }

    if (!pmethFirst)
        return NULL;

    METHLIST * pmethAmbig1;
    METHLIST * pmethAmbig2;
    METHLIST * pmethBest = FindBestMethod(pmethFirst, info, &pmethAmbig1, &pmethAmbig2);

    if (!pmethBest) {
        // No winner, so its an ambigous call...
        compiler()->Error(tree, ERR_AmbigCall, pmethAmbig1->mpwi, pmethAmbig2->mpwi);
        return newError(tree, NULL);
    }

    if (compiler()->CheckBogus(pmethBest->mpwi.Meth())) {
        compiler()->ErrorRef(tree, ERR_BindToBogus, pmethBest->mpwi);
        return newError(tree, NULL);
    }

    if (pmethBest->mpwi.Meth()->IsDeprecated()) {
        ReportDeprecated(tree, pmethBest->mpwi);
    }

    EXPR * exprVal1 = arg1;
    EXPR * exprVal2 = arg2;
    TypeArray * params = pmethBest->params;
    TypeArray * paramsRaw = params;
    TYPESYM * typeRetRaw = compiler()->getBSymmgr().SubstType(pmethBest->mpwi.Meth()->retType, pmethBest->mpwi.Type());
    NubInfo nin;

    if (pmethBest->ctypeLift) {
        ASSERT(pmethBest->ctypeLift == 2);
        ASSERT(!fDontLift);

        // This is a lifted user defined operator.
        paramsRaw = compiler()->getBSymmgr().SubstTypeArray(pmethBest->mpwi.Meth()->params, pmethBest->mpwi.Type());
        ASSERT(params != paramsRaw);

        LiftFlagsEnum grflt;

        ASSERT(paramsRaw->Item(0) == params->Item(0)->parent);
        ASSERT(paramsRaw->Item(1) == params->Item(1)->parent);
        grflt = LiftFlags::LiftBoth;

        if (!canConvert(arg1->type->StripNubs(), paramsRaw->Item(0), NOUDC)) {
            // Need to convert first.
            exprVal1 = mustConvert(arg1, params->Item(0));
        }
        if (!canConvert(arg2->type->StripNubs(), paramsRaw->Item(1), NOUDC)) {
            // Need to convert first.
            exprVal2 = mustConvert(arg2, params->Item(1));
        }

        BindNubCondValBin(tree, exprVal1, exprVal2, nin, grflt);
        ASSERT(nin.fActive);

        // Equality operators are special. If the return type is bool and the parameter types
        // are the same then they treat null as a value and return bool.
        if (fEqOp && nin.FAlwaysNull()) {
            ASSERT((ek == EK_EQ || ek == EK_NE) && typeRetRaw->isPredefType(PT_BOOL) && paramsRaw->Item(0) == paramsRaw->Item(1));
            // At least one of them is a constant null.
            EXPR * exprRes;

            if (nin.rgfNull[0] && nin.rgfNull[1]) {
                // Both are constant nulls.
                exprRes = newExprConstant(tree, typeRetRaw, ConstValInit(ek == EK_EQ));
                exprRes = AddSideEffects(tree, exprRes, exprVal2, true, true);
                return AddSideEffects(tree, exprRes, exprVal1, true, true);
            }

            if (nin.rgfNull[0] ? !exprVal2->type->isNUBSYM() : !exprVal1->type->isNUBSYM()) {
                // One is null and the other is not nullable.
                exprRes = newExprConstant(tree, typeRetRaw, ConstValInit(ek == EK_NE));
                exprRes = AddSideEffects(tree, exprRes, exprVal2, true, true);
                return AddSideEffects(tree, exprRes, exprVal1, true, true);
            }

            // Generate seq(a, !b.HasValue) or rev(!a.HasValue, b).
            exprRes = BindNubHasValue(tree, nin.rgfNull[0] ? exprVal2 : exprVal1, ek == EK_NE);
            return AddSideEffects(tree, exprRes, nin.rgfNull[0] ? exprVal1 : exprVal2, nin.rgfNull[0], true);
        }

        exprVal1 = nin.Val(0);
        exprVal2 = nin.Val(1);
    }

    exprVal1 = mustConvert(exprVal1, paramsRaw->Item(0));
    exprVal2 = mustConvert(exprVal2, paramsRaw->Item(1));
    if (args->asBIN()->p1 != exprVal1 || args->asBIN()->p2 != exprVal2)
        args = newExprBinop(NULL, EK_LIST, NULL, exprVal1, exprVal2);

    EXPRCALL * call = newExpr(tree, EK_CALL, typeRetRaw)->asCALL();
    call->object = NULL;
    call->mwi = pmethBest->mpwi;
    call->args = args;

    verifyMethodArgs(call);

    if (!pmethBest->ctypeLift)
        return call;

    // The lifted case.
    TYPESYM * typeRet = typeRetRaw;

    if (fEqOp) {
        ASSERT((ek == EK_EQ || ek == EK_NE) && typeRetRaw->isPredefType(PT_BOOL) && paramsRaw->Item(0) == paramsRaw->Item(1));
        EXPR * exprRes;

        if (nin.FAlwaysNonNull())
            return call;

        // Generate tmp1.HasValue == tmp2.HasValue && (!tmp1.HasValue || op_Equal(tmp1.Value, tmp2.Value))
        // When one of them is not nullable this simplifies to: tmp1.HasValue && op_Equal(tmp1.Value, tmp2)

        ASSERT(nin.exprCnd && (nin.exprCnd->kind == EK_BITAND) == (nin.rgexprCnd[0] && nin.rgexprCnd[1] && !nin.fSameTemp));

        if (nin.exprCnd->kind == EK_BITAND) {
            // Generate tmp1.HasValue == tmp2.HasValue && (!tmp1.HasValue || op_Equal(tmp1.Value, tmp2.Value))
            ASSERT(!nin.fSameTemp);
            nin.exprCnd->kind = ek;
            exprRes = nin.rgexprCnd[0];
            if (ek == EK_EQ)
                exprRes = newExprBinop(tree, EK_LOGNOT, typeRet, exprRes, NULL);
            exprRes = newExprBinop(tree, ek == EK_EQ ? EK_LOGOR : EK_LOGAND, typeRet, exprRes, call);
            exprRes = newExprBinop(tree, ek == EK_NE ? EK_LOGOR : EK_LOGAND, typeRet, nin.exprCnd, exprRes);
            return BindNubAddTmps(tree, exprRes, nin);
        }
        if (nin.fSameTemp) {
            // They share the same local so have a single HasValue check.
            // Generate: !tmp.HasValue || op_Equal(tmp1.Value, dup)
            ASSERT(nin.exprCnd == nin.rgexprCnd[0]);
            exprRes = nin.exprCnd;
            if (ek == EK_EQ)
                exprRes = newExprBinop(tree, EK_LOGNOT, typeRet, exprRes, NULL);
            exprRes = newExprBinop(tree, ek == EK_EQ ? EK_LOGOR : EK_LOGAND, typeRet, exprRes, call);
            return BindNubAddTmps(tree, exprRes, nin);
        }

        // One of them is not nullable so generate: tmp1.HasValue && op_Equal(tmp1.Value, tmp2)
        if (ek == EK_NE)
            nin.exprCnd = newExprBinop(tree, EK_LOGNOT, typeRet, nin.exprCnd, NULL);
        exprRes = newExprBinop(tree, ek == EK_NE ? EK_LOGOR : EK_LOGAND, typeRet, nin.exprCnd, call);
        return BindNubAddTmps(tree, exprRes, nin);
    }

    if (fRelOp) {
        ASSERT(typeRet->isPredefType(PT_BOOL) &&
            (ek == EK_GT || ek == EK_GE || ek == EK_LT || ek == EK_LE));
        // These ones don't lift the return type. Instead, if either side
        // is null, the result is false.
        return BindNubOpRes(tree, typeRet, call, newExprZero(tree, typeRet), nin);
    }

    if (!typeRet->isNUBSYM())
        typeRet = compiler()->getBSymmgr().GetNubType(typeRet);

    return BindNubOpRes(tree, typeRet->asNUBSYM(), typeRet, call, nin, true);
}


// Determine best method for overload resolution. Returns NULL if no best method, in which
// case two tying methods are returned for error reporting.
METHLIST * FUNCBREC::FindBestMethod(METHLIST * list, ArgInfos * args, METHLIST ** methAmbig1, METHLIST ** methAmbig2)
{
    ASSERT(list && list->mpwi);

    // select the best method:
    /*
    Effectively, we pick the best item from a set using a non-transitive ranking function
    So, pick the first item (candidate) and compare against next (contender), if there is
        no next, goto phase 2
    If first is better, move to next contender, if none proceed to phase 2
    If second is better, make the contender the candidate and make the item following
        contender into the new contender, if there is none, goto phase 2
    If neither, make contender+1 into candidate and contender+2 into contender, if possible,
        otherwise, if contender was last, return null, otherwise if new condidate is last,
        goto phase 2
    Phase 2: compare all items before candidate to candidate
        If candidate always better, return it, otherwise return null

    */
    // Record two method that are ambiguous for error reporting.
    METHLIST * ambig1 = NULL;
    METHLIST * ambig2 = NULL;

    METHLIST * candidate = list;
    for (METHLIST * contender = list->next; contender; contender = contender->next) {
        ASSERT(candidate != contender);

        int result = WhichMethodIsBetter(candidate, contender, args);
        if (result > 0)  {
            continue;  // (meaning m1 is better...)
        }

        // so m2 is better, or a tie...
        if (!result) {
            // in case of tie we don't want to bother with the contender who tied...
            ambig1 = candidate;
            ambig2 = contender;
            contender = candidate = contender->next;
            if (!candidate) goto AMBIG;
        } else {
            candidate = contender;
        }
    }

    // Now, compare the candidate with items previous to it...
    for (METHLIST * contender = list; contender; contender = contender->next)
    {
        if (contender == candidate) {
            // We hit our winner, so its good enough...
            return candidate;
        }
        int result = WhichMethodIsBetter(contender, candidate, args);
        if (result < 0) { // meaning m2 is better
            continue;
        } else if (result == 0) {
            ambig1 = candidate;
            ambig2 = contender;
        }
        break;
    }

AMBIG:
    // an ambig call. Return two of the ambiguous set.
    if (ambig1 && ambig2) {
        *methAmbig1 = ambig1; *methAmbig2 = ambig2;
    }
    else {
        // For some reason, we have an ambiguity but never had a tie.
        // This can easily happen in a circular graph of candidate methods.
        *methAmbig1 = list; *methAmbig2 = list->next;
    }

    return NULL;
}


TypeArray * FUNCBREC::GetExpandedParams(TypeArray * params, int count)
{
    ASSERT(count >= params->size - 1);

    TYPESYM ** prgtype = STACK_ALLOC(TYPESYM *, count);
    params->CopyItems(0, params->size - 1, prgtype);

    TYPESYM * type = params->Item(params->size - 1)->asARRAYSYM()->elementType();

    for (int itype = params->size - 1; itype < count; itype++) {
        prgtype[itype] = type;
    }

    return compiler()->getBSymmgr().AllocParams(count, prgtype);
}


// Is the method/property callable. Not if it's an override or not user-callable.
bool FUNCBREC::isMethPropCallable(METHPROPSYM * sym, bool requireUC)
{
    // The hide-by-name option for binding other languages takes precedence over general
    // rules of not binding to overrides.
    return (!sym->isOverride || sym->isHideByName) && (!requireUC || sym->isUserCallable());
}


// For a base call we need to remap from the virtual to the specific override to invoke.
// This is also used to map a virtual on object (like ToString) to the specific override
// when the object is a simple type (int, bool, char, etc). In these cases it is safe to
// assume that any override won't later be removed....
// We start searching from "typeObj" up the superclass hierarchy until we find a method
// with an exact signature match.
void FUNCBREC::RemapToOverride(SymWithType * pswt, TYPESYM * typeObj)
{
    // For a property/indexer we remap the accessors, not the property/indexer.
    // Since every event has both accessors we remap the event instead of the accessors.
    ASSERT(pswt && (pswt->Sym()->isMETHSYM() || pswt->Sym()->isEVENTSYM()));
    ASSERT(typeObj);

    // Don't remap static or interface methods.
    if (typeObj->isNUBSYM()) {
        typeObj = typeObj->asNUBSYM()->GetAts();
        if (!typeObj) {
            VSFAIL("Why did GetAts return null?");
            return;
        }
    }
    else if (!typeObj->isAGGTYPESYM() || typeObj->isInterfaceType())
        return;

    ASSERT(typeObj->isAGGTYPESYM());

    // Don't remap non-virtual methods or events.
    if (!pswt->Sym()->IsVirtual())
        return;

    symbmask_t mask = pswt->Sym()->mask();

    AGGTYPESYM * atsObj = typeObj->asAGGTYPESYM();

    while (atsObj && atsObj->getAggregate() != pswt->Sym()->parent) {
        for (SYM * symT = compiler()->getBSymmgr().LookupAggMember(pswt->Sym()->name, atsObj->getAggregate(), mask);
            symT;
            symT = compiler()->getBSymmgr().LookupNextSym(symT, atsObj->getAggregate(), mask))
        {
            if (symT->IsOverride() && (symT->SymBaseVirtual() == pswt->Sym() || symT->SymBaseVirtual() == pswt->Sym()->SymBaseVirtual())) {
                pswt->Set(symT, atsObj);
                return;
            }
        }
        atsObj = atsObj->GetBaseClass();
    }
    ASSERT(atsObj && (!pswt->Type() || atsObj == pswt->Type()));
}


EXPRBLOCK * FUNCBREC::newExprBlock(BASENODE * tree)
{
    EXPRBLOCK * block = newExpr(tree, EK_BLOCK, NULL)->asBLOCK();
    block->owningBlock = pCurrentBlock;

    return block;
}


EXPRCONSTANT * FUNCBREC::newExprConstant(BASENODE * tree, TYPESYM * type, CONSTVAL cv)
{
    EXPRCONSTANT * rval = newExpr(tree, EK_CONSTANT, type)->asCONSTANT();
    rval->list = NULL;
    rval->pList = &(rval->list);
    rval->getSVal().init = cv.init; // Always big enough for everything

    return rval;
}

// Create an error node
EXPRERROR * FUNCBREC::newError(BASENODE * tree, TYPESYM * type)
{
    EXPRERROR * expr;
    ASSERT(tree != NULL);
    if (!type || type->IsNeverSameType())
        type = compiler()->getBSymmgr().GetErrorSym();
    expr = newExpr(tree, EK_ERROR, type)->asERROR();
    return expr;
}

// Create a delimiter expr
EXPRDELIM * FUNCBREC::newExprDelim(BASENODE *tree, DELIMKIND delim)
{
    EXPRDELIM * expr;
    ASSERT(tree != NULL);
    expr = newExpr(tree, EK_DELIM, NULL)->asDELIM();
    expr->delim = delim;
    return expr;
}


EXPRMEMGRP * FUNCBREC::newExprMemGrp(BASENODE * tree, BASENODE * nodeName, MemberLookup & mem)
{
    ASSERT(!mem.FError());

    EXPRMEMGRP * grp = newExpr(tree, EK_MEMGRP, compiler()->getBSymmgr().GetMethGrpType())->asMEMGRP();
    grp->nodeName = nodeName;

    mem.FillGroup(grp);

    return grp;
}


// Create a generic node...
EXPR * FUNCBREC::newExprCore(PBASENODE tree, EXPRKIND kindReal, TYPESYM * type, int flags, EXPRKIND kindAlloc)
{
    ASSERT(compiler()->CompPhase() >= CompilerPhase::EvalConstants);

    // in debug, allocate through new so that we get a vtable for the debugger
    // else, just allocate enough space and use that...
#if DEBUG
    EXPR * rval;
    switch (kindAlloc) {
#define EXPRDEF(e) case EK_##e: rval = new (allocator) EXPR##e; break;
#include "exprkind.h"
        default:    ASSERT(!"bad kind passed to newExpr"); rval = new (allocator) EXPR;
    }

#else // debug
    unsigned size;

    switch (kindAlloc) {
#define EXPRDEF(e) case EK_##e: size = sizeof (EXPR##e); break;
#include "exprkind.h"
        default:    ASSERT(!"bad kind passed to newExpr"); size = sizeof(EXPR);
    }
    EXPR * rval = (EXPR*) allocator->AllocZero(size);
#endif // debug


    rval->kind = kindReal;
    rval->setType(type);
    rval->tree = tree;
    rval->flags = flags;

    return rval;
}


EXPRSTMTAS * FUNCBREC::MakeStmt(BASENODE * tree, EXPR * expr, int flags)
{
    ASSERT(expr && expr->kind != EK_STMTAS);

    EXPRSTMTAS * stmt = newExpr(tree, EK_STMTAS, NULL)->asSTMTAS();
    stmt->expression = expr;
    stmt->flags |= flags;
    return stmt;
}

EXPRWRAP * FUNCBREC::newExprWrap(EXPR * wrap, TEMP_KIND tempKind)
{
    EXPRWRAP * rval = newExpr(EK_WRAP)->asWRAP();
    rval->asWRAP()->expr = wrap;
    if (wrap) {
        rval->setType(wrap->type);
    }
    rval->flags |= EXF_LVALUE;
    rval->tempKind = tempKind;
    rval->containingScope = pCurrentScope;
    return rval;
}

// Create a binop node...
EXPRBINOP * FUNCBREC::newExprBinop(PBASENODE tree, EXPRKIND kind, TYPESYM *type, EXPR *p1, EXPR *p2)
{
    EXPRBINOP * rval = (EXPRBINOP*) newExprCore(tree, kind, type, EXF_BINOP, EK_BINOP);
    rval->p1 = p1;
    rval->p2 = p2;
    ASSERT(rval->kind == kind);
    ASSERT(rval->flags & EXF_BINOP);

    return rval->asBIN();
}

// create a new label expr statment...
EXPRLABEL * FUNCBREC::newExprLabel()
{
    // The second part of this assert checks that pCB is not garbage by dereferencing it...
    ASSERT(pCurrentBlock && !pCurrentBlock->type);
    ASSERT(pCurrentScope && pCurrentScope->parent);

    EXPRLABEL * rval = newExpr(NULL, EK_LABEL, NULL)->asLABEL();
    rval->scope = pCurrentScope;
    return rval;
}

// This returns a null for reference types and an EXPRZEROINIT for all others.
EXPR * FUNCBREC::newExprZero(BASENODE * tree, TYPESYM * type)
{
    CONSTVAL cv;

    switch(type->fundType()) {
    default:
        ASSERT(0);
        return NULL;

    case FT_PTR:
        {
            EXPR * expr;
            bindSimpleCast(tree, bindNull(NULL), type, &expr);
            return expr;
        }

    case FT_REF:
    case FT_I1:
    case FT_U1:
    case FT_I2:
    case FT_U2:
    case FT_I4:
    case FT_U4:
        return newExprConstant(tree, type, ConstValInit());

    case FT_I8:
    case FT_U8:
    case FT_R4:
    case FT_R8:
        cv.doubleVal = (double *)allocator->AllocZero(sizeof(double));
        return newExprConstant(tree, type, cv);

    case FT_STRUCT:
        if (type->isPredefType(PT_DECIMAL)) {
            cv.decVal = (DECIMAL *)allocator->AllocZero(sizeof(DECIMAL));
            return newExprConstant(tree, type, cv);
        }
        break;

    case FT_VAR:
        break;
    }

    EXPRZEROINIT * rval = newExpr(tree, EK_ZEROINIT, type)->asZEROINIT();
    ASSERT(!rval->p1);
    return rval;
}


// Concatenate an item onto a list
void FUNCBREC::newList(EXPR * newItem, EXPR *** list)
{
    ASSERT(list != NULL);
    ASSERT(*list != NULL);

    if (!newItem) return;

    if (**list == NULL) {
        **list = newItem;
        return;
    }
    EXPRBINOP * temp = newExprBinop(
        NULL,
        EK_LIST,
        getVoidType(),
        (**list),
        newItem);
    (**list) = temp;
    (*list) = &(temp->p2);
}


EXPRSWITCH * FUNCBREC::MakeIteratorSwitch(SCOPESYM *pScope, IteratorRewriteInfo * irwInfo, EXPRLABEL * pKey, EXPRLABEL * pNoKey)
{
    int labelCount = 0;
    bool fIsNested = (pCurrentScope != pOuterScope);
    FOREACHCHILD( pScope, pSym)
        if (!pSym->isLABELSYM() || pSym->asLABELSYM()->labelExpr->kind != EK_SWITCHLABEL)
            continue;

        if (!fIsNested || !pSym->asLABELSYM()->labelExpr->asSWITCHLABEL()->statements)
            labelCount++;

        if (fIsNested && !pSym->asLABELSYM()->labelExpr->asSWITCHLABEL()->statements &&
            pSym->asLABELSYM()->labelExpr->asSWITCHLABEL()->key)
        {
            // This is an unbound 'finally label'
            // Thats says for this case we need to jump into an inner try-block
            // Create an unbound 'finally' label so the outer finally scope knows to jump into
            // us for each of these values too
            EXPRSWITCHLABEL* pOuterLab;
            pOuterLab = MakeSwitchLabel(pSym->asLABELSYM()->labelExpr->asSWITCHLABEL()->key->asCONSTANT(), pCurrentScope);
        }
    ENDFOREACHCHILD;

    EXPRSWITCH * pSwitch = newExpr(NULL, EK_SWITCH, NULL)->asSWITCH();
    pSwitch->hashtableToken = 0;
    pSwitch->breakLabel = pSwitch->nullLabel = NULL;
    pSwitch->arg = irwInfo->exprState;
    pSwitch->flags |= EXF_HASDEFAULT | EXF_NODEBUGINFO;
    EXPRSWITCHLABEL ** labArray = (EXPRSWITCHLABEL**) allocator->Alloc(sizeof(EXPRSWITCHLABEL*) * labelCount);
    pSwitch->labels = labArray;
    pSwitch->bodies = NULL;
    StmtListBldr bldrLab((EXPRSTMT **)&pSwitch->bodies);
    FOREACHCHILD( pScope, pSym)
        if (!pSym->isLABELSYM() || pSym->asLABELSYM()->labelExpr->kind != EK_SWITCHLABEL)
            continue;
        if (!fIsNested || !pSym->asLABELSYM()->labelExpr->asSWITCHLABEL()->statements) {
            *labArray++ = pSym->asLABELSYM()->labelExpr->asSWITCHLABEL();
            if (!pSym->asLABELSYM()->labelExpr->asSWITCHLABEL()->statements) {
                if (pSym->asLABELSYM()->labelExpr->asSWITCHLABEL()->key)
                    pSym->asLABELSYM()->labelExpr->asSWITCHLABEL()->statements = MakeGoto(NULL, pKey, EXF_NODEBUGINFO);
                else
                    pSym->asLABELSYM()->labelExpr->asSWITCHLABEL()->statements = MakeGoto(NULL, pNoKey, EXF_NODEBUGINFO);
            }
            bldrLab.Add(pSym->asLABELSYM()->labelExpr);
        }
    ENDFOREACHCHILD;
    qsort(pSwitch->labels, labelCount, sizeof(EXPR*), &FUNCBREC::compareSwitchLabels);
    pSwitch->labelCount = labelCount;

    return pSwitch;
}

EXPRSWITCHLABEL * FUNCBREC::MakeSwitchLabel( EXPRCONSTANT * pKey, SCOPESYM * pScope)
{
    NAME * pName = getSwitchLabelName(pKey);
    LABELSYM * sym = compiler()->getLSymmgr().CreateLocalSym(SK_LABELSYM, pName, pScope)->asLABELSYM();
    EXPRSWITCHLABEL *pLab = newExpr(NULL, EK_SWITCHLABEL, NULL)->asSWITCHLABEL();
    pLab->key = pKey;
    pLab->label = sym;
    sym->labelExpr = pLab;
    return pLab;
}

// ExprFactory routines copied from MSR (todd proebstring et.al.)
EXPR * FUNCBREC::MakeFieldAccess(EXPR* pObject , NAME* pName, int flags)
{
    // Return an expression which references a field (1st overload).
    ASSERT(pName);
    ASSERT(pObject);
    AGGTYPESYM * ats = pObject->type->asAGGTYPESYM();

    MemberLookup mem;

    if (!mem.Lookup(compiler(), ats, pObject, parentDecl, pName, 0, MemLookFlags::UserCallable) || !mem.SymFirst()->isMEMBVARSYM()) {
        return NULL;
    }
    ASSERT(mem.TypeCount() == 1);

    FieldWithType fwt = mem.SwtFirst();

    EXPRFIELD* pExpr = newExpr(NULL, EK_FIELD, fwt.Field()->type)->asFIELD();

    DebugOnly(pExpr->fCheckedMarshalByRef = true);

    pExpr->fwt = fwt;
    pExpr->object = pObject;
    pExpr->flags = flags;

    int jbitPar;
    if (pObject && (jbitPar = pObject->getOffset()) && ats->isStructType())
        pExpr->asFIELD()->offset = jbitPar + FlowChecker::GetIbit(compiler(), fwt);
    else
        pExpr->asFIELD()->offset = 0;

    return pExpr;
}

EXPR * FUNCBREC::MakeFieldAccess(EXPR* pObject, MEMBVARSYM * pSym, int flags)
{
    ASSERT(pObject && pObject->type->GetNakedAgg() == pSym->getClass());

    AGGTYPESYM * ats = pObject->type->GetNakedType()->asAGGTYPESYM();

    // Return an expression which references a field.
    EXPRFIELD* pExpr = newExpr(NULL, EK_FIELD, compiler()->getBSymmgr().SubstType(pSym->type, ats, NULL))->asFIELD();

    DebugOnly(pExpr->fCheckedMarshalByRef = true);

    pExpr->fwt.Set(pSym, ats);
    pExpr->object = pObject;
    pExpr->flags = flags;

    int jbitPar;
    if (pObject && (jbitPar = pObject->getOffset()) && pObject->type->isStructType())
        pExpr->asFIELD()->offset = jbitPar + FlowChecker::GetIbit(compiler(), pExpr->fwt);
    else
        pExpr->asFIELD()->offset = 0;

    return pExpr;
}

EXPRRETURN * FUNCBREC::MakeReturn(SCOPESYM* pCurrentScope, EXPR* pValue, int flags) {
    ASSERT(pValue);

    EXPRRETURN* pReturn = newExpr(NULL, EK_RETURN, NULL)->asRETURN();
    pReturn->object = pValue;
    pReturn->currentScope = pCurrentScope;
    pReturn->flags |= flags;

    return pReturn;
}

EXPRLABEL * FUNCBREC::MakeFreshLabel()
{
    EXPRLABEL* pExpr = newExpr(NULL, EK_LABEL, NULL)->asLABEL();
    return pExpr;
}

EXPRSTMT * FUNCBREC::MakeAssignment(BASENODE * tree, EXPR * exprLeft, EXPR * exprRight)
{
    // Make an assignment expression.
    EXPR * expr = newExprBinop(tree, EK_ASSG, exprLeft->type, exprLeft, exprRight);
    expr->flags |= EXF_ASSGOP;
    return MakeStmt(tree, expr);
}

EXPRLOCAL * FUNCBREC::MakeLocal(BASENODE * tree, LOCVARSYM * loc, bool fLVal)
{
    EXPRLOCAL * expr = newExpr(tree, EK_LOCAL, loc->type)->asLOCAL();
    expr->local = loc;
    if (fLVal)
        expr->flags |= EXF_LVALUE;
    return expr;
}

EXPRGOTOIF * FUNCBREC::MakeGotoIf(BASENODE * tree, EXPR * exprCond, EXPRLABEL * lab, bool fSense, int flags)
{
    // Make a conditional goto statement.
    EXPRGOTOIF * gtif = newExpr(tree, EK_GOTOIF, NULL)->asGOTOIF();

    gtif->label = lab;
    gtif->condition = exprCond;
    gtif->sense = fSense;
    gtif->flags |= flags;

    return gtif;
}

EXPRGOTO * FUNCBREC::MakeGoto(BASENODE * tree, EXPRLABEL * lab, int flags)
{
    // Make an unconditional goto statement.
    EXPRGOTO * gt = newExpr(tree, EK_GOTO, NULL)->asGOTO();

    gt->label = lab;
    gt->flags |= flags;

    return gt;
}

bool EXPR::hasSideEffects(COMPILER *compiler)
{
    EXPR * object;

    if (flags & (EXF_ASSGOP | EXF_CHECKOVERFLOW)) {
        return true;
    }
    if (flags & EXF_BINOP) {
        return
            asBIN()->p1->hasSideEffects(compiler) ||
            asBIN()->p2 && asBIN()->p2->hasSideEffects(compiler);
    }

    switch (kind) {
    // Always true
    case EK_PROP:
    case EK_CONCAT:
    case EK_CALL:
    case EK_ERROR:
        return true;

    // Always false
    case EK_DELIM:
    case EK_NOOP:
    case EK_LOCAL:
    case EK_CONSTANT:
    case EK_FUNCPTR:
    case EK_TYPEOF:
    case EK_SIZEOF:
    case EK_LDTMP:
    case EK_ANONMETH:
    case EK_MEMGRP:
        return false;

    case EK_ZEROINIT:
        object = asZEROINIT()->p1;
        return object && (object->kind != EK_LOCAL || object->asLOCAL()->local->slot.type);
    case EK_ARRINIT:
        return asARRINIT()->args && asARRINIT()->args->hasSideEffects(compiler);
    case EK_FIELD:
        object = asFIELD()->object;
        // a static field has the sideeffect of executing the cctor
        // Volatile fields always have side-effects
        if (!object || asFIELD()->fwt.Field()->isVolatile)
            return true;
        ASSERT(compiler != NULL);
        if (object->type->fundType() == FT_REF && !compiler->funcBRec.isThisPointer(object))
            return true;
        return object->hasSideEffects( compiler);
    case EK_WRAP:
        return asWRAP()->expr->hasSideEffects(compiler);
    case EK_CAST:
        if (flags & (EXF_BOX | EXF_UNBOX | EXF_FORCE_UNBOX | EXF_REFCHECK | EXF_CHECKOVERFLOW))
            return true;
        return asCAST()->p1->hasSideEffects(compiler);
    case EK_DBLQMARK:
        return
            asDBLQMARK()->exprTest->hasSideEffects(compiler) ||
            asDBLQMARK()->exprConv->hasSideEffects(compiler) ||
            asDBLQMARK()->exprElse->hasSideEffects(compiler);
    default:
        ASSERT(!"bad expr");
        return true;
    }
}


/***************************************************************************************************
    Scan through EK_SEQUENCE and EK_SEQREV exprs to get the real value.
***************************************************************************************************/
EXPR * EXPR::GetSeqVal()
{
    // Scan through EK_SEQUENCE and EK_SEQREV exprs to get the real value.
    if (!this)
        return NULL;

    EXPR * exprVal = this;
    for (;;) {
        switch (exprVal->kind) {
        default:
            return exprVal;
        case EK_SEQUENCE:
            exprVal = exprVal->asBIN()->p2;
            break;
        case EK_SEQREV:
            exprVal = exprVal->asBIN()->p1;
            break;
        }
    }
}


/***************************************************************************************************
    Determine whether this expr has a constant value (EK_CONSTANT or EK_ZEROINIT), possibly with
    side effects (via EK_SEQUENCE or EK_SEQREV). Returns NULL if not, or the constant expr if so.
    The returned EXPR will always be an EK_CONSTANT or EK_ZEROINIT.
***************************************************************************************************/
EXPR * EXPR::GetConst()
{
    EXPR * exprVal = GetSeqVal();
    if (!exprVal || exprVal->kind != EK_CONSTANT && exprVal->kind != EK_ZEROINIT)
        return NULL;
    return exprVal;
}


LOCSLOTINFO * FUNCBREC::getThisPointerSlot()
{
    if (thisPointer) {
        return &(thisPointer->slot);
    } else {
        return NULL;
    }
}

__forceinline NAME * FUNCBREC::ekName(EXPRKIND ek)
{
    ASSERT(ek >= EK_FIRSTOP && (ek - EK_FIRSTOP) < (int)lengthof(EK2NAME));
    return compiler()->namemgr->GetPredefName(FetchAtIndex(EK2NAME, ek - EK_FIRSTOP));
}


const EXPRKIND FUNCBREC::OP2EK[] = {
#define OP(n,p,a,stmt,t,pn,e) (EXPRKIND) (e),
#include "ops.h"
#undef OP
};

const BOOL FUNCBREC::opCanBeStatement[] = {
#define OP(n,p,a,stmt,t,pn,e) stmt,
#include "ops.h"
#undef OP
};

const PREDEFNAME FUNCBREC::EK2NAME[] = {

    PN_OPEQUALS,
    PN_OPCOMPARE,

    PN_OPTRUE,
    PN_OPFALSE,

    PN_OPINCREMENT,
    PN_OPDECREMENT,

    PN_OPNEGATION,

    PN_OPEQUALITY,
    PN_OPINEQUALITY,
    PN_OPLESSTHAN,
    PN_OPLESSTHANOREQUAL,
    PN_OPGREATERTHAN,
    PN_OPGREATERTHANOREQUAL,

    PN_OPPLUS,
    PN_OPMINUS,
    PN_OPMULTIPLY,
    PN_OPDIVISION,
    PN_OPMODULUS,
    PN_OPUNARYMINUS,
    PN_OPUNARYPLUS,

    PN_OPBITWISEAND,
    PN_OPBITWISEOR,
    PN_OPXOR,
    PN_OPCOMPLEMENT,

    PN_OPLEFTSHIFT,
    PN_OPRIGHTSHIFT,
};

NAME * FUNCBREC::CreateName(PCWSTR szTemplate, ...)
{
    WCHAR sz[MAX_FULLNAME_SIZE];
    HRESULT hr;
    va_list args;

    va_start(args, szTemplate);
    hr = StringCchVPrintfW( sz, lengthof(sz), szTemplate, args);
    va_end(args);

    // We shouldn't be tacking on so much goop that it overflows the full-name
    // size allowed, assuming that the user-parts are all <= MAX_IDENT_SIZE
    ASSERT(SUCCEEDED(hr));
    return compiler()->namemgr->AddString(sz);
}

SpecialNameKindEnum DecodeSpecialNameChar(WCHAR ch)
{
    SpecialNameKindEnum rval = SpecialNameKind::None;
    if (ch >= L'1' && ch <= L'9') {
        rval = (SpecialNameKindEnum) (SpecialNameKind::None + (ch - L'1') + 1);
    } else if (ch >= L'a' && ch <= L'f') {
        rval = (SpecialNameKindEnum) (SpecialNameKind::None + (ch - L'a') + 10);
    }
    if (rval < SpecialNameKind::None || rval > SpecialNameKind::LastKind) {
        rval = SpecialNameKind::None;
    }
    return rval;
}


SpecialNameKindEnum FUNCBREC::IsSpecialName(NAME * name)
{
    if (name->text[0] != L'<') return SpecialNameKind::None;
    const WCHAR * ptr = name->text + 1;
    while (*ptr != L'>' && *ptr != 0) {
        ptr ++;
    }
    if (*ptr != L'>') return SpecialNameKind::None;
    return DecodeSpecialNameChar(*(ptr + 1));

}

NAME * FUNCBREC::GetDisplayNameOfPossibleSpecialName(NAME * specialName)
{
    if (specialName->text[0] != L'<') return specialName;
    const WCHAR * ptr = specialName->text + 1;
    const WCHAR * endPtr = ptr;
    while (*endPtr != L'>' && *endPtr != 0) {
        endPtr ++;
    }
    if (endPtr == ptr) return NULL;
    if (*endPtr != L'>') return specialName;
    return compiler()->getNamemgr()->AddString(ptr, (int)(endPtr - ptr));
}


NAME * FUNCBREC::CreateSpecialName( SpecialNameKindEnum snk, PCWSTR szName)
{
    ASSERT(snk > 0 && snk < 16);
    PCWSTR szInvisibleSuffix = L"";

    switch (snk) {
    case SpecialNameKind::HoistedIteratorLocal:
    case SpecialNameKind::HoistedThis:
    case SpecialNameKind::SavedParamOrThis:
        ASSERT(szName != NULL);
        break;

    case SpecialNameKind::OuterscopeLocals:
        // Move the name to the invisible part since it's not really
        // a user-provided name anyway
        ASSERT(szName != NULL);
        szInvisibleSuffix = szName;
        szName = L"";
        break;

    case SpecialNameKind::HoistedWrap:
        ASSERT(szName == NULL);
        szName = L"";
        szInvisibleSuffix = L"wrap";
        break;

    // Locals
    case SpecialNameKind::DisplayClassInstance:
        ASSERT(szName == NULL);
        szName = L"";
        szInvisibleSuffix = L"locals";
        break;

    case SpecialNameKind::IteratorInstance:
        ASSERT(szName == NULL);
        szName = L"";
        szInvisibleSuffix = L"iterator";
        break;

    case SpecialNameKind::CachedDelegateInstance:
        ASSERT(szName == NULL);
        szName = L"";
        szInvisibleSuffix = L"CachedAnonymousMethodDelegate";
        break;

    // Methods
    case SpecialNameKind::AnonymousMethod:
        ASSERT(szName != NULL);
        break;

    // Types
    case SpecialNameKind::AnonymousMethodDisplayClass:
        ASSERT(szName == NULL);
        szName = L"";
        szInvisibleSuffix = L"DisplayClass";
        break;

    case SpecialNameKind::IteratorClass:
        ASSERT(szName != NULL);
        break;

    case SpecialNameKind::FixedBufferStruct:
        ASSERT(szName != NULL);
        szInvisibleSuffix = L"FixedBuffer";
        break;

    case SpecialNameKind::IteratorState:
    case SpecialNameKind::CurrentField:
        // These should never come through here!
    default:
        VSFAIL("Unhandled SpecialNameKind!!!!");
        break;
    }

    return CreateName( L"<%s>%x__%s%x", szName, (int)snk, szInvisibleSuffix, uniqueNameIndex++);
}



void FUNCBREC::FncBindState::Save(FUNCBREC * fnc)
{
    ASSERT(fnc);

    this->fnc = fnc;
    meth = fnc->pMSym;
    field = fnc->pFSym;
    tree = fnc->pTree;
    aggPar = fnc->parentAgg;
    declPar = fnc->parentDecl;
    btfFlags = fnc->btfFlags;
    infoCls = fnc->pClassInfo;
    scopeOuter = fnc->pOuterScope;
}


void FUNCBREC::FncBindState::Restore()
{
    if (!fnc)
        return;

    fnc->pMSym = meth;
    fnc->pFSym = field;
    fnc->pTree = tree;
    fnc->parentAgg = aggPar;
    fnc->parentDecl = declPar;
    fnc->btfFlags = btfFlags;
    fnc->pClassInfo = infoCls;
    fnc->pOuterScope = scopeOuter;

    fnc = NULL;
}


void FUNCBREC::DiscardLocalState()
{
    pOuterScope = NULL; // the argument scope
    pCurrentScope = NULL; // the current scope
    pFinallyScope = NULL; // the scope of the innermost finally, or pOuterScope if none...
    pTryScope = NULL; // the scope of the innermost try, or pOuterScope if none...
    pSwitchScope = NULL; // the scope of the innermost switch, or NULL if none
    pCatchScope = NULL; // the scope of the innermose catch, or NULL if none
    pCurrentBlock = NULL;
    exprSwitchCur = NULL;
    thisPointer = NULL;

    outerThisPointer = NULL; // ONLY used by anonymous methods, keeps a <this> pointer for the outermost containing method.
    userLabelList = NULL;
    pUserLabelList = NULL;
    gotos = NULL;

}

