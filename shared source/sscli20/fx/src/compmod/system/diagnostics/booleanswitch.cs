//------------------------------------------------------------------------------
// <copyright file="BooleanSwitch.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

/*
 */

namespace System.Diagnostics {
    using System.Diagnostics;
    using System;
    using System.Security;
    using System.Security.Permissions;

    /// <devdoc>
    ///    <para>Provides a simple on/off switch that can be used to control debugging and tracing
    ///       output.</para>
    /// </devdoc>
    [SwitchLevel(typeof(bool))]
    public class BooleanSwitch : Switch {
        /// <devdoc>
        /// <para>Initializes a new instance of the <see cref='System.Diagnostics.BooleanSwitch'/>
        /// class.</para>
        /// </devdoc>
        public BooleanSwitch(string displayName, string description)
            : base(displayName, description) {
        }

        public BooleanSwitch(string displayName, string description, string defaultSwitchValue) 
            : base(displayName, description, defaultSwitchValue) { }

        /// <devdoc>
        ///    <para>Specifies whether the switch is enabled
        ///       (<see langword='true'/>) or disabled (<see langword='false'/>).</para>
        /// </devdoc>
        public bool Enabled {
            get {
                return (SwitchSetting == 0) ? false : true;
            }
            [SecurityPermission(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            set {
                SwitchSetting = value ? 1 : 0;
            }
        }

        protected override void OnValueChanged() {
            bool b;
            if (Boolean.TryParse(Value, out b))
                SwitchSetting = ( b ? 1 : 0);
            else
                base.OnValueChanged();
        }
    }
}

