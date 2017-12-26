//------------------------------------------------------------------------------
// <copyright file="ActiveDocumentEvent.cs" company="Microsoft">
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
namespace System.ComponentModel.Design {
    using System;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.Security.Permissions;
    using Microsoft.Win32;

    /// <devdoc>
    /// <para>Provides data for the <see cref='System.ComponentModel.Design.IDesignerEventService.ActiveDesigner'/>
    /// event.</para>
    /// </devdoc>
    [HostProtection(SharedState = true)]
    [System.Security.Permissions.PermissionSetAttribute(System.Security.Permissions.SecurityAction.InheritanceDemand, Name="FullTrust")]
    [System.Security.Permissions.PermissionSetAttribute(System.Security.Permissions.SecurityAction.LinkDemand, Name="FullTrust")]
    public class ActiveDesignerEventArgs : EventArgs {
        /// <devdoc>
        ///     The document that is losing activation.
        /// </devdoc>
        private readonly IDesignerHost oldDesigner;

        /// <devdoc>
        ///     The document that is gaining activation.
        /// </devdoc>
        private readonly IDesignerHost newDesigner;

        /// <devdoc>
        /// <para>Initializes a new instance of the <see cref='System.ComponentModel.Design.ActiveDesignerEventArgs'/>
        /// class.</para>
        /// </devdoc>
        public ActiveDesignerEventArgs(IDesignerHost oldDesigner, IDesignerHost newDesigner) {
            this.oldDesigner = oldDesigner;
            this.newDesigner = newDesigner;
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or
        ///       sets the document that is losing activation.
        ///    </para>
        /// </devdoc>
        public IDesignerHost OldDesigner {
            get {
                return oldDesigner;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or
        ///       sets the document that is gaining activation.
        ///    </para>
        /// </devdoc>
        public IDesignerHost NewDesigner {
            get {
                return newDesigner;
            }
        }

    }
}
