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

namespace CSWPFTwoLevelGrouping
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private ObservableCollection<Student> _students;

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            // Set up data source 
            _students = new ObservableCollection<Student>();

            _students.Add(new Student() { ID = 1, Name = "Johnson", Class = "Class A", Grade = "Grade 1" });
            _students.Add(new Student() { ID = 2, Name = "Bill", Class = "Class A", Grade = "Grade 1" });
            _students.Add(new Student() { ID = 3, Name = "Amy", Class = "Class B", Grade = "Grade 1" });
            _students.Add(new Student() { ID = 4, Name = "Polo", Class = "Class B", Grade = "Grade 1" });
            _students.Add(new Student() { ID = 5, Name = "Shalia", Class = "Class C", Grade = "Grade 1" });
            _students.Add(new Student() { ID = 6, Name = "Olay", Class = "Class C", Grade = "Grade 1" });
            _students.Add(new Student() { ID = 7, Name = "Sea", Class = "Class C", Grade = "Grade 1" });
            _students.Add(new Student() { ID = 8, Name = "Jim", Class = "Class A", Grade = "Grade 2" });
            _students.Add(new Student() { ID = 9, Name = "Tim", Class = "Class A", Grade = "Grade 2" });
            _students.Add(new Student() { ID = 10, Name = "Bruce", Class = "Class A", Grade = "Grade 2" });
            _students.Add(new Student() { ID = 11, Name = "Bockei", Class = "Class B", Grade = "Grade 2" });
            _students.Add(new Student() { ID = 12, Name = "Obama", Class = "Class A", Grade = "Grade 3" });
            _students.Add(new Student() { ID = 13, Name = "Linkon", Class = "Class B", Grade = "Grade 3" });
            _students.Add(new Student() { ID = 14, Name = "Asura", Class = "Class B", Grade = "Grade 3" });
            _students.Add(new Student() { ID = 15, Name = "Jack", Class = "Class C", Grade = "Grade 3" });
            _students.Add(new Student() { ID = 16, Name = "Rickie", Class = "Class C", Grade = "Grade 3" });

            CollectionViewSource _csv = new CollectionViewSource();
            _csv.Source = _students;

            // Group by Grade and Class
            _csv.GroupDescriptions.Add(new PropertyGroupDescription("Grade"));
            _csv.GroupDescriptions.Add(new PropertyGroupDescription("Class"));

            this.listView1.DataContext = _csv;

        }
    }

    public class GroupItemStyleSelector : StyleSelector
    {
        public override Style SelectStyle(object item, DependencyObject container)
        {
            Style s;

            CollectionViewGroup group = item as CollectionViewGroup;
            Window window = Application.Current.MainWindow;

            if (!group.IsBottomLevel)
            {
                s = window.FindResource("GroupHeaderStyleForFirstLevel") as Style;
            }
            else
            {
                s = window.FindResource("GroupHeaderStyleForSecondLevel") as Style;
            }

            return s;
        }
    }
}
