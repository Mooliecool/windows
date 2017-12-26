//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Header file for defining XML CodeModel tags and attributes
//
//-------------------------------------------------------------------------------------------------

// This is the extra amount of space we need to generate DebAssert error messages.
#define DEBUG_STRING_BUFFER    64
#define XML_VERSION   L"<?xml version=\"1.0\"?>\r\n"

#define XMLDOC_MEMBER_TAG L"<member name=\"\">\r\n"

#define NO_INDENTATION          0   // Don't indent

// Does a tag string match
#define TagMatchesCase(x,y) (x && y && (CompareCase(x, y) == 0))
#define TagMatchesNoCase(x,y) (x && y && (CompareNoCase(x, y) == 0))

//      XMLDoc tag                          Name
//      --------------                      ----
#define XMLDoc_Doc                          L"doc"
#define XMLDoc_Assembly                     L"assembly"
#define XMLDoc_Members                      L"members"
#define XMLDoc_Member                       L"member"
#define XMLDoc_Name                         L"name"
#define XMLDoc_Summary                      L"summary"
#define XMLDoc_Para                         L"para"
#define XMLDoc_Param                        L"param"
#define XMLDoc_ParamRef                     L"paramref"
#define XMLDoc_Returns                      L"returns"
#define XMLDoc_Remarks                      L"remarks"
#define XMLDoc_See                          L"see"
#define XMLDoc_SeeAlso                      L"seealso"
#define XMLDoc_Permission                   L"permission"
#define XMLDoc_Value                        L"value"
#define XMLDoc_Example                      L"example"
#define XMLDoc_Exception                    L"exception"
#define XMLDoc_C                            L"c"
#define XMLDoc_Code                         L"code"
#define XMLDoc_List                         L"list"
#define XMLDoc_CRef                         L"cref"
#define XMLDoc_Include                      L"include"
#define XMLDoc_File                         L"file"
#define XMLDoc_Path                         L"path"
#define XMLDoc_CompletionList               L"completionlist"
#define XMLDoc_Text                         L"#text"
#define XMLDoc_FilterPriority               L"filterpriority"
#define XMLDoc_PermissionSet                L"PermissionSet"
#define XMLDoc_TypeParam                    L"typeparam"
#define XMLDoc_TypeParamRef                 L"typeparamref"
#define XMLDoc_LangWord                     L"langword"
#define XMLDoc_Capability                   L"capability"
#define XMLDoc_Type                         L"type"

// There are the boolean attributes. They are used by AttribFields0.
// Order matters here. DONNOT Change the order. If you want to add more stuff,
// add it to the end of the list

// ORDER HAS TO MATCH THE ORDER OF THE BIT-MAP BELOW

#ifdef DEF_ATTRIB0
// Bools
DEF_ATTRIB0("implicit")                     // index = 0
DEF_ATTRIB0("implicitvalue")                // index = 1
DEF_ATTRIB0("byref")                        // index = 2
DEF_ATTRIB0("out")                          // index = 3
DEF_ATTRIB0("indexer")                      // index = 4
DEF_ATTRIB0("static")                       // index = 5
DEF_ATTRIB0("instance")                     // index = 6
DEF_ATTRIB0("constant")                     // index = 7
DEF_ATTRIB0("nonzerolowerbound")            // index = 8
DEF_ATTRIB0("scientificnotation")           // index = 9
DEF_ATTRIB0("minusone")                     // index = 10
DEF_ATTRIB0("zero")                         // index = 11
DEF_ATTRIB0("resumenext")                   // index = 12
DEF_ATTRIB0("shadows")                      // index = 13
DEF_ATTRIB0("sealed")                       // index = 14
DEF_ATTRIB0("readonly")                     // index = 15
DEF_ATTRIB0("virtual")                      // index = 16
DEF_ATTRIB0("notoverridable")               // index = 17
DEF_ATTRIB0("overrides")                    // index = 18
DEF_ATTRIB0("overloads")                    // index = 19
DEF_ATTRIB0("abstract")                     // index = 20
DEF_ATTRIB0("extern")                       // index = 21
DEF_ATTRIB0("withevents")                   // index = 22
DEF_ATTRIB0("optional")                     // index = 23
DEF_ATTRIB0("value")                        // index = 24
DEF_ATTRIB0("module")                       // index = 25
DEF_ATTRIB0("special")                      // index = 26
DEF_ATTRIB0("function")                     // index = 27
DEF_ATTRIB0("root")                         // index = 28
DEF_ATTRIB0("mustoverride")                 // index = 29
DEF_ATTRIB0("mustinherit")                  // index = 30
DEF_ATTRIB0("bracketed")                    // index = 31
DEF_ATTRIB0("paramarray")                   // index = 32
DEF_ATTRIB0("shared")                       // index = 33
DEF_ATTRIB0("default")                      // index = 34
DEF_ATTRIB0("directcast")                   // index = 35
#endif  // DEF_ATTRIB0

// These are the Enum, unsigned, WCHAR, and WCHAR* attributres. They are use by AttribFields1
// Again, order matters here big time.
#ifdef DEF_ATTRIB1
// Enums
DEF_ATTRIB1("variablekind")                 // index = 0
DEF_ATTRIB1("accessor")                     // index = 1
DEF_ATTRIB1("unaryoperator")                // index = 2
DEF_ATTRIB1("binaryoperator")               // index = 3
DEF_ATTRIB1("conversionoperator")           // index = 4
DEF_ATTRIB1("base")                         // index = 5
DEF_ATTRIB1("access")                       // index = 6
DEF_ATTRIB1("propertytype")                 // index = 7
DEF_ATTRIB1("breakkind")                    // index = 8

// unsigned
DEF_ATTRIB1("rank")                         // index = 9
DEF_ATTRIB1("count")                        // index = 10

// PWCHAR
DEF_ATTRIB1("alias")                        // index = 11
DEF_ATTRIB1("name")                         // index = 12
DEF_ATTRIB1("fullname")                     // index = 13
DEF_ATTRIB1("eventname")                    // index = 14
#endif  // DEF_ATTRIB1

#ifndef DEF_ATTRIB0
#ifndef DEF_ATTRIB1

// Order doesn't matter here
//
//      Element Kind                        Name
//      --------------                      ----

#define MISC_Language_VB                    L"VB7.0"
#define MISC_Language_MetaData              L"MetaData"

#define ELEMENT_This_Reference              L"ThisReference"
#define ELEMENT_Base_Reference              L"BaseReference"
#define ELEMENT_Name_Ref                    L"NameRef"
#define ELEMENT_ArrayElementAccess          L"ArrayElementAccess"
#define ELEMENT_Local                       L"Local"
#define ELEMENT_Type                        L"Type"
#define ELEMENT_Array_Type                  L"ArrayType"
#define ELEMENT_NewLine                     L"NewLine"
#define ELEMENT_Null                        L"Null"
#define ELEMENT_Void                        L"Void"
#define ELEMENT_Nothing                     L"Nothing"
#define ELEMENT_Number                      L"Number"
#define ELEMENT_Boolean                     L"Boolean"
#define ELEMENT_Char                        L"Char"
#define ELEMENT_String                      L"String"
#define ELEMENT_Array                       L"Array"
#define ELEMENT_Comment                     L"Comment"
#define ELEMENT_Quote                       L"Quote"
#define ELEMENT_Method                      L"Method"
#define ELEMENT_Property                    L"Property"
#define ELEMENT_Event                       L"Event"
#define ELEMENT_Class                       L"Class"
#define ELEMENT_Struct                      L"Structure"
#define ELEMENT_Module                      L"Module"
#define ELEMENT_Base                        L"Base"
#define ELEMENT_Inherits                    L"Inherits"
#define ELEMENT_Enum                        L"Enum"
#define ELEMENT_Enumeration                 L"Enumeration"
#define ELEMENT_Variable                    L"Variable"
#define ELEMENT_Const                       L"Const"
#define ELEMENT_Constant                    L"Constant"
#define ELEMENT_Delegate                    L"Delegate"
#define ELEMENT_Field                       L"Field"
#define ELEMENT_Using                       L"Using"
#define ELEMENT_Interface                   L"Interface"
#define ELEMENT_Imports                     L"Imports"
#define ELEMENT_Name                        L"Name"
#define ELEMENT_Imported_NameSpace          L"ImportedNameSpace"
#define ELEMENT_Implements                  L"Implements"
#define ELEMENT_Implemented_Method          L"ImplementedMethod"
#define ELEMENT_Handles                     L"Handles"
#define ELEMENT_Method_Name                 L"MethodName"
#define ELEMENT_Constructor                 L"Constructor"
#define ELEMENT_Inherits                    L"Inherits"
#define ELEMENT_Declarations                L"Declarations"
#define ELEMENT_Sub                         L"Sub"
#define ELEMENT_Function                    L"Function"
#define ELEMENT_Parameter                   L"Parameter"
#define ELEMENT_Base                        L"Base"
#define ELEMENT_Me                          L"Me"
#define ELEMENT_Block                       L"Block"
#define ELEMENT_While                       L"While"
#define ELEMENT_Do                          L"Do"
#define ELEMENT_Loop                        L"Loop"
#define ELEMENT_If                          L"If"
#define ELEMENT_Switch                      L"Switch"
#define ELEMENT_Select                      L"Select"
#define ELEMENT_Case                        L"Case"
#define ELEMENT_Special                     L"Special"
#define ELEMENT_Then                        L"Then"
#define ELEMENT_Else                        L"Else"
#define ELEMENT_ElseIf                      L"ElseIf"
#define ELEMENT_For                         L"For"
#define ELEMENT_Each                        L"Each"
#define ELEMENT_For_Each                    L"ForEach"
#define ELEMENT_To                          L"To"
#define ELEMENT_In                          L"In"
#define ELEMENT_Step                        L"Step"
#define ELEMENT_Next                        L"Next"
#define ELEMENT_Break                       L"Break"
#define ELEMENT_Try                         L"Try"
#define ELEMENT_Catch                       L"Catch"
#define ELEMENT_When                        L"When"
#define ELEMENT_With                        L"With"
#define ELEMENT_Finally                     L"Finally"
#define ELEMENT_OnError                     L"OnError"
#define ELEMENT_GetType                     L"GetType"
#define ELEMENT_GoTo                        L"GoTo"
#define ELEMENT_Label                       L"Label"
#define ELEMENT_Resume                      L"Resume"
#define ELEMENT_ResumeNext                  L"ResumeNext"
#define ELEMENT_Exit                        L"Exit"
#define ELEMENT_Stop                        L"Stop"
#define ELEMENT_End                         L"End"
#define ELEMENT_Expression                  L"Expression"
#define ELEMENT_ExpressionStatement         L"ExpressionStatement"
#define ELEMENT_Signature                   L"Signature"
#define ELEMENT_Source_Name                 L"Sourcename"
#define ELEMENT_NameSpace                   L"NameSpace"
#define ELEMENT_Attributes                  L"Attributes"
#define ELEMENT_AttributeSection            L"AttributeSection"
#define ELEMENT_Attribute                   L"Attribute"
#define ELEMENT_AttributeProperty           L"AttributeProperty"
#define ELEMENT_Argument                    L"Argument"
#define ELEMENT_Dim                         L"Dim"
#define ELEMENT_Bound                       L"Bound"
#define ELEMENT_Ranks                       L"Ranks"
#define ELEMENT_RankEntry                   L"RankEntry"
#define ELEMENT_IdReference                 L"IdReference"
#define ELEMENT_Get                         L"Get"
#define ELEMENT_Set                         L"Set"

#define EXPRESSION_Literal                  L"Literal"
#define EXPRESSION_MethodCall               L"MethodCall"
#define EXPRESSION_ArrayElementAccess       L"ArrayElementAccess"
#define EXPRESSION_NameRef                  L"NameRef"
#define EXPRESSION_ConstructorCall          L"ConstructorCall"
#define EXPRESSION_BaseReference            L"BaseReference"
#define EXPRESSION_WithAccess               L"WithAccess"
#define EXPRESSION_LateAccess               L"LateAccess"
#define EXPRESSION_Parentheses              L"Parentheses"
#define EXPRESSION_UnaryOperation           L"UnaryOperation"
#define EXPRESSION_BinaryOperation          L"BinaryOperation"
#define EXPRESSION_ConditionalOperation     L"ConditionalOperation"
#define EXPRESSION_NewClass                 L"NewClass"
#define EXPRESSION_NewArray                 L"NewArray"
#define EXPRESSION_NewDelegate              L"NewDelegate"
#define EXPRESSION_Cast                     L"Cast"
#define EXPRESSION_Assignment               L"Assignment"
#define EXPRESSION_Special                  L"Special"
#define EXPRESSION_ThisReference            L"ThisReference"

#define ATTRIB_VALUE_Boolean                L"Boolean"

#define ATTRIB_VALUE_Rank                   L"rank"
#define ATTRIB_VALUE_Count                  L"count"
#define ATTRIB_VALUE_Name                   L"name"
#define ATTRIB_VALUE_Full_Name              L"fullname"
#define ATTRIB_VALUE_Source_Name            L"sourcename"
#define ATTRIB_VALUE_Bracketed              L"bracketed"
#define ATTRIB_VALUE_Root                   L"root"
#define ATTRIB_VALUE_Event_Name             L"eventname"
#define ATTRIB_VALUE_Id                     L"id"
#define ATTRIB_VALUE_Param_ID               L"paramid"
#define ATTRIB_VALUE_Reference_ID           L"refid"
#define ATTRIB_VALUE_Key                    L"key"
#define ATTRIB_VALUE_Access                 L"access"
#define ATTRIB_VALUE_Special_Name           L"specialname"
#define ATTRIB_VALUE_Unknown                L"unknown"
#define ATTRIB_VALUE_Unresolved             L"unresolved"
#define ATTRIB_VALUE_Private                L"private"
#define ATTRIB_VALUE_Public                 L"public"
#define ATTRIB_VALUE_ProtectedFriend        L"protectedfriend"
#define ATTRIB_VALUE_Protected              L"protected"
#define ATTRIB_VALUE_Internal               L"internal"
#define ATTRIB_VALUE_Friend                 L"friend"
#define ATTRIB_VALUE_Shadows                L"shadows"
#define ATTRIB_VALUE_Module                 L"module"
#define ATTRIB_VALUE_Alias                  L"alias"
#define ATTRIB_VALUE_VariableKind           L"variablekind"
#define ATTRIB_VALUE_BreakKind              L"breakkind"
#define ATTRIB_VALUE_DirectCast             L"directcast"
#define ATTRIB_VALUE_TryCast                L"trycast"

#define ATTRIB_VALUE_Local                  L"local"
#define ATTRIB_VALUE_Field                  L"field"
#define ATTRIB_VALUE_Constant               L"constant"
#define ATTRIB_VALUE_Property               L"property"
#define ATTRIB_VALUE_Acessor                L"accessor"
#define ATTRIB_VALUE_Indexer                L"indexer"
#define ATTRIB_VALUE_Method                 L"method"
#define ATTRIB_VALUE_Static                 L"static"
#define ATTRIB_VALUE_Instance               L"instance"
#define ATTRIB_VALUE_Shared                 L"shared"
#define ATTRIB_VALUE_Function               L"function"

#define ATTRIB_VALUE_Not_Inheritable        L"notinheritable"
#define ATTRIB_VALUE_Sealed                 L"sealed"
#define ATTRIB_VALUE_ReadOnly               L"readonly"
#define ATTRIB_VALUE_No_Debug               L"nodebug"
#define ATTRIB_VALUE_Virtual                L"virtual"
#define ATTRIB_VALUE_Over_Rides             L"overrides"
#define ATTRIB_VALUE_Over_Loads             L"overloads"
#define ATTRIB_VALUE_Over_Ridable           L"overridable"
#define ATTRIB_VALUE_Not_Over_Ridable       L"notoverridable"
#define ATTRIB_VALUE_Abstract               L"abstract"
#define ATTRIB_VALUE_Must_OverRide          L"MustOverride"
#define ATTRIB_VALUE_Must_Inherit           L"MustInherit"
#define ATTRIB_VALUE_WithEvents             L"withevents"
#define ATTRIB_VALUE_Implicit               L"implicit"
#define ATTRIB_VALUE_Implicit_Value         L"implicitvalue"
#define ATTRIB_VALUE_Optional               L"optional"
#define ATTRIB_VALUE_ParamArray             L"paramarray"
#define ATTRIB_VALUE_ByVal                  L"byval"
#define ATTRIB_VALUE_ByRef                  L"byref"
#define ATTRIB_VALUE_Extern                 L"extern"
#define ATTRIB_VALUE_Default                L"default"
#define ATTRIB_VALUE_Line                   L"line"

#define ATTRIB_VALUE_Get                    L"get"
#define ATTRIB_VALUE_Set                    L"set"
#define ATTRIB_VALUE_Value                  L"value"

#define ATTRIB_VALUE_Scientificnotation     L"scientificnotation"
#define ATTRIB_VALUE_Base                   L"base"
#define ATTRIB_VALUE_8                      L"8"
#define ATTRIB_VALUE_10                     L"10"
#define ATTRIB_VALUE_16                     L"16"
#define ATTRIB_VALUE_TypeChar               L"typechar"
#define ATTRIB_VALUE_Type                   L"type"

#define ATTRIB_VALUE_Yes                    L"yes"
#define ATTRIB_VALUE_True                   L"true"
#define ATTRIB_VALUE_False                  L"false"

#define ATTRIB_VALUE_Property_Type          L"propertytype"
#define ATTRIB_VALUE_Prop_ReadOnly          L"readonly"
#define ATTRIB_VALUE_Prop_WriteOnly         L"writeonly"
#define ATTRIB_VALUE_Prop_ReadWrite         L"readwrite"

#define ATTRIB_VALUE_Unary_Operator         L"unaryoperator"
#define ATTRIB_VALUE_Binary_Operator        L"binaryoperator"

// Unary Operators
#define ATTRIBUTE_VALUE_Plus                L"plus"
#define ATTRIBUTE_VALUE_Minus               L"minus"
#define ATTRIBUTE_VALUE_Not                 L"not"
#define ATTRIBUTE_VALUE_Bitnot              L"bitnot"
#define ATTRIBUTE_VALUE_Complement          L"complement"
#define ATTRIBUTE_VALUE_Increment           L"increment"
#define ATTRIBUTE_VALUE_Decrement           L"decrement"
#define ATTRIBUTE_VALUE_Postincrement       L"postincrement"
#define ATTRIBUTE_VALUE_Postdecrement       L"postdecrement"
#define ATTRIBUTE_VALUE_True                L"true"
#define ATTRIBUTE_VALUE_False               L"false"
#define ATTRIBUTE_VALUE_Sizeof              L"sizeof"
#define ATTRIBUTE_VALUE_Ref                 L"ref"
#define ATTRIBUTE_VALUE_Out                 L"out"
#define ATTRIBUTE_VALUE_Value               L"value"

// Binary Operators
#define ATTRIBUTE_VALUE_Dot                 L"dot"
#define ATTRIBUTE_VALUE_Bang                L"bang"
#define ATTRIBUTE_VALUE_Plus                L"plus"
#define ATTRIBUTE_VALUE_Minus               L"minus"
#define ATTRIBUTE_VALUE_Times               L"times"
#define ATTRIBUTE_VALUE_Divide              L"divide"
#define ATTRIBUTE_VALUE_Remainder           L"remainder"
#define ATTRIBUTE_VALUE_Power               L"power"
#define ATTRIBUTE_VALUE_Bitand              L"bitand"
#define ATTRIBUTE_VALUE_Bitor               L"bitor"
#define ATTRIBUTE_VALUE_Bitxor              L"bitxor"
#define ATTRIBUTE_VALUE_Logicaland          L"logicaland"
#define ATTRIBUTE_VALUE_Logicalor           L"logicalor"
#define ATTRIBUTE_VALUE_Shiftleft           L"shiftleft"
#define ATTRIBUTE_VALUE_Shiftright          L"shiftright"
#define ATTRIBUTE_VALUE_Equals              L"equals"
#define ATTRIBUTE_VALUE_Notequals           L"notequals"
#define ATTRIBUTE_VALUE_Refequals           L"refequals"
#define ATTRIBUTE_VALUE_Refnotequals        L"refnotequals"
#define ATTRIBUTE_VALUE_Greaterthan         L"greaterthan"
#define ATTRIBUTE_VALUE_Lessthan            L"lessthan"
#define ATTRIBUTE_VALUE_Greaterthanorequals L"greaterthanorequals"
#define ATTRIBUTE_VALUE_Lessthanorequals    L"lessthanorequals"
#define ATTRIBUTE_VALUE_Istype              L"istype"
#define ATTRIBUTE_VALUE_Like                L"like"
#define ATTRIBUTE_VALUE_Adddelegate         L"adddelegate"
#define ATTRIBUTE_VALUE_Removedelegate      L"removedelegate"
#define ATTRIBUTE_VALUE_Concatenate         L"concatenate"
#define ATTRIBUTE_VALUE_To                  L"to"

// Break kinds
#define ATTRIBUTE_VALUE_Try                 L"try"
#define ATTRIBUTE_VALUE_Sub                 L"sub"
#define ATTRIBUTE_VALUE_Function            L"function"
#define ATTRIBUTE_VALUE_Do                  L"do"
#define ATTRIBUTE_VALUE_For                 L"for"
#define ATTRIBUTE_VALUE_Select              L"select"
#define ATTRIBUTE_VALUE_Property            L"property"

// Some characters often used
#define CHAR_XMLOpenTag                     WIDE('<')
#define CHAR_XMLCloseTag                    WIDE('>')
#define CHAR_DoubleQuote                    WIDE('\"')
#define CHAR_OpenBracket                    WIDE('[')
#define CHAR_CloseBracket                   WIDE(']')
#define CHAR_Space                          WIDE(' ')
#define CHAR_Period                         WIDE('.')
#define CHAR_UnderScore                     WIDE('_')
#define CHAR_Forwartd_Slash                 WIDE('/')
#define COMMA_SEPERATOR                     WIDE(", ")

#define FirstPrintableChar                  0x0020
#define LastPrintableChar                   0x007E
#define LastValidCharacter                  0xFFFD

#define ENCODING_MASK                       0x00FF
#define START_OF_ENCODING_PRIVATE_USER_AREA 0xE000
#define END_OF_ENCODING_PRIVATE_USER_AREA   START_OF_ENCODING_PRIVATE_USER_AREA + FirstPrintableChar

typedef unsigned XMLDOCFLAGS;

#define TAG_NONE         ((XMLDOCFLAGS)0x00000000)
#define TAG_C            ((XMLDOCFLAGS)0x00000001)
#define TAG_CODE         ((XMLDOCFLAGS)0x00000002)
#define TAG_EXAMPLE      ((XMLDOCFLAGS)0x00000004)
#define TAG_EXCEPTION    ((XMLDOCFLAGS)0x00000008)
#define TAG_LIST         ((XMLDOCFLAGS)0x00000010)
#define TAG_PARA         ((XMLDOCFLAGS)0x00000020)
#define TAG_PARAM        ((XMLDOCFLAGS)0x00000040)
#define TAG_PARAMREF     ((XMLDOCFLAGS)0x00000080)
#define TAG_PERMISSION   ((XMLDOCFLAGS)0x00000100)
#define TAG_REMARKS      ((XMLDOCFLAGS)0x00000200)
#define TAG_RETURNS      ((XMLDOCFLAGS)0x00000400)
#define TAG_SEE          ((XMLDOCFLAGS)0x00000800)
#define TAG_SEEALSO      ((XMLDOCFLAGS)0x00001000)
#define TAG_SUMMARY      ((XMLDOCFLAGS)0x00002000)
#define TAG_VALUE        ((XMLDOCFLAGS)0x00004000)
#define TAG_INCLUDE      ((XMLDOCFLAGS)0x00008000)
#define TAG_TYPEPARAM ((XMLDOCFLAGS)0x00010000)
#define TAG_LAST_TAG     ((XMLDOCFLAGS)0x00010000)

typedef unsigned __int64 XMLFLAGS;          // Be carefull to put cast (XMLFLAGS) on
                                            // its defines otherwise the compiler
                                            // will treat them as 32 bit values and ~,&, etc.
                                            // won't operate on the upper 32 bits

#define BCSYM_Class_Disallowed_TAGS                     (TAG_PARAM | TAG_PARAMREF | TAG_RETURNS | TAG_VALUE | TAG_EXCEPTION)
#define BCSYM_Interface_Disallowed_TAGS                 (TAG_PARAM | TAG_PARAMREF | TAG_RETURNS | TAG_VALUE | TAG_EXCEPTION)
#define BCSYM_Delegate_Disallowed_TAGS                  (TAG_VALUE | TAG_EXCEPTION)
#define BCSYM_Proc_Disallowed_TAGS                      (TAG_VALUE)
#define BCSYM_Property_Disallowed_TAGS                  (TAG_NONE | TAG_TYPEPARAM)
#define BCSYM_Variable_Disallowed_TAGS                  (TAG_PARAM | TAG_PARAMREF | TAG_RETURNS | TAG_VALUE | TAG_TYPEPARAM| TAG_EXCEPTION)
#define BCSYM_Event_Disallowed_TAGS                     (TAG_RETURNS | TAG_VALUE | TAG_TYPEPARAM)

// Msks out everything, except for the lowest bit
// Order does matter here
#define ATTRIB_MASK                         ((XMLFLAGS)0x0000000000000001)

// All zeors. No attributes are set
#define ATTRIB_NONE                         ((XMLFLAGS)0x0000000000000000)

// bool Attributes

// ORDER HAS TO MATCH THE DEF_ATTRIB0 ARRAY ABOVE

// Order does matter here
#define ATTRIB_IMPLICIT                     ((XMLFLAGS)0x0000000000000001)
#define ATTRIB_IMPLICITVALUE                ((XMLFLAGS)0x0000000000000002)
#define ATTRIB_BYREF                        ((XMLFLAGS)0x0000000000000004)
#define ATTRIB_OUT                          ((XMLFLAGS)0x0000000000000008)
#define ATTRIB_INDEXER                      ((XMLFLAGS)0x0000000000000010)
#define ATTRIB_STATIC                       ((XMLFLAGS)0x0000000000000020)
#define ATTRIB_INSTANCE                     ((XMLFLAGS)0x0000000000000040)
#define ATTRIB_CONSTANT                     ((XMLFLAGS)0x0000000000000080)
#define ATTRIB_NON_ZERO_LOWER_BOUND         ((XMLFLAGS)0x0000000000000100)
#define ATTRIB_SCIENTIFIC_NOTATION          ((XMLFLAGS)0x0000000000000200)
#define ATTRIB_MINUS_ONE                    ((XMLFLAGS)0x0000000000000400)
#define ATTRIB_ZERO                         ((XMLFLAGS)0x0000000000000800)
#define ATTRIB_RESUME_NEXT                  ((XMLFLAGS)0x0000000000001000)
#define ATTRIB_SHADOWS                      ((XMLFLAGS)0x0000000000002000)
#define ATTRIB_SEALED                       ((XMLFLAGS)0x0000000000004000)
#define ATTRIB_READONLY                     ((XMLFLAGS)0x0000000000008000)
#define ATTRIB_VIRTUAL                      ((XMLFLAGS)0x0000000000010000)
#define ATTRIB_NOT_OVERRIDABLE              ((XMLFLAGS)0x0000000000020000)
#define ATTRIB_OVERRIDES                    ((XMLFLAGS)0x0000000000040000)
#define ATTRIB_OVERLOADS                    ((XMLFLAGS)0x0000000000080000)
#define ATTRIB_ABSTRACT                     ((XMLFLAGS)0x0000000000100000)
#define ATTRIB_EXTERN                       ((XMLFLAGS)0x0000000000200000)
#define ATTRIB_WITH_EVENTS                  ((XMLFLAGS)0x0000000000400000)
#define ATTRIB_OPTIONAL                     ((XMLFLAGS)0x0000000000800000)
#define ATTRIB_VALUE                        ((XMLFLAGS)0x0000000001000000)
#define ATTRIB_MODULE                       ((XMLFLAGS)0x0000000002000000)
#define ATTRIB_SPECIAL_NAME                 ((XMLFLAGS)0x0000000004000000)
#define ATTRIB_FUNCTION                     ((XMLFLAGS)0x0000000008000000)
#define ATTRIB_ROOT_NAMESPACE               ((XMLFLAGS)0x0000000010000000)
#define ATTRIB_MUST_OVERRIDE                ((XMLFLAGS)0x0000000020000000)
#define ATTRIB_MUST_INHERIT                 ((XMLFLAGS)0x0000000040000000)
#define ATTRIB_BRACKETED                    ((XMLFLAGS)0x0000000080000000)
#define ATTRIB_PARAMARRAY                   ((XMLFLAGS)0x0000000100000000)
#define ATTRIB_SHARED                       ((XMLFLAGS)0x0000000200000000)
#define ATTRIB_DEFAULT                      ((XMLFLAGS)0x0000000400000000)
#define ATTRIB_DIRECTCAST                   ((XMLFLAGS)0x0000000800000000)
#define ATTRIB_TRYCAST                      ((XMLFLAGS)0x0000001000000000)
#define END_ATTRIB0_BOOL                    ((XMLFLAGS)0x0000001000000000)

// End bool Attributes

// ORDER HAS TO MATCH THE DEF_ATTRIB1 ARRAY ABOVE

// enum Attributes
// Order does matter here
#define ATTRIB_VARIABLE_KIND                ((XMLFLAGS)0x0000000000000001)
#define ATTRIB_ACCESSOR                     ((XMLFLAGS)0x0000000000000002)
#define ATTRIB_UNARY_OPERATOR               ((XMLFLAGS)0x0000000000000004)
#define ATTRIB_BINARY_OPERATOR              ((XMLFLAGS)0x0000000000000008)
#define ATTRIB_CONVERTION_OPERATOR          ((XMLFLAGS)0x0000000000000010)
#define ATTRIB_BASE                         ((XMLFLAGS)0x0000000000000020)
#define ATTRIB_ACCESS                       ((XMLFLAGS)0x0000000000000040)
#define ATTRIB_PROPERTY_TYPE                ((XMLFLAGS)0x0000000000000080)
#define ATTRIB_BREAK_KIND                   ((XMLFLAGS)0x0000000000000100)
#define END_ATTRIB1_ENUM                    ((XMLFLAGS)0x0000000000000100)

// End enum Attributes

// unsigned Attributes
// Order does matter here
#define ATTRIB_RANK                         ((XMLFLAGS)0x0000000000000200)
#define ATTRIB_COUNT                        ((XMLFLAGS)0x0000000000000400)
#define END_ATTRIB1_INT                     ((XMLFLAGS)0x0000000000000400)

// End unsigned Attributes

// WCHAR* Attributes
// Order does matter here
#define ATTRIB_ALIAS                        ((XMLFLAGS)0x0000000000000800)
#define ATTRIB_NAME                         ((XMLFLAGS)0x0000000000001000)
#define ATTRIB_FULL_NAME                    ((XMLFLAGS)0x0000000000002000)
#define ATTRIB_EVENT_NAME                   ((XMLFLAGS)0x0000000000004000)
#define END_ATTRIB1_PWCHAR                  ((XMLFLAGS)0x0000000000004000)

// End WCHAR* Attributes

// End Attributes defs

// Order doesn't matter here
// All valid attributes for each type of element
#define NAME_SPACE_ATTRIB_0                 (ATTRIB_SPECIAL_NAME | ATTRIB_ROOT_NAMESPACE | ATTRIB_BRACKETED)
#define NAME_SPACE_ATTRIB_1                 (ATTRIB_NAME | ATTRIB_FULL_NAME)
#define CLASS_ATTRIB_0                      (ATTRIB_SPECIAL_NAME | ATTRIB_SHADOWS | ATTRIB_SEALED | ATTRIB_ABSTRACT | ATTRIB_VALUE | ATTRIB_MODULE | ATTRIB_BRACKETED)
#define CLASS_ATTRIB_1                      (ATTRIB_NAME | ATTRIB_FULL_NAME | ATTRIB_MODULE | ATTRIB_ACCESS)
#define INTERFACE_ATTRIB_0                  (ATTRIB_SPECIAL_NAME | ATTRIB_SHADOWS | ATTRIB_BRACKETED)
#define INTERFACE_ATTRIB_1                  (ATTRIB_NAME | ATTRIB_FULL_NAME | ATTRIB_ACCESS)
#define ENUMERATION_ATTRIB_0                (ATTRIB_SPECIAL_NAME | ATTRIB_SHADOWS | ATTRIB_BRACKETED)
#define ENUMERATION_ATTRIB_1                (ATTRIB_NAME | ATTRIB_FULL_NAME | ATTRIB_ACCESS)
#define DELEGATE_ATTRIB_0                   (ATTRIB_SPECIAL_NAME | ATTRIB_SHADOWS | ATTRIB_BRACKETED)
#define DELEGATE_ATTRIB_1                   (ATTRIB_NAME | ATTRIB_FULL_NAME | ATTRIB_ACCESS)
#define USING_ATTRIB_0                      (ATTRIB_NONE)
#define USING_ATTRIB_1                      (ATTRIB_NONE)
#define IMPOTRTED_NAME_SPACE_ATTRIB_0       (ATTRIB_NONE)
#define IMPOTRTED_NAME_SPACE_ATTRIB_1       (ATTRIB_ALIAS)
#define NEW_LINE_ATTRIB_0                   (ATTRIB_NONE)
#define NEW_LINE_ATTRIB_1                   (ATTRIB_COUNT)
#define COMMENT_ATTRIB_0                    (ATTRIB_NONE)
#define COMMENT_ATTRIB_1                    (ATTRIB_NONE)
#define QUOTE_ATTRIB_0                      (ATTRIB_NONE)
#define QUOTE_ATTRIB_1                      (ATTRIB_NONE)
#define DESTRUCTOR_ATTRIB_0                 (ATTRIB_NONE)
#define DESTRUCTOR_ATTRIB_1                 (ATTRIB_NONE)
#define CONSTRUCTOR_ATTRIB_0                (ATTRIB_BRACKETED)
#define CONSTRUCTOR_ATTRIB_1                (ATTRIB_NAME | ATTRIB_FULL_NAME | ATTRIB_ACCESS)
#define EVENT_ATTRIB_0                      (ATTRIB_SPECIAL_NAME | ATTRIB_SHADOWS | ATTRIB_STATIC | ATTRIB_SHARED | ATTRIB_BRACKETED)
#define EVENT_ATTRIB_1                      (ATTRIB_NAME | ATTRIB_FULL_NAME | ATTRIB_ACCESS)
#define PROPERTY_ATTRIB_0                   (ATTRIB_SPECIAL_NAME | ATTRIB_SHADOWS | ATTRIB_STATIC | ATTRIB_SHARED | ATTRIB_DEFAULT | ATTRIB_INDEXER | ATTRIB_READONLY | ATTRIB_OVERRIDES | ATTRIB_OVERLOADS | ATTRIB_BRACKETED | ATTRIB_VIRTUAL | ATTRIB_NOT_OVERRIDABLE)
#define PROPERTY_ATTRIB_1                   (ATTRIB_NAME | ATTRIB_FULL_NAME | ATTRIB_ACCESS | ATTRIB_PROPERTY_TYPE)
#define PROPERTY_ACCESSOR_ATTRIB_0          (ATTRIB_ACCESSOR)
#define PROPERTY_ACCESSOR_ATTRIB_1          (ATTRIB_NONE)
#define METHOD_ATTRIB_0                     (ATTRIB_SPECIAL_NAME | ATTRIB_SHADOWS | ATTRIB_STATIC | ATTRIB_SHARED | ATTRIB_VIRTUAL | ATTRIB_NOT_OVERRIDABLE | ATTRIB_OVERRIDES | ATTRIB_ABSTRACT | ATTRIB_EXTERN | ATTRIB_FUNCTION | ATTRIB_OVERLOADS | ATTRIB_BRACKETED)
#define METHOD_ATTRIB_1                     (ATTRIB_NAME | ATTRIB_FULL_NAME | ATTRIB_ACCESS)
#define HANDLES_ATTRIB_0                    (ATTRIB_NONE)
#define HANDLES_ATTRIB_1                    (ATTRIB_EVENT_NAME)
#define IMPLEMENTED_METHOD_ATTRIB_0         (ATTRIB_NONE)
#define IMPLEMENTED_METHOD_ATTRIB_1         (ATTRIB_NONE)
#define FIELD_ATTRIB_0                      (ATTRIB_SHADOWS | ATTRIB_STATIC | ATTRIB_SHARED | ATTRIB_WITH_EVENTS)
#define FIELD_ATTRIB_1                      (ATTRIB_ACCESS)
#define CONSTANT_ATTRIB_0                   (ATTRIB_SHADOWS)
#define CONSTANT_ATTRIB_1                   (ATTRIB_ACCESS)
#define VARIABLE_ATTRIB_0                   (ATTRIB_SPECIAL_NAME | ATTRIB_IMPLICIT | ATTRIB_BRACKETED)
#define VARIABLE_ATTRIB_1                   (ATTRIB_NAME | ATTRIB_FULL_NAME)
#define IMPLEMENTS_ATTRIB_0                 (ATTRIB_NONE)
#define IMPLEMENTS_ATTRIB_1                 (ATTRIB_NONE)
#define BASE_ATTRIB_0                       (ATTRIB_NONE)
#define BASE_ATTRIB_1                       (ATTRIB_NONE)
#define SIGNATURE_ATTRIB_0                  (ATTRIB_NONE)
#define SIGNATURE_ATTRIB_1                  (ATTRIB_NONE)
#define PARAMETER_ATTRIB_0                  (ATTRIB_BYREF | ATTRIB_OPTIONAL | ATTRIB_PARAMARRAY | ATTRIB_BRACKETED)
#define PARAMETER_ATTRIB_1                  (ATTRIB_NAME)
#define ATTRIBUTES_ATTRIB_0                 (ATTRIB_NONE)
#define ATTRIBUTES_ATTRIB_1                 (ATTRIB_NONE)
#define ATTRIBUTE_SECTION_ATTRIB_0          (ATTRIB_NONE)
#define ATTRIBUTE_SECTION_ATTRIB_1          (ATTRIB_NONE)
#define ATTRIBUTE_ATTRIB_0                  (ATTRIB_NONE)
#define ATTRIBUTE_ATTRIB_1                  (ATTRIB_NONE)
#define ATTRIBUTE_ATTRIB_PROPERTY_0         (ATTRIB_BRACKETED)
#define ATTRIBUTE_ATTRIB_PROPERTY_1         (ATTRIB_NAME)
#define BLOCK_ATTRIB_0                      (ATTRIB_NONE)
#define BLOCK_ATTRIB_1                      (ATTRIB_NONE)
#define ERASE_ATTRIB_0                      (ATTRIB_NONE)
#define REASE_ATTRIB_1                      (ATTRIB_NONE)
#define END_ATTRIB_0                        (ATTRIB_NONE)
#define RND_ATTRIB_1                        (ATTRIB_NONE)
#define STOP_ATTRIB_0                       (ATTRIB_NONE)
#define STOP_ATTRIB_1                       (ATTRIB_NONE)
#define STOP_ATTRIB_0                       (ATTRIB_NONE)
#define STOP_ATTRIB_1                       (ATTRIB_NONE)
#define RESUME_ATTRIB_0                     (ATTRIB_NONE)
#define RESUME_ATTRIB_1                     (ATTRIB_NONE)
#define RESUME_NEXT_ATTRIB_0                (ATTRIB_NONE)
#define RESUME_NEXT_ATTRIB_1                (ATTRIB_NONE)
#define ON_ERROR_ATTRIB_0                   (ATTRIB_MINUS_ONE | ATTRIB_ZERO |  ATTRIB_RESUME_NEXT | ATTRIB_BRACKETED)
#define ON_ERROR_ATTRIB_1                   (ATTRIB_NAME)
#define ERROR_ATTRIB_0                      (ATTRIB_NONE)
#define ERROR_ATTRIB_1                      (ATTRIB_NONE)
#define RAISE_EVENT_ATTRIB_0                (ATTRIB_NONE)
#define RAISE_EVENT_ATTRIB_1                (ATTRIB_NONE)
#define WITH_ATTRIB_0                       (ATTRIB_NONE)
#define WITH_ATTRIB_1                       (ATTRIB_NONE)
#define MID_ATTRIB_0                        (ATTRIB_NONE)
#define MID_ATTRIB_1                        (ATTRIB_NONE)
#define LOCK_ATTRIB_0                       (ATTRIB_NONE)
#define LOCK_ATTRIB_1                       (ATTRIB_NONE)
#define LOCK_ATTRIB_0                       (ATTRIB_NONE)
#define LOCK_ATTRIB_1                       (ATTRIB_NONE)
#define TRY_ATTRIB_0                        (ATTRIB_NONE)
#define TRY_ATTRIB_1                        (ATTRIB_NONE)
#define CATCH_ATTRIB_0                      (ATTRIB_NONE)
#define CATCH_ATTRIB_1                      (ATTRIB_NONE)
#define THROW_ATTRIB_0                      (ATTRIB_NONE)
#define THROW_ATTRIB_1                      (ATTRIB_NONE)
#define GOTO_ATTRIB_0                       (ATTRIB_NONE)
#define GOTO_ATTRIB_1                       (ATTRIB_NONE)
#define CONTINUE_ATTRIB_0                   (ATTRIB_NONE)
#define CONTINUE_ATTRIB_1                   (ATTRIB_NONE)
#define BREAK_ATTRIB_0                      (ATTRIB_NONE)
#define BREAK_ATTRIB_1                      (ATTRIB_BREAK_KIND)
#define FOR_EACH_ATTRIB_0                   (ATTRIB_NONE)
#define FOR_EACH_ATTRIB_1                   (ATTRIB_NONE)
#define FOR_ATTRIB_0                        (ATTRIB_NONE)
#define FOR_ATTRIB_1                        (ATTRIB_NONE)
#define DO_ATTRIB_0                         (ATTRIB_NONE)
#define DO_ATTRIB_1                         (ATTRIB_NONE)
#define WHILE_ATTRIB_0                      (ATTRIB_NONE)
#define WHILE_ATTRIB_1                      (ATTRIB_NONE)
#define SWITCH_ATTRIB_0                     (ATTRIB_NONE)
#define SWITCH_ATTRIB_1                     (ATTRIB_NONE)
#define CASE_ATTRIB_0                       (ATTRIB_NONE)
#define CASE_ATTRIB_1                       (ATTRIB_NONE)
#define IF_ATTRIB_0                         (ATTRIB_NONE)
#define IF_ATTRIB_1                         (ATTRIB_NONE)
#define EMPTY_STATEMENT_ATTRIB_0            (ATTRIB_NONE)
#define EMPTY_STATEMENT_ATTRIB_1            (ATTRIB_NONE)
#define LOCAL_ATTRIB_0                      (ATTRIB_STATIC | ATTRIB_SHARED | ATTRIB_INSTANCE | ATTRIB_IMPLICIT | ATTRIB_CONSTANT)
#define LOCAL_ATTRIB_1                      (ATTRIB_LINE)
#define LABEL_ATTRIB_0                      (ATTRIB_NONE)
#define LABEL_ATTRIB_1                      (ATTRIB_NONE)
#define EXPRESSION_ATTRIB_0                 (ATTRIB_NONE)
#define EXPRESSION_ATTRIB_1                 (ATTRIB_NONE)
#define EXPRESSION_STATEMENT_ATTRIB_0       (ATTRIB_NONE)
#define EXPRESSION_STATEMENT_ATTRIB_1       (ATTRIB_LINE)
#define DESIGN_STATEMENT_ATTRIB_0           (ATTRIB_NONE)
#define DESIGH_STATEMENT_ATTRIB_1           (ATTRIB_NONE)
#define SPECIAL_ATTRIB_0                    (ATTRIB_NONE)
#define SPECIAL_ATTRIB_1                    (ATTRIB_NONE)
#define ASSIGNMENT_ATTRIB_0                 (ATTRIB_NONE)
#define ASSIGNMENT_ATTRIB_1                 (ATTRIB_BINARY_OPERATOR)
#define CAST_ATTRIB_0                       (ATTRIB_IMPLICIT | ATTRIB_DIRECTCAST | ATTRIB_TRYCAST)
#define CAST_ATTRIB_1                       (ATTRIB_NONE)
#define NEWDELEGATE_ATTRIB_0                (ATTRIB_BRACKETED)
#define NEWDELEGATE_ATTRIB_1                (ATTRIB_NAME)
#define NEWARRAY_ATTRIB_0                   (ATTRIB_NONE)
#define NEWARRAY_ATTRIB_1                   (ATTRIB_NONE)
#define BOUND_ATTRIB_0                      (ATTRIB_NONE)
#define BOUND_ATTRIB_1                      (ATTRIB_NONE)
#define NEWCLASS_ATTRIB_0                   (ATTRIB_NONE)
#define NEWCLASS_ATTRIB_1                   (ATTRIB_NONE)
#define BINARYOPERATION_ATTRIB_0            (ATTRIB_NONE)
#define BINARYOPERATION_ATTRIB_1            (ATTRIB_BINARY_OPERATOR)
#define UNARYOPERATION_ATTRIB_0             (ATTRIB_NONE)
#define UNARYOPERATION_ATTRIB_1             (ATTRIB_UNARY_OPERATOR)
#define PARENTHESES_ATTRIB_0                (ATTRIB_NONE)
#define PARENTHESES_ATTRIB_1                (ATTRIB_NONE)
#define LATEACCESS_ATTRIB_0                 (ATTRIB_BRACKETED)
#define LATEACCESS_ATTRIB_1                 (ATTRIB_NAME)
#define WITHACCESS_ATTRIB_0                 (ATTRIB_NONE)
#define WITHACCESS_ATTRIB_1                 (ATTRIB_NONE)
#define NAMEREF_ATTRIB_0                    (ATTRIB_BRACKETED)
#define NAMEREF_ATTRIB_1                    (ATTRIB_NAME | ATTRIB_FULL_NAME | ATTRIB_VARIABLE_KIND)
#define ARRAYELEMENTACCESS_ATTRIB_0         (ATTRIB_NONE)
#define ARRAYELEMENTACCESS_ATTRIB_1         (ATTRIB_NONE)
#define CONSTRUCTORCALL_ATTRIB_0            (ATTRIB_NONE)
#define CONSTRUCTORCALL_ATTRIB_1            (ATTRIB_NONE)
#define METHODCALL_ATTRIB_0                 (ATTRIB_NONE)
#define METHODCALL_ATTRIB_1                 (ATTRIB_NONE)
#define ARGUMENT_ATTRIB_0                   (ATTRIB_NONE)
#define ARGUMENT_ATTRIB_1                   (ATTRIB_NONE)
#define LITERAL_ATTRIB_0                    (ATTRIB_NONE)
#define LITERAL_ATTRIB_1                    (ATTRIB_NONE)
#define ARRAY_ATTRIB_0                      (ATTRIB_NONE)
#define ARRAY_ATTRIB_1                      (ATTRIB_NONE)
#define STRING_ATTRIB_0                     (ATTRIB_NONE)
#define STRING_ATTRIB_1                     (ATTRIB_DELIMITER)
#define NUMBER_ATTRIB_0                     (ATTRIB_SCIENTIFIC_NOTATION)
#define NUMBER_ATTRIB_1                     (ATTRIB_BASE)
#define ARRAYTYPE_ATTRIB_0                  (ATTRIB_NON_ZERO_LOWER_BOUND)
#define ARRAYTYPE_ATTRIB_1                  (ATTRIB_RANK)
#define TYPE_ATTRIB_0                       (ATTRIB_IMPLICIT | ATTRIB_OUT)
#define TYPE_ATTRIB_1                       (ATTRIB_NONE)
#define RANKENTRY_ATTRIB_0                  (ATTRIB_NONE)
#define RANKENTRY_ATTRIB_1                  (ATTRIB_COUNT)

#endif DEF_ATTRIB0
#endif DEF_ATTRIB1
