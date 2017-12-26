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
// MTYPES.H -
//
// Defines the mapping between MARSHAL_TYPE constants and their Marshaler
// classes. Used to generate all the enums and tables.
//





DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_GENERIC_1,       CopyMarshaler1)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_GENERIC_U1,      CopyMarshalerU1)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_GENERIC_2,       CopyMarshaler2)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_GENERIC_U2,      CopyMarshalerU2)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_GENERIC_4,       CopyMarshaler4)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_GENERIC_U4,      CopyMarshalerU4)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_GENERIC_8,       CopyMarshaler8)

DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_WINBOOL,         WinBoolMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_CBOOL,           CBoolMarshaler)

DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_ANSICHAR,        AnsiCharMarshaler)

DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_FLOAT,           FloatMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_DOUBLE,          DoubleMarshaler)

DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_CURRENCY,        CurrencyMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_DECIMAL,         DecimalMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_DECIMAL_PTR,     DecimalPtrMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_GUID,            GuidMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_GUID_PTR,        GuidPtrMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_DATE,            DateMarshaler)
 
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_LPWSTR,          WSTRMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_LPSTR,           CSTRMarshaler)

DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_LPWSTR_BUFFER,   WSTRBufferMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_LPSTR_BUFFER,    CSTRBufferMarshaler)

DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_INTERFACE,       InterfaceMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_NATIVEARRAY,     NativeArrayMarshaler)

DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_ASANYA,          AsAnyAMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_ASANYW,          AsAnyWMarshaler)

DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_DELEGATE,        DelegateMarshaler)

DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_BLITTABLEPTR,    BlittablePtrMarshaler)


DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_LAYOUTCLASSPTR,  LayoutClassPtrMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_ARRAYWITHOFFSET, ArrayWithOffsetMarshaler)

DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_BLITTABLEVALUECLASS,             BlittableValueClassMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_VALUECLASS,                      ValueClassMarshaler)

DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_REFERENCECUSTOMMARSHALER,        ReferenceCustomMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_ARGITERATOR,                     ArgIteratorMarshaler)

DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_BLITTABLEVALUECLASSWITHCOPYCTOR, BlittableValueClassWithCopyCtorMarshaler)


DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_HANDLEREF,                       HandleRefMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_SAFEHANDLE,                      SafeHandleMarshaler)
DEFINE_MARSHALER_TYPE(MARSHAL_TYPE_CRITICALHANDLE,                  CriticalHandleMarshaler)



#undef DEFINE_MARSHALER_TYPE

