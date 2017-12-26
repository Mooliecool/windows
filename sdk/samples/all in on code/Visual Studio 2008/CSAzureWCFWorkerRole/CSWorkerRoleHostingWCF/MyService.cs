/****************************** Module Header ******************************\
* Module Name:	MyService.cs
* Project:		CSWorkerRoleHostingWCF
* Copyright (c) Microsoft Corporation.
* 
* This module includes the service contract of MyService.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;

namespace WorkerRoleHostingWCF
{
    [ServiceContract]
    public interface IMyService
    {

        [OperationContract]
        string DoWork();
    }


    public class MyService : IMyService
    {

        public string DoWork()
        {
            return "Hello World";
        }

    } 
  
}
