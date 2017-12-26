//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
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
using System.Threading;
using System.Collections.Generic;
using System.Text;

namespace Microsoft.Samples.Workflow.WorkflowThreading
{
    public static class ThreadMonitor
    {
        private static int threadCount;
        private static Dictionary<string, ConsoleColor> threadList = new Dictionary<string, ConsoleColor>();

        public static void Enlist(Thread thread, string instanceName)
        {
            if ((thread.Name == null) || (thread.Name.Length == 0))
            {
                thread.Name = string.Format("{0} Thread [{1}]", instanceName, threadCount++);
                if (!threadList.ContainsKey(thread.Name))
                    threadList.Add(thread.Name, GetConsoleColor());
            }
        }

        public static void WriteToConsole(Thread thread, string instanceName, string message)
        {
            Enlist(thread, instanceName);
            WriteToConsole(thread, message);
        }
        
        public static void WriteToConsole(Thread thread, string message)
        {
            if (threadList.ContainsKey(thread.Name))
                Console.ForegroundColor = threadList[thread.Name];
            Console.WriteLine("{0} --> {1}", thread.Name, message);
        }

        private static ConsoleColor GetConsoleColor()
        {
            if ((int)Console.ForegroundColor < 9)
                return ConsoleColor.White;
            else
                return (ConsoleColor)(int)Console.ForegroundColor - 1;
        }
    }
}
