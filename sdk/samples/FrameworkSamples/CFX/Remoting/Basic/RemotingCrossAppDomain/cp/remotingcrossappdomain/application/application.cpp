//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------
// This is the main project file for VC++ application project 
// generated using an Application Wizard.

#include "stdafx.h"

using namespace System;
using namespace System::Reflection;
using namespace cli;

int _tmain()
{
    //Create new appDomain
    AppDomain^ newAppDomain = AppDomain::CreateDomain("NewAppDomain");

    // Create remote object in new appDomain via shared interface
    // to avoid loading the implementation library into this appDomain
    IHelloWorld^ proxy = 
        dynamic_cast<IHelloWorld^>(newAppDomain->CreateInstanceAndUnwrap(
        "ImplementationLibrary", 
        "HelloWorld"));

    // Output results of the call
    Console::WriteLine("\nReturn:\n\t{0}", proxy->Echo("Hello"));
    Console::WriteLine();

    Console::WriteLine("Non-GAC assemblies loaded in {0} appDomain", AppDomain::CurrentDomain->FriendlyName);

    array<Assembly^>^ assemblies = AppDomain::CurrentDomain->GetAssemblies();
    int numAssem = assemblies->Length;

    for (int i = 0; i < numAssem ; i++)
    {
        if (!assemblies[i]->GlobalAssemblyCache)
        {
            Console::WriteLine("\t{0}", assemblies[i]->GetName()->Name);
        }
    }
    Console::WriteLine("\nImplementationLibrary should not be loaded.");

    Console::ReadLine();

    return 0;
}