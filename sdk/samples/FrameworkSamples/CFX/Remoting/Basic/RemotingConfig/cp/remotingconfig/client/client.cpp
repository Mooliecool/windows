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
using namespace System::Runtime::Remoting;
using namespace Microsoft::Samples::SharedImplementation;
using namespace cli;

int _tmain()
{
    RemotingConfiguration::Configure("Client.exe.config", true);

    HelloWorld^ proxy = gcnew HelloWorld();
    Console::WriteLine("Server responds: {0}", proxy->Echo("Hello"));

    return 0;
}