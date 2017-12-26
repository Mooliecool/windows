//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Enumeration of runtime language helpers.
//
//-------------------------------------------------------------------------------------------------

#pragma once

enum RuntimeVersion;

//-------------------------------------------------------------------------------------------------
//
// ClassTypes are t_ equivalents for well-known classes that will show up in signatures.
//
enum ClassTypes
{
    c_class = t_max + 1,
    c_object,
    c_enum,
    c_array,
    c_typehandle,
    c_exception,
    c_max
};

enum ModifierTypes
{
    m_byref = 0x01000000,
    m_array = 0x02000000,
};

enum RuntimeLibraries
{
    COMLibrary, // refers to mscorlib.dll  COM is an archaic reference to when .NET was going to be COM+
    VBLibrary, // Microsoft.VisualBasic.dll
    MaxRuntimeLibrary
};

enum RuntimeClasses
{
    UndefinedRuntimeClass,

    VBStringClass,
    VBCharClass,
    VBDateClass,
    VBDecimalClass,
    VBBooleanClass,
    VBByteClass,
    VBCharArrayClass,
    VBShortClass,
    VBIntegerClass,
    VBLongClass,
    VBSingleClass,
    VBDoubleClass,
    VBObjectClass,
    VBLateBindingV1Class,
    VBLateBindingClass,
    VBObjectFlowControlClass,
    VBOperatorsClass,
    VBEmbeddedOperatorsClass,
    VBLikeOperatorClass,
    VBProjectDataClass,
    VBUtilsClass,
    VBStandardModuleAttributeClass,
    VBOptionTextAttributeClass,
    VBConversionsClass,
    VBForLoopControlClass,
    VBLateBinderForLoopControlClass,
    VBVersionedClass,
    VBInformationClass,
    VBInteractionClass,
    VBFileSystem,
    VBApplicationBase,
    VBWindowsFormsApplicationBase,
    VBErrObject,

    COMTypeClass, // The COM prefix refers to MSCORLIB types, not C.O.M. types.
    COMStringClass,
    COMMathClass,
    COMDecimalClass,
    COMDateTimeClass,
    COMConvertClass,
    COMDebuggerClass,
    COMDebuggerNonUserCodeAttributeClass,
    COMDebuggerStepThroughAttributeClass,
    COMDebuggerDisplayAttributeClass,
    COMDebuggerBrowsableAttributeClass,
    COMAssemblyClass,
    COMAssemblyFlagsAttributeClass,
    COMObjectClass,
    COMDefaultMemberAttributeClass,
    COMParamArrayAttributeClass,
    COMSTAThreadAttributeClass,
    COMDecimalConstantAttributeClass,
    COMDateTimeConstantAttributeClass,
    COMDebuggableAttributeClass,
    COMRuntimeHelpersClass,
    COMArrayClass,
    COMAccessedThroughPropertyAttributeClass,
    COMGuidAttributeClass,
    COMClassInterfaceAttributeClass,
    COMComSourceInterfacesAttributeClass,
    COMInterfaceTypeAttributeClass,
    COMDispIdAttributeClass,
    COMComVisibleAttributeClass,
    COMTypeIdentifierAttributeClass,
    COMMarshalAsAttributeClass,
    COMStructLayoutAttributeClass,
    COMBestFitMappingAttributeClass,
    COMFieldOffsetAttributeClass,
    COMLCIDConversionAttributeClass,
    COMInAttributeClass,
    COMOutAttributeClass,
    COMTypeLibTypeAttributeClass,
    COMTypeLibFuncAttributeClass,
    COMTypeLibVarAttributeClass,
    COMUnmanagedFunctionPointerAttributeClass,
    COMActivatorClass,
    COMCompilationRelaxationsAttributeClass,
    COMCompilerGeneratedAttributeClass,
    COMRuntimeCompatibilityAttributeClass,
    COMFieldInfoClass,
    COMMethodBaseClass,
    COMMethodInfoClass,
    COMDelegateClass,
    COMMonitorClass,
    COMFlagsAttributeClass,
    COMEnvironmentClass,
    COMWinRTDesignerContextClass,
    COMAppDomainClass,
    MaxRuntimeClass,
};

enum RuntimeMembers
{
    UndefinedRuntimeMember,

    //
    // Microsoft.VisualBasic.Globals.StandardModuleAttribute
    //
    StandardModuleAttributeCtor,

    //
    // Microsoft.VisualBasic.Globals.OptionTextAttribute
    //
    OptionTextAttributeCtor,

    //======================================
    // V1 Versioned Stuff
    //======================================

    //
    // Microsoft.VisualBasic.CompilerServices.BooleanType
    //

    ObjectToBooleanV1Member,
    StringToBooleanV1Member,

    //
    // Microsoft.VisualBasic.CompilerServices.ByteType
    //
    StringToByteV1Member,
    ObjectToByteV1Member,

    //
    // Microsoft.VisualBasic.CompilerServices.CharArrayType
    //
    StringToCharArrayV1Member,
    ObjectToCharArrayV1Member,

    //
    // Microsoft.VisualBasic.CompilerServices.CharType
    //
    StringToCharV1Member,
    ObjectToCharV1Member,

    //
    // Microsoft.VisualBasic.CompilerServices.DateType
    //

    StringToDateV1Member,
    ObjectToDateV1Member,

    //
    // Microsoft.VisualBasic.CompilerServices.DecimalType
    //
    BooleanToDecimalV1Member,
    StringToDecimalV1Member,
    ObjectToDecimalV1Member,

    //
    // Microsoft.VisualBasic.CompilerServices.DoubleType
    //
    StringToDoubleV1Member,
    ObjectToDoubleV1Member,


    //
    // Microsoft.VisualBasic.CompilerServices.IntegerType
    //
    StringToIntegerV1Member,
    ObjectToIntegerV1Member,

    //
    // Microsoft.VisualBasic.CompilerServices.LateBinding
    //
    LateGetV1Member,                      // Late bound get helper
    LateSetV1Member,                      // Late bound set helper
    LateSetComplexV1Member,               // Late bound set helper for complex tasks like OptimisticSet and RvalueBase
    LateCallV1Member,                     // Late bound call helper

    //
    // Microsoft.VisualBasic.CompilerServices.LongType
    //
    StringToLongV1Member,
    ObjectToLongV1Member,

    //
    // Microsoft.VisualBasic.CompilerServices.ObjectType
    //
    ObjectCompareV1Member,
    ObjectPlusV1Member,
    ObjectNegateV1Member,
    ObjectAddV1Member,
    ObjectSubtractV1Member,
    ObjectMultiplicationV1Member,
    ObjectDivisionV1Member,
    ObjectPowerV1Member,
    ObjectModuloV1Member,
    ObjectIntegerDivisionV1Member,
    ObjectShiftLeftV1Member,
    ObjectShiftRightV1Member,
    ObjectNotV1Member,
    ObjectBitAndV1Member,
    ObjectBitOrV1Member,
    ObjectBitXorV1Member,
    ObjectConcatenationV1Member,
    ObjectLikeV1Member,

    //
    // Microsoft.VisualBasic.CompilerServices.ShortType
    //
    StringToShortV1Member,
    ObjectToShortV1Member,

    //
    // Microsoft.VisualBasic.CompilerServices.SingleType
    //
    StringToSingleV1Member,
    ObjectToSingleV1Member,

    //
    // Microsoft.VisualBasic.CompilerServices.StringType
    //
    BooleanToStringV1Member,
    ByteToStringV1Member,
    IntegerToStringV1Member,
    LongToStringV1Member,
    SingleToStringV1Member,
    DoubleToStringV1Member,
    DateToStringV1Member,
    DecimalToStringV1Member,
    CharToStringV1Member,
    ArrayOfCharToStringV1Member, // See Comment in rtbltin's
    CharAndCountToStringV1Member, // See Comment in rtbltin's
    ArrayOfCharToStringV1MemberSubset, // See Comment in rtbltin's
    ObjectToStringV1Member,

    StringCompareV1Member,
    StringLikeV1Member,                   // Like operator helper

    MidStmtStrMember,

    //
    // Microsoft.VisualBasic.Information
    //
    IsNumericV1Member,
    TypeNameV1Member,
    SystemTypeNameV1Member,
    VbTypeNameV1Member,

    //
    // Microsoft.VisualBasic.Interaction
    //
    CallByNameV1Member,

    //======================================
    // End V1 Versioned Stuff
    //======================================

    //
    // Microsoft.VisualBasic.CompilerServices.Conversions
    //
    ObjectToBooleanMember,
    StringToBooleanMember,
    StringToSignedByteMember,
    ObjectToSignedByteMember,
    StringToByteMember,
    ObjectToByteMember,
    StringToCharArrayMember,
    ObjectToCharArrayMember,
    StringToCharMember,
    ObjectToCharMember,
    StringToDateMember,
    ObjectToDateMember,
    BooleanToDecimalMember,
    StringToDecimalMember,
    ObjectToDecimalMember,
    StringToDoubleMember,
    ObjectToDoubleMember,
    StringToIntegerMember,
    ObjectToIntegerMember,
    StringToUnsignedIntegerMember,
    ObjectToUnsignedIntegerMember,
    StringToLongMember,
    ObjectToLongMember,
    StringToUnsignedLongMember,
    ObjectToUnsignedLongMember,
    StringToShortMember,
    ObjectToShortMember,
    StringToUnsignedShortMember,
    ObjectToUnsignedShortMember,
    StringToSingleMember,
    ObjectToSingleMember,
    BooleanToStringMember,
    ByteToStringMember,
    IntegerToStringMember,
    UnsignedIntegerToStringMember,
    LongToStringMember,
    UnsignedLongToStringMember,
    SingleToStringMember,
    DoubleToStringMember,
    DateToStringMember,
    DecimalToStringMember,
    CharToStringMember,
    ArrayOfCharToStringMember, // See Comment in rtbltin's
    CharAndCountToStringMember, // See Comment in rtbltin's
    ArrayOfCharToStringMemberSubset, // See Comment in rtbltin's
    ObjectToStringMember,
    ChangeTypeMember,

    //
    // Microsoft.VisualBasic.CompilerServices.ObjectFlowControl
    //
    SyncLockCheckMember,                // Verifies SyncLock is not being done on a value type

    //
    // Microsoft.VisualBasic.CompilerServices.ObjectFlowControl.ForLoopControl
    //
    SingleForCheckMember,               // Single for loop check
    DoubleForCheckMember,               // Double for loop check
    DecimalForCheckMember,              // Decimal for loop check
    ObjectForInitMember,                // Object for loop init
    ObjectForNextMember,                // Object for loop next

    //
    // Microsoft.VisualBasic.CompilerServices.LateBinderObjectFlowControl.ForLoopControl
    //
    LateBinderObjectForInitMember,
    LateBinderObjectForNextMember,


    //
    // Microsoft.VisualBasic.CompilerServices.NewLateBinder
    //
    LateCanEvaluateMember,              // Late bound call to check accessibility based on evaluation flags
    LateGetMember,                      // Late bound get helper
    LateSetMember,                      // Late bound set helper
    LateSetComplexMember,               // Late bound set helper for complex tasks like OptimisticSet and RvalueBase

    LateCallMember,                     // Late bound call helper
    LateIndexGetMember,                 // Late bound index get helper
    LateIndexSetMember,                 // Late bound index set helper
    LateIndexSetComplexMember,          // Late bound index set helper for complex tasks like OptimisticSet and RvalueBase

    //
    // Microsoft.VisualBasic.CompilerServices.Operators
    //
    // The CompareObject*Member functions are used when the result isn't boolean, e.g. Object o3 = o1 > o2
    // The ConditionalCompareObject*Member functions are used when the result is boolean, e.g. dim b as boolean = o1 > o2
    CompareObjectEqualMember,
    ConditionalCompareObjectEqualMember,
    CompareObjectNotEqualMember,
    ConditionalCompareObjectNotEqualMember,
    CompareObjectLessMember,
    ConditionalCompareObjectLessMember,
    CompareObjectLessEqualMember,
    ConditionalCompareObjectLessEqualMember,
    CompareObjectGreaterEqualMember,
    ConditionalCompareObjectGreaterEqualMember,
    CompareObjectGreaterMember,
    ConditionalCompareObjectGreaterMember,
    ObjectCompareMember,
    CompareStringMember,
    PlusObjectMember,
    NegateObjectMember,
    NotObjectMember,
    AndObjectMember,
    OrObjectMember,
    XorObjectMember,
    AddObjectMember,
    SubtractObjectMember,
    MultiplyObjectMember,
    DivideObjectMember,
    ExponentObjectMember,
    ModObjectMember,
    IntDivideObjectMember,
    LeftShiftObjectMember,
    RightShiftObjectMember,
    LikeStringMember,
    LikeObjectMember,
    ConcatenateObjectMember,

    //
    // Microsoft.VisualBasic.CompilerServices.EmbeddedOperators
    //
    EmbeddedCompareStringMember,

    //
    // Microsoft.VisualBasic.CompilerServices.ProjectData
    //
    EndStatementMember,
    CreateErrorMember,
    ClearErrorMember,
    SetErrorMember,
    SetErrorMemberWithLineNum,

    //
    // Microsoft.VisualBasic.CompilerServices.Utils
    //
    CopyArrayMember,                    // Helper for Redim Preserve

    //Microsoft.VisualBasic.CompilerServices.Versioned
    IsNumericMember,
    TypeNameMember,
    SystemTypeNameMember,
    VbTypeNameMember,
    CallByNameMember,

    //
    // System.Runtime.CompilerServices.AccessedThroughPropertyAttribute
    //
    AccessedThroughPropertyAttributeCtor,

    //
    // System.Convert
    //
    DecimalToBooleanMember,
    DecimalToSignedByteMember,
    DecimalToByteMember,
    DecimalToShortMember,
    DecimalToUnsignedShortMember,
    DecimalToIntegerMember,
    DecimalToUnsignedIntegerMember,
    DecimalToLongMember,
    DecimalToUnsignedLongMember,
    DecimalToSingleMember,
    DecimalToDoubleMember,
    IntegerToBooleanMember,
    UnsignedIntegerToBooleanMember,
    LongToBooleanMember,
    UnsignedLongToBooleanMember,
    SingleToBooleanMember,
    DoubleToBooleanMember,

    SingleToSignedByteMember,
    SingleToByteMember,
    SingleToShortMember,
    SingleToUnsignedShortMember,
    SingleToIntegerMember,
    SingleToUnsignedIntegerMember,
    SingleToLongMember,
    SingleToUnsignedLongMember,

    DoubleToSignedByteMember,
    DoubleToByteMember,
    DoubleToShortMember,
    DoubleToUnsignedShortMember,
    DoubleToIntegerMember,
    DoubleToUnsignedIntegerMember,
    DoubleToLongMember,
    DoubleToUnsignedLongMember,

    //
    // System.DateTime
    //

    DateFromConstantMember,
    CompareDateMember,
    DateConstZeroField,
    DateTimeEqual,
    DateTimeGreaterThan,
    DateTimeGreaterThanOrEqual,
    DateTimeInequality,
    DateTimeLessThan,
    DateTimeLessThanOrEqual,

    //
    // System.Decimal
    //

    CompareDecimalMember,

    DecimalFromConstantMember,
    DecimalFromInt64Member,
    DecimalConstZeroField,
    DecimalConstOneField,
    DecimalConstMinusOneField,

    DecimalNegateMember,
    DecimalModuloMember,
    DecimalAddMember,
    DecimalSubtractMember,
    DecimalMultiplicationMember,
    DecimalDivisionMember,

    IntegerToDecimalMember,
    UnsignedIntegerToDecimalMember,
    LongToDecimalMember,
    UnsignedLongToDecimalMember,
    SingleToDecimalMember,
    DoubleToDecimalMember,

    DecimalToSignedByteCast,
    DecimalToByteCast,
    DecimalToShortCast,
    DecimalToUnsignedShortCast,
    DecimalToIntegerCast,
    DecimalToUnsignedIntegerCast,
    DecimalToLongCast,
    DecimalToUnsignedLongCast,
    DecimalToSingleCast,
    DecimalToDoubleCast,

    IntegerToDecimalCast,
    UnsignedIntegerToDecimalCast,
    LongToDecimalCast,
    UnsignedLongToDecimalCast,
    SingleToDecimalCast,
    DoubleToDecimalCast,

    DecimalEqual,
    DecimalGreaterThan,
    DecimalGreaterThanOrEqual,
    DecimalInequality,
    DecimalLessThan,
    DecimalLessThanOrEqual,

    //
    // System.Diagnostics.DebuggableAttribute
    //

    DebuggableAttributeCtor,
    DebuggableAttributeCtor2,

    //
    // System.Diagnostics.Debugger
    //
    SystemDebugBreakMember,             // Stop statement helper

    //
    // System.Diagnostics.DebuggerNonUserCode
    //
    DebuggerNonUserCodeAttributeCtor,

    //
    // System.Diagnostics.DebuggerStepThrough
    //
    DebuggerStepThroughAttributeCtor,

    //
    // System.Diagnostics.DebuggerDisplay
    //
    DebuggerDisplayAttributeCtor,

    //
    // System.Diagnostics.DebuggerBrowsable
    //
    DebuggerBrowsableAttributeCtor,

    //
    // System.Math
    //
    NumericPowerMember,
    NumericRoundMember,

    //
    // System.ParamArrayAttribute
    //

    ParamArrayAttributeCtor,

    //
    // System.STAThreadAttribute
    //

    STAThreadAttributeCtor,

    //
    // System.Reflection.Assembly
    //

    LoadAssembly,

    // 
    // System.Reflection.AssemblyFlagsAttribute
    //
    AssemblyFlagsAttributeIntCtor,
    AssemblyFlagsAttributeUIntCtor,
    AssemblyFlagsAttributeAssemblyNameFlagsCtor,

    //
    // System.Reflection.DefaultMemberAttribute
    //

    DefaultMemberAttributeCtor,

    //
    // System.Reflection.FieldInfo
    //
    GetFieldFromHandleMember,
    GetFieldFromHandleGenericMember,

    //
    // System.Reflection.MethodBase
    //
    GetMethodFromHandleMember,
    GetMethodFromHandleGenericMember,

    //
    // System.Reflection.MethodInfo
    //
    IsGenericMethodDefinitionMember,
    CreateDelegateMemberFromMethodInfo,

    //
    // System.Runtime.CompilerServices.RuntimeHelpers
    //

    GetObjectValue,

    //
    // System.Runtime.CompilerServices.DecimalConstantAttribute
    //

    DecimalConstantAttributeCtor,

    //
    // System.Runtime.CompilerServices.DateTimeConstantAttribute
    //

    DateTimeConstantAttributeCtor,

    //
    // System.String
    //
    StringConcatenationMember,
    CharArrayToStringMember,

    //
    // System.Type
    //
    GetTypeFromHandleMember,
    GetTypeFromString,

    //
    // System.Array
    //
    CreateArrayInstance,

    //
    // System.Runtime.InteropServices.GuidAttribute
    //
    GuidAttributeCtor,

    //
    // System.Runtime.InteropServices.ClassInterfaceAttribute
    //
    ClassInterfaceAttributeCtor,

    //
    // System.Runtime.InteropServices.ComSourceInterfacesAttribute
    //
    ComSourceInterfacesAttributeCtor,

    //
    // System.Runtime.InteropServices.InterfaceTypeAttribute
    //
    InterfaceTypeAttributeCtor,

    //
    // System.Runtime.InteropServices.DispIdAttribute
    //
    DispIdAttributeCtor,
   
    // System.Runtime.InteropServices.ComVisibleAttribute,
    ComVisibleAttributeCtor,

    //
    // System.Runtime.InteropServices.TypeIdentifierAttribute
    TypeIdentifierAttributeCtor,
    TypeIdentifierAttributePlusGuidCtor,

    // System.Runtime.InteropServices.MarshalAsAttribute
    MarshalAsAttributeCtor,

    // System.Runtime.InteropServices.StructLayoutAttribute
    StructLayoutAttributeCtor,

    // System.Runtime.InteropServices.BestFitMappingAttribute
    BestFitMappingAttributeCtor,

    // System.Runtime.InteropServices.FieldOffsetAttribute
    FieldOffsetAttributeCtor,

    // System.Runtime.InteropServices.LCIDConversionAttribute
    LCIDConversionAttributeCtor,

    // System.Runtime.InteropServices.InAttribute
    InAttributeCtor,

    // System.Runtime.InteropServices.OutAttribute
    OutAttributeCtor,

    // System.Runtime.InteropServices.TypeLibTypeAttribute
    TypeLibTypeAttributeCtor,

    // System.Runtime.InteropServices.TypeLibFuncAttribute
    TypeLibFuncAttributeCtor,

    // System.Runtime.InteropServices.TypeLibVarAttribute
    TypeLibVarAttributeCtor,

    // System.Runtime.InteropServices.UnmanagedFunctionPointerAttribute
    UnmanagedFunctionPointerAttributeCtor,

    //
    // System.Activator
    //
    CreateInstanceMember,
    GenericCreateInstanceMember,

    //
    // System.Runtime.CompilerServices.CompilationRelaxationsAttribute
    //
    CompilationRelaxationsAttributeCtor,

    //
    // System.Runtime.CompilerServices.CompilerGeneratedAttribute
    //
    CompilerGeneratedAttributeCtor,

    //
    // System.Runtime.CompilerServices.RuntimeCompatibilityAttribute
    //
    RuntimeCompatibilityAttributeCtor,

    //
    // System.Delegate
    //
    CreateDelegateMember,


    //
    // System.Threading.Monitor.Enter with two arguments
    //
    MonitorEnter_ObjectByRefBoolean,

    //
    // System.FlagsAttribute
    //
    FlagsAttributeCtor,

    //
    // System.Environment
    //
    GetCurrentManagedThreadId,
    
    //
    // System.Runtime.DesignerServices
    //
    WinRTDesignerContextCtor,
    WinRTDesignerContextGetAssembly,

    //
    // System.AppDomain
    //
    AppDomainIsAppXModel,

    MaxRuntimeMember,
};


#define RTF_METHOD              0x02
#define RTF_FIELD               0x04
#define RTF_CONSTRUCTOR         0x08
#define RTF_VIRTUAL             0x10
#define RTF_STATIC              0x20
#define RTF_NONVIRTUAL          0x40
#define RTF_GET_PROP            0x80

struct RuntimeLibraryDescriptor
{
#if DEBUG
    RuntimeLibraries rtCheck;
#endif
    WCHAR          *wszLibraryName;
};

struct  RuntimeClassDescriptor
{
#if DEBUG
    RuntimeClasses rtCheck;
#endif
    RuntimeLibraries rtLibrary;
    WCHAR         * wszClassName;
};

struct  RuntimeMemberDescriptor
{
#if DEBUG
    RuntimeMembers rtCheck;
    unsigned short     TargetLibrary;     //UV Support
    RuntimeVersion VersionIntroduced;
#endif

    static
    bool DoesMemberMatchDescriptor(
        BCSYM_NamedRoot * pMember,
        unsigned short usFlags,
        Vtypes vtypRet,
        Vtypes *vtypArgs,
        Compiler * pCompiler);

    static
    bool DoesTypeMatchVtype(
        BCSYM * ptyp,
        Vtypes vtype,
        Compiler * pCompiler);

    unsigned short usFlags;         // class/method/field
    RuntimeClasses rtParent;        // parent class
    WCHAR        * wszName;         // method, field, or class name
    Vtypes         vtypRet;         // return type
    Vtypes         vtypArgs[9];     // argument types
};

extern RuntimeMemberDescriptor g_rgRTLangMembers[];
extern RuntimeClassDescriptor  g_rgRTLangClasses[];
extern RuntimeLibraryDescriptor g_rgRTLangLibraries[];

#if DEBUG

void CheckRTLangHelper();
void VerifyRTLangEnum(unsigned rt);
void VerifyRTLangClassEnum(unsigned rt);

#else

#define CheckRTLangHelper()
#define VerifyRTLangEnum(x)
#define VerifyRTLangClassEnum(x)

#endif
