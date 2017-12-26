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
/*============================================================
**
** Header:  COMVariant
**       
**
** Purpose: Headers for the Variant class.
**
** Date:  July 22, 1998
** 
===========================================================*/

#ifndef _COMVARIANT_H_
#define _COMVARIANT_H_

#include <cor.h>
#include "fcall.h"

class COMVariant;
class OleVariant;


//These types must be kept in sync with the CorElementTypes defined in cor.h
//NOTE: If you add values to this enum you need to look at COMOAVariant.cpp.  There is
//      a mapping between CV type and VT types found there.
//NOTE: This is also found in a table in OleVariant.cpp.
//NOTE: These are also found in Variant.cs
typedef enum
{
    CV_EMPTY               = 0x0,                   // CV_EMPTY
    CV_VOID                = ELEMENT_TYPE_VOID,
    CV_BOOLEAN             = ELEMENT_TYPE_BOOLEAN,
    CV_CHAR                = ELEMENT_TYPE_CHAR,
    CV_I1                  = ELEMENT_TYPE_I1,
    CV_U1                  = ELEMENT_TYPE_U1,
    CV_I2                  = ELEMENT_TYPE_I2,
    CV_U2                  = ELEMENT_TYPE_U2,
    CV_I4                  = ELEMENT_TYPE_I4,
    CV_U4                  = ELEMENT_TYPE_U4,
    CV_I8                  = ELEMENT_TYPE_I8,
    CV_U8                  = ELEMENT_TYPE_U8,
    CV_R4                  = ELEMENT_TYPE_R4,
    CV_R8                  = ELEMENT_TYPE_R8,
    CV_STRING              = ELEMENT_TYPE_STRING,

    // For the rest, we map directly if it is defined in CorHdr.h and fill
    //  in holes for the rest.
    CV_PTR                 = ELEMENT_TYPE_PTR,
    CV_DATETIME            = 0x10,      // ELEMENT_TYPE_BYREF
    CV_TIMESPAN            = 0x11,      // ELEMENT_TYPE_VALUETYPE
    CV_OBJECT              = ELEMENT_TYPE_CLASS,
    CV_DECIMAL             = 0x13,      // ELEMENT_TYPE_UNUSED1
    CV_CURRENCY            = 0x14,      // ELEMENT_TYPE_ARRAY
    CV_ENUM                = 0x15,      //
    CV_MISSING             = 0x16,      //
    CV_NULL                = 0x17,      //
    CV_LAST                = 0x18,      //
} CVTypes;

// The following values are used to represent underlying
//  type of the Enum..
#define EnumI1          0x100000
#define EnumU1          0x200000
#define EnumI2          0x300000
#define EnumU2          0x400000
#define EnumI4          0x500000
#define EnumU4          0x600000
#define EnumI8          0x700000
#define EnumU8          0x800000
#define EnumMask        0xF00000


//ClassItem is used to store the CVType of a class and a 
//reference to the TypeHandle.  Used for conversion between
//the two internally.
typedef struct {
    BinderClassID ClassID;
    MethodTable * m_pMT;
} ClassItem;

extern ClassItem CVClasses[];

//ConversionMethod is used to keep track of the name of the conversion
//method and the class on which it is found.
typedef struct
{
    LPCUTF8 pwzConvMethodName;
    CVTypes ConvClass;
} ConversionMethod;

inline TypeHandle GetTypeHandleForCVType(const unsigned int elemType) 
{
    CONTRACT (TypeHandle)
    {
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
        PRECONDITION(elemType < CV_LAST);
    }
    CONTRACT_END;
    
    if (CVClasses[elemType].m_pMT == 0)
    {
        CVClasses[elemType].m_pMT = ((Binder *)&g_Mscorlib)->FetchClass(CVClasses[elemType].ClassID);
    }

    RETURN TypeHandle(CVClasses[elemType].m_pMT);
}

#include <pshpack1.h>


/***  Variant Design Restrictions  (ie, decisions we've had to re-do differently):
      1)  A Variant containing all zeros should be a valid Variant of type empty.
      2)  Variant must contain an OBJECTREF field for Objects, etc.  Since we
          have no way of expressing a union between an OBJECTREF and an int, we
          always box Decimals in a Variant.
      3)  The m_type field is not a CVType and will contain extra bits.  People
          should use VariantData::GetType() to get the CVType.
      4)  You should use SetObjRef and GetObjRef to manipulate the OBJECTREF field.
          These will handle write barriers correctly, as well as CV_EMPTY.
      

   Empty, Missing & Null:
      Variants of type CV_EMPTY will be all zero's.  This forces us to add in
   special cases for all functions that convert a Variant into an object (such
   as copying a Variant into an Object[]).  

      Variants of type Missing and Null will have their objectref field set to 
   Missing.Value and Null.Value respectively.  This simplifies the code in 
   Variant.cs and strewn throughout the EE.  
*/

#define VARIANT_TYPE_MASK  0xFFFF
#define VARIANT_ARRAY_MASK 0x00010000
#define VT_MASK            0xFF000000
#define VT_BITSHIFT        24

struct VariantData
{
public:        
    FORCEINLINE CVTypes GetType() const
    {
        LEAF_CONTRACT;

        return (CVTypes)(m_type & VARIANT_TYPE_MASK);
    }

    FORCEINLINE void SetType(INT32 in)
    {
        LEAF_CONTRACT;
        m_type = in;
    }

    FORCEINLINE VARTYPE GetVT() const
    {
        LEAF_CONTRACT;

        VARTYPE vt = (m_type & VT_MASK) >> VT_BITSHIFT;
        if (vt & 0x80)
        {
            vt &= ~0x80;
            vt |= VT_ARRAY;
        }
        return vt;
    }

    FORCEINLINE void SetVT(VARTYPE vt)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION( !(vt & VT_BYREF) );
            PRECONDITION( (vt & ~VT_ARRAY) < 128 );
        }
        CONTRACTL_END;
            
        if (vt & VT_ARRAY)
        {
            vt &= ~VT_ARRAY;
            vt |= 0x80;
        }
        m_type = (m_type & ~((INT32)VT_MASK)) | (vt << VT_BITSHIFT);
    }

    OBJECTREF GetEmptyObjectRef() const;

    FORCEINLINE OBJECTREF GetObjRef() const
    {
        WRAPPER_CONTRACT;
        
        return (OBJECTREF)m_or;
    }

    OBJECTREF* GetObjRefPtr()
    {
        CONTRACT (OBJECTREF*)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        }
        CONTRACT_END;

        RETURN (OBJECTREF*)&m_or;
    }

    void SetObjRef(OBJECTREF objRef)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;
        
        if (objRef!=NULL)
        {
            SetObjectReferenceUnchecked((OBJECTREF*)&m_or, objRef);
        }
        else
        {
            // Casting trick to avoid going thru overloaded operator= (which
            // in this case would trigger a false write barrier violation assert.)
            *(LPVOID*)(OBJECTREF*)&m_or=NULL;
        }
    }

    FORCEINLINE void* GetData() const
    {
        // LEAF_CONTRACT; - Need to inline so this is commented out.
        return (void *)(&m_data);
    }

    FORCEINLINE INT8 GetDataAsInt8() const
    {
        // LEAF_CONTRACT; - Need to inline so this is commented out.
        return (INT8)m_data;
    }

    FORCEINLINE UINT8 GetDataAsUInt8() const
    {
        // LEAF_CONTRACT; - Need to inline so this is commented out.
        return (UINT8)m_data;
    }

    FORCEINLINE INT16 GetDataAsInt16() const
    {
        // LEAF_CONTRACT; - Need to inline so this is commented out.
        return (INT16)m_data;
    }

    FORCEINLINE UINT16 GetDataAsUInt16() const
    {
        // LEAF_CONTRACT; - Need to inline so this is commented out.
        return (UINT16)m_data;
    }

    FORCEINLINE INT32 GetDataAsInt32() const
    {
        // LEAF_CONTRACT; - Need to inline so this is commented out.
        return (INT32)m_data;
    }

    FORCEINLINE UINT32 GetDataAsUInt32() const
    {
        // LEAF_CONTRACT; - Need to inline so this is commented out.
        return (UINT32)m_data;
    }

    FORCEINLINE INT64 GetDataAsInt64() const
    {
        LEAF_CONTRACT;

        // LEAF_CONTRACT; - Need to inline so this is commented out.
        return (INT64)m_data;
    }

    FORCEINLINE UINT64 GetDataAsUInt64() const
    {
        // LEAF_CONTRACT; - Need to inline so this is commented out.
        return (UINT64)m_data;
    }

    FORCEINLINE void SetData(void *in)
    {
        LEAF_CONTRACT;

        // LEAF_CONTRACT; - Need to inline so this is commented out.
        
        if (!in)
            m_data=0;
        else
            m_data = *(INT64 *)in;
    }

    // When possible, please use the most specific SetDataAsXxx function.
    // This is necessary to guarantee we do sign extension correctly
    // for all types smaller than 32 bits.  R4's, R8's, U8's, DateTimes,
    // Currencies, and TimeSpans can all be treated as ints of the appropriate 
    // size - sign extension is irrelevant in those cases.
    FORCEINLINE void SetDataAsInt8(INT8 data)
    {
        LEAF_CONTRACT;

        // LEAF_CONTRACT; - Need to inline so this is commented out.
        m_data=data;
    }

    FORCEINLINE void SetDataAsUInt8(UINT8 data)
    {
        LEAF_CONTRACT;

        // LEAF_CONTRACT; - Need to inline so this is commented out.
        m_data=data;
    }

    FORCEINLINE void SetDataAsInt16(INT16 data)
    {
        LEAF_CONTRACT;

        // LEAF_CONTRACT; - Need to inline so this is commented out.
        m_data=data;
    }

    FORCEINLINE void SetDataAsUInt16(UINT16 data)
    {
        LEAF_CONTRACT;

        // LEAF_CONTRACT; - Need to inline so this is commented out.
        m_data=data;
    }

    FORCEINLINE void SetDataAsInt32(INT32 data)
    {
        LEAF_CONTRACT;

        // LEAF_CONTRACT; - Need to inline so this is commented out.
        m_data=data;
    }

    FORCEINLINE void SetDataAsUInt32(UINT32 data)
    {
        LEAF_CONTRACT;

        // LEAF_CONTRACT; - Need to inline so this is commented out.
        m_data=data;
    }

    FORCEINLINE void SetDataAsInt64(INT64 data)
    {
        LEAF_CONTRACT;

        // LEAF_CONTRACT; - Need to inline so this is commented out.
        m_data=data;
    }


// #ifdef-ing the private->public change so that the abstraction is maintained
// in the general case
private:
    Object*     m_or;
    INT64       m_data;
    INT32       m_type;
};


#define GCPROTECT_BEGIN_VARIANTDATA(/*VARIANTDATA*/vd) do {             \
                FrameWithCookie<GCFrame> __gcframe(vd.GetObjRefPtr(),   \
                1,                                                      \
                FALSE);                                                 \
                /* work around unreachable code warning */              \
                if (true) { DEBUG_ASSURE_NO_RETURN_BEGIN;


#define GCPROTECT_END_VARIANTDATA()                         \
                DEBUG_ASSURE_NO_RETURN_END; }               \
                __gcframe.Pop(); } while(0)




#include <poppack.h>

class COMVariant
{
    friend class OleVariant;

private:
    //
    // Private Helper Routines
    //
    static void LoadVariant();

public:
    static void NewVariant(VariantData * const& dest, const CVTypes type, UINT32 data
                                            DEBUG_ARG(BOOL bDestIsInterior = FALSE));
	
    //
    // Helper Routines
    //

    // Use this very carefully.  There is not a direct mapping between
    //  CorElementType and CVTypes for a bunch of things.  In this case
    //  we return CV_LAST.  You need to check this at the call site.
    static CVTypes CorElementTypeToCVTypes(CorElementType type);


    //
    // Initialization Methods
    // s_pVariantClass will be initialized to zero.  When the first
    //  variant is defined we will fill in this method table.
    static void EnsureVariantInitialized()
    {
        WRAPPER_CONTRACT;
        if (!s_pVariantClass)
            LoadVariant();
    }

    static FCDECL2_IV(void, SetFieldsR4, VariantData* vThisRef, float val);
    static FCDECL2_IV(void, SetFieldsR8, VariantData* vThisRef, double val);
    static FCDECL2(void, SetFieldsObject, VariantData* vThisRef, Object* vVal);
    static FCDECL1(float, GetR4FromVar, VariantData* var);
    static FCDECL1(double, GetR8FromVar, VariantData* var);

    static FCDECL0(void, InitVariant);

    static FCDECL1(Object*, BoxEnum, VariantData* var);

private:
    // GetCVTypeFromClass
    // This method will return the CVTypes from the Variant instance
    static CVTypes GetCVTypeFromClass(TypeHandle th);
    static int GetEnumFlags(TypeHandle th);

public:
    // Static Variables
    static MethodTable* s_pVariantClass;
};

#endif // _COMVARIANT_H_

