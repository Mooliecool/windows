//------------------------------------------------------------------------------
// <copyright file="Operator.cs" company="Microsoft">
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
    internal class Operator : AstNode {
        public enum Op {
            LT,
            GT,
            LE,
            GE,
            EQ,
            NE,
            PLUS,
            MINUS,
            MUL,
            MOD,
            DIV,
            OR,
            AND,
            UNION,
            INVALID
        };

        private Op opType;
        private AstNode opnd1;
        private AstNode opnd2;

        public Operator(Op op, AstNode opnd1, AstNode opnd2) {
            this.opType = op;
            this.opnd1 = opnd1;
            this.opnd2 = opnd2;
        }

        public override AstType Type { get {return  AstType.Operator;} }
        public override XPathResultType ReturnType {
            get {
                if (opType < Op.LT) {
                    return XPathResultType.Number;
                }
                if (opType < Op.UNION) {
                    return XPathResultType.Boolean;
                }
                return XPathResultType.NodeSet;
            }
        }

        public Op      OperatorType { get { return opType; } }
        public AstNode Operand1     { get { return opnd1;  } }
        public AstNode Operand2     { get { return opnd2;  } }
    }
}
