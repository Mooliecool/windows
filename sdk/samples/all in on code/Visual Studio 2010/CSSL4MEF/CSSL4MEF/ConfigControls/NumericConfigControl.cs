/****************************** Module Header ******************************\
* Module Name:                NumericConfigControl.cs
* Project:                    CSSL4MEF
* Copyright (c) Microsoft Corporation.
* 
* NumericConfigControl
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using ConfigControl.Contract;
using System.Reflection;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Windows.Data;

namespace CSSL4MEF.ConfigControls
{
    /// <summary>
    /// Numeric class config control.
    /// </summary>
    [ExportConfigControl(Name = "Numeric", PropertyValueType = typeof(double))]
    public class NumericConfigControl : IConfigControl
    {
        public FrameworkElement CreateView(PropertyInfo property)
        {
            var rangeAttr = property.GetCustomAttributes(typeof(RangeAttribute),
                false).FirstOrDefault() as RangeAttribute;
            if (rangeAttr != null)
            {
                var inputControl = new Grid();

                inputControl.ColumnDefinitions.Add(new ColumnDefinition
                {
                    Width = new GridLength(1, GridUnitType.Star)
                });
                inputControl.ColumnDefinitions.Add(new ColumnDefinition
                {
                    Width = new GridLength(0, GridUnitType.Auto)
                });

                var slider = new Slider();
                slider.Minimum = Convert.ToDouble(rangeAttr.Minimum);
                slider.Maximum = Convert.ToDouble(rangeAttr.Maximum);

                var binding = new Binding(property.Name);
                binding.Mode = BindingMode.TwoWay;
                slider.SetBinding(Slider.ValueProperty, binding);
                inputControl.Children.Add(slider);

                var tb = new TextBlock();
                slider.ValueChanged += (s, e) =>
                {
                    tb.Text = e.NewValue.ToString(".00");
                };

                inputControl.Children.Add(tb);
                Grid.SetColumn(tb, 1);
                return inputControl;
            }
            else
            {
                return new StringConfigControl().CreateView(property);
            }

        }

        public MatchResult MatchTest(PropertyInfo property)
        {
            var result = MatchResult.NotMatch;
            var type = property.PropertyType;

            if (type.IsAssignableFrom(typeof(int)) ||
                type.IsAssignableFrom(typeof(double)) ||
                type.IsAssignableFrom(typeof(decimal)))
                result = MatchResult.Match;

            if (property.GetCustomAttributes(typeof(RangeAttribute), false).Count() > 0)
                result = MatchResult.Recommended;

            return result;
        }
    }
}
