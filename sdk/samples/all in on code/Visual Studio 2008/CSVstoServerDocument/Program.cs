/************************************* Module Header **************************************\
* Module Name:  Program.cs
* Project:      CSVstoServerDocument
* Copyright (c) Microsoft Corporation.
* 
* The CSVstoServerDocument project demonstrates how to use the ServerDocument
* class to extract information from a VSTO customized Word document or Excel
* Workbook; and also how to programmatically add / remove VSTO customizations.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 4/13/2009 11:00 AM Wang Jie Created
\******************************************************************************************/


#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
#endregion

namespace CSVstoServerDocument
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
