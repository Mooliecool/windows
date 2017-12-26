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

#include "common.h"
#include "peimagelayout.h"
#include "safegetfilesize.h"
#include "peimagelayout.inl"


#ifndef DACCESS_COMPILE
PEImageLayout* PEImageLayout::CreateFlat(const void *flat, COUNT_T size,PEImage* pOwner)
{
    WRAPPER_CONTRACT;
    return new RawImageLayout(flat,size,pOwner);
}

PEImageLayout* PEImageLayout::CreateFromStream(IStream* pIStream,PEImage* pOwner)
{
    WRAPPER_CONTRACT;
    return new StreamImageLayout(pIStream,pOwner);
}


PEImageLayout* PEImageLayout::CreateFromHMODULE(HMODULE hModule,PEImage* pOwner, BOOL bTakeOwnership)
{
    WRAPPER_CONTRACT;
    return new RawImageLayout(hModule,pOwner,bTakeOwnership,TRUE);
}

PEImageLayout* PEImageLayout::LoadFromFlat(PEImageLayout* pflatimage)
{
    WRAPPER_CONTRACT;
    return new ConvertedImageLayout(pflatimage);
}

PEImageLayout* PEImageLayout::Load(PEImage* pOwner, BOOL bNTSafeLoad)
{
    return PEImageLayout::Map(pOwner->GetFileHandle(), pOwner);
}

PEImageLayout* PEImageLayout::LoadFlat(HANDLE hFile,PEImage* pOwner)
{
    WRAPPER_CONTRACT;
    return new FlatImageLayout(hFile,pOwner);
}

PEImageLayout* PEImageLayout::Map(HANDLE hFile, PEImage* pOwner)
{
    CONTRACT(PEImageLayout*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pOwner));
        POSTCONDITION(CheckPointer(RETVAL));
        POSTCONDITION(RETVAL->CheckFormat());
    }
    CONTRACT_END;
    
    PEImageLayoutHolder pAlloc(new MappedImageLayout(hFile,pOwner));
    if (pAlloc->GetBase()==NULL)
    {
        //cross-platform or a bad image
        PEImageLayoutHolder pFlat(new FlatImageLayout(hFile, pOwner));
        if (!pFlat->CheckFormat())
            ThrowHR(COR_E_BADIMAGEFORMAT);

        pAlloc=new ConvertedImageLayout(pFlat);
    }
    else
        if(!pAlloc->CheckFormat())
            ThrowHR(COR_E_BADIMAGEFORMAT);
    RETURN pAlloc.Extract();    
}



RawImageLayout::RawImageLayout(const void *flat, COUNT_T size,PEImage* pOwner)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    m_pOwner=pOwner;
    m_Layout=LAYOUT_FLAT;
    if (size)
    {
        HandleHolder mapping(WszCreateFileMapping(INVALID_HANDLE_VALUE, NULL, 
                                               PAGE_READWRITE, 0, 
                                               size, NULL));
        if (mapping==NULL)
            ThrowLastError();
        m_DataCopy.Assign(CLRMapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0));
        if(m_DataCopy==NULL)
            ThrowLastError();    
        memcpy(m_DataCopy,flat,size);
        flat=m_DataCopy;
    }
    Init((void*)flat,size);
}
RawImageLayout::RawImageLayout(const void *mapped, PEImage* pOwner, BOOL bTakeOwnership, BOOL bFixedUp)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    m_pOwner=pOwner;
    m_Layout=LAYOUT_MAPPED;
    if (bTakeOwnership)
    {
    _ASSERTE(!"bTakeOwnership Should not be used on FEATURE_PAL");
    }
    IfFailThrow(Init((void*)mapped,(bool)(bFixedUp!=FALSE)));
}

ConvertedImageLayout::ConvertedImageLayout(PEImageLayout* source)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    m_Layout=LAYOUT_LOADED;    
    m_pOwner=source->m_pOwner;
    _ASSERTE(!source->IsMapped());

    if (!source->HasNTHeaders())
        EEFileLoadException::Throw(GetPath(), COR_E_BADIMAGEFORMAT);
    LOG((LF_LOADER, LL_INFO100, "PEImage: Opening manually mapped stream\n"));


    m_FileMap.Assign(WszCreateFileMapping(INVALID_HANDLE_VALUE, NULL, 
                                               PAGE_READWRITE, 0, 
                                               source->GetVirtualSize(), NULL));
    if (m_FileMap == NULL)
        ThrowLastError();
        

    if (m_FileView == NULL)
        m_FileView.Assign(CLRMapViewOfFile(m_FileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0));
    
    if (m_FileView == NULL)
        ThrowLastError();
    
    source->LayoutILOnly(m_FileView, TRUE);
        
    IfFailThrow(Init(m_FileView));
}

MappedImageLayout::MappedImageLayout(HANDLE hFile, PEImage* pOwner)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    m_Layout=LAYOUT_MAPPED;
    m_pOwner=pOwner;
}


FlatImageLayout::FlatImageLayout(HANDLE hFile, PEImage* pOwner)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pOwner));        
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    m_Layout=LAYOUT_FLAT;    
    m_pOwner=pOwner;    
    LOG((LF_LOADER, LL_INFO100, "PEImage: Opening flat %S\n", (LPCWSTR) GetPath()));

    COUNT_T size = SafeGetFileSize(hFile, NULL);
    if (size == 0xffffffff && GetLastError() != NOERROR)
    {
        ThrowLastError();
    }
        
    // It's okay if resource files are length zero
    if (size > 0) 
    {
        m_FileMap.Assign(WszCreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL));
        if (m_FileMap == NULL)
            ThrowLastError();

        m_FileView.Assign(CLRMapViewOfFile(m_FileMap, FILE_MAP_READ, 0, 0, 0));
        if (m_FileView == NULL)
            ThrowLastError();
    }

    Init(m_FileView, size);
}

StreamImageLayout::StreamImageLayout(IStream* pIStream,PEImage* pOwner)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM();); 
    }
    CONTRACTL_END;
    
    m_Layout=LAYOUT_FLAT;
    m_pOwner=pOwner;
    
    STATSTG statStg;
    IfFailThrow(pIStream->Stat(&statStg, STATFLAG_NONAME));
    if (statStg.cbSize.u.HighPart > 0)
        ThrowHR(COR_E_FILELOAD);

    DWORD cbRead = 0;

    // Resources files may have zero length (and would be mapped as FLAT)
    if (statStg.cbSize.u.LowPart) {
         m_FileMap.Assign(WszCreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 
                                                   statStg.cbSize.u.LowPart, NULL));
        if (m_FileMap == NULL)
            ThrowWin32(GetLastError());

        m_FileView.Assign(CLRMapViewOfFile(m_FileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0));
        
        if (m_FileView == NULL)
            ThrowWin32(GetLastError());
        
        HRESULT hr = pIStream->Read(m_FileView, statStg.cbSize.u.LowPart, &cbRead);
        if (hr == S_FALSE)
            hr = COR_E_FILELOAD;

        IfFailThrow(hr);
    }
    Init(m_FileView,(COUNT_T)cbRead);
    
}


#endif //DACESS_COMPILE

#ifdef DACCESS_COMPILE
void
PEImageLayout::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;
    DAC_ENUM_VTHIS();
    EMEM_OUT(("MEM: %p PEFile\n", PTR_HOST_TO_TADDR(this)));
    PEDecoder::EnumMemoryRegions(flags,false);
}
#endif //DACCESS_COMPILE
