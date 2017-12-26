//------------------------------------------------------------------------------
// <copyright file="CompiledXpathExpr.cs" company="Microsoft">
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

namespace MS.Internal.Xml.XPath {
    using System;
    using System.Xml;
    using System.Xml.XPath;
    using System.Diagnostics;
    using System.Globalization;
    using System.Collections;
    using System.Xml.Xsl;

    internal class CompiledXpathExpr : XPathExpression {
        Query query;
        string expr;
        bool needContext;
        
        internal CompiledXpathExpr(Query query, string expression, bool needContext) {
            this.query = query;
            this.expr = expression;
            this.needContext = needContext;
        }

        internal Query QueryTree {
            get { 
                if (needContext) {
                    throw XPathException.Create(Res.Xp_NoContext);
                }
                return query; 
            }
        }
        
        public override string Expression {
            get { return expr; }
        }

        public virtual void CheckErrors() {
            Debug.Assert(query != null, "In case of error in XPath we create ErrorXPathExpression");
        }

        public override void AddSort(object expr, IComparer comparer) {
            // sort makes sense only when we are dealing with a query that
            // returns a nodeset.
	        Query evalExpr;
            if (expr is string) {
                evalExpr = new QueryBuilder().Build((string)expr, out needContext); // this will throw if expr is invalid
            } else if (expr is CompiledXpathExpr) {
                evalExpr = ((CompiledXpathExpr)expr).QueryTree;
	        } else {
                throw XPathException.Create(Res.Xp_BadQueryObject);
            }
            SortQuery sortQuery = query as SortQuery;
            if (sortQuery == null) {
                query = sortQuery = new SortQuery(query);
            }
            sortQuery.AddSort(evalExpr, comparer);
        }
        
        public override void AddSort(object expr, XmlSortOrder order, XmlCaseOrder caseOrder, string lang, XmlDataType dataType) {
            AddSort(expr, new XPathComparerHelper(order, caseOrder, lang, dataType));
        }
        
        public override XPathExpression Clone() {
            return new CompiledXpathExpr(Query.Clone(query), expr, needContext);
        }
        
        public override void SetContext(XmlNamespaceManager nsManager) {
            XsltContext xsltContext = nsManager as XsltContext;
            if(xsltContext == null) {
                if(nsManager == null) {
                    nsManager = new XmlNamespaceManager(new NameTable());
                }
                xsltContext = new UndefinedXsltContext(nsManager);
            }
            query.SetXsltContext(xsltContext);

            needContext = false;
        }

        public override void SetContext(IXmlNamespaceResolver nsResolver) {
            XmlNamespaceManager nsManager = nsResolver as XmlNamespaceManager;
            if (nsManager == null) {
                if (nsResolver != null) {
                    nsManager = new XmlNamespaceManager(new NameTable());
                }
            }
            SetContext(nsManager);
        }

        public override XPathResultType ReturnType { get { return query.StaticType; } }

        private class UndefinedXsltContext : XsltContext {
            private XmlNamespaceManager nsManager;

            public UndefinedXsltContext(XmlNamespaceManager nsManager) : base(/*dummy*/false) {
                this.nsManager = nsManager;
            }
            //----- Namespace support -----
            public override string DefaultNamespace {
                get { return string.Empty; }
            }
            public override string LookupNamespace(string prefix) {
                Debug.Assert(prefix != null);
                if(prefix.Length == 0) {
                    return string.Empty;
                }
                string ns = this.nsManager.LookupNamespace(prefix);
                if(ns == null) {
                    throw XPathException.Create(Res.XmlUndefinedAlias, prefix);
                }
                Debug.Assert(ns.Length != 0, "No XPath prefix can be mapped to 'null namespace'");
                return ns;
            }
            //----- XsltContext support -----
            public override IXsltContextVariable ResolveVariable(string prefix, string name) {
                throw XPathException.Create(Res.Xp_UndefinedXsltContext);
            }
            public override IXsltContextFunction ResolveFunction(string prefix, string name, XPathResultType[] ArgTypes) {
                throw XPathException.Create(Res.Xp_UndefinedXsltContext);
            }
            public override bool Whitespace { get{ return false; } }
            public override bool PreserveWhitespace(XPathNavigator node) { return false; }
            public override int CompareDocument (string baseUri, string nextbaseUri) {
                return string.CompareOrdinal(baseUri, nextbaseUri);
            }
        }
    }

    internal sealed class XPathComparerHelper : IComparer {
        private XmlSortOrder order;
        private XmlCaseOrder caseOrder;
        private CultureInfo  cinfo;
        private XmlDataType  dataType;
        
        public XPathComparerHelper(XmlSortOrder order, XmlCaseOrder caseOrder, string lang, XmlDataType dataType) {
            if (lang == null) {
                this.cinfo = System.Threading.Thread.CurrentThread.CurrentCulture;
            } else {
                try {
                    this.cinfo = new CultureInfo(lang);
                }
                catch (System.ArgumentException) {
                    throw;
				}
            }

            if (order == XmlSortOrder.Descending) {
                if (caseOrder == XmlCaseOrder.LowerFirst) {
                    caseOrder = XmlCaseOrder.UpperFirst;
                }
                else if (caseOrder == XmlCaseOrder.UpperFirst) {
                    caseOrder = XmlCaseOrder.LowerFirst;
                }
            }

            this.order     = order;
            this.caseOrder = caseOrder;
            this.dataType  = dataType;
        }

        public Int32 Compare(object x, object y) {
            Int32 sortOrder = (this.order == XmlSortOrder.Ascending) ? 1 : -1;
            switch(this.dataType) {
            case XmlDataType.Text:
                string s1 = Convert.ToString(x, this.cinfo);
                string s2 = Convert.ToString(y, this.cinfo);
                Int32 result = string.Compare(s1, s2, (this.caseOrder == XmlCaseOrder.None) ? false : true, this.cinfo);
                if (result != 0 || this.caseOrder == XmlCaseOrder.None)
                    return (sortOrder * result);

                // If we came this far, it means that strings s1 and s2 are
                // equal to each other when case is ignored. Now it's time to check
                // and see if they differ in case only and take into account the user
                // requested case order for sorting purposes.
                Int32 caseOrder = (this.caseOrder == XmlCaseOrder.LowerFirst) ? 1 : -1;
                result = string.Compare(s1, s2, false, this.cinfo);
                return (caseOrder * result);

            case XmlDataType.Number:
                double r1 = XmlConvert.ToXPathDouble(x);
                double r2 = XmlConvert.ToXPathDouble(y);

                // trying to return the result of (r1 - r2) casted down
                // to an Int32 can be dangerous. E.g 100.01 - 100.00 would result
                // erroneously in zero when casted down to Int32.
                if (r1 > r2) {
                    return (1*sortOrder);
                }
                else if (r1 < r2) {
                    return (-1*sortOrder);
                }
                else {
                    if (r1 == r2) {
                        return 0;
                    }
                    if (Double.IsNaN(r1)) {
                        if (Double.IsNaN(r2)) {
                            return 0;
                        }
                        //r2 is not NaN .NaN comes before any other number
                        return (-1*sortOrder);
                    }
                    //r2 is NaN. So it should come after r1
                    return (1*sortOrder);
                }
            default:
                // dataType doesn't support any other value
                throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
            } // switch
        } // Compare ()
    } // class XPathComparerHelper
}
