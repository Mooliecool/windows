/****************************** Module Header ******************************\
* Module Name:                ColorConfigControl.cs
* Project:                    ConfigControl.Extension
* Copyright (c) Microsoft Corporation.
* 
* ColorConfigControl
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
using System.Windows.Data;
using System.Windows.Media;

namespace ConfigControl.Extension
{
    /// <summary>
    /// Color type config control
    /// </summary>
    [ExportConfigControl(Name="ColorPicker",PropertyValueType=typeof(Color))]
    public class ColorConfigControl:IConfigControl
    {

        public FrameworkElement CreateView(System.Reflection.PropertyInfo property)
        {
            var inputcontrol = new ColorPicker();
            var binding = new Binding(property.Name);
            binding.Mode = BindingMode.TwoWay;
            inputcontrol.SetBinding(ColorPicker.SelectedColorProperty, binding);
            return inputcontrol;
        }

        public MatchResult MatchTest(System.Reflection.PropertyInfo property)
        {
            if (property.PropertyType.Equals(typeof(Color)))
                return MatchResult.Match;
            else
                return MatchResult.NotMatch;
        }
    }
}
