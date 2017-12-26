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


/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
//@@MIDL_FILE_HEADING(  )

#ifdef _MSC_VER
#pragma warning( disable: 4049 )  /* more than 64k source lines */
#endif


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __csiface_h__
#define __csiface_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ICSError_FWD_DEFINED__
#define __ICSError_FWD_DEFINED__
typedef interface ICSError ICSError;
#endif 	/* __ICSError_FWD_DEFINED__ */


#ifndef __ICSErrorContainer_FWD_DEFINED__
#define __ICSErrorContainer_FWD_DEFINED__
typedef interface ICSErrorContainer ICSErrorContainer;
#endif 	/* __ICSErrorContainer_FWD_DEFINED__ */


#ifndef __ICSSourceTextEvents_FWD_DEFINED__
#define __ICSSourceTextEvents_FWD_DEFINED__
typedef interface ICSSourceTextEvents ICSSourceTextEvents;
#endif 	/* __ICSSourceTextEvents_FWD_DEFINED__ */


#ifndef __ICSSourceText_FWD_DEFINED__
#define __ICSSourceText_FWD_DEFINED__
typedef interface ICSSourceText ICSSourceText;
#endif 	/* __ICSSourceText_FWD_DEFINED__ */


#ifndef __ICSCompilerHost_FWD_DEFINED__
#define __ICSCompilerHost_FWD_DEFINED__
typedef interface ICSCompilerHost ICSCompilerHost;
#endif 	/* __ICSCompilerHost_FWD_DEFINED__ */


#ifndef __ICSCommandLineCompilerHost_FWD_DEFINED__
#define __ICSCommandLineCompilerHost_FWD_DEFINED__
typedef interface ICSCommandLineCompilerHost ICSCommandLineCompilerHost;
#endif 	/* __ICSCommandLineCompilerHost_FWD_DEFINED__ */


#ifndef __ICSInteriorTree_FWD_DEFINED__
#define __ICSInteriorTree_FWD_DEFINED__
typedef interface ICSInteriorTree ICSInteriorTree;
#endif 	/* __ICSInteriorTree_FWD_DEFINED__ */


#ifndef __ICSSourceData_FWD_DEFINED__
#define __ICSSourceData_FWD_DEFINED__
typedef interface ICSSourceData ICSSourceData;
#endif 	/* __ICSSourceData_FWD_DEFINED__ */


#ifndef __ICSSourceModuleEvents_FWD_DEFINED__
#define __ICSSourceModuleEvents_FWD_DEFINED__
typedef interface ICSSourceModuleEvents ICSSourceModuleEvents;
#endif 	/* __ICSSourceModuleEvents_FWD_DEFINED__ */


#ifndef __ICSSourceModule_FWD_DEFINED__
#define __ICSSourceModule_FWD_DEFINED__
typedef interface ICSSourceModule ICSSourceModule;
#endif 	/* __ICSSourceModule_FWD_DEFINED__ */


#ifndef __ICSNameTable_FWD_DEFINED__
#define __ICSNameTable_FWD_DEFINED__
typedef interface ICSNameTable ICSNameTable;
#endif 	/* __ICSNameTable_FWD_DEFINED__ */


#ifndef __ICSLexer_FWD_DEFINED__
#define __ICSLexer_FWD_DEFINED__
typedef interface ICSLexer ICSLexer;
#endif 	/* __ICSLexer_FWD_DEFINED__ */


#ifndef __ICSParser_FWD_DEFINED__
#define __ICSParser_FWD_DEFINED__
typedef interface ICSParser ICSParser;
#endif 	/* __ICSParser_FWD_DEFINED__ */


#ifndef __ICSCompilerConfig_FWD_DEFINED__
#define __ICSCompilerConfig_FWD_DEFINED__
typedef interface ICSCompilerConfig ICSCompilerConfig;
#endif 	/* __ICSCompilerConfig_FWD_DEFINED__ */


#ifndef __ICSInputSet_FWD_DEFINED__
#define __ICSInputSet_FWD_DEFINED__
typedef interface ICSInputSet ICSInputSet;
#endif 	/* __ICSInputSet_FWD_DEFINED__ */


#ifndef __ICSCompileProgress_FWD_DEFINED__
#define __ICSCompileProgress_FWD_DEFINED__
typedef interface ICSCompileProgress ICSCompileProgress;
#endif 	/* __ICSCompileProgress_FWD_DEFINED__ */


#ifndef __ICSEncProjectServices_FWD_DEFINED__
#define __ICSEncProjectServices_FWD_DEFINED__
typedef interface ICSEncProjectServices ICSEncProjectServices;
#endif 	/* __ICSEncProjectServices_FWD_DEFINED__ */


#ifndef __ICSCompiler_FWD_DEFINED__
#define __ICSCompiler_FWD_DEFINED__
typedef interface ICSCompiler ICSCompiler;
#endif 	/* __ICSCompiler_FWD_DEFINED__ */


#ifndef __ICSCompilerFactory_FWD_DEFINED__
#define __ICSCompilerFactory_FWD_DEFINED__
typedef interface ICSCompilerFactory ICSCompilerFactory;
#endif 	/* __ICSCompilerFactory_FWD_DEFINED__ */


#ifndef __CSCompilerFactory_FWD_DEFINED__
#define __CSCompilerFactory_FWD_DEFINED__

#ifdef __cplusplus
typedef class CSCompilerFactory CSCompilerFactory;
#else
typedef struct CSCompilerFactory CSCompilerFactory;
#endif /* __cplusplus */

#endif 	/* __CSCompilerFactory_FWD_DEFINED__ */


#ifndef __CSEEFactory_FWD_DEFINED__
#define __CSEEFactory_FWD_DEFINED__

#ifdef __cplusplus
typedef class CSEEFactory CSEEFactory;
#else
typedef struct CSEEFactory CSEEFactory;
#endif /* __cplusplus */

#endif 	/* __CSEEFactory_FWD_DEFINED__ */


#ifndef __CSEECasFactory_FWD_DEFINED__
#define __CSEECasFactory_FWD_DEFINED__

#ifdef __cplusplus
typedef class CSEECasFactory CSEECasFactory;
#else
typedef struct CSEECasFactory CSEECasFactory;
#endif /* __cplusplus */

#endif 	/* __CSEECasFactory_FWD_DEFINED__ */


#ifndef __CSEEUIHost_FWD_DEFINED__
#define __CSEEUIHost_FWD_DEFINED__

#ifdef __cplusplus
typedef class CSEEUIHost CSEEUIHost;
#else
typedef struct CSEEUIHost CSEEUIHost;
#endif /* __cplusplus */

#endif 	/* __CSEEUIHost_FWD_DEFINED__ */


#ifndef __CSRegisteredNameTable_FWD_DEFINED__
#define __CSRegisteredNameTable_FWD_DEFINED__

#ifdef __cplusplus
typedef class CSRegisteredNameTable CSRegisteredNameTable;
#else
typedef struct CSRegisteredNameTable CSRegisteredNameTable;
#endif /* __cplusplus */

#endif 	/* __CSRegisteredNameTable_FWD_DEFINED__ */


#ifndef __CSEELocalObject_FWD_DEFINED__
#define __CSEELocalObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class CSEELocalObject CSEELocalObject;
#else
typedef struct CSEELocalObject CSEELocalObject;
#endif /* __cplusplus */

#endif 	/* __CSEELocalObject_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_csiface_0000 */
/* [local] */ 

#define MIDL_POSDATA POSDATA
struct POSDATA;
#define MIDL_CSTOKEN CSTOKEN
struct CSTOKEN;
#define MIDL_BASENODE BASENODE
struct BASENODE;
#define MIDL_NAMENODE NAMENODE
struct NAMENODE;
#define MIDL_CALLNODE CALLNODE
struct CALLNODE;
#define MIDL_TOKENDATA TOKENDATA
struct TOKENDATA;
#define MIDL_NAME NAME
struct NAME;
#define MIDL_LEXDATA LEXDATA
struct LEXDATA;
#define MIDL_EDITNODE EDITNODE
struct EDITNODE;
#define MIDL_EVENTNODE EVENTNODE
struct EVENTNODE;
#define MIDL_ORGANIZATIONINFO ORGANIZATIONINFO
struct ORGANIZATIONINFO;
#define MIDL_GENERATEMETHODINFO GENERATEMETHODINFO
struct GENERATEMETHODINFO;
#define MIDL_FINDDEFINITIONRESULT FINDDEFINITIONRESULT
struct FINDDEFINITIONRESULT;
#define MIDL_FINDREFERENCESRESULT FINDREFERENCESRESULT
struct FINDREFERENCESRESULT;
#define MIDL_MultiFindReferencesParams MultiFindReferencesParams
struct MultiFindReferencesParams;
typedef 
enum _ERRORKIND
    {	ERROR_FATAL	= 0,
	ERROR_ERROR	= ERROR_FATAL + 1,
	ERROR_WARNING	= ERROR_ERROR + 1
    } 	ERRORKIND;

typedef 
enum _ERRORCATEGORY
    {	EC_TOKENIZATION	= 0,
	EC_TOPLEVELPARSE	= EC_TOKENIZATION + 1,
	EC_METHODPARSE	= EC_TOPLEVELPARSE + 1,
	EC_COMPILATION	= EC_METHODPARSE + 1
    } 	ERRORCATEGORY;

typedef 
enum _CompilerOptions
    {	OPTID_WARNINGLEVEL	= 1,
	OPTID_WARNINGSAREERRORS	= OPTID_WARNINGLEVEL + 1,
	OPTID_CCSYMBOLS	= OPTID_WARNINGSAREERRORS + 1,
	OPTID_NOSTDLIB	= OPTID_CCSYMBOLS + 1,
	OPTID_EMITDEBUGINFO	= OPTID_NOSTDLIB + 1,
	OPTID_OPTIMIZATIONS	= OPTID_EMITDEBUGINFO + 1,
	OPTID_IMPORTS	= OPTID_OPTIMIZATIONS + 1,
	OPTID_INTERNALTESTS	= 8,
	OPTID_NOCODEGEN	= 14,
	OPTID_TIMING	= 15,
	OPTID_INCBUILD	= 17,
	OPTID_MODULES	= 18,
	OPTID_NOWARNLIST	= 20,
	OPTID_XML_DOCFILE	= 24,
	OPTID_CHECKED	= OPTID_XML_DOCFILE + 1,
	OPTID_UNSAFE	= OPTID_CHECKED + 1,
	OPTID_DEBUGTYPE	= OPTID_UNSAFE + 1,
	OPTID_LIBPATH	= OPTID_DEBUGTYPE + 1,
	OPTID_DELAYSIGN	= OPTID_LIBPATH + 1,
	OPTID_KEYFILE	= OPTID_DELAYSIGN + 1,
	OPTID_KEYNAME	= OPTID_KEYFILE + 1,
	OPTID_COMPATIBILITY	= OPTID_KEYNAME + 1,
	OPTID_WARNASERRORLIST	= OPTID_COMPATIBILITY + 1,
	OPTID_WATSONMODE	= OPTID_WARNASERRORLIST + 1,
	OPTID_PDBALTPATH	= OPTID_WATSONMODE + 1,
	OPTID_SOURCEMAP	= OPTID_PDBALTPATH + 1,
	OPTID_PLATFORM	= OPTID_SOURCEMAP + 1,
	OPTID_MODULEASSEMBLY	= OPTID_PLATFORM + 1,
	OPTID_CompileSkeleton	= OPTID_MODULEASSEMBLY + 1,
	LARGEST_OPTION_ID	= OPTID_CompileSkeleton + 1
    } 	CompilerOptions;

typedef 
enum _OutputFileTypes
    {	OUTPUT_CONSOLE	= 0,
	OUTPUT_WINDOWS	= OUTPUT_CONSOLE + 1,
	OUTPUT_LIBRARY	= OUTPUT_WINDOWS + 1,
	OUTPUT_MODULE	= OUTPUT_LIBRARY + 1
    } 	OutputFileTypes;

typedef 
enum _PlatformType
    {	platformAgnostic	= 0,
	platformX86	= platformAgnostic + 1,
	platformIA64	= platformX86 + 1,
	platformAMD64	= platformIA64 + 1,
	platformLast	= platformAMD64 + 1
    } 	PlatformType;

typedef 
enum _CompilerOptionFlags
    {	COF_BOOLEAN	= 0x1,
	COF_HIDDEN	= 0x2,
	COF_DEFAULTON	= 0x4,
	COF_HASDEFAULT	= 0x4,
	COF_WARNONOLDUSE	= 0x8,
	COF_GRP_OUTPUT	= 0x1000,
	COF_GRP_INPUT	= 0x2000,
	COF_GRP_RES	= 0x3000,
	COF_GRP_CODE	= 0x4000,
	COF_GRP_ERRORS	= 0x5000,
	COF_GRP_LANGUAGE	= 0x6000,
	COF_GRP_MISC	= 0x7000,
	COF_GRP_ADVANCED	= 0x8000,
	COF_GRP_MASK	= 0xff000,
	COF_ARG_NONE	= 0,
	COF_ARG_FILELIST	= 0x100000,
	COF_ARG_FILE	= 0x200000,
	COF_ARG_SYMLIST	= 0x300000,
	COF_ARG_WILDCARD	= 0x400000,
	COF_ARG_TYPE	= 0x500000,
	COF_ARG_RESINFO	= 0x600000,
	COF_ARG_WARNLIST	= 0x700000,
	COF_ARG_ADDR	= 0x800000,
	COF_ARG_NUMBER	= 0x900000,
	COF_ARG_DEBUGTYPE	= 0xa00000,
	COF_ARG_STRING	= 0xb00000,
	COF_ARG_ALIAS	= 0xc00000,
	COF_ARG_NOCOLON	= 0x8000000,
	COF_ARG_BOOLSTRING	= 0x10000000,
	COF_ARG_MASK	= 0x7f00000
    } 	CompilerOptionFlags;

typedef 
enum _CompilerCreationFlags
    {	CCF_KEEPIDENTTABLES	= 0x1,
	CCF_KEEPNODETABLES	= 0x2,
	CCF_TRACKCOMMENTS	= 0x4,
	CCF_IDEUSAGE	= CCF_KEEPIDENTTABLES | CCF_KEEPNODETABLES | CCF_TRACKCOMMENTS
    } 	CompilerCreationFlags;

typedef 
enum _ParseTreeScope
    {	PTS_NAMESPACEBODY	= 0,
	PTS_TYPEBODY	= PTS_NAMESPACEBODY + 1,
	PTS_ENUMBODY	= PTS_TYPEBODY + 1,
	PTS_STATEMENT	= PTS_ENUMBODY + 1,
	PTS_EXPRESION	= PTS_STATEMENT + 1,
	PTS_TYPE	= PTS_EXPRESION + 1,
	PTS_MEMBER	= PTS_TYPE + 1,
	PTS_PARAMETER	= PTS_MEMBER + 1,
	PTS_MEMBER_REF_SPECIAL	= PTS_PARAMETER + 1
    } 	ParseTreeScope;

typedef 
enum _ExtentFlags
    {	EF_FULL	= 0,
	EF_SINGLESTMT	= EF_FULL + 1,
	EF_POSSIBLE_GENERIC_NAME	= EF_SINGLESTMT + 1,
	EF_PREFER_LEFT_NODE	= 0x4,
	EF_IGNORE_TOKEN_STREAM	= 0x80
    } 	ExtentFlags;

typedef 
enum _CompatibilityMode
    {	CompatibilityECMA1	= 0,
	CompatibilityNone	= 1
    } 	CompatibilityMode;




















typedef struct _Checksum
    {
    GUID guidID;
    DWORD cbData;
    /* [size_is] */ void *pvData;
    } 	Checksum;

typedef struct _KEYEDNODE
    {
    MIDL_NAME *pKey;
    MIDL_BASENODE *pNode;
    } 	KEYEDNODE;

typedef 
enum _LocalVarModifier
    {	LVM_IN	= 0,
	LVM_REF	= LVM_IN + 1,
	LVM_OUT	= LVM_REF + 1,
	LVM_PARAMS	= LVM_OUT + 1
    } 	LocalVarModifier;

typedef 
enum _LocalVarAssignedState
    {	LVAS_UNASSIGNED	= 0,
	LVAS_PARTIAL	= LVAS_UNASSIGNED + 1,
	LVAS_ASSIGNED	= LVAS_PARTIAL + 1
    } 	LocalVarAssignedState;

typedef struct _PARAMETERINFO
    {
    MIDL_NAME *pName;
    MIDL_NAME *pTypeName;
    LocalVarModifier lvModifier;
    } 	PARAMETERINFO;

typedef struct _LOCALVARINFO
    {
    MIDL_NAME *pName;
    MIDL_NAME *pTypeName;
    BOOL fIsDeclaredInsideSelection;
    MIDL_BASENODE *pDeclNode;
    BOOL fIsModifiedInsideSelection;
    BOOL fIsReferencedOutsideDeclarationOutsideSelection;
    BOOL fIsReferencedOutsideDeclarationInsideSelection;
    BOOL fIsLastReferenceInsideSelection;
    LocalVarModifier lvmModifier;
    BOOL fIsAliasedByAddrExpr;
    BOOL fIsHoistedForAnonymousMethod;
    LocalVarAssignedState lvasStateBeforeSelection;
    LocalVarAssignedState lvasStateAfterSelection;
    } 	LOCALVARINFO;

typedef struct _EXTRACTMETHODINFO
    {
    BOOL fContainsReferencesToThis;
    BOOL fContainsReturnStatement;
    BOOL fLastStatementExitReachable;
    ULONG iLocalsCount;
    LOCALVARINFO *pLocals;
    ULONG iMethTypeVarCount;
    MIDL_NAME **ppMethTypeVarNames;
    } 	EXTRACTMETHODINFO;

typedef 
enum _FIND_REFERENCES_OPTIONS
    {	RO_REFERENCES	= 0x1,
	RO_EXTERNAL	= 0x2,
	RO_OVERLOADS	= 0x4,
	RO_ENCFIELD	= 0x8,
	RO_ADDCTORDTORDEFS	= 0x10,
	RO_REFERENCESTOTYPES	= 0x20,
	RO_END	= RO_REFERENCESTOTYPES + 1
    } 	FindReferencesOptions;



extern RPC_IF_HANDLE __MIDL_itf_csiface_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_csiface_0000_v0_0_s_ifspec;


#ifndef __CSharp_LIBRARY_DEFINED__
#define __CSharp_LIBRARY_DEFINED__

/* library CSharp */
/* [helpstringdll][helpstring][version][uuid] */ 

typedef 
enum ReferenceSelectionEnum
    {	NoReferences	= 0,
	ExternalReferences	= NoReferences + 1,
	AllReferences	= ExternalReferences + 1
    } 	_ReferenceSelectionEnum;

typedef 
enum PropertyTypeEnum
    {	ReadOnly	= 0,
	WriteOnly	= ReadOnly + 1,
	ReadWrite	= WriteOnly + 1
    } 	_PropertyTypeEnum;

typedef struct _ENC_NODE_STATE
    {
    BOOL fModified;
    BOOL fMoved;
    long iDeltaLinesMoved;
    BOOL fContainsActiveStatement;
    } 	ENC_NODE_STATE;


EXTERN_C const IID LIBID_CSharp;

#ifndef __ICSError_INTERFACE_DEFINED__
#define __ICSError_INTERFACE_DEFINED__

/* interface ICSError */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSError;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4CEEE5D4-FBB0-42ac-B558-5D764C8240C7")
    ICSError : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetErrorInfo( 
            long *piErrorID,
            ERRORKIND *pKind,
            LPCWSTR *ppszText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLocationCount( 
            long *piCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLocationAt( 
            long iIndex,
            LPCWSTR *ppszFileName,
            MIDL_POSDATA *pposStart,
            MIDL_POSDATA *pposEnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUnmappedLocationAt( 
            long iIndex,
            LPCWSTR *ppszFileName,
            MIDL_POSDATA *pposStart,
            MIDL_POSDATA *pposEnd) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSErrorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSError * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSError * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSError * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetErrorInfo )( 
            ICSError * This,
            long *piErrorID,
            ERRORKIND *pKind,
            LPCWSTR *ppszText);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocationCount )( 
            ICSError * This,
            long *piCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocationAt )( 
            ICSError * This,
            long iIndex,
            LPCWSTR *ppszFileName,
            MIDL_POSDATA *pposStart,
            MIDL_POSDATA *pposEnd);
        
        HRESULT ( STDMETHODCALLTYPE *GetUnmappedLocationAt )( 
            ICSError * This,
            long iIndex,
            LPCWSTR *ppszFileName,
            MIDL_POSDATA *pposStart,
            MIDL_POSDATA *pposEnd);
        
        END_INTERFACE
    } ICSErrorVtbl;

    interface ICSError
    {
        CONST_VTBL struct ICSErrorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSError_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSError_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSError_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSError_GetErrorInfo(This,piErrorID,pKind,ppszText)	\
    (This)->lpVtbl -> GetErrorInfo(This,piErrorID,pKind,ppszText)

#define ICSError_GetLocationCount(This,piCount)	\
    (This)->lpVtbl -> GetLocationCount(This,piCount)

#define ICSError_GetLocationAt(This,iIndex,ppszFileName,pposStart,pposEnd)	\
    (This)->lpVtbl -> GetLocationAt(This,iIndex,ppszFileName,pposStart,pposEnd)

#define ICSError_GetUnmappedLocationAt(This,iIndex,ppszFileName,pposStart,pposEnd)	\
    (This)->lpVtbl -> GetUnmappedLocationAt(This,iIndex,ppszFileName,pposStart,pposEnd)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSError_GetErrorInfo_Proxy( 
    ICSError * This,
    long *piErrorID,
    ERRORKIND *pKind,
    LPCWSTR *ppszText);


void __RPC_STUB ICSError_GetErrorInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSError_GetLocationCount_Proxy( 
    ICSError * This,
    long *piCount);


void __RPC_STUB ICSError_GetLocationCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSError_GetLocationAt_Proxy( 
    ICSError * This,
    long iIndex,
    LPCWSTR *ppszFileName,
    MIDL_POSDATA *pposStart,
    MIDL_POSDATA *pposEnd);


void __RPC_STUB ICSError_GetLocationAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSError_GetUnmappedLocationAt_Proxy( 
    ICSError * This,
    long iIndex,
    LPCWSTR *ppszFileName,
    MIDL_POSDATA *pposStart,
    MIDL_POSDATA *pposEnd);


void __RPC_STUB ICSError_GetUnmappedLocationAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSError_INTERFACE_DEFINED__ */


#ifndef __ICSErrorContainer_INTERFACE_DEFINED__
#define __ICSErrorContainer_INTERFACE_DEFINED__

/* interface ICSErrorContainer */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSErrorContainer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("376A7828-DB0D-4cfd-956E-5143F71F5CCD")
    ICSErrorContainer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetContainerInfo( 
            ERRORCATEGORY *pCategory,
            DWORD_PTR *pdwID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetErrorCount( 
            long *piWarnings,
            long *piErrors,
            long *piFatals,
            long *piTotal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetErrorAt( 
            long iIndex,
            ICSError **ppError) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWarnAsErrorCount( 
            long *piWarnAsErrors) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSErrorContainerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSErrorContainer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSErrorContainer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSErrorContainer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainerInfo )( 
            ICSErrorContainer * This,
            ERRORCATEGORY *pCategory,
            DWORD_PTR *pdwID);
        
        HRESULT ( STDMETHODCALLTYPE *GetErrorCount )( 
            ICSErrorContainer * This,
            long *piWarnings,
            long *piErrors,
            long *piFatals,
            long *piTotal);
        
        HRESULT ( STDMETHODCALLTYPE *GetErrorAt )( 
            ICSErrorContainer * This,
            long iIndex,
            ICSError **ppError);
        
        HRESULT ( STDMETHODCALLTYPE *GetWarnAsErrorCount )( 
            ICSErrorContainer * This,
            long *piWarnAsErrors);
        
        END_INTERFACE
    } ICSErrorContainerVtbl;

    interface ICSErrorContainer
    {
        CONST_VTBL struct ICSErrorContainerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSErrorContainer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSErrorContainer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSErrorContainer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSErrorContainer_GetContainerInfo(This,pCategory,pdwID)	\
    (This)->lpVtbl -> GetContainerInfo(This,pCategory,pdwID)

#define ICSErrorContainer_GetErrorCount(This,piWarnings,piErrors,piFatals,piTotal)	\
    (This)->lpVtbl -> GetErrorCount(This,piWarnings,piErrors,piFatals,piTotal)

#define ICSErrorContainer_GetErrorAt(This,iIndex,ppError)	\
    (This)->lpVtbl -> GetErrorAt(This,iIndex,ppError)

#define ICSErrorContainer_GetWarnAsErrorCount(This,piWarnAsErrors)	\
    (This)->lpVtbl -> GetWarnAsErrorCount(This,piWarnAsErrors)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSErrorContainer_GetContainerInfo_Proxy( 
    ICSErrorContainer * This,
    ERRORCATEGORY *pCategory,
    DWORD_PTR *pdwID);


void __RPC_STUB ICSErrorContainer_GetContainerInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSErrorContainer_GetErrorCount_Proxy( 
    ICSErrorContainer * This,
    long *piWarnings,
    long *piErrors,
    long *piFatals,
    long *piTotal);


void __RPC_STUB ICSErrorContainer_GetErrorCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSErrorContainer_GetErrorAt_Proxy( 
    ICSErrorContainer * This,
    long iIndex,
    ICSError **ppError);


void __RPC_STUB ICSErrorContainer_GetErrorAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSErrorContainer_GetWarnAsErrorCount_Proxy( 
    ICSErrorContainer * This,
    long *piWarnAsErrors);


void __RPC_STUB ICSErrorContainer_GetWarnAsErrorCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSErrorContainer_INTERFACE_DEFINED__ */


#ifndef __ICSSourceTextEvents_INTERFACE_DEFINED__
#define __ICSSourceTextEvents_INTERFACE_DEFINED__

/* interface ICSSourceTextEvents */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSSourceTextEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ED8A54FC-E2EA-11d2-B555-00C04F68D4DB")
    ICSSourceTextEvents : public IUnknown
    {
    public:
        virtual void STDMETHODCALLTYPE OnEdit( 
            MIDL_POSDATA posStart,
            MIDL_POSDATA posOldEnd,
            MIDL_POSDATA posNewEnd) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSSourceTextEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSSourceTextEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSSourceTextEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSSourceTextEvents * This);
        
        void ( STDMETHODCALLTYPE *OnEdit )( 
            ICSSourceTextEvents * This,
            MIDL_POSDATA posStart,
            MIDL_POSDATA posOldEnd,
            MIDL_POSDATA posNewEnd);
        
        END_INTERFACE
    } ICSSourceTextEventsVtbl;

    interface ICSSourceTextEvents
    {
        CONST_VTBL struct ICSSourceTextEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSSourceTextEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSSourceTextEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSSourceTextEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSSourceTextEvents_OnEdit(This,posStart,posOldEnd,posNewEnd)	\
    (This)->lpVtbl -> OnEdit(This,posStart,posOldEnd,posNewEnd)

#endif /* COBJMACROS */


#endif 	/* C style interface */



void STDMETHODCALLTYPE ICSSourceTextEvents_OnEdit_Proxy( 
    ICSSourceTextEvents * This,
    MIDL_POSDATA posStart,
    MIDL_POSDATA posOldEnd,
    MIDL_POSDATA posNewEnd);


void __RPC_STUB ICSSourceTextEvents_OnEdit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSSourceTextEvents_INTERFACE_DEFINED__ */


#ifndef __ICSSourceText_INTERFACE_DEFINED__
#define __ICSSourceText_INTERFACE_DEFINED__

/* interface ICSSourceText */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSSourceText;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E3783F08-E098-11d2-B554-00C04F68D4DB")
    ICSSourceText : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CloneInMemory( 
            ICSSourceText **ppClone) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetText( 
            LPCWSTR *ppszText,
            MIDL_POSDATA *pposEnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            LPCWSTR *ppszText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLineCount( 
            long *piLines) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReleaseText( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AdviseChangeEvents( 
            ICSSourceTextEvents *pSink,
            DWORD_PTR *pdwCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnadviseChangeEvents( 
            DWORD_PTR dwCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GenerateChecksum( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetChecksum( 
            /* [out] */ Checksum *checksum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReportExceptionalOperation( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSSourceTextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSSourceText * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSSourceText * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSSourceText * This);
        
        HRESULT ( STDMETHODCALLTYPE *CloneInMemory )( 
            ICSSourceText * This,
            ICSSourceText **ppClone);
        
        HRESULT ( STDMETHODCALLTYPE *GetText )( 
            ICSSourceText * This,
            LPCWSTR *ppszText,
            MIDL_POSDATA *pposEnd);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            ICSSourceText * This,
            LPCWSTR *ppszText);
        
        HRESULT ( STDMETHODCALLTYPE *GetLineCount )( 
            ICSSourceText * This,
            long *piLines);
        
        HRESULT ( STDMETHODCALLTYPE *ReleaseText )( 
            ICSSourceText * This);
        
        HRESULT ( STDMETHODCALLTYPE *AdviseChangeEvents )( 
            ICSSourceText * This,
            ICSSourceTextEvents *pSink,
            DWORD_PTR *pdwCookie);
        
        HRESULT ( STDMETHODCALLTYPE *UnadviseChangeEvents )( 
            ICSSourceText * This,
            DWORD_PTR dwCookie);
        
        HRESULT ( STDMETHODCALLTYPE *GenerateChecksum )( 
            ICSSourceText * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetChecksum )( 
            ICSSourceText * This,
            /* [out] */ Checksum *checksum);
        
        HRESULT ( STDMETHODCALLTYPE *ReportExceptionalOperation )( 
            ICSSourceText * This);
        
        END_INTERFACE
    } ICSSourceTextVtbl;

    interface ICSSourceText
    {
        CONST_VTBL struct ICSSourceTextVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSSourceText_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSSourceText_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSSourceText_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSSourceText_CloneInMemory(This,ppClone)	\
    (This)->lpVtbl -> CloneInMemory(This,ppClone)

#define ICSSourceText_GetText(This,ppszText,pposEnd)	\
    (This)->lpVtbl -> GetText(This,ppszText,pposEnd)

#define ICSSourceText_GetName(This,ppszText)	\
    (This)->lpVtbl -> GetName(This,ppszText)

#define ICSSourceText_GetLineCount(This,piLines)	\
    (This)->lpVtbl -> GetLineCount(This,piLines)

#define ICSSourceText_ReleaseText(This)	\
    (This)->lpVtbl -> ReleaseText(This)

#define ICSSourceText_AdviseChangeEvents(This,pSink,pdwCookie)	\
    (This)->lpVtbl -> AdviseChangeEvents(This,pSink,pdwCookie)

#define ICSSourceText_UnadviseChangeEvents(This,dwCookie)	\
    (This)->lpVtbl -> UnadviseChangeEvents(This,dwCookie)

#define ICSSourceText_GenerateChecksum(This)	\
    (This)->lpVtbl -> GenerateChecksum(This)

#define ICSSourceText_GetChecksum(This,checksum)	\
    (This)->lpVtbl -> GetChecksum(This,checksum)

#define ICSSourceText_ReportExceptionalOperation(This)	\
    (This)->lpVtbl -> ReportExceptionalOperation(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSSourceText_CloneInMemory_Proxy( 
    ICSSourceText * This,
    ICSSourceText **ppClone);


void __RPC_STUB ICSSourceText_CloneInMemory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceText_GetText_Proxy( 
    ICSSourceText * This,
    LPCWSTR *ppszText,
    MIDL_POSDATA *pposEnd);


void __RPC_STUB ICSSourceText_GetText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceText_GetName_Proxy( 
    ICSSourceText * This,
    LPCWSTR *ppszText);


void __RPC_STUB ICSSourceText_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceText_GetLineCount_Proxy( 
    ICSSourceText * This,
    long *piLines);


void __RPC_STUB ICSSourceText_GetLineCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceText_ReleaseText_Proxy( 
    ICSSourceText * This);


void __RPC_STUB ICSSourceText_ReleaseText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceText_AdviseChangeEvents_Proxy( 
    ICSSourceText * This,
    ICSSourceTextEvents *pSink,
    DWORD_PTR *pdwCookie);


void __RPC_STUB ICSSourceText_AdviseChangeEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceText_UnadviseChangeEvents_Proxy( 
    ICSSourceText * This,
    DWORD_PTR dwCookie);


void __RPC_STUB ICSSourceText_UnadviseChangeEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceText_GenerateChecksum_Proxy( 
    ICSSourceText * This);


void __RPC_STUB ICSSourceText_GenerateChecksum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceText_GetChecksum_Proxy( 
    ICSSourceText * This,
    /* [out] */ Checksum *checksum);


void __RPC_STUB ICSSourceText_GetChecksum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceText_ReportExceptionalOperation_Proxy( 
    ICSSourceText * This);


void __RPC_STUB ICSSourceText_ReportExceptionalOperation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSSourceText_INTERFACE_DEFINED__ */


#ifndef __ICSCompilerHost_INTERFACE_DEFINED__
#define __ICSCompilerHost_INTERFACE_DEFINED__

/* interface ICSCompilerHost */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSCompilerHost;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D2686AF8-E098-11d2-B554-00C04F68D4DB")
    ICSCompilerHost : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ReportErrors( 
            ICSErrorContainer *pErrors) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSourceModule( 
            LPCWSTR pszFileName,
            BOOL fNeedChecksum,
            ICSSourceModule **ppModule) = 0;
        
        virtual void STDMETHODCALLTYPE OnConfigChange( 
            long iOptionID,
            VARIANT varNewValue) = 0;
        
        virtual void STDMETHODCALLTYPE OnCatastrophicError( 
            BOOL fException,
            DWORD dwException,
            void *pAddress) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSCompilerHostVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSCompilerHost * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSCompilerHost * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSCompilerHost * This);
        
        HRESULT ( STDMETHODCALLTYPE *ReportErrors )( 
            ICSCompilerHost * This,
            ICSErrorContainer *pErrors);
        
        HRESULT ( STDMETHODCALLTYPE *GetSourceModule )( 
            ICSCompilerHost * This,
            LPCWSTR pszFileName,
            BOOL fNeedChecksum,
            ICSSourceModule **ppModule);
        
        void ( STDMETHODCALLTYPE *OnConfigChange )( 
            ICSCompilerHost * This,
            long iOptionID,
            VARIANT varNewValue);
        
        void ( STDMETHODCALLTYPE *OnCatastrophicError )( 
            ICSCompilerHost * This,
            BOOL fException,
            DWORD dwException,
            void *pAddress);
        
        END_INTERFACE
    } ICSCompilerHostVtbl;

    interface ICSCompilerHost
    {
        CONST_VTBL struct ICSCompilerHostVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSCompilerHost_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSCompilerHost_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSCompilerHost_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSCompilerHost_ReportErrors(This,pErrors)	\
    (This)->lpVtbl -> ReportErrors(This,pErrors)

#define ICSCompilerHost_GetSourceModule(This,pszFileName,fNeedChecksum,ppModule)	\
    (This)->lpVtbl -> GetSourceModule(This,pszFileName,fNeedChecksum,ppModule)

#define ICSCompilerHost_OnConfigChange(This,iOptionID,varNewValue)	\
    (This)->lpVtbl -> OnConfigChange(This,iOptionID,varNewValue)

#define ICSCompilerHost_OnCatastrophicError(This,fException,dwException,pAddress)	\
    (This)->lpVtbl -> OnCatastrophicError(This,fException,dwException,pAddress)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSCompilerHost_ReportErrors_Proxy( 
    ICSCompilerHost * This,
    ICSErrorContainer *pErrors);


void __RPC_STUB ICSCompilerHost_ReportErrors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompilerHost_GetSourceModule_Proxy( 
    ICSCompilerHost * This,
    LPCWSTR pszFileName,
    BOOL fNeedChecksum,
    ICSSourceModule **ppModule);


void __RPC_STUB ICSCompilerHost_GetSourceModule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE ICSCompilerHost_OnConfigChange_Proxy( 
    ICSCompilerHost * This,
    long iOptionID,
    VARIANT varNewValue);


void __RPC_STUB ICSCompilerHost_OnConfigChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE ICSCompilerHost_OnCatastrophicError_Proxy( 
    ICSCompilerHost * This,
    BOOL fException,
    DWORD dwException,
    void *pAddress);


void __RPC_STUB ICSCompilerHost_OnCatastrophicError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSCompilerHost_INTERFACE_DEFINED__ */


#ifndef __ICSCommandLineCompilerHost_INTERFACE_DEFINED__
#define __ICSCommandLineCompilerHost_INTERFACE_DEFINED__

/* interface ICSCommandLineCompilerHost */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSCommandLineCompilerHost;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BD7CF4EF-1821-4719-AA60-DF81B9EAFA01")
    ICSCommandLineCompilerHost : public IUnknown
    {
    public:
        virtual void STDMETHODCALLTYPE NotifyBinaryFile( 
            LPCWSTR pszFileName) = 0;
        
        virtual void STDMETHODCALLTYPE NotifyMetadataFile( 
            LPCWSTR pszFileName,
            LPCWSTR pszCorSystemDirectory) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBugReportFileName( 
            LPWSTR pszFileName,
            DWORD cchLength) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSCommandLineCompilerHostVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSCommandLineCompilerHost * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSCommandLineCompilerHost * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSCommandLineCompilerHost * This);
        
        void ( STDMETHODCALLTYPE *NotifyBinaryFile )( 
            ICSCommandLineCompilerHost * This,
            LPCWSTR pszFileName);
        
        void ( STDMETHODCALLTYPE *NotifyMetadataFile )( 
            ICSCommandLineCompilerHost * This,
            LPCWSTR pszFileName,
            LPCWSTR pszCorSystemDirectory);
        
        HRESULT ( STDMETHODCALLTYPE *GetBugReportFileName )( 
            ICSCommandLineCompilerHost * This,
            LPWSTR pszFileName,
            DWORD cchLength);
        
        END_INTERFACE
    } ICSCommandLineCompilerHostVtbl;

    interface ICSCommandLineCompilerHost
    {
        CONST_VTBL struct ICSCommandLineCompilerHostVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSCommandLineCompilerHost_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSCommandLineCompilerHost_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSCommandLineCompilerHost_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSCommandLineCompilerHost_NotifyBinaryFile(This,pszFileName)	\
    (This)->lpVtbl -> NotifyBinaryFile(This,pszFileName)

#define ICSCommandLineCompilerHost_NotifyMetadataFile(This,pszFileName,pszCorSystemDirectory)	\
    (This)->lpVtbl -> NotifyMetadataFile(This,pszFileName,pszCorSystemDirectory)

#define ICSCommandLineCompilerHost_GetBugReportFileName(This,pszFileName,cchLength)	\
    (This)->lpVtbl -> GetBugReportFileName(This,pszFileName,cchLength)

#endif /* COBJMACROS */


#endif 	/* C style interface */



void STDMETHODCALLTYPE ICSCommandLineCompilerHost_NotifyBinaryFile_Proxy( 
    ICSCommandLineCompilerHost * This,
    LPCWSTR pszFileName);


void __RPC_STUB ICSCommandLineCompilerHost_NotifyBinaryFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE ICSCommandLineCompilerHost_NotifyMetadataFile_Proxy( 
    ICSCommandLineCompilerHost * This,
    LPCWSTR pszFileName,
    LPCWSTR pszCorSystemDirectory);


void __RPC_STUB ICSCommandLineCompilerHost_NotifyMetadataFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCommandLineCompilerHost_GetBugReportFileName_Proxy( 
    ICSCommandLineCompilerHost * This,
    LPWSTR pszFileName,
    DWORD cchLength);


void __RPC_STUB ICSCommandLineCompilerHost_GetBugReportFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSCommandLineCompilerHost_INTERFACE_DEFINED__ */


#ifndef __ICSInteriorTree_INTERFACE_DEFINED__
#define __ICSInteriorTree_INTERFACE_DEFINED__

/* interface ICSInteriorTree */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSInteriorTree;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4D5D4C20-EE19-11d2-B556-00C04F68D4DB")
    ICSInteriorTree : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetTree( 
            MIDL_BASENODE **ppNode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetErrors( 
            ICSErrorContainer **ppErrors) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSInteriorTreeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSInteriorTree * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSInteriorTree * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSInteriorTree * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTree )( 
            ICSInteriorTree * This,
            MIDL_BASENODE **ppNode);
        
        HRESULT ( STDMETHODCALLTYPE *GetErrors )( 
            ICSInteriorTree * This,
            ICSErrorContainer **ppErrors);
        
        END_INTERFACE
    } ICSInteriorTreeVtbl;

    interface ICSInteriorTree
    {
        CONST_VTBL struct ICSInteriorTreeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSInteriorTree_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSInteriorTree_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSInteriorTree_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSInteriorTree_GetTree(This,ppNode)	\
    (This)->lpVtbl -> GetTree(This,ppNode)

#define ICSInteriorTree_GetErrors(This,ppErrors)	\
    (This)->lpVtbl -> GetErrors(This,ppErrors)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSInteriorTree_GetTree_Proxy( 
    ICSInteriorTree * This,
    MIDL_BASENODE **ppNode);


void __RPC_STUB ICSInteriorTree_GetTree_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInteriorTree_GetErrors_Proxy( 
    ICSInteriorTree * This,
    ICSErrorContainer **ppErrors);


void __RPC_STUB ICSInteriorTree_GetErrors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSInteriorTree_INTERFACE_DEFINED__ */


#ifndef __ICSSourceData_INTERFACE_DEFINED__
#define __ICSSourceData_INTERFACE_DEFINED__

/* interface ICSSourceData */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSSourceData;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0C12CCE0-E21B-11d2-B555-00C04F68D4DB")
    ICSSourceData : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetSourceModule( 
            ICSSourceModule **ppModule) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLexResults( 
            MIDL_LEXDATA *pLexData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSingleTokenPos( 
            long iToken,
            MIDL_POSDATA *pposToken) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSingleTokenData( 
            long iToken,
            MIDL_TOKENDATA *pTokenData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTokenText( 
            long iTokenId,
            LPCWSTR *ppszText,
            long *piLen) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsInsideComment( 
            const MIDL_POSDATA pos,
            BOOL *pfInComment) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ParseTopLevel( 
            MIDL_BASENODE **ppTree) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ParseTopLevel2( 
            MIDL_BASENODE **ppTree,
            BOOL fCreateParseDiffs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetErrors( 
            ERRORCATEGORY iCategory,
            ICSErrorContainer **ppErrors) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInteriorParseTree( 
            MIDL_BASENODE *pNode,
            ICSInteriorTree **ppTree) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LookupNode( 
            MIDL_NAME *pKey,
            long iOrdinal,
            MIDL_BASENODE **ppNode,
            long *piGlobalOrdinal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNodeKeyOrdinal( 
            MIDL_BASENODE *pNode,
            MIDL_NAME **ppKey,
            long *piKeyOrdinal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetGlobalKeyArray( 
            KEYEDNODE *pKeyedNodes,
            long iSize,
            long *piCopied) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ParseForErrors( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FindLeafNode( 
            const MIDL_POSDATA pos,
            MIDL_BASENODE **ppNode,
            ICSInteriorTree **ppTree) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FindLeafNodeEx( 
            const MIDL_POSDATA pos,
            ExtentFlags flags,
            MIDL_BASENODE **ppNode,
            ICSInteriorTree **ppTree) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FindLeafNodeForToken( 
            long iToken,
            MIDL_BASENODE **ppNode,
            ICSInteriorTree **ppTree) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FindLeafNodeForTokenEx( 
            long iToken,
            ExtentFlags flags,
            MIDL_BASENODE **ppNode,
            ICSInteriorTree **ppTree) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetExtent( 
            MIDL_BASENODE *pNode,
            MIDL_POSDATA *pposStart,
            MIDL_POSDATA *pposEnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetExtentEx( 
            MIDL_BASENODE *pNode,
            MIDL_POSDATA *pposStart,
            MIDL_POSDATA *pposEnd,
            ExtentFlags flags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTokenExtent( 
            MIDL_BASENODE *pNode,
            long *piFirstToken,
            long *piLastToken) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDocComment( 
            MIDL_BASENODE *pNode,
            long *piIndex,
            long *piCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDocCommentXML( 
            MIDL_BASENODE *pNode,
            BSTR *pbstrDoc) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsUpToDate( 
            BOOL *pfTokenized,
            BOOL *pfTopParsed) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MapSourceLine( 
            long iline,
            long *piMappedLine,
            LPCWSTR *ppszFilename,
            BOOL *pbIsHidden) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLastRenamedTokenIndex( 
            long *piTokIdx,
            MIDL_NAME **ppPreviousName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ResetRenamedTokenData( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CloneToNonLocking( 
            ICSSourceData **ppData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnRename( 
            MIDL_NAME *pNewName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSSourceDataVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSSourceData * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSSourceData * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSSourceData * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSourceModule )( 
            ICSSourceData * This,
            ICSSourceModule **ppModule);
        
        HRESULT ( STDMETHODCALLTYPE *GetLexResults )( 
            ICSSourceData * This,
            MIDL_LEXDATA *pLexData);
        
        HRESULT ( STDMETHODCALLTYPE *GetSingleTokenPos )( 
            ICSSourceData * This,
            long iToken,
            MIDL_POSDATA *pposToken);
        
        HRESULT ( STDMETHODCALLTYPE *GetSingleTokenData )( 
            ICSSourceData * This,
            long iToken,
            MIDL_TOKENDATA *pTokenData);
        
        HRESULT ( STDMETHODCALLTYPE *GetTokenText )( 
            ICSSourceData * This,
            long iTokenId,
            LPCWSTR *ppszText,
            long *piLen);
        
        HRESULT ( STDMETHODCALLTYPE *IsInsideComment )( 
            ICSSourceData * This,
            const MIDL_POSDATA pos,
            BOOL *pfInComment);
        
        HRESULT ( STDMETHODCALLTYPE *ParseTopLevel )( 
            ICSSourceData * This,
            MIDL_BASENODE **ppTree);
        
        HRESULT ( STDMETHODCALLTYPE *ParseTopLevel2 )( 
            ICSSourceData * This,
            MIDL_BASENODE **ppTree,
            BOOL fCreateParseDiffs);
        
        HRESULT ( STDMETHODCALLTYPE *GetErrors )( 
            ICSSourceData * This,
            ERRORCATEGORY iCategory,
            ICSErrorContainer **ppErrors);
        
        HRESULT ( STDMETHODCALLTYPE *GetInteriorParseTree )( 
            ICSSourceData * This,
            MIDL_BASENODE *pNode,
            ICSInteriorTree **ppTree);
        
        HRESULT ( STDMETHODCALLTYPE *LookupNode )( 
            ICSSourceData * This,
            MIDL_NAME *pKey,
            long iOrdinal,
            MIDL_BASENODE **ppNode,
            long *piGlobalOrdinal);
        
        HRESULT ( STDMETHODCALLTYPE *GetNodeKeyOrdinal )( 
            ICSSourceData * This,
            MIDL_BASENODE *pNode,
            MIDL_NAME **ppKey,
            long *piKeyOrdinal);
        
        HRESULT ( STDMETHODCALLTYPE *GetGlobalKeyArray )( 
            ICSSourceData * This,
            KEYEDNODE *pKeyedNodes,
            long iSize,
            long *piCopied);
        
        HRESULT ( STDMETHODCALLTYPE *ParseForErrors )( 
            ICSSourceData * This);
        
        HRESULT ( STDMETHODCALLTYPE *FindLeafNode )( 
            ICSSourceData * This,
            const MIDL_POSDATA pos,
            MIDL_BASENODE **ppNode,
            ICSInteriorTree **ppTree);
        
        HRESULT ( STDMETHODCALLTYPE *FindLeafNodeEx )( 
            ICSSourceData * This,
            const MIDL_POSDATA pos,
            ExtentFlags flags,
            MIDL_BASENODE **ppNode,
            ICSInteriorTree **ppTree);
        
        HRESULT ( STDMETHODCALLTYPE *FindLeafNodeForToken )( 
            ICSSourceData * This,
            long iToken,
            MIDL_BASENODE **ppNode,
            ICSInteriorTree **ppTree);
        
        HRESULT ( STDMETHODCALLTYPE *FindLeafNodeForTokenEx )( 
            ICSSourceData * This,
            long iToken,
            ExtentFlags flags,
            MIDL_BASENODE **ppNode,
            ICSInteriorTree **ppTree);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtent )( 
            ICSSourceData * This,
            MIDL_BASENODE *pNode,
            MIDL_POSDATA *pposStart,
            MIDL_POSDATA *pposEnd);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtentEx )( 
            ICSSourceData * This,
            MIDL_BASENODE *pNode,
            MIDL_POSDATA *pposStart,
            MIDL_POSDATA *pposEnd,
            ExtentFlags flags);
        
        HRESULT ( STDMETHODCALLTYPE *GetTokenExtent )( 
            ICSSourceData * This,
            MIDL_BASENODE *pNode,
            long *piFirstToken,
            long *piLastToken);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocComment )( 
            ICSSourceData * This,
            MIDL_BASENODE *pNode,
            long *piIndex,
            long *piCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocCommentXML )( 
            ICSSourceData * This,
            MIDL_BASENODE *pNode,
            BSTR *pbstrDoc);
        
        HRESULT ( STDMETHODCALLTYPE *IsUpToDate )( 
            ICSSourceData * This,
            BOOL *pfTokenized,
            BOOL *pfTopParsed);
        
        HRESULT ( STDMETHODCALLTYPE *MapSourceLine )( 
            ICSSourceData * This,
            long iline,
            long *piMappedLine,
            LPCWSTR *ppszFilename,
            BOOL *pbIsHidden);
        
        HRESULT ( STDMETHODCALLTYPE *GetLastRenamedTokenIndex )( 
            ICSSourceData * This,
            long *piTokIdx,
            MIDL_NAME **ppPreviousName);
        
        HRESULT ( STDMETHODCALLTYPE *ResetRenamedTokenData )( 
            ICSSourceData * This);
        
        HRESULT ( STDMETHODCALLTYPE *CloneToNonLocking )( 
            ICSSourceData * This,
            ICSSourceData **ppData);
        
        HRESULT ( STDMETHODCALLTYPE *OnRename )( 
            ICSSourceData * This,
            MIDL_NAME *pNewName);
        
        END_INTERFACE
    } ICSSourceDataVtbl;

    interface ICSSourceData
    {
        CONST_VTBL struct ICSSourceDataVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSSourceData_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSSourceData_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSSourceData_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSSourceData_GetSourceModule(This,ppModule)	\
    (This)->lpVtbl -> GetSourceModule(This,ppModule)

#define ICSSourceData_GetLexResults(This,pLexData)	\
    (This)->lpVtbl -> GetLexResults(This,pLexData)

#define ICSSourceData_GetSingleTokenPos(This,iToken,pposToken)	\
    (This)->lpVtbl -> GetSingleTokenPos(This,iToken,pposToken)

#define ICSSourceData_GetSingleTokenData(This,iToken,pTokenData)	\
    (This)->lpVtbl -> GetSingleTokenData(This,iToken,pTokenData)

#define ICSSourceData_GetTokenText(This,iTokenId,ppszText,piLen)	\
    (This)->lpVtbl -> GetTokenText(This,iTokenId,ppszText,piLen)

#define ICSSourceData_IsInsideComment(This,pos,pfInComment)	\
    (This)->lpVtbl -> IsInsideComment(This,pos,pfInComment)

#define ICSSourceData_ParseTopLevel(This,ppTree)	\
    (This)->lpVtbl -> ParseTopLevel(This,ppTree)

#define ICSSourceData_ParseTopLevel2(This,ppTree,fCreateParseDiffs)	\
    (This)->lpVtbl -> ParseTopLevel2(This,ppTree,fCreateParseDiffs)

#define ICSSourceData_GetErrors(This,iCategory,ppErrors)	\
    (This)->lpVtbl -> GetErrors(This,iCategory,ppErrors)

#define ICSSourceData_GetInteriorParseTree(This,pNode,ppTree)	\
    (This)->lpVtbl -> GetInteriorParseTree(This,pNode,ppTree)

#define ICSSourceData_LookupNode(This,pKey,iOrdinal,ppNode,piGlobalOrdinal)	\
    (This)->lpVtbl -> LookupNode(This,pKey,iOrdinal,ppNode,piGlobalOrdinal)

#define ICSSourceData_GetNodeKeyOrdinal(This,pNode,ppKey,piKeyOrdinal)	\
    (This)->lpVtbl -> GetNodeKeyOrdinal(This,pNode,ppKey,piKeyOrdinal)

#define ICSSourceData_GetGlobalKeyArray(This,pKeyedNodes,iSize,piCopied)	\
    (This)->lpVtbl -> GetGlobalKeyArray(This,pKeyedNodes,iSize,piCopied)

#define ICSSourceData_ParseForErrors(This)	\
    (This)->lpVtbl -> ParseForErrors(This)

#define ICSSourceData_FindLeafNode(This,pos,ppNode,ppTree)	\
    (This)->lpVtbl -> FindLeafNode(This,pos,ppNode,ppTree)

#define ICSSourceData_FindLeafNodeEx(This,pos,flags,ppNode,ppTree)	\
    (This)->lpVtbl -> FindLeafNodeEx(This,pos,flags,ppNode,ppTree)

#define ICSSourceData_FindLeafNodeForToken(This,iToken,ppNode,ppTree)	\
    (This)->lpVtbl -> FindLeafNodeForToken(This,iToken,ppNode,ppTree)

#define ICSSourceData_FindLeafNodeForTokenEx(This,iToken,flags,ppNode,ppTree)	\
    (This)->lpVtbl -> FindLeafNodeForTokenEx(This,iToken,flags,ppNode,ppTree)

#define ICSSourceData_GetExtent(This,pNode,pposStart,pposEnd)	\
    (This)->lpVtbl -> GetExtent(This,pNode,pposStart,pposEnd)

#define ICSSourceData_GetExtentEx(This,pNode,pposStart,pposEnd,flags)	\
    (This)->lpVtbl -> GetExtentEx(This,pNode,pposStart,pposEnd,flags)

#define ICSSourceData_GetTokenExtent(This,pNode,piFirstToken,piLastToken)	\
    (This)->lpVtbl -> GetTokenExtent(This,pNode,piFirstToken,piLastToken)

#define ICSSourceData_GetDocComment(This,pNode,piIndex,piCount)	\
    (This)->lpVtbl -> GetDocComment(This,pNode,piIndex,piCount)

#define ICSSourceData_GetDocCommentXML(This,pNode,pbstrDoc)	\
    (This)->lpVtbl -> GetDocCommentXML(This,pNode,pbstrDoc)

#define ICSSourceData_IsUpToDate(This,pfTokenized,pfTopParsed)	\
    (This)->lpVtbl -> IsUpToDate(This,pfTokenized,pfTopParsed)

#define ICSSourceData_MapSourceLine(This,iline,piMappedLine,ppszFilename,pbIsHidden)	\
    (This)->lpVtbl -> MapSourceLine(This,iline,piMappedLine,ppszFilename,pbIsHidden)

#define ICSSourceData_GetLastRenamedTokenIndex(This,piTokIdx,ppPreviousName)	\
    (This)->lpVtbl -> GetLastRenamedTokenIndex(This,piTokIdx,ppPreviousName)

#define ICSSourceData_ResetRenamedTokenData(This)	\
    (This)->lpVtbl -> ResetRenamedTokenData(This)

#define ICSSourceData_CloneToNonLocking(This,ppData)	\
    (This)->lpVtbl -> CloneToNonLocking(This,ppData)

#define ICSSourceData_OnRename(This,pNewName)	\
    (This)->lpVtbl -> OnRename(This,pNewName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSSourceData_GetSourceModule_Proxy( 
    ICSSourceData * This,
    ICSSourceModule **ppModule);


void __RPC_STUB ICSSourceData_GetSourceModule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_GetLexResults_Proxy( 
    ICSSourceData * This,
    MIDL_LEXDATA *pLexData);


void __RPC_STUB ICSSourceData_GetLexResults_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_GetSingleTokenPos_Proxy( 
    ICSSourceData * This,
    long iToken,
    MIDL_POSDATA *pposToken);


void __RPC_STUB ICSSourceData_GetSingleTokenPos_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_GetSingleTokenData_Proxy( 
    ICSSourceData * This,
    long iToken,
    MIDL_TOKENDATA *pTokenData);


void __RPC_STUB ICSSourceData_GetSingleTokenData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_GetTokenText_Proxy( 
    ICSSourceData * This,
    long iTokenId,
    LPCWSTR *ppszText,
    long *piLen);


void __RPC_STUB ICSSourceData_GetTokenText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_IsInsideComment_Proxy( 
    ICSSourceData * This,
    const MIDL_POSDATA pos,
    BOOL *pfInComment);


void __RPC_STUB ICSSourceData_IsInsideComment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_ParseTopLevel_Proxy( 
    ICSSourceData * This,
    MIDL_BASENODE **ppTree);


void __RPC_STUB ICSSourceData_ParseTopLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_ParseTopLevel2_Proxy( 
    ICSSourceData * This,
    MIDL_BASENODE **ppTree,
    BOOL fCreateParseDiffs);


void __RPC_STUB ICSSourceData_ParseTopLevel2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_GetErrors_Proxy( 
    ICSSourceData * This,
    ERRORCATEGORY iCategory,
    ICSErrorContainer **ppErrors);


void __RPC_STUB ICSSourceData_GetErrors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_GetInteriorParseTree_Proxy( 
    ICSSourceData * This,
    MIDL_BASENODE *pNode,
    ICSInteriorTree **ppTree);


void __RPC_STUB ICSSourceData_GetInteriorParseTree_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_LookupNode_Proxy( 
    ICSSourceData * This,
    MIDL_NAME *pKey,
    long iOrdinal,
    MIDL_BASENODE **ppNode,
    long *piGlobalOrdinal);


void __RPC_STUB ICSSourceData_LookupNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_GetNodeKeyOrdinal_Proxy( 
    ICSSourceData * This,
    MIDL_BASENODE *pNode,
    MIDL_NAME **ppKey,
    long *piKeyOrdinal);


void __RPC_STUB ICSSourceData_GetNodeKeyOrdinal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_GetGlobalKeyArray_Proxy( 
    ICSSourceData * This,
    KEYEDNODE *pKeyedNodes,
    long iSize,
    long *piCopied);


void __RPC_STUB ICSSourceData_GetGlobalKeyArray_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_ParseForErrors_Proxy( 
    ICSSourceData * This);


void __RPC_STUB ICSSourceData_ParseForErrors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_FindLeafNode_Proxy( 
    ICSSourceData * This,
    const MIDL_POSDATA pos,
    MIDL_BASENODE **ppNode,
    ICSInteriorTree **ppTree);


void __RPC_STUB ICSSourceData_FindLeafNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_FindLeafNodeEx_Proxy( 
    ICSSourceData * This,
    const MIDL_POSDATA pos,
    ExtentFlags flags,
    MIDL_BASENODE **ppNode,
    ICSInteriorTree **ppTree);


void __RPC_STUB ICSSourceData_FindLeafNodeEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_FindLeafNodeForToken_Proxy( 
    ICSSourceData * This,
    long iToken,
    MIDL_BASENODE **ppNode,
    ICSInteriorTree **ppTree);


void __RPC_STUB ICSSourceData_FindLeafNodeForToken_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_FindLeafNodeForTokenEx_Proxy( 
    ICSSourceData * This,
    long iToken,
    ExtentFlags flags,
    MIDL_BASENODE **ppNode,
    ICSInteriorTree **ppTree);


void __RPC_STUB ICSSourceData_FindLeafNodeForTokenEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_GetExtent_Proxy( 
    ICSSourceData * This,
    MIDL_BASENODE *pNode,
    MIDL_POSDATA *pposStart,
    MIDL_POSDATA *pposEnd);


void __RPC_STUB ICSSourceData_GetExtent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_GetExtentEx_Proxy( 
    ICSSourceData * This,
    MIDL_BASENODE *pNode,
    MIDL_POSDATA *pposStart,
    MIDL_POSDATA *pposEnd,
    ExtentFlags flags);


void __RPC_STUB ICSSourceData_GetExtentEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_GetTokenExtent_Proxy( 
    ICSSourceData * This,
    MIDL_BASENODE *pNode,
    long *piFirstToken,
    long *piLastToken);


void __RPC_STUB ICSSourceData_GetTokenExtent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_GetDocComment_Proxy( 
    ICSSourceData * This,
    MIDL_BASENODE *pNode,
    long *piIndex,
    long *piCount);


void __RPC_STUB ICSSourceData_GetDocComment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_GetDocCommentXML_Proxy( 
    ICSSourceData * This,
    MIDL_BASENODE *pNode,
    BSTR *pbstrDoc);


void __RPC_STUB ICSSourceData_GetDocCommentXML_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_IsUpToDate_Proxy( 
    ICSSourceData * This,
    BOOL *pfTokenized,
    BOOL *pfTopParsed);


void __RPC_STUB ICSSourceData_IsUpToDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_MapSourceLine_Proxy( 
    ICSSourceData * This,
    long iline,
    long *piMappedLine,
    LPCWSTR *ppszFilename,
    BOOL *pbIsHidden);


void __RPC_STUB ICSSourceData_MapSourceLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_GetLastRenamedTokenIndex_Proxy( 
    ICSSourceData * This,
    long *piTokIdx,
    MIDL_NAME **ppPreviousName);


void __RPC_STUB ICSSourceData_GetLastRenamedTokenIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_ResetRenamedTokenData_Proxy( 
    ICSSourceData * This);


void __RPC_STUB ICSSourceData_ResetRenamedTokenData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_CloneToNonLocking_Proxy( 
    ICSSourceData * This,
    ICSSourceData **ppData);


void __RPC_STUB ICSSourceData_CloneToNonLocking_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceData_OnRename_Proxy( 
    ICSSourceData * This,
    MIDL_NAME *pNewName);


void __RPC_STUB ICSSourceData_OnRename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSSourceData_INTERFACE_DEFINED__ */


#ifndef __ICSSourceModuleEvents_INTERFACE_DEFINED__
#define __ICSSourceModuleEvents_INTERFACE_DEFINED__

/* interface ICSSourceModuleEvents */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSSourceModuleEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F78E9D16-E63F-11d2-B556-00C04F68D4DB")
    ICSSourceModuleEvents : public IUnknown
    {
    public:
        virtual void STDMETHODCALLTYPE OnModuleModified( void) = 0;
        
        virtual void STDMETHODCALLTYPE OnStateTransitionsChanged( 
            long iFirstLine,
            long iLastLine) = 0;
        
        virtual void STDMETHODCALLTYPE OnCommentTableChanged( 
            long iStart,
            long iOldEnd,
            long iNewEnd) = 0;
        
        virtual void STDMETHODCALLTYPE OnBeginTokenization( 
            long iStartingAtLine) = 0;
        
        virtual void STDMETHODCALLTYPE OnEndTokenization( 
            ICSSourceData *pData,
            long iFirstLine,
            long iLastLine,
            long iDelta) = 0;
        
        virtual void STDMETHODCALLTYPE OnBeginParse( 
            MIDL_BASENODE *pNode) = 0;
        
        virtual void STDMETHODCALLTYPE OnEndParse( 
            MIDL_BASENODE *pNode) = 0;
        
        virtual void STDMETHODCALLTYPE ReportErrors( 
            ICSErrorContainer *pErrors) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSSourceModuleEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSSourceModuleEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSSourceModuleEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSSourceModuleEvents * This);
        
        void ( STDMETHODCALLTYPE *OnModuleModified )( 
            ICSSourceModuleEvents * This);
        
        void ( STDMETHODCALLTYPE *OnStateTransitionsChanged )( 
            ICSSourceModuleEvents * This,
            long iFirstLine,
            long iLastLine);
        
        void ( STDMETHODCALLTYPE *OnCommentTableChanged )( 
            ICSSourceModuleEvents * This,
            long iStart,
            long iOldEnd,
            long iNewEnd);
        
        void ( STDMETHODCALLTYPE *OnBeginTokenization )( 
            ICSSourceModuleEvents * This,
            long iStartingAtLine);
        
        void ( STDMETHODCALLTYPE *OnEndTokenization )( 
            ICSSourceModuleEvents * This,
            ICSSourceData *pData,
            long iFirstLine,
            long iLastLine,
            long iDelta);
        
        void ( STDMETHODCALLTYPE *OnBeginParse )( 
            ICSSourceModuleEvents * This,
            MIDL_BASENODE *pNode);
        
        void ( STDMETHODCALLTYPE *OnEndParse )( 
            ICSSourceModuleEvents * This,
            MIDL_BASENODE *pNode);
        
        void ( STDMETHODCALLTYPE *ReportErrors )( 
            ICSSourceModuleEvents * This,
            ICSErrorContainer *pErrors);
        
        END_INTERFACE
    } ICSSourceModuleEventsVtbl;

    interface ICSSourceModuleEvents
    {
        CONST_VTBL struct ICSSourceModuleEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSSourceModuleEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSSourceModuleEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSSourceModuleEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSSourceModuleEvents_OnModuleModified(This)	\
    (This)->lpVtbl -> OnModuleModified(This)

#define ICSSourceModuleEvents_OnStateTransitionsChanged(This,iFirstLine,iLastLine)	\
    (This)->lpVtbl -> OnStateTransitionsChanged(This,iFirstLine,iLastLine)

#define ICSSourceModuleEvents_OnCommentTableChanged(This,iStart,iOldEnd,iNewEnd)	\
    (This)->lpVtbl -> OnCommentTableChanged(This,iStart,iOldEnd,iNewEnd)

#define ICSSourceModuleEvents_OnBeginTokenization(This,iStartingAtLine)	\
    (This)->lpVtbl -> OnBeginTokenization(This,iStartingAtLine)

#define ICSSourceModuleEvents_OnEndTokenization(This,pData,iFirstLine,iLastLine,iDelta)	\
    (This)->lpVtbl -> OnEndTokenization(This,pData,iFirstLine,iLastLine,iDelta)

#define ICSSourceModuleEvents_OnBeginParse(This,pNode)	\
    (This)->lpVtbl -> OnBeginParse(This,pNode)

#define ICSSourceModuleEvents_OnEndParse(This,pNode)	\
    (This)->lpVtbl -> OnEndParse(This,pNode)

#define ICSSourceModuleEvents_ReportErrors(This,pErrors)	\
    (This)->lpVtbl -> ReportErrors(This,pErrors)

#endif /* COBJMACROS */


#endif 	/* C style interface */



void STDMETHODCALLTYPE ICSSourceModuleEvents_OnModuleModified_Proxy( 
    ICSSourceModuleEvents * This);


void __RPC_STUB ICSSourceModuleEvents_OnModuleModified_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE ICSSourceModuleEvents_OnStateTransitionsChanged_Proxy( 
    ICSSourceModuleEvents * This,
    long iFirstLine,
    long iLastLine);


void __RPC_STUB ICSSourceModuleEvents_OnStateTransitionsChanged_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE ICSSourceModuleEvents_OnCommentTableChanged_Proxy( 
    ICSSourceModuleEvents * This,
    long iStart,
    long iOldEnd,
    long iNewEnd);


void __RPC_STUB ICSSourceModuleEvents_OnCommentTableChanged_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE ICSSourceModuleEvents_OnBeginTokenization_Proxy( 
    ICSSourceModuleEvents * This,
    long iStartingAtLine);


void __RPC_STUB ICSSourceModuleEvents_OnBeginTokenization_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE ICSSourceModuleEvents_OnEndTokenization_Proxy( 
    ICSSourceModuleEvents * This,
    ICSSourceData *pData,
    long iFirstLine,
    long iLastLine,
    long iDelta);


void __RPC_STUB ICSSourceModuleEvents_OnEndTokenization_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE ICSSourceModuleEvents_OnBeginParse_Proxy( 
    ICSSourceModuleEvents * This,
    MIDL_BASENODE *pNode);


void __RPC_STUB ICSSourceModuleEvents_OnBeginParse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE ICSSourceModuleEvents_OnEndParse_Proxy( 
    ICSSourceModuleEvents * This,
    MIDL_BASENODE *pNode);


void __RPC_STUB ICSSourceModuleEvents_OnEndParse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE ICSSourceModuleEvents_ReportErrors_Proxy( 
    ICSSourceModuleEvents * This,
    ICSErrorContainer *pErrors);


void __RPC_STUB ICSSourceModuleEvents_ReportErrors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSSourceModuleEvents_INTERFACE_DEFINED__ */


#ifndef __ICSSourceModule_INTERFACE_DEFINED__
#define __ICSSourceModule_INTERFACE_DEFINED__

/* interface ICSSourceModule */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSSourceModule;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D9BF2800-E098-11d2-B554-00C04F68D4DB")
    ICSSourceModule : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetSourceData( 
            BOOL fBlockForNewest,
            ICSSourceData **ppSourceData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSourceText( 
            ICSSourceText **ppText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Flush( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DisconnectTextEvents( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParseDiffQ( 
            MIDL_EVENTNODE **__MIDL_0010) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMemoryConsumption( 
            long *piTopTree,
            long *piInteriorTrees,
            long *piInteriorNodes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AdviseChangeEvents( 
            ICSSourceModuleEvents *pSink,
            DWORD_PTR *pdwCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnadviseChangeEvents( 
            DWORD_PTR dwCookie) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSSourceModuleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSSourceModule * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSSourceModule * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSSourceModule * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSourceData )( 
            ICSSourceModule * This,
            BOOL fBlockForNewest,
            ICSSourceData **ppSourceData);
        
        HRESULT ( STDMETHODCALLTYPE *GetSourceText )( 
            ICSSourceModule * This,
            ICSSourceText **ppText);
        
        HRESULT ( STDMETHODCALLTYPE *Flush )( 
            ICSSourceModule * This);
        
        HRESULT ( STDMETHODCALLTYPE *DisconnectTextEvents )( 
            ICSSourceModule * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetParseDiffQ )( 
            ICSSourceModule * This,
            MIDL_EVENTNODE **__MIDL_0010);
        
        HRESULT ( STDMETHODCALLTYPE *GetMemoryConsumption )( 
            ICSSourceModule * This,
            long *piTopTree,
            long *piInteriorTrees,
            long *piInteriorNodes);
        
        HRESULT ( STDMETHODCALLTYPE *AdviseChangeEvents )( 
            ICSSourceModule * This,
            ICSSourceModuleEvents *pSink,
            DWORD_PTR *pdwCookie);
        
        HRESULT ( STDMETHODCALLTYPE *UnadviseChangeEvents )( 
            ICSSourceModule * This,
            DWORD_PTR dwCookie);
        
        END_INTERFACE
    } ICSSourceModuleVtbl;

    interface ICSSourceModule
    {
        CONST_VTBL struct ICSSourceModuleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSSourceModule_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSSourceModule_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSSourceModule_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSSourceModule_GetSourceData(This,fBlockForNewest,ppSourceData)	\
    (This)->lpVtbl -> GetSourceData(This,fBlockForNewest,ppSourceData)

#define ICSSourceModule_GetSourceText(This,ppText)	\
    (This)->lpVtbl -> GetSourceText(This,ppText)

#define ICSSourceModule_Flush(This)	\
    (This)->lpVtbl -> Flush(This)

#define ICSSourceModule_DisconnectTextEvents(This)	\
    (This)->lpVtbl -> DisconnectTextEvents(This)

#define ICSSourceModule_GetParseDiffQ(This,__MIDL_0010)	\
    (This)->lpVtbl -> GetParseDiffQ(This,__MIDL_0010)

#define ICSSourceModule_GetMemoryConsumption(This,piTopTree,piInteriorTrees,piInteriorNodes)	\
    (This)->lpVtbl -> GetMemoryConsumption(This,piTopTree,piInteriorTrees,piInteriorNodes)

#define ICSSourceModule_AdviseChangeEvents(This,pSink,pdwCookie)	\
    (This)->lpVtbl -> AdviseChangeEvents(This,pSink,pdwCookie)

#define ICSSourceModule_UnadviseChangeEvents(This,dwCookie)	\
    (This)->lpVtbl -> UnadviseChangeEvents(This,dwCookie)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSSourceModule_GetSourceData_Proxy( 
    ICSSourceModule * This,
    BOOL fBlockForNewest,
    ICSSourceData **ppSourceData);


void __RPC_STUB ICSSourceModule_GetSourceData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceModule_GetSourceText_Proxy( 
    ICSSourceModule * This,
    ICSSourceText **ppText);


void __RPC_STUB ICSSourceModule_GetSourceText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceModule_Flush_Proxy( 
    ICSSourceModule * This);


void __RPC_STUB ICSSourceModule_Flush_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceModule_DisconnectTextEvents_Proxy( 
    ICSSourceModule * This);


void __RPC_STUB ICSSourceModule_DisconnectTextEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceModule_GetParseDiffQ_Proxy( 
    ICSSourceModule * This,
    MIDL_EVENTNODE **__MIDL_0010);


void __RPC_STUB ICSSourceModule_GetParseDiffQ_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceModule_GetMemoryConsumption_Proxy( 
    ICSSourceModule * This,
    long *piTopTree,
    long *piInteriorTrees,
    long *piInteriorNodes);


void __RPC_STUB ICSSourceModule_GetMemoryConsumption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceModule_AdviseChangeEvents_Proxy( 
    ICSSourceModule * This,
    ICSSourceModuleEvents *pSink,
    DWORD_PTR *pdwCookie);


void __RPC_STUB ICSSourceModule_AdviseChangeEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSSourceModule_UnadviseChangeEvents_Proxy( 
    ICSSourceModule * This,
    DWORD_PTR dwCookie);


void __RPC_STUB ICSSourceModule_UnadviseChangeEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSSourceModule_INTERFACE_DEFINED__ */


#ifndef __ICSNameTable_INTERFACE_DEFINED__
#define __ICSNameTable_INTERFACE_DEFINED__

/* interface ICSNameTable */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSNameTable;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3246651A-E0B4-11d2-B555-00C04F68D4DB")
    ICSNameTable : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Lookup( 
            LPCWSTR pszName,
            MIDL_NAME **ppName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LookupLen( 
            LPCWSTR pszName,
            long iLen,
            MIDL_NAME **ppName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Add( 
            LPCWSTR pszName,
            MIDL_NAME **ppName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddLen( 
            LPCWSTR pszName,
            long iLen,
            MIDL_NAME **ppName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsValidIdentifier( 
            LPCWSTR pszName,
            CompatibilityMode eKeywordMode,
            VARIANT_BOOL *pfValid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTokenText( 
            long iTokenId,
            LPCWSTR *ppszText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPredefinedName( 
            long iPredefName,
            MIDL_NAME **ppName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPreprocessorDirective( 
            MIDL_NAME *pName,
            long *piToken) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsKeyword( 
            MIDL_NAME *pName,
            CompatibilityMode eKeywordMode,
            long *piToken) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CheckCompilerVersion( 
            HINSTANCE hInstance,
            DWORD dwVersion,
            BSTR *pbstrError) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ValidateName( 
            MIDL_NAME *pName,
            BOOL *pfValid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSNameTableVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSNameTable * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSNameTable * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSNameTable * This);
        
        HRESULT ( STDMETHODCALLTYPE *Lookup )( 
            ICSNameTable * This,
            LPCWSTR pszName,
            MIDL_NAME **ppName);
        
        HRESULT ( STDMETHODCALLTYPE *LookupLen )( 
            ICSNameTable * This,
            LPCWSTR pszName,
            long iLen,
            MIDL_NAME **ppName);
        
        HRESULT ( STDMETHODCALLTYPE *Add )( 
            ICSNameTable * This,
            LPCWSTR pszName,
            MIDL_NAME **ppName);
        
        HRESULT ( STDMETHODCALLTYPE *AddLen )( 
            ICSNameTable * This,
            LPCWSTR pszName,
            long iLen,
            MIDL_NAME **ppName);
        
        HRESULT ( STDMETHODCALLTYPE *IsValidIdentifier )( 
            ICSNameTable * This,
            LPCWSTR pszName,
            CompatibilityMode eKeywordMode,
            VARIANT_BOOL *pfValid);
        
        HRESULT ( STDMETHODCALLTYPE *GetTokenText )( 
            ICSNameTable * This,
            long iTokenId,
            LPCWSTR *ppszText);
        
        HRESULT ( STDMETHODCALLTYPE *GetPredefinedName )( 
            ICSNameTable * This,
            long iPredefName,
            MIDL_NAME **ppName);
        
        HRESULT ( STDMETHODCALLTYPE *GetPreprocessorDirective )( 
            ICSNameTable * This,
            MIDL_NAME *pName,
            long *piToken);
        
        HRESULT ( STDMETHODCALLTYPE *IsKeyword )( 
            ICSNameTable * This,
            MIDL_NAME *pName,
            CompatibilityMode eKeywordMode,
            long *piToken);
        
        HRESULT ( STDMETHODCALLTYPE *CheckCompilerVersion )( 
            ICSNameTable * This,
            HINSTANCE hInstance,
            DWORD dwVersion,
            BSTR *pbstrError);
        
        HRESULT ( STDMETHODCALLTYPE *ValidateName )( 
            ICSNameTable * This,
            MIDL_NAME *pName,
            BOOL *pfValid);
        
        END_INTERFACE
    } ICSNameTableVtbl;

    interface ICSNameTable
    {
        CONST_VTBL struct ICSNameTableVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSNameTable_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSNameTable_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSNameTable_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSNameTable_Lookup(This,pszName,ppName)	\
    (This)->lpVtbl -> Lookup(This,pszName,ppName)

#define ICSNameTable_LookupLen(This,pszName,iLen,ppName)	\
    (This)->lpVtbl -> LookupLen(This,pszName,iLen,ppName)

#define ICSNameTable_Add(This,pszName,ppName)	\
    (This)->lpVtbl -> Add(This,pszName,ppName)

#define ICSNameTable_AddLen(This,pszName,iLen,ppName)	\
    (This)->lpVtbl -> AddLen(This,pszName,iLen,ppName)

#define ICSNameTable_IsValidIdentifier(This,pszName,eKeywordMode,pfValid)	\
    (This)->lpVtbl -> IsValidIdentifier(This,pszName,eKeywordMode,pfValid)

#define ICSNameTable_GetTokenText(This,iTokenId,ppszText)	\
    (This)->lpVtbl -> GetTokenText(This,iTokenId,ppszText)

#define ICSNameTable_GetPredefinedName(This,iPredefName,ppName)	\
    (This)->lpVtbl -> GetPredefinedName(This,iPredefName,ppName)

#define ICSNameTable_GetPreprocessorDirective(This,pName,piToken)	\
    (This)->lpVtbl -> GetPreprocessorDirective(This,pName,piToken)

#define ICSNameTable_IsKeyword(This,pName,eKeywordMode,piToken)	\
    (This)->lpVtbl -> IsKeyword(This,pName,eKeywordMode,piToken)

#define ICSNameTable_CheckCompilerVersion(This,hInstance,dwVersion,pbstrError)	\
    (This)->lpVtbl -> CheckCompilerVersion(This,hInstance,dwVersion,pbstrError)

#define ICSNameTable_ValidateName(This,pName,pfValid)	\
    (This)->lpVtbl -> ValidateName(This,pName,pfValid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSNameTable_Lookup_Proxy( 
    ICSNameTable * This,
    LPCWSTR pszName,
    MIDL_NAME **ppName);


void __RPC_STUB ICSNameTable_Lookup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSNameTable_LookupLen_Proxy( 
    ICSNameTable * This,
    LPCWSTR pszName,
    long iLen,
    MIDL_NAME **ppName);


void __RPC_STUB ICSNameTable_LookupLen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSNameTable_Add_Proxy( 
    ICSNameTable * This,
    LPCWSTR pszName,
    MIDL_NAME **ppName);


void __RPC_STUB ICSNameTable_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSNameTable_AddLen_Proxy( 
    ICSNameTable * This,
    LPCWSTR pszName,
    long iLen,
    MIDL_NAME **ppName);


void __RPC_STUB ICSNameTable_AddLen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSNameTable_IsValidIdentifier_Proxy( 
    ICSNameTable * This,
    LPCWSTR pszName,
    CompatibilityMode eKeywordMode,
    VARIANT_BOOL *pfValid);


void __RPC_STUB ICSNameTable_IsValidIdentifier_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSNameTable_GetTokenText_Proxy( 
    ICSNameTable * This,
    long iTokenId,
    LPCWSTR *ppszText);


void __RPC_STUB ICSNameTable_GetTokenText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSNameTable_GetPredefinedName_Proxy( 
    ICSNameTable * This,
    long iPredefName,
    MIDL_NAME **ppName);


void __RPC_STUB ICSNameTable_GetPredefinedName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSNameTable_GetPreprocessorDirective_Proxy( 
    ICSNameTable * This,
    MIDL_NAME *pName,
    long *piToken);


void __RPC_STUB ICSNameTable_GetPreprocessorDirective_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSNameTable_IsKeyword_Proxy( 
    ICSNameTable * This,
    MIDL_NAME *pName,
    CompatibilityMode eKeywordMode,
    long *piToken);


void __RPC_STUB ICSNameTable_IsKeyword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSNameTable_CheckCompilerVersion_Proxy( 
    ICSNameTable * This,
    HINSTANCE hInstance,
    DWORD dwVersion,
    BSTR *pbstrError);


void __RPC_STUB ICSNameTable_CheckCompilerVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSNameTable_ValidateName_Proxy( 
    ICSNameTable * This,
    MIDL_NAME *pName,
    BOOL *pfValid);


void __RPC_STUB ICSNameTable_ValidateName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSNameTable_INTERFACE_DEFINED__ */


#ifndef __ICSLexer_INTERFACE_DEFINED__
#define __ICSLexer_INTERFACE_DEFINED__

/* interface ICSLexer */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSLexer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("064C8C67-5F66-4bf1-AF98-ED8BFA7B690A")
    ICSLexer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetInput( 
            LPCWSTR pszInputText,
            long iLen) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLexResults( 
            MIDL_LEXDATA *pLexData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RescanToken( 
            long iToken,
            MIDL_TOKENDATA *pTokenData) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSLexerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSLexer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSLexer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSLexer * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetInput )( 
            ICSLexer * This,
            LPCWSTR pszInputText,
            long iLen);
        
        HRESULT ( STDMETHODCALLTYPE *GetLexResults )( 
            ICSLexer * This,
            MIDL_LEXDATA *pLexData);
        
        HRESULT ( STDMETHODCALLTYPE *RescanToken )( 
            ICSLexer * This,
            long iToken,
            MIDL_TOKENDATA *pTokenData);
        
        END_INTERFACE
    } ICSLexerVtbl;

    interface ICSLexer
    {
        CONST_VTBL struct ICSLexerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSLexer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSLexer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSLexer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSLexer_SetInput(This,pszInputText,iLen)	\
    (This)->lpVtbl -> SetInput(This,pszInputText,iLen)

#define ICSLexer_GetLexResults(This,pLexData)	\
    (This)->lpVtbl -> GetLexResults(This,pLexData)

#define ICSLexer_RescanToken(This,iToken,pTokenData)	\
    (This)->lpVtbl -> RescanToken(This,iToken,pTokenData)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSLexer_SetInput_Proxy( 
    ICSLexer * This,
    LPCWSTR pszInputText,
    long iLen);


void __RPC_STUB ICSLexer_SetInput_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSLexer_GetLexResults_Proxy( 
    ICSLexer * This,
    MIDL_LEXDATA *pLexData);


void __RPC_STUB ICSLexer_GetLexResults_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSLexer_RescanToken_Proxy( 
    ICSLexer * This,
    long iToken,
    MIDL_TOKENDATA *pTokenData);


void __RPC_STUB ICSLexer_RescanToken_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSLexer_INTERFACE_DEFINED__ */


#ifndef __ICSParser_INTERFACE_DEFINED__
#define __ICSParser_INTERFACE_DEFINED__

/* interface ICSParser */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSParser;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E5B1B490-850B-4aad-9D68-B5B7CA2DAAD2")
    ICSParser : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetInputText( 
            LPCWSTR pszInputText,
            long iLen) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateParseTree( 
            ParseTreeScope iScope,
            MIDL_BASENODE *pParent,
            MIDL_BASENODE **ppNode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AllocateNode( 
            long iKind,
            MIDL_BASENODE **ppNode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FindLeafNode( 
            const MIDL_POSDATA pos,
            MIDL_BASENODE *pNode,
            MIDL_BASENODE **ppLeafNode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FindLeafNodeForToken( 
            long iToken,
            MIDL_BASENODE *pNode,
            MIDL_BASENODE **ppNode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLexResults( 
            MIDL_LEXDATA *pLexData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetExtent( 
            MIDL_BASENODE *pNode,
            MIDL_POSDATA *pposStart,
            MIDL_POSDATA *pposEnd,
            ExtentFlags flags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTokenExtent( 
            MIDL_BASENODE *pNode,
            long *piFirstToken,
            long *piLastToken,
            ExtentFlags flags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSParserVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSParser * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSParser * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSParser * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetInputText )( 
            ICSParser * This,
            LPCWSTR pszInputText,
            long iLen);
        
        HRESULT ( STDMETHODCALLTYPE *CreateParseTree )( 
            ICSParser * This,
            ParseTreeScope iScope,
            MIDL_BASENODE *pParent,
            MIDL_BASENODE **ppNode);
        
        HRESULT ( STDMETHODCALLTYPE *AllocateNode )( 
            ICSParser * This,
            long iKind,
            MIDL_BASENODE **ppNode);
        
        HRESULT ( STDMETHODCALLTYPE *FindLeafNode )( 
            ICSParser * This,
            const MIDL_POSDATA pos,
            MIDL_BASENODE *pNode,
            MIDL_BASENODE **ppLeafNode);
        
        HRESULT ( STDMETHODCALLTYPE *FindLeafNodeForToken )( 
            ICSParser * This,
            long iToken,
            MIDL_BASENODE *pNode,
            MIDL_BASENODE **ppNode);
        
        HRESULT ( STDMETHODCALLTYPE *GetLexResults )( 
            ICSParser * This,
            MIDL_LEXDATA *pLexData);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtent )( 
            ICSParser * This,
            MIDL_BASENODE *pNode,
            MIDL_POSDATA *pposStart,
            MIDL_POSDATA *pposEnd,
            ExtentFlags flags);
        
        HRESULT ( STDMETHODCALLTYPE *GetTokenExtent )( 
            ICSParser * This,
            MIDL_BASENODE *pNode,
            long *piFirstToken,
            long *piLastToken,
            ExtentFlags flags);
        
        END_INTERFACE
    } ICSParserVtbl;

    interface ICSParser
    {
        CONST_VTBL struct ICSParserVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSParser_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSParser_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSParser_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSParser_SetInputText(This,pszInputText,iLen)	\
    (This)->lpVtbl -> SetInputText(This,pszInputText,iLen)

#define ICSParser_CreateParseTree(This,iScope,pParent,ppNode)	\
    (This)->lpVtbl -> CreateParseTree(This,iScope,pParent,ppNode)

#define ICSParser_AllocateNode(This,iKind,ppNode)	\
    (This)->lpVtbl -> AllocateNode(This,iKind,ppNode)

#define ICSParser_FindLeafNode(This,pos,pNode,ppLeafNode)	\
    (This)->lpVtbl -> FindLeafNode(This,pos,pNode,ppLeafNode)

#define ICSParser_FindLeafNodeForToken(This,iToken,pNode,ppNode)	\
    (This)->lpVtbl -> FindLeafNodeForToken(This,iToken,pNode,ppNode)

#define ICSParser_GetLexResults(This,pLexData)	\
    (This)->lpVtbl -> GetLexResults(This,pLexData)

#define ICSParser_GetExtent(This,pNode,pposStart,pposEnd,flags)	\
    (This)->lpVtbl -> GetExtent(This,pNode,pposStart,pposEnd,flags)

#define ICSParser_GetTokenExtent(This,pNode,piFirstToken,piLastToken,flags)	\
    (This)->lpVtbl -> GetTokenExtent(This,pNode,piFirstToken,piLastToken,flags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSParser_SetInputText_Proxy( 
    ICSParser * This,
    LPCWSTR pszInputText,
    long iLen);


void __RPC_STUB ICSParser_SetInputText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSParser_CreateParseTree_Proxy( 
    ICSParser * This,
    ParseTreeScope iScope,
    MIDL_BASENODE *pParent,
    MIDL_BASENODE **ppNode);


void __RPC_STUB ICSParser_CreateParseTree_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSParser_AllocateNode_Proxy( 
    ICSParser * This,
    long iKind,
    MIDL_BASENODE **ppNode);


void __RPC_STUB ICSParser_AllocateNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSParser_FindLeafNode_Proxy( 
    ICSParser * This,
    const MIDL_POSDATA pos,
    MIDL_BASENODE *pNode,
    MIDL_BASENODE **ppLeafNode);


void __RPC_STUB ICSParser_FindLeafNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSParser_FindLeafNodeForToken_Proxy( 
    ICSParser * This,
    long iToken,
    MIDL_BASENODE *pNode,
    MIDL_BASENODE **ppNode);


void __RPC_STUB ICSParser_FindLeafNodeForToken_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSParser_GetLexResults_Proxy( 
    ICSParser * This,
    MIDL_LEXDATA *pLexData);


void __RPC_STUB ICSParser_GetLexResults_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSParser_GetExtent_Proxy( 
    ICSParser * This,
    MIDL_BASENODE *pNode,
    MIDL_POSDATA *pposStart,
    MIDL_POSDATA *pposEnd,
    ExtentFlags flags);


void __RPC_STUB ICSParser_GetExtent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSParser_GetTokenExtent_Proxy( 
    ICSParser * This,
    MIDL_BASENODE *pNode,
    long *piFirstToken,
    long *piLastToken,
    ExtentFlags flags);


void __RPC_STUB ICSParser_GetTokenExtent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSParser_INTERFACE_DEFINED__ */


#ifndef __ICSCompilerConfig_INTERFACE_DEFINED__
#define __ICSCompilerConfig_INTERFACE_DEFINED__

/* interface ICSCompilerConfig */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSCompilerConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4D5D4C21-EE19-11d2-B556-00C04F68D4DB")
    ICSCompilerConfig : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetOptionCount( 
            long *piCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOptionInfoAt( 
            long iIndex,
            long *piOptionID,
            LPCWSTR *ppszSwitchName,
            LPCWSTR *ppszDescription,
            DWORD *pdwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOptionInfoAtEx( 
            long iIndex,
            long *piOptionID,
            LPCWSTR *ppszShortSwitchName,
            LPCWSTR *ppszLongSwitchName,
            LPCWSTR *ppszDescSwitchName,
            LPCWSTR *ppszDescription,
            DWORD *pdwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ResetAllOptions( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetOption( 
            long iOptionID,
            VARIANT vtValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOption( 
            long iOptionID,
            VARIANT *pvtValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CommitChanges( 
            ICSError **ppError) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCompiler( 
            ICSCompiler **ppCompiler) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWarnNumbers( 
            long *piCount,
            const long **pWarnIds) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWarnInfo( 
            long iWarnIndex,
            LPCWSTR *ppszWarnDescription) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSCompilerConfigVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSCompilerConfig * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSCompilerConfig * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSCompilerConfig * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetOptionCount )( 
            ICSCompilerConfig * This,
            long *piCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetOptionInfoAt )( 
            ICSCompilerConfig * This,
            long iIndex,
            long *piOptionID,
            LPCWSTR *ppszSwitchName,
            LPCWSTR *ppszDescription,
            DWORD *pdwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetOptionInfoAtEx )( 
            ICSCompilerConfig * This,
            long iIndex,
            long *piOptionID,
            LPCWSTR *ppszShortSwitchName,
            LPCWSTR *ppszLongSwitchName,
            LPCWSTR *ppszDescSwitchName,
            LPCWSTR *ppszDescription,
            DWORD *pdwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *ResetAllOptions )( 
            ICSCompilerConfig * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetOption )( 
            ICSCompilerConfig * This,
            long iOptionID,
            VARIANT vtValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetOption )( 
            ICSCompilerConfig * This,
            long iOptionID,
            VARIANT *pvtValue);
        
        HRESULT ( STDMETHODCALLTYPE *CommitChanges )( 
            ICSCompilerConfig * This,
            ICSError **ppError);
        
        HRESULT ( STDMETHODCALLTYPE *GetCompiler )( 
            ICSCompilerConfig * This,
            ICSCompiler **ppCompiler);
        
        HRESULT ( STDMETHODCALLTYPE *GetWarnNumbers )( 
            ICSCompilerConfig * This,
            long *piCount,
            const long **pWarnIds);
        
        HRESULT ( STDMETHODCALLTYPE *GetWarnInfo )( 
            ICSCompilerConfig * This,
            long iWarnIndex,
            LPCWSTR *ppszWarnDescription);
        
        END_INTERFACE
    } ICSCompilerConfigVtbl;

    interface ICSCompilerConfig
    {
        CONST_VTBL struct ICSCompilerConfigVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSCompilerConfig_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSCompilerConfig_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSCompilerConfig_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSCompilerConfig_GetOptionCount(This,piCount)	\
    (This)->lpVtbl -> GetOptionCount(This,piCount)

#define ICSCompilerConfig_GetOptionInfoAt(This,iIndex,piOptionID,ppszSwitchName,ppszDescription,pdwFlags)	\
    (This)->lpVtbl -> GetOptionInfoAt(This,iIndex,piOptionID,ppszSwitchName,ppszDescription,pdwFlags)

#define ICSCompilerConfig_GetOptionInfoAtEx(This,iIndex,piOptionID,ppszShortSwitchName,ppszLongSwitchName,ppszDescSwitchName,ppszDescription,pdwFlags)	\
    (This)->lpVtbl -> GetOptionInfoAtEx(This,iIndex,piOptionID,ppszShortSwitchName,ppszLongSwitchName,ppszDescSwitchName,ppszDescription,pdwFlags)

#define ICSCompilerConfig_ResetAllOptions(This)	\
    (This)->lpVtbl -> ResetAllOptions(This)

#define ICSCompilerConfig_SetOption(This,iOptionID,vtValue)	\
    (This)->lpVtbl -> SetOption(This,iOptionID,vtValue)

#define ICSCompilerConfig_GetOption(This,iOptionID,pvtValue)	\
    (This)->lpVtbl -> GetOption(This,iOptionID,pvtValue)

#define ICSCompilerConfig_CommitChanges(This,ppError)	\
    (This)->lpVtbl -> CommitChanges(This,ppError)

#define ICSCompilerConfig_GetCompiler(This,ppCompiler)	\
    (This)->lpVtbl -> GetCompiler(This,ppCompiler)

#define ICSCompilerConfig_GetWarnNumbers(This,piCount,pWarnIds)	\
    (This)->lpVtbl -> GetWarnNumbers(This,piCount,pWarnIds)

#define ICSCompilerConfig_GetWarnInfo(This,iWarnIndex,ppszWarnDescription)	\
    (This)->lpVtbl -> GetWarnInfo(This,iWarnIndex,ppszWarnDescription)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSCompilerConfig_GetOptionCount_Proxy( 
    ICSCompilerConfig * This,
    long *piCount);


void __RPC_STUB ICSCompilerConfig_GetOptionCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompilerConfig_GetOptionInfoAt_Proxy( 
    ICSCompilerConfig * This,
    long iIndex,
    long *piOptionID,
    LPCWSTR *ppszSwitchName,
    LPCWSTR *ppszDescription,
    DWORD *pdwFlags);


void __RPC_STUB ICSCompilerConfig_GetOptionInfoAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompilerConfig_GetOptionInfoAtEx_Proxy( 
    ICSCompilerConfig * This,
    long iIndex,
    long *piOptionID,
    LPCWSTR *ppszShortSwitchName,
    LPCWSTR *ppszLongSwitchName,
    LPCWSTR *ppszDescSwitchName,
    LPCWSTR *ppszDescription,
    DWORD *pdwFlags);


void __RPC_STUB ICSCompilerConfig_GetOptionInfoAtEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompilerConfig_ResetAllOptions_Proxy( 
    ICSCompilerConfig * This);


void __RPC_STUB ICSCompilerConfig_ResetAllOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompilerConfig_SetOption_Proxy( 
    ICSCompilerConfig * This,
    long iOptionID,
    VARIANT vtValue);


void __RPC_STUB ICSCompilerConfig_SetOption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompilerConfig_GetOption_Proxy( 
    ICSCompilerConfig * This,
    long iOptionID,
    VARIANT *pvtValue);


void __RPC_STUB ICSCompilerConfig_GetOption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompilerConfig_CommitChanges_Proxy( 
    ICSCompilerConfig * This,
    ICSError **ppError);


void __RPC_STUB ICSCompilerConfig_CommitChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompilerConfig_GetCompiler_Proxy( 
    ICSCompilerConfig * This,
    ICSCompiler **ppCompiler);


void __RPC_STUB ICSCompilerConfig_GetCompiler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompilerConfig_GetWarnNumbers_Proxy( 
    ICSCompilerConfig * This,
    long *piCount,
    const long **pWarnIds);


void __RPC_STUB ICSCompilerConfig_GetWarnNumbers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompilerConfig_GetWarnInfo_Proxy( 
    ICSCompilerConfig * This,
    long iWarnIndex,
    LPCWSTR *ppszWarnDescription);


void __RPC_STUB ICSCompilerConfig_GetWarnInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSCompilerConfig_INTERFACE_DEFINED__ */


#ifndef __ICSInputSet_INTERFACE_DEFINED__
#define __ICSInputSet_INTERFACE_DEFINED__

/* interface ICSInputSet */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSInputSet;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4D5D4C22-EE19-11d2-B556-00C04F68D4DB")
    ICSInputSet : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCompiler( 
            ICSCompiler **ppCompiler) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddSourceFile( 
            LPCWSTR pszFileName,
            FILETIME *pFT) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveSourceFile( 
            LPCWSTR pszFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveAllSourceFiles( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddResourceFile( 
            LPCWSTR pszFileName,
            LPCWSTR pszIdent,
            BOOL bEmbed,
            BOOL bVis) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveResourceFile( 
            LPCWSTR pszFileName,
            LPCWSTR pszIdent,
            BOOL bEmbed,
            BOOL bVis) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWin32Resource( 
            LPCWSTR pszFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetOutputFileName( 
            LPCWSTR pszFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetOutputFileType( 
            DWORD dwFileType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetImageBase( 
            DWORD dwImageBase) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMainClass( 
            LPCWSTR pszFQClassName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWin32Icon( 
            LPCWSTR pszIconFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFileAlignment( 
            DWORD dwAlign) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetImageBase2( 
            ULONGLONG ImageBase) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPDBFileName( 
            LPCWSTR pszFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWin32Resource( 
            /* [out] */ LPCWSTR *ppszFileName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSInputSetVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSInputSet * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSInputSet * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSInputSet * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCompiler )( 
            ICSInputSet * This,
            ICSCompiler **ppCompiler);
        
        HRESULT ( STDMETHODCALLTYPE *AddSourceFile )( 
            ICSInputSet * This,
            LPCWSTR pszFileName,
            FILETIME *pFT);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveSourceFile )( 
            ICSInputSet * This,
            LPCWSTR pszFileName);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveAllSourceFiles )( 
            ICSInputSet * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddResourceFile )( 
            ICSInputSet * This,
            LPCWSTR pszFileName,
            LPCWSTR pszIdent,
            BOOL bEmbed,
            BOOL bVis);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveResourceFile )( 
            ICSInputSet * This,
            LPCWSTR pszFileName,
            LPCWSTR pszIdent,
            BOOL bEmbed,
            BOOL bVis);
        
        HRESULT ( STDMETHODCALLTYPE *SetWin32Resource )( 
            ICSInputSet * This,
            LPCWSTR pszFileName);
        
        HRESULT ( STDMETHODCALLTYPE *SetOutputFileName )( 
            ICSInputSet * This,
            LPCWSTR pszFileName);
        
        HRESULT ( STDMETHODCALLTYPE *SetOutputFileType )( 
            ICSInputSet * This,
            DWORD dwFileType);
        
        HRESULT ( STDMETHODCALLTYPE *SetImageBase )( 
            ICSInputSet * This,
            DWORD dwImageBase);
        
        HRESULT ( STDMETHODCALLTYPE *SetMainClass )( 
            ICSInputSet * This,
            LPCWSTR pszFQClassName);
        
        HRESULT ( STDMETHODCALLTYPE *SetWin32Icon )( 
            ICSInputSet * This,
            LPCWSTR pszIconFileName);
        
        HRESULT ( STDMETHODCALLTYPE *SetFileAlignment )( 
            ICSInputSet * This,
            DWORD dwAlign);
        
        HRESULT ( STDMETHODCALLTYPE *SetImageBase2 )( 
            ICSInputSet * This,
            ULONGLONG ImageBase);
        
        HRESULT ( STDMETHODCALLTYPE *SetPDBFileName )( 
            ICSInputSet * This,
            LPCWSTR pszFileName);
        
        HRESULT ( STDMETHODCALLTYPE *GetWin32Resource )( 
            ICSInputSet * This,
            /* [out] */ LPCWSTR *ppszFileName);
        
        END_INTERFACE
    } ICSInputSetVtbl;

    interface ICSInputSet
    {
        CONST_VTBL struct ICSInputSetVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSInputSet_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSInputSet_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSInputSet_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSInputSet_GetCompiler(This,ppCompiler)	\
    (This)->lpVtbl -> GetCompiler(This,ppCompiler)

#define ICSInputSet_AddSourceFile(This,pszFileName,pFT)	\
    (This)->lpVtbl -> AddSourceFile(This,pszFileName,pFT)

#define ICSInputSet_RemoveSourceFile(This,pszFileName)	\
    (This)->lpVtbl -> RemoveSourceFile(This,pszFileName)

#define ICSInputSet_RemoveAllSourceFiles(This)	\
    (This)->lpVtbl -> RemoveAllSourceFiles(This)

#define ICSInputSet_AddResourceFile(This,pszFileName,pszIdent,bEmbed,bVis)	\
    (This)->lpVtbl -> AddResourceFile(This,pszFileName,pszIdent,bEmbed,bVis)

#define ICSInputSet_RemoveResourceFile(This,pszFileName,pszIdent,bEmbed,bVis)	\
    (This)->lpVtbl -> RemoveResourceFile(This,pszFileName,pszIdent,bEmbed,bVis)

#define ICSInputSet_SetWin32Resource(This,pszFileName)	\
    (This)->lpVtbl -> SetWin32Resource(This,pszFileName)

#define ICSInputSet_SetOutputFileName(This,pszFileName)	\
    (This)->lpVtbl -> SetOutputFileName(This,pszFileName)

#define ICSInputSet_SetOutputFileType(This,dwFileType)	\
    (This)->lpVtbl -> SetOutputFileType(This,dwFileType)

#define ICSInputSet_SetImageBase(This,dwImageBase)	\
    (This)->lpVtbl -> SetImageBase(This,dwImageBase)

#define ICSInputSet_SetMainClass(This,pszFQClassName)	\
    (This)->lpVtbl -> SetMainClass(This,pszFQClassName)

#define ICSInputSet_SetWin32Icon(This,pszIconFileName)	\
    (This)->lpVtbl -> SetWin32Icon(This,pszIconFileName)

#define ICSInputSet_SetFileAlignment(This,dwAlign)	\
    (This)->lpVtbl -> SetFileAlignment(This,dwAlign)

#define ICSInputSet_SetImageBase2(This,ImageBase)	\
    (This)->lpVtbl -> SetImageBase2(This,ImageBase)

#define ICSInputSet_SetPDBFileName(This,pszFileName)	\
    (This)->lpVtbl -> SetPDBFileName(This,pszFileName)

#define ICSInputSet_GetWin32Resource(This,ppszFileName)	\
    (This)->lpVtbl -> GetWin32Resource(This,ppszFileName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSInputSet_GetCompiler_Proxy( 
    ICSInputSet * This,
    ICSCompiler **ppCompiler);


void __RPC_STUB ICSInputSet_GetCompiler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInputSet_AddSourceFile_Proxy( 
    ICSInputSet * This,
    LPCWSTR pszFileName,
    FILETIME *pFT);


void __RPC_STUB ICSInputSet_AddSourceFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInputSet_RemoveSourceFile_Proxy( 
    ICSInputSet * This,
    LPCWSTR pszFileName);


void __RPC_STUB ICSInputSet_RemoveSourceFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInputSet_RemoveAllSourceFiles_Proxy( 
    ICSInputSet * This);


void __RPC_STUB ICSInputSet_RemoveAllSourceFiles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInputSet_AddResourceFile_Proxy( 
    ICSInputSet * This,
    LPCWSTR pszFileName,
    LPCWSTR pszIdent,
    BOOL bEmbed,
    BOOL bVis);


void __RPC_STUB ICSInputSet_AddResourceFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInputSet_RemoveResourceFile_Proxy( 
    ICSInputSet * This,
    LPCWSTR pszFileName,
    LPCWSTR pszIdent,
    BOOL bEmbed,
    BOOL bVis);


void __RPC_STUB ICSInputSet_RemoveResourceFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInputSet_SetWin32Resource_Proxy( 
    ICSInputSet * This,
    LPCWSTR pszFileName);


void __RPC_STUB ICSInputSet_SetWin32Resource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInputSet_SetOutputFileName_Proxy( 
    ICSInputSet * This,
    LPCWSTR pszFileName);


void __RPC_STUB ICSInputSet_SetOutputFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInputSet_SetOutputFileType_Proxy( 
    ICSInputSet * This,
    DWORD dwFileType);


void __RPC_STUB ICSInputSet_SetOutputFileType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInputSet_SetImageBase_Proxy( 
    ICSInputSet * This,
    DWORD dwImageBase);


void __RPC_STUB ICSInputSet_SetImageBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInputSet_SetMainClass_Proxy( 
    ICSInputSet * This,
    LPCWSTR pszFQClassName);


void __RPC_STUB ICSInputSet_SetMainClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInputSet_SetWin32Icon_Proxy( 
    ICSInputSet * This,
    LPCWSTR pszIconFileName);


void __RPC_STUB ICSInputSet_SetWin32Icon_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInputSet_SetFileAlignment_Proxy( 
    ICSInputSet * This,
    DWORD dwAlign);


void __RPC_STUB ICSInputSet_SetFileAlignment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInputSet_SetImageBase2_Proxy( 
    ICSInputSet * This,
    ULONGLONG ImageBase);


void __RPC_STUB ICSInputSet_SetImageBase2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInputSet_SetPDBFileName_Proxy( 
    ICSInputSet * This,
    LPCWSTR pszFileName);


void __RPC_STUB ICSInputSet_SetPDBFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSInputSet_GetWin32Resource_Proxy( 
    ICSInputSet * This,
    /* [out] */ LPCWSTR *ppszFileName);


void __RPC_STUB ICSInputSet_GetWin32Resource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSInputSet_INTERFACE_DEFINED__ */


#ifndef __ICSCompileProgress_INTERFACE_DEFINED__
#define __ICSCompileProgress_INTERFACE_DEFINED__

/* interface ICSCompileProgress */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSCompileProgress;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("93615958-1FA4-4d6a-AA14-CB3B9A6B08AC")
    ICSCompileProgress : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ReportProgress( 
            LPCWSTR pszTask,
            long iTasksLeft,
            long iTotalTasks) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSCompileProgressVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSCompileProgress * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSCompileProgress * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSCompileProgress * This);
        
        HRESULT ( STDMETHODCALLTYPE *ReportProgress )( 
            ICSCompileProgress * This,
            LPCWSTR pszTask,
            long iTasksLeft,
            long iTotalTasks);
        
        END_INTERFACE
    } ICSCompileProgressVtbl;

    interface ICSCompileProgress
    {
        CONST_VTBL struct ICSCompileProgressVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSCompileProgress_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSCompileProgress_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSCompileProgress_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSCompileProgress_ReportProgress(This,pszTask,iTasksLeft,iTotalTasks)	\
    (This)->lpVtbl -> ReportProgress(This,pszTask,iTasksLeft,iTotalTasks)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSCompileProgress_ReportProgress_Proxy( 
    ICSCompileProgress * This,
    LPCWSTR pszTask,
    long iTasksLeft,
    long iTotalTasks);


void __RPC_STUB ICSCompileProgress_ReportProgress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSCompileProgress_INTERFACE_DEFINED__ */


#ifndef __ICSEncProjectServices_INTERFACE_DEFINED__
#define __ICSEncProjectServices_INTERFACE_DEFINED__

/* interface ICSEncProjectServices */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSEncProjectServices;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("09bddb73-784a-4791-a962-2699bf0706f6")
    ICSEncProjectServices : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetNodeMappingState( 
            /* [in] */ ICSSourceData *pData,
            /* [in] */ MIDL_BASENODE *pNode,
            /* [out] */ ENC_NODE_STATE *pNodeState) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSEncProjectServicesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSEncProjectServices * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSEncProjectServices * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSEncProjectServices * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetNodeMappingState )( 
            ICSEncProjectServices * This,
            /* [in] */ ICSSourceData *pData,
            /* [in] */ MIDL_BASENODE *pNode,
            /* [out] */ ENC_NODE_STATE *pNodeState);
        
        END_INTERFACE
    } ICSEncProjectServicesVtbl;

    interface ICSEncProjectServices
    {
        CONST_VTBL struct ICSEncProjectServicesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSEncProjectServices_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSEncProjectServices_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSEncProjectServices_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSEncProjectServices_GetNodeMappingState(This,pData,pNode,pNodeState)	\
    (This)->lpVtbl -> GetNodeMappingState(This,pData,pNode,pNodeState)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSEncProjectServices_GetNodeMappingState_Proxy( 
    ICSEncProjectServices * This,
    /* [in] */ ICSSourceData *pData,
    /* [in] */ MIDL_BASENODE *pNode,
    /* [out] */ ENC_NODE_STATE *pNodeState);


void __RPC_STUB ICSEncProjectServices_GetNodeMappingState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSEncProjectServices_INTERFACE_DEFINED__ */


#ifndef __ICSCompiler_INTERFACE_DEFINED__
#define __ICSCompiler_INTERFACE_DEFINED__

/* interface ICSCompiler */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSCompiler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BD6EE4C6-3BE2-4df9-98D5-4BB2BC874BC1")
    ICSCompiler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateSourceModule( 
            ICSSourceText *pText,
            ICSSourceModule **ppModule) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNameTable( 
            ICSNameTable **ppNameTable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Shutdown( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetConfiguration( 
            ICSCompilerConfig **ppConfig) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddInputSet( 
            ICSInputSet **ppInputSet) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveInputSet( 
            ICSInputSet *pInputSet) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Compile( 
            ICSCompileProgress *pProgress) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BuildForEnc( 
            ICSCompileProgress *pProgress,
            ICSEncProjectServices *pEncService,
            IUnknown *punkPE) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOutputFileName( 
            LPCWSTR *ppszFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateParser( 
            ICSParser **ppParser) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSCompilerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSCompiler * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSCompiler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSCompiler * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateSourceModule )( 
            ICSCompiler * This,
            ICSSourceText *pText,
            ICSSourceModule **ppModule);
        
        HRESULT ( STDMETHODCALLTYPE *GetNameTable )( 
            ICSCompiler * This,
            ICSNameTable **ppNameTable);
        
        HRESULT ( STDMETHODCALLTYPE *Shutdown )( 
            ICSCompiler * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetConfiguration )( 
            ICSCompiler * This,
            ICSCompilerConfig **ppConfig);
        
        HRESULT ( STDMETHODCALLTYPE *AddInputSet )( 
            ICSCompiler * This,
            ICSInputSet **ppInputSet);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveInputSet )( 
            ICSCompiler * This,
            ICSInputSet *pInputSet);
        
        HRESULT ( STDMETHODCALLTYPE *Compile )( 
            ICSCompiler * This,
            ICSCompileProgress *pProgress);
        
        HRESULT ( STDMETHODCALLTYPE *BuildForEnc )( 
            ICSCompiler * This,
            ICSCompileProgress *pProgress,
            ICSEncProjectServices *pEncService,
            IUnknown *punkPE);
        
        HRESULT ( STDMETHODCALLTYPE *GetOutputFileName )( 
            ICSCompiler * This,
            LPCWSTR *ppszFileName);
        
        HRESULT ( STDMETHODCALLTYPE *CreateParser )( 
            ICSCompiler * This,
            ICSParser **ppParser);
        
        END_INTERFACE
    } ICSCompilerVtbl;

    interface ICSCompiler
    {
        CONST_VTBL struct ICSCompilerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSCompiler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSCompiler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSCompiler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSCompiler_CreateSourceModule(This,pText,ppModule)	\
    (This)->lpVtbl -> CreateSourceModule(This,pText,ppModule)

#define ICSCompiler_GetNameTable(This,ppNameTable)	\
    (This)->lpVtbl -> GetNameTable(This,ppNameTable)

#define ICSCompiler_Shutdown(This)	\
    (This)->lpVtbl -> Shutdown(This)

#define ICSCompiler_GetConfiguration(This,ppConfig)	\
    (This)->lpVtbl -> GetConfiguration(This,ppConfig)

#define ICSCompiler_AddInputSet(This,ppInputSet)	\
    (This)->lpVtbl -> AddInputSet(This,ppInputSet)

#define ICSCompiler_RemoveInputSet(This,pInputSet)	\
    (This)->lpVtbl -> RemoveInputSet(This,pInputSet)

#define ICSCompiler_Compile(This,pProgress)	\
    (This)->lpVtbl -> Compile(This,pProgress)

#define ICSCompiler_BuildForEnc(This,pProgress,pEncService,punkPE)	\
    (This)->lpVtbl -> BuildForEnc(This,pProgress,pEncService,punkPE)

#define ICSCompiler_GetOutputFileName(This,ppszFileName)	\
    (This)->lpVtbl -> GetOutputFileName(This,ppszFileName)

#define ICSCompiler_CreateParser(This,ppParser)	\
    (This)->lpVtbl -> CreateParser(This,ppParser)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSCompiler_CreateSourceModule_Proxy( 
    ICSCompiler * This,
    ICSSourceText *pText,
    ICSSourceModule **ppModule);


void __RPC_STUB ICSCompiler_CreateSourceModule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompiler_GetNameTable_Proxy( 
    ICSCompiler * This,
    ICSNameTable **ppNameTable);


void __RPC_STUB ICSCompiler_GetNameTable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompiler_Shutdown_Proxy( 
    ICSCompiler * This);


void __RPC_STUB ICSCompiler_Shutdown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompiler_GetConfiguration_Proxy( 
    ICSCompiler * This,
    ICSCompilerConfig **ppConfig);


void __RPC_STUB ICSCompiler_GetConfiguration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompiler_AddInputSet_Proxy( 
    ICSCompiler * This,
    ICSInputSet **ppInputSet);


void __RPC_STUB ICSCompiler_AddInputSet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompiler_RemoveInputSet_Proxy( 
    ICSCompiler * This,
    ICSInputSet *pInputSet);


void __RPC_STUB ICSCompiler_RemoveInputSet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompiler_Compile_Proxy( 
    ICSCompiler * This,
    ICSCompileProgress *pProgress);


void __RPC_STUB ICSCompiler_Compile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompiler_BuildForEnc_Proxy( 
    ICSCompiler * This,
    ICSCompileProgress *pProgress,
    ICSEncProjectServices *pEncService,
    IUnknown *punkPE);


void __RPC_STUB ICSCompiler_BuildForEnc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompiler_GetOutputFileName_Proxy( 
    ICSCompiler * This,
    LPCWSTR *ppszFileName);


void __RPC_STUB ICSCompiler_GetOutputFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompiler_CreateParser_Proxy( 
    ICSCompiler * This,
    ICSParser **ppParser);


void __RPC_STUB ICSCompiler_CreateParser_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSCompiler_INTERFACE_DEFINED__ */


#ifndef __ICSCompilerFactory_INTERFACE_DEFINED__
#define __ICSCompilerFactory_INTERFACE_DEFINED__

/* interface ICSCompilerFactory */
/* [object][uuid] */ 


EXTERN_C const IID IID_ICSCompilerFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("694DD9B6-B865-4c5b-AD85-86356E9C88DC")
    ICSCompilerFactory : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateNameTable( 
            ICSNameTable **ppTable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateCompiler( 
            DWORD dwFlags,
            ICSCompilerHost *pHost,
            ICSNameTable *pNameTable,
            ICSCompiler **ppCompiler) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateLexer( 
            ICSNameTable *pNameTable,
            CompatibilityMode eKeywordMode,
            ICSLexer **ppLexer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CheckVersion( 
            HINSTANCE hInstance,
            BSTR *pbstrError) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CheckCompilerVersion( 
            HINSTANCE hInstance,
            DWORD dwVersion,
            BSTR *pbstrError) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSCompilerFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSCompilerFactory * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSCompilerFactory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSCompilerFactory * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateNameTable )( 
            ICSCompilerFactory * This,
            ICSNameTable **ppTable);
        
        HRESULT ( STDMETHODCALLTYPE *CreateCompiler )( 
            ICSCompilerFactory * This,
            DWORD dwFlags,
            ICSCompilerHost *pHost,
            ICSNameTable *pNameTable,
            ICSCompiler **ppCompiler);
        
        HRESULT ( STDMETHODCALLTYPE *CreateLexer )( 
            ICSCompilerFactory * This,
            ICSNameTable *pNameTable,
            CompatibilityMode eKeywordMode,
            ICSLexer **ppLexer);
        
        HRESULT ( STDMETHODCALLTYPE *CheckVersion )( 
            ICSCompilerFactory * This,
            HINSTANCE hInstance,
            BSTR *pbstrError);
        
        HRESULT ( STDMETHODCALLTYPE *CheckCompilerVersion )( 
            ICSCompilerFactory * This,
            HINSTANCE hInstance,
            DWORD dwVersion,
            BSTR *pbstrError);
        
        END_INTERFACE
    } ICSCompilerFactoryVtbl;

    interface ICSCompilerFactory
    {
        CONST_VTBL struct ICSCompilerFactoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSCompilerFactory_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSCompilerFactory_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSCompilerFactory_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSCompilerFactory_CreateNameTable(This,ppTable)	\
    (This)->lpVtbl -> CreateNameTable(This,ppTable)

#define ICSCompilerFactory_CreateCompiler(This,dwFlags,pHost,pNameTable,ppCompiler)	\
    (This)->lpVtbl -> CreateCompiler(This,dwFlags,pHost,pNameTable,ppCompiler)

#define ICSCompilerFactory_CreateLexer(This,pNameTable,eKeywordMode,ppLexer)	\
    (This)->lpVtbl -> CreateLexer(This,pNameTable,eKeywordMode,ppLexer)

#define ICSCompilerFactory_CheckVersion(This,hInstance,pbstrError)	\
    (This)->lpVtbl -> CheckVersion(This,hInstance,pbstrError)

#define ICSCompilerFactory_CheckCompilerVersion(This,hInstance,dwVersion,pbstrError)	\
    (This)->lpVtbl -> CheckCompilerVersion(This,hInstance,dwVersion,pbstrError)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICSCompilerFactory_CreateNameTable_Proxy( 
    ICSCompilerFactory * This,
    ICSNameTable **ppTable);


void __RPC_STUB ICSCompilerFactory_CreateNameTable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompilerFactory_CreateCompiler_Proxy( 
    ICSCompilerFactory * This,
    DWORD dwFlags,
    ICSCompilerHost *pHost,
    ICSNameTable *pNameTable,
    ICSCompiler **ppCompiler);


void __RPC_STUB ICSCompilerFactory_CreateCompiler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompilerFactory_CreateLexer_Proxy( 
    ICSCompilerFactory * This,
    ICSNameTable *pNameTable,
    CompatibilityMode eKeywordMode,
    ICSLexer **ppLexer);


void __RPC_STUB ICSCompilerFactory_CreateLexer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompilerFactory_CheckVersion_Proxy( 
    ICSCompilerFactory * This,
    HINSTANCE hInstance,
    BSTR *pbstrError);


void __RPC_STUB ICSCompilerFactory_CheckVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICSCompilerFactory_CheckCompilerVersion_Proxy( 
    ICSCompilerFactory * This,
    HINSTANCE hInstance,
    DWORD dwVersion,
    BSTR *pbstrError);


void __RPC_STUB ICSCompilerFactory_CheckCompilerVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSCompilerFactory_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_CSCompilerFactory;

#ifdef __cplusplus

class DECLSPEC_UUID("E15850ED-31E6-40f4-AB51-E9FC4CCB48D3")
CSCompilerFactory;
#endif

EXTERN_C const CLSID CLSID_CSEEFactory;

#ifdef __cplusplus

class DECLSPEC_UUID("60F5556F-7EBC-4992-8E83-E9B49187FDE3")
CSEEFactory;
#endif

EXTERN_C const CLSID CLSID_CSEECasFactory;

#ifdef __cplusplus

class DECLSPEC_UUID("E7018011-AB0E-473a-8A77-5E8E428731B9")
CSEECasFactory;
#endif

EXTERN_C const CLSID CLSID_CSEEUIHost;

#ifdef __cplusplus

class DECLSPEC_UUID("0A9F2A56-3827-42dc-8A8F-B594879873AF")
CSEEUIHost;
#endif

EXTERN_C const CLSID CLSID_CSRegisteredNameTable;

#ifdef __cplusplus

class DECLSPEC_UUID("E7ECE2BD-58D1-434e-B34F-5BD2CDCA135A")
CSRegisteredNameTable;
#endif

EXTERN_C const CLSID CLSID_CSEELocalObject;

#ifdef __cplusplus

class DECLSPEC_UUID("5749A995-CBC2-4de4-86AF-C934D5E57BB9")
CSEELocalObject;
#endif
#endif /* __CSharp_LIBRARY_DEFINED__ */

/* interface __MIDL_itf_csiface_0134 */
/* [local] */ 

HRESULT STDMETHODCALLTYPE CreateCompilerFactory (ICSCompilerFactory **ppFactory);
HINSTANCE STDMETHODCALLTYPE GetMessageDll();
#define SID_SCSharpLangService IID_ICSCompilerFactory
EXTERN_C const IID IID_ICSharpProjCommonPropertyTable;
EXTERN_C const IID IID_ICSharpProjConfigPropertyTable;
EXTERN_C const IID IID_CCSharpTempPECompiler;


extern RPC_IF_HANDLE __MIDL_itf_csiface_0134_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_csiface_0134_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


