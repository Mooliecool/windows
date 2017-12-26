/****************************** Module Header ******************************\
 Module Name:  Program.cs
 Project:      CSApplicationToCrash
 Copyright (c) Microsoft Corporation.
 
 When this application starts, it will launch a Watchdog process. It will also
 create an unhandled exception.
 
 This source is subject to the Microsoft Public License.
 See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 All other rights reserved.
 
 THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace CSApplicationToCrash
{
    class Program
    {
        static void Main(string[] args)
        {
            Process demoProcess = Process.GetCurrentProcess();
            Console.WriteLine("The ID of this Demo Process is " + demoProcess.Id);

            LaunchWatchdog(demoProcess);

            // Wait 2 seconds, so the watch dog process can attach a debugger to this 
            // application.
            System.Threading.Thread.Sleep(2000);

            Console.WriteLine("Press ENTER to throw an unhandled exception...");
            Console.ReadLine();

            try
            {
                int zero = 0;

                // This exception will be handled by the catch block, so the watch dog
                // will not create a minidump at this moment.
                Console.WriteLine(1 / zero);

            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.GetType());

                // Rethrowing the exception will cause an unhandled exception, and the watch
                // dog will create a minidump now.
                throw;
            }
        }

        /// <summary>
        /// Launch the Watchdog process.
        /// </summary>
        /// <param name="demoProcess"></param>
        static void LaunchWatchdog(Process demoProcess)
        {
            try
            {
                Console.WriteLine("Launch Watchdog process...");
                ProcessStartInfo start = new ProcessStartInfo
                {
                    Arguments = demoProcess.Id.ToString(),
                    FileName = "CSCreateMiniDump.exe"
                };
                Process miniDumpCreatorProcess = Process.Start(start);
                Console.WriteLine("The Watchdog process was launched!");
            }
            catch (Exception ex)
            {
                Console.WriteLine("The Watchdog process was not launched!");
                Console.WriteLine(ex.Message);
            }
        }

    }
}
