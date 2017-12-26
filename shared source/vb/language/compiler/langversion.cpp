//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Shared implementation for the /LangVersion switch.  Contains constants and data used by
//  components of the compiler that need access to which features were introduced in version 10
//  and above.
// 
//-------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "LangVersion.h"

// For every feature we define in version 10 and above, we make an entry in this array indicating
// which version it was introduced in.
// the FEATUREID_* constants are defined in Errors.inc because they are also the ID we use to 
// log errors with.
LanguageFeatureMap g_LanguageFeatureMap[] = {
    { FEATUREID_AutoProperties,   LANGUAGE_10 }, // Property foo() = 42
    { FEATUREID_LineContinuation, LANGUAGE_10 }, // dim x = 1 + 
    { FEATUREID_StatementLambdas, LANGUAGE_10 }, // Function() a=1:b=2:c=3
    { FEATUREID_CoContraVariance, LANGUAGE_10 },   // Interface IReadOnly(Of Out T)
    { FEATUREID_CollectionInitializers, LANGUAGE_10 }, // dim l as new List(of integer) From {1,2}
    { FEATUREID_SubLambdas,       LANGUAGE_10 }, // sub() msgbox("lambdas rock")
    { FEATUREID_ArrayLiterals,    LANGUAGE_10 }, // dim x = {1,2}
    { FEATUREID_AsyncExpressions, LANGUAGE_11 }, // dim x = await G();
    { FEATUREID_Iterators,        LANGUAGE_11 }, // Iterator Function f() as IEnumerable(Of Integer)
    { FEATUREID_GlobalNamespace,  LANGUAGE_11 }, // Namespace Global
};

// Contains the constant strings for the previous versions of the compiler 
// Used in error reporting to indicate which version of the compiler we are targeting
WCHAR* g_LanguageVersionStrings[] = {
    L"???",
    L"9.0",
    L"10.0",
    L"11.0"
};

// Tracks the number of entries in g_LanguageVersionStrings 
int g_CountOfPreviousLanguageVersions = _countof( g_LanguageVersionStrings );
