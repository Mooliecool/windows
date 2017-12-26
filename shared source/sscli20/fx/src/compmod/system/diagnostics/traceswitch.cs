//------------------------------------------------------------------------------
// <copyright file="TraceSwitch.cs" company="Microsoft">
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
    using System;
    using System.ComponentModel;
    using System.Security;
    using System.Security.Permissions;

    /// <devdoc>
    ///    <para>Provides a multi-level switch to enable or disable tracing
    ///       and debug output for a compiled application or framework.</para>
    /// </devdoc>
    [SwitchLevel(typeof(TraceLevel))]
    public class TraceSwitch : Switch {

        /// <devdoc>
        /// <para>Initializes a new instance of the <see cref='System.Diagnostics.TraceSwitch'/> class.</para>
        /// </devdoc>
        public TraceSwitch(string displayName, string description)
            : base(displayName, description) {
        }

        public TraceSwitch(string displayName, string description, string defaultSwitchValue) 
            : base(displayName, description, defaultSwitchValue) { }

        /// <devdoc>
        ///    <para>Gets or sets the trace
        ///       level that specifies what messages to output for tracing and debugging.</para>
        /// </devdoc>
        public TraceLevel Level {
            get {
                return (TraceLevel)SwitchSetting;
            }

            [SecurityPermission(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            set {
                if (value < TraceLevel.Off || value > TraceLevel.Verbose)
                    throw new ArgumentException(SR.GetString(SR.TraceSwitchInvalidLevel));
                SwitchSetting = (int)value;
            }
        }

        /// <devdoc>
        ///    <para>Gets a value
        ///       indicating whether the <see cref='System.Diagnostics.TraceSwitch.Level'/> is set to
        ///    <see langword='Error'/>, <see langword='Warning'/>, <see langword='Info'/>, or
        ///    <see langword='Verbose'/>.</para>
        /// </devdoc>
        public bool TraceError {
            get {
                return (Level >= TraceLevel.Error);
            }
        }

        /// <devdoc>
        ///    <para>Gets a value
        ///       indicating whether the <see cref='System.Diagnostics.TraceSwitch.Level'/> is set to
        ///    <see langword='Warning'/>, <see langword='Info'/>, or <see langword='Verbose'/>.</para>
        /// </devdoc>
        public bool TraceWarning {
            get {
                return (Level >= TraceLevel.Warning);
            }
        }

        /// <devdoc>
        ///    <para>Gets a value
        ///       indicating whether the <see cref='System.Diagnostics.TraceSwitch.Level'/> is set to
        ///    <see langword='Info'/> or <see langword='Verbose'/>.</para>
        /// </devdoc>
        public bool TraceInfo {
            get {
                return (Level >= TraceLevel.Info);
            }
        }

        /// <devdoc>
        ///    <para>Gets a value
        ///       indicating whether the <see cref='System.Diagnostics.TraceSwitch.Level'/> is set to
        ///    <see langword='Verbose'/>.</para>
        /// </devdoc>
        public bool TraceVerbose {
            get {
                return (Level == TraceLevel.Verbose);
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Update the level for this switch.
        ///    </para>
        /// </devdoc>
        protected override void OnSwitchSettingChanged() {
            int level = SwitchSetting;
            if (level < (int)TraceLevel.Off) {
                Trace.WriteLine(SR.GetString(SR.TraceSwitchLevelTooLow, DisplayName));
                SwitchSetting = (int)TraceLevel.Off;
            }
            else if (level > (int)TraceLevel.Verbose) {
                Trace.WriteLine(SR.GetString(SR.TraceSwitchLevelTooHigh, DisplayName));
                SwitchSetting = (int)TraceLevel.Verbose;
            }
        }

        protected override void OnValueChanged() {
            SwitchSetting = (int) Enum.Parse(typeof(TraceLevel), Value, true);
        }
    }
}

