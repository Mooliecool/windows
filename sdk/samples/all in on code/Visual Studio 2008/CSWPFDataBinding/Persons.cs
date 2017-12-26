/************************************* Module Header **************************************\
* Module Name:  Persons.cs
* Project:      CSWPFDataBinding
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to use DataBinding in WPF
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 10/29/2009 3:00 PM Bruce Zhou Created
 * 
\******************************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;

namespace CSWPFDataBinding
{
    public class Persons : ObservableCollection<Person>
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="Persons"/> class.
        /// </summary>
        public Persons()
        {
            this.Add(new Person() { 
                Name="employee1", 
                Age=20, 
                Interest="Basketball", 
                Job="programmer", 
                Salary=300 });

            this.Add(new Person() { 
                Name = "employee2", 
                Age = 21, 
                Interest = "PC game", 
                Job = "programmer", 
                Salary = 200 });

            this.Add(new Person() { 
                Name = "employee3", 
                Age = 21, 
                Interest = "PC game", 
                Job = "programmer", 
                Salary = 300 });
        }
    }
}
