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
using System;
using System.Security.Principal;
using Microsoft.Samples.SharedInterface;

namespace Microsoft.Samples.Implementation
{
    internal class ImplementationClass : MarshalByRefObject, ISharedInterface
    {
        string Microsoft.Samples.SharedInterface.ISharedInterface.HelloWorld(string input)
        {
            Console.WriteLine("Client says: " + input);
            return "Hello to you too, client";
        }
    }
}