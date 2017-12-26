//------------------------------------------------------------------------------
// <copyright file="CodeSnippetTypeMember.cs" company="Microsoft">
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

namespace System.CodeDom {

    using System.Diagnostics;
    using System;
    using Microsoft.Win32;
    using System.Collections;
    using System.Reflection;
    using System.Runtime.InteropServices;

    /// <devdoc>
    ///    <para>
    ///       Represents a
    ///       snippet member of a class.
    ///    </para>
    /// </devdoc>
    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeSnippetTypeMember : CodeTypeMember {
        private string text;

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeSnippetTypeMember'/>.
        ///    </para>
        /// </devdoc>
        public CodeSnippetTypeMember() {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeSnippetTypeMember'/>.
        ///    </para>
        /// </devdoc>
        public CodeSnippetTypeMember(string text) {
            Text = text;
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the code for the class member.
        ///    </para>
        /// </devdoc>
        public string Text {
            get {
                return (text == null) ? string.Empty : text;
            }
            set {
                text = value;
            }
        }
    }
}

