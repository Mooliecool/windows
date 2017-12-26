/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:	    CSCheckEXEType
* Copyright (c) Microsoft Corporation.
* 
* This source file is used to handle the input command.
* 
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
using System.IO;

namespace CSCheckEXEType
{
    class Program
    {
        static void Main(string[] args)
        {
            while (true)
            {
                Console.WriteLine("Please type the exe file path:");
                Console.WriteLine("<Empty to exit>");
                string path = Console.ReadLine();

                if (string.IsNullOrEmpty(path))
                {
                    break;
                }

                if (!File.Exists(path))
                {
                    Console.WriteLine("The path does not exist!");
                    continue;
                }
                try
                {
                    ExecutableFile exeFile = new ExecutableFile(path);

                    var isConsole = exeFile.IsConsoleApplication;
                    var isDotNet = exeFile.IsDotNetAssembly;
                    

                    Console.WriteLine(string.Format(
@"ConsoleApplication: {0}
.NetApplication: {1}",
isConsole, isDotNet));

                    if (isDotNet)
                    {
                        Console.WriteLine("Compiled .NET Runtime: " + exeFile.GetCompiledRuntimeVersion());
                        Console.WriteLine("Full Name: " + exeFile.GetFullDisplayName());
                        var attributes = exeFile.GetAttributes();
                        foreach (var attribute in attributes)
                        {
                            Console.WriteLine(string.Format("{0}: {1}",
                                attribute.Key,attribute.Value));
                        }
                    }
                    else
                    {
                        var is32Bit = exeFile.Is32bitImage;
                        Console.WriteLine("32bit application: " + is32Bit);
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
                Console.WriteLine();
            }

        }
    }
}

