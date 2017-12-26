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
//====================================================================
//
// Purpose: Lists the commonly-used Runtime Exceptions visible to users.
// 
// Date: This file was generated on 98/08/31 11:51:02 AM
//
//====================================================================

// If you add an exception, modify CorError.h to add an HResult there.
// (Guidelines for picking a unique number for your HRESULT are in CorError.h)
// Also modify your managed Exception class to include its HResult.  
// Modify __HResults in the same directory as your exception, to include
// your new HResult.  And of course, add your exception and symbolic
// name for your HResult to the list below so it can be thrown from
// within the EE and recognized in Interop scenarios.


// This is an exhaustive list of all exceptions that can be
// thrown by the EE itself.  If you add to this list the IL spec
// needs to be updated!                                             

// Note: When multiple exceptions map to the same hresult it is very important
//       that the exception that should be created when the hresult in question
//       is returned by a function be FIRST in the list.
//


//
// These are the macro's that need to be implemented before this file is included.
//

//
// EXCEPTION_BEGIN_DEFINE(ns, reKind, hr, bHRformessage)
//
// This macro starts an exception definition.
//
// ns               Namespace of the exception.
// reKind           Name of the exception.
// hr               Basic HRESULT that this exception maps to.
// bHRformessage    When the exception is thrown from the EE, if this argument is true
//                  the EE will create a string with the HRESULT, so that you get a more 
//                  meaningful error message than let's say AssemblyLoadException.
//                  Usually you want to set this to true if your exception corresponds to
//                  more than one HRESULT.
//
//

//
// #define EXCEPTION_ADD_HR(hr)
//
// This macro adds an additional HRESULT that maps to the exception.
//
// hr          Additional HRESULT that maps to the exception.
//

//
// #define EXCEPTION_END_DEFINE()
//
// This macro terminates the exception definition.
//


//
// Namespaces used to define the exceptions.
//


#include "namespace.h"

//
// A helper macro to define simple exceptions. A simple exception is an exception that maps
// to a single HR.
//

#define DEFINE_EXCEPTION_SIMPLE(ns, reKind, bHRformessage, hr) \
    EXCEPTION_BEGIN_DEFINE(ns, reKind, bHRformessage, hr) \
    EXCEPTION_END_DEFINE() \

// 
// This is a more convenient helper macro for exceptions when you need two different
// HRESULTs to map to the same exception.  You can pretty trivially expand this to
// support N different HRESULTs.
//

#define DEFINE_EXCEPTION_2HRESULTS(ns, reKind, bHRformessage, hr1, hr2) \
    EXCEPTION_BEGIN_DEFINE(ns, reKind, bHRformessage, hr1) \
    EXCEPTION_ADD_HR(hr2) \
    EXCEPTION_END_DEFINE() \

#define DEFINE_EXCEPTION_3HRESULTS(ns, reKind, bHRformessage, hr1, hr2, hr3) \
    EXCEPTION_BEGIN_DEFINE(ns, reKind, bHRformessage, hr1) \
    EXCEPTION_ADD_HR(hr2) \
    EXCEPTION_ADD_HR(hr3) \
    EXCEPTION_END_DEFINE() \

#define DEFINE_EXCEPTION_4HRESULTS(ns, reKind, bHRformessage, hr1, hr2, hr3, hr4) \
    EXCEPTION_BEGIN_DEFINE(ns, reKind, bHRformessage, hr1) \
    EXCEPTION_ADD_HR(hr2) \
    EXCEPTION_ADD_HR(hr3) \
    EXCEPTION_ADD_HR(hr4) \
    EXCEPTION_END_DEFINE() \

#define DEFINE_EXCEPTION_5HRESULTS(ns, reKind, bHRformessage, hr1, hr2, hr3, hr4, hr5) \
    EXCEPTION_BEGIN_DEFINE(ns, reKind, bHRformessage, hr1) \
    EXCEPTION_ADD_HR(hr2) \
    EXCEPTION_ADD_HR(hr3) \
    EXCEPTION_ADD_HR(hr4) \
    EXCEPTION_ADD_HR(hr5) \
    EXCEPTION_END_DEFINE() \

#define DEFINE_EXCEPTION_6HRESULTS(ns, reKind, bHRformessage, hr1, hr2, hr3, hr4, hr5, hr6) \
    EXCEPTION_BEGIN_DEFINE(ns, reKind, bHRformessage, hr1) \
    EXCEPTION_ADD_HR(hr2) \
    EXCEPTION_ADD_HR(hr3) \
    EXCEPTION_ADD_HR(hr4) \
    EXCEPTION_ADD_HR(hr5) \
    EXCEPTION_ADD_HR(hr6) \
    EXCEPTION_END_DEFINE() \

#define DEFINE_EXCEPTION_13HRESULTS(ns, reKind, bHRformessage, hr1, hr2, hr3, hr4, hr5, hr6, hr7, hr8, hr9, hr10,  hr11, hr12, hr13) \
    EXCEPTION_BEGIN_DEFINE(ns, reKind, bHRformessage, hr1) \
    EXCEPTION_ADD_HR(hr2) \
    EXCEPTION_ADD_HR(hr3) \
    EXCEPTION_ADD_HR(hr4) \
    EXCEPTION_ADD_HR(hr5) \
    EXCEPTION_ADD_HR(hr6) \
    EXCEPTION_ADD_HR(hr7) \
    EXCEPTION_ADD_HR(hr8) \
    EXCEPTION_ADD_HR(hr9) \
    EXCEPTION_ADD_HR(hr10) \
    EXCEPTION_ADD_HR(hr11) \
    EXCEPTION_ADD_HR(hr12) \
    EXCEPTION_ADD_HR(hr13) \
    EXCEPTION_END_DEFINE() \

#define DEFINE_EXCEPTION_16HRESULTS(ns, reKind, bHRformessage, hr1, hr2, hr3, hr4, hr5, hr6, hr7, \
                                    hr8, hr9, hr10,  hr11, hr12, hr13, hr14, hr15, hr16) \
    EXCEPTION_BEGIN_DEFINE(ns, reKind, bHRformessage, hr1) \
    EXCEPTION_ADD_HR(hr2) \
    EXCEPTION_ADD_HR(hr3) \
    EXCEPTION_ADD_HR(hr4) \
    EXCEPTION_ADD_HR(hr5) \
    EXCEPTION_ADD_HR(hr6) \
    EXCEPTION_ADD_HR(hr7) \
    EXCEPTION_ADD_HR(hr8) \
    EXCEPTION_ADD_HR(hr9) \
    EXCEPTION_ADD_HR(hr10) \
    EXCEPTION_ADD_HR(hr11) \
    EXCEPTION_ADD_HR(hr12) \
    EXCEPTION_ADD_HR(hr13) \
    EXCEPTION_ADD_HR(hr14) \
    EXCEPTION_ADD_HR(hr15) \
    EXCEPTION_ADD_HR(hr16) \
    EXCEPTION_END_DEFINE() \

#define DEFINE_EXCEPTION_17HRESULTS(ns, reKind, bHRformessage, hr1, hr2, hr3, hr4, hr5, hr6, hr7, \
                                    hr8, hr9, hr10,  hr11, hr12, hr13, hr14, hr15, hr16, hr17) \
    EXCEPTION_BEGIN_DEFINE(ns, reKind, bHRformessage, hr1) \
    EXCEPTION_ADD_HR(hr2) \
    EXCEPTION_ADD_HR(hr3) \
    EXCEPTION_ADD_HR(hr4) \
    EXCEPTION_ADD_HR(hr5) \
    EXCEPTION_ADD_HR(hr6) \
    EXCEPTION_ADD_HR(hr7) \
    EXCEPTION_ADD_HR(hr8) \
    EXCEPTION_ADD_HR(hr9) \
    EXCEPTION_ADD_HR(hr10) \
    EXCEPTION_ADD_HR(hr11) \
    EXCEPTION_ADD_HR(hr12) \
    EXCEPTION_ADD_HR(hr13) \
    EXCEPTION_ADD_HR(hr14) \
    EXCEPTION_ADD_HR(hr15) \
    EXCEPTION_ADD_HR(hr16) \
    EXCEPTION_ADD_HR(hr17) \
    EXCEPTION_END_DEFINE() \

#define DEFINE_EXCEPTION_24HRESULTS(ns, reKind, bHRformessage, hr1, hr2, hr3, hr4, hr5, hr6, hr7, \
                                    hr8, hr9, hr10,  hr11, hr12, hr13, hr14, \
                                    hr15, hr16, hr17, hr18, hr19, hr20,  hr21, hr22, hr23, hr24) \
    EXCEPTION_BEGIN_DEFINE(ns, reKind, bHRformessage, hr1) \
    EXCEPTION_ADD_HR(hr2) \
    EXCEPTION_ADD_HR(hr3) \
    EXCEPTION_ADD_HR(hr4) \
    EXCEPTION_ADD_HR(hr5) \
    EXCEPTION_ADD_HR(hr6) \
    EXCEPTION_ADD_HR(hr7) \
    EXCEPTION_ADD_HR(hr8) \
    EXCEPTION_ADD_HR(hr9) \
    EXCEPTION_ADD_HR(hr10) \
    EXCEPTION_ADD_HR(hr11) \
    EXCEPTION_ADD_HR(hr12) \
    EXCEPTION_ADD_HR(hr13) \
    EXCEPTION_ADD_HR(hr14) \
    EXCEPTION_ADD_HR(hr15) \
    EXCEPTION_ADD_HR(hr16) \
    EXCEPTION_ADD_HR(hr17) \
    EXCEPTION_ADD_HR(hr18) \
    EXCEPTION_ADD_HR(hr19) \
    EXCEPTION_ADD_HR(hr20) \
    EXCEPTION_ADD_HR(hr21) \
    EXCEPTION_ADD_HR(hr22) \
    EXCEPTION_ADD_HR(hr23) \
    EXCEPTION_ADD_HR(hr24) \
    EXCEPTION_END_DEFINE() \

#define DEFINE_EXCEPTION_25HRESULTS(ns, reKind, bHRformessage, hr1, hr2, hr3, hr4, hr5, hr6, hr7, \
                                    hr8, hr9, hr10,  hr11, hr12, hr13, hr14, \
                                    hr15, hr16, hr17, hr18, hr19, hr20,  hr21, hr22, hr23, hr24, hr25) \
    EXCEPTION_BEGIN_DEFINE(ns, reKind, bHRformessage, hr1) \
    EXCEPTION_ADD_HR(hr2) \
    EXCEPTION_ADD_HR(hr3) \
    EXCEPTION_ADD_HR(hr4) \
    EXCEPTION_ADD_HR(hr5) \
    EXCEPTION_ADD_HR(hr6) \
    EXCEPTION_ADD_HR(hr7) \
    EXCEPTION_ADD_HR(hr8) \
    EXCEPTION_ADD_HR(hr9) \
    EXCEPTION_ADD_HR(hr10) \
    EXCEPTION_ADD_HR(hr11) \
    EXCEPTION_ADD_HR(hr12) \
    EXCEPTION_ADD_HR(hr13) \
    EXCEPTION_ADD_HR(hr14) \
    EXCEPTION_ADD_HR(hr15) \
    EXCEPTION_ADD_HR(hr16) \
    EXCEPTION_ADD_HR(hr17) \
    EXCEPTION_ADD_HR(hr18) \
    EXCEPTION_ADD_HR(hr19) \
    EXCEPTION_ADD_HR(hr20) \
    EXCEPTION_ADD_HR(hr21) \
    EXCEPTION_ADD_HR(hr22) \
    EXCEPTION_ADD_HR(hr23) \
    EXCEPTION_ADD_HR(hr24) \
    EXCEPTION_ADD_HR(hr25) \
    EXCEPTION_END_DEFINE() \

//
// Actual definition of the exceptions and their matching HRESULT's.
// HRESULTs are expected to be defined in CorError.h, and must also be
// redefined in managed code in an __HResults class.  The managed exception
// object MUST use the same HRESULT in all of its constructors for COM Interop.
// Read comments near top of this file.
//
//
// NOTE: Please keep this list sorted according to the name of the HRESULT.
//
//

DEFINE_EXCEPTION_SIMPLE(g_ReflectionNS,       AmbiguousMatchException,        false,  COR_E_AMBIGUOUSMATCH)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           ApplicationException,           false,  COR_E_APPLICATION)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           AppDomainUnloadedException,     false,  COR_E_APPDOMAINUNLOADED)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           ArithmeticException,            false,  COR_E_ARITHMETIC)

DEFINE_EXCEPTION_3HRESULTS(g_SystemNS,        ArgumentException,              false, COR_E_ARGUMENT, STD_CTL_SCODE(449), STD_CTL_SCODE(450))

DEFINE_EXCEPTION_2HRESULTS(g_SystemNS,        ArgumentOutOfRangeException,    false,  COR_E_ARGUMENTOUTOFRANGE, HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION))
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           ArrayTypeMismatchException,     false,  COR_E_ARRAYTYPEMISMATCH)

DEFINE_EXCEPTION_13HRESULTS(g_SystemNS,       BadImageFormatException,        true, 
                            COR_E_BADIMAGEFORMAT, CLDB_E_FILE_OLDVER,
                            CLDB_E_FILE_CORRUPT, COR_E_NEWER_RUNTIME,
                            COR_E_ASSEMBLYEXPECTED,
                            HRESULT_FROM_WIN32(ERROR_BAD_EXE_FORMAT),
                            HRESULT_FROM_WIN32(ERROR_EXE_MARKED_INVALID),
                            CORSEC_E_INVALID_IMAGE_FORMAT,
                            HRESULT_FROM_WIN32(ERROR_NOACCESS),
                            HRESULT_FROM_WIN32(ERROR_INVALID_ORDINAL),
                            HRESULT_FROM_WIN32(ERROR_INVALID_DLL),
                            HRESULT_FROM_WIN32(ERROR_FILE_CORRUPT),
                            IDS_CLASSLOAD_32BITCLRLOADING64BITASSEMBLY)

DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           CannotUnloadAppDomainException, false,  COR_E_CANNOTUNLOADAPPDOMAIN)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           ContextMarshalException,        false,  COR_E_CONTEXTMARSHAL)
DEFINE_EXCEPTION_SIMPLE(g_ReflectionNS,       CustomAttributeFormatException, false,  COR_E_CUSTOMATTRIBUTEFORMAT)


DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           DataMisalignedException,        false, COR_E_DATAMISALIGNED)

DEFINE_EXCEPTION_3HRESULTS(g_IONS,            DirectoryNotFoundException,     true, COR_E_DIRECTORYNOTFOUND, STG_E_PATHNOTFOUND, CTL_E_PATHNOTFOUND)

DEFINE_EXCEPTION_2HRESULTS(g_SystemNS,        DivideByZeroException,          false, COR_E_DIVIDEBYZERO, CTL_E_DIVISIONBYZERO)

DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           DllNotFoundException,           false,  COR_E_DLLNOTFOUND)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           DuplicateWaitObjectException,   false,  COR_E_DUPLICATEWAITOBJECT)

DEFINE_EXCEPTION_2HRESULTS(g_IONS,            EndOfStreamException,           false, COR_E_ENDOFSTREAM, STD_CTL_SCODE(62))

DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           EntryPointNotFoundException,    false,  COR_E_ENTRYPOINTNOTFOUND)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           Exception,                      false,  COR_E_EXCEPTION)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           ExecutionEngineException,       false,  COR_E_EXECUTIONENGINE)

DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           FieldAccessException,           false,  COR_E_FIELDACCESS)

DEFINE_EXCEPTION_24HRESULTS(g_IONS,           FileLoadException,              true,
                            COR_E_FILELOAD, FUSION_E_INVALID_PRIVATE_ASM_LOCATION,
                            FUSION_E_SIGNATURE_CHECK_FAILED, 
                            FUSION_E_ASM_MODULE_MISSING, FUSION_E_INVALID_NAME,
                            FUSION_E_PRIVATE_ASM_DISALLOWED, FUSION_E_HOST_GAC_ASM_MISMATCH,
                            COR_E_MODULE_HASH_CHECK_FAILED, FUSION_E_REF_DEF_MISMATCH,
                            SECURITY_E_INCOMPATIBLE_SHARE, SECURITY_E_INCOMPATIBLE_EVIDENCE,
                            SECURITY_E_UNVERIFIABLE, COR_E_FIXUPSINEXE, HRESULT_FROM_WIN32(ERROR_TOO_MANY_OPEN_FILES),
                            HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION), HRESULT_FROM_WIN32(ERROR_LOCK_VIOLATION),
                            HRESULT_FROM_WIN32(ERROR_OPEN_FAILED), HRESULT_FROM_WIN32(ERROR_DISK_CORRUPT),
                            HRESULT_FROM_WIN32(ERROR_UNRECOGNIZED_VOLUME),
                            HRESULT_FROM_WIN32(ERROR_DLL_INIT_FAILED),
                            FUSION_E_CODE_DOWNLOAD_DISABLED, CORSEC_E_MISSING_STRONGNAME,
                            MSEE_E_ASSEMBLYLOADINPROGRESS,
                            HRESULT_FROM_WIN32(ERROR_FILE_INVALID))

DEFINE_EXCEPTION_17HRESULTS(g_IONS,            FileNotFoundException,           true, 
                            HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND), HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND), 
                            HRESULT_FROM_WIN32(ERROR_INVALID_NAME), CTL_E_FILENOTFOUND,
                            HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND), HRESULT_FROM_WIN32(ERROR_BAD_NET_NAME),
                            HRESULT_FROM_WIN32(ERROR_BAD_NETPATH), HRESULT_FROM_WIN32(ERROR_NOT_READY),
                            HRESULT_FROM_WIN32(ERROR_WRONG_TARGET_NAME), INET_E_UNKNOWN_PROTOCOL,
                            INET_E_CONNECTION_TIMEOUT, INET_E_CANNOT_CONNECT,
                            INET_E_RESOURCE_NOT_FOUND, INET_E_OBJECT_NOT_FOUND,
                            INET_E_DOWNLOAD_FAILURE,
                            INET_E_DATA_NOT_AVAILABLE, HRESULT_FROM_WIN32(ERROR_DLL_NOT_FOUND))

DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           FormatException,                false,  COR_E_FORMAT)

DEFINE_EXCEPTION_2HRESULTS(g_SystemNS,        IndexOutOfRangeException,       false, COR_E_INDEXOUTOFRANGE, 0x800a0009 /*Subscript out of range*/)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           InsufficientMemoryException,    false,  COR_E_INSUFFICIENTMEMORY)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           InvalidCastException,           false,  COR_E_INVALIDCAST)
DEFINE_EXCEPTION_SIMPLE(g_ReflectionNS,       InvalidFilterCriteriaException, false,  COR_E_INVALIDFILTERCRITERIA)
DEFINE_EXCEPTION_SIMPLE(g_InteropNS,          InvalidOleVariantTypeException, false,  COR_E_INVALIDOLEVARIANTTYPE)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           InvalidOperationException,      false,  COR_E_INVALIDOPERATION)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           InvalidProgramException,        false,  COR_E_INVALIDPROGRAM)

DEFINE_EXCEPTION_4HRESULTS(g_IONS,            IOException,                    false, COR_E_IO, CTL_E_DEVICEIOERROR, STD_CTL_SCODE(31036), STD_CTL_SCODE(31037))

DEFINE_EXCEPTION_SIMPLE(g_InteropNS,          MarshalDirectiveException,      false,  COR_E_MARSHALDIRECTIVE)
DEFINE_EXCEPTION_2HRESULTS(g_SystemNS,        MethodAccessException,          false,  COR_E_METHODACCESS, META_E_CA_FRIENDS_SN_REQUIRED)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           MemberAccessException,          false,  COR_E_MEMBERACCESS)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           MissingFieldException,          false,  COR_E_MISSINGFIELD)
DEFINE_EXCEPTION_SIMPLE(g_ResourcesNS,        MissingManifestResourceException, false,  COR_E_MISSINGMANIFESTRESOURCE)

DEFINE_EXCEPTION_2HRESULTS(g_SystemNS,        MissingMemberException,         false, COR_E_MISSINGMEMBER, STD_CTL_SCODE(461))

DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           MissingMethodException,         false,  COR_E_MISSINGMETHOD)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           MulticastNotSupportedException, false,  COR_E_MULTICASTNOTSUPPORTED)

DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           NotFiniteNumberException,       false,  COR_E_NOTFINITENUMBER)

DEFINE_EXCEPTION_5HRESULTS(g_SystemNS,        NotSupportedException,          false, COR_E_NOTSUPPORTED, STD_CTL_SCODE(438), STD_CTL_SCODE(445), STD_CTL_SCODE(458), STD_CTL_SCODE(459))

DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           NullReferenceException,         false,  COR_E_NULLREFERENCE)
// Note: this has to come after NullReferenceException since we want NullReferenceException to be created
// when E_POINTER is returned from COM interfaces.
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           AccessViolationException,       false,  E_POINTER)

DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           ObjectDisposedException,        false,  COR_E_OBJECTDISPOSED)

DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           OperationCanceledException,     false,  COR_E_OPERATIONCANCELED)

DEFINE_EXCEPTION_2HRESULTS(g_SystemNS,        OverflowException,              false, COR_E_OVERFLOW, CTL_E_OVERFLOW)

DEFINE_EXCEPTION_SIMPLE(g_IONS,               PathTooLongException,           false,  COR_E_PATHTOOLONG)
 
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           PlatformNotSupportedException,  false,  COR_E_PLATFORMNOTSUPPORTED)

DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           RankException,                  false,  COR_E_RANK)
DEFINE_EXCEPTION_SIMPLE(g_ReflectionNS,       ReflectionTypeLoadException,    false,  COR_E_REFLECTIONTYPELOAD)
DEFINE_EXCEPTION_SIMPLE(g_RemotingNS,         RemotingException,              false,  COR_E_REMOTING)
DEFINE_EXCEPTION_SIMPLE(g_CompilerServicesNS, RuntimeWrappedException,        false,  COR_E_RUNTIMEWRAPPED)

DEFINE_EXCEPTION_SIMPLE(g_RemotingNS,         ServerException,                false,  COR_E_SERVER)

DEFINE_EXCEPTION_6HRESULTS(g_SecurityNS,      SecurityException,              true,
                           COR_E_SECURITY, CORSEC_E_INVALID_STRONGNAME,
                           CTL_E_PERMISSIONDENIED, STD_CTL_SCODE(419),
                           CORSEC_E_INVALID_PUBLICKEY, CORSEC_E_SIGNATURE_MISMATCH)

DEFINE_EXCEPTION_SIMPLE(g_SerializationNS,    SerializationException,         false,  COR_E_SERIALIZATION)

DEFINE_EXCEPTION_2HRESULTS(g_SystemNS,        StackOverflowException,         false, COR_E_STACKOVERFLOW, CTL_E_OUTOFSTACKSPACE)

DEFINE_EXCEPTION_SIMPLE(g_ThreadingNS,        SynchronizationLockException,   false,  COR_E_SYNCHRONIZATIONLOCK)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           SystemException,                false,  COR_E_SYSTEM)

DEFINE_EXCEPTION_SIMPLE(g_ReflectionNS,       TargetException,                false,  COR_E_TARGET)
DEFINE_EXCEPTION_SIMPLE(g_ReflectionNS,       TargetInvocationException,      false,  COR_E_TARGETINVOCATION)
DEFINE_EXCEPTION_SIMPLE(g_ReflectionNS,       TargetParameterCountException,  false,  COR_E_TARGETPARAMCOUNT)
DEFINE_EXCEPTION_SIMPLE(g_ThreadingNS,        ThreadAbortException,           false,  COR_E_THREADABORTED)
DEFINE_EXCEPTION_SIMPLE(g_ThreadingNS,        ThreadInterruptedException,     false,  COR_E_THREADINTERRUPTED)
DEFINE_EXCEPTION_SIMPLE(g_ThreadingNS,        ThreadStateException,           false,  COR_E_THREADSTATE)
DEFINE_EXCEPTION_SIMPLE(g_ThreadingNS,        ThreadStartException,           false,  COR_E_THREADSTART)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           TypeInitializationException,    false,  COR_E_TYPEINITIALIZATION)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           TypeLoadException,              false,  COR_E_TYPELOAD)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           TypeUnloadedException,          false,  COR_E_TYPEUNLOADED)

DEFINE_EXCEPTION_3HRESULTS(g_SystemNS,        UnauthorizedAccessException,    true, COR_E_UNAUTHORIZEDACCESS, CTL_E_PATHFILEACCESSERROR, STD_CTL_SCODE(335))

DEFINE_EXCEPTION_SIMPLE(g_SecurityNS,         VerificationException,          false,  COR_E_VERIFICATION)

DEFINE_EXCEPTION_3HRESULTS(g_PolicyNS,        PolicyException,                true, CORSEC_E_POLICY_EXCEPTION, CORSEC_E_NO_EXEC_PERM, CORSEC_E_MIN_GRANT_FAIL)
DEFINE_EXCEPTION_SIMPLE(g_SecurityNS,         XmlSyntaxException,             false,  CORSEC_E_XMLSYNTAX)

DEFINE_EXCEPTION_SIMPLE(g_InteropNS,          COMException,                   false,  E_FAIL)
DEFINE_EXCEPTION_SIMPLE(g_InteropNS,          ExternalException,              false,  E_FAIL)
DEFINE_EXCEPTION_SIMPLE(g_InteropNS,          SEHException,                   false,  E_FAIL)
DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           NotImplementedException,        false,  E_NOTIMPL)

DEFINE_EXCEPTION_3HRESULTS(g_SystemNS,        OutOfMemoryException,           false, E_OUTOFMEMORY, CTL_E_OUTOFMEMORY, STD_CTL_SCODE(31001))

DEFINE_EXCEPTION_SIMPLE(g_SystemNS,           ArgumentNullException,          false,  E_POINTER)

DEFINE_EXCEPTION_24HRESULTS(g_IsolatedStorageNS,  IsolatedStorageException,   true, ISS_E_ISOSTORE, ISS_E_ISOSTORE, ISS_E_OPEN_STORE_FILE, 
                            ISS_E_OPEN_FILE_MAPPING, ISS_E_MAP_VIEW_OF_FILE, ISS_E_GET_FILE_SIZE, ISS_E_CREATE_MUTEX, ISS_E_LOCK_FAILED,
                            ISS_E_FILE_WRITE, ISS_E_SET_FILE_POINTER, ISS_E_CREATE_DIR,
                            ISS_E_CORRUPTED_STORE_FILE, ISS_E_STORE_VERSION, ISS_E_FILE_NOT_MAPPED, ISS_E_BLOCK_SIZE_TOO_SMALL, 
                            ISS_E_ALLOC_TOO_LARGE, ISS_E_USAGE_WILL_EXCEED_QUOTA, ISS_E_TABLE_ROW_NOT_FOUND, ISS_E_DEPRECATE, ISS_E_CALLER,
                            ISS_E_PATH_LENGTH, ISS_E_MACHINE, ISS_E_STORE_NOT_OPEN, ISS_E_MACHINE_DACL)

// Please see comments on at the top of this list 



