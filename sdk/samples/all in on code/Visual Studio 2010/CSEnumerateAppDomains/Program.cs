/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:	    CSEnumerateAppDomains
* Copyright (c) Microsoft Corporation.
* 
* This source file is used to handle the input command. If this application
* starts with an argument, process the command directly and then exit, else 
* show the help text to let user choose a command.
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
using Microsoft.Samples.Debugging.CorDebug;

namespace CSEnumerateAppDomains
{
    class Program
    {
        static void Main(string[] args)
        {

            // Create new AppDomain in current process.
            AppDomain.CreateDomain("Hello world!");
          
            try
            {
                // If this application starts without any argument, show the help text to
                // let user choose a command.
                if (args.Length == 0)
                {
                    // Application will not exit until user types the exit command.
                    // If the command is not correct, it will show the help text in the
                    // next loop. 
                    while (true)
                    {
                        Console.WriteLine(@"
Please choose a command:
1: Show AppDomains in current process.
2: List all managed processes.
3: Show help text.
4: Exit this application.
To show the AppDomains in a specified process, please type ""PID"" and
the ID of the process directly, like PID1234.
");

                        string cmd = Console.ReadLine();
                        int cmdID = 0;
                        if (int.TryParse(cmd, out cmdID))
                        {
                            switch (cmdID)
                            {
                                case 1:
                                    ProcessCommand("CurrentProcess");
                                    break;
                                case 2:
                                    ProcessCommand("ListAllManagedProcesses");
                                    break;
                                case 4:
                                    Environment.Exit(0);
                                    break;
                                default:

                                    // Show the help text in the next loop.
                                    break;

                            }
                        }
                        else if (cmd.StartsWith("PID", StringComparison.OrdinalIgnoreCase))
                        {
                            ProcessCommand(cmd);
                        }

                    }
                }
                else if (args.Length == 1)
                {
                    ProcessCommand(args[0]);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);

                // The exit code 100 means that this application does not run successfully.           
                Environment.Exit(100);
            }
        }

        static void ProcessCommand(string arg)
        {
            // List AppDomains in current process.
            if (arg.Equals("CurrentProcess", StringComparison.OrdinalIgnoreCase))
            {
                Console.WriteLine("List AppDomains in current process...");
                ShowAppDomainsInCurrentProcess();
            }

            // List all managed processes.
            else if (arg.Equals("ListAllManagedProcesses", StringComparison.OrdinalIgnoreCase))
            {
                Console.WriteLine("List all managed processes...");
                ListAllManagedProcesses();
            }

            // Show the AppDomains in a specified process, arg must starts with "PID".
            else if (arg.StartsWith("PID", StringComparison.OrdinalIgnoreCase))
            {
                int pid = 0;
                int.TryParse(arg.Substring(3), out pid);
                Console.WriteLine(string.Format(
                    "List AppDomains in the process {0} ...", pid));
                ShowAppDomains(pid);
            }

            else
            {
                throw new ArgumentException("Please type a valid command.");
            }

        }

        /// <summary>
        /// Show AppDomains in Current Process.
        /// </summary>
        static void ShowAppDomainsInCurrentProcess()
        {

            // GetAppDomainsInCurrentProcess is a static method of the class ManagedProcess.
            // This method is used to get all AppDomains in Current Process.
            var appDomains = ManagedProcess.GetAppDomainsInCurrentProcess();

            foreach (var appDomain in appDomains)
            {
                Console.WriteLine("AppDomain Id={0}, Name={1}",
                    appDomain.Id, appDomain.FriendlyName);
            }
        }

        /// <summary>
        /// Show AppDomains in a specified process.
        /// </summary>
        /// <param name="pid"> The ID of the Process.</param>
        static void ShowAppDomains(int pid)
        {
            if (pid <= 0)
            {
                throw new ArgumentException("Please type a valid PID.");
            }

            ManagedProcess process = null;
            try
            {
                // GetManagedProcessByID is a static method of the class ManagedProcess.
                // This method is used to get an instance of ManagedProcessInfo. If there is
                // no managed process with this PID, an ArgumentException will be thrown.
                process = ManagedProcess.GetManagedProcessByID(pid);

                foreach (CorAppDomain appDomain in process.AppDomains)
                {
                    Console.WriteLine("AppDomain Id={0}, Name={1}",
                        appDomain.Id,
                        appDomain.Name);
                }

            }
            catch (ArgumentException _argumentException)
            {
                Console.WriteLine(_argumentException.Message);
            }
            catch (ApplicationException _applicationException)
            {
                Console.WriteLine(_applicationException.Message);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.WriteLine(ex.GetType());
                Console.WriteLine(ex.StackTrace);
                Console.WriteLine("Cannot get the process. "
                   + " Make sure the process exists and it's a managed process");
            }
            finally
            {
                if (process != null)
                {
                    process.Dispose();
                }
            }
        }

        /// <summary>
        /// List all managed processes.
        /// </summary>
        static void ListAllManagedProcesses()
        {

            // GetManagedProcesses is a static method of the class ManagedProcess.
            // This method is used to get a list that contains all managed processes 
            // in current machine.
            var processes = ManagedProcess.GetManagedProcesses();

            foreach (var process in processes)
            {
                Console.WriteLine("ID={0}\tName={1}",
                     process.ProcessID, process.ProcessName);
                Console.Write("Loaded Runtimes: ");
                foreach (var runtime in process.LoadedRuntimes)
                {
                    Console.Write(runtime.GetVersionString() + "\t");
                }
                Console.WriteLine("\n");
            }

        }

    }
}
