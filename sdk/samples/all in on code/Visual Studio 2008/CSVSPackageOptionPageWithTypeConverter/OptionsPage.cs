/***************************************** Module Header *****************************\
* Module Name:  OptionsPage.cs
* Project:      CSVSPackageOptionPageWithTypeConverter
* Copyright (c) Microsoft Corporation.
* 
* This sample demonstrates how to use TypeConverter in Option Page.
* A type converter can be used to convert values between data types, and to
* assist property configuration at design time by providing text-to-value
* conversion or a drop-down list of values to select from.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 02/25/2010 04:35 PM Wesley Yao Created
\*************************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.VisualStudio.Shell;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Microsoft.CSVSPackageOptionPageWithTypeConverter
{
    ///////////////////////////////////////////////////////////////////////////////
    // Define an enum type which will be shown in the Option Page.
    // 
    public enum MyEnumProperty {
        None,
        First,
        Second,
        Third,
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Create a class named OptionsPage which derived from DialogPage class, add
    // a MyEnumProperty property in it.
    // 
    class OptionsPage : DialogPage
    {
        #region Fields
        private MyEnumProperty myProperty = MyEnumProperty.None;
        #endregion Fields

        #region Properties
        [Category("Enum Options")]
        [Description("My enum option")]
        [TypeConverter(typeof(EnumTypeConverter))]
        public MyEnumProperty MyProperty
        {
            get
            {
                return myProperty;
            }
            set
            {
                myProperty = value;
            }
        }
        #endregion Properties
    }
}
