//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Header to create binding between Core BCSYM_Hash and HostedCompiler
//
//-------------------------------------------------------------------------------------------------

#pragma once

#if HOSTED

enum HostedLookup
{
    HostedUnknown = 0,
    HostedType = 1,
    HostedVariable = 2,
    HostedExtension = 3,
};

class IVbHostedSymbolSource
{
public:
    virtual BCSYM_NamedRoot* GetSymbolForName(const STRING* Name) = 0;
    virtual void ImportTypeChildren(BCSYM_Container* pContainer) = 0;
};

class VBHostIntegration
{
public:
    static void ImportTypeChildren(BCSYM_Container* pContainer);
};

#endif
