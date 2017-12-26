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
// METASIG.H
//
// All literal MetaData signatures should be defined here.
//

#ifndef __METASIG_H__
#define __METASIG_H__

typedef const struct HardCodedMetaSig *LPHARDCODEDMETASIG;

// As hard-coded metasigs are constant data ordinarily it
// wouldn't be necessary to use PTR access.  However, access
// through the Binder class requires it.
typedef DPTR(const struct HardCodedMetaSig) PTR_HARDCODEDMETASIG;

#include "crst.h"

class Module;

struct HardCodedMetaSig {

        const BYTE* m_pMetaSig; // metasig prefixed with INT8 length:
                                // length > 0 - resolved, lenght < 0 - has unresolved type references

        // Get the metasig, do a one-time conversion if necessary
        void GetBinarySig(PCCOR_SIGNATURE *ppBinarySig, ULONG *pcbBinarySigLength) const;

        // This returns mscorlib's module.
        Module* GetModule() const;

        static CrstStatic m_Crst;

        static void Init();
};

#endif  //__METASIG_H__

// Generic sig's based on types.

// All sigs are alphabetized by the signature string and given a canonical name.  Do not
// give them "meaningful" names because we want to share them aggressively.  Do not add
// duplicates!

// The canonical form:
//
//   <what>(<type>*, <name>*)
//
//   where <what> is:
//
//         Fld  -- field
//         IM   -- instance method (HASTHIS == TRUE)
//         SM   -- static method
//
//   and <name> -- <type> is:
//
//   a  -- Arr  -- array
//   P  -- Ptr  -- a pointer
//   r  -- Ref  -- a byref
//         Ret  -- indicates function return type
//
//         PMS  -- PermissionSet
//         Var  -- Variant
//
//   b  -- Byte -- (unsigned) byte
//   u  -- Char -- character (2 byte unsigned unicode)
//   d  -- Dbl  -- double
//   f  -- Flt  -- float
//   i  -- Int  -- integer
//   K  -- UInt  -- unsigned integer
//   I  -- IntPtr -- agnostic integer
//   U  -- UIntPtr -- agnostic unsigned integer
//   l  -- Long -- long integer
//   L  -- ULong -- unsigned long integer
//   h  -- Shrt -- short integer
//   H  -- UShrt -- unsigned short integer
//   v  -- Void -- Void
//   B  -- Sbyt -- signed byte
//   F  -- Bool -- boolean
//   j  -- Obj  -- System.Object
//   s  -- Str  -- System.String 
//   C  --      -- class
//   g  --      -- struct
//   T  -- TypedReference -- TypedReference 
//

// Use DEFINE_METASIG for signatures that does not reference other types
// Use DEFINE_METASIG_T for signatures that reference other types (contains C or g)


// This part of the file is included multiple times with different macro definitions 
// to generate the hardcoded metasigs.


// SM, IM and Fld macros have often a very similar definitions. METASIG_BODY is 
// a helper to avoid these redundant SM, IM and Fld definitions.
#ifdef METASIG_BODY
#ifndef DEFINE_METASIG
#define DEFINE_METASIG(body)            body
#endif
#define SM(varname, args, retval)       METASIG_BODY( SM_ ## varname, args retval )
#define IM(varname, args, retval)       METASIG_BODY( IM_ ## varname, args retval )
#define Fld(varname, val)               METASIG_BODY( Fld_ ## varname, val )
#endif


#ifdef DEFINE_METASIG

// Use default if undefined
#ifndef DEFINE_METASIG_T
#define DEFINE_METASIG_T(body) DEFINE_METASIG(body)
#endif

// One letter shortcuts are defined for all types that can occur in the signature.
// The shortcuts are defined indirectly through METASIG_ATOM. METASIG_ATOM is
// designed to control whether to generate the initializer for the signature or 
// just compute the size of the signature.

#define b METASIG_ATOM(ELEMENT_TYPE_U1)
#define u METASIG_ATOM(ELEMENT_TYPE_CHAR)
#define d METASIG_ATOM(ELEMENT_TYPE_R8)
#define f METASIG_ATOM(ELEMENT_TYPE_R4)
#define i METASIG_ATOM(ELEMENT_TYPE_I4)
#define K METASIG_ATOM(ELEMENT_TYPE_U4)
#define I METASIG_ATOM(ELEMENT_TYPE_I)
#define U METASIG_ATOM(ELEMENT_TYPE_U)
#define l METASIG_ATOM(ELEMENT_TYPE_I8)
#define L METASIG_ATOM(ELEMENT_TYPE_U8)
#define h METASIG_ATOM(ELEMENT_TYPE_I2)
#define H METASIG_ATOM(ELEMENT_TYPE_U2)
#define v METASIG_ATOM(ELEMENT_TYPE_VOID)
#define B METASIG_ATOM(ELEMENT_TYPE_I1)
#define F METASIG_ATOM(ELEMENT_TYPE_BOOLEAN)
#define j METASIG_ATOM(ELEMENT_TYPE_OBJECT)
#define s METASIG_ATOM(ELEMENT_TYPE_STRING)
#define T METASIG_ATOM(ELEMENT_TYPE_TYPEDBYREF)


// METASIG_RECURSE controls whether to recurse into the complex types 
// in the macro expansion. METASIG_RECURSE is designed to control 
// whether to compute the byte size of the signature or just compute 
// the number of arguments in the signature.

#if METASIG_RECURSE

#define a(x) METASIG_ATOM(ELEMENT_TYPE_SZARRAY) x
#define P(x) METASIG_ATOM(ELEMENT_TYPE_PTR) x
#define r(x) METASIG_ATOM(ELEMENT_TYPE_BYREF) x

// The references to other types have special definition in some cases
#ifndef C
#define C(x) METASIG_ATOM(ELEMENT_TYPE_CLASS) METASIG_ATOM(CLASS__ ## x)
#endif
#ifndef g
#define g(x) METASIG_ATOM(ELEMENT_TYPE_VALUETYPE) METASIG_ATOM(CLASS__ ## x)
#endif

#else

#define a(x) METASIG_ATOM(ELEMENT_TYPE_SZARRAY)
#define P(x) METASIG_ATOM(ELEMENT_TYPE_PTR)
#define r(x) METASIG_ATOM(ELEMENT_TYPE_BYREF)

// The references to other types have special definition in some cases
#ifndef C
#define C(x) METASIG_ATOM(ELEMENT_TYPE_CLASS)
#endif
#ifndef g
#define g(x) METASIG_ATOM(ELEMENT_TYPE_VALUETYPE)
#endif

#endif



// to avoid empty arguments for macros
#define _


// static methods:
DEFINE_METASIG(SM(Int_IntPtr_Obj_RetObj, i I j, j))
DEFINE_METASIG(SM(IntPtr_IntPtr_int_IntPtr_int_RetIntPtr, I I i I i, I))
DEFINE_METASIG(SM(IntPtr_IntPtr_IntPtr_Int_RetObj, I I I i, j))
DEFINE_METASIG(SM(Obj_IntPtr_RetIntPtr, j I, I))
DEFINE_METASIG(SM(Obj_IntPtr_IntPtr_RetVoid, j I I, v))
DEFINE_METASIG(SM(Obj_IntPtr_IntPtr_Int_RetIntPtr, j I I i, I))
DEFINE_METASIG(SM(Obj_IntPtr_IntPtr_Int_IntPtr_IntPtr_RetIntPtr, j I I i I I, I))
DEFINE_METASIG(SM(Obj_IntPtr_IntPtr_IntPtr_IntPtr_Int_IntPtr_IntPtr_RetIntPtr, j I I I I i I I, I))
DEFINE_METASIG(SM(Int_IntPtr_IntPtr_RetVoid, i I I, v))
DEFINE_METASIG(SM(Int_RetVoid, i, v))
DEFINE_METASIG(SM(Int_Int_RetVoid, i i, v))
DEFINE_METASIG(SM(Long_RetVoid, l, v))
DEFINE_METASIG(SM(Str_RetIntPtr, s, I))
DEFINE_METASIG(SM(Str_Bool_RetIntPtr, s F, I))
DEFINE_METASIG(SM(IntPtr_IntPtr_RetVoid, I I, v))
DEFINE_METASIG(SM(IntPtr_IntPtr_Obj_RetIntPtr, I I j, I))
DEFINE_METASIG(SM(IntPtr_IntPtr_IntPtr_RetVoid, I I I, v))
DEFINE_METASIG(SM(IntPtr_IntPtr_IntPtr_Obj_RetVoid, I I I j, v))
DEFINE_METASIG(SM(IntPtr_Int_IntPtr_RetIntPtr, I i I, I))
DEFINE_METASIG(SM(RetUIntPtr, _, U))
DEFINE_METASIG(SM(RetIntPtr, _, I))
DEFINE_METASIG(SM(IntPtr_RetStr, I, s))
DEFINE_METASIG(SM(IntPtrIntPtrIntPtr_RetVoid, I I I, v))
DEFINE_METASIG(SM(IntPtrIntPtrIntPtrIntPtr_RetVoid, I I I I, v))
DEFINE_METASIG_T(SM(RuntimeTypeHandle_RuntimeMethodHandle_RetMethodBase, g(TYPE_HANDLE) g(METHOD_HANDLE), C(METHOD_BASE) ))
DEFINE_METASIG_T(SM(RuntimeTypeHandle_RuntimeFieldHandle_RetFieldInfo, g(TYPE_HANDLE) g(FIELD_HANDLE), C(FIELD_INFO) ))
DEFINE_METASIG_T(SM(RuntimeTypeHandle_Int_RetPropertyInfo, g(TYPE_HANDLE) i, C(PROPERTY_INFO) ))
DEFINE_METASIG(SM(Char_Bool_Bool_RetByte, u F F, b))
DEFINE_METASIG(SM(Byte_RetChar, b, u))
DEFINE_METASIG(SM(Str_Bool_Bool_RefInt_RetIntPtr, s F F r(i), I))
DEFINE_METASIG(SM(IntPtr_Int_RetStr, I i, s))
DEFINE_METASIG(SM(Obj_PtrByte_IntPtr_RetVoid, j P(b) I, v))
DEFINE_METASIG(SM(Obj_PtrByte_RetVoid, j P(b), v))
DEFINE_METASIG(SM(PtrByte_IntPtr_RetVoid, P(b) I, v))
DEFINE_METASIG(SM(Str_Bool_Bool_RetArrByte, s F F, a(b) ))
DEFINE_METASIG(SM(ArrByte_Int_PtrByte_Int_Int_RetVoid, a(b) i P(b) i i, v))
DEFINE_METASIG(SM(PtrSByt_RetInt, P(B), i))
DEFINE_METASIG(SM(IntPtr_RetIntPtr, I, I))
DEFINE_METASIG(SM(PtrByte_PtrByte_Int_RetVoid, P(b) P(b) i, v))
DEFINE_METASIG(SM(RefObj_Int_IntPtr_IntPtr_RetIntPtr, r(j) i I I, I))
DEFINE_METASIG(SM(RefObj_IntPtr_RetVoid, r(j) I, v))
DEFINE_METASIG(SM(Int_IntPtr_RefObj_IntPtr_IntPtr_IntPtr_RetVoid, i I r(j) I I I, v))
DEFINE_METASIG(SM(IntPtr_Int_RefObj_IntPtr_RetVoid, I i r(j) I, v))
DEFINE_METASIG(SM(IntPtr_RefObj_IntPtr_RetVoid, I r(j) I, v))
DEFINE_METASIG(SM(IntPtr_Int_IntPtr_Int_Int_Int_RetVoid, I i I i i i, v))
DEFINE_METASIG(SM(IntPtr_IntPtr_Int_Int_RetVoid, I I i i, v))
DEFINE_METASIG(SM(IntPtr_RefObj_IntPtr_Obj_RetVoid, I r(j) I j, v))


DEFINE_METASIG(SM(Flt_RetFlt, f, f))
DEFINE_METASIG(SM(Dbl_RetDbl, d, d))
DEFINE_METASIG(SM(RefDbl_Dbl_RetDbl, r(d) d, d))
DEFINE_METASIG(SM(RefDbl_Dbl_Dbl_RetDbl, r(d) d d, d))
DEFINE_METASIG(SM(RefInt_RetInt, r(i), i))
DEFINE_METASIG(SM(RefLong_RetLong, r(l), l))
DEFINE_METASIG(SM(RefLong_Long_RetLong, r(l) l, l))
DEFINE_METASIG(SM(RefLong_Long_Long_RetLong, r(l) l l, l))
DEFINE_METASIG(SM(RefFlt_Flt_RetFlt, r(f) f, f))
DEFINE_METASIG(SM(RefFlt_Flt_Flt_RetFlt, r(f) f f, f))
DEFINE_METASIG(SM(RefInt_Int_RetInt, r(i) i, i))
DEFINE_METASIG(SM(RefInt_Int_Int_RetInt, r(i) i i, i))
DEFINE_METASIG(SM(RefIntPtr_IntPtr_RetIntPtr, r(I) I, I))
DEFINE_METASIG(SM(RefIntPtr_IntPtr_IntPtr_RetIntPtr, r(I) I I, I))
DEFINE_METASIG(SM(RefObj_Obj_RetObj, r(j) j, j))
DEFINE_METASIG(SM(RefObj_Obj_Obj_RetObj, r(j) j j, j))
DEFINE_METASIG(SM(ObjIntPtr_RetVoid, j I, v))

DEFINE_METASIG_T(SM(RetContext, _, C(CONTEXT)))
DEFINE_METASIG_T(SM(RetMethodBase, _, C(METHOD_BASE)))
DEFINE_METASIG(SM(RetVoid, _, v))
DEFINE_METASIG(SM(Str_IntPtr_Int_RetVoid, s I i, v))
DEFINE_METASIG(SM(Int_RetIntPtr, i, I))

DEFINE_METASIG_T(SM(DateTime_RetDbl, g(DATE_TIME), d))
DEFINE_METASIG_T(SM(Dbl_RetDateTime, d, g(DATE_TIME)))

DEFINE_METASIG(SM(IntPtr_RetObj, I, j))
DEFINE_METASIG(SM(Int_RetObj, i, j))
DEFINE_METASIG(SM(Int_IntPtr_RetObj, i I, j))
DEFINE_METASIG(SM(IntPtr_IntPtr_Int_RetVoid, I I i, v))

DEFINE_METASIG_T(SM(LocalDataStore_RetVoid, C(LOCAL_DATA_STORE), v))
DEFINE_METASIG_T(SM(ContextBoundObject_RetObj, C(CONTEXT_BOUND_OBJECT), j))
DEFINE_METASIG_T(SM(Evidence_PMS_PMS_PMS_PMS_PMS_RetBool, \
                 C(EVIDENCE) C(PERMISSION_SET) C(PERMISSION_SET) C(PERMISSION_SET) C(PERMISSION_SET) C(PERMISSION_SET), F))
DEFINE_METASIG_T(SM(PMS_PMS_RetInt, C(PERMISSION_SET) C(PERMISSION_SET), i))

DEFINE_METASIG(SM(IntPtr_RetVoid, I, v))
DEFINE_METASIG(SM(IntPtr_Bool_RetVoid, I F, v))
DEFINE_METASIG(SM(PtrChar_RetInt, P(u), i))
DEFINE_METASIG(SM(IntPtr_IntPtr_RetIntPtr, I I, I))
DEFINE_METASIG(SM(PtrVoid_PtrVoid_RetVoid, P(v) P(v), v))
DEFINE_METASIG(SM(Obj_RetVoid, j, v))
DEFINE_METASIG(SM(Obj_RetInt, j, i))
DEFINE_METASIG(SM(Obj_RetIntPtr, j, I))
DEFINE_METASIG(SM(Obj_RetObj, j, j))
DEFINE_METASIG(SM(Obj_RetArrByte, j, a(b)))
DEFINE_METASIG_T(SM(Obj_Int_RefVariant_RetVoid, j i r(g(VARIANT)), v))
DEFINE_METASIG_T(SM(Obj_RefMessageData_RetVoid, j r(g(MESSAGE_DATA)), v))
DEFINE_METASIG(SM(Obj_Obj_RefArrByte_RetArrByte, j j r(a(b)), a(b)))
DEFINE_METASIG_T(SM(Obj_RefVariant_RetVoid, j r(g(VARIANT)), v))

DEFINE_METASIG_T(SM(RealProxy_Type_RetBool, C(REAL_PROXY) C(TYPE), F))
DEFINE_METASIG_T(SM(IPermission_RetPermissionToken, C(IPERMISSION), C(PERMISSION_TOKEN)))
DEFINE_METASIG_T(SM(FrameSecurityDescriptor_IPermission_PermissionToken_RuntimeMethodHandle_RetBool, \
                 C(FRAME_SECURITY_DESCRIPTOR) C(IPERMISSION) C(PERMISSION_TOKEN) g(METHOD_HANDLE), F))
DEFINE_METASIG_T(SM(FrameSecurityDescriptor_PMS_OutPMS_RuntimeMethodHandle_RetBool, \
                 C(FRAME_SECURITY_DESCRIPTOR) C(PERMISSION_SET) r(C(PERMISSION_SET)) g(METHOD_HANDLE), F))
DEFINE_METASIG_T(SM(FrameSecurityDescriptor_RetInt, C(FRAME_SECURITY_DESCRIPTOR), i))
DEFINE_METASIG_T(SM(PermissionListSet_PMS_PMS_RetPermissionListSet, \
                 C(PERMISSION_LIST_SET) C(PERMISSION_SET) C(PERMISSION_SET), C(PERMISSION_LIST_SET)))
DEFINE_METASIG_T(SM(CS_PMS_PMS_CodeAccessPermission_PermissionToken_RuntimeMethodHandle_Assembly_SecurityAction_RetVoid, \
                 C(COMPRESSED_STACK) C(PERMISSION_SET) C(PERMISSION_SET) C(CODE_ACCESS_PERMISSION) C(PERMISSION_TOKEN) g(METHOD_HANDLE) C(ASSEMBLY) g(SECURITY_ACTION), v))
DEFINE_METASIG_T(SM(PMS_IntPtr_RuntimeMethodHandle_Assembly_SecurityAction_RetVoid, C(PERMISSION_SET) I g(METHOD_HANDLE) C(ASSEMBLY) g(SECURITY_ACTION), v))
DEFINE_METASIG_T(SM(CS_PMS_PMS_PMS_RuntimeMethodHandle_Assembly_SecurityAction_RetVoid, C(COMPRESSED_STACK) C(PERMISSION_SET) C(PERMISSION_SET) C(PERMISSION_SET) g(METHOD_HANDLE) C(ASSEMBLY) g(SECURITY_ACTION), v))
DEFINE_METASIG_T(SM(Assembly_PMS_PMS_RuntimeMethodHandle_SecurityAction_Obj_IPermission_RetVoid, C(ASSEMBLY) C(PERMISSION_SET) C(PERMISSION_SET) g(METHOD_HANDLE) g(SECURITY_ACTION) j C(IPERMISSION), v))
DEFINE_METASIG_T(SM(Evidence_PMS_PMS_PMS_PMS_int_Bool_RetPMS, \
                 C(EVIDENCE) C(PERMISSION_SET) C(PERMISSION_SET) C(PERMISSION_SET) r(C(PERMISSION_SET)) r(i) F, C(PERMISSION_SET)))
DEFINE_METASIG_T(SM(IntPtr_RetDCS, I, C(DOMAIN_COMPRESSED_STACK)))
DEFINE_METASIG_T(SM(RefVariant_RetObject, r(g(VARIANT)), j))

DEFINE_METASIG_T(SM(ExecutionContext_ContextCallback_Object_RetVoid, \
                 C(EXECUTIONCONTEXT) C(CONTEXTCALLBACK) j, v))
DEFINE_METASIG_T(SM(SecurityContext_ContextCallback_Object_RetVoid, \
                 C(SECURITYCONTEXT) C(CONTEXTCALLBACK) j, v))
DEFINE_METASIG_T(SM(CompressedStack_ContextCallback_Object_RetVoid, \
                 C(COMPRESSED_STACK) C(CONTEXTCALLBACK) j, v))

DEFINE_METASIG(SM(Str_RetInt, s, i))
DEFINE_METASIG_T(SM(Str_RetICustomMarshaler, s, C(ICUSTOM_MARSHALER)))
DEFINE_METASIG(SM(Int_Str_RetIntPtr, i s, I))
DEFINE_METASIG(SM(Str_Bool_Int_RetV, s F i, v))

DEFINE_METASIG_T(SM(Type_RetInt, C(TYPE), i))
DEFINE_METASIG_T(SM(Type_ArrObject_Bool_RetMarshalByRefObject, C(TYPE) a(j) F, C(MARSHAL_BY_REF_OBJECT)))
DEFINE_METASIG(SM(ArrByte_RetObj, a(b), j))
DEFINE_METASIG(SM(ArrByte_ArrByte_RefObj_RetObj, a(b) a(b) r(j), j))
DEFINE_METASIG_T(SM(PtrSByt_Int_Int_Encoding_RetStr, P(B) i i C(ENCODING), s))
DEFINE_METASIG_T(SM(ArrObj_Bool_RefArrByte_OutPMS_HostProtectionResource_RetArrByte, a(j) F r(a(b)) r(C(PERMISSION_SET)) g(HOST_PROTECTION_RESOURCE), a(b)))
DEFINE_METASIG_T(SM(ASM_Str_Int_ArrByte_ArrByte_Str_Int_Int_Int_Int_ArrByte_Evidence_RetEvidence, C(ASSEMBLY) s i a(b) a(b) s i i i i a(b) C(EVIDENCE), C(EVIDENCE)))

DEFINE_METASIG_T(SM(Void_RetRuntimeTypeHandle, _, g(TYPE_HANDLE)))
DEFINE_METASIG_T(SM(CS_PMS_PMS_ArrayList_ArrayList_RetVoid, \
                 C(COMPRESSED_STACK) C(PERMISSION_SET) C(PERMISSION_SET) C(ARRAY_LIST) C(ARRAY_LIST), v))
DEFINE_METASIG_T(SM(UInt_UInt_PtrNativeOverlapped_RetVoid, K K P(g(NATIVEOVERLAPPED)), v))
DEFINE_METASIG_T(SM(CrossContextDelegate_ArrObj_RetObj, C(CROSS_CONTEXT_DELEGATE) a(j), j))

DEFINE_METASIG(IM(IntInt_RetArrByte, i i, a(b)))
DEFINE_METASIG(IM(RetIntPtr, _, I))
DEFINE_METASIG(IM(RetInt, _, i))
DEFINE_METASIG_T(IM(RetRuntimeFieldHandle, _, g(FIELD_HANDLE)))
DEFINE_METASIG_T(IM(RetRuntimeMethodHandle, _, g(METHOD_HANDLE)))
DEFINE_METASIG_T(IM(RetAssemblyName, _, C(ASSEMBLY_NAME)))
DEFINE_METASIG_T(IM(RetModule, _, C(MODULE)))
DEFINE_METASIG_T(IM(Str_ArrB_ArrB_Ver_CI_AHA_AVC_Str_ANF_SNKP_RetV,
                 s a(b) a(b) C(VERSION) C(CULTURE_INFO) g(ASSEMBLY_HASH_ALGORITHM) g(ASSEMBLY_VERSION_COMPATIBILITY) s g(ASSEMBLY_NAME_FLAGS) C(STRONG_NAME_KEY_PAIR), v))
DEFINE_METASIG(IM(RetObj, _, j))
DEFINE_METASIG_T(IM(RetIEnumerator, _, C(IENUMERATOR)))
DEFINE_METASIG(IM(RetStr, _, s))
DEFINE_METASIG(IM(RetLong, _, l))

DEFINE_METASIG_T(IM(RetType, _, C(TYPE)))
DEFINE_METASIG(IM(RetVoid, _, v))
DEFINE_METASIG(IM(RetBool, _, F))
DEFINE_METASIG(IM(RetArrByte, _, a(b)))
DEFINE_METASIG_T(IM(RetArrParameterInfo, _, a(C(PARAMETER))))
DEFINE_METASIG_T(IM(RetCultureInfo, _, C(CULTURE_INFO)))
DEFINE_METASIG_T(IM(RetSecurityElement, _, C(SECURITY_ELEMENT)))

DEFINE_METASIG_T(SM(RetThread, _, C(THREAD)))

DEFINE_METASIG(IM(Bool_RetIntPtr, F, I))
DEFINE_METASIG_T(IM(Bool_RetMethodInfo, F, C(METHOD_INFO)))
DEFINE_METASIG(SM(Bool_RetStr, F, s))

DEFINE_METASIG(IM(PtrChar_RetVoid, P(u), v))
DEFINE_METASIG(IM(PtrChar_Int_Int_RetVoid, P(u) i i, v))
DEFINE_METASIG(IM(PtrSByt_RetVoid, P(B), v))
DEFINE_METASIG(IM(PtrSByt_Int_Int_RetVoid, P(B) i i, v))
DEFINE_METASIG_T(IM(PtrSByt_Int_Int_Encoding_RetVoid, P(B) i i C(ENCODING), v))
DEFINE_METASIG(IM(PtrChar_Int_RetVoid, P(u) i, v))
DEFINE_METASIG(IM(PtrSByt_Int_RetVoid, P(B) i, v))

DEFINE_METASIG(IM(ArrChar_RetStr, a(u), s))
DEFINE_METASIG(IM(ArrChar_Int_Int_RetStr, a(u) i i, s))
DEFINE_METASIG(IM(Char_Int_RetStr, u i, s))
DEFINE_METASIG(IM(PtrChar_RetStr, P(u), s))
DEFINE_METASIG(IM(PtrChar_Int_Int_RetStr, P(u) i i, s))

DEFINE_METASIG(IM(Char_Char_RetStr, u u, s))
DEFINE_METASIG(IM(Char_Int_RetVoid, u i, v))
DEFINE_METASIG_T(IM(CultureInfo_RetVoid, C(CULTURE_INFO), v))
DEFINE_METASIG(IM(Dbl_RetVoid, d, v))
DEFINE_METASIG(IM(Flt_RetVoid, f, v))
DEFINE_METASIG(IM(Int_RetInt, i, i))
DEFINE_METASIG(IM(Int_RetPtrVoid, i, P(v)))
DEFINE_METASIG(IM(Int_RetStr, i, s))
DEFINE_METASIG(IM(Int_RetVoid, i, v))
DEFINE_METASIG(IM(Int_RetBool, i, F))
DEFINE_METASIG_T(IM(RefMessageData_Int_RetVoid, r(g(MESSAGE_DATA)) i, v))
DEFINE_METASIG(IM(Int_Int_Int_Int_RetVoid, i i i i, v))
DEFINE_METASIG(IM(Int_Str_Str_RetVoid, i s s, v))
DEFINE_METASIG_T(IM(Obj_EventArgs_RetVoid, j C(EVENT_ARGS), v))
DEFINE_METASIG_T(IM(Obj_UnhandledExceptionEventArgs_RetVoid, j C(UNHANDLED_EVENTARGS), v))

DEFINE_METASIG_T(IM(Assembly_RetVoid, C(ASSEMBLY), v))
DEFINE_METASIG_T(IM(Assembly_RetBool, C(ASSEMBLY), F))
DEFINE_METASIG_T(IM(Exception_RetVoid, C(EXCEPTION), v))

DEFINE_METASIG(IM(IntPtr_RetObj, I, j))
DEFINE_METASIG(IM(IntPtr_RetVoid, I, v))
DEFINE_METASIG(IM(IntPtr_PtrVoid_RetVoid, I P(v), v))
DEFINE_METASIG_T(IM(RefGuid_RetIntPtr, r(g(GUID)), I))

DEFINE_METASIG(IM(Obj_RetInt, j, i))
DEFINE_METASIG(IM(Obj_RetIntPtr, j, I))
DEFINE_METASIG(IM(Obj_RetVoid, j, v))
DEFINE_METASIG(IM(Obj_RetObj, j, j))
DEFINE_METASIG(IM(Obj_IntPtr_RetVoid, j I, v))
DEFINE_METASIG(IM(Obj_IntPtr_IntPtr_RetVoid, j I I, v))
DEFINE_METASIG(IM(Obj_IntPtr_IntPtr_IntPtr_RetVoid, j I I I, v))
DEFINE_METASIG(IM(Obj_IntPtr_IntPtr_IntPtr_IntPtr_RetVoid, j I I I I, v))
DEFINE_METASIG(IM(Obj_Bool_RetVoid, j F, v))
DEFINE_METASIG_T(IM(Str_BindingFlags_Obj_ArrInt_RefMessageData_RetObj, s g(BINDING_FLAGS) j a(i) r(g(MESSAGE_DATA)), j))
DEFINE_METASIG_T(IM(Obj_Obj_BindingFlags_Binder_CultureInfo_RetVoid, j j g(BINDING_FLAGS) C(BINDER) C(CULTURE_INFO), v))
DEFINE_METASIG_T(IM(Obj_Obj_BindingFlags_Binder_ArrObj_CultureInfo_RetVoid, j j g(BINDING_FLAGS) C(BINDER) a(j) C(CULTURE_INFO), v))
DEFINE_METASIG_T(IM(Obj_BindingFlags_Binder_ArrObj_CultureInfo_RetObj, j g(BINDING_FLAGS) C(BINDER) a(j) C(CULTURE_INFO), j))
DEFINE_METASIG_T(IM(Obj_Type_CultureInfo_RetObj, j C(TYPE) C(CULTURE_INFO), j))
DEFINE_METASIG_T(IM(IPrincipal_RetVoid, C(IPRINCIPAL), v))
DEFINE_METASIG_T(IM(MemberInfo_RetVoid, C(MEMBER), v))
DEFINE_METASIG_T(IM(RuntimeMethodHandle_ArrObj_Obj_Int_Bool_RefArrObj_RetObj, g(METHOD_HANDLE) a(j) j i F r(a(j)), j))
DEFINE_METASIG_T(IM(CodeAccessPermission_RetBool, C(CODE_ACCESS_PERMISSION), F))
DEFINE_METASIG_T(IM(IPermission_RetIPermission, C(IPERMISSION), C(IPERMISSION)))
DEFINE_METASIG_T(IM(IPermission_RetBool, C(IPERMISSION), F))
DEFINE_METASIG_T(IM(PMS_RetVoid, C(PERMISSION_SET), v))
DEFINE_METASIG_T(IM(PMS_RetPMS, C(PERMISSION_SET), C(PERMISSION_SET)))
DEFINE_METASIG_T(IM(PMS_PMS_RetVoid, C(PERMISSION_SET) C(PERMISSION_SET), v))
DEFINE_METASIG_T(IM(PMS_RetBool, C(PERMISSION_SET), F))
DEFINE_METASIG(IM(RefObject_RetBool, r(j), F))
DEFINE_METASIG_T(IM(Type_RetObj, C(TYPE), j))
DEFINE_METASIG(IM(Int_VoidPtr_RetVoid, i P(v), v))
DEFINE_METASIG(IM(VoidPtr_RetVoid, P(v), v))
DEFINE_METASIG(IM(Void_RetInt, v, i))

DEFINE_METASIG_T(IM(Str_RetModule, s, C(MODULE)))
DEFINE_METASIG_T(IM(Str_RetAssembly, s, C(ASSEMBLY)))
DEFINE_METASIG_T(IM(Str_Str_Str_Assembly_Assembly_RetVoid, s s s C(ASSEMBLY) C(ASSEMBLY), v))
DEFINE_METASIG(IM(Str_Str_Obj_RetVoid, s s j, v))
DEFINE_METASIG(IM(Str_Str_RefObj_RetVoid, s s r(j), v))
DEFINE_METASIG(IM(Str_RetObj, s, j))
DEFINE_METASIG_T(IM(Str_RetFieldInfo, s, C(FIELD_INFO)))
DEFINE_METASIG_T(IM(Str_RetPropertyInfo, s, C(PROPERTY_INFO)))
DEFINE_METASIG(SM(Str_RetStr, s, s))
DEFINE_METASIG(IM(Str_ArrStr_ArrStr_RetVoid, s a(s) a(s), v))

DEFINE_METASIG(IM(Str_RetVoid, s, v))
DEFINE_METASIG_T(IM(Str_Exception_RetVoid, s C(EXCEPTION), v))
DEFINE_METASIG_T(IM(Evidence_Evidence_RetEvidence, C(EVIDENCE) C(EVIDENCE), C(EVIDENCE)))
DEFINE_METASIG(IM(Str_Obj_RetVoid, s j, v))
DEFINE_METASIG_T(IM(Str_BindingFlags_Binder_ArrType_ArrParameterModifier_RetMethodInfo, \
                 s g(BINDING_FLAGS) C(BINDER) a(C(TYPE)) a(g(PARAMETER_MODIFIER)), C(METHOD_INFO)))
DEFINE_METASIG_T(IM(Str_BindingFlags_Binder_Type_ArrType_ArrParameterModifier_RetPropertyInfo, \
                 s g(BINDING_FLAGS) C(BINDER) C(TYPE) a(C(TYPE)) a(g(PARAMETER_MODIFIER)), C(PROPERTY_INFO)))
DEFINE_METASIG(IM(Str_Str_RetStr, s s, s))
DEFINE_METASIG(IM(Str_Str_RetVoid, s s, v))
DEFINE_METASIG(IM(Str_Str_Int_RetVoid, s s i, v))
DEFINE_METASIG(IM(Str_Str_Str_Int_RetVoid, s s s i, v))
DEFINE_METASIG(IM(Str_Str_ArrByte_RetVoid, s s a(b), v))
DEFINE_METASIG_T(IM(Str_BindingFlags_RetFieldInfo, s g(BINDING_FLAGS), C(FIELD_INFO)))
DEFINE_METASIG_T(IM(Str_BindingFlags_RetMemberInfo, s g(BINDING_FLAGS), a(C(MEMBER))))
DEFINE_METASIG_T(IM(Str_BindingFlags_RetMethodInfo, s g(BINDING_FLAGS), C(METHOD_INFO)))
DEFINE_METASIG_T(IM(Str_BindingFlags_RetPropertyInfo, s g(BINDING_FLAGS), C(PROPERTY_INFO)))
DEFINE_METASIG_T(IM(Str_BindingFlags_Binder_Obj_ArrObj_ArrParameterModifier_CultureInfo_ArrStr_RetObj, \
                 s g(BINDING_FLAGS) C(BINDER) j a(j) a(g(PARAMETER_MODIFIER)) C(CULTURE_INFO) a(s), j))
DEFINE_METASIG_T(IM(Str_Delegate_RetMethodInfo, s C(DELEGATE), C(METHOD_INFO)))
DEFINE_METASIG_T(IM(Str_Type_Str_RetVoid, s C(TYPE) s, v))
DEFINE_METASIG_T(SM(Delegate_RetIntPtr, C(DELEGATE), I))
DEFINE_METASIG_T(SM(RuntimeTypeHandle_RetType, g(TYPE_HANDLE), C(TYPE)))
DEFINE_METASIG_T(SM(IntPtr_Type_RetDelegate, I C(TYPE), C(DELEGATE)))


DEFINE_METASIG_T(IM(Type_RetArrObj, C(TYPE) F, a(j)))
DEFINE_METASIG(IM(Bool_RetVoid, F, v))
DEFINE_METASIG_T(IM(BindingFlags_RetArrFieldInfo, g(BINDING_FLAGS), a(C(FIELD_INFO))))
DEFINE_METASIG_T(IM(BindingFlags_RetArrMemberInfo, g(BINDING_FLAGS), a(C(MEMBER))))
DEFINE_METASIG_T(IM(BindingFlags_RetArrMethodInfo, g(BINDING_FLAGS), a(C(METHOD_INFO))))
DEFINE_METASIG_T(IM(BindingFlags_RetArrPropertyInfo, g(BINDING_FLAGS), a(C(PROPERTY_INFO))))
DEFINE_METASIG(IM(ArrByte_RetVoid, a(b), v))
DEFINE_METASIG_T(IM(ArrByte_HostProtectionResource_HostProtectionResource_RetBool, a(b) g(HOST_PROTECTION_RESOURCE) g(HOST_PROTECTION_RESOURCE), F))
DEFINE_METASIG(IM(ArrByte_RetBool, a(b), F))
DEFINE_METASIG(IM(ArrChar_RetVoid, a(u), v))
DEFINE_METASIG(IM(ArrChar_Int_Int_RetVoid, a(u) i i, v))
DEFINE_METASIG_T(IM(ArrType_ArrException_Str_RetVoid, a(C(TYPE)) a(C(EXCEPTION)) s, v))
DEFINE_METASIG(IM(RefInt_RefInt_RefInt_RetArrByte, r(i) r(i) r(i), a(b)))
DEFINE_METASIG_T(IM(RefInt_RefRuntimeTypeHandle_RetVoid, r(i) r(g(TYPE_HANDLE)), v))

DEFINE_METASIG_T(IM(RuntimeArgumentHandle_PtrVoid_RetVoid, g(ARGUMENT_HANDLE) P(v), v))
DEFINE_METASIG_T(IM(SecurityPermissionFlag_RetVoid, g(SECURITY_PERMISSION_FLAG), v))
DEFINE_METASIG_T(IM(PermissionState_RetVoid, g(PERMISSION_STATE), v))
DEFINE_METASIG_T(IM(SecurityAction_RetVoid, g(SECURITY_ACTION), v))
DEFINE_METASIG_T(IM(ReflectionPermissionFlag_RetVoid, g(REFLECTION_PERMISSION_FLAG), v))
DEFINE_METASIG_T(IM(LicenseInteropHelper_GetCurrentContextInfo, r(i) r(I) g(TYPE_HANDLE), v))
DEFINE_METASIG(IM(LicenseInteropHelper_SaveKeyInCurrentContext, I, v))
DEFINE_METASIG_T(SM(LicenseInteropHelper_AllocateAndValidateLicense, g(TYPE_HANDLE) I i, j))
DEFINE_METASIG_T(SM(LicenseInteropHelper_RequestLicKey, g(TYPE_HANDLE) r(I), i))
DEFINE_METASIG_T(IM(LicenseInteropHelper_GetLicInfo, g(TYPE_HANDLE) r(i) r(i), v))

// App Domain related defines
DEFINE_METASIG(IM(Bool_Str_Str_RetVoid, F s s, v))
DEFINE_METASIG_T(IM(LoaderOptimization_RetVoid, g(LOADER_OPTIMIZATION), v))
DEFINE_METASIG_T(SM(Str_Evidence_AppDomainSetup_RetAppDomain, s C(EVIDENCE) C(APPDOMAIN_SETUP), C(APP_DOMAIN)))
DEFINE_METASIG_T(SM(Str_Evidence_Str_Str_Bool_RetAppDomain, s C(EVIDENCE) s s F, C(APP_DOMAIN)))
DEFINE_METASIG_T(SM(Str_RetAppDomain, s, C(APP_DOMAIN)))
DEFINE_METASIG_T(SM(AppDomain_RetVoid, C(APP_DOMAIN), v))
DEFINE_METASIG_T(SM(AppDomain_Str_AppDomainSetup_Evidence_Evidence_IntPtr_RetObj, C(APP_DOMAIN) s C(APPDOMAIN_SETUP) C(EVIDENCE) C(EVIDENCE) I, j))

// Object Clone 
DEFINE_METASIG_T(IM(SerInfo_RetVoid, C(SERIALIZATION_INFO), v))
DEFINE_METASIG_T(IM(SerInfo_StrContext_RetVoid, C(SERIALIZATION_INFO) g(STREAMING_CONTEXT), v))
DEFINE_METASIG(SM(Obj_OutStr_OutStr_OutArrStr_OutArrObj_RetObj, j r(s) r(s) r(a(s)) r(a(j)), j))
DEFINE_METASIG_T(SM(Obj_ArrStr_ArrObj_OutStreamingContext_RetSerializationInfo, j a(s) a(j) r(g(STREAMING_CONTEXT)), C(SERIALIZATION_INFO)))

// Execution Context
DEFINE_METASIG_T(SM(SyncCtx_ArrIntPtr_Bool_Int_RetInt, C(SYNCHRONIZATION_CONTEXT) a(I) F i, i))

// HostProtectionException
DEFINE_METASIG_T(IM(HPR_HPR_RetVoid, g(HOST_PROTECTION_RESOURCE) g(HOST_PROTECTION_RESOURCE), v))

// ThreadPool
DEFINE_METASIG(SM(Obj_Bool_RetVoid, j F, v))

// fields - e.g.:
// DEFINE_METASIG(Fld(PtrVoid, P(v)))

// Runtime Helpers
DEFINE_METASIG(SM(Obj_Obj_Bool_RetVoid, j j F, v))

DEFINE_METASIG_T(SM(Dec_RetDec, g(DECIMAL), g(DECIMAL)))

// Undefine macros in case we include the file again in the compilation unit

#undef  DEFINE_METASIG
#undef  DEFINE_METASIG_T

#undef METASIG_BODY
#undef METASIG_ATOM
#undef METASIG_RECURSE


#undef SM
#undef IM
#undef Fld

#undef a
#undef P
#undef r
#undef b
#undef u
#undef d
#undef f
#undef i
#undef K
#undef I
#undef U
#undef l
#undef L
#undef h
#undef H
#undef v
#undef B
#undef F
#undef j
#undef s
#undef C
#undef g
#undef T

#undef _


#endif // DEFINE_METASIG
