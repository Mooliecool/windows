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
/*  FieldMarshaler.CPP:
 *
 */

#include "common.h"
#include "vars.hpp"
#include "class.h"
#include "ceeload.h"
#include "excep.h"
#include "fieldmarshaler.h"
#include "corjit.h"
#include "comstring.h"
#include "field.h"
#include "frames.h"
#include "gcscan.h"
#include "dllimport.h"
#include "comdelegate.h"
#include "eeconfig.h"
#include "comdatetime.h"
#include "olevariant.h"
#include <cor.h>
#include <corpriv.h>
#include <corerror.h>
#include "sigformat.h"
#include "marshalnative.h"
#include "typeparse.h"


// forward declaration
BOOL CheckForPrimitiveType(CorElementType elemType, CQuickArray<WCHAR> *pStrPrimitiveType);
TypeHandle ArraySubTypeLoadWorker(const SString &strUserDefTypeName, Assembly* pAssembly);
TypeHandle GetFieldTypeHandleWorker(MetaSig *pFieldSig);  


//=======================================================================
// A database of NFT types.
//=======================================================================
struct NFTDataBaseEntry
{
    UINT32            m_cbNativeSize;     // native size of field (0 if not constant)
};

static const NFTDataBaseEntry NFTDataBase[] =
{
    #undef DEFINE_NFT
    #define DEFINE_NFT(name, nativesize) { nativesize },
    #include "nsenums.h"
};


//=======================================================================
// This is invoked from the class loader while building the internal structures for a type
// This function should check if explicit layout metadata exists.
//
// Returns:
//  TRUE    - yes, there's layout metadata
//  FALSE   - no, there's no layout.
//  fail    - throws a typeload exception
//
// If TRUE,
//   *pNLType            gets set to nltAnsi or nltUnicode
//   *pPackingSize       declared packing size
//   *pfExplicitoffsets  offsets explicit in metadata or computed?
//=======================================================================
BOOL HasLayoutMetadata(Assembly* pAssembly, IMDInternalImport *pInternalImport, mdTypeDef cl, MethodTable*pParentMT, BYTE *pPackingSize, BYTE *pNLTType, BOOL *pfExplicitOffsets)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pInternalImport));
        PRECONDITION(CheckPointer(pPackingSize));
        PRECONDITION(CheckPointer(pNLTType));
        PRECONDITION(CheckPointer(pfExplicitOffsets));
    }
    CONTRACTL_END;
    
    HRESULT hr;
    ULONG clFlags;
#ifdef _DEBUG
    clFlags = 0xcccccccc;
#endif


    pInternalImport->GetTypeDefProps(cl, &clFlags, NULL);

    if (IsTdAutoLayout(clFlags))
    {
        //
        //
        ULONG cbTotalSize = 0;
        if (SUCCEEDED(pInternalImport->GetClassTotalSize(cl, &cbTotalSize)) && cbTotalSize != 0)
        {
            if (pParentMT && pParentMT->IsValueTypeClass())
            {
                MDEnumHolder hEnumField(pInternalImport);
                if (SUCCEEDED(pInternalImport->EnumInit(mdtFieldDef, cl, &hEnumField)))
                {
                    ULONG numFields = pInternalImport->EnumGetCount(&hEnumField);
                    if (numFields == 0)
                    {
                        *pfExplicitOffsets = FALSE;
                        *pNLTType = nltAnsi;
                        *pPackingSize = 1;
                        return TRUE;
                    }
                }
            }
        }

        return FALSE;
    }
    else if (IsTdSequentialLayout(clFlags))
    {
        *pfExplicitOffsets = FALSE;
    }
    else if (IsTdExplicitLayout(clFlags))
    {
        *pfExplicitOffsets = TRUE;
    }
    else
    {
        pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_BADFORMAT);
    }
    
    // We now know this class has seq. or explicit layout. Ensure the parent does too.
    if (pParentMT && !(pParentMT->IsObjectClass() || pParentMT->IsValueTypeClass()) && !(pParentMT->HasLayout()))
        pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_BADFORMAT);

    if (IsTdAnsiClass(clFlags))
    {
        *pNLTType = nltAnsi;
    }
    else if (IsTdUnicodeClass(clFlags))
    {
        *pNLTType = nltUnicode;
    }
    else if (IsTdAutoClass(clFlags))
    {
        *pNLTType = (NDirectOnUnicodeSystem() ? nltUnicode : nltAnsi);
    }
    else
    {
        pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_BADFORMAT);
    }

    DWORD dwPackSize;
    hr = pInternalImport->GetClassPackSize(cl, &dwPackSize);
    if (FAILED(hr) || dwPackSize == 0) 
        dwPackSize = DEFAULT_PACKING_SIZE;

    // This has to be reduced to a BYTE value, so we had better make sure it fits. If
    // not, we'll throw an exception instead of trying to munge the value to what we
    // think the user might want.
    if (!FitsInU1((UINT64)(dwPackSize)))
    {
        pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_BADFORMAT);
    }

    *pPackingSize = (BYTE)dwPackSize;
    
    return TRUE;
}

VOID    ParseNativeType(Module*                     pModule,
                        PCCOR_SIGNATURE             pCOMSignature,
                        DWORD                       cbCOMSignature,
                        BYTE                        nlType,
                        LayoutRawFieldInfo* const   pfwalk,
                        PCCOR_SIGNATURE             pNativeType,
                        ULONG                       cbNativeType,
                        IMDInternalImport*          pInternalImport,
                        mdTypeDef                   cl,
                        const SigTypeContext *      pTypeContext,
                        BOOL                       *pfDisqualifyFromManagedSequential  // set to TRUE if needed (never set to FALSE, it may come in as TRUE!)
#ifdef _DEBUG
                        ,
                        LPCUTF8             szNamespace,
                        LPCUTF8             szClassName,
                        LPCUTF8             szFieldName
#endif
                        )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pfwalk));
    }
    CONTRACTL_END;
    
#define INITFIELDMARSHALER(nfttype, fmtype, args)       \
do                                                      \
{                                                       \
    C_ASSERT(sizeof(fmtype) <= MAXFIELDMARSHALERSIZE);  \
    pfwalk->m_nft = (nfttype);                          \
    new ( &(pfwalk->m_FieldMarshaler) ) fmtype args;    \
} while(0)

    BOOL                fAnsi               = (nlType == nltAnsi);
    CorElementType      corElemType         = ELEMENT_TYPE_END;
    PCCOR_SIGNATURE     pNativeTypeStart    = pNativeType;    
    ULONG               cbNativeTypeStart   = cbNativeType;
    BYTE                ntype;
    BOOL                fDefault;
    BOOL                BestFit;
    BOOL                ThrowOnUnmappableChar;
    
    pfwalk->m_nft                           = NFT_NONE;

    if (cbNativeType == 0)
    {
        ntype = NATIVE_TYPE_MAX;
        fDefault = TRUE;
    }
    else
    {
        ntype = *( ((BYTE*&)pNativeType)++ );
        cbNativeType--;
        fDefault = FALSE;
    }

    // Setup the signature and normalize
    MetaSig fsig(pCOMSignature, cbCOMSignature, pModule, pTypeContext, FALSE, MetaSig::sigField);
    corElemType = fsig.NextArgNormalized();


    if (!(*pfDisqualifyFromManagedSequential))
    {
        // This type may qualify for ManagedSequential. Collect managed size and alignment info.
        if (CorTypeInfo::IsPrimitiveType(corElemType))
        {
            pfwalk->m_managedSize = ((UINT32)CorTypeInfo::Size(corElemType)); // Safe cast - no primitive type is larger than 4gb!
            pfwalk->m_managedAlignmentReq = pfwalk->m_managedSize;
        }
        else if (corElemType == ELEMENT_TYPE_PTR)
        {
            pfwalk->m_managedSize = sizeof(LPVOID);
            pfwalk->m_managedAlignmentReq = sizeof(LPVOID);
        }
        else if (corElemType == ELEMENT_TYPE_VALUETYPE)
        {
            TypeHandle pNestedType = fsig.GetLastTypeHandleThrowing(ClassLoader::LoadTypes,
                                                                    CLASS_LOAD_APPROXPARENTS,
                                                                    TRUE);
            if (pNestedType.GetMethodTable()->IsManagedSequential())
            {
                pfwalk->m_managedSize = (pNestedType.GetMethodTable()->GetNumInstanceFieldBytes());

                _ASSERTE(pNestedType.GetMethodTable()->HasLayout()); // If it is ManagedSequential(), it also has Layout but doesn't hurt to check before we do a cast!
                pfwalk->m_managedAlignmentReq = pNestedType.GetMethodTable()->GetLayoutInfo()->m_ManagedLargestAlignmentRequirementOfAllMembers;
            }
            else
            {
                *pfDisqualifyFromManagedSequential = TRUE;
            }
        }
        else
        {
            // No other type permitted for ManagedSequential.
            *pfDisqualifyFromManagedSequential = TRUE;
        }
    }

    CorElementType sigElemType;
    IfFailThrow(fsig.GetArgProps().PeekElemType(&sigElemType));
    if ((sigElemType == ELEMENT_TYPE_GENERICINST || sigElemType == ELEMENT_TYPE_VAR) && corElemType == ELEMENT_TYPE_CLASS)
    {
        INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_GENERICS_RESTRICTION));
    }
    else switch (corElemType)
    {
        case ELEMENT_TYPE_CHAR:
            if (fDefault)
            {
                if (fAnsi)
                {
                    ReadBestFitCustomAttribute(pInternalImport, cl, &BestFit, &ThrowOnUnmappableChar);
                    INITFIELDMARSHALER(NFT_ANSICHAR, FieldMarshaler_Ansi, (BestFit, ThrowOnUnmappableChar));
                }
                else
                {
                    INITFIELDMARSHALER(NFT_COPY2, FieldMarshaler_Copy2, ());
                }
            }
            else if (ntype == NATIVE_TYPE_I1 || ntype == NATIVE_TYPE_U1)
            {
                ReadBestFitCustomAttribute(pInternalImport, cl, &BestFit, &ThrowOnUnmappableChar);
                INITFIELDMARSHALER(NFT_ANSICHAR, FieldMarshaler_Ansi, (BestFit, ThrowOnUnmappableChar));
            }
            else if (ntype == NATIVE_TYPE_I2 || ntype == NATIVE_TYPE_U2)
            {
                INITFIELDMARSHALER(NFT_COPY2, FieldMarshaler_Copy2, ());
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_CHAR));
            }
            break;

        case ELEMENT_TYPE_BOOLEAN:
            if (fDefault)
            {
                INITFIELDMARSHALER(NFT_WINBOOL, FieldMarshaler_WinBool, ());
            }
            else if (ntype == NATIVE_TYPE_BOOLEAN)
            {
                INITFIELDMARSHALER(NFT_WINBOOL, FieldMarshaler_WinBool, ());
            }
            else if (ntype == NATIVE_TYPE_U1 || ntype == NATIVE_TYPE_I1)
            {
                INITFIELDMARSHALER(NFT_CBOOL, FieldMarshaler_CBool, ());
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_BOOLEAN));
            }
            break;


        case ELEMENT_TYPE_I1:
            if (fDefault || ntype == NATIVE_TYPE_I1 || ntype == NATIVE_TYPE_U1)
            {
                INITFIELDMARSHALER(NFT_COPY1, FieldMarshaler_Copy1, ());
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_I1));
            }
            break;

        case ELEMENT_TYPE_U1:
            if (fDefault || ntype == NATIVE_TYPE_U1 || ntype == NATIVE_TYPE_I1)
            {
                INITFIELDMARSHALER(NFT_COPY1, FieldMarshaler_Copy1, ());
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_I1));
            }
            break;

        case ELEMENT_TYPE_I2:
            if (fDefault || ntype == NATIVE_TYPE_I2 || ntype == NATIVE_TYPE_U2)
            {
                INITFIELDMARSHALER(NFT_COPY2, FieldMarshaler_Copy2, ());
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_I2));
            }
            break;

        case ELEMENT_TYPE_U2:
            if (fDefault || ntype == NATIVE_TYPE_U2 || ntype == NATIVE_TYPE_I2)
            {
                INITFIELDMARSHALER(NFT_COPY2, FieldMarshaler_Copy2, ());
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_I2));
            }
            break;

        case ELEMENT_TYPE_I4:
            if (fDefault || ntype == NATIVE_TYPE_I4 || ntype == NATIVE_TYPE_U4 || ntype == NATIVE_TYPE_ERROR)
            {
                INITFIELDMARSHALER(NFT_COPY4, FieldMarshaler_Copy4, ());
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_I4));
            }
            break;
            
        case ELEMENT_TYPE_U4:
            if (fDefault || ntype == NATIVE_TYPE_U4 || ntype == NATIVE_TYPE_I4 || ntype == NATIVE_TYPE_ERROR)
            {
                INITFIELDMARSHALER(NFT_COPY4, FieldMarshaler_Copy4, ());
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_I4));
            }
            break;

        case ELEMENT_TYPE_I8:
            if (fDefault || ntype == NATIVE_TYPE_I8 || ntype == NATIVE_TYPE_U8)
            {
                INITFIELDMARSHALER(NFT_COPY8, FieldMarshaler_Copy8, ());
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_I8));
            }
            break;

        case ELEMENT_TYPE_U8:
            if (fDefault || ntype == NATIVE_TYPE_U8 || ntype == NATIVE_TYPE_I8)
            {
                INITFIELDMARSHALER(NFT_COPY8, FieldMarshaler_Copy8, ());
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_I8));
            }
            break;

        case ELEMENT_TYPE_I: //fallthru
        case ELEMENT_TYPE_U:
            if (fDefault || ntype == NATIVE_TYPE_INT || ntype == NATIVE_TYPE_UINT)
            {
                if (sizeof(LPVOID)==4)
                {
                    INITFIELDMARSHALER(NFT_COPY4, FieldMarshaler_Copy4, ());
                }
                else
                {
                    INITFIELDMARSHALER(NFT_COPY8, FieldMarshaler_Copy8, ());
                }
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_I));
            }
            break;

        case ELEMENT_TYPE_R4:
            if (fDefault || ntype == NATIVE_TYPE_R4)
            {
                INITFIELDMARSHALER(NFT_COPY4, FieldMarshaler_Copy4, ());
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_R4));
            }
            break;
            
        case ELEMENT_TYPE_R8:
            if (fDefault || ntype == NATIVE_TYPE_R8)
            {
                INITFIELDMARSHALER(NFT_COPY8, FieldMarshaler_Copy8, ());
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_R8));
            }
            break;

        case ELEMENT_TYPE_PTR:
            if (fDefault)
            {
                switch (sizeof(LPVOID))
                {
                    case 4:
                        INITFIELDMARSHALER(NFT_COPY4, FieldMarshaler_Copy4, ());
                        break;
                        
                    case 8:
                        INITFIELDMARSHALER(NFT_COPY8, FieldMarshaler_Copy8, ());
                        break;

                    default:
                        INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_BADMANAGED));
                        break;
                }
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_PTR));
            }
            break;

        case ELEMENT_TYPE_VALUETYPE:
        {
            // <dsyme> I didn't write this code, but am documetning it...
            // Sadly this may cause a TypeLoadException, which we currently seem to have to swallow.
            // This happens with structs that contain fields of class type where the class itself
            // refers to the struct in a field.
            TypeHandle thNestedType = GetFieldTypeHandleWorker(&fsig);
            if (!thNestedType.GetClass())
                break;
            
            if (fsig.IsClass(g_DateClassName))
            {
                if (fDefault || ntype == NATIVE_TYPE_STRUCT)
                {
                        INITFIELDMARSHALER(NFT_DATE, FieldMarshaler_Date, ());
                }
                else
                {
                        INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_DATETIME));                    
                }
            }
            else if (fsig.IsClass(g_DecimalClassName))
            {
                if (fDefault || ntype == NATIVE_TYPE_STRUCT)
                {
                    INITFIELDMARSHALER(NFT_DECIMAL, FieldMarshaler_Decimal, ());
                }
                else
                {
                    INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_DECIMAL));                    
                }
            }
            else if (thNestedType.GetMethodTable()->HasLayout())
            {
                if (fDefault || ntype == NATIVE_TYPE_STRUCT)
                {
                    if (IsStructMarshalable(thNestedType))
                    {
                        INITFIELDMARSHALER(NFT_NESTEDVALUECLASS, FieldMarshaler_NestedValueClass, (thNestedType.GetMethodTable()));
                    }
                    else
                    {
                        INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_NOTMARSHALABLE));
                    }
                }
                else
                {
                    INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_VALUETYPE));                                        
                }
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_NOTMARSHALABLE));
            }
            break;
        }

        case ELEMENT_TYPE_CLASS:
        {
            // <dsyme> I didn't write this code, but am documetning it...
            // Sadly this may cause a TypeLoadException, which we currently seem to have to swallow.
            // This happens with structs that contain fields of class type where the class itself
            // refers to the struct in a field.
            TypeHandle thNestedType = GetFieldTypeHandleWorker(&fsig);
            if (!thNestedType.GetClass())
                break;
                       
            if (thNestedType.GetMethodTable()->IsObjectClass())
            {
                if (fDefault || ntype == NATIVE_TYPE_IUNKNOWN)
                {
                    INITFIELDMARSHALER(NFT_INTERFACE, FieldMarshaler_Interface, (NULL, NULL));
                }
                else
                {
                    INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_OBJECT));
                }
            }
            else if (ntype == NATIVE_TYPE_CUSTOMMARSHALER)
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHALFIELD_NOCUSTOMMARSH));
            }
            else if (thNestedType == TypeHandle(g_pStringClass))
            {
                if (fDefault)
                {
                    if (fAnsi)
                    {
                        ReadBestFitCustomAttribute(pInternalImport, cl, &BestFit, &ThrowOnUnmappableChar);
                        INITFIELDMARSHALER(NFT_STRINGANSI, FieldMarshaler_StringAnsi, (BestFit, ThrowOnUnmappableChar));
                    }
                    else
                    {
                        INITFIELDMARSHALER(NFT_STRINGUNI, FieldMarshaler_StringUni, ());
                    }
                }
                else
                {
                    switch (ntype)
                    {
                        case NATIVE_TYPE_LPSTR:
                            ReadBestFitCustomAttribute(pInternalImport, cl, &BestFit, &ThrowOnUnmappableChar);
                            INITFIELDMARSHALER(NFT_STRINGANSI, FieldMarshaler_StringAnsi, (BestFit, ThrowOnUnmappableChar));
                            break;

                        case NATIVE_TYPE_LPWSTR:
                            INITFIELDMARSHALER(NFT_STRINGUNI, FieldMarshaler_StringUni, ());
                            break;

                        case NATIVE_TYPE_LPTSTR:
                            if (NDirectOnUnicodeSystem())
                            {
                                INITFIELDMARSHALER(NFT_STRINGUNI, FieldMarshaler_StringUni, ());
                            }
                            else
                            {
                                ReadBestFitCustomAttribute(pInternalImport, cl, &BestFit, &ThrowOnUnmappableChar);
                                INITFIELDMARSHALER(NFT_STRINGANSI, FieldMarshaler_StringAnsi, (BestFit, ThrowOnUnmappableChar));
                            }
                            break;
                            case NATIVE_TYPE_FIXEDSYSSTRING:
                            {
                                ULONG nchars;
                                ULONG udatasize = CorSigUncompressedDataSize(pNativeType);

                                if (cbNativeType < udatasize)
                                    break;

                                nchars = CorSigUncompressData(pNativeType);
                                cbNativeType -= udatasize;

                                if (nchars == 0)
                                {
                                    INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHALFIELD_ZEROLENGTHFIXEDSTRING));
                                    break;  
                                }

                                if (fAnsi)
                                {
                                    ReadBestFitCustomAttribute(pInternalImport, cl, &BestFit, &ThrowOnUnmappableChar);
                                    INITFIELDMARSHALER(NFT_FIXEDSTRINGANSI, FieldMarshaler_FixedStringAnsi, (nchars, BestFit, ThrowOnUnmappableChar));
                                }
                                else
                                {
                                    INITFIELDMARSHALER(NFT_FIXEDSTRINGUNI, FieldMarshaler_FixedStringUni, (nchars));
                                }
                            }
                            break;

                        default:
                        INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHALFIELD_STRING));
                            break;
                    }
                }
            }
            else if (COMDelegate::IsDelegate(thNestedType.GetMethodTable()))
            {
                if ( (fDefault || ntype == NATIVE_TYPE_FUNC ) )
                {
                    INITFIELDMARSHALER(NFT_DELEGATE, FieldMarshaler_Delegate, (thNestedType.GetMethodTable()));
                }
                else
                {
                    INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_DELEGATE));
                }
            } 
            else if (thNestedType.CanCastTo(TypeHandle(g_Mscorlib.GetClass(CLASS__SAFE_HANDLE))))
            {
                if (fDefault) 
                {
                    INITFIELDMARSHALER(NFT_SAFEHANDLE, FieldMarshaler_SafeHandle, ());
                }
                else 
                {
                    INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_SAFEHANDLE));
                }
            }
            else if (thNestedType.CanCastTo(TypeHandle(g_Mscorlib.GetClass(CLASS__CRITICAL_HANDLE))))
            {
                if (fDefault) 
                {
                    INITFIELDMARSHALER(NFT_CRITICALHANDLE, FieldMarshaler_CriticalHandle, ());
                }
                else 
                {
                    INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_CRITICALHANDLE));
                }
            }
            else if (fsig.IsClass(g_StringBufferClassName))
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHALFIELD_NOSTRINGBUILDER));
            }
            else if (IsStructMarshalable(thNestedType))
            {
                if (fDefault || (ntype == NATIVE_TYPE_STRUCT))
                {
                    INITFIELDMARSHALER(NFT_NESTEDLAYOUTCLASS, FieldMarshaler_NestedLayoutClass, (thNestedType.GetMethodTable()));
                }
                else
                {
                    INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHALFIELD_LAYOUTCLASS));
                }
            }
            break;
        }

        case ELEMENT_TYPE_SZARRAY:
        case ELEMENT_TYPE_ARRAY:
        {
            // <dsyme> I didn't write this code, but am documetning it...
            // Sadly this may cause a TypeLoadException, which we currently seem to have to swallow.
            // This happens with structs that contain fields of class type where the class itself
            // refers to the struct in a field.
            TypeHandle thArray = GetFieldTypeHandleWorker(&fsig);
            if (thArray.IsNull() || !thArray.IsArray())
                break;

            TypeHandle thElement = thArray.AsArray()->GetArrayElementTypeHandle();
            if (thElement.IsNull())
                break;

            if (ntype == NATIVE_TYPE_FIXEDARRAY)
            {
                CorNativeType elementNativeType = NATIVE_TYPE_DEFAULT;
                
                // The size constant must be specified, if it isn't then the struct can't be marshalled.
                if (S_OK != CheckForCompressedData(pNativeTypeStart, pNativeType, cbNativeTypeStart))
                {
                    INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHALFIELD_FIXEDARRAY_NOSIZE));                            
                    break;
                }

                // Read the size const, if it's 0, then the struct can't be marshalled.
                ULONG numElements = CorSigUncompressData(pNativeType);            
                if (numElements == 0)
                {
                    INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHALFIELD_FIXEDARRAY_ZEROSIZE));                            
                    break;
                }

                // The array sub type is optional so extract it if specified.
                if (S_OK == CheckForCompressedData(pNativeTypeStart, pNativeType, cbNativeTypeStart))
                    elementNativeType = (CorNativeType)CorSigUncompressData(pNativeType);

                ArrayMarshalInfo arrayMarshalInfo(amiRuntime);
                arrayMarshalInfo.InitForFixedArray(thElement, elementNativeType, fAnsi);

                if (!arrayMarshalInfo.IsValid())
                {
                    INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (arrayMarshalInfo.GetErrorResourceId())); 
                    break;
                }

                if (arrayMarshalInfo.GetElementVT() == VTHACK_ANSICHAR)
                {
                    // We need to special case fixed sized arrays of ANSI chars since the OleVariant code
                    // that is used by the generic fixed size array marshaller doesn't support them
                    // properly. 
                    ReadBestFitCustomAttribute(pInternalImport, cl, &BestFit, &ThrowOnUnmappableChar);
                    INITFIELDMARSHALER(NFT_FIXEDCHARARRAYANSI, FieldMarshaler_FixedCharArrayAnsi, (numElements, BestFit, ThrowOnUnmappableChar));
                    break;                    
                }
                else
                {
                    INITFIELDMARSHALER(NFT_FIXEDARRAY, FieldMarshaler_FixedArray, (pInternalImport, cl, numElements, arrayMarshalInfo.GetElementVT(), arrayMarshalInfo.GetElementTypeHandle().GetMethodTable()));
                    break;
                }
            }
            else
            {
                INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHALFIELD_ARRAY));                     
            }
            break;
        }            

        case ELEMENT_TYPE_OBJECT:
        case ELEMENT_TYPE_STRING:
            break;

        default:
            // let it fall thru as NFT_NONE
            break;
    }

    if (pfwalk->m_nft == NFT_NONE)
    {
        INITFIELDMARSHALER(NFT_ILLEGAL, FieldMarshaler_Illegal, (IDS_EE_BADMARSHAL_BADMANAGED));
    }
#undef INITFIELDMARSHALER
}


TypeHandle ArraySubTypeLoadWorker(const SString &strUserDefTypeName, Assembly* pAssembly)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pAssembly));
    }
    CONTRACTL_END;
    
    TypeHandle th; 

    EX_TRY
    {
        // Load the user defined type.
        StackScratchBuffer utf8Name;
        th = TypeName::GetTypeUsingCASearchRules(strUserDefTypeName.GetUTF8(utf8Name), pAssembly);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(RethrowTerminalExceptions)
    
    return th;
}


TypeHandle GetFieldTypeHandleWorker(MetaSig *pFieldSig)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pFieldSig));
    }
    CONTRACTL_END;

    TypeHandle th; 

    EX_TRY
    {
        // Load the user defined type.
        th = pFieldSig->GetLastTypeHandleThrowing(ClassLoader::LoadTypes, 
                                                  CLASS_LOAD_APPROXPARENTS,
                                                  TRUE /*dropGenericArgumentLevel*/);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(RethrowTerminalExceptions)

    return th;
}


//=======================================================================
// This function returns TRUE if the type passed in is either a value class or a class and if it has layout information 
// and is marshalable. In all other cases it will return FALSE. 
//=======================================================================
BOOL IsStructMarshalable(TypeHandle th)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(!th.IsNull());
    }
    CONTRACTL_END;
    
    if (th.IsBlittable())
    {
        // th.IsBlittable will return true for arrays of blittable types, however since IsStructMarshalable
        // is only supposed to return true for value classes or classes with layout that are marshallable
        // we need to return false if the type is an array.
        if (th.IsArray())
            return FALSE;
        else
            return TRUE;
    }

    // Check to see if the type has layout.
    if (!th.HasLayout())
        return FALSE;

    MethodTable *pMT= th.GetMethodTable();
    PREFIX_ASSUME(pMT != NULL);
    
    const FieldMarshaler *pFieldMarshaler = pMT->GetLayoutInfo()->GetFieldMarshalers();
    UINT  numReferenceFields              = pMT->GetLayoutInfo()->GetNumCTMFields();

    while (numReferenceFields--)
    {
        if (pFieldMarshaler->GetClass() == FieldMarshaler::CLASS_ILLEGAL)
            return FALSE;

        ((BYTE*&)pFieldMarshaler) += MAXFIELDMARSHALERSIZE;
    }

    return TRUE;
}


//=======================================================================
// Called from the clsloader to load up and summarize the field metadata
// for layout classes.
//
// Warning: This function can load other classes (esp. for nested structs.)
//=======================================================================
VOID EEClassLayoutInfo::CollectLayoutFieldMetadataThrowing(
   BaseDomain *pDomain,          // Domain in which to allocate anything we allocate
   mdTypeDef cl,                // cl of the NStruct being loaded
   BYTE packingSize,            // packing size (from @dll.struct)
   BYTE nlType,                 // nltype (from @dll.struct)
   BOOL fExplicitOffsets,       // explicit offsets?
   MethodTable *pParentMT,       // the loaded superclass
   ULONG cMembers,              // total number of members (methods + fields)
   HENUMInternal *phEnumField,  // enumerator for field
   Module* pModule,             // Module that defines the scope, loader and heap (for allocate FieldMarshalers)
   const SigTypeContext *pTypeContext,          // Type parameters for NStruct being loaded
   EEClassLayoutInfo *pEEClassLayoutInfoOut,  // caller-allocated structure to fill in.
   LayoutRawFieldInfo *pInfoArrayOut, // caller-allocated array to fill in.  Needs room for cMember+1 elements
   AllocMemTracker    *pamTracker
)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(pDomain));
    }
    CONTRACTL_END;

    HRESULT             hr;
    MD_CLASS_LAYOUT     classlayout;
    mdFieldDef          fd;
    ULONG               ulOffset;
    ULONG cFields = 0;

    // Running tote - if anything in this type disqualifies it from being ManagedSequential, somebody will set this to TRUE by the the time
    // function exits.
    BOOL                fDisqualifyFromManagedSequential = FALSE; 

    // Internal interface for the NStruct being loaded.
    IMDInternalImport *pInternalImport = pModule->GetMDImport();


#ifdef _DEBUG
    LPCUTF8 szName; 
    LPCUTF8 szNamespace; 
    pInternalImport->GetNameOfTypeDef(cl, &szName, &szNamespace);

    if (g_pConfig->ShouldBreakOnStructMarshalSetup(szName))
        _ASSERTE(!"BreakOnStructMarshalSetup");
#endif


    // Check if this type might be ManagedSequential. Only valuetypes marked Sequential can be
    // ManagedSequential. Other issues checked below might also disqualify the type.
    if ( (!fExplicitOffsets) &&    // Is it marked sequential?
         (pParentMT && (pParentMT->IsValueTypeClass() || pParentMT->IsManagedSequential()))  // Is it a valuetype or derived from a qualifying valuetype?
       )
    {
        // Type qualifies so far... need do nothing.
    }
    else
    {
        fDisqualifyFromManagedSequential = TRUE;
    }


    BOOL fHasNonTrivialParent = pParentMT &&
                                !pParentMT->IsObjectClass() &&
                                !pParentMT->IsValueTypeClass();


    //====================================================================
    // First, some validation checks.
    //====================================================================
    _ASSERTE(!(fHasNonTrivialParent && !(pParentMT->HasLayout())));

    hr = pInternalImport->GetClassLayoutInit(cl, &classlayout);
    if (FAILED(hr))
    {
        COMPlusThrowHR(hr, BFA_CANT_GET_CLASSLAYOUT);
    }

    pEEClassLayoutInfoOut->m_numCTMFields        = fHasNonTrivialParent ? pParentMT->GetLayoutInfo()->m_numCTMFields : 0;
    pEEClassLayoutInfoOut->m_pFieldMarshalers    = NULL;
    pEEClassLayoutInfoOut->SetIsBlittable(TRUE);
    if (fHasNonTrivialParent)
        pEEClassLayoutInfoOut->SetIsBlittable(pParentMT->IsBlittable());
    pEEClassLayoutInfoOut->SetIsZeroSized(FALSE);

    LayoutRawFieldInfo *pfwalk = pInfoArrayOut;
    LayoutRawFieldInfo **pSortArray = (LayoutRawFieldInfo**)_alloca(cMembers * sizeof(LayoutRawFieldInfo*));
    LayoutRawFieldInfo **pSortArrayEnd = pSortArray;

    ULONG   maxRid = pInternalImport->GetCountWithTokenKind(mdtFieldDef);

    
    //=====================================================================
    // Phase 1: Figure out the NFT of each field based on both the CLR
    // signature of the field and the FieldMarshaler metadata. 
    //=====================================================================
    BOOL fParentHasLayout = pParentMT && pParentMT->HasLayout();
    UINT32 cbAdjustedParentLayoutNativeSize = 0;
    EEClassLayoutInfo *pParentLayoutInfo = NULL;;
    if (fParentHasLayout)
    {
        pParentLayoutInfo = pParentMT->GetLayoutInfo();
        // Treat base class as an initial member.
        cbAdjustedParentLayoutNativeSize = pParentLayoutInfo->GetNativeSize();
        // If the parent was originally a zero-sized explicit type but
        // got bumped up to a size of 1 for compatability reasons, then
        // we need to remove the padding, but ONLY for inheritance situations.
        if (pParentLayoutInfo->IsZeroSized()) {
            CONSISTENCY_CHECK(cbAdjustedParentLayoutNativeSize == 1);
            cbAdjustedParentLayoutNativeSize = 0;
        }
    }

    ULONG i;
    for (i = 0; pInternalImport->EnumNext(phEnumField, &fd); i++)
    {
        DWORD       dwFieldAttrs;
        ULONG       rid = RidFromToken(fd);

        if((rid == 0)||(rid > maxRid))
        {
            COMPlusThrowHR(COR_E_TYPELOAD, BFA_BAD_FIELD_TOKEN);
        }

        dwFieldAttrs = pInternalImport->GetFieldDefProps(fd);

        PCCOR_SIGNATURE pNativeType = NULL;
        ULONG cbNativeType;
        if ( !(IsFdStatic(dwFieldAttrs)) )
        {
            PCCOR_SIGNATURE pCOMSignature;
            ULONG       cbCOMSignature;

            if (IsFdHasFieldMarshal(dwFieldAttrs))
            {
                hr = pInternalImport->GetFieldMarshal(fd, &pNativeType, &cbNativeType);
                if (FAILED(hr))
                    cbNativeType = 0;
            }
            else
                cbNativeType = 0;

            pCOMSignature = pInternalImport->GetSigOfFieldDef(fd,&cbCOMSignature);

            hr = ::validateTokenSig(fd,pCOMSignature,cbCOMSignature,dwFieldAttrs,pInternalImport);
            if(FAILED(hr)) COMPlusThrowHR(hr);

            // fill the appropriate entry in pInfoArrayOut
            pfwalk->m_MD = fd;
            pfwalk->m_nft = NULL;
            pfwalk->m_offset = (UINT32) -1;
            pfwalk->m_sequence = 0;

#ifdef _DEBUG
            LPCUTF8 szFieldName = pInternalImport->GetNameOfFieldDef(fd);
#endif

            ParseNativeType(pModule,
                            pCOMSignature,
                            cbCOMSignature,
                            nlType,
                            pfwalk,
                            pNativeType,
                            cbNativeType,
                            pInternalImport,
                            cl,
                            pTypeContext,
                            &fDisqualifyFromManagedSequential
#ifdef _DEBUG
                            ,
                            szNamespace,
                            szName,
                            szFieldName
#endif
                                );



            BOOL    resetBlittable = TRUE;

            // if it's a simple copy...
            if (pfwalk->m_nft == NFT_COPY1    ||
                pfwalk->m_nft == NFT_COPY2    ||
                pfwalk->m_nft == NFT_COPY4    ||
                pfwalk->m_nft == NFT_COPY8)
            {
                resetBlittable = FALSE;
            }

            // Or if it's a nested value class that is itself blittable...
            if (pfwalk->m_nft == NFT_NESTEDVALUECLASS)
            {
                FieldMarshaler *pFM = (FieldMarshaler*)&(pfwalk->m_FieldMarshaler);
                _ASSERTE(pFM->IsNestedValueClassMarshaler());
                if (((FieldMarshaler_NestedValueClass *) pFM)->IsBlittable())
                    resetBlittable = FALSE;
            }

            // ...Otherwise, this field prevents blitting
            if (resetBlittable)
                pEEClassLayoutInfoOut->SetIsBlittable(FALSE);

            cFields++;
            pfwalk++;
        }
    }

    _ASSERTE(i == cMembers);

    // NULL out the last entry
    pfwalk->m_MD = mdFieldDefNil;
    
    
    //
    // fill in the layout information 
    //
    
    // pfwalk points to the beginging of the array
    pfwalk = pInfoArrayOut;

    while (SUCCEEDED(hr = pInternalImport->GetClassLayoutNext(
                                     &classlayout,
                                     &fd,
                                     &ulOffset)) &&
                                     fd != mdFieldDefNil)
    {
        // watch for the last entry: must be mdFieldDefNil
        while ((mdFieldDefNil != pfwalk->m_MD)&&(pfwalk->m_MD < fd))
            pfwalk++;

        if(pfwalk->m_MD != fd) continue;

        // if we haven't found a matching token, it must be a static field with layout -- ignore it
        _ASSERTE(pfwalk->m_MD == fd);

        if (!fExplicitOffsets)
        {
            // ulOffset is the sequence
            pfwalk->m_sequence = ulOffset;
        }
        else
        {
            // ulOffset is the explicit offset
            pfwalk->m_offset = ulOffset;
            pfwalk->m_sequence = (ULONG) -1;

            // Treat base class as an initial member.
            if (!SafeAddUINT32(&(pfwalk->m_offset), cbAdjustedParentLayoutNativeSize))
                COMPlusThrowOM();
        }
    }
    
    if (FAILED(hr))
        COMPlusThrowHR(hr);
    
    // now sort the array
    if (!fExplicitOffsets)
    { 
        // sort sequential by ascending sequence
        for (i = 0; i < cFields; i++)
        {
            LayoutRawFieldInfo**pSortWalk = pSortArrayEnd;
            while (pSortWalk != pSortArray)
            {
                if (pInfoArrayOut[i].m_sequence >= (*(pSortWalk-1))->m_sequence)
                    break;

                pSortWalk--;
            }

            // pSortWalk now points to the target location for new FieldInfo.
            MoveMemory(pSortWalk + 1, pSortWalk, (pSortArrayEnd - pSortWalk) * sizeof(LayoutRawFieldInfo*));
            *pSortWalk = &pInfoArrayOut[i];
            pSortArrayEnd++;
        }
    }
    else // no sorting for explicit layout
    {
        for (i = 0; i < cFields; i++)
        {
            if(pInfoArrayOut[i].m_MD != mdFieldDefNil)
            {
                if (pInfoArrayOut[i].m_offset == (UINT32)-1)
                {
                    LPCUTF8 szFieldName;
                    szFieldName = pInternalImport->GetNameOfFieldDef(pInfoArrayOut[i].m_MD);
                    pModule->GetAssembly()->ThrowTypeLoadException(pInternalImport, 
                                                                   cl,
                                                                   szFieldName,
                                                                   IDS_CLASSLOAD_NSTRUCT_EXPLICIT_OFFSET);
                }
                else if ((INT)pInfoArrayOut[i].m_offset < 0)
                {
                    LPCUTF8 szFieldName;
                    szFieldName = pInternalImport->GetNameOfFieldDef(pInfoArrayOut[i].m_MD);
                    pModule->GetAssembly()->ThrowTypeLoadException(pInternalImport, 
                                                                   cl,
                                                                   szFieldName,
                                                                   IDS_CLASSLOAD_NSTRUCT_NEGATIVE_OFFSET);
                }
            }
                
            *pSortArrayEnd = &pInfoArrayOut[i];
            pSortArrayEnd++;
        }
    }

    //=====================================================================
    // Phase 2: Compute the native size (in bytes) of each field.
    // Store this in pInfoArrayOut[].cbNativeSize;
    //=====================================================================

    // Now compute the native size of each field
    for (pfwalk = pInfoArrayOut; pfwalk->m_MD != mdFieldDefNil; pfwalk++)
    {
        UINT8 nft = pfwalk->m_nft;
        pEEClassLayoutInfoOut->m_numCTMFields++;

        // If the NFT's size never changes, it is stored in the database.
        UINT32 cbNativeSize = NFTDataBase[nft].m_cbNativeSize;

        if (cbNativeSize == 0)
        {
            // Size of 0 means NFT's size is variable, so we have to figure it
            // out case by case.
            cbNativeSize = ((FieldMarshaler*)&(pfwalk->m_FieldMarshaler))->NativeSize();
        }
        pfwalk->m_cbNativeSize = cbNativeSize;
    }

    if (pEEClassLayoutInfoOut->m_numCTMFields)
    {
        pEEClassLayoutInfoOut->m_pFieldMarshalers = (FieldMarshaler*)(pamTracker->Track(pDomain->GetLowFrequencyHeap()->AllocMem(MAXFIELDMARSHALERSIZE * pEEClassLayoutInfoOut->m_numCTMFields)));

        // Bring in the parent's fieldmarshalers
        if (fHasNonTrivialParent)
        {
            CONSISTENCY_CHECK(fParentHasLayout);
            UINT numChildCTMFields = pEEClassLayoutInfoOut->m_numCTMFields - pParentLayoutInfo->m_numCTMFields;
            memcpyNoGCRefs( ((BYTE*)pEEClassLayoutInfoOut->m_pFieldMarshalers) + MAXFIELDMARSHALERSIZE*numChildCTMFields,
                            pParentLayoutInfo->m_pFieldMarshalers,
                            MAXFIELDMARSHALERSIZE * (pParentLayoutInfo->m_numCTMFields) );
        }

    }


    //=====================================================================
    // Phase 3: If FieldMarshaler requires autooffsetting, compute the offset
    // of each field and the size of the total structure. We do the layout
    // according to standard VC layout rules:
    //
    //   Each field has an alignment requirement. The alignment-requirement
    //   of a scalar field is the smaller of its size and the declared packsize.
    //   The alighnment-requirement of a struct field is the smaller of the
    //   declared packsize and the largest of the alignment-requirement
    //   of its fields. The alignment requirement of an array is that
    //   of one of its elements.
    //
    //   In addition, each struct gets padding at the end to ensure
    //   that an array of such structs contain no unused space between
    //   elements.
    //=====================================================================
    {
        BYTE   LargestAlignmentRequirement = 1;
        UINT32 cbCurOffset = 0;

        // Treat base class as an initial member.
        if (!SafeAddUINT32(&cbCurOffset, cbAdjustedParentLayoutNativeSize))
            COMPlusThrowOM();

        if (fParentHasLayout)
        {
            BYTE alignmentRequirement;
            
            alignmentRequirement = min(packingSize, pParentLayoutInfo->GetLargestAlignmentRequirementOfAllMembers());
    
            LargestAlignmentRequirement = max(LargestAlignmentRequirement, alignmentRequirement);                                          
        }

        // Start with the size inherited from the parent (if any).
        unsigned calcTotalSize = cbAdjustedParentLayoutNativeSize;
     
        LayoutRawFieldInfo **pSortWalk;
        for (pSortWalk = pSortArray, i=cFields; i; i--, pSortWalk++)
        {
            pfwalk = *pSortWalk;
    
            BYTE alignmentRequirement = ((FieldMarshaler*)&(pfwalk->m_FieldMarshaler))->AlignmentRequirement();
            _ASSERTE(alignmentRequirement == 1 ||
                     alignmentRequirement == 2 ||
                     alignmentRequirement == 4 ||
                     alignmentRequirement == 8);
    
            alignmentRequirement = min(alignmentRequirement, packingSize);
    
            LargestAlignmentRequirement = max(LargestAlignmentRequirement, alignmentRequirement);
    
            // This assert means I forgot to special-case some NFT in the
            // above switch.
            _ASSERTE(alignmentRequirement <= 8);
    
            // Check if this field is overlapped with other(s)
            pfwalk->m_fIsOverlapped = FALSE;
            if (fExplicitOffsets) {
                LayoutRawFieldInfo *pfwalk1;
                DWORD dwBegin = pfwalk->m_offset;
                DWORD dwEnd = dwBegin+pfwalk->m_cbNativeSize;
                for (pfwalk1 = pInfoArrayOut; pfwalk1 < pfwalk; pfwalk1++)
                {
                    if((pfwalk1->m_offset >= dwEnd) || (pfwalk1->m_offset+pfwalk1->m_cbNativeSize <= dwBegin)) continue;
                    pfwalk->m_fIsOverlapped = TRUE;
                    pfwalk1->m_fIsOverlapped = TRUE;
                }
            }
            else
            {
                // Insert enough padding to align the current data member.
                while (cbCurOffset % alignmentRequirement)
                {
                    if (!SafeAddUINT32(&cbCurOffset, 1))
                        COMPlusThrowOM();
                }
    
                // Insert current data member.
                pfwalk->m_offset = cbCurOffset;
    
                // if we overflow we will catch it below
                cbCurOffset += pfwalk->m_cbNativeSize;
            } 
    
            unsigned fieldEnd = pfwalk->m_offset + pfwalk->m_cbNativeSize;
            if (fieldEnd < pfwalk->m_offset)
                COMPlusThrowOM();
    
                // size of the structure is the size of the last field.  
            if (fieldEnd > calcTotalSize)
                calcTotalSize = fieldEnd;
        }
    
        ULONG clstotalsize = 0;
        pInternalImport->GetClassTotalSize(cl, &clstotalsize);
    
        if (clstotalsize != 0)
        {
            if (!SafeAddULONG(&clstotalsize, (ULONG)cbAdjustedParentLayoutNativeSize))
                COMPlusThrowOM();
    
            // size must be large enough to accomodate layout. If not, we use the layout size instead.
            if (clstotalsize < calcTotalSize)
            {
                clstotalsize = calcTotalSize;
            }
            calcTotalSize = clstotalsize;   // use the size they told us 
        } 
        else
        {
            // The did not give us an explict size, so lets round up to a good size (for arrays) 
            while (calcTotalSize % LargestAlignmentRequirement)
            {
                if (!SafeAddUINT32(&calcTotalSize, 1))
                    COMPlusThrowOM();
            }
        }
    
        // We'll cap the total native size at a (somewhat) arbitrary limit to ensure
        // that we don't expose some overflow bug later on.
        if (calcTotalSize >= 0x7ffffff0)
            COMPlusThrowOM();

        // This is a zero-sized struct - need to record the fact and bump it up to 1.
        if (calcTotalSize == 0)
        {
            pEEClassLayoutInfoOut->SetIsZeroSized(TRUE);
            calcTotalSize = 1;
        }
    
        pEEClassLayoutInfoOut->m_cbNativeSize = calcTotalSize;
    
        // The packingSize acts as a ceiling on all individual alignment
        // requirements so it follows that the largest alignment requirement
        // is also capped.
        _ASSERTE(LargestAlignmentRequirement <= packingSize);
        pEEClassLayoutInfoOut->m_LargestAlignmentRequirementOfAllMembers = LargestAlignmentRequirement;
    }



    //=====================================================================
    // Phase 4: Now we do the same thing again for managedsequential layout.
    //=====================================================================
    if (!fDisqualifyFromManagedSequential)
    {
        BYTE   LargestAlignmentRequirement = 1;
        UINT32 cbCurOffset = 0;
    
        if (pParentMT && pParentMT->IsManagedSequential())
        {
            // Treat base class as an initial member.
            if (!SafeAddUINT32(&cbCurOffset, pParentMT->GetNumInstanceFieldBytes()))
                COMPlusThrowOM();
    
            BYTE alignmentRequirement = 0;
                
            alignmentRequirement = min(packingSize, pParentLayoutInfo->m_ManagedLargestAlignmentRequirementOfAllMembers);
    
            LargestAlignmentRequirement = max(LargestAlignmentRequirement, alignmentRequirement);                                          
        }
    
        // The current size of the structure as a whole, we start at 1, because we disallow 0 sized structures.
        // NOTE: We do not need to do the same checking for zero-sized types as phase 3 because only ValueTypes
        //       can be ManagedSequential and ValueTypes can not be inherited from.
        unsigned calcTotalSize = 1;
     
        LayoutRawFieldInfo **pSortWalk;
        for (pSortWalk = pSortArray, i=cFields; i; i--, pSortWalk++)
        {
            pfwalk = *pSortWalk;
    
            BYTE alignmentRequirement = ((BYTE)(pfwalk->m_managedAlignmentReq));
            _ASSERTE(alignmentRequirement == 1 ||
                     alignmentRequirement == 2 ||
                     alignmentRequirement == 4 ||
                     alignmentRequirement == 8);
    
            alignmentRequirement = min(alignmentRequirement, packingSize);
    
            LargestAlignmentRequirement = max(LargestAlignmentRequirement, alignmentRequirement);
    
            _ASSERTE(alignmentRequirement <= 8);
    
            // Insert enough padding to align the current data member.
            while (cbCurOffset % alignmentRequirement)
            {
                if (!SafeAddUINT32(&cbCurOffset, 1))
                    COMPlusThrowOM();
            }

            // Insert current data member.
            pfwalk->m_managedOffset = cbCurOffset;

            // if we overflow we will catch it below
            cbCurOffset += pfwalk->m_managedSize;
    
            unsigned fieldEnd = pfwalk->m_managedOffset + pfwalk->m_managedSize;
            if (fieldEnd < pfwalk->m_managedOffset)
                COMPlusThrowOM();
    
                // size of the structure is the size of the last field.  
            if (fieldEnd > calcTotalSize)
                calcTotalSize = fieldEnd;


#ifdef _DEBUG
            // @perf: If the type is blittable, the managed and native layouts have to be identical
            // so they really shouldn't be calculated twice. Until this code has been well tested and
            // stabilized, however, it is useful to compute both and assert that they are equal in the blittable
            // case.
            if (pEEClassLayoutInfoOut->IsBlittable())
            {
                _ASSERTE(pfwalk->m_managedOffset == pfwalk->m_offset);
                _ASSERTE(pfwalk->m_managedSize   == pfwalk->m_cbNativeSize);
            }
#endif

        } //for
    
        ULONG clstotalsize = 0;
        pInternalImport->GetClassTotalSize(cl, &clstotalsize);
    
        if (clstotalsize != 0)
        {
            if (pParentMT && pParentMT->IsManagedSequential())
            {
                // Treat base class as an initial member.
                UINT32 parentSize = pParentMT->GetNumInstanceFieldBytes();
                if (!SafeAddULONG(&clstotalsize, parentSize))
                    COMPlusThrowOM();
            }
    
            // size must be large enough to accomodate layout. If not, we use the layout size instead.
            if (clstotalsize < calcTotalSize)
            {
                clstotalsize = calcTotalSize;
            }
            calcTotalSize = clstotalsize;   // use the size they told us 
        } 
        else
        {
            // The did not give us an explict size, so lets round up to a good size (for arrays) 
            while (calcTotalSize % LargestAlignmentRequirement)
            {
                if (!SafeAddUINT32(&calcTotalSize, 1))
                    COMPlusThrowOM();
            }
        } 
    
        pEEClassLayoutInfoOut->m_cbManagedSize = calcTotalSize;

        // The packingSize acts as a ceiling on all individual alignment
        // requirements so it follows that the largest alignment requirement
        // is also capped.
        _ASSERTE(LargestAlignmentRequirement <= packingSize);
        pEEClassLayoutInfoOut->m_ManagedLargestAlignmentRequirementOfAllMembers = LargestAlignmentRequirement;

#ifdef _DEBUG
            // @perf: If the type is blittable, the managed and native layouts have to be identical
            // so they really shouldn't be calculated twice. Until this code has been well tested and
            // stabilized, however, it is useful to compute both and assert that they are equal in the blittable
            // case.
            if (pEEClassLayoutInfoOut->IsBlittable())
            {
                _ASSERTE(pEEClassLayoutInfoOut->m_cbManagedSize == pEEClassLayoutInfoOut->m_cbNativeSize);
                _ASSERTE(pEEClassLayoutInfoOut->m_ManagedLargestAlignmentRequirementOfAllMembers == pEEClassLayoutInfoOut->m_LargestAlignmentRequirementOfAllMembers);
            }
#endif
    } //if

    pEEClassLayoutInfoOut->SetIsManagedSequential(!fDisqualifyFromManagedSequential);

#ifdef _DEBUG
    {
        BOOL illegalMarshaler = FALSE;
        
        LOG((LF_INTEROP, LL_INFO100000, "\n\n"));
        LOG((LF_INTEROP, LL_INFO100000, "%s.%s\n", szNamespace, szName));
        LOG((LF_INTEROP, LL_INFO100000, "Packsize      = %lu\n", (ULONG)packingSize));
        LOG((LF_INTEROP, LL_INFO100000, "Max align req = %lu\n", (ULONG)(pEEClassLayoutInfoOut->m_LargestAlignmentRequirementOfAllMembers)));
        LOG((LF_INTEROP, LL_INFO100000, "----------------------------\n"));
        for (pfwalk = pInfoArrayOut; pfwalk->m_MD != mdFieldDefNil; pfwalk++)
        {
            LPCUTF8 fieldname = "??";
            fieldname = pInternalImport->GetNameOfFieldDef(pfwalk->m_MD);
            LOG((LF_INTEROP, LL_INFO100000, "+%-5lu  ", (ULONG)(pfwalk->m_offset)));
            LOG((LF_INTEROP, LL_INFO100000, "%s", fieldname));
            LOG((LF_INTEROP, LL_INFO100000, "\n"));

            if (((FieldMarshaler*)&pfwalk->m_FieldMarshaler)->GetClass() == FieldMarshaler::CLASS_ILLEGAL)
                illegalMarshaler = TRUE;             
        }

        // If we are dealing with a non trivial parent, determine if it has any illegal marshallers.
        if (fHasNonTrivialParent)
        {
            FieldMarshaler *pParentFM = pParentMT->GetLayoutInfo()->GetFieldMarshalers();
            for (i = 0; i < pParentMT->GetLayoutInfo()->m_numCTMFields; i++)
            {
                if (pParentFM->GetClass() == FieldMarshaler::CLASS_ILLEGAL)
                    illegalMarshaler = TRUE;                                 
                ((BYTE*&)pParentFM) += MAXFIELDMARSHALERSIZE;
            }
        }
        
        LOG((LF_INTEROP, LL_INFO100000, "+%-5lu   EOS\n", (ULONG)(pEEClassLayoutInfoOut->m_cbNativeSize)));
        LOG((LF_INTEROP, LL_INFO100000, "Allocated %d %s field marshallers for %s.%s\n", pEEClassLayoutInfoOut->m_numCTMFields, (illegalMarshaler ? "pointless" : "usable"), szNamespace, szName));
    }
#endif
    return;
}

//=======================================================================
// For each reference-typed FieldMarshaler field, marshals the current CLR value
// to a new native instance and stores it in the fixed portion of the FieldMarshaler.
//
// This function does not attempt to delete the native value that it overwrites.
//
// If pOptionalCleanupWorkList is non-null, this function also schedules
// (unconditionally) a nativedestroy on that field (note that if the
// contents of that field changes before the cleanupworklist fires,
// the new value is what will be destroyed. This is by design, as it
// unifies cleanup for [in,out] parameters.)
//=======================================================================
VOID LayoutUpdateNative(LPVOID *ppProtectedManagedData, UINT offsetbias, MethodTable *pMT, BYTE* pNativeData, CleanupWorkList *pOptionalCleanupWorkList)
{       
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pMT));
    }
    CONTRACTL_END;
    
    FieldMarshaler* pFM                   = pMT->GetLayoutInfo()->GetFieldMarshalers();
    UINT  numReferenceFields              = pMT->GetLayoutInfo()->GetNumCTMFields();
    
    OBJECTREF pCLRValue    = NULL;
    LPVOID scalar           = NULL;
    
    GCPROTECT_BEGIN(pCLRValue)
    GCPROTECT_BEGININTERIOR(scalar)
    {
        g_IBCLogger.LogFieldMarshalersReadAccess(pMT->GetClass());

        while (numReferenceFields--)
        {
            pFM->Restore();

            DWORD internalOffset = pFM->GetFieldDesc()->GetOffset();

            if (pFM->IsScalarMarshaler())
            {
                scalar = (LPVOID)(internalOffset + offsetbias + (BYTE*)(*ppProtectedManagedData));
                // Note this will throw for FieldMarshaler_Illegal
                pFM->ScalarUpdateNative(scalar, pNativeData + pFM->GetExternalOffset() );
                
            }
            else if (pFM->IsNestedValueClassMarshaler())
            {
                pFM->NestedValueClassUpdateNative((const VOID **)ppProtectedManagedData, internalOffset + offsetbias, pNativeData + pFM->GetExternalOffset(), pOptionalCleanupWorkList);
            }
            else
            {
                pCLRValue = *(OBJECTREF*)(internalOffset + offsetbias + (BYTE*)(*ppProtectedManagedData));
                pFM->UpdateNative(&pCLRValue, pNativeData + pFM->GetExternalOffset(), pOptionalCleanupWorkList);
                SetObjectReferenceUnchecked( (OBJECTREF*) (internalOffset + offsetbias + (BYTE*)(*ppProtectedManagedData)), pCLRValue);
            }

            // The cleanup work list is not used to clean up the native contents. It is used
            // to handle cleanup of any additionnal resources the FieldMarshalers allocate.

            ((BYTE*&)pFM) += MAXFIELDMARSHALERSIZE;
        }
    }
    GCPROTECT_END();
    GCPROTECT_END();
}


VOID FmtClassUpdateNative(OBJECTREF *ppProtectedManagedData, BYTE *pNativeData, CleanupWorkList *pOptionalCleanupWorkList)
{        
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppProtectedManagedData));
    }
    CONTRACTL_END;

    MethodTable *pMT = (*ppProtectedManagedData)->GetMethodTable();
    _ASSERTE(pMT->IsBlittable() || pMT->HasLayout());
    UINT32   cbsize = pMT->GetNativeSize();

    if (pMT->IsBlittable())
    {
        memcpyNoGCRefs(pNativeData, (*ppProtectedManagedData)->GetData(), cbsize);
    }
    else
    {
        // This allows us to do a partial LayoutDestroyNative in the case of
        // a marshaling error on one of the fields.
        FillMemory(pNativeData, cbsize, 0);
        NativeLayoutDestroyer nld(pNativeData, pMT, cbsize);
        
        LayoutUpdateNative( (VOID**)ppProtectedManagedData,
                                Object::GetOffsetOfFirstField(),
                                pMT,
                                pNativeData,
                            pOptionalCleanupWorkList);
        
        nld.SuppressRelease();
    }

}


VOID FmtClassUpdateCLR(OBJECTREF *ppProtectedManagedData, BYTE *pNativeData, BOOL fDeleteOld)
{       
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    MethodTable *pMT = (*ppProtectedManagedData)->GetMethodTable();
    _ASSERTE(pMT->IsBlittable() || pMT->HasLayout());
    UINT32   cbsize = pMT->GetNativeSize();

    if (pMT->IsBlittable())
    {
        memcpyNoGCRefs((*ppProtectedManagedData)->GetData(), pNativeData, cbsize);
    }
    else
    {
        LayoutUpdateCLR((VOID**)ppProtectedManagedData,
                            Object::GetOffsetOfFirstField(),
                            pMT,
                            (BYTE*)pNativeData,
                            fDeleteOld
                           );
    }
}



//=======================================================================
// For each reference-typed FieldMarshaler field, marshals the current CLR value
// to a new CLR instance and stores it in the GC portion of the FieldMarshaler.
//
// If fDeleteNativeCopies is true, it will also destroy the native version.
//
// NOTE: To avoid error-path leaks, this function attempts to destroy
// all of the native fields even if one or more of the conversions fail.
//=======================================================================
VOID LayoutUpdateCLR(LPVOID *ppProtectedManagedData, UINT offsetbias, MethodTable *pMT, BYTE *pNativeData, BOOL fDeleteNativeCopies)
{        
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pMT));
    }
    CONTRACTL_END;

    // Don't care about zero-ing the data afterward - pass in a 0 for size.
    NativeLayoutDestroyer nld(pNativeData, pMT, 0);
    
    FieldMarshaler* pFM                   = pMT->GetLayoutInfo()->GetFieldMarshalers();
    UINT  numReferenceFields              = pMT->GetLayoutInfo()->GetNumCTMFields(); 

    struct _gc
    {
        OBJECTREF pCLRValue;
        OBJECTREF pOldCLRValue;
    } gc;

    gc.pCLRValue    = NULL;
    gc.pOldCLRValue = NULL;
    LPVOID scalar    = NULL;
    
    GCPROTECT_BEGIN(gc)
    GCPROTECT_BEGININTERIOR(scalar)
    {
        g_IBCLogger.LogFieldMarshalersReadAccess(pMT->GetClass());

        while (numReferenceFields--)
        {
            pFM->Restore();

            DWORD internalOffset = pFM->GetFieldDesc()->GetOffset();

            if (pFM->IsScalarMarshaler())
            {
                scalar = (LPVOID)(internalOffset + offsetbias + (BYTE*)(*ppProtectedManagedData));
                // Note this will throw for FieldMarshaler_Illegal
                pFM->ScalarUpdateCLR( pNativeData + pFM->GetExternalOffset(), scalar);
            }
            else if (pFM->IsNestedValueClassMarshaler())
            {
                pFM->NestedValueClassUpdateCLR(pNativeData + pFM->GetExternalOffset(), ppProtectedManagedData, internalOffset + offsetbias);
            }
            else
            {
                gc.pOldCLRValue = *(OBJECTREF*)(internalOffset + offsetbias + (BYTE*)(*ppProtectedManagedData));
                pFM->UpdateCLR( pNativeData + pFM->GetExternalOffset(), &gc.pCLRValue, &gc.pOldCLRValue );
                SetObjectReferenceUnchecked( (OBJECTREF*) (internalOffset + offsetbias + (BYTE*)(*ppProtectedManagedData)), gc.pCLRValue );
            }

            ((BYTE*&)pFM) += MAXFIELDMARSHALERSIZE;
        }
    }
    GCPROTECT_END();
    GCPROTECT_END();
    
    if (!fDeleteNativeCopies)
        nld.SuppressRelease();
}


VOID LayoutDestroyNative(LPVOID pNative, MethodTable *pMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMT));
    }
    CONTRACTL_END;
    
    FieldMarshaler *pFM                   = pMT->GetLayoutInfo()->GetFieldMarshalers();
    UINT  numReferenceFields              = pMT->GetLayoutInfo()->GetNumCTMFields();
    BYTE *pNativeData                     = (BYTE*)pNative;

    while (numReferenceFields--)
    {
        pFM->DestroyNative( pNativeData + pFM->GetExternalOffset() );
        ((BYTE*&)pFM) += MAXFIELDMARSHALERSIZE;
    }
}

VOID FmtClassDestroyNative(LPVOID pNative, MethodTable *pMT)
{       
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMT));
    }
    CONTRACTL_END;
    
    if (pNative)
    {
        if (!(pMT->IsBlittable()))
        {
            _ASSERTE(pMT->HasLayout());
            LayoutDestroyNative(pNative, pMT);
        }
    }
}

VOID FmtValueTypeUpdateNative(LPVOID pProtectedManagedData, MethodTable *pMT, BYTE *pNativeData, CleanupWorkList *pOptionalCleanupWorkList)
{        
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pMT));
    }
    CONTRACTL_END;
    
    _ASSERTE(pMT->IsValueClass() && (pMT->IsBlittable() || pMT->HasLayout()));
    UINT32   cbsize = pMT->GetNativeSize();

    if (pMT->IsBlittable())
    {
        memcpyNoGCRefs(pNativeData, pProtectedManagedData, cbsize);
    }
    else
    {
        // This allows us to do a partial LayoutDestroyNative in the case of
        // a marshaling error on one of the fields.
        FillMemory(pNativeData, cbsize, 0);
        
        NativeLayoutDestroyer nld(pNativeData, pMT, cbsize);
        
        LayoutUpdateNative( (VOID**)pProtectedManagedData,
                                0,
                                pMT,
                                pNativeData,
                                pOptionalCleanupWorkList);
        
        nld.SuppressRelease();
    }
}

VOID FmtValueTypeUpdateCLR(LPVOID pProtectedManagedData, MethodTable *pMT, BYTE *pNativeData, BOOL fDeleteOld)
{       
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pMT));
    }
    CONTRACTL_END;

    _ASSERTE(pMT->IsValueClass() && (pMT->IsBlittable() || pMT->HasLayout()));
    UINT32   cbsize = pMT->GetNativeSize();

    if (pMT->IsBlittable())
    {
        memcpyNoGCRefs(pProtectedManagedData, pNativeData, cbsize);
    }
    else
    {
        LayoutUpdateCLR((VOID**)pProtectedManagedData,
                            0,
                            pMT,
                            (BYTE*)pNativeData,
                            fDeleteOld);
    }
}




//=======================================================================
// Nested structure conversion
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_NestedLayoutClass::UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pNativeValue));
    }
    CONTRACTL_END;

    UINT32 cbNativeSize = GetMethodTable()->GetNativeSize();

    if (*pCLRValue == NULL)
    {
        ZeroMemory(pNativeValue, cbNativeSize);
    }
    else
    {
        LayoutUpdateNative((LPVOID*)pCLRValue, Object::GetOffsetOfFirstField(), 
                           GetMethodTable(), (BYTE*)pNativeValue, pOptionalCleanupWorkList);
    }

}


//=======================================================================
// Nested structure conversion
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_NestedLayoutClass::UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pNativeValue));
        PRECONDITION(CheckPointer(ppProtectedCLRValue));
    }
    CONTRACTL_END;

    *ppProtectedCLRValue = GetMethodTable()->Allocate();

    LayoutUpdateCLR( (LPVOID*)ppProtectedCLRValue,
                         Object::GetOffsetOfFirstField(),
                         GetMethodTable(),
                         (BYTE *)pNativeValue,
                         FALSE);

}


//=======================================================================
// Nested structure conversion
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_NestedLayoutClass::DestroyNative(LPVOID pNativeValue) const 
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pNativeValue));
    }
    CONTRACTL_END;

    LayoutDestroyNative(pNativeValue, GetMethodTable());
}



//=======================================================================
// Nested structure conversion
// See FieldMarshaler for details.
//=======================================================================
UINT32 FieldMarshaler_NestedLayoutClass::NativeSize() const
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    return GetMethodTable()->GetLayoutInfo()->GetNativeSize();
}

//=======================================================================
// Nested structure conversion
// See FieldMarshaler for details.
//=======================================================================
UINT32 FieldMarshaler_NestedLayoutClass::AlignmentRequirement() const
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    return GetMethodTable()->GetLayoutInfo()->GetLargestAlignmentRequirementOfAllMembers();
}



//=======================================================================
// Nested structure conversion
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_NestedValueClass::NestedValueClassUpdateNative(const VOID **ppProtectedCLR, UINT startoffset, LPVOID pNative, CleanupWorkList *pOptionalCleanupWorkList) const
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(ppProtectedCLR));
        PRECONDITION(CheckPointer(pNative));
    }
    CONTRACTL_END;

    // would be better to detect this at class load time (that have a nested value
    // class with no layout) but don't have a way to know
    if (! GetMethodTable()->GetLayoutInfo())
        COMPlusThrow(kArgumentException, IDS_NOLAYOUT_IN_EMBEDDED_VALUECLASS);

    LayoutUpdateNative((LPVOID*)ppProtectedCLR, startoffset, GetMethodTable(), (BYTE*)pNative, pOptionalCleanupWorkList);
}


//=======================================================================
// Nested structure conversion
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_NestedValueClass::NestedValueClassUpdateCLR(const VOID *pNative, LPVOID *ppProtectedCLR, UINT startoffset) const
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNative));
        PRECONDITION(CheckPointer(ppProtectedCLR));
    }
    CONTRACTL_END;

    // would be better to detect this at class load time (that have a nested value
    // class with no layout) but don't have a way to know
    if (! GetMethodTable()->GetLayoutInfo())
        COMPlusThrow(kArgumentException, IDS_NOLAYOUT_IN_EMBEDDED_VALUECLASS);

    LayoutUpdateCLR( (LPVOID*)ppProtectedCLR,
                         startoffset,
                         GetMethodTable(),
                         (BYTE *)pNative,
                         FALSE);
    

}


//=======================================================================
// Nested structure conversion
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_NestedValueClass::DestroyNative(LPVOID pNativeValue) const 
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pNativeValue));
    }
    CONTRACTL_END;

    LayoutDestroyNative(pNativeValue, GetMethodTable());
}



//=======================================================================
// Nested structure conversion
// See FieldMarshaler for details.
//=======================================================================
UINT32 FieldMarshaler_NestedValueClass::NativeSize() const
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // this can't be marshalled as native type if no layout, so we allow the 
    // native size info to be created if available, but the size will only
    // be valid for native, not unions. Marshaller will throw exception if
    // try to marshall a value class with no layout
    if (GetMethodTable()->HasLayout())
        return GetMethodTable()->GetLayoutInfo()->GetNativeSize();
    
    return 0;
}

//=======================================================================
// Nested structure conversion
// See FieldMarshaler for details.
//=======================================================================
UINT32 FieldMarshaler_NestedValueClass::AlignmentRequirement() const
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    // this can't be marshalled as native type if no layout, so we allow the 
    // native size info to be created if available, but the alignment will only
    // be valid for native, not unions. Marshaller will throw exception if
    // try to marshall a value class with no layout
    if (GetMethodTable()->HasLayout())
    {
        UINT32  uAlignmentReq = GetMethodTable()->GetLayoutInfo()->GetLargestAlignmentRequirementOfAllMembers();
        return uAlignmentReq;
    }
    return 1;
}



//=======================================================================
// CoTask Uni <--> System.String
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_StringUni::UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const 
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pNativeValue));
    }
    CONTRACTL_END;

    STRINGREF pString;
    *((OBJECTREF*)&pString) = *pCLRValue;
    
    if (pString == NULL)
    {
        MAYBE_UNALIGNED_WRITE(pNativeValue, _PTR, NULL);
    }
    else
    {
        DWORD nc   = pString->GetStringLength();
        if (nc > 0x7ffffff0)
            COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

        LPWSTR wsz = (LPWSTR)CoTaskMemAlloc( (nc + 1) * sizeof(WCHAR) );
        if (!wsz)
            COMPlusThrowOM();

        CopyMemory(wsz, pString->GetBuffer(), nc*sizeof(WCHAR));
        wsz[nc] = L'\0';
        MAYBE_UNALIGNED_WRITE(pNativeValue, _PTR, wsz);
    }
}


//=======================================================================
// CoTask Uni <--> System.String
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_StringUni::UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNativeValue));
        PRECONDITION(CheckPointer(ppProtectedCLRValue));
    }
    CONTRACTL_END;

    STRINGREF pString;
    LPCWSTR wsz = (LPCWSTR)MAYBE_UNALIGNED_READ(pNativeValue, _PTR);
    
    if (!wsz)
        pString = NULL;
    else
    {
        DWORD length = (DWORD)wcslen(wsz);
        if (length > 0x7ffffff0)
            COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);

        pString = COMString::NewString(wsz, length);
    }
    
    *((STRINGREF*)ppProtectedCLRValue) = pString;
}


//=======================================================================
// CoTask Uni <--> System.String
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_StringUni::DestroyNative(LPVOID pNativeValue) const 
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pNativeValue));        
    }
    CONTRACTL_END;
    
    LPWSTR wsz = (LPWSTR)MAYBE_UNALIGNED_READ(pNativeValue, _PTR);
    MAYBE_UNALIGNED_WRITE(pNativeValue, _PTR, NULL);
    if (wsz)
        CoTaskMemFree(wsz);
}



//=======================================================================
// CoTask Ansi <--> System.String
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_StringAnsi::UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pNativeValue));
    }
    CONTRACTL_END;

    STRINGREF pString;
    *((OBJECTREF*)&pString) = *pCLRValue;
    
    if (pString == NULL)
    {
        MAYBE_UNALIGNED_WRITE(pNativeValue, _PTR, NULL);
    }
    else
    {
        DWORD nc   = pString->GetStringLength();
        if (nc > 0x7ffffff0)
            COMPlusThrow(kMarshalDirectiveException, IDS_EE_STRING_TOOLONG);
 
        LPSTR sz = (LPSTR)CoTaskMemAlloc( (nc + 1) * 2 /* 2 for MBCS */ );
        if (!sz)
            COMPlusThrowOM(); 

        int nbytes = InternalWideToAnsi(pString->GetBuffer(),
                                        nc,
                                        sz,
                                        nc*2,
                                        m_BestFitMap,
                                        m_ThrowOnUnmappableChar);
        sz[nbytes] = '\0';

        MAYBE_UNALIGNED_WRITE(pNativeValue, _PTR, sz);
     }
}


//=======================================================================
// CoTask Ansi <--> System.String
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_StringAnsi::UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pNativeValue));
        PRECONDITION(CheckPointer(ppProtectedCLRValue));
    }
    CONTRACTL_END;

    STRINGREF pString = NULL;
    LPCSTR sz = (LPCSTR)MAYBE_UNALIGNED_READ(pNativeValue, _PTR);
    if (!sz) 
        pString = NULL;
    else 
    {
        MAKE_WIDEPTR_FROMANSI(wsztemp, sz);
        pString = COMString::NewString(wsztemp, __lwsztemp - 1);
    }
    
    *((STRINGREF*)ppProtectedCLRValue) = pString;
}


//=======================================================================
// CoTask Ansi <--> System.String
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_StringAnsi::DestroyNative(LPVOID pNativeValue) const 
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pNativeValue));        
    }
    CONTRACTL_END;
    
    LPSTR sz = (LPSTR)MAYBE_UNALIGNED_READ(pNativeValue, _PTR);
    MAYBE_UNALIGNED_WRITE(pNativeValue, _PTR, NULL);
    if (sz)
        CoTaskMemFree(sz);
}



//=======================================================================
// FixedString <--> System.String
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_FixedStringUni::UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const 
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNativeValue));
    }
    CONTRACTL_END;

    STRINGREF pString;
    *((OBJECTREF*)&pString) = *pCLRValue;
    
    if (pString == NULL)
    {
        MAYBE_UNALIGNED_WRITE(pNativeValue, 16, L'\0');
    }
    else
    {
        DWORD nc = pString->GetStringLength();
        if (nc >= m_numchar)
            nc = m_numchar - 1;

        CopyMemory(pNativeValue, pString->GetBuffer(), nc*sizeof(WCHAR));
        MAYBE_UNALIGNED_WRITE(&(((WCHAR*)pNativeValue)[nc]), 16, L'\0');
    }

}


//=======================================================================
// FixedString <--> System.String
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_FixedStringUni::UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNativeValue));
        PRECONDITION(CheckPointer(ppProtectedCLRValue));
    }
    CONTRACTL_END;

    STRINGREF pString;
    DWORD     ncActual;
    for (ncActual = 0; *(ncActual + (WCHAR*)pNativeValue) != L'\0' && ncActual < m_numchar; ncActual++)
    {
        //nothing
    }
    
    pString = COMString::NewString((const WCHAR *)pNativeValue, ncActual);
    *((STRINGREF*)ppProtectedCLRValue) = pString;
}







//=======================================================================
// FixedString <--> System.String
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_FixedStringAnsi::UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNativeValue));
    }
    CONTRACTL_END;

    STRINGREF pString;
    *((OBJECTREF*)&pString) = *pCLRValue;
    
    if (pString == NULL)
        *((CHAR*)pNativeValue) = L'\0';
    else
    {
        DWORD nc = pString->GetStringLength();
        if (nc >= m_numchar)
            nc = m_numchar - 1;

        int cbwritten = InternalWideToAnsi(pString->GetBuffer(),
                                           nc,
                                            (CHAR*)pNativeValue,
                                           m_numchar,
                                           m_BestFitMap,
                                           m_ThrowOnUnmappableChar);
        ((CHAR*)pNativeValue)[cbwritten] = '\0';
    }

}


//=======================================================================
// FixedString <--> System.String
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_FixedStringAnsi::UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pNativeValue));
        PRECONDITION(CheckPointer(ppProtectedCLRValue));

        // should not have slipped past the metadata
        PRECONDITION(m_numchar != 0);
    }
    CONTRACTL_END;

    STRINGREF pString;
    if (m_numchar == 0)
    {
        // but if it does, better to throw an exception tardily rather than
        // allow a memory corrupt.
        COMPlusThrow(kMarshalDirectiveException);
    }

    UINT32 allocSize = m_numchar + 2;
    if (allocSize < m_numchar)
        ThrowOutOfMemory();
    
    LPSTR tempbuf = (LPSTR)(_alloca((size_t)allocSize));
    if (!tempbuf)
        ThrowOutOfMemory();

    memcpyNoGCRefs(tempbuf, pNativeValue, m_numchar);
    tempbuf[m_numchar-1] = '\0';
    tempbuf[m_numchar] = '\0';
    tempbuf[m_numchar+1] = '\0';

    allocSize = m_numchar * sizeof(WCHAR);
    if (allocSize < m_numchar)
        ThrowOutOfMemory();
    
    LPWSTR    wsztemp = (LPWSTR)_alloca( (size_t)allocSize );
    int ncwritten = MultiByteToWideChar(CP_ACP,
                                        MB_PRECOMPOSED,
                                        tempbuf,
                                        -1,  // # of CHAR's in inbuffer
                                        wsztemp,
                                        m_numchar                       // size (in WCHAR) of outbuffer
                                        );

    if (!ncwritten)
    {
        // intentionally not throwing for MB2WC failure. We don't always know
        // whether to expect a valid string in the buffer and we don't want
        // to throw exceptions randomly.
        ncwritten++;
    }

    pString = COMString::NewString((const WCHAR *)wsztemp, ncwritten-1);
    *((STRINGREF*)ppProtectedCLRValue) = pString;
}


//=======================================================================
// CHAR[] <--> char[]
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_FixedCharArrayAnsi::UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pNativeValue));
    }
    CONTRACTL_END;

    I2ARRAYREF pArray;
    *((OBJECTREF*)&pArray) = *pCLRValue;

    if (pArray == NULL)
        FillMemory(pNativeValue, m_numElems * sizeof(CHAR), 0);
    else
    {
        if (pArray->GetNumComponents() < m_numElems)
            COMPlusThrow(kArgumentException, IDS_WRONGSIZEARRAY_IN_NSTRUCT);
        else
        {
            InternalWideToAnsi((const WCHAR*) pArray->GetDataPtr(),
                               m_numElems,
                              (CHAR*)pNativeValue,
                               m_numElems * sizeof(CHAR),
                               m_BestFitMap,
                               m_ThrowOnUnmappableChar);
        }
    }
}


//=======================================================================
// CHAR[] <--> char[]
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_FixedCharArrayAnsi::UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pNativeValue));
        PRECONDITION(CheckPointer(ppProtectedCLRValue));
    }
    CONTRACTL_END;

    *ppProtectedCLRValue = AllocatePrimitiveArray(ELEMENT_TYPE_CHAR, m_numElems);

    MultiByteToWideChar(CP_ACP,
                        MB_PRECOMPOSED,
                        (const CHAR *)pNativeValue,
                        m_numElems * sizeof(CHAR), // size, in bytes, of in buffer
                        (WCHAR*) ((*((I2ARRAYREF*)ppProtectedCLRValue))->GetDirectPointerToNonObjectElements()),
                        m_numElems);               // size, in WCHAR's of outbuffer                       
}


//=======================================================================
// Embedded array
// See FieldMarshaler for details.
//=======================================================================
FieldMarshaler_FixedArray::FieldMarshaler_FixedArray(IMDInternalImport *pMDImport, mdTypeDef cl, UINT32 numElems, VARTYPE vt, MethodTable* pElementMT)
: m_numElems(numElems)
, m_vt(vt)
, m_BestFitMap(FALSE)
, m_ThrowOnUnmappableChar(FALSE)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pElementMT));
        PRECONDITION(vt != VTHACK_ANSICHAR);        // This must be handled by the FixedCharArrayAnsi marshaler.
    }
    CONTRACTL_END;

    // Only attempt to read the best fit mapping attribute if required to minimize
    // custom attribute accesses.
    if (vt == VT_LPSTR || vt == VT_RECORD)
    {
        BOOL BestFitMap = FALSE;
        BOOL ThrowOnUnmappableChar = FALSE;
        ReadBestFitCustomAttribute(pMDImport, cl, &BestFitMap, &ThrowOnUnmappableChar);      
        m_BestFitMap = !!BestFitMap;
        m_ThrowOnUnmappableChar = !!ThrowOnUnmappableChar;
                }
            
    m_arrayType = ClassLoader::LoadArrayTypeThrowing(TypeHandle(pElementMT), ELEMENT_TYPE_SZARRAY);
            }


//=======================================================================
// Embedded array
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_FixedArray::UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNativeValue));
    }
    CONTRACTL_END;

    if (*pCLRValue == NULL)
    {
        FillMemory(pNativeValue, NativeSize(), 0);
    }
    else
    {
        // Make sure the size of the array is the same as specified in the MarshalAs attribute (via the SizeConst field).
        if ((*pCLRValue)->GetNumComponents() < m_numElems)
            COMPlusThrow(kArgumentException, IDS_WRONGSIZEARRAY_IN_NSTRUCT);

        // Marshal the contents from the managed array to the native array.
        const OleVariant::Marshaler *pMarshaler = OleVariant::GetMarshalerForVarType(m_vt, TRUE);  
        if (pMarshaler == NULL || pMarshaler->ComToOleArray == NULL)
    {
            memcpyNoGCRefs(pNativeValue, (*(BASEARRAYREF*)pCLRValue)->GetDataPtr(), NativeSize());
        }
        else
        {
            MethodTable *pElementMT = m_arrayType.AsArray()->GetArrayElementTypeHandle().GetMethodTable();
            pMarshaler->ComToOleArray((BASEARRAYREF*)pCLRValue, pNativeValue, pElementMT, m_BestFitMap, m_ThrowOnUnmappableChar);
        }
    }
}


//=======================================================================
// Embedded array
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_FixedArray::UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pNativeValue));
        PRECONDITION(CheckPointer(ppProtectedCLRValue));
    }
    CONTRACTL_END;

    // Allocate the value class array.
    *ppProtectedCLRValue = AllocateArrayEx(m_arrayType, (INT32*)&m_numElems, 1);

    // Marshal the contents from the native array to the managed array.
    const OleVariant::Marshaler *pMarshaler = OleVariant::GetMarshalerForVarType(m_vt, TRUE);        
    if (pMarshaler == NULL || pMarshaler->OleToComArray == NULL)
    {
        memcpyNoGCRefs((*(BASEARRAYREF*)ppProtectedCLRValue)->GetDataPtr(), pNativeValue, NativeSize());
    }
    else
    {
        MethodTable *pElementMT = m_arrayType.AsArray()->GetArrayElementTypeHandle().GetMethodTable();
        pMarshaler->OleToComArray((VOID *)pNativeValue, (BASEARRAYREF*)ppProtectedCLRValue, pElementMT);
    }
}


//=======================================================================
// Embedded array
// See FieldMarshaler for details.
//=======================================================================
UINT32 FieldMarshaler_FixedArray::AlignmentRequirement() const
    {
    WRAPPER_CONTRACT;

    UINT32 alignment = 0;
    TypeHandle elementType = m_arrayType.AsArray()->GetArrayElementTypeHandle();

    switch (m_vt)
    {
        case VT_DECIMAL:
            alignment = 8;
            break;

        case VT_VARIANT:
            alignment = 8;
            break;

        case VT_RECORD:
            alignment = elementType.GetMethodTable()->GetLayoutInfo()->GetLargestAlignmentRequirementOfAllMembers();    
            break;

        default:
            alignment = OleVariant::GetElementSizeForVarType(m_vt, elementType.GetMethodTable());
            break;                
}

    return alignment;        
    }
  



//=======================================================================
// function ptr <--> Delegate
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_Delegate::UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNativeValue));
    }
    CONTRACTL_END;

    LPVOID fnPtr = COMDelegate::ConvertToCallback(*pCLRValue);
    MAYBE_UNALIGNED_WRITE(pNativeValue, _PTR, fnPtr);
}


//=======================================================================
// function ptr <--> Delegate
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_Delegate::UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNativeValue));
        PRECONDITION(CheckPointer(ppProtectedCLRValue));
    }
    CONTRACTL_END;

    *ppProtectedCLRValue = COMDelegate::ConvertToDelegate((LPVOID)MAYBE_UNALIGNED_READ(pNativeValue, _PTR), GetMethodTable());
}


//=======================================================================
// SafeHandle <--> Handle
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_SafeHandle::UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNativeValue));
        PRECONDITION(CheckPointer(pOptionalCleanupWorkList, NULL_OK));
    }
    CONTRACTL_END;

    // A cleanup list MUST be specified in order for us to be able to marshal
    // the SafeHandle.
    if (!pOptionalCleanupWorkList)
        COMPlusThrow(kInvalidOperationException, IDS_EE_SH_FIELD_INVALID_OPERATION);

    LPVOID handle = SafeHandleC2NHelper((SAFEHANDLE*)pCLRValue, pOptionalCleanupWorkList);
    MAYBE_UNALIGNED_WRITE(pNativeValue, _PTR, handle);
}


//=======================================================================
// SafeHandle <--> Handle
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_SafeHandle::UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNativeValue));
        PRECONDITION(CheckPointer(ppProtectedCLRValue));
        PRECONDITION(CheckPointer(ppProtectedOldCLRValue));
    }
    CONTRACTL_END;

    // Since we dissallow marshaling SafeHandle fields from unmanaged to managed, check
    // to see if this handle was obtained from a SafeHandle and if it was that the
    // handle value hasn't changed.
    SAFEHANDLE *pSafeHandleObj = (SAFEHANDLE *)ppProtectedOldCLRValue;
    if (!*pSafeHandleObj || (*pSafeHandleObj)->GetHandle() != (LPVOID)MAYBE_UNALIGNED_READ(pNativeValue, _PTR))
        COMPlusThrow(kNotSupportedException, IDS_EE_CANNOT_CREATE_SAFEHANDLE_FIELD);

    // Now that we know the handle hasn't changed we just copy set the new SafeHandle
    // to the old one.
    *ppProtectedCLRValue = *ppProtectedOldCLRValue;
}


//=======================================================================
// CriticalHandle <--> Handle
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_CriticalHandle::UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNativeValue));
        PRECONDITION(CheckPointer(pOptionalCleanupWorkList, NULL_OK));
    }
    CONTRACTL_END;

    LPVOID handle = ((CRITICALHANDLE)*pCLRValue)->GetHandle();
    MAYBE_UNALIGNED_WRITE(pNativeValue, _PTR, handle);
}


//=======================================================================
// CriticalHandle <--> Handle
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_CriticalHandle::UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNativeValue));
        PRECONDITION(CheckPointer(ppProtectedCLRValue));
        PRECONDITION(CheckPointer(ppProtectedOldCLRValue));
    }
    CONTRACTL_END;

    // Since we dissallow marshaling CriticalHandle fields from unmanaged to managed, check
    // to see if this handle was obtained from a CriticalHandle and if it was that the
    // handle value hasn't changed.
    CRITICALHANDLE *pCriticalHandleObj = (CRITICALHANDLE *)ppProtectedOldCLRValue;
    if (!*pCriticalHandleObj || (*pCriticalHandleObj)->GetHandle() != (LPVOID)MAYBE_UNALIGNED_READ(pNativeValue, _PTR))
        COMPlusThrow(kNotSupportedException, IDS_EE_CANNOT_CREATE_CRITICALHANDLE_FIELD);

    // Now that we know the handle hasn't changed we just copy set the new CriticalHandle
    // to the old one.
    *ppProtectedCLRValue = *ppProtectedOldCLRValue;
}


//=======================================================================
// COM IP <--> interface
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_Interface::UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNativeValue));
    }
    CONTRACTL_END;

    IUnknown *pUnk = NULL;

    pUnk = GetComIPFromObjectRef(pCLRValue);

    MAYBE_UNALIGNED_WRITE(pNativeValue, _PTR, pUnk);    
}


//=======================================================================
// COM IP <--> interface
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_Interface::UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNativeValue));
        PRECONDITION(CheckPointer(ppProtectedCLRValue));
    }
    CONTRACTL_END;

    IUnknown *pUnk = (IUnknown*)MAYBE_UNALIGNED_READ(pNativeValue, _PTR);

    *ppProtectedCLRValue = GetObjectRefFromComIP(pUnk, GetMethodTable());
}


//=======================================================================
// COM IP <--> interface
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_Interface::DestroyNative(LPVOID pNativeValue) const 
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pNativeValue));
    }
    CONTRACTL_END;

    IUnknown *pUnk = (IUnknown*)MAYBE_UNALIGNED_READ(pNativeValue, _PTR);
    MAYBE_UNALIGNED_WRITE(pNativeValue, _PTR, NULL);

    if (pUnk != NULL)
    {
        ULONG cbRef = SafeRelease(pUnk);
        LogInteropRelease(pUnk, cbRef, "Field marshaler destroy native");
    }
}



//=======================================================================
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_Date::ScalarUpdateNative(LPVOID pCLR, LPVOID pNative) const
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pCLR));
        PRECONDITION(CheckPointer(pNative));
    }
    CONTRACTL_END;

    *((DATE*)pNative) =  COMDateTime::TicksToDoubleDate(*((INT64*)pCLR));
}


//=======================================================================
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_Date::ScalarUpdateCLR(const VOID *pNative, LPVOID pCLR) const
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNative));
        PRECONDITION(CheckPointer(pCLR));
    }
    CONTRACTL_END;

    *((INT64*)pCLR) = COMDateTime::DoubleDateToTicks(*((DATE*)pNative));
}




//=======================================================================
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_Illegal::ScalarUpdateNative(LPVOID pCLR, LPVOID pNative) const
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pCLR));
        PRECONDITION(CheckPointer(pNative));
    }
    CONTRACTL_END;

    DefineFullyQualifiedNameForClassW();

    StackSString ssFieldName(SString::Utf8, m_pFD->GetName());

    StackSString errorString(L"Unknown error.");
    errorString.LoadResource(m_resIDWhy);

    COMPlusThrow(kTypeLoadException, IDS_EE_BADMARSHALFIELD_ERROR_MSG,
                 GetFullyQualifiedNameForClassW(m_pFD->GetEnclosingClass()),
                 ssFieldName.GetUnicode(), errorString.GetUnicode());
}


//=======================================================================
// See FieldMarshaler for details.
//=======================================================================
VOID FieldMarshaler_Illegal::ScalarUpdateCLR(const VOID *pNative, LPVOID pCLR) const
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNative));
        PRECONDITION(CheckPointer(pCLR));
    }
    CONTRACTL_END;

    DefineFullyQualifiedNameForClassW();

    StackSString ssFieldName(SString::Utf8, m_pFD->GetName());

    StackSString errorString(L"Unknown error.");
    errorString.LoadResource(m_resIDWhy);

    COMPlusThrow(kTypeLoadException, IDS_EE_BADMARSHALFIELD_ERROR_MSG, 
                 GetFullyQualifiedNameForClassW(m_pFD->GetEnclosingClass()),
                 ssFieldName.GetUnicode(), errorString.GetUnicode());
}



VOID NStructFieldTypeToString(FieldMarshaler* pFM, SString& strNStructFieldType)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pFM));
    }
    CONTRACTL_END;

    FieldMarshaler::Class cls = pFM->GetClass();
    LPWSTR  strRetVal;
    CorElementType elemType = pFM->GetFieldDesc()->GetFieldType();

    // Some NStruct Field Types have extra information and require special handling.
    if (cls == FieldMarshaler::CLASS_FIXED_CHAR_ARRAY_ANSI)
    {
        strNStructFieldType.Printf(L"fixed array of ANSI char (size = %i bytes)", pFM->NativeSize());
        return;
    }
    else if (cls == FieldMarshaler::CLASS_FIXED_ARRAY)
    {
        VARTYPE vtElement = ((FieldMarshaler_FixedArray*)pFM)->GetElementVT();
        TypeHandle thElement = ((FieldMarshaler_FixedArray*)pFM)->GetElementTypeHandle();
        BOOL fElementTypeUserDefined = FALSE;

        // Determine if the array type is a user defined type.
        if (vtElement == VT_RECORD)
        {
            fElementTypeUserDefined = TRUE;
        }
        else if (vtElement == VT_UNKNOWN || vtElement == VT_DISPATCH)
        {
            fElementTypeUserDefined = !thElement.IsObjectType();
        }

        // Retrieve the string representation for the VARTYPE.
        StackSString strVarType;
        MarshalInfo::VarTypeToString(vtElement, strVarType);

        MethodTable *pMT = ((FieldMarshaler_FixedArray*)pFM)->GetElementTypeHandle().GetMethodTable();
        DefineFullyQualifiedNameForClassW();
        WCHAR* szClassName = (WCHAR*)GetFullyQualifiedNameForClassW(pMT->GetClass());

        if (fElementTypeUserDefined)
        {
            strNStructFieldType.Printf(L"fixed array of %s exposed as %s elements (array size = %i bytes)",
                                       szClassName,
                                       strVarType.GetUnicode(), pFM->NativeSize());
        }
        else
        {
            strNStructFieldType.Printf(L"fixed array of %s (array size = %i bytes)", 
                szClassName, pFM->NativeSize());
        }

        return;
    }
    else if (cls == FieldMarshaler::CLASS_NESTED_LAYOUT_CLASS)
    {
        MethodTable *pMT = ((FieldMarshaler_NestedLayoutClass*)pFM)->GetMethodTable();
        DefineFullyQualifiedNameForClassW();
        strNStructFieldType.Printf(L"nested layout class %s",
                                   GetFullyQualifiedNameForClassW(pMT->GetClass()));
        return;
    }
    else if (cls == FieldMarshaler::CLASS_NESTED_VALUE_CLASS)
    {
        MethodTable     *pMT                = ((FieldMarshaler_NestedValueClass*)pFM)->GetMethodTable();
        DefineFullyQualifiedNameForClassW();
        strNStructFieldType.Printf(L"nested value class %s",
                                   GetFullyQualifiedNameForClassW(pMT->GetClass()));
        return;
    }
    else if (cls == FieldMarshaler::CLASS_COPY1)
    {
        // The following CorElementTypes are the only ones handled with FieldMarshaler_Copy1. 
        switch (elemType)
        {
            case ELEMENT_TYPE_I1:
                strRetVal = L"SByte";
                break;

            case ELEMENT_TYPE_U1:
                strRetVal = L"Byte";
                break;

            default:
                strRetVal = L"Unknown";
                break;
        }
    }
    else if (cls == FieldMarshaler::CLASS_COPY2)
    {
        // The following CorElementTypes are the only ones handled with FieldMarshaler_Copy2. 
        switch (elemType)
        {
            case ELEMENT_TYPE_CHAR:
                strRetVal = L"Unicode char";
                break;

            case ELEMENT_TYPE_I2:
                strRetVal = L"Int16";
                break;

            case ELEMENT_TYPE_U2:
                strRetVal = L"UInt16";
                break;

            default:
                strRetVal = L"Unknown";
                break;
        }
    }
    else if (cls == FieldMarshaler::CLASS_COPY4)
    {
        // The following CorElementTypes are the only ones handled with FieldMarshaler_Copy4. 
        switch (elemType)
        {
            // At this point, ELEMENT_TYPE_I must be 4 bytes long.  Same for ELEMENT_TYPE_U.
            case ELEMENT_TYPE_I:
            case ELEMENT_TYPE_I4:
                strRetVal = L"Int32";
                break;

            case ELEMENT_TYPE_U:
            case ELEMENT_TYPE_U4:
                strRetVal = L"UInt32";
                break;

            case ELEMENT_TYPE_R4:
                strRetVal = L"Single";
                break;

            case ELEMENT_TYPE_PTR:
                strRetVal = L"4-byte pointer";
                break;

            default:
                strRetVal = L"Unknown";
                break;
        }
    }
    else if (cls == FieldMarshaler::CLASS_COPY8)
    {
        // The following CorElementTypes are the only ones handled with FieldMarshaler_Copy8. 
        switch (elemType)
        {
            // At this point, ELEMENT_TYPE_I must be 8 bytes long.  Same for ELEMENT_TYPE_U.
            case ELEMENT_TYPE_I:
            case ELEMENT_TYPE_I8:
                strRetVal = L"Int64";
                break;

            case ELEMENT_TYPE_U:
            case ELEMENT_TYPE_U8:
                strRetVal = L"UInt64";
                break;

            case ELEMENT_TYPE_R8:
                strRetVal = L"Double";
                break;

            case ELEMENT_TYPE_PTR:
                strRetVal = L"8-byte pointer";
                break;

            default:
                strRetVal = L"Unknown";
                break;
        }
    }
    else if (cls == FieldMarshaler::CLASS_FIXED_STRING_UNI)
    {
        int nativeSize = pFM->NativeSize();
        int strLength = nativeSize / sizeof(WCHAR);

        strNStructFieldType.Printf(L"embedded LPWSTR (length %d)", strLength);
        
        return;
    }
    else if (cls == FieldMarshaler::CLASS_FIXED_STRING_ANSI)
    {
        int nativeSize = pFM->NativeSize();
        int strLength = nativeSize / sizeof(CHAR);

        strNStructFieldType.Printf(L"embedded LPSTR (length %d)", strLength);

        return;
    }
    else
    {
        // All other NStruct Field Types which do not require special handling.
        switch (cls)
        {
            case FieldMarshaler::CLASS_STRING_UNI:
                strRetVal = L"LPWSTR";
                break;
            case FieldMarshaler::CLASS_STRING_ANSI:
                strRetVal = L"LPSTR";
                break;
            case FieldMarshaler::CLASS_DELEGATE:
                strRetVal = L"Delegate";
                break;
            case FieldMarshaler::CLASS_ANSI:
                strRetVal = L"ANSI char";
                break;
            case FieldMarshaler::CLASS_WINBOOL:
                strRetVal = L"Windows Bool";
                break;
            case FieldMarshaler::CLASS_CBOOL:
                strRetVal = L"CBool";
                break;
            case FieldMarshaler::CLASS_DECIMAL:
                strRetVal = L"DECIMAL";
                break;
            case FieldMarshaler::CLASS_DATE:
                strRetVal = L"DATE";
                break;
            case FieldMarshaler::CLASS_ILLEGAL:
                strRetVal = L"illegal type";
                break;
            case FieldMarshaler::CLASS_SAFEHANDLE:
                strRetVal = L"SafeHandle";
                break;
            case FieldMarshaler::CLASS_CRITICALHANDLE:
                strRetVal = L"CriticalHandle";
                break;
            default:
                strRetVal = L"<UNKNOWN>";
                break;
        }
    }

    strNStructFieldType.Set(strRetVal);

    return;
}

