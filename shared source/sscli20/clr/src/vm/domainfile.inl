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

inline Module* DomainFile::GetCurrentModule() 
{ 
    LEAF_CONTRACT; 
    return m_pModule; 
}

inline Module* DomainFile::GetLoadedModule() 
{ 
    LEAF_CONTRACT; 
    CONSISTENCY_CHECK(CheckLoaded());
    return m_pModule;
}

inline Module* DomainFile::GetModule()
{ 
    WRAPPER_CONTRACT; 
    CONSISTENCY_CHECK(CheckLoadLevel(FILE_LOAD_ALLOCATE));
    return m_pModule; 
}

inline Assembly* DomainAssembly::GetCurrentAssembly()
{
    LEAF_CONTRACT;
    return m_pAssembly;
}

inline Assembly* DomainAssembly::GetLoadedAssembly()
{
    LEAF_CONTRACT;
	CONSISTENCY_CHECK(CheckLoaded());
    return m_pAssembly;
}

inline Assembly* DomainAssembly::GetAssembly()
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(CheckLoadLevel(FILE_LOAD_ALLOCATE));
    return m_pAssembly;
}

inline void DomainFile::UpdatePEFile(PTR_PEFile pFile)
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(CheckPointer(pFile));
    if (pFile==m_pFile)
        return;
    _ASSERTE(m_pOriginalFile==NULL);
    m_pOriginalFile=m_pFile;
    pFile->AddRef();
    m_pFile=pFile;
}


