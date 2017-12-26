//-----------------------------------------------------------------------
// <copyright company="Microsoft">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>
//-----------------------------------------------------------------------

using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using System.Collections.Generic;

namespace System.Windows.Input
{
    /// <summary>
    /// Stylus data is made up of n number of properties. Each property can contain one or more
    /// values such as x or y coordinate or button states.
    /// This enum defines the various possible units for the values in the stylus data
    /// </summary>
    /// <ExternalAPI/>
    public enum StylusPointPropertyUnit
    {
        /// <summary>Specifies that the units are unknown.</summary>
        /// <ExternalAPI/>
        None = 0,
        /// <summary>Specifies that the property value is in inches (distance units).</summary>
        /// <ExternalAPI/>
        Inches = 1,
        /// <summary>Specifies that the property value is in centimeters (distance units).</summary>
        /// <ExternalAPI/>
        Centimeters = 2,
        /// <summary>Specifies that the property value is in degrees (angle units).</summary>
        /// <ExternalAPI/>
        Degrees = 3,
        /// <summary>Specifies that the property value is in radians (angle units).</summary>
        /// <ExternalAPI/>
        Radians = 4,
        /// <summary>Specifies that the property value is in seconds (angle units).</summary>
        /// <ExternalAPI/>
        Seconds = 5,
        /// <ExternalAPI/>
        /// <summary>Specifies that the property value is in pounds (force, or mass, units).</summary>
        Pounds = 6,
        /// <ExternalAPI/>
        /// <summary>Specifies that the property value is in grams (force, or mass, units).</summary>
        Grams = 7
    }

    /// <summary>
    /// Used to validate the enum
    /// </summary>
    internal static class StylusPointPropertyUnitHelper
    {
        internal static bool IsDefined(StylusPointPropertyUnit unit)
        {
            if (unit >= StylusPointPropertyUnit.None && unit <= StylusPointPropertyUnit.Grams)
            {
                return true;
            }
            return false;
        }
    }
}
