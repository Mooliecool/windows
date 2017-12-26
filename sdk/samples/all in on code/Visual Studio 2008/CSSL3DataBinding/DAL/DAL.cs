/****************************** Module Header ******************************\
* Module Name:  DAL.cs
* Project:      CSSL3DataBinding
* Copyright (c) Microsoft Corporation.
* 
* This module contains Data Access Layer code and custom validation class. 
* For the simplicity, it gets in-momory data.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 11/26/2009 04:00 PM Allen Chen Created
\***************************************************************************/

using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Collections;
using System.ComponentModel.DataAnnotations;

namespace CSSL3DataBinding.DAL
{
    public static class CustomerGenerator
    {  
        private static ObservableCollection<Customer> _customercollection=new ObservableCollection<Customer>() {
            new Customer(){ ID=1, Name="Allen Chen"},
            new Customer(){ ID=2, Name="Mick Sun"},
            new Customer(){ ID=3, Name="Jialiang Ge"}};
        private static Customer _customer=new Customer() { ID = 1, Name = "Allen Chen" };
      
        public static object GetSingleCustomer() 
        {
            return _customer;        
        }

        public static IEnumerable GetCustomersList() 
        {
            return _customercollection;
        }

    }

    public class Customer : INotifyPropertyChanged
    {
        public int _id;
        // Use custom validation class for this property
        [CustomValidation(typeof(MyValidation),"Validate")]
        public int ID
        {            
            get
            {
                return _id;
            }
            set
            {
               
                Validator.ValidateProperty(value,
                       new ValidationContext(this, null, null) { MemberName = "ID" });
                _id = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("ID"));
                }
            }
        }
        private string _name;
        // Use DataAnnotation to restrict the length of Name property
        [StringLength(30, MinimumLength = 1, ErrorMessage =
           "Name must be between 1 and 30 characters long.")]
        public string Name
        {
            get
            {
                return _name;
            }
            set
            {

                Validator.ValidateProperty(value,
                       new ValidationContext(this, null, null) { MemberName = "Name" });
                _name = value;
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Name"));
                }
            }
        }

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        #endregion
    }

    public static class MyValidation
    {
        public static ValidationResult Validate(object property,
          ValidationContext context)
        {
            // You can get Property name from context.MemberName. It's not used here for simplicity.
            int customerid;
            // For the simplicity, the change is invalid if new ID equals 11.
            if (Int32.TryParse(property.ToString(),out customerid) && customerid == 11)
            {
                ValidationResult validationResult = new ValidationResult("Custom Validation Failed. ID cannot be 11");
                return validationResult;
            }
            else 
            {
                return ValidationResult.Success;
            }
        }
    }

}
