//------------------------------------------------------------------------------
// <copyright file="XPathPatternBuilder.cs" company="Microsoft">
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

using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.Xml.XPath;
using System.Xml.Schema;
using System.Xml.Xsl.Qil;
using System.Xml.Xsl.XPath;

namespace System.Xml.Xsl.Xslt {
    using T = XmlQueryTypeFactory;

    internal class XPathPatternBuilder : XPathPatternParser.IPatternBuilder {
        private XPathPredicateEnvironment predicateEnvironment;
        private XPathBuilder              predicateBuilder;
        private bool                      inTheBuild;
        private XPathQilFactory           f;
        private QilNode                   fixupNode;
        private IXPathEnvironment         environment;

        public XPathPatternBuilder(IXPathEnvironment environment) {
            Debug.Assert(environment != null);
            this.environment = environment;
            this.f           = environment.Factory;
            this.predicateEnvironment = new XPathPredicateEnvironment(environment);
            this.predicateBuilder     = new XPathBuilder(predicateEnvironment);

            this.fixupNode = f.Unknown(T.NodeNotRtfS);
        }

        public QilNode FixupNode {
            get { return fixupNode; }
        }

        public virtual void StartBuild() {
            Debug.Assert(! inTheBuild, "XPathBuilder is buisy!");
            inTheBuild = true;
            return;
        }

        [Conditional("DEBUG")]
        public void AssertFilter(QilLoop filter) {
            Debug.Assert(filter.NodeType == QilNodeType.Filter, "XPathPatternBuilder expected to generate list of Filters on top level");
            Debug.Assert(filter.Variable.XmlType.IsSubtypeOf(T.NodeNotRtf));
            Debug.Assert(filter.Variable.Binding.NodeType == QilNodeType.Unknown);  // fixupNode
            Debug.Assert(filter.Body.XmlType.IsSubtypeOf(T.Boolean));
        }

        private void FixupFilterBinding(QilLoop filter, QilNode newBinding) {
            AssertFilter(filter);
            filter.Variable.Binding = newBinding;
        }

        public virtual QilNode EndBuild(QilNode result) {
            Debug.Assert(inTheBuild, "StartBuild() wasn't called");
            if (result == null) {
                // Special door to clean builder state in exception handlers
            }
            inTheBuild = false;
            return result;
        }

        public QilNode Operator(XPathOperator op, QilNode left, QilNode right) {
            Debug.Assert(op == XPathOperator.Union);
            Debug.Assert(left  != null);
            Debug.Assert(right != null);
            // It is important to not create nested lists here
            Debug.Assert(right.NodeType == QilNodeType.Filter, "LocationPathPattern must be compiled into a filter");
            if (left.NodeType == QilNodeType.Sequence) {
                ((QilList)left).Add(right);
                return left;
            } else {
                Debug.Assert(left.NodeType == QilNodeType.Filter, "LocationPathPattern must be compiled into a filter");
                return f.Sequence(left, right);
            }
        }

        private static QilLoop BuildAxisFilter(QilPatternFactory f, QilIterator itr, XPathAxis xpathAxis, XPathNodeType nodeType, string name, string nsUri) {
            QilNode nameTest = (
                name  != null && nsUri != null ? f.Eq(f.NameOf(itr), f.QName(name, nsUri))    : // ns:bar || bar
                nsUri != null                  ? f.Eq(f.NamespaceUriOf(itr), f.String(nsUri)) : // ns:*
                name  != null                  ? f.Eq(f.LocalNameOf(itr), f.String(name))     : // *:foo
                /*name  == nsUri == null*/       f.True()                                       // *
            );

            XmlNodeKindFlags intersection = XPathBuilder.AxisTypeMask(itr.XmlType.NodeKinds, nodeType, xpathAxis);

            QilNode typeTest = (
                intersection == 0                     ? f.False() :  // input & required doesn't intersect
                intersection == itr.XmlType.NodeKinds ? f.True()  :  // input is subset of required
                /*else*/                                f.IsType(itr, T.NodeChoice(intersection))
            );

            QilLoop filter = f.BaseFactory.Filter(itr, f.And(typeTest, nameTest));
            filter.XmlType = T.PrimeProduct(T.NodeChoice(intersection), filter.XmlType.Cardinality);

            return filter;
        }

        public QilNode Axis(XPathAxis xpathAxis, XPathNodeType nodeType, string prefix, string name) {
            Debug.Assert(
                xpathAxis == XPathAxis.Child ||
                xpathAxis == XPathAxis.Attribute ||
                xpathAxis == XPathAxis.DescendantOrSelf ||
                xpathAxis == XPathAxis.Root
            );
            QilLoop result;
            double  priority;
            switch (xpathAxis) {
            case XPathAxis.DescendantOrSelf :
                Debug.Assert(nodeType == XPathNodeType.All && prefix == null && name == null, " // is the only d-o-s axes that we can have in pattern");
                return f.Nop(this.fixupNode); // We using Nop as a flag that DescendantOrSelf exis was used between steps.
            case XPathAxis.Root :
                QilIterator i;
                result = f.BaseFactory.Filter(i = f.For(fixupNode), f.IsType(i, T.Document));
                priority = 0.5;
                break;
            default :
                string   nsUri = prefix == null ? null : this.environment.ResolvePrefix(prefix);
                result = BuildAxisFilter(f, f.For(fixupNode), xpathAxis, nodeType, name, nsUri);
                switch (nodeType) {
                case XPathNodeType.Element :
                case XPathNodeType.Attribute :
                    if (name != null) {
                        priority = 0;
                    } else {
                        if (prefix != null) {
                            priority = -0.25;
                        } else {
                            priority = -0.5;
                        }
                    }
                    break;
                case XPathNodeType.ProcessingInstruction :
                    priority = name != null ? 0 : -0.5;
                    break;
                default:
                    priority = -0.5;
                    break;
                }
                break;
            }

            SetPriority(result, priority);
            SetLastParent(result, result);
            return result;
        }

        // a/b/c -> self::c[parent::b[parent::a]]
        // a/b//c -> self::c[ancestor::b[parent::a]]
        // a/b -> self::b[parent::a]
        //  -> JoinStep(Axis('a'), Axis('b'))
        //   -> Filter('b' & Parent(Filter('a')))
        // a//b
        //  -> JoinStep(Axis('a'), JoingStep(Axis(DescendantOrSelf), Axis('b')))
        //   -> JoinStep(Filter('a'), JoingStep(Nop(null), Filter('b')))
        //    -> JoinStep(Filter('a'), Nop(Filter('b')))
        //     -> Filter('b' & Ancestor(Filter('a')))
        public QilNode JoinStep(QilNode left, QilNode right) {
            Debug.Assert(left  != null);
            Debug.Assert(right != null);
            if (left.NodeType == QilNodeType.Nop) {
                QilUnary nop = (QilUnary)left;
                Debug.Assert(nop.Child == this.fixupNode);
                nop.Child = right;  // We use Nop as a flag that DescendantOrSelf axis was used between steps.
                return nop;
            }
            Debug.Assert(GetLastParent(left) == left, "Left is always single axis and never the step");
            Debug.Assert(left.NodeType == QilNodeType.Filter);
            CleanAnnotation(left);
            QilLoop parentFilter = (QilLoop) left;
            bool ancestor = false; {
                if (right.NodeType == QilNodeType.Nop) {
                    ancestor = true;
                    QilUnary nop = (QilUnary)right;
                    Debug.Assert(nop.Child != null);
                    right = nop.Child;
                }
            }
            Debug.Assert(right.NodeType == QilNodeType.Filter);
            QilLoop lastParent = GetLastParent(right);
            FixupFilterBinding(parentFilter, ancestor ? f.Ancestor(lastParent.Variable) : f.Parent(lastParent.Variable));
            lastParent.Body = f.And(lastParent.Body, f.Not(f.IsEmpty(parentFilter)));
            SetPriority(right, 0.5);
            SetLastParent(right, parentFilter);
            return right;
        }

        public QilNode Predicate(QilNode node, QilNode condition, bool isReverseStep) {
            Debug.Assert(isReverseStep == false, "patterns can't have reverse axe");
            QilLoop nodeFilter = (QilLoop) node;
            if (condition.XmlType.TypeCode == XmlTypeCode.Double) {
                predicateEnvironment.SetContext(nodeFilter);
                condition = f.Eq(condition, predicateEnvironment.GetPosition());
            } else {
                condition = f.ConvertToBoolean(condition);
            }

            nodeFilter.Body = f.And(nodeFilter.Body, condition);
            SetPriority(node, 0.5);
            return node;
        }

        public QilNode Function(string prefix, string name, IList<QilNode> args) {
            Debug.Assert(prefix.Length == 0);
            QilIterator i = f.For(fixupNode);
            QilNode     matches;

            if (name == "id") {
                Debug.Assert(
                    args.Count == 1 && args[0].NodeType == QilNodeType.LiteralString,
                    "Function id() must have one literal string argument"
                );
                matches = f.Id(i, args[0]);
            } else {
                Debug.Assert(name == "key", "Unexpected function");
                Debug.Assert(
                    args.Count == 2 &&
                    args[0].NodeType == QilNodeType.LiteralString && args[1].NodeType == QilNodeType.LiteralString,
                    "Function key() must have two literal string arguments"
                );
                matches = environment.ResolveFunction(prefix, name, args, new XsltFunctionFocus(i));
            }

            QilIterator j;
            QilLoop result = f.BaseFactory.Filter(i, f.Not(f.IsEmpty(f.Filter(j = f.For(matches), f.Is(j, i)))));
            SetPriority(result, 0.5);
            SetLastParent(result, result);
            return result;
        }

        public QilNode String(string value)                     { return f.String(value); }     // As argument of id() or key() function
        public QilNode Number(double value)                     { return UnexpectedToken("Literal number"); }
        public QilNode Variable(string prefix, string name)     { return UnexpectedToken("Variable"); }

        private QilNode UnexpectedToken(string tokenName) {
            string prompt = string.Format(CultureInfo.InvariantCulture, "Internal Error: {0} is not allowed in XSLT pattern outside of predicate.", tokenName);
            Debug.Assert(false, prompt);
            throw new Exception(prompt);
        }

        // -------------------------------------- Priority / Parent ---------------------------------------

        private class Annotation {
            public double   Priority;
            public QilLoop  Parent;
        }

        public static void SetPriority(QilNode node, double priority) {
            Annotation ann = (Annotation)node.Annotation ?? new Annotation();
            ann.Priority = priority;
            node.Annotation = ann;
        }

        public static double GetPriority(QilNode node) {
            return ((Annotation)node.Annotation).Priority;
        }

        private static void SetLastParent(QilNode node, QilLoop parent) {
            Debug.Assert(parent.NodeType == QilNodeType.Filter);
            Annotation ann = (Annotation)node.Annotation ?? new Annotation();
            ann.Parent = parent;
            node.Annotation = ann;
        }

        private static QilLoop GetLastParent(QilNode node) {
            return ((Annotation)node.Annotation).Parent;
        }

        public static void CleanAnnotation(QilNode node) {
            node.Annotation = null;
        }

        // -------------------------------------- GetPredicateBuilder() ---------------------------------------

        public IXPathBuilder<QilNode> GetPredicateBuilder(QilNode ctx) {
            QilLoop context = (QilLoop) ctx;
            Debug.Assert(context != null, "Predicate always has step so it can't have context == null");
            Debug.Assert(context.Variable.NodeType == QilNodeType.For, "It shouldn't be Let, becaus predicates in PatternBuilder don't produce cached tuples.");
            predicateEnvironment.SetContext(context);
            return predicateBuilder;
        }

        private class XPathPredicateEnvironment : IXPathEnvironment {
            private IXPathEnvironment   baseEnvironment;
            private QilLoop             baseContext;
            private XPathQilFactory     f;
            private Cloner              cloner;

            public XPathPredicateEnvironment(IXPathEnvironment baseEnvironment) {
                this.baseEnvironment = baseEnvironment;
                this.f = baseEnvironment.Factory;
                cloner = new Cloner(f.BaseFactory);
            }

            public void SetContext(QilLoop filter) {
                this.baseContext = filter;
            }

            /*  ----------------------------------------------------------------------------
                IXPathEnvironment interface
            */
            public XPathQilFactory Factory         { get { return f; } }

            public QilNode ResolveVariable(string prefix, string name)  {
                return baseEnvironment.ResolveVariable(prefix, name);
            }
            public QilNode ResolveFunction(string prefix, string name, IList<QilNode> args, IFocus env) {
                return baseEnvironment.ResolveFunction(prefix, name, args, env);
            }
            public string  ResolvePrefix(string prefix) {
                return baseEnvironment.ResolvePrefix(prefix);
            }

            public QilNode GetCurrent() {
                return baseContext.Variable;
            }
            public QilNode GetPosition() {
                QilLoop clone = (QilLoop) cloner.Clone(baseContext);
                XmlNodeKindFlags nodeKinds = baseContext.XmlType.NodeKinds;
                // baseContext either always returns attributes (attribute::), or never returns attributes or namespaces (child::)
                if (nodeKinds == XmlNodeKindFlags.Attribute) {
                    QilIterator i = f.For(f.Parent(GetCurrent()));
                    clone.Variable.Binding = f.Content(i);
                    clone.Body = f.And(clone.Body, f.Before(clone.Variable, GetCurrent()));
                    clone = f.BaseFactory.Loop(i, clone);
                } else {
                    Debug.Assert((nodeKinds & (XmlNodeKindFlags.Attribute | XmlNodeKindFlags.Namespace)) == XmlNodeKindFlags.None);
                    clone.Variable.Binding = f.PrecedingSibling(GetCurrent());
                }
                return f.Add(f.Double(1), f.XsltConvert(f.Length(clone), T.DoubleX));
            }
            public QilNode GetLast() {
                QilLoop clone  = (QilLoop) cloner.Clone(baseContext);
                QilIterator i = f.For(f.Parent(GetCurrent()));
                clone.Variable.Binding = f.Content(i);
                return f.XsltConvert(f.Length(f.Loop(i, clone)), T.DoubleX);
            }

            internal class Cloner : QilCloneVisitor {
                public Cloner(QilFactory f) : base(f) {}
                // Expressions we are cloning have fixup node of type QilNodeType.Unknown
                // This node will be replaced anyway so we leave it as it is
                protected override QilNode VisitUnknown(QilNode n) {
                    return n;
                }
            }
        }

        private class XsltFunctionFocus : IFocus {
            private QilIterator current;

            public XsltFunctionFocus(QilIterator current) {
                Debug.Assert(current != null);
                this.current = current;
            }

            /*  ----------------------------------------------------------------------------
                IFocus interface
            */
            public QilNode GetCurrent()  {
                return current;
            }

            public QilNode GetPosition() {
                Debug.Fail("GetPosition() must not be called");
                return null;
            }

            public QilNode GetLast() {
                Debug.Fail("GetLast() must not be called");
                return null;
            }
        }
    }
}
