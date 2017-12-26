/****************************** Module Header ******************************\
* Module Name:  COMService.cs
* Project:      CSCOMService
* Copyright (c) Microsoft Corporation.
* 
* Define the Windows Service module. The service is responsible for 
* initializing the security of the process, and registering / revoking the 
* registration of the COM class.
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
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.ServiceProcess;
using System.Text;
using System.Threading;
using System.Runtime.InteropServices;
#endregion


namespace CSCOMService
{
    public partial class COMService : ServiceBase
    {
        public COMService()
        {
            InitializeComponent();
            
            // Initialize COM security
            int hResult = COMNative.CoInitializeSecurity(
                IntPtr.Zero,    // Add your security descriptor here
                -1,
                IntPtr.Zero, 
                IntPtr.Zero, 
                RPC_C_AUTHN_LEVEL.PKT_PRIVACY,
                RPC_C_IMP_LEVEL.IDENTIFY,
                IntPtr.Zero, 
                EOLE_AUTHENTICATION_CAPABILITIES.DISABLE_AAA | 
                EOLE_AUTHENTICATION_CAPABILITIES.SECURE_REFS |
                EOLE_AUTHENTICATION_CAPABILITIES.NO_CUSTOM_MARSHAL,
                IntPtr.Zero);
   
            if (hResult != 0)
                throw new ApplicationException(
                    "CoIntializeSecurity failed w/err 0x" + hResult.ToString("X"));
        }

        private int _cookieSimpleObj = 0;

        protected override void OnStart(string[] args)
        {
            Guid clsidSimpleObj = new Guid(SimpleObject.ClassId);

            // Register the SimpleObject class object on start
            int hResult = COMNative.CoRegisterClassObject(
                ref clsidSimpleObj,                 // CLSID to be registered
                new SimpleObjectClassFactory(),   // Class factory
                CLSCTX.LOCAL_SERVER,                // Context to run
                REGCLS.MULTIPLEUSE | REGCLS.SUSPENDED,
                out _cookieSimpleObj);

            if (hResult != 0)
                throw new ApplicationException(
                    "CoRegisterClassObject failed w/err 0x" + hResult.ToString("X"));

            // Register other class objects 
            // ...

            // Inform the SCM about all the registered classes, and begins 
            // letting activation requests into the server process.
            hResult = COMNative.CoResumeClassObjects();

            if (hResult != 0)
            {
                // Revoke the registration of SimpleObject
                if (_cookieSimpleObj != 0)
                    COMNative.CoRevokeClassObject(_cookieSimpleObj);

                throw new ApplicationException(
                    "CoResumeClassObjects failed w/err 0x" + hResult.ToString("X"));
            }
        }

        protected override void OnStop()
        {
            // Revoke the registration of SimpleObject on stop
            if (_cookieSimpleObj != 0)
                COMNative.CoRevokeClassObject(_cookieSimpleObj);

            // Revoke the registration of other classes
            // ...
        }
    }
}