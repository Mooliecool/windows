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

namespace Microsoft.Samples.ImplementationLibrary
{
    internal class HelloWorld : MarshalByRefObject, IHelloWorld
    {

        #region IHelloWorld Members

        string IHelloWorld.Echo(string input)
        {
            ConsoleColor originalColor = Console.BackgroundColor;
            Console.BackgroundColor = ConsoleColor.Blue;

            string currentAppDomainName = AppDomain.CurrentDomain.FriendlyName;
            Console.WriteLine("AppDomain: {0}", currentAppDomainName);
            Console.WriteLine("Echo Input: {0}", input);
            Console.WriteLine();
            Console.WriteLine("Non-GAC assemblies loaded in {0} appDomain:", AppDomain.CurrentDomain.FriendlyName);
            foreach (Assembly assembly in AppDomain.CurrentDomain.GetAssemblies())
            {
                if (!assembly.GlobalAssemblyCache)
                {
                    Console.WriteLine("\t" + assembly.GetName().Name);
                }
            }


            Console.BackgroundColor = originalColor;

            
            return input + " from AppDomain: " + currentAppDomainName;

        }

        #endregion

    }
}
