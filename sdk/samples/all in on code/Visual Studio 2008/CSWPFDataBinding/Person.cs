/************************************* Module Header **************************************\
* Module Name:  Person.cs
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
using System.ComponentModel;

namespace CSWPFDataBinding
{
    public class Person : INotifyPropertyChanged
    {
        string name;

        /// <summary>
        /// Gets or sets the name.
        /// </summary>
        /// <value>The name.</value>
        public string Name
        {
            get { return name; }
            set
            {
                if (value != name)
                {
                    name = value;
                    OnPropertyChanged("Name");
                }
            }
        }
        int age;

        /// <summary>
        /// Gets or sets the age.
        /// </summary>
        /// <value>The age.</value>
        public int Age
        {
            get { return age; }
            set
            {
                if (value != age)
                {
                    age = value;
                    OnPropertyChanged("Age");
                }
            }
        }
        string job;

        /// <summary>
        /// Gets or sets the job.
        /// </summary>
        /// <value>The job.</value>
        public string Job
        {
            get { return job; }
            set
            {
                if (value != job)
                {
                    job = value;
                    OnPropertyChanged("Job");
                }
            }
        }

        int salary;

        /// <summary>
        /// Gets or sets the salary.
        /// </summary>
        /// <value>The salary.</value>
        public int Salary
        {
            get { return salary; }
            set {
                if (value != salary)
                {
                    salary = value;
                    OnPropertyChanged("Salary");
                }
            }
        }


        string interest;

        /// <summary>
        /// Gets or sets the interest.
        /// </summary>
        /// <value>The interest.</value>
        public string Interest
        {
            get { return interest; }
            set
            {
                if (value != interest)
                {
                    interest = value;
                    OnPropertyChanged("Interest");
                }

            }
        }

        #region INotifyPropertyChanged Members


        public event PropertyChangedEventHandler PropertyChanged;

        #endregion

        /// <summary>
        /// Fire the property changed event
        /// </summary>
        /// <param name="propertyName">Name of the property.</param>
        void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        #region Methods

        /// <summary>
        /// Initializes a new instance of the <see cref="Person"/> class.
        /// </summary>
        public Person()
        {
            Name = "Default";
            Job = "None";
            Age = 20;
            Interest = "None";
            Salary = 500;
        }
  
        #endregion
    }
}
