//------------------------------------------------------------------------------
// <copyright file="CancelEventArgs.cs" company="Microsoft">
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
    using System;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.Security.Permissions;


    /// <devdoc>
    ///    <para>
    ///       Provides data for the <see cref='System.ComponentModel.CancelEventArgs.Cancel'/>
    ///       event.
    ///    </para>
    /// </devdoc>
    [HostProtection(SharedState = true)]
    public class CancelEventArgs : EventArgs {

        /// <devdoc>
        ///     Indicates, on return, whether or not the operation should be cancelled
        ///     or not.  'true' means cancel it, 'false' means don't.
        /// </devdoc>
        private bool cancel;
        
        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='System.ComponentModel.CancelEventArgs'/> class with
        ///       cancel set to <see langword='false'/>.
        ///    </para>
        /// </devdoc>
        public CancelEventArgs() : this(false) {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='System.ComponentModel.CancelEventArgs'/> class with
        ///       cancel set to the given value.
        ///    </para>
        /// </devdoc>
        public CancelEventArgs(bool cancel)
        : base() {
            this.cancel = cancel;
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets a value
        ///       indicating whether the operation should be cancelled.
        ///    </para>
        /// </devdoc>
        public bool Cancel {
            get {
                return cancel;
            }
            set {
                this.cancel = value;
            }
        }
    }
}
