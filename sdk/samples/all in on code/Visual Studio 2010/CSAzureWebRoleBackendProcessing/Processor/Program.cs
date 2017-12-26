/****************************** Module Header ******************************\
* Project Name:   CSAzureWebRoleBackendProcessing
* Module Name:    Processor
* File Name:      Program.cs
* Copyright (c) Microsoft Corporation
*
* This console application instantiates a BackendProcessor object and start it up.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using System.Diagnostics;
using CSAzureWebRoleBackendProcessing.Common;

namespace CSAzureWebRoleBackendProcessing.Processor
{
    class Program
    {
        static void Main(string[] args)
        {
            // Trace to the console window.
            Trace.Listeners.Add(new ConsoleTraceListener());

            // Start-up the backend processor.
            new BackendProcessor().Start();

            // Pause.
            Console.ReadKey();
        }
    }
}
