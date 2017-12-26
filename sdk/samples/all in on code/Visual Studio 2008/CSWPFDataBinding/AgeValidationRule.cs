/************************************* Module Header **************************************\
* Module Name:  AgeValidationRule.cs
* Project:      CSWPFDataBinding
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to use DataBinding in WPF
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 10/29/2009 3:00 PM Bruce Zhou Created
 * 
\******************************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;

namespace CSWPFDataBinding
{
    public class AgeValidationRule : ValidationRule
    {

        /// <summary>
        /// When overridden in a derived class, performs validation checks on a value.
        /// </summary>
        /// <param name="value">The value from the binding target to check.</param>
        /// <param name="cultureInfo">The culture to use in this rule.</param>
        /// <returns>
        /// A <see cref="T:System.Windows.Controls.ValidationResult"/> object.
        /// </returns>
        public override ValidationResult Validate(object value, 
            System.Globalization.CultureInfo cultureInfo)
        {
            int age =0;
            if (value != null)
            {
                 Int32.TryParse(value.ToString(),out age);
            }
            if (age < -1 || age > 300)
            {
                return new ValidationResult(false, "Please input corrent age");
            }
            else
            {
                return new ValidationResult(true, null);
            }
        }
    }
}
