//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Generic visitor which visits every node in a parse tree.  This is
//  useful when only a few node types need to be handled, and most node
//  types simply need to visit their children.

//  For now, this visitor only handles expressions, but could easily
//  be extended to handle other kinds of parse tree nodes as well.
//
//-------------------------------------------------------------------------------------------------

#pragma once

//-------------------------------------------------------------------------------------------------
//
// 1. Derive your visitor from this type, and override the Visit() method.  Create your
//    own switch statement where you handle only those parse tree nodes that you need.
//
// 2. All the unreasonable number of impossibly idiosyncratic parse tree structures are not
//    represented here.  Feel free to add support for what you need, taking care not to break
//    previously derived visitors.
//
class ParseTreeVisitor
{
protected:
    virtual
    void Visit(_In_opt_ ParseTree::Statement * pStatement);
    
    virtual
    void Visit(_In_opt_ ParseTree::Expression * pExpression);

    virtual
    void VisitParameter(_In_opt_ ParseTree::Parameter * pParam);
    
    virtual
    void VisitInitializer(_In_opt_ ParseTree::Initializer * pInit);

    virtual
    void VisitFromItem(_In_ ParseTree::FromItem * pFromItem);

    virtual
    void VisitType(_In_opt_ ParseTree::Type * pType);

    virtual
    void VisitVariableDeclaration(_In_opt_ ParseTree::VariableDeclaration * pVarDecl);

private:
    void VisitCaseList(ParseTree::CaseList * pCaseList);
    void VisitExpressionList(ParseTree::ExpressionList * pExprList);
    void VisitArgumentList(ParseTree::ArgumentList * pArgList);
    void VisitParenthesizedArgumentList(_In_opt_ ParseTree::ParenthesizedArgumentList * pParenArgList);
    void VisitInitializerList(ParseTree::InitializerList * pInitList);
    void VisitBracedInitializerList(_In_opt_ ParseTree::BracedInitializerList * pBracedInitList);
    void VisitParameterList(ParseTree::ParameterList * pParamList);
    void VisitDeclaratorList(ParseTree::DeclaratorList * pDeclList);
    void VisitVariableDeclarationList(ParseTree::VariableDeclarationList * pVarDeclList);
};
