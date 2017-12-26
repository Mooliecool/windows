//------------------------------------------------------------------------------
// <copyright file="ISupportInitialize.cs" company="Microsoft">
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

    using System;

    /// <devdoc>
    ///    <para>Specifies that this object supports
    ///       a simple,
    ///       transacted notification for batch initialization.</para>
    /// </devdoc>
    [SRDescription(SR.ISupportInitializeDescr)]
    public interface ISupportInitialize {
        /// <devdoc>
        ///    <para>
        ///       Signals
        ///       the object that initialization is starting.
        ///    </para>
        /// </devdoc>
        void BeginInit();

        /// <devdoc>
        ///    <para>
        ///       Signals the object that initialization is
        ///       complete.
        ///    </para>
        /// </devdoc>
        void EndInit();
    }
}
