/******************************** Module Header ********************************\
Module Name:  Program.cs
Project:      CSOutOfMemory
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

using System;
using System.Runtime.InteropServices;


namespace CSOutOfMemory
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length > 0 && (args[0].StartsWith("-") || args[0].StartsWith("/")))
            {
                string cmd = args[0].Substring(1);

                if (String.Compare(cmd, "f", true) == 0)
                {
                    // Out of memory caused by managed heap fragmentation.
                    HaveManagedHeapFragmentation();
                }
                else if (String.Compare(cmd, "h", true) == 0)
                {
                    // Out of memory caused by high memory usage.
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
            Console.WriteLine("CSOutOfMemory Instructions:");
            Console.WriteLine("-f     Out of memory caused by managed heap fragmentation");
            Console.WriteLine("-h     Out of memory caused by high memory usage");
        }


        #region Managed Heap Fragmentation

        /// <summary>
        /// 
        /// </summary>
        static void HaveManagedHeapFragmentation()
        {
            Console.Write("Press ENTER to build memory environment ...");
            Console.ReadLine();

            const int numAllocs = 0x6000;

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

            // So far the function has allocated 24MB (0x6000 * 1KB) pinned 
            // buffers and 1.5GB (0x6000 * 64KB) non-pinned buffers. The pinned 
            // buffers are interwined with non-pinned buffers.

            // Promote the buffers to Gen2.
            GC.Collect();
            GC.Collect();

            // Free the non-pinned buffers. This will theoretically free 1.5GB 
            // (0x6000 x 64KB) memory. However, the large freed blocks are 
            // interwined with the live pinned buffers. It results in a highly 
            // fragmented heap.
            for (int i = 0; i < nonPinned.Length; i++)
            {
                nonPinned[i] = null;
            }
            GC.Collect();

            Console.Write("Press ENTER to cause \"out of memory\" ...");
            Console.ReadLine();

            // Allocate 0x4CCC 80KB buffers, whose total size equals the 
            // previously freed buffers: 1.5GB. Since the total allocation sizes 
            // are equivalent, it appears that the allocation will succeed. 
            // However, because of the highly fragmented GC heap, the allocation 
            // will fail with an OutOfMemory exception very soon.
            const int numNewAllocs = 0x4CCC;
            byte[][] newBuffers = new byte[numNewAllocs][];
            const int newSize = 0x14000; // 80KB
            for (int i = 0; i < newBuffers.Length; i++)
            {
                newBuffers[i] = new byte[newSize];
            }

            // Free the GCHandles.
            for (int i = 0; i < pinnedHandles.Length; i++)
            {
                pinnedHandles[i].Free();
            }
        }

        #endregion
    }
}
