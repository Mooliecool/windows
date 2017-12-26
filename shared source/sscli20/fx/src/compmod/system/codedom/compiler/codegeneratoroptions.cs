//------------------------------------------------------------------------------
// <copyright file="CodeGeneratorOptions.cs" company="Microsoft">
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

namespace System.CodeDom.Compiler {
    using System;
    using System.CodeDom;
    using System.Collections;
    using System.Collections.Specialized;
    using System.Security.Permissions;


    /// <devdoc>
    ///    <para>
    ///       Represents options used in code generation
    ///    </para>
    /// </devdoc>
    [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
    [PermissionSet(SecurityAction.InheritanceDemand, Name="FullTrust")]
    public class CodeGeneratorOptions {
        private IDictionary options = new ListDictionary();

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public CodeGeneratorOptions() {
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public object this[string index] {
            get {
                return options[index];
            }
            set {
                options[index] = value;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string IndentString {
            get {
                object o = options["IndentString"];
                return ((o == null) ? "    " : (string)o);
            }
            set {
                options["IndentString"] = value;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string BracingStyle {
            get {
                object o = options["BracingStyle"];
                return ((o == null) ? "Block" : (string)o);
            }
            set {
                options["BracingStyle"] = value;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool ElseOnClosing {
            get {
                object o = options["ElseOnClosing"];
                return ((o == null) ? false : (bool)o);
            }
            set {
                options["ElseOnClosing"] = value;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool BlankLinesBetweenMembers {
            get {
                object o = options["BlankLinesBetweenMembers"];
                return ((o == null) ? true : (bool)o);
            }
            set {
                options["BlankLinesBetweenMembers"] = value;
            }
        }

        [System.Runtime.InteropServices.ComVisible(false)]
        public bool VerbatimOrder {
            get {
                object o = options["VerbatimOrder"];
                return ((o == null) ? false : (bool)o);
            }
            set {
                options["VerbatimOrder"] = value;
            }
        }
    }
}
