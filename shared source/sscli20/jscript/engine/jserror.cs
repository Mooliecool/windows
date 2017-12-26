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

namespace Microsoft.JScript {
  
  [System.Runtime.InteropServices.GuidAttribute("268CA962-2FEF-3152-BA46-E18658B7FA4F")]
  [System.Runtime.InteropServices.ComVisible(true)]
  public enum JSError {
    //0 - 1000 legacy scripting errors, not JScript specific.
    NoError = 0,
    InvalidCall = 5, //"Invalid procedure call or argument"
    OutOfMemory = 7, // "Out of memory"
    TypeMismatch = 13, // "Type mismatch"
    OutOfStack = 28, // "Out of stack space"
    InternalError = 51, // "Internal error"
    FileNotFound = 53,  // "File not found"
    NeedObject = 424, // "Object required"
    CantCreateObject = 429, // "Can't create object"
    OLENoPropOrMethod = 438, // "Object doesn't support this property or method"
    ActionNotSupported = 445, // "Object doesn't support this action"
    NotCollection = 451, // "Object not a collection"

    //1000 - 2000 JScript errors that occur during compilation only. (regard Eval and Function as compilation). Typically used only in HandleError.
    SyntaxError = 1002, // "Syntax error"
    NoColon = 1003, // "Expected ':'"
    NoSemicolon = 1004, // "Expected ';'"
    NoLeftParen = 1005, // "Expected '('"
    NoRightParen = 1006, // "Expected ')'"
    NoRightBracket = 1007, // "Expected ']'"
    NoLeftCurly = 1008, // "Expected '{'"
    NoRightCurly = 1009, // "Expected '}'"
    NoIdentifier = 1010, // "Expected identifier"
    NoEqual = 1011, // "Expected '='"
    
    IllegalChar = 1014, // "Invalid character"
    UnterminatedString = 1015, // "Unterminated string constant"
    NoCommentEnd = 1016, // "Unterminated comment"
    
    BadReturn = 1018, // "'return' statement outside of function"
    BadBreak = 1019, // "Can't have 'break' outside of loop"
    BadContinue = 1020, // "Can't have 'continue' outside of loop"
    
    BadHexDigit = 1023, // "Expected hexadecimal digit"
    NoWhile = 1024, // "Expected 'while'"
    BadLabel = 1025, // "Label redefined"
    NoLabel = 1026, // "Label not found"
    DupDefault = 1027, // "'default' can only appear once in a 'switch' statement"
    NoMemberIdentifier = 1028, // "Expected identifier or string"
    NoCcEnd = 1029, // "Expected '@end'"
    CcOff = 1030, // "Conditional compilation is turned off"
    NotConst = 1031, // "Expected constant"
    NoAt = 1032, // "Expected '@'"
    NoCatch = 1033, // "Expected 'catch'"
    InvalidElse = 1034, // "Unmatched 'else'; no 'if' defined"
    
    NoComma = 1100, // "Expected ','"
    DupVisibility = 1101, // "Visibility modifier already definied"
    IllegalVisibility = 1102, // "Illegal visibility modifier"
    BadSwitch = 1103, // "Missing 'case' or 'default' statement"
    CcInvalidEnd = 1104, // "Unmatched '@end'; no '@if' defined"
    CcInvalidElse = 1105, // "Unmatched '@else'; no '@if' defined"
    CcInvalidElif = 1106, // "Unmatched '@elif'; no '@if' defined"
    ErrEOF = 1107, // "Expecting more source characters"
    IncompatibleVisibility = 1108, // "Incompatible visibility modifier"
    ClassNotAllowed = 1109, //"Class definition not allowed in this context"
    NeedCompileTimeConstant = 1110, //"Expression must be a compile time constant"
    DuplicateName = 1111, // "Identifier already in use"
    NeedType = 1112, // "Type name expected"
    NotInsideClass = 1113, // "Only valid inside a class definition"
    InvalidPositionDirective = 1114, // "Unknown position directive"
    MustBeEOL = 1115, // "Directive may not be followed by other code on the same line"
    
    WrongDirective = 1118,
    CannotNestPositionDirective = 1119, // "Position directive must be ended before a new one can be started"
    CircularDefinition = 1120, // "Circular definition"
    Deprecated = 1121, // "Deprecated"
    IllegalUseOfThis = 1122, //"Illegal to use 'this' in current context"
    NotAccessible = 1123, // "Not accessible from this scope"
    CannotUseNameOfClass = 1124, //"Only a constructor function can have the same name as the class it appears in"
    
    MustImplementMethod = 1128, //"Class must provide implementation"
    NeedInterface = 1129, //"Interface name expected"
    
    UnreachableCatch = 1133, //"Catch clause will never be reached"
    TypeCannotBeExtended = 1134, //"Type cannot be extended"
    UndeclaredVariable = 1135, // "Variable has not been declared"
    VariableLeftUninitialized = 1136, // "Uninitialized variables are dangerous and slow to use. Did you intend to leave it uninitialized?"
    KeywordUsedAsIdentifier = 1137, // "'xxxx' is a new reserved word and should not be used as an identifier"
    
    NotAllowedInSuperConstructorCall = 1140, //"Not allowed in a call to the super class constructor"
    NotMeantToBeCalledDirectly = 1141, //"This method is not meant to be called directly. It is probably part of a property definition."
    GetAndSetAreInconsistent = 1142, //"The get and set methods of this property do not match each other"
    InvalidCustomAttribute = 1143, //"A custom attribute class must derive from System.Attribute"
    InvalidCustomAttributeArgument = 1144, //"Only primitive types are allowed in a custom attribute constructor arguments list"
    
    InvalidCustomAttributeClassOrCtor = 1146, //"Unknown custom attribute class or constructor"
    
    TooManyParameters = 1148, //"There are too many actual parameters. The excess parameters will be ignored."
    AmbiguousBindingBecauseOfWith = 1149, //"The with statement has made the use of this name ambiguous"
    AmbiguousBindingBecauseOfEval = 1150, //"The presence of eval has made the use of this name ambiguous"
    NoSuchMember = 1151, //"Object of this type do not have such a member"
    ItemNotAllowedOnExpandoClass = 1152, //"Cannot define the property Item on an Expando class. Item is reserved for the expando fields."
    MethodNotAllowedOnExpandoClass = 1153, //"Cannot define get_Item or set_Item on an Expando class. Methods reserved for the expando fields."
    MethodClashOnExpandoSuperClass = 1155, //"Base class defines get_Item or set_Item, cannot create expando class. Methods reserved for the expando fields."
    BaseClassIsExpandoAlready = 1156, //"A base class is already marked expando; current specification will be ignored."
    AbstractCannotBePrivate = 1157, //"An abstract method cannot be private"
    NotIndexable = 1158, //"Objects of this type are not indexable"
    StaticMissingInStaticInit = 1159, //"Static intializer must specify the 'static' keyword
    MissingConstructForAttributes = 1160, //"The list of attributes does not apply to the current context."
    OnlyClassesAllowed = 1161, //"Only classes are allowed inside a package."
    ExpandoClassShouldNotImpleEnumerable = 1162, //"Expando class should not implement IEnumerable or GetEnumerator. The interface is implicitely defined on expando classes"
    NonCLSCompliantMember = 1163, //"The specified member is not CLS compliant"
    NotDeletable = 1164, //"'xxxx' can not be deleted"
    PackageExpected = 1165, //"Package name expected"
    
    UselessExpression = 1169, //"Expression has no effect. Parentheses are required for function calls"
    HidesParentMember = 1170, // "Base class contains already a member by this name"
    CannotChangeVisibility = 1171, // "Cannot change visibility specification of a base method"
    HidesAbstractInBase = 1172, // "Method hides abstract in a base class"
    NewNotSpecifiedInMethodDeclaration = 1173, // "Method matches a method in a base class. Must specify 'override' or 'hide'"
    MethodInBaseIsNotVirtual = 1174, // "Method in base class is final or not virtual, 'override' ignored. Should specify 'hide'"
    NoMethodInBaseToNew = 1175, // "There is no member in a base class to 'hide'"
    DifferentReturnTypeFromBase = 1176, // "Method in base has a different return type"
    ClashWithProperty = 1177, // "Clashes with property"
    OverrideAndHideUsedTogether = 1178, // "Cannot use 'override' and 'hide' together in a member declaraton"
    InvalidLanguageOption = 1179, // "Must specify either 'fast' or 'versionSafe'"
    NoMethodInBaseToOverride = 1180, // "There is no member in a base class to 'override'"
    NotValidForConstructor = 1181, //"Not valid for a constructor"
    CannotReturnValueFromVoidFunction = 1182, //"Cannot return a value from a void function or constructor"
    AmbiguousMatch = 1183, //"More than one method or property matches this parameter list"
    AmbiguousConstructorCall = 1184, //"More than one constructor matches this parameter list"
    SuperClassConstructorNotAccessible = 1185, //"Superclass constructor is not accessible from this scope"
    OctalLiteralsAreDeprecated = 1186, //"Octal literals are deprecated"
    VariableMightBeUnitialized = 1187, //"Variable might not be initialized"
    NotOKToCallSuper = 1188, //"It is not valid to call a super constructor from this location"
    IllegalUseOfSuper = 1189, //"It is not valid to use the super keyword in this way
    BadWayToLeaveFinally = 1190, //"It is slow and potentially confusing to leave a finally block this way. Is this intentional?"
    NoCommaOrTypeDefinitionError = 1191, // "Expected ',' or illegal type declaration, write '<Identifier> : <Type>' not '<Type> <Identifier>'"
    AbstractWithBody = 1192, // "Abstract function cannot have body"
    NoRightParenOrComma = 1193, // "Expected ',' or ')'"
    NoRightBracketOrComma = 1194, // "Expected ',' or ']'"
    ExpressionExpected = 1195, // "Expected expression"
    UnexpectedSemicolon = 1196, // "Unexpected ';'"
    TooManyTokensSkipped = 1197, // "Too many tokens have been skipped in the process of recovering from errors. The file may not be a JScript file"
    BadVariableDeclaration = 1198, // "Possible illegal variable declaration, 'var' missing, or unrecognized syntax error"
    BadFunctionDeclaration = 1199, // "Possible illegal function declaration, 'function' missing, or unrecognized syntax error"
    BadPropertyDeclaration = 1200, // "Illegal properties declaration. The getter must not have arguments and the setter must have one argument"
    
    DoesNotHaveAnAddress = 1203, //"Expression does not have an address"
    TooFewParameters = 1204, //"Not all required parameters have been supplied"
    UselessAssignment = 1205, //"Assignment creates an expando property that is immediately thrown away"
    SuspectAssignment = 1206, //"Did you intend to write an assignment here?"
    SuspectSemicolon = 1207, //"Did you intend to have an empty statement for this branch of the if statement?"
    ImpossibleConversion = 1208, //"The specified conversion or coercion is not possible"
    FinalPrecludesAbstract = 1209, //"final and abstract cannot be used together"
    NeedInstance = 1210, //"Must be an instance"
    
    CannotBeAbstract = 1212, //"Cannot be abstract unless class is marked as abstract"
    InvalidBaseTypeForEnum = 1213, //"Enum base type must be a primitive integral type"
    CannotInstantiateAbstractClass = 1214, //"It is not possible to construct an instance of an abstract class"
    ArrayMayBeCopied = 1215, //"Assigning a JScript Array to a System.Array may cause the array to be copied."
    AbstractCannotBeStatic = 1216, //"Static methods cannot be abstract"
    StaticIsAlreadyFinal = 1217, //"Static methods cannot be final"
    StaticMethodsCannotOverride = 1218, //"Static methods cannot override base class methods"
    StaticMethodsCannotHide = 1219, //"Static methods cannot hide base class methods"
    ExpandoPrecludesOverride = 1220, //"Expando methods cannot override base class methods"
    IllegalParamArrayAttribute = 1221, //"A variable argument list must be of an array type"
    ExpandoPrecludesAbstract = 1222, //"Expando methods cannot be abstract"
    ShouldBeAbstract = 1223, // "A function without a body should be abstract"
    BadModifierInInterface = 1224, // "This modifier cannot be used on an interface member"
    VarIllegalInInterface = 1226, //"Variables cannot be declared in an interface"
    InterfaceIllegalInInterface = 1227, //"Interfaces cannot be declared in an interface"
    NoVarInEnum = 1228, //"Enum member declarations should not use the 'var' keyword"
    InvalidImport = 1229, //"The import statement is not valid in this context"
    EnumNotAllowed = 1230, //"Enum definition not allowed in this context"
    InvalidCustomAttributeTarget = 1231, //"Attribute not valid for this type of declaration"
    PackageInWrongContext = 1232, //"Package definition is not allowed in this context"
    ConstructorMayNotHaveReturnType = 1233, //"A constructor function may not have a return type"
    OnlyClassesAndPackagesAllowed = 1234, //"Only classes and packages are allowed inside a library"
    InvalidDebugDirective = 1235, // "Invalid debug directive"
    CustomAttributeUsedMoreThanOnce = 1236, //"This type of attribute must be unique"
    NestedInstanceTypeCannotBeExtendedByStatic = 1237, //"A non-static nested type can only be extended by non-static type nested in the same class"
    PropertyLevelAttributesMustBeOnGetter = 1238, //"An attribute that targets the property must be specified on the property getter, if present"
    BadThrow = 1239, //"A throw must have an argument when not inside the catch block of a try statement
    ParamListNotLast = 1240, //"A variable argument list must be the last argument
    NoSuchType = 1241, //"Type could not be found, is an assembly reference missing?
    BadOctalLiteral = 1242, //"Malformed octal literal treated as decimal literal"
    InstanceNotAccessibleFromStatic = 1243, //"A non-static member is not accessible from a static scope"
    StaticRequiresTypeName = 1244, //"A static member must be accessed with the class name"
    NonStaticWithTypeName = 1245, //"A non-static member cannot be accessed with the class name"
    NoSuchStaticMember = 1246, //"Type does not have such a static member"
    SuspectLoopCondition = 1247, //"The loop condition is a function reference.  Did you intend to call the function?"
    ExpectedAssembly = 1248, //"Expected assembly"
    AssemblyAttributesMustBeGlobal = 1249, //Assembly custom attributes may not be part of another construct
    ExpandoPrecludesStatic = 1250, //"Expando methods cannot be static"
    DuplicateMethod = 1251, //"This method has the same name, parameter types and return type as another method in this class"
    NotAnExpandoFunction = 1252, //"Class members used as constructors should be marked as expando functions"
    NotValidVersionString = 1253, //"Not a valid version string"
    ExecutablesCannotBeLocalized = 1254, //Executables cannot be localized, Culture should always be empty
    StringConcatIsSlow = 1255,
    CcInvalidInDebugger = 1256, //"Conditional compilation directives and variables cannot be used in the debugger"
    ExpandoMustBePublic = 1257, //Expando methods must be public
    DelegatesShouldNotBeExplicitlyConstructed = 1258, //Delegates should not be explicitly constructed, simply use the method name
    ImplicitlyReferencedAssemblyNotFound = 1259, //"A referenced assembly depends on another assembly that is not referenced or could not be found"
    PossibleBadConversion = 1260, //"This conversion may fail at runtime"
    PossibleBadConversionFromString = 1261, //"Converting a string to a number or boolean is slow and may fail at runtime"
    InvalidResource = 1262, //"Not a valid .resources file"
    WrongUseOfAddressOf = 1263, //"The address of operator can only be used in a list of arguments"
    NonCLSCompliantType = 1264, //"The specified type is not CLS compliant"
    MemberTypeCLSCompliantMismatch = 1265, //"Class member cannot be marked CLS compliant because the class is not marked as CLS compliant"
    TypeAssemblyCLSCompliantMismatch = 1266, //"Type cannot be marked CLS compliant because the assembly is not marked as CLS compliant"
    IncompatibleAssemblyReference = 1267, //"Referenced assembly targets a different processor"
    InvalidAssemblyKeyFile = 1268, //"Assembly key file not found or contains invalid data"
    TypeNameTooLong = 1269, //"Fully qualified type name must be less than 1024 characters"
    MemberInitializerCannotContainFuncExpr = 1270, //"A class member initializer cannot contain a function expression"
    
    //5000 - 6000 JScript errors that can occur during execution. Typically (also) used in "throw new JScriptException".
    CantAssignThis = 5000, // "Cannot assign to 'this'"
    NumberExpected = 5001, // "Number expected"
    FunctionExpected = 5002, // "Function expected"
    CannotAssignToFunctionResult = 5003, // "Cannot assign to a function result"
    
    StringExpected = 5005, // "String expected"
    DateExpected = 5006, // "Date object expected"
    ObjectExpected = 5007, // "Object expected"
    IllegalAssignment = 5008, // "Illegal assignment"
    UndefinedIdentifier = 5009, // "Undefined identifier"
    BooleanExpected = 5010, // "Boolean expected"
    
    VBArrayExpected = 5013, // "VBArray expected"
    
    EnumeratorExpected = 5015, // "Enumerator object expected"
    RegExpExpected = 5016, // "Regular Expression object expected"
    RegExpSyntax = 5017, // "Syntax error in regular expression"
    
    UncaughtException = 5022, // "Exception thrown and not caught"
    InvalidPrototype = 5023, // "Function does not have a valid prototype object"
    URIEncodeError = 5024, //"The URI to be encoded contains an invalid character"
    URIDecodeError = 5025,  //"The URI to be decoded is not a valid encoding"
    FractionOutOfRange = 5026, //"The number of fractional digits is out of range"
    PrecisionOutOfRange = 5027, //"The precision is out of range"
    
    ArrayLengthConstructIncorrect = 5029, //"Array length must be a finite positive integer"
    ArrayLengthAssignIncorrect = 5030, //"Array length must be assigned a finite positive number"
    NeedArrayObject = 5031, //"| is not an Array object", "Array object expected"
    NoConstructor = 5032, // "No such constructor"
    IllegalEval = 5033, // "Eval may not be called via an alias"
    NotYetImplemented = 5034, // "Not yet implemented"    
    MustProvideNameForNamedParameter = 5035, // "Cannot provide null or empty named parameter name"
    DuplicateNamedParameter = 5036, // "Duplicate named parameter name"
    MissingNameParameter = 5037, // "The specified named parameter name it is not one of the formal parameters"
    MoreNamedParametersThanArguments = 5038, // "Too few arguments specified. The number of named parameters names cannot exceed the number of arguments passed in."
    NonSupportedInDebugger = 5039, //"The expression cannot be evaluated in the debugger"
    AssignmentToReadOnly = 5040, //"Assignment to read-only field or property"
    WriteOnlyProperty = 5041, //"The property can only be assigned to"
    IncorrectNumberOfIndices = 5042, //"The number of indices does not match the dimension of the array"
    RefParamsNonSupportedInDebugger = 5043, //"Methods with ref parameters cannot be called in the debugger"
    CannotCallSecurityMethodLateBound = 5044, //"The Deny, PermitOnly and Assert security methods cannot be called using late binding"
    CannotUseStaticSecurityAttribute = 5045, //"JScript does not support static security attributes"
    NonClsException = 5046, //"A target threw a non-CLS exception"
    
    // 6000 - 7000 Errors which are only given by the JScript debugger
    FuncEvalAborted = 6000, //"Function evaluation was aborted"
    FuncEvalTimedout = 6001, //"Function evaluation timed out"
    FuncEvalThreadSuspended = 6002, //"Function evaluation failed : the thread is suspended"
    FuncEvalThreadSleepWaitJoin = 6003, //"Function evaluation failed : the thread is sleeping, waiting on an object, or waiting for another thread to finish"
    FuncEvalBadThreadState = 6004, //"Function evaluation failed : the thread is in a bad state"
    FuncEvalBadThreadNotStarted = 6005, //"Function evaluation failed : the thread has not started"
    NoFuncEvalAllowed = 6006,  //"Function evaluation aborted. To turn property evaluation on goto Tools - Options - Debugging"
    FuncEvalBadLocation = 6007, //"Function evaluation cannot be be done when stopped at this point in the program"
    FuncEvalWebMethod = 6008, //"Cannot call a web method in the debugger"
    StaticVarNotAvailable = 6009, //"Static variable is not available"
    TypeObjectNotAvailable = 6010, //"The type object for this type is not available"
    ExceptionFromHResult = 6011, //"Exception from HRESULT : 
    SideEffectsDisallowed = 6012, //"This expression causes side effects and will not be evaluated"
 }
}
