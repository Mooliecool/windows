/****************************** Module Header ******************************\
* Module Name:    One2OneClass.cs
* Project:        CSEFEntityDataModel
* Copyright (c) Microsoft Corporation.
*
* This example illustrates how to insert, update and query the two entities 
* with one to one association.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
#endregion


namespace CSEFEntityDataModel.One2One
{
    public static class One2OneClass
    {
        // Test all the mothods in One2OneClass
        public static void One2OneTest()
        {
            InsertPersonWithPersonAddress();

            UpdatePerson();
        }

        // Insert new person with new personAddress
        public static void InsertPersonWithPersonAddress()
        {
            using (EFO2OEntities context = new EFO2OEntities())
            {
                Person person = new Person() 
                {
                    FirstName = "Lingzhi", 
                    LastName ="Sun" 
                };

                // The PersonID in PersonAddress will be 27 because it 
                // depends on person.PersonID
                PersonAddress personAddress = new PersonAddress() 
                { 
                    PersonID = 100, 
                    Address = "Shanghai", 
                    Postcode = "200021" 
                };

                // Set navigation property (one-to-one)
                person.PersonAddress = personAddress;

                context.AddToPerson(person);

                try
                {
                    Console.WriteLine("Inserting a person with "
                        + "person address");

                    context.SaveChanges();

                    Query();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }


        // Get all the people with their addresses
        public static void Query()
        {
            using (EFO2OEntities context = new EFO2OEntities())
            {
                var query = from p in context.Person.Include("PersonAddress") 
                            select p;

                Console.WriteLine("People with their addresses:");

                foreach (Person p in query)
                {
                    Console.WriteLine("{0} {1}", p.PersonID, p.LastName);

                    if (p.PersonAddress != null)
                    {
                        Console.WriteLine("   {0}", p.PersonAddress.Address);
                    }
                }

                Console.WriteLine();
            }
        }


        // Update one existing person
        public static void UpdatePerson()
        {
            using (EFO2OEntities context = new EFO2OEntities())
            {
                Person person = new Person();

                person.PersonID = 1;

                context.AttachTo("Person", person);

                person.LastName = "Chen";

                // Set the foreign key as null
                person.PersonAddress = null;

                try
                {
                    Console.WriteLine("Modifying Person 1's LastName to {0}"
                        + ", and PersonAddress to null", person.LastName);

                    context.SaveChanges();

                    Query();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }

    }
   
}
