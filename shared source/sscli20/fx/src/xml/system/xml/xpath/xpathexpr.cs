//------------------------------------------------------------------------------
// <copyright file="XPathExpr.cs" company="Microsoft">
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

namespace System.Xml.XPath {

    using System;
    using System.Xml;
    using System.Collections;
    using MS.Internal.Xml.XPath;

    public enum XmlSortOrder {
        Ascending       = 1,
        Descending      = 2,
    }

    public enum XmlCaseOrder {
        None            = 0,
        UpperFirst      = 1,
        LowerFirst      = 2,
    }

    public enum XmlDataType {
        Text            = 1,
        Number          = 2,
    }

    public enum XPathResultType {
        Number         = 0 ,
        String          = 1,
        Boolean         = 2,
        NodeSet        = 3,
        Navigator       = XPathResultType.String,
        Any            = 5,
        Error
    };

    public abstract class XPathExpression {
        internal XPathExpression(){}

        public  abstract string Expression { get; }

        public abstract void AddSort(object expr, IComparer comparer);

        public abstract void AddSort(object expr, XmlSortOrder order, XmlCaseOrder caseOrder, string lang, XmlDataType dataType);

        public abstract XPathExpression Clone();

        public abstract void SetContext(XmlNamespaceManager nsManager);

        public abstract void SetContext(IXmlNamespaceResolver nsResolver);

        public abstract XPathResultType ReturnType { get; }
        
        public static XPathExpression Compile(string xpath) {
            return Compile(xpath, /*nsResolver:*/null);
        }

        public static XPathExpression Compile(string xpath, IXmlNamespaceResolver nsResolver) {
            bool hasPrefix;
            Query query = new QueryBuilder().Build(xpath, out hasPrefix);
            CompiledXpathExpr expr = new CompiledXpathExpr(query, xpath, hasPrefix);
            if (null != nsResolver) {
                XmlNamespaceManager mngr = XPathNavigator.GetNamespaces(nsResolver);
                expr.SetContext(mngr);
            }
            return expr;
        }

        private void PrintQuery(XmlWriter w) {
            ((CompiledXpathExpr)this).QueryTree.PrintQuery(w);
        }
    }
}
