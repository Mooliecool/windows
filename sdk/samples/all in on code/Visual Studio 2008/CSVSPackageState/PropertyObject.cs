/************************************* Module Header **************************************\
* Module Name:  PropertyObject.cs
* Project:      CSVSPackageState
* Copyright (c) Microsoft Corporation.
* 
* The Visual Studio package state sample demostrate the state persisting for
* application options and show object states in properties window.
*
* The sample doesn't include the state management for solution and project,
* which will be included in project package sample.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 9/6/2009 1:00 PM Hongye Sun Created
\******************************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AllInOne.CSVSPackageState
{
    public class PropertyObject
    {
        private int propertyInteger = 123;

        public int PropertyInteger
        {
            get { return propertyInteger; }
            set { propertyInteger = value; }
        }

        private string propertyString = "AllInOne";

        public string PropertyString
        {
            get { return propertyString; }
            set { propertyString = value; }
        }
    }
}
