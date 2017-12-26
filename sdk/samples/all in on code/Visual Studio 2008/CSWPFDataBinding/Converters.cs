/************************************* Module Header **************************************\
* Module Name:  Converters.cs
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
using System.Windows.Data;

namespace CSWPFDataBinding
{
    public class SalaryFormmatingConverter:IValueConverter
    {

        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            double dolloars =0;
            string formattedSalary = string.Empty;
            
            if (value == null)
            {
                throw new NullReferenceException("value can not be null");
            }
            else
            {
                Double.TryParse(value.ToString(), out dolloars);
            }
            formattedSalary = String.Format("Total={0}$", dolloars);
            return formattedSalary;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
             double dolloars =0;
            if (value == null)
            {
                throw new NullReferenceException("value can not be null");
            }

            Double.TryParse(value.ToString().TrimStart(new Char[] {'T', 't', 'o', 'a', 'l', '='}).TrimEnd('$'), out dolloars);        
            return dolloars;
        }

        #endregion
    }
}
