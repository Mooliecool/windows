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
#include "ml.h"
#include "mlgen.h"
#include "stubgen.h"
#include "custommarshalerinfo.h"

#ifndef _MLINFO_H_
#define _MLINFO_H_


#define NATIVE_TYPE_DEFAULT NATIVE_TYPE_MAX
#define VARIABLESIZE ((BYTE)(-1))


class DataImage;



enum 
{
    MarshalerFlag_ReturnsComByref              = 0x01,
    MarshalerFlag_ReturnsNativeByref           = 0x02,
    MarshalerFlag_InOnly                       = 0x04,
};


typedef enum
{
    HANDLEASNORMAL  = 0,
    OVERRIDDEN      = 1,
    DISALLOWED      = 2,
} MarshalerOverrideStatus;


enum MarshalFlags
{
    MARSHAL_FLAG_CLR_TO_NATIVE  = 0x01,
    MARSHAL_FLAG_IN             = 0x02,
    MARSHAL_FLAG_OUT            = 0x04,
    MARSHAL_FLAG_BYREF          = 0x08,
    MARSHAL_FLAG_HRESULT_SWAP   = 0x10,
};


struct OverrideProcArgs
{
    class MarshalInfo*  m_pMarshalInfo;
    
    union 
    {

    UINT8               m_arrayMarshalerID;
    UINT16              m_blittablenativesize;
    MethodTable*        m_pMT;

    struct
    {
        VARTYPE         m_vt;
        MethodTable*    m_pMT;
        UINT16          m_optionalbaseoffset; //for fast marshaling, offset of dataptr if known and less than 64k (0 otherwise)
    } na;

    struct
    {
        MethodTable* m_pMT;
        MethodDesc*  m_pCopyCtor;
        MethodDesc*  m_pDtor;
    } mm;

    struct
    {
        MethodDesc* m_pMD;
        mdToken     m_paramToken;
    } rcm;  // MARSHAL_TYPE_REFERENCECUSTOMMARSHALER

    };
};

typedef MarshalerOverrideStatus (*OVERRIDEPROC)(InteropStubLinker*    psl,
                                                InteropStubLinker*    pslPost,
                                                BOOL                  byref,
                                                BOOL                  fin,
                                                BOOL                  fout,
                                                BOOL                  comToNative,
                                                OverrideProcArgs*     pargs,
                                                UINT*                 pResID,
                                                UINT                  argidx);

typedef MarshalerOverrideStatus (*RETURNOVERRIDEPROC)(InteropStubLinker*  psl,
                                                      InteropStubLinker*  pslPost,
                                                      BOOL                comToNative,
                                                      BOOL                fThruBuffer,
                                                      OverrideProcArgs*   pargs,
                                                      UINT*               pResID);

//==========================================================================
// This structure contains the native type information for a given 
// parameter.
//==========================================================================
struct NativeTypeParamInfo
{
    NativeTypeParamInfo()
    : m_NativeType(NATIVE_TYPE_DEFAULT)
    , m_ArrayElementType(NATIVE_TYPE_DEFAULT)
    , m_SizeIsSpecified(FALSE)
    , m_CountParamIdx(0)
    , m_Multiplier(0)
    , m_Additive(1)
    , m_strCMMarshalerTypeName(NULL) 
    , m_cCMMarshalerTypeNameBytes(0)
    , m_strCMCookie(NULL)
    , m_cCMCookieStrBytes(0)
    {
        LEAF_CONTRACT;
    }   

    // The native type of the parameter.
    CorNativeType           m_NativeType;

    // for NT_ARRAY only
    CorNativeType           m_ArrayElementType; // The array element type.

    BOOL                    m_SizeIsSpecified;  // used to do some validation
    UINT16                  m_CountParamIdx;    // index of "sizeis" parameter
    UINT32                  m_Multiplier;       // multipler for "sizeis"
    UINT32                  m_Additive;         // additive for 'sizeis"

    // For NT_CUSTOMMARSHALER only.
    LPUTF8                  m_strCMMarshalerTypeName;
    DWORD                   m_cCMMarshalerTypeNameBytes;
    LPUTF8                  m_strCMCookie;
    DWORD                   m_cCMCookieStrBytes;

};

HRESULT CheckForCompressedData(PCCOR_SIGNATURE pvNativeTypeStart, PCCOR_SIGNATURE pvNativeType, ULONG cbNativeType);

BOOL ParseNativeTypeInfo(mdToken                    token,
                         IMDInternalImport*         pScope,
                         NativeTypeParamInfo*       pParamInfo);




class EEMarshalingData
{
public:
    EEMarshalingData(BaseDomain *pDomain, LoaderHeap *pHeap, CrstBase *pCrst);
    ~EEMarshalingData();

    // EEMarshalingData's are always allocated on the loader heap so we need to redefine
    // the new and delete operators to ensure this.
    void *operator new(size_t size, LoaderHeap *pHeap);
    void operator delete(void *pMem);

    // This method returns the custom marshaling helper associated with the name cookie pair. If the 
    // CM info has not been created yet for this pair then it will be created and returned.
    CustomMarshalerHelper *GetCustomMarshalerHelper(Assembly *pAssembly, TypeHandle hndManagedType, LPCUTF8 strMarshalerTypeName, DWORD cMarshalerTypeNameBytes, LPCUTF8 strCookie, DWORD cCookieStrBytes);

    // This method returns the custom marshaling info associated with shared CM helper.
    CustomMarshalerInfo *GetCustomMarshalerInfo(SharedCustomMarshalerHelper *pSharedCMHelper);


private:
    EECMHelperHashTable                 m_CMHelperHashtable;
    EEPtrHashTable                      m_SharedCMHelperToCMInfoMap;
    LoaderHeap*                         m_pHeap;
    BaseDomain*                         m_pDomain;
    CMINFOLIST                          m_pCMInfoList;
};

struct ItfMarshalInfo
{
    enum ItfMarshalFlags
    {
        ITF_MARSHAL_CLASS_IS_HINT = 1,
        ITF_MARSHAL_DISP_ITF = 2,
        ITF_MARSHAL_USE_BASIC_ITF = 4
    };
    
    TypeHandle      thClass;
    TypeHandle      thItf;
    DWORD           dwFlags;
};

class MarshalInfo
{
public:
    enum MarshalType
    {
#define DEFINE_MARSHALER_TYPE(mtype, mclass) mtype,
#include "mtypes.h"
        MARSHAL_TYPE_UNKNOWN
    };

    enum MarshalScenario
    {
        MARSHAL_SCENARIO_NDIRECT,
        MARSHAL_SCENARIO_FIELD
    };

private:


public:
    void *operator new(size_t size, void *pInPlace)
    {
        LEAF_CONTRACT;
        return pInPlace;
    }

    MarshalInfo()
    {
        LEAF_CONTRACT;
    }


    MarshalInfo(Module* pModule,
                SigPointer sig,
                mdToken token,
                MarshalScenario ms,
                BYTE nlType,
                BYTE nlFlags,
                BOOL isParam,
                UINT paramidx,    // parameter # for use in error messages (ignored if not parameter)
                BOOL BestFit,
                BOOL ThrowOnUnmappableChar,
                BOOL fEmitsIL,
                MethodDesc* pMD = NULL,
                BOOL fUseCustomMarshal = TRUE
#ifdef _DEBUG
                ,
                LPCUTF8 pDebugName = NULL,
                LPCUTF8 pDebugClassName = NULL,
                LPCUTF8 pDebugNameSpace = NULL,
                UINT    argidx = 0  // 0 for return value, -1 for field
#endif

                );

    VOID EmitOrThrowInteropParamException(InteropStubLinker* psl, UINT resID, UINT paramIdx);
    VOID GetInteropParamException(OBJECTREF* pThrowable);

    // These methods retrieve the information for different element types.
    HRESULT HandleArrayElemType(NativeTypeParamInfo *pParamInfo, 
                                UINT16 optbaseoffset, 
                                TypeHandle elemTypeHnd, 
                                int iRank, 
                                BOOL fNoLowerBounds, 
                                BOOL isParam, 
                                Assembly *pAssembly);

#define GENARGML_STACK_ARGUMENT (FramedMethodFrame::GetOffsetOfArgumentRegisters() + NUM_ARGUMENT_REGISTERS * sizeof(void*))

    void GenerateArgumentML(InteropStubLinker* psl,
                            InteropStubLinker* pslPost,
                            int argOffset,
                            BOOL comToNative);
    
    void GenerateReturnML(InteropStubLinker* psl,
                          InteropStubLinker* pslPost,
                          BOOL comToNative,
                          BOOL retval);
    
    void GenerateSetterML(InteropStubLinker* psl);
    
    void GenerateGetterML(InteropStubLinker* psl);
    
    UINT16 EmitCreateOpcode(InteropStubLinker* psl);

    UINT16 GetComArgSize()
    {
        LEAF_CONTRACT;
        return m_comArgSize;
    }
    
    UINT16 GetNativeArgSize()
    {
        LEAF_CONTRACT;
        return m_nativeArgSize;
    }

    UINT16 GetNativeSize()
    {
        WRAPPER_CONTRACT;
        return GetNativeSize(m_type, m_ms);
    }
    
    MarshalType GetMarshalType()
    {
        LEAF_CONTRACT;
        return m_type;
    }

    BYTE    GetBestFitMapping()
    {
        LEAF_CONTRACT;
        return ((m_BestFit == 0) ? 0 : 1);
    }
    
    BYTE    GetThrowOnUnmappableChar()
    {
        LEAF_CONTRACT;
        return ((m_ThrowOnUnmappableChar == 0) ? 0 : 1);
    }

    BOOL   IsFpuReturn()
    {
        LEAF_CONTRACT;

        
        return m_type == MARSHAL_TYPE_FLOAT || m_type == MARSHAL_TYPE_DOUBLE;
    }


    BOOL   IsIn()
    {
        LEAF_CONTRACT;
        return m_in;
    }

    BOOL   IsOut()
    {
        LEAF_CONTRACT;
        return m_out;
    }

    BOOL   IsByRef()
    {
        LEAF_CONTRACT;
        return m_byref;
    }

    Module* GetModule()
    {
        LEAF_CONTRACT;
        return m_pModule;
    }

    int GetArrayRank()
    {
        LEAF_CONTRACT;
        return m_iArrayRank;
    }

    BOOL GetNoLowerBounds()
    {
        LEAF_CONTRACT;
        return m_nolowerbounds;
    }

    void GetMops(ML_CREATE_MARSHALER_CARRAY_OPERANDS* pMopsOut)
    {
        pMopsOut->methodTable = m_hndArrayElemType.AsMethodTable();
        pMopsOut->elementType = m_arrayElementType;
        pMopsOut->countParamIdx = m_countParamIdx;
        pMopsOut->countSize   = 0; //placeholder for later patching (if left unpatched, this value signals marshaler to use managed size of array)
        pMopsOut->multiplier  = m_multiplier;
        pMopsOut->additive    = m_additive;
        pMopsOut->bestfitmapping = m_BestFit;
        pMopsOut->throwonunmappablechar = m_ThrowOnUnmappableChar;
    }



    void EmitMarshalOpcode(InteropStubLinker* psl, BOOL comToNative, UINT16 local);
    void EmitUnmarshalOpcode(InteropStubLinker* psl, BOOL comToNative, UINT16 localMarshaler);
    void EmitPreReturnOpcode(InteropStubLinker* psl, BOOL comToNative, UINT16 local, BOOL retval);
    void EmitReturnOpcode(InteropStubLinker* pslPre, InteropStubLinker* pslPost, BOOL comToNative, UINT16 local, BOOL retval);

    void GetItfMarshalInfo(ItfMarshalInfo* pInfo);

    // Helper functions used to map the specified type to its interface marshalling info.
    static void GetItfMarshalInfo(TypeHandle th, BOOL fDispItf, ItfMarshalInfo *pInfo);
    static HRESULT TryGetItfMarshalInfo(TypeHandle th, BOOL fDispItf, ItfMarshalInfo *pInfo);

    VOID MarshalTypeToString(SString& strMarshalType, BOOL fSizeIsSpecified);
    static VOID VarTypeToString(VARTYPE vt, SString& strVarType);

private:

    UINT16                      GetComSize(MarshalType mtype, MarshalScenario ms);
    UINT16                      GetNativeSize(MarshalType mtype, MarshalScenario ms);

    static UINT16               GetLocalSize(MarshalType mtype, BOOL useILMarshalers);
    static BYTE                 GetFlags(MarshalType mtype, BOOL useILMarshalers);
    static BYTE                 GetUnmarshalFlagsN2C(MarshalType mtype, BOOL useILMarshalers);
    static BYTE                 GetUnmarshalFlagsC2N(MarshalType mtype, BOOL useILMarshalers);
    static OVERRIDEPROC         GetArgumentOverrideProc(MarshalType mtype, BOOL useILMarshalers);
    static RETURNOVERRIDEPROC   GetReturnOverrideProc(MarshalType mtype, BOOL useILMarshalers);
    
#ifdef _DEBUG
    VOID DumpMarshalInfo(Module* pModule, SigPointer sig, mdToken token, MarshalScenario ms, BYTE nlType, BYTE nlFlags);
#endif

private:
    MarshalType     m_type;
    BOOL            m_byref;
    BOOL            m_in;
    BOOL            m_out;
    MethodTable*    m_pMT;  // Used if this is a true value type
    TypeHandle      m_hndArrayElemType;
    VARTYPE         m_arrayElementType;
    int             m_iArrayRank;
    BOOL            m_nolowerbounds;  // if managed type is SZARRAY, don't allow lower bounds

    // for NT_ARRAY only
    UINT16          m_countParamIdx;  // index of "sizeis" parameter
    UINT32          m_multiplier;     // multipler for "sizeis"
    UINT32          m_additive;       // additive for 'sizeis"

    UINT16          m_nativeArgSize;
    UINT16          m_comArgSize;

    MarshalScenario m_ms;
    BOOL            m_fAnsi;
    BOOL            m_fDispItf;
    BOOL            m_fErrorNativeType;

    // Information used by NT_CUSTOMMARSHALER.
    CustomMarshalerHelper* m_pCMHelper;
    VARTYPE         m_CMVt;

    OverrideProcArgs  m_args;

    UINT            m_paramidx;
    UINT            m_resID;     // resource ID for error message (if any)
    BOOL            m_BestFit;
    BOOL            m_ThrowOnUnmappableChar;

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
    BOOL            m_fComArgImplicitByref;
    BOOL            m_fNativeArgImplicitByref;
#endif
    
#if defined(_DEBUG)
    LPCUTF8         m_strDebugMethName;
    LPCUTF8         m_strDebugClassName;
    LPCUTF8         m_strDebugNameSpace;
    UINT            m_iArg;  // 0 for return value, -1 for field
#endif

    Module*         m_pModule;


};



//
// Flags used to control the behavior of the ArrayMarshalInfo class.
//

enum ArrayMarshalInfoFlags
{
    amiRuntime                                  = 0x0001,
    amiExport32Bit                              = 0x0002,
    amiExport64Bit                              = 0x0004,
    amiIsPtr                                    = 0x0008,
    amiSafeArraySubTypeExplicitlySpecified      = 0x0010
};

#define IsAMIRuntime(flags) (flags & amiRuntime)
#define IsAMIExport(flags) (flags & (amiExport32Bit | amiExport64Bit))
#define IsAMIExport32Bit(flags) (flags & amiExport32Bit)
#define IsAMIExport64Bit(flags) (flags & amiExport64Bit)
#define IsAMIPtr(flags) (flags & amiIsPtr)
#define IsAMISafeArraySubTypeExplicitlySpecified(flags) (flags & amiSafeArraySubTypeExplicitlySpecified)
//
// Helper classes to determine the marshalling information for arrays.
//

class ArrayMarshalInfo
{
public:
    ArrayMarshalInfo(ArrayMarshalInfoFlags flags)
    : m_vtElement(VT_EMPTY) 
    , m_errorResourceId(0) 
    , m_flags(flags)
    {   
        WRAPPER_CONTRACT;
    }

    void InitForNativeArray(MarshalInfo::MarshalScenario ms, TypeHandle elemTypeHnd, CorNativeType elementNativeType, BOOL isAnsi);
    void InitForFixedArray(TypeHandle elemTypeHnd, CorNativeType elementNativeType, BOOL isAnsi);

    
    TypeHandle GetElementTypeHandle()
    {
        LEAF_CONTRACT;
        return m_thElement;    
    }

    BOOL IsPtr()
    {
        LEAF_CONTRACT;
        return IsAMIPtr(m_flags);
    }

    VARTYPE GetElementVT()
    {
        LEAF_CONTRACT;
        return m_vtElement;
    }

    BOOL IsValid()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;        
        
        return m_vtElement != VT_EMPTY;
    }

    BOOL IsSafeArraySubTypeExplicitlySpecified()
    {
        LEAF_CONTRACT;
        
        return IsAMISafeArraySubTypeExplicitlySpecified(m_flags);
    }
    
    DWORD GetErrorResourceId()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(!IsValid());
        }
        CONTRACTL_END;        
    
        return m_errorResourceId;
    }

protected:    
    // Helper function that does the actual work to figure out the element type handle and var type.    
    void InitElementInfo(CorNativeType arrayNativeType, MarshalInfo::MarshalScenario ms, TypeHandle elemTypeHnd, CorNativeType elementNativeType, BOOL isAnsi);

    VARTYPE GetPointerSize()
    {
        // If we are exporting, use the pointer size specified via the flags, otherwise use
        // the current size of a pointer.
        if (IsAMIExport32Bit(m_flags))
            return 4;
        else if (IsAMIExport64Bit(m_flags))
            return 8;
        else 
            return sizeof(LPVOID);
    }

protected:
    TypeHandle m_thElement;
    TypeHandle m_thInterfaceArrayElementClass;
    VARTYPE m_vtElement;
    DWORD m_errorResourceId;
    ArrayMarshalInfoFlags m_flags;
};


//===================================================================================
// Throws an exception indicating a param has invalid element type / native type
// information.
//===================================================================================
VOID ThrowInteropParamException(UINT resID, UINT paramIdx);

//===================================================================================
// Post-patches ML stubs for the sizeis feature.
//===================================================================================
VOID PatchMLStubForSizeIs(BYTE *pMLCode, UINT32 numArgs, MarshalInfo *pMLInfo);


VOID CollateParamTokens(IMDInternalImport *pInternalImport, mdMethodDef md, ULONG numargs, mdParamDef *aParams);
bool IsUnsupportedValueTypeReturn(MetaSig& msig);

#endif // _MLINFO_H_


