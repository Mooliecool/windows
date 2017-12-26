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
// --------------------------------------------------------------------------------
// PEImageLayout.h
// --------------------------------------------------------------------------------

#ifndef PEIMAGELAYOUT_H_
#define PEIMAGELAYOUT_H_

// --------------------------------------------------------------------------------
// Required headers
// --------------------------------------------------------------------------------

#include "clrtypes.h"
#include "pedecoder.h"
#include "holder.h"

// --------------------------------------------------------------------------------
// Forward declarations
// --------------------------------------------------------------------------------

class Crst;
class PEImage;



typedef VPTR(class PEImageLayout) PTR_PEImageLayout;

class PEImageLayout : public PEDecoder
{
    VPTR_BASE_CONCRETE_VTABLE_CLASS(PEImageLayout)
    friend class PEModule;
    friend class CCLRDebugManager;
public:
    // ------------------------------------------------------------
    // Public constants
    // ------------------------------------------------------------
    enum 
    {
        LAYOUT_MAPPED =1,
        LAYOUT_FLAT =2,
        LAYOUT_LOADED =4,
        LAYOUT_LOADED_FOR_INTROSPECTION =8,
        LAYOUT_ANY =0xf
    };


public:
#ifndef DACCESS_COMPILE
    static PEImageLayout* CreateFlat(const void *flat, COUNT_T size,PEImage* pOwner);
    static PEImageLayout* CreateFromStream(IStream* pIStream, PEImage* pOwner);
    static PEImageLayout* CreateFromHMODULE(HMODULE mappedbase,PEImage* pOwner, BOOL bTakeOwnership);
    static PEImageLayout* LoadFromFlat(PEImageLayout* pflatimage);
    static PEImageLayout* Load(PEImage* pOwner, BOOL bNTSafeLoad);
    static PEImageLayout* LoadFlat(HANDLE hFile, PEImage* pOwner);
    static PEImageLayout* Map (HANDLE hFile, PEImage* pOwner);
#endif    
    PEImageLayout();
    virtual ~PEImageLayout();
    static void Startup();
    static CHECK CheckStartup();
    static BOOL CompareBase(UPTR path, UPTR mapping);
    
    // Refcount above images.
    void AddRef();
    ULONG Release();
    const SString& GetPath();
    virtual BOOL IsLoadLibraryHandle();
    void MakePersistent();
    BOOL IsPersistent();    


public:
#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

private:
    volatile LONG m_refCount;
public:    
    PEImage* m_pOwner;

    DWORD m_Layout;
    BOOL      m_bPersistent;

};

FORCEINLINE void PEImageLayoutRelease(PEImageLayout *i)
{
    WRAPPER_CONTRACT;
    i->Release();
}

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(PEImageLayout*);
#endif
typedef Wrapper<PEImageLayout *, DoNothing, PEImageLayoutRelease> PEImageLayoutHolder;


//RawImageView is built on external data, does not need cleanup
class RawImageLayout: public PEImageLayout
{
    VPTR_VTABLE_CLASS(RawImageLayout,PEImageLayout)
protected:
    CLRMapViewHolder m_DataCopy;
public:
    RawImageLayout(const void *flat, COUNT_T size,PEImage* pOwner);    
    RawImageLayout(const void *mapped, PEImage* pOwner, BOOL bTakeOwnerShip, BOOL bFixedUp);    
};

// ConvertedImageView is for the case when we manually layout a flat image
class ConvertedImageLayout: public PEImageLayout
{
    VPTR_VTABLE_CLASS(ConvertedImageLayout,PEImageLayout)
protected:
    HandleHolder m_FileMap;  
    CLRMapViewHolder m_FileView;
public:
#ifndef DACCESS_COMPILE    
    ConvertedImageLayout(PEImageLayout* source);    
#endif
};

class MappedImageLayout: public PEImageLayout
{
    VPTR_VTABLE_CLASS(MappedImageLayout,PEImageLayout)
    VPTR_UNIQUE(0x15)
protected:
    HandleHolder m_FileMap;
    CLRMapViewHolder m_FileView;
public:
#ifndef DACCESS_COMPILE    
    MappedImageLayout(HANDLE hFile, PEImage* pOwner);    
#endif

};


class FlatImageLayout: public PEImageLayout
{
    VPTR_VTABLE_CLASS(FlatImageLayout,PEImageLayout)
    VPTR_UNIQUE(0x59)
protected:
    HandleHolder m_FileMap;
    CLRMapViewHolder m_FileView;
public:
#ifndef DACCESS_COMPILE    
    FlatImageLayout(HANDLE hFile, PEImage* pOwner);   
#endif

};

class StreamImageLayout: public PEImageLayout
{
    VPTR_VTABLE_CLASS(StreamImageLayout,PEImageLayout)
    VPTR_UNIQUE(0x71)
protected:
    HandleHolder m_FileMap;
    CLRMapViewHolder m_FileView;
public:
#ifndef DACCESS_COMPILE    
    StreamImageLayout(IStream* pIStream,PEImage* pOwner);   
#endif
};



#endif  // PEIMAGELAYOUT_H_

