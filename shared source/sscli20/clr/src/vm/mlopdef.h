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
// MLOPDEF.H -
//
// Defines ML opcodes.
//
//
// Columns:
//
//  op   -- size in bytes of the instruction, excluding the opcode byte.
//          variable-length instructions not allowed.
//
//  fC   -- 1 if instruction makes use of the CleanupWorkList passed to
//          RunML. The EE optimizes out the creation of a CleanupWorkList
//          if the ML stream for a method makes no use of it.
//
//  loc  -- # of bytes of localspace required by instruction.
//          Currently, this is stored only for _DEBUG builds.
//
//  XHndl - requires extra handles for GC protection
// 
//  DEFINE_ML(Name,
//              op,
//              fC,
//              loc,
//              XHndl)
//

#define SIZE_MT             sizeof(MethodTable*)
#define SIZE_MD             sizeof(MethodDesc*)
#define SIZE_8              sizeof(UINT8)
#define SIZE_16             sizeof(UINT16)
#define SIZE_32             sizeof(UINT32)
#define SIZE_OP             sizeof(ML_CREATE_MARSHALER_CARRAY_OPERANDS)

#define SIMPLE_ML(name)     DEFINE_ML(name, 0, 0, 0, 0)

// End of ML stream
SIMPLE_ML(ML_END)

// Ends interpretation w/out ending stream
SIMPLE_ML(ML_INTERRUPT)

// copy 1 byte and sign extend it 
SIMPLE_ML(ML_COPYI1)

// copy 1 byte and zero extend it 
SIMPLE_ML(ML_COPYU1)

// copy 2 byte and sign extend it
SIMPLE_ML(ML_COPYI2)

// copy 2 byte and mask the high bytes
SIMPLE_ML(ML_COPYU2)

// copy 4 byte and sign extend it 
SIMPLE_ML(ML_COPYI4)

// copy 4 byte and mask the high bytes
SIMPLE_ML(ML_COPYU4)

// Copy 1 byte from source to destination
SIMPLE_ML(ML_COPY1)

// Copy 2 bytes from source to destination
SIMPLE_ML(ML_COPY2)

// Copy 4 bytes from source to destination
SIMPLE_ML(ML_COPY4)

// Copy 8 bytes from source to destination
SIMPLE_ML(ML_COPY8)

#if defined(TRACK_FLOATING_POINT_REGISTERS)
// Copy 4 float bytes from source to destination 
SIMPLE_ML(ML_COPYR4_N2C)

// Copy 4 float bytes from source to destination 
SIMPLE_ML(ML_COPYR4_C2N)

// Copy 8 float bytes from source to destination 
SIMPLE_ML(ML_COPYR8_N2C)

// Copy 8 float bytes from source to destination 
SIMPLE_ML(ML_COPYR8_C2N)
#endif // TRACK_FLOATING_POINT_REGISTERS

// 32-bit BOOL -> boolean
SIMPLE_ML(ML_BOOL_N2C)

// boolean -> 32-bit BOOL
SIMPLE_ML(ML_BOOL_C2N)

// Push ptr to 1-byte retval buffer
DEFINE_ML(ML_PUSHRETVALBUFFER1,
          0,
          0,
          sizeof(RetValBuffer),
          0)

// Push ptr to 2-byte retval buffer
DEFINE_ML(ML_PUSHRETVALBUFFER2,
          0,
          0,
          sizeof(RetValBuffer),
          0)

// Push ptr to 4-byte retval buffer
DEFINE_ML(ML_PUSHRETVALBUFFER4,
          0,
          0,
          sizeof(RetValBuffer),
          0) 

// Push ptr to 8-byte retval buffer
DEFINE_ML(ML_PUSHRETVALBUFFER8,
          0,
          0,
          sizeof(RetValBuffer),
          0)

// Redirect psrc to local
DEFINE_ML(ML_SETSRCTOLOCAL,
          2,
          0,
          0,
          0)        

// Throw if FAILED(hr)
SIMPLE_ML(ML_THROWIFHRFAILED)

// Do an "any"-style parameter
DEFINE_ML(ML_OBJECT_C2N,
          4,
          1,
          sizeof(ML_OBJECT_C2N_SR),
          0)  

// Backpropagation for "any"-style parameter
DEFINE_ML(ML_OBJECT_C2N_POST,
          2,
          0,
          0,
          0)

//////////////////////////////////////////////////////////////////////////////////////////
//COM TO COM+ stuff

// Marker to indicate the ML stub is for a late bound call
SIMPLE_ML(ML_LATEBOUNDMARKER)

// Marker to indicate the ML stub is for a COM event call
SIMPLE_ML(ML_COMEVENTCALLMARKER)

// Increment source pointer by 16-bit signed value
DEFINE_ML(ML_BUMPSRC,
          2,
          0,
          0,
          0)

// Increment destination pointer by 16-bit signed value
DEFINE_ML(ML_BUMPDST,
          2,
          0,
          0,
          0)

DEFINE_ML(ML_ARRAYWITHOFFSET_C2N,
          0,
          1,
          sizeof(ML_ARRAYWITHOFFSET_C2N_SR),
          0)

DEFINE_ML(ML_ARRAYWITHOFFSET_C2N_POST,
          2,
          0,
          0,
          0)


//==========================================================================
// !! These must appear in the same order that marshalers are defined in
// mtypes.h. That's because mlinfo uses opcode arithmetic to find
// the correct ML_CREATE.
//==========================================================================


DEFINE_ML(ML_CREATE_MARSHALER_GENERIC_1,
          0,
          0,
          sizeof(CopyMarshaler1),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_GENERIC_U1,
          0,
          0,
          sizeof(CopyMarshalerU1),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_GENERIC_2,
          0,
          0,
          sizeof(CopyMarshaler2),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_GENERIC_U2,
          0,
          0,
          sizeof(CopyMarshalerU2),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_GENERIC_4,
          0,
          0,
          sizeof(CopyMarshaler4),
          0)

DEFINE_ML(ML_CREATE_MARSHALER_GENERIC_U4,
          0,
          0,
          sizeof(CopyMarshalerU4),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_GENERIC_8,
          0,
          0,
          sizeof(CopyMarshaler8),
          0)

DEFINE_ML(ML_CREATE_MARSHALER_WINBOOL,
          0,
          0,
          sizeof(WinBoolMarshaler),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_CBOOL,
          0,
          1,
          sizeof(CBoolMarshaler),
          0)
          

DEFINE_ML(ML_CREATE_MARSHALER_ANSICHAR,
          2*SIZE_8,
          0,
          sizeof(AnsiCharMarshaler),
          0)

DEFINE_ML(ML_CREATE_MARSHALER_FLOAT,
          0,
          0,
          sizeof(FloatMarshaler),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_DOUBLE,
          0,
          0,
          sizeof(DoubleMarshaler),
          0)

DEFINE_ML(ML_CREATE_MARSHALER_CURRENCY,
          0,
          0,
          sizeof(CurrencyMarshaler),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_DECIMAL,
          0,
          0,
          sizeof(DecimalMarshaler),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_DECIMAL_PTR,
          0,
          0,
          sizeof(DecimalPtrMarshaler),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_GUID,
          0,
          0,
          sizeof(GuidMarshaler),
          0)

DEFINE_ML(ML_CREATE_MARSHALER_GUID_PTR,
          0,
          0,
          sizeof(GuidPtrMarshaler),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_DATE,
          0,
          0,
          sizeof(DateMarshaler),
          0)
 
DEFINE_ML(ML_CREATE_MARSHALER_WSTR,
          0,
          1,
          sizeof(WSTRMarshaler),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_CSTR,
          2 * SIZE_8,
          1,
          sizeof(CSTRMarshaler),
          0)
          

DEFINE_ML(ML_CREATE_MARSHALER_WSTR_BUFFER,
          0,
          1,
          sizeof(WSTRBufferMarshaler),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_CSTR_BUFFER,
          2 * SIZE_8,
          1,
          sizeof(CSTRBufferMarshaler),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_INTERFACE,
          2*SIZE_MT + 3*SIZE_8,
          1,
          sizeof(InterfaceMarshaler),
          0)


DEFINE_ML(ML_CREATE_MARSHALER_CARRAY,
          SIZE_OP,
          1,
          sizeof(NativeArrayMarshaler),
          0)

DEFINE_ML(ML_CREATE_MARSHALER_ASANYA,
          0,
          1,
          sizeof(AsAnyAMarshaler),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_ASANYW,
          0,
          1,
          sizeof(AsAnyWMarshaler),
          0)

DEFINE_ML(ML_CREATE_MARSHALER_DELEGATE,
          SIZE_MT,
          1,
          sizeof(DelegateMarshaler),
          0)

DEFINE_ML(ML_CREATE_MARSHALER_BLITTABLEPTR,
          sizeof(void*),
          1,
          sizeof(BlittablePtrMarshaler),
          0)


DEFINE_ML(ML_CREATE_MARSHALER_LAYOUTCLASSPTR,
          sizeof(void*),
          1,
          sizeof(LayoutClassPtrMarshaler),
          0)
                                            
SIMPLE_ML(ML_CREATE_MARSHALER_ARRAYWITHOFFSET)

DEFINE_ML(ML_CREATE_MARSHALER_BLITTABLEVALUECLASS, 
          0,
          1,
          sizeof(BlittableValueClassMarshaler),
          0)
                                            
DEFINE_ML(ML_CREATE_MARSHALER_VALUECLASS,
          0,
          1,
          sizeof(ValueClassMarshaler),
          0)

DEFINE_ML(ML_CREATE_MARSHALER_REFERENCECUSTOMMARSHALER, 
          sizeof(CustomMarshalerInfo*),
          1,
          sizeof(ReferenceCustomMarshaler),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_ARGITERATOR,
          0,
          1,
          sizeof(ArgIteratorMarshaler),
          0)

DEFINE_ML(ML_CREATE_MARSHALER_BLITTABLEVALUECLASSWITHCOPYCTOR,
          0,
          1,
          sizeof(BlittableValueClassWithCopyCtorMarshaler),
          0)


DEFINE_ML(ML_CREATE_MARSHALER_HANDLEREF,
          0,
          1,
          sizeof(HandleRefMarshaler),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_SAFEHANDLE,
          0,
          1,
          sizeof(SafeHandleMarshaler),
          0)
          
DEFINE_ML(ML_CREATE_MARSHALER_CRITICALHANDLE,
          0,
          1,
          sizeof(CriticalHandleMarshaler),
          0)
          

//==========================================================================


SIMPLE_ML(ML_MARSHAL_N2C)
          
SIMPLE_ML(ML_MARSHAL_N2C_OUT)
          
SIMPLE_ML(ML_MARSHAL_N2C_BYREF)
          
SIMPLE_ML(ML_MARSHAL_N2C_BYREF_OUT)
          
DEFINE_ML(ML_UNMARSHAL_N2C_IN,
          2,
          0,
          0,
          0)
          
DEFINE_ML(ML_UNMARSHAL_N2C_OUT,
          2,
          0,
          0,
          0)
          
DEFINE_ML(ML_UNMARSHAL_N2C_IN_OUT,
          2,
          0,
          0,
          0)
          
DEFINE_ML(ML_UNMARSHAL_N2C_BYREF_IN,
          2,
          0,
          0,
          0)
          
DEFINE_ML(ML_UNMARSHAL_N2C_BYREF_OUT,
          2,
          0,
          0,
          0)
          
DEFINE_ML(ML_UNMARSHAL_N2C_BYREF_IN_OUT,
          2,
          0,
          0,
          0)
          
SIMPLE_ML(ML_MARSHAL_C2N)
          
SIMPLE_ML(ML_MARSHAL_C2N_OUT)
          
SIMPLE_ML(ML_MARSHAL_C2N_BYREF)
          
SIMPLE_ML(ML_MARSHAL_C2N_BYREF_OUT)
          
DEFINE_ML(ML_UNMARSHAL_C2N_IN,
          2,
          0,
          0,
          0)
          
DEFINE_ML(ML_UNMARSHAL_C2N_OUT,
          2,
          0,
          0,
          0)
          
DEFINE_ML(ML_UNMARSHAL_C2N_IN_OUT,
          2,
          0,
          0,
          0)
          
DEFINE_ML(ML_UNMARSHAL_C2N_BYREF_IN,
          2,
          0,
          0,
          0)
          
DEFINE_ML(ML_UNMARSHAL_C2N_BYREF_OUT,
          2,
          0,
          0,
          0)
          
DEFINE_ML(ML_UNMARSHAL_C2N_BYREF_IN_OUT,
          2,
          0,
          0,
          0)

SIMPLE_ML(ML_PRERETURN_N2C)
          
SIMPLE_ML(ML_PRERETURN_N2C_RETVAL)
          
DEFINE_ML(ML_RETURN_N2C,
          2,
          0,
          0,
          0)
          
DEFINE_ML(ML_RETURN_N2C_RETVAL,
          2,
          0,
          0,
          0)

SIMPLE_ML(ML_PRERETURN_C2N)
          
SIMPLE_ML(ML_PRERETURN_C2N_RETVAL)
          
DEFINE_ML(ML_RETURN_C2N,
          2,
          0,
          0,
          0)
          
DEFINE_ML(ML_RETURN_C2N_RETVAL,
          2,
          0,
          0,
          0)
          
SIMPLE_ML(ML_SET_COM)
          
SIMPLE_ML(ML_GET_COM)
          
SIMPLE_ML(ML_PREGET_COM_RETVAL)
          
SIMPLE_ML(ML_PINNEDUNISTR_C2N)


// Marshal a blittable layoutclass
SIMPLE_ML(ML_BLITTABLELAYOUTCLASS_C2N)

DEFINE_ML(ML_BLITTABLEVALUECLASS_C2N,
          SIZE_32,
          0,
          0,
          0)
          
DEFINE_ML(ML_BLITTABLEVALUECLASS_N2C,
          SIZE_32,
          0,
          0,
          0)
          
DEFINE_ML(ML_REFBLITTABLEVALUECLASS_C2N,
          SIZE_32,
          0,
          0,
          0)

DEFINE_ML(ML_VALUECLASS_C2N,
          SIZE_MT,
          1,
          0,
          0)
          
DEFINE_ML(ML_VALUECLASS_N2C,
          SIZE_MT,
          0,
          0,
          0)
          
DEFINE_ML(ML_REFVALUECLASS_C2N,
          1+SIZE_MT,
          1,
          sizeof(ML_REFVALUECLASS_C2N_SR),
          0)
          
DEFINE_ML(ML_REFVALUECLASS_C2N_POST,
          SIZE_16,
          0,
          0,
          0)
          
DEFINE_ML(ML_REFVALUECLASS_N2C,
          1+SIZE_MT,
          1,
          sizeof(ML_REFVALUECLASS_N2C_SR),
          0)
          
DEFINE_ML(ML_REFVALUECLASS_N2C_POST,\
          SIZE_16,
          0,
          0,
          0)

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
          
DEFINE_ML(ML_VALUECLASS_C2BYREFN,
          SIZE_MT,
          1,
          0,
          0)

DEFINE_ML(ML_VALUECLASS_BYREFC2N,
          SIZE_MT,
          1,
          0,
          0)

DEFINE_ML(ML_VALUECLASS_BYREFN2C,
          SIZE_MT,
          1,
          0,
          0)

DEFINE_ML(ML_VALUECLASS_N2BYREFC,
          SIZE_MT,
          1,
          sizeof(ML_REFVALUECLASS_N2C_SR),
          0)

#endif // ENREGISTERED_PARAMTYPE_MAXSIZE

DEFINE_ML(ML_PINNEDISOMORPHICARRAY_C2N,
          2,
          0,
          0,
          0)
          
DEFINE_ML(ML_PINNEDISOMORPHICARRAY_C2N_EXPRESS,
          2,
          0,
          0,
          0)


SIMPLE_ML(ML_ARGITERATOR_C2N)
          
SIMPLE_ML(ML_ARGITERATOR_N2C)

DEFINE_ML(ML_COPYCTOR_C2N,
          SIZE_MT + 2 * SIZE_MD,
          0,
          0,
          0)
          
DEFINE_ML(ML_COPYCTOR_N2C,
          SIZE_MT,
          0,
          0,
          0)

DEFINE_ML(ML_CAPTURE_PSRC,
          2,
          0,
          sizeof(BYTE*),
          0)
          
DEFINE_ML(ML_MARSHAL_RETVAL_SMBLITTABLEVALUETYPE_C2N,
          6,
          0,
          0,
          0)
          
DEFINE_ML(ML_MARSHAL_RETVAL_SMBLITTABLEVALUETYPE_N2C,
          6,
          0,
          0,
          0)

DEFINE_ML(ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_C2N,
          4,
          1,
          sizeof(ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_C2N_SR),
          0)
          
DEFINE_ML(ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_C2N_POST,
          SIZE_16,
          0,
          0,
          0)

DEFINE_ML(ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_N2C,
          0,
          0,
          sizeof(LPVOID),
          0)
          
DEFINE_ML(ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_N2C_POST,
          SIZE_16,
          0,
          0,
          0)

DEFINE_ML(ML_PUSHVASIGCOOKIEEX,
          SIZE_16,
          0,
          sizeof(VASigCookieEx),
          0)
          
DEFINE_ML(ML_CSTR_C2N,
          2,
          1,
          sizeof(ML_CSTR_C2N_SR),
          0)
          

SIMPLE_ML(ML_HANDLEREF_C2N)

// SafeHandle support.
DEFINE_ML(ML_SAFEHANDLE_C2N,
          0,
          1,
          0,
          0)

DEFINE_ML(ML_REFSAFEHANDLE_C2N,
          1+SIZE_MT,
          1,
          sizeof(ML_REFSAFEHANDLE_C2N_SR),
          0)

DEFINE_ML(ML_REFSAFEHANDLE_C2N_POST,
          SIZE_16,
          1,
          0,
          0)

DEFINE_ML(ML_RETSAFEHANDLE_C2N,
          SIZE_MT,
          1,
          sizeof(ML_RETSAFEHANDLE_C2N_SR),
          0)

DEFINE_ML(ML_RETSAFEHANDLE_C2N_POST,
          SIZE_16,
          1,
          0,
          0)

DEFINE_ML(ML_RETVALSAFEHANDLE_C2N,
          SIZE_MT,
          1,
          sizeof(ML_REFSAFEHANDLE_C2N_SR),
          0)

DEFINE_ML(ML_RETVALSAFEHANDLE_C2N_POST,
          SIZE_16,
          1,
          0,
          0)

// CriticalHandle support.
DEFINE_ML(ML_CRITICALHANDLE_C2N,
          0,
          0,
          0,
          0)

DEFINE_ML(ML_REFCRITICALHANDLE_C2N,
          1+SIZE_MT,
          1,
          sizeof(ML_REFCRITICALHANDLE_C2N_SR),
          0)

DEFINE_ML(ML_REFCRITICALHANDLE_C2N_POST,
          SIZE_16,
          1,
          0,
          0)

DEFINE_ML(ML_RETCRITICALHANDLE_C2N,
          SIZE_MT,
          1,
          sizeof(ML_RETCRITICALHANDLE_C2N_SR),
          0)

DEFINE_ML(ML_RETCRITICALHANDLE_C2N_POST,
          SIZE_16,
          1,
          0,
          0)

DEFINE_ML(ML_RETVALCRITICALHANDLE_C2N,
          SIZE_MT,
          1,
          sizeof(ML_REFCRITICALHANDLE_C2N_SR),
          0)

DEFINE_ML(ML_RETVALCRITICALHANDLE_C2N_POST,
          SIZE_16,
          1,
          0,
          0)

// Do an "any"-style parameter
DEFINE_ML(ML_WSTRBUILDER_C2N,
          0,
          1,
          sizeof(ML_WSTRBUILDER_C2N_SR),
          0)
          
// Backpropagation for "any"-style parameter
DEFINE_ML(ML_WSTRBUILDER_C2N_POST,
          2,
          0,
          0,
          0)

// Do an "any"-style parameter
DEFINE_ML(ML_CSTRBUILDER_C2N,
          2,
          1,
          sizeof(ML_CSTRBUILDER_C2N_SR),
          0)

// Backpropagation for "any"-style parameter
DEFINE_ML(ML_CSTRBUILDER_C2N_POST,
          2,
          0,
          0,
          0)


SIMPLE_ML(ML_CBOOL_C2N)
          
SIMPLE_ML(ML_CBOOL_N2C)

SIMPLE_ML(ML_LCID_C2N)
          
SIMPLE_ML(ML_LCID_N2C)

DEFINE_ML(ML_STRUCTRETN2C,
          SIZE_MT,
          1,
          sizeof(ML_STRUCTRETN2C_SR),
          0)
          
DEFINE_ML(ML_STRUCTRETN2C_POST,
          SIZE_16,
          0,
          0,
          0)

DEFINE_ML(ML_STRUCTRETC2N,
          SIZE_MT,
          1,
          sizeof(ML_STRUCTRETC2N_SR),
          0)
          
DEFINE_ML(ML_STRUCTRETC2N_POST,
          SIZE_16,
          0,
          0,
          0)

DEFINE_ML(ML_CURRENCYRETC2N,
          0,
          0,
          sizeof(ML_CURRENCYRETC2N_SR),
          0)
          
DEFINE_ML(ML_CURRENCYRETC2N_POST,
          SIZE_16,
          0,
          0,
          0)

SIMPLE_ML(ML_COPYPINNEDGCREF)


DEFINE_ML(ML_CURRENCYRETN2C,
          0,
          1,
          sizeof(ML_CURRENCYRETN2C_SR),
          0)
          
DEFINE_ML(ML_CURRENCYRETN2C_POST,
          SIZE_16,
          0,
          0,
          0)

DEFINE_ML(ML_DATETIMERETN2C,
          0,
          1,
          sizeof(ML_DATETIMERETN2C_SR),
          0)
          
DEFINE_ML(ML_DATETIMERETN2C_POST,
          SIZE_16,
          0,
          0,
          0)

DEFINE_ML(ML_COPY_SMBLITTABLEVALUETYPE,
          SIZE_32,
          0,
          0,
          0)
