/****************************** Module Header ******************************\
* Module Name:       Program.cs
* Project:           CSThreadingBasic
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to create threads using C#.NET in three 
* different approaches. And it also shows how to create a thread that require
* a parameter. In the target threads, it also shows how to use lock keyword 
* to ensure a code snippet executed without interruption.
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
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Threading;

namespace CSThreadingBasic
{
    class Program
    {
        static int i = 0;
        static object o = new object();

        static void Main(string[] args)
        {
            Console.WriteLine("Main Thread's managed thread id: " + Thread.CurrentThread.ManagedThreadId.ToString());

            //The following region shows serval ways to create a thread
            //The first way directly create a new thread manually
            //The second and third way run the target function from a thread pool
            //Second way has the best performance.
            //The forth way invokes a parameterized function within the new thread
            
            
            //Method1: Create a thread by new Thread object
            ThreadStart ts1 = new ThreadStart(ThreadFunction1);
            Thread t1 = new Thread(ts1);
            t1.Start();

            //Method2: Create a thread by ThreadPool.QueueUserWorkItem
            ThreadPool.QueueUserWorkItem(new WaitCallback(ThreadFunction2));

            //Method3: Create a thread by ThreadStart.BeginInvoke;
            ThreadStart ts2 = new ThreadStart(ThreadFunction3);
            ts2.BeginInvoke(null, null);

            //Method4: Create a thread with 
            ParameterizedThreadStart pts = new ParameterizedThreadStart(ThreadFunction4);
            Thread t2 = new Thread(pts);
            t2.Start("Message");


            Console.ReadKey();
        }

        static void ThreadFunction1()
        {
            lock (o)
            {
                Console.WriteLine("\r\nMethod1: Current thread's managed thread id: " + Thread.CurrentThread.ManagedThreadId.ToString());
                Console.WriteLine((i++).ToString());
            }
        }

        static void ThreadFunction2(Object stateInfo)
        {
            lock (o)
            {
                Console.WriteLine("\r\nMethod2: Current thread's managed thread id: " + Thread.CurrentThread.ManagedThreadId.ToString());
                Console.WriteLine((i++).ToString());
            }
        }

        static void ThreadFunction3()
        {
            lock (o)
            {
                Console.WriteLine("\r\nMethod3: Current thread's managed thread id: " + Thread.CurrentThread.ManagedThreadId.ToString());
                Console.WriteLine((i++).ToString());
            }
        }

        static void ThreadFunction4(object message)
        {
            lock (o)
            {
                Console.WriteLine("\r\nMethod4: Current thread's managed thread id: " + Thread.CurrentThread.ManagedThreadId.ToString() + "\t" + message);
                Console.WriteLine((i++).ToString());
            }
        }
    }
}
