//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  This file is light weight and used for name functionality that must be shared
//  between the EE and the core compiler.  It's very difficult for the EE to include
//  many of the core Compiler header files because of memory allocation and preventing
//  cross thread issues.  This file is designed to be very light weight and
//  usable by everyone
//
//-------------------------------------------------------------------------------------------------

#pragma once

// Non localized type names we use in the compiler/debugger
#define NONLOC_ANON_TYPE_DISPLAY_NAME       L"<anonymous type>"
#define NONLOC_ANON_DELEGATE_DISPLAY_NAME   L"<anonymous delegate>"
#define NONLOC_CLOSURE_DISPLAY_NAME         L"<closure>"
#define NONLOC_LAMBDA_DISPLAY_NAME          L"<lambda>"
#define NONLOC_LAMBDA_DISPLAY_NAME_1        L"<lambda%u>"
#define NONLOC_ENUMERABLE_TYPE_DISPLAY_NAME L"<enumerable type>"
#define NONLOC_GENERATED_TYPE_DISPLAY_NAME  L"<generated type>"
#define NONLOC_DYNAMICNODE_TYPE             L"<dynamic type>"
#define NONLOC_DYNAMICNODE_COMTYPE          L"<COM type>"
#define NONLOC_WINRT_TYPE                   L"<Windows Runtime type>"

#define CLOSURE_CLASS_PREFIX            L"_Closure$_"
#define CLOSURE_CLASS_PREFIX_LENGTH     10

#define CLOSURE_VARIABLE_PREFIX         L"$VB$Closure"
#define CLOSURE_VARIABLE_PREFIX_LENGTH  11

#define LIFTED_LOCAL_PREFIX             L"$VB$Local"
#define LIFTED_LOCAL_PREFIX_LENGTH      9

#define LIFTED_RESUMABLELOCAL_PREFIX             L"$VB$ResumableLocal"
#define LIFTED_RESUMABLELOCAL_PREFIX_LENGTH      18

#define LIFTED_NONLOCAL_PREFIX          L"$VB$NonLocal"
#define LIFTED_NONLOCAL_PREFIX_LENGTH   12

#define LIFTED_ME_PREFIX                L"$VB$Me"
#define LIFTED_ME_PREFIX_LENGTH         6

// !! Do not change this string. Other teams (FxCop) is uses this string to identify lambda functions in its analysis
// If you have to change this string, please contact the VB langauge PM and consider the impact of that break.
#define LAMBDA_PREFIX                   L"_Lambda$_"
#define LAMBDA_PREFIX_LENGTH            9

#define CLOSURE_GENERICPARAM_PREFIX         L"$CLS"
#define CLOSURE_GENERICPARAM_PREFIX_LENGTH  4

#define CLOSURE_MYSTUB_PREFIX           L"$VB$ClosureStub_"
#define CLOSURE_MYSTUB_PREFIX_LENGTH    16

class ClosureHelpers
{
public:
    static bool IsStringClosureClass(_In_ const WCHAR*);
    static bool IsStringClosureVariable(_In_ const WCHAR*);
    static bool IsStringLambda(_In_ const WCHAR*);
    static bool IsStringLiftedLocal(_In_ const WCHAR*);
    static bool IsStringLiftedNonLocal(_In_ const WCHAR*);
    static bool IsStringLiftedMe(_In_ const WCHAR*);
    static bool IsStringClosureGenericParameter(_In_ const WCHAR*);
    static bool IsStringLiftedResumableLocal(_In_ const WCHAR*);
};

#define VB_ANONYMOUSTYPE_PREFIX     (WIDE("VB$AnonymousType"))
#define VB_ANONYMOUSTYPE_PREFIX_LEN 16
#define VB_ANONYMOUSTYPE_DISPLAY_PROPERTY_NAME L"AtDebuggerDisplay"

class AnonymousTypeHelpers
{
public:
    static bool IsAnonymousTypeName(_In_ const WCHAR*);
};

#define VB_STATEMACHINE_PREFIX   L"VB$StateMachine"
#define VB_STATEMACHINE_PREFIX_LEN 15
                           
#define VB_ANONYMOUSTYPE_DELEGATE_PREFIX        L"VB$AnonymousDelegate"
#define VB_ANONYMOUSTYPE_DELEGATE_PREFIX_LENGTH 20

class AnonymousDelegateHelpers
{
public:
    static bool IsAnonymousDelegateName(_In_ const WCHAR*);
};

class StaticVariableHelpers
{
public:
    static
    bool IsStaticVariableName(_In_z_ const WCHAR*);
    static
    bool DecodeStaticVariableName(
        _In_z_ const WCHAR* emittedName,
        _In_ StringBuffer* pNameBuffer,
        _In_ StringBuffer* pProcedureNameBuffer,
        _In_ StringBuffer* pEncodedSigBuffer);
};

