//------------------------------------------------------------------------------
// <copyright file="CodeAssignStatement.cs" company="Microsoft">
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
    ///       Represents a simple assignment statement.
    ///    </para>
    /// </devdoc>
    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeAssignStatement : CodeStatement {
        private CodeExpression left;
        private CodeExpression right;

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeAssignStatement'/>.
        ///    </para>
        /// </devdoc>
        public CodeAssignStatement() {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeAssignStatement'/> that represents the
        ///       specified assignment values.
        ///    </para>
        /// </devdoc>
        public CodeAssignStatement(CodeExpression left, CodeExpression right) {
            Left = left;
            Right = right;
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets
        ///       the variable to be assigned to.
        ///    </para>
        /// </devdoc>
        public CodeExpression Left {
            get {
                return left;
            }
            set {
                left = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets
        ///       the value to assign.
        ///    </para>
        /// </devdoc>
        public CodeExpression Right {
            get {
                return right;
            }
            set {
                right = value;
            }
        }
    }
}
