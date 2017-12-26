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
// ---------------------------------------------------------------------------
// typeparse.h
// ---------------------------------------------------------------------------

#ifndef TYPEPARSE_H
#define TYPEPARSE_H

#include "common.h"
#include "class.h"
#include "typehandle.h"

#define TYPE_NAME_RESERVED_CHAR L",[]&*+\\"

DomainAssembly* LoadAssemblyFromPartialNameHack(SString* psszAssemblySpec, BOOL fCropPublicKey = FALSE);

class TypeNameFactory : public ITypeNameFactory
{    
public:
    static HRESULT CreateObject(REFIID riid, void **ppUnk);
    
public:
    virtual HRESULT __stdcall QueryInterface(REFIID riid, void **ppUnk);
    virtual ULONG __stdcall AddRef() { LEAF_CONTRACT; m_count++; return m_count; }
    virtual ULONG __stdcall Release() { LEAF_CONTRACT; m_count--; ULONG count = m_count; if (count == 0) delete this; return count; }

public:
    virtual HRESULT __stdcall ParseTypeName(LPCWSTR szName, DWORD* pError, ITypeName** ppTypeName);
    virtual HRESULT __stdcall GetTypeNameBuilder(ITypeNameBuilder** ppTypeBuilder);

public:
    TypeNameFactory() : m_count(0) { LEAF_CONTRACT; }
        
private:
    DWORD m_count;
};

class TypeName : public ITypeName
{
private:
    template<typename PRODUCT>
    class Factory
    {
    public:
        const static DWORD MAX_PRODUCT = 10;
        
    public:
        Factory() : m_cProduct(0), m_next(NULL) { LEAF_CONTRACT; }
        ~Factory()
          {     
               CONTRACTL{ 
                                     NOTHROW; 
            } CONTRACTL_END; 

            if (m_next) 
                delete m_next; 
          } 

        PRODUCT* Create()
            { WRAPPER_CONTRACT; if (m_cProduct == (INT32)MAX_PRODUCT) return GetNext()->Create(); return &m_product[m_cProduct++]; }

    private:
        Factory* GetNext() { if (!m_next) m_next = new Factory<PRODUCT>(); return m_next; }   

    private:
        PRODUCT m_product[MAX_PRODUCT];
        INT32 m_cProduct;
        Factory* m_next;
    };
    friend class TypeName::Factory<TypeName>;
    friend class TypeNameBuilder;

private:
    class TypeNameParser
    {
        TypeNameParser(LPCWSTR szTypeName, TypeName* pTypeName, DWORD* pError) 
        { 
            WRAPPER_CONTRACT; 

            m_currentToken = TypeNameEmpty;
            m_nextToken = TypeNameEmpty;

            *pError = (DWORD)-1;
            m_pTypeName = pTypeName;
            m_sszTypeName.Set(szTypeName);
            m_currentItr = m_itr = m_sszTypeName.Begin(); 

            if (!START())
                *pError = (DWORD)(m_currentItr - m_sszTypeName.Begin()) - 1;
        }

    private:
        friend class TypeName;
        
    private:
        typedef enum {
            //
            // TOKENS
            //
            TypeNameEmpty               = 0x8000,
            TypeNameIdentifier          = 0x0001,
            TypeNamePostIdentifier      = 0x0002,
            TypeNameOpenSqBracket       = 0x0004,
            TypeNameCloseSqBracket      = 0x0008,
            TypeNameComma               = 0x0010,
            TypeNamePlus                = 0x0020,
            TypeNameAstrix              = 0x0040,
            TypeNameAmperstand          = 0x0080,
            TypeNameBackSlash           = 0x0100,
            TypeNameEnd                 = 0x4000,

            //
            // 1 TOKEN LOOK AHEAD 
            //
            TypeNameNAME                = TypeNameIdentifier,
            TypeNameNESTNAME            = TypeNameIdentifier,
            TypeNameASSEMSPEC           = TypeNameIdentifier, 
            TypeNameGENPARAM            = TypeNameOpenSqBracket | TypeNameEmpty,
            TypeNameFULLNAME            = TypeNameNAME,
            TypeNameAQN                 = TypeNameFULLNAME | TypeNameEnd,
            TypeNameASSEMBLYSPEC        = TypeNameIdentifier,
            TypeNameGENARG              = TypeNameOpenSqBracket | TypeNameFULLNAME,
            TypeNameGENARGS             = TypeNameGENARG,
            TypeNameEAQN                = TypeNameIdentifier,
            TypeNameEASSEMSPEC          = TypeNameIdentifier,
            TypeNameARRAY               = TypeNameOpenSqBracket,
            TypeNameQUALIFIER           = TypeNameAmperstand | TypeNameAstrix | TypeNameARRAY | TypeNameEmpty,
            TypeNameRANK                = TypeNameComma | TypeNameEmpty,            
        } TypeNameTokens;

        typedef enum {
            TypeNameNone                = 0x00,
            TypeNameId                  = 0x01,
            TypeNameFusionName          = 0x02,
            TypeNameEmbeddedFusionName  = 0x03,
        } TypeNameIdentifiers;

    //
    // LEXIFIER 
    //
    private:
        TypeNameTokens LexAToken();
        BOOL GetIdentifier(SString* sszId, TypeNameIdentifiers identiferType);
        void NextToken()  { WRAPPER_CONTRACT; m_currentToken = m_nextToken; m_currentItr = m_itr; m_nextToken = LexAToken(); }
        BOOL NextTokenIs(TypeNameTokens token) { LEAF_CONTRACT; return !!(m_nextToken & token); }
        BOOL TokenIs(TypeNameTokens token) { LEAF_CONTRACT; return !!(m_currentToken & token); }
        BOOL TokenIs(int token) { LEAF_CONTRACT; return TokenIs((TypeNameTokens)token); }
        
    //
    // PRODUCTIONS
    //
    private: 
        BOOL START();
        
        BOOL AQN();
        // /* empty */
        // FULLNAME ',' ASSEMSPEC
        // FULLNAME
      
        BOOL ASSEMSPEC();
        // fusionName
        
        BOOL FULLNAME();
        // NAME GENPARAMS QUALIFIER
        
        BOOL GENPARAMS();
        // *empty*
        // '[' GENARGS ']'        
        
        BOOL GENARGS();
        // GENARG
        // GENARG ',' GENARGS
                
        BOOL GENARG();
        // '[' EAQN ']'
        // FULLNAME
        
        BOOL EAQN();
        // FULLNAME ',' EASSEMSPEC
        // FULLNAME
        
        BOOL EASSEMSPEC();
        // embededFusionName
        
        BOOL QUALIFIER();
        // *empty*
        // '&'
        // *' QUALIFIER
        // ARRAY QUALIFIER
        
        BOOL ARRAY();
        // '[' RANK ']'
        // '[' '*' ']'
        
        BOOL RANK(DWORD* pdwRank);
        // *empty*
        // ',' RANK
        
        BOOL NAME();
        // id
        // id '+' NESTNAME
        
        BOOL NESTNAME();
        // id
        // id '+' NESTNAME
  
    public:
        void MakeRotorHappy() { WRAPPER_CONTRACT; }
    
    private:
        TypeName* m_pTypeName;
        StackSString m_sszTypeName;
        SString::Iterator m_itr;
        SString::Iterator m_currentItr;
        TypeNameTokens m_currentToken;
        TypeNameTokens m_nextToken;
    };
    friend class TypeName::TypeNameParser;
    
public:
    virtual HRESULT __stdcall QueryInterface(REFIID riid, void **ppUnk);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();

public:
    virtual HRESULT __stdcall GetNameCount(DWORD* pCount);
    virtual HRESULT __stdcall GetNames(DWORD count, BSTR* rgbszNames, DWORD* pFetched);
    virtual HRESULT __stdcall GetTypeArgumentCount(DWORD* pCount);
    virtual HRESULT __stdcall GetTypeArguments(DWORD count, ITypeName** rgpArguments, DWORD* pFetched);
    virtual HRESULT __stdcall GetModifierLength(DWORD* pCount);
    virtual HRESULT __stdcall GetModifiers(DWORD count, DWORD* rgModifiers, DWORD* pFetched);
    virtual HRESULT __stdcall GetAssemblyName(BSTR* rgbszAssemblyNames);
    
public:
    TypeName(LPCWSTR szTypeName, DWORD* pError) : m_bIsGenericArgument(FALSE), m_count(0) { WRAPPER_CONTRACT; TypeNameParser parser(szTypeName, this, pError); parser.MakeRotorHappy(); }
    ~TypeName();
    
public:
    //-------------------------------------------------------------------------------------------
    // Retrieves a type from an assembly. It requires the caller to know which assembly
    // the type is in.
    //-------------------------------------------------------------------------------------------
    static TypeHandle GetTypeFromAssembly(LPCWSTR szTypeName, Assembly *pAssembly);

    //-------------------------------------------------------------------------------------------
    // Retrieves a type. Will assert if the name is not fully qualified.
    //-------------------------------------------------------------------------------------------
    static TypeHandle GetTypeFromAsmQualifiedName(LPCWSTR szFullyQualifiedName, BOOL bForIntrospection);


    //-------------------------------------------------------------------------------------------
    // This version is used for resolving types named in custom attributes such as those used
    // for interop. Thus, it follows a well-known multistage set of rules for determining which
    // assembly the type is in. It will also enforce that the requesting assembly has access
    // rights to the type being loaded.
    //
    // The search logic is:
    //
    //    if szTypeName is ASM-qualified, only that assembly will be searched.
    //    if szTypeName is not ASM-qualified, we will search for the types in the following order:
    //       - in pRequestingAssembly (if not NULL). pRequestingAssembly is the assembly that contained
    //         the custom attribute from which the typename was derived.
    //       - in mscorlib.dll
    //       - raise an AssemblyResolveEvent() in the current appdomain
    //
    // pRequestingAssembly may be NULL. In that case, the "visibility" check will simply check that
    // the loaded type has public access.
    //
    //--------------------------------------------------------------------------------------------
    static TypeHandle GetTypeUsingCASearchRules(LPCUTF8 szTypeName, Assembly *pRequestingAssembly, BOOL *pfTypeNameWasQualified = NULL, BOOL bDoVisibilityChecks = TRUE);
    static TypeHandle GetTypeUsingCASearchRules(LPCWSTR szTypeName, Assembly *pRequestingAssembly, BOOL *pfTypeNameWasQualified = NULL, BOOL bDoVisibilityChecks = TRUE);


    static TypeHandle GetTypeManaged(
        LPCWSTR szTypeName, 
        DomainAssembly* pAssemblyGetType,
        BOOL bThrowIfNotFound,
        BOOL bIgnoreCase,
        BOOL bIntrospectionOnly,
        BOOL bProhibitAssemblyQualifiedName,
        StackCrawlMark* pStackMark,
        BOOL bLoadTypeFromPartialNameHack);
    
    
public:
    SString* GetAssembly() { WRAPPER_CONTRACT; return &m_assembly; }
    
private:
    TypeName() : m_bIsGenericArgument(FALSE), m_count(0) { WRAPPER_CONTRACT; }    
    TypeName* AddGenericArgument();
    SString* AddName() { WRAPPER_CONTRACT; return m_names.AppendEx(m_nestNameFactory.Create()); }
    SArray<SString*>& GetNames() { WRAPPER_CONTRACT; return m_names; }
    SArray<TypeName*>& GetGenericArguments() { WRAPPER_CONTRACT; return m_genericArguments; }
    SArray<DWORD>& GetSignature() { WRAPPER_CONTRACT; return m_signature; }
    void SetByRef() { WRAPPER_CONTRACT; m_signature.Append(ELEMENT_TYPE_BYREF); }
    void SetPointer() { WRAPPER_CONTRACT;  m_signature.Append(ELEMENT_TYPE_PTR); }
    void SetSzArray() { WRAPPER_CONTRACT; m_signature.Append(ELEMENT_TYPE_SZARRAY); }
    void SetArray(DWORD rank) { WRAPPER_CONTRACT; m_signature.Append(ELEMENT_TYPE_ARRAY); m_signature.Append(rank); } 
    SString* ToString(SString* pBuf, BOOL bAssemblySpec = FALSE, BOOL bSignature = FALSE, BOOL bGenericArguments = FALSE);
        
private:
    //----------------------------------------------------------------------------------------------------------------
    // This is the "uber" GetType() that all public GetType() funnels through. It's main job is to figure out which
    // Assembly to load the type from and then invoke GetTypeHaveAssembly.
    //
    // It's got a highly baroque interface partly for historical reasons and partly because it's the uber-function
    // for all of the possible GetTypes.
    //----------------------------------------------------------------------------------------------------------------
    TypeHandle GetTypeWorker(
        BOOL bThrowIfNotFound, 
        BOOL bIgnoreCase, 
        BOOL bIntrospectionOnly, 
        Assembly* pAssemblyGetType,

        BOOL fEnableCASearchRules,  
                                    
        BOOL bProhibitAssemblyQualifiedName,
                                    
        StackCrawlMark* pStackMark, 
        Assembly* pRequestingAssembly, 
        BOOL bLoadTypeFromPartialNameHack);    

    //----------------------------------------------------------------------------------------------------------------
    // This is the one that actually loads the type once we've pinned down the Assembly it's in.
    //----------------------------------------------------------------------------------------------------------------
    TypeHandle GetTypeHaveAssembly(Assembly* pAssembly, BOOL bThrowIfNotFound, BOOL bIgnoreCase, BOOL bRecurse = TRUE);

private:
    BOOL m_bIsGenericArgument;
    DWORD m_count;
    InlineSArray<DWORD, 128> m_signature;
    InlineSArray<TypeName*, 128> m_genericArguments;
    InlineSArray<SString*, 128> m_names;
    InlineSString<128> m_assembly;
    Factory<InlineSString<128> > m_nestNameFactory;
};

#endif
