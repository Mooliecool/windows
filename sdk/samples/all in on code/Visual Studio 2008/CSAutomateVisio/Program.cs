/***************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:      CSAutomateVisio
* Copyright (c) Microsoft Corporation.
* 
* The CSAutomateVisio sample demonstrates how to use Visio Object Model to 
* automate Office Visio 2007.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
#endregion


namespace CSAutomateVisio
{
    class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            Solution1.AutomateVisio();

            Console.WriteLine();

            Solution2.AutomateVisio();
        }
    }
}