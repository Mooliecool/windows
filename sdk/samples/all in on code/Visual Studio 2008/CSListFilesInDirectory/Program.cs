/************************************* Module Header **************************************\
* Module Name:  Program.cs
* Project:      CSListFilesInDirectory
* Copyright (c) Microsoft Corporation.
* 
* The CSListFilesInDirectory project demonstrates how to implement an IEnumerable<string>
* that utilizes the Win32 File Management functions to enable application to get files and
* sub-directories in a specified directory one item a time.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/7/2009 8:00 PM Jie Wang Created
\******************************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
#endregion


namespace CSListFilesInDirectory
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm());
        }
    }
}
