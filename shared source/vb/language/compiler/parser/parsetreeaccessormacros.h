//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Definition of ugly macros to enable g----ful type-safe narrowing casts on parse trees.
//
//-------------------------------------------------------------------------------------------------

#pragma once

#if DEBUG

#define Accessor(Specific, General)                                 \
    virtual Specific##General * As##Specific()                      \
    {                                                               \
        VSFAIL("Not a " #Specific " " #General ".");                \
        return NULL;                                                \
    }

#define SpecificAccessor(Specific, General)                         \
    Specific##General * As##Specific()                              \
    {                                                               \
        return this;                                                \
    }

#else

#define Accessor(Specific, General)                                 \
    inline Specific##General * As##Specific()                       \
    {                                                               \
        return (Specific##General *)this;                           \
    }

#define SpecificAccessor(Specific, General)

#endif

#define StatementAccessor(Specific) Accessor(Specific, Statement)
#define SpecificStatementAccessor(Specific) SpecificAccessor(Specific, Statement)

#define ExpressionAccessor(Specific) Accessor(Specific, Expression)
#define SpecificExpressionAccessor(Specific) SpecificAccessor(Specific, Expression)

#define AttribExpressionAccessor(Specific) Accessor(Specific, AttribExpression)
#define SpecificAttribExpressionAccessor(Specific) SpecificAccessor(Specific, AttribExpression)

#define NameAccessor(Specific) Accessor(Specific, Name)
#define SpecificNameAccessor(Specific) SpecificAccessor(Specific, Name)

#define TypeAccessor(Specific) Accessor(Specific, Type)
#define SpecificTypeAccessor(Specific) SpecificAccessor(Specific, Type)

#define InitializerAccessor(Specific) Accessor(Specific, Initializer)
#define SpecificInitializerAccessor(Specific) SpecificAccessor(Specific, Initializer)

#define CaseAccessor(Specific) Accessor(Specific, Case)
#define SpecificCaseAccessor(Specific) SpecificAccessor(Specific, Case)

#define ParameterAccessor(Specific) Accessor(Specific, Parameter)
#define SpecificParameterAccessor(Specific) SpecificAccessor(Specific, Parameter)

#define VariableDeclarationAccessor(Specific) Accessor(Specific, VariableDeclaration)
#define SpecificVariableDeclarationAccessor(Specific) SpecificAccessor(Specific, VariableDeclaration)

#define AutoPropertyDeclarationAccessor(Specific) Accessor(Specific, AutoPropertyDeclaration)
#define SpecificAutoPropertyDeclarationAccessor(Specific) SpecificAccessor(Specific, AutoPropertyDeclaration)

#define ImportDirectiveAccessor(Specific) Accessor(Specific, ImportDirective)
#define SpecificImportDirectiveAccessor(Specific) SpecificAccessor(Specific, ImportDirective)

#define ConstraintAccessor(Specific) Accessor(Specific, Constraint)
#define SpecificConstraintAccessor(Specific) SpecificAccessor(Specific, Constraint)
