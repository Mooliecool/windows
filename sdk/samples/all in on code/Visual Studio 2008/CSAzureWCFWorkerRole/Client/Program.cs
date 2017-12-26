/****************************** Module Header ******************************\
* Module Name:	Program.cs
* Project:		Client
* Copyright (c) Microsoft Corporation.
* 
* This class demonstrates how to consume the WCF services that is hosted in Worker Role.
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

namespace Client
{
    class Program
    {
        static void Main(string[] args)
        {
            ServiceReference1.MyServiceClient proxy = new ServiceReference1.MyServiceClient();
            var result = proxy.DoWork();
            Console.WriteLine(string.Format("Server Returned: {0}",result));
            Console.ReadLine();
        }
    }
}
