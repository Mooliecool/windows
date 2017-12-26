/****************************** Module Header ******************************\
* Module Name:	Program.cs
* Project:		CSCheckOSVersion
* Copyright (c) Microsoft Corporation.
* 
* The CSCheckOSVersion sample demonstrates how to detect the version of the 
* current operating system, and how to make application that checks for the 
* minimum operating system version work with later operating system versions.
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


class Program
{
    static void Main(string[] args)
    {
        // Detect the current OS version.
        Console.WriteLine("Current OS: {0}", Environment.OSVersion.VersionString);

        // Make application that checks for the minimum operating system 
        // version work with later operating system versions. (For example, 
        // Check if the current OS is at least Windows XP.)
        if (Environment.OSVersion.Version < new Version(5, 1))
        {
            Console.WriteLine("Windows XP or later required.");
            // Quit the application due to incompatible OS
            return;
        }

        Console.WriteLine("Application Running...");
        Console.Read();
    }
}