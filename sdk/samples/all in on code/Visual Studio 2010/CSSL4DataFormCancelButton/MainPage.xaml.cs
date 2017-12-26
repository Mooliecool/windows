
/****************************** Module Header ******************************\
* Module Name:                MainPage.xaml.cs
* Project:                    CSSL4DataFormCancelButton
* Copyright (c) Microsoft Corporation.
* 
* MainPage's code hehind file.
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
using System.Windows;
using System.Windows.Controls;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.ComponentModel.DataAnnotations;

namespace CSSL4DataFormCancelButton
{
    public partial class MainPage : UserControl
    {
        public MainPage()
        {
            InitializeComponent();
            Loaded += new RoutedEventHandler(MainPage_Loaded);
        }

        void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            myDataForm.ItemsSource = new Employee();
        }
    }

    // Your class inherit from IEditableObject to achieve the purpose
    public class EmployeeItem : INotifyPropertyChanged, IEditableObject
    {
        struct ItemData
        {
            internal string EmployeName;
            internal string Gender;
            internal int Age;
            internal DateTime OnboardDate;
        }

        ItemData originalItem;
        ItemData currentItem;

        public EmployeeItem(): this("Employee Name", 25, "Male", DateTime.Now)
        {
        }

        public EmployeeItem(string empName, int age,string gender, DateTime onboardDate)
        {
            EmployeeName = empName;
            Gender = gender;
            Age = age;
            OnboardDate = onboardDate;
        }

        public string EmployeeName
        {
            get { return currentItem.EmployeName; }
            set
            {
                if (currentItem.EmployeName != value)
                {
                    currentItem.EmployeName = value;
                    NotifyPropertyChanged("EmployeName");
                }
            }
        }

        public string Gender
        {
            get { return currentItem.Gender; }
            set
            {
                if (currentItem.Gender != value)
                {
                    currentItem.Gender = value;
                    NotifyPropertyChanged("Gender");
                }
            }
        }


        [Range(10, 80, ErrorMessage = "Employee's Age range should within 10 to 80")]
        [Display(Name = "Age", Description = "Employee's Age")]
        public int Age
        {
            get { return currentItem.Age; }
            set
            {
                if (currentItem.Age != value)
                {
                    Validator.ValidateProperty(value, new ValidationContext(this, null, null) { MemberName = "Age" });
                    currentItem.Age = value;
                    NotifyPropertyChanged("Age");
                }
            }
        }

        public DateTime OnboardDate
        {
            get { return currentItem.OnboardDate; }
            set
            {
                if (value != currentItem.OnboardDate)
                {
                    currentItem.OnboardDate = value;
                    NotifyPropertyChanged("OnboardDate");
                }
            }
        }


        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged(String info)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(info));
            }
        }

        #endregion

        #region IEditableObject Members

        // Copy the original value before editing
        public void BeginEdit()
        {
            originalItem = currentItem;
        }

        // Restore the original value if edit operation is cancelled.
        public void CancelEdit()
        {
            currentItem = originalItem;
            NotifyPropertyChanged("");

        }

        public void EndEdit()
        {
           
        }

        #endregion

    }

    public class Employee : ObservableCollection<EmployeeItem>
    {
        public Employee()
        {
            Add((new EmployeeItem("Steven", 20,"Male", new DateTime(2010, 9, 1))));
            Add((new EmployeeItem("Vivian", 26,"Female", new DateTime(2008, 5, 1))));
            Add((new EmployeeItem("Bill", 28,"Male", new DateTime(2006, 2, 2))));
            Add((new EmployeeItem("Janney",30,"Female", new DateTime(2004, 3, 10))));
            Add((new EmployeeItem("Bob", 40, "Male", new DateTime(2009, 5, 27))));
            Add((new EmployeeItem("Jonathan", 27, "Male", new DateTime(2000, 10, 25))));
        }

    }

}
