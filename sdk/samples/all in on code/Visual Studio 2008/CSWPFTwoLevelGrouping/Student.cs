/************************************* Module Header **************************************\
* Module Name:  MainWindow.xaml.cs
* Project:      CSWPFTwoLevelGrouping
* Copyright (c) Microsoft Corporation.
* 
* The sample demonstrates how to display two level grouped data in WPF.
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

namespace CSWPFTwoLevelGrouping
{
    class Student
    {
        public int ID { get; set; }
        public string Name { get; set; }
        public string Class { get; set; }
        public string Grade { get; set; }
    }
}
