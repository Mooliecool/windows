/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:	    CSTFSEventListener
* Copyright (c) Microsoft Corporation.
* 
* Create a ServiceHost to listener the TFS notification.
* 
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
using System.Linq;
using System.ServiceModel;

namespace CSTFSEventListener
{
    class Program
    {
        static void Main(string[] args)
        {
            using (ServiceHost host = new ServiceHost(typeof(CheckinEventService)))
            {
                host.Open();
                Console.WriteLine(host.BaseAddresses.First());

                Console.WriteLine("Press <Enter> to exit...");
                Console.ReadLine();
            }
        }
    }
}
