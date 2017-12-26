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
// ===========================================================================
// File: common.h
//
// Inlinable functions
//
// ===========================================================================

#ifndef __common_h__
#define __common_h__


__forceinline void MODULESYM::Init(int iscope, IMetaDataImport2 * metaimport)
{
    ASSERT(parent->isINFILESYM());
    ASSERT(this->metaimportV2 == NULL);
    ASSERT(this->assemimport == NULL);

    this->iscope = iscope;
    this->metaimportV2 = metaimport;
    this->assemimport = NULL;
    if (metaimportV2) {
        this->metaimportV2->AddRef();
        HRESULT hr; hr = metaimportV2->QueryInterface(IID_IMetaDataAssemblyImport, (void**)&this->assemimport);
        ASSERT(SUCCEEDED(hr));       
    }
}

__forceinline IMetaDataImport2 * MODULESYM::GetMetaImportV2(COMPILER *compiler)
{
    ASSERT(parent->isINFILESYM());
    ASSERT(this->metaimportV2);
    return this->metaimportV2;
}

__forceinline IMetaDataImport * MODULESYM::GetMetaImport(COMPILER *compiler)
{
    ASSERT(parent->isINFILESYM());
    ASSERT(this->metaimportV2);
    return this->metaimportV2;
}

__forceinline IMetaDataAssemblyImport * MODULESYM::GetAssemblyImport(COMPILER *compiler)
{
    ASSERT(parent->isINFILESYM());
    ASSERT(this->assemimport);
    return this->assemimport;
}

__forceinline void MODULESYM::Clear()
{
    ASSERT(parent->isINFILESYM());
    if (this->metaimportV2) {
        this->metaimportV2->Release(); 
        this->metaimportV2 = NULL;
    }
    if (this->assemimport) {
        this->assemimport->Release(); 
        this->assemimport = NULL;
    }
}



#endif // __common_h__
