//------------------------------------------------------------------------------
// <copyright file="TheQuery.cs" company="Microsoft">
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

namespace System.Xml.Xsl.XsltOld {
    using Res = System.Xml.Utils.Res;
    using System;
    using System.Xml;
    using System.Xml.XPath;
    using MS.Internal.Xml.XPath;

    internal sealed class TheQuery {
        internal InputScopeManager   _ScopeManager;
        private CompiledXpathExpr _CompiledQuery;

        internal CompiledXpathExpr CompiledQuery { get { return _CompiledQuery; } }
        
        internal TheQuery( CompiledXpathExpr compiledQuery, InputScopeManager manager) {
            _CompiledQuery = compiledQuery;
            _ScopeManager = manager.Clone();
        }
    }
}
