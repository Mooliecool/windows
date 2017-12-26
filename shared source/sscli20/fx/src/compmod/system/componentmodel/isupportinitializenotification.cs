//------------------------------------------------------------------------------
// <copyright file="ISupportInitializeNotification.cs" company="Microsoft">
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

    /// <include file='doc\ISupportInitializeNotification.uex' path='docs/doc[@for="ISupportInitializeNotification"]/*' />
    /// <devdoc>
    ///    <para>
    ///         Extends ISupportInitialize to allow dependent components to be notified when initialization is complete.
    ///    </para>
    /// </devdoc>
    public interface ISupportInitializeNotification : ISupportInitialize {
        /// <include file='doc\ISupportInitializeNotification.uex' path='docs/doc[@for="ISupportInitializeNotification.IsInitialized"]/*' />
        /// <devdoc>
        ///    <para>
        ///         Indicates whether initialization is complete yet.
        ///    </para>
        /// </devdoc>
        bool IsInitialized { get; }

        /// <include file='doc\ISupportInitializeNotification.uex' path='docs/doc[@for="ISupportInitializeNotification.Initialized"]/*' />
        /// <devdoc>
        ///    <para>
        ///         Sent when initialization is complete.
        ///    </para>
        /// </devdoc>
        event EventHandler Initialized;
    }
}
