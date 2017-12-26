//------------------------------------------------------------------------------
// <copyright file="InitializationEventAttribute.cs" company="Microsoft">
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
    using System.Security.Permissions;

    /// <devdoc>
    ///    <para>Specifies which event is fired on initialization.</para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Class)]
    public sealed class InitializationEventAttribute : Attribute {

        private string eventName = null;

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='System.ComponentModel.InitializationEventAttribute'/> class.
        ///    </para>
        /// </devdoc>
        public InitializationEventAttribute(string eventName) {
            this.eventName = eventName;
        }

        
        /// <devdoc>
        ///    <para>
        ///       Gets the name of the initialization event.
        ///    </para>
        /// </devdoc>
        public string EventName {
            get {
                return this.eventName;
            }
        }
    }
}
