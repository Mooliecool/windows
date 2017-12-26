//------------------------------------------------------------------------------
// <copyright file="CodeChecksumPragma.cs" company="Microsoft">
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

namespace System.CodeDom {

    using System.Diagnostics;
    using System;
    using Microsoft.Win32;
    using System.Collections;
    using System.Runtime.InteropServices;

    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeRegionDirective: CodeDirective {
        private string regionText;
        private CodeRegionMode regionMode;

        public CodeRegionDirective() {
        }
        
        public CodeRegionDirective(CodeRegionMode regionMode, string regionText) {
            this.RegionText = regionText;
            this.regionMode = regionMode;
        }

        public string RegionText {
            get {
                return (regionText == null) ? string.Empty : regionText;
            }
            set {
                regionText = value;
            }
        }
                
        public CodeRegionMode RegionMode {
            get {
                return regionMode;
            }
            set {
                regionMode = value;
            }
        }
    }
}
