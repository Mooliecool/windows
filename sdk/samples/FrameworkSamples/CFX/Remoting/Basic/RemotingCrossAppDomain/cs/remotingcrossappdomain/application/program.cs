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
using System.Collections.Generic;
using System.Reflection;
using System.Text;
using Microsoft.Samples.SharedInterface;

namespace Microsoft.Samples.Application
{
    static class Program
    {
        static void Main()
        {
            //Create new appDomain
            AppDomain domain = AppDomain.CreateDomain("NewAppDomain");

            // Create remote object in new appDomain via shared interface
            // to avoid loading the implementation library into this appDomain
            IHelloWorld proxy = 
                (IHelloWorld)domain.CreateInstanceAndUnwrap(
                    "ImplementationLibrary", 
                    "Microsoft.Samples.ImplementationLibrary.HelloWorld");

            // Output results of the call
            Console.WriteLine("\nReturn:\n\t{0}", proxy.Echo("Hello"));
            Console.WriteLine();

            Console.WriteLine("Non-GAC assemblies loaded in {0} appDomain:", AppDomain.CurrentDomain.FriendlyName);
            foreach (Assembly assembly in AppDomain.CurrentDomain.GetAssemblies())
            {
                if (!assembly.GlobalAssemblyCache)
                {
                    Console.WriteLine("\t" + assembly.GetName().Name);
                }
            }
            Console.WriteLine("\nImplementationLibrary should not be loaded.");

            Console.ReadLine();
        }
    }
}
