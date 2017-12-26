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
using System.Reflection;
using System.Runtime.Remoting;
using Microsoft.Samples.Implementation;

namespace Microsoft.Samples.Client
{
    /// <summary>
    /// This is a sample IPC client.
    /// </summary>
    public static class ClientApp
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        public static void Main()
        {
            RemotingConfiguration.Configure(
                Assembly.GetExecutingAssembly().GetName().Name +
                ".exe.config", true /*ensureSecurity*/);

            ImplementationClass<string> stringRemoteObject = 
                new ImplementationClass<string>();

            Console.WriteLine("String Remote Object responds: " +
                stringRemoteObject.HelloWorld("Hi Server"));

            Console.WriteLine();

            ImplementationClass<int> intRemoteObject = 
                new ImplementationClass<int>();

            Console.WriteLine("Int Remote Object responds: " +
                intRemoteObject.HelloWorld(42));

        }
    }
}
