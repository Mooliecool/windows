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
// This is the main DLL file.

#include "stdafx.h"


#include "ImplementationLibrary.h"

using namespace System::Reflection;

public ref class HelloWorld : public MarshalByRefObject, public IHelloWorld
{
public:

    virtual String^ Echo(String^ input)
    {
        ConsoleColor originalColor = Console::BackgroundColor;
        Console::BackgroundColor = ConsoleColor::Blue;

       String^ currentAppDomain = AppDomain::CurrentDomain->FriendlyName;

        Console::WriteLine("AppDomain: {0}", currentAppDomain);
        Console::WriteLine("Echo Input: {0}", input);
        Console::WriteLine();
        Console::WriteLine("Non-GAC assemblies loaded in {0} appDomain", currentAppDomain);


        array<Assembly^>^ assemblies = AppDomain::CurrentDomain->GetAssemblies();
        int numAssem = assemblies->Length;


        for (int i = 0; i < numAssem ; i++)
        {
            if (!assemblies[i]->GlobalAssemblyCache)
            {
                Console::WriteLine("\t{0}", assemblies[i]->GetName()->Name);
            }
        }

        Console::BackgroundColor = originalColor;
        
        System::String^ result = gcnew System::String(input);
        result = result->Concat(" from AppDomain ");
        result = result->Concat(currentAppDomain);

        return result;
  }
};