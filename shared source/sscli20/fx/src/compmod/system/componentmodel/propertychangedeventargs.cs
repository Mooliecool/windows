//------------------------------------------------------------------------------
// <copyright file="PropertyChangedEventArgs.cs" company="Microsoft">
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
    using System.Diagnostics;
    using System.Security.Permissions;

    /// <devdoc>
    /// <para>Provides data for the <see langword='PropertyChanged'/>
    /// event.</para>
    /// </devdoc>
    [HostProtection(SharedState = true)]
    public class PropertyChangedEventArgs : EventArgs {
        private readonly string propertyName;

        /// <devdoc>
        /// <para>Initializes a new instance of the <see cref='System.ComponentModel.PropertyChangedEventArgs'/>
        /// class.</para>
        /// </devdoc>
        public PropertyChangedEventArgs(string propertyName) {
            this.propertyName = propertyName;
        }

        /// <devdoc>
        ///    <para>Indicates the name of the property that changed.</para>
        /// </devdoc>
        public virtual string PropertyName {
            get {
                return propertyName;
            }
        }
    }
}
