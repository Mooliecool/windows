/****************************** Module Header ******************************\
Module Name:  Program.cs
Project:      CSControlWindowsService
Copyright (c) Microsoft Corporation.

The code sample demonstrates how to install, uninstall, start, stop a Windows 
service and set the service DACL to grant start, stop, delete and read 
control access to all authenticated users programmatically.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.ServiceProcess;


namespace CSControlWindowsService
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length > 0 && (args[0].StartsWith("-") || args[0].StartsWith("/")))
            {
                string cmd = args[0].Substring(1);

                if (String.Compare(cmd, "install", true) == 0 && args.Length > 2)
                {
                    // Install a service - requires 2 additional parameters:
                    // -install <ServiceName> <BinaryPath>
                    string serviceName = args[1];
                    string binaryPath = args[2];

                    ServiceControllerEx.InstallService(
                        null, 
                        serviceName, 
                        serviceName, 
                        ServiceStart.SERVICE_DEMAND_START,
                        binaryPath, 
                        null, 
                        "");
                    Console.WriteLine("Service installed successfully.");
                }
                else if (String.Compare(cmd, "uninstall", true) == 0 && args.Length > 1)
                {
                    // Uninstall the service - requires 1 additional parameter:
                    // -uninstall <ServiceName>
                    string serviceName = args[1];

                    ServiceControllerEx.UninstallService(null, serviceName);
                    Console.WriteLine("Service uninstalled successfully.");
                }
                else if (String.Compare(cmd, "start", true) == 0 && args.Length > 1)
                {
                    // Start the service - requires 1 additional parameter:
                    // -start <ServiceName>
                    string serviceName = args[1];

                    ServiceController service = new ServiceController(serviceName, ".");
                    service.Start();
                    service.WaitForStatus(ServiceControllerStatus.Running, 
                        new TimeSpan(0, 0, 30));
                    Console.WriteLine("Service started successfully.");
                }
                else if (String.Compare(cmd, "stop", true) == 0 && args.Length > 1)
                {
                    // Stop the service - requires 1 additional parameter:
                    // -stop <ServiceName>
                    string serviceName = args[1];

                    ServiceController service = new ServiceController(serviceName, ".");
                    service.Stop();
                    service.WaitForStatus(ServiceControllerStatus.Stopped, 
                        new TimeSpan(0, 0, 30));
                    Console.WriteLine("Service stopped successfully.");
                }
                else if (String.Compare(cmd, "update", true) == 0 && args.Length > 2 &&
                    args[1] == "dacl")
                {
                    // Update the service DACL - requires 2 additional parameters:
                    // -update dacl <ServiceName>
                    string serviceName = args[2];

                }
                else
                {
                    PrintInstructions();
                }
            }
            else
            {
                PrintInstructions();
            }
        }


        static void PrintInstructions()
        {
            Console.WriteLine("CppControlWindowsService Instructions:");
            Console.WriteLine("-install <ServiceName> <BinaryPath>  Install a service");
            Console.WriteLine("-uninstall <ServiceName>             Uninstall a service");
            Console.WriteLine("-start <ServiceName>                 Start a service");
            Console.WriteLine("-stop <ServiceName>                  Stop a service");
            Console.WriteLine("-update dacl <ServiceName>           Update the DACL of a service");
        }
    }
}