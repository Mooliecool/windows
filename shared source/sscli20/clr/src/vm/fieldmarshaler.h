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
// FieldMarshaler.H -
//
// FieldMarshalers are used to allow CLR programs to allocate and access
// native structures for interop purposes. FieldMarshalers are actually normal GC
// objects with a class, but instead of keeping fields in the GC object,
// it keeps a hidden pointer to a fixed memory block (which may have been
// allocated by a third party.) Field accesses to FieldMarshalers are redirected
// to this fixed block.
//

#ifndef __FieldMarshaler_h__
#define __FieldMarshaler_h__


#include "util.hpp"
#include "mlinfo.h"
#include "comvariant.h"
#include "eeconfig.h"
#include "olevariant.h"



// Forward refernces
class EEClassLayoutInfo;
class FieldDesc;
class MethodTable;

class FieldMarshaler_NestedLayoutClass;
class FieldMarshaler_NestedValueClass;
class FieldMarshaler_StringUni;
class FieldMarshaler_StringAnsi;
class FieldMarshaler_FixedStringUni;
class FieldMarshaler_FixedStringAnsi;
class FieldMarshaler_FixedArray;
class FieldMarshaler_FixedCharArrayAnsi;
class FieldMarshaler_Delegate;
class FieldMarshaler_Illegal;
class FieldMarshaler_Copy1;
class FieldMarshaler_Copy2;
class FieldMarshaler_Copy4;
class FieldMarshaler_Copy8;
class FieldMarshaler_Ansi;
class FieldMarshaler_WinBool;
class FieldMarshaler_CBool;
class FieldMarshaler_Decimal;
class FieldMarshaler_Date;

VOID NStructFieldTypeToString(FieldMarshaler* pFM, SString& strNStructFieldType);

//=======================================================================
// Each possible COM+/Native pairing of data type has a
// NLF_* id. This is used to select the marshaling code.
//=======================================================================
#undef DEFINE_NFT
#define DEFINE_NFT(name, nativesize) name,
enum NStructFieldType
{
#include "nsenums.h"
    NFT_COUNT
};


//=======================================================================
// Magic number for default struct packing size.
//=======================================================================
#define DEFAULT_PACKING_SIZE 8


//=======================================================================
// This is invoked from the class loader while building the data structures for a type.
// This function checks if explicit layout metadata exists.
//
// Returns:
//  TRUE    - yes, there's layout metadata
//  FALSE   - no, there's no layout.
//  fail    - throws a typeload exception
//
// If S_OK,
//   *pNLType            gets set to nltAnsi or nltUnicode
//   *pPackingSize       declared packing size
//   *pfExplicitoffsets  offsets explicit in metadata or computed?
//=======================================================================
BOOL HasLayoutMetadata(Assembly* pAssembly, IMDInternalImport *pInternalImport, mdTypeDef cl, 
                            MethodTable *pParentMT, BYTE *pPackingSize, BYTE *pNLTType,
                            BOOL *pfExplicitOffsets);


//=======================================================================
// This function returns TRUE if the type passed in is either a value class or a class and if it has layout information 
// and is marshalable. In all other cases it will return FALSE. 
//=======================================================================
BOOL IsStructMarshalable(TypeHandle th);

//=======================================================================
// The classloader stores an intermediate representation of the layout
// metadata in an array of these structures. The dual-pass nature
// is a bit extra overhead but building this structure requiring loading
// other classes (for nested structures) and I'd rather keep this
// next to the other places where we load other classes (e.g. the superclass
// and implemented interfaces.)
//
// Each redirected field gets one entry in LayoutRawFieldInfo.
// The array is terminated by one dummy record whose m_MD == mdMemberDefNil.
// WARNING!! Before you change this struct see the comment above the m_FieldMarshaler field
//=======================================================================
struct LayoutRawFieldInfo
{
    mdFieldDef  m_MD;             // mdMemberDefNil for end of array
    UINT8       m_nft;            // NFT_* value
    UINT32      m_offset;         // native offset of field
    UINT32      m_cbNativeSize;   // native size of field in bytes
    ULONG       m_sequence;       // sequence # from metadata
    BOOL        m_fIsOverlapped;


    //----- Post v1.0 addition: The LayoutKind.Sequential attribute now affects managed layout as well.
    //----- So we need to keep a parallel set of layout data for the managed side. The Size and AlignmentReq
    //----- is redundant since we can figure it out from the sig but since we're already accessing the sig
    //----- in ParseNativeType, we might as well capture it at that time.
    UINT32      m_managedSize;    // managed size of field
    UINT32      m_managedAlignmentReq; // natural alignment of field
    UINT32      m_managedOffset;  // managed offset of field
    UINT32      m_pad;            // needed to keep m_FieldMarshaler 8-byte aligned

    // WARNING!
    // We in-place create a field marshaler in the following
    // memory, so keep it 8-byte aligned or 
    // the vtable pointer initialization will cause a 
    // misaligned memory write on IA64.
    // The entire struct's size must also be multiple of 8 bytes
    struct
    {
        private:
            char m_space[MAXFIELDMARSHALERSIZE];
    } m_FieldMarshaler;
};


//=======================================================================
// 
//=======================================================================

VOID LayoutUpdateNative(LPVOID *ppProtectedManagedData, UINT offsetbias, MethodTable *pMT, BYTE* pNativeData, CleanupWorkList *pOptionalCleanupWorkList);
VOID LayoutUpdateCLR(LPVOID *ppProtectedManagedData, UINT offsetbias, MethodTable *pMT, BYTE *pNativeData, BOOL fDeleteNativeCopies);
VOID LayoutDestroyNative(LPVOID pNative, MethodTable *pMT);

VOID FmtClassUpdateNative(OBJECTREF *ppProtectedManagedData, BYTE *pNativeData, CleanupWorkList *pOptionalCleanupWorkList);
VOID FmtClassUpdateCLR(OBJECTREF *ppProtectedManagedData, BYTE *pNativeData, BOOL fDeleteOld);
VOID FmtClassDestroyNative(LPVOID pNative, MethodTable *pMT);

VOID FmtValueTypeUpdateNative(LPVOID pProtectedManagedData, MethodTable *pMT, BYTE *pNativeData, CleanupWorkList *pOptionalCleanupWorkList);
VOID FmtValueTypeUpdateCLR(LPVOID pProtectedManagedData, MethodTable *pMT, BYTE *pNativeData, BOOL fDeleteOld);


//=======================================================================
// Abstract base class. Each type of NStruct reference field extends
// this class and implements the necessary methods.
//
//   UpdateNative
//       - this method receives a COM+ field value and a pointer to
//         native field inside the fixed portion. it should marshal
//         the COM+ value to a new native instance and store it
//         inside *pNativeValue. Do not destroy the value you overwrite
//         in *pNativeValue.
//
//         may throw COM+ exceptions
//
//   UpdateCLR
//       - this method receives a read-only pointer to the native field inside
//         the fixed portion. it should marshal the native value to
//         a new CLR instance and store it in *ppCLRValue.
//         (the caller keeps *ppCLRValue gc-protected.)
//
//         may throw CLR exceptions
//
//   DestroyNative
//       - should do the type-specific deallocation of a native instance.
//         if the type has a "NULL" value, this method should
//         overwrite the field with this "NULL" value (whether or not
//         it does, however, it's considered a bug to depend on the
//         value left over after a DestroyNative.)
//
//         must NOT throw a CLR exception
//
//   NativeSize
//       - returns the size, in bytes, of the native version of the field.
//
//   AlignmentRequirement
//       - returns one of 1,2,4 or 8; indicating the "natural" alignment
//         of the native field. In general,
//
//            for scalars, the AR is equal to the size
//            for arrays,  the AR is that of a single element
//            for structs, the AR is that of the member with the largest AR
//
//
//=======================================================================



//=======================================================================
//
// FieldMarshaler's are constructed in place and replicated via bit-wise
// copy, so you can't have a destructor. Make sure you don't define a 
// destructor in derived classes!!
// We used to enforce this by defining a private destructor, by the C++
// compiler doesn't allow that anymore.
//
//=======================================================================

class FieldMarshaler
{
public:    
    enum Class
    {
        CLASS_NESTED_LAYOUT_CLASS,
        CLASS_NESTED_VALUE_CLASS,
        CLASS_STRING_UNI,
        CLASS_STRING_ANSI,
        CLASS_FIXED_STRING_UNI,
        CLASS_FIXED_STRING_ANSI,
        CLASS_FIXED_CHAR_ARRAY_ANSI,
        CLASS_FIXED_ARRAY,
        CLASS_DELEGATE,
        CLASS_ILLEGAL,
        CLASS_COPY1,
        CLASS_COPY2,
        CLASS_COPY4,
        CLASS_COPY8,
        CLASS_ANSI,
        CLASS_WINBOOL,
        CLASS_CBOOL,
        CLASS_DATE,
        CLASS_DECIMAL,
        CLASS_INTERFACE,
        CLASS_SAFEHANDLE,
        CLASS_CRITICALHANDLE,
    };
    
    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const = 0;
    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const = 0;
    virtual VOID DestroyNative(LPVOID pNativeValue) const 
    {
        LEAF_CONTRACT;
    }
    
    virtual UINT32 NativeSize() const = 0;
    virtual UINT32 AlignmentRequirement() const = 0;

    virtual BOOL IsScalarMarshaler() const
    {
        LEAF_CONTRACT; 
        return FALSE;
    }

    virtual BOOL IsNestedValueClassMarshaler() const
    {
        LEAF_CONTRACT; 
        return FALSE;
    }

    virtual VOID ScalarUpdateNative(LPVOID pCLR, LPVOID pNative) const
    {
        LEAF_CONTRACT; 
        _ASSERTE(!"Not supposed to get here.");
    }
    
    virtual VOID ScalarUpdateCLR(const VOID *pNative, LPVOID pCLR) const
    {
         LEAF_CONTRACT; 
       _ASSERTE(!"Not supposed to get here.");
    }

    virtual VOID NestedValueClassUpdateNative(const VOID **ppProtectedCLR, UINT startoffset, LPVOID pNative, CleanupWorkList *pOptionalCleanupWorkList) const
    {
        LEAF_CONTRACT; 
        _ASSERTE(!"Not supposed to get here.");
    }
    
    virtual VOID NestedValueClassUpdateCLR(const VOID *pNative, LPVOID *ppProtectedCLR, UINT startoffset) const
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    // 
    // Methods for saving & restoring in prejitted images:
    //
    virtual Class GetClass() const = 0;


    virtual void Restore()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

        return;
    }

    void SetFieldDesc(FieldDesc* pFD)
    {
        LEAF_CONTRACT;
        m_pFD = pFD;
    }

    FieldDesc* GetFieldDesc()
    {
        CONTRACT (FieldDesc*)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            SO_TOLERANT;
            POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        }
        CONTRACT_END;

        RETURN m_pFD;
    }

    void SetExternalOffset(UINT32 dwExternalOffset)
    {
        LEAF_CONTRACT;
        m_dwExternalOffset = dwExternalOffset;
    }

    UINT32 GetExternalOffset()
    {
        LEAF_CONTRACT;
        return m_dwExternalOffset;
    }
    
protected:
    FieldMarshaler()
    {
        LEAF_CONTRACT;
        
#ifdef _DEBUG
        m_pFD = (FieldDesc*)(size_t)INVALID_POINTER_CC;
        m_dwExternalOffset = 0xcccccccc;
#endif
    }


    FieldDesc*       m_pFD;                // FieldDesc
    UINT32           m_dwExternalOffset;   // offset of field in the fixed portion
};






//=======================================================================
// Embedded struct <--> LayoutClass
//=======================================================================
class FieldMarshaler_NestedLayoutClass : public FieldMarshaler
{
public:
    FieldMarshaler_NestedLayoutClass(MethodTable *pMT)
    {
        WRAPPER_CONTRACT;
        m_pNestedMethodTable = pMT;
    }

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const ;
    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const ;
    virtual VOID DestroyNative(LPVOID pNativeValue) const;

    virtual UINT32 NativeSize() const;
    virtual UINT32 AlignmentRequirement() const;

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_NESTED_LAYOUT_CLASS;
    }
    

    MethodTable *GetMethodTable() const
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(IsRestored());
        }
        CONTRACTL_END;

        return m_pNestedMethodTable;
    }

private:
    // MethodTable of nested FieldMarshaler.
    MethodTable*    m_pNestedMethodTable;
};


//=======================================================================
// Embedded struct <--> ValueClass
//=======================================================================
class FieldMarshaler_NestedValueClass : public FieldMarshaler
{
public:
    FieldMarshaler_NestedValueClass(MethodTable *pMT)
    {
        WRAPPER_CONTRACT;
        m_pNestedMethodTable = pMT;
    }

    virtual BOOL IsNestedValueClassMarshaler() const
    {
        LEAF_CONTRACT;
        return TRUE;
    }


    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual VOID DestroyNative(LPVOID pNativeValue) const;

    virtual UINT32 NativeSize() const;
    virtual UINT32 AlignmentRequirement() const;
    virtual VOID NestedValueClassUpdateNative(const VOID **ppProtectedCLR, UINT startoffset, LPVOID pNative, CleanupWorkList *pOptionalCleanupWorkList) const;
    virtual VOID NestedValueClassUpdateCLR(const VOID *pNative, LPVOID *ppProtectedCLR, UINT startoffset) const;

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_NESTED_VALUE_CLASS;
    }


    BOOL IsBlittable()
    {
        WRAPPER_CONTRACT;
        return GetMethodTable()->IsBlittable();
    }

    MethodTable *GetMethodTable() const
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(IsRestored());
        }
        CONTRACTL_END;

        return m_pNestedMethodTable;
    }


private:
    // MethodTable of nested NStruct.
    MethodTable*    m_pNestedMethodTable;
};


//=======================================================================
// LPWSTR <--> System.String
//=======================================================================
class FieldMarshaler_StringUni : public FieldMarshaler
{
public:

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const ;
    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const ;
    virtual VOID DestroyNative(LPVOID pNativeValue) const;

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return sizeof(LPWSTR);
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return sizeof(LPWSTR);
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_STRING_UNI;
    }
};


//=======================================================================
// LPSTR <--> System.String
//=======================================================================
class FieldMarshaler_StringAnsi : public FieldMarshaler
{
public:
    FieldMarshaler_StringAnsi(BOOL BestFit, BOOL ThrowOnUnmappableChar) : 
        m_BestFitMap(!!BestFit), m_ThrowOnUnmappableChar(!!ThrowOnUnmappableChar)
    {
        WRAPPER_CONTRACT;
    }

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const ;
    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const ;
    virtual VOID DestroyNative(LPVOID pNativeValue) const;

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return sizeof(LPSTR);
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return sizeof(LPSTR);
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_STRING_ANSI;
    }
    
    BOOL GetBestFit()
    {
        LEAF_CONTRACT;
        return m_BestFitMap;
    }
    
    BOOL GetThrowOnUnmappableChar()
    {
        LEAF_CONTRACT;
        return m_ThrowOnUnmappableChar;
    }
    
private:
    bool m_BestFitMap:1;
    bool m_ThrowOnUnmappableChar:1;
};


//=======================================================================
// Embedded LPWSTR <--> System.String
//=======================================================================
class FieldMarshaler_FixedStringUni : public FieldMarshaler
{
public:

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const ;
    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const ;

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return m_numchar * sizeof(WCHAR);
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return sizeof(WCHAR);
    }

    FieldMarshaler_FixedStringUni(UINT32 numChar)
    {
        WRAPPER_CONTRACT;
        m_numchar = numChar;
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_FIXED_STRING_UNI;
    }
    
private:
    // # of characters for fixed strings
    UINT32           m_numchar;
};


//=======================================================================
// Embedded LPSTR <--> System.String
//=======================================================================
class FieldMarshaler_FixedStringAnsi : public FieldMarshaler
{
public:
    FieldMarshaler_FixedStringAnsi(BOOL BestFitMap, BOOL ThrowOnUnmappableChar) :
        m_BestFitMap(!!BestFitMap), m_ThrowOnUnmappableChar(!!ThrowOnUnmappableChar)
    {
        LEAF_CONTRACT;
    }

    FieldMarshaler_FixedStringAnsi(UINT32 numChar, BOOL BestFitMap, BOOL ThrowOnUnmappableChar) :
        m_numchar(numChar), m_BestFitMap(!!BestFitMap), m_ThrowOnUnmappableChar(!!ThrowOnUnmappableChar)
    {
        WRAPPER_CONTRACT;
    }

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const ;
    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const ;

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return m_numchar * sizeof(CHAR);
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return sizeof(CHAR);
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_FIXED_STRING_ANSI;
    }
    
    BOOL GetBestFit()
    {
        LEAF_CONTRACT;
        return m_BestFitMap;
    }
    
    BOOL GetThrowOnUnmappableChar()
    {
        LEAF_CONTRACT;
        return m_ThrowOnUnmappableChar;
    }
    
private:
    // # of characters for fixed strings
    UINT32           m_numchar;
    bool             m_BestFitMap:1;
    bool             m_ThrowOnUnmappableChar:1;
};


//=======================================================================
// Embedded AnsiChar array <--> char[]
//=======================================================================
class FieldMarshaler_FixedCharArrayAnsi : public FieldMarshaler
{
public:
    FieldMarshaler_FixedCharArrayAnsi(UINT32 numElems, BOOL BestFit, BOOL ThrowOnUnmappableChar) :
        m_numElems(numElems), m_BestFitMap(!!BestFit), m_ThrowOnUnmappableChar(!!ThrowOnUnmappableChar)
    {
        WRAPPER_CONTRACT;
    }

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const ;
    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const ;

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return m_numElems * sizeof(CHAR);
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return sizeof(CHAR);
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_FIXED_CHAR_ARRAY_ANSI;
    }

    BOOL GetBestFit()
    {
        LEAF_CONTRACT;
        return m_BestFitMap;
    }
    
    BOOL GetThrowOnUnmappableChar()
    {
        LEAF_CONTRACT;
        return m_ThrowOnUnmappableChar;
    }
    
private:
    // # of elements for fixedchararray
    UINT32           m_numElems;
    bool             m_BestFitMap:1;
    bool             m_ThrowOnUnmappableChar:1;
};


//=======================================================================
// Embedded arrays
//=======================================================================
class FieldMarshaler_FixedArray : public FieldMarshaler
{
public:
    FieldMarshaler_FixedArray(IMDInternalImport *pMDImport, mdTypeDef cl, UINT32 numElems, VARTYPE vt, MethodTable* pElementMT);
    
    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const ;
    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const ;
    virtual UINT32 AlignmentRequirement() const;

    UINT32 NativeSize() const
    {
        WRAPPER_CONTRACT;

        MethodTable *pElementMT = m_arrayType.AsArray()->GetArrayElementTypeHandle().GetMethodTable();
        return OleVariant::GetElementSizeForVarType(m_vt, pElementMT) * m_numElems;
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_FIXED_ARRAY;
    }

    TypeHandle GetElementTypeHandle() const
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(IsRestored());
        }
        CONTRACTL_END;
        
        return m_arrayType;
    }
    
    VARTYPE GetElementVT() const
    {
        LEAF_CONTRACT;
        return m_vt;
    }

    
private:
    TypeHandle       m_arrayType;
    UINT32           m_numElems;
    VARTYPE          m_vt;
    bool             m_BestFitMap:1; // Note: deliberately use small bools to save on working set - this is the largest FieldMarshaler and dominates the cost of the FieldMarshaler array
    bool             m_ThrowOnUnmappableChar:1; // Note: deliberately use small bools to save on working set - this is the largest FieldMarshaler and dominates the cost of the FieldMarshaler array
};




//=======================================================================
// Embedded function ptr <--> Delegate (note: function ptr must have
// come from delegate!!!)
//=======================================================================
class FieldMarshaler_Delegate : public FieldMarshaler
{
public:
    FieldMarshaler_Delegate(MethodTable* pMT)
    {
        WRAPPER_CONTRACT;
        m_pNestedMethodTable = pMT;
    }

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const ;
    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const ;

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return sizeof(LPVOID);
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return sizeof(LPVOID);
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_DELEGATE;
    }


    MethodTable *GetMethodTable() const
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(IsRestored());
        }
        CONTRACTL_END;

        return m_pNestedMethodTable;
    }

    MethodTable* m_pNestedMethodTable;
};


//=======================================================================
// Embedded SafeHandle <--> Handle. This field really only supports
// going from managed to unmanaged. In the other direction, 
//=======================================================================
class FieldMarshaler_SafeHandle : public FieldMarshaler
{
public:

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const ;
    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const ;

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return sizeof(LPVOID);
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return sizeof(LPVOID);
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_SAFEHANDLE;
    }
};


//=======================================================================
// Embedded CriticalHandle <--> Handle. This field really only supports
// going from managed to unmanaged. In the other direction, 
//=======================================================================
class FieldMarshaler_CriticalHandle : public FieldMarshaler
{
public:

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const ;
    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const ;

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return sizeof(LPVOID);
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return sizeof(LPVOID);
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_CRITICALHANDLE;
    }
};


//=======================================================================
// COM IP <--> Interface
//=======================================================================
class FieldMarshaler_Interface : public FieldMarshaler
{
public:

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const ;
    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const ;
    virtual VOID DestroyNative(LPVOID pNativeValue) const;

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return sizeof(IUnknown*);
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return sizeof(IUnknown*);
    }

    FieldMarshaler_Interface(MethodTable *pClassMT, MethodTable *pItfMT)
    {
        WRAPPER_CONTRACT;
        m_pClassMT = pClassMT;
        m_pItfMT = pItfMT;
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_INTERFACE;
    }



    MethodTable *GetMethodTable() const
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(IsRestored());
        }
        CONTRACTL_END;

        return m_pClassMT;
    }

    MethodTable *GetInterfaceMethodTable() const
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(IsRestored());
        }
        CONTRACTL_END;

        return m_pItfMT;
    }

private:
    MethodTable*    m_pClassMT;
    MethodTable*    m_pItfMT;
};






//=======================================================================
// Dummy marshaler
//=======================================================================
class FieldMarshaler_Illegal : public FieldMarshaler
{
public:
    FieldMarshaler_Illegal(UINT resIDWhy)
    {
        WRAPPER_CONTRACT;
        m_resIDWhy = resIDWhy;
    }

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual VOID ScalarUpdateNative(LPVOID pCLR, LPVOID pNative) const;
    virtual VOID ScalarUpdateCLR(const VOID *pNative, LPVOID pCLR) const;

    virtual BOOL IsScalarMarshaler() const
    {
        LEAF_CONTRACT;
        return TRUE;
    }

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return 1;
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return 1;
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_ILLEGAL;
    }
    
private:
    UINT m_resIDWhy;
};



class FieldMarshaler_Copy1 : public FieldMarshaler
{
public:

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual BOOL IsScalarMarshaler() const
    {
        LEAF_CONTRACT;
        return TRUE;
    }

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return 1;
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return 1;
    }


    virtual VOID ScalarUpdateNative(LPVOID pCLR, LPVOID pNative) const
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR));            
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;
        
        *((U1*)pNative) = *((U1*)pCLR);
    }


    virtual VOID ScalarUpdateCLR(const VOID *pNative, LPVOID pCLR) const
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR));            
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;
        
        *((U1*)pCLR) = *((U1*)pNative);
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_COPY1;
    }
    
};



class FieldMarshaler_Copy2 : public FieldMarshaler
{
public:

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual BOOL IsScalarMarshaler() const
    {
        LEAF_CONTRACT;
        return TRUE;
    }

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return 2;
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return 2;
    }


    virtual VOID ScalarUpdateNative(LPVOID pCLR, LPVOID pNative) const
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR));            
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;
        
        MAYBE_UNALIGNED_WRITE(pNative, 16, MAYBE_UNALIGNED_READ(pCLR, 16));
    }


    virtual VOID ScalarUpdateCLR(const VOID *pNative, LPVOID pCLR) const
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR));            
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;
        
        MAYBE_UNALIGNED_WRITE(pCLR, 16, MAYBE_UNALIGNED_READ(pNative, 16));
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_COPY2;
    }
    
};


class FieldMarshaler_Copy4 : public FieldMarshaler
{
public:

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual BOOL IsScalarMarshaler() const
    {
        LEAF_CONTRACT;
        return TRUE;
    }

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return 4;
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return 4;
    }

    virtual VOID ScalarUpdateNative(LPVOID pCLR, LPVOID pNative) const
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR));            
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;
        
        MAYBE_UNALIGNED_WRITE(pNative, 32, MAYBE_UNALIGNED_READ(pCLR, 32));
    }


    virtual VOID ScalarUpdateCLR(const VOID *pNative, LPVOID pCLR) const
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR));            
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;
        
        MAYBE_UNALIGNED_WRITE(pCLR, 32, MAYBE_UNALIGNED_READ(pNative, 32));
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_COPY4;
    }
    
};


class FieldMarshaler_Copy8 : public FieldMarshaler
{
public:

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual BOOL IsScalarMarshaler() const
    {
        LEAF_CONTRACT;
        return TRUE;
    }

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return 8;
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return 8;
    }


    virtual VOID ScalarUpdateNative(LPVOID pCLR, LPVOID pNative) const
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR));            
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;
        
        MAYBE_UNALIGNED_WRITE(pNative, 64, MAYBE_UNALIGNED_READ(pCLR, 64));
    }


    virtual VOID ScalarUpdateCLR(const VOID *pNative, LPVOID pCLR) const
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR));            
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;
        
        MAYBE_UNALIGNED_WRITE(pCLR, 64, MAYBE_UNALIGNED_READ(pNative, 64));
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_COPY8;
    }
    
};



class FieldMarshaler_Ansi : public FieldMarshaler
{
public:
    FieldMarshaler_Ansi(BOOL BestFitMap, BOOL ThrowOnUnmappableChar) :
        m_BestFitMap(!!BestFitMap), m_ThrowOnUnmappableChar(!!ThrowOnUnmappableChar)
    {
        WRAPPER_CONTRACT;
    }

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual BOOL IsScalarMarshaler() const
    {
        LEAF_CONTRACT;
        return TRUE;
    }

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return sizeof(CHAR);
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return sizeof(CHAR);
    }

    virtual VOID ScalarUpdateNative(LPVOID pCLR, LPVOID pNative) const
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR, NULL_OK));
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;
      
        char c;
        InternalWideToAnsi((LPCWSTR)pCLR,
                           1,
                           &c,
                           1,
                           m_BestFitMap,
                           m_ThrowOnUnmappableChar);
        
        *((char*)pNative) = c;
    }


    virtual VOID ScalarUpdateCLR(const VOID *pNative, LPVOID pCLR) const
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR));
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;
      
        MultiByteToWideChar(CP_ACP, 0, (char*)pNative, 1, (LPWSTR)pCLR, 1);
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_ANSI;
    }

    BOOL GetBestFit()
    {
        LEAF_CONTRACT;
        return m_BestFitMap;
    }
    
    BOOL GetThrowOnUnmappableChar()
    {
        LEAF_CONTRACT;
        return m_ThrowOnUnmappableChar;
    }
    
private:
    bool             m_BestFitMap:1;
    bool             m_ThrowOnUnmappableChar:1;
};



class FieldMarshaler_WinBool : public FieldMarshaler
{
public:

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual BOOL IsScalarMarshaler() const
    {
        LEAF_CONTRACT;
        return TRUE;
    }

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return sizeof(BOOL);
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return sizeof(BOOL);
    }

    virtual VOID ScalarUpdateNative(LPVOID pCLR, LPVOID pNative) const
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR));
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;
        C_ASSERT(sizeof(BOOL) == sizeof(UINT32));
        MAYBE_UNALIGNED_WRITE(pNative, 32, ((*((U1 UNALIGNED*)pCLR)) ? 1 : 0));
    }


    virtual VOID ScalarUpdateCLR(const VOID *pNative, LPVOID pCLR) const
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR));
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;
      
        C_ASSERT(sizeof(BOOL) == sizeof(UINT32));
        *((U1*)pCLR)  = MAYBE_UNALIGNED_READ(pNative, 32) ? 1 : 0;       
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_WINBOOL;
    }
    
};






class FieldMarshaler_CBool : public FieldMarshaler
{
public:

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual BOOL IsScalarMarshaler() const
    {
        LEAF_CONTRACT;
        return TRUE;
    }

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return 1;
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return 1;
    }

    virtual VOID ScalarUpdateNative(LPVOID pCLR, LPVOID pNative) const
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR));
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;

        *((U1*)pNative) = (*((U1*)pCLR)) ? 1 : 0;
    }

    virtual VOID ScalarUpdateCLR(const VOID *pNative, LPVOID pCLR) const
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR));
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;
      
        *((U1*)pCLR) = (*((U1*)pNative)) ? 1 : 0;
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_CBOOL;
    }
    
};


class FieldMarshaler_Decimal : public FieldMarshaler
{
public:

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual BOOL IsScalarMarshaler() const
    {
        LEAF_CONTRACT;
        return TRUE;
    }

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return sizeof(DECIMAL);
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return 8;
    }

    virtual VOID ScalarUpdateNative(LPVOID pCLR, LPVOID pNative) const
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR));
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;
      
        memcpy(pNative, pCLR, sizeof(DECIMAL));
    }

    virtual VOID ScalarUpdateCLR(const VOID *pNative, LPVOID pCLR) const
    {
        CONTRACTL
        {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pCLR));
            PRECONDITION(CheckPointer(pNative));
        }
        CONTRACTL_END;
      
        memcpy(pCLR, pNative, sizeof(DECIMAL));
    }

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_DECIMAL;
    }
    
};

class FieldMarshaler_Date : public FieldMarshaler
{
public:

    virtual VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, CleanupWorkList *pOptionalCleanupWorkList) const
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const 
    {
        LEAF_CONTRACT;
        _ASSERTE(!"Not supposed to get here.");
    }

    virtual BOOL IsScalarMarshaler() const
    {
        LEAF_CONTRACT;
        return TRUE;
    }

    virtual UINT32 NativeSize() const
    {
        LEAF_CONTRACT;
        return sizeof(DATE);
    }

    virtual UINT32 AlignmentRequirement() const
    {
        LEAF_CONTRACT;
        return sizeof(DATE);
    }

    virtual VOID ScalarUpdateNative(LPVOID pCLR, LPVOID pNative) const;
    virtual VOID ScalarUpdateCLR(const VOID *pNative, LPVOID pCLR) const;

    Class GetClass() const
    {
        LEAF_CONTRACT;
        return CLASS_DATE;
    }
    
};





//========================================================================
// Used to ensure that native data is properly deleted in exception cases.
//========================================================================
class NativeLayoutDestroyer
{
public:
    NativeLayoutDestroyer(BYTE* pNativeData, MethodTable* pMT, UINT32 cbSize)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pNativeData));
            PRECONDITION(CheckPointer(pMT));
        }
        CONTRACTL_END;
        
        m_pNativeData = pNativeData;
        m_pMT = pMT;
        m_cbSize = cbSize;
        m_fDestroy = TRUE;
    }

    ~NativeLayoutDestroyer()
    {
        WRAPPER_CONTRACT;

        if (m_fDestroy)
        {
            LayoutDestroyNative(m_pNativeData, m_pMT);
            FillMemory(m_pNativeData, m_cbSize, 0);
        }
    }

    void SuppressRelease()
    {
        m_fDestroy = FALSE;
    }
    
private:
    NativeLayoutDestroyer()
    {
        LEAF_CONTRACT;
    }

    BYTE*       m_pNativeData;
    MethodTable*    m_pMT;
    UINT32      m_cbSize;
    BOOL        m_fDestroy;
};


#endif // __FieldMarshaler_h__
