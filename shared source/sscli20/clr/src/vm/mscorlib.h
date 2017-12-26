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
// This file contains the classes, methods, and field used by the EE from mscorlib

//
// To use this, define one of the following macros & include the file like so:
//
// #define DEFINE_CLASS(id, nameSpace, stringName)         CLASS__ ## id,
// #define DEFINE_METHOD(classId, id, stringName, gSign)
// #define DEFINE_FIELD(classId, id, stringName)
// #include "mscorlib.h"
//

#ifndef DEFINE_CLASS
#define DEFINE_CLASS(id, nameSpace, stringName)
#endif

#ifndef DEFINE_METHOD
#define DEFINE_METHOD(classId, id, stringName, gSign)
#endif

#ifndef DEFINE_FIELD
#define DEFINE_FIELD(classId, id, stringName)
#endif

#ifndef DEFINE_CLASS_U
#define DEFINE_CLASS_U(id, nameSpace, stringName, unmanagedType)        DEFINE_CLASS(id, nameSpace, stringName)
#endif

#ifndef DEFINE_FIELD_U
#define DEFINE_FIELD_U(classId, id, stringName, unmanagedContainingType, unmanagedOffset)
#endif

#ifndef DEFINE_PROPERTY
#define DEFINE_PROPERTY(classId, id, stringName, gSign) DEFINE_METHOD(classId, GET_ ## id, get_ ## stringName, IM_Ret ## gSign)
#endif

#ifndef DEFINE_STATIC_PROPERTY
#define DEFINE_STATIC_PROPERTY(classId, id, stringName, gSign) DEFINE_METHOD(classId, GET_ ## id, get_ ## stringName, SM_Ret ## gSign)
#endif

#ifndef DEFINE_SET_PROPERTY
#define DEFINE_SET_PROPERTY(classId, id, stringName, gSign) \
    DEFINE_PROPERTY(classId, id, stringName, gSign) \
    DEFINE_METHOD(classId, SET_ ## id, set_ ## stringName, IM_## gSign ## _RetVoid)
#endif

// NOTE: Make this window really wide if you want to read the table...

DEFINE_CLASS(ACTIVATOR,             System,                 Activator)

DEFINE_CLASS(ACCESS_VIOLATION_EXCEPTION, System,            AccessViolationException)
DEFINE_FIELD(ACCESS_VIOLATION_EXCEPTION, IP,                _ip)
DEFINE_FIELD(ACCESS_VIOLATION_EXCEPTION, TARGET,            _target)
DEFINE_FIELD(ACCESS_VIOLATION_EXCEPTION, ACCESSTYPE,        _accessType)

DEFINE_CLASS_U(APP_DOMAIN,          System,                 AppDomain,      AppDomainBaseObject)
DEFINE_FIELD_U(APP_DOMAIN,          DOMAIN_MANAGER,         _domainManager,             AppDomainBaseObject, m_pDomainManager)
DEFINE_FIELD_U(APP_DOMAIN,          LOCAL_STORE,            _LocalStore,                AppDomainBaseObject, m_LocalStore)
DEFINE_FIELD_U(APP_DOMAIN,          FUSION_STORE,           _FusionStore,               AppDomainBaseObject, m_FusionTable)
DEFINE_FIELD_U(APP_DOMAIN,          SECURITY_IDENTITY,      _SecurityIdentity,          AppDomainBaseObject, m_pSecurityIdentity)
DEFINE_FIELD_U(APP_DOMAIN,          POLICIES,               _Policies,                  AppDomainBaseObject, m_pPolicies)
DEFINE_FIELD_U(APP_DOMAIN,          ASSEMBLY_LOAD,          AssemblyLoad,               AppDomainBaseObject, m_pAssemblyEventHandler)
DEFINE_FIELD_U(APP_DOMAIN,          TYPE_RESOLVE,           TypeResolve,                AppDomainBaseObject, m_pTypeEventHandler)
DEFINE_FIELD_U(APP_DOMAIN,          RESOURCE_RESOLVE,       ResourceResolve,            AppDomainBaseObject, m_pResourceEventHandler)
DEFINE_FIELD_U(APP_DOMAIN,          ASSEMBLY_RESOLVE,       AssemblyResolve,            AppDomainBaseObject, m_pAsmResolveEventHandler)
DEFINE_FIELD_U(APP_DOMAIN,          REFLECTION_ASSEMBLY_RESOLVE, ReflectionOnlyAssemblyResolve, AppDomainBaseObject, m_pReflectionAsmResolveEventHandler)
DEFINE_FIELD_U(APP_DOMAIN,          DEFAULT_CONTEXT,        _DefaultContext,            AppDomainBaseObject, m_pDefaultContext)
DEFINE_FIELD_U(APP_DOMAIN,          DEFAULT_PRINCIPAL,      _DefaultPrincipal,          AppDomainBaseObject, m_pDefaultPrincipal)
DEFINE_FIELD_U(APP_DOMAIN,          REMOTING_DATA,          _RemotingData,              AppDomainBaseObject, m_pURITable)
DEFINE_FIELD_U(APP_DOMAIN,          PROCESS_EXIT,           _processExit,               AppDomainBaseObject, m_pProcessExitEventHandler)
DEFINE_FIELD_U(APP_DOMAIN,          DOMAIN_UNLOAD,          _domainUnload,              AppDomainBaseObject, m_pDomainUnloadEventHandler)
DEFINE_FIELD_U(APP_DOMAIN,          UNHANDLED_EXCEPTION,    _unhandledException,        AppDomainBaseObject, m_pUnhandledExceptionEventHandler)
DEFINE_FIELD_U(APP_DOMAIN,          DUMMY_FIELD,            _dummyField,                AppDomainBaseObject, m_pDomain)
DEFINE_FIELD_U(APP_DOMAIN,          PRINCIPAL_POLICY,       _PrincipalPolicy,           AppDomainBaseObject, m_iPrincipalPolicy)
DEFINE_FIELD_U(APP_DOMAIN,          HAS_SET_POLICY,         _HasSetPolicy,              AppDomainBaseObject, m_bHasSetPolicy)
DEFINE_METHOD(APP_DOMAIN,           REMOTELY_SETUP_REMOTE_DOMAIN,RemotelySetupRemoteDomain,SM_AppDomain_Str_AppDomainSetup_Evidence_Evidence_IntPtr_RetObj)
DEFINE_METHOD(APP_DOMAIN,           ON_ASSEMBLY_LOAD,       OnAssemblyLoadEvent,        IM_Assembly_RetVoid)
DEFINE_METHOD(APP_DOMAIN,           ON_RESOURCE_RESOLVE,    OnResourceResolveEvent,     IM_Str_RetAssembly)
DEFINE_METHOD(APP_DOMAIN,           ON_TYPE_RESOLVE,        OnTypeResolveEvent,         IM_Str_RetAssembly)
DEFINE_METHOD(APP_DOMAIN,           ON_ASSEMBLY_RESOLVE,    OnAssemblyResolveEvent,     IM_Str_RetAssembly)
DEFINE_METHOD(APP_DOMAIN,           ON_REFLECTION_ONLY_ASSEMBLY_RESOLVE, OnReflectionOnlyAssemblyResolveEvent, IM_Str_RetAssembly) 
DEFINE_METHOD(APP_DOMAIN,           ENABLE_RESOLVE_ASSEMBLIES_FOR_INTROSPECTION, EnableResolveAssembliesForIntrospection, IM_RetVoid)
DEFINE_METHOD(APP_DOMAIN,           GET_DATA,               GetData,                    IM_Str_RetObj)
DEFINE_METHOD(APP_DOMAIN,           SET_DATA,               SetData,                    IM_Str_Obj_RetVoid)
DEFINE_METHOD(APP_DOMAIN,           SETUP_DOMAIN,           SetupDomain,                IM_Bool_Str_Str_RetVoid)
DEFINE_METHOD(APP_DOMAIN,           SETUP_LOADER_OPTIMIZATION,SetupLoaderOptimization,  IM_LoaderOptimization_RetVoid)
DEFINE_METHOD(APP_DOMAIN,           CREATE_DOMAIN,          CreateDomain,               SM_Str_Evidence_AppDomainSetup_RetAppDomain)
DEFINE_METHOD(APP_DOMAIN,           CREATE_DOMAINEX,        CreateDomain,               SM_Str_Evidence_Str_Str_Bool_RetAppDomain)
DEFINE_METHOD(APP_DOMAIN,           VAL_CREATE_DOMAIN,      InternalCreateDomain,       SM_Str_RetAppDomain)
DEFINE_METHOD(APP_DOMAIN,           SET_DOMAIN_CONTEXT,     InternalSetDomainContext,       IM_Str_RetVoid)
DEFINE_METHOD(APP_DOMAIN,           UNLOAD,                 Unload,                     SM_AppDomain_RetVoid)
DEFINE_METHOD(APP_DOMAIN,           MARSHAL_OBJECT,         MarshalObject,              SM_Obj_RetArrByte)
DEFINE_METHOD(APP_DOMAIN,           MARSHAL_OBJECTS,        MarshalObjects,             SM_Obj_Obj_RefArrByte_RetArrByte)
DEFINE_METHOD(APP_DOMAIN,           UNMARSHAL_OBJECT,       UnmarshalObject,            SM_ArrByte_RetObj)
DEFINE_METHOD(APP_DOMAIN,           UNMARSHAL_OBJECTS,      UnmarshalObjects,           SM_ArrByte_ArrByte_RefObj_RetObj)
DEFINE_METHOD(APP_DOMAIN,           TURN_ON_BINDING_REDIRECTS, TurnOnBindingRedirects,     IM_RetVoid)
DEFINE_METHOD(APP_DOMAIN,           SET_DEFAULT_DOMAIN_MANAGER, SetDefaultDomainManager, IM_Str_ArrStr_ArrStr_RetVoid)
DEFINE_METHOD(APP_DOMAIN,           ACTIVATE_APPLICATION,   ActivateApplication,        IM_RetInt)

DEFINE_CLASS(APPDOMAIN_SETUP,       System,                 AppDomainSetup)
DEFINE_CLASS(ARGUMENT_HANDLE,       System,                 RuntimeArgumentHandle)

DEFINE_CLASS(ARRAY,                 System,                 Array)
DEFINE_PROPERTY(ARRAY,              LENGTH,                 Length,                     Int)
DEFINE_METHOD(ARRAY,                GET_DATA_PTR_OFFSET_INTERNAL, GetDataPtrOffsetInternal, IM_RetInt)

DEFINE_CLASS(ARRAY_LIST,            Collections,            ArrayList)
DEFINE_METHOD(ARRAY_LIST,           CTOR,                   .ctor,                      IM_RetVoid)
DEFINE_METHOD(ARRAY_LIST,           ADD,                    Add,                        IM_Obj_RetInt)

DEFINE_CLASS(ARRAY_WITH_OFFSET,     Interop,                ArrayWithOffset)                 
DEFINE_FIELD(ARRAY_WITH_OFFSET,     M_ARRAY,                m_array)
DEFINE_FIELD(ARRAY_WITH_OFFSET,     M_OFFSET,               m_offset)
DEFINE_FIELD(ARRAY_WITH_OFFSET,     M_COUNT,                m_count)


DEFINE_CLASS(ASSEMBLY_BUILDER,      ReflectionEmit,         AssemblyBuilder)

DEFINE_CLASS(ASSEMBLY_HASH_ALGORITHM, Assemblies,           AssemblyHashAlgorithm)

DEFINE_CLASS_U(ASSEMBLY_NAME,       Reflection,             AssemblyName,           AssemblyNameBaseObject)
DEFINE_FIELD_U(ASSEMBLY_NAME,       NAME,                   _Name,                      AssemblyNameBaseObject, m_pSimpleName)
DEFINE_FIELD_U(ASSEMBLY_NAME,       PUBLICKEY,              _PublicKey,                 AssemblyNameBaseObject, m_pPublicKey)
DEFINE_FIELD_U(ASSEMBLY_NAME,       PUBLICKEYTOKEN,         _PublicKeyToken,            AssemblyNameBaseObject, m_pPublicKeyToken)
DEFINE_FIELD_U(ASSEMBLY_NAME,       CULTURE_INFO,           _CultureInfo,               AssemblyNameBaseObject, m_pCultureInfo)
DEFINE_FIELD_U(ASSEMBLY_NAME,       CODE_BASE,              _CodeBase,                  AssemblyNameBaseObject, m_pCodeBase)
DEFINE_FIELD_U(ASSEMBLY_NAME,       VERSION,                _Version,                   AssemblyNameBaseObject, m_pVersion)
DEFINE_FIELD_U(ASSEMBLY_NAME,       SERIALIZATION_INFO,     m_siInfo,                   AssemblyNameBaseObject, m_siInfo)
DEFINE_FIELD_U(ASSEMBLY_NAME,       HASH_FOR_CONTROL,       _HashForControl,            AssemblyNameBaseObject, m_HashForControl)
DEFINE_FIELD_U(ASSEMBLY_NAME,       HASH_ALGORITHM,         _HashAlgorithm,             AssemblyNameBaseObject, m_HashAlgorithm)
DEFINE_FIELD_U(ASSEMBLY_NAME,       HASH_ALGORITHM_FOR_CONTROL, _HashAlgorithmForControl, AssemblyNameBaseObject, m_HashAlgorithmForControl)
DEFINE_FIELD_U(ASSEMBLY_NAME,       VERSION_COMPATIBILITY,  _VersionCompatibility,      AssemblyNameBaseObject, m_VersionCompatibility)
DEFINE_FIELD_U(ASSEMBLY_NAME,       FLAGS,                  _Flags,                     AssemblyNameBaseObject, m_Flags)
DEFINE_METHOD(ASSEMBLY_NAME,        INIT,                   Init,                      IM_Str_ArrB_ArrB_Ver_CI_AHA_AVC_Str_ANF_SNKP_RetV)

DEFINE_CLASS(ASSEMBLY_VERSION_COMPATIBILITY, Assemblies,    AssemblyVersionCompatibility)

DEFINE_CLASS(ASSEMBLY_NAME_FLAGS,   Reflection,             AssemblyNameFlags)

DEFINE_CLASS_U(ASSEMBLY,              Reflection,           Assembly,           AssemblyBaseObject)
DEFINE_FIELD_U(ASSEMBLY,            DATA,                   m_assemblyData,             AssemblyBaseObject,       m_pAssemblyBuilderData)
DEFINE_FIELD_U(ASSEMBLY,            MODULE_RESOLVE,         ModuleResolve,              AssemblyBaseObject,       m_pModuleEventHandler)
DEFINE_FIELD_U(ASSEMBLY,            CACHE,                  m_cachedData,               AssemblyBaseObject,       m_cache)
DEFINE_FIELD_U(ASSEMBLY,            FIELD,                  m_assembly,                 AssemblyBaseObject,       m_pAssembly)
DEFINE_METHOD(ASSEMBLY,             GET_NAME,               GetName,                    IM_RetAssemblyName)
DEFINE_METHOD(ASSEMBLY,             LOAD_WITH_PARTIAL_NAME_HACK,  LoadWithPartialNameHack, SM_Str_Bool_RetIntPtr)
DEFINE_METHOD(ASSEMBLY,             ON_MODULE_RESOLVE,      OnModuleResolveEvent,       IM_Str_RetModule)
DEFINE_METHOD(ASSEMBLY,             CREATE_SECURITY_IDENTITY,CreateSecurityIdentity,    SM_ASM_Str_Int_ArrByte_ArrByte_Str_Int_Int_Int_Int_ArrByte_Evidence_RetEvidence)
DEFINE_METHOD(ASSEMBLY,             DEMAND_PERMISSION,      DemandPermission,           SM_Str_Bool_Int_RetV)


DEFINE_CLASS(ACTIVATION_SERVICES,   Activation,             ActivationServices)
DEFINE_METHOD(ACTIVATION_SERVICES,  IS_CURRENT_CONTEXT_OK,  IsCurrentContextOK,         SM_Type_ArrObject_Bool_RetMarshalByRefObject)


DEFINE_CLASS(ASYNCCALLBACK,         System,                 AsyncCallback)

DEFINE_CLASS(BINDER,                Reflection,             Binder)
DEFINE_METHOD(BINDER,               CHANGE_TYPE,            ChangeType,                 IM_Obj_Type_CultureInfo_RetObj)

DEFINE_CLASS(BINDING_FLAGS,         Reflection,             BindingFlags)

DEFINE_CLASS(BOOLEAN,               System,                 Boolean)

DEFINE_CLASS(BYTE,                  System,                 Byte)


DEFINE_CLASS(CHAR,                  System,                 Char)

DEFINE_CLASS_U(CLASS,               System,                 RuntimeType,                ReflectClassBaseObject)
DEFINE_FIELD_U(CLASS,               CACHE,                  m_cache,                ReflectClassBaseObject,        m_cache)
DEFINE_FIELD_U(CLASS,               TYPEHANDLE,             m_handle,               ReflectClassBaseObject,        m_typeHandle)

DEFINE_METHOD(CLASS,                GET_PROPERTIES,         GetProperties,              IM_BindingFlags_RetArrPropertyInfo)
DEFINE_METHOD(CLASS,                GET_FIELDS,             GetFields,                  IM_BindingFlags_RetArrFieldInfo)
DEFINE_METHOD(CLASS,                GET_METHODS,            GetMethods,                 IM_BindingFlags_RetArrMethodInfo)
DEFINE_METHOD(CLASS,                INVOKE_MEMBER,          InvokeMember,               IM_Str_BindingFlags_Binder_Obj_ArrObj_ArrParameterModifier_CultureInfo_ArrStr_RetObj)
DEFINE_METHOD(CLASS,                GET_METHOD_BASE,        GetMethodBase,              SM_RuntimeTypeHandle_RuntimeMethodHandle_RetMethodBase)
DEFINE_METHOD(CLASS,                GET_FIELD_INFO,         GetFieldInfo,               SM_RuntimeTypeHandle_RuntimeFieldHandle_RetFieldInfo)
DEFINE_METHOD(CLASS,                GET_PROPERTY_INFO,      GetPropertyInfo,            SM_RuntimeTypeHandle_Int_RetPropertyInfo)

DEFINE_CLASS(CLASS_INTROSPECTION_ONLY, System,              ReflectionOnlyType)

DEFINE_CLASS(CODE_ACCESS_PERMISSION, Security,              CodeAccessPermission)


DEFINE_CLASS_U(CRITICAL_HANDLE,     Interop,                CriticalHandle,             CriticalHandle)
#ifdef _DEBUG
DEFINE_FIELD_U(CRITICAL_HANDLE,     DEBUG_STACK_TRACE,      _stackTrace,                CriticalHandle,     m_debugStackTrace)
#endif
DEFINE_FIELD(CRITICAL_HANDLE,       HANDLE,                 handle)
DEFINE_FIELD_U(CRITICAL_HANDLE,     IS_CLOSED,              _isClosed,                  CriticalHandle,     m_isClosed)
DEFINE_METHOD(CRITICAL_HANDLE,      RELEASE_HANDLE,         ReleaseHandle,              IM_RetBool)
DEFINE_METHOD(CRITICAL_HANDLE,      GET_IS_INVALID,         get_IsInvalid,              IM_RetBool)
DEFINE_METHOD(CRITICAL_HANDLE,      DISPOSE,                Dispose,                    IM_RetVoid)
DEFINE_METHOD(CRITICAL_HANDLE,      DISPOSE_BOOL,           Dispose,                    IM_Bool_RetVoid)

DEFINE_CLASS(CRITICAL_FINALIZER_OBJECT, ConstrainedExecution, CriticalFinalizerObject)
DEFINE_METHOD(CRITICAL_FINALIZER_OBJECT, FINALIZE,          Finalize,                   IM_RetVoid)

DEFINE_CLASS(CONSTRUCTOR,           Reflection,             RuntimeConstructorInfo)

DEFINE_CLASS(CONSTRUCTOR_INFO,      Reflection,             ConstructorInfo)

DEFINE_CLASS_U(CUSTOM_ATTRIBUTE_ENCODED_ARGUMENT, Reflection, CustomAttributeEncodedArgument, CustomAttributeValue)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_ENCODED_ARGUMENT,   PRIMITIVE_VALUE,    m_primitiveValue,   CustomAttributeValue,           m_rawValue)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_ENCODED_ARGUMENT,   ARRAY_VALUE,        m_arrayValue,       CustomAttributeValue,           m_value)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_ENCODED_ARGUMENT,   STRING_VALUE,       m_stringValue,      CustomAttributeValue,           m_enumOrTypeName)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_ENCODED_ARGUMENT,   TYPE,               m_type,             CustomAttributeValue,           m_type)

DEFINE_CLASS_U(CUSTOM_ATTRIBUTE_NAMED_PARAMETER, Reflection, CustomAttributeNamedParameter, CustomAttributeNamedArgument)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_NAMED_PARAMETER,    PRIMITIVE_VALUE,    m_argumentName,     CustomAttributeNamedArgument,   m_argumentName)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_NAMED_PARAMETER,    FIELD_OR_PROP,      m_fieldOrProperty,  CustomAttributeNamedArgument,   m_propertyOrField)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_NAMED_PARAMETER,    PADDING,            m_padding,          CustomAttributeNamedArgument,   m_padding)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_NAMED_PARAMETER,    TYPE,               m_type,             CustomAttributeNamedArgument,   m_type)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_NAMED_PARAMETER,    ENCODED_ARG,        m_encodedArgument,  CustomAttributeNamedArgument,   m_value)

DEFINE_CLASS_U(CUSTOM_ATTRIBUTE_CTOR_PARAMETER, Reflection, CustomAttributeCtorParameter, CustomAttributeArgument)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_CTOR_PARAMETER,     TYPE,               m_type,             CustomAttributeArgument,        m_type)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_CTOR_PARAMETER,     ENCODED_ARG,        m_encodedArgument,  CustomAttributeArgument,        m_value)

DEFINE_CLASS_U(CUSTOM_ATTRIBUTE_TYPE, Reflection, CustomAttributeType, CustomAttributeType)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_TYPE,               ENCODED_ARG,        m_enumName,         CustomAttributeType,            m_enumName)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_TYPE,               ENCODED_TYPE,       m_encodedType,      CustomAttributeType,            m_tag)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_TYPE,               ENCODED_ENUM_TYPE,  m_encodedEnumType,  CustomAttributeType,            m_enumType)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_TYPE,               ENCODED_ARRAY_TYPE, m_encodedArrayType, CustomAttributeType,            m_arrayType)
DEFINE_FIELD_U(CUSTOM_ATTRIBUTE_TYPE,               PADDING,            m_padding,          CustomAttributeType,            m_padding)

DEFINE_CLASS_U(CONTEXT,             Contexts,               Context,        ContextBaseObject)
DEFINE_FIELD_U(CONTEXT,             PROPS,                  _ctxProps,                  ContextBaseObject, m_ctxProps)
DEFINE_FIELD_U(CONTEXT,             DPH,                    _dphCtx,                    ContextBaseObject, m_dphCtx)
DEFINE_FIELD_U(CONTEXT,             LOCAL_DATA_STORE,       _localDataStore,            ContextBaseObject, m_localDataStore)
DEFINE_FIELD_U(CONTEXT,             SERVER_CONTEXT_CHAIN,   _serverContextChain,        ContextBaseObject, m_serverContextChain)
DEFINE_FIELD_U(CONTEXT,             CLIENT_CONTEXT_CHAIN,   _clientContextChain,        ContextBaseObject, m_clientContextChain)
DEFINE_FIELD_U(CONTEXT,             APP_DOMAIN,             _appDomain,                 ContextBaseObject, m_exposedAppDomain)
DEFINE_FIELD_U(CONTEXT,             CONTEXT_STATICS,        _ctxStatics,                ContextBaseObject, m_ctxStatics)
DEFINE_FIELD_U(CONTEXT,             INTERNAL_CONTEXT,       _internalContext,           ContextBaseObject, m_internalContext)
DEFINE_FIELD_U(CONTEXT,             ID,                     _ctxID,                     ContextBaseObject, _ctxID)
DEFINE_FIELD_U(CONTEXT,             FLAGS,                  _ctxFlags,                  ContextBaseObject, _ctxFlags)
DEFINE_FIELD_U(CONTEXT,             NUM_CONTEXT_PROPS,      _numCtxProps,               ContextBaseObject, _numCtxProps)
DEFINE_FIELD_U(CONTEXT,             STATICS_BUCKET,         _ctxStaticsCurrentBucket,   ContextBaseObject, _ctxStaticsCurrentBucket)
DEFINE_FIELD_U(CONTEXT,             STATICS_FREE_INDEX,     _ctxStaticsFreeIndex,       ContextBaseObject, _ctxStaticsFreeIndex)
DEFINE_METHOD(CONTEXT,              CALLBACK,               DoCallBackFromEE,           SM_IntPtr_IntPtr_Int_RetVoid)
DEFINE_METHOD(CONTEXT,              RESERVE_SLOT,           ReserveSlot,                IM_RetInt)

DEFINE_CLASS(CONTEXT_BOUND_OBJECT,  System,                 ContextBoundObject)


DEFINE_CLASS(TEXT_INFO,             Globalization,          TextInfo)

DEFINE_CLASS(CHAR_UNICODE_INFO,     Globalization,          CharUnicodeInfo)

DEFINE_CLASS_U(CULTURE_INFO,        Globalization,          CultureInfo,        CultureInfoBaseObject)
DEFINE_METHOD(CULTURE_INFO,         STR_CTOR,               .ctor,                      IM_Str_RetVoid)
DEFINE_METHOD(CULTURE_INFO,         INT_CTOR,               .ctor,                      IM_Int_RetVoid)
DEFINE_FIELD(CULTURE_INFO,          CURRENT_CULTURE,        m_userDefaultCulture)
DEFINE_FIELD_U(CULTURE_INFO,        COMPARE_INFO,           compareInfo,        CultureInfoBaseObject,  compareInfo)
DEFINE_FIELD_U(CULTURE_INFO,        TEXT_INFO,              textInfo,           CultureInfoBaseObject,  textInfo)
DEFINE_FIELD_U(CULTURE_INFO,        NUM_INFO,               numInfo,            CultureInfoBaseObject,  numInfo)
DEFINE_FIELD_U(CULTURE_INFO,        DATE_TIME_INFO,         dateTimeInfo,       CultureInfoBaseObject,  dateTimeInfo)
DEFINE_FIELD_U(CULTURE_INFO,        CALENDAR,               calendar,           CultureInfoBaseObject,  calendar)
DEFINE_FIELD_U(CULTURE_INFO,        CULTURE_TABLE_RECORD,   m_cultureTableRecord, CultureInfoBaseObject, m_cultureTableRecord)
DEFINE_FIELD_U(CULTURE_INFO,        CONSOLE_FALLBACK_CULTURE, m_consoleFallbackCulture, CultureInfoBaseObject, m_consoleFallbackCulture)
DEFINE_FIELD_U(CULTURE_INFO,        NAME,                   m_name,             CultureInfoBaseObject,  m_name)
DEFINE_FIELD_U(CULTURE_INFO,        NON_SORT_NAME,          m_nonSortName,      CultureInfoBaseObject,  m_nonSortName)
DEFINE_FIELD_U(CULTURE_INFO,        SORT_NAME,              m_sortName,         CultureInfoBaseObject,  m_sortName)
DEFINE_FIELD_U(CULTURE_INFO,        IETF_NAME,              m_ietfName,         CultureInfoBaseObject,  m_ietfName)
DEFINE_FIELD_U(CULTURE_INFO,        PARENT,                 m_parent,           CultureInfoBaseObject,  m_parent)
DEFINE_FIELD_U(CULTURE_INFO,        CULTURE_ID,             cultureID,          CultureInfoBaseObject,  cultureID)
DEFINE_FIELD_U(CULTURE_INFO,        CREATED_DOMAIN,         m_createdDomainID,  CultureInfoBaseObject,  m_createdDomainID)
DEFINE_FIELD_U(CULTURE_INFO,        DATA_ITEM,              m_dataItem,         CultureInfoBaseObject,  m_dataItem)
DEFINE_FIELD_U(CULTURE_INFO,        IS_READ_ONLY,           m_isReadOnly,       CultureInfoBaseObject,  m_isReadOnly)
DEFINE_FIELD_U(CULTURE_INFO,        IS_INHERITED,           m_isInherited,      CultureInfoBaseObject,  m_isInherited)
DEFINE_FIELD_U(CULTURE_INFO,        IS_SAFE_CROSSDOMAIN,    m_isSafeCrossDomain, CultureInfoBaseObject, m_isSafeCrossDomain)
DEFINE_FIELD_U(CULTURE_INFO,        USE_USER_OVERRIDE,      m_useUserOverride,  CultureInfoBaseObject,  m_useUserOverride)
DEFINE_PROPERTY(CULTURE_INFO,       NAME,                   Name,                       Str)
DEFINE_PROPERTY(CULTURE_INFO,       ID,                     LCID,                       Int)
DEFINE_PROPERTY(CULTURE_INFO,       PARENT,                 Parent,                     CultureInfo)

DEFINE_CLASS(CURRENCY,              System,                 Currency)


DEFINE_CLASS(DATE_TIME,             System,                 DateTime)
DEFINE_METHOD(DATE_TIME,            TO_FILE_TIME,           ToFileTime,                 IM_RetLong)

DEFINE_CLASS(DECIMAL,               System,                 Decimal)      

// Note: DelegateObject is used for both Delegate and MulticastDelegate.
DEFINE_CLASS(DELEGATE,              System,                 Delegate)
DEFINE_FIELD_U(DELEGATE,            TARGET,                 _target,                    DelegateObject,   _target)
DEFINE_FIELD_U(DELEGATE,            METHOD_PTR,             _methodPtr,                 DelegateObject,   _methodPtr)
DEFINE_FIELD_U(DELEGATE,            METHOD_PTR_AUX,         _methodPtrAux,              DelegateObject,   _methodPtrAux)
DEFINE_METHOD(DELEGATE,             CONSTRUCT_DELEGATE,     DelegateConstruct,          IM_Obj_IntPtr_RetVoid)


DEFINE_CLASS(DOUBLE,                System,                 Double)

DEFINE_CLASS(DYNAMICRESOLVER,       ReflectionEmit,         DynamicResolver)
DEFINE_FIELD(DYNAMICRESOLVER,       SCOPE,                  m_scope)
DEFINE_FIELD(DYNAMICRESOLVER,       DYNAMIC_METHOD,         m_method)

DEFINE_CLASS(DYNAMICSCOPE,          ReflectionEmit,         DynamicScope)
DEFINE_FIELD(DYNAMICSCOPE,          TOKENS,                 m_tokens)

DEFINE_CLASS(EMPTY,                 System,                 Empty)
DEFINE_FIELD(EMPTY,                 VALUE,          Value)


DEFINE_CLASS(ENCODING,              Text,                   Encoding)

DEFINE_CLASS(ENUM,                  System,                 Enum)

DEFINE_CLASS(ENVIRONMENT,           System,                 Environment)
DEFINE_METHOD(ENVIRONMENT,       GET_RESOURCE_STRING_LOCAL, GetResourceStringLocal,     SM_Str_RetStr)

DEFINE_CLASS(ENVIRONMENT_PERMISSION,Permissions,            EnvironmentPermission)


DEFINE_CLASS(EVENT,                 Reflection,             RuntimeEventInfo)

DEFINE_CLASS(EVENT_ARGS,            System,                 EventArgs)

DEFINE_CLASS(EVENT_HANDLER,         System,                 EventHandler)
DEFINE_METHOD(EVENT_HANDLER,        INVOKE,                 Invoke,                     IM_Obj_EventArgs_RetVoid)

DEFINE_CLASS(EVENT_INFO,            Reflection,             EventInfo)

DEFINE_CLASS(EVIDENCE,              Policy,                 Evidence)

DEFINE_CLASS_U(EXCEPTION,           System,                 Exception,      ExceptionObject)
DEFINE_METHOD(EXCEPTION,            GET_CLASS_NAME,         GetClassName,               IM_RetStr)
DEFINE_PROPERTY(EXCEPTION,          MESSAGE,                Message,                    Str)
DEFINE_PROPERTY(EXCEPTION,          SOURCE,                 Source,                     Str)
DEFINE_PROPERTY(EXCEPTION,          HELP_LINK,              HelpLink,                   Str)
DEFINE_FIELD_U(EXCEPTION,           CLASS_NAME,             _className,         ExceptionObject,    _className)
DEFINE_FIELD_U(EXCEPTION,           EXCEPTION_METHOD,       _exceptionMethod,   ExceptionObject,    _exceptionMethod)
DEFINE_FIELD_U(EXCEPTION,           EXCEPTION_METHOD_STRING,_exceptionMethodString,ExceptionObject, _exceptionMethodString)
DEFINE_FIELD_U(EXCEPTION,           MESSAGE,                _message,           ExceptionObject,    _message)
DEFINE_FIELD_U(EXCEPTION,           DATA,                   _data,              ExceptionObject,    _data)
DEFINE_FIELD_U(EXCEPTION,           INNER_EXCEPTION,        _innerException,    ExceptionObject,    _innerException)
DEFINE_FIELD_U(EXCEPTION,           HELP_URL,               _helpURL,           ExceptionObject,    _helpURL)
DEFINE_FIELD_U(EXCEPTION,           SOURCE,                 _source,            ExceptionObject,    _source)
DEFINE_FIELD_U(EXCEPTION,           STACK_TRACE,            _stackTrace,        ExceptionObject,    _stackTrace)
DEFINE_FIELD_U(EXCEPTION,           STACK_TRACE_STRING,     _stackTraceString,  ExceptionObject,    _stackTraceString)
DEFINE_FIELD_U(EXCEPTION,           REMOTE_STACK_TRACE_STRING, _remoteStackTraceString, ExceptionObject, _remoteStackTraceString)
DEFINE_FIELD_U(EXCEPTION,           DYNAMIC_METHODS,        _dynamicMethods,    ExceptionObject,    _dynamicMethods)
DEFINE_FIELD_U(EXCEPTION,           XPTRS,                  _xptrs,             ExceptionObject,    _xptrs)
DEFINE_FIELD_U(EXCEPTION,           HRESULT,                _HResult,           ExceptionObject,    _HResult)
DEFINE_FIELD_U(EXCEPTION,           XCODE,                  _xcode,             ExceptionObject,    _xcode)
DEFINE_FIELD_U(EXCEPTION,           REMOTE_STACK_INDEX,     _remoteStackIndex,  ExceptionObject,    _remoteStackIndex)
DEFINE_METHOD(EXCEPTION,            INTERNAL_TO_STRING,     InternalToString,           IM_RetStr)
DEFINE_METHOD(EXCEPTION,            INTERNAL_PRESERVE_STACK_TRACE, InternalPreserveStackTrace, IM_RetVoid)

DEFINE_CLASS(SYSTEM_EXCEPTION,      System,                 SystemException)
DEFINE_METHOD(SYSTEM_EXCEPTION,     STR_EX_CTOR,            .ctor,                      IM_Str_Exception_RetVoid)

DEFINE_CLASS(TYPE_INIT_EXCEPTION,   System,                 TypeInitializationException)
DEFINE_METHOD(TYPE_INIT_EXCEPTION,  STR_EX_CTOR,            .ctor,                      IM_Str_Exception_RetVoid)

DEFINE_CLASS(THREAD_START_EXCEPTION,Threading,              ThreadStartException)
DEFINE_METHOD(THREAD_START_EXCEPTION,EX_CTOR,               .ctor,                      IM_Exception_RetVoid)

DEFINE_CLASS(RT_TYPE_HANDLE,        System,                 RuntimeTypeHandle)
DEFINE_METHOD(RT_TYPE_HANDLE,       GET_TYPE_HELPER,        GetTypeHelper,              SM_IntPtr_IntPtr_int_IntPtr_int_RetIntPtr)
DEFINE_METHOD(RT_TYPE_HANDLE,       PVOID_CTOR,             .ctor,                      IM_VoidPtr_RetVoid)

DEFINE_CLASS(RT_FIELD_INFO,         Reflection,             RtFieldInfo)
DEFINE_CLASS(FIELD,                 Reflection,             RuntimeFieldInfo)
DEFINE_METHOD(FIELD,                SET_VALUE,              SetValue,                   IM_Obj_Obj_BindingFlags_Binder_CultureInfo_RetVoid)
DEFINE_METHOD(FIELD,                GET_VALUE,              GetValue,                   IM_Obj_RetObj)

DEFINE_CLASS(FIELD_HANDLE,          System,                 RuntimeFieldHandle)

DEFINE_CLASS(FIELD_INFO,            Reflection,             FieldInfo)

DEFINE_CLASS(FILEDIALOG_PERMISSION, Permissions,            FileDialogPermission)
DEFINE_CLASS(FILEIO_PERMISSION,     Permissions,            FileIOPermission)

DEFINE_CLASS_U(FILESTREAM_ASYNCRESULT, IO,               FileStreamAsyncResult, AsyncResultBase)
DEFINE_FIELD_U(FILESTREAM_ASYNCRESULT,  USER_CALLBACK,      _userCallback,          AsyncResultBase,    _userCallback)
DEFINE_FIELD_U(FILESTREAM_ASYNCRESULT,  USER_STATE,         _userStateObject,       AsyncResultBase,    _userStateObject)
DEFINE_FIELD_U(FILESTREAM_ASYNCRESULT,  WAIT_HANDLE,        _waitHandle,            AsyncResultBase,    _waitHandle)
DEFINE_FIELD_U(FILESTREAM_ASYNCRESULT,  FILE_HANDLE,        _handle,                AsyncResultBase,    _fileHandle)
DEFINE_FIELD_U(FILESTREAM_ASYNCRESULT,  END_CALLED,         _EndXxxCalled,          AsyncResultBase,    _EndXxxCalled)
DEFINE_FIELD_U(FILESTREAM_ASYNCRESULT,  NUM_BYTES,          _numBytes,              AsyncResultBase,    _numBytes)
DEFINE_FIELD_U(FILESTREAM_ASYNCRESULT,  ERROR_CODE,         _errorCode,             AsyncResultBase,    _errorCode)
DEFINE_FIELD_U(FILESTREAM_ASYNCRESULT,  BUFFERED_BYTES,     _numBufferedBytes,      AsyncResultBase,    _numBufferedBytes)
DEFINE_FIELD_U(FILESTREAM_ASYNCRESULT,  IS_WRITE,           _isWrite,               AsyncResultBase,    _isWrite)
DEFINE_FIELD_U(FILESTREAM_ASYNCRESULT,  IS_COMPLETE,        _isComplete,            AsyncResultBase,    _isComplete)
DEFINE_FIELD_U(FILESTREAM_ASYNCRESULT,  COMPLETED_SYNCHRONOUSLY, _completedSynchronously, AsyncResultBase, _completedSynchronously)

DEFINE_CLASS_U(FRAME_SECURITY_DESCRIPTOR, Security,           FrameSecurityDescriptor, FrameSecurityDescriptorBaseObject)
DEFINE_FIELD_U(FRAME_SECURITY_DESCRIPTOR, ASSERT_PERMSET,       m_assertions,       FrameSecurityDescriptorBaseObject,  m_assertions)
DEFINE_FIELD_U(FRAME_SECURITY_DESCRIPTOR, DENY_PERMSET,         m_denials,          FrameSecurityDescriptorBaseObject,  m_denials)
DEFINE_FIELD_U(FRAME_SECURITY_DESCRIPTOR, RESTRICTION_PERMSET,  m_restriction,      FrameSecurityDescriptorBaseObject,  m_restriction)
DEFINE_FIELD_U(FRAME_SECURITY_DESCRIPTOR, ASSERT_FT,            m_AssertFT,         FrameSecurityDescriptorBaseObject,  m_assertFT)
DEFINE_FIELD_U(FRAME_SECURITY_DESCRIPTOR, ASSERT_ALL_POSSIBLE,  m_assertAllPossible,FrameSecurityDescriptorBaseObject,  m_assertAllPossible)
DEFINE_FIELD_U(FRAME_SECURITY_DESCRIPTOR, DECLARATIVE_ASSERT_PERMSET,       m_DeclarativeAssertions,       FrameSecurityDescriptorBaseObject,  m_DeclarativeAssertions)
DEFINE_FIELD_U(FRAME_SECURITY_DESCRIPTOR, DECLARATIVE_DENY_PERMSET,         m_DeclarativeDenials,          FrameSecurityDescriptorBaseObject,  m_DeclarativeDenials)
DEFINE_FIELD_U(FRAME_SECURITY_DESCRIPTOR, DECLARATIVE_RESTRICTION_PERMSET,  m_DeclarativeRestrictions,      FrameSecurityDescriptorBaseObject,  m_DeclarativeRestrictions)

DEFINE_CLASS(GUID,                  System,                 Guid)

DEFINE_CLASS(IASYNCRESULT,          System,                 IAsyncResult)

DEFINE_CLASS(ICUSTOM_ATTR_PROVIDER, Reflection,             ICustomAttributeProvider)
DEFINE_METHOD(ICUSTOM_ATTR_PROVIDER,GET_CUSTOM_ATTRIBUTES,  GetCustomAttributes,        IM_Type_RetArrObj)

DEFINE_CLASS(ICUSTOM_MARSHALER,     Interop,                ICustomMarshaler)
DEFINE_METHOD(ICUSTOM_MARSHALER,    MARSHAL_NATIVE_TO_MANAGED,MarshalNativeToManaged,   IM_IntPtr_RetObj)
DEFINE_METHOD(ICUSTOM_MARSHALER,    MARSHAL_MANAGED_TO_NATIVE,MarshalManagedToNative,   IM_Obj_RetIntPtr)
DEFINE_METHOD(ICUSTOM_MARSHALER,    CLEANUP_NATIVE_DATA,    CleanUpNativeData,          IM_IntPtr_RetVoid)
DEFINE_METHOD(ICUSTOM_MARSHALER,    CLEANUP_MANAGED_DATA,   CleanUpManagedData,         IM_Obj_RetVoid)
DEFINE_METHOD(ICUSTOM_MARSHALER,    GET_NATIVE_DATA_SIZE,   GetNativeDataSize,         IM_RetInt)

DEFINE_CLASS(IDENTITY,              Remoting,               Identity)
DEFINE_FIELD(IDENTITY,              TP_OR_OBJECT,           _tpOrObject)
DEFINE_FIELD(IDENTITY,              LEASE,                  _lease)
DEFINE_FIELD(IDENTITY,              OBJURI,                 _ObjURI)

DEFINE_CLASS(ISERIALIZABLE,         Serialization,          ISerializable)
DEFINE_CLASS(IOBJECTREFERENCE,      Serialization,          IObjectReference)
DEFINE_CLASS(IDESERIALIZATIONCB,    Serialization,          IDeserializationCallback)
DEFINE_CLASS(STREAMING_CONTEXT,     Serialization,          StreamingContext)
DEFINE_CLASS(SERIALIZATION_INFO,    Serialization,          SerializationInfo)

DEFINE_CLASS(OBJECTCLONEHELPER,     Serialization,          ObjectCloneHelper)
DEFINE_METHOD(OBJECTCLONEHELPER,    GET_OBJECT_DATA,        GetObjectData,              SM_Obj_OutStr_OutStr_OutArrStr_OutArrObj_RetObj)
DEFINE_METHOD(OBJECTCLONEHELPER,    PREPARE_DATA,           PrepareConstructorArgs,     SM_Obj_ArrStr_ArrObj_OutStreamingContext_RetSerializationInfo)


DEFINE_CLASS(IENUMERATOR,           Collections,            IEnumerator)

DEFINE_CLASS(IENUMERABLE,           Collections,            IEnumerable)


DEFINE_CLASS(ISSEXCEPTION,          IsolatedStorage,        IsolatedStorageException)

DEFINE_CLASS(ILLOGICAL_CALL_CONTEXT,Messaging,              IllogicalCallContext)

DEFINE_CLASS(INT16,                 System,                 Int16)

DEFINE_CLASS(INT32,                 System,                 Int32)

DEFINE_CLASS(INT64,                 System,                 Int64)

DEFINE_CLASS(IPERMISSION,           Security,               IPermission)

DEFINE_CLASS(IPRINCIPAL,            Principal,              IPrincipal)

DEFINE_CLASS(IREFLECT,              Reflection,             IReflect)
DEFINE_METHOD(IREFLECT,             GET_PROPERTIES,         GetProperties,              IM_BindingFlags_RetArrPropertyInfo)
DEFINE_METHOD(IREFLECT,             GET_FIELDS,             GetFields,                  IM_BindingFlags_RetArrFieldInfo)
DEFINE_METHOD(IREFLECT,             GET_METHODS,            GetMethods,                 IM_BindingFlags_RetArrMethodInfo)
DEFINE_METHOD(IREFLECT,             INVOKE_MEMBER,          InvokeMember,               IM_Str_BindingFlags_Binder_Obj_ArrObj_ArrParameterModifier_CultureInfo_ArrStr_RetObj)

DEFINE_CLASS(ISS_STORE,             IsolatedStorage,        IsolatedStorage)
DEFINE_CLASS(ISS_STORE_FILE,        IsolatedStorage,        IsolatedStorageFile)
DEFINE_CLASS(ISS_STORE_FILE_STREAM, IsolatedStorage,        IsolatedStorageFileStream)


// Interop has one field of type Hashtable - search for Hashtable in this file.
DEFINE_CLASS(HASHTABLE,             Collections,            Hashtable)

DEFINE_CLASS(LOADER_OPTIMIZATION,   System,                 LoaderOptimization)

DEFINE_CLASS(LOCAL_DATA_STORE,      System,                 LocalDataStore)

DEFINE_CLASS(CALL_CONTEXT_SECURITY_DATA,  Messaging,              CallContextSecurityData)
DEFINE_FIELD(CALL_CONTEXT_SECURITY_DATA,   PRINCIPAL,   _principal)

DEFINE_CLASS(LOGICAL_CALL_CONTEXT,  Messaging,              LogicalCallContext)
DEFINE_FIELD(LOGICAL_CALL_CONTEXT,   SECURITY_DATA,     m_SecurityData)

DEFINE_CLASS(MARSHAL,               Interop,                Marshal)

DEFINE_CLASS_U(MARSHAL_BY_REF_OBJECT, System,                 MarshalByRefObject,   MarshalByRefObjectBaseObject)
DEFINE_FIELD_U(MARSHAL_BY_REF_OBJECT, IDENTITY,               __identity,               MarshalByRefObjectBaseObject,   m_ServerIdentity)

DEFINE_CLASS(MEMBER,                Reflection,             MemberInfo)

DEFINE_CLASS_U(MESSAGE,             Messaging,              Message,                    MessageObject)
DEFINE_FIELD_U(MESSAGE,             METHOD_NAME,            _MethodName,                MessageObject,       pMethodName)
DEFINE_FIELD_U(MESSAGE,             METHOD_SIG,             _MethodSignature,           MessageObject,       pMethodSig)
DEFINE_FIELD_U(MESSAGE,             METHOD_BASE,            _MethodBase,                MessageObject,       pMethodBase)
DEFINE_FIELD_U(MESSAGE,             HASH_TABLE,             _properties,                MessageObject,       pHashTable)
DEFINE_FIELD_U(MESSAGE,             URI,                    _URI,                       MessageObject,       pURI)
DEFINE_FIELD_U(MESSAGE,             TYPE_NAME,              _typeName,                  MessageObject,       pTypeName)
DEFINE_FIELD_U(MESSAGE,             FAULT,                  _Fault,                     MessageObject,       pFault)
DEFINE_FIELD_U(MESSAGE,             ID,                     _ID,                        MessageObject,       pID)
DEFINE_FIELD_U(MESSAGE,             SRV_ID,                 _srvID,                     MessageObject,       pSrvID)
DEFINE_FIELD_U(MESSAGE,             ARG_MAPPER,             _argMapper,                 MessageObject,       pArgMapper)
DEFINE_FIELD_U(MESSAGE,             CALL_CONTEXT,           _callContext,               MessageObject,       pCallCtx)
DEFINE_FIELD_U(MESSAGE,             FRAME,                  _frame,                     MessageObject,       pFrame)
DEFINE_FIELD_U(MESSAGE,             METHOD_DESC,            _methodDesc,                MessageObject,       pMethodDesc)
DEFINE_FIELD_U(MESSAGE,             METASIG_HOLDER,         _metaSigHolder,             MessageObject,       pMetaSigHolder)
DEFINE_FIELD_U(MESSAGE,             DELEGATE_MD,            _delegateMD,                MessageObject,       pDelegateMD)
DEFINE_FIELD_U(MESSAGE,             GOVERNING_TYPE,         _governingType,             MessageObject,       thGoverningType)
DEFINE_FIELD_U(MESSAGE,             FLAGS,                  _flags,                     MessageObject,       iFlags)
DEFINE_FIELD_U(MESSAGE,             INIT_DONE,              _initDone,                  MessageObject,       initDone)

DEFINE_CLASS(MESSAGE_DATA,          Proxies,              MessageData)

DEFINE_CLASS(METHOD,                Reflection,             RuntimeMethodInfo)
DEFINE_METHOD(METHOD,               INVOKE,                 Invoke,                     IM_Obj_BindingFlags_Binder_ArrObj_CultureInfo_RetObj)
DEFINE_METHOD(METHOD,               GET_PARAMETERS,         GetParameters,              IM_RetArrParameterInfo)
DEFINE_METHOD(METHOD,               GET_METHODHANDLE,       get_MethodHandle,           IM_RetRuntimeMethodHandle)

DEFINE_CLASS(METHOD_BASE,           Reflection,             MethodBase)
DEFINE_METHOD(METHOD_BASE,          GET_CURRENT_METHOD,     GetCurrentMethod,           SM_RetMethodBase)

DEFINE_CLASS_U(EH_CLAUSE,           Reflection,             ExceptionHandlingClause,    ExceptionHandlingClause)
DEFINE_FIELD_U(EH_CLAUSE,           METHOD_BODY,            m_methodBody,               ExceptionHandlingClause,        m_methodBody)
DEFINE_FIELD_U(EH_CLAUSE,           FLAGS,                  m_flags,                    ExceptionHandlingClause,        m_flags)
DEFINE_FIELD_U(EH_CLAUSE,           TRY_OFFSET,             m_tryOffset,                ExceptionHandlingClause,        m_tryOffset)
DEFINE_FIELD_U(EH_CLAUSE,           TRY_LENGTH,             m_tryLength,                ExceptionHandlingClause,        m_tryLength)
DEFINE_FIELD_U(EH_CLAUSE,           HANDLER_OFFSET,         m_handlerOffset,            ExceptionHandlingClause,        m_handlerOffset)
DEFINE_FIELD_U(EH_CLAUSE,           HANDLER_LENGTH,         m_handlerLength,            ExceptionHandlingClause,        m_handlerLength)
DEFINE_FIELD_U(EH_CLAUSE,           TYPE_TOKEN,             m_catchMetadataToken,       ExceptionHandlingClause,        m_catchToken)
DEFINE_FIELD_U(EH_CLAUSE,           FILTER_OFFSET,          m_filterOffset,             ExceptionHandlingClause,        m_filterOffset)

DEFINE_CLASS_U(LOCAL_VARIABLE_INFO, Reflection,             LocalVariableInfo,          LocalVariableInfo)
DEFINE_FIELD_U(LOCAL_VARIABLE_INFO, LOCAL_TYPE,             m_typeHandle,               LocalVariableInfo,        m_typeHandle)
DEFINE_FIELD_U(LOCAL_VARIABLE_INFO, IS_PINNED,              m_isPinned,                 LocalVariableInfo,        m_bIsPinned)
DEFINE_FIELD_U(LOCAL_VARIABLE_INFO, LOCAL_INDEX,            m_localIndex,               LocalVariableInfo,        m_localIndex)

DEFINE_CLASS_U(METHOD_BODY,         Reflection,             MethodBody,                 MethodBody)
DEFINE_FIELD_U(METHOD_BODY,         RAW_IL,                 m_IL,                       MethodBody,         m_IL)
DEFINE_FIELD_U(METHOD_BODY,         EH_CLAUSES,             m_exceptionHandlingClauses, MethodBody,         m_exceptionClauses)
DEFINE_FIELD_U(METHOD_BODY,         LOCAL_VARIABLES,        m_localVariables,           MethodBody,         m_localVariables)
DEFINE_FIELD_U(METHOD_BODY,         METHOD_BASE,            m_methodBase,               MethodBody,         m_methodBase)
DEFINE_FIELD_U(METHOD_BODY,         RAW_LOCALS,             m_localSignatureMetadataToken, MethodBody,      m_localVarSigToken)
DEFINE_FIELD_U(METHOD_BODY,         MAX_STACK_SIZE,         m_maxStackSize,             MethodBody,         m_maxStackSize)
DEFINE_FIELD_U(METHOD_BODY,         INIT_LOCALS,            m_initLocals,               MethodBody,         m_initLocals)

DEFINE_CLASS(METHOD_INFO,           Reflection,             MethodInfo)

DEFINE_CLASS(METHOD_HANDLE,         System,                 RuntimeMethodHandle)

DEFINE_CLASS(METHOD_RENTAL,         ReflectionEmit,         MethodRental)

DEFINE_CLASS(MISSING,               Reflection,             Missing)
DEFINE_FIELD(MISSING,               VALUE,                  Value)

DEFINE_CLASS_U(MODULE,              Reflection,             Module,                     ReflectModuleBaseObject)
DEFINE_FIELD_U(MODULE,              TYPE_BUILDER_LIST,      m_TypeBuilderList,          ReflectModuleBaseObject,    m_TypeBuilderList)
DEFINE_FIELD_U(MODULE,              SYM_WRITER,             m_iSymWriter,               ReflectModuleBaseObject,    m_ISymWriter)
DEFINE_FIELD_U(MODULE,              MODULE_DATA,            m_moduleData,               ReflectModuleBaseObject,    m_moduleData)
DEFINE_FIELD_U(MODULE,              RUNTIME_TYPE,           m_runtimeType,              ReflectModuleBaseObject,    m_runtimeType)
DEFINE_FIELD_U(MODULE,              CLASS,                  m_pRefClass,                ReflectModuleBaseObject,    m_ReflectClass)
DEFINE_FIELD_U(MODULE,              DATA,                   m_pData,                    ReflectModuleBaseObject,    m_pData)
DEFINE_FIELD_U(MODULE,              INTERNAL_SYM_WRITER,    m_pInternalSymWriter,       ReflectModuleBaseObject,    m_pInternalSymWriter)
DEFINE_FIELD_U(MODULE,              GLOBALS,                m_pGlobals,                 ReflectModuleBaseObject,    m_pGlobals)
DEFINE_FIELD_U(MODULE,              FIELDS,                 m_pFields,                  ReflectModuleBaseObject,    m_pGlobalsFlds)
DEFINE_FIELD_U(MODULE,              ENTRYPOINT,             m_EntryPoint,               ReflectModuleBaseObject,    m_EntryPoint)

DEFINE_CLASS(MODULE_BUILDER,        ReflectionEmit,         ModuleBuilder)
DEFINE_CLASS(TYPE_BUILDER,          ReflectionEmit,         TypeBuilder)
DEFINE_CLASS(ENUM_BUILDER,          ReflectionEmit,         EnumBuilder)

DEFINE_CLASS(MODULE_HANDLE,         System,                 ModuleHandle)

DEFINE_CLASS_U(MULTICAST_DELEGATE,  System,                 MulticastDelegate,          DelegateObject)
DEFINE_FIELD_U(MULTICAST_DELEGATE,  INVOCATION_LIST,        _invocationList,            DelegateObject,   _invocationList)
DEFINE_FIELD_U(MULTICAST_DELEGATE,  INVOCATION_COUNT,       _invocationCount,           DelegateObject,   _invocationCount)
DEFINE_METHOD(MULTICAST_DELEGATE,   CTOR_CLOSED,            CtorClosed,                 IM_Obj_IntPtr_RetVoid)
DEFINE_METHOD(MULTICAST_DELEGATE,   CTOR_CLOSED_STATIC,     CtorClosedStatic,           IM_Obj_IntPtr_RetVoid)
DEFINE_METHOD(MULTICAST_DELEGATE,   CTOR_RT_CLOSED,         CtorRTClosed,               IM_Obj_IntPtr_RetVoid)
DEFINE_METHOD(MULTICAST_DELEGATE,   CTOR_OPENED,            CtorOpened,                 IM_Obj_IntPtr_IntPtr_RetVoid)
DEFINE_METHOD(MULTICAST_DELEGATE,   CTOR_SECURE_CLOSED,     CtorSecureClosed,           IM_Obj_IntPtr_IntPtr_IntPtr_RetVoid)
DEFINE_METHOD(MULTICAST_DELEGATE,   CTOR_SECURE_CLOSED_STATIC,CtorSecureClosedStatic,   IM_Obj_IntPtr_IntPtr_IntPtr_RetVoid)
DEFINE_METHOD(MULTICAST_DELEGATE,   CTOR_SECURE_RT_CLOSED,  CtorSecureRTClosed,         IM_Obj_IntPtr_IntPtr_IntPtr_RetVoid)
DEFINE_METHOD(MULTICAST_DELEGATE,   CTOR_SECURE_OPENED,     CtorSecureOpened,           IM_Obj_IntPtr_IntPtr_IntPtr_IntPtr_RetVoid)
DEFINE_METHOD(MULTICAST_DELEGATE,   CTOR_VIRTUAL_DISPATCH,  CtorVirtualDispatch,        IM_Obj_IntPtr_IntPtr_RetVoid)
DEFINE_METHOD(MULTICAST_DELEGATE,   CTOR_SECURE_VIRTUAL_DISPATCH,  CtorSecureVirtualDispatch, IM_Obj_IntPtr_IntPtr_IntPtr_IntPtr_RetVoid)

DEFINE_CLASS(NULL,                  System,                 DBNull)
DEFINE_FIELD(NULL,                  VALUE,          Value)

DEFINE_CLASS(NULLABLE,              System,                 Nullable`1)

// Keep this in sync with System.Globalization.NumberFormatInfo
DEFINE_CLASS_U(NUMBERFORMATINFO,       Globalization,       NumberFormatInfo,   NumberFormatInfo)
DEFINE_FIELD_U(NUMBERFORMATINFO,       NUMBERGROUPSIZES,    numberGroupSizes,       NumberFormatInfo,   cNumberGroup)
DEFINE_FIELD_U(NUMBERFORMATINFO,       CURRENCYGROUPSIZES,  currencyGroupSizes,     NumberFormatInfo,   cCurrencyGroup)
DEFINE_FIELD_U(NUMBERFORMATINFO,       PERCENTGROUPSIZS,    percentGroupSizes,      NumberFormatInfo,   cPercentGroup)
DEFINE_FIELD_U(NUMBERFORMATINFO,       POSITIVESIGN,        positiveSign,           NumberFormatInfo,   sPositive)
DEFINE_FIELD_U(NUMBERFORMATINFO,       NEGATIVESIGN,        negativeSign,           NumberFormatInfo,   sNegative)
DEFINE_FIELD_U(NUMBERFORMATINFO,       NUMBERDECIMALSEP,    numberDecimalSeparator, NumberFormatInfo,   sNumberDecimal)
DEFINE_FIELD_U(NUMBERFORMATINFO,       NUMBERGROUPSEP,      numberGroupSeparator,   NumberFormatInfo,   sNumberGroup)
DEFINE_FIELD_U(NUMBERFORMATINFO,       CURRENCYGROUPSEP,    currencyGroupSeparator, NumberFormatInfo,   sCurrencyGroup)
DEFINE_FIELD_U(NUMBERFORMATINFO,       CURRENCYDECIMALSEP,  currencyDecimalSeparator,NumberFormatInfo,   sCurrencyDecimal)
DEFINE_FIELD_U(NUMBERFORMATINFO,       CURRENCYSYMBOL,      currencySymbol,         NumberFormatInfo,   sCurrency)
DEFINE_FIELD_U(NUMBERFORMATINFO,       ANSICURRENCYSYMBOL,  ansiCurrencySymbol,     NumberFormatInfo,   sAnsiCurrency)
DEFINE_FIELD_U(NUMBERFORMATINFO,       NANSYMBOL,           nanSymbol,              NumberFormatInfo,   sNaN)
DEFINE_FIELD_U(NUMBERFORMATINFO,       POSITIVEINFINITYSYM, positiveInfinitySymbol, NumberFormatInfo,   sPositiveInfinity)
DEFINE_FIELD_U(NUMBERFORMATINFO,       NEGATIVEEINFINITYSYM,negativeInfinitySymbol, NumberFormatInfo,   sNegativeInfinity)
DEFINE_FIELD_U(NUMBERFORMATINFO,       PERCENTDECIMALSEP,   percentDecimalSeparator,NumberFormatInfo,   sPercentDecimal)
DEFINE_FIELD_U(NUMBERFORMATINFO,       PERCENTGROUPSEP,     percentGroupSeparator,  NumberFormatInfo,   sPercentGroup)
DEFINE_FIELD_U(NUMBERFORMATINFO,       PERCENTSYMBOL,       percentSymbol,          NumberFormatInfo,   sPercent)
DEFINE_FIELD_U(NUMBERFORMATINFO,       PERMILLESYMBOL,      perMilleSymbol,         NumberFormatInfo,   sPerMille)
DEFINE_FIELD_U(NUMBERFORMATINFO,       NATIVEDIGITS,        nativeDigits,           NumberFormatInfo,   sNativeDigits)
DEFINE_FIELD_U(NUMBERFORMATINFO,       DATAITEM,            m_dataItem,             NumberFormatInfo,   iDataItem)
DEFINE_FIELD_U(NUMBERFORMATINFO,       NUMBERDECIMALDIGITS, numberDecimalDigits,    NumberFormatInfo,   cNumberDecimals)
DEFINE_FIELD_U(NUMBERFORMATINFO,       CURRENCYDECIMALDIGITS,currencyDecimalDigits, NumberFormatInfo,   cCurrencyDecimals)
DEFINE_FIELD_U(NUMBERFORMATINFO,       CURRENCYPOSPATTERN,  currencyPositivePattern,NumberFormatInfo,   cPosCurrencyFormat)
DEFINE_FIELD_U(NUMBERFORMATINFO,       CURRENCYNEGPATTERN,  currencyNegativePattern,NumberFormatInfo,   cNegCurrencyFormat)
DEFINE_FIELD_U(NUMBERFORMATINFO,       NUMBERNEGPATTERN,    numberNegativePattern,  NumberFormatInfo,   cNegativeNumberFormat)
DEFINE_FIELD_U(NUMBERFORMATINFO,       PERCENTPOSPATTERN,   percentPositivePattern, NumberFormatInfo,   cPositivePercentFormat)
DEFINE_FIELD_U(NUMBERFORMATINFO,       PERCENTNEGPATTERN,   percentNegativePattern, NumberFormatInfo,   cNegativePercentFormat)
DEFINE_FIELD_U(NUMBERFORMATINFO,       PERCENTDECIMALDIGITS,percentDecimalDigits,   NumberFormatInfo,   cPercentDecimals)
DEFINE_FIELD_U(NUMBERFORMATINFO,       DIGITSUBSTITUTION,   digitSubstitution,      NumberFormatInfo,   iDigitSubstitution)
DEFINE_FIELD_U(NUMBERFORMATINFO,       ISREADONLY,          isReadOnly,             NumberFormatInfo,   bIsReadOnly)
DEFINE_FIELD_U(NUMBERFORMATINFO,       USEROVERRIDE,        m_useUserOverride,      NumberFormatInfo,   bUseUserOverride)


DEFINE_CLASS(OBJECT,                System,                 Object)
DEFINE_METHOD(OBJECT,               CTOR,                   .ctor,                      IM_RetVoid)
DEFINE_METHOD(OBJECT,               FINALIZE,               Finalize,                   IM_RetVoid)
DEFINE_METHOD(OBJECT,               TO_STRING,              ToString,                   IM_RetStr)
DEFINE_METHOD(OBJECT,               GET_TYPE,               GetType,                    IM_RetType)
DEFINE_METHOD(OBJECT,               GET_HASH_CODE,          GetHashCode,                IM_RetInt)
DEFINE_METHOD(OBJECT,               FIELD_SETTER,           FieldSetter,                IM_Str_Str_Obj_RetVoid)
DEFINE_METHOD(OBJECT,               FIELD_GETTER,           FieldGetter,                IM_Str_Str_RefObj_RetVoid)

DEFINE_CLASS(__CANON,              System,                 __Canon)


DEFINE_CLASS(OLE_AUT_BINDER,        System,                 OleAutBinder)    

// Note: The size of the OverlappedData can be inflated by the CLR host
DEFINE_CLASS(OVERLAPPEDDATA,            Threading,              OverlappedData)
DEFINE_FIELD_U(OVERLAPPEDDATA,      ASYNC_RESULT,           m_asyncResult,              OverlappedDataObject,       m_asyncResult)
DEFINE_FIELD_U(OVERLAPPEDDATA,      IOCB,                   m_iocb,                     OverlappedDataObject,       m_iocb)
DEFINE_FIELD_U(OVERLAPPEDDATA,      IOCB_HELPER,            m_iocbHelper,               OverlappedDataObject,       m_iocbHelper)
DEFINE_FIELD_U(OVERLAPPEDDATA,      OVERLAPPED,             m_overlapped,               OverlappedDataObject,       m_overlapped)
DEFINE_FIELD_U(OVERLAPPEDDATA,      USER_OBJECT,            m_userObject,               OverlappedDataObject,       m_userObject)
DEFINE_FIELD_U(OVERLAPPEDDATA,      PIN_SELF,               m_pinSelf,                  OverlappedDataObject,       m_pinSelf)
DEFINE_FIELD_U(OVERLAPPEDDATA,      APPDOMAIN_ID,           m_AppDomainId,              OverlappedDataObject,       m_AppDomainId)
DEFINE_FIELD_U(OVERLAPPEDDATA,      SLOT,                   m_slot,                     OverlappedDataObject,       m_slot)
DEFINE_FIELD_U(OVERLAPPEDDATA,      IS_ARRAY,               m_isArray,                  OverlappedDataObject,       m_isArray)

DEFINE_CLASS(NATIVEOVERLAPPED,            Threading,              NativeOverlapped)

DEFINE_CLASS(PARAMETER,             Reflection,             ParameterInfo)

DEFINE_CLASS(PARAMETER_MODIFIER,    Reflection,             ParameterModifier)

// Keep this in sync with System.Security.PermissionSet
DEFINE_CLASS_U(PERMISSION_SET,      Security,               PermissionSet,      PermissionSetObject)
DEFINE_FIELD_U(PERMISSION_SET,      SET,                    m_permSet,                  PermissionSetObject, _permSet)
DEFINE_FIELD_U(PERMISSION_SET,      UNRESTRICTED,           m_Unrestricted,             PermissionSetObject, _Unrestricted)
DEFINE_FIELD_U(PERMISSION_SET,      ALL_PERMISSIONS_DECODED,m_allPermissionsDecoded,    PermissionSetObject, _allPermissionsDecoded)
DEFINE_FIELD_U(PERMISSION_SET,      CAN_UNRESTRICTED_OVERRIDE,m_canUnrestrictedOverride,PermissionSetObject, _canUnrestrictedOverride)
DEFINE_FIELD_U(PERMISSION_SET,      IGNORE_TYPE_LOAD_FAILURES,m_ignoreTypeLoadFailures, PermissionSetObject, _ignoreTypeLoadFailures)
DEFINE_FIELD_U(PERMISSION_SET,      CHECK_FOR_NONCAS,       m_CheckedForNonCas,         PermissionSetObject, _CheckedForNonCas)
DEFINE_FIELD_U(PERMISSION_SET,      CONTAINS_CAS,           m_ContainsCas,              PermissionSetObject, _ContainsCas)
DEFINE_FIELD_U(PERMISSION_SET,      CONTAINS_NONCAS,        m_ContainsNonCas,           PermissionSetObject, _ContainsNonCas)
DEFINE_METHOD(PERMISSION_SET,       CTOR,                   .ctor,                      IM_Bool_RetVoid)
DEFINE_METHOD(PERMISSION_SET,       CREATE_SERIALIZED,      CreateSerialized,           SM_ArrObj_Bool_RefArrByte_OutPMS_HostProtectionResource_RetArrByte)
DEFINE_METHOD(PERMISSION_SET,       SETUP_SECURITY,         SetupSecurity,              SM_RetVoid)
DEFINE_METHOD(PERMISSION_SET,       CONTAINS,               Contains,                   IM_IPermission_RetBool)
DEFINE_METHOD(PERMISSION_SET,       DEMAND,                 Demand,                     IM_RetVoid)
DEFINE_METHOD(PERMISSION_SET,       DEMAND_NON_CAS,         DemandNonCAS,               IM_RetVoid)
DEFINE_METHOD(PERMISSION_SET,       DECODE_XML,             DecodeXml,                  IM_ArrByte_HostProtectionResource_HostProtectionResource_RetBool)
DEFINE_METHOD(PERMISSION_SET,       ENCODE_XML,             EncodeXml,                  IM_RetArrByte)
DEFINE_METHOD(PERMISSION_SET,       IS_UNRESTRICTED,        IsUnrestricted,             IM_RetBool)
DEFINE_METHOD(PERMISSION_SET,       IS_SUBSET_OF,           IsSubsetOf,                 IM_PMS_RetBool)
DEFINE_METHOD(PERMISSION_SET,       ADD_PERMISSION,         AddPermission,              IM_IPermission_RetIPermission)
DEFINE_METHOD(PERMISSION_SET,       INPLACE_UNION,          InplaceUnion,               IM_PMS_RetVoid)
DEFINE_METHOD(PERMISSION_SET,       UNION,                  Union,                      IM_PMS_RetPMS)
DEFINE_METHOD(PERMISSION_SET,       INTERSECT,              Intersect,                  IM_PMS_RetPMS)
DEFINE_METHOD(PERMISSION_SET,       IS_EMPTY,               IsEmpty,                    IM_RetBool)

DEFINE_CLASS(IUNRESTRICTED_PERMISSION,        Permissions,               IUnrestrictedPermission )


DEFINE_CLASS_U(PERMISSION_LIST_SET,   Security,             PermissionListSet,     PermissionListSetObject)
DEFINE_FIELD_U(PERMISSION_LIST_SET,   FIRSTTRIPLE,          m_firstPermSetTriple,  PermissionListSetObject, _firstPermSetTriple)
DEFINE_FIELD_U(PERMISSION_LIST_SET,   TRIPLEARRAY,          m_permSetTriples,      PermissionListSetObject, _permSetTriples)
DEFINE_FIELD_U(PERMISSION_LIST_SET,   ZONELIST,             m_zoneList,            PermissionListSetObject, _zoneList)
DEFINE_FIELD_U(PERMISSION_LIST_SET,   ORIGINLIST,           m_originList,          PermissionListSetObject, _originList)
DEFINE_METHOD(PERMISSION_LIST_SET,  CTOR,                   .ctor,                      IM_RetVoid)
DEFINE_METHOD(PERMISSION_LIST_SET,  CHECK_DEMAND_NO_THROW,  CheckDemandNoThrow,         IM_CodeAccessPermission_RetBool)
DEFINE_METHOD(PERMISSION_LIST_SET,  CHECK_SET_DEMAND_NO_THROW, CheckSetDemandNoThrow,   IM_PMS_RetBool)
DEFINE_METHOD(PERMISSION_LIST_SET,  UPDATE,                  Update,                     IM_PMS_RetVoid)

DEFINE_CLASS(PERMISSION_STATE,      Permissions,            PermissionState)

DEFINE_CLASS(PERMISSION_TOKEN,      Security,               PermissionToken)
DEFINE_METHOD(PERMISSION_TOKEN,     GET_TOKEN,              GetToken,                   SM_IPermission_RetPermissionToken)

DEFINE_CLASS(ALLOW_PARTIALLY_TRUSTED_CALLER, Security,              AllowPartiallyTrustedCallersAttribute)
DEFINE_CLASS(SECURITY_TRANSPARENT_ATTRIBUTE, Security,              SecurityTransparentAttribute)
DEFINE_CLASS(SECURITY_CRITICAL_ATTRIBUTE, Security,              SecurityCriticalAttribute)
DEFINE_CLASS(SECURITY_TREATASSAFE_ATTRIBUTE, Security,              SecurityTreatAsSafeAttribute)

DEFINE_CLASS(POINTER,               Reflection,             Pointer)
DEFINE_FIELD(POINTER,               VALUE,                  _ptr)
DEFINE_FIELD(POINTER,               TYPE,                   _ptrType)

DEFINE_CLASS(PROPERTY,              Reflection,             RuntimePropertyInfo)
DEFINE_METHOD(PROPERTY,             SET_VALUE,              SetValue,                   IM_Obj_Obj_BindingFlags_Binder_ArrObj_CultureInfo_RetVoid)
DEFINE_METHOD(PROPERTY,             GET_VALUE,              GetValue,                   IM_Obj_BindingFlags_Binder_ArrObj_CultureInfo_RetObj)
DEFINE_METHOD(PROPERTY,             GET_INDEX_PARAMETERS,   GetIndexParameters,         IM_RetArrParameterInfo)
DEFINE_METHOD(PROPERTY,             GET_TOKEN,              get_MetadataToken,          IM_RetInt)
DEFINE_METHOD(PROPERTY,             GET_MODULE,             get_Module,                 IM_RetModule)
DEFINE_METHOD(PROPERTY,             GET_SETTER,             GetSetMethod,               IM_Bool_RetMethodInfo)
DEFINE_METHOD(PROPERTY,             GET_GETTER,             GetGetMethod,               IM_Bool_RetMethodInfo)

DEFINE_CLASS(PROPERTY_INFO,         Reflection,             PropertyInfo)

DEFINE_CLASS(PROXY_ATTRIBUTE,       Proxies,                ProxyAttribute)

DEFINE_CLASS_U(REAL_PROXY,          Proxies,                RealProxy,      RealProxyObject)
DEFINE_FIELD_U(REAL_PROXY,          TP,                     _tp,                        RealProxyObject,    _tp)
DEFINE_FIELD_U(REAL_PROXY,          IDENTITY,               _identity,                  RealProxyObject,    _identity)
DEFINE_FIELD_U(REAL_PROXY,          SERVER,                 _serverObject,              RealProxyObject,    _serverObject)
DEFINE_FIELD_U(REAL_PROXY,          FLAGS,                  _flags,                     RealProxyObject,    _flags)
DEFINE_FIELD_U(REAL_PROXY,          OPT_FLAGS,              _optFlags,                  RealProxyObject,    _optFlags)
DEFINE_FIELD_U(REAL_PROXY,          DOMAIN_ID,              _domainID,                  RealProxyObject,    _domainID)
DEFINE_FIELD_U(REAL_PROXY,          SRV_DENTITY,            _srvIdentity,               RealProxyObject,    _srvIdentity)
DEFINE_METHOD(REAL_PROXY,           PRIVATE_INVOKE,         PrivateInvoke,              IM_RefMessageData_Int_RetVoid)

DEFINE_CLASS(REFLECTION_PERMISSION, Permissions,            ReflectionPermission)
DEFINE_METHOD(REFLECTION_PERMISSION,  CTOR,                   .ctor,                    IM_ReflectionPermissionFlag_RetVoid)

DEFINE_CLASS(REFLECTION_PERMISSION_FLAG, Permissions,       ReflectionPermissionFlag)


DEFINE_CLASS_U(READER_WRITER_LOCK,  Threading,              ReaderWriterLock,           CRWLock)
DEFINE_FIELD_U(READER_WRITER_LOCK,  WRITER_EVENT,           _hWriterEvent,              CRWLock, _hWriterEvent)
DEFINE_FIELD_U(READER_WRITER_LOCK,  READER_EVENT,           _hReaderEvent,              CRWLock, _hReaderEvent)
DEFINE_FIELD_U(READER_WRITER_LOCK,  OBJECT_HANDLE,          _hObjectHandle,             CRWLock, _hObjectHandle)
DEFINE_FIELD_U(READER_WRITER_LOCK,  STATE,                  _dwState,                   CRWLock, _dwState)
DEFINE_FIELD_U(READER_WRITER_LOCK,  ULOCKID,                _dwULockID,                 CRWLock, _dwULockID)
DEFINE_FIELD_U(READER_WRITER_LOCK,  LLOCKID,                _dwLLockID,                 CRWLock, _dwLLockID)
DEFINE_FIELD_U(READER_WRITER_LOCK,  WRITER_ID,              _dwWriterID,                CRWLock, _dwWriterID)
DEFINE_FIELD_U(READER_WRITER_LOCK,  WRITER_SEQ_NUM,         _dwWriterSeqNum,            CRWLock, _dwWriterSeqNum)
DEFINE_FIELD_U(READER_WRITER_LOCK,  WRITER_LEVEL,           _wWriterLevel,              CRWLock, _wWriterLevel)

DEFINE_CLASS(LEASE,                 Lifetime,               Lease)
DEFINE_METHOD(LEASE,                RENEW_ON_CALL,          RenewOnCall,                IM_RetVoid)

DEFINE_CLASS(REMOTING_PROXY,        Proxies,                RemotingProxy)
DEFINE_METHOD(REMOTING_PROXY,       INVOKE,                 Invoke,                     SM_Obj_RefMessageData_RetVoid)

DEFINE_CLASS(REMOTING_SERVICES,     Remoting,               RemotingServices)
DEFINE_METHOD(REMOTING_SERVICES,    CHECK_CAST,             CheckCast,                  SM_RealProxy_Type_RetBool)
DEFINE_METHOD(REMOTING_SERVICES,    GET_TYPE,               GetType,                    SM_Obj_RetObj)
DEFINE_METHOD(REMOTING_SERVICES,    WRAP,                   Wrap,                       SM_ContextBoundObject_RetObj)
DEFINE_METHOD(REMOTING_SERVICES,    CREATE_PROXY_FOR_DOMAIN,CreateProxyForDomain,       SM_Int_IntPtr_RetObj)
DEFINE_METHOD(REMOTING_SERVICES,    GET_SERVER_CONTEXT_FOR_PROXY,GetServerContextForProxy,  SM_Obj_RetIntPtr)        
DEFINE_METHOD(REMOTING_SERVICES,    GET_SERVER_DOMAIN_ID_FOR_PROXY,GetServerDomainIdForProxy,  SM_Obj_RetInt)        
DEFINE_METHOD(REMOTING_SERVICES,    MARSHAL_TO_BUFFER,      MarshalToBuffer,            SM_Obj_RetArrByte)
DEFINE_METHOD(REMOTING_SERVICES,    UNMARSHAL_FROM_BUFFER,  UnmarshalFromBuffer,        SM_ArrByte_RetObj)
DEFINE_METHOD(REMOTING_SERVICES,    DOMAIN_UNLOADED,        DomainUnloaded,             SM_Int_RetVoid)

DEFINE_CLASS(METADATA_IMPORT,       Reflection,             MetadataImport)
DEFINE_METHOD(METADATA_IMPORT,      THROW_ERROR,            ThrowError,                 SM_Int_RetVoid)

DEFINE_CLASS(RESOLVER,              System,                 Resolver)
DEFINE_METHOD(RESOLVER,             GET_JIT_CONTEXT,        GetJitContext,              IM_RefInt_RefRuntimeTypeHandle_RetVoid)
DEFINE_METHOD(RESOLVER,             GET_CODE_INFO,          GetCodeInfo,                IM_RefInt_RefInt_RefInt_RetArrByte)
DEFINE_METHOD(RESOLVER,             GET_LOCALS_SIGNATURE,   GetLocalsSignature,         IM_RetArrByte)
DEFINE_METHOD(RESOLVER,             GET_EH_INFO,            GetEHInfo,                  IM_Int_VoidPtr_RetVoid)
DEFINE_METHOD(RESOLVER,             GET_RAW_EH_INFO,        GetRawEHInfo,               IM_RetArrByte)
DEFINE_METHOD(RESOLVER,             GET_STRING_LITERAL,     GetStringLiteral,           IM_Int_RetStr)
DEFINE_METHOD(RESOLVER,             RESOLVE_TOKEN,          ResolveToken,               IM_Int_RetPtrVoid)
DEFINE_METHOD(RESOLVER,             RESOLVE_SIGNATURE,      ResolveSignature,           IM_IntInt_RetArrByte)
DEFINE_METHOD(RESOLVER,             PARENT_TOKEN,           ParentToken,                IM_Int_RetInt)
DEFINE_METHOD(RESOLVER,             IS_VALID_TOKEN,         IsValidToken,               IM_Int_RetInt)
DEFINE_METHOD(RESOLVER,             GET_INSTANTIATION_INFO, GetInstantiationInfo,       IM_Int_RetPtrVoid)

DEFINE_CLASS(RESOURCE_MANAGER,      Resources,              ResourceManager)

DEFINE_CLASS(RTFIELD,               Reflection,             RtFieldInfo)
DEFINE_METHOD(RTFIELD,              GET_FIELDHANDLE,        get_FieldHandle,            IM_RetRuntimeFieldHandle)

DEFINE_CLASS(RUNTIME_HELPERS,       CompilerServices,       RuntimeHelpers)
DEFINE_METHOD(RUNTIME_HELPERS,      PREPARE_CONSTRAINED_REGIONS, PrepareConstrainedRegions, SM_RetVoid)
DEFINE_METHOD(RUNTIME_HELPERS,      PREPARE_CONSTRAINED_REGIONS_NOOP, PrepareConstrainedRegionsNoOP, SM_RetVoid)
DEFINE_METHOD(RUNTIME_HELPERS,      EXECUTE_BACKOUT_CODE_HELPER, ExecuteBackoutCodeHelper, SM_Obj_Obj_Bool_RetVoid)


DEFINE_CLASS(RUNTIME_WRAPPED_EXCEPTION, CompilerServices,   RuntimeWrappedException)
DEFINE_METHOD(RUNTIME_WRAPPED_EXCEPTION, OBJ_CTOR,          .ctor,                      IM_Obj_RetVoid)
DEFINE_FIELD(RUNTIME_WRAPPED_EXCEPTION, WRAPPED_EXCEPTION,  m_wrappedException)

DEFINE_CLASS_U(SAFE_HANDLE,         Interop,                SafeHandle,         SafeHandle)
DEFINE_FIELD(SAFE_HANDLE,           HANDLE,                 handle)
DEFINE_FIELD_U(SAFE_HANDLE,         STATE,                  _state,                     SafeHandle,            m_state)
DEFINE_FIELD_U(SAFE_HANDLE,         OWNS_HANDLE,            _ownsHandle,                SafeHandle,            m_ownsHandle)
DEFINE_FIELD_U(SAFE_HANDLE,         INITIALIZED,            _fullyInitialized,          SafeHandle,            m_fullyInitialized)
DEFINE_METHOD(SAFE_HANDLE,          GET_IS_INVALID,         get_IsInvalid,              IM_RetBool)
DEFINE_METHOD(SAFE_HANDLE,          RELEASE_HANDLE,         ReleaseHandle,              IM_RetBool)
DEFINE_METHOD(SAFE_HANDLE,          DISPOSE,                Dispose,                    IM_RetVoid)
DEFINE_METHOD(SAFE_HANDLE,          DISPOSE_BOOL,           Dispose,                    IM_Bool_RetVoid)


DEFINE_CLASS(SAFE_CSHANDLE, Threading, SafeCompressedStackHandle)



DEFINE_CLASS(SBYTE,                 System,                 SByte)

DEFINE_CLASS(SECURITY_ACTION,       Permissions,            SecurityAction)
DEFINE_CLASS(HOST_PROTECTION_RESOURCE, Permissions,         HostProtectionResource)

DEFINE_CLASS(SECURITY_ATTRIBUTE,    Permissions,            SecurityAttribute)
DEFINE_METHOD(SECURITY_ATTRIBUTE, FIND_SECURITY_ATTRIBUTE_TYPE_HANDLE, FindSecurityAttributeTypeHandle, SM_Str_RetIntPtr)

DEFINE_CLASS(SECURITY_ELEMENT,      Security,               SecurityElement)
DEFINE_METHOD(SECURITY_ELEMENT,     TO_STRING,              ToString,                   IM_RetStr)

DEFINE_CLASS(SECURITY_ENGINE,       Security,               CodeAccessSecurityEngine)
DEFINE_METHOD(SECURITY_ENGINE,      CHECK_HELPER,           CheckHelper,                SM_CS_PMS_PMS_CodeAccessPermission_PermissionToken_RuntimeMethodHandle_Assembly_SecurityAction_RetVoid)
DEFINE_METHOD(SECURITY_ENGINE,      LAZY_CHECK_SET_HELPER,  LazyCheckSetHelper,         SM_PMS_IntPtr_RuntimeMethodHandle_Assembly_SecurityAction_RetVoid)
DEFINE_METHOD(SECURITY_ENGINE,      CHECK_SET_HELPER,       CheckSetHelper,             SM_CS_PMS_PMS_PMS_RuntimeMethodHandle_Assembly_SecurityAction_RetVoid)
DEFINE_METHOD(SECURITY_ENGINE,      THROW_SECURITY_EXCEPTION, ThrowSecurityException,   SM_Assembly_PMS_PMS_RuntimeMethodHandle_SecurityAction_Obj_IPermission_RetVoid)
DEFINE_METHOD(SECURITY_ENGINE,      UPDATE_APPDOMAIN_PLS,   UpdateAppDomainPLS,         SM_PermissionListSet_PMS_PMS_RetPermissionListSet)
DEFINE_METHOD(SECURITY_ENGINE,      GET_ZONE_AND_ORIGIN_HELPER, GetZoneAndOriginHelper, SM_CS_PMS_PMS_ArrayList_ArrayList_RetVoid)

DEFINE_CLASS(SECURITY_EXCEPTION,    Security,               SecurityException)
DEFINE_METHOD(SECURITY_EXCEPTION,   CTOR,                   .ctor,                      IM_Str_Type_Str_RetVoid)
DEFINE_METHOD(SECURITY_EXCEPTION,   CTOR2,                  .ctor,                      IM_PMS_PMS_RetVoid)

DEFINE_CLASS(HOST_PROTECTION_EXCEPTION, Security,         HostProtectionException)
DEFINE_METHOD(HOST_PROTECTION_EXCEPTION, CTOR,            .ctor,                        IM_HPR_HPR_RetVoid)

DEFINE_CLASS(SECURITY_MANAGER,      Security,               SecurityManager)
DEFINE_METHOD(SECURITY_MANAGER,     RESOLVE_POLICY,         ResolvePolicy,              SM_Evidence_PMS_PMS_PMS_PMS_int_Bool_RetPMS)
DEFINE_METHOD(SECURITY_MANAGER,     GET_SPECIAL_FLAGS,      GetSpecialFlags,            SM_PMS_PMS_RetInt)

DEFINE_CLASS(SECURITY_PERMISSION,   Permissions,            SecurityPermission)
DEFINE_METHOD(SECURITY_PERMISSION,  CTOR,                   .ctor,                      IM_SecurityPermissionFlag_RetVoid)
DEFINE_METHOD(SECURITY_PERMISSION,  TOXML,                  ToXml,                      IM_RetSecurityElement)
DEFINE_METHOD(SECURITY_PERMISSION,  METHOD_WITH_SKIP_VERIF_LINK_DEMAND, MethodWithSkipVerificationLinkDemand, SM_RetVoid)

DEFINE_CLASS(SECURITY_PERMISSION_FLAG,Permissions,          SecurityPermissionFlag)

DEFINE_CLASS(SECURITY_RUNTIME,      Security,               SecurityRuntime)
DEFINE_METHOD(SECURITY_RUNTIME,     FRAME_DESC_HELPER,      FrameDescHelper,            SM_FrameSecurityDescriptor_IPermission_PermissionToken_RuntimeMethodHandle_RetBool)
DEFINE_METHOD(SECURITY_RUNTIME,     FRAME_DESC_SET_HELPER,  FrameDescSetHelper,         SM_FrameSecurityDescriptor_PMS_OutPMS_RuntimeMethodHandle_RetBool)
DEFINE_METHOD(SECURITY_RUNTIME,     OVERRIDES_HELPER,       OverridesHelper,            SM_FrameSecurityDescriptor_RetInt)

DEFINE_CLASS(DOMAIN_COMPRESSED_STACK, Threading, DomainCompressedStack)
DEFINE_METHOD(DOMAIN_COMPRESSED_STACK, CREATE_MANAGED_OBJECT, CreateManagedObject, SM_IntPtr_RetDCS)
DEFINE_CLASS(SERVER_IDENTITY,       Remoting,               ServerIdentity)
DEFINE_FIELD(SERVER_IDENTITY,       SERVER_CONTEXT,         _srvCtx)

DEFINE_CLASS(COMPRESSED_STACK, Threading, CompressedStack)
DEFINE_METHOD(COMPRESSED_STACK,    RUN,                   Run,                SM_CompressedStack_ContextCallback_Object_RetVoid)


DEFINE_CLASS(SHARED_STATICS,        System,                 SharedStatics)
DEFINE_FIELD(SHARED_STATICS,        SHARED_STATICS,         _sharedStatics)

DEFINE_CLASS(SINGLE,                System,                 Single)

DEFINE_CLASS(STACK_BUILDER_SINK,    Messaging,              StackBuilderSink)
DEFINE_METHOD(STACK_BUILDER_SINK,   PRIVATE_PROCESS_MESSAGE,PrivateProcessMessage,      IM_RuntimeMethodHandle_ArrObj_Obj_Int_Bool_RefArrObj_RetObj)

DEFINE_CLASS_U(STACK_FRAME_HELPER,  Diagnostics,            StackFrameHelper,           StackFrameHelper)
DEFINE_FIELD_U(STACK_FRAME_HELPER,  TARGET_THREAD,          targetThread,               StackFrameHelper,   TargetThread)
DEFINE_FIELD_U(STACK_FRAME_HELPER,  OFFSETS,                rgiOffset,                  StackFrameHelper,   rgiOffset)
DEFINE_FIELD_U(STACK_FRAME_HELPER,  IL_OFFSETS,             rgiILOffset,                StackFrameHelper,   rgiILOffset)
DEFINE_FIELD_U(STACK_FRAME_HELPER,  METHOD_BASES,           rgMethodBase,               StackFrameHelper,   rgMethodBase)
DEFINE_FIELD_U(STACK_FRAME_HELPER,  DYNAMIC_METHODS,        dynamicMethods,             StackFrameHelper,   dynamicMethods)
DEFINE_FIELD_U(STACK_FRAME_HELPER,  METHOD_HANDLES,         rgMethodHandle,             StackFrameHelper,   rgMethodHandle)
DEFINE_FIELD_U(STACK_FRAME_HELPER,  FILENAMES,              rgFilename,                 StackFrameHelper,   rgFilename)
DEFINE_FIELD_U(STACK_FRAME_HELPER,  LINE_NUMBERS,           rgiLineNumber,              StackFrameHelper,   rgiLineNumber)
DEFINE_FIELD_U(STACK_FRAME_HELPER,  COLUMN_NUMBERS,         rgiColumnNumber,            StackFrameHelper,   rgiColumnNumber)
DEFINE_FIELD_U(STACK_FRAME_HELPER,  FRAME_COUNT,            iFrameCount,                StackFrameHelper,   iFrameCount)
DEFINE_FIELD_U(STACK_FRAME_HELPER,  NEED_FILE_INFO,         fNeedFileInfo,              StackFrameHelper,   fNeedFileInfo)

DEFINE_CLASS(STACK_TRACE,           Diagnostics,            StackTrace)
DEFINE_METHOD(STACK_TRACE,          GET_MANAGED_STACK_TRACE_HELPER, GetManagedStackTraceStringHelper, SM_Bool_RetStr)

DEFINE_CLASS(STREAM,                IO,                     Stream)

DEFINE_CLASS(STRING,                System,                 String)
DEFINE_FIELD(STRING,                M_FIRST_CHAR,           m_firstChar)
DEFINE_METHOD(STRING,               CREATE_STRING,          CreateString,               SM_PtrSByt_Int_Int_Encoding_RetStr)
DEFINE_METHOD(STRING,               CTOR_CHARPTR,           .ctor,                      IM_PtrChar_RetVoid)
DEFINE_METHOD(STRING,               CTORF_CHARARRAY,        CtorCharArray,              IM_ArrChar_RetStr)
DEFINE_METHOD(STRING,               CTORF_CHARARRAY_START_LEN,CtorCharArrayStartLength, IM_ArrChar_Int_Int_RetStr)
DEFINE_METHOD(STRING,               CTORF_CHAR_COUNT,       CtorCharCount,              IM_Char_Int_RetStr)
DEFINE_METHOD(STRING,               CTORF_CHARPTR,          CtorCharPtr,                IM_PtrChar_RetStr)
DEFINE_METHOD(STRING,               CTORF_CHARPTR_START_LEN,CtorCharPtrStartLength,     IM_PtrChar_Int_Int_RetStr)
DEFINE_METHOD(STRING,               INTERNAL_COPY,          InternalCopy,               SM_Str_IntPtr_Int_RetVoid)
DEFINE_METHOD(STRING,               WCSLEN,                 wcslen,                     SM_PtrChar_RetInt)
DEFINE_PROPERTY(STRING,             LENGTH,                 Length,                     Int)

DEFINE_CLASS_U(STRING_BUILDER,      Text,                   StringBuilder,              StringBufferObject)
DEFINE_FIELD(STRING_BUILDER,        STRING,                 m_StringValue)
DEFINE_FIELD_U(STRING_BUILDER,      STRING_U,               m_StringValue,              StringBufferObject,     m_orString)
DEFINE_FIELD_U(STRING_BUILDER,      CURRENT_THREAD,         m_currentThread,            StringBufferObject,     m_currentThread)
DEFINE_FIELD_U(STRING_BUILDER,      MAX_CAPACITY,           m_MaxCapacity,              StringBufferObject,     m_MaxCapacity)
DEFINE_PROPERTY(STRING_BUILDER,     LENGTH,                 Length,                     Int)
DEFINE_PROPERTY(STRING_BUILDER,     CAPACITY,               Capacity,                   Int)
DEFINE_METHOD(STRING_BUILDER,       CTOR_INT,               .ctor,                      IM_Int_RetVoid)

DEFINE_CLASS(STRONG_NAME_KEY_PAIR,  Reflection,             StrongNameKeyPair)
DEFINE_METHOD(STRONG_NAME_KEY_PAIR, GET_KEY_PAIR,           GetKeyPair,                 IM_RefObject_RetBool) 

DEFINE_CLASS_U(SYNCHRONIZATION_CONTEXT,    Threading,              SynchronizationContext, SynchronizationContextObject)
DEFINE_FIELD_U(SYNCHRONIZATION_CONTEXT, SYNCCTX_PROPS, _props, SynchronizationContextObject, _props)
DEFINE_METHOD(SYNCHRONIZATION_CONTEXT,  INVOKE_WAIT_METHOD_HELPER, InvokeWaitMethodHelper, SM_SyncCtx_ArrIntPtr_Bool_Int_RetInt)


DEFINE_CLASS(CONTEXTCALLBACK,       Threading,       ContextCallback)

DEFINE_CLASS_U(SECURITYCONTEXT,     Security,           SecurityContext,        SecurityContextObject)
DEFINE_FIELD_U(SECURITYCONTEXT,     EXECUTION_CONTEXT,  _executionContext,      SecurityContextObject,      _executionContext)
DEFINE_FIELD_U(SECURITYCONTEXT,     COMPRESSED_STACK,   _compressedStack,       SecurityContextObject,      _compressedStack)
DEFINE_FIELD_U(SECURITYCONTEXT,     DISABLE_FLOW,       _disableFlow,           SecurityContextObject,      _disableFlow)
DEFINE_FIELD_U(SECURITYCONTEXT,     IS_NEW_CAPTURE,     isNewCapture,           SecurityContextObject,      _isNewCapture)
DEFINE_METHOD(SECURITYCONTEXT,               RUN,                   Run,                SM_SecurityContext_ContextCallback_Object_RetVoid)

DEFINE_CLASS_U(EXECUTIONCONTEXT,    Threading,                  ExecutionContext,       ExecutionContextObject)
DEFINE_FIELD_U(EXECUTIONCONTEXT,    HOST_CONTEXT,               _hostExecutionContext,  ExecutionContextObject,     _hostExecutionContext)
DEFINE_FIELD_U(EXECUTIONCONTEXT,    SYNC_CONTEXT,               _syncContext,           ExecutionContextObject,     _syncContext)
DEFINE_FIELD_U(EXECUTIONCONTEXT,    SECURITY_CONTEXT,           _securityContext,       ExecutionContextObject,     _securityContext)
DEFINE_FIELD_U(EXECUTIONCONTEXT,    LOGICAL_CALL_CONTEXT,       _logicalCallContext,    ExecutionContextObject,     _logicalCallContext)
DEFINE_FIELD_U(EXECUTIONCONTEXT,    ILLOGICAL_CALL_CONTEXT,     _illogicalCallContext,  ExecutionContextObject,     _illogicalCallContext)
DEFINE_FIELD_U(EXECUTIONCONTEXT,    THREAD,                     _thread,                ExecutionContextObject,     _threadBaseObject)
DEFINE_FIELD_U(EXECUTIONCONTEXT,    IS_NEW_CAPTURE,             isNewCapture,           ExecutionContextObject,     _isNewCapture)
DEFINE_FIELD_U(EXECUTIONCONTEXT,    IS_FLOW_SUPRESSED,          isFlowSuppressed,       ExecutionContextObject,     _isFlowSuppressed)
DEFINE_METHOD(EXECUTIONCONTEXT,               RUN,                   Run,                SM_ExecutionContext_ContextCallback_Object_RetVoid)

DEFINE_CLASS(CROSS_CONTEXT_DELEGATE, Threading, InternalCrossContextDelegate)

DEFINE_CLASS_U(THREAD,              Threading,              Thread,                     ThreadBaseObject)
DEFINE_FIELD_U(THREAD,              CONTEXT,                m_Context,                  ThreadBaseObject,   m_ExposedContext)
DEFINE_FIELD_U(THREAD,              EXECUTIONCONTEXT,       m_ExecutionContext,         ThreadBaseObject,   m_ExecutionContext)
DEFINE_FIELD_U(THREAD,              NAME,                   m_Name,                     ThreadBaseObject,   m_Name)
DEFINE_FIELD_U(THREAD,              DELEGATE,               m_Delegate,                 ThreadBaseObject,   m_Delegate)
DEFINE_FIELD_U(THREAD,              THREAD_STATICS_BUCKETS, m_ThreadStaticsBuckets,     ThreadBaseObject,   m_ThreadStaticsBuckets)
DEFINE_FIELD_U(THREAD,              THREAD_STATICS_BITS,    m_ThreadStaticsBits,        ThreadBaseObject,   m_ThreadStaticsBits)
DEFINE_FIELD_U(THREAD,              CULTURE,                m_CurrentCulture,           ThreadBaseObject,   m_CurrentUserCulture)
DEFINE_FIELD_U(THREAD,              UI_CULTURE,             m_CurrentUICulture,         ThreadBaseObject,   m_CurrentUICulture)
DEFINE_FIELD_U(THREAD,              START_ARG,              m_ThreadStartArg,           ThreadBaseObject,   m_ThreadStartArg)
DEFINE_FIELD_U(THREAD,              INTERNAL_THREAD,        DONT_USE_InternalThread,    ThreadBaseObject,   m_InternalThread)
DEFINE_FIELD_U(THREAD,              PRIORITY,               m_Priority,                 ThreadBaseObject,   m_Priority)
DEFINE_METHOD(THREAD,               SET_PRINCIPAL_INTERNAL, SetPrincipalInternal,       IM_IPrincipal_RetVoid)
DEFINE_METHOD(THREAD,               REMOVE_DLS,             RemoveDomainLocalStore,     SM_LocalDataStore_RetVoid)
DEFINE_STATIC_PROPERTY(THREAD,      CURRENT_CONTEXT,        CurrentContext,             Context)
DEFINE_METHOD(THREAD,               RESERVE_SLOT,           ReserveSlot,                IM_RetInt)
DEFINE_SET_PROPERTY(THREAD,         CULTURE,                CurrentCulture,             CultureInfo)
DEFINE_SET_PROPERTY(THREAD,         UI_CULTURE,             CurrentUICulture,           CultureInfo)
DEFINE_SET_PROPERTY(THREAD,         NAME,                   Name,                       Str)
DEFINE_STATIC_PROPERTY(THREAD,      CURRENT_THREAD,         CurrentThread,              Thread)
DEFINE_METHOD(THREAD,               COMPLETE_CROSSCONTEXTCALLBACK,           CompleteCrossContextCallback,                SM_CrossContextDelegate_ArrObj_RetObj)

DEFINE_CLASS(PARAMETERIZEDTHREADSTART,     Threading,                 ParameterizedThreadStart)

DEFINE_CLASS(IOCB_HELPER,              Threading,            _IOCompletionCallback)
DEFINE_METHOD(IOCB_HELPER,             PERFORM_IOCOMPLETION_CALLBACK,        PerformIOCompletionCallback,          SM_UInt_UInt_PtrNativeOverlapped_RetVoid)

DEFINE_CLASS(TPWAITORTIMER_HELPER,              Threading,            _ThreadPoolWaitOrTimerCallback)
DEFINE_METHOD(TPWAITORTIMER_HELPER,             PERFORM_WAITORTIMER_CALLBACK,        PerformWaitOrTimerCallback,          SM_Obj_Bool_RetVoid)

DEFINE_CLASS(TPWAIT_HELPER,              Threading,            _ThreadPoolWaitCallback)
DEFINE_METHOD(TPWAIT_HELPER,             PERFORM_WAIT_CALLBACK,        PerformWaitCallback,          SM_Obj_RetVoid)

DEFINE_CLASS(TPTIMER_HELPER,              Threading,            _TimerCallback)
DEFINE_METHOD(TPTIMER_HELPER,             PERFORM_TIMER_CALLBACK,        PerformTimerCallback,          SM_Obj_RetVoid)

DEFINE_CLASS(TIMESPAN,              System,                 TimeSpan)

DEFINE_CLASS_U(TRANSPARENT_PROXY,   Proxies,                __TransparentProxy,         TransparentProxyObject)
DEFINE_FIELD_U(TRANSPARENT_PROXY,   RP,                     _rp,                        TransparentProxyObject, _rp)
DEFINE_FIELD_U(TRANSPARENT_PROXY,   MT,                     _pMT,                       TransparentProxyObject, _pMT)
DEFINE_FIELD_U(TRANSPARENT_PROXY,   INTERFACE_MT,           _pInterfaceMT,              TransparentProxyObject, _pInterfaceMT)
DEFINE_FIELD_U(TRANSPARENT_PROXY,   STUB,                   _stub,                      TransparentProxyObject, _stub)
DEFINE_FIELD_U(TRANSPARENT_PROXY,   STUB_DATA,              _stubData,                  TransparentProxyObject, _stubData)

DEFINE_CLASS(TYPE,                  System,                 Type)

DEFINE_CLASS(TYPE_DELEGATOR,        Reflection,             TypeDelegator)

DEFINE_CLASS(TYPE_HANDLE,           System,                 RuntimeTypeHandle)

DEFINE_CLASS(TYPED_REFERENCE,       System,                 TypedReference)

DEFINE_CLASS(UINT16,                System,                 UInt16)

DEFINE_CLASS(UINT32,                System,                 UInt32)

DEFINE_CLASS(UINT64,                System,                 UInt64)

DEFINE_CLASS(UI_PERMISSION,         Permissions,            UIPermission)
DEFINE_METHOD(UI_PERMISSION,        CTOR,                   .ctor,                      IM_PermissionState_RetVoid)

DEFINE_CLASS(UNHANDLED_EVENTARGS,   System,                 UnhandledExceptionEventArgs)
DEFINE_METHOD(UNHANDLED_EVENTARGS,  CTOR,                   .ctor,                      IM_Obj_Bool_RetVoid)

DEFINE_CLASS(UNHANDLED_HANDLER,     System,                 UnhandledExceptionEventHandler)
DEFINE_METHOD(UNHANDLED_HANDLER,    INVOKE,                 Invoke,                     IM_Obj_UnhandledExceptionEventArgs_RetVoid)



DEFINE_CLASS(VALUE_TYPE,            System,                 ValueType)

DEFINE_CLASS(VARIANT,               System,                 Variant)
DEFINE_METHOD(VARIANT,              CONVERT_OBJECT_TO_VARIANT,MarshalHelperConvertObjectToVariant,SM_Obj_RefVariant_RetVoid)
DEFINE_METHOD(VARIANT,              CAST_VARIANT,           MarshalHelperCastVariant,   SM_Obj_Int_RefVariant_RetVoid)
DEFINE_METHOD(VARIANT,              CONVERT_VARIANT_TO_OBJECT,MarshalHelperConvertVariantToObject,SM_RefVariant_RetObject)


DEFINE_CLASS_U(VERSION,             System,                 Version,                    VersionBaseObject)
DEFINE_FIELD_U(VERSION,             MAJOR,                  _Major,                     VersionBaseObject,    m_Major)
DEFINE_FIELD_U(VERSION,             MINOR,                  _Minor,                     VersionBaseObject,    m_Minor)
DEFINE_FIELD_U(VERSION,             BUILD,                  _Build,                     VersionBaseObject,    m_Build)
DEFINE_FIELD_U(VERSION,             REVISION,               _Revision,                  VersionBaseObject,    m_Revision)
DEFINE_METHOD(VERSION,              CTOR,                   .ctor,                      IM_Int_Int_Int_Int_RetVoid)

DEFINE_CLASS(VOID,                  System,                 Void)


DEFINE_CLASS(GC,                    System,                 GC)
DEFINE_METHOD(GC,                   FIRE_CACHE_EVENT,       FireCacheEvent,             SM_RetVoid)
DEFINE_METHOD(GC,                   KEEP_ALIVE,             KeepAlive,                  SM_Obj_RetVoid)

DEFINE_CLASS(INTPTR,                System,                 IntPtr)
DEFINE_FIELD(INTPTR,                VALUE,                  m_value)

DEFINE_CLASS(UINTPTR,               System,                 UIntPtr)
DEFINE_FIELD(UINTPTR,               VALUE,                  m_value)

DEFINE_CLASS_U(WEAKREFERENCE,       System,                 WeakReference,          WeakReferenceObject)
DEFINE_FIELD_U(WEAKREFERENCE,       HANDLE,                 m_handle,               WeakReferenceObject,    m_Handle)
DEFINE_FIELD_U(WEAKREFERENCE,       ISLONGREFERENCE,        m_IsLongReference,      WeakReferenceObject,    m_IsLongReference)

DEFINE_CLASS_U(WAITHANDLE,          Threading,              WaitHandle,             WaitHandleBase)
DEFINE_FIELD_U(WAITHANDLE,          SAFEHANDLE,             safeWaitHandle,         WaitHandleBase,         m_safeHandle)
DEFINE_FIELD_U(WAITHANDLE,          HANDLE,                 waitHandle,             WaitHandleBase,         m_handle)
DEFINE_FIELD_U(WAITHANDLE,          HAS_THREAD_AFFINITY,    hasThreadAffinity,      WaitHandleBase,         m_hasThreadAffinity)

DEFINE_CLASS(DEBUGGER,              Diagnostics,            Debugger)
DEFINE_METHOD(DEBUGGER,             BREAK_CAN_THROW,        BreakCanThrow,          SM_RetVoid)



DEFINE_CLASS(SZARRAYHELPER,         System,                        SZArrayHelper)
// Note: The order of methods here has to match order they are implemented on the interfaces in
// IEnumerable`1
DEFINE_METHOD(SZARRAYHELPER,        GETENUMERATOR,          GetEnumerator,              NoSig)
// ICollection`1
DEFINE_METHOD(SZARRAYHELPER,        GET_COUNT,              get_Count,                  NoSig)
DEFINE_METHOD(SZARRAYHELPER,        ISREADONLY,             get_IsReadOnly,             NoSig)
DEFINE_METHOD(SZARRAYHELPER,        ADD,                    Add,                        NoSig)
DEFINE_METHOD(SZARRAYHELPER,        CLEAR,                  Clear,                      NoSig)
DEFINE_METHOD(SZARRAYHELPER,        CONTAINS,               Contains,                   NoSig)
DEFINE_METHOD(SZARRAYHELPER,        COPYTO,                 CopyTo,                     NoSig)
DEFINE_METHOD(SZARRAYHELPER,        REMOVE,                 Remove,                     NoSig)
// IList`1
DEFINE_METHOD(SZARRAYHELPER,        GET_ITEM,               get_Item,                   NoSig)
DEFINE_METHOD(SZARRAYHELPER,        SET_ITEM,               set_Item,                   NoSig)
DEFINE_METHOD(SZARRAYHELPER,        INDEXOF,                IndexOf,                    NoSig)
DEFINE_METHOD(SZARRAYHELPER,        INSERT,                 Insert,                     NoSig)
DEFINE_METHOD(SZARRAYHELPER,        REMOVEAT,               RemoveAt,                   NoSig)

DEFINE_CLASS(IENUMERABLEGENERIC,    CollectionsGeneric,     IEnumerable`1)
DEFINE_CLASS(ICOLLECTIONGENERIC,    CollectionsGeneric,     ICollection`1)
DEFINE_CLASS(ILISTGENERIC,          CollectionsGeneric,     IList`1)

#undef DEFINE_CLASS
#undef DEFINE_METHOD
#undef DEFINE_FIELD
#undef DEFINE_CLASS_U
#undef DEFINE_FIELD_U
