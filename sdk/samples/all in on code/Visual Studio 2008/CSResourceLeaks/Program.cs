/******************************** Module Header ********************************\
Module Name:  Program.cs
Project:      CSResourceLeaks
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

using System;


namespace CSResourceLeaks
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length > 0 && (args[0].StartsWith("-") || args[0].StartsWith("/")))
            {
                string cmd = args[0].Substring(1);

                if (String.Compare(cmd, "nm1", true) == 0)
                {
                    // Leak native memory (Demo 1).
                    MemoryLeaks.LeakNativeMemory1();
                }
                else if (String.Compare(cmd, "nm2", true) == 0)
                {
                    // Leak native memory (Demo 2).
                    MemoryLeaks.LeakNativeMemory2();
                }
                else if (String.Compare(cmd, "gcm1", true) == 0)
                {
                    // Leak managed GC heap memory (Demo 1).
                    MemoryLeaks.LeakManagedGCHeapMemory1();
                }
                else if (String.Compare(cmd, "gcm2", true) == 0)
                {
                    // Leak managed GC heap memory (Demo 2).
                    MemoryLeaks.LeakManagedGCHeapMemory2();
                }
                else if (String.Compare(cmd, "lm", true) == 0)
                {
                    // Leak managed loader heap memory.
                    MemoryLeaks.LeakManagedLoaderHeapMemory();
                }
                else if (String.Compare(cmd, "fh", true) == 0)
                {
                    // Leak file handle.
                    HandleLeaks.LeakFileHandle();
                }
                else if (String.Compare(cmd, "gch", true) == 0)
                {
                    // Leak GC handle.
                    HandleLeaks.LeakGCHandle();
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
            Console.WriteLine("CSResourceLeaks Instructions:");
            Console.WriteLine(" Memory leaks");
            Console.WriteLine("-nm1   Leak native memory (Demo 1)");
            Console.WriteLine("-nm2   Leak native memory (Demo 2)");
            Console.WriteLine("-gcm1  Leak managed GC heap memory (Demo 1)");
            Console.WriteLine("-gcm2  Leak managed GC heap memory (Demo 2)");
            Console.WriteLine("-lm    Leak managed loader heap memory");
            Console.WriteLine(" Handle leaks");
            Console.WriteLine("-fh    Leak file handle");
            Console.WriteLine("-gch   Leak GC handle");
        }
    }
}
