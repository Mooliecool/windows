/****************************** Module Header ******************************\
* Module Name:    TableSplittingClass.cs
* Project:        CSEFEntityDataModel
* Copyright (c) Microsoft Corporation.
*
* This example demonstrates how split one table into two entities. Then it 
* shows how to insert records into the two entities and query the result.
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


namespace CSEFEntityDataModel.TableSplitting
{
    public static class TableSplittingClass
    {
        public static void TableSplittingTest()
        {
            InsertQueryPersonWithPersonDetail();
        }


        // Insert and Query PersonWithPersonDetail
        public static void InsertQueryPersonWithPersonDetail()
        {
            // Create the new Person entity
            Person person = new Person();
            person.FirstName = "Lopez";
            person.LastName = "Typot";

            // Create the new PersonDetail entity
            PersonDetail personDetail = new PersonDetail();
            personDetail.PersonCategory = 0;
            personDetail.HireDate = System.DateTime.Now;

            // Set the PersonDetail to Person
            person.PersonDetail = personDetail;

            // Insert PersonWithPersonDetail
            using (EFTblSplitEntities context = new EFTblSplitEntities())
            {
                context.AddToPerson(person);

                Console.WriteLine("Saving person {0} {1}", person.FirstName, 
                    person.LastName);

                // Note that personDetail.PersonID is the same as  
                // person.PersonID. This is why we love the Entity Framework.
                Console.WriteLine("Saving person detail with the same " + 
                    "personID.\n");

                context.SaveChanges();
            }

            // Query PersonWithPersonDetail
            using (EFTblSplitEntities context = new EFTblSplitEntities())
            {
                // Retrieve the newly inserted person
                Person person2 = (from p in context.Person
                                  where p.PersonID == person.PersonID
                                  select p).FirstOrDefault();

                Console.WriteLine(
                    "Retrieved person {0} with person detail",
                    person2.PersonID,
                    person2.PersonDetail);

                person2.PersonDetailReference.Load();

                Console.WriteLine(
                    "Retrieved hiredate for person detail {0}",
                    person2.PersonDetail.HireDate);
            }
        }
    }
}
