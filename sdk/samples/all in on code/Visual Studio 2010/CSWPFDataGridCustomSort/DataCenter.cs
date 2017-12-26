/****************************** Module Header ******************************\
Module Name:	DataCenter.cs
Project:	    CSWPFDataGridCustomSort
Copyright (c) Microsoft Corporation.

The CSWPFDataGridCustomSort demonstrates how to implement
a custom sort for one or severalcolumns in WPF DataGrid control.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Windows;
using System.Collections.ObjectModel;

namespace CSWPFDataGridCustomSort
{
    public class Employees : ObservableCollection<Employee>
    {
        public Employees()
        {
            Add(new Employee("heryb", "Hern Brown", "Member"));
            Add(new Employee("smith", "Smith", "Manager"));
            Add(new Employee("mike", "Michael", "CTO"));
            Add(new Employee("abill", "Alen Bill", "Member"));
            Add(new Employee("leevick", "Vick Lee", "Leader"));
        }
    }

    public class Employee
    {
        public Employee(string alias, string name, string job)
        {
            Alias = alias;
            Name = name;
            Job = job;
        }

        public string Alias { get; set; }
        public string Name { get; set; }
        public string Job { get; set; }
    }
}
