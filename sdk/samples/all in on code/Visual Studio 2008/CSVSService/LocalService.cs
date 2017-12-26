/****************************** Module Header *************************************\
* Module Name:  LocalService.cs
* Project:      CSVSService
* Copyright (c) Microsoft Corporation.
* 
* The CSVSService example demonstrates how to provide, register, consume 
* service in VSPackage. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 10/08/2009 6:02 PM Wesley Yao Created
\**********************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace VSX.CSVSService
{
    ////////////////////////////////////////////////////////////////////////////////
    // This is the local service interface implemented by the
    // LocalService class below.
    // We specify one method named LocalServiceFunction() for testing.
    //
    [Guid(GuidList.guidCSVSServiceLocalServiceInterfaceString)]
    // The ComVisibleAttribute is required to query the interface from
    // native code.
    [ComVisible(true)]
    public interface ICSLocalService
    {
        void LocalServiceMethod();
    }

    ////////////////////////////////////////////////////////////////////////////////
    // This interface is used to identifies the service.
    //
    [Guid(GuidList.guidCSVSServiceLocalServiceString)]
    public interface SCSLocalService
    {
    }

    ////////////////////////////////////////////////////////////////////////////////
    // This is the local service class that implements the
    // ICSVSSLocalService and SCSVSSLocalService interface.
    // We can get this service only in our VSPackage, or to any object it
    // creates.
    //
    public class LocalService : ICSLocalService, SCSLocalService
    {
        private IServiceProvider serviceProvider;
        ////////////////////////////////////////////////////////////////////////////
        // Site the service provider in constructor.
        // We could use this service provider to get other services.
        //
        public LocalService(IServiceProvider sp)
        {
            serviceProvider = sp;
        }

        #region ICSLocalService Members

        public void LocalServiceMethod()
        {
            // Show a message box to prompt user that method of local service
            // is called.
            MessageBox.Show("Local Method Called!");
        }

        #endregion
    }
}
