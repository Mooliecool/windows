//------------------------------------------------------------------------------
// <copyright file="CodeCommentStatement.cs" company="Microsoft">
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
    using System.Runtime.InteropServices;

    /// <devdoc>
    ///    <para> Represents a comment.</para>
    /// </devdoc>
    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeCommentStatement : CodeStatement {
        private CodeComment comment;

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeCommentStatement'/>.
        ///    </para>
        /// </devdoc>
        public CodeCommentStatement() {
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public CodeCommentStatement(CodeComment comment) {
            this.comment = comment;
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeCommentStatement'/> with the specified text as
        ///       contents.
        ///    </para>
        /// </devdoc>
        public CodeCommentStatement(string text) {
            comment = new CodeComment(text);
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public CodeCommentStatement(string text, bool docComment) {
            comment = new CodeComment(text, docComment);
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public CodeComment Comment {
            get {
                return comment;
            }
            set {
                comment = value;
            }
        }
    }
}
