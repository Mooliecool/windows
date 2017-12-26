//------------------------------------------------------------------------------
// <copyright file="CodeTryCatchFinallyStatement.cs" company="Microsoft">
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
    ///     Represents a try block, with any number of catch clauses and an
    ///     optionally finally block.
    /// </devdoc>
    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeTryCatchFinallyStatement : CodeStatement {
        private CodeStatementCollection tryStatments = new CodeStatementCollection();
        private CodeStatementCollection finallyStatments = new CodeStatementCollection();
        private CodeCatchClauseCollection catchClauses = new CodeCatchClauseCollection();

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeTryCatchFinallyStatement'/>.
        ///    </para>
        /// </devdoc>
        public CodeTryCatchFinallyStatement() {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeTryCatchFinallyStatement'/> using the specified statements to try and catch
        ///       clauses.
        ///    </para>
        /// </devdoc>
        public CodeTryCatchFinallyStatement(CodeStatement[] tryStatements, CodeCatchClause[] catchClauses) {
            TryStatements.AddRange(tryStatements);
            CatchClauses.AddRange(catchClauses);
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeTryCatchFinallyStatement'/> using the specified statements to
        ///       try, catch clauses, and finally statements.
        ///    </para>
        /// </devdoc>
        public CodeTryCatchFinallyStatement(CodeStatement[] tryStatements, CodeCatchClause[] catchClauses, CodeStatement[] finallyStatements) {
            TryStatements.AddRange(tryStatements);
            CatchClauses.AddRange(catchClauses);
            FinallyStatements.AddRange(finallyStatements);
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets
        ///       the try statements to try.
        ///    </para>
        /// </devdoc>
        public CodeStatementCollection TryStatements {
            get {
                return tryStatments;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the catch clauses to use.
        ///    </para>
        /// </devdoc>
        public CodeCatchClauseCollection CatchClauses {
            get {
                return catchClauses;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets
        ///       the finally statements to use.
        ///    </para>
        /// </devdoc>
        public CodeStatementCollection FinallyStatements {
            get {
                return finallyStatments;
            }
        }
    }
}
