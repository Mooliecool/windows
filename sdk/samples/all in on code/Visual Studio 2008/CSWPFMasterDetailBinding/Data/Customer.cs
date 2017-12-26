/************************************* Module Header **************************************\
* Module Name:  Customer.cs
* Project:      CSWPFMasterDetailBinding
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to do master/detail data binding in WPF.
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 10/29/2009 3:00 PM Zhi-Xin Created
 * 
\******************************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using System.ComponentModel;

namespace CSWPFMasterDetailBinding.Data
{
    class Customer : INotifyPropertyChanged
    {
        private int _id;
        private string _name;
        private ObservableCollection<Order> _orders
            = new ObservableCollection<Order>();

        public int ID
        {
            get { return _id; }
            set { 
                _id = value;
                OnPropertyChanged("ID");
            }
        }

        public string Name
        {
            get { return _name; }
            set
            {
                _name = value;
                OnPropertyChanged("Name");
            }
        }

        public ObservableCollection<Order> Orders
        {
            get { return _orders; }
        }

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        public void OnPropertyChanged(string name)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(name));
            }
        }

        #endregion
    }
}
