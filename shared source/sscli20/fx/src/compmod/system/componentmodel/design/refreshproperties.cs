//------------------------------------------------------------------------------
// <copyright file="RefreshProperties.cs" company="Microsoft">
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
namespace System.ComponentModel {

    /// <devdoc>
    ///    <para>
    ///       Specifies identifiers that indicate the nature of
    ///       the refresh, for use in refreshing the design time view.
    ///    </para>
    /// </devdoc>
    public enum RefreshProperties {
        /// <devdoc>
        ///    <para>Indicates to use the no refresh mode.</para>
        /// </devdoc>
        None,
        /// <devdoc>
        ///    <para>Indicates to use the refresh all refresh mode.</para>
        /// </devdoc>
        All,
        /// <devdoc>
        ///    <para>Indicates to use the repaint refresh mode.</para>
        /// </devdoc>
        Repaint,
    }
}

