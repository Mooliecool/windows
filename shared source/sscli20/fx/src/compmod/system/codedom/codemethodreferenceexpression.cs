//------------------------------------------------------------------------------
// <copyright file="codemethodreferenceexpression.cs" company="Microsoft">
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
    using System.Runtime.Serialization;    

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
    public class CodeMethodReferenceExpression : CodeExpression {
        private CodeExpression targetObject;
        private string methodName;
        [OptionalField]
        private CodeTypeReferenceCollection typeArguments;

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeMethodReferenceExpression'/>.
        ///    </para>
        /// </devdoc>
        public CodeMethodReferenceExpression() {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeMethodReferenceExpression'/> using the specified
        ///       target object and method name.
        ///    </para>
        /// </devdoc>
        public CodeMethodReferenceExpression(CodeExpression targetObject, string methodName) {
            TargetObject = targetObject;
            MethodName = methodName;
        }

        public CodeMethodReferenceExpression(CodeExpression targetObject, string methodName, params CodeTypeReference[] typeParameters) {
            TargetObject = targetObject;
            MethodName = methodName;
            if( typeParameters != null && typeParameters.Length > 0) {
                TypeArguments.AddRange(typeParameters);
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the target object.
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
        ///       Gets or sets the name of the method to invoke.
        ///    </para>
        /// </devdoc>
        public string MethodName {
            get {
                return (methodName == null) ? string.Empty : methodName;
            }
            set {
                methodName = value;
            }
        }

        [System.Runtime.InteropServices.ComVisible(false)]
        public CodeTypeReferenceCollection TypeArguments{ 
            get {
                if( typeArguments == null) {
                    typeArguments = new CodeTypeReferenceCollection();
                }
                return typeArguments;
            }
        }
    }
}
