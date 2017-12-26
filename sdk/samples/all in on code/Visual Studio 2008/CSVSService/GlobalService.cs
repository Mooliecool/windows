/****************************** Module Header *************************************\
* Module Name:  GlobalService.cs
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
    // This is the global service interface implemented by the
    // GlobalService class below.
    // We specify one method named GlobalServiceFunction() and
    // CallLocalService() for testing.
    // CallLocalService() demostrates calling the method of local service
    // internally.
    //
    [Guid(GuidList.guidCSVSServiceGlobalServiceInterfaceString)]
    // The ComVisibleAttribute is required to query the interface from
    // native code.
    [ComVisible(true)]
    public interface ICSGlobalService
    {
        void GlobalServiceMethod();
        void CallLocalServiceMethod();
    }

    ////////////////////////////////////////////////////////////////////////////////
    // This interface is used to identifies the service.
    //
    [Guid(GuidList.guidCSVSServiceGlobalServiceString)]
    public interface SCSGlobalService
    {
    }

    ////////////////////////////////////////////////////////////////////////////////
    // This is the global service class that implements the
    // ICSVSSGlobalService and SCSVSSGlobalService interface.
    // We can get this service only in our VSPackage, or to any object it
    // creates.
    //
    public class GlobalService : ICSGlobalService, SCSGlobalService
    {
        private IServiceProvider serviceProvider;
        // Site the service provider in constructor.
        // We could use this service provider to get other services.
        public GlobalService(IServiceProvider sp)
        {
            serviceProvider = sp;
        }

        #region ICSGlobalService Members

        public void GlobalServiceMethod()
        {
            // Show a message box to prompt user that method of local service
            // is called.
            MessageBox.Show("Global Method Called!");
        }

        public void CallLocalServiceMethod()
        {
            // Get the local service and then call its method.
            ICSLocalService localService
                = serviceProvider.GetService(typeof(SCSLocalService))
                as ICSLocalService;
            if (null != localService)
            {
                localService.LocalServiceMethod();
            }
        }

        #endregion
    }
}
