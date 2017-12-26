//------------------------------------------------------------------------------
// <copyright file="BooleanFunctions.cs" company="Microsoft">
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
    using System.Xml.Xsl;
    using FT = MS.Internal.Xml.XPath.Function.FunctionType;
        
    internal sealed class BooleanFunctions : ValueQuery {
        Query arg;
        FT    funcType;

        public BooleanFunctions(FT funcType, Query arg) {
            this.arg      = arg;
            this.funcType = funcType;
        }
        private BooleanFunctions(BooleanFunctions other) : base(other) {
            this.arg      = Clone(other.arg);
            this.funcType = other.funcType;
        }

        public override void SetXsltContext(XsltContext context) {
            if (arg != null) {
                arg.SetXsltContext(context);
            }
        }

        public override object Evaluate(XPathNodeIterator nodeIterator) {
            switch (funcType) {
            case FT.FuncBoolean : return toBoolean(nodeIterator);
            case FT.FuncNot     : return Not(nodeIterator);
            case FT.FuncTrue    : return true;
            case FT.FuncFalse   : return false;
            case FT.FuncLang    : return Lang(nodeIterator);
            }
            return false;
        }

        internal static bool toBoolean(double number) {
            return number != 0 && ! double.IsNaN(number);
        }
        internal static bool toBoolean(string str) {
            return str.Length > 0;
        }

        internal bool toBoolean(XPathNodeIterator nodeIterator) {
            object result = arg.Evaluate(nodeIterator);
            if (result is XPathNodeIterator) return arg.Advance() != null;
            if (result is string           ) return toBoolean((string)result);
            if (result is double           ) return toBoolean((double)result);
            if (result is bool             ) return (bool)result;
            Debug.Assert(result is XPathNavigator, "Unknown value type");
            return true;
        }

        public override XPathResultType StaticType { get { return XPathResultType.Boolean; } }

        private bool Not(XPathNodeIterator nodeIterator) {
            return ! (bool) arg.Evaluate(nodeIterator);
        }  

        private bool Lang(XPathNodeIterator nodeIterator) {
            string str  = arg.Evaluate(nodeIterator).ToString();
            string lang = nodeIterator.Current.XmlLang;
            return (
               lang.StartsWith(str, StringComparison.OrdinalIgnoreCase) &&
               (lang.Length == str.Length || lang[str.Length] == '-')
            );
        }

        public override XPathNodeIterator Clone() { return new BooleanFunctions(this); }

        public override void PrintQuery(XmlWriter w) {
            w.WriteStartElement(this.GetType().Name);
            w.WriteAttributeString("name", funcType.ToString());
            if (arg != null) {
                arg.PrintQuery(w);
            }
            w.WriteEndElement();
        }
    }
}
