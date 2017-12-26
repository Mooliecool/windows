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
#include "namespace.h"

// These system class names are not assembly qualified.

#define g_AppDomainClassName "System.AppDomain"
#define g_ArrayClassName "System.Array"

#define g_CollectionsEnumerableItfName "System.Collections.IEnumerable"
#define g_CollectionsEnumeratorClassName "System.Collections.IEnumerator"


#define g_ContextBoundObjectClassName "System.ContextBoundObject"

#define g_DateClassName     "System.DateTime"
#define g_DecimalClassName "System.Decimal"

#define g_EnumeratorToEnumClassName "System.Runtime.InteropServices.CustomMarshalers.EnumeratorToEnumVariantMarshaler"
#define g_ExceptionClassName "System.Exception"
#define g_ExecutionEngineExceptionClassName "System.ExecutionEngineException"

#define g_LocalDataStoreClassName "System.LocalDataStore"
#define g_LocalDataStoreMgrClassName "System.LocalDataStoreMgr"


#define g_MarshalByRefObjectClassName "System.MarshalByRefObject"

#define g_ThreadStaticAttributeClassName "System.ThreadStaticAttribute"
#define g_ContextStaticAttributeClassName "System.ContextStaticAttribute"
#define g_FrozenStaticAttributeClassName "System.FrozenStaticAttribute"
#define g_StringFreezingAttributeClassName "System.Runtime.CompilerServices.StringFreezingAttribute"

#define g_ObjectClassName "System.Object"
#define g_ObjectName "Object"
#define g_OutOfMemoryExceptionClassName "System.OutOfMemoryException"
#define g_AppDomainUnloadedExceptionClassName "System.AppDomainUnloadedException"

#define g_PermissionTokenFactoryName "System.Security.PermissionTokenFactory"
#define g_PolicyExceptionClassName "System.Security.Policy.PolicyException"

#define g_ReflectionClassName "System.RuntimeType"
#define g_ReflectionConstructorName "System.Reflection.RuntimeConstructorInfo"
#define g_ReflectionEventInfoName "System.Reflection.RuntimeEventInfo"
#define g_ReflectionExpandoItfName "System.Runtime.InteropServices.Expando.IExpando"
#define g_CMExpandoToDispatchExMarshaler "System.Runtime.InteropServices.CustomMarshalers.ExpandoToDispatchExMarshaler"
#define g_CMExpandoViewOfDispatchEx "System.Runtime.InteropServices.CustomMarshalers.ExpandoViewOfDispatchEx"
#define g_ReflectionFieldName "System.Reflection.RuntimeFieldInfo"
#define g_ReflectionMemberInfoName "System.Reflection.MemberInfo"
#define g_MethodBaseName "System.Reflection.RuntimeMethodBase"
#define g_ReflectionFieldInfoName "System.Reflection.FieldInfo"
#define g_ReflectionPropertyInfoName "System.Reflection.PropertyInfo"
#define g_ReflectionConstructorInfoName "System.Reflection.ConstructorInfo"
#define g_ReflectionMethodName "System.Reflection.RuntimeMethodInfo"
#define g_ReflectionModuleName "System.Reflection.Module"
#define g_ReflectionNameValueName "System.Reflection.NameValueName"
#define g_ReflectionParamInfoName "System.Reflection.ParameterInfo"
#define g_ReflectionPermissionClassName "System.Security.Permissions.ReflectionPermission"
#define g_ReflectionPointerClassName "System.Reflection.Pointer"
#define g_ReflectionPropInfoName "System.Reflection.RuntimePropertyInfo"
#define g_ReflectionReflectItfName "System.Reflection.IReflect"
#define g_RuntimeArgumentHandleClassName "System.RuntimeArgumentHandle"
#define g_RuntimeArgumentHandleName      "RuntimeArgumentHandle"
#define g_RuntimeFieldHandleClassName    "System.RuntimeFieldHandle"
#define g_RuntimeFieldHandleName         "RuntimeFieldHandle"
#define g_RuntimeMethodHandleClassName   "System.RuntimeMethodHandle"
#define g_RuntimeMethodHandleName        "RuntimeMethodHandle"
#define g_RuntimeTypeHandleClassName     "System.RuntimeTypeHandle"
#define g_RuntimeTypeHandleName          "RuntimeTypeHandle"

#define g_SecurityPermissionClassName "System.Security.Permissions.SecurityPermission"
#define g_StackOverflowExceptionClassName "System.StackOverflowException"
#define g_StringBufferClassName "System.Text.StringBuilder"
#define g_StringBufferName "StringBuilder"
#define g_StringClassName "System.String"
#define g_StringName "String"
#define g_SharedStaticsClassName "System.SharedStatics"

#define g_ThreadClassName "System.Threading.Thread"
#define g_TypeClassName   "System.Type"

#define g_VariantClassName "System.Variant"
#define g_GuidClassName "System.Guid"

#define g_CompilerServicesFixedAddressValueTypeAttribute "System.Runtime.CompilerServices.FixedAddressValueTypeAttribute"
#define g_CompilerServicesUnsafeValueTypeAttribute "System.Runtime.CompilerServices.UnsafeValueTypeAttribute"

#define g_CompilerServicesTypeDependencyAttribute "System.Runtime.CompilerServices.TypeDependencyAttribute"
#define g_AllowPartiallyTrustedCallersAttribute "System.Security.AllowPartiallyTrustedCallersAttribute"
#define g_ProxyAttribute "System.Runtime.Remoting.Proxies.ProxyAttribute"

#define g_SecurityCriticalAttribute "System.Security.SecurityCriticalAttribute"
#define g_SecurityTransparentAttribute "System.Security.SecurityTransparentAttribute"
#define g_SecurityTreatAsSafeAttribute "System.Security.SecurityTreatAsSafeAttribute"
#define g_SecurityAPTCA "System.Security.AllowPartiallyTrustedCallersAttribute"
