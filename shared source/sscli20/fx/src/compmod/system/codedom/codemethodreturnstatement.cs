//------------------------------------------------------------------------------
// <copyright file="CodeMethodReturnStatement.cs" company="Microsoft">
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
    ///       Represents a return statement.
    ///    </para>
    /// </devdoc>
    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeMethodReturnStatement : CodeStatement {
        private CodeExpression expression;

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeMethodReturnStatement'/>.
        ///    </para>
        /// </devdoc>
        public CodeMethodReturnStatement() {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeMethodReturnStatement'/> using the specified expression.
        ///    </para>
        /// </devdoc>
        public CodeMethodReturnStatement(CodeExpression expression) {
            Expression = expression;
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the expression that indicates the return statement.
        ///    </para>
        /// </devdoc>
        public CodeExpression Expression {
            get {
                return expression;
            }
            set {
                expression = value;
            }
        }
    }
}
