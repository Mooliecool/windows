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
// ECALL.CPP -
//
// Handles our private native calling interface.
//

#include "common.h"
#include "vars.hpp"
#include "object.h"
#include "util.hpp"
#include "stublink.h"
#include "cgensys.h"
#include "ecall.h"
#include "excep.h"
#include "jitinterface.h"
#include "security.h"
#include "securitydescriptor.h"
#include "dllimport.h"
#include "comarrayinfo.h"
#include "comstring.h"
#include "comstringbuffer.h"
#include "securityimperative.h"
#include "securitystackwalk.h"
#include "comobject.h"
#include "comdelegate.h"
#include "customattribute.h"
#include "comdynamic.h"
#include "commethodrental.h"
#include "comnlsinfo.h"
#include "commodule.h"
#include "marshalnative.h"
#include "comsystem.h"
#include "comutilnative.h"
#include "comsynchronizable.h"
#include "comfloatclass.h"
#include "comstreams.h"
#include "comvariant.h"
#include "comdecimal.h"
#include "comcurrency.h"
#include "comdatetime.h"
#include "comisolatedstorage.h"
#include "securityconfig.h"
#include "array.h"
#include "comnumber.h"
#include "remoting.h"
#include "debugdebugger.h"
#include "message.h"
#include "stackbuildersink.h"
#include "remoting.h"
#include "assemblyname.hpp"
#include "assemblynative.hpp"
#include "rwlock.h"
#include "comthreadpool.h"
#include "comwaithandle.h"
#include "nativeoverlapped.h"
#include "jitinterface.h"
#include "proftoeeinterfaceimpl.h"
#include "eeconfig.h"
#include "appdomainnative.hpp"
#include "handletablepriv.h"
#include "objecthandle.h"
#include "confighelper.h"
#include "comarrayhelpers.h"
#include "ceeload.h"
#include "method.hpp"
#include "runtimehandles.h"
#include "reflectioninvocation.h"
#include "managedmdimport.hpp"
#include "synchronizationcontextnative.h"
#include "stackcompressor.h"
#include "newcompressedstack.h"
#include "commemoryfailpoint.h"
#include "typestring.h"


#include "fusionwrap.h"



#include "hostexecutioncontext.h"

#ifndef DACCESS_COMPILE

ArrayStubCache *ECall::m_pArrayStubCache = NULL;

#define FCFuncFlags(intrinsicID, dynamicID) \
    (BYTE*)( (((BYTE)intrinsicID) << 16) + (((BYTE)dynamicID) << 24) )

#define FCFuncElement(name, impl) FCFuncFlags(CORINFO_INTRINSIC_Illegal, ECall::InvalidDynamicFCallId), \
    GetEEFuncEntryPoint(impl), NULL, NULL, (LPVOID)name,

#define FCFuncElementSig(name,sig,impl) \
    FCFuncFlag_HasSignature + FCFuncElement(name, impl) (LPVOID)sig,

#define FCIntrinsic(name,impl,intrinsicID) FCFuncFlags(intrinsicID, ECall::InvalidDynamicFCallId), \
    GetEEFuncEntryPoint(impl), NULL, NULL, (LPVOID)name,

#define FCIntrinsicSig(name,sig,impl,intrinsicID) \
    FCFuncFlag_HasSignature + FCIntrinsic(name,impl,intrinsicID) (LPVOID)sig,

#define FCDynamic(name,intrinsicID,dynamicID) FCFuncFlags(intrinsicID, dynamicID), \
    NULL, NULL, NULL, (LPVOID)name,

#define FCDynamicSig(name,sig,intrinsicID,dynamicID) \
    FCFuncFlag_HasSignature + FCDynamic(name,intrinsicID,dynamicID) (LPVOID)sig,

#define FCFuncStart(name) static LPVOID name[] = {
#define FCFuncEnd() FCFuncFlag_EndOfArray + FCFuncFlags(CORINFO_INTRINSIC_Illegal, ECall::InvalidDynamicFCallId) };


FCFuncStart(gRemotingFuncs)
    FCFuncElement("IsTransparentProxy", CRemotingServices::FCIsTransparentProxy)
    FCFuncElement("GetRealProxy", CRemotingServices::FCGetRealProxy)
    FCFuncElement("Unwrap", CRemotingServices::FCUnwrap)
    FCFuncElement("AlwaysUnwrap", CRemotingServices::FCAlwaysUnwrap)
    FCFuncElement("CheckCast", CRemotingServices::NativeCheckCast)
    FCFuncElement("nSetRemoteActivationConfigured", CRemotingServices::SetRemotingConfiguredFlag)
    FCFuncElement("CORProfilerTrackRemoting", ProfilingFCallHelper::FC_TrackRemoting)
    FCFuncElement("CORProfilerTrackRemotingCookie", ProfilingFCallHelper::FC_TrackRemotingCookie)
    FCFuncElement("CORProfilerTrackRemotingAsync", ProfilingFCallHelper::FC_TrackRemotingAsync)
    FCFuncElement("CORProfilerRemotingClientSendingMessage", ProfilingFCallHelper::FC_RemotingClientSendingMessage)
    FCFuncElement("CORProfilerRemotingClientReceivingReply", ProfilingFCallHelper::FC_RemotingClientReceivingReply)
    FCFuncElement("CORProfilerRemotingServerReceivingMessage", ProfilingFCallHelper::FC_RemotingServerReceivingMessage)
    FCFuncElement("CORProfilerRemotingServerSendingReply", ProfilingFCallHelper::FC_RemotingServerSendingReply)
    FCFuncElement("CreateTransparentProxy", CRemotingServices::CreateTransparentProxy)
    FCFuncElement("AllocateUninitializedObject", CRemotingServices::AllocateUninitializedObject)
    FCFuncElement("CallDefaultCtor", CRemotingServices::CallDefaultCtor)
    FCFuncElement("AllocateInitializedObject", CRemotingServices::AllocateInitializedObject)
    FCFuncElement("ResetInterfaceCache", CRemotingServices::ResetInterfaceCache)
FCFuncEnd()

FCFuncStart(gRealProxyFuncs)
    FCFuncElement("SetStubData", CRealProxy::SetStubData)
    FCFuncElement("GetStubData", CRealProxy::GetStubData)
    FCFuncElement("GetStub", CRealProxy::GetStub)
    FCFuncElement("GetDefaultStub", CRealProxy::GetDefaultStub)
    FCFuncElement("GetProxiedType", CRealProxy::GetProxiedType)
FCFuncEnd()

FCFuncStart(gContextFuncs)
    FCFuncElement("SetupInternalContext", Context::SetupInternalContext)
    FCFuncElement("CleanupInternalContext", Context::CleanupInternalContext)
    FCFuncElement("ExecuteCallBackInEE", Context::ExecuteCallBack)
FCFuncEnd()

FCFuncStart(gRWLockFuncs)
    FCFuncElement("AcquireReaderLockInternal",  CRWLock::StaticAcquireReaderLockPublic)
    FCFuncElement("AcquireWriterLockInternal",  CRWLock::StaticAcquireWriterLockPublic)
    FCFuncElement("ReleaseReaderLockInternal",  CRWLock::StaticReleaseReaderLockPublic)
    FCFuncElement("ReleaseWriterLockInternal",  CRWLock::StaticReleaseWriterLockPublic)
    FCFuncElement("FCallUpgradeToWriterLock",  CRWLock::StaticDoUpgradeToWriterLockPublic)
    FCFuncElement("DowngradeFromWriterLockInternal",  CRWLock::StaticDowngradeFromWriterLock)
    FCFuncElement("FCallReleaseLock",  CRWLock::StaticDoReleaseLock)
    FCFuncElement("RestoreLockInternal",  CRWLock::StaticRestoreLockPublic)
    FCFuncElement("PrivateGetIsReaderLockHeld",  CRWLock::StaticIsReaderLockHeld)
    FCFuncElement("PrivateGetIsWriterLockHeld",  CRWLock::StaticIsWriterLockHeld)
    FCFuncElement("PrivateGetWriterSeqNum",  CRWLock::StaticGetWriterSeqNum)
    FCFuncElement("AnyWritersSince",  CRWLock::StaticAnyWritersSince)
    FCFuncElement("PrivateInitialize",  CRWLock::StaticPrivateInitialize)
    FCFuncElement("PrivateDestruct",  CRWLock::StaticPrivateDestruct)
FCFuncEnd()

FCFuncStart(gMessageFuncs)
    FCFuncElement("InternalGetArgCount", CMessage::GetArgCount)
    FCFuncElement("InternalHasVarArgs", CMessage::HasVarArgs)
    FCFuncElement("InternalGetArg", CMessage::GetArg)
    FCFuncElement("InternalGetArgs", CMessage::GetArgs)
    FCFuncElement("PropagateOutParameters", CMessage::PropagateOutParameters)
    FCFuncElement("GetReturnValue", CMessage::GetReturnValue)
    FCFuncElement("GetAsyncBeginInfo", CMessage::GetAsyncBeginInfo)
    FCFuncElement("GetAsyncResult", CMessage::GetAsyncResult)
    FCFuncElement("GetThisPtr", CMessage::GetAsyncObject)
    FCFuncElement("OutToUnmanagedDebugger", CMessage::DebugOut)
    FCFuncElement("Dispatch", CMessage::Dispatch)
FCFuncEnd()

FCFuncStart(gConvertFuncs)
    FCFuncElement("FromBase64String", BitConverter::Base64StringToByteArray)
    FCFuncElement("FromBase64CharArray", BitConverter::Base64CharArrayToByteArray)
FCFuncEnd()

FCFuncStart(gChannelServicesFuncs)
    FCFuncElement("GetPrivateContextsPerfCounters", GetPrivateContextsPerfCountersEx)
FCFuncEnd()

FCFuncStart(gEnumFuncs)
    FCFuncElement("InternalGetUnderlyingType",  ReflectionEnum::InternalGetEnumUnderlyingType)
    FCFuncElement("InternalGetValue",  ReflectionEnum::InternalGetEnumValue)
    FCFuncElement("InternalGetEnumValues",  ReflectionEnum::InternalGetEnumValues)
    FCFuncElement("InternalBoxEnum", ReflectionEnum::InternalBoxEnum)
    FCFuncElement("Equals", ReflectionEnum::InternalEquals)
    FCFuncElement("InternalCompareTo", ReflectionEnum::InternalCompareTo)    
FCFuncEnd()

FCFuncStart(gStackBuilderSinkFuncs)
    FCFuncElement("_PrivateProcessMessage", CStackBuilderSink::PrivateProcessMessage)
FCFuncEnd()

#include "comvarargs.h"

FCFuncStart(gParseNumbersFuncs)
    FCFuncElement("IntToString", ParseNumbers::IntToString)
    FCFuncElement("LongToString", ParseNumbers::LongToString)
    FCFuncElement("StringToInt", ParseNumbers::StringToInt)
    FCFuncElement("StringToLong", ParseNumbers::StringToLong)
FCFuncEnd()

FCFuncStart(gTimeZoneFuncs)
    FCFuncElement("nativeGetTimeZoneMinuteOffset", COMNlsInfo::nativeGetTimeZoneMinuteOffset)
    FCFuncElement("nativeGetStandardName", COMNlsInfo::nativeGetStandardName)
    FCFuncElement("nativeGetDaylightName", COMNlsInfo::nativeGetDaylightName)
    FCFuncElement("nativeGetDaylightChanges", COMNlsInfo::nativeGetDaylightChanges)
FCFuncEnd()

FCFuncStart(gGuidFuncs)
    FCFuncElement("CompleteGuid", GuidNative::CompleteGuid)
FCFuncEnd()


FCFuncStart(gObjectFuncs)
    FCIntrinsic("GetType", ObjectNative::GetClass, CORINFO_INTRINSIC_Object_GetType)
    FCFuncElement("InternalGetHashCode", ObjectNative::GetHashCode)
    FCFuncElement("InternalEquals", ObjectNative::Equals)
    FCFuncElement("MemberwiseClone", ObjectNative::Clone)
FCFuncEnd()

FCFuncStart(gStringFuncs)
    FCDynamic("FastAllocateString", CORINFO_INTRINSIC_Illegal, ECall::FastAllocateString)
    FCDynamicSig(COR_CTOR_METHOD_NAME, &gsig_IM_ArrChar_RetVoid, CORINFO_INTRINSIC_Illegal, ECall::CtorCharArrayManaged)
    FCDynamicSig(COR_CTOR_METHOD_NAME, &gsig_IM_ArrChar_Int_Int_RetVoid, CORINFO_INTRINSIC_Illegal, ECall::CtorCharArrayStartLengthManaged)
    FCDynamicSig(COR_CTOR_METHOD_NAME, &gsig_IM_PtrChar_RetVoid, CORINFO_INTRINSIC_Illegal, ECall::CtorCharPtrManaged)
    FCDynamicSig(COR_CTOR_METHOD_NAME, &gsig_IM_PtrChar_Int_Int_RetVoid, CORINFO_INTRINSIC_Illegal, ECall::CtorCharPtrStartLengthManaged)
    FCDynamicSig(COR_CTOR_METHOD_NAME, &gsig_IM_Char_Int_RetVoid, CORINFO_INTRINSIC_Illegal, ECall::CtorCharCountManaged)

    FCFuncElement("nativeCompareOrdinal", COMString::FCCompareOrdinal)
    FCFuncElement("nativeCompareOrdinalWC", COMString::FCCompareOrdinalWC)
    FCIntrinsic("get_Length", COMString::Length, CORINFO_INTRINSIC_StringLength)
    FCIntrinsic("get_Chars", COMString::GetCharAt, CORINFO_INTRINSIC_StringGetChar)
    FCFuncElement("IsFastSort", COMString::IsFastSort)
    FCFuncElement("IsAscii", COMString::IsAscii)
#ifdef _DEBUG
    FCFuncElement("ValidModifiableString", COMString::ValidModifiableString)
#endif
    FCFuncElement("nativeCompareOrdinalEx", COMString::CompareOrdinalEx)
    FCFuncElement("IndexOf", COMString::IndexOfChar)
    FCFuncElement("IndexOfAny", COMString::IndexOfCharArray)
    FCFuncElement("LastIndexOf", COMString::LastIndexOfChar)
    FCFuncElement("LastIndexOfAny", COMString::LastIndexOfCharArray)
    FCFuncElementSig(COR_CTOR_METHOD_NAME, &gsig_IM_PtrSByt_RetVoid, COMString::StringInitCharPtr)
    FCFuncElementSig(COR_CTOR_METHOD_NAME, &gsig_IM_PtrSByt_Int_Int_RetVoid, COMString::StringInitCharPtrPartial)
    FCFuncElementSig(COR_CTOR_METHOD_NAME, &gsig_IM_PtrSByt_Int_Int_Encoding_RetVoid, COMString::StringInitSBytPtrPartialEx)
    FCFuncElement("Remove", COMString::Remove)
    FCFuncElementSig("Replace", &gsig_IM_Char_Char_RetStr, COMString::Replace)
    FCFuncElementSig("Replace", &gsig_IM_Str_Str_RetStr, COMString::ReplaceString)
    FCFuncElement("Insert", COMString::Insert)
    FCFuncElement("PadHelper", COMString::PadHelper)
FCFuncEnd()

FCFuncStart(gStringBufferFuncs)
    FCFuncElement("Replace", COMStringBuffer::ReplaceString)
FCFuncEnd()

FCFuncStart(gValueTypeFuncs)
    FCFuncElement("CanCompareBits", ValueTypeHelper::CanCompareBits)
    FCFuncElement("FastEqualsCheck", ValueTypeHelper::FastEqualsCheck)
    FCFuncElement("GetHashCode", ValueTypeHelper::GetHashCode)
FCFuncEnd()

FCFuncStart(gDiagnosticsDebugger)
    FCFuncElement("BreakInternal", DebugDebugger::Break)
    FCFuncElement("LaunchInternal", DebugDebugger::Launch)
    FCFuncElement("IsDebuggerAttached", DebugDebugger::IsDebuggerAttached)
    FCFuncElement("Log", DebugDebugger::Log)
    FCFuncElement("IsLogging", DebugDebugger::IsLogging)
FCFuncEnd()

FCFuncStart(gDiagnosticsStackTrace)
    FCFuncElement("GetStackFramesInternal", DebugStackTrace::GetStackFramesInternal)
FCFuncEnd()

FCFuncStart(gDiagnosticsLog)
    FCFuncElement("AddLogSwitch", Log::AddLogSwitch)
    FCFuncElement("ModifyLogSwitch", Log::ModifyLogSwitch)
FCFuncEnd()

FCFuncStart(gDiagnosticsAssert)
    FCFuncElement("ShowDefaultAssertDialog", DebuggerAssert::ShowDefaultAssertDialog)
FCFuncEnd()

FCFuncStart(gDateTimeFuncs)
    FCFuncElement("GetSystemTimeAsFileTime", SystemNative::__GetSystemTimeAsFileTime)
FCFuncEnd()


FCFuncStart(gEnvironmentFuncs)
    FCFuncElement("nativeGetTickCount", SystemNative::GetTickCount)
    FCFuncElement("ExitNative", SystemNative::Exit)
    FCFuncElement("nativeSetExitCode", SystemNative::SetExitCode)
    FCFuncElement("nativeGetExitCode", SystemNative::GetExitCode)
    FCFuncElement("nativeGetWorkingSet", SystemNative::GetWorkingSet)
    FCFuncElement("nativeGetEnvironmentVariable", SystemNative::_GetEnvironmentVariable)
    FCFuncElement("nativeHasShutdownStarted", SystemNative::HasShutdownStarted)
    FCFuncElement("GetCompatibilityFlag", SystemNative::_GetCompatibilityFlag)
    FCFuncElement("nativeGetEnvironmentCharArray", SystemNative::GetEnvironmentCharArray)
    FCFuncElement("GetCommandLineNative", SystemNative::_GetCommandLine)
    FCFuncElement("GetCommandLineArgsNative", SystemNative::GetCommandLineArgs)
    FCFuncElement("GetResourceFromDefault", GetResourceFromDefault)
    FCFuncElement("nativeIsWin9x", SystemNative::IsWin9x)
    FCFuncElement("FailFast", SystemNative::FailFast)
FCFuncEnd()

FCFuncStart(gRuntimeEnvironmentFuncs)
    FCFuncElement("GetModuleFileName", SystemNative::_GetModuleFileName)
    FCFuncElement("GetDeveloperPath", SystemNative::GetDeveloperPath)
    FCFuncElement("GetRuntimeDirectoryImpl", SystemNative::GetRuntimeDirectory)
    FCFuncElement("GetHostBindingFile", SystemNative::GetHostBindingFile)
    FCFuncElement("FromGlobalAccessCache", SystemNative::FromGlobalAccessCache)
FCFuncEnd()

FCFuncStart(gSerializationFuncs)
    FCFuncElement("nativeGetSafeUninitializedObject", ReflectionSerialization::GetSafeUninitializedObject)
    FCFuncElement("nativeGetUninitializedObject", ReflectionSerialization::GetUninitializedObject)
FCFuncEnd()

FCFuncStart(gExceptionFuncs)
    FCFuncElement("GetClassName", ExceptionNative::GetClassName)
    FCFuncElement("IsImmutableAgileException", ExceptionNative::IsImmutableAgileException)
    FCFuncElement("_InternalGetMethod", SystemNative::CaptureStackTraceMethod)
    FCFuncElement("nIsTransient", ExceptionNative::IsTransient)
    FCFuncElement("GetMessageFromNativeResources", ExceptionNative::GetMessageFromNativeResources)
FCFuncEnd()

FCFuncStart(gSafeHandleFuncs)
    FCFuncElement("InternalDispose", SafeHandle::DisposeNative)
    FCFuncElement("InternalFinalize", SafeHandle::Finalize)
    FCFuncElement("SetHandleAsInvalid", SafeHandle::SetHandleAsInvalid)
    FCFuncElement("DangerousAddRef", SafeHandle::DangerousAddRef)
    FCFuncElement("DangerousRelease", SafeHandle::DangerousRelease)
FCFuncEnd()

FCFuncStart(gCriticalHandleFuncs)
    FCFuncElement("FireCustomerDebugProbe", CriticalHandle::FireCustomerDebugProbe)
FCFuncEnd()

FCFuncStart(gPathFuncs)
FCFuncEnd()

FCFuncStart(gFusionWrapFuncs)
    FCFuncElement("GetNextAssembly",  FusionWrap::GetNextAssembly)
    FCFuncElement("GetDisplayName",  FusionWrap::GetDisplayName)
    FCFuncElement("ReleaseFusionHandle",  FusionWrap::ReleaseFusionHandle)
FCFuncEnd()

FCFuncStart(gNormalizationFuncs)
    FCFuncElement("nativeLoadNormalizationDLL", COMNlsInfo::nativeLoadNormalizationDLL)
    FCFuncElement("nativeNormalizationIsNormalizedString", COMNlsInfo::nativeNormalizationIsNormalizedString)
    FCFuncElement("nativeNormalizationNormalizeString", COMNlsInfo::nativeNormalizationNormalizeString)
    FCFuncElement("nativeNormalizationInitNormalization", COMNlsInfo::nativeNormalizationInitNormalization)
FCFuncEnd()

FCFuncStart(gTypedReferenceFuncs)
    FCFuncElement("InternalToObject", ReflectionInvocation::TypedReferenceToObject)
    FCFuncElement("InternalSetTypedReference", ReflectionInvocation::SetTypedReference)
    FCFuncElement("InternalMakeTypedReference", ReflectionInvocation::MakeTypedReference)
FCFuncEnd()

FCFuncStart(gSystem_Type)
    FCFuncElement("GetTypeHandle", RuntimeTypeHandle::GetTrueTypeHandle)
    FCIntrinsic("GetTypeFromHandle", RuntimeTypeHandle::GetRuntimeType, CORINFO_INTRINSIC_GetClassFromHandle)
    FCIntrinsic("get_TypeHandle", RuntimeTypeHandle::TypeHandle_get, CORINFO_INTRINSIC_Type_TypeHandle_get)
FCFuncEnd()

FCFuncStart(gSystem_RuntimeType)
    FCFuncElement("GetGUID", ReflectionInvocation::GetGUID)
    FCFuncElement("_CreateEnum", ReflectionInvocation::CreateEnum)
    FCFuncElement("CanValueSpecialCast", ReflectionInvocation::CanValueSpecialCast)
    FCFuncElement("AllocateObjectForByRef", ReflectionInvocation::AllocateObjectForByRef)
    FCFuncElement("PrepareMemberInfoCache", RuntimeTypeHandle::PrepareMemberInfoCache)
FCFuncEnd()

FCFuncStart(gFastArrayFuncs)
    FCFuncElement("SetValueAt", FastArray::SetValueAt)
//    FCFuncElement("CopyArray", FastArray::CopyArray)
FCFuncEnd()

FCFuncStart(gCOMTypeHandleFuncs)
    FCFuncElement("CreateInstance", RuntimeTypeHandle::CreateInstance)
    FCFuncElement("CreateCaInstance", RuntimeTypeHandle::CreateCaInstance)
    FCFuncElement("CreateInstanceForAnotherGenericParameter", RuntimeTypeHandle::CreateInstanceForGenericType)
    FCFuncElement("GetGCHandle", RuntimeTypeHandle::GetGCHandle)
    FCFuncElement("FreeGCHandle", RuntimeTypeHandle::FreeGCHandle)

    FCFuncElement("IsInstanceOfType", RuntimeTypeHandle::IsInstanceOfType)
    FCFuncElement("_GetDeclaringMethod", RuntimeTypeHandle::GetDeclaringMethod)
    FCFuncElement("GetDeclaringType", RuntimeTypeHandle::GetDeclaringType)
    FCFuncElement("_GetDefaultConstructor", RuntimeTypeHandle::GetDefaultConstructor)
    FCFuncElement("_MakePointer", RuntimeTypeHandle::MakePointer)
    FCFuncElement("_MakeByRef", RuntimeTypeHandle::MakeByRef)
    FCFuncElement("_MakeSZArray", RuntimeTypeHandle::MakeSZArray)
    FCFuncElement("_MakeArray", RuntimeTypeHandle::MakeArray)
    FCFuncElement("_GetMethodDescChunk", RuntimeTypeHandle::GetMethodDescChunk)
    FCFuncElement("GetCorElementType", RuntimeTypeHandle::GetCorElementType)
    FCFuncElement("_GetAssemblyHandle", RuntimeTypeHandle::GetAssemblyHandle)
    FCFuncElement("_GetModuleHandle", RuntimeTypeHandle::GetModuleHandle)
    FCFuncElement("_GetBaseTypeHandle", RuntimeTypeHandle::GetBaseTypeHandle)
    FCFuncElement("_GetElementType", RuntimeTypeHandle::GetElementType)
    FCFuncElement("GetArrayRank", RuntimeTypeHandle::GetArrayRank)
    FCFuncElement("GetToken", RuntimeTypeHandle::GetToken)
    FCFuncElement("_GetUtf8Name", RuntimeTypeHandle::GetUtf8Name)
    FCFuncElement("_GetMethodAt", RuntimeTypeHandle::GetMethodAt)
    FCFuncElement("GetFields", RuntimeTypeHandle::GetFields)
    FCFuncElement("GetInterfaces", RuntimeTypeHandle::GetInterfaces)
    FCFuncElement("GetConstraints", RuntimeTypeHandle::GetConstraints)
    FCFuncElement("GetAttributes", RuntimeTypeHandle::GetAttributes)
    FCFuncElement("_GetMethodFromToken", RuntimeTypeHandle::GetMethodFromToken)
    FCFuncElement("GetNumVtableSlots", RuntimeTypeHandle::GetNumVtableSlots)
    FCFuncElement("GetInterfaceMethodSlots", RuntimeTypeHandle::GetInterfaceMethodSlots)
    FCFuncElement("GetFirstSlotForInterface", RuntimeTypeHandle::GetFirstSlotForInterface)
    FCFuncElement("GetInterfaceMethodImplementationSlot", RuntimeTypeHandle::GetInterfaceMethodImplementationSlot)
    FCFuncElement("IsComObject", RuntimeTypeHandle::IsComObject)
    FCFuncElement("HasProxyAttribute", RuntimeTypeHandle::HasProxyAttribute)
    FCFuncElement("IsContextful", RuntimeTypeHandle::IsContextful)
    FCFuncElement("IsVisible", RuntimeTypeHandle::IsVisible)
    FCFuncElement("_IsVisibleFromModule", RuntimeTypeHandle::IsVisibleFromModule)
    FCFuncElement("ConstructName", RuntimeTypeHandle::ConstructName)
    FCFuncElement("CanCastTo", RuntimeTypeHandle::CanCastTo)
    FCFuncElement("SupportsInterface", RuntimeTypeHandle::SupportsInterface)
    FCFuncElement("_GetTypeByName", RuntimeTypeHandle::GetTypeByName)
    FCFuncElement("_GetTypeByNameUsingCARules", RuntimeTypeHandle::GetTypeByNameUsingCARules)
    FCFuncElement("GetInstantiation", RuntimeTypeHandle::GetInstantiation)
    FCFuncElement("_Instantiate", RuntimeTypeHandle::Instantiate)
    FCFuncElement("_GetGenericTypeDefinition", RuntimeTypeHandle::GetGenericTypeDefinition)
    FCFuncElement("HasInstantiation", RuntimeTypeHandle::HasInstantiation)
    FCFuncElement("GetGenericVariableIndex", RuntimeTypeHandle::GetGenericVariableIndex)
    FCFuncElement("IsGenericVariable", RuntimeTypeHandle::IsGenericVariable)
    FCFuncElement("IsGenericTypeDefinition", RuntimeTypeHandle::IsGenericTypeDefinition)
    FCFuncElement("ContainsGenericVariables", RuntimeTypeHandle::ContainsGenericVariables)
    FCFuncElement("SatisfiesConstraints", RuntimeTypeHandle::SatisfiesConstraints)
    FCFuncElement("Allocate", RuntimeTypeHandle::Allocate) //for A.CI
    FCFuncElement("GetCanonicalHandle", RuntimeTypeHandle::GetCanonicalHandle) //for A.CI
FCFuncEnd()

FCFuncStart(gMetaDataImport)
    FCFuncElement("_GetDefaultValue", MetaDataImport::GetDefaultValue) 
    FCFuncElement("_GetName", MetaDataImport::GetName) 
    FCFuncElement("_GetUserString", MetaDataImport::GetUserString) 
    FCFuncElement("_GetNamespace", MetaDataImport::GetNamespace) 
    FCFuncElement("_GetEventProps", MetaDataImport::GetEventProps)
    FCFuncElement("_GetFieldDefProps", MetaDataImport::GetFieldDefProps)
    FCFuncElement("_GetPropertyProps", MetaDataImport::GetPropertyProps)  
    FCFuncElement("_GetParentToken", MetaDataImport::GetParentToken)  
    FCFuncElement("_GetParamDefProps", MetaDataImport::GetParamDefProps) 
    FCFuncElement("_GetGenericParamProps", MetaDataImport::GetGenericParamProps) 
    
    FCFuncElement("_Enum", MetaDataImport::Enum) 
    FCFuncElement("_EnumCount", MetaDataImport::EnumCount) 
    FCFuncElement("_GetScopeProps", MetaDataImport::GetScopeProps)  
    FCFuncElement("_GetMemberRefProps", MetaDataImport::GetMemberRefProps) 
    FCFuncElement("_GetCustomAttributeProps", MetaDataImport::GetCustomAttributeProps) 
    FCFuncElement("_GetClassLayout", MetaDataImport::GetClassLayout) 
    FCFuncElement("_GetFieldOffset", MetaDataImport::GetFieldOffset) 

    FCFuncElement("_GetSigOfFieldDef", MetaDataImport::GetSigOfFieldDef) 
    FCFuncElement("_GetSignatureFromToken", MetaDataImport::GetSignatureFromToken) 
    FCFuncElement("_GetSigOfMethodDef", MetaDataImport::GetSigOfMethodDef) 
    FCFuncElement("_GetFieldMarshal", MetaDataImport::GetFieldMarshal) 
    FCFuncElement("_GetPInvokeMap", MetaDataImport::GetPinvokeMap) 
    FCFuncElement("_IsValidToken", MetaDataImport::IsValidToken) 
    FCFuncElement("_GetMarshalAs", MetaDataImport::GetMarshalAs)  
FCFuncEnd()

FCFuncStart(gRuntimeFieldInfoFuncs)
    FCFuncElement("PerformVisibilityCheckOnField",  ReflectionInvocation::PerformVisibilityCheckOnField)
FCFuncEnd()

FCFuncStart(gMethodDescChunk)
    FCFuncElement("_GetNextMethodDescChunk", MethodDescChunkHandle::GetNextMethodDescChunk)
    FCFuncElement("_GetMethodAt", MethodDescChunkHandle::GetMethodAt)
    FCFuncElement("GetMethodCount", MethodDescChunkHandle::GetMethodCount)
FCFuncEnd()

FCFuncStart(gMethodBase)
    FCFuncElement("GetSpecialSecurityFlags", ReflectionInvocation::GetSpecialSecurityFlags)
    FCFuncElement("PerformSecurityCheck", ReflectionInvocation::PerformSecurityCheck)
FCFuncEnd()

FCFuncStart(gSignatureNative)
    FCFuncElement("_GetSignature", SignatureNative::GetSignature)
    FCFuncElement("GetCustomModifiers", SignatureNative::GetCustomModifiers)
    FCFuncElement("CompareSig", SignatureNative::CompareSig)
FCFuncEnd()

FCFuncStart(gRuntimeMethodHandle)
    FCFuncElement("ConstructInstantiation", RuntimeMethodHandle::ConstructInstantiation)
    FCFuncElement("_GetCurrentMethod", RuntimeMethodHandle::GetCurrentMethod)
    FCFuncElement("_InvokeConstructor", RuntimeMethodHandle::InvokeConstructor)
    FCFuncElement("_SerializationInvoke", RuntimeMethodHandle::SerializationInvoke)
    FCFuncElement("IsILStub", RuntimeMethodHandle::IsILStub)
    FCFuncElement("_InvokeMethodFast", RuntimeMethodHandle::InvokeMethodFast)
    FCFuncElement("GetFunctionPointer", RuntimeMethodHandle::GetFunctionPointer)
    FCFuncElement("GetImplAttributes", RuntimeMethodHandle::GetImplAttributes)
    FCFuncElement("GetAttributes", RuntimeMethodHandle::GetAttributes)
    FCFuncElement("GetDeclaringType", RuntimeMethodHandle::GetDeclaringType)
    FCFuncElement("GetSlot", RuntimeMethodHandle::GetSlot)
    FCFuncElement("GetMethodDef", RuntimeMethodHandle::GetMethodDef)
    FCFuncElement("GetName", RuntimeMethodHandle::GetName)
    FCFuncElement("_GetUtf8Name", RuntimeMethodHandle::GetUtf8Name)
    FCFuncElement("GetMethodInstantiation", RuntimeMethodHandle::GetMethodInstantiation)
    FCFuncElement("HasMethodInstantiation", RuntimeMethodHandle::HasMethodInstantiation)
    FCFuncElement("IsGenericMethodDefinition", RuntimeMethodHandle::IsGenericMethodDefinition)
    FCFuncElement("_GetTypicalMethodDefinition", RuntimeMethodHandle::GetTypicalMethodDefinition)
    FCFuncElement("_StripMethodInstantiation", RuntimeMethodHandle::StripMethodInstantiation)
    FCFuncElement("GetInstantiatingStub", RuntimeMethodHandle::GetInstantiatingStub)
    FCFuncElement("GetMethodFromCanonical", RuntimeMethodHandle::GetMethodFromCanonical)
    FCFuncElement("IsDynamicMethod", RuntimeMethodHandle::IsDynamicMethod)
    FCFuncElement("GetResolver", RuntimeMethodHandle::GetResolver)
    FCFuncElement("Destroy", RuntimeMethodHandle::Destroy)
    FCFuncElement("_GetMethodBody", RuntimeMethodHandle::GetMethodBody)    
    FCFuncElement("_CheckLinktimeDemands", RuntimeMethodHandle::CheckLinktimeDemands)    
    FCFuncElement("_IsVisibleFromModule", RuntimeMethodHandle::IsVisibleFromModule)
    FCFuncElement("_IsVisibleFromType", RuntimeMethodHandle::IsVisibleFromType)
    FCFuncElement("IsConstructor", RuntimeMethodHandle::IsConstructor)    
FCFuncEnd()

FCFuncStart(gCOMDefaultBinderFuncs)
    FCFuncElement("CanConvertPrimitive", ReflectionBinder::DBCanConvertPrimitive)
    FCFuncElement("CanConvertPrimitiveObjectToType",  ReflectionBinder::DBCanConvertObjectPrimitive)
FCFuncEnd()


FCFuncStart(gCOMFieldHandleNewFuncs)
    FCFuncElement("GetValue", RuntimeFieldHandle::GetValue)
    FCFuncElement("SetValue", RuntimeFieldHandle::SetValue)
    FCFuncElement("GetValueDirect", RuntimeFieldHandle::GetValueDirect)
    FCFuncElement("SetValueDirect", RuntimeFieldHandle::SetValueDirect)
    FCFuncElement("GetName", RuntimeFieldHandle::GetName)
    FCFuncElement("_GetUtf8Name", RuntimeFieldHandle::GetUtf8Name)
    FCFuncElement("GetAttributes", RuntimeFieldHandle::GetAttributes)
    FCFuncElement("GetApproxDeclaringType", RuntimeFieldHandle::GetApproxDeclaringType)
    FCFuncElement("GetToken", RuntimeFieldHandle::GetToken)
    FCFuncElement("GetStaticFieldForGenericType", RuntimeFieldHandle::GetStaticFieldForGenericType)
    FCFuncElement("AcquiresContextFromThis", RuntimeFieldHandle::AcquiresContextFromThis)
FCFuncEnd()


FCFuncStart(gCOMModuleFuncs)
    FCFuncElement("GetAssemblyInternal", COMModule::GetAssembly)
    FCFuncElement("InternalGetTypeToken", COMModule::GetClassToken)
    FCFuncElement("InternalGetTypeSpecTokenWithBytes", COMModule::GetTypeSpecTokenWithBytes)
    FCFuncElement("InternalGetMemberRef", COMModule::GetMemberRefToken)
    FCFuncElement("_InternalGetMemberRefOfMethodInfo", COMModule::GetMemberRefTokenOfMethodInfo)
    FCFuncElement("_InternalGetMemberRefOfFieldInfo", COMModule::GetMemberRefTokenOfFieldInfo)
    FCFuncElement("InternalGetMemberRefFromSignature", COMModule::GetMemberRefTokenFromSignature)
    FCFuncElement("InternalSetFieldRVAContent", COMModule::SetFieldRVAContent)
    FCFuncElement("InternalLoadInMemoryTypeByName", COMModule::LoadInMemoryTypeByName)
    FCFuncElement("nativeGetArrayMethodToken", COMModule::GetArrayMethodToken)
    FCFuncElement("GetTypeInternal", COMModule::GetClass)
    FCFuncElement("InternalGetName", COMModule::GetName)
    FCFuncElement("GetTypesInternal", COMModule::GetClasses)
    FCFuncElement("InternalGetStringConstant", COMModule::GetStringConstant)
    FCFuncElement("InternalSetModuleProps", COMModule::SetModuleProps)
    FCFuncElement("InternalGetFullyQualifiedName", COMModule::GetFullyQualifiedName)
    FCFuncElement("GetHINSTANCE", COMModule::GetHINST)
    FCFuncElement("IsResourceInternal", COMModule::IsResource)
    FCFuncElement("InternalPreSavePEFile", COMDynamicWrite::PreSavePEFile)
    FCFuncElement("_InternalSavePEFile", COMDynamicWrite::SavePEFile)
    FCFuncElement("InternalAddResource", COMDynamicWrite::AddResource)
    FCFuncElement("InternalSetResourceCounts", COMDynamicWrite::SetResourceCounts)
FCFuncEnd()

FCFuncStart(gCOMModuleHandleFuncs)
    FCFuncElement("GetToken", ModuleHandle::GetToken)
    FCFuncElement("_GetModuleTypeHandle", ModuleHandle::GetModuleTypeHandle)
    FCFuncElement("GetDynamicMethod", ModuleHandle::GetDynamicMethod)
    FCFuncElement("_GetMetadataImport", ModuleHandle::GetMetadataImport)
    FCFuncElement("ResolveType", ModuleHandle::ResolveType)
    FCFuncElement("ResolveMethod", ModuleHandle::ResolveMethod)
    FCFuncElement("ResolveField", ModuleHandle::ResolveField)
    FCFuncElement("GetModule", ModuleHandle::GetModule)
    FCFuncElement("GetCallerType", ModuleHandle::GetCallerType)
    FCFuncElement("_GetPEKind", ModuleHandle::GetPEKind)
    FCFuncElement("_GetMDStreamVersion", ModuleHandle::GetMDStreamVersion)
FCFuncEnd()

FCFuncStart(gCOMAssemblyHandleFuncs)
    FCFuncElement("_GetManifestModule", AssemblyHandle::GetManifestModule)
    FCFuncElement("GetAssembly", AssemblyHandle::GetAssembly)
    FCFuncElement("GetToken", AssemblyHandle::GetToken)
    FCFuncElement("_AptcaCheck", AssemblyHandle::AptcaCheck)
FCFuncEnd()


FCFuncStart(gCustomAttributeEncodedArgument)
    FCFuncElement("ParseAttributeArguments", Attribute::ParseAttributeArguments)
FCFuncEnd()

FCFuncStart(gPseudoCustomAttribute)
    FCFuncElement("_GetSecurityAttributes", COMCustomAttribute::GetSecurityAttributes)
FCFuncEnd()

FCFuncStart(gCOMCustomAttributeFuncs)
    FCFuncElement("_ParseAttributeUsageAttribute", COMCustomAttribute::ParseAttributeUsageAttribute)
    FCFuncElement("_CreateCaObject", COMCustomAttribute::CreateCaObject)
    FCFuncElement("_GetPropertyOrFieldData",  COMCustomAttribute::GetPropertyOrFieldData)
FCFuncEnd()

FCFuncStart(gSecurityContextFrameFuncs)
    FCFuncElement("Push", COMCustomAttribute::PushSecurityContextFrame)
    FCFuncElement("Pop", COMCustomAttribute::PopSecurityContextFrame)
FCFuncEnd()

FCFuncStart(gCOMClassWriter)
    FCFuncElement("InternalDefineGenParam", COMDynamicWrite::CWCreateGenParam)
    FCFuncElement("InternalDefineClass", COMDynamicWrite::CWCreateClass)
    FCFuncElement("InternalSetParentType", COMDynamicWrite::CWSetParentType)
    FCFuncElement("InternalAddInterfaceImpl", COMDynamicWrite::CWAddInterfaceImpl)
    FCFuncElement("InternalDefineMethod", COMDynamicWrite::CWCreateMethod)
    FCFuncElement("InternalDefineMethodSpec", COMDynamicWrite::CWCreateMethodSpec)
    FCFuncElement("InternalSetMethodIL", COMDynamicWrite::CWSetMethodIL)
    FCFuncElement("TermCreateClass", COMDynamicWrite::CWTermCreateClass)
    FCFuncElement("InternalDefineField", COMDynamicWrite::CWCreateField)
    FCFuncElement("InternalSetPInvokeData", COMDynamicWrite::InternalSetPInvokeData)
    FCFuncElement("InternalDefineProperty", COMDynamicWrite::CWDefineProperty)
    FCFuncElement("InternalDefineEvent", COMDynamicWrite::CWDefineEvent)
    FCFuncElement("InternalDefineMethodSemantics", COMDynamicWrite::CWDefineMethodSemantics)
    FCFuncElement("InternalSetMethodImpl", COMDynamicWrite::CWSetMethodImpl)
    FCFuncElement("InternalDefineMethodImpl", COMDynamicWrite::CWDefineMethodImpl)
    FCFuncElement("InternalGetTokenFromSig",  COMDynamicWrite::CWGetTokenFromSig)
    FCFuncElement("InternalSetFieldOffset", COMDynamicWrite::CWSetFieldLayoutOffset)
    FCFuncElement("InternalSetClassLayout", COMDynamicWrite::CWSetClassLayout)
    FCFuncElement("InternalSetParamInfo", COMDynamicWrite::CWSetParamInfo)
    FCFuncElement("InternalSetMarshalInfo", COMDynamicWrite::CWSetMarshal)
    FCFuncElement("InternalSetConstantValue", COMDynamicWrite::CWSetConstantValue)
    FCFuncElement("InternalCreateCustomAttribute", COMDynamicWrite::CWInternalCreateCustomAttribute)
    FCFuncElement("InternalAddDeclarativeSecurity", COMDynamicWrite::CWAddDeclarativeSecurity)
FCFuncEnd()


FCFuncStart(gCOMMethodRental)
    FCFuncElement("SwapMethodBodyHelper", COMMethodRental::SwapMethodBody)
FCFuncEnd()


FCFuncStart(gCOMCodeAccessSecurityEngineFuncs)
    FCFuncElement("IncrementOverridesCount", SecurityPolicy::IncrementOverridesCount)
    FCFuncElement("DecrementOverridesCount", SecurityPolicy::DecrementOverridesCount)
    FCFuncElement("IncrementAssertCount", SecurityPolicy::IncrementAssertCount)
    FCFuncElement("DecrementAssertCount", SecurityPolicy::DecrementAssertCount)
    FCFuncElement("SpecialDemand", SecurityStackWalk::FcallSpecialDemand)
    FCFuncElement("DestroyDelayedCompressedStack", SecurityStackWalk::FcallDestroyDelayedCompressedStack)
    FCFuncElement("DestroyDCSList", NewCompressedStack::DestroyDCSList)
    FCFuncElement("Check", SecurityStackWalk::Check)
    FCFuncElement("QuickCheckForAllDemands", SecurityStackWalk::FCallQuickCheckForAllDemands)
    FCFuncElement("AllDomainsHomogeneousWithNoStackModifiers", SecurityStackWalk::FCallAllDomainsHomogeneousWithNoStackModifiers)    
    FCFuncElement("CheckNReturnSO", SecurityStackWalk::CheckNReturnSO)
    FCFuncElement("_GetGrantedPermissionSet", SecurityStackWalk::GetGrantedPermissionSet)
    FCFuncElement("GetDelayedCompressedStack", SecurityStackWalk::EcallGetDelayedCompressedStack)
    FCFuncElement("GetZoneAndOriginInternal", SecurityStackWalk::GetZoneAndOrigin)
    FCFuncElement("GetDCSCount", NewCompressedStack::FCallGetDCSCount)
    FCFuncElement("IsImmediateCompletionCandidate", NewCompressedStack::FCallIsImmediateCompletionCandidate)
    FCFuncElement("GetDomainCompressedStack", NewCompressedStack::GetDomainCompressedStack)    
    FCFuncElement("GetHomogeneousPLS", NewCompressedStack::FCallGetHomogeneousPLS)
    FCFuncElement("GetDescCount", DomainCompressedStack::GetDescCount)    
    FCFuncElement("GetDomainPermissionSets", DomainCompressedStack::GetDomainPermissionSets)    
    FCFuncElement("GetDescriptorInfo", DomainCompressedStack::GetDescriptorInfo)    
    FCFuncElement("IgnoreDomain", DomainCompressedStack::IgnoreDomain)    
    FCFuncElement("DoesFullTrustMeanFullTrust", SecurityPolicy::FcallDoesFullTrustMeanFullTrust)
FCFuncEnd()

FCFuncStart(gCOMSecurityManagerFuncs)
    FCFuncElement("_IsSameType", SecurityPolicy::IsSameType)
    FCFuncElement("_SetThreadSecurity", SecurityPolicy::SetThreadSecurity)
    FCFuncElement("_IsSecurityOn", SecurityPolicy::IsSecurityOnNative)
    FCFuncElement("GetGlobalFlags", SecurityPolicy::GetGlobalSecurity)
    FCFuncElement("SetGlobalFlags", SecurityPolicy::SetGlobalSecurity)
    FCFuncElement("SaveGlobalFlags", SecurityPolicy::SaveGlobalSecurity)
    FCFuncElement("_GetGrantedPermissions", SecurityPolicy::GetGrantedPermissions)
    FCFuncElement("GetImpersonationFlowMode", SecurityPolicy::GetImpersonationFlowMode)
    FCFuncElement("IsDefaultThreadSecurityInfo", SecurityPolicy::IsDefaultThreadSecurityInfo)    
FCFuncEnd()

FCFuncStart(gCOMSecurityZone)
    FCFuncElement("_CreateFromUrl", SecurityPolicy::CreateFromUrl)
FCFuncEnd()

FCFuncStart(gCOMFileIOAccessFuncs)
    FCFuncElement("_LocalDrive", SecurityPolicy::LocalDrive)
FCFuncEnd()

FCFuncStart(gCOMStringExpressionSetFuncs)
    FCFuncElement("GetLongPathName", SecurityPolicy::EcallGetLongPathName)
FCFuncEnd()


#ifndef PLATFORM_UNIX
FCFuncStart(gCOMUrlStringFuncs)
    FCFuncElement("_GetDeviceName", SecurityPolicy::GetDeviceName)
FCFuncEnd()
#endif // !PLATFORM_UNIX

FCFuncStart(gCOMSecurityRuntimeFuncs)
    FCFuncElement("GetSecurityObjectForFrame", SecurityRuntime::GetSecurityObjectForFrame)
FCFuncEnd()


FCFuncStart(gBCLDebugFuncs)
    FCFuncElement("GetRegistryLoggingValues", ManagedLoggingHelper::GetRegistryLoggingValues)
FCFuncEnd()


FCFuncStart(gAppDomainManagerFuncs)
    FCFuncElement("nGetEntryAssembly", AssemblyNative::GetEntryAssembly)
    FCFuncElement("nRegisterWithHost", AppDomainNative::RegisterWithHost)
FCFuncEnd()

FCFuncStart(gAppDomainSetupFuncs)
    FCFuncElement("UpdateContextProperty", AppDomainNative::UpdateContextProperty)
FCFuncEnd()

FCFuncStart(gAppDomainFuncs)
    FCFuncElement("GetDefaultDomain", AppDomainNative::GetDefaultDomain)
    FCFuncElement("GetFusionContext", AppDomainNative::GetFusionContext)
    FCFuncElement("IsStringInterned", AppDomainNative::IsStringInterned)
    FCFuncElement("IsUnloadingForcedFinalize", AppDomainNative::IsUnloadingForcedFinalize)
    FCFuncElement("nCreateDomain", AppDomainNative::CreateDomain)
    FCFuncElement("nCreateInstance", AppDomainNative::CreateInstance)
    FCFuncElement("GetSecurityDescriptor", AppDomainNative::GetSecurityDescriptor)
    FCFuncElement("UpdateLoaderOptimization", AppDomainNative::UpdateLoaderOptimization)
    FCFuncElement("nGetDomainManagerAsm", AppDomainNative::GetDomainManagerAsm)
    FCFuncElement("nGetDomainManagerType", AppDomainNative::GetDomainManagerType)
    FCFuncElement("nGetFriendlyName", AppDomainNative::GetFriendlyName)
    FCFuncElement("nIsDefaultAppDomainForSecurity", AppDomainNative::IsDefaultAppDomainForSecurity)
    FCFuncElement("nSetHostSecurityManagerFlags", AppDomainNative::SetHostSecurityManagerFlags)
    FCFuncElement("nSetSecurityHomogeneousFlag", AppDomainNative::SetSecurityHomogeneousFlag)
    FCFuncElement("nSetupDomainSecurity", AppDomainNative::SetupDomainSecurity)
    FCFuncElement("nSetupFriendlyName", AppDomainNative::SetupFriendlyName)
    FCFuncElement("nGetAssemblies", AppDomainNative::GetAssemblies)
    FCFuncElement("nCreateDynamicAssembly", AppDomainNative::CreateDynamicAssembly)
    FCFuncElement("nExecuteAssembly", AppDomainNative::ExecuteAssembly)
    FCFuncElement("nUnload", AppDomainNative::Unload)
    FCFuncElement("GetId", AppDomainNative::GetId)
    FCFuncElement("GetOrInternString", AppDomainNative::GetOrInternString)
    FCFuncElement("GetDynamicDir", AppDomainNative::GetDynamicDir)
    FCFuncElement("nGetGrantSet", AppDomainNative::GetGrantSet)
    FCFuncElement("nChangeSecurityPolicy", AppDomainNative::ChangeSecurityPolicy)
    FCFuncElement("IsDomainIdValid", AppDomainNative::IsDomainIdValid)
    FCFuncElement("IsFinalizingForUnload", AppDomainNative::IsFinalizingForUnload)
    FCFuncElement("nApplyPolicy", AppDomainNative::nApplyPolicy)
    FCFuncElement("GetAppDomainId", AppDomainNative::GetAppDomainId)
FCFuncEnd()

FCFuncStart(gUtf8String)
    FCFuncElement("EqualsCaseSensitive", Utf8String::EqualsCaseSensitive)
    FCFuncElement("EqualsCaseInsensitive", Utf8String::EqualsCaseInsensitive)
FCFuncEnd()

FCFuncStart(gTypeNameBuilder)
    FCFuncElement("_CreateTypeNameBuilder", TypeNameBuilder::_CreateTypeNameBuilder)
    FCFuncElement("_ReleaseTypeNameBuilder", TypeNameBuilder::_ReleaseTypeNameBuilder)
    FCFuncElement("_OpenGenericArguments", TypeNameBuilder::_OpenGenericArguments)
    FCFuncElement("_CloseGenericArguments", TypeNameBuilder::_CloseGenericArguments)
    FCFuncElement("_OpenGenericArgument", TypeNameBuilder::_OpenGenericArgument)
    FCFuncElement("_CloseGenericArgument", TypeNameBuilder::_CloseGenericArgument)
    FCFuncElement("_AddName", TypeNameBuilder::_AddName)
    FCFuncElement("_AddPointer", TypeNameBuilder::_AddPointer)
    FCFuncElement("_AddByRef", TypeNameBuilder::_AddByRef)
    FCFuncElement("_AddSzArray", TypeNameBuilder::_AddSzArray)
    FCFuncElement("_AddArray", TypeNameBuilder::_AddArray)
    FCFuncElement("_AddAssemblySpec", TypeNameBuilder::_AddAssemblySpec)
    FCFuncElement("_ToString", TypeNameBuilder::_ToString)
    FCFuncElement("_Clear", TypeNameBuilder::_Clear)
FCFuncEnd()

FCFuncStart(gAssemblyFuncs)
    FCFuncElement("GetFullName", AssemblyNative::GetStringizedName)
    FCFuncElement("GetLocation", AssemblyNative::GetLocation)
    FCFuncElement("GetResource", AssemblyNative::GetResource)
    FCFuncElement("nGetCodeBase", AssemblyNative::GetCodeBase)
    FCFuncElement("nGetExecutingAssembly", AssemblyNative::GetExecutingAssembly)
    FCFuncElement("nGetFlags", AssemblyNative::GetFlags)
    FCFuncElement("nGetHashAlgorithm", AssemblyNative::GetHashAlgorithm)
    FCFuncElement("nGetLocale", AssemblyNative::GetLocale)
    FCFuncElement("nGetPublicKey", AssemblyNative::GetPublicKey)
    FCFuncElement("nGetSimpleName", AssemblyNative::GetSimpleName)
    FCFuncElement("nGetVersion", AssemblyNative::GetVersion)
    FCFuncElement("nIsDynamic", AssemblyNative::IsDynamic)
    FCFuncElement("nLoad", AssemblyNative::Load)
    FCFuncElement("nLoadImage", AssemblyNative::LoadImage)
    FCFuncElement("nLoadFile", AssemblyNative::LoadFile)
    FCFuncElement("nLoadModule", AssemblyNative::LoadModuleImage)
    FCFuncElement("GetType", AssemblyNative::GetType)
    FCFuncElement("nGetManifestResourceInfo", AssemblyNative::GetManifestResourceInfo)
    FCFuncElement("nGetModules", AssemblyNative::GetModules)
    FCFuncElement("GetModule", AssemblyNative::GetModule)
    FCFuncElement("GetReferencedAssemblies", AssemblyNative::GetReferencedAssemblies)
    FCFuncElement("GetExportedTypes", AssemblyNative::GetExportedTypes)
    FCFuncElement("nGetManifestResourceNames", AssemblyNative::GetResourceNames)
    FCFuncElement("nPrepareForSavingManifestToDisk", AssemblyNative::PrepareSavingManifest)
    FCFuncElement("nSaveToFileList", AssemblyNative::AddFileList)
    FCFuncElement("nSetHashValue", AssemblyNative::SetHashValue)
    FCFuncElement("nSaveExportedType", AssemblyNative::AddExportedType)
    FCFuncElement("nAddStandAloneResource", AssemblyNative::AddStandAloneResource)
    FCFuncElement("nSavePermissionRequests", AssemblyNative::SavePermissionRequests)
    FCFuncElement("nSaveManifestToDisk", AssemblyNative::SaveManifestToDisk)
    FCFuncElement("nAddFileToInMemoryFileList", AssemblyNative::AddFileToInMemoryFileList)
    FCFuncElement("_nGetEntryPoint", AssemblyNative::GetEntryPoint)
    FCFuncElement("CreateQualifiedName", AssemblyNative::CreateQualifiedName)
    FCFuncElement("nGetGrantSet", AssemblyNative::GetGrantSet)
    FCFuncElement("nGetOnDiskAssemblyModule", AssemblyNative::GetOnDiskAssemblyModule)
    FCFuncElement("nGetInMemoryAssemblyModule", AssemblyNative::GetInMemoryAssemblyModule)
    FCFuncElement("nGlobalAssemblyCache", AssemblyNative::GlobalAssemblyCache)
    FCFuncElement("nDefineDynamicModule", COMModule::DefineDynamicModule)
    FCFuncElement("nGetEvidence", SecurityPolicy::GetEvidence)
    FCFuncElement("nGetImageRuntimeVersion", AssemblyNative::GetImageRuntimeVersion)
    FCFuncElement("nReflection", AssemblyNative::Reflection)
    FCFuncElement("GetHostContext", AssemblyNative::GetHostContext)
FCFuncEnd()

FCFuncStart(gAssemblyNameFuncs)
    FCFuncElement("nGetFileInformation", AssemblyNameNative::GetFileInformation)
    FCFuncElement("nToString", AssemblyNameNative::ToString)
    FCFuncElement("nGetPublicKeyToken", AssemblyNameNative::GetPublicKeyToken)
    FCFuncElement("EscapeCodeBase", AssemblyNameNative::EscapeCodeBase)
    FCFuncElement("nInit", AssemblyNameNative::Init)
    FCFuncElement("ReferenceMatchesDefinition", AssemblyNameNative::ReferenceMatchesDefinition)
FCFuncEnd()


FCFuncStart(gDelegateFuncs)
    FCFuncElement("BindToMethodName", COMDelegate::BindToMethodName)
    FCFuncElement("BindToMethodInfo", COMDelegate::BindToMethodInfo)
    FCFuncElement("GetMulticastInvoke", COMDelegate::GetMulticastInvoke)
    FCFuncElement("GetInvokeMethod", COMDelegate::GetInvokeMethod)
    FCFuncElement("GetUnmanagedCallSite", COMDelegate::GetUnmanagedCallSite)
    FCFuncElement("InternalAlloc", COMDelegate::InternalAlloc)
    FCFuncElement("InternalAllocLike", COMDelegate::InternalAllocLike)
    FCFuncElement("InternalEqualTypes", COMDelegate::InternalEqualTypes)
    FCFuncElement("FindMethodHandle", COMDelegate::FindMethodHandle)
    FCFuncElement("AdjustTarget", COMDelegate::AdjustTarget)
    FCFuncElement("GetCallStub", COMDelegate::GetCallStub)

    // The FCall mechanism knows how to wire multiple different constructor calls into a
    // single entrypoint, without the following entry.  But we need this entry to satisfy
    // frame creation within the body:
    FCFuncElement("DelegateConstruct", COMDelegate::DelegateConstruct)
FCFuncEnd()

FCFuncStart(gMathFuncs)
    FCIntrinsic("Sin", COMDouble::Sin, CORINFO_INTRINSIC_Sin)
    FCIntrinsic("Cos", COMDouble::Cos, CORINFO_INTRINSIC_Cos)
    FCIntrinsic("Sqrt", COMDouble::Sqrt, CORINFO_INTRINSIC_Sqrt)
    FCIntrinsic("Round", COMDouble::Round, CORINFO_INTRINSIC_Round)
    FCIntrinsicSig("Abs", &gsig_SM_Flt_RetFlt, COMDouble::AbsFlt, CORINFO_INTRINSIC_Abs)
    FCIntrinsicSig("Abs", &gsig_SM_Dbl_RetDbl, COMDouble::AbsDbl, CORINFO_INTRINSIC_Abs)
    FCFuncElement("Exp", COMDouble::Exp)
    FCFuncElement("Pow", COMDouble::Pow)
    FCFuncElement("Tan", COMDouble::Tan)
    FCFuncElement("Floor", COMDouble::Floor)
    FCFuncElement("Log", COMDouble::Log)
    FCFuncElement("Sinh", COMDouble::Sinh)
    FCFuncElement("Cosh", COMDouble::Cosh)
    FCFuncElement("Tanh", COMDouble::Tanh)
    FCFuncElement("Acos", COMDouble::Acos)
    FCFuncElement("Asin", COMDouble::Asin)
    FCFuncElement("Atan", COMDouble::Atan)
    FCFuncElement("Atan2", COMDouble::Atan2)
    FCFuncElement("Log10", COMDouble::Log10)
    FCFuncElement("Ceiling", COMDouble::Ceil)
    FCFuncElement("SplitFractionDouble", COMDouble::ModFDouble)
FCFuncEnd()

FCFuncStart(gThreadFuncs)
    FCDynamic("InternalGetCurrentThread", CORINFO_INTRINSIC_Illegal, ECall::InternalGetCurrentThread)
    FCFuncElement("GetHashCode", ThreadNative::GetHashCode)
    FCFuncElement("StartInternal", ThreadNative::Start)
    FCFuncElement("SuspendInternal", ThreadNative::Suspend)
    FCFuncElement("ResumeInternal", ThreadNative::Resume)
    FCFuncElement("GetPriorityNative", ThreadNative::GetPriority)
    FCFuncElement("SetPriorityNative", ThreadNative::SetPriority)
    FCFuncElement("InterruptInternal", ThreadNative::Interrupt)
    FCFuncElement("IsAliveNative", ThreadNative::IsAlive)
    FCFuncElement("nativeGetSafeCulture", ThreadNative::nativeGetSafeCulture)
    FCFuncElementSig("JoinInternal", &gsig_IM_RetVoid, ThreadNative::Join)
    FCFuncElementSig("JoinInternal", &gsig_IM_Int_RetBool, ThreadNative::JoinTimeout)
#undef Sleep
    FCFuncElement("SleepInternal", ThreadNative::Sleep)
#define Sleep(a) Dont_Use_Sleep(a)
    FCFuncElement("SetStart", ThreadNative::SetStart)
    FCFuncElement("SetBackgroundNative", ThreadNative::SetBackground)
    FCFuncElement("IsBackgroundNative", ThreadNative::IsBackground)
    FCFuncElement("GetThreadStateNative", ThreadNative::GetThreadState)
    FCFuncElement("GetContextInternal", ThreadNative::GetContextFromContextID)
    FCFuncElement("GetDomainInternal", ThreadNative::GetDomain)
    FCFuncElement("GetFastDomainInternal", ThreadNative::FastGetDomain)
    FCFuncElement("SetAppDomainStack", ThreadNative::SetAppDomainStack)
    FCFuncElement("RestoreAppDomainStack", ThreadNative::RestoreAppDomainStack)    
    FCFuncElement("InternalCrossContextCallback", ThreadNative::InternalCrossContextCallback)
    FCFuncElement("InformThreadNameChangeEx", ThreadNative::InformThreadNameChangeEx)
    FCFuncElement("BeginCriticalRegion", ThreadNative::BeginCriticalRegion)
    FCFuncElement("EndCriticalRegion", ThreadNative::EndCriticalRegion)
    FCFuncElement("BeginThreadAffinity", ThreadNative::BeginThreadAffinity)
    FCFuncElement("EndThreadAffinity", ThreadNative::EndThreadAffinity)
    FCFuncElement("AbortInternal", ThreadNative::Abort)
    FCFuncElement("ResetAbortNative", ThreadNative::ResetAbort)
    FCFuncElement("IsThreadpoolThreadNative", ThreadNative::IsThreadpoolThread)
    FCFuncElement("SpinWaitInternal", ThreadNative::SpinWait)
    FCFuncElement("GetFastCurrentThreadNative", ThreadNative::FastGetCurrentThread)
    FCFuncElement("GetCurrentThreadNative", ThreadNative::GetCurrentThread)
    FCFuncElement("GetDomainLocalStore", ThreadNative::GetDomainLocalStore)
    FCFuncElement("InternalFinalize", ThreadNative::Finalize)
    FCFuncElement("SetDomainLocalStore", ThreadNative::SetDomainLocalStore)
    FCFuncElement("nativeSetThreadUILocale", ThreadNative::SetThreadUILocale)
    FCFuncElement("MemoryBarrier", ThreadNative::FCMemoryBarrier)
    FCFuncElement("SetIsThreadStaticsArray", ThreadNative::SetIsThreadStaticsArray)
    FCFuncElement("SetAbortReason", ThreadNative::SetAbortReason)
    FCFuncElement("GetAbortReason", ThreadNative::GetAbortReason)
    FCFuncElement("ClearAbortReason", ThreadNative::ClearAbortReason)
    FCFuncElement("get_ManagedThreadId", ThreadNative::GetManagedThreadId)
FCFuncEnd()

FCFuncStart(gThreadPoolFuncs)
    FCFuncElement("RegisterWaitForSingleObjectNative", ThreadPoolNative::CorRegisterWaitForSingleObject)
    FCFuncElement("QueueUserWorkItem", ThreadPoolNative::CorQueueUserWorkItem)
    FCFuncElement("PostQueuedCompletionStatus", ThreadPoolNative::CorPostQueuedCompletionStatus)
    FCFuncElement("BindIOCompletionCallbackNative", ThreadPoolNative::CorBindIoCompletionCallback)
    FCFuncElement("SetMaxThreadsNative", ThreadPoolNative::CorSetMaxThreads)
    FCFuncElement("GetMaxThreadsNative", ThreadPoolNative::CorGetMaxThreads)
    FCFuncElement("SetMinThreadsNative", ThreadPoolNative::CorSetMinThreads)
    FCFuncElement("GetMinThreadsNative", ThreadPoolNative::CorGetMinThreads)
    FCFuncElement("GetAvailableThreadsNative", ThreadPoolNative::CorGetAvailableThreads)
FCFuncEnd()

FCFuncStart(gTimerFuncs)
    FCFuncElement("ChangeTimerNative", TimerNative::CorChangeTimer)
    FCFuncElement("DeleteTimerNative", TimerNative::CorDeleteTimer)
    FCFuncElement("AddTimerNative", TimerNative::CorCreateTimer)
FCFuncEnd()

FCFuncStart(gRegisteredWaitHandleFuncs)
    FCFuncElement("UnregisterWaitNative", ThreadPoolNative::CorUnregisterWait)
    FCFuncElement("WaitHandleCleanupNative", ThreadPoolNative::CorWaitHandleCleanupNative)
FCFuncEnd()

FCFuncStart(gWaitHandleFuncs)
    FCFuncElement("WaitOneNative", WaitHandleNative::CorWaitOneNative)
    FCFuncElement("WaitMultiple", WaitHandleNative::CorWaitMultipleNative)
FCFuncEnd()

FCFuncStart(gNumberFuncs)
    FCFuncElement("FormatDecimal", COMNumber::FormatDecimal)
    FCFuncElement("FormatDouble", COMNumber::FormatDouble)
    FCFuncElement("FormatInt32", COMNumber::FormatInt32)
    FCFuncElement("FormatUInt32", COMNumber::FormatUInt32)
    FCFuncElement("FormatInt64", COMNumber::FormatInt64)
    FCFuncElement("FormatUInt64", COMNumber::FormatUInt64)
    FCFuncElement("FormatSingle", COMNumber::FormatSingle)
    FCFuncElement("NumberBufferToDecimal", COMNumber::NumberBufferToDecimal)
    FCFuncElement("NumberBufferToDouble", COMNumber::NumberBufferToDouble)
FCFuncEnd()

FCFuncStart(gVariantFuncs)
    FCFuncElement("SetFieldsR4", COMVariant::SetFieldsR4)
    FCFuncElement("SetFieldsR8", COMVariant::SetFieldsR8)
    FCFuncElement("SetFieldsObject", COMVariant::SetFieldsObject)
    FCFuncElement("GetR4FromVar", COMVariant::GetR4FromVar)
    FCFuncElement("GetR8FromVar", COMVariant::GetR8FromVar)
    FCFuncElement("InitVariant", COMVariant::InitVariant)
    FCFuncElement("BoxEnum", COMVariant::BoxEnum)
FCFuncEnd()


FCFuncStart(gDecimalFuncs)
    FCFuncElementSig(COR_CTOR_METHOD_NAME, &gsig_IM_Flt_RetVoid, COMDecimal::InitSingle)
    FCFuncElementSig(COR_CTOR_METHOD_NAME, &gsig_IM_Dbl_RetVoid, COMDecimal::InitDouble)
    FCFuncElement("FCallAdd", COMDecimal::DoAdd)
    FCFuncElement("Compare", COMDecimal::Compare)
    FCFuncElement("FCallDivide", COMDecimal::DoDivide)
    FCFuncElement("FCallFloor", COMDecimal::DoFloor)
    FCFuncElement("GetHashCode", COMDecimal::GetHashCode)
    FCFuncElement("FCallMultiply", COMDecimal::DoMultiply)
    FCFuncElement("FCallRound", COMDecimal::DoRound)
    FCFuncElement("FCallSubtract", COMDecimal::DoSubtract)
    FCFuncElement("FCallToCurrency", COMDecimal::DoToCurrency)
    FCFuncElement("FCallToInt32", COMDecimal::ToInt32)    
    FCFuncElement("ToDouble", COMDecimal::ToDouble)
    FCFuncElement("ToSingle", COMDecimal::ToSingle)
    FCFuncElement("FCallTruncate", COMDecimal::DoTruncate)
FCFuncEnd()

FCFuncStart(gCurrencyFuncs)
    FCFuncElement("FCallToDecimal", COMCurrency::DoToDecimal)
FCFuncEnd()

FCFuncStart(gCharacterInfoFuncs)
    FCFuncElement("nativeInitTable", COMNlsInfo::nativeInitUnicodeCatTable)
FCFuncEnd()

FCFuncStart(gCompareInfoFuncs)
    FCFuncElement("Compare", COMNlsInfo::Compare)
    FCFuncElement("CompareRegion", COMNlsInfo::CompareRegion)
    FCFuncElement("IndexOfChar", COMNlsInfo::IndexOfChar)
    FCFuncElement("IndexOfString", COMNlsInfo::IndexOfString)
    FCFuncElement("LastIndexOfChar", COMNlsInfo::LastIndexOfChar)
    FCFuncElement("LastIndexOfString", COMNlsInfo::LastIndexOfString)
    FCFuncElement("nativeIsPrefix", COMNlsInfo::nativeIsPrefix)
    FCFuncElement("nativeIsSuffix", COMNlsInfo::nativeIsSuffix)
    FCFuncElement("InitializeNativeCompareInfo", COMNlsInfo::InitializeNativeCompareInfo)
    FCFuncElement("nativeCreateSortKey", COMNlsInfo::nativeCreateSortKey)
    FCFuncElement("nativeGetGlobalizedHashCode", COMNlsInfo::nativeGetGlobalizedHashCode)
    FCFuncElement("nativeCompareString", COMNlsInfo::nativeCompareString)
    FCFuncElement("nativeIsSortable", COMNlsInfo::nativeIsSortable)
FCFuncEnd()

FCFuncStart(gGlobalizationAssemblyFuncs)
    FCFuncElement("nativeCreateGlobalizationAssembly", COMNlsInfo::nativeCreateGlobalizationAssembly)
FCFuncEnd()

FCFuncStart(gEncodingTableFuncs)
    FCFuncElement("GetNumEncodingItems", COMNlsInfo::nativeGetNumEncodingItems)
    FCFuncElement("GetEncodingData", COMNlsInfo::nativeGetEncodingTableDataPointer)
    FCFuncElement("GetCodePageData", COMNlsInfo::nativeGetCodePageTableDataPointer)
    FCFuncElement("nativeCreateOpenFileMapping", COMNlsInfo::nativeCreateOpenFileMapping)
FCFuncEnd()

FCFuncStart(gCalendarFuncs)
    FCFuncElement("nativeGetTwoDigitYearMax", COMNlsInfo::nativeGetTwoDigitYearMax)
FCFuncEnd()

FCFuncStart(gCultureInfoFuncs)
    FCFuncElement("IsValidLCID", COMNlsInfo::IsValidLCID)
    FCFuncElement("IsWin9xInstalledCulture", COMNlsInfo::IsWin9xInstalledCulture)
    FCFuncElement("nativeGetUserDefaultLCID", COMNlsInfo::nativeGetUserDefaultLCID)
    FCFuncElement("nativeGetUserDefaultUILanguage", COMNlsInfo::nativeGetUserDefaultUILanguage)
    FCFuncElement("nativeGetSystemDefaultUILanguage", COMNlsInfo::nativeGetSystemDefaultUILanguage)
    FCFuncElement("nativeSetThreadLocale", COMNlsInfo::nativeSetThreadLocale)
    FCFuncElement("nativeGetLocaleInfo", COMNlsInfo::nativeGetLocaleInfo)
    FCFuncElement("nativeGetDTFIUserValues", COMNlsInfo::nativeGetDTFIUserValues)
    FCFuncElement("nativeGetNFIUserValues", COMNlsInfo::nativeGetNFIUserValues)
    FCFuncElement("nativeGetCultureData", COMNlsInfo::nativeGetCultureData)
    FCFuncElement("nativeEnumSystemLocales", COMNlsInfo::nativeEnumSystemLocales)
    FCFuncElement("nativeGetCultureName", COMNlsInfo::nativeGetCultureName)
    FCFuncElement("nativeGetCurrentCalendar", COMNlsInfo::nativeGetCurrentCalendar)
    FCFuncElement("nativeGetWindowsDirectory", COMNlsInfo::nativeGetWindowsDirectory)
    FCFuncElement("nativeFileExists", COMNlsInfo::nativeFileExists)
    FCFuncElement("nativeGetStaticInt32DataTable", COMNlsInfo::nativeGetStaticInt32DataTable)
FCFuncEnd()



FCFuncStart(gCalendarTableFuncs)
    FCFuncElement("nativeGetEraName", COMNlsInfo::nativeGetEraName)
FCFuncEnd()

FCFuncStart(gTextInfoFuncs)
    FCFuncElement("nativeChangeCaseChar", COMNlsInfo::nativeChangeCaseChar)
    FCFuncElement("nativeChangeCaseString", COMNlsInfo::nativeChangeCaseString)
    FCFuncElement("nativeGetInvariantTextInfo", COMNlsInfo::nativeGetInvariantTextInfo)
    FCFuncElement("AllocateDefaultCasingTable", COMNlsInfo::AllocateDefaultCasingTable)
    FCFuncElement("InternalAllocateCasingTable", COMNlsInfo::AllocateCasingTable)
    FCFuncElement("nativeGetCaseInsHash", COMNlsInfo::GetCaseInsHash)
    FCFuncElement("nativeGetTitleCaseChar", COMNlsInfo::nativeGetTitleCaseChar)
    FCFuncElement("nativeCompareOrdinalIgnoreCase", COMNlsInfo::CompareOrdinalIgnoreCase)
    FCFuncElement("nativeCompareOrdinalIgnoreCaseEx", COMNlsInfo::CompareOrdinalIgnoreCaseEx)
    FCFuncElement("nativeChangeCaseSurrogate", COMNlsInfo::nativeChangeCaseSurrogate)
    FCFuncElement("nativeGetHashCodeOrdinalIgnoreCase", COMNlsInfo::GetHashCodeOrdinalIgnoreCase)
    FCFuncElement("nativeIndexOfStringOrdinalIgnoreCase", COMNlsInfo::IndexOfStringOrdinalIgnoreCase)
    FCFuncElement("nativeLastIndexOfStringOrdinalIgnoreCase", COMNlsInfo::LastIndexOfStringOrdinalIgnoreCase)
    FCFuncElement("nativeIndexOfCharOrdinalIgnoreCase", COMNlsInfo::IndexOfCharOrdinalIgnoreCase)
    FCFuncElement("nativeLastIndexOfCharOrdinalIgnoreCase", COMNlsInfo::LastIndexOfCharOrdinalIgnoreCase)
    
FCFuncEnd()

FCFuncStart(gArrayFuncs)
    FCFuncElement("Copy", SystemNative::ArrayCopy)
    FCFuncElement("Clear", SystemNative::ArrayClear)
    FCFuncElement("get_Rank", Array_Rank)
    FCFuncElement("GetLowerBound", Array_LowerBound)
    FCFuncElement("GetUpperBound", Array_UpperBound)
    FCIntrinsicSig("GetLength", &gsig_IM_Int_RetInt, Array_GetLength, CORINFO_INTRINSIC_Array_GetDimLength)
    FCIntrinsicSig("get_Length", &gsig_IM_RetInt, Array_GetLengthNoRank, CORINFO_INTRINSIC_Array_GetLengthTotal)
    FCFuncElement("Initialize", Array_Initialize)
    FCFuncElement("InternalCreate", COMArrayInfo::CreateInstance)
    FCFuncElement("InternalGetReference", COMArrayInfo::GetReference)
    FCFuncElement("InternalSetValue", COMArrayInfo::SetValue)
    FCFuncElement("TrySZIndexOf", ArrayHelper::TrySZIndexOf)
    FCFuncElement("TrySZLastIndexOf", ArrayHelper::TrySZLastIndexOf)
    FCFuncElement("TrySZBinarySearch", ArrayHelper::TrySZBinarySearch)
    FCFuncElement("TrySZSort", ArrayHelper::TrySZSort)
    FCFuncElement("TrySZReverse", ArrayHelper::TrySZReverse)
    FCFuncElement("GetDataPtrOffsetInternal", Array_GetDataPtrOffsetInternal)
FCFuncEnd()

FCFuncStart(gBufferFuncs)
    FCFuncElement("BlockCopy", Buffer::BlockCopy)
    FCFuncElement("InternalBlockCopy", Buffer::InternalBlockCopy)
    FCFuncElement("GetByte", Buffer::GetByte)
    FCFuncElement("SetByte", Buffer::SetByte)
    FCFuncElement("ByteLength", Buffer::ByteLength)
FCFuncEnd()

FCFuncStart(gGCInterfaceFuncs)
    FCFuncElement("GetGenerationWR", GCInterface::GetGenerationWR)
    FCFuncElement("GetGeneration", GCInterface::GetGeneration)
    FCFuncElement("nativeGetTotalMemory", GCInterface::GetTotalMemory)
    FCFuncElement("nativeCollectGeneration", GCInterface::CollectGeneration)
    FCFuncElement("nativeGetMaxGeneration", GCInterface::GetMaxGeneration)
    FCFuncElement("WaitForPendingFinalizers", GCInterface::RunFinalizers)

    FCFuncElement("KeepAlive", GCInterface::KeepAlive)
    FCFuncElement("SetCleanupCache", GCInterface::NativeSetCleanupCache)
    FCFuncElement("nativeSuppressFinalize", GCInterface::FCSuppressFinalize)
    FCFuncElement("nativeReRegisterForFinalize", GCInterface::FCReRegisterForFinalize)
    FCFuncElement("nativeCollectionCount", GCInterface::CollectionCount)
    FCFuncElement("nativeIsServerGC", SystemNative::IsServerHeap)
    FCFuncElement("nativeAddMemoryPressure", GCInterface::NativeAddMemoryPressure)
    FCFuncElement("nativeRemoveMemoryPressure", GCInterface::NativeRemoveMemoryPressure)
    
FCFuncEnd()

FCFuncStart(gMemoryFailPointFuncs)
    FCFuncElement("GetMemorySettings", COMMemoryFailPoint::GetMemorySettings)
FCFuncEnd()

FCFuncStart(gInteropMarshalFuncs)
    FCFuncElement("GetLastWin32Error", MarshalNative::GetLastWin32Error)
    FCFuncElement("SetLastWin32Error", MarshalNative::SetLastWin32Error)
    FCFuncElementSig("SizeOf", &gsig_SM_Type_RetInt, MarshalNative::SizeOfClass)
    FCFuncElement("GetSystemMaxDBCSCharSize", MarshalNative::GetSystemMaxDBCSCharSize)
    FCFuncElement("PtrToStructureHelper", MarshalNative::PtrToStructureHelper)
    FCFuncElementSig("SizeOf", &gsig_SM_Obj_RetInt, MarshalNative::FCSizeOfObject)
    FCFuncElement("OffsetOfHelper", MarshalNative::OffsetOfHelper)
    FCFuncElement("UnsafeAddrOfPinnedArrayElement", MarshalNative::FCUnsafeAddrOfPinnedArrayElement)
    FCFuncElement("InternalPrelink", NDirect_Prelink_Wrapper)
    FCFuncElement("InternalNumParamBytes", NDirect_NumParamBytes)
    FCFuncElement("CopyToNative", MarshalNative::CopyToNative)
    FCFuncElement("CopyToManaged", MarshalNative::CopyToManaged)
    FCFuncElement("PtrToStringAnsi", MarshalNative::PtrToStringAnsi)
    FCFuncElement("PtrToStringUni", MarshalNative::PtrToStringUni)
    FCFuncElement("StructureToPtr", MarshalNative::StructureToPtr)
    FCFuncElement("DestroyStructure", MarshalNative::DestroyStructure)
    FCFuncElement("GetExceptionPointers", ExceptionNative::GetExceptionPointers)
    FCFuncElement("GetExceptionCode", ExceptionNative::GetExceptionCode)
    FCFuncElement("ThrowExceptionForHRInternal", MarshalNative::ThrowExceptionForHR)
    FCFuncElement("GetExceptionForHRInternal", MarshalNative::GetExceptionForHR)
    FCFuncElement("GetHRForException", MarshalNative::GetHRForException)
    FCFuncElement("GetDelegateForFunctionPointerInternal", MarshalNative::GetDelegateForFunctionPointerInternal)
    FCFuncElement("GetFunctionPointerForDelegateInternal", MarshalNative::GetFunctionPointerForDelegateInternal)
FCFuncEnd()


FCFuncStart(gArrayWithOffsetFuncs)
    FCFuncElement("CalculateCount", MarshalNative::CalculateCount)
FCFuncEnd()


FCFuncStart(gPolicyManagerFuncs)
    FCFuncElement("ResetCacheData", SecurityConfig::EcallResetCacheData)
    FCFuncElement("SaveDataByte", SecurityConfig::EcallSaveDataByte)
    FCFuncElement("RecoverData", SecurityConfig::EcallRecoverData)
    FCFuncElement("SetQuickCache", SecurityConfig::EcallSetQuickCache)
    FCFuncElement("GetCacheEntry", SecurityConfig::GetCacheEntry)
    FCFuncElement("AddCacheEntry", SecurityConfig::AddCacheEntry)
    FCFuncElement("_GetMachineDirectory", SecurityConfig::EcallGetMachineDirectory)
    FCFuncElement("_GetUserDirectory", SecurityConfig::EcallGetUserDirectory)
    FCFuncElement("WriteToEventLog", SecurityConfig::EcallWriteToEventLog)
#ifdef _DEBUG
    FCFuncElement("_DebugOut", SecurityConfig::DebugOut)
#endif
FCFuncEnd()


FCFuncStart(gHostExecutionContextManagerFuncs)
    FCFuncElement("ReleaseHostSecurityContext", HostExecutionContextManager::ReleaseSecurityContext)
    FCFuncElement("CloneHostSecurityContext", HostExecutionContextManager::CloneSecurityContext)
    FCFuncElement("CaptureHostSecurityContext", HostExecutionContextManager::CaptureSecurityContext)
    FCFuncElement("SetHostSecurityContext", HostExecutionContextManager::SetSecurityContext)
    FCFuncElement("HostSecurityManagerPresent", HostExecutionContextManager::HostPresent)
FCFuncEnd()

FCFuncStart(gIsolatedStorage)
    FCFuncElement("nGetCaller", COMIsolatedStorage::GetCaller)
FCFuncEnd()

FCFuncStart(gIsolatedStorageFile)
    FCFuncElement("nGetRootDir", COMIsolatedStorageFile::GetRootDir)
    FCFuncElement("nReserve", COMIsolatedStorageFile::Reserve)
    FCFuncElement("nGetUsage", COMIsolatedStorageFile::GetUsage)
    FCFuncElement("nOpen", COMIsolatedStorageFile::Open)
    FCFuncElement("nClose", COMIsolatedStorageFile::Close)
    FCFuncElement("nLock", COMIsolatedStorageFile::Lock)
    FCFuncElement("nCreateDirectoryWithDacl", COMIsolatedStorageFile::CreateDirectoryWithDacl)
FCFuncEnd()

FCFuncStart(gTypeLoadExceptionFuncs)
    FCFuncElement("GetTypeLoadExceptionMessage", GetTypeLoadExceptionMessage)
FCFuncEnd()

FCFuncStart(gFileLoadExceptionFuncs)
    FCFuncElement("GetFileLoadExceptionMessage", GetFileLoadExceptionMessage)
    FCFuncElement("GetMessageForHR", FileLoadException_GetMessageForHR)
FCFuncEnd()

FCFuncStart(gSignatureHelperFuncs)
    FCFuncElement("GetCorElementTypeFromClass", COMModule::GetSigTypeFromClassWrapper)
FCFuncEnd()


FCFuncStart(gMissingMemberExceptionFuncs)
    FCFuncElement("FormatSignature", MissingMemberException_FormatSignature)
FCFuncEnd()

FCFuncStart(gInterlockedFuncs)
    FCFuncElementSig("Increment", &gsig_SM_RefInt_RetInt, COMInterlocked::Increment32)
    FCFuncElementSig("Decrement", &gsig_SM_RefInt_RetInt, COMInterlocked::Decrement32)
    FCFuncElementSig("Increment", &gsig_SM_RefLong_RetLong, COMInterlocked::Increment64)
    FCFuncElementSig("Decrement", &gsig_SM_RefLong_RetLong, COMInterlocked::Decrement64)
    FCFuncElementSig("Exchange", &gsig_SM_RefInt_Int_RetInt, COMInterlocked::Exchange)
    FCFuncElementSig("Exchange", &gsig_SM_RefLong_Long_RetLong, COMInterlocked::Exchange64)    
    FCFuncElementSig("Exchange", &gsig_SM_RefFlt_Flt_RetFlt, COMInterlocked::ExchangeFloat)
    FCFuncElementSig("Exchange", &gsig_SM_RefDbl_Dbl_RetDbl, COMInterlocked::ExchangeDouble)
    FCFuncElementSig("Exchange", &gsig_SM_RefObj_Obj_RetObj, COMInterlocked::ExchangeObject)
    FCFuncElementSig("Exchange", &gsig_SM_RefIntPtr_IntPtr_RetIntPtr, COMInterlocked::ExchangePointer)
    FCFuncElementSig("CompareExchange", &gsig_SM_RefInt_Int_Int_RetInt, COMInterlocked::CompareExchange)
    FCFuncElementSig("CompareExchange", &gsig_SM_RefLong_Long_Long_RetLong, COMInterlocked::CompareExchange64)
    FCFuncElementSig("CompareExchange", &gsig_SM_RefFlt_Flt_Flt_RetFlt, COMInterlocked::CompareExchangeFloat)
    FCFuncElementSig("CompareExchange", &gsig_SM_RefDbl_Dbl_Dbl_RetDbl, COMInterlocked::CompareExchangeDouble)
    FCFuncElementSig("CompareExchange", &gsig_SM_RefObj_Obj_Obj_RetObj, COMInterlocked::CompareExchangeObject)
    FCFuncElementSig("CompareExchange", &gsig_SM_RefIntPtr_IntPtr_IntPtr_RetIntPtr, COMInterlocked::CompareExchangePointer)
    FCFuncElementSig("ExchangeAdd", &gsig_SM_RefInt_Int_RetInt, COMInterlocked::ExchangeAdd32)
    FCFuncElementSig("ExchangeAdd", &gsig_SM_RefLong_Long_RetLong, COMInterlocked::ExchangeAdd64)

    FCFuncElement("_Exchange", COMInterlocked::ExchangeGeneric)
    FCFuncElement("_CompareExchange", COMInterlocked::CompareExchangeGeneric)
    
FCFuncEnd()

FCFuncStart(gVarArgFuncs)
    FCFuncElementSig(COR_CTOR_METHOD_NAME, &gsig_IM_IntPtr_RetVoid, COMVarArgs::Init)
    FCFuncElementSig(COR_CTOR_METHOD_NAME, &gsig_IM_IntPtr_PtrVoid_RetVoid, COMVarArgs::Init2)
    FCFuncElement("GetRemainingCount", COMVarArgs::GetRemainingCount)
    FCFuncElement("_GetNextArgType", COMVarArgs::GetNextArgType)
    FCFuncElement("FCallGetNextArg", COMVarArgs::DoGetNextArg)
    FCFuncElement("InternalGetNextArg", COMVarArgs::GetNextArg2)
FCFuncEnd()

FCFuncStart(gMonitorFuncs)
    FCFuncElement("Enter", JIT_MonEnter)
    FCFuncElement("Exit", JIT_MonExit)
    FCFuncElement("TryEnterTimeout", JIT_MonTryEnter)
    FCFuncElement("ObjWait", ObjectNative::WaitTimeout)
    FCFuncElement("ObjPulse", ObjectNative::Pulse)
    FCFuncElement("ObjPulseAll", ObjectNative::PulseAll)
    FCFuncElement("ReliableEnter", JIT_MonReliableEnter)
FCFuncEnd()

FCFuncStart(gOverlappedFuncs)
    FCFuncElement("AllocateNativeOverlapped", AllocateNativeOverlapped)
    FCFuncElement("FreeNativeOverlapped", FreeNativeOverlapped)
    FCFuncElement("GetOverlappedFromNative", GetOverlappedFromNative)
FCFuncEnd()

FCFuncStart(gCompilerFuncs)
    FCFuncElement("GetObjectValue", ObjectNative::GetObjectValue)
    FCIntrinsic("InitializeArray", COMArrayInfo::InitializeArray, CORINFO_INTRINSIC_InitializeArray)
    FCFuncElement("_RunClassConstructor", ReflectionInvocation::RunClassConstructor)
    FCFuncElement("_RunModuleConstructor", ReflectionInvocation::RunModuleConstructor)
    FCFuncElement("_PrepareMethod", ReflectionInvocation::PrepareMethod)
    FCFuncElement("PrepareDelegate", ReflectionInvocation::PrepareDelegate)
    FCFuncElement("ProbeForSufficientStack", ReflectionInvocation::ProbeForSufficientStack)
    FCFuncElement("ExecuteCodeWithGuaranteedCleanup", ReflectionInvocation::ExecuteCodeWithGuaranteedCleanup)
FCFuncEnd()

FCFuncStart(gStrongNameKeyPairFuncs)
    FCFuncElement("nGetPublicKey", SecurityPolicy::GetPublicKey)
FCFuncEnd()

FCFuncStart(gContextSynchronizationFuncs)
    FCFuncElement("WaitHelper", SynchronizationContextNative::WaitHelper)
FCFuncEnd()



FCFuncStart(gGCHandleFuncs)
    FCFuncElement("InternalAlloc", MarshalNative::GCHandleInternalAlloc)
    FCFuncElement("InternalFree", MarshalNative::GCHandleInternalFree)
    FCFuncElement("InternalGet", MarshalNative::GCHandleInternalGet)
    FCFuncElement("InternalSet", MarshalNative::GCHandleInternalSet)
    FCFuncElement("InternalCompareExchange", MarshalNative::GCHandleInternalCompareExchange)
    FCFuncElement("InternalAddrOfPinnedObject", MarshalNative::GCHandleInternalAddrOfPinnedObject)
    FCFuncElement("InternalCheckDomain", MarshalNative::GCHandleInternalCheckDomain)
FCFuncEnd()

FCFuncStart(gConfigHelper)
    FCFuncElement("RunParser", ConfigNative::RunParser)
FCFuncEnd()



#define FCClassElement(name,namespace,funcs) {name, namespace, funcs},

// Note these have to remain sorted by name:namespace pair (Assert will wack you if you dont)
static const
ECClass gECClasses[] =
{
    FCClassElement("AppDomain", "System", gAppDomainFuncs)
    FCClassElement("AppDomainManager", "System", gAppDomainManagerFuncs)
    FCClassElement("AppDomainSetup", "System", gAppDomainSetupFuncs)
    FCClassElement("ArgIterator", "System", gVarArgFuncs)
    FCClassElement("Array", "System", gArrayFuncs)
    FCClassElement("ArrayWithOffset", "System.Runtime.InteropServices", gArrayWithOffsetFuncs)
    FCClassElement("Assembly", "System.Reflection", gAssemblyFuncs)
    FCClassElement("AssemblyHandle", "System", gCOMAssemblyHandleFuncs)
    FCClassElement("AssemblyName", "System.Reflection", gAssemblyNameFuncs)
    FCClassElement("Assert", "System.Diagnostics", gDiagnosticsAssert)
    FCClassElement("BCLDebug", "System", gBCLDebugFuncs)
    FCClassElement("Buffer", "System", gBufferFuncs)
    FCClassElement("Calendar", "System.Globalization", gCalendarFuncs)
    FCClassElement("CalendarTable", "System.Globalization", gCalendarTableFuncs)
    FCClassElement("ChannelServices", "System.Runtime.Remoting.Channels", gChannelServicesFuncs)
    FCClassElement("CharUnicodeInfo", "System.Globalization", gCharacterInfoFuncs)
    FCClassElement("CodeAccessSecurityEngine", "System.Security", gCOMCodeAccessSecurityEngineFuncs)
    FCClassElement("CompareInfo", "System.Globalization", gCompareInfoFuncs)
    FCClassElement("CompressedStack", "System.Threading", gCOMCodeAccessSecurityEngineFuncs)
    FCClassElement("Config", "System.Security.Util", gPolicyManagerFuncs)
    FCClassElement("ConfigServer", "System", gConfigHelper)
    FCClassElement("Context", "System.Runtime.Remoting.Contexts", gContextFuncs)
    FCClassElement("Convert", "System", gConvertFuncs)
    FCClassElement("CriticalHandle", "System.Runtime.InteropServices", gCriticalHandleFuncs)
    FCClassElement("CultureInfo", "System.Globalization", gCultureInfoFuncs)
    FCClassElement("Currency", "System", gCurrencyFuncs)
    FCClassElement("CurrentSystemTimeZone", "System", gTimeZoneFuncs)
    FCClassElement("CustomAttribute", "System.Reflection", gCOMCustomAttributeFuncs)
    FCClassElement("CustomAttributeEncodedArgument", "System.Reflection", gCustomAttributeEncodedArgument)
    FCClassElement("DateTime", "System", gDateTimeFuncs)
    FCClassElement("Debugger", "System.Diagnostics", gDiagnosticsDebugger)
    FCClassElement("Decimal", "System", gDecimalFuncs)
    FCClassElement("DefaultBinder", "System", gCOMDefaultBinderFuncs)
    FCClassElement("Delegate", "System", gDelegateFuncs)
    FCClassElement("DomainCompressedStack", "System.Threading", gCOMCodeAccessSecurityEngineFuncs)    
    FCClassElement("EncodingTable", "System.Globalization", gEncodingTableFuncs)
    FCClassElement("Enum", "System", gEnumFuncs)
    FCClassElement("Environment", "System", gEnvironmentFuncs)
    FCClassElement("Exception", "System", gExceptionFuncs)
    FCClassElement("FastArrayHandle", "System", gFastArrayFuncs)
    FCClassElement("FileIOAccess", "System.Security.Permissions", gCOMFileIOAccessFuncs)
    FCClassElement("FileLoadException", "System.IO", gFileLoadExceptionFuncs)
    FCClassElement("FormatterServices", "System.Runtime.Serialization", gSerializationFuncs)
    FCClassElement("FrameSecurityDescriptor", "System.Security", gCOMCodeAccessSecurityEngineFuncs)
    FCClassElement("Fusion", "Microsoft.Win32", gFusionWrapFuncs)
    FCClassElement("GC", "System", gGCInterfaceFuncs)
    FCClassElement("GCHandle", "System.Runtime.InteropServices", gGCHandleFuncs)
    FCClassElement("GlobalizationAssembly", "System.Globalization", gGlobalizationAssemblyFuncs)
    FCClassElement("Guid", "System", gGuidFuncs)
    FCClassElement("HostExecutionContextManager", "System.Threading", gHostExecutionContextManagerFuncs)
    FCClassElement("Interlocked", "System.Threading", gInterlockedFuncs)
    FCClassElement("IsolatedStorage", "System.IO.IsolatedStorage", gIsolatedStorage)
    FCClassElement("IsolatedStorageFile", "System.IO.IsolatedStorage", gIsolatedStorageFile)
    FCClassElement("Log", "System.Diagnostics", gDiagnosticsLog)
    FCClassElement("Marshal", "System.Runtime.InteropServices", gInteropMarshalFuncs)
    FCClassElement("Math", "System", gMathFuncs)
    FCClassElement("MemoryFailPoint", "System.Runtime", gMemoryFailPointFuncs)
    FCClassElement("Message", "System.Runtime.Remoting.Messaging", gMessageFuncs)
    FCClassElement("MetadataImport", "System.Reflection", gMetaDataImport)
    FCClassElement("MethodBase", "System.Reflection", gMethodBase)
    FCClassElement("MethodDescChunkHandle", "System", gMethodDescChunk)
    FCClassElement("MethodRental", "System.Reflection.Emit", gCOMMethodRental)
    FCClassElement("MissingMemberException", "System",  gMissingMemberExceptionFuncs)
    FCClassElement("Module", "System.Reflection", gCOMModuleFuncs)
    FCClassElement("ModuleHandle", "System", gCOMModuleHandleFuncs)
    FCClassElement("Monitor", "System.Threading", gMonitorFuncs)
    FCClassElement("Normalization", "System.Text", gNormalizationFuncs)
    FCClassElement("Number", "System", gNumberFuncs)
    FCClassElement("Object", "System", gObjectFuncs)
    FCClassElement("OverlappedData", "System.Threading", gOverlappedFuncs)
    FCClassElement("ParseNumbers", "System", gParseNumbersFuncs)
    FCClassElement("Path", "System.IO", gPathFuncs)
    FCClassElement("PolicyLevel", "System.Security.Policy", gPolicyManagerFuncs)
    FCClassElement("PolicyManager", "System.Security", gPolicyManagerFuncs)
    FCClassElement("PseudoCustomAttribute", "System.Reflection", gPseudoCustomAttribute)
    FCClassElement("ReaderWriterLock", "System.Threading", gRWLockFuncs)
    FCClassElement("RealProxy", "System.Runtime.Remoting.Proxies", gRealProxyFuncs)
    FCClassElement("RegisteredWaitHandleSafe", "System.Threading", gRegisteredWaitHandleFuncs)

    FCClassElement("RemotingServices", "System.Runtime.Remoting", gRemotingFuncs)
    FCClassElement("RtFieldInfo", "System.Reflection", gRuntimeFieldInfoFuncs)
    FCClassElement("RuntimeEnvironment", "System.Runtime.InteropServices", gRuntimeEnvironmentFuncs)
    FCClassElement("RuntimeFieldHandle", "System", gCOMFieldHandleNewFuncs)
    FCClassElement("RuntimeHelpers", "System.Runtime.CompilerServices", gCompilerFuncs)
    FCClassElement("RuntimeMethodHandle", "System", gRuntimeMethodHandle)
    FCClassElement("RuntimeType", "System", gSystem_RuntimeType)
    FCClassElement("RuntimeTypeHandle", "System", gCOMTypeHandleFuncs)
    FCClassElement("SafeFusionHandle", "Microsoft.Win32.SafeHandles", gFusionWrapFuncs)
    FCClassElement("SafeHandle", "System.Runtime.InteropServices", gSafeHandleFuncs)
    FCClassElement("SecurityContext", "System.Security", gCOMSecurityManagerFuncs)
    FCClassElement("SecurityContextFrame", "System.Reflection", gSecurityContextFrameFuncs)
    FCClassElement("SecurityManager", "System.Security", gCOMSecurityManagerFuncs)
    FCClassElement("SecurityRuntime", "System.Security", gCOMSecurityRuntimeFuncs)
    FCClassElement("Signature", "System", gSignatureNative)
    FCClassElement("SignatureHelper", "System.Reflection.Emit", gSignatureHelperFuncs)
    FCClassElement("StackBuilderSink", "System.Runtime.Remoting.Messaging", gStackBuilderSinkFuncs)
    FCClassElement("StackTrace", "System.Diagnostics", gDiagnosticsStackTrace)
    FCClassElement("String", "System", gStringFuncs)
    FCClassElement("StringBuilder", "System.Text", gStringBufferFuncs)
    FCClassElement("StringExpressionSet", "System.Security.Util", gCOMStringExpressionSetFuncs)
    FCClassElement("StrongNameKeyPair", "System.Reflection", gStrongNameKeyPairFuncs)
    FCClassElement("SynchronizationContext", "System.Threading", gContextSynchronizationFuncs)
    FCClassElement("TextInfo", "System.Globalization", gTextInfoFuncs)
    FCClassElement("Thread", "System.Threading", gThreadFuncs)
    FCClassElement("ThreadPool", "System.Threading", gThreadPoolFuncs)
    FCClassElement("TimerBase", "System.Threading", gTimerFuncs)
    FCClassElement("Type", "System", gSystem_Type)
    FCClassElement("TypeBuilder", "System.Reflection.Emit", gCOMClassWriter)
    FCClassElement("TypeLoadException", "System", gTypeLoadExceptionFuncs)
    FCClassElement("TypeNameBuilder", "System.Reflection.Emit", gTypeNameBuilder)
    FCClassElement("TypedReference", "System", gTypedReferenceFuncs)
#ifndef PLATFORM_UNIX
    FCClassElement("URLString", "System.Security.Util", gCOMUrlStringFuncs)
#endif
    FCClassElement("Utf8String", "System", gUtf8String)
    FCClassElement("ValueType", "System", gValueTypeFuncs)
    FCClassElement("Variant", "System", gVariantFuncs)
    FCClassElement("WaitHandle", "System.Threading", gWaitHandleFuncs)
    FCClassElement("Zone", "System.Security.Policy", gCOMSecurityZone)
};

// METHOD__STRING__CTORF_XXX has to be in same order as ECall::CtorCharXxx
#define METHOD__STRING__CTORF_FIRST METHOD__STRING__CTORF_CHARARRAY
C_ASSERT(METHOD__STRING__CTORF_FIRST + 0 == METHOD__STRING__CTORF_CHARARRAY);
C_ASSERT(METHOD__STRING__CTORF_FIRST + 1 == METHOD__STRING__CTORF_CHARARRAY_START_LEN);
C_ASSERT(METHOD__STRING__CTORF_FIRST + 2 == METHOD__STRING__CTORF_CHAR_COUNT);
C_ASSERT(METHOD__STRING__CTORF_FIRST + 3 == METHOD__STRING__CTORF_CHARPTR);
C_ASSERT(METHOD__STRING__CTORF_FIRST + 4 == METHOD__STRING__CTORF_CHARPTR_START_LEN);

// ECall::CtorCharXxx has to be in same order as METHOD__STRING__CTORF_XXX
#define ECallCtor_First ECall::CtorCharArrayManaged
C_ASSERT(ECallCtor_First + 0 == ECall::CtorCharArrayManaged);
C_ASSERT(ECallCtor_First + 1 == ECall::CtorCharArrayStartLengthManaged);
C_ASSERT(ECallCtor_First + 2 == ECall::CtorCharCountManaged);
C_ASSERT(ECallCtor_First + 3 == ECall::CtorCharPtrManaged);
C_ASSERT(ECallCtor_First + 4 == ECall::CtorCharPtrStartLengthManaged);

#define NumberOfStringConstructors 5

void ECall::PopulateManagedStringConstructors()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    INDEBUG(static bool fInitialized = false);
    _ASSERTE(!fInitialized);    // assume this method is only called once
    _ASSERTE(g_pStringClass != NULL);

#ifndef HARDBOUND_DYNAMIC_CALLS
    Module *pModule = g_Mscorlib.GetModule();
    _ASSERTE(pModule != NULL);

    LoaderHeap* pMscorlibStubHeap = pModule->GetDomain()->GetStubHeap();
#endif

    for (int i = 0; i < NumberOfStringConstructors; i++)
    {
        MethodDesc* pMD = g_Mscorlib.FetchMethod((BinderMethodID)(METHOD__STRING__CTORF_FIRST + i));
        _ASSERTE(pMD != NULL);
    
        void* pDest = (void*) pMD->GetMultiCallableAddrOfCode();

#ifndef HARDBOUND_DYNAMIC_CALLS
        // The method entrypoints has to be unique. Create extra indirection
        // do distinguish the ctor method from the actual ctor.
        Stub* pStub = InterceptStub::NewInterceptedStub(pMscorlibStubHeap,
                                                        JUMP_ALLOCATE_SIZE,
                                                        NULL,
                                                        pDest,
                                                        FALSE);

        const BYTE* pEntryPoint = pStub->GetEntryPoint();
        emitJump((BYTE*)pEntryPoint, pDest);

        pDest = (void*)pEntryPoint;
#endif

        ECall::DynamicallyAssignFCallImpl(pDest, ECallCtor_First + i);
    }
    INDEBUG(fInitialized = true);
}

static SpinLock* gFCallLock;

#endif // !DACCESS_COMPILE

// To provide a quick check, this is the lowest and highest
// addresses of any FCALL starting address
GVAL_IMPL_INIT(TADDR, gLowestFCall, (TADDR)-1);
GVAL_IMPL(TADDR, gHighestFCall);

GARY_IMPL(PTR_ECFunc, gFCallMethods, FCALL_HASH_SIZE);

inline unsigned FCallHash(TADDR pTarg) {
    LEAF_CONTRACT;

    C_ASSERT(FCALL_HASH_SIZE < PAGE_SIZE);
    return (unsigned)((pTarg ^ ((pTarg % PAGE_SIZE) / FCALL_HASH_SIZE)) % FCALL_HASH_SIZE);
}

#ifdef DACCESS_COMPILE

GARY_IMPL(TADDR, g_FCDynamicallyAssignedImplementations,
          ECall::NUM_DYNAMICALLY_ASSIGNED_FCALL_IMPLEMENTATIONS);

#else // !DACCESS_COMPILE

TADDR g_FCDynamicallyAssignedImplementations[ECall::NUM_DYNAMICALLY_ASSIGNED_FCALL_IMPLEMENTATIONS] = {
    #undef DYNAMICALLY_ASSIGNED_FCALL_IMPL
    #define DYNAMICALLY_ASSIGNED_FCALL_IMPL(id,defaultimpl) (TADDR)GetEEFuncEntryPoint(defaultimpl),
    DYNAMICALLY_ASSIGNED_FCALLS()
};

INDEBUG(static BOOL g_fDynamicalyAssignedFCallsPopulated = FALSE;)

void ECall::DynamicallyAssignFCallImpl(LPVOID impl, DWORD index)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    _ASSERTE(index < NUM_DYNAMICALLY_ASSIGNED_FCALL_IMPLEMENTATIONS);

    // the code assumes that all dynamic implementations are assigned to FCALLs
    // before ECall::PopulateDynamicallyAssignedFCalls is called
    _ASSERTE(!g_fDynamicalyAssignedFCallsPopulated);

    g_FCDynamicallyAssignedImplementations[index] = (TADDR)impl;
}

void ECall::PopulateDynamicallyAssignedFCalls()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

#ifdef HARDBOUND_DYNAMIC_CALLS


    for (int i=0; i<NUM_DYNAMICALLY_ASSIGNED_FCALL_IMPLEMENTATIONS; i++)
    {
        TADDR pImplCode = g_FCDynamicallyAssignedImplementations[i];
        _ASSERTE(pImplCode != NULL);

        emitJump(GetDynamicFCallThunk(i), (LPVOID)pImplCode);
    }
#else // HARDBOUND_DYNAMIC_CALLS


#endif // HARDBOUND_DYNAMIC_CALLS

    INDEBUG(g_fDynamicalyAssignedFCallsPopulated = TRUE;)
}

/*******************************************************************************/
static INT FindImplsIndexForClass(MethodTable* pMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    LPCUTF8 pszNamespace = 0;
    LPCUTF8 pszName = pMT->GetClass()->GetFullyQualifiedNameInfo(&pszNamespace);

    // Array classes get null from the above routine, but they have no ecalls.
    if (pszName == NULL)
        return (-1);

    unsigned low  = 0;
    unsigned high = sizeof(gECClasses)/sizeof(ECClass);

#ifdef _DEBUG
    static bool checkedSort = false;
    if (!checkedSort) {
        checkedSort = true;
        for (unsigned i = 1; i < high; i++)  {
                // Make certain list is sorted!
            int cmp = strcmp(gECClasses[i].m_wszClassName, gECClasses[i-1].m_wszClassName);
            if (cmp == 0)
                cmp = strcmp(gECClasses[i].m_wszNameSpace, gECClasses[i-1].m_wszNameSpace);
            _ASSERTE(cmp > 0);      // Hey, you forgot to sort the new class
        }
    }
#endif // _DEBUG
    while (high > low) {
        unsigned mid  = (high + low) / 2;
        int cmp = strcmp(pszName, gECClasses[mid].m_wszClassName);
        if (cmp == 0)
            cmp = strcmp(pszNamespace, gECClasses[mid].m_wszNameSpace);

        if (cmp == 0) {
            return(mid);
        }
        if (cmp > 0)
            low = mid+1;
        else
            high = mid;
    }

    return (-1);
}

/*******************************************************************************/
/*  Finds the implementation for the given method desc.  */

static INT FindECIndexForMethod(MethodDesc *pMD, LPVOID* impls)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    LPCUTF8 szMethodName = pMD->GetName();
    PCCOR_SIGNATURE pMethodSig;
    ULONG       cbMethodSigLen;

    pMD->GetSig(&pMethodSig, &cbMethodSigLen);
    Module* pModule = pMD->GetModule();

    for (ECFunc* cur = (ECFunc*)impls; !cur->IsEndOfArray(); cur = cur->NextInArray())
    {
        if (strcmp(cur->m_wszMethodName, szMethodName) != 0)
            continue;

        if (cur->HasSignature())
        {
            PCCOR_SIGNATURE pBinarySig;
            ULONG       pBinarySigLen;

            cur->m_wszMethodSig->GetBinarySig(&pBinarySig, &pBinarySigLen);

            //@GENERICS: none of these methods belong to generic classes so there is no instantiation info to pass in
            if (!MetaSig::CompareMethodSigs(pMethodSig, cbMethodSigLen, pModule, NULL,
                                                pBinarySig, pBinarySigLen, cur->m_wszMethodSig->GetModule(), NULL))
            {
                continue;
            }
        }

        // We have found a match!
        return (LPVOID*)cur - impls;
    }

    return -1;
}

/*******************************************************************************/
/* ID is formed of 2 USHORTs - class index  in high word, method index in low word.  */
/* class index starts at 1. id == 0 means no implementation.                    */

DWORD ECall::GetIDForMethod(MethodDesc *pMD)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    INT ImplsIndex = FindImplsIndexForClass(pMD->GetMethodTable());
    if (ImplsIndex < 0)
        return 0;
    INT ECIndex = FindECIndexForMethod(pMD, gECClasses[ImplsIndex].m_pECFunc);
    if (ECIndex < 0)
        return 0;

    return (ImplsIndex<<16) | (ECIndex + 1);
}

static ECFunc *FindECFuncForID(DWORD id)
{
    LEAF_CONTRACT;

    if (id == 0)
        return NULL;

    INT ImplsIndex  = (id >> 16);
    INT ECIndex     = (id & 0xffff) - 1;

    return (ECFunc*)(gECClasses[ImplsIndex].m_pECFunc + ECIndex);
}

static ECFunc* FindECFuncForMethod(MethodDesc* pMD)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // check the cache
    #define BYMD_CACHE_SIZE 32
    C_ASSERT(((BYMD_CACHE_SIZE-1) & BYMD_CACHE_SIZE) == 0);     // Must be a power of 2
    static ECFunc* gByMDCache[BYMD_CACHE_SIZE];
    ECFunc**cacheEntry = &gByMDCache[((((size_t) pMD) >> 3) & BYMD_CACHE_SIZE-1)];

    ECFunc* cur = *cacheEntry;
    if (cur != 0 && cur->m_pMD == pMD)
        return(cur);

    cur = FindECFuncForID(ECall::GetIDForMethod(pMD));
    if (cur == 0)
        return(0);

    // put in the cache
    *cacheEntry = cur;

    return cur;
}

static void CacheFCallImpl(MethodDesc* pMDofCall, TADDR pImplementation)
{
    WRAPPER_CONTRACT;

    if (pMDofCall->HasPrecode() && !pMDofCall->HasNativeCode())
    {
        pMDofCall->GetPrecode()->SetNativeCode(pImplementation);
    }
    _ASSERTE(!pMDofCall->HasNativeCode() || pMDofCall->GetNativeCode() == pImplementation);
}

/*******************************************************************************
* Returns 0 if it is an ECALL,
* Otherwise returns the native entry point (FCALL)
*/
void* ECall::GetFCallImpl(MethodDesc* pMDofCall, BOOL fStaticOnly /*=FALSE*/)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(pMDofCall->IsFCall());
    }
    CONTRACTL_END;

#ifdef HARDBOUND_DYNAMIC_CALLS
    // All calls are static for the outside world with HARDBOUND_DYNAMIC_CALLS
    fStaticOnly = FALSE;
#endif

    // We may return right away if we do not need to check the fStaticOnly flag
    if (!fStaticOnly)
    {
        TADDR pNativeCode = pMDofCall->GetNativeCode();
        if (pNativeCode != NULL)
            return (void*)pNativeCode;
    }

    MethodDesc *pBaseMD = pMDofCall;

    MethodTable *pMTofCall = pMDofCall->GetMethodTable();


    //
    // Delegate constructors are FCalls for which the entrypoint points to the target of the delegate
    // We have to intercept these and set the call target to the helper COMDelegate::DelegateConstruct
    //
    if (pMTofCall->IsAnyDelegateClass())
    {
        // COMDelegate::DelegateConstruct is the only fcall used by user delegates.
        // All the other gDelegateFuncs are only used by System.Delegate
        _ASSERTE(pMDofCall->IsCtor());

        // We need to set up the ECFunc properly.  We don't want to use the pMD passed in,
        // since it may disappear.  Instead, use the stable one on Delegate.  Remember
        // that this is 1:M between the FCall and the pMDofCalls.
        pBaseMD = g_Mscorlib.GetMethod(METHOD__DELEGATE__CONSTRUCT_DELEGATE);

        TADDR pNativeCode = pBaseMD->GetNativeCode();
        if (pNativeCode != NULL)
        {
            CacheFCallImpl(pMDofCall, pNativeCode);
            return (void*)pNativeCode;
        }
    }

    // All cases of nativeimage should have been handled already - we do not want to
    // bring the fcall tables into working set. The only exception is the fStaticOnly check
    // as it happens only the ngen time.
    _ASSERTE(!pBaseMD->GetModule()->HasNativeImage() || fStaticOnly);

    ECFunc* ret = FindECFuncForMethod(pBaseMD);

    if (ret == 0)
        return(0);

    TADDR pImplementation = (TADDR)ret->m_pImplementation;

    int iDynamicID = ret->DynamicID();
    if (iDynamicID != InvalidDynamicFCallId) {
#ifdef HARDBOUND_DYNAMIC_CALLS
        pImplementation = (TADDR)GetDynamicFCallThunk(iDynamicID);
#else
        pImplementation = g_FCDynamicallyAssignedImplementations[iDynamicID];
#endif
        ret->m_pImplementation = (LPVOID)pImplementation;
    }
    _ASSERTE(pImplementation != NULL);

    {
        SpinLockHolder slh(gFCallLock);

        if(gLowestFCall > pImplementation)
            gLowestFCall = pImplementation;
        if(gHighestFCall < pImplementation)
            gHighestFCall = pImplementation;

        // add to hash table, making sure I am not already there
        ECFunc** spot = &gFCallMethods[FCallHash(pImplementation)];
        for(;;) {
            if (*spot == 0) {                   // found end of list
                ret->m_pMD = pBaseMD;           // remember for reverse mapping
                *spot = ret;
                _ASSERTE(ret->m_pNext == 0);
                break;
            }
            if (*spot == ret)                   // already in list
                break;
            spot = &(*spot)->m_pNext;
        }
    }

    CacheFCallImpl(pMDofCall, pImplementation);

    if (fStaticOnly && iDynamicID != InvalidDynamicFCallId) {
        return NULL;
    }

    return (LPVOID)(pImplementation);
}

BOOL ECall::IsSharedFCallImpl(TADDR pImpl)
{
    LEAF_CONTRACT;

    return 
        ((void*)pImpl == GetEEFuncEntryPoint(COMDelegate::DelegateConstruct));
}

BOOL ECall::CheckUnusedFCalls()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    BOOL fUnusedFCallsFound = FALSE;

    unsigned num = sizeof(gECClasses)/sizeof(ECClass);
    for (unsigned i=0; i < num; i++)
    {
        for (ECFunc* ptr = (ECFunc*)gECClasses[i].m_pECFunc; !ptr->IsEndOfArray(); ptr = ptr->NextInArray())
        {
            if (MapTargetBackToMethod((TADDR)ptr->m_pImplementation) == NULL
                && ptr->DynamicID() == InvalidDynamicFCallId && !ptr->IsUnreferenced())
            {
                CONSISTENCY_CHECK_MSGF(false, ("Unused fcall found: %s.%s::%s\n", 
                    gECClasses[i].m_wszNameSpace, gECClasses[i].m_wszClassName, ptr->m_wszMethodName));

                fUnusedFCallsFound = TRUE;
            }
        }
    }

    return !fUnusedFCallsFound;
}



/* static */
void ECall::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("ECall");

    m_pArrayStubCache = new ArrayStubCache();

    gFCallLock = new  SpinLock();
    gFCallLock->Init(LOCK_FCALL);
}

#endif // !DACCESS_COMPILE

/* static */
ECFunc* ECall::FindTarget(TADDR pTarg)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // Could this possibily be an FCall?
    if (pTarg < gLowestFCall || pTarg > gHighestFCall)
        return(NULL);

    ECFunc *pECFunc = gFCallMethods[FCallHash(pTarg)];
    while (pECFunc != 0) {
        if ((TADDR)pECFunc->m_pImplementation == pTarg)
            return pECFunc;
        pECFunc = pECFunc->m_pNext;
    }
    return NULL;
}

MethodDesc* ECall::MapTargetBackToMethod(TADDR pTarg)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // Searching all of the entries is expensive
    // and we are often called with pTarg == NULL so
    // check for tyhis value and early exit.

    if (!pTarg)
        return NULL;


    ECFunc *info = ECall::FindTarget(pTarg);
    if (info == NULL)
        return NULL;
    return(info->m_pMD);
}

#ifndef DACCESS_COMPILE

/* static */
CorInfoIntrinsics ECall::GetIntrinsicID(MethodDesc* pMD)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(pMD->IsFCall());
    }
    CONTRACTL_END;

    ECFunc* info = NULL;

    TADDR pNativeCode = pMD->GetNativeCode();
    if (pNativeCode != NULL)
        info = FindTarget(pNativeCode); // fast hash lookup

    if (info == NULL)
        info = FindECFuncForMethod(pMD); // try slow path

    if (info == NULL)
        return(CORINFO_INTRINSIC_Illegal);

    return info->IntrinsicID();
}

/* static */
void ECall::ReportMissingFCall(MethodDesc *pMD)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_INTOLERANT;
    }
    CONTRACTL_END;

#ifdef _DEBUG
    if (Security::IsSystem(pMD->GetModule()->GetSecurityDescriptor())) {
        //

        CONSISTENCY_CHECK_MSGF(false,
            ("Could not find an ECALL entry for %s::%s.\n"
            "Read comment above this assert in vm/ecall.cpp\n",
            pMD->m_pszDebugClassName, pMD->m_pszDebugMethodName));
    }
#endif // _DEBUG

    // As an added security measure, ECall methods only work if
    // they're packed into MSCORLIB.DLL.
    COMPlusThrow(kSecurityException, BFA_ECALLS_MUST_BE_IN_SYS_MOD);
}




#ifdef _DEBUG

void FCallAssert(void*& cache, void* target)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_DEBUG_ONLY;

    if (CLRHosted())
    {
        _ASSERTE (!GetThread()->HasThreadStateNC(Thread::TSNC_SOIntolerant));
    }

    if (cache != 0)
    {
        return;
    }

    //
    // Special case fcalls with 1:N mapping between implementation and methoddesc
    //
    if (ECall::IsSharedFCallImpl((TADDR)target))
    {
        cache = (void*)1;
        return;
    }

    MethodDesc* pMD = ECall::MapTargetBackToMethod((TADDR)target);
    if (pMD != 0)
    {

        return;
    }

    // Slow but only for debugging.  This is needed because in some places
    // we call FCALLs directly from EE code.

    unsigned num = sizeof(gECClasses)/sizeof(ECClass);
    for (unsigned i=0; i < num; i++)
    {
        for (ECFunc* ptr = (ECFunc*)gECClasses[i].m_pECFunc; !ptr->IsEndOfArray(); ptr = ptr->NextInArray())
        {
            if (ptr->m_pImplementation  == target)
            {                
                cache = target;
                return;
            }
        }
    }

    // Now check the dynamically assigned table too.
    for (unsigned i=0; i<ECall::NUM_DYNAMICALLY_ASSIGNED_FCALL_IMPLEMENTATIONS; i++)
    {
        if (g_FCDynamicallyAssignedImplementations[i] == (TADDR)target)
        {
            cache = target;
            return;
        }
    }

    _ASSERTE(!"Could not find FCall implemenation in ECall.cpp");
}

void HCallAssert(void*& cache, void* target)
{
    CONTRACTL
    {
        SO_TOLERANT;     // STATIC_CONTRACT_DEBUG_ONLY
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        DEBUG_ONLY;
    }
    CONTRACTL_END;

    if (cache != 0)
        cache = ECall::MapTargetBackToMethod((TADDR)target);
    _ASSERTE(cache == 0 || "Use FCIMPL for fcalls");
}

#endif // _DEBUG

#endif // !DACCESS_COMPILE

#ifdef DACCESS_COMPILE

void ECall::EnumFCallMethods()
{
    gLowestFCall.EnumMem();
    gHighestFCall.EnumMem();
    gFCallMethods.EnumMem();
    
    for (UINT i=0;i<FCALL_HASH_SIZE;i++)
    {
        ECFunc *ecFunc = gFCallMethods[i];
        while (ecFunc)
        {
            DacEnumMemoryRegion(PTR_HOST_TO_TADDR(ecFunc),sizeof(ECFunc));    
            ecFunc = ecFunc->m_pNext;
        }
    }
}

#endif // DACCESS_COMPILE
