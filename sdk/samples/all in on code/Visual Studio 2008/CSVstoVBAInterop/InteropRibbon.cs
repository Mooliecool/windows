/************************************* Module Header **************************************\
* Module Name:  InteropRibbon.cs
* Project:      CSVstoVBAInterop
* Copyright (c) Microsoft Corporation.
* 
* The CSVstoVBAInterop project demonstrates how to interop with VBA project object model in 
* VSTO projects. Including how to programmatically add Macros (or VBA UDF in Excel) into an
* Office document; how to call Macros / VBA UDFs from VSTO code; and how to call VSTO code
* from VBA code. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\******************************************************************************************/

#region Using directives
using System;
using System.Windows.Forms;
using System.Diagnostics;
using Microsoft.Office.Tools.Ribbon;
#endregion


namespace CSVstoVBAInterop
{
    public partial class InteropRibbon : OfficeRibbon
    {
        private InteropForm interopForm = null; // The demo UI.

        public InteropRibbon()
        {
            InitializeComponent();
        }

        private void InteropRibbon_Load(object sender, RibbonUIEventArgs e)
        {
        }

        private void btnShowFormCS_Click(object sender, RibbonControlEventArgs e)
        {
            if (interopForm == null || interopForm.IsDisposed)
            {
                interopForm = new InteropForm();
                interopForm.Show(NativeWindow.FromHandle(Process.GetCurrentProcess().MainWindowHandle));
            }
            else
            {
                interopForm.Activate();
            }
        }
    }
}
