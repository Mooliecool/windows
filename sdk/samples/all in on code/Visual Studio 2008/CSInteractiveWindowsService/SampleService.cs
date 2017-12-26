/******************************** Module Header ********************************\
* Module Name:  SampleService.cs
* Project:      CSInteractiveWindowsService
* Copyright (c) Microsoft Corporation.
* 
* To be finished - Jialiang Ge
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************/

using System;
using System.ComponentModel;
using System.ServiceProcess;
using System.Runtime.InteropServices;


namespace CSInteractiveWindowsService
{
    public partial class SampleService : ServiceBase
    {
        public SampleService()
        {
            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {
        }

        protected override void OnStop()
        {
        }


        protected uint GetSessionIdOfUser(string userName, string domain)
        {
            uint sessionId = 0xFFFFFFFF;

            if (string.IsNullOrEmpty(userName))
            {
                // If the user name is not provided, try to get the session 
                // attached to the physical console. The physical console is the 
                // monitor, keyboard, and mouse.
                sessionId = WTSHelper.ActiveConsoleSessionId;
            }
            else
            {
                // If the user name is provided, get the session of the provided 
                // user. The same user could have more than one session, this 
                // sample just retrieves the first one found. You can add more 
                // sophisticated checks by requesting different types of 
                // information from WTSQuerySessionInformation.

                // Enumerate the sessions on the current server.
            }

            return sessionId;
        }
    }
}
