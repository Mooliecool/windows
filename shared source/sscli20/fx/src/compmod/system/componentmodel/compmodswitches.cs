//------------------------------------------------------------------------------
// <copyright file="CompModSwitches.cs" company="Microsoft">
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

namespace System.ComponentModel {
    using System.Configuration.Assemblies;
    using System.Diagnostics;
    using System.Runtime.InteropServices;
    using System.Runtime.Remoting;
    using System.Runtime.Serialization.Formatters;
    using System.Security.Permissions;
    using System.Threading;

    /// <internalonly/>    
    [HostProtection(SharedState = true)]
    internal static class CompModSwitches
    {
        
        private static BooleanSwitch commonDesignerServices;
        private static TraceSwitch eventLog;
                
        public static BooleanSwitch CommonDesignerServices {
            get {
                if (commonDesignerServices == null) {
                    commonDesignerServices = new BooleanSwitch("CommonDesignerServices", "Assert if any common designer service is not found.");
                }
                return commonDesignerServices;
            }
        }   
        
        public static TraceSwitch EventLog {
            get {
                if (eventLog == null) {
                    eventLog = new TraceSwitch("EventLog", "Enable tracing for the EventLog component.");
                }
                return eventLog;
            }
        }
                                                                                                                                                                               
    }
}

