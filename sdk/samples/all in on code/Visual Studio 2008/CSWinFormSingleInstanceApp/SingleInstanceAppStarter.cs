/****************************** Module Header ******************************\
* Module Name:  SingleInstanceAppStarter.cs
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
using System.Windows.Forms;
using Microsoft.VisualBasic.ApplicationServices;

namespace CSWinFormSingleInstanceApp
{
    public class SingleInstanceAppStarter
    {
        static SingleInstanceApp app = null;

        //Construct SingleInstanceApp object, and invoke its run method
        public static void Start(Form f, StartupNextInstanceEventHandler handler)
        {
            if (app == null && f != null)
                app = new SingleInstanceApp(f);

            //wire up StartupNextInstance event handler
            app.StartupNextInstance += handler;
            app.Run(Environment.GetCommandLineArgs());
        }
    }
}
