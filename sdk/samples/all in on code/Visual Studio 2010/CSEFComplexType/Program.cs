/****************************** Module Header ******************************\
* Module Name:    Program.cs
* Project:        CSEFComplexType
* Copyright (c) Microsoft Corporation.
*
* The CSEFComplexType example illustrates how to work with the Complex Type
* which is new in Entity Framework 4.0.  It shows how to add Complex Type 
* properties to entities, how to map Complex Type properties to table columns,
* and how to map a Function Import to a Complex Type.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directive
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
#endregion

namespace CSEFComplexType
{
    class Program
    {
        static void Main(string[] args)
        {
            InsertEntityWithComplexTypeProperties();
            QueryEntityWithComplexTypeProperties();
            GetComplexTypeByFunctionImport();
        }


        /// <summary>
        /// Insert Person entity with Complex Types PersonAddress and 
        /// PersonDate
        /// </summary>
        static void InsertEntityWithComplexTypeProperties()
        {
            using (ComplexTypeEntities context = new ComplexTypeEntities())
            {
                // Create a new PersonAddress Complex Type object.
                PersonAddress personAddress = new PersonAddress()
                {
                    Address = "Shanghai, China",
                    Postcode = "200000"
                };

                // Create a new PersonDate Complex Type object.
                PersonDate personDate = new PersonDate()
                {
                    EnrollmentDate = DateTime.Now
                };

                // Create a new Person entity with the two Complex Types.
                Person person = new Person()
                {
                    PersonID = 40,
                    FirstName = "Lingzhi",
                    LastName = "Sun",
                    // Set PersonDate Complex Type property.
                    PersonDate = personDate,
                    // Set the PersonAddress Complex Type property.
                    PersonAddress = personAddress
                };

                try
                {
                    context.AddToPeople(person);
                    context.SaveChanges();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }


        /// <summary>
        /// Query Person entity with Complex Types PersonAddress and 
        /// PersonDate
        /// </summary>
        static void QueryEntityWithComplexTypeProperties()
        {
            using (ComplexTypeEntities context = new ComplexTypeEntities())
            {
                // Get the Person entity whose PersonID is 40.
                // Note: The Single method is new in EF 4.0.
                Person person = context.People.Single(p => p.PersonID == 40);

                // Display the Person information and Complex Type properties.
                Console.WriteLine(
                    "Person:{0} {1}\n" + 
                    "EnrollmentDate:{2}\n" +
                    "Address:{3}\n" + 
                    "Postcode:{4}", 
                    person.FirstName, person.LastName, 
                    person.PersonDate.EnrollmentDate,
                    person.PersonAddress.Address, 
                    person.PersonAddress.Postcode);
            }
        }


        /// <summary>
        /// Retrieve Complex Type value by Function Import
        /// </summary>
        static void GetComplexTypeByFunctionImport()
        {
            using (ComplexTypeEntities context = new ComplexTypeEntities())
            {
                // Retrieve the PersonName Complex Type object by the
                // Function Import GetPersonNameByPersonID.
                var personNames = context.GetPersonNameByPersonID(40);

                // Display the PersonName Complex Type object.
                // Note: The ToString() method of the PersonName class has 
                // been overrided.  
                Console.WriteLine("Person name whose ID is 40: {0}", 
                    personNames.Single());
            }
        }
    }


    // Complex Type PersonName partial class
    public partial class PersonName
    {
        // Override the ToString() method
        public override string ToString()
        {
            return this.FirstName + " " + this.LastName;
        }
    }
}
