/******************************** Module Header ********************************\
Module Name:  Program.cs
Project:      CSHighMemoryUsage
Copyright (c) Microsoft Corporation.

CSHighMemoryUsage is designed to show typical causes of high memory usage in .NET 
applications.

1. Long-lived rootless objects
2. Memory leak

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

using System;
using System.Collections.Specialized;


namespace CSHighMemoryUsage
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length > 0 && (args[0].StartsWith("-") || args[0].StartsWith("/")))
            {
                string cmd = args[0].Substring(1);

                if (String.Compare(cmd, "r", true) == 0)
                {
                    // High memory usage caused by long-lived rootless objects.
                    HaveLongLiveRootlessObjects();
                }
                else if (String.Compare(cmd, "l", true) == 0)
                {
                    // High memory usage caused by memory leaks.
                    LeakMemory();
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

            Console.Write("Press ENTER to exit ...");
            Console.ReadLine();
        }


        static void PrintInstructions()
        {
            Console.WriteLine("CSHighMemoryUsage Instructions:");
            Console.WriteLine("-r   High memory usage caused by long-lived rootless objects");
            Console.WriteLine("-l   High memory usage caused by memory leaks");
        }


        #region HaveLongLiveRootlessObjects

        /// <summary>
        /// The function demonstrates high memory usage caused by long-lived 
        /// rootless objects.
        /// </summary>
        static void HaveLongLiveRootlessObjects()
        {
            Console.Write("Press ENTER to build memory environment ...");
            Console.ReadLine();

            HaveLongLiveRootlessObjectsImpl();

            Console.WriteLine("Please check the current memory usage");

            Console.Write("Press ENTER to trigger a Generation 0 GC ...");
            Console.ReadLine();

            // Force a generation 0 GC.
            GC.Collect(0);

            Console.Write("Press ENTER to trigger a Generation 1 GC ...");
            Console.ReadLine();

            // Force a generation 1 GC again.
            GC.Collect(1);
        }


        private static void HaveLongLiveRootlessObjectsImpl()
        {
            HybridDictionary[] caches = new HybridDictionary[100];

            // Force two GCs to promote the caches object to Gen2. It simulates 
            // that the caches object is a long-lived object.
            GC.Collect();
            GC.Collect();

            for (int i = 0; i < caches.Length; i++)
            {
                // Create a cache object and add it to the cache array.
                HybridDictionary cache = new HybridDictionary();
                caches[i] = cache;

                // Add objects to the cache.
                for (int j = 0; j < 500; j++)
                {
                    cache.Add(j, new MyObject());
                }
            }

            // After the function returns, the caches object becomes rootless.
        }

        #endregion


        static void LeakMemory()
        {
        }
    }


    class MyObject
    {
        private int[] buffer = new int[500];
    }
}