/****************************** Module Header ******************************\
* Module Name:  SingleInstanceAppHelper.cs
* Project:      CSWinFormSingleInstanceApp
* Copyright (c) Microsoft Corporation.
* 
* The  sample demonstrates how to achieve the goal that only 
* one instance of the application is allowed in Windows Forms application..
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 8/31/2009 3:00 PM Bruce Zhou Created
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.VisualBasic.ApplicationServices;
using System.Windows.Forms;

namespace CSWinFormSingleInstanceApp
{
    //We need to add Microsoft.VisualBasic reference to use
    //WindowsFormsApplicationBase type.
    class SingleInstanceApp : WindowsFormsApplicationBase 
    {
        public SingleInstanceApp()
        {
        }
        public SingleInstanceApp(Form f)
        {
            //set IsSingleInstance property to true to make the application 
            base.IsSingleInstance = true;
            //set MainForm of the application.
            this.MainForm = f;
        }
    }
}
