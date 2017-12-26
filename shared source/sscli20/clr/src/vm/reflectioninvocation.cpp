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

#include "common.h"
#include "reflectioninvocation.h"
#include "invokeutil.h"
#include "object.h"
#include "class.h"
#include "method.hpp"
#include "typehandle.h"
#include "field.h"
#include "security.h"
#include "remoting.h"
#include "eeconfig.h"
#include "vars.hpp"
#include "jitinterface.h"
#include "contractimpl.h"
#include "virtualcallstub.h"
#include "comdelegate.h"
#include "constrainedexecutionregion.h"
#include "securitydescriptor.h"
#include "generics.h"


#include "threads.inl"

//TODO: To Be Removed
#define RM_ATTR_INITTED          0x80000000
#define RM_ATTR_NEED_SECURITY    0x00000001
#define RM_ATTR_IS_CTOR          0x00000020
#define RM_ATTR_RISKY_METHOD     0x00000040
#define RM_ATTR_SECURITY_IMPOSED 0x00000080

// these flags are defined in XXXInfo.cs and only those that are used are replicated here
#define INVOCATION_FLAGS_UNKNOWN                    0x00000000
#define INVOCATION_FLAGS_INITIALIZED                0x00000001

// it's used for both method and field to signify that no access is allowed
#define INVOCATION_FLAGS_NO_INVOKE                  0x00000002

#define INVOCATION_FLAGS_NEED_SECURITY              0x00000004

// because field and method are different we can reuse the same bits
//method
#define INVOCATION_FLAGS_IS_CTOR                    0x00000010
#define INVOCATION_FLAGS_RISKY_METHOD               0x00000020
#define INVOCATION_FLAGS_SECURITY_IMPOSED           0x00000040
#define INVOCATION_FLAGS_IS_DELEGATE_CTOR           0x00000080
#define INVOCATION_FLAGS_CONTAINS_STACK_POINTERS    0x00000100
// field
#define INVOCATION_FLAGS_SPECIAL_FIELD              0x00000010
#define INVOCATION_FLAGS_FIELD_SPECIAL_CAST         0x00000020

// temporary flag used for flagging invocation of method vs ctor
#define INVOCATION_FLAGS_CONSTRUCTOR_INVOKE         0x10000000

/**************************************************************************/
/* if the type handle 'th' is a byref to a nullable type, return the
   type handle to the nullable type in the byref.  Otherwise return 
   the null type handle  */
static TypeHandle NullableTypeOfByref(TypeHandle th) {

    if (th.GetVerifierCorElementType() != ELEMENT_TYPE_BYREF)
        return TypeHandle();
    
    TypeHandle subType = th.AsTypeDesc()->GetTypeParam();
    if (!Nullable::IsNullableType(subType))
        return TypeHandle();
            
    return subType;
}

/**************************************************************************/
BOOL CanCacheTargetAndCrackedSig(MethodDesc* pMD)
{
    return (
        !pMD->IsInterface() &&                   // Can't tell target
        !pMD->IsVtableMethod() &&                // Can't tell target
        !pMD->HasClassOrMethodInstantiation() && // Can't crack signature
        !pMD->GetMethodTable()->IsArray() &&     // Can't tell target?
        !pMD->IsDynamicMethod() &&               // raw signature pointer (which is used for cache
                                                 // is volatile (another md can't get the same pointer
                                                 // when the previous owner is collected).
        !pMD->GetModule()->IsReflection() &&     // Reflection can also alter the raw signature pointer
        !pMD->IsEnCMethod()         
        );
}


static void TryDemand(DWORD whatPermission, RuntimeExceptionKind reKind, LPCWSTR wszTag) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;


    EX_TRY {
        Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, whatPermission);
    } 
    EX_CATCH {
        COMPlusThrow(reKind, wszTag);
    }
    EX_END_CATCH_UNREACHABLE
}

static void TryCallMethod(MethodDescCallSite* pMethodCallSite, ARG_SLOT* args) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    OBJECTREF ppException = NULL;
    GCPROTECT_BEGIN(ppException);

    EX_TRY {
        pMethodCallSite->CallWithValueTypes(args);
    } 
    EX_CATCH {
        ppException = GETTHROWABLE();
        _ASSERTE(ppException);
    }
    EX_END_CATCH(RethrowTransientExceptions)

    // It is important to re-throw outside the catch block because re-throwing will invoke
    // the jitter and managed code and will cause us to use more than the backout stack limit.
    if (ppException != NULL) 
    {
        // If we get here we need to throw an TargetInvocationException
        OBJECTREF except = InvokeUtil::CreateTargetExcept(&ppException);
        COMPlusThrow(except);
    }
    GCPROTECT_END();
}

static bool IsDangerousMethod(MethodDesc *pMD) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    static MethodTable *s_pTypeAppDomain = NULL;
    static MethodTable *s_pTypeAssembly = NULL;
    static MethodTable *s_pTypeAssemblyBuilder = NULL;
    static MethodTable *s_pTypeMethodRental = NULL;
    static MethodTable *s_pTypeIsolatedStorageFile = NULL;
    static MethodTable *s_pTypeMethodBase = NULL;
    static MethodTable *s_pTypeRuntimeMethodInfo = NULL;
    static MethodTable *s_pTypeConstructorInfo = NULL;
    static MethodTable *s_pTypeRuntimeConstructorInfo = NULL;
    static MethodTable *s_pTypeType = NULL;
    static MethodTable *s_pTypeRuntimeType = NULL;
    static MethodTable *s_pTypeRuntimeTypeHandle = NULL;
    static MethodTable *s_pTypeFieldInfo = NULL;
    static MethodTable *s_pTypeRtFieldInfo = NULL;
    static MethodTable *s_pTypeRuntimeFieldInfo = NULL;
    static MethodTable *s_pTypeEventInfo = NULL;
    static MethodTable *s_pTypeRuntimeEventInfo = NULL;
    static MethodTable *s_pTypePropertyInfo = NULL;
    static MethodTable *s_pTypeRuntimePropertyInfo = NULL;
    static MethodTable *s_pTypeResourceManager = NULL;
    static MethodTable *s_pTypeActivator = NULL;
    static MethodTable *s_pTypeRuntimeMethodHandle = NULL;
    static MethodTable *s_pTypeRuntimeFieldHandle = NULL;

    // One time only initialization. Check relies on write ordering.
    if (s_pTypeActivator == NULL) {
        s_pTypeAppDomain = g_Mscorlib.FetchClass(CLASS__APP_DOMAIN);
        s_pTypeAssembly = g_Mscorlib.FetchClass(CLASS__ASSEMBLY);
        s_pTypeAssemblyBuilder = g_Mscorlib.FetchClass(CLASS__ASSEMBLY_BUILDER);
        s_pTypeMethodRental = g_Mscorlib.FetchClass(CLASS__METHOD_RENTAL);
        s_pTypeIsolatedStorageFile = g_Mscorlib.FetchClass(CLASS__ISS_STORE_FILE);
        s_pTypeMethodBase = g_Mscorlib.FetchClass(CLASS__METHOD_BASE);
        s_pTypeRuntimeMethodInfo = g_Mscorlib.FetchClass(CLASS__METHOD);
        s_pTypeConstructorInfo = g_Mscorlib.FetchClass(CLASS__CONSTRUCTOR_INFO);
        s_pTypeRuntimeConstructorInfo = g_Mscorlib.FetchClass(CLASS__CONSTRUCTOR);
        s_pTypeType = g_Mscorlib.FetchClass(CLASS__TYPE);
        s_pTypeRuntimeType = g_Mscorlib.FetchClass(CLASS__CLASS);
        s_pTypeRuntimeTypeHandle = g_Mscorlib.FetchClass(CLASS__TYPE_HANDLE);
        s_pTypeRtFieldInfo = g_Mscorlib.FetchClass(CLASS__RT_FIELD_INFO);
        s_pTypeFieldInfo = g_Mscorlib.FetchClass(CLASS__FIELD_INFO);
        s_pTypeRuntimeFieldInfo = g_Mscorlib.FetchClass(CLASS__FIELD);
        s_pTypeEventInfo = g_Mscorlib.FetchClass(CLASS__EVENT_INFO);
        s_pTypeRuntimeEventInfo = g_Mscorlib.FetchClass(CLASS__EVENT);
        s_pTypePropertyInfo = g_Mscorlib.FetchClass(CLASS__PROPERTY_INFO);
        s_pTypeRuntimePropertyInfo = g_Mscorlib.FetchClass(CLASS__PROPERTY);
        s_pTypeRuntimeMethodHandle = g_Mscorlib.FetchClass(CLASS__METHOD_HANDLE);
        s_pTypeRuntimeFieldHandle = g_Mscorlib.FetchClass(CLASS__FIELD_HANDLE);
        s_pTypeResourceManager = g_Mscorlib.FetchClass(CLASS__RESOURCE_MANAGER);
        s_pTypeActivator = g_Mscorlib.FetchClass(CLASS__ACTIVATOR);
    }
    _ASSERTE(s_pTypeAppDomain &&
             s_pTypeAssembly &&
             s_pTypeAssemblyBuilder &&
             s_pTypeMethodRental &&
             s_pTypeIsolatedStorageFile &&
             s_pTypeMethodBase &&
             s_pTypeRuntimeMethodInfo &&
             s_pTypeConstructorInfo &&
             s_pTypeRuntimeConstructorInfo &&
             s_pTypeType &&
             s_pTypeRuntimeType &&
             s_pTypeRuntimeTypeHandle &&
             s_pTypeRtFieldInfo &&
             s_pTypeFieldInfo &&
             s_pTypeRuntimeFieldInfo &&
             s_pTypeEventInfo &&
             s_pTypeRuntimeEventInfo &&
             s_pTypePropertyInfo &&
             s_pTypeRuntimePropertyInfo &&
             s_pTypeResourceManager &&
             s_pTypeActivator &&
             s_pTypeRuntimeMethodHandle &&
             s_pTypeRuntimeFieldHandle);

    MethodTable *pMT = pMD->GetMethodTable();

    return pMT == s_pTypeAppDomain ||
           pMT == s_pTypeAssembly ||
           pMT == s_pTypeAssemblyBuilder ||
           pMT == s_pTypeRuntimeMethodHandle ||
           pMT == s_pTypeRuntimeFieldHandle ||
           pMT == s_pTypeMethodRental ||
           pMT == s_pTypeIsolatedStorageFile ||
           pMT == s_pTypeMethodBase ||
           pMT == s_pTypeRuntimeMethodInfo ||
           pMT == s_pTypeConstructorInfo ||
           pMT == s_pTypeRuntimeConstructorInfo ||
           pMT == s_pTypeType ||
           pMT == s_pTypeRuntimeType ||
           pMT == s_pTypeRuntimeTypeHandle ||
           pMT == s_pTypeRtFieldInfo ||
           pMT == s_pTypeFieldInfo ||
           pMT == s_pTypeRuntimeFieldInfo ||
           pMT == s_pTypeEventInfo ||
           pMT == s_pTypeRuntimeEventInfo ||
           pMT == s_pTypePropertyInfo ||
           pMT == s_pTypeRuntimePropertyInfo ||
           pMT == s_pTypeResourceManager ||
           pMT == s_pTypeActivator ||
           pMT == g_pDelegateClass ||
           pMT == g_pMultiDelegateClass ||
           (pMT->GetClass()->IsAnyDelegateClass() && pMD->IsCtor());
}


void CanAccess(MethodDesc* pMeth, MethodTable* pParentMT, MethodTable* pInstanceMT, RefSecContext *pSCtx, bool verifyAccess, bool checkSkipVer, bool thisIsImposedSecurity, bool knowForSureImposedSecurityState) 
{
    CONTRACTL 
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    _ASSERTE(!thisIsImposedSecurity  || knowForSureImposedSecurityState);

    BOOL fRet = FALSE;
    BOOL isEveryoneFullyTrusted = FALSE;
    BOOL isSecurityOn = Security::IsSecurityOn();

    struct _gc 
    {
        OBJECTREF refClassNonCasDemands;
        OBJECTREF refClassCasDemands;
        OBJECTREF refMethodNonCasDemands;
        OBJECTREF refMethodCasDemands;
        OBJECTREF refThrowable;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    GCPROTECT_BEGIN(gc);

    if (isSecurityOn) 
    {
        if (thisIsImposedSecurity || !knowForSureImposedSecurityState) 
        {
            isEveryoneFullyTrusted = Security::AllDomainsOnStackFullyTrusted();

            // If all assemblies in the domain are fully trusted then we are not
            // going to do any security checks anyway..
            if (thisIsImposedSecurity && isEveryoneFullyTrusted) 
            {
                fRet = TRUE;
                goto Exit1;
            }
        }

        if (pMeth->RequiresLinktimeCheck()) 
        {
            // Fetch link demand sets from all the places in metadata where we might
            // find them (class and method). These might be split into CAS and non-CAS
            // sets as well.
            Security::RetrieveLinktimeDemands(pMeth,
                                              &gc.refClassCasDemands,
                                              &gc.refClassNonCasDemands,
                                              &gc.refMethodCasDemands,
                                              &gc.refMethodNonCasDemands);
    
            if (gc.refClassCasDemands == NULL && gc.refClassNonCasDemands == NULL &&
                gc.refMethodCasDemands == NULL && gc.refMethodNonCasDemands == NULL &&
                isEveryoneFullyTrusted) 
            {
                // All code access security demands will pass anyway.
                fRet = TRUE;
                goto Exit1;
            }
        }
    }

    if (verifyAccess)
        InvokeUtil::CheckAccess(pSCtx,
                                pMeth->GetAttrs(),
                                pParentMT,
                                pInstanceMT,
                                pMeth,
                                REFSEC_CHECK_MEMBERACCESS|REFSEC_THROW_MEMBERACCESS);
    
    if (isSecurityOn) 
    {
        if (pMeth->RequiresLinktimeCheck()) 
        {    
       
            // The following logic turns link demands on the target method into full
            // stack walks in order to close security holes in poorly written
            // reflection users.
    
            _ASSERTE(pMeth);
            _ASSERTE(pSCtx);
    
            if (Security::IsUntrustedCallerCheckNeeded(pMeth) )
            {
                if (pSCtx->GetCallerMT()) 
                { 
                    // Check for untrusted caller
                    // It is possible that wrappers like VBHelper libraries that are
                    // fully trusted, make calls to public methods that do not have
                    // safe for Untrusted caller custom attribute set.
                    // Like all other link demand that gets transformed to a full stack 
                    // walk for reflection, calls to public methods also gets 
                    // converted to full stack walk
        
                    if (!Security::DoUntrustedCallerChecks(
                        pSCtx->GetCallerMT()->GetAssembly(), pMeth,
                        &gc.refThrowable, TRUE))
                        COMPlusThrow(gc.refThrowable);
                }
            }

            // CAS Link Demands
            if (gc.refClassCasDemands != NULL)
                Security::DemandSet(SSWT_LATEBOUND_LINKDEMAND, gc.refClassCasDemands);

            if (gc.refMethodCasDemands != NULL)
                Security::DemandSet(SSWT_LATEBOUND_LINKDEMAND, gc.refMethodCasDemands);

            // Non-CAS demands are not applied against a grant
            // set, they're standalone.
            if (gc.refClassNonCasDemands != NULL)
                Security::CheckNonCasDemand(&gc.refClassNonCasDemands);

            if (gc.refMethodNonCasDemands != NULL)
                Security::CheckNonCasDemand(&gc.refMethodNonCasDemands);



            // We perform automatic linktime checks for UnmanagedCode in three cases:
            //   o  P/Invoke calls.
            //   o  Calls through an interface that have a suppress runtime check
            //      attribute on them (these are almost certainly interop calls).
            //   o  Interop calls made through method impls.
            if (pMeth->IsNDirect() ||
                (pMeth->IsInterface() &&
                 (pMeth->GetMDImport()->GetCustomAttributeByName(pParentMT->GetCl(),
                                                                COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                                NULL,
                                                                NULL) == S_OK ||
                  pMeth->GetMDImport()->GetCustomAttributeByName(pMeth->GetMemberDef(),
                                                                COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                                NULL,
                                                                NULL) == S_OK) ) ||
                (pMeth->IsComPlusCall() && !pMeth->IsInterface()))
                Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, SECURITY_UNMANAGED_CODE);
        }
    }

Exit1:;

    GCPROTECT_END();

    if (isSecurityOn && !fRet) 
    {
        if (checkSkipVer)
            Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, SECURITY_SKIP_VER);
    }
}

FCIMPL5(Object*, RuntimeFieldHandle::GetValue, FieldDesc **ppFieldDesc, Object *instanceUNSAFE, EnregisteredTypeHandle enregFieldType, EnregisteredTypeHandle declaringTH, CLR_BOOL *pDomainInitialized) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(ppFieldDesc));
        PRECONDITION(CheckPointer(*ppFieldDesc));
        PRECONDITION(CheckPointer(enregFieldType));
    }
    CONTRACTL_END;

    TypeHandle fieldType = TypeHandle::FromPtr(enregFieldType);
    TypeHandle declaringType = TypeHandle::FromPtr(declaringTH);
    
    OBJECTREF target = ObjectToOBJECTREF(instanceUNSAFE);
    OBJECTREF rv = NULL; // not protected

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, target);
    // There can be no GC after this until the Object is returned.
    rv = InvokeUtil::GetFieldValue(*ppFieldDesc, fieldType, &target, declaringType, pDomainInitialized);
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(rv);
}
FCIMPLEND

FCIMPL5(void, ReflectionInvocation::PerformVisibilityCheckOnField, FieldDesc *pFieldDesc, Object *target, void *declaringTH, DWORD attr, DWORD invocationFlags) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(pFieldDesc));
    }
    CONTRACTL_END;

    TypeHandle declaringType = TypeHandle::FromPtr(declaringTH);
    OBJECTREF targetObj = (OBJECTREF)target;

    HELPER_METHOD_FRAME_BEGIN_1(targetObj);

    if ((invocationFlags & INVOCATION_FLAGS_SPECIAL_FIELD) != 0) {
        // Verify that this is not a Final Field
        if (IsFdInitOnly(attr))
            TryDemand(SECURITY_SERIALIZATION, kFieldAccessException, L"Acc_ReadOnly");
        if (IsFdHasFieldRVA(attr))
            TryDemand(SECURITY_SKIP_VER, kFieldAccessException, L"Acc_RvaStatic");
    }

    if ((invocationFlags & INVOCATION_FLAGS_NEED_SECURITY) != 0) {
        // Verify the callee/caller access
        RefSecContext sCtx;
        MethodTable* pInstanceMT = NULL;
        if (targetObj != NULL && !pFieldDesc->IsStatic()) {
            TypeHandle targetType = targetObj->GetTypeHandle();
            if (!targetType.IsTypeDesc())
                pInstanceMT = targetType.GetMethodTable();
        }

        // Perform the normal access check (caller vs field).
        InvokeUtil::CheckAccess(&sCtx,
                                attr,
                                declaringType.GetMethodTable(), pInstanceMT,
                                NULL,
                                REFSEC_CHECK_MEMBERACCESS|REFSEC_THROW_FIELDACCESS,  pFieldDesc);
    }
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL2(FC_BOOL_RET, ReflectionInvocation::CanValueSpecialCast, void *valueTH, void *targetTH) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(valueTH));
        PRECONDITION(CheckPointer(targetTH));
    }
    CONTRACTL_END;
    
    TypeHandle valueType = TypeHandle::FromPtr(valueTH);
    TypeHandle targetType = TypeHandle::FromPtr(targetTH);

    // we are here only if the target type is a primitive, an enum or a pointer

    CorElementType targetCorElement = targetType.GetVerifierCorElementType();

    BOOL ret = TRUE;
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    // the field type is a pointer
    if (targetCorElement == ELEMENT_TYPE_PTR || targetCorElement == ELEMENT_TYPE_FNPTR) {
        // the object must be an IntPtr or a System.Reflection.Pointer
        if (valueType == TypeHandle(g_Mscorlib.FetchClass(CLASS__INTPTR))) {
            //
            // it's an IntPtr, it's good. Demand SkipVerification and proceed

            Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, SECURITY_SKIP_VER);
        }
        //
        // it's a System.Reflection.Pointer object

        // void* assigns to any pointer. Otherwise the type of the pointer must match
        else if (targetType != g_Mscorlib.GetType(TYPE__VOID_PTR)) {
            if (!valueType.CanCastTo(targetType))
                ret = FALSE;
            else
                // demand SkipVerification and proceed
                Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, SECURITY_SKIP_VER);
        }
        else
            // demand SkipVerification and proceed
            Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, SECURITY_SKIP_VER);
    } else {
        // the field type is an enum or a primitive. To have any chance of assignement the object type must
        // be an enum or primitive as well.
        // So get the internal cor element and that must be the same or widen
        CorElementType valueCorElement = valueType.GetVerifierCorElementType();
        if (InvokeUtil::IsPrimitiveType(valueCorElement))
            ret = (InvokeUtil::CanPrimitiveWiden(targetCorElement, valueCorElement)) ? TRUE : FALSE;
        else
            ret = FALSE;
    }
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(ret);
}
FCIMPLEND

FCIMPL2(Object*, ReflectionInvocation::AllocateObjectForByRef, void *targetTH, Object *valueUNSAFE) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(targetTH));
        PRECONDITION(CheckPointer(valueUNSAFE, NULL_OK));
    }
    CONTRACTL_END;

    TypeHandle targetType = TypeHandle::FromPtr(targetTH);
    OBJECTREF value = ObjectToOBJECTREF(valueUNSAFE);
    OBJECTREF obj = value;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, value, obj);
    CorElementType targetElementType = targetType.GetSignatureCorElementType();
    if (InvokeUtil::IsPrimitiveType(targetElementType) || targetElementType == ELEMENT_TYPE_VALUETYPE) {
        MethodTable* allocMT = targetType.AsMethodTable();
        if (value != NULL)
            allocMT = value->GetMethodTable();

            // for null Nullable<T> we don't want a default value being created.  
            // just allow the null value to be passed, as it will be converted to 
            // a true nullable 
        if (!(value == NULL && Nullable::IsNullableType(targetType))) {
                // boxed value type are 'read-only' in the sence that you can't
                // only the implementor of the value type can expose mutators.
                // To insure byrefs don't mutate value classes in place, we make
                // a copy (and if we were not given one, we create a null value type
                // instance.
            obj = allocMT->Allocate();
        if (value != NULL)
                CopyValueClassUnchecked(obj->UnBox(), value->UnBox(), allocMT);
        }
    }

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(obj);
}
FCIMPLEND

FCIMPL7(void, RuntimeFieldHandle::SetValue, FieldDesc **ppFieldDesc, Object *targetUNSAFE, Object *valueUNSAFE, EnregisteredTypeHandle enregFieldType, DWORD attr, EnregisteredTypeHandle declaringTH, CLR_BOOL *pDomainInitialized) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(ppFieldDesc));
        PRECONDITION(CheckPointer(*ppFieldDesc));
        PRECONDITION(CheckPointer(enregFieldType));
    }
    CONTRACTL_END;

    TypeHandle fieldType = TypeHandle::FromPtr(enregFieldType);
    TypeHandle declaringType = TypeHandle::FromPtr(declaringTH);

    FC_GC_POLL_NOT_NEEDED();

    FieldDesc* pFieldDesc = *ppFieldDesc;

    struct _gc {
        OBJECTREF       target;
        OBJECTREF       value;
    } gc;

    gc.target   = (OBJECTREF)       targetUNSAFE;
    gc.value    = (OBJECTREF)       valueUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_2(gc.target, gc.value);

    //TODO: cleanup this function
    InvokeUtil::SetValidField(fieldType.GetSignatureCorElementType(), fieldType, pFieldDesc, &gc.target, &gc.value, declaringType, pDomainInitialized);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

//A.CI work
FCIMPL1(Object*, RuntimeTypeHandle::Allocate, TypeHandle *pTypeHandle)  
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(pTypeHandle));
    }
    CONTRACTL_END

        // Handle the nullable<T> special case
    if (Nullable::IsNullableType(*pTypeHandle)) {
        return OBJECTREFToObject(Nullable::BoxedNullableNull(*pTypeHandle));
    }

    OBJECTREF rv = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    rv = AllocateObject(pTypeHandle->GetMethodTable());
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(rv);

}//Allocate
FCIMPLEND

FCIMPL6(Object*, RuntimeTypeHandle::CreateInstance, ReflectClassBaseObject* refThisUNSAFE, CLR_BOOL publicOnly, CLR_BOOL securityOff, CLR_BOOL* bCanBeCached, MethodDesc** constructor, CLR_BOOL *bNeedSecurityCheck) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(refThisUNSAFE));
    }
    CONTRACTL_END;

    if (refThisUNSAFE == NULL) 
        FCThrow(kNullReferenceException);

    MethodDesc* pMeth;

    OBJECTREF           rv      = NULL;
    REFLECTCLASSBASEREF refThis = (REFLECTCLASSBASEREF)ObjectToOBJECTREF(refThisUNSAFE);
    TypeHandle thisTH = refThis->GetType();

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, rv, refThis);

    // Get the type information associated with refThis
    MethodTable* pVMT = thisTH.GetMethodTable();
    if (pVMT == 0 || thisTH.IsTypeDesc())
        COMPlusThrow(kMissingMethodException,L"Arg_NoDefCTor");

    pVMT->EnsureInstanceActive();

    Assembly *pAssem = pVMT->GetAssembly();
    if (pAssem->IsDynamic() && !pAssem->HasRunAccess())
        COMPlusThrow(kNotSupportedException, L"NotSupported_DynamicAssemblyNoRunAccess");

    if (bCanBeCached)
        *bCanBeCached = false;
    
    {
        // If we are creating a COM object which has backing metadata we still
        // need to ensure that the caller has unmanaged code access permission.
        if (pVMT->IsComObjectType())
            Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, SECURITY_UNMANAGED_CODE);

        // if this is an abstract class then we will fail this
        if (pVMT->IsAbstract())  {
            if (pVMT->IsInterface())
                COMPlusThrow(kMissingMethodException,L"Acc_CreateInterface");
            else
                COMPlusThrow(kMissingMethodException,L"Acc_CreateAbst");
        }
        else if (thisTH.ContainsGenericVariables()) {
            COMPlusThrow(kArgumentException,L"Acc_CreateGeneric");
        }
        
        if (pVMT->ContainsStackPtr()) 
            COMPlusThrow(kNotSupportedException, L"NotSupported_ContainsStackPtr");
        
        if (!pVMT->HasDefaultConstructor()) {
            // We didn't find the parameterless constructor,
            //  if this is a Value class we can simply allocate one and return it

            if (!pVMT->IsValueClass()) {
                COMPlusThrow(kMissingMethodException,L"Arg_NoDefCTor");
            }

            bool verifyAccess = !securityOff && !pVMT->IsExternallyVisible();

            if (verifyAccess) {
                RefSecContext sCtx;
                InvokeUtil::CheckAccessType(&sCtx, pVMT, REFSEC_CHECK_MEMBERACCESS|REFSEC_THROW_MEMBERACCESS);
            }

                // Handle the nullable<T> special case
            if (Nullable::IsNullableType(thisTH)) {
                rv = Nullable::BoxedNullableNull(thisTH);
            }
            else 
                rv = pVMT->Allocate();

            if (bNeedSecurityCheck)
                *bNeedSecurityCheck = verifyAccess;
            if (constructor)
                *constructor = NULL;
            if (bCanBeCached)
                *bCanBeCached = !verifyAccess;

            // return rv;
        }
        else {
            pMeth = pVMT->GetDefaultConstructor();
            
            // Validate the method can be called by this caller
            DWORD attr = pMeth->GetAttrs();

            if (!IsMdPublic(attr) && publicOnly)
                COMPlusThrow(kMissingMethodException,L"Arg_NoDefCTor");

            // this security context will be used in cast checking as well
            RefSecContext sCtx;
            bool verifyAccess = !securityOff && (!IsMdPublic(attr) || !pVMT->IsExternallyVisible());
            bool bNeedAccessCheck = verifyAccess || pMeth->RequiresLinktimeCheck();
            if (bNeedAccessCheck) 
            {
                *bNeedSecurityCheck = true;

                CanAccess(pMeth, pVMT, NULL, &sCtx, verifyAccess);
            }
            else
                *bNeedSecurityCheck = false;


            // call the <cinit>
            OBJECTREF Throwable = NULL;
            GCPROTECT_BEGIN(Throwable);
            if (FAILED(pVMT->CheckRunClassInitNT(&Throwable))) {
                OBJECTREF except = InvokeUtil::CreateTargetExcept(&Throwable);
                COMPlusThrow(except);
            }
            GCPROTECT_END();

            // We've got the class, lets allocate it and call the constructor
            if (pVMT->IsThunking())
                COMPlusThrow(kMissingMethodException,L"NotSupported_Constructor");

            OBJECTREF o;
            BOOL remoting=FALSE ;
            if (CRemotingServices::IsRemoteActivationRequired(thisTH)) {
                o = CRemotingServices::CreateProxyOrObject(thisTH.GetMethodTable());
                remoting=true ;
            }
            else
                o = AllocateObject(thisTH.GetMethodTable());

            if (!remoting)
            {
                if (constructor)
                    *constructor = pMeth;

                if (bCanBeCached)
                    *bCanBeCached = true;
            }
            
            GCPROTECT_BEGIN(o);

            MethodDescCallSite ctor(pMeth, &o);

            // Copy "this" pointer
            ARG_SLOT arg;
            if (pVMT->IsValueClass())
                arg = PtrToArgSlot(o->UnBox());
            else
                arg = ObjToArgSlot(o);

            // Call the method
            TryCallMethod(&ctor, &arg);

            rv = o;
            GCPROTECT_END();

        }
    }

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(rv);
}
FCIMPLEND

FCIMPL2(Object*, RuntimeTypeHandle::CreateInstanceForGenericType, TypeHandle* pThisTypeHandle, ReflectClassBaseObject* parameterType) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF           rv      = NULL;
    MethodDesc* pMeth;
    TypeHandle genericType(*pThisTypeHandle);

    REFLECTCLASSBASEREF pt = (REFLECTCLASSBASEREF)ObjectToOBJECTREF(parameterType);
    TypeHandle paramaterHandle = pt->GetType();

    _ASSERTE (genericType.HasInstantiation());

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, rv);

    TypeHandle instantiatedType = ((TypeHandle)genericType.GetCanonicalMethodTable()).Instantiate(&paramaterHandle, 1);

    // Get the type information associated with refThis
    MethodTable* pVMT = instantiatedType.GetMethodTable();
    _ASSERTE (pVMT != 0 &&  !instantiatedType.IsTypeDesc());
    _ASSERTE(!(pVMT->GetAssembly()->IsDynamic() && !pVMT->GetAssembly()->HasRunAccess()));
    _ASSERTE( !pVMT->IsAbstract() ||! instantiatedType.ContainsGenericVariables());
    _ASSERTE(!pVMT->ContainsStackPtr() && pVMT->HasDefaultConstructor()); 

     pMeth = pVMT->GetDefaultConstructor();            
     MethodDescCallSite ctor(pMeth);

     // call the <cinit>
     OBJECTREF Throwable = NULL;
     GCPROTECT_BEGIN(Throwable);
     if (FAILED(pVMT->CheckRunClassInitNT(&Throwable))) {
         OBJECTREF except = InvokeUtil::CreateTargetExcept(&Throwable);
         COMPlusThrow(except);
     }
     GCPROTECT_END();

      // We've got the class, lets allocate it and call the constructor
     _ASSERTE(!pVMT->IsThunking());
     _ASSERTE(!CRemotingServices::IsRemoteActivationRequired(instantiatedType));
   
    // Nullables don't take this path, if they do we need special logic to make an instance
    _ASSERTE(!Nullable::IsNullableType(instantiatedType));
    OBJECTREF o = instantiatedType.GetMethodTable()->Allocate();

    GCPROTECT_BEGIN(o);
    ARG_SLOT arg = ObjToArgSlot(o); 

    // Call the method
    TryCallMethod(&ctor, &arg);

    rv = o;
    GCPROTECT_END();

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(rv);
}
FCIMPLEND


FCIMPL2(FC_BOOL_RET, RuntimeTypeHandle::IsInstanceOfType, TypeHandle* pTypeHandle, Object *object) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(pTypeHandle));
        PRECONDITION(CheckPointer(pTypeHandle->AsPtr()));
    }
    CONTRACTL_END;

    // Null is not instance of anything in reflection world
    if (object == NULL)
        FC_RETURN_BOOL(false);

    BOOL canCast = false;

    switch (ObjIsInstanceOfNoGC(object, *pTypeHandle)) {
    case TypeHandle::CanCast:
        FC_RETURN_BOOL(true);
    case TypeHandle::CannotCast:
        FC_RETURN_BOOL(false);
    default:
        // fall through to the slow helper
        break;
    }

    OBJECTREF obj = (OBJECTREF)object;
    HELPER_METHOD_FRAME_BEGIN_RET_1(obj);
    {
        canCast = ObjIsInstanceOf(OBJECTREFToObject(obj), *pTypeHandle);
    }
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(canCast);
}
FCIMPLEND


FCIMPL1(DWORD, ReflectionInvocation::GetSpecialSecurityFlags, MethodDesc *pMethod) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(pMethod));
    }
    CONTRACTL_END;
    
    DWORD dwFlags = 0;
    HELPER_METHOD_FRAME_BEGIN_RET_0();

    // this is an information that is critical for ctors, otherwise is not important
    // we get it here anyway to semplify code
    MethodTable *pMT = pMethod->GetMethodTable();
    _ASSERTE(pMT);
    if (pMT->ContainsStackPtr())
        dwFlags |= INVOCATION_FLAGS_CONTAINS_STACK_POINTERS;

    // Is this a call to a potentially dangerous method? (If so, we're going
    // to demand additional permission).
    if (IsDangerousMethod(pMethod))
        dwFlags |= INVOCATION_FLAGS_RISKY_METHOD;

    // Is there a link demand?
    if (pMethod->RequiresLinktimeCheck()) {
        dwFlags |= INVOCATION_FLAGS_NEED_SECURITY;
         // Check if we are imposing a security check on the caller though the callee didnt ask for it
         // DONT USE THE GC REFS OTHER THAN FOR TESTING NULL !!!!!
         OBJECTREF
            refClassCasDemands = NULL,
            refClassNonCasDemands = NULL,
            refMethodCasDemands = NULL,
            refMethodNonCasDemands = NULL;

         Security::RetrieveLinktimeDemands(
            pMethod,
            &refClassCasDemands,
            &refClassNonCasDemands,
            &refMethodCasDemands,
            &refMethodNonCasDemands);

         if (refClassCasDemands == NULL && refClassNonCasDemands == NULL &&
             refMethodCasDemands == NULL && refMethodNonCasDemands == NULL)
             dwFlags |= INVOCATION_FLAGS_SECURITY_IMPOSED;

    }

    HELPER_METHOD_FRAME_END();
    return dwFlags;
}
FCIMPLEND

// Can not inline this function.
#ifdef _MSC_VER
__declspec(noinline)
#endif
void PerformSecurityCheckHelper(Object *targetUnsafe, MethodDesc *pMeth, MethodTable* pParentMT, DWORD dwFlags)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;

        PRECONDITION(CheckPointer(pMeth));
    }
    CONTRACTL_END;

    OBJECTREF target (targetUnsafe);
    GCPROTECT_BEGIN (target);
    FrameWithCookie<DebuggerSecurityCodeMarkFrame> __dbgSecFrame;
    RefSecContext sCtx;   
    MethodTable* pInstanceMT = NULL;
    if (target != NULL) {
        if (!target->GetTypeHandle().IsTypeDesc())
            pInstanceMT = target->GetTypeHandle().GetMethodTable();
    }

    if (dwFlags & INVOCATION_FLAGS_CONSTRUCTOR_INVOKE)
        CanAccess(pMeth, pParentMT, pInstanceMT, &sCtx);
    else
        CanAccess(pMeth, pParentMT, pInstanceMT, &sCtx,
                  TRUE, (dwFlags & INVOCATION_FLAGS_IS_CTOR) != 0, 
                  (dwFlags & INVOCATION_FLAGS_SECURITY_IMPOSED) != 0, TRUE);
    __dbgSecFrame.Pop();
    GCPROTECT_END ();
}

FCIMPL4(void, ReflectionInvocation::PerformSecurityCheck, Object *target, MethodDesc *pMeth, EnregisteredTypeHandle enregParent, DWORD dwFlags) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(pMeth));
    }
    CONTRACTL_END;
    HELPER_METHOD_FRAME_BEGIN_0();
    //CAUTION: PerformSecurityCheckHelper could trigger GC!
    
    TypeHandle parent = TypeHandle::FromPtr(enregParent);
    PerformSecurityCheckHelper(target,pMeth,parent.GetMethodTable(),dwFlags);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

#ifndef DACCESS_COMPILE

// macros for gc protection
#define STACK_STRUCT_GCPROTECT_BEGIN(context)           do {              \
                FrameWithCookie<CustomGCFrame> __gcframe(ReportPointersFromStruct,    \
                context);                                           \
                /* work around unreachable code warning */          \
                if (true) { DEBUG_ASSURE_NO_RETURN_BEGIN

#define STACK_STRUCT_GCPROTECT_END()                                      \
                DEBUG_ASSURE_NO_RETURN_END; }                       \
                __gcframe.Pop(); } while(0)


#endif // #ifndef DACCESS_COMPILE

/****************************************************************************/
/* boxed Nullable<T> are represented as a boxed T, so there is no unboxed
   Nullable<T> inside to point at by reference.  Because of this a byref
   parameters  of type Nullable<T> are copied out of the boxed instance
   (to a place on the stack), before the call is made (and this copy is
   pointed at).  After the call returns, this copy must be copied back to
   the original argument array.  ByRefToNullable, is a simple linked list
   that remembers what copy-backs are needed */

struct ByRefToNullable  {
    unsigned argNum;            // The argument number for this byrefNullable argument
    void* data;                 // The data to copy back to the ByRefNullable.  This points to the stack 
    TypeHandle type;            // The type of Nullable for this argument
    ByRefToNullable* next;      // list of these

    ByRefToNullable(unsigned aArgNum, void* aData, TypeHandle aType, ByRefToNullable* aNext) {
        argNum = aArgNum;
        data = aData;
        type = aType;
        next = aNext;
    }

    static void CopyBackToArgs(ByRefToNullable* ptr, PTRARRAYREF* args) {
        CONTRACTL {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        while (ptr != NULL) {
            OBJECTREF obj = Nullable::Box(ptr->data, ptr->type.GetMethodTable());
            SetObjectReference(&(*args)->m_Array[ptr->argNum], obj, (*args)->GetAppDomain());
            ptr = ptr->next;
        }
    }
};

OBJECTREF InvokeImpl(MethodDesc *pMeth, OBJECTREF targetUNSAFE, PTRARRAYREF argsUNSAFE, SignatureNative* pSig, DWORD attr, TypeHandle ownerType) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    OBJECTREF rv = 0;

    if (pMeth == NULL)
        COMPlusThrow(kNullReferenceException, L"NullReference_This");

#ifdef _DEBUG 

    if (g_pConfig->ShouldInvokeHalt(pMeth))
    {
        _ASSERTE(!"InvokeHalt");
    }

#endif
    
    struct _gc {
        OBJECTREF       target;
        PTRARRAYREF     args;
        OBJECTREF       targetException;
    } gc;
    gc.target = (OBJECTREF)targetUNSAFE;
    gc.args = (PTRARRAYREF)argsUNSAFE;
    gc.targetException = NULL;

    GCPROTECT_BEGIN(gc);

    // GC could happen here
    pMeth->EnsureActive();

    ARG_SLOT ret = 0;
    ByRefToNullable* byRefToNullables = 0;

    // Build the arguments on the stack
    UINT nNumSlots = pSig->NumVirtualFixedArgs(IsMdStatic(attr));

    // Make sure we have enough room on the stack for this.
    INTERIOR_STACK_PROBE_FOR(GetThread(), (nNumSlots*sizeof(ARG_SLOT) / OS_PAGE_SIZE) + HOLDER_CODE_NORMAL_STACK_LIMIT);

    ARG_SLOT *pNewArgs = (ARG_SLOT*) _alloca( nNumSlots*sizeof(ARG_SLOT) );
    ARG_SLOT *pTmpPtr = pNewArgs;

#ifdef STRESS_HEAP
    if (g_pConfig->GetGCStressLevel() != 0)
        g_pGCHeap->StressHeap();
#endif

    // if we have the magic Value Class return, we need to allocate that class
    // and place a pointer to it on the stack.
    OBJECTREF objRet = NULL;
    GCPROTECT_BEGIN(objRet);

    TypeHandle retTH = pSig->GetReturnTypeHandle();
    CorElementType retType = retTH.GetInternalCorElementType();
    if (retType == ELEMENT_TYPE_VALUETYPE) {
        objRet = retTH.GetMethodTable()->Allocate();
    }
    else  {
        _ASSERTE(!pSig->IsRetBuffArg());
    }

    BOOL bIsTargetValueClass = pMeth->GetClass()->IsValueClass();

    BOOL bCanCacheTargetAndCrackedSig = CanCacheTargetAndCrackedSig(pMeth);
    MethodDescCallSite method(pMeth, pSig, bCanCacheTargetAndCrackedSig, &gc.target, ownerType);

    // NO GC AFTER THIS POINT
    // actually this statement is not completely accurate. If an exception occurs a gc may happen
    // but we are going to dump the stack anyway and we do not need to protect anything.
    // But if anywhere between here and the method invocation we enter preemptive mode the stack
    // we are about to setup (pDst in the loop) may contain references to random parts of memory

    // Copy "this" pointer
    if (!IsMdStatic(attr)) {
        if (!bIsTargetValueClass)
            *pTmpPtr = ObjToArgSlot(gc.target);
        else {
            if (pMeth->IsUnboxingStub())
                *pTmpPtr = ObjToArgSlot(gc.target);
            else {
                    // Create a true boxed Nullable<T> and use that as the 'this' pointer.
                    // since what is passed in is just a boxed T
                MethodTable* pMT = pMeth->GetMethodTable();
                if (Nullable::IsNullableType(pMT)) {
                    OBJECTREF bufferObj = pMT->Allocate();
                    void* buffer = bufferObj->GetData();
                    Nullable::UnBox(buffer, gc.target, pMT);
                    *pTmpPtr = PtrToArgSlot(buffer);
                }
            else
                *pTmpPtr = PtrToArgSlot(gc.target->UnBox());
        }
        }
        pTmpPtr++;
    }

    ARG_SLOT *retBuffPtr = NULL;
    // Take care of any return arguments
    if (pSig->IsRetBuffArg()) {
        retBuffPtr = pTmpPtr++;
    }    


    // count whether there is any struct that will need stack allocation. They need to be protected
    // We do a first pass to determine stack allocation and then we loop again to copy the args with a frame in place
    int stackStructCount = 0;
    void *pEnum;
    pSig->Reset(&pEnum);
    for (int i = 0 ; i < (int)pSig->NumFixedArgs(); i++) {
        TypeHandle th = pSig->NextArgExpanded(&pEnum);
        UINT cbSize = MetaSig::GetElemSize(th);

        bool needsStackCopy = (cbSize > sizeof(ARG_SLOT));
        TypeHandle nullableType = NullableTypeOfByref(th);
        if (!nullableType.IsNull()) {
            th = nullableType;
            needsStackCopy = true;
        }

        if(needsStackCopy) {
            MethodTable *pMT = th.GetMethodTable();
            if (pMT && pMT->ContainsPointers())
                stackStructCount++;
        }
    }
    
#ifndef DACCESS_COMPILE
    // make an allocation for the frame needed to report possible GC pointers living inside stack allocated structs
    StackStructData *pStackInfo = (StackStructData*)_alloca(sizeof(StackStructData)*(stackStructCount+1)); // use one off as a guard
    memset((void*)pStackInfo, 0, sizeof(StackStructData)*(stackStructCount+1));
    STACK_STRUCT_GCPROTECT_BEGIN(pStackInfo);
#endif

    // copy args
    pSig->Reset(&pEnum);
    for (int i = 0 ; i < (int)pSig->NumFixedArgs(); i++) {
        TypeHandle th = pSig->NextArgExpanded(&pEnum);
        UINT cbSize = MetaSig::GetElemSize(th);

        bool needsStackCopy = (cbSize > sizeof(ARG_SLOT));
        TypeHandle nullableType = NullableTypeOfByref(th);
        if (!nullableType.IsNull()) {
            th = nullableType;
            cbSize = th.GetSize();
            needsStackCopy = true;
        }

        PVOID argDst;
        if(needsStackCopy)
        {
            _ASSERTE(th.GetMethodTable() && th.GetMethodTable()->IsValueType());

            // argument address is placed in the slot
            argDst = _alloca(cbSize);
            *pTmpPtr = PtrToArgSlot(argDst);
            if (!nullableType.IsNull())
                byRefToNullables = new(_alloca(sizeof(ByRefToNullable))) ByRefToNullable(i, argDst, nullableType, byRefToNullables);

#ifndef DACCESS_COMPILE
            // save the info into StackStructData
            if (th.GetMethodTable()->ContainsPointers()) 
            {
                _ASSERTE(stackStructCount > 0);
                pStackInfo->pLocation = argDst;
                pStackInfo->pValueType = th.GetMethodTable();
                pStackInfo++;
                --stackStructCount;
            }
#endif
        }
        else
        {
            // the argument itself is placed in the slot
            argDst = ArgSlotEndianessFixup(pTmpPtr, cbSize);
        }
        InvokeUtil::CopyArg(th, &(gc.args->m_Array[i]), pTmpPtr, argDst);
        pTmpPtr ++;
    }
    _ASSERTE(stackStructCount == 0);

    // Call the method
    EX_TRY {
        if (pSig->IsRetBuffArg()) {
            _ASSERTE(objRet);
            COMPILER_ASSUME_MSG(retBuffPtr != NULL, "retBuffPtr should not be NULL");
            // The return buffer is a true boxed nullable, which we convert later on
            *retBuffPtr = PtrToArgSlot(objRet->GetData());
        }

        ret = method.CallWithValueTypes_RetArgSlot(pNewArgs);
    } EX_CATCH {
        // If we get here we need to throw a TargetInvocationException.
        // However we don't want to do this inside the catch block since we need
        // to limit our backout stack consumption.
        gc.targetException = GETTHROWABLE();
        _ASSERTE(gc.targetException);
    } EX_END_CATCH(SwallowAllExceptions);

    // Now that we are safely out of the catch block, we can create and raise the
    // TargetInvocationException.
    if (gc.targetException != NULL)
    {
        OBJECTREF except = InvokeUtil::CreateTargetExcept(&gc.targetException);
        COMPlusThrow(except);
    }

    // It is still illegal to do a GC here.  The return type might have/contain GC pointers.
    if (retType == ELEMENT_TYPE_VALUETYPE) 
    {
        _ASSERTE(objRet != NULL);

        // if the structure is returned by value, then we need to copy in the boxed object
        // we have allocated for this purpose.
        if (!pSig->IsRetBuffArg()) 
        {
            void* retDataPtr = &ret;
#ifdef CALLDESCR_RETBUF
            //

            BYTE SmallVCRetVal[ENREGISTERED_RETURNTYPE_MAXSIZE];
            MethodTable* pMT      = retTH.GetMethodTable();
            CorElementType hfaType = pMT->GetClass()->GetHFAType();
            UINT           cbVC    = pMT->GetNumInstanceFieldBytes();

            if (ELEMENT_TYPE_R4_HFA == hfaType ||
                ELEMENT_TYPE_R8_HFA == hfaType ||
                cbVC > sizeof(void*))
            {
                memcpy(SmallVCRetVal, GetThread()->m_SmallVCRetVal, sizeof(SmallVCRetVal));
                retDataPtr = SmallVCRetVal;
            }
#endif // CALLDESCR_RETBUF
            CopyValueClass(objRet->GetData(), retDataPtr, objRet->GetMethodTable(), objRet->GetAppDomain());
        }

        objRet = Nullable::NormalizeBox(objRet);
    }
    else 
    {
        objRet =  InvokeUtil::CreateObject(retTH, ret);
    }
    
    ByRefToNullable::CopyBackToArgs(byRefToNullables, &gc.args);
    *((OBJECTREF*)&rv) = objRet;

#ifndef DACCESS_COMPILE
    STACK_STRUCT_GCPROTECT_END();
#endif    
    GCPROTECT_END();        // objRet
    END_INTERIOR_STACK_PROBE;
    GCPROTECT_END();        // gc
    
    return rv;
}

FCIMPL6(Object*, RuntimeMethodHandle::InvokeMethodFast, 
    MethodDesc **ppMethod, Object *target, PTRArray *objs, SignatureNative* pSig, DWORD attr, EnregisteredTypeHandle enregOwnerType) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
    }
    CONTRACTL_END;

    MethodDesc *pMethod = *ppMethod;
    TypeHandle ownerType = TypeHandle::FromPtr(enregOwnerType);

    OBJECTREF retVal = NULL;
    
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, retVal);
    {
        retVal = InvokeImpl(pMethod, (OBJECTREF)target, (PTRARRAYREF)objs, pSig, attr, ownerType);
    }
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(retVal);
}
FCIMPLEND

///////////////////////////////////////////////
// Constructor invocation code
//

LPVOID InvokeArrayConstructor(ArrayTypeDesc* arrayDesc, MethodDesc* pMeth, PTRARRAYREF* objs, int argCnt) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    LPVOID rv = 0;
    DWORD i;

    // If we're trying to create an array of pointers or function pointers,
    // check that the caller has skip verification permission.
    CorElementType et = arrayDesc->GetArrayElementTypeHandle().GetVerifierCorElementType();
    if (et == ELEMENT_TYPE_PTR || et == ELEMENT_TYPE_FNPTR)
        Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, SECURITY_SKIP_VER);

    // Validate the argCnt an the Rank. Also allow nested SZARRAY's.
    _ASSERTE(argCnt == (int) arrayDesc->GetRank() || argCnt == (int) arrayDesc->GetRank() * 2 ||
             arrayDesc->GetInternalCorElementType() == ELEMENT_TYPE_SZARRAY);

    // Validate all of the parameters.  These all typed as integers
    int allocSize = 0;
    if (!ClrSafeInt<int>::multiply(sizeof(INT32), argCnt, allocSize))
        COMPlusThrow(kArgumentException, IDS_EE_SIGTOOCOMPLEX);
        
    INT32* indexes = (INT32*) _alloca((size_t)allocSize);
    ZeroMemory(indexes, allocSize);

    for (i=0; i<(DWORD)argCnt; i++)
    {
        if (!(*objs)->m_Array[i])
            COMPlusThrowArgumentException(L"parameters", L"Arg_NullIndex");
        
        MethodTable* pMT = ((*objs)->m_Array[i])->GetMethodTable();
        CorElementType oType = TypeHandle(pMT).GetVerifierCorElementType();
        
        if (!InvokeUtil::IsPrimitiveType(oType) || !InvokeUtil::CanPrimitiveWiden(ELEMENT_TYPE_I4,oType))
            COMPlusThrow(kArgumentException,L"Arg_PrimWiden");
        
        memcpy(&indexes[i],(*objs)->m_Array[i]->UnBox(),pMT->GetNumInstanceFieldBytes());
    }

    PTRARRAYREF pRet = (PTRARRAYREF) AllocateArrayEx(TypeHandle(arrayDesc), indexes, argCnt);
    *((PTRARRAYREF *)&rv) = pRet;
    return rv;
}

Object* InvokeConstructorHelper(MethodDesc *pMeth, PTRARRAYREF *args, SignatureNative* pSig, TypeHandle declaringType) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    // Activate the method's assembly if necessary
    pMeth->EnsureActive();

    // Build the Args...This is in [0]
    // All of the rest of the args are placed in reverse order into the arg array.
    struct _gc {
        OBJECTREF o;
    } gc;

    gc.o = NULL;

    OBJECTREF rv = NULL;
    ByRefToNullable* byRefToNullables = 0;
    
    UINT argCnt = pSig->NumFixedArgs();
    // If we are invoking a constructor on an array then we must
    // handle this specially.  String objects allocate themselves
    // so they are a special case.
    if (declaringType.IsArray()) {
        gc.o = ObjectToOBJECTREF((Object*)InvokeArrayConstructor(declaringType.AsArray(),
                                                              pMeth,
                                                              args,
                                                              argCnt));
        goto lExit;
    }

    GCPROTECT_BEGIN(gc);

    if (CRemotingServices::IsRemoteActivationRequired(declaringType))
    {
        gc.o = CRemotingServices::CreateProxyOrObject(declaringType.AsMethodTable());
    }
    else
    {
        if (declaringType != TypeHandle(g_pStringClass))
            gc.o = declaringType.AsMethodTable()->Allocate();
    }

    BOOL bCanCacheTargetAndCrackedSig = CanCacheTargetAndCrackedSig(pMeth);
    MethodDescCallSite method(pMeth, pSig, bCanCacheTargetAndCrackedSig);

    // Make sure we allocated the callArgs.  We must have
    //  at least one because of the this pointer
    UINT nArgSlots = pSig->NumVirtualFixedArgs(pMeth->IsStatic());
    ARG_SLOT *pNewArgs = (ARG_SLOT *) _alloca(nArgSlots*sizeof(ARG_SLOT));

#ifdef STRESS_HEAP
    if (g_pConfig->GetGCStressLevel() != 0)
        g_pGCHeap->StressHeap();
#endif

    // NO GC AFTER THIS POINT
    // actually this statement is not completely accurate. If an exception occurs a gc may happen
    // but we are going to dump the stack anyway and we do not need to protect anything.
    // But if anywhere between here and the method invocation we enter preemptive mode the stack
    // we are about to setup (pDst in the loop) may contain references to random parts of memory

    ARG_SLOT *  pTmpPtr = pNewArgs;

    // Copy "this" pointer
    if (declaringType.IsValueType()) {
        // Note that we create a true boxed nullabe<T> and then convert it to a T below
        *pTmpPtr = PtrToArgSlot(gc.o->GetData());
    }
    else
        *pTmpPtr = ObjToArgSlot(gc.o);
    pTmpPtr++;

    // count whether there is any struct that will need stack allocation. They need to be protected
    // We do a first pass to determine stack allocation and then we loop again to copy the args with a frame in place
    int stackStructCount = 0;
    void *pEnum;
    pSig->Reset(&pEnum);
    for (int i = 0 ; i < (int)pSig->NumFixedArgs(); i++) {
        TypeHandle th = pSig->NextArgExpanded(&pEnum);
        UINT cbSize = MetaSig::GetElemSize(th);

        bool needsStackCopy = (cbSize > sizeof(ARG_SLOT));
        TypeHandle nullableType = NullableTypeOfByref(th);
        if (!nullableType.IsNull()) {
            th = nullableType;
            needsStackCopy = true;
        }

        if(needsStackCopy) {
            if (th.GetMethodTable()->ContainsPointers())
                stackStructCount++;
        }
    }
    
#ifndef DACCESS_COMPILE
    // make an allocation for the frame needed to report possible GC pointers living inside stack allocated structs
    size_t cbAlloc = 0;
    size_t iStackStructCount = 0;

    // use one off as a guard
    if (!ClrSafeInt<size_t>::addition(stackStructCount, 1, iStackStructCount))
        ThrowHR(E_INVALIDARG);
    if (!ClrSafeInt<size_t>::multiply(sizeof(StackStructData), iStackStructCount, cbAlloc))
        ThrowHR(E_INVALIDARG);

    StackStructData *pStackInfo = (StackStructData*)_alloca(cbAlloc);
    memset((void*)pStackInfo, 0, cbAlloc);
    STACK_STRUCT_GCPROTECT_BEGIN(pStackInfo);
#endif

    // copy args
    pSig->Reset(&pEnum);
    for (int i = 0; i < (int)argCnt; i++) {
        TypeHandle th = pSig->NextArgExpanded(&pEnum);

        UINT cbSize = MetaSig::GetElemSize(th);
        PVOID argDst;
        bool needsStackCopy = (cbSize > sizeof(ARG_SLOT));
        TypeHandle nullableType = NullableTypeOfByref(th);
        if (!nullableType.IsNull()) {
            th = nullableType;
            cbSize = th.GetSize();
            needsStackCopy = true;
        }

        if(needsStackCopy) {
            _ASSERTE(th.GetMethodTable() && th.GetMethodTable()->IsValueType());
            // argument address is placed in the slot
            argDst = _alloca(cbSize);
            *pTmpPtr = PtrToArgSlot(argDst);
            if (!nullableType.IsNull())
                byRefToNullables = new(_alloca(sizeof(ByRefToNullable))) ByRefToNullable(i, argDst, nullableType, byRefToNullables);

#ifndef DACCESS_COMPILE
            // save the info into StackStructData
            if (th.GetMethodTable()->ContainsPointers()) 
            {
                PREFIX_ASSUME(stackStructCount > 0);
                pStackInfo->pLocation = argDst;
                pStackInfo->pValueType = th.GetMethodTable();
                pStackInfo++;
                --stackStructCount;
            }
#endif
        }
        else {
            // the argument itself is placed in the slot
            argDst = ArgSlotEndianessFixup(pTmpPtr, cbSize);
        }
        InvokeUtil::CopyArg(th, &((*args)->m_Array[i]), pTmpPtr, argDst);
        pTmpPtr ++;
    }
    _ASSERTE(stackStructCount == 0);

    // Call the method
    // Constructors always return null....
    ARG_SLOT ret = 0;
    OBJECTREF ppException = NULL;
    GCPROTECT_BEGIN(ppException);
    EX_TRY {
        ret = method.CallWithValueTypes_RetArgSlot(pNewArgs);
    } EX_CATCH {        
        ppException = GETTHROWABLE();
        _ASSERTE(ppException);
    } EX_END_CATCH(RethrowTransientExceptions)
    
    // It is important to re-throw outside the catch block because re-throwing will invoke
    // the jitter and managed code and will cause us to use more than the backout stack limit.
    if (ppException != NULL)
    {
        // If we get here we need to throw an TargetInvocationException
        OBJECTREF except = InvokeUtil::CreateTargetExcept(&ppException);
        COMPlusThrow(except);
    }
    GCPROTECT_END();
    
    // We have a special case for Strings...The object is returned...
    if (declaringType == TypeHandle(g_pStringClass))
        gc.o = ArgSlotToObj(ret);

    ByRefToNullable::CopyBackToArgs(byRefToNullables, args);

#ifndef DACCESS_COMPILE
    STACK_STRUCT_GCPROTECT_END();
#endif    
    GCPROTECT_END();

    gc.o = Nullable::NormalizeBox(gc.o);
lExit:    
    rv = gc.o;

    return OBJECTREFToObject(rv);
}

FCIMPL4(Object*, RuntimeMethodHandle::InvokeConstructor, MethodDesc **ppMethod, PTRArray *argsUNSAFE, SignatureNative* pSig, EnregisteredTypeHandle enregDeclaringType) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;

        PRECONDITION(CheckPointer(ppMethod));
        PRECONDITION(CheckPointer(*ppMethod));
        PRECONDITION(CheckPointer(enregDeclaringType));
        SO_TOLERANT;
    }
    CONTRACTL_END;

    TypeHandle declaringType = TypeHandle::FromPtr(enregDeclaringType);

    MethodDesc *pMethod = *ppMethod;

    OBJECTREF rv = NULL;
    PTRARRAYREF args = (PTRARRAYREF)argsUNSAFE;
    
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, args);
    {
        rv = ObjectToOBJECTREF(InvokeConstructorHelper(pMethod, &args, pSig, declaringType));
    }
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(rv);
}
FCIMPLEND

FCIMPL5(void, RuntimeMethodHandle::SerializationInvoke, 
    MethodDesc **ppMethod, Object* targetUNSAFE, SignatureNative* pSig, Object* serializationInfoUNSAFE, struct StreamingContextData context) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MethodDesc *pMethod = *ppMethod;

    struct _gc
    {
        OBJECTREF       target;
        OBJECTREF       serializationInfo;
    } gc;

    gc.target               = (OBJECTREF)      targetUNSAFE;
    gc.serializationInfo    = (OBJECTREF)      serializationInfoUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_PROTECT(gc);

    // Make sure we call the <cinit>
    OBJECTREF Throwable = NULL;
    GCPROTECT_BEGIN(Throwable);
    if (FAILED(pMethod->GetMethodTable()->CheckRunClassInitNT(&Throwable))) {
        OBJECTREF except = InvokeUtil::CreateTargetExcept(&Throwable);
        COMPlusThrow(except);
    }
    GCPROTECT_END();

    {
        BOOL bCanCacheTargetAndCrackedSig = CanCacheTargetAndCrackedSig(pMethod);
        MethodDescCallSite method(pMethod, pSig, bCanCacheTargetAndCrackedSig);

    ARG_SLOT newArgs[4];

    // make sure method has correct size sig
    //_ASSERTE(mSig->SizeOfVirtualFixedArgStack(false/*IsStatic*) == sizeof(newArgs));

        // Nullable<T> does not support the ISerializable constructor, so we should never get here.  
    _ASSERTE(!Nullable::IsNullableType(gc.target->GetMethodTable()));

    // NO GC AFTER THIS POINT
    // Copy "this" pointer
    if (pMethod->GetMethodTable()->IsValueClass())
        newArgs[0] = PtrToArgSlot(gc.target->UnBox());
    else
        newArgs[0] = ObjToArgSlot(gc.target);

    newArgs[1] = ObjToArgSlot(gc.serializationInfo);

    //
    // on x86 the struct fits in an ARG_SLOT, so we pass it by value
    //
    C_ASSERT(sizeof(context) == sizeof(ARG_SLOT));
    newArgs[2] = *(ARG_SLOT*)&context;

        TryCallMethod(&method, newArgs);
    }

    HELPER_METHOD_FRAME_END_POLL();
}
FCIMPLEND

struct SkipStruct {
    StackCrawlMark* pStackMark;
    MethodDesc*     pMeth;
};

// This method is called by the GetMethod function and will crawl backward
//  up the stack for integer methods.
static StackWalkAction SkipMethods(CrawlFrame* frame, VOID* data) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    SkipStruct* pSkip = (SkipStruct*) data;

    MethodDesc *pFunc = frame->GetFunction();

    /* We asked to be called back only for functions */
    _ASSERTE(pFunc);

    // The check here is between the address of a local variable
    // (the stack mark) and a pointer to the EIP for a frame
    // (which is actually the pointer to the return address to the
    // function from the previous frame). So we'll actually notice
    // which frame the stack mark was in one frame later. This is
    // fine since we only implement LookForMyCaller.
    _ASSERTE(*pSkip->pStackMark == LookForMyCaller);
    if (!IsInCalleesFrames(frame->GetRegisterSet(), pSkip->pStackMark))
        return SWA_CONTINUE;

    if (pFunc->RequiresInstMethodDescArg())
    {
        pSkip->pMeth = (MethodDesc *) frame->GetParamTypeArg();
        if (pSkip->pMeth == NULL)
            pSkip->pMeth = pFunc;
    }
    else
        pSkip->pMeth = pFunc;
    return SWA_ABORT;
}

// Return the MethodInfo that represents the current method (two above this one)
FCIMPL1(MethodDesc*, RuntimeMethodHandle::GetCurrentMethod, StackCrawlMark* stackMark) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    SkipStruct skip;
    skip.pStackMark = stackMark;
    skip.pMeth = 0;
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    StackWalkFunctions(GetThread(), SkipMethods, &skip);
    HELPER_METHOD_FRAME_END();
   
    return skip.pMeth;
}
FCIMPLEND

static OBJECTREF DirectObjectFieldGet(FieldDesc *pField, TypeHandle fieldType, TypeHandle enclosingType, TypedByRef target, CLR_BOOL *pDomainInitialized) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;

        PRECONDITION(CheckPointer(pField));
    }
    CONTRACTL_END;
    
    OBJECTREF refRet;
    OBJECTREF objref = NULL;
    GCPROTECT_BEGIN(objref);
    if (!pField->IsStatic()) {
        objref = ObjectToOBJECTREF(*((Object**)target.data));
    }

    InvokeUtil::ValidateObjectTarget(pField, enclosingType, &objref);
    refRet = InvokeUtil::GetFieldValue(pField, fieldType, &objref, enclosingType, pDomainInitialized);
    GCPROTECT_END();
    return refRet;
}

FCIMPL4(Object*, RuntimeFieldHandle::GetValueDirect, FieldDesc **ppField, EnregisteredTypeHandle enregFieldType, TypedByRef target, EnregisteredTypeHandle enregDeclaringType) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    
        PRECONDITION(CheckPointer(ppField));
        PRECONDITION(CheckPointer(*ppField));
        PRECONDITION(CheckPointer(enregFieldType));
        PRECONDITION(CheckPointer(enregDeclaringType));
    }
    CONTRACTL_END;

    TypeHandle fieldType = TypeHandle::FromPtr(enregFieldType);

    FieldDesc *pField = *ppField;

    OBJECTREF refRet  = NULL;
    CorElementType fieldElType;

    HELPER_METHOD_FRAME_BEGIN_RET_0();

    // Find the Object and its type
    TypeHandle targetType = target.type;
    _ASSERTE(TypeHandle::FromPtr(enregDeclaringType).IsUnsharedMT());
    MethodTable *pEnclosingMT = (MethodTable*)enregDeclaringType; 

    // Verify the callee/caller access
    if (!pField->IsPublic() || !pEnclosingMT->IsExternallyVisible()) {
        RefSecContext sCtx;
        MethodTable* pInstanceMT = NULL;
        if (!pField->IsStatic()) {
            if (!targetType.IsTypeDesc())
                pInstanceMT = targetType.GetMethodTable();
        }

        //TODO: missing check that the field is consistent

        // Perform the normal access check (caller vs field).
        InvokeUtil::CheckAccess(&sCtx,
                                pField->GetAttributes(), // should we cache?
                                pEnclosingMT,
                                pInstanceMT,
                                NULL,
                                REFSEC_CHECK_MEMBERACCESS|REFSEC_THROW_FIELDACCESS,  pField);
    }

    CLR_BOOL domainInitialized = FALSE;
    if (pField->IsStatic() || !targetType.IsValueType()) {
        refRet = DirectObjectFieldGet(pField, fieldType, TypeHandle(pEnclosingMT), target, &domainInitialized);
        goto lExit;
    }

    // Validate that the target type can be cast to the type that owns this field info.
    if (!targetType.CanCastTo(TypeHandle(pEnclosingMT)))
        COMPlusThrowArgumentException(L"obj", NULL);

    void* p;
    fieldElType = fieldType.GetSignatureCorElementType();
    switch (fieldElType) {
    case ELEMENT_TYPE_VOID:
        _ASSERTE(!"Void used as Field Type!");
        COMPlusThrow(kInvalidProgramException);

    case ELEMENT_TYPE_BOOLEAN:  // boolean
    case ELEMENT_TYPE_I1:       // byte
    case ELEMENT_TYPE_U1:       // unsigned byte
    case ELEMENT_TYPE_I2:       // short
    case ELEMENT_TYPE_U2:       // unsigned short
    case ELEMENT_TYPE_CHAR:     // char
    case ELEMENT_TYPE_I4:       // int
    case ELEMENT_TYPE_U4:       // unsigned int
    case ELEMENT_TYPE_I:
    case ELEMENT_TYPE_U:
    case ELEMENT_TYPE_R4:       // float
    case ELEMENT_TYPE_I8:       // long
    case ELEMENT_TYPE_U8:       // unsigned long
    case ELEMENT_TYPE_R8:       // double
    case ELEMENT_TYPE_VALUETYPE:
        _ASSERTE(fieldType.IsUnsharedMT());
        p = ((BYTE*) target.data) + pField->GetOffset();
        refRet = fieldType.AsMethodTable()->Box(p);
        break;

    case ELEMENT_TYPE_OBJECT:
    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_SZARRAY:          // Single Dim, Zero
    case ELEMENT_TYPE_ARRAY:            // general array
        p = ((BYTE*) target.data) + pField->GetOffset();
        refRet = ObjectToOBJECTREF(*(Object**) p);
        break;

    case ELEMENT_TYPE_PTR:
        {
            p = ((BYTE*) target.data) + pField->GetOffset();
            UINT32 value = *(UINT32*) p;

            FieldDesc* ptrTypeFld = g_Mscorlib.GetField(FIELD__POINTER__TYPE);
            FieldDesc* ptrValueFld = g_Mscorlib.GetField(FIELD__POINTER__VALUE);
            OBJECTREF obj = g_Mscorlib.GetClass(CLASS__POINTER)->Allocate();
            GCPROTECT_BEGIN(obj);

            // Ignore null return
            OBJECTREF typeOR = fieldType.GetManagedClassObject();
            ptrTypeFld->SetRefValue(obj, typeOR);
            ptrValueFld->SetValue32(obj, (int)value);
            refRet = obj;
            GCPROTECT_END();
            break;
        }

    default:
        _ASSERTE(!"Unknown Type");
        // this is really an impossible condition
        COMPlusThrow(kNotSupportedException);
    }

lExit: ;
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRet);
}
FCIMPLEND

static void DirectObjectFieldSet(FieldDesc *pField, TypeHandle fieldType, TypeHandle enclosingType, TypedByRef target, OBJECTREF *pValue, CLR_BOOL *pDomainInitialized) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    
        PRECONDITION(CheckPointer(pField));
        PRECONDITION(!fieldType.IsNull());
    }
    CONTRACTL_END;

    OBJECTREF objref = NULL;
    GCPROTECT_BEGIN(objref);
    if (!pField->IsStatic()) {
        objref = ObjectToOBJECTREF(*((Object**)target.data));
    }
    // Validate the target/fld type relationship
    InvokeUtil::ValidateObjectTarget(pField, enclosingType, &objref);

    RefSecContext sCtx;
    InvokeUtil::ValidField(fieldType, pValue, &sCtx);
    InvokeUtil::SetValidField(pField->GetFieldType(), fieldType, pField, &objref, pValue, enclosingType, pDomainInitialized);
    GCPROTECT_END();
}

FCIMPL5(void, RuntimeFieldHandle::SetValueDirect, FieldDesc **ppField, EnregisteredTypeHandle enregFieldType, TypedByRef target, Object *valueUNSAFE, EnregisteredTypeHandle enregContextTH) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    
        PRECONDITION(CheckPointer(ppField));
        PRECONDITION(CheckPointer(*ppField));
        PRECONDITION(CheckPointer(enregFieldType));
        PRECONDITION(CheckPointer(enregContextTH, NULL_OK));
    }
    CONTRACTL_END;

    TypeHandle fieldType = TypeHandle::FromPtr(enregFieldType);
    TypeHandle contextType = TypeHandle::FromPtr(enregContextTH);

    FieldDesc *pField = *ppField;

    OBJECTREF       oValue   = (OBJECTREF)       valueUNSAFE;
    BYTE           *pDst = NULL;
    ARG_SLOT        value = NULL;
    CorElementType  fieldElType;

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();
    GCPROTECT_BEGIN(oValue);
    GCPROTECT_BEGININTERIOR(target.data);

    // Find the Object and its type
    TypeHandle targetType = target.type;
    MethodTable *pEnclosingMT = contextType.GetMethodTable();

    {
        // security and consistency checks
        RefSecContext sCtx;

        // Verify that the value passed can be widened into the target
        InvokeUtil::ValidField(fieldType, &oValue, &sCtx);

        // Verify that this is not a Final Field
        DWORD attr = pField->GetAttributes(); // should we cache?
        if (IsFdInitOnly(attr)) {
            TryDemand(SECURITY_SERIALIZATION, kFieldAccessException, L"Acc_ReadOnly");
        }
        if (IsFdHasFieldRVA(attr)) {
            TryDemand(SECURITY_SKIP_VER, kFieldAccessException, L"Acc_RvaStatic");
        }
        if (IsFdLiteral(attr))
            COMPlusThrow(kFieldAccessException,L"Acc_ReadOnly");

        // Verify the callee/caller access
        if (!pField->IsPublic() || (pEnclosingMT != NULL && !pEnclosingMT->IsExternallyVisible())) {
            RefSecContext sCtx;
            MethodTable* pInstanceMT = NULL;
            if (!pField->IsStatic()) {
                if (!targetType.IsTypeDesc())
                    pInstanceMT = targetType.GetMethodTable();
            }

            //TODO: missing check that the field is consistent

            // Perform the normal access check (caller vs field).
            InvokeUtil::CheckAccess(&sCtx,
                                    pField->GetAttributes(), // should we cache?
                                    pEnclosingMT,
                                    pInstanceMT,
                                    NULL,
                                    REFSEC_CHECK_MEMBERACCESS|REFSEC_THROW_FIELDACCESS,  pField);
        }

    }

    CLR_BOOL domainInitialized = FALSE;
    if (pField->IsStatic() || !targetType.IsValueType()) {
        DirectObjectFieldSet(pField, fieldType, TypeHandle(pEnclosingMT), target, &oValue, &domainInitialized);
        goto lExit;
    }

    if (oValue == NULL && !Nullable::IsNullableType(fieldType))
        COMPlusThrowArgumentNull(L"value");

    // Validate that the target type can be cast to the type that owns this field info.
    if (!targetType.CanCastTo(TypeHandle(pEnclosingMT)))
        COMPlusThrowArgumentException(L"obj", NULL);

    // Set the field
    fieldElType = fieldType.GetInternalCorElementType();
    if (ELEMENT_TYPE_BOOLEAN <= fieldElType && fieldElType <= ELEMENT_TYPE_R8) {
        CorElementType objType = oValue->GetTypeHandle().GetInternalCorElementType();
        if (objType != fieldElType)
            InvokeUtil::CreatePrimitiveValue(fieldElType, objType, oValue, &value);
        else
            value = *(ARG_SLOT*)oValue->UnBox();
    }
    pDst = ((BYTE*) target.data) + pField->GetOffset();

    switch (fieldElType) {
    case ELEMENT_TYPE_VOID:
        _ASSERTE(!"Void used as Field Type!");
        COMPlusThrow(kInvalidProgramException);

    case ELEMENT_TYPE_BOOLEAN:  // boolean
    case ELEMENT_TYPE_I1:       // byte
    case ELEMENT_TYPE_U1:       // unsigned byte
        *(volatile UINT8*)pDst = *(UINT8*)&value;
    break;

    case ELEMENT_TYPE_I2:       // short
    case ELEMENT_TYPE_U2:       // unsigned short
    case ELEMENT_TYPE_CHAR:     // char
        *(volatile UINT16*)pDst = *(UINT16*)&value;
    break;

    case ELEMENT_TYPE_I4:       // int
    case ELEMENT_TYPE_U4:       // unsigned int
    case ELEMENT_TYPE_R4:       // float
        *(volatile UINT32*)pDst = *(UINT32*)&value;
    break;

    case ELEMENT_TYPE_I8:       // long
    case ELEMENT_TYPE_U8:       // unsigned long
    case ELEMENT_TYPE_R8:       // double
        *(volatile UINT64*)pDst = *(UINT64*)&value;
    break;

    case ELEMENT_TYPE_I:
    {
        INT_PTR valuePtr = (INT_PTR) InvokeUtil::GetIntPtrValue(&oValue);
        *(volatile INT_PTR*) pDst = valuePtr;
    }    
    break;
    case ELEMENT_TYPE_U:
    {
        UINT_PTR valuePtr = (UINT_PTR) InvokeUtil::GetUIntPtrValue(&oValue);
        *(volatile UINT_PTR*) pDst = valuePtr;
    }
    break;

    case ELEMENT_TYPE_PTR:      // pointers
        if (oValue != 0) {
            value = 0;
            if (oValue->GetTypeHandle() == TypeHandle(g_Mscorlib.GetClass(CLASS__POINTER))) {
                value = (size_t) InvokeUtil::GetPointerValue(&oValue);
                *(volatile size_t*) pDst = (size_t) value;
                break;
            }
        }
    // drop through
    case ELEMENT_TYPE_FNPTR:
    {
        value = 0;
        if (oValue != 0) {
            CorElementType objType = oValue->GetTypeHandle().GetInternalCorElementType();
            InvokeUtil::CreatePrimitiveValue(objType, objType, oValue, &value);
        }
        *(volatile size_t*) pDst = (size_t) value;
    }
    break;

    case ELEMENT_TYPE_SZARRAY:          // Single Dim, Zero
    case ELEMENT_TYPE_ARRAY:            // General Array
    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_OBJECT:
        SetObjectReferenceUnchecked((OBJECTREF*)pDst, oValue);
    break;

    case ELEMENT_TYPE_VALUETYPE:
    {
        _ASSERTE(fieldType.IsUnsharedMT());
        MethodTable* pMT = fieldType.AsMethodTable();

        // If we have a null value then we must create an empty field
        if (oValue == 0)
            InitValueClass(pDst, pMT);
        else {
            pMT->UnBoxIntoUnchecked(pDst, oValue);
        }
    }
    break;

    default:
        _ASSERTE(!"Unknown Type");
        // this is really an impossible condition
        COMPlusThrow(kNotSupportedException);
    }

lExit: ;
    GCPROTECT_END ();
    GCPROTECT_END ();
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

// This method triggers the class constructor for a give type
FCIMPL1(void, ReflectionInvocation::RunClassConstructor, LPVOID handle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    if (handle == NULL)
        FCThrowArgumentVoidEx(kArgumentException, NULL, L"InvalidOperation_HandleIsNotInitialized");

    TypeHandle typeHnd = TypeHandle::FromPtr(handle);
    if (typeHnd.IsUnsharedMT()) {
        MethodTable *pMT = typeHnd.AsMethodTable();

        if (pMT->IsClassInited()) 
            return;

        Assembly *pAssem = typeHnd.GetAssembly();
        if (pAssem->IsDynamic() && !pAssem->HasRunAccess())
            FCThrowResVoid(kNotSupportedException, L"NotSupported_DynamicAssemblyNoRunAccess");

        HELPER_METHOD_FRAME_BEGIN_0();
        pMT->CheckRestore();
        pMT->EnsureInstanceActive();
        pMT->CheckRunClassInitThrowing();
        HELPER_METHOD_FRAME_END();
    }
}
FCIMPLEND

// This method triggers the module constructor for a give module
FCIMPL1(void, ReflectionInvocation::RunModuleConstructor, LPVOID handle) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    if (handle == NULL)
        FCThrowArgumentVoidEx(kArgumentException, NULL, L"InvalidOperation_HandleIsNotInitialized");

    Module *module = (Module*) handle;
    DomainFile *pDomainFile = module->FindDomainFile(GetAppDomain());
    if (pDomainFile==NULL || !pDomainFile->IsActive())
    {
        HELPER_METHOD_FRAME_BEGIN_0();
        if(pDomainFile==NULL)
            pDomainFile=module->GetDomainFile();
        pDomainFile->EnsureActive();
        HELPER_METHOD_FRAME_END();
    }
}
FCIMPLEND

static void PrepareMethodDesc(MethodDesc *pMD, TypeHandle *pClassInst, TypeHandle *pMethInst)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    

    SigTypeContext sTypeContext(pMD, pClassInst, pMethInst);
    MethodCallGraphPreparer mcgp(pMD, &sTypeContext, true, true);
    mcgp.Run();
}

// This method triggers a given method to be jitted
FCIMPL2(void, ReflectionInvocation::PrepareMethod, LPVOID handle, PTRArray* instantiationUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    
        PRECONDITION(CheckPointer(handle, NULL_OK));
        PRECONDITION(CheckPointer(instantiationUNSAFE, NULL_OK));
    }
    CONTRACTL_END;
    
    if (handle == NULL)
        FCThrowArgumentVoidEx(kArgumentException, NULL, L"InvalidOperation_HandleIsNotInitialized");

    MethodDesc *pMD = (MethodDesc*) handle;
    
    if (pMD->IsAbstract())
        FCThrowArgumentVoidEx(kArgumentException, L"method", L"Argument_CannotPrepareAbstract");

    PTRARRAYREF pInstantiation = (PTRARRAYREF)instantiationUNSAFE;
    
    HELPER_METHOD_FRAME_BEGIN_PROTECT(pInstantiation);
    pMD->CheckRestore();

    TypeHandle *pClassInst = NULL;
    TypeHandle *pMethInst = NULL;
    if (pInstantiation != NULL)
    {
        // We were handed an instantiation, check that the method expects it and the right number of types has been provided (the
        // caller supplies one array containing the class instantiation immediately followed by the method instantiation).
        if (pInstantiation->GetNumComponents() != (pMD->GetNumGenericMethodArgs() + pMD->GetNumGenericClassArgs()))
            COMPlusThrow(kArgumentException, L"Argument_InvalidGenericInstantiation");

        // We need to find the actual class and/or method instantiations, even though we've been passed them. This is an issue of
        // lifetime -- the instantiation passed in will go away at some point whereas preparation of the method has the potential to
        // persist a copy of the instantiation pointer. By finding the actual instantiation we get a stable pointer whose lifetime
        // is at least as long as the data generated by preparation.

        // Make a copy of the type handles passed in (since they're sitting in an array on the GC heap and the class loader call
        // below can cause trigger GC).
        int iAllocSize;
        if (!ClrSafeInt<int>::multiply(sizeof(TypeHandle), pInstantiation->GetNumComponents(), iAllocSize))
            COMPlusThrow(kArgumentException, L"Argument_InvalidGenericInstantiation");
        TypeHandle *pTempInst = (TypeHandle*)_alloca(iAllocSize);
        CopyMemory(pTempInst, pInstantiation->GetDataPtr(), iAllocSize);

        // Check we've got a reasonable looking instantiation.
        if (!Generics::CheckInstantiation(pInstantiation->GetNumComponents(), pTempInst))
            COMPlusThrow(kArgumentException, L"Argument_InvalidGenericInstantiation");
        for (ULONG i = 0; i < pInstantiation->GetNumComponents(); i++)
            if (pTempInst[i].ContainsGenericVariables())
                COMPlusThrow(kArgumentException, L"Argument_InvalidGenericInstantiation");

        // Load the exact type of the method if it needs to be instantiated (because it's a generic type definition, e.g. C<T>, or a
        // shared type instantiation, e.g. C<Object>).
        MethodTable *pExactMT = pMD->GetMethodTable();
        if (pExactMT->IsGenericTypeDefinition() || pExactMT->IsSharedByGenericInstantiations())
        {
            TypeHandle thExactType = ClassLoader::LoadGenericInstantiationThrowing(pMD->GetModule(),
                                                                                   pMD->GetMethodTable()->GetCl(),
                                                                                   pMD->GetNumGenericClassArgs(),
                                                                                   pTempInst);
            pExactMT = thExactType.AsMethodTable();
        }

        // Grab the class part of the instantiation.
        pClassInst = pExactMT->GetInstantiation();

        // As for the class we might need to find a method desc with an exact instantiation if the one we have is too vague.
        if (pMD->IsGenericMethodDefinition() || pMD->IsSharedByGenericInstantiations() || pMD->ContainsGenericVariables())
            pMD = MethodDesc::FindOrCreateAssociatedMethodDesc(pMD,
                                                               pExactMT,
                                                               FALSE,
                                                               pMD->GetNumGenericMethodArgs(),
                                                               &pTempInst[pMD->GetNumGenericClassArgs()],
                                                               FALSE);
        // Grab the method part of the instantiation.
        pMethInst = pMD->GetMethodInstantiation();
    }
    else
    {
        // No instantiation provided, the method better not be expecting one.

        // Methods that are generic definitions (e.g. C.Foo<U>) and those that are shared (e.g. C<Object>.Foo, C.Foo<Object>) need
        // extra instantiation data.
        if (pMD->IsGenericMethodDefinition() || pMD->IsSharedByGenericInstantiations() || pMD->ContainsGenericVariables())
            COMPlusThrow(kArgumentException, L"Argument_InvalidGenericInstantiation");

        // The rest of the cases (non-generics related methods, instantiating stubs, methods instantiated over non-shared types
        // etc.) should be able to provide their instantiation for us as necessary (normal methods will just return NULL in the
        // calls below).
        pClassInst = pMD->GetClassInstantiation();
        pMethInst = pMD->GetMethodInstantiation();
    }

    // Go prepare the method at the specified instantiation.
    PrepareMethodDesc(pMD, pClassInst, pMethInst);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

// This method triggers a given delegate to be prepared.  This involves preparing the
// delegate's Invoke method and preparing the target of that Invoke.  In the case of
// a multi-cast delegate, we rely on the fact that each individual component was prepared
// prior to the Combine.  If our event sinks perform the Combine, this is always true.
// If the client calls Combine himself, he is responsible for his own preparation.
FCIMPL1(void, ReflectionInvocation::PrepareDelegate, Object* delegateUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    
        PRECONDITION(CheckPointer(delegateUNSAFE, NULL_OK));
    }
    CONTRACTL_END;
    
    if (delegateUNSAFE == NULL)
        return;

    OBJECTREF delegate = ObjectToOBJECTREF(delegateUNSAFE);
    HELPER_METHOD_FRAME_BEGIN_1(delegate);

    PrepareDelegateHelper(&delegate);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


void ReflectionInvocation::PrepareDelegateHelper(OBJECTREF *pDelegate)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pDelegate));
        PRECONDITION(CheckPointer(OBJECTREFToObject(*pDelegate)));
    }
    CONTRACTL_END;
    
    // Make sure the delegate subsystem itself is prepared.
    // Force the immediate creation of any global stubs required. This is platform specific.
    COMDelegate::TheDelegateInvokeStub(FALSE);
    COMDelegate::TheDelegateInvokeStub(TRUE);

    MethodDesc *pMDTarget = COMDelegate::GetMethodDesc(*pDelegate);
    MethodDesc *pMDInvoke = COMDelegate::FindDelegateInvokeMethod((*pDelegate)->GetMethodTable());

    // If someone does give us a multicast delegate, then both MDs will be the same -- they
    // will both be the Delegate's Invoke member.  Normally, pMDTarget points at the method
    // the delegate is wrapping, of course.
    if (pMDTarget == pMDInvoke)
    {
        pMDTarget->CheckRestore();

        // The invoke method itself is never generic, but the delegate class itself might be.
        PrepareMethodDesc(pMDInvoke, pMDInvoke->GetExactClassInstantiation((*pDelegate)->GetTypeHandle()), NULL);
    }
    else
    {
        pMDTarget->CheckRestore();
        pMDInvoke->CheckRestore();

        // Prepare the eventual target method first.

        // Load the exact type of the method if it needs to be instantiated (because it's a generic type definition, e.g. C<T>, or a
        // shared type instantiation, e.g. C<Object>).
        MethodTable *pExactMT = pMDTarget->GetMethodTable();
        if (pExactMT->IsGenericTypeDefinition() || pExactMT->IsSharedByGenericInstantiations())
            pExactMT = COMDelegate::GetTargetObject(*pDelegate)->GetMethodTable();

        // For delegates with generic target methods it must be the case that we are passed an instantiating stub -- there's no
        // other way the necessary method instantiation information can be passed to us.
        _ASSERTE(!pMDTarget->IsGenericMethodDefinition() &&
                 !pMDTarget->IsSharedByGenericInstantiations() &&
                 !pMDTarget->ContainsGenericVariables());

        PrepareMethodDesc(pMDTarget, pExactMT->GetInstantiation(), pMDTarget->GetMethodInstantiation());

        // Now prepare the delegate invoke method.
        // The invoke method itself is never generic, but the delegate class itself might be.
        PrepareMethodDesc(pMDInvoke, pMDInvoke->GetExactClassInstantiation((*pDelegate)->GetTypeHandle()), NULL);
    }

    PCCOR_SIGNATURE pSig;
    DWORD cbSigSize;
    g_Mscorlib.GetMethodBinarySigAndSize(METHOD__EVENT_HANDLER__INVOKE, &pSig, &cbSigSize);
}

FCIMPL0(void, ReflectionInvocation::ProbeForSufficientStack)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

#ifndef TOTALLY_DISBLE_STACK_GUARDS
    Thread *pThread = GetThread();
    // probe for our entry point amount and throw if not enough stack
    _ASSERTE (pThread->IsSOTolerant());
    RetailStackProbe(ADJUST_PROBE(DEFAULT_ENTRY_PROBE_AMOUNT), pThread);
#else
    FCUnique(0x69);
#endif

}
FCIMPLEND

struct ECWGCFContext
{
    BOOL fHandled;
    Frame *pStartFrame;
};

// Crawl the stack looking for Thread Abort related information (whether we're executing inside a CER or an error handling clauses
// of some sort).
StackWalkAction ECWGCFCrawlCallBack(CrawlFrame* pCf, void* data)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    ECWGCFContext *pData = (ECWGCFContext *)data;

    Frame *pFrame = pCf->GetFrame();
    if (pFrame && pFrame->GetFunction() != NULL && pFrame != pData->pStartFrame)
    {
        // We walk through a transition frame, but it is not our start frame.
        // This means ExecuteCodeWithGuarantee is not at the bottom of stack.
        pData->fHandled = TRUE;
        return SWA_ABORT;
    }

    MethodDesc *pMD = pCf->GetFunction();

    // Non-method frames don't interest us.
    if (pMD == NULL)
        return SWA_CONTINUE;

    if (!pMD->GetModule()->IsSystem())
    {
        // We walk through some user code.  This means that ExecuteCodeWithGuarantee is not at the bottom of stack.
        pData->fHandled = TRUE;
        return SWA_ABORT;
    }

    return SWA_CONTINUE;
}

struct ECWGC_Param
{
    BOOL fExceptionThrownInTryCode;
    BOOL fStackOverflow;
    ECWGC_Param()
    {
        fExceptionThrownInTryCode = FALSE;
        fStackOverflow = FALSE;
    }
};

LONG SODetectionFilter(EXCEPTION_POINTERS *ep, void* pv)
{
    if (DefaultCatchFilter(ep, COMPLUS_EXCEPTION_EXECUTE_HANDLER) == EXCEPTION_CONTINUE_EXECUTION)
    {
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    // We should never hit a soft stack overflow exception here.
    CONSISTENCY_CHECK(DebugGetCxxException(ep->ExceptionRecord) != (void*)Exception::GetSOException());

    // Record the fact that an exception occured while running the try code.
    ECWGC_Param *pParam= (ECWGC_Param *)pv;
    pParam->fExceptionThrownInTryCode = TRUE;

    // We unwind the stack only in the case of a stack overflow.
    if (ep->ExceptionRecord->ExceptionCode == STATUS_STACK_OVERFLOW)
    {
        pParam->fStackOverflow = TRUE;
        return EXCEPTION_EXECUTE_HANDLER;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

struct ECWGC_GC
{
    OBJECTREF       codeDelegate;
    OBJECTREF       backoutDelegate;
    OBJECTREF       userData;
    ECWGC_GC()
    {
        codeDelegate = NULL;
        backoutDelegate = NULL;
        userData = NULL;
    }
};

void ExecuteCodeWithGuaranteedCleanupBackout(ECWGC_GC *gc, MethodDescCallSite* pExecuteBackoutCodeHelper, BOOL fExceptionThrownInTryCode)
{
    // We need to prevent thread aborts from occuring for the duration of the call to the backout code. 
    // Once we enter managed code, the CER will take care of it as well; however without this holder, 
    // MethodDesc::Call would raise a thread abort exception if the thread is currently requesting one.
    ThreadPreventAbortHolder preventAbort;

    GCX_COOP();

    // Now we can safely call the helper which will call the user backout code from within a CER.
    ARG_SLOT args[] = 
    {
        ObjToArgSlot(gc->backoutDelegate),
        ObjToArgSlot(gc->userData),
        (ARG_SLOT)fExceptionThrownInTryCode
    };        
    pExecuteBackoutCodeHelper->Call(args);
}

void ExecuteCodeWithGuaranteedCleanupHelper (ECWGC_GC *gc)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    // Set up the call sites for the invoke and backout helper.
    MethodDesc *pTryCodeInvokeMD = ((DelegateEEClass*)((gc->codeDelegate)->GetMethodTable()->GetClass()))->m_pInvokeMethod;    
    _ASSERTE(pTryCodeInvokeMD);
    MethodDescCallSite tryCodeInvoke(pTryCodeInvokeMD, &gc->codeDelegate);
    MethodDescCallSite executeBackoutCodeHelper(g_pExecuteBackoutCodeHelperMethod, (OBJECTREF*)NULL, TypeHandle(), TRUE);

    ECWGC_Param param;

    PAL_TRY
    {
        PAL_TRY
        {
            // Call the user's try code.
            ARG_SLOT args[] = 
            {
                ObjToArgSlot(gc->codeDelegate),
                ObjToArgSlot(gc->userData)
            };            
            tryCodeInvoke.Call(args);
        }
        PAL_EXCEPT_FILTER(SODetectionFilter, &param)
        {
        }
        PAL_ENDTRY;

        if (param.fStackOverflow)
        {
            Thread *pThread = GetThread();
            if (!pThread->PreemptiveGCDisabled())
            {
                pThread->DisablePreemptiveGC();
            }
        }
    }
    PAL_FINALLY
    {
        ExecuteCodeWithGuaranteedCleanupBackout(gc, &executeBackoutCodeHelper, param.fExceptionThrownInTryCode);
    }
    PAL_ENDTRY;

    if (param.fStackOverflow)   
        COMPlusThrowSO();
}

//
// ExecuteCodeWithGuaranteedCleanup ensures that we will call the backout code delegate even if an SO occurs. We do this by calling the 
// try delegate from within an EX_TRY/EX_CATCH block that will catch any thrown exceptions and thus cause the stack to be unwound. This 
// guarantees that the backout delegate is called with at least DEFAULT_ENTRY_PROBE_SIZE pages of stack. After the backout delegate is called, 
// we re-raise any exceptions that occured inside the try delegate. Note that any CER that uses large or arbitraty amounts of stack in 
// it's try block must use ExecuteCodeWithGuaranteedCleanup. 
//
// ExecuteCodeWithGuaranteedCleanup also guarantees that the backount code will be run before any filters higher up on the stack. This
// is important to prevent security exploits.
//
FCIMPL3(void, ReflectionInvocation::ExecuteCodeWithGuaranteedCleanup, Object* codeDelegateUNSAFE, Object* backoutDelegateUNSAFE, Object* userDataUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;

        PRECONDITION(CheckPointer(codeDelegateUNSAFE, NULL_OK));
        PRECONDITION(CheckPointer(backoutDelegateUNSAFE, NULL_OK));
        PRECONDITION(CheckPointer(userDataUNSAFE, NULL_OK));
    }
    CONTRACTL_END;

    ECWGC_GC gc;

    gc.codeDelegate = ObjectToOBJECTREF(codeDelegateUNSAFE);
    gc.backoutDelegate = ObjectToOBJECTREF(backoutDelegateUNSAFE);
    gc.userData = ObjectToOBJECTREF(userDataUNSAFE);

    HELPER_METHOD_FRAME_BEGIN_PROTECT(gc);

    if (gc.codeDelegate == NULL)
        COMPlusThrowArgumentNull(L"code");
    if (gc.backoutDelegate == NULL)
        COMPlusThrowArgumentNull(L"backoutCode");

    {
        // Delegates are prepared as part of the ngen process, so only prepare the backout 
        // delegate for non-ngen processes. 
        PrepareDelegateHelper(&gc.backoutDelegate);

        // Make sure the managed backout code helper function has been prepared before we 
        // attempt to run the backout code.
        PrepareMethodDesc(g_pExecuteBackoutCodeHelperMethod, NULL, NULL);
    }

    ExecuteCodeWithGuaranteedCleanupHelper(&gc);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


FCIMPL4(void, ReflectionInvocation::MakeTypedReference, TypedByRef * value, Object* targetUNSAFE, ArrayBase* fldsUNSAFE, EnregisteredTypeHandle enregFieldType)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    
        PRECONDITION(CheckPointer(targetUNSAFE));
        PRECONDITION(CheckPointer(fldsUNSAFE));
    }
    CONTRACTL_END;
    
    DWORD offset = 0;

    OBJECTREF   target  = (OBJECTREF)   targetUNSAFE;
    BASEARRAYREF flds   = (BASEARRAYREF) fldsUNSAFE;
    TypeHandle fieldType = TypeHandle::FromPtr(enregFieldType);

    HELPER_METHOD_FRAME_BEGIN_2(target, flds);
    GCPROTECT_BEGININTERIOR (value)

    DWORD cnt = flds->GetNumComponents();
    FieldDesc** fields = (FieldDesc**)flds->GetDataPtr();
    for (DWORD i = 0; i < cnt; i++) {
        FieldDesc* pField = fields[i];
        offset += pField->GetOffset();
    }

        // Fields already are prohibted from having ArgIterator and RuntimeArgumentHandles
    _ASSERTE(!target->GetTypeHandle().GetMethodTable()->ContainsStackPtr());

    // Create the ByRef
    value->data = ((BYTE *)(target->GetAddress() + offset)) + sizeof(Object);
    value->type = fieldType;

    GCPROTECT_END ();
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL2(void, ReflectionInvocation::SetTypedReference, TypedByRef * target, Object* objUNSAFE) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    HELPER_METHOD_FRAME_BEGIN_0();
    COMPlusThrow(kNotSupportedException);
    HELPER_METHOD_FRAME_END();

}
FCIMPLEND


// This is an internal helper function to TypedReference class.
// It extracts the object from the typed reference.
FCIMPL1(Object*, ReflectionInvocation::TypedReferenceToObject, TypedByRef * value) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    OBJECTREF       Obj = NULL;

    TypeHandle th(value->type);

    if (th.IsNull())
        FCThrowRes(kArgumentNullException, L"ArgumentNull_TypedRefType");

    MethodTable* pMT = th.GetMethodTable();
    PREFIX_ASSUME(NULL != pMT);

    if (pMT->IsValueType())
    {
        // value->data is protected by the caller
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, Obj);

        Obj = pMT->Box(value->data);

        HELPER_METHOD_FRAME_END();
    }
    else {
        Obj = ObjectToOBJECTREF(*((Object**)value->data));
    }

    return OBJECTREFToObject(Obj);
}
FCIMPLEND

FCIMPL2_IV(Object*, ReflectionInvocation::CreateEnum, void *typeHandle, INT64 value) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    _ASSERTE(TypeHandle::FromPtr(typeHandle).IsEnum());
    OBJECTREF obj = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(Frame::FRAME_ATTR_RETURNOBJ);
    MethodTable *pEnumMT = TypeHandle::FromPtr(typeHandle).AsMethodTable();
    obj = pEnumMT->Box(ArgSlotEndianessFixup ((ARG_SLOT*)&value,
                                             pEnumMT->GetNumInstanceFieldBytes()));

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(obj);
}
FCIMPLEND


FCIMPL2(void, ReflectionInvocation::GetGUID, ReflectClassBaseObject* refThisUNSAFE, GUID * result) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    REFLECTCLASSBASEREF refThis = (REFLECTCLASSBASEREF) refThisUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_1(refThis);
    GCPROTECT_BEGININTERIOR (result);

    if (result == NULL || refThis == NULL)
        COMPlusThrow(kNullReferenceException);

    TypeHandle type = refThis->GetType();
    if (type.IsTypeDesc()) {
        memset(result,0,sizeof(GUID));
        goto lExit;
    }


    MethodTable *pVMT;
    pVMT = type.GetMethodTable();
    _ASSERTE(pVMT);
    GUID guid;
    pVMT->GetGuid(&guid, TRUE);
    memcpyNoGCRefs(result, &guid, sizeof(GUID));

lExit: ;
    GCPROTECT_END ();
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************
//      ReflectionSerialization
//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************
FCIMPL1(Object*, ReflectionSerialization::GetUninitializedObject, ReflectClassBaseObject* objTypeUNSAFE) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    OBJECTREF           retVal  = NULL;
    REFLECTCLASSBASEREF objType = (REFLECTCLASSBASEREF) objTypeUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(Frame::FRAME_ATTR_RETURNOBJ);

    if (objType == NULL) {
        COMPlusThrowArgumentNull(L"type", L"ArgumentNull_Type");
    }

    TypeHandle type = objType->GetType();

    // Don't allow arrays, pointers, byrefs or function pointers.
    if (!type.IsUnsharedMT())
        COMPlusThrow(kArgumentException, L"Argument_InvalidValue");

    MethodTable *pMT = type.GetMethodTable();
    PREFIX_ASSUME(pMT != NULL);

    //We don't allow unitialized strings.
    if (pMT == g_pStringClass) {
        COMPlusThrow(kArgumentException, L"Argument_NoUninitializedStrings");
    }

    // if this is an abstract class or an interface type then we will
    //  fail this
    if (pMT->IsAbstract()) {
        COMPlusThrow(kMemberAccessException,L"Acc_CreateAbst");
    }
    else if (pMT->ContainsGenericVariables()) {
        COMPlusThrow(kMemberAccessException,L"Acc_CreateGeneric");
    }

    // Never allow the allocation of an unitialized ContextBoundObject derived type, these must always be created with a paired
    // transparent proxy or the jit will get confused.
    if (pMT->IsContextful())
        COMPlusThrow(kNotSupportedException, L"NotSupported_ManagedActivation");

    // If it is a nullable, return the underlying type instead.  
    if (Nullable::IsNullableType(pMT)) 
        pMT = pMT->GetInstantiation()[0].GetMethodTable();
 
    retVal = pMT->Allocate();

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(retVal);
}
FCIMPLEND

FCIMPL1(Object*, ReflectionSerialization::GetSafeUninitializedObject, ReflectClassBaseObject* objTypeUNSAFE) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    OBJECTREF           retVal  = NULL;
    REFLECTCLASSBASEREF objType = (REFLECTCLASSBASEREF) objTypeUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, objType);
    
    if (objType == NULL) 
        COMPlusThrowArgumentNull(L"type", L"ArgumentNull_Type");

    TypeHandle type = objType->GetType();

    // Don't allow arrays, pointers, byrefs or function pointers.
    if (!type.IsUnsharedMT())
        COMPlusThrow(kArgumentException, L"Argument_InvalidValue");

    MethodTable *pMT = type.GetMethodTable();
    PREFIX_ASSUME(pMT != NULL);

    //We don't allow unitialized strings.
    if (pMT == g_pStringClass) 
        COMPlusThrow(kArgumentException, L"Argument_NoUninitializedStrings");


    // if this is an abstract class or an interface type then we will
    //  fail this
    if (pMT->IsAbstract())
        COMPlusThrow(kMemberAccessException,L"Acc_CreateAbst");
    else if (pMT->ContainsGenericVariables()) {
        COMPlusThrow(kMemberAccessException,L"Acc_CreateGeneric");
    }

    // Never allow the allocation of an unitialized ContextBoundObject derived type, these must always be created with a paired
    // transparent proxy or the jit will get confused.
    if (pMT->IsContextful())
        COMPlusThrow(kNotSupportedException, L"NotSupported_ManagedActivation");

    if (!pMT->GetAssembly()->AllowUntrustedCaller()) {
        OBJECTREF permSet = NULL;
        Security::GetPermissionInstance(&permSet, SECURITY_FULL_TRUST);
        Security::DemandSet(SSWT_LATEBOUND_LINKDEMAND, permSet);
    }

    if (pMT->GetClass()->RequiresLinktimeCheck()) {
        OBJECTREF refClassNonCasDemands = NULL;
        OBJECTREF refClassCasDemands = NULL;

        refClassCasDemands = TypeSecurityDescriptor::GetLinktimePermissions(pMT->GetClass(), &refClassNonCasDemands);

        if (refClassCasDemands != NULL)
            Security::DemandSet(SSWT_LATEBOUND_LINKDEMAND, refClassCasDemands);

    }

    // If it is a nullable, return the underlying type instead.  
    if (Nullable::IsNullableType(pMT)) 
        pMT = pMT->GetInstantiation()[0].GetMethodTable();
 
    retVal = pMT->Allocate();

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(retVal);
}
FCIMPLEND


//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************
//      ReflectionEnum
//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

FCIMPL1(Object *, ReflectionEnum::InternalGetEnumUnderlyingType, ReflectClassBaseObject *target) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    VALIDATEOBJECTREF(target);
    TypeHandle th = target->GetType();
    if (!th.IsEnum())
        FCThrowArgument(NULL, NULL);

    OBJECTREF result = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    MethodTable *pMT = g_Mscorlib.FetchElementType(th.AsMethodTable()->GetInternalCorElementType());
    result = pMT->GetManagedClassObject();
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(result);
}
FCIMPLEND

FCIMPL1(Object *, ReflectionEnum::InternalGetEnumValue, Object *pRefThis) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    VALIDATEOBJECTREF(pRefThis);
    if (pRefThis == NULL)
        FCThrowArgumentNull(NULL);

    OBJECTREF result = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_1(pRefThis);
    MethodTable *pMT = g_Mscorlib.FetchElementType(pRefThis->GetTrueMethodTable()->GetInternalCorElementType());
    result = pMT->Box(pRefThis->UnBox(), TRUE);
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(result);
}
FCIMPLEND

FCIMPL3(void, ReflectionEnum::InternalGetEnumValues, ReflectClassBaseObject *target, Object **pReturnValues, Object **pReturnNames) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    VALIDATEOBJECTREF(target);
    TypeHandle th = target->GetType();

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();

    if (!th.IsEnum())
        COMPlusThrow(kArgumentException, L"Arg_MustBeEnum");
    EnumEEClass *pClass = (EnumEEClass*) th.GetClass();
    HRESULT hr = pClass->BuildEnumTables();
    if (FAILED(hr))
        COMPlusThrowHR(hr);
    DWORD cFields = pClass->GetEnumCount();
    struct gc {
        I8ARRAYREF values;
        PTRARRAYREF names;
    } gc;
    gc.values = NULL;
    gc.names = NULL;
    GCPROTECT_BEGIN(gc);

    gc.values = (I8ARRAYREF) AllocatePrimitiveArray(ELEMENT_TYPE_U8, cFields);

    CorElementType type = pClass->GetMethodTable()->GetInternalCorElementType();
    INT64 *pToValues = gc.values->GetDirectPointerToNonObjectElements();

    DWORD i;
    for (i = 0; i < cFields; i++) {
        switch (type) {
        case ELEMENT_TYPE_I1:
            pToValues[i] = (INT8) pClass->GetEnumByteValues()[i];
            break;

        case ELEMENT_TYPE_U1:
        case ELEMENT_TYPE_BOOLEAN:
            pToValues[i] = pClass->GetEnumByteValues()[i];
            break;

        case ELEMENT_TYPE_I2:
            pToValues[i] = (INT16) pClass->GetEnumShortValues()[i];
            break;

        case ELEMENT_TYPE_U2:
        case ELEMENT_TYPE_CHAR:
            pToValues[i] = pClass->GetEnumShortValues()[i];
            break;

        case ELEMENT_TYPE_I4:
        IN_WIN32(case ELEMENT_TYPE_I:)
            pToValues[i] = (INT32) pClass->GetEnumIntValues()[i];
            break;

        case ELEMENT_TYPE_U4:
        IN_WIN32(case ELEMENT_TYPE_U:)
            pToValues[i] = pClass->GetEnumIntValues()[i];
            break;

        case ELEMENT_TYPE_I8:
        case ELEMENT_TYPE_U8:
        IN_WIN64(case ELEMENT_TYPE_I:)
        IN_WIN64(case ELEMENT_TYPE_U:)
            pToValues[i] = pClass->GetEnumLongValues()[i];
            break;

        default:
            break;
        }
    }

    gc.names = (PTRARRAYREF) AllocateObjectArray(cFields, g_pStringClass);

    LPCUTF8 *pNames = pClass->GetEnumNames();
    for (i = 0; i < cFields; i++) {
        STRINGREF str = COMString::NewString(pNames[i]);
        gc.names->SetAt(i, str);
    }

    *pReturnValues = OBJECTREFToObject(gc.values);
    *pReturnNames = OBJECTREFToObject(gc.names);

    GCPROTECT_END();
    HELPER_METHOD_FRAME_END_POLL();
}
FCIMPLEND

FCIMPL2_IV(Object*, ReflectionEnum::InternalBoxEnum, ReflectClassBaseObject* target, INT64 value) {
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    VALIDATEOBJECTREF(target);
    OBJECTREF ret = NULL;

    MethodTable* pMT = target->GetType().AsMethodTable();
    HELPER_METHOD_FRAME_BEGIN_RET_0();

    ret = pMT->Box(ArgSlotEndianessFixup((ARG_SLOT*)&value, pMT->GetNumInstanceFieldBytes()), FALSE);

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(ret);
}
FCIMPLEND

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************
//      ReflectionBinder
//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

FCIMPL2(FC_BOOL_RET, ReflectionBinder::DBCanConvertPrimitive, ReflectClassBaseObject* source, ReflectClassBaseObject* target) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    VALIDATEOBJECTREF(source);
    VALIDATEOBJECTREF(target);

    CorElementType tSRC = source->GetType().GetSignatureCorElementType();
    CorElementType tTRG = target->GetType().GetSignatureCorElementType();

    FC_RETURN_BOOL(InvokeUtil::IsPrimitiveType(tTRG) && InvokeUtil::CanPrimitiveWiden(tTRG, tSRC));
}
FCIMPLEND

FCIMPL2(FC_BOOL_RET, ReflectionBinder::DBCanConvertObjectPrimitive, Object* sourceObj, ReflectClassBaseObject* target) {
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    VALIDATEOBJECTREF(sourceObj);
    VALIDATEOBJECTREF(target);

    if (sourceObj == 0)
        FC_RETURN_BOOL(true);

    TypeHandle th(sourceObj->GetMethodTable());
    CorElementType tSRC = th.GetVerifierCorElementType();

    CorElementType tTRG = target->GetType().GetSignatureCorElementType();
    FC_RETURN_BOOL(InvokeUtil::IsPrimitiveType(tTRG) && InvokeUtil::CanPrimitiveWiden(tTRG, tSRC));
}
FCIMPLEND

FCIMPL2(FC_BOOL_RET, ReflectionEnum::InternalEquals, Object *pRefThis, Object* pRefTarget)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    VALIDATEOBJECTREF(pRefThis);
    BOOL ret = false;
    if (pRefTarget == NULL) {
        FC_RETURN_BOOL(ret);
    }

    if( pRefThis == pRefTarget)
        FC_RETURN_BOOL(true);

    //Make sure we are comparing same type.
    MethodTable* pMTThis = pRefThis->GetMethodTable();
    _ASSERTE(!pMTThis->IsArray());
    if ( pMTThis != pRefTarget->GetMethodTable()) {
        FC_RETURN_BOOL(ret);
    }

    void * pThis = pRefThis->UnBox();
    void * pTarget = pRefTarget->UnBox();
    switch (pMTThis->GetNumInstanceFieldBytes()) {
    case 1:
        ret = (*(UINT8*)pThis == *(UINT8*)pTarget);
        break;
    case 2:
        ret = (*(UINT16*)pThis == *(UINT16*)pTarget);
        break;
    case 4:
        ret = (*(UINT32*)pThis == *(UINT32*)pTarget);
        break;
    case 8:
        ret = (*(UINT64*)pThis == *(UINT64*)pTarget);
        break;
    default:
        // should not reach here.
        UNREACHABLE_MSG("Incorrect Enum Type size!");
        break;
    }

    FC_RETURN_BOOL(ret);
}
FCIMPLEND


// compare two boxed enums using their underlying enum type
FCIMPL2(int, ReflectionEnum::InternalCompareTo, Object *pRefThis, Object* pRefTarget)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    const int retIncompatibleMethodTables = 2;  // indicates that the method tables did not match
    const int retInvalidEnumType = 3; // indicates that the enum was of an unknown/unsupported unerlying type

    VALIDATEOBJECTREF(pRefThis);
    
    if (pRefTarget == NULL) {
        return 1; // all values are greater than null
    }

    if( pRefThis == pRefTarget)
        return 0;

    VALIDATEOBJECTREF(pRefTarget);

    //Make sure we are comparing same type.
    MethodTable* pMTThis = pRefThis->GetMethodTable();

    _ASSERTE(pMTThis->IsEnum());  

    if ( pMTThis != pRefTarget->GetMethodTable()) {
        return retIncompatibleMethodTables;   // error case, types incompatible
    }

    void * pThis = pRefThis->UnBox();
    void * pTarget = pRefTarget->UnBox();

    #define CMPEXPR(x1,x2) ((x1) == (x2)) ? 0 : ((x1) < (x2)) ? -1 : 1

    switch (pMTThis->GetInternalCorElementType()) {

    case ELEMENT_TYPE_I1:
        {
            INT8 i1 = *(INT8*)pThis;
            INT8 i2 = *(INT8*)pTarget;

            return CMPEXPR(i1,i2);
        }
        break;

    case ELEMENT_TYPE_I2:
        {
            INT16 i1 = *(INT16*)pThis;
            INT16 i2 = *(INT16*)pTarget;

            return CMPEXPR(i1,i2);
        }
        break;

        
    case ELEMENT_TYPE_I4:
    IN_WIN32(case ELEMENT_TYPE_I:)
        {
            INT32 i1 = *(INT32*)pThis;
            INT32 i2 = *(INT32*)pTarget;

            return CMPEXPR(i1,i2);
        }
        break;
     

    case ELEMENT_TYPE_I8:
    IN_WIN64(case ELEMENT_TYPE_I:)
        {
            INT64 i1 = *(INT64*)pThis;
            INT64 i2 = *(INT64*)pTarget;

            return CMPEXPR(i1,i2);
        }
        break;
    
    case ELEMENT_TYPE_BOOLEAN:
        {
            bool b1 = !!*(UINT8 *)pThis;
            bool b2 = !!*(UINT8 *)pTarget;

            return CMPEXPR(b1,b2);
        }
        break;

    case ELEMENT_TYPE_U1:
        {
            UINT8 u1 = *(UINT8 *)pThis;
            UINT8 u2 = *(UINT8 *)pTarget;

            return CMPEXPR(u1,u2);
        }
        break;
        
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_CHAR:
        {
            UINT16 u1 = *(UINT16 *)pThis;
            UINT16 u2 = *(UINT16 *)pTarget;

            return CMPEXPR(u1,u2);
        }
        break;

    case ELEMENT_TYPE_U4:
    IN_WIN32(case ELEMENT_TYPE_U:)
        {
            UINT32 u1 = *(UINT32 *)pThis;
            UINT32 u2 = *(UINT32 *)pTarget;

            return CMPEXPR(u1,u2);
        }
        break;

    case ELEMENT_TYPE_U8:
    IN_WIN64(case ELEMENT_TYPE_U:)
        {
            UINT64 u1 = *(UINT64*)pThis;
            UINT64 u2 = *(UINT64*)pTarget;

            return CMPEXPR(u1,u2);
        }
        break;

    case ELEMENT_TYPE_R4:
        {
            C_ASSERT(sizeof(float) == 4);

            float f1 = *(float*)pThis;
            float f2 = *(float*)pTarget;

            return CMPEXPR(f1,f2);
        }
        break;
        
    case ELEMENT_TYPE_R8:
        {
            C_ASSERT(sizeof(double) == 8);

            double d1 = *(double*)pThis;
            double d2 = *(double*)pTarget;

            return CMPEXPR(d1,d2);
        }
        break;

    default:
        break;
    }
   
    return retInvalidEnumType; // second error case -- unsupported enum type
}
FCIMPLEND

