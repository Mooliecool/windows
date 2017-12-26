/************************************* Module Header **************************************\
* Module Name:  MainWindow.xaml.cs
* Project:      CSWPFPaging
* Copyright (c) Microsoft Corporation.
* 
* The sample demonstrates how to page data in WPF.
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 12/02/2009 3:00 PM Zhi-Xin Ye Created
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CSWPFPaging
{
    class Customer
    {
        public int ID { get; set; }
        public string Name { get; set; }
        public int Age { get; set; }
        public string Country { get; set; }
    }
}
