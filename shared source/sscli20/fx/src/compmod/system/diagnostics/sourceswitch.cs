//------------------------------------------------------------------------------
// <copyright file="SourceSwitch.cs" company="Microsoft">
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

using System;
using System.Collections;
using System.Collections.Specialized;
using System.Threading;
using System.Security;
using System.Security.Permissions;

namespace System.Diagnostics {
    public class SourceSwitch : Switch {
        public SourceSwitch(string name) : base(name, String.Empty) {}

        public SourceSwitch(string displayName, string defaultSwitchValue) 
            : base(displayName, String.Empty, defaultSwitchValue) { }

        public SourceLevels Level {
            get {
                return (SourceLevels) SwitchSetting;
            }
            [SecurityPermission(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            set {
                SwitchSetting = (int) value;
            }
        }

        public bool ShouldTrace(TraceEventType eventType) {
            return (SwitchSetting & (int) eventType) != 0;
        }

        protected override void OnValueChanged() {
            SwitchSetting = (int) Enum.Parse(typeof(SourceLevels), Value, true);
        }
    }
}
