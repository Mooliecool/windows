//------------------------------------------------------------------------------
// <copyright file="RefreshEventArgs.cs" company="Microsoft">
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

    using System;
    using System.Diagnostics;
    using System.Security.Permissions;

    /// <devdoc>
    ///    <para>
    ///       Provides data for the <see cref='System.ComponentModel.TypeDescriptor.Refresh'/> event.
    ///    </para>
    /// </devdoc>
    [HostProtection(SharedState = true)]
    public class RefreshEventArgs : EventArgs {

        private object componentChanged;
        private Type   typeChanged;

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='System.ComponentModel.RefreshEventArgs'/> class with
        ///       the component that has
        ///       changed.
        ///    </para>
        /// </devdoc>
        public RefreshEventArgs(object componentChanged) {
            this.componentChanged = componentChanged;
            this.typeChanged = componentChanged.GetType();
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='System.ComponentModel.RefreshEventArgs'/> class with
        ///       the type
        ///       of component that has changed.
        ///    </para>
        /// </devdoc>
        public RefreshEventArgs(Type typeChanged) {
            this.typeChanged = typeChanged;
        }

        /// <devdoc>
        ///    <para>
        ///       Gets the component that has changed
        ///       its properties, events, or
        ///       extenders.
        ///    </para>
        /// </devdoc>
        public object ComponentChanged {
            get {
                return componentChanged;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets the type that has changed its properties, or events.
        ///    </para>
        /// </devdoc>
        public Type TypeChanged {
            get {
                return typeChanged;
            }
        }
    }
}

