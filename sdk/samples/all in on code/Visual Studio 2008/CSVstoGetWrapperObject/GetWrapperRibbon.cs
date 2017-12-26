/************************************* Module Header **************************************\
* Module Name:	GetWrapperRibbon.cs
* Project:		CSVstoGetWrapperObject
* Copyright (c) Microsoft Corporation.
* 
* The CSVstoGetWrapperObject project demonstrates how to get a VSTO wrapper
* object from an existing Office COM object.
*
* This feature requires Visual Studio Tools for Office 3.0 SP1 (included in 
* Visual Studio 2008 SP1) for both design-time and runtime support.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 6/13/2009 3:00 PM Wang Jie Created
\******************************************************************************************/


#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Office.Tools.Ribbon;
using System.Windows.Forms;
using System.Diagnostics;
#endregion

namespace CSVstoGetWrapperObject
{
    public partial class GetWrapperRibbon : OfficeRibbon
    {
        private GetWrapperForm getWrapperForm = null;

        public GetWrapperRibbon()
        {
            InitializeComponent();
        }

        private void GetWrapperRibbon_Load(object sender, RibbonUIEventArgs e)
        {

        }

        private void btnShowGetWrapperCS_Click(object sender, RibbonControlEventArgs e)
        {
            if (getWrapperForm == null ||
                getWrapperForm.IsDisposed)
            {
                getWrapperForm = new GetWrapperForm();
                getWrapperForm.Show(NativeWindow.FromHandle(Process.GetCurrentProcess().MainWindowHandle));
            }
            else
            {
                getWrapperForm.Activate();
            }
        }
    }
}
