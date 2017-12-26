/****************************** Module Header ******************************\
Module Name:  Deadlock.cs
Project:      CSApplicationHang
Copyright (c) Microsoft Corporation.

Deadlock refers to a specific condition when two or more processes are each 
waiting for each other to release a resource, or more than two processes 
are waiting for resources in a circular chain. Because none of threads are 
willing to release their protected resources, what ultimately happens is 
that none of the threads will ever make any progress. They simply sit there 
and wait for the others to make a move, and a deadlock ensues.

In this sample, two threads are running Deadlock.Trigger and 
DeadlockThreadProc respectively. The thread that runs Deadlock.Trigger 
acquires resource2 first, and waits for resource1. The thread running 
DeadlockThreadProc acquires resource1, and waits for the leave of resource2 
in the first thread. Neither thread is willing to release its protected 
resource first, so a deadlock occurs.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Threading;


namespace CSApplicationHang
{
    class Deadlock
    {
        private static object _lock1 = new object();  // Guard resrouce1
        private static object _lock2 = new object();  // Guard resrouce2

        private static void DeadlockThreadProc()
        {
            // Acquire resource1 
            lock (_lock1)
            {
                // Do work with resource1
                // ...
                Thread.Sleep(3000);

                // Acquire resource2
                lock (_lock2)
                {
                    // Do work with resource2
                    // ...
                }
            }
        }

        public static void Trigger()
        {
            // Create the second thread
            Thread thread2 = new Thread(new ThreadStart(DeadlockThreadProc));
            thread2.Start();

            // Acquire resource2
            lock (_lock2)
            {
                // Do work with resource2
                // ...
                Thread.Sleep(2000);

                // Acquire resource1
                lock (_lock1)
                {
                    // Do work with resource1
                    // ...
                }
            }

            // Wait for the second thread to finish
            thread2.Join();
        }
    }
}