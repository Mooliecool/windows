//------------------------------------------------------------------------------
// <copyright file="CodeVariableReferenceExpression.cs" company="Microsoft">
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
    ///       Represents a reference to a field.
    ///    </para>
    /// </devdoc>
    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeVariableReferenceExpression : CodeExpression {
        private string variableName;

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeVariableReferenceExpression'/>.
        ///    </para>
        /// </devdoc>
        public CodeVariableReferenceExpression() {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeArgumentReferenceExpression'/>.
        ///    </para>
        /// </devdoc>
        public CodeVariableReferenceExpression(string variableName) {
            this.variableName = variableName;
        }


        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string VariableName {
            get {
                return (variableName == null) ? string.Empty : variableName;
            }
            set {
                variableName = value;
            }
        }
    }
}
