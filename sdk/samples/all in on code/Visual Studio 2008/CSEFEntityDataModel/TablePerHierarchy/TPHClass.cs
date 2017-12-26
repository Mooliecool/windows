/****************************** Module Header ******************************\
* Module Name:    TPHClass.cs
* Project:        CSEFEntityDataModel
* Copyright (c) Microsoft Corporation.
*
* This example demonstrates how to establish table per hierarchy inheritance.
* A table-per-type model is a way to model inheritance where each entity is 
* mapped to a distinct table in the store. Then it shows how to query a list 
* of people, get the corresponding properties of Person, Student and 
* BusinessStudent.
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


namespace CSEFEntityDataModel.TablePerHierarchy
{
    public static class TPHClass
    {
        // Test the query method in TPHClass
        public static void TPHTest()
        {
            Query();
        }

        // Query a list of people, print out the properties of Person, 
        // Student and BusinessStudent
        public static void Query()
        {
            using (EFTPHEntities context = new EFTPHEntities())
            {
                var people = from p in context.People 
                             select p;

                foreach (var p in people)
                {
                    Console.WriteLine("Student {0} {1}",
                        p.LastName,
                        p.FirstName);

                    if (p is Student)
                    {
                        Console.WriteLine("EnrollmentDate: {0}", 
                            ((Student)p).EnrollmentDate);
                    }
                    if (p is BusinessStudent)
                    {
                        Console.WriteLine("BusinessCredits: {0}", 
                            ((BusinessStudent)p).BusinessCredits);
                    }
                }

            }
        }
    }
}
