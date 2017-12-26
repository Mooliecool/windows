//------------------------------------------------------------------------------
// <copyright file="CodeConstructor.cs" company="Microsoft">
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
    ///       Represents a class constructor.
    ///    </para>
    /// </devdoc>
    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeConstructor : CodeMemberMethod {
        private CodeExpressionCollection baseConstructorArgs = new CodeExpressionCollection();
        private CodeExpressionCollection chainedConstructorArgs = new CodeExpressionCollection();

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeConstructor'/>.
        ///    </para>
        /// </devdoc>
        public CodeConstructor() {
            Name = ".ctor";
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the base constructor arguments.
        ///    </para>
        /// </devdoc>
        public CodeExpressionCollection BaseConstructorArgs {
            get {
                return baseConstructorArgs;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the chained constructor arguments.
        ///    </para>
        /// </devdoc>
        public CodeExpressionCollection ChainedConstructorArgs {
            get {
                return chainedConstructorArgs;
            }
        }
    }
}
