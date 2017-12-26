//------------------------------------------------------------------------------
// <copyright file="DesignerVerb.cs" company="Microsoft">
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
    using Microsoft.Win32;
    using System;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.Security.Permissions;
    using System.Text.RegularExpressions;
    
    /// <devdoc>
    ///    <para> Represents a verb that can be executed by a component's designer.</para>
    /// </devdoc>
    [HostProtection(SharedState = true)]
    [System.Runtime.InteropServices.ComVisible(true)]
    [System.Security.Permissions.PermissionSetAttribute(System.Security.Permissions.SecurityAction.InheritanceDemand, Name="FullTrust")]
    [System.Security.Permissions.PermissionSetAttribute(System.Security.Permissions.SecurityAction.LinkDemand, Name="FullTrust")]
    public class DesignerVerb : MenuCommand {

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='System.ComponentModel.Design.DesignerVerb'/> class.
        ///    </para>
        /// </devdoc>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Usage", "CA2214:DoNotCallOverridableMethodsInConstructors")]
        public DesignerVerb(string text, EventHandler handler)  : base(handler, StandardCommands.VerbFirst) {
            Properties["Text"] = text == null ? null : Regex.Replace(text, @"\(\&.\)", "");
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='System.ComponentModel.Design.DesignerVerb'/>
        ///       class.
        ///    </para>
        /// </devdoc>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Usage", "CA2214:DoNotCallOverridableMethodsInConstructors")]
        public DesignerVerb(string text, EventHandler handler, CommandID startCommandID)  : base(handler, startCommandID) {
            Properties["Text"] = text == null ? null : Regex.Replace(text, @"\(\&.\)", "");
        }

        /// <devdoc>
        /// Gets or sets the description of the menu item for the verb.
        /// </devdoc>
        public string Description {
            get {
                object result = Properties["Description"];
                if (result == null) {
                    return String.Empty;
                }
                return (string)result;
            }
            set {
                Properties["Description"] = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the text to show on the menu item for the verb.
        ///    </para>
        /// </devdoc>
        public string Text {
            get {
                object result = Properties["Text"];
                if (result == null) {
                    return String.Empty;
                }
                return (string)result;
            }
        }


        /// <devdoc>
        ///    <para>
        ///       Overrides object's ToString().
        ///    </para>
        /// </devdoc>
        public override string ToString() {
            return Text + " : " + base.ToString();
        }
    }
}

