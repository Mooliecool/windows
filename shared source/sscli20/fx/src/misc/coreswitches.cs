
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

namespace System.ComponentModel {
    using System.Diagnostics;
    using System.Diagnostics.CodeAnalysis;

    /// <internalonly/>
    // Shared between dlls
    [SuppressMessage("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode")]
    internal static class CoreSwitches {   
    
        private static BooleanSwitch perfTrack;                        
        
        public static BooleanSwitch PerfTrack {            
            [SuppressMessage("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode")]
            get {
                if (perfTrack == null) {
                    perfTrack  = new BooleanSwitch("PERFTRACK", "Debug performance critical sections.");       
                }
                return perfTrack;
            }
        }
    }
}    

