using System;
using System.Windows;
using System.Collections.ObjectModel;

namespace Editing
{
    public partial class Window1 : Window
    {

        public Window1()
        {
            InitializeComponent();
        }
       
    }

    public class EmployeeInfo
    {
        private string _firstName;
        private string _lastName;
        private string _employeeNumber;

        public string FirstName
        {
            get { return _firstName; }
            set { _firstName = value; }
        }

        public string LastName
        {
            get { return _lastName; }
            set { _lastName = value; }
        }

        public string EmployeeNumber
        {
            get { return _employeeNumber; }
            set { _employeeNumber = value; }
        }

        public EmployeeInfo(string firstname, string lastname, string empnumber)
        {
            _firstName = firstname;
            _lastName = lastname;
            _employeeNumber = empnumber;
        }
    }
    public class myEmployees :
            ObservableCollection<EmployeeInfo>
    {
        public myEmployees()
        {
            Add(new EmployeeInfo("Jesper", "Aaberg", "1234567890"));
            Add(new EmployeeInfo("Dominik", "Paiha", "9876543210"));
            Add(new EmployeeInfo("Yale", "Li", "2387534291"));
            Add(new EmployeeInfo("Muru", "Subramani", "4939291992"));
        }
    }
}