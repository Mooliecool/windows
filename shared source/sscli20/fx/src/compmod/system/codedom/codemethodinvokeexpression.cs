//------------------------------------------------------------------------------
// <copyright file="CodeMethodInvokeExpression.cs" company="Microsoft">
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
    ///       expression to invoke a method, to be called on a given target.
    ///    </para>
    /// </devdoc>
    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeMethodInvokeExpression : CodeExpression {
        private CodeMethodReferenceExpression method;
        private CodeExpressionCollection parameters = new CodeExpressionCollection();

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeMethodInvokeExpression'/>.
        ///    </para>
        /// </devdoc>
        public CodeMethodInvokeExpression() {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeMethodInvokeExpression'/> using the specified target object, method name
        ///       and parameters.
        ///    </para>
        /// </devdoc>
        public CodeMethodInvokeExpression(CodeMethodReferenceExpression method, params CodeExpression[] parameters) {
            this.method = method;
            Parameters.AddRange(parameters);
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public CodeMethodInvokeExpression(CodeExpression targetObject, string methodName, params CodeExpression[] parameters) {
            this.method = new CodeMethodReferenceExpression(targetObject, methodName);
            Parameters.AddRange(parameters);
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the name of the method to invoke.
        ///    </para>
        /// </devdoc>
        public CodeMethodReferenceExpression Method {
            get {
                if (method == null) {
                    method = new CodeMethodReferenceExpression();
                }
                return method;
            }
            set {
                method = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets
        ///       the parameters to invoke the method with.
        ///    </para>
        /// </devdoc>
        public CodeExpressionCollection Parameters {
            get {
                return parameters;
            }
        }
    }
}
