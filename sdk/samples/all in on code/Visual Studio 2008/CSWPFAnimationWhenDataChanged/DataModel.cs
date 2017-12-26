/************************************* Module Header **************************************\
* Module Name:  DataModel.cs
* Project:      CSWPFAnimationWhenDataChanged
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to trigger animation when the value of the datagrid cell is
* changed.
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 11/30/2009 3:00 PM Bruce Zhou Created
 * 
\******************************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Collections.ObjectModel;
namespace CSWPFAnimationWhenDataChanged
{
    //datamodel used by the application, fetched from msdn
    public class NameList : ObservableCollection<PersonName>
    {
        public NameList()
            : base()
        {
            Add(new PersonName("Willa", "Cather"));
            Add(new PersonName("Isak", "Dinesen"));
            Add(new PersonName("Victor", "Hugo"));
            Add(new PersonName("Jules", "Verne"));
        }
    }
    public class PersonName : INotifyPropertyChanged
    {
        private string firstName;
        private string lastName;
        /// <summary>
        /// Initializes a new instance of the <see cref="PersonName"/> class.
        /// </summary>
        /// <param name="first">firstName.</param>
        /// <param name="last">lastName.</param>
        public PersonName(string first, string last)
        {
            this.firstName = first;
            this.lastName = last;
        }
        /// <summary>
        /// Gets or sets the firstName.
        /// </summary>
        /// <value>The first name.</value>
        public string FirstName
        {
            get { return firstName; }
            set
            {
                firstName = value;
                OnPropertyChanged("FirstName");
            }
        }

        /// <summary>
        /// Gets or sets the lastName.
        /// </summary>
        /// <value>The last name.</value>
        public string LastName
        {
            get { return lastName; }
            set
            {
                lastName = value;
                OnPropertyChanged("LastName");
            }
        }

        #region INotifyPropertyChanged Members
        public event PropertyChangedEventHandler PropertyChanged;
        #endregion
        /// <summary>
        /// Called when [property changed].
        /// </summary>
        /// <param name="propertyName">Name of the property.</param>
        void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}
