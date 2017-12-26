//------------------------------------------------------------------------------
// <copyright file="XPathParser.cs" company="Microsoft">
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

using System.Collections.Generic;
using System.Diagnostics;

namespace System.Xml.Xsl.XPath {
    using Res           = System.Xml.Utils.Res;
    using XPathNodeType = System.Xml.XPath.XPathNodeType;

    internal class XPathParser<Node> {
        private XPathScanner        scanner;
        private IXPathBuilder<Node> builder;
        private Stack<int>          posInfo = new Stack<int>();


        public Node Parse(XPathScanner scanner, IXPathBuilder<Node> builder, LexKind endLex) {
            Debug.Assert(this.scanner == null && this.builder == null);
            Debug.Assert(scanner != null && builder != null);

            Node result     = default(Node);
            this.scanner    = scanner;
            this.builder    = builder;
            this.posInfo.Clear();

            try {
                builder.StartBuild();
                result = ParseExpr();
                scanner.CheckToken(endLex);
            }
            catch (XPathCompileException e) {
                if (e.queryString == null) {
                    e.queryString = scanner.Source;
                    PopPosInfo(out e.startChar, out e.endChar);
                }
                throw;
            }
            finally {
                result = builder.EndBuild(result);
#if DEBUG
                this.builder = null;
                this.scanner = null;
#endif
            }
            Debug.Assert(posInfo.Count == 0, "PushPosInfo() and PopPosInfo() calls have been unbalanced");
            return result;
        }

        #region Location paths and node tests
        /**************************************************************************************************/
        /*  Location paths and node tests                                                                 */
        /**************************************************************************************************/

        internal static bool IsStep(LexKind lexKind) {
            return (
                lexKind == LexKind.Dot    ||
                lexKind == LexKind.DotDot ||
                lexKind == LexKind.At     ||
                lexKind == LexKind.Axis   ||
                lexKind == LexKind.Star   ||
                lexKind == LexKind.Name   // NodeTest is also Name
            );
        }

        /*
        *   LocationPath ::= RelativeLocationPath | '/' RelativeLocationPath? | '//' RelativeLocationPath
        */
        private Node ParseLocationPath() {
            if (scanner.Kind == LexKind.Slash) {
                scanner.NextLex();
                Node opnd = builder.Axis(XPathAxis.Root, XPathNodeType.All, null, null);

                if (IsStep(scanner.Kind)) {
                    opnd = builder.JoinStep(opnd, ParseRelativeLocationPath());
                }
                return opnd;
            } else if (scanner.Kind == LexKind.SlashSlash) {
                scanner.NextLex();
                return builder.JoinStep(
                    builder.Axis(XPathAxis.Root, XPathNodeType.All, null, null),
                    builder.JoinStep(
                        builder.Axis(XPathAxis.DescendantOrSelf, XPathNodeType.All, null, null),
                        ParseRelativeLocationPath()
                    )
                );
            } else {
                return ParseRelativeLocationPath();
            }
        }

        /*
        *   RelativeLocationPath ::= Step (('/' | '//') Step)*
        */
        private Node ParseRelativeLocationPath() {
            Node opnd = ParseStep();
            if (scanner.Kind == LexKind.Slash) {
                scanner.NextLex();
                opnd = builder.JoinStep(opnd, ParseRelativeLocationPath());
            } else if (scanner.Kind == LexKind.SlashSlash) {
                scanner.NextLex();
                opnd = builder.JoinStep(opnd,
                    builder.JoinStep(
                        builder.Axis(XPathAxis.DescendantOrSelf, XPathNodeType.All, null, null),
                        ParseRelativeLocationPath()
                    )
                );
            }
            return opnd;
        }

        internal static XPathAxis GetAxis(string axisName, XPathScanner scanner) {
            switch (axisName) {
            case "ancestor"          : return XPathAxis.Ancestor        ;
            case "ancestor-or-self"  : return XPathAxis.AncestorOrSelf  ;
            case "attribute"         : return XPathAxis.Attribute       ;
            case "child"             : return XPathAxis.Child           ;
            case "descendant"        : return XPathAxis.Descendant      ;
            case "descendant-or-self": return XPathAxis.DescendantOrSelf;
            case "following"         : return XPathAxis.Following       ;
            case "following-sibling" : return XPathAxis.FollowingSibling;
            case "namespace"         : return XPathAxis.Namespace       ;
            case "parent"            : return XPathAxis.Parent          ;
            case "preceding"         : return XPathAxis.Preceding       ;
            case "preceding-sibling" : return XPathAxis.PrecedingSibling;
            case "self"              : return XPathAxis.Self            ;
            default :
                throw scanner.CreateException(Res.XPath_UnknownAxis, axisName);
            }
        }

        /*
        *   Step ::= '.' | '..' | (AxisName '::' | '@')? NodeTest Predicate*
        */
        private Node ParseStep() {
            Node opnd;
            if (LexKind.Dot == scanner.Kind) {                  //>> '.'
                scanner.NextLex();
                opnd = builder.Axis(XPathAxis.Self, XPathNodeType.All, null, null);
                if (LexKind.LBracket == scanner.Kind) {
                    throw scanner.CreateException(Res.XPath_PredicateAfterDot);
                }
            } else if (LexKind.DotDot == scanner.Kind) {        //>> '..'
                scanner.NextLex();
                opnd = builder.Axis(XPathAxis.Parent, XPathNodeType.All, null, null);
                if (LexKind.LBracket == scanner.Kind) {
                    throw scanner.CreateException(Res.XPath_PredicateAfterDotDot);
                }
            } else {                                           //>> (AxisName '::' | '@')? NodeTest Predicate*
                XPathAxis axis;
                switch (scanner.Kind) {
                case LexKind.Axis:                             //>> AxisName '::'
                    axis = GetAxis(scanner.Name, scanner);
                    scanner.NextLex();
                    break;
                case LexKind.At:                               //>> '@'
                    axis = XPathAxis.Attribute;
                    scanner.NextLex();
                    break;
                case LexKind.Name:
                case LexKind.Star:
                    // NodeTest must start with Name or '*'
                    axis = XPathAxis.Child;
                    break;
                default:
                    throw scanner.CreateException(Res.XPath_UnexpectedToken, scanner.RawValue);
                }

                opnd = ParseNodeTest(axis);

                while (LexKind.LBracket == scanner.Kind) {
                    opnd = builder.Predicate(opnd, ParsePredicate(), IsReverseAxis(axis));
                }
            }
            return opnd;
        }

        private static bool IsReverseAxis(XPathAxis axis) {
            return (
                axis == XPathAxis.Ancestor       || axis == XPathAxis.Preceding ||
                axis == XPathAxis.AncestorOrSelf || axis == XPathAxis.PrecedingSibling
            );
        }

        /*
        *   NodeTest ::= NameTest | ('comment' | 'text' | 'node') '(' ')' | 'processing-instruction' '('  Literal? ')'
        *   NameTest ::= '*' | NCName ':' '*' | QName
        */
        private Node ParseNodeTest(XPathAxis axis) {
            XPathNodeType nodeType;
            string        nodePrefix, nodeName;

            int startChar = scanner.LexStart;
            InternalParseNodeTest(scanner, axis, out nodeType, out nodePrefix, out nodeName);
            PushPosInfo(startChar, scanner.PrevLexEnd);
            Node result = builder.Axis(axis, nodeType, nodePrefix, nodeName);
            PopPosInfo();
            return result;
        }

        private static bool IsNodeType(XPathScanner scanner) {
            return scanner.Prefix.Length == 0 && (
                scanner.Name == "node"                   ||
                scanner.Name == "text"                   ||
                scanner.Name == "processing-instruction" ||
                scanner.Name == "comment"
            );
        }

        private static XPathNodeType PrincipalNodeType(XPathAxis axis) {
            return (
                axis == XPathAxis.Attribute ? XPathNodeType.Attribute :
                axis == XPathAxis.Namespace ? XPathNodeType.Namespace :
                /*else*/                      XPathNodeType.Element
            );
        }

        internal static void InternalParseNodeTest(XPathScanner scanner, XPathAxis axis, out XPathNodeType nodeType, out string nodePrefix, out string nodeName) {
            switch (scanner.Kind) {
            case LexKind.Name :
                if (scanner.CanBeFunction && IsNodeType(scanner)) {
                    nodePrefix = null;
                    nodeName   = null;
                    switch (scanner.Name) {
                    case "comment": nodeType = XPathNodeType.Comment; break;
                    case "text"   : nodeType = XPathNodeType.Text;    break;
                    case "node"   : nodeType = XPathNodeType.All;     break;
                    default:
                        Debug.Assert(scanner.Name == "processing-instruction");
                        nodeType = XPathNodeType.ProcessingInstruction;
                        break;
                    }

                    scanner.NextLex();
                    scanner.PassToken(LexKind.LParens);

                    if (nodeType == XPathNodeType.ProcessingInstruction) {
                        if (scanner.Kind != LexKind.RParens) {  //>> 'processing-instruction' '(' Literal ')'
                            scanner.CheckToken(LexKind.String);
                            // It is not needed to set nodePrefix here, but for our current implementation
                            // comparing whole QNames is faster than comparing just local names
                            nodePrefix = string.Empty;
                            nodeName   = scanner.StringValue;
                            scanner.NextLex();
                        }
                    }

                    scanner.PassToken(LexKind.RParens);
                } else {
                    nodePrefix = scanner.Prefix;
                    nodeName   = scanner.Name;
                    nodeType   = PrincipalNodeType(axis);
                    scanner.NextLex();
                    if (nodeName == "*") {
                        nodeName = null;
                    }
                }
                break;
            case LexKind.Star :
                nodePrefix = null;
                nodeName   = null;
                nodeType   = PrincipalNodeType(axis);
                scanner.NextLex();
                break;
            default :
                throw scanner.CreateException(Res.XPath_NodeTestExpected, scanner.RawValue);
            }
        }

        /*
        *   Predicate ::= '[' Expr ']'
        */
        private Node ParsePredicate() {
            scanner.PassToken(LexKind.LBracket);
            Node opnd = ParseExpr();
            scanner.PassToken(LexKind.RBracket);
            return opnd;
        }
        #endregion

        #region Expressions
        /**************************************************************************************************/
        /*  Expressions                                                                                   */
        /**************************************************************************************************/

        /*
        *   Expr   ::= OrExpr
        *   OrExpr ::= AndExpr ('or' AndExpr)*
        */
        private Node ParseExpr() {
            Node opnd = ParseAndExpr();

            while (scanner.IsKeyword("or")) {
                scanner.NextLex();
                opnd = builder.Operator(XPathOperator.Or, opnd, ParseAndExpr());
            }
            return opnd;
        }

        /*
        *   AndExpr ::= EqualityExpr ('and' EqualityExpr)*
        */
        private Node ParseAndExpr() {
            Node opnd = ParseEqualityExpr();

            while (scanner.IsKeyword("and")) {
                scanner.NextLex();
                opnd = builder.Operator(XPathOperator.And, opnd, ParseEqualityExpr());
            }
            return opnd;
        }

        /*
        *   EqualityExpr ::= RelationalExpr (('=' | '!=') RelationalExpr)*
        */
        private Node ParseEqualityExpr() {
            Node opnd = ParseRelationalExpr();
            bool eq;

            while ((eq = scanner.Kind == LexKind.Eq) || scanner.Kind == LexKind.Ne) {
                XPathOperator op = eq ? XPathOperator.Eq : XPathOperator.Ne;
                scanner.NextLex();
                opnd = builder.Operator(op, opnd, ParseRelationalExpr());
            }
            return opnd;
        }

        /*
        *   RelationalExpr ::= AdditiveExpr (('<' | '>' | '<=' | '>=') AdditiveExpr)*
        */
        private Node ParseRelationalExpr() {
            Node opnd = ParseAdditiveExpr();

            while (true) {
                XPathOperator op;
                switch (scanner.Kind) {
                case LexKind.Lt:  op = XPathOperator.Lt; break;
                case LexKind.Le:  op = XPathOperator.Le; break;
                case LexKind.Gt:  op = XPathOperator.Gt; break;
                case LexKind.Ge:  op = XPathOperator.Ge; break;
                default:          return opnd;
                }
                scanner.NextLex();
                opnd = builder.Operator(op, opnd, ParseAdditiveExpr());
            }
        }

        /*
        *   AdditiveExpr ::= MultiplicativeExpr (('+' | '-') MultiplicativeExpr)*
        */
        private Node ParseAdditiveExpr() {
            Node opnd = ParseMultiplicativeExpr();
            bool plus;

            while ((plus = scanner.Kind == LexKind.Plus) || scanner.Kind == LexKind.Minus) {
                XPathOperator op = plus ? XPathOperator.Plus : XPathOperator.Minus;
                scanner.NextLex();
                opnd = builder.Operator(op, opnd, ParseMultiplicativeExpr());
            }
            return opnd;
        }

        /*
        *   MultiplicativeExpr ::= UnaryExpr (('*' | 'div' | 'mod') UnaryExpr)*
        */
        private Node ParseMultiplicativeExpr() {
            Node opnd = ParseUnaryExpr();

            while (true) {
                XPathOperator op;
                if (scanner.Kind == LexKind.Star) {
                    op = XPathOperator.Multiply;
                } else if (scanner.IsKeyword("div")) {
                    op = XPathOperator.Divide;
                } else if (scanner.IsKeyword("mod")) {
                    op = XPathOperator.Modulo;
                } else {
                    return opnd;
                }
                scanner.NextLex();
                opnd = builder.Operator(op, opnd, ParseUnaryExpr());
            }
        }

        /*
        *   UnaryExpr ::= ('-')* UnionExpr
        */
        private Node ParseUnaryExpr() {
            if (scanner.Kind == LexKind.Minus) {
                scanner.NextLex();
                return builder.Operator(XPathOperator.UnaryMinus, ParseUnaryExpr(), default(Node));
            } else {
                return ParseUnionExpr();
            }
        }

        /*
        *   UnionExpr ::= PathExpr ('|' PathExpr)*
        */
        private Node ParseUnionExpr() {
            int startChar = scanner.LexStart;
            Node opnd1 = ParsePathExpr();

            if (scanner.Kind == LexKind.Union) {
                PushPosInfo(startChar, scanner.PrevLexEnd);
                opnd1 = builder.Operator(XPathOperator.Union, default(Node), opnd1);
                PopPosInfo();

                while (scanner.Kind == LexKind.Union) {
                    scanner.NextLex();
                    startChar = scanner.LexStart;
                    Node opnd2 = ParsePathExpr();
                    PushPosInfo(startChar, scanner.PrevLexEnd);
                    opnd1 = builder.Operator(XPathOperator.Union, opnd1, opnd2);
                    PopPosInfo();
                }
            }
            return opnd1;
        }

        /*
        *   PathExpr ::= LocationPath | FilterExpr (('/' | '//') RelativeLocationPath )?
        */
        private Node ParsePathExpr() {
            // Here we distinguish FilterExpr from LocationPath - the former starts with PrimaryExpr
            if (IsPrimaryExpr()) {
                int startChar = scanner.LexStart;
                Node opnd = ParseFilterExpr();
                int endChar = scanner.PrevLexEnd;

                if (scanner.Kind == LexKind.Slash) {
                    scanner.NextLex();
                    PushPosInfo(startChar, endChar);
                    opnd = builder.JoinStep(opnd, ParseRelativeLocationPath());
                    PopPosInfo();
                } else if (scanner.Kind == LexKind.SlashSlash) {
                    scanner.NextLex();
                    PushPosInfo(startChar, endChar);
                    opnd = builder.JoinStep(opnd,
                        builder.JoinStep(
                            builder.Axis(XPathAxis.DescendantOrSelf, XPathNodeType.All, null, null),
                            ParseRelativeLocationPath()
                        )
                    );
                    PopPosInfo();
                }
                return opnd;
            } else {
                return ParseLocationPath();
            }
        }

        /*
        *   FilterExpr ::= PrimaryExpr Predicate*
        */
        private Node ParseFilterExpr() {
            int startChar = scanner.LexStart;
            Node opnd = ParsePrimaryExpr();
            int endChar = scanner.PrevLexEnd;

            while (scanner.Kind == LexKind.LBracket) {
                PushPosInfo(startChar, endChar);
                opnd = builder.Predicate(opnd, ParsePredicate(), /*reverseStep:*/false);
                PopPosInfo();
            }
            return opnd;
        }

        private bool IsPrimaryExpr() {
            return (
                scanner.Kind == LexKind.String  ||
                scanner.Kind == LexKind.Number  ||
                scanner.Kind == LexKind.Dollar  ||
                scanner.Kind == LexKind.LParens ||
                scanner.Kind == LexKind.Name && scanner.CanBeFunction && !IsNodeType(scanner)
            );
        }

        /*
        *   PrimaryExpr ::= Literal | Number | VariableReference | '(' Expr ')' | FunctionCall
        */
        private Node ParsePrimaryExpr() {
            Debug.Assert(IsPrimaryExpr());
            Node opnd;
            switch (scanner.Kind) {
            case LexKind.String:
                opnd = builder.String(scanner.StringValue);
                scanner.NextLex();
                break;
            case LexKind.Number:
                opnd = builder.Number(scanner.NumberValue);
                scanner.NextLex();
                break;
            case LexKind.Dollar:
                int startChar = scanner.LexStart;
                scanner.NextLex();
                scanner.CheckToken(LexKind.Name);
                PushPosInfo(startChar, scanner.LexStart + scanner.LexSize);
                opnd = builder.Variable(scanner.Prefix, scanner.Name);
                PopPosInfo();
                scanner.NextLex();
                break;
            case LexKind.LParens:
                scanner.NextLex();
                opnd = ParseExpr();
                scanner.PassToken(LexKind.RParens);
                break;
            default:
                Debug.Assert(
                    scanner.Kind == LexKind.Name && scanner.CanBeFunction && !IsNodeType(scanner),
                    "IsPrimaryExpr() returned true, but the lexeme is not recognized"
                );
                opnd = ParseFunctionCall();
                break;
            }
            return opnd;
        }

        /*
        *   FunctionCall ::= FunctionName '(' (Expr (',' Expr)* )? ')'
        */
        private Node ParseFunctionCall() {
            List<Node> argList = new List<Node>();
            string name   = scanner.Name;
            string prefix = scanner.Prefix;
            int startChar = scanner.LexStart;

            scanner.PassToken(LexKind.Name);
            scanner.PassToken(LexKind.LParens);

            if (scanner.Kind != LexKind.RParens) {
                while (true) {
                    argList.Add(ParseExpr());
                    if (scanner.Kind != LexKind.Comma) {
                        scanner.CheckToken(LexKind.RParens);
                        break;
                    }
                    scanner.NextLex();  // move off the ','
                }
            }

            scanner.NextLex();          // move off the ')'
            PushPosInfo(startChar, scanner.PrevLexEnd);
            Node result = builder.Function(prefix, name, argList);
            PopPosInfo();
            return result;
        }
        #endregion

        /**************************************************************************************************/
        /*  Helper methods                                                                                */
        /**************************************************************************************************/

        private void PushPosInfo(int startChar, int endChar) {
            posInfo.Push(startChar);
            posInfo.Push(endChar);
        }

        private void PopPosInfo() {
            posInfo.Pop();
            posInfo.Pop();
        }

        private void PopPosInfo(out int startChar, out int endChar) {
            endChar   = posInfo.Pop();
            startChar = posInfo.Pop();
        }
    }
}
