/****************************** Module Header ******************************\
* Module Name:    TableMergingClass.cs
* Project:        CSEFEntityDataModel
* Copyright (c) Microsoft Corporation.
*
* This example demonstrates how merge tables into one entity and query the 
* fields from the two tables.
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


namespace CSEFEntityDataModel.TableMerging
{
    public static class TableMergingClass
    {
        // Test all the methods in TableMergingClass
        public static void TableMergingTest()
        {
            Query();
        }

        // Query the first Person in the merged table
        public static void Query()
        {
            using (EFTblMergeEntities context = new EFTblMergeEntities())
            {
                Person person = (context.Person).First();

                Console.WriteLine("{0}   \n{1} {2}   \n{3}",
                    person.PersonID, 
                    person.FirstName, 
                    person.LastName, 
                    person.Address);
            }
        }
    }

}
