/******************************** Module Header ********************************\
Module Name:  Program.cs
Project:      CSHighMemoryUsage
Copyright (c) Microsoft Corporation.

CSHighMemoryUsage is designed to show typical causes of high memory usage in .NET 
applications.

1. Long-lived rootless objects
2. Managed heap fragmentation
3. Memory leak

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

using System;
using System.Collections.Specialized;
using System.Runtime.InteropServices;


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
                    HaveLongLivedRootlessObjects();
                }
                else if (String.Compare(cmd, "f", true) == 0)
                {
                    // High memory usage caused by managed heap fragmentation.
                    HaveManagedHeapFragmentation();
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
            Console.WriteLine("-f   High memory usage caused by heap fragmentation");
            Console.WriteLine("-l   High memory usage caused by memory leaks");
        }


        #region Long-lived Rootless Objects

        /// <summary>
        /// The function demonstrates high memory usage caused by long-lived 
        /// rootless objects.
        /// </summary>
        static void HaveLongLivedRootlessObjects()
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


        #region Managed Heap Fragmentation

        /// <summary>
        /// 
        /// </summary>
        static void HaveManagedHeapFragmentation()
        {
            Console.Write("Press ENTER to build memory environment ...");
            Console.ReadLine();

            const int numAllocs = 0x1000;

            GCHandle[] pinnedHandles = new GCHandle[numAllocs];
            byte[][] pinned = new byte[numAllocs][];
            const int pinnedSize = 0x400; // 1KB

            byte[][] nonPinned = new byte[numAllocs][];
            const int nonPinnedSize = 0x10000; // 64KB

            for (int i = 0; i < numAllocs; i++)
            {
                // Allocate a 1KB pinned buffer.
                pinned[i] = new byte[pinnedSize];
                pinnedHandles[i] = GCHandle.Alloc(pinned[i], GCHandleType.Pinned);

                // Allocate a 64KB non-pinned buffer.
                nonPinned[i] = new byte[nonPinnedSize];
            }

            // So far the function has allocated 4MB (0x1000 * 1KB) pinned 
            // buffers and 256MB (0x1000 * 64KB) non-pinned buffers. The pinned 
            // buffers are interwined with non-pinned buffers.

            // Promote the buffers to Gen2.
            GC.Collect();
            GC.Collect();

            // Free the non-pinned buffers. This will theoretically free 256MB 
            // (0x1000 x 64KB) memory. However, the large freed blocks are 
            // interwined with the live pinned buffers. It results in a highly 
            // fragmented heap.
            for (int i = 0; i < numAllocs; i++)
            {
                nonPinned[i] = null;
            }
            GC.Collect();

            Console.WriteLine("Please check the current memory usage");

            Console.Write("Press ENTER to trigger a full GC ...");
            Console.ReadLine();

            // Force a full GC.
            GC.Collect();

            Console.Write("Press ENTER to unpin the buffers ...");
            Console.ReadLine();

            // Free the GCHandles and the pinned buffers.
            for (int i = 0; i < numAllocs; i++)
            {
                pinnedHandles[i].Free();
                pinned[i] = null;
            }
            GC.Collect();
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