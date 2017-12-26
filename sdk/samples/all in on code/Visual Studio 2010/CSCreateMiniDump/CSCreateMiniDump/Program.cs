/****************************** Module Header ******************************\
 * Module Name:  Program.cs
 * Project:      CSCreateMiniDump
 * Copyright (c) Microsoft Corporation.
 * 
 * When this application starts, it will attach a debugger to the given process. 
 * 
 * This source is subject to the Microsoft Public License.
 * See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 * All other rights reserved.
 * 
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/


using System;
using System.Diagnostics;
using System.ComponentModel;

namespace CSCreateMiniDump
{
    class Program
    {
        static void Main(string[] args)
        {

            int processID = 0;
            ManagedProcess process = null;
            Process targetProcess = null;

            // The argument must be a number that represents the ID of the target process
            if (args.Length == 1)
            {
                int.TryParse(args[0], out processID);
                targetProcess = Process.GetProcessById(processID);
            }

            // Must point to a valid process.
            while (targetProcess == null)
            {
                Console.WriteLine("Please type the ID of the process to be debugged:");
                string idStr = Console.ReadLine();
                int.TryParse(idStr, out processID);
                targetProcess = Process.GetProcessById(processID);
            }

            try
            {
                Console.WriteLine("Process Name: " + targetProcess.ProcessName);
                Console.WriteLine("Process ID:   " + targetProcess.Id);
                Console.WriteLine("Start to  Watch...");

                // Attach a debugger to the target process.
                process = new ManagedProcess(targetProcess);

                // This event occurs when the watch dog start to handle the exception.
                // If it is cancelled, the watch dog will not continue to handle the 
                // exception.
                process.StartHandleException +=
                    new EventHandler<CancelEventArgs>(process_StartHandleException);

                // This event occurs when the watch dog has got the information of the 
                // unhandled exception.
                process.UnhandledExceptionOccurred +=
                    new EventHandler<ManagedProcessUnhandledExceptionOccurredEventArgs>(
                        process_UnhandledExceptionOccurred);

                // Start to watch the target process.
                process.StartWatch();

            }
            catch (Exception ex)
            {
                Console.WriteLine("Failed to create minidump: " + ex.Message);
            }
            finally
            {
                if (process != null)
                {
                    process.Dispose();
                }
            }

            Console.WriteLine("Press ENTER to continue...");
            Console.ReadLine();
        }

        // Show the message when the watch dog starts to handle the exception.
        static void process_StartHandleException(object sender, CancelEventArgs e)
        {
            Console.WriteLine("Start to handle exception...");
            Console.WriteLine("Getting exception information...");
        }

        // Create a minidump if there is a unhandled exception in the target process.
        static void process_UnhandledExceptionOccurred(object sender,
            ManagedProcessUnhandledExceptionOccurredEventArgs e)
        {
            Console.WriteLine("Creating Minidump...");
            try
            {
                var dumpFilePath = MiniDump.MiniDumpCreator.CreateMiniDump(e.ProcessID,
                    e.ThreadID, e.ExceptionPointers);
                Console.WriteLine("The minidump file is {0}",dumpFilePath);
                Console.WriteLine("Done...");
            }
            catch (Exception ex)
            {
                Console.WriteLine("Failed to create minidump:");
                Console.WriteLine(ex.Message);
            }
        }
    }
}
