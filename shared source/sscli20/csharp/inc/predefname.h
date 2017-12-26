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
// File: predefname.h
//
// Contains a list of the various predefined names.
// ===========================================================================

#if !defined(PREDEFNAMEDEF)
#error Must define PREDEFNAMEDEF macro before including predefname.h
#endif

//               id             text    )
PREDEFNAMEDEF(PN_CTOR,       ".ctor")          // name of constructors.
PREDEFNAMEDEF(PN_DTOR,       "Finalize")          // name of destructors.
PREDEFNAMEDEF(PN_STATCTOR,   ".cctor")        // name of static constructors.
PREDEFNAMEDEF(PN_ENUMVALUE,  "value__")           // name of special value field in enums.

PREDEFNAMEDEF(PN_PTR,        "*")               // name used for pointer types
PREDEFNAMEDEF(PN_NUB,        "?*")              // name used for nullable NUBSYM types
PREDEFNAMEDEF(PN_PINNED,     "@")               // name used for pinned types
PREDEFNAMEDEF(PN_OUTPARAM,   "#")               // name used for out param types
PREDEFNAMEDEF(PN_REFPARAM,   "&")               // name used for ref param types
PREDEFNAMEDEF(PN_ARRAY0,     "[X\001")           // name used for unknown rank arrays
PREDEFNAMEDEF(PN_ARRAY1,     "[X\002")           // name used for rank 1 arrays
PREDEFNAMEDEF(PN_ARRAY2,     "[X\003")           // name used for rank 2 arrays
PREDEFNAMEDEF(PN_GARRAY0,    "[G\001")           // name used for unknown rank arrays
PREDEFNAMEDEF(PN_GARRAY1,    "[G\002")           // name used for rank 1 arrays
PREDEFNAMEDEF(PN_GARRAY2,    "[G\003")           // name used for rank 2 arrays
PREDEFNAMEDEF(PN_THIS,       "<this>")          // name used for current instance pointers
PREDEFNAMEDEF(PN_MAIN,       "Main")            // name for the startup method
PREDEFNAMEDEF(PN_EMPTY,      "")                // an empty name
PREDEFNAMEDEF(PN_DEFAULT_CASE,"default:")        // name of default label
PREDEFNAMEDEF(PN_INVOKE,     "Invoke")          // delegate's Invoke
PREDEFNAMEDEF(PN_BEGININVOKE,"BeginInvoke")     // delegate's BeginInvoke
PREDEFNAMEDEF(PN_ENDINVOKE,  "EndInvoke")       // delegate's EndInvoke
PREDEFNAMEDEF(PN_VALUE,      "value")           // argument to set accessor
PREDEFNAMEDEF(PN_LENGTH,     "Length")          // array's Length
PREDEFNAMEDEF(PN_INDEXER,    "Item")            // name for indexer property
PREDEFNAMEDEF(PN_INDEXERINTERNAL, "$Item$")     // internal name for indexer property
PREDEFNAMEDEF(PN_COMBINE,    "Combine")         // Delegate Combine method
PREDEFNAMEDEF(PN_REMOVE,     "Remove")          // Delegate Remove method
PREDEFNAMEDEF(PN_COMPAREEXCHANGE, "CompareExchange") // System.Threading.Interlocked.CompareExchange
PREDEFNAMEDEF(PN_ITERSTATE,  "<>1__state")      // Iterator's state counter field
PREDEFNAMEDEF(PN_ITERCURRENT, "<>2__current")   // Iterator's current value field
PREDEFNAMEDEF(PN_HOISTEDTHIS, "<>4__this")      // Hoisted copy of <this> pointer
PREDEFNAMEDEF(PN_TYVARCONTAINER, "<TypeVariableContainer>") // name for container for a AGGSYM's type variables

// internal method name for conversion operators
// these get mangled when converted to CLS names
PREDEFNAMEDEF(PN_OPEXPLICITMN,          "op_Explicit") // name for explicit conversion operators
PREDEFNAMEDEF(PN_OPIMPLICITMN,          "op_Implicit") // name for implicit conversion operators

PREDEFNAMEDEF(PN_OPEXPLICIT,            "explicit") // name for explicit conversion operators
PREDEFNAMEDEF(PN_OPIMPLICIT,            "implicit") // name for implicit conversion operators

// CLS method names for user defined operators
PREDEFNAMEDEF(PN_OPUNARYPLUS,           "op_UnaryPlus")
PREDEFNAMEDEF(PN_OPUNARYMINUS,          "op_UnaryNegation")
PREDEFNAMEDEF(PN_OPCOMPLEMENT,          "op_OnesComplement")
PREDEFNAMEDEF(PN_OPINCREMENT,           "op_Increment")
PREDEFNAMEDEF(PN_OPDECREMENT,           "op_Decrement")
PREDEFNAMEDEF(PN_OPPLUS,                "op_Addition")
PREDEFNAMEDEF(PN_OPMINUS,               "op_Subtraction")
PREDEFNAMEDEF(PN_OPMULTIPLY,            "op_Multiply")
PREDEFNAMEDEF(PN_OPDIVISION,            "op_Division")
PREDEFNAMEDEF(PN_OPMODULUS,             "op_Modulus")
PREDEFNAMEDEF(PN_OPXOR,                 "op_ExclusiveOr")
PREDEFNAMEDEF(PN_OPBITWISEAND,          "op_BitwiseAnd")
PREDEFNAMEDEF(PN_OPBITWISEOR,           "op_BitwiseOr")
PREDEFNAMEDEF(PN_OPLEFTSHIFT,           "op_LeftShift")
PREDEFNAMEDEF(PN_OPRIGHTSHIFT,          "op_RightShift")
PREDEFNAMEDEF(PN_OPEQUALS,              "op_Equals")
PREDEFNAMEDEF(PN_OPCOMPARE,             "op_Compare")

PREDEFNAMEDEF(PN_OPEQUALITY,            "op_Equality")
PREDEFNAMEDEF(PN_OPINEQUALITY,          "op_Inequality")
PREDEFNAMEDEF(PN_OPGREATERTHAN,         "op_GreaterThan")
PREDEFNAMEDEF(PN_OPLESSTHAN,            "op_LessThan")
PREDEFNAMEDEF(PN_OPGREATERTHANOREQUAL,  "op_GreaterThanOrEqual")
PREDEFNAMEDEF(PN_OPLESSTHANOREQUAL,     "op_LessThanOrEqual")
PREDEFNAMEDEF(PN_OPTRUE,                "op_True")
PREDEFNAMEDEF(PN_OPFALSE,               "op_False")
PREDEFNAMEDEF(PN_OPNEGATION,            "op_LogicalNot")


PREDEFNAMEDEF(PN_GETTYPE,    "GetType")         // Object.GetType() -> Type
PREDEFNAMEDEF(PN_GETTYPEFROMHANDLE,"GetTypeFromHandle")         // Object.GetType() -> Type
PREDEFNAMEDEF(PN_ISINSTANCEOF,"IsInstanceOfType")           // A is B
PREDEFNAMEDEF(PN_ENTER,      "Enter")           // CS.Enter(object)
PREDEFNAMEDEF(PN_EXIT,       "Exit")            // CS.Exit(object)
PREDEFNAMEDEF(PN_EQUALS,     "Equals")          // String.Equals for == on strings.
PREDEFNAMEDEF(PN_STRCONCAT,  "Concat")          // String.Concat for concatining objects/strings.
PREDEFNAMEDEF(PN_GETENUMERATOR, "GetEnumerator")    // For iterators...
PREDEFNAMEDEF(PN_MOVENEXT,      "MoveNext")         // For iterators...
PREDEFNAMEDEF(PN_CURRENT,       "Current")          // For iterators...
PREDEFNAMEDEF(PN_RESET,         "Reset")            // For iterators...
PREDEFNAMEDEF(PN_GETLOWERBOUND, "GetLowerBound")    // For array iterators
PREDEFNAMEDEF(PN_GETUPPERBOUND, "GetUpperBound")    // For array iterators
PREDEFNAMEDEF(PN_ADD,           "Add")              // For hashtable (string switch)
PREDEFNAMEDEF(PN_GETITEM,       "get_Item")         // for hashtable (string switch)
PREDEFNAMEDEF(PN_TRYGETVALUE,   "TryGetValue")  // for dictionary (string switch)
PREDEFNAMEDEF(PN_GETLENGTH,     "get_Length")       // for foreach on strings
PREDEFNAMEDEF(PN_GETCHARS,      "get_Chars")        // for foreach on strings
PREDEFNAMEDEF(PN_GETHASHCODE,   "GetHashCode")      // hash code
PREDEFNAMEDEF(PN_DISPOSE,       "Dispose")            // for using
PREDEFNAMEDEF(PN_INITIALIZEARRAY,"InitializeArray")   // for arrays of constants
PREDEFNAMEDEF(PN_OFFSETTOSTRINGDATA, "OffsetToStringData") // for fixed on string

PREDEFNAMEDEF(PN_ARGLIST,       "__arglist")       // for oldstyle varargs
PREDEFNAMEDEF(PN_NATURALINT,    "Natural Int")       // for temps on array indexing (this name is used strictly to disambiguiate the symbol)

//
// predefined attributes
//
PREDEFNAMEDEF(PN_VALIDON,      "ValidOn")           // attribute attribute named argument
PREDEFNAMEDEF(PN_ALLOWMULTIPLE,"AllowMultiple")     // attribute attribute named argument
PREDEFNAMEDEF(PN_INHERITED,    "Inherited")         // attribute attribute named argument
PREDEFNAMEDEF(PN_REQUESTMINIMUM, "RequestMinimum")
PREDEFNAMEDEF(PN_SKIPVERIFICATION, "SkipVerification")

PREDEFNAMEDEF(PN_SubType,      "SubType")
PREDEFNAMEDEF(PN_Size,         "Size")
PREDEFNAMEDEF(PN_ComType,      "ComType")
PREDEFNAMEDEF(PN_Marshaller,   "Marshaller")
PREDEFNAMEDEF(PN_Cookie,       "Cookie")


PREDEFNAMEDEF(PN_ENTRYPOINT, "EntryPoint")
PREDEFNAMEDEF(PN_CharSet,    "CharSet")
PREDEFNAMEDEF(PN_SetLastError,"SetLastError")
PREDEFNAMEDEF(PN_EXACTSPELLING, "ExactSpelling")
PREDEFNAMEDEF(PN_CALLINGCONVENTION, "CallingConvention")
PREDEFNAMEDEF(PN_SEQUENTIAL, "Sequential")
PREDEFNAMEDEF(PN_EXPLICIT,   "Explicit")
PREDEFNAMEDEF(PN_WRAPNONEXCEPTIONTHROWS,    "WrapNonExceptionThrows")

PREDEFNAMEDEF(PN_Pack,       "Pack")

PREDEFNAMEDEF(PN_TYPEPARAMS, "TypeParamNames")

PREDEFNAMEDEF(PN_OBSOLETE_CLASS,        "System.ObsoleteAttribute")     // Attribute marking obsolete items.
PREDEFNAMEDEF(PN_ATTRIBUTEUSAGE_CLASS,  "System.AttributeUsageAttribute")// Attribute marking attributes.
PREDEFNAMEDEF(PN_CONDITIONAL_CLASS,     "System.Diagnostics.ConditionalAttribute")// Attribute marking conditional methods
PREDEFNAMEDEF(PN_DECIMALLITERAL_CLASS,  "System.Runtime.CompilerServices.DecimalConstantAttribute")               // Attribute marking decimal constants
PREDEFNAMEDEF(PN_DEPRECATEDHACK_CLASS,  "Deprecated")
PREDEFNAMEDEF(PN_PARAMS_CLASS,          "System.ParamArrayAttribute")
PREDEFNAMEDEF(PN_CLSCOMPLIANT,          "System.CLSCompliantAttribute")          // mark as CLS compliant or not
PREDEFNAMEDEF(PN_DEFAULTMEMBER_CLASS,   "System.Reflection.DefaultMemberAttribute")  // marks the default member of a class
PREDEFNAMEDEF(PN_DEFAULTMEMBER_CLASS2,  "System.Runtime.InteropServices.DefaultMemberAttribute")
PREDEFNAMEDEF(PN_PARAMARRAY_CLASS,      "System.ParamArrayAttribute")  // marks a param array
PREDEFNAMEDEF(PN_OUT_CLASS,             "System.Runtime.InteropServices.OutAttribute")  // marks an out parameter
PREDEFNAMEDEF(PN_REQUIRED_CLASS,        "System.Runtime.CompilerServices.RequiredAttributeAttribute")  // marks a 'special' type
PREDEFNAMEDEF(PN_BROWSABLE_CLASS,       "System.ComponentModel.EditorBrowsableAttribute")  // marks members as advanced/normal/hidden
PREDEFNAMEDEF(PN_COCLASS_CLASS,         "System.Runtime.InteropServices.CoClassAttribute") // Attribute that sets the coclass of an interface
PREDEFNAMEDEF(PN_FIXEDBUFFER_CLASS,     "System.Runtime.CompilerServices.FixedBufferAttribute") // marks a fixed buffer field with the element type and length
PREDEFNAMEDEF(PN_COMPILATIONRELAXATIONS_CLASS, "System.Runtime.CompilerServices.CompilationRelaxationsAttribute")
PREDEFNAMEDEF(PN_RUNTIMECOMPATIBILTY_CLASS, "System.Runtime.CompilerServices.RuntimeCompatibilityAttribute")
PREDEFNAMEDEF(PN_DELEGATECTORPARAM0, "object")
PREDEFNAMEDEF(PN_DELEGATECTORPARAM1, "method")

PREDEFNAMEDEF(PN_DELEGATEBIPARAM0, "callback")
PREDEFNAMEDEF(PN_DELEGATEBIPARAM1, "object")

PREDEFNAMEDEF(PN_DELEGATEEIPARAM0, "result")
PREDEFNAMEDEF(PN_DELEGATEEIPARAM0ALT, "__result")            // used if a delegate's parameter is already named "result"

// attribute location overrides
PREDEFNAMEDEF(PN_ASSEMBLY,  "assembly")
PREDEFNAMEDEF(PN_MODULE,    "module")
PREDEFNAMEDEF(PN_TYPE,      "type")
PREDEFNAMEDEF(PN_METHOD,    "method")
PREDEFNAMEDEF(PN_PROPERTY,  "property")
PREDEFNAMEDEF(PN_FIELD,     "field")
PREDEFNAMEDEF(PN_PARAM,     "param")
PREDEFNAMEDEF(PN_EVENT,     "event")

// Since the compiler and EE share the same nametable instance,
// we cannot have disperate defines in this table...
PREDEFNAMEDEF(PN_TRUE,      "true")
PREDEFNAMEDEF(PN_FALSE,     "false")
PREDEFNAMEDEF(PN_NULL,      "null")
PREDEFNAMEDEF(PN_BASE,      "base")
PREDEFNAMEDEF(PN_DEBUGGERDISPLAYATTR, "System.Diagnostics.DebuggerDisplayAttribute")
PREDEFNAMEDEF(PN_DEBUGGERVISUALIZERPROXYATTR, "System.Diagnostics.DebuggerVisualizerAttribute")
PREDEFNAMEDEF(PN_DEBUGGERTYPEPROXYATTR, "System.Diagnostics.DebuggerTypeProxyAttribute")
PREDEFNAMEDEF(PN_DEBUGGERBROWSABLEATTR, "System.Diagnostics.DebuggerBrowsableAttribute")
PREDEFNAMEDEF(PN_INTEROPGUIDATTR, "System.Runtime.InteropServices.GuidAttribute")
PREDEFNAMEDEF(PN_SECONDARYSYM, "secondary sym")
PREDEFNAMEDEF(PN_PURETHIS,   "this")
PREDEFNAMEDEF(PN_GETINITIALDATA, "GetInitialData")
PREDEFNAMEDEF(PN_INPLACEUPDATE, "InPlaceUpdate")
PREDEFNAMEDEF(PN_CREATEREPLACEMENTOBJECT, "CreateReplacementObject")
PREDEFNAMEDEF(PN_CREATESOURCE, "CreateSource")
PREDEFNAMEDEF(PN_CREATENONVISUALPROXYCONSTRUCTOR, "CreateNonVisualProxyConstructor")
PREDEFNAMEDEF(PN_CREATENONVISUALPROXYOBJECT, "CreateNonVisualProxyObject")
PREDEFNAMEDEF(PN_TOSTRING,  "ToString")
PREDEFNAMEDEF(PN_DEXCEPTION, "$exception")
PREDEFNAMEDEF(PN_EXCEPTION, "exception")
PREDEFNAMEDEF(PN_INFINITY, "Infinity")
PREDEFNAMEDEF(PN_NINFINITY, "-Infinity")
PREDEFNAMEDEF(PN_NAN, "NaN")
PREDEFNAMEDEF(PN_SOAPMETHOD, "System.Runtime.Remoting.Metadata.SoapMethodAttribute")
PREDEFNAMEDEF(PN_SOAPDOCUMENTMETHOD, "System.Web.Services.Protocols.SoapDocumentMethodAttribute")
PREDEFNAMEDEF(PN_SORTEDCHILDREN, "sorted children")
PREDEFNAMEDEF(PN_CREATEDELEGATE, "CreateDelegate")
PREDEFNAMEDEF(PN_SIZEOF, "SizeOf")
PREDEFNAMEDEF(PN_FLAGS, "System.FlagsAttribute")
PREDEFNAMEDEF(PN_LOAD, "Load")
PREDEFNAMEDEF(PN_LOADFROM, "LoadFrom")
PREDEFNAMEDEF(PN_GETDATA, "GetData")
PREDEFNAMEDEF(PN_GETSOURCE, "GetSource")
PREDEFNAMEDEF(PN_ISINSTANCEOFTYPE, "IsInstanceOfType")
PREDEFNAMEDEF(PN_REPLACE, "Replace")
PREDEFNAMEDEF(PN_VISUALIZEROBJECTSOURCE, "System.Diagnostics.VisualizerObjectSource")
PREDEFNAMEDEF(PN_STRINGVALUEMEMBER, "m_StringValue")
PREDEFNAMEDEF(PN_SQLSTRINGVALUEMEMBER, "m_value")
PREDEFNAMEDEF(PN_H, "h")
PREDEFNAMEDEF(PN_D, "d")
PREDEFNAMEDEF(PN_RAW, "raw")
PREDEFNAMEDEF(PN_PRIVATE, "hidden")
PREDEFNAMEDEF(PN_NQ, "nq")
PREDEFNAMEDEF(PN_T, "t")
PREDEFNAMEDEF(PN_AC, "ac")
PREDEFNAMEDEF(PN_BUCKETS, "buckets")
PREDEFNAMEDEF(PN_KEY, "key")
PREDEFNAMEDEF(PN_VAL, "val")
PREDEFNAMEDEF(PN_UITEMS, "_items")
PREDEFNAMEDEF(PN_USIZE, "_size")
PREDEFNAMEDEF(PN_ANONYMOUSMETHOD, "AnonymousMethod")
PREDEFNAMEDEF(PN_CREATESTATICDELEGATE, "CreateStaticDelegate")
PREDEFNAMEDEF(PN_CREATEINSTANCEDELEGATE, "CreateInstanceDelegate")

PREDEFNAMEDEF(PN_CREATEINSTANCE,     "CreateInstance")
PREDEFNAMEDEF(PN_FIXEDELEMENT,       "FixedElementField")
PREDEFNAMEDEF(PN_EXTERNALIASCONTAINER, "<ExternAliasContainer>") // name for container for a EXTERNALIASSYMs
PREDEFNAMEDEF(PN_GLOBAL,             "global")

PREDEFNAMEDEF(PN_FRIENDASSEMBLY_CLASS,  "System.Runtime.CompilerServices.InternalsVisibleToAttribute")
PREDEFNAMEDEF(PN_ERROR_ASSEM, ":ErrAssem:")

PREDEFNAMEDEF(PN_HASVALUE, "HasValue")
PREDEFNAMEDEF(PN_CAP_VALUE, "Value")
PREDEFNAMEDEF(PN_GET_VALUE_OR_DEF, "GetValueOrDefault")
PREDEFNAMEDEF(PN_NOCAP_HASVALUE, "hasValue")

PREDEFNAMEDEF(PN_DEBUGGINGMODES_CLASS,  "DebuggingModes")
PREDEFNAMEDEF(PN_DISABLEOPTIMIZATIONS,  "DisableOptimizations")
PREDEFNAMEDEF(PN_DEFAULT,               "Default")
PREDEFNAMEDEF(PN_IGNORESYMBOLSTORESEQUENCEPOINTS,
                                        "IgnoreSymbolStoreSequencePoints")
PREDEFNAMEDEF(PN_ENABLEEDITANDCONTINUE, "EnableEditAndContinue")
