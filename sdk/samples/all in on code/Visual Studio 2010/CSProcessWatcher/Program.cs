/******************************** Module Header ****************************\
Module Name:  Program.cs
Project:      CSProcessWatcher
Copyright (c) Microsoft Corporation.

This project illustrates how to watch the process creation/modify/shutdown events
by using Windows Management Instrumentation(WMI).

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Management;
#endregion

namespace CSProcessWatcher
{
    class Program
    {
        private static string processName = "notepad.exe"; // Default process name.

        static void Main(string[] args)
        {
            if (args.Length > 0)
            {
                processName = args[0];
            }

            ProcessWatcher procWatcher = new ProcessWatcher(processName);
            procWatcher.ProcessCreated += new ProcessEventHandler(procWatcher_ProcessCreated);
            procWatcher.ProcessDeleted += new ProcessEventHandler(procWatcher_ProcessDeleted);
            procWatcher.ProcessModified += new ProcessEventHandler(procWatcher_ProcessModified);
            procWatcher.Start();

            Console.WriteLine(processName + " is under watch...");
            Console.WriteLine("Press Enter to stop watching...");

            Console.ReadLine();

            procWatcher.Stop();

        }

        private static void procWatcher_ProcessCreated(WMI.Win32.Process proc)
        {
            Console.Write("\nCreated\n " + proc.Name + " " + proc.ProcessId + "  " + "DateTime:" + DateTime.Now);
        }

        private static void procWatcher_ProcessDeleted(WMI.Win32.Process proc)
        {
            Console.Write("\nDeleted\n " + proc.Name + " " + proc.ProcessId + "  " + "DateTime:" + DateTime.Now);
        }

        private static void procWatcher_ProcessModified(WMI.Win32.Process proc)
        {
            Console.Write("\nModified\n " + proc.Name + " " + proc.ProcessId + "  " + "DateTime:" + DateTime.Now);
        }
    }
}
