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
/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XX                                                                           XX
XX                          _typeInfo                                        XX
XX                                                                           XX
XX                                                                           XX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
*/

/*****************************************************************************
 This header file is named _typeInfo.h to be distinguished from typeinfo.h
 in the NT SDK
******************************************************************************/

#include <specstrings.h>

/*****************************************************************************/
#ifndef _TYPEINFO_H_
#define _TYPEINFO_H_
/*****************************************************************************/

class typeInfo;
typedef typeInfo vertype;

typedef BYTE var_types;
void GVerifyOrReturn(int cond, __in __in_z char *message);
void GVerifyOrReturn(int cond, HRESULT message);
#define GVerify(cond, msg) GVerifyOrReturn(cond, msg)

enum ti_types
{
    TI_ERROR,

    TI_REF,
    TI_STRUCT,
    TI_METHOD,

    TI_ONLY_ENUM = TI_METHOD,   //Enum values above this are completely described by the enumeration

    TI_BYTE,
    TI_SHORT,
    TI_INT,
    TI_LONG,
    TI_I,
    TI_FLOAT,
    TI_DOUBLE,
    TI_NULL,
    TI_PTR,

    TI_COUNT
};

enum VerErrTypeEnum
{
    LocalError  = 1,
    GlobalError = 2,
    TokenValid  = 4,
    OffsetValid = 8
};

// going to OR these together
typedef UINT VerErrType;



/*
static char * ti_names[] =
{
    "TI_ERROR",
    "TI_REF",
    "TI_STRUCT",
    "TI_METHOD",
    "TI_BYTE",
    "TI_SHORT",
    "TI_INT",
    "TI_LONG",
    "TI_I",
    "TI_FLOAT",
    "TI_DOUBLE",
    "TI_NULL",
    "TI_COUNT"
};
*/

// typeInfo does not care about distinction between signed/unsigned
// This routine converts all unsigned types to signed ones
/*
inline ti_types varType2tiType(var_types type)
{
    static const ti_types map[] =
    {
#define DEF_TP(tn,nm,jitType,verType,sz,sze,asze,st,al,tf,howUsed) verType,
#include "typelist.h"
#undef  DEF_TP
    };

    assertx(map[TYP_BYTE] == TI_BYTE);
    assertx(map[TYP_INT] == TI_INT);
    assertx(map[TYP_UINT] == TI_INT);
    assertx(map[TYP_FLOAT] == TI_FLOAT);
    assertx(map[TYP_BYREF] == TI_ERROR);
    assertx(map[type] != TI_ERROR);
    return map[type];
}
*/

// Convert the type returned from the VM to a ti_type.

#define assertx(x)


#ifdef DEBUG
#define WHENDEBUG(x)          x
#define PREFIX_ASSERT(x)    assert(x)
#else
#define WHENDEBUG(x)
#define PREFIX_ASSERT(x)
#endif

static const ti_types map_ti_type[CORINFO_TYPE_COUNT] =
    { // see the definition of enum CorInfoType in file inc/corinfo.h
      TI_ERROR,        // CORINFO_TYPE_UNDEF
      TI_ERROR,        // CORINFO_TYPE_VOID
      TI_BYTE,         // CORINFO_TYPE_BOOL
      TI_SHORT,        // CORINFO_TYPE_CHAR
      TI_BYTE,         // CORINFO_TYPE_BYTE
      TI_BYTE,         // CORINFO_TYPE_UBYTE
      TI_SHORT,        // CORINFO_TYPE_SHORT
      TI_SHORT,        // CORINFO_TYPE_USHORT
      TI_INT,          // CORINFO_TYPE_INT
      TI_INT,          // CORINFO_TYPE_UINT
      TI_LONG,         // CORINFO_TYPE_LONG
      TI_LONG,         // CORINFO_TYPE_ULONG
      TI_I,            // CORINFO_TYPE_NATIVEINT
      TI_I,            // CORINFO_TYPE_NATIVEUINT
      TI_FLOAT,        // CORINFO_TYPE_FLOAT
      TI_DOUBLE,       // CORINFO_TYPE_DOUBLE
      TI_REF,          // CORINFO_TYPE_STRING
      TI_PTR,          // CORINFO_TYPE_PTR
      TI_ERROR,        // CORINFO_TYPE_BYREF
      TI_STRUCT,       // CORINFO_TYPE_VALUECLASS
      TI_REF,          // CORINFO_TYPE_CLASS
      TI_STRUCT,       // CORINFO_TYPE_REFANY
      TI_REF,          // CORINFO_TYPE_VAR
    };

inline ti_types JITtype2tiType(CorInfoType type)
{
    // spot check to make certain enumerations have not changed
    assertx(map_ti_type[CORINFO_TYPE_CLASS]      == TI_REF);
    assertx(map_ti_type[CORINFO_TYPE_BYREF]      == TI_ERROR);
    assertx(map_ti_type[CORINFO_TYPE_DOUBLE]     == TI_DOUBLE);
    assertx(map_ti_type[CORINFO_TYPE_VALUECLASS] == TI_STRUCT);
    assertx(map_ti_type[CORINFO_TYPE_STRING]     == TI_REF);

    type = CorInfoType(type & CORINFO_TYPE_MASK); // strip off modifiers

    assertx(type < CORINFO_TYPE_COUNT);

    assertx(map_ti_type[type] != TI_ERROR || type == CORINFO_TYPE_VOID);
    return map_ti_type[type];
};

/*****************************************************************************
 * Declares the typeInfo class, which represents the type of an entity on the
 * stack, in a local variable or an argument.
 *
 * Flags: LLLLLLLLLLLLLLLLffffffffffTTTTTT
 *
 * L = local var # or instance field #
 * x = unused
 * f = flags
 * T = type
 *
 * The lower bits are used to store the type component, and may be one of:
 *
 * TI_* (primitive)   - see tyelist.h for enumeration (BYTE, SHORT, INT..)
 * TI_REF             - OBJREF / ARRAY use m_cls for the type
 *                       (including arrays and null objref)
 * TI_STRUCT          - VALUE type, use m_cls for the actual type
 *
 * NOTE carefully that BYREF info is not stored here.  You will never see a
 * TI_BYREF in this component.  For example, the type component
 * of a "byref TI_INT" is TI_FLAG_BYREF | TI_INT.
 *
 * NOTE carefully that Generic Type Variable info is
 * only stored here in part.  Values of type "T" (e.g "!0" in ILASM syntax),
 * i.e. some generic variable type, appear only when verifying generic
 * code.  They come in two flavours: unboxed and boxed.  Unboxed
 * is the norm, e.g. a local, field or argument of type T.  Boxed
 * values arise from an IL instruction such as "box !0".
 * The EE provides type handles for each different type
 * variable and the EE's "canCast" operation decides casting
 * for boxed type variable. Thus:
 *
 *    (TI_REF, <type-variable-type-handle>) == boxed type variable
 *
 *    (TI_REF, <type-variable-type-handle>)
 *          + TI_FLAG_GENERIC_TYPE_VAR      == unboxed type variable
 *
 * Using TI_REF for these may seem odd but using TI_STRUCT means the
 * code-generation parts of the importer get confused when they
 * can't work out the size, GC-ness etc. of the "struct".  So using TI_REF
 * just tricks these backend parts into generating pseudo-trees for
 * the generic code we're verifying.  These trees then get thrown away
 * anyway as we do verification of genreic code in import-only mode.
 *
*/

    // TI_COUNT is less than or equal to TI_FLAG_DATA_MASK

#define TI_FLAG_DATA_BITS              6
#define TI_FLAG_DATA_MASK              ((1 << TI_FLAG_DATA_BITS)-1)

    // Flag indicating this item is uninitialised
    // Note that if UNINIT and BYREF are both set,
    // it means byref (uninit x) - i.e. we are pointing to an uninit <something>

#define TI_FLAG_UNINIT_OBJREF          0x00000040

    // Flag indicating this item is a byref <something>

#define TI_FLAG_BYREF                  0x00000080

    // This item is a byref generated using the readonly. prefix
    // to a ldelema or Address function on an array type.  The
    // runtime type check is ignored in these cases, but the
    // resulting byref can only be used in order to perform a
    // constraint call.

#define TI_FLAG_BYREF_READONLY   0x00000100

    // This item contains the 'this' pointer (used for tracking)

#define TI_FLAG_THIS_PTR               0x00001000

// This item is a byref to something which has a permanent home 
// (e.g. a static field, or instance field of an object in GC heap, as 
// opposed to the stack or a local variable).  TI_FLAG_BYREF must also be 
// set. This information is useful for tail calls and return byrefs.
//
// Instructions that generate a permanent home byref:
//  
//  ldelema
//  ldflda of a ref object or another permanent home byref
//  array element address Get() helper
//  call or calli to a method that returns a byref and is verifiable or SkipVerify
//  dup
//  unbox

#define TI_FLAG_BYREF_PERMANENT_HOME   0x00002000


// This is for use when verifying generic code.
// This indicates that the type handle is really an unboxed
// generic type variable (e.g. the result of loading an argument
// of type T in a class List<T>).  Without this flag
// the same type handle indicates a boxed generic value,
// e.g. the result of a "box T" instruction.
#define TI_FLAG_GENERIC_TYPE_VAR   0x00004000

    // Number of bits local var # is shifted

#define TI_FLAG_LOCAL_VAR_SHIFT       16
#define TI_FLAG_LOCAL_VAR_MASK        0xFFFF0000

    // Field info uses the same space as the local info

#define TI_FLAG_FIELD_SHIFT           TI_FLAG_LOCAL_VAR_SHIFT
#define TI_FLAG_FIELD_MASK            TI_FLAG_LOCAL_VAR_MASK

#define TI_ALL_BYREF_FLAGS           (TI_FLAG_BYREF|                    \
                                      TI_FLAG_BYREF_READONLY |          \
                                      TI_FLAG_BYREF_PERMANENT_HOME)

/*****************************************************************************
 * A typeInfo can be one of several types:
 * - A primitive type (I4,I8,R4,R8,I)
 * - A type (ref, array, value type) (m_cls describes the type)
 * - An array (m_cls describes the array type)
 * - A byref (byref flag set, otherwise the same as the above),
 * - A Function Pointer (m_method)
 * - A byref local variable (byref and byref local flags set), can be
 *   uninitialised
 *
 * The reason that there can be 2 types of byrefs (general byrefs, and byref
 * locals) is that byref locals initially point to uninitialised items.
 * Therefore these byrefs must be tracked specialy.
 */

class typeInfo
{
    friend  class   Compiler;

public:
//private:
    union {
            // Right now m_bits is for debugging,
         struct {
            ti_types type       : 6;
            unsigned uninitobj  : 1;    // used
            unsigned byref      : 1;    // used
            unsigned byref_readonly: 1;
            unsigned : 3;               // unused?
            unsigned thisPtr    : 1;    // used
            unsigned : 1;               // unused?
            unsigned generic_type_var: 1; // used
        } m_bits;

        DWORD       m_flags;
     };

    union {
            // Valid only for TI_STRUCT or TI_REF
        CORINFO_CLASS_HANDLE  m_cls;
            // Valid only for type TI_METHOD
        CORINFO_METHOD_HANDLE m_method;
    };

public:
    typeInfo():m_flags(TI_ERROR)
    {
        m_bits.type = TI_ERROR;
        WHENDEBUG(m_cls = BAD_CLASS_HANDLE);
    }

    typeInfo(ti_types tiType)
    {
        assertx((tiType >= TI_BYTE) && (tiType <= TI_NULL));
        assertx(tiType <= TI_FLAG_DATA_MASK);

        m_flags = (DWORD) tiType;
        WHENDEBUG(m_cls = BAD_CLASS_HANDLE);
    }


    typeInfo(CorInfoType varType)
    {
        m_flags = (DWORD) JITtype2tiType(varType);
        WHENDEBUG(m_cls = BAD_CLASS_HANDLE);
    }

    typeInfo(ti_types tiType, CORINFO_CLASS_HANDLE cls, bool typeVar = false)
    {
        assertx(tiType == TI_STRUCT || tiType == TI_REF);
        assertx(cls != 0 && cls != CORINFO_CLASS_HANDLE(0xcccccccc));
        m_flags = tiType;
        if (typeVar)
            m_flags |= TI_FLAG_GENERIC_TYPE_VAR;
        m_cls   = cls;
    }

    typeInfo(CORINFO_METHOD_HANDLE method)
    {
        assertx(method != 0 && method != CORINFO_METHOD_HANDLE(0xcccccccc));
        m_flags = TI_METHOD;
        m_method = method;
    }


    int operator==(const typeInfo& ti)  const
    {
        ti_types ltype, rtype;
        ltype = GetRawType();
        rtype = ti.GetRawType();

        // I interchanges with INT
        // except when we are talking about addresses
        if (ltype != rtype)
        {
            if (!IsByRef())
            {
                if (!((ltype == TI_I && rtype == TI_INT)
                      || (ltype == TI_INT && rtype == TI_I)))
                    return false;
            }
            else
            {
                return false;
            }
        }

        if (ltype == TI_PTR || rtype == TI_PTR)
            return false;


        if ((m_flags & (TI_FLAG_BYREF|TI_FLAG_BYREF_READONLY|TI_FLAG_GENERIC_TYPE_VAR|TI_FLAG_UNINIT_OBJREF)) !=
            (ti.m_flags & (TI_FLAG_BYREF|TI_FLAG_BYREF_READONLY|TI_FLAG_GENERIC_TYPE_VAR|TI_FLAG_UNINIT_OBJREF)))
            return false;

        assertx(TI_ERROR < TI_ONLY_ENUM);        // TI_ERROR looks like it needs more than enum.  This optimises the success case a bit
        if (ltype > TI_ONLY_ENUM)
            return true;
        if (ltype == TI_ERROR)
            return false;       // TI_ERROR != TI_ERROR
        assertx(m_cls != BAD_CLASS_HANDLE && ti.m_cls != BAD_CLASS_HANDLE);
        return m_cls == ti.m_cls;
    }

    static BOOL tiMergeToCommonParent     (ICorJitInfo *jitInfo, typeInfo *pDest, 
                                                  const typeInfo *pSrc) ;
    static BOOL tiCompatibleWith          (ICorJitInfo *jitInfo, const typeInfo& child,
                                                  const typeInfo& parent) ;

    static BOOL tiMergeCompatibleWith     (ICorJitInfo *jitInfo, const typeInfo& child,
                                          const typeInfo& parent) ;
    /////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////

    void SetIsThisPtr()
    {
        m_flags |= TI_FLAG_THIS_PTR;
        assertx(m_bits.thisPtr);
    }

    void ClearThisPtr()
    {
        m_flags &= ~(TI_FLAG_THIS_PTR);
    }

    void SetIsPermanentHomeByRef()
    {
        assertx(IsByRef());
        m_flags |= TI_FLAG_BYREF_PERMANENT_HOME;
    }

    void SetIsReadonlyByRef()
    {
        assertx(IsByRef());
        m_flags |= TI_FLAG_BYREF_READONLY;
    }

    // Set that this item is uninitialised.
    void SetUninitialisedObjRef()
    {
        assertx((IsObjRef() && IsThisPtr()));
        // For now, this is used only  to track uninit this ptrs in ctors

        m_flags |= TI_FLAG_UNINIT_OBJREF;
        assertx(m_bits.uninitobj);
    }

    // Set that this item is initialised.
    void SetInitialisedObjRef()
    {
        assertx((IsObjRef() && IsThisPtr()));
        // For now, this is used only  to track uninit this ptrs in ctors

        m_flags &= ~TI_FLAG_UNINIT_OBJREF;
    }

    typeInfo& DereferenceByRef()
    {
        if (!IsByRef()) {
            m_flags = TI_ERROR;
            WHENDEBUG(m_cls = BAD_CLASS_HANDLE);
        }
        m_flags &= ~(TI_FLAG_THIS_PTR | TI_ALL_BYREF_FLAGS);
        return *this;
    }

    typeInfo& MakeByRef()
    {
        assertx(!IsByRef());
        m_flags &= ~(TI_FLAG_THIS_PTR);
        m_flags |= TI_FLAG_BYREF;
        return *this;
    }

    // I1,I2 --> I4
    // FLOAT --> DOUBLE
    // objref, arrays, byrefs, value classes are unchanged
    //
    typeInfo& NormaliseForStack()
    {
        switch (GetType())
        {
        case TI_BYTE:
        case TI_SHORT:
            m_flags = TI_INT;
            break;

        case TI_FLOAT:
            m_flags = TI_DOUBLE;
            break;
        default:
            break;
        }
        return (*this);
    }

    /////////////////////////////////////////////////////////////////////////
    // Getters
    /////////////////////////////////////////////////////////////////////////

    CORINFO_CLASS_HANDLE GetClassHandle()  const
    {
        //crusso: not sure about this conditional
        if (!IsType(TI_REF) && !IsType(TI_STRUCT))
            return 0;

        return m_cls;
    }

    CORINFO_CLASS_HANDLE GetClassHandleForValueClass()  const
    {
        assertx(IsType(TI_STRUCT));
        assertx(m_cls && m_cls != BAD_CLASS_HANDLE);
        return m_cls;
    }

    CORINFO_CLASS_HANDLE GetClassHandleForObjRef()  const
    {
        assertx(IsType(TI_REF));
        assertx(m_cls && m_cls != BAD_CLASS_HANDLE);
        return m_cls;
    }

    CORINFO_METHOD_HANDLE GetMethod()  const
    {
        assertx(GetType() == TI_METHOD);
        return m_method;
    }

    // Get this item's type
    // If primitive, returns the primitive type (TI_*)
    // If not primitive, returns:
    //  - TI_ERROR if a byref anything
    //  - TI_REF if a class or array or null or a generic type variable
    //  - TI_STRUCT if a value class
    ti_types GetType() const
    {
        if (m_flags & TI_FLAG_BYREF)
            return TI_ERROR;

        // objref/array/null (objref), value class, ptr, primitive
        return GetRawType();
    }

    ti_types GetRawType() const
    {
        return (ti_types)(m_flags & TI_FLAG_DATA_MASK);
    }

    BOOL IsType(ti_types type) const {
        assertx(type != TI_ERROR);
        return (m_flags & (TI_FLAG_DATA_MASK|TI_ALL_BYREF_FLAGS|TI_FLAG_GENERIC_TYPE_VAR)) == DWORD(type);
    }

    // Returns whether this is an objref
    BOOL IsObjRef() const
    {
        return IsType(TI_REF) || IsType(TI_NULL);
    }

    // Returns whether this is a by-ref
    BOOL IsByRef() const
    {
        return (m_flags & TI_FLAG_BYREF);
    }

    // Returns whether this is the this pointer
    BOOL IsThisPtr() const
    {
        return (m_flags & TI_FLAG_THIS_PTR);
    }

    BOOL IsUnboxedGenericTypeVar() const
    {
        return !IsByRef() && (m_flags & TI_FLAG_GENERIC_TYPE_VAR);
    }

    BOOL IsReadonlyByRef() const
    {
        return IsByRef() && (m_flags & TI_FLAG_BYREF_READONLY);
    }

    BOOL IsPermanentHomeByRef() const
    {
        return IsByRef() && (m_flags & TI_FLAG_BYREF_PERMANENT_HOME);
    }

    // Returns whether this is a method desc
    BOOL IsMethod() const
    {
        return (GetType() == TI_METHOD);
    }

    // A byref value class is NOT a value class
    BOOL IsValueClass() const
    {
        return (IsType(TI_STRUCT) || IsPrimitiveType());
    }

    // Does not return true for primitives. Will return true for value types that behave
    // as primitives
    BOOL IsValueClassWithClsHnd() const
    {
        if ((GetType() == TI_STRUCT) ||
             (m_cls && GetType() != TI_REF && 
             GetType() != TI_METHOD && 
             GetType() != TI_ERROR)) // necessary because if byref bit is set, we return TI_ERROR)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    // Returns whether this is an integer or real number
    // NOTE: Use NormaliseToPrimitiveType() if you think you may have a
    // System.Int32 etc., because those types are not considered number
    // types by this function.
    BOOL IsNumberType() const
    {
        ti_types Type = GetType();

        // I1, I2, Boolean, character etc. cannot exist nakedly -
        // everything is at least an I4

        return (Type == TI_INT ||
                Type == TI_LONG ||
                Type == TI_I ||
                Type == TI_DOUBLE);
    }

    // Returns whether this is an integer
    // NOTE: Use NormaliseToPrimitiveType() if you think you may have a
    // System.Int32 etc., because those types are not considered number
    // types by this function.
    BOOL IsIntegerType() const
    {
        ti_types Type = GetType();

        // I1, I2, Boolean, character etc. cannot exist nakedly -
        // everything is at least an I4

        return (Type == TI_INT
                || Type == TI_LONG
                || Type == TI_I
                );
    }

    // Returns whether this is a primitive type (not a byref, objref,
    // array, null, value class, invalid value)
    // May Need to normalise first (m/r/I4 --> I4)
    BOOL IsPrimitiveType() const
    {
        DWORD Type = GetType();

        // boolean, char, u1,u2 never appear on the operand stack
        return (Type == TI_BYTE ||
                Type == TI_SHORT ||
                Type == TI_INT ||
                Type == TI_I   ||
                Type == TI_LONG ||
                Type == TI_FLOAT ||
                Type == TI_DOUBLE);
    }


    // Returns whether this is the null objref
    BOOL IsNullObjRef() const
    {
        return (IsType(TI_NULL));
    }

    // must be for a local which is an object type (i.e. has a slot >= 0)
    // for primitive locals, use the liveness bitmap instead
    // Note that this works if the error is 'Byref'
    BOOL IsDead() const
    {
        return GetRawType() == TI_ERROR || GetRawType() == TI_PTR;
    }

    BOOL IsUninitialisedObjRef() const
    {
        return (m_flags & TI_FLAG_UNINIT_OBJREF);
    }

    // In the watch window of the debugger, type tiVarName.ToStaticString()
    // to view a string representation of this instance.

    void Dump () const
    {
        ti_types tiType = GetType();
        if (tiType == TI_REF || tiType == TI_STRUCT)
            printf("< %s m:%p isbyref:%d isreadonly:%d isthis:%d isvar:%d>", ToStaticString(), m_method, IsByRef(), IsReadonlyByRef(), IsThisPtr(), (m_flags & TI_FLAG_GENERIC_TYPE_VAR)!=0);
        else
            printf("< %s >", ToStaticString());

    }

    void ToString(__out_ecount (nBufLen) WCHAR* pBuf, int nBufLen, ICorJitInfo* pJI, CORINFO_METHOD_HANDLE ctxt) const;

    char *ToStaticString () const
    {
#define TI_DEBUG_STR_LEN 100

        assertx(TI_COUNT <= TI_FLAG_DATA_MASK);

        static char str[TI_DEBUG_STR_LEN];
        char *      p = "";
        ti_types    tiType;

        str[0] = 0;

        if (IsMethod())
        {
            strcpy_s(str, TI_DEBUG_STR_LEN, "method");
            return str;
        }

        if (IsByRef())
            strcat_s(str, TI_DEBUG_STR_LEN, "&");

        if (IsNullObjRef())
            strcat_s(str, TI_DEBUG_STR_LEN, "nullref");

        if (IsUninitialisedObjRef())
            strcat_s(str, TI_DEBUG_STR_LEN, "<uninit>");

        if (IsPermanentHomeByRef())
            strcat_s(str, TI_DEBUG_STR_LEN, "<permanent home>");

        if (IsThisPtr())
            strcat_s(str, TI_DEBUG_STR_LEN, "<this>");

        if (m_flags & TI_FLAG_GENERIC_TYPE_VAR)
            strcat_s(str, TI_DEBUG_STR_LEN, "<generic>");

        tiType = GetRawType();


        switch (tiType)
        {
        default:
            p = "<<internal error>>";
            break;

        case TI_BYTE:
            p = "byte";
            break;

        case TI_SHORT:
            p = "short";
            break;

        case TI_INT:
            p = "int";
            break;

        case TI_LONG:
            p = "long";
            break;

        case TI_I:
            p = "native int";
            break;

        case TI_FLOAT:
            p = "float";
            break;

        case TI_DOUBLE:
            p = "double";
            break;

        case TI_REF:
            p = "ref";
            break;

        case TI_STRUCT:
            p = "struct";
            break;

        case TI_PTR:
            p = "pointer";
            break;

        case TI_ERROR:
            p = "error";
            break;
        }

        strcat_s(str, TI_DEBUG_STR_LEN, " ");
        strcat_s(str, TI_DEBUG_STR_LEN, p);

        return str;
    }


private:
        // used to make functions that return typeinfo efficient.
    typeInfo(DWORD flags, CORINFO_CLASS_HANDLE cls)
    {
        m_cls   = cls;
        m_flags = flags;
    }

    friend typeInfo ByRef(const typeInfo& ti);
    friend typeInfo DereferenceByRef(const typeInfo& ti);
    friend typeInfo NormaliseForStack(const typeInfo& ti);
};

inline
typeInfo NormaliseForStack(const typeInfo& ti)
{
    return typeInfo(ti).NormaliseForStack();
}

    // given ti make a byref to that type.
inline
typeInfo ByRef(const typeInfo& ti)
{
    return typeInfo(ti).MakeByRef();
}


    // given ti which is a byref, return the type it points at
inline
typeInfo DereferenceByRef(const typeInfo& ti)
{
    return typeInfo(ti).DereferenceByRef();
}

BOOL tiCompatibleWith (ICorJitInfo *jitInfo,
                       const typeInfo& child,
                       const typeInfo& parent);

BOOL tiMergeToCommonParent (ICorJitInfo *jitInfo,
                            typeInfo *pDest,
                            const typeInfo *pSrc);



#define MVER_E_HRESULT                                VER_E_HRESULT
#define MVER_E_OFFSET                                 VER_E_OFFSET
#define MVER_E_OPCODE                                 VER_E_OPCODE
#define MVER_E_OPERAND                                VER_E_OPERAND
#define MVER_E_TOKEN                                  VER_E_TOKEN
#define MVER_E_EXCEPT                                 VER_E_EXCEPT
#define MVER_E_STACK_SLOT                             VER_E_STACK_SLOT
#define MVER_E_LOC                                    VER_E_LOC
#define MVER_E_LOC_BYNAME                             VER_E_LOC_BYNAME
#define MVER_E_ARG                                    VER_E_ARG
#define MVER_E_FOUND                                  VER_E_FOUND
#define MVER_E_EXPECTED                               VER_E_EXPECTED
#define MVER_E_UNKNOWN_OPCODE                         VER_E_UNKNOWN_OPCODE
#define MVER_E_SIG_CALLCONV                           VER_E_SIG_CALLCONV
#define MVER_E_SIG_ELEMTYPE                           VER_E_SIG_ELEMTYPE
#define MVER_E_RET_SIG                                VER_E_RET_SIG
#define MVER_E_FIELD_SIG                              VER_E_FIELD_SIG
#define MVER_E_INTERNAL                               VER_E_INTERNAL
#define MVER_E_STACK_TOO_LARGE                        VER_E_STACK_TOO_LARGE
#define MVER_E_ARRAY_NAME_LONG                        VER_E_ARRAY_NAME_LONG
#define MVER_E_FALLTHRU                               VER_E_FALLTHRU
#define MVER_E_TRY_GTEQ_END                           VER_E_TRY_GTEQ_END
#define MVER_E_TRYEND_GT_CS                           VER_E_TRYEND_GT_CS
#define MVER_E_HND_GTEQ_END                           VER_E_HND_GTEQ_END
#define MVER_E_HNDEND_GT_CS                           VER_E_HNDEND_GT_CS
#define MVER_E_TRY_START                              VER_E_TRY_START
#define MVER_E_HND_START                              VER_E_HND_START
#define MVER_E_TRY_OVERLAP                            VER_E_TRY_OVERLAP
#define MVER_E_TRY_EQ_HND_FIL                         VER_E_TRY_EQ_HND_FIL
#define MVER_E_TRY_SHARE_FIN_FAL                      VER_E_TRY_SHARE_FIN_FAL
#define MVER_E_HND_OVERLAP                            VER_E_HND_OVERLAP
#define MVER_E_HND_EQ                                 VER_E_HND_EQ
#define MVER_E_FIL_OVERLAP                            VER_E_FIL_OVERLAP
#define MVER_E_FIL_EQ                                 VER_E_FIL_EQ
#define MVER_E_FIL_CONT_TRY                           VER_E_FIL_CONT_TRY
#define MVER_E_FIL_CONT_HND                           VER_E_FIL_CONT_HND
#define MVER_E_FIL_CONT_FIL                           VER_E_FIL_CONT_FIL
#define MVER_E_FIL_GTEQ_CS                            VER_E_FIL_GTEQ_CS
#define MVER_E_FIL_START                              VER_E_FIL_START
#define MVER_E_FALLTHRU_EXCEP                         VER_E_FALLTHRU_EXCEP
#define MVER_E_FALLTHRU_INTO_HND                      VER_E_FALLTHRU_INTO_HND
#define MVER_E_FALLTHRU_INTO_FIL                      VER_E_FALLTHRU_INTO_FIL
#define MVER_E_LEAVE                                  VER_E_LEAVE
#define MVER_E_RETHROW                                VER_E_RETHROW
#define MVER_E_ENDFINALLY                             VER_E_ENDFINALLY
#define MVER_E_ENDFILTER                              VER_E_ENDFILTER
#define MVER_E_ENDFILTER_MISSING                      VER_E_ENDFILTER_MISSING
#define MVER_E_BR_INTO_TRY                            VER_E_BR_INTO_TRY
#define MVER_E_BR_INTO_HND                            VER_E_BR_INTO_HND
#define MVER_E_BR_INTO_FIL                            VER_E_BR_INTO_FIL
#define MVER_E_BR_OUTOF_TRY                           VER_E_BR_OUTOF_TRY
#define MVER_E_BR_OUTOF_HND                           VER_E_BR_OUTOF_HND
#define MVER_E_BR_OUTOF_FIL                           VER_E_BR_OUTOF_FIL
#define MVER_E_BR_OUTOF_FIN                           VER_E_BR_OUTOF_FIN
#define MVER_E_RET_FROM_TRY                           VER_E_RET_FROM_TRY
#define MVER_E_RET_FROM_HND                           VER_E_RET_FROM_HND
#define MVER_E_RET_FROM_FIL                           VER_E_RET_FROM_FIL
#define MVER_E_BAD_JMP_TARGET                         VER_E_BAD_JMP_TARGET
#define MVER_E_PATH_LOC                               VER_E_PATH_LOC
#define MVER_E_PATH_THIS                              VER_E_PATH_THIS
#define MVER_E_PATH_STACK                             VER_E_PATH_STACK
#define MVER_E_PATH_STACK_DEPTH                       VER_E_PATH_STACK_DEPTH
#define MVER_E_THIS                                   VER_E_THIS
#define MVER_E_THIS_UNINIT_EXCEP                      VER_E_THIS_UNINIT_EXCEP
#define MVER_E_THIS_UNINIT_STORE                      VER_E_THIS_UNINIT_STORE
#define MVER_E_THIS_UNINIT_RET                        VER_E_THIS_UNINIT_RET
#define MVER_E_THIS_UNINIT_V_RET                      VER_E_THIS_UNINIT_V_RET
#define MVER_E_THIS_UNINIT_BR                         VER_E_THIS_UNINIT_BR
#define MVER_E_LDFTN_CTOR                             VER_E_LDFTN_CTOR
#define MVER_E_STACK_NOT_EQ                           VER_E_STACK_NOT_EQ
#define MVER_E_STACK_UNEXPECTED                       VER_E_STACK_UNEXPECTED
#define MVER_E_STACK_EXCEPTION                        VER_E_STACK_EXCEPTION
#define MVER_E_STACK_OVERFLOW                         VER_E_STACK_OVERFLOW
#define MVER_E_STACK_UNDERFLOW                        VER_E_STACK_UNDERFLOW
#define MVER_E_STACK_EMPTY                            VER_E_STACK_EMPTY
#define MVER_E_STACK_UNINIT                           VER_E_STACK_UNINIT
#define MVER_E_STACK_I_I4_I8                          VER_E_STACK_I_I4_I8
#define MVER_E_STACK_I_I4                             VER_E_STACK_I_I4
#define MVER_E_STACK_R_R4_R8                          VER_E_STACK_R_R4_R8
#define MVER_E_STACK_NO_R_I8                          VER_E_STACK_NO_R_I8
#define MVER_E_STACK_NUMERIC                          VER_E_STACK_NUMERIC
#define MVER_E_STACK_OBJREF                           VER_E_STACK_OBJREF
#define MVER_E_STACK_P_OBJREF                         VER_E_STACK_P_OBJREF
#define MVER_E_STACK_BYREF                            VER_E_STACK_BYREF
#define MVER_E_STACK_METHOD                           VER_E_STACK_METHOD
#define MVER_E_STACK_ARRAY_SD                         VER_E_STACK_ARRAY_SD
#define MVER_E_STACK_VALCLASS                         VER_E_STACK_VALCLASS
#define MVER_E_STACK_P_VALCLASS                       VER_E_STACK_P_VALCLASS
#define MVER_E_STACK_NO_VALCLASS                      VER_E_STACK_NO_VALCLASS
#define MVER_E_LOC_DEAD                               VER_E_LOC_DEAD
#define MVER_E_LOC_NUM                                VER_E_LOC_NUM
#define MVER_E_ARG_NUM                                VER_E_ARG_NUM
#define MVER_E_TOKEN_RESOLVE                          VER_E_TOKEN_RESOLVE
#define MVER_E_TOKEN_TYPE                             VER_E_TOKEN_TYPE
#define MVER_E_TOKEN_TYPE_MEMBER                      VER_E_TOKEN_TYPE_MEMBER
#define MVER_E_TOKEN_TYPE_FIELD                       VER_E_TOKEN_TYPE_FIELD
#define MVER_E_TOKEN_TYPE_SIG                         VER_E_TOKEN_TYPE_SIG
#define MVER_E_UNVERIFIABLE                           VER_E_UNVERIFIABLE
#define MVER_E_LDSTR_OPERAND                          VER_E_LDSTR_OPERAND
#define MVER_E_RET_PTR_TO_STACK                       VER_E_RET_PTR_TO_STACK
#define MVER_E_RET_VOID                               VER_E_RET_VOID
#define MVER_E_RET_MISSING                            VER_E_RET_MISSING
#define MVER_E_RET_EMPTY                              VER_E_RET_EMPTY
#define MVER_E_RET_UNINIT                             VER_E_RET_UNINIT
#define MVER_E_ARRAY_ACCESS                           VER_E_ARRAY_ACCESS
#define MVER_E_ARRAY_V_STORE                          VER_E_ARRAY_V_STORE
#define MVER_E_ARRAY_SD                               VER_E_ARRAY_SD
#define MVER_E_ARRAY_SD_PTR                           VER_E_ARRAY_SD_PTR
#define MVER_E_ARRAY_FIELD                            VER_E_ARRAY_FIELD
#define MVER_E_ARGLIST                                VER_E_ARGLIST
#define MVER_E_VALCLASS                               VER_E_VALCLASS
#define MVER_E_METHOD_ACCESS                          VER_E_METHOD_ACCESS
#define MVER_E_FIELD_ACCESS                           VER_E_FIELD_ACCESS
#define MVER_E_DEAD                                   VER_E_DEAD
#define MVER_E_FIELD_STATIC                           VER_E_FIELD_STATIC
#define MVER_E_FIELD_NO_STATIC                        VER_E_FIELD_NO_STATIC
#define MVER_E_ADDR                                   VER_E_ADDR
#define MVER_E_ADDR_BYREF                             VER_E_ADDR_BYREF
#define MVER_E_ADDR_LITERAL                           VER_E_ADDR_LITERAL
#define MVER_E_INITONLY                               VER_E_INITONLY
#define MVER_E_WRITE_RVA_STATIC                       VER_E_WRITE_RVA_STATIC
#define MVER_E_THROW                                  VER_E_THROW
#define MVER_E_CALLVIRT_VALCLASS                      VER_E_CALLVIRT_VALCLASS
#define MVER_E_CALL_SIG                               VER_E_CALL_SIG
#define MVER_E_CALL_STATIC                            VER_E_CALL_STATIC
#define MVER_E_CTOR                                   VER_E_CTOR
#define MVER_E_CTOR_VIRT                              VER_E_CTOR_VIRT
#define MVER_E_CTOR_OR_SUPER                          VER_E_CTOR_OR_SUPER
#define MVER_E_CTOR_MUL_INIT                          VER_E_CTOR_MUL_INIT
#define MVER_E_SIG                                    VER_E_SIG
#define MVER_E_SIG_ARRAY                              VER_E_SIG_ARRAY
#define MVER_E_SIG_ARRAY_PTR                          VER_E_SIG_ARRAY_PTR
#define MVER_E_SIG_ARRAY_BYREF                        VER_E_SIG_ARRAY_BYREF
#define MVER_E_SIG_ELEM_PTR                           VER_E_SIG_ELEM_PTR
#define MVER_E_SIG_VARARG                             VER_E_SIG_VARARG
#define MVER_E_SIG_VOID                               VER_E_SIG_VOID
#define MVER_E_SIG_BYREF_BYREF                        VER_E_SIG_BYREF_BYREF
#define MVER_E_CODE_SIZE_ZERO                         VER_E_CODE_SIZE_ZERO
#define MVER_E_BAD_VARARG                             VER_E_BAD_VARARG
#define MVER_E_TAIL_CALL                              VER_E_TAIL_CALL
#define MVER_E_TAIL_BYREF                             VER_E_TAIL_BYREF
#define MVER_E_TAIL_RET                               VER_E_TAIL_RET
#define MVER_E_TAIL_RET_VOID                          VER_E_TAIL_RET_VOID
#define MVER_E_TAIL_RET_TYPE                          VER_E_TAIL_RET_TYPE
#define MVER_E_TAIL_STACK_EMPTY                       VER_E_TAIL_STACK_EMPTY
#define MVER_E_METHOD_END                             VER_E_METHOD_END
#define MVER_E_BAD_BRANCH                             VER_E_BAD_BRANCH
#define MVER_E_FIN_OVERLAP                            VER_E_FIN_OVERLAP
#define MVER_E_LEXICAL_NESTING                        VER_E_LEXICAL_NESTING
#define MVER_E_VOLATILE                               VER_E_VOLATILE
#define MVER_E_UNALIGNED                              VER_E_UNALIGNED
#define MVER_E_INNERMOST_FIRST                        VER_E_INNERMOST_FIRST
#define MVER_E_CALLI_VIRTUAL                          VER_E_CALLI_VIRTUAL
#define MVER_E_CALL_ABSTRACT                          VER_E_CALL_ABSTRACT
#define MVER_E_STACK_UNEXP_ARRAY                      VER_E_STACK_UNEXP_ARRAY
#define MVER_E_NOT_IN_GC_HEAP                         VER_E_NOT_IN_GC_HEAP
#define MVER_E_TRY_N_EMPTY_STACK                      VER_E_TRY_N_EMPTY_STACK
#define MVER_E_DLGT_CTOR                              VER_E_DLGT_CTOR
#define MVER_E_DLGT_BB                                VER_E_DLGT_BB
#define MVER_E_DLGT_PATTERN                           VER_E_DLGT_PATTERN
#define MVER_E_DLGT_LDFTN                             VER_E_DLGT_LDFTN
#define MVER_E_FTN_ABSTRACT                           VER_E_FTN_ABSTRACT
#define MVER_E_SIG_C_VC                               VER_E_SIG_C_VC
#define MVER_E_SIG_VC_C                               VER_E_SIG_VC_C
#define MVER_E_BOX_PTR_TO_STACK                       VER_E_BOX_PTR_TO_STACK
#define MVER_E_SIG_BYREF_TB_AH                        VER_E_SIG_BYREF_TB_AH
#define MVER_E_SIG_ARRAY_TB_AH                        VER_E_SIG_ARRAY_TB_AH
#define MVER_E_ENDFILTER_STACK                        VER_E_ENDFILTER_STACK
#define MVER_E_DLGT_SIG_I                             VER_E_DLGT_SIG_I
#define MVER_E_DLGT_SIG_O                             VER_E_DLGT_SIG_O
#define MVER_E_RA_PTR_TO_STACK                        VER_E_RA_PTR_TO_STACK
#define MVER_E_CATCH_VALUE_TYPE                       VER_E_CATCH_VALUE_TYPE
#define MVER_E_CATCH_BYREF                            VER_E_CATCH_BYREF
#define MVER_E_FIL_PRECEED_HND                        VER_E_FIL_PRECEED_HND
#define MVER_E_LDVIRTFTN_STATIC                       VER_E_LDVIRTFTN_STATIC
#define MVER_E_CALLVIRT_STATIC                        VER_E_CALLVIRT_STATIC
#define MVER_E_INITLOCALS                             VER_E_INITLOCALS
#define MVER_E_BR_TO_EXCEPTION                        VER_E_BR_TO_EXCEPTION
#define MVER_E_CALL_CTOR                              VER_E_CALL_CTOR
#define MVER_E_VALCLASS_OBJREF_VAR					  VER_E_VALCLASS_OBJREF_VAR
#define MVER_E_STACK_P_VALCLASS_OBJREF_VAR			  VER_E_STACK_P_VALCLASS_OBJREF_VAR
#define MVER_E_SIG_VAR_PARAM                          VER_E_SIG_VAR_PARAM
#define MVER_E_SIG_MVAR_PARAM                         VER_E_SIG_MVAR_PARAM
#define MVER_E_SIG_VAR_ARG                            VER_E_SIG_VAR_ARG
#define MVER_E_SIG_MVAR_ARG                           VER_E_SIG_MVAR_ARG
#define MVER_E_SIG_GENERICINST                        VER_E_SIG_GENERICINST
#define MVER_E_SIG_METHOD_INST                        VER_E_SIG_METHOD_INST
#define MVER_E_SIG_METHOD_PARENT_INST                 VER_E_SIG_METHOD_PARENT_INST
#define MVER_E_SIG_FIELD_PARENT_INST                  VER_E_SIG_FIELD_PARENT_INST
#define MVER_E_CALLCONV_NOT_GENERICINST               VER_E_CALLCONV_NOT_GENERICINST
#define MVER_E_TOKEN_BAD_METHOD_SPEC				  VER_E_TOKEN_BAD_METHOD_SPEC
#define MVER_E_BAD_READONLY_PREFIX                    VER_E_BAD_READONLY_PREFIX   
#define MVER_E_BAD_CONSTRAINED_PREFIX                 VER_E_BAD_CONSTRAINED_PREFIX
#define MVER_E_CIRCULAR_VAR_CONSTRAINTS               VER_E_CIRCULAR_VAR_CONSTRAINTS  
#define MVER_E_CIRCULAR_MVAR_CONSTRAINTS              VER_E_CIRCULAR_MVAR_CONSTRAINTS 

#define MVER_E_UNSATISFIED_METHOD_INST                VER_E_UNSATISFIED_METHOD_INST
#define MVER_E_UNSATISFIED_METHOD_PARENT_INST         VER_E_UNSATISFIED_METHOD_PARENT_INST
#define MVER_E_UNSATISFIED_FIELD_PARENT_INST          VER_E_UNSATISFIED_FIELD_PARENT_INST
#define MVER_E_UNSATISFIED_BOX_OPERAND                VER_E_UNSATISFIED_BOX_OPERAND
#define MVER_E_CONSTRAINED_CALL_WITH_NON_BYREF_THIS   VER_E_CONSTRAINED_CALL_WITH_NON_BYREF_THIS
#define MVER_E_CONSTRAINED_OF_NON_VARIABLE_TYPE       VER_E_CONSTRAINED_OF_NON_VARIABLE_TYPE
#define MVER_E_READONLY_UNEXPECTED_CALLEE             VER_E_READONLY_UNEXPECTED_CALLEE
#define MVER_E_READONLY_ILLEGAL_WRITE                 VER_E_READONLY_ILLEGAL_WRITE
#define MVER_E_READONLY_IN_MKREFANY                   VER_E_READONLY_IN_MKREFANY
#define MVER_E_UNALIGNED_ALIGNMENT                    VER_E_UNALIGNED_ALIGNMENT
#define MVER_E_TAILCALL_INSIDE_EH                     VER_E_TAILCALL_INSIDE_EH
#define MVER_E_BACKWARD_BRANCH                        VER_E_BACKWARD_BRANCH
#define MVER_E_CALL_TO_VTYPE_BASE                     VER_E_CALL_TO_VTYPE_BASE               

#define MVER_E_BAD_PE                                 VER_E_BAD_PE
#define MVER_E_BAD_MD                                 VER_E_BAD_MD

#define MVER_E_BAD_APPDOMAIN						  VER_E_BAD_APPDOMAIN
#define MVER_E_TYPELOAD								  VER_E_TYPELOAD
#define MVER_E_PE_LOAD								  VER_E_PE_LOAD
#define MVER_E_NEWOBJ_OF_ABSTRACT_CLASS               VER_E_NEWOBJ_OF_ABSTRACT_CLASS
#define MVER_E_FIELD_OVERLAP                          VER_E_FIELD_OVERLAP
#define MVER_E_LDFTN_NON_FINAL_VIRTUAL                VER_E_LDFTN_NON_FINAL_VIRTUAL
#define MVER_E_UNMANAGED_POINTER                      VER_E_UNMANAGED_POINTER                      
#define MVER_E_THIS_MISMATCH                          VER_E_THIS_MISMATCH




/*****************************************************************************/
#endif // _TYPEINFO_H_
/*****************************************************************************/
