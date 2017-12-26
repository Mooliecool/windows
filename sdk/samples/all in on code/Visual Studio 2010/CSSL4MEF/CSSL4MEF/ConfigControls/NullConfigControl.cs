/****************************** Module Header ******************************\
* Module Name:                NullConfigControl.cs
* Project:                    CSSL4MEF
* Copyright (c) Microsoft Corporation.
* 
* NullConfigControl
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
using System.Windows.Media;

namespace CSSL4MEF.ConfigControls
{
    /// <summary>
    /// Default config control, display match failure information. 
    /// </summary>
    [ExportConfigControl(Name = "Null", PropertyValueType = typeof(object))]
    public class NullConfigControl : IConfigControl
    {
        public FrameworkElement CreateView(PropertyInfo property)
        {
            var inputControl = new TextBlock();
            inputControl.Foreground = new SolidColorBrush(Colors.Red);
            inputControl.TextWrapping = TextWrapping.Wrap;
            inputControl.Text = "No appropriate control for this property.";
            return inputControl;
        }

        public MatchResult MatchTest(PropertyInfo property)
        {
            return MatchResult.NotRecommended;
        }
    }
}
