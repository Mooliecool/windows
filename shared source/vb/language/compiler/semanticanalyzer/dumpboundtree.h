//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Routines to dump biltrees (only in debug builds).
//
//-------------------------------------------------------------------------------------------------

#pragma once

// Biltree namespace temporarily included as code cleanup item - [....]

#if DEBUG

namespace ILTree
{
	class ILNode;
}

class Compiler;

class BILDUMP
{
public:
    BILDUMP(Compiler *pCompiler)
    {
        m_pCompiler = pCompiler;
    }

    void DumpBlockTrees(
        ILTree::ILNode *ptreeContainer, 
        bool fVerbose = true);
    
    void DumpBilTree(ILTree::ILNode *ptree);

private:
    void DumpSxFlags(ILTree::ILNode *ptree);

    void DumpExpressionFlags(ExpressionFlags flags);

    void DumpStmtFlags(ILTree::ILNode *ptree);

    void DumpBilNode(
        ILTree::ILNode *ptree, 
        unsigned uIndent);
    
    void DumpStmtNode(
        ILTree::ILNode *ptreeStmt, 
        unsigned uIndent);

    void DumpBilTree(
        ILTree::ILNode *ptree, 
        unsigned uIndent);

    void DumpSxLocation(ILTree::ILNode *ptree);

    void DumpStmtLocation(ILTree::ILNode *ptree);

    void DumpStmtList(ILTree::ILNode *ptreeStmt);

    Compiler *m_pCompiler;

    bool m_fParseTrees:1;
    bool m_fVerbose:1;

};

#endif DEBUG
