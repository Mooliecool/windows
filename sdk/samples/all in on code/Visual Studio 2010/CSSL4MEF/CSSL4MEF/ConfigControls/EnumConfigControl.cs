/****************************** Module Header ******************************\
* Module Name:                EnumConfigControl.cs
* Project:                    CSSL4MEF
* Copyright (c) Microsoft Corporation.
* 
* EnumConfigControl
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
using System.Collections.Generic;

namespace CSSL4MEF.ConfigControls
{
    /// <summary>
    /// Enum type config control.
    /// </summary>
    [ExportConfigControl(Name = "Enum", PropertyValueType = typeof(object))]
    public class EnumConfigControl : IConfigControl
    {
        public FrameworkElement CreateView(PropertyInfo property)
        {
            var inputControl = new ComboBox();
            var enumtype = property.PropertyType;

            inputControl.ItemsSource = GetNameList(enumtype);
            var binding = new Binding(property.Name);
            binding.Mode = BindingMode.TwoWay;
            inputControl.SetBinding(ComboBox.SelectedItemProperty, binding);

            return inputControl;
        }

        static List<object> GetNameList(Type type)
        {
            if (!type.IsEnum)
            {
                throw new Exception("Not an enumeration type.");
            }
            List<object> nameList = new List<object>();

            FieldInfo[] fiArray = type.GetFields(BindingFlags.Public | BindingFlags.Static);
            foreach (FieldInfo fi in fiArray)
            {
                nameList.Add(Enum.Parse(type, fi.Name, true));
            }

            return nameList;
        }

        public MatchResult MatchTest(PropertyInfo property)
        {
            if (property.PropertyType.IsEnum)
                return MatchResult.Match;
            else
                return MatchResult.NotMatch;
        }
    }
}
