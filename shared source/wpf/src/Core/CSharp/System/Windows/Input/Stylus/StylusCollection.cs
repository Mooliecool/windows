// <copyright file="StylusDeviceCollection.cs" company="Microsoft">
//    Copyright (C) Microsoft Corporation.  All rights reserved.
// </copyright>

using System;
using System.Windows;
using System.Security;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows.Media;

using SR=MS.Internal.PresentationCore.SR;
using SRID=MS.Internal.PresentationCore.SRID;

namespace System.Windows.Input
{
    /////////////////////////////////////////////////////////////////////////
    /// <summary>
    ///		Collection of the stylus devices that are available on the tablet.
    /// </summary>
    public class StylusDeviceCollection : ReadOnlyCollection<StylusDevice>
    {
        /////////////////////////////////////////////////////////////////////

        internal StylusDeviceCollection(StylusDevice[] styluses)
            : base(new List<StylusDevice>(styluses))
        {
        }

        /////////////////////////////////////////////////////////////////////
        /// <SecurityNote>
        ///     Critical: calls SecurityCritical method stylusDevice.Dispose.
        /// </SecurityNote>
        [SecurityCritical]
        internal void Dispose()
        {
            foreach (StylusDevice stylusDevice in this.Items)
            {
                stylusDevice.Dispose();
            }
        }

        /////////////////////////////////////////////////////////////////////
        
        internal void AddStylusDevice(int index, StylusDevice stylusDevice)
        {
            base.Items.Insert(index, stylusDevice); // add it to our list.
        }
        
    }
}
