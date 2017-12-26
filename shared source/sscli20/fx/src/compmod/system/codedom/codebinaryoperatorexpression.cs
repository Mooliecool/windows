//------------------------------------------------------------------------------
// <copyright file="CodeBinaryOperatorExpression.cs" company="Microsoft">
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
    ///       Represents a binary operator expression.
    ///    </para>
    /// </devdoc>
    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeBinaryOperatorExpression : CodeExpression {
        private CodeBinaryOperatorType op;
        private CodeExpression left;
        private CodeExpression right;

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeBinaryOperatorExpression'/>.
        ///    </para>
        /// </devdoc>
        public CodeBinaryOperatorExpression() {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeBinaryOperatorExpression'/>
        ///       using the specified
        ///       parameters.
        ///    </para>
        /// </devdoc>
        public CodeBinaryOperatorExpression(CodeExpression left, CodeBinaryOperatorType op, CodeExpression right) {
            Right = right;
            Operator = op;
            Left = left;
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets
        ///       the code expression on the right of the operator.
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

        /// <devdoc>
        ///    <para>
        ///       Gets or sets
        ///       the code expression on the left of the operator.
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
        ///       the operator in the binary operator expression.
        ///    </para>
        /// </devdoc>
        public CodeBinaryOperatorType Operator {
            get {
                return op;
            }
            set {
                op = value;
            }
        }
    }
}
