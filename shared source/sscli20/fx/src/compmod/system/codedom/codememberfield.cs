//------------------------------------------------------------------------------
// <copyright file="CodeMemberField.cs" company="Microsoft">
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
    ///    <para>
    ///       Represents a class field member.
    ///    </para>
    /// </devdoc>
    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeMemberField : CodeTypeMember {
        private CodeTypeReference type;
        private CodeExpression initExpression;

        /// <devdoc>
        ///    <para>
        ///       Initializes a new <see cref='System.CodeDom.CodeMemberField'/>.
        ///    </para>
        /// </devdoc>
        public CodeMemberField() {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new <see cref='System.CodeDom.CodeMemberField'/> with the specified member field type and
        ///       name.
        ///    </para>
        /// </devdoc>
        public CodeMemberField(CodeTypeReference type, string name) {
            Type = type;
            Name = name;
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public CodeMemberField(string type, string name) {
            Type = new CodeTypeReference(type);
            Name = name;
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public CodeMemberField(Type type, string name) {
            Type = new CodeTypeReference(type);
            Name = name;
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the member field type.
        ///    </para>
        /// </devdoc>
        public CodeTypeReference Type {
            get {
                if (type == null) {
                    type = new CodeTypeReference("");
                }
                return type;
            }
            set {
                type = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the initialization expression for the member field.
        ///    </para>
        /// </devdoc>
        public CodeExpression InitExpression {
            get {
                return initExpression;
            }
            set {
                initExpression = value;
            }
        }
    }
}
