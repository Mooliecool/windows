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
// File: predeftyp.h
//
// Contains a list of the various predefined types.
// ===========================================================================

#if !defined(PREDEFTYPEDEF)
#error Must define PREDEFTYPEDEF macro before including predeftyp.h
#endif

// The "required" column is very important. It means that the compiler requires that type to
// be present in any compilation. It should only be set for types we know will be on any
// platform. If not set, then the compiler may only require the type for certain constructs.


//         id            full type name       required  simple     numer    AggKind  fund type   elementtype,      nice name,    zero, quasi simple numer, attribute arg size serialization type,  predef attribute, arity, in mscorlib)
PREDEFTYPEDEF(PT_BYTE,   "System.Byte",         1,            1,      1,     Struct,   FT_U1,   ELEMENT_TYPE_U1,      L"byte",      0,                 0,      1,      SERIALIZATION_TYPE_U1,      PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_SHORT,  "System.Int16",        1,            1,      1,     Struct,   FT_I2,   ELEMENT_TYPE_I2,      L"short",     0,                 0,      2,      SERIALIZATION_TYPE_I2,      PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_INT,    "System.Int32",        1,            1,      1,     Struct,   FT_I4,   ELEMENT_TYPE_I4,      L"int",       0,                 0,      4,      SERIALIZATION_TYPE_I4,      PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_LONG,   "System.Int64",        1,            1,      1,     Struct,   FT_I8,   ELEMENT_TYPE_I8,      L"long",      &longZero,         0,      8,      SERIALIZATION_TYPE_I8,      PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_FLOAT,  "System.Single",       1,            1,      1,     Struct,   FT_R4,   ELEMENT_TYPE_R4,      L"float",     &doubleZero,       0,      4,      SERIALIZATION_TYPE_R4,      PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_DOUBLE, "System.Double",       1,            1,      1,     Struct,   FT_R8,   ELEMENT_TYPE_R8,      L"double",    &doubleZero,       0,      8,      SERIALIZATION_TYPE_R8,      PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_DECIMAL,"System.Decimal",      0,            1,      1,     Struct,   FT_STRUCT, ELEMENT_TYPE_END,   L"decimal",   &decimalZero,      0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_CHAR,   "System.Char",         1,            1,      0,     Struct,   FT_U2,   ELEMENT_TYPE_CHAR,    L"char",      0,                 0,      2,      SERIALIZATION_TYPE_CHAR,    PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_BOOL,   "System.Boolean",      1,            1,      0,     Struct,   FT_I1,   ELEMENT_TYPE_BOOLEAN, L"bool",      0,                 0,      1,      SERIALIZATION_TYPE_BOOLEAN, PA_COUNT, 0, 1)

// "simple" types are certain types that the compiler knows about for conversion and operator purposes.ses.
// Keep these first so that we can build conversion tables on their ordinals... Don't change the orderder
// of the simple types because it will mess up conversion tables.
// The following Quasi-Simple types are considered simple, except they are non-CLS compliant
PREDEFTYPEDEF(PT_SBYTE,  "System.SByte",         1,           1,      1,     Struct,   FT_I1,   ELEMENT_TYPE_I1,      L"sbyte",     0,                 1,      1,      SERIALIZATION_TYPE_I1,      PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_USHORT, "System.UInt16",        1,           1,      1,     Struct,   FT_U2,   ELEMENT_TYPE_U2,      L"ushort",    0,                 1,      2,      SERIALIZATION_TYPE_U2,      PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_UINT,   "System.UInt32",        1,           1,      1,     Struct,   FT_U4,   ELEMENT_TYPE_U4,      L"uint",      0,                 1,      4,      SERIALIZATION_TYPE_U4,      PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_ULONG,  "System.UInt64",        1,           1,      1,     Struct,   FT_U8,   ELEMENT_TYPE_U8,      L"ulong",     &longZero,         1,      8,      SERIALIZATION_TYPE_U8,      PA_COUNT, 0, 1)

// The special "pointer-sized int" types. Note that this are not considered numeric types from the compiler's point of view --
// they are special only in that they have special signature encodings.  
//         id            full type name       required  simple     numer    AggKind  fund type   elementtype,      nice name,    zero, quasi simple numer, attribute arg size serialization type,  predef attribute, arity, in mscorlib)
PREDEFTYPEDEF(PT_INTPTR,  "System.IntPtr",       1,     0,     0,    Struct,   FT_STRUCT,ELEMENT_TYPE_I,       NULL,          0,                 0,      0,                     0,              PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_UINTPTR, "System.UIntPtr",      1,     0,     0,    Struct,   FT_STRUCT,ELEMENT_TYPE_U,       NULL,          0,                 0,      0,                     0,              PA_COUNT, 0, 1)

//         id            full type name       required  simple     numer    AggKind  fund type   elementtype,      nice name,    zero, quasi simple numer, attribute arg size serialization type,  predef attribute, arity, in mscorlib)
PREDEFTYPEDEF(PT_OBJECT, "System.Object",        1,     0,     0,    Class,    FT_REF,  ELEMENT_TYPE_OBJECT,   L"object",     0,                 0,      -1,     SERIALIZATION_TYPE_TAGGED_OBJECT,  PA_COUNT, 0, 1)
// THE ORDER ABOVE HERE IS IMPORTANT!!!  It is used in tables in both fncbind and ilgen


PREDEFTYPEDEF(PT_STRING, "System.String",             1,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_STRING,   L"string",     0,                 0,      -1,     SERIALIZATION_TYPE_STRING,  PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_DELEGATE,"System.Delegate",          1,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_MULTIDEL,"System.MulticastDelegate", 1,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_ARRAY,   "System.Array",             1,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_EXCEPTION,"System.Exception",        1,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_TYPE,   "System.Type",               1,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      -1,     SERIALIZATION_TYPE_TYPE,    PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_CRITICAL,"System.Threading.Monitor", 0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_VALUE,   "System.ValueType",         1,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_ENUM,    "System.Enum",              1,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)


// predefined attribute types
PREDEFTYPEDEF(PT_SECURITYATTRIBUTE, "System.Security.Permissions.CodeAccessSecurityAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_SECURITYPERMATTRIBUTE, "System.Security.Permissions.SecurityPermissionAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_UNVERIFCODEATTRIBUTE, "System.Security.UnverifiableCodeAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_DEBUGGABLEATTRIBUTE, "System.Diagnostics.DebuggableAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_DEBUGGABLE, 0, 1)
PREDEFTYPEDEF(PT_MARSHALBYREF, "System.MarshalByRefObject",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_CONTEXTBOUND, "System.ContextBoundObject",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_IN,            "System.Runtime.InteropServices.InAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_IN, 0, 1)
PREDEFTYPEDEF(PT_OUT,           "System.Runtime.InteropServices.OutAttribute",
                                                      1,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_OUT, 0, 1)
PREDEFTYPEDEF(PT_ATTRIBUTE, "System.Attribute",       1,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_ATTRIBUTEUSAGE, "System.AttributeUsageAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_ATTRIBUTEUSAGE, 0, 1)
PREDEFTYPEDEF(PT_ATTRIBUTETARGETS, "System.AttributeTargets",
                                                      0,     0,     0,   Enum,    FT_STRUCT, ELEMENT_TYPE_END,    NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_OBSOLETE, "System.ObsoleteAttribute",0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_OBSOLETE, 0, 1)
PREDEFTYPEDEF(PT_CONDITIONAL, "System.Diagnostics.ConditionalAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_CONDITIONAL, 0, 1)
PREDEFTYPEDEF(PT_CLSCOMPLIANT, "System.CLSCompliantAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_CLSCOMPLIANT, 0, 1)
PREDEFTYPEDEF(PT_GUID, "System.Runtime.InteropServices.GuidAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_GUID, 0, 1)
PREDEFTYPEDEF(PT_DEFAULTMEMBER, "System.Reflection.DefaultMemberAttribute",
                                                      1,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_DEFAULTMEMBER, 0, 1)
PREDEFTYPEDEF(PT_PARAMS, "System.ParamArrayAttribute",1,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_PARAMARRAY, 0, 1)
PREDEFTYPEDEF(PT_COMIMPORT, "System.Runtime.InteropServices.ComImportAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COMIMPORT, 0, 1)
PREDEFTYPEDEF(PT_FIELDOFFSET, "System.Runtime.InteropServices.FieldOffsetAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_STRUCTOFFSET, 0, 1)
PREDEFTYPEDEF(PT_STRUCTLAYOUT, "System.Runtime.InteropServices.StructLayoutAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_STRUCTLAYOUT, 0, 1)
PREDEFTYPEDEF(PT_LAYOUTKIND, "System.Runtime.InteropServices.LayoutKind",
                                                      0,     0,     0,   Enum,    FT_STRUCT, ELEMENT_TYPE_END,    NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_MARSHALAS, "System.Runtime.InteropServices.MarshalAsAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_DLLIMPORT, "System.Runtime.InteropServices.DllImportAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_DLLIMPORT, 0, 1)
PREDEFTYPEDEF(PT_INDEXERNAME, "System.Runtime.CompilerServices.IndexerNameAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_NAME, 0, 1)
PREDEFTYPEDEF(PT_DECIMALCONSTANT, "System.Runtime.CompilerServices.DecimalConstantAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_REQUIRED, "System.Runtime.CompilerServices.RequiredAttributeAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_REQUIRED, 0, 1)
PREDEFTYPEDEF(PT_DEFAULTVALUE, "System.Runtime.InteropServices.DefaultParameterValueAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_DEFAULTVALUE, 0, 0)
PREDEFTYPEDEF(PT_UNMANAGEDFUNCTIONPOINTER, "System.Runtime.InteropServices.UnmanagedFunctionPointerAttribute",
                                                      0,     0,     0,   Class,   FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_UNMANAGEDFUNCTIONPOINTER, 0, 0)

// predefined types for the BCL
PREDEFTYPEDEF(PT_REFANY,  "System.TypedReference",    0,     0,     0,   Struct,   FT_STRUCT,ELEMENT_TYPE_TYPEDBYREF, NULL,       0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_ARGITERATOR,   "System.ArgIterator", 0,     0,     0,   Struct,   FT_STRUCT,ELEMENT_TYPE_END,     NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_TYPEHANDLE, "System.RuntimeTypeHandle",
                                                      1,     0,     0,   Struct,   FT_STRUCT,ELEMENT_TYPE_END,     NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_FIELDHANDLE, "System.RuntimeFieldHandle",
                                                      1,     0,     0,   Struct,   FT_STRUCT,ELEMENT_TYPE_END,     NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_METHODHANDLE, "System.RuntimeMethodHandle",
                                                      0,     0,     0,   Struct,   FT_STRUCT,ELEMENT_TYPE_END,     NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_ARGUMENTHANDLE, "System.RuntimeArgumentHandle",
                                                      0,     0,     0,   Struct,   FT_STRUCT,ELEMENT_TYPE_END,     NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_HASHTABLE, "System.Collections.Hashtable",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,     NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_G_DICTIONARY, "System.Collections.Generic.Dictionary",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,     NULL,          0,                 0,      0,      0,                          PA_COUNT, 2, 1)
PREDEFTYPEDEF(PT_IASYNCRESULT, "System.IAsyncResult", 0,     0,     0,   Interface,FT_REF,  ELEMENT_TYPE_END,     NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_ASYNCCBDEL, "System.AsyncCallback",  0,     0,     0,   Delegate, FT_REF,  ELEMENT_TYPE_END,     NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_SECURITYACTION,"System.Security.Permissions.SecurityAction",
                                                      0,     0,     0,   Enum,     FT_I4,   ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_IDISPOSABLE, "System.IDisposable",   1,     0,     0,   Interface,FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_IENUMERABLE, "System.Collections.IEnumerable",
                                                      1,     0,     0,   Interface,FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_IENUMERATOR, "System.Collections.IEnumerator",
                                                      1,     0,     0,   Interface,FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_SYSTEMVOID, "System.Void",
                                                      1,     0,     0,   Struct,   FT_STRUCT, ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)

PREDEFTYPEDEF(PT_RUNTIMEHELPERS, "System.Runtime.CompilerServices.RuntimeHelpers",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,     NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)

// signature MODIFIER for marking volatile fields
PREDEFTYPEDEF(PT_VOLATILEMOD, "System.Runtime.CompilerServices.IsVolatile",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
// Sets the CoClass for a COM interface wrapper
PREDEFTYPEDEF(PT_COCLASS,    "System.Runtime.InteropServices.CoClassAttribute",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COCLASS, 0, 1)

// For instantiating a type variable.
PREDEFTYPEDEF(PT_ACTIVATOR,  "System.Activator",      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)

// Generic variants of enumerator interfaces
PREDEFTYPEDEF(PT_G_IENUMERABLE, "System.Collections.Generic.IEnumerable",
                                                      0,     0,     0,   Interface,FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 1, 1)
PREDEFTYPEDEF(PT_G_IENUMERATOR, "System.Collections.Generic.IEnumerator",
                                                      0,     0,     0,   Interface,FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 1, 1)

// Nullable<T>
PREDEFTYPEDEF(PT_G_OPTIONAL, "System.Nullable",  0,     0,     0,   Struct,   FT_STRUCT,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 1, 1)

// Marks a fixed buffer field
PREDEFTYPEDEF(PT_FIXEDBUFFER, "System.Runtime.CompilerServices.FixedBufferAttribute",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_FIXED, 0, 1)

// Sets the module-level default character set marshalling
PREDEFTYPEDEF(PT_DEFAULTCHARSET, "System.Runtime.InteropServices.DefaultCharSetAttribute",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_DEFAULTCHARSET, 0, 1)

// Used to disable string interning
PREDEFTYPEDEF(PT_COMPILATIONRELAXATIONS, "System.Runtime.CompilerServices.CompilationRelaxationsAttribute",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COMPILATIONRELAXATIONS, 0, 1)

// Used to enable wrapped exceptions
PREDEFTYPEDEF(PT_RUNTIMECOMPATIBILITY, "System.Runtime.CompilerServices.RuntimeCompatibilityAttribute",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_RUNTIMECOMPATIBILITY, 0, 1)

// Used for friend assmeblies
PREDEFTYPEDEF(PT_FRIENDASSEMBLY, "System.Runtime.CompilerServices.InternalsVisibleToAttribute",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_FRIENDASSEMBLY, 0, 1)

// Used to hide compiler-generated code from the debugger
PREDEFTYPEDEF(PT_DEBUGGERHIDDEN, "System.Diagnostics.DebuggerHiddenAttribute",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)

// Used for type forwarders
PREDEFTYPEDEF(PT_TYPEFORWARDER, "System.Runtime.CompilerServices.TypeForwardedToAttribute",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_TYPEFORWARDER, 0, 1)

// Used to warn on usage of this instead of command-line options
PREDEFTYPEDEF(PT_KEYFILE, "System.Reflection.AssemblyKeyFileAttribute",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_KEYFILE, 0, 1)
PREDEFTYPEDEF(PT_KEYNAME, "System.Reflection.AssemblyKeyNameAttribute",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_KEYNAME, 0, 1)
PREDEFTYPEDEF(PT_DELAYSIGN, "System.Reflection.AssemblyDelaySignAttribute",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_DELAYSIGN, 0, 1)
PREDEFTYPEDEF(PT_NOTSUPPORTEDEXCEPTION, "System.NotSupportedException",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_INTERLOCKED, "System.Threading.Interlocked",
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)
PREDEFTYPEDEF(PT_COMPILERGENERATED, "System.Runtime.CompilerServices.CompilerGeneratedAttribute", 
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)

PREDEFTYPEDEF(PT_UNSAFEVALUETYPE, "System.Runtime.CompilerServices.UnsafeValueTypeAttribute", 
                                                      0,     0,     0,   Class,    FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 0, 1)


PREDEFTYPEDEF(PT_G_ICOLLECTION, "System.Collections.Generic.ICollection",
                                                      0,     0,     0,   Interface,FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 1, 1)

PREDEFTYPEDEF(PT_G_ILIST, "System.Collections.Generic.IList",
                                                      0,     0,     0,   Interface,FT_REF,  ELEMENT_TYPE_END,      NULL,          0,                 0,      0,      0,                          PA_COUNT, 1, 1)

// Auto undef the macro.
#undef PREDEFTYPEDEF
