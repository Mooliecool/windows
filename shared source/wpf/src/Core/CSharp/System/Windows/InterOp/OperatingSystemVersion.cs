//---------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation.  All rights reserved.
//
//---------------------------------------------------------------------------

using System;

namespace System.Windows.Interop
{
    // This is not a complete list of operating system versions and service packs.
    // These are the interesting versions where features or behaviors were introduced 
    // or changed, and code needs to detect those points and do different things.
    internal enum OperatingSystemVersion
    {
        /// <summary>
        ///     WPF minimum requirement.
        /// </summary>
        WindowsXPSP2,

        /// <summary>
        ///     Introduced Aero glass, DWM, new common dialogs.
        /// </summary>
        WindowsVista,

        /// <summary>
        ///     Introduced multi-touch.
        /// </summary>
        Windows7,
        
        /// <summary>
        ///     Introduced feature on demand
        /// </summary>
        Windows8,
    }
}
