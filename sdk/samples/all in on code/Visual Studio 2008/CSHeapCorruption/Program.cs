/******************************** Module Header ********************************\
Module Name:  Program.cs
Project:      CSHeapCorruption
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


namespace CSHeapCorruption
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length > 0 && (args[0].StartsWith("-") || args[0].StartsWith("/")))
            {
                string cmd = args[0].Substring(1);

                if (String.Compare(cmd, "o", true) == 0)
                {
                    // Overrun the managed GC heap.
                    OverrunManagedGCHeap();
                }
                else if (String.Compare(cmd, "h", true) == 0)
                {
                    // Mismatch the heap handle.
                    MismatchHeapHandle();
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
            Console.WriteLine("CSHeapCorruption Instructions:");
            Console.WriteLine("-o     Overrun the managed GC heap");
            Console.WriteLine("-h     Mismatch the heap handle");
        }


        #region Managed GC Heap Overrun

        /// <summary>
        /// 
        /// </summary>
        static void OverrunManagedGCHeap()
        {
            int[] buffer = new int[50];
            for (int i = 0; i < buffer.Length; i++)
            {
                buffer[i] = 0xFF;
            }

            Console.Write("Press ENTER to overrun GC heap ...");
            Console.ReadLine();

            InitializeBuffer(buffer, buffer.Length);

            //GC.Collect();
        }

        [DllImport("CSHeapCorruption.NativeDll.dll", CharSet = CharSet.Unicode)]
        static extern void InitializeBuffer(int[] buffer, int size);

        #endregion


        #region Heap Handle Mismatch

        /// <summary>
        /// 
        /// </summary>
        static void MismatchHeapHandle()
        {
            // Allocate a block of memory. The native function AllocateMemory 
            // allocates the memory on the CRT heap.
            IntPtr pMem = AllocateMemory(50);

            Console.Write("Press ENTER to mismatch heap handle ...");
            Console.ReadLine();

            // Free the allocated memory on the wrong process heap.
            Marshal.FreeHGlobal(pMem);
        }

        [DllImport("CSHeapCorruption.NativeDll.dll", CharSet = CharSet.Unicode)]
        static extern IntPtr AllocateMemory(int bytes);
        
        #endregion
    }
}