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
// File: dumpnode.h
//
// ===========================================================================

// NOTE:  This is actually an implementation file, it is just included into
// SRCMOD.CPP to keep it from making the file too big.  Debug only.

#define NODEKIND(k,t,g,p) "NK_" #k,
PCSTR rgszKind[] = {
#include "nodekind.h"
};

#define OP(n,p,a,stmt,t,pn,e) "OP_" #n,
PCSTR rgszOpName[] = {
#include "ops.h"
};

// make an indentation string
PSTR Spaces (int iIndent)
{
    static  char szBuf[256];
    size_t cch = SizeMul(iIndent, 2);
    if (cch >= lengthof(szBuf))
        cch = lengthof(szBuf) - 1;
    memset(szBuf, ' ', cch);
    szBuf[cch] = 0;
    return szBuf;
}

void Heading (PCSTR psz, int iIndent)
{
    printf ("%s%s:\n", Spaces(iIndent), psz);
}

#define HEADING(t) Heading (t, iIndent)
#define DUMPNEXT(p) \
            if (p != NULL) \
            { \
                printf ("%s----------\n", Spaces(iIndent)); \
                iIndent--; \
                DumpTree (p); \
                iIndent++; \
            }


void DumpTree (BASENODE *pNode)
{
    static  int iIndent = -1;

    if (pNode == NULL)
    {
        printf ("%s<null>\n", Spaces(iIndent+1));
        return;
    }

    iIndent++;

    // general node data                                                              
    printf ("%s%06X: %s (tokidx %ld)\n", Spaces(iIndent), pNode->flags, rgszKind[pNode->kind], pNode->tokidx);

    // specifics
    switch (pNode->kind)
    {
        case NK_ACCESSOR          :    // ACCESSOR
            DumpTree (pNode->asACCESSOR()->pBody);
            break;

        case NK_ANONBLOCK         :    // ANONBLOCK
            HEADING ("PARAMETERS");
            DumpTree (pNode->asANONBLOCK()->pArgs);
            HEADING ("BODY");
            DumpTree (pNode->asANONBLOCK()->pBody);
            break;

        case NK_ARRAYINIT         :    // UNOP
            DumpTree (pNode->asANYUNOP()->p1);
            break;

        case NK_ATTR              :    // ATTR
        case NK_ATTRARG           :    // ATTR
            HEADING ("NAME");
            DumpTree (pNode->asANYATTR()->pName);
            HEADING (pNode->kind == NK_ATTR ? "ARGS" : "EXPR");
            DumpTree (pNode->asANYATTR()->pArgs);
            break;

        case NK_CALL:
        case NK_DEREF:
        case NK_BINOP             :    // BINOP
            printf ("%sOPERATOR: %s\n", Spaces(iIndent), rgszOpName[pNode->Op()]);
            HEADING ("LEFT");
            DumpTree (pNode->asANYBINOP()->p1);
            HEADING ("RIGHT");
            DumpTree (pNode->asANYBINOP()->p2);
            break;

        case NK_BLOCK             :    // BLOCK
            DumpTree (pNode->asBLOCK()->pStatements);
            break;

        case NK_BREAK             :    // EXPRSTMT
        case NK_CONTINUE          :    // EXPRSTMT
        case NK_EXPRSTMT          :    // EXPRSTMT
        case NK_GOTO              :    // EXPRSTMT
        case NK_RETURN            :    // EXPRSTMT
        case NK_THROW             :    // EXPRSTMT
            DumpTree (pNode->asANYEXPRSTMT()->pArg);       // NOTE:  Can't use as*() here, because of different kinds...
            break;

        case NK_CASE              :    // BASE
            HEADING ("LABELS");
            DumpTree (pNode->asCASE()->pLabels);
            HEADING ("STATEMENTS");
            DumpTree (pNode->asCASE()->pStmts);
            break;

        case NK_CASELABEL         :    // UNOP
            DumpTree (pNode->asCASELABEL()->p1);
            break;

        case NK_CATCH             :    // CATCH
            HEADING ("TYPE");
            DumpTree (pNode->asCATCH()->pType);
            HEADING ("NAME");
            DumpTree (pNode->asCATCH()->pName);
            HEADING ("BLOCK");
            DumpTree (pNode->asCATCH()->pBlock);
            break;

        case NK_CONST             :    // CONST
            HEADING ("ATTR");
            DumpTree (pNode->asCONST()->pAttr);
            HEADING ("TYPE");
            DumpTree (pNode->asCONST()->pType);
            HEADING ("DECLARED");
            DumpTree (pNode->asCONST()->pDecls);
            break;

        case NK_CLASS             :    // CLASS
        case NK_ENUM              :    // CLASS
        case NK_INTERFACE         :    // CLASS
        case NK_STRUCT            :    // CLASS
            HEADING ("ATTRIBUTES");
            DumpTree (pNode->asAGGREGATE()->pAttr);
            HEADING ("NAME");
            DumpTree (pNode->asAGGREGATE()->pName);
            if (pNode->asAGGREGATE()->pTypeParams)
            {
                HEADING ("TYPEPARAMS");
                DumpTree (pNode->asAGGREGATE()->pTypeParams);
            }
            HEADING ("BASES");
            DumpTree (pNode->asAGGREGATE()->pBases);
            if (pNode->asAGGREGATE()->pConstraints)
            {
                HEADING ("CONSTRAINTS");
                DumpTree (pNode->asAGGREGATE()->pConstraints);
            }
            HEADING ("MEMBERS");
            DumpTree (pNode->asAGGREGATE()->pMembers);
            break;

        case NK_CONSTVAL          :    // CONSTVAL

            if (pNode->PredefType() == PT_STRING)
                printf ("%sPT_STRING: '%S'\n", Spaces(iIndent), pNode->asCONSTVAL()->val.strVal->text);
            if (pNode->PredefType() == PT_CHAR)
                printf ("%sPT_CHAR: '%c' (%d)\n", Spaces(iIndent), pNode->asCONSTVAL()->val.iVal, pNode->asCONSTVAL()->val.iVal);
            else if (pNode->PredefType() == PT_LONG)
                printf ("%sPT_LONG: %I64d (%016I64x)\n", Spaces(iIndent), *(pNode->asCONSTVAL()->val.longVal), *(pNode->asCONSTVAL()->val.longVal));
            else if (pNode->PredefType() == PT_INT)
                printf ("%sPT_INT: %d (%08x)\n", Spaces(iIndent), pNode->asCONSTVAL()->val.iVal, pNode->asCONSTVAL()->val.iVal);
            else if (pNode->PredefType() == PT_BYTE)
                printf ("%sPT_BYTE: %u (%02x)\n", Spaces(iIndent), pNode->asCONSTVAL()->val.uiVal, pNode->asCONSTVAL()->val.uiVal);
            else if (pNode->PredefType() == PT_SHORT)
                printf ("%sPT_SHORT: %d (%04x)\n", Spaces(iIndent), pNode->asCONSTVAL()->val.iVal, pNode->asCONSTVAL()->val.iVal);
            else if (pNode->PredefType() == PT_ULONG)
                printf ("%sPT_ULONG: %I64u (%016I64x)\n", Spaces(iIndent), *(pNode->asCONSTVAL()->val.ulongVal), *(pNode->asCONSTVAL()->val.ulongVal));
            else if (pNode->PredefType() == PT_UINT)
                printf ("%sPT_UINT: %u (%08x)\n", Spaces(iIndent), pNode->asCONSTVAL()->val.uiVal, pNode->asCONSTVAL()->val.uiVal);
            else if (pNode->PredefType() == PT_SBYTE)
                printf ("%sPT_SBYTE: %d (%02x)\n", Spaces(iIndent), pNode->asCONSTVAL()->val.iVal, pNode->asCONSTVAL()->val.iVal);
            else if (pNode->PredefType() == PT_USHORT)
                printf ("%sPT_USHORT: %u (%04x)\n", Spaces(iIndent), pNode->asCONSTVAL()->val.uiVal, pNode->asCONSTVAL()->val.uiVal);
            else if (pNode->PredefType() == PT_FLOAT)
                printf ("%sPT_FLOAT: %f\n", Spaces(iIndent), *(pNode->asCONSTVAL()->val.doubleVal));
            else if (pNode->PredefType() == PT_DOUBLE)
                printf ("%sPT_DOUBLE: %lf\n", Spaces(iIndent), *(pNode->asCONSTVAL()->val.doubleVal));
            else
                printf ("%sTYPE: %d\n", Spaces(iIndent), pNode->PredefType());
            break;

        case NK_DECLSTMT          :    // DECLSTMT
            HEADING ("TYPE");
            DumpTree (pNode->asDECLSTMT()->pType);
            HEADING ("DECLARED");
            DumpTree (pNode->asDECLSTMT()->pVars);
            break;

        case NK_DELEGATE          :    // DELEGATE
            HEADING ("TYPE");
            DumpTree (pNode->asDELEGATE()->pType);
            HEADING ("NAME");
            DumpTree (pNode->asDELEGATE()->pName);
            if (pNode->asDELEGATE()->pTypeParams)
            {
                HEADING ("TYPEPARAMS");
                DumpTree (pNode->asDELEGATE()->pTypeParams);
            }
            HEADING ("PARAMETERS");
            DumpTree (pNode->asDELEGATE()->pParms);
            if (pNode->asDELEGATE()->pConstraints)
            {
                HEADING ("CONSTRAINTS");
                DumpTree (pNode->asDELEGATE()->pConstraints);
            }
            break;

        case NK_DO                :    // LOOPSTMT
        case NK_WHILE             :    // LOOPSTMT
            HEADING ("EXPRESION");
            DumpTree (pNode->asANYLOOPSTMT()->pExpr);
            HEADING ("STATEMENT");
            DumpTree (pNode->asANYLOOPSTMT()->pStmt);
            break;

        case NK_DOT               :    // BINOP
        case NK_ARROW             :    // BINOP
            HEADING ("LEFT");
            DumpTree (pNode->asANYBINOP()->p1);
            HEADING ("RIGHT");
            DumpTree (pNode->asANYBINOP()->p2);
            break;

        case NK_EMPTYSTMT         :    // STATEMENT
            break;

        case NK_ENUMMBR           :    // ENUMMBR
            HEADING ("ATTR");
            DumpTree (pNode->asENUMMBR()->pAttr);
            HEADING ("NAME");
            DumpTree (pNode->asENUMMBR()->pName);
            HEADING ("VALUE");
            DumpTree (pNode->asENUMMBR()->pValue);
            break;

        case NK_FIELD             :    // FIELD
            HEADING ("ATTR");
            DumpTree (pNode->asFIELD()->pAttr);
            HEADING ("TYPE");
            DumpTree (pNode->asFIELD()->pType);
            HEADING ("DECLARED");
            DumpTree (pNode->asFIELD()->pDecls);
            break;

        case NK_FOR               :    // FORSTMT
            HEADING ("INIT");
            DumpTree (pNode->asFOR()->pInit);
            HEADING ("CONDITION");
            DumpTree (pNode->asFOR()->pExpr);
            HEADING ("INCREMENT");
            DumpTree (pNode->asFOR()->pInc);
            HEADING ("STATEMENT");
            DumpTree (pNode->asFOR()->pStmt);
            break;

        case NK_IF                :    // IFSTMT
            HEADING ("CONDITION");
            DumpTree (pNode->asIF()->pExpr);
            HEADING ("TRUE STATEMENT");
            DumpTree (pNode->asIF()->pStmt);
            HEADING ("FALSE STATEMENT");
            DumpTree (pNode->asIF()->pElse);
            break;

        case NK_LABEL             :    // LABELSTMT
            HEADING ("LABEL");
            DumpTree (pNode->asLABEL()->pLabel);
            HEADING ("STATEMENT");
            DumpTree (pNode->asLABEL()->pStmt);
            break;

        case NK_LIST              :    // BINOP
            HEADING ("P1");
            DumpTree (pNode->asLIST()->p1);
            HEADING ("P2");
            DumpTree (pNode->asLIST()->p2);
            break;

        case NK_MEMBER            :    // MEMBER
            ASSERT (FALSE);     // No node should use this node kind...
            break;

        case NK_CTOR              :    // METHOD
        case NK_DTOR              :    // METHOD
        case NK_METHOD            :    // METHOD
        case NK_OPERATOR          :    // OPERATOR
            HEADING ("ATTR");
            DumpTree (pNode->asANYMEMBER()->pAttr);
            if (pNode->kind == NK_METHOD)
            {
                HEADING ("NAME");
                DumpTree (pNode->asMETHOD()->pName);
            }
            if (pNode->kind == NK_OPERATOR)
            {
                printf ("%sOPERATOR: %s\n", Spaces(iIndent), rgszOpName[pNode->asOPERATOR()->iOp]);
            }
            if (pNode->kind != NK_CTOR && pNode->kind != NK_DTOR)
            {
                HEADING ("RETURN TYPE");
                DumpTree (pNode->asANYMETHOD()->pType);
            }
            HEADING ("PARAMETERS");
            DumpTree (pNode->asANYMETHOD()->pParms);
            if (pNode->kind == NK_CTOR && (pNode->other & (NFEX_CTOR_BASE|NFEX_CTOR_THIS)))
            {
                HEADING ((pNode->other & NFEX_CTOR_BASE) ? "ARGS TO BASE" : "ARGS TO THIS");
                
                if (pNode->asCTOR()->pThisBaseCall)
                    DumpTree (pNode->asCTOR()->pThisBaseCall->p2);
            }
            if (pNode->kind == NK_METHOD && pNode->asMETHOD()->pConstraints)
            {
                HEADING ("CONSTRAINTS");
                DumpTree (pNode->asMETHOD()->pConstraints);
            }

            HEADING ("BODY");
            DumpTree (pNode->asANYMETHOD()->pBody);
            break;

        case NK_NAME              :    // NAME
            printf ("%sNAME: \"%ls\"\n", Spaces(iIndent), pNode->asNAME()->pName->text);
            break;

        case NK_GENERICNAME       :    // GENERICNAME
            printf ("%sGENERICNAME: \"%ls\"\n", Spaces(iIndent), pNode->asGENERICNAME()->pName->text);
            HEADING ("TYPE PARAMETERS");
            DumpTree (pNode->asGENERICNAME()->pParams);
            break;

        case NK_OPENNAME:
            printf ("%sOPENNAME: \"%ls<", Spaces(iIndent), pNode->asOPENNAME()->pName->text);
            for (int carg = pNode->asOPENNAME()->carg; --carg > 0; )
                printf(",");
            printf(">\n");
            break;

        case NK_ALIASNAME:
            printf ("%sALIASNAME: \"%ls\"\n", Spaces(iIndent), pNode->asALIASNAME()->pName->text);
            break;

        case NK_CONSTRAINT        :    // CONSTRAINT
            HEADING("CONSTRAINT");
            DumpTree(pNode->asCONSTRAINT()->pName);
            HEADING("TYPE");
            if (pNode->flags & NF_CONSTRAINT_REFTYPE)
                printf("class ");
            else if (pNode->flags & NF_CONSTRAINT_VALTYPE)
                printf("struct ");
            DumpTree(pNode->asCONSTRAINT()->pBounds);
            if (pNode->flags & NF_CONSTRAINT_NEWABLE)
                printf("new()");
            break;

        case NK_NAMESPACE         :    // NAMESPACE
            HEADING ("NAME");
            DumpTree (pNode->asNAMESPACE()->pName);
            HEADING ("USING");
            DumpTree (pNode->asNAMESPACE()->pUsing);
            HEADING ("ELEMENTS");
            DumpTree (pNode->asNAMESPACE()->pElements);
            break;

        case NK_NESTEDTYPE        :    // NESTEDTYPE
            DumpTree (pNode->asNESTEDTYPE()->pType);
            break;

        case NK_NEW:
            HEADING ("TYPE");
            DumpTree (pNode->asNEW()->pType);
            HEADING ("ARGS/DIMS");
            DumpTree (pNode->asNEW()->pArgs);
            HEADING ("INIT");
            DumpTree (pNode->asNEW()->pInit);
            break;

        case NK_OP                :    // BASE
            printf ("%sOPERATOR: %s\n", Spaces(iIndent), rgszOpName[pNode->Op()]);
            break;

        case NK_PARAMETER         :    // PARAMETER
            HEADING ("ATTR");
            DumpTree (pNode->asPARAMETER()->pAttr);
            HEADING ("TYPE");
            DumpTree (pNode->asPARAMETER()->pType);
            HEADING ("NAME");
            DumpTree (pNode->asPARAMETER()->pName);
            break;

        case NK_PARTIALMEMBER     :    // PARTIALMEMBER
            HEADING ("ATTR");
            DumpTree (pNode->asPARTIALMEMBER()->pAttr);
            HEADING ("PARTIAL TREE");
            DumpTree (pNode->asPARTIALMEMBER()->pNode);
            break;

        case NK_INDEXER           :    // INDEXER
        case NK_PROPERTY          :    // PROPERTY
            HEADING ("ATTR");
            DumpTree (pNode->asANYPROPERTY()->pAttr);
            HEADING ("TYPE");
            DumpTree (pNode->asANYPROPERTY()->pType);
            HEADING ("NAME");
            DumpTree (pNode->asANYPROPERTY()->pName);
            HEADING ("PARAMETERS");
            DumpTree (pNode->asANYPROPERTY()->pParms);
            HEADING ("GET ACCESSOR");
            DumpTree (pNode->asANYPROPERTY()->pGet);
            HEADING ("SET ACCESSOR");
            DumpTree (pNode->asANYPROPERTY()->pSet);
            break;

        case NK_TRY               :    // TRYSTMT
            HEADING ("GUARDED STATMENT");
            DumpTree (pNode->asTRY()->pBlock);
            if (pNode->flags & NF_TRY_CATCH)
                HEADING ("CATCHES");
            else
                HEADING ("FINALLY");
            DumpTree (pNode->asTRY()->pCatch);
            break;

        case NK_ARRAYTYPE:
            printf ("%sARRAY[%d]:\n", Spaces(iIndent), pNode->asARRAYTYPE()->iDims);
            DumpTree (pNode->asARRAYTYPE()->pElementType);
            break;

        case NK_POINTERTYPE:
            HEADING ("POINTER");
            DumpTree (pNode->asPOINTERTYPE()->pElementType);
            break;

        case NK_NULLABLETYPE:
            HEADING ("NULLABLE");
            DumpTree (pNode->asNULLABLETYPE()->pElementType);
            break;

        case NK_NAMEDTYPE:
            HEADING ("NAMED");
            DumpTree (pNode->asNAMEDTYPE()->pName);
            break;

        case NK_OPENTYPE:
            HEADING ("OPEN");
            DumpTree (pNode->asOPENTYPE()->pName);
            break;

        case NK_PREDEFINEDTYPE:
            printf ("%sPREDEFINED TYPE %d\n", Spaces(iIndent), pNode->asPREDEFINEDTYPE()->iType);
            break;

        case NK_SWITCH            :    // SWITCHSTMT
            HEADING ("EXPRESSION");
            DumpTree (pNode->asSWITCH()->pExpr);
            HEADING ("CASES");
            DumpTree (pNode->asSWITCH()->pCases);
            break;

        case NK_UNOP              :    // UNOP
            printf ("%sOPERATOR: %s\n", Spaces(iIndent), rgszOpName[pNode->Op()]);
            HEADING ("OPERAND");
            DumpTree (pNode->asUNOP()->p1);
            break;

        case NK_USING             :    // USING
            HEADING ("NAME");
            DumpTree (pNode->asUSING()->pName);
            HEADING ("ALIAS");
            DumpTree (pNode->asUSING()->pAlias);
            break;

        case NK_VARDECL           :    // VARDECL
            HEADING ("NAME");
            DumpTree (pNode->asVARDECL()->pName);
            HEADING ("INIT EXPR");
            DumpTree (pNode->asVARDECL()->pArg);
            break;

        case NK_TYPEWITHATTR      :    // TYPEWITHATTR
            HEADING ("ATTR");
            DumpTree (pNode->asTYPEWITHATTR()->pAttr);
            HEADING ("TYPE");
            DumpTree (pNode->asTYPEWITHATTR()->pType);
            break;

        default:
            break;
    }

    // Check for nodes that have 'next' pointers
    switch (pNode->kind)
    {
        // Statements
        case NK_BLOCK             :    // BLOCK
        case NK_BREAK             :    // EXPRSTMT
        case NK_CONTINUE          :    // EXPRSTMT
        case NK_EXPRSTMT          :    // EXPRSTMT
        case NK_GOTO              :    // EXPRSTMT
        case NK_RETURN            :    // EXPRSTMT
        case NK_THROW             :    // EXPRSTMT
        case NK_DECLSTMT          :    // DECLSTMT
        case NK_DO                :    // LOOPSTMT
        case NK_EMPTYSTMT         :    // STATEMENT
        case NK_FOR               :    // FORSTMT
        case NK_IF                :    // IFSTMT
        case NK_LABEL             :    // LABELSTMT
        case NK_TRY               :    // TRYSTMT
        case NK_SWITCH            :    // SWITCHSTMT
        case NK_WHILE             :    // LOOPSTMT
            DUMPNEXT (pNode->asANYSTATEMENT()->pNext);
            break;

        // Members
        case NK_CONST             :    // CONST
        case NK_CTOR              :    // METHOD
        case NK_DTOR              :    // METHOD
        case NK_ENUMMBR           :    // ENUMMBR
        case NK_FIELD             :    // FIELD
        case NK_METHOD            :    // METHOD
        case NK_NESTEDTYPE        :    // NESTEDTYPE
        case NK_PARTIALMEMBER     :    // PARTIALMEMBER
        case NK_PROPERTY          :    // PROPERTY
        case NK_OPERATOR          :    // OPERATOR
            DUMPNEXT (pNode->asANYMEMBER()->pNext);
            break;

        default:
            break;
    }

    iIndent--;
}



