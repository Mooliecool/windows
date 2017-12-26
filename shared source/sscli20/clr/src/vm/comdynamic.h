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
////////////////////////////////////////////////////////////////////////////////
// COMDynamic.h
//  This module defines the native methods that are used for Dynamic IL generation  
//
// Date: November 1998
////////////////////////////////////////////////////////////////////////////////
#ifndef _COMDYNAMIC_H_
#define _COMDYNAMIC_H_

#include "iceefilegen.h"
#include "dbginterface.h"
#include "comvariant.h"

typedef enum PEFileKinds {
    Dll = 0x1,
    ConsoleApplication = 0x2,
    WindowApplication = 0x3,
} PEFileKinds;

// COMDynamicWrite
// This class defines all the methods that implement the dynamic IL creation process
//  inside reflection.  
class COMDynamicWrite
{
private:

    static void UpdateMethodRVAs(IMetaDataEmit*, IMetaDataImport*, ICeeFileGen *, HCEEFILE, mdTypeDef td, HCEESECTION sdataSection);

public:

    // the module that it pass in is already the reflection module
    static ReflectionModule* GetReflectionModule(Module* pModule) 
    {    
        WRAPPER_CONTRACT;   
        return pModule->GetReflectionModule();
    }   

    // CWCreateClass    
    // ClassWriter.InternalDefineClass -- This function will create the class's metadata definition  
    static FCDECL9(UINT32, CWCreateClass, Object* refThisUNSAFE, StringObject* strFullNameUNSAFE, 
        UINT32 parent, I4Array* interfacesUNSAFE, UINT32 attr, 
        ReflectModuleBaseObject* moduleUNSAFE, GUID guid, INT32 tkEnclosingType, INT32 tkTypeDef);
    
    static FCDECL8(UINT32, CWCreateGenParam, 
        Object* refThisUNSAFE, StringObject* strFullNameUNSAFE, UINT32 parent, UINT32 position, UINT32 attributes, I4Array* constraintsUNSAFE, 
        ReflectModuleBaseObject* moduleUNSAFE, INT32 tkTypeDef);

    // CWSetParentType    
    // ClassWriter.InternalSetParentType -- This function will reset the parent class in metadata
    static FCDECL3(void, CWSetParentType, UINT32 tdType, UINT32 tkParent, ReflectModuleBaseObject* moduleUNSAFE);
    // CWAddInterfaceImpl    
    // ClassWriter.InternalAddInterfaceImpl -- This function will add another interface impl
    static FCDECL3(void, CWAddInterfaceImpl, UINT32 tdType, UINT32 tkInterface, ReflectModuleBaseObject* moduleUNSAFE);
    // CWCreateMethod   
    // ClassWriter.InternalDefineMethod -- This function will create a method within the class  
    static FCDECL6 (UINT32, CWCreateMethod, UINT32 handle, StringObject* nameUNSAFE, U1Array* signatureUNSAFE, UINT32 sigLength, UINT32 attributes, ReflectModuleBaseObject* moduleUNSAFE);
    static FCDECL4 (UINT32, CWCreateMethodSpec, UINT32 tkMethod, U1Array* signatureUNSAFE, UINT32 sigLength, ReflectModuleBaseObject* moduleUNSAFE);
    // CWSetMethodIL    
    // ClassWriter.InternalSetMethodIL -- This function will create a method within the class   
    static FCDECL11(void, CWSetMethodIL,
                                UINT32 handle,
                                CLR_BOOL isInitLocal,
                                U1Array* bodyUNSAFE,
                                U1Array* localSigUNSAFE,
                                UINT32 sigLength,
                                UINT32 maxStackSize,
                                UINT32 numExceptions,
                                PTRArray* exceptionsUNSAFE,
                                I4Array* tokenFixupsUNSAFE,
                                I4Array* rvaFixupsUNSAFE,
                                ReflectModuleBaseObject* moduleUNSAFE);
    // CWTermCreateClass    
    // ClassWriter.TermCreateClass --   
    static FCDECL3(Object*, CWTermCreateClass, Object* refThisUNSAFE, UINT32 handle, ReflectModuleBaseObject* moduleUNSAFE);
    static FCDECL6(mdFieldDef, CWCreateField, UINT32 handle, StringObject* nameUNSAFE, U1Array* signatureUNSAFE, UINT32 sigLength, UINT32 attr, ReflectModuleBaseObject* moduleUNSAFE);
    static FCDECL3(void, PreSavePEFile, Object* refThisUNSAFE, INT32 portableExecutableKind, INT32 imageFileMachine);
    static FCDECL5(void, SavePEFile, Object* refThisUNSAFE, StringObject* peNameUNSAFE, UINT32 entryPoint, UINT32 fileKind, CLR_BOOL isManifestFile);

    // not an ecall!
    static HRESULT COMDynamicWrite::EmitDebugInfoBegin(
        Module *pModule,
        ICeeFileGen *pCeeFileGen,
        HCEEFILE ceeFile,
        HCEESECTION pILSection,
        const WCHAR *filename,
        ISymUnmanagedWriter *pWriter);

    // not an ecall!
    static HRESULT COMDynamicWrite::EmitDebugInfoEnd(
        Module *pModule,
        ICeeFileGen *pCeeFileGen,
        HCEEFILE ceeFile,
        HCEESECTION pILSection,
        const WCHAR *filename,
        ISymUnmanagedWriter *pWriter);

    static FCDECL2(void, SetResourceCounts, Object* refThisUNSAFE, UINT32 iCount);
    static FCDECL8(void, AddResource, Object* refThisUNSAFE, StringObject* strNameUNSAFE, U1Array* byteResUNSAFE, UINT32 iByteCount, UINT32 tkFile, UINT32 iAttribute, INT32 portableExecutableKind, INT32 imageFileMachine);
    static FCDECL6(void, InternalSetPInvokeData, ReflectModuleBaseObject* moduleUNSAFE, StringObject* dllNameUNSAFE, StringObject* functionNameUNSAFE, UINT32 token, UINT32 linkType, UINT32 linkFlags);
    // DefineProperty's argument
    static FCDECL8(UINT32, CWDefineProperty, ReflectModuleBaseObject* moduleUNSAFE, UINT32 handle, StringObject* nameUNSAFE, UINT32 attr, U1Array* signatureUNSAFE, UINT32 sigLength, UINT32 tkNotifyChanging, UINT32 tkNotifyChanged);
    // DefineEvent's argument
    static FCDECL5(UINT32, CWDefineEvent, ReflectModuleBaseObject* moduleUNSAFE, UINT32 handle, StringObject* nameUNSAFE, UINT32 attr, UINT32 eventtype);
    // functions to set Setter, Getter, Reset, TestDefault, and other methods
    static FCDECL4(void, CWDefineMethodSemantics, ReflectModuleBaseObject* moduleUNSAFE, UINT32 association, UINT32 attr, UINT32 method);
    // functions to set method's implementation flag
    static FCDECL3(void, CWSetMethodImpl, ReflectModuleBaseObject* moduleUNSAFE, UINT32 tkMethod, UINT32 attr);
    // functions to create MethodImpl record
    static FCDECL4(void, CWDefineMethodImpl, ReflectModuleBaseObject* moduleUNSAFE, UINT32 tkType, UINT32 tkBody, UINT32 tkDecl);
    // GetTokenFromSig's argument
    static FCDECL3(int, CWGetTokenFromSig, ReflectModuleBaseObject* moduleUNSAFE, U1Array* signatureUNSAFE, UINT32 sigLength);
    // Set Field offset
    static FCDECL3(void, CWSetFieldLayoutOffset, ReflectModuleBaseObject* moduleUNSAFE, UINT32 tkField, UINT32 iOffset);
    // Set classlayout info
    static FCDECL4(void, CWSetClassLayout, ReflectModuleBaseObject* moduleUNSAFE, UINT32 handle, UINT32 iPackSize, UINT32 iTotalSize);
    // Set a custom attribute
    static FCDECL6(void, CWInternalCreateCustomAttribute, UINT32 token, UINT32 conTok, U1Array* blobUNSAFE, ReflectModuleBaseObject* moduleUNSAFE, CLR_BOOL toDisk, CLR_BOOL updateCompilerFlags);
    // functions to set ParamInfo
    static FCDECL5(int, CWSetParamInfo, ReflectModuleBaseObject* moduleUNSAFE, UINT32 tkMethod, UINT32 iSequence, UINT32 iAttributes, StringObject* strParamNameUNSAFE);
    // functions to set FieldMarshal
    static FCDECL4(void, CWSetMarshal, ReflectModuleBaseObject* moduleUNSAFE, UINT32 tk, U1Array* ubMarshalUNSAFE, UINT32 cbMarshal);
    // functions to set default value
    static FCDECL3(void, CWSetConstantValue, ReflectModuleBaseObject* moduleUNSAFE, UINT32 tk, VariantData* pvarValue);
    // functions to add declarative security
    static FCDECL4(void, CWAddDeclarativeSecurity, ReflectModuleBaseObject* moduleUNSAFE, UINT32 tk, DWORD action, U1Array* blobUNSAFE);
};



//*********************************************************************
//
// This CSymMapToken class implemented the IMapToken. It is used in catching
// token remap information from Merge and send the notifcation to CeeFileGen
// and SymbolWriter
//
//*********************************************************************
class CSymMapToken : public IMapToken
{
public:
    STDMETHODIMP QueryInterface(REFIID riid, PVOID *pp);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
    STDMETHODIMP Map(mdToken tkImp, mdToken tkEmit);
    CSymMapToken(ISymUnmanagedWriter *pWriter, IMapToken *pMapToken);
    ~CSymMapToken();
private:
    LONG        m_cRef;
    ISymUnmanagedWriter *m_pWriter;
    IMapToken   *m_pMapToken;
};

#endif  // _COMDYNAMIC_H_   
