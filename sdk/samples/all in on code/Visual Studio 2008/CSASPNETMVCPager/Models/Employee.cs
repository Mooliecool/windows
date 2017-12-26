using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace CSASPNETMVCPager.Models
{
    public class Employee
    {
        public string Name
        {
            get;
            set;
        }
        public string Sex
        {
            get;
            set;
        }
        public int Age
        {
            get;
            set;
        }
        public string Address
        {
            get;
            set;
        }
    }
    public static class EmployeeSet
    {

        public static List<Employee> Employees
        {
            get
            {
                List<Employee> empList = new List<Employee>();
                int count = 50;
                Random ran = new Random();
                for (int i = 0; i < count; i++)
                {
                    Employee e = new Employee();
                    e.Name = "Name" + i.ToString();
                    e.Sex = ran.Next(4) / 2 == 0 ? "Male" : "Female";
                    e.Age = 20 + i;
                    e.Address = "Address" + i.ToString();
                    empList.Add(e);
                }
                return empList;
            }

        }
    }
}
