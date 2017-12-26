/****************************** Module Header ******************************\
* Module Name:	Program.cs
* Project:		CSEFForeignKeyAssociation
* Copyright (c) Microsoft Corporation.
* 
* CSEFForeignKeyAssociation example demonstrates one of the new features of
* Entity Framework(EF) 4.0, Foreign Key Association.  This example compares 
* the new Foreign Key Association and the Independent Association and shows 
* how to insert new related entities, insert by existing entities and update
* existing entities with the two associations.
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

namespace CSEFForeignKeyAssociation
{
    class Program
    {
        static void Main(string[] args)
        {
            // Test the insert and update methods with the Foreign Key 
            // Association.
            FKAssociation.FKAssociationClass.Test();
            
            // Test the insert and update methods with the Independent
            // Association.
            IndependentAssociation.IndependentAssociationClass.Test();
        }
    }
}
