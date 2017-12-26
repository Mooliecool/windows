/****************************** Module Header ******************************\
* Module Name:                EffectConverter.cs
* Project:                    CSSL4MEF
* Copyright (c) Microsoft Corporation.
* 
* ValueConverter.
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
using System.Windows.Data;
using System.Windows.Media.Effects;

namespace CSSL4MEF
{
    /// <summary>
    /// ValueConverter, converting EnumEffect enum type to Effect type
    /// </summary>
    public class EffectConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            switch ((EnumEffect)value)
            {
                case EnumEffect.Blur:
                    return new BlurEffect();
                case EnumEffect.Shadow:
                    return new DropShadowEffect();
                default:
                    return null;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
