/************************************* Module Header **************************************\
* Module Name:  MainWindow.xaml.cs
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
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
namespace CSWPFDataBinding
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        Person p = new Person();
        Persons pers = new Persons();
        public MainWindow()
        {
            InitializeComponent();
            p.Name = "Bruce";
            p.Job = "Nothing";
            p.Interest = "PC Game";
            p.Salary = 20;
            p.Age = 15;
            SetBindings();
        }
        void SetBindings()
        {
            //set Binding for Name TextBox
            Binding b = new Binding();
            b.Mode = BindingMode.TwoWay;
            b.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged;
            b.Source = p;
            b.Path = new PropertyPath("Name");
            BindingOperations.SetBinding(this.tbPersonName, TextBox.TextProperty, b);

            //set Binding for Job TextBox

            Binding b1 = new Binding();
            b1.Mode = BindingMode.OneWayToSource;
            b1.UpdateSourceTrigger = UpdateSourceTrigger.LostFocus;
            b1.Source = p;
            b1.Path = new PropertyPath("Job");
            BindingOperations.SetBinding(this.tbPersonJob, TextBox.TextProperty, b1);

            //set Binding for Age TextBox

            Binding b2 = new Binding();
            b2.UpdateSourceTrigger = UpdateSourceTrigger.LostFocus;
            b2.Source = p;
            b2.Mode = BindingMode.TwoWay;
            b2.ValidationRules.Add(new AgeValidationRule());
            b2.Path = new PropertyPath("Age");
            BindingOperations.SetBinding(this.tbPersonAge, TextBox.TextProperty, b2);

            //set Binding for Salary TextBox 
            Binding b3 = new Binding();
            b3.Source = p;
            b3.Mode = BindingMode.TwoWay;
            b3.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged;
            b3.Path = new PropertyPath("Salary");
           
            BindingOperations.SetBinding(this.tbPersonSalary, TextBox.TextProperty, b3);

            //Set Binding for  Interest TextBox

            Binding b4 = new Binding();
            b4.Source = p;
            b4.Mode = BindingMode.OneWay;
            b4.Path = new PropertyPath("Interest");
            BindingOperations.SetBinding(this.tbPersonInterest, TextBox.TextProperty, b4);

            //set Binding for Name Label
            Binding b5 = new Binding();
            b5.Source = p;
            b5.Path = new PropertyPath("Name");
            BindingOperations.SetBinding(this.lblPersonName,  Label.ContentProperty, b5);

            //set Binding for Job Label
            Binding b6 = new Binding();
            b6.Source = p;
            b6.Path = new PropertyPath("Job");
            BindingOperations.SetBinding(this.lblPersonJob, Label.ContentProperty, b6);

            //set Binding for Age Label;

            Binding b7 = new Binding();
            b7.Source = p;
            b7.Path = new PropertyPath("Age");
            BindingOperations.SetBinding(this.lblPersonAge, Label.ContentProperty, b7);

            //set Binding for Salary Label
            Binding b8 = new Binding();
            b8.Source = p;
            b8.Converter = new SalaryFormmatingConverter();
            b8.Path = new PropertyPath("Salary");
            BindingOperations.SetBinding(this.lblPersonSalary, Label.ContentProperty, b8);

            //set Binding for  Label Interest
            Binding b9 = new Binding();
            b9.Source = p;
            b9.Path = new PropertyPath("Interest");
            BindingOperations.SetBinding(this.lblPersonInterest, Label.ContentProperty, b9);

            //set Binding for ListBox
            lb.ItemsSource = pers;
        }
    }
}
