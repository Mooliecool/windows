/******************************** Module Header ********************************\
Module Name:  Program.cs
Project:      CSStackOverflow
Copyright (c) Microsoft Corporation.

CSStackOverflow is designed to show how stack overflow happens in C# applications. 
When a thread is created, 1MB of virtual memory is reserved for use by the thread 
as a stack. Unlike the heap, it does not expand as needed. When too much memory 
is used on the call stack the stack is said to overflow, typically resulting in a 
program crash. This class of software bug in .NET applications is usually caused 
by deeply recursive function calls.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

using System;


namespace CSStackOverflow
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
                    Console.Write("Press ENTER to call deeply recursive function ...");
                    Console.ReadLine();

                    // Call deeply recursive function to overflow the stack.
                    CallRecursiveFunction(0);
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
            Console.WriteLine("CSStackOverflow Instructions:");
            Console.WriteLine("-r   Call deeply recursive function to overflow the stack");
        }


        static void CallRecursiveFunction(int level)
        {
            // Recursively call the function.
            CallRecursiveFunction(level + 1);
        }
    }
}
