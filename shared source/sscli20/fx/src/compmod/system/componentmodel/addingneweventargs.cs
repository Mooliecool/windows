//------------------------------------------------------------------------------
// <copyright file="AddingNewEventArgs.cs" company="Microsoft">
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
    using System.Security.Permissions;

    /// <devdoc>
    ///     Provides data for an event that signals the adding of a new object
    ///     to a list, allowing any event handler to supply the new object. If
    ///     no event handler supplies a new object to use, the list should create
    ///     one itself.
    /// </devdoc>
    [HostProtection(SharedState = true)]
    public class AddingNewEventArgs : EventArgs
    {
        private object newObject = null;

        /// <devdoc>
        ///     Initializes a new instance of the <see cref='System.ComponentModel.AddingNewEventArgs'/> class,
        ///     with no new object defined.
        /// </devdoc>
        public AddingNewEventArgs() : base() {
        }

        /// <devdoc>
        ///     Initializes a new instance of the <see cref='System.ComponentModel.AddingNewEventArgs'/> class,
        ///     with the specified object defined as the default new object.
        /// </devdoc>
        public AddingNewEventArgs(object newObject) : base() {
            this.newObject = newObject;
        }

        /// <devdoc>
        ///     Gets or sets the new object that will be added to the list.
        /// </devdoc>
        public object NewObject {
            get {
                return newObject;
            }

            set {
                newObject = value;
            }
        }
    }
}
