
//------------------------------------------------------------------------------
// <copyright file="Component.cs" company="Microsoft">
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

[assembly: System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode", Scope="member", Target="System.ComponentModel.CompModSwitches.get_DGEditColumnEditing():System.Diagnostics.TraceSwitch")]
[assembly: System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode", Scope="member", Target="System.ComponentModel.CompModSwitches.get_LayoutPerformance():System.Diagnostics.TraceSwitch")]

namespace System.ComponentModel {
    using System.Diagnostics;  

    /// <internalonly/>
    // Shared between dlls
    
    internal static class CompModSwitches {




        private static TraceSwitch handleLeak;

        public static TraceSwitch HandleLeak {
            get {
                if (handleLeak == null) {
                    handleLeak = new TraceSwitch("HANDLELEAK", "HandleCollector: Track Win32 Handle Leaks");
                }
                return handleLeak;
            }
        }

        private static BooleanSwitch traceCollect;
        public static BooleanSwitch TraceCollect {
            get {
                if (traceCollect == null) {
                    traceCollect = new BooleanSwitch("TRACECOLLECT", "HandleCollector: Trace HandleCollector operations");
                }
                return traceCollect;
            }
        }

    }
}
