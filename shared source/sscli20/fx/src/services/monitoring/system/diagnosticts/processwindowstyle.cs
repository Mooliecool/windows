//------------------------------------------------------------------------------
// <copyright file="ProcessWindowStyle.cs" company="Microsoft">
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

namespace System.Diagnostics {

    using System.Diagnostics;
    /// <devdoc>
    ///     A set of values indicating how the window should appear when starting
    ///     a process.
    /// </devdoc>
    public enum ProcessWindowStyle {
        /// <devdoc>
        ///     Show the window in a default location.
        /// </devdoc>
        Normal,

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        Hidden,
        
        /// <devdoc>
        ///     Show the window minimized.
        /// </devdoc>
        Minimized,
        
        /// <devdoc>
        ///     Show the window maximized.
        /// </devdoc>
        Maximized
    }
}
