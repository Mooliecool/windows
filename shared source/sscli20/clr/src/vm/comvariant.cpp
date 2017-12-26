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
** Class:  COMVariant
**
**                                        
**
** Purpose: Native Implementation of the Variant Class
**
** Date:  July 22, 1998
** 
===========================================================*/

#include "common.h"
#include "object.h"
#include "excep.h"
#include "frames.h"
#include "vars.hpp"
#include "comvariant.h"
#include "comstring.h"
#include "comstringcommon.h"
#include "field.h"

//
// Class Variable Initialization
//
MethodTable*    COMVariant::s_pVariantClass=NULL;


//The Name of the classes and the eeClass that we've looked up
//for them.  The typeHandle is initialized to null in all instances.
ClassItem CVClasses[] = 
{
    {CLASS__EMPTY,   NULL},  //CV_EMPTY
    {CLASS__VOID,    NULL},  //CV_VOID, Changing this to object messes up signature resolution very badly.
    {CLASS__BOOLEAN, NULL},  //CV_BOOLEAN
    {CLASS__CHAR,    NULL},  //CV_CHAR
    {CLASS__SBYTE,   NULL},  //CV_I1
    {CLASS__BYTE,    NULL},  //CV_U1
    {CLASS__INT16,   NULL},  //CV_I2
    {CLASS__UINT16,  NULL},  //CV_U2
    {CLASS__INT32,   NULL},  //CV_I4
    {CLASS__UINT32,  NULL},  //CV_UI4
    {CLASS__INT64,   NULL},  //CV_I8
    {CLASS__UINT64,  NULL},  //CV_UI8
    {CLASS__SINGLE,  NULL},  //CV_R4   
    {CLASS__DOUBLE,  NULL},  //CV_R8   
    {CLASS__STRING,  NULL},  //CV_STRING
    {CLASS__VOID,  NULL},    //CV_PTR...We treat this as void
    {CLASS__DATE_TIME,NULL}, //CV_DATETIME
    {CLASS__TIMESPAN,NULL},  //CV_TIMESPAN
    {CLASS__OBJECT,  NULL},  //CV_OBJECT
    {CLASS__DECIMAL, NULL},  //CV_DECIMAL
    {CLASS__CURRENCY,NULL},  //CV_CURRENCY
    {CLASS__OBJECT,  NULL},  //ENUM...We treat this as OBJECT
    {CLASS__MISSING, NULL},  //CV_MISSING
    {CLASS__NULL,    NULL},  //CV_NULL
    {CLASS__NIL, NULL},      //CV_LAST
};


// Returns System.Empty.Value.
OBJECTREF VariantData::GetEmptyObjectRef() const
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    FieldDesc * pFD = g_Mscorlib.GetField(FIELD__EMPTY__VALUE);
    _ASSERTE(pFD);
    
    OBJECTREF obj = pFD->GetStaticOBJECTREF();
    
    return obj;
}

//
// Current Conversions
// 

FCIMPL1(float, COMVariant::GetR4FromVar, VariantData* var)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(var));
    }
    CONTRACTL_END;
    
    INT32 val = var->GetDataAsInt32();
    return (float&)val;
}
FCIMPLEND
    
FCIMPL1(double, COMVariant::GetR8FromVar, VariantData* var)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(var));
    }
    CONTRACTL_END;
    
    INT64 val = var->GetDataAsInt64();
    return (double&)val;
}
FCIMPLEND


/*=================================SetFieldsR4==================================
**
==============================================================================*/
FCIMPL2_IV(void, COMVariant::SetFieldsR4, VariantData* var, float val)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;        
        PRECONDITION(CheckPointer(var));
    }
    CONTRACTL_END;
    
    INT64 tempData;

    tempData = *((INT32 *)(&val));
    var->SetData(&tempData);
    var->SetType(CV_R4);
}
FCIMPLEND


/*=================================SetFieldsR8==================================
**
==============================================================================*/
FCIMPL2_IV(void, COMVariant::SetFieldsR8, VariantData* var, double val)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(var));
    }
    CONTRACTL_END;
    
    var->SetData((void *)(&val));
    var->SetType(CV_R8);
}
FCIMPLEND


/*===============================SetFieldsObject================================
**
==============================================================================*/
FCIMPL2(void, COMVariant::SetFieldsObject, VariantData* var, Object* vVal)
{
    OBJECTREF val = ObjectToOBJECTREF(vVal);

    HELPER_METHOD_FRAME_BEGIN_1(val);
    GCPROTECT_BEGININTERIOR(var)

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(var));
        PRECONDITION(CheckPointer(vVal));
    }
    CONTRACTL_END;

    CVTypes cvt = CV_EMPTY;
    TypeHandle typeHandle;

    MethodTable *valMT = val->GetMethodTable();

    //If this isn't a value class, we should just skip out because we're not going
    //to do anything special with it.
    if (!valMT->IsValueClass())
    {
        EnsureVariantInitialized();             // ensure type handles below are initialized
        var->SetObjRef(val);
        typeHandle = TypeHandle(valMT);
        
        if (typeHandle==GetTypeHandleForCVType(CV_MISSING))
        {
            var->SetType(CV_MISSING);
        }
        else if (typeHandle==GetTypeHandleForCVType(CV_NULL))
        {
            var->SetType(CV_NULL);
        }
        else if (typeHandle==GetTypeHandleForCVType(CV_EMPTY))
        {
            var->SetType(CV_EMPTY);
            var->SetObjRef(NULL);
        }
        else
        {
            var->SetType(CV_OBJECT);
        }
    }
    else
    {
        //If this is a primitive type, we need to unbox it, get the value and create a variant
        //with just those values.
        void *UnboxData = val->UnBox();

        ClearObjectReference(var->GetObjRefPtr());
        typeHandle = TypeHandle(valMT);
        CorElementType cet = typeHandle.GetSignatureCorElementType();
        
        if (cet>=ELEMENT_TYPE_BOOLEAN && cet<=ELEMENT_TYPE_STRING)
        {
            cvt = (CVTypes)cet;
        }
        else
        {
            cvt = GetCVTypeFromClass(valMT);
        }
        var->SetType(cvt);


        //copy all of the data.
        // Copies must be done based on the exact number of bytes to copy.
        // We don't want to read garbage from other blocks of memory.
        //CV_I8 --> CV_R8, CV_DATETIME, CV_TIMESPAN, & CV_CURRENCY are all of the 8 byte quantities
        //If we don't find one of those ranges, we've found a value class 
        //of which we don't have inherent knowledge, so just slam that into an
        //ObjectRef.
        if (cvt>=CV_BOOLEAN && cvt<=CV_U1 && cvt != CV_CHAR)
        {
            var->SetDataAsInt64(*((UINT8 *)UnboxData));
        }
        else if (cvt==CV_CHAR || cvt>=CV_I2 && cvt<=CV_U2)
        {
            var->SetDataAsInt64(*((UINT16 *)UnboxData));
        }
        else if (cvt>=CV_I4 && cvt<=CV_U4 || cvt==CV_R4)
        {
            var->SetDataAsInt64(*((UINT32 *)UnboxData));
        }
        else if ((cvt>=CV_I8 && cvt<=CV_R8) || (cvt==CV_DATETIME) || (cvt==CV_TIMESPAN) || (cvt==CV_CURRENCY))
        {
            var->SetDataAsInt64(*((INT64 *)UnboxData));
        }
        else if (cvt==CV_EMPTY || cvt==CV_NULL || cvt==CV_MISSING)
        {
            var->SetType(cvt);
        }
        else if (cvt==CV_ENUM)
        {
            var->SetDataAsInt64(*((INT32 *)UnboxData));
            var->SetObjRef(typeHandle.GetManagedClassObject());
            var->SetType(GetEnumFlags(typeHandle));
        }
        else
        {
            // Decimals and other boxed value classes get handled here.
            var->SetObjRef(val);
        }
    }

    GCPROTECT_END();
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


FCIMPL0(void, COMVariant::InitVariant)
{  
    HELPER_METHOD_FRAME_BEGIN_0();

    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    EnsureVariantInitialized();
    
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(Object*, COMVariant::BoxEnum, VariantData* var)
{   
    OBJECTREF retO = NULL;
    
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, retO);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(var));
        PRECONDITION(var->GetObjRef() != NULL);
    }
    CONTRACTL_END;
        
#ifdef _DEBUG
    CVTypes vType = (CVTypes) var->GetType();
#endif

    _ASSERTE(vType == CV_ENUM);

    MethodTable* mt = ((REFLECTCLASSBASEREF) var->GetObjRef())->GetType().GetMethodTable();
    _ASSERTE(mt);

    retO = mt->Box(var->GetData(), FALSE);

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(retO);
}
FCIMPLEND


/*=================================LoadVariant==================================
**Action:  Initializes the variant class within the runtime.  Stores pointers to the
**         MethodTable in static members of COMVariant
**
**Arguments: None
**
**Returns: void
**
**Exceptions: TypeLoading exceptions can occur
==============================================================================*/
void COMVariant::LoadVariant()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    MethodTable *variantClass = g_Mscorlib.FetchClass(CLASS__VARIANT);

    // Fixup the ELEMENT_TYPE Void
    // We never create one of these, but we do depend on the value on the class being set properly in 
    // reflection.
    MethodTable *pVoid = GetTypeHandleForCVType(CV_VOID).GetMethodTable();
    PREFIX_ASSUME(pVoid != NULL);

    _ASSERTE(pVoid->GetInternalCorElementType() == ELEMENT_TYPE_VOID);

    // Run class initializers for Empty, Missing, and Null to set Value field
    GetTypeHandleForCVType(CV_EMPTY).GetMethodTable()->CheckRunClassInitThrowing();
    GetTypeHandleForCVType(CV_MISSING).GetMethodTable()->CheckRunClassInitThrowing();
    GetTypeHandleForCVType(CV_NULL).GetMethodTable()->CheckRunClassInitThrowing();
    pVoid->CheckRunClassInitThrowing();

    // This has to be the last to avoid race.
    s_pVariantClass = variantClass;
}


/*===============================GetTypeFromClass===============================
**Action: Takes an MethodTable * and returns the associated CVType.
**Arguments: MethodTable * -- a pointer to the class for which we want the CVType.
**Returns:  The CVType associated with the MethodTable or CV_OBJECT if this can't be 
**          determined.
**Exceptions: None
==============================================================================*/

CVTypes COMVariant::GetCVTypeFromClass(TypeHandle th)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    if (th.IsNull())
        return CV_EMPTY;

    //We'll start looking from Variant.  Empty and Void are handled below.
    for (int i=CV_EMPTY; i<CV_LAST; i++)
    {
        if (th == GetTypeHandleForCVType((CVTypes)i))
            return (CVTypes)i;
    }

    if (th.IsEnum())
        return CV_ENUM;

    return CV_OBJECT;    
}


/*==================================NewVariant==================================
**N.B.:  This method does a GC Allocation.  Any method calling it is required to
**       GC_PROTECT the OBJECTREF.
**
**Actions:  Allocates a new Variant and fills it with the appropriate data.  
**Returns:  A new Variant with all of the appropriate fields filled out.
**Exceptions: OutOfMemoryError if v can't be allocated.       
==============================================================================*/
void COMVariant::NewVariant(VariantData * const& dest, const CVTypes type, UINT32 data
                                            DEBUG_ARG(BOOL bDestIsInterior))
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        // Don't pass an object in for Empty.
        PRECONDITION(CheckPointer(dest));
        PRECONDITION((bDestIsInterior && IsProtectedByGCFrame ((OBJECTREF *) &dest))
            || (!bDestIsInterior && IsProtectedByGCFrame (dest->GetObjRefPtr ())));
        PRECONDITION((type == CV_EMPTY) || (type == CV_NULL) || (type == CV_U4));
    }
    CONTRACTL_END;

    //If both arguments are null or both are specified, we're in an illegal situation.  Bail.
    //If all three are null, we're creating an empty variant
    if ( (type != CV_EMPTY) && (type != CV_NULL) && (type != CV_U4) )
    {
        COMPlusThrow(kArgumentException);
    }

    //Fill in the data.
    dest->SetType(type);

    switch (type)
    {
        case CV_U4:
            dest->SetObjRef(NULL);
            dest->SetDataAsUInt32(data);
            break;
            
        case CV_NULL:
        {
            FieldDesc * pFD = g_Mscorlib.GetField(FIELD__NULL__VALUE);
            _ASSERTE(pFD);
            
            OBJECTREF obj = pFD->GetStaticOBJECTREF();
            _ASSERTE(obj!=NULL);
            
            dest->SetObjRef(obj);
            dest->SetDataAsInt64(0);
            break;
        }

        case CV_EMPTY:
        {
            dest->SetObjRef(NULL);
            break;
        }
        
        default:
            // Did you add any new CVTypes?
            COMPlusThrow(kNotSupportedException, L"Arg_InvalidOleVariantTypeException");
    }
}


int COMVariant::GetEnumFlags(TypeHandle th)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(!th.IsNull());
        PRECONDITION(th.IsEnum());
    }
    CONTRACTL_END;
    
    ApproxFieldDescIterator fdIterator(th.GetMethodTable(), ApproxFieldDescIterator::INSTANCE_FIELDS);
    FieldDesc* p = fdIterator.Next();
    if (NULL == p)
    {
        _ASSERTE(!"NULL FieldDesc returned");
        return 0;
    }
    
#ifdef _DEBUG
    WORD fldCnt = th.GetClass()->GetNumInstanceFields();
#endif

    _ASSERTE(fldCnt == 1);

    CorElementType cet = p[0].GetFieldType();
    switch (cet)
    {
        case ELEMENT_TYPE_I1:
            return (CV_ENUM | EnumI1);
            
        case ELEMENT_TYPE_U1:
            return (CV_ENUM | EnumU1);
            
        case ELEMENT_TYPE_I2:
            return (CV_ENUM | EnumI2);
            
        case ELEMENT_TYPE_U2:
            return (CV_ENUM | EnumU2);
            
        IN_WIN32(case ELEMENT_TYPE_I:)
        case ELEMENT_TYPE_I4:
            return (CV_ENUM | EnumI4);
            
        IN_WIN32(case ELEMENT_TYPE_U:)
        case ELEMENT_TYPE_U4:
            return (CV_ENUM | EnumU4);
            
        IN_WIN64(case ELEMENT_TYPE_I:)
        case ELEMENT_TYPE_I8:
            return (CV_ENUM | EnumI8);
            
        IN_WIN64(case ELEMENT_TYPE_U:)
        case ELEMENT_TYPE_U8:
            return (CV_ENUM | EnumU8);
            
        default:
            _ASSERTE(!"UNknown Type");
            return 0;
    }
}

    
// Use this very carefully.  There is not a direct mapping between
//  CorElementType and CVTypes for a bunch of things.  In this case
//  we return CV_LAST.  You need to check this at the call site.
CVTypes COMVariant::CorElementTypeToCVTypes(CorElementType type)
{
    LEAF_CONTRACT;
    
    if (type <= ELEMENT_TYPE_STRING)
        return (CVTypes) type;
    
    if (type == ELEMENT_TYPE_CLASS || type == ELEMENT_TYPE_OBJECT)
        return (CVTypes) ELEMENT_TYPE_CLASS;
    
    return CV_LAST;
}

