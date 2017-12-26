//------------------------------------------------------------------------------
// <copyright file="CodeDelegateInvokeExpression.cs" company="Microsoft">
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
    ///       Represents an
    ///       expression that invokes a delegate.
    ///    </para>
    /// </devdoc>
    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeDelegateInvokeExpression : CodeExpression {
        private CodeExpression targetObject;
        private CodeExpressionCollection parameters = new CodeExpressionCollection();

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeDelegateInvokeExpression'/>.
        ///    </para>
        /// </devdoc>
        public CodeDelegateInvokeExpression() {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeDelegateInvokeExpression'/>.
        ///    </para>
        /// </devdoc>
        public CodeDelegateInvokeExpression(CodeExpression targetObject) {
            TargetObject = targetObject;
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeDelegateInvokeExpression'/>
        ///       .
        ///    </para>
        /// </devdoc>
        public CodeDelegateInvokeExpression(CodeExpression targetObject, params CodeExpression[] parameters) {
            TargetObject = targetObject;
            Parameters.AddRange(parameters);
        }

        /// <devdoc>
        ///    <para>
        ///       The
        ///       delegate's target object.
        ///    </para>
        /// </devdoc>
        public CodeExpression TargetObject {
            get {
                return targetObject;
            }
            set {
                this.targetObject = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       The
        ///       delegate parameters.
        ///    </para>
        /// </devdoc>
        public CodeExpressionCollection Parameters {
            get {
                return parameters;
            }
        }
    }
}
