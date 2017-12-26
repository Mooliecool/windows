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
using System.Runtime.Remoting;
using Microsoft.Samples.SharedInterface;

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
            try
            {
                ISharedInterface remoteObject = (ISharedInterface)Activator.GetObject(typeof(ISharedInterface), "ipc://test/server.rem");
                Console.WriteLine("Server responds: " + remoteObject.HelloWorld("Hi Server"));
            }
            catch (RemotingException e)
            {
                Console.WriteLine("An exception has occured: " + e.Message);
                Console.WriteLine("Perhaps you do not have access to the IPC channel.");
                Console.WriteLine("Stack trace:\n" + e.StackTrace);
            }

        }
    }
}
