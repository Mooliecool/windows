/********************************** Module Header **********************************\
* Module Name:  Service.cs
* Project:      CSTriggerStartWindowsService
* Copyright (c) Microsoft Corporation.
* 
* The file implements the service body. To define what occurs when the service starts 
* and stops, locate the OnStart and OnStop methods in this file that were 
* automatically overridden when you created the project, and write code to determine 
* what occurs when the service starts running. In this example, we simply use an 
* event log object to report the service start and stop events to the Application log.
* You can also override the OnPause, OnContinue, and OnShutdown methods to define 
* additional processing for your service. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***********************************************************************************/

#region Using directives
using System;
using System.ComponentModel;
using System.Diagnostics;
using System.ServiceProcess;
using System.Runtime.InteropServices;
#endregion


namespace CSTriggerStartWindowsService
{
    public partial class Service : ServiceBase
    {
        public Service()
        {
            InitializeComponent();
        }


        /// <summary>
        /// Add code here to start your service. This method should set things in 
        /// motion so your service can do its work.
        /// </summary>
        /// <param name="args"></param>
        protected override void OnStart(string[] args)
        {
            this.eventLog1.WriteEntry("CSTriggerStartWindowsService is in OnStart.");
        }


        /// <summary>
        /// Add code here to perform any tear-down necessary to stop your service.
        /// </summary>
        protected override void OnStop()
        {
            this.eventLog1.WriteEntry("CSTriggerStartWindowsService is in OnStop.");
        }
    }
}
