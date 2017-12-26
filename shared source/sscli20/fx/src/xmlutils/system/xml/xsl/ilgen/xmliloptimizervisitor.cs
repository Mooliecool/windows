//------------------------------------------------------------------------------
// <copyright file="XmlILOptimizerVisitor.cs" company="Microsoft">
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
using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Xml;
using System.Xml.XPath;
using System.Xml.Schema;
using TypeFactory = System.Xml.Xsl.XmlQueryTypeFactory;
using System.Xml.Xsl.Qil;
using System.Xml.Xsl.Runtime;

namespace System.Xml.Xsl.IlGen {

    /// <summary>
    /// </summary>
    internal class XmlILOptimizerVisitor : QilPatternVisitor {
        private static readonly QilPatterns PatternsNoOpt, PatternsOpt;
        private QilExpression qil;
        private XmlILElementAnalyzer elemAnalyzer;
        private XmlILStateAnalyzer contentAnalyzer;
        private XmlILNamespaceAnalyzer nmspAnalyzer;
        private NodeCounter nodeCounter = new NodeCounter();
        private SubstitutionList subs = new SubstitutionList();

        static XmlILOptimizerVisitor() {
            // Enable all normalizations and annotations for Release code
            // Enable all patterns for Release code
            PatternsOpt = new QilPatterns((int) XmlILOptimization.Last_, true);

            // Only enable Required and OptimizedConstruction pattern groups
            // Only enable Required patterns
            PatternsNoOpt = new QilPatterns((int) XmlILOptimization.Last_, false);

            PatternsNoOpt.Add((int) XmlILOptimization.FoldNone);
            PatternsNoOpt.Add((int) XmlILOptimization.EliminatePositionOf);
            PatternsNoOpt.Add((int) XmlILOptimization.EliminateTypeAssert);
            PatternsNoOpt.Add((int) XmlILOptimization.EliminateIsType);
            PatternsNoOpt.Add((int) XmlILOptimization.EliminateIsEmpty);
            PatternsNoOpt.Add((int) XmlILOptimization.EliminateAverage);
            PatternsNoOpt.Add((int) XmlILOptimization.EliminateSum);
            PatternsNoOpt.Add((int) XmlILOptimization.EliminateMinimum);
            PatternsNoOpt.Add((int) XmlILOptimization.EliminateMaximum);
            PatternsNoOpt.Add((int) XmlILOptimization.EliminateSort);
            PatternsNoOpt.Add((int) XmlILOptimization.EliminateStrConcatSingle);

            PatternsNoOpt.Add((int) XmlILOptimization.NormalizeUnion);
            PatternsNoOpt.Add((int) XmlILOptimization.NormalizeIntersect);
            PatternsNoOpt.Add((int) XmlILOptimization.NormalizeDifference);

            PatternsNoOpt.Add((int) XmlILOptimization.AnnotatePositionalIterator);
            PatternsNoOpt.Add((int) XmlILOptimization.AnnotateTrackCallers);
            PatternsNoOpt.Add((int) XmlILOptimization.AnnotateDod);
            PatternsNoOpt.Add((int) XmlILOptimization.AnnotateConstruction);
            PatternsNoOpt.Add((int) XmlILOptimization.AnnotateIndex1);
            PatternsNoOpt.Add((int) XmlILOptimization.AnnotateIndex2);
        }

        public XmlILOptimizerVisitor(QilExpression qil, bool optimize) : base(optimize ? PatternsOpt : PatternsNoOpt, qil.Factory) {
            this.qil = qil;
            this.elemAnalyzer = new XmlILElementAnalyzer(qil.Factory);
            this.contentAnalyzer = new XmlILStateAnalyzer(qil.Factory);
            this.nmspAnalyzer = new XmlILNamespaceAnalyzer();
        }

        /// <summary>
        /// Perform normalization and annotation.
        /// </summary>
        public QilExpression Optimize() {
            QilExpression qil = (QilExpression) Visit(this.qil);

            // Perform tail-call analysis on all functions within the Qil expression
            if (this[XmlILOptimization.TailCall])
                TailCallAnalyzer.Analyze(qil);

            return qil;
        }

        /// <summary>
        /// Override the Visit method in order to scan for redundant namespaces and compute side-effect bit.
        /// </summary>
        protected override QilNode Visit(QilNode nd) {
            if (nd != null) {
                if (this[XmlILOptimization.EliminateNamespaceDecl]) {
                    // Eliminate redundant namespaces in the tree.  Don't perform the scan on an ElementCtor which
                    // has already been marked as having a redundant namespace.
                    switch (nd.NodeType) {
                        case QilNodeType.QilExpression:
                            // Perform namespace analysis on root expression (xmlns="" is in-scope for this expression)
                            this.nmspAnalyzer.Analyze(((QilExpression) nd).Root, true);
                            break;

                        case QilNodeType.ElementCtor:
                            if (!XmlILConstructInfo.Read(nd).IsNamespaceInScope)
                                this.nmspAnalyzer.Analyze(nd, false);
                            break;

                        case QilNodeType.DocumentCtor:
                            this.nmspAnalyzer.Analyze(nd, true);
                            break;
                    }
                }
            }

            // Continue visitation
            return base.Visit(nd);
        }

        /// <summary>
        /// Override the VisitReference method in order to possibly substitute.
        /// </summary>
        protected override QilNode VisitReference(QilNode oldNode) {
            QilNode newNode = this.subs.FindReplacement(oldNode);

            if (newNode == null)
                newNode = oldNode;

            // Fold reference to constant value
            // This is done here because "p" currently cannot match references
            if (this[XmlILOptimization.FoldConstant] && newNode != null) {
                if (newNode.NodeType == QilNodeType.Let || newNode.NodeType == QilNodeType.For) {
                    QilNode binding = ((QilIterator) oldNode).Binding;

                    if (IsLiteral(binding))
                        return Replace(XmlILOptimization.FoldConstant, newNode, binding.ShallowClone(f));
                }
            }

            return base.VisitReference(newNode);
        }

        /// <summary>
        /// Strongly-typed AllowReplace.
        /// </summary>
        protected bool AllowReplace(XmlILOptimization pattern, QilNode original) {
            return base.AllowReplace((int) pattern, original);
        }

        /// <summary>
        /// Strongly-typed Replace.
        /// </summary>
        protected QilNode Replace(XmlILOptimization pattern, QilNode original, QilNode replacement) {
            return base.Replace((int) pattern, original, replacement);
        }

        /// <summary>
        /// Called when all replacements have already been made and all annotations are complete.
        /// </summary>
        protected override QilNode NoReplace(QilNode node) {
            // Calculate MaybeSideEffects pattern.  This is done here rather than using P because every node needs
            // to compute it and P has no good way of matching every node type.
            if (node != null) {
                switch (node.NodeType) {
                    case QilNodeType.Error:
                    case QilNodeType.Warning:
                    case QilNodeType.XsltInvokeLateBound:
                        // Error, Warning, and XsltInvokeLateBound are always assumed to have side-effects
                        OptimizerPatterns.Write(node).AddPattern(OptimizerPatternName.MaybeSideEffects);
                        break;

                    case QilNodeType.XsltInvokeEarlyBound:
                        // XsltInvokeEarlyBound is assumed to have side-effects if it is not a built-in function
                        if (((QilInvokeEarlyBound) node).Name.NamespaceUri.Length != 0)
                            goto case QilNodeType.XsltInvokeLateBound;
                        goto default;

                    case QilNodeType.Invoke:
                        // Invoke is assumed to have side-effects if it invokes a function with its SideEffects flag set
                        if (((QilInvoke) node).Function.MaybeSideEffects)
                            goto case QilNodeType.XsltInvokeLateBound;

                        // Otherwise, check children
                        goto default;

                    default:
                        // If any of the visited node's children have side effects, then mark the node as also having side effects
                        for (int i = 0; i < node.Count; i++) {
                            if (node[i] != null) {
                                if (OptimizerPatterns.Read(node[i]).MatchesPattern(OptimizerPatternName.MaybeSideEffects))
                                    goto case QilNodeType.XsltInvokeLateBound;
                            }
                        }
                        break;
                }
            }

            return node;
        }

        /// <summary>
        /// Override the RecalculateType method so that global variable type is not recalculated.
        /// </summary>
        protected override void RecalculateType(QilNode node, XmlQueryType oldType) {
             if (node.NodeType != QilNodeType.Let || !this.qil.GlobalVariableList.Contains(node))
                base.RecalculateType(node, oldType);
        }

        // Do not edit this region
        // It is auto-generated
        #region AUTOGENERATED

        #region meta
        protected override QilNode VisitQilExpression(QilExpression local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.EliminateUnusedFunctions]) {
                if (AllowReplace(XmlILOptimization.EliminateUnusedFunctions, local0)) {
                    
    IList<QilNode> funcList = local0.FunctionList;
    for (int i = funcList.Count - 1; i >= 0; i--) {
        if (XmlILConstructInfo.Write(funcList[i]).CallersInfo.Count == 0)
            funcList.RemoveAt(i);
    }
}
            }
            if (this[XmlILOptimization.AnnotateConstruction]) {
                if (AllowReplace(XmlILOptimization.AnnotateConstruction, local0)) {
                    
    foreach (QilFunction ndFunc in local0.FunctionList) {
        // Functions that construct Xml trees should stream output to writer; otherwise, results should
        // be cached and returned.
        if (IsConstructedExpression(ndFunc.Definition)) {
            // Perform state analysis on function's content
            ndFunc.Definition = this.contentAnalyzer.Analyze(ndFunc, ndFunc.Definition);
        }
    }

    // Perform state analysis on the root expression
    local0.Root = this.contentAnalyzer.Analyze(null, local0.Root);

    // Make sure that root expression is pushed to writer
    XmlILConstructInfo.Write(local0.Root).PushToWriterLast = true;
}
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitOptimizeBarrier(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.AnnotateBarrier]) {
                if (AllowReplace(XmlILOptimization.AnnotateBarrier, local0)) {
                     OptimizerPatterns.Inherit((QilNode) (local1), (QilNode) (local0), OptimizerPatternName.IsDocOrderDistinct);  OptimizerPatterns.Inherit((QilNode) (local1), (QilNode) (local0), OptimizerPatternName.SameDepth);  }
            }
            return NoReplace(local0);
        }
        
        #endregion // meta
        
        #region specials
        protected override QilNode VisitDataSource(QilDataSource local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitNop(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.EliminateNop]) {
                if (AllowReplace(XmlILOptimization.EliminateNop, local0)) {
                    return Replace(XmlILOptimization.EliminateNop, local0, local1);
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitError(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitWarning(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        #endregion // specials
        
        #region variables
        protected override QilNode VisitLet(QilIterator local0) {
            QilNode local1 = local0[0];
            if ((( ( (local0).XmlType ).IsSingleton ) && (!( IsGlobalVariable(local0) ))) && (this[XmlILOptimization.NormalizeSingletonLet])) {
                if (AllowReplace(XmlILOptimization.NormalizeSingletonLet, local0)) {
                    
    local0.NodeType = QilNodeType.For;
    VisitFor(local0);
}
            }
            if (this[XmlILOptimization.AnnotateLet]) {
                if (AllowReplace(XmlILOptimization.AnnotateLet, local0)) {
                     OptimizerPatterns.Inherit((QilNode) (local1), (QilNode) (local0), OptimizerPatternName.Step);  OptimizerPatterns.Inherit((QilNode) (local1), (QilNode) (local0), OptimizerPatternName.IsDocOrderDistinct);  OptimizerPatterns.Inherit((QilNode) (local1), (QilNode) (local0), OptimizerPatternName.SameDepth);  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitPositionOf(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.EliminatePositionOf]) {
                if (!( (local1).NodeType == QilNodeType.For )) {
                    if (AllowReplace(XmlILOptimization.EliminatePositionOf, local0)) {
                        return Replace(XmlILOptimization.EliminatePositionOf, local0, VisitLiteralInt32(f.LiteralInt32(1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminatePositionOf]) {
                if (local1.NodeType == QilNodeType.For) {
                    QilNode local2 = local1[0];
                    if ( ( (local2).XmlType ).IsSingleton ) {
                        if (AllowReplace(XmlILOptimization.EliminatePositionOf, local0)) {
                            return Replace(XmlILOptimization.EliminatePositionOf, local0, VisitLiteralInt32(f.LiteralInt32(1)));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotatePositionalIterator]) {
                if (AllowReplace(XmlILOptimization.AnnotatePositionalIterator, local0)) {
                     OptimizerPatterns.Write((QilNode) (local1)).AddPattern(OptimizerPatternName.IsPositional);  }
            }
            return NoReplace(local0);
        }
        
        #endregion // variables
        
        #region literals
        #endregion // literals
        
        #region boolean operators
        protected override QilNode VisitAnd(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateAnd]) {
                if (local1.NodeType == QilNodeType.True) {
                    if (AllowReplace(XmlILOptimization.EliminateAnd, local0)) {
                        return Replace(XmlILOptimization.EliminateAnd, local0, local2);
                    }
                }
            }
            if (this[XmlILOptimization.EliminateAnd]) {
                if (local1.NodeType == QilNodeType.False) {
                    if (AllowReplace(XmlILOptimization.EliminateAnd, local0)) {
                        return Replace(XmlILOptimization.EliminateAnd, local0, local1);
                    }
                }
            }
            if (this[XmlILOptimization.EliminateAnd]) {
                if (local2.NodeType == QilNodeType.True) {
                    if (AllowReplace(XmlILOptimization.EliminateAnd, local0)) {
                        return Replace(XmlILOptimization.EliminateAnd, local0, local1);
                    }
                }
            }
            if (this[XmlILOptimization.EliminateAnd]) {
                if (local2.NodeType == QilNodeType.False) {
                    if (AllowReplace(XmlILOptimization.EliminateAnd, local0)) {
                        return Replace(XmlILOptimization.EliminateAnd, local0, local2);
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitOr(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateOr]) {
                if (local1.NodeType == QilNodeType.True) {
                    if (AllowReplace(XmlILOptimization.EliminateOr, local0)) {
                        return Replace(XmlILOptimization.EliminateOr, local0, local1);
                    }
                }
            }
            if (this[XmlILOptimization.EliminateOr]) {
                if (local1.NodeType == QilNodeType.False) {
                    if (AllowReplace(XmlILOptimization.EliminateOr, local0)) {
                        return Replace(XmlILOptimization.EliminateOr, local0, local2);
                    }
                }
            }
            if (this[XmlILOptimization.EliminateOr]) {
                if (local2.NodeType == QilNodeType.True) {
                    if (AllowReplace(XmlILOptimization.EliminateOr, local0)) {
                        return Replace(XmlILOptimization.EliminateOr, local0, local2);
                    }
                }
            }
            if (this[XmlILOptimization.EliminateOr]) {
                if (local2.NodeType == QilNodeType.False) {
                    if (AllowReplace(XmlILOptimization.EliminateOr, local0)) {
                        return Replace(XmlILOptimization.EliminateOr, local0, local1);
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitNot(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateNot]) {
                if (local1.NodeType == QilNodeType.True) {
                    if (AllowReplace(XmlILOptimization.EliminateNot, local0)) {
                        return Replace(XmlILOptimization.EliminateNot, local0, VisitFalse(f.False()));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateNot]) {
                if (local1.NodeType == QilNodeType.False) {
                    if (AllowReplace(XmlILOptimization.EliminateNot, local0)) {
                        return Replace(XmlILOptimization.EliminateNot, local0, VisitTrue(f.True()));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        #endregion // boolean operators
        
        #region choice
        protected override QilNode VisitConditional(QilTernary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            QilNode local3 = local0[2];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateConditional]) {
                if (local1.NodeType == QilNodeType.True) {
                    if (AllowReplace(XmlILOptimization.EliminateConditional, local0)) {
                        return Replace(XmlILOptimization.EliminateConditional, local0, local2);
                    }
                }
            }
            if (this[XmlILOptimization.EliminateConditional]) {
                if (local1.NodeType == QilNodeType.False) {
                    if (AllowReplace(XmlILOptimization.EliminateConditional, local0)) {
                        return Replace(XmlILOptimization.EliminateConditional, local0, local3);
                    }
                }
            }
            if (this[XmlILOptimization.EliminateConditional]) {
                if (local2.NodeType == QilNodeType.True) {
                    if (local3.NodeType == QilNodeType.False) {
                        if (AllowReplace(XmlILOptimization.EliminateConditional, local0)) {
                            return Replace(XmlILOptimization.EliminateConditional, local0, local1);
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateConditional]) {
                if (local2.NodeType == QilNodeType.False) {
                    if (local3.NodeType == QilNodeType.True) {
                        if (AllowReplace(XmlILOptimization.EliminateConditional, local0)) {
                            return Replace(XmlILOptimization.EliminateConditional, local0, VisitNot(f.Not(local1)));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.FoldConditionalNot]) {
                if (local1.NodeType == QilNodeType.Not) {
                    QilNode local4 = local1[0];
                    if (AllowReplace(XmlILOptimization.FoldConditionalNot, local0)) {
                        return Replace(XmlILOptimization.FoldConditionalNot, local0, VisitConditional(f.Conditional(local4, local3, local2)));
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeConditionalText]) {
                if (local2.NodeType == QilNodeType.TextCtor) {
                    QilNode local4 = local2[0];
                    if (local3.NodeType == QilNodeType.TextCtor) {
                        QilNode local5 = local3[0];
                        if (AllowReplace(XmlILOptimization.NormalizeConditionalText, local0)) {
                            return Replace(XmlILOptimization.NormalizeConditionalText, local0, VisitTextCtor(f.TextCtor(VisitConditional(f.Conditional(local1, local4, local5)))));
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitChoice(QilChoice local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.AnnotateConstruction]) {
                if (AllowReplace(XmlILOptimization.AnnotateConstruction, local0)) {
                    
    this.contentAnalyzer.Analyze(local0, null);
}
            }
            return NoReplace(local0);
        }
        
        #endregion // choice
        
        #region collection operators
        protected override QilNode VisitLength(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateLength]) {
                if (local1.NodeType == QilNodeType.Sequence) {
                    if ( (local1).Count == (0) ) {
                        if (AllowReplace(XmlILOptimization.EliminateLength, local0)) {
                            return Replace(XmlILOptimization.EliminateLength, local0, VisitLiteralInt32(f.LiteralInt32(0)));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateLength]) {
                if (( ( (local1).XmlType ).IsSingleton ) && ( !OptimizerPatterns.Read(local1).MatchesPattern(OptimizerPatternName.MaybeSideEffects) )) {
                    if (AllowReplace(XmlILOptimization.EliminateLength, local0)) {
                        return Replace(XmlILOptimization.EliminateLength, local0, VisitLiteralInt32(f.LiteralInt32(1)));
                    }
                }
            }
            if (this[XmlILOptimization.IntroducePrecedingDod]) {
                if ((!( IsDocOrderDistinct(local1) )) && (( IsStepPattern(local1, QilNodeType.XPathPreceding) ) || ( IsStepPattern(local1, QilNodeType.PrecedingSibling) ))) {
                    if (AllowReplace(XmlILOptimization.IntroducePrecedingDod, local0)) {
                        return Replace(XmlILOptimization.IntroducePrecedingDod, local0, VisitLength(f.Length(VisitDocOrderDistinct(f.DocOrderDistinct(local1)))));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitSequence(QilList local0) {
            if (( (local0).Count == (1) ) && (this[XmlILOptimization.EliminateSequence])) {
                if (AllowReplace(XmlILOptimization.EliminateSequence, local0)) {
                    return Replace(XmlILOptimization.EliminateSequence, local0,  (QilNode) (local0)[0] );
                }
            }
            if (( HasNestedSequence(local0) ) && (this[XmlILOptimization.NormalizeNestedSequences])) {
                if (AllowReplace(XmlILOptimization.NormalizeNestedSequences, local0)) {
                    QilNode local1 = VisitSequence(f.Sequence());
                    
    foreach (QilNode nd in local0) {
        if (nd.NodeType == QilNodeType.Sequence)
            local1.Add((IList<QilNode>) nd);
        else
            local1.Add(nd);
    }

    // Match patterns on new sequence
    local1 = VisitSequence((QilList) local1);
return Replace(XmlILOptimization.NormalizeNestedSequences, local0, local1);
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitUnion(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateUnion]) {
                if (local2 == local1) {
                    if (AllowReplace(XmlILOptimization.EliminateUnion, local0)) {
                        return Replace(XmlILOptimization.EliminateUnion, local0, VisitDocOrderDistinct(f.DocOrderDistinct(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateUnion]) {
                if (local1.NodeType == QilNodeType.Sequence) {
                    if ( (local1).Count == (0) ) {
                        if (AllowReplace(XmlILOptimization.EliminateUnion, local0)) {
                            return Replace(XmlILOptimization.EliminateUnion, local0, VisitDocOrderDistinct(f.DocOrderDistinct(local2)));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateUnion]) {
                if (local2.NodeType == QilNodeType.Sequence) {
                    if ( (local2).Count == (0) ) {
                        if (AllowReplace(XmlILOptimization.EliminateUnion, local0)) {
                            return Replace(XmlILOptimization.EliminateUnion, local0, VisitDocOrderDistinct(f.DocOrderDistinct(local1)));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateUnion]) {
                if (local1.NodeType == QilNodeType.XmlContext) {
                    if (local2.NodeType == QilNodeType.XmlContext) {
                        if (AllowReplace(XmlILOptimization.EliminateUnion, local0)) {
                            return Replace(XmlILOptimization.EliminateUnion, local0, local1);
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeUnion]) {
                if ((!( IsDocOrderDistinct(local1) )) || (!( IsDocOrderDistinct(local2) ))) {
                    if (AllowReplace(XmlILOptimization.NormalizeUnion, local0)) {
                        return Replace(XmlILOptimization.NormalizeUnion, local0, VisitUnion(f.Union(VisitDocOrderDistinct(f.DocOrderDistinct(local1)), VisitDocOrderDistinct(f.DocOrderDistinct(local2)))));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateUnion]) {
                if (AllowReplace(XmlILOptimization.AnnotateUnion, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.IsDocOrderDistinct);  }
            }
            if (this[XmlILOptimization.AnnotateUnionContent]) {
                if (( IsStepPattern(local1, QilNodeType.Content) ) || ( IsStepPattern(local1, QilNodeType.Union) )) {
                    if ((( IsStepPattern(local2, QilNodeType.Content) ) || ( IsStepPattern(local2, QilNodeType.Union) )) && ( ( OptimizerPatterns.Read((QilNode) (local1)).GetArgument(OptimizerPatternArgument.StepInput) ) == ( OptimizerPatterns.Read((QilNode) (local2)).GetArgument(OptimizerPatternArgument.StepInput) ) )) {
                        if (AllowReplace(XmlILOptimization.AnnotateUnionContent, local0)) {
                             AddStepPattern((QilNode) (local0), (QilNode) ( OptimizerPatterns.Read((QilNode) (local1)).GetArgument(OptimizerPatternArgument.StepInput) ));  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.SameDepth);  }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitIntersection(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateIntersection]) {
                if (local2 == local1) {
                    if (AllowReplace(XmlILOptimization.EliminateIntersection, local0)) {
                        return Replace(XmlILOptimization.EliminateIntersection, local0, VisitDocOrderDistinct(f.DocOrderDistinct(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateIntersection]) {
                if (local1.NodeType == QilNodeType.Sequence) {
                    if ( (local1).Count == (0) ) {
                        if (AllowReplace(XmlILOptimization.EliminateIntersection, local0)) {
                            return Replace(XmlILOptimization.EliminateIntersection, local0, local1);
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateIntersection]) {
                if (local2.NodeType == QilNodeType.Sequence) {
                    if ( (local2).Count == (0) ) {
                        if (AllowReplace(XmlILOptimization.EliminateIntersection, local0)) {
                            return Replace(XmlILOptimization.EliminateIntersection, local0, local2);
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateIntersection]) {
                if (local1.NodeType == QilNodeType.XmlContext) {
                    if (local2.NodeType == QilNodeType.XmlContext) {
                        if (AllowReplace(XmlILOptimization.EliminateIntersection, local0)) {
                            return Replace(XmlILOptimization.EliminateIntersection, local0, local1);
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeIntersect]) {
                if ((!( IsDocOrderDistinct(local1) )) || (!( IsDocOrderDistinct(local2) ))) {
                    if (AllowReplace(XmlILOptimization.NormalizeIntersect, local0)) {
                        return Replace(XmlILOptimization.NormalizeIntersect, local0, VisitIntersection(f.Intersection(VisitDocOrderDistinct(f.DocOrderDistinct(local1)), VisitDocOrderDistinct(f.DocOrderDistinct(local2)))));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateIntersect]) {
                if (AllowReplace(XmlILOptimization.AnnotateIntersect, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.IsDocOrderDistinct);  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitDifference(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateDifference]) {
                if (local1.NodeType == QilNodeType.Sequence) {
                    if ( (local1).Count == (0) ) {
                        if (AllowReplace(XmlILOptimization.EliminateDifference, local0)) {
                            return Replace(XmlILOptimization.EliminateDifference, local0, local1);
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateDifference]) {
                if (local2.NodeType == QilNodeType.Sequence) {
                    if ( (local2).Count == (0) ) {
                        if (AllowReplace(XmlILOptimization.EliminateDifference, local0)) {
                            return Replace(XmlILOptimization.EliminateDifference, local0, VisitDocOrderDistinct(f.DocOrderDistinct(local1)));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateDifference]) {
                if (local2 == local1) {
                    if (AllowReplace(XmlILOptimization.EliminateDifference, local0)) {
                        return Replace(XmlILOptimization.EliminateDifference, local0, VisitSequence(f.Sequence()));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateDifference]) {
                if (local1.NodeType == QilNodeType.XmlContext) {
                    if (local2.NodeType == QilNodeType.XmlContext) {
                        if (AllowReplace(XmlILOptimization.EliminateDifference, local0)) {
                            return Replace(XmlILOptimization.EliminateDifference, local0, VisitSequence(f.Sequence()));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeDifference]) {
                if ((!( IsDocOrderDistinct(local1) )) || (!( IsDocOrderDistinct(local2) ))) {
                    if (AllowReplace(XmlILOptimization.NormalizeDifference, local0)) {
                        return Replace(XmlILOptimization.NormalizeDifference, local0, VisitDifference(f.Difference(VisitDocOrderDistinct(f.DocOrderDistinct(local1)), VisitDocOrderDistinct(f.DocOrderDistinct(local2)))));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateDifference]) {
                if (AllowReplace(XmlILOptimization.AnnotateDifference, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.IsDocOrderDistinct);  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitAverage(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateAverage]) {
                if ( ( (local1).XmlType ).Cardinality == XmlQueryCardinality.Zero ) {
                    if (AllowReplace(XmlILOptimization.EliminateAverage, local0)) {
                        return Replace(XmlILOptimization.EliminateAverage, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitSum(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateSum]) {
                if ( ( (local1).XmlType ).Cardinality == XmlQueryCardinality.Zero ) {
                    if (AllowReplace(XmlILOptimization.EliminateSum, local0)) {
                        return Replace(XmlILOptimization.EliminateSum, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitMinimum(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateMinimum]) {
                if ( ( (local1).XmlType ).Cardinality == XmlQueryCardinality.Zero ) {
                    if (AllowReplace(XmlILOptimization.EliminateMinimum, local0)) {
                        return Replace(XmlILOptimization.EliminateMinimum, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitMaximum(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateMaximum]) {
                if ( ( (local1).XmlType ).Cardinality == XmlQueryCardinality.Zero ) {
                    if (AllowReplace(XmlILOptimization.EliminateMaximum, local0)) {
                        return Replace(XmlILOptimization.EliminateMaximum, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        #endregion // collection operators
        
        #region arithmetic operators
        protected override QilNode VisitNegate(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateNegate]) {
                if (local1.NodeType == QilNodeType.LiteralDecimal) {
                    decimal local2 = (decimal)((QilLiteral)local1).Value;
                    if (AllowReplace(XmlILOptimization.EliminateNegate, local0)) {
                        return Replace(XmlILOptimization.EliminateNegate, local0, VisitLiteralDecimal(f.LiteralDecimal( -local2 )));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateNegate]) {
                if (local1.NodeType == QilNodeType.LiteralDouble) {
                    double local2 = (double)((QilLiteral)local1).Value;
                    if (AllowReplace(XmlILOptimization.EliminateNegate, local0)) {
                        return Replace(XmlILOptimization.EliminateNegate, local0, VisitLiteralDouble(f.LiteralDouble( -local2 )));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateNegate]) {
                if (local1.NodeType == QilNodeType.LiteralInt32) {
                    int local2 = (int)((QilLiteral)local1).Value;
                    if (AllowReplace(XmlILOptimization.EliminateNegate, local0)) {
                        return Replace(XmlILOptimization.EliminateNegate, local0, VisitLiteralInt32(f.LiteralInt32( -local2 )));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateNegate]) {
                if (local1.NodeType == QilNodeType.LiteralInt64) {
                    long local2 = (long)((QilLiteral)local1).Value;
                    if (AllowReplace(XmlILOptimization.EliminateNegate, local0)) {
                        return Replace(XmlILOptimization.EliminateNegate, local0, VisitLiteralInt64(f.LiteralInt64( -local2 )));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitAdd(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateAdd]) {
                if ( IsLiteral((local1)) ) {
                    if (( IsLiteral((local2)) ) && ( CanFoldArithmetic(QilNodeType.Add, (QilLiteral) local1, (QilLiteral) local2) )) {
                        if (AllowReplace(XmlILOptimization.EliminateAdd, local0)) {
                            return Replace(XmlILOptimization.EliminateAdd, local0,  FoldArithmetic(QilNodeType.Add, (QilLiteral) local1, (QilLiteral) local2) );
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeAddLiteral]) {
                if ( IsLiteral((local1)) ) {
                    if (!( IsLiteral((local2)) )) {
                        if (AllowReplace(XmlILOptimization.NormalizeAddLiteral, local0)) {
                            return Replace(XmlILOptimization.NormalizeAddLiteral, local0, VisitAdd(f.Add(local2, local1)));
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitSubtract(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateSubtract]) {
                if ( IsLiteral((local1)) ) {
                    if (( IsLiteral((local2)) ) && ( CanFoldArithmetic(QilNodeType.Subtract, (QilLiteral) local1, (QilLiteral) local2) )) {
                        if (AllowReplace(XmlILOptimization.EliminateSubtract, local0)) {
                            return Replace(XmlILOptimization.EliminateSubtract, local0,  FoldArithmetic(QilNodeType.Subtract, (QilLiteral) local1, (QilLiteral) local2) );
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitMultiply(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateMultiply]) {
                if ( IsLiteral((local1)) ) {
                    if (( IsLiteral((local2)) ) && ( CanFoldArithmetic(QilNodeType.Multiply, (QilLiteral) local1, (QilLiteral) local2) )) {
                        if (AllowReplace(XmlILOptimization.EliminateMultiply, local0)) {
                            return Replace(XmlILOptimization.EliminateMultiply, local0,  FoldArithmetic(QilNodeType.Multiply, (QilLiteral) local1, (QilLiteral) local2) );
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeMultiplyLiteral]) {
                if ( IsLiteral((local1)) ) {
                    if (!( IsLiteral((local2)) )) {
                        if (AllowReplace(XmlILOptimization.NormalizeMultiplyLiteral, local0)) {
                            return Replace(XmlILOptimization.NormalizeMultiplyLiteral, local0, VisitMultiply(f.Multiply(local2, local1)));
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitDivide(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateDivide]) {
                if ( IsLiteral((local1)) ) {
                    if (( IsLiteral((local2)) ) && ( CanFoldArithmetic(QilNodeType.Divide, (QilLiteral) local1, (QilLiteral) local2) )) {
                        if (AllowReplace(XmlILOptimization.EliminateDivide, local0)) {
                            return Replace(XmlILOptimization.EliminateDivide, local0,  FoldArithmetic(QilNodeType.Divide, (QilLiteral) local1, (QilLiteral) local2) );
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitModulo(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateModulo]) {
                if ( IsLiteral((local1)) ) {
                    if (( IsLiteral((local2)) ) && ( CanFoldArithmetic(QilNodeType.Modulo, (QilLiteral) local1, (QilLiteral) local2) )) {
                        if (AllowReplace(XmlILOptimization.EliminateModulo, local0)) {
                            return Replace(XmlILOptimization.EliminateModulo, local0,  FoldArithmetic(QilNodeType.Modulo, (QilLiteral) local1, (QilLiteral) local2) );
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        #endregion // arithmetic operators
        
        #region string operators
        protected override QilNode VisitStrLength(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateStrLength]) {
                if (local1.NodeType == QilNodeType.LiteralString) {
                    string local2 = (string)((QilLiteral)local1).Value;
                    if (AllowReplace(XmlILOptimization.EliminateStrLength, local0)) {
                        return Replace(XmlILOptimization.EliminateStrLength, local0, VisitLiteralInt32(f.LiteralInt32( local2.Length )));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitStrConcat(QilStrConcat local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (( ( (local2).XmlType ).IsSingleton ) && (this[XmlILOptimization.EliminateStrConcatSingle])) {
                if (AllowReplace(XmlILOptimization.EliminateStrConcatSingle, local0)) {
                    return Replace(XmlILOptimization.EliminateStrConcatSingle, local0, VisitNop(f.Nop(local2)));
                }
            }
            if (this[XmlILOptimization.EliminateStrConcat]) {
                if (local1.NodeType == QilNodeType.LiteralString) {
                    string local3 = (string)((QilLiteral)local1).Value;
                    if (local2.NodeType == QilNodeType.Sequence) {
                        if ( AreLiteralArgs(local2) ) {
                            if (AllowReplace(XmlILOptimization.EliminateStrConcat, local0)) {
                                
    // Concatenate all constant arguments
    StringConcat concat = new StringConcat();
    concat.Delimiter = local3;

    foreach (QilLiteral lit in local2)
        concat.Concat((string) lit);
return Replace(XmlILOptimization.EliminateStrConcat, local0, VisitLiteralString(f.LiteralString( concat.GetResult() )));
                            }
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitStrParseQName(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        #endregion // string operators
        
        #region value comparison operators
        protected override QilNode VisitNe(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateNe]) {
                if ( IsLiteral((local1)) ) {
                    if ( IsLiteral((local2)) ) {
                        if (AllowReplace(XmlILOptimization.EliminateNe, local0)) {
                            return Replace(XmlILOptimization.EliminateNe, local0,  FoldComparison(QilNodeType.Ne, local1, local2) );
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeNeLiteral]) {
                if ( IsLiteral((local1)) ) {
                    if (!( IsLiteral((local2)) )) {
                        if (AllowReplace(XmlILOptimization.NormalizeNeLiteral, local0)) {
                            return Replace(XmlILOptimization.NormalizeNeLiteral, local0, VisitNe(f.Ne(local2, local1)));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeXsltConvertNe]) {
                if (local1.NodeType == QilNodeType.XsltConvert) {
                    QilNode local3 = local1[0];
                    QilNode local4 = local1[1];
                    if (local4.NodeType == QilNodeType.LiteralType) {
                        XmlQueryType local5 = (XmlQueryType)((QilLiteral)local4).Value;
                        if (( IsPrimitiveNumeric( (local3).XmlType ) ) && ( IsPrimitiveNumeric(local5) )) {
                            if (( IsLiteral((local2)) ) && ( CanFoldXsltConvertNonLossy(local2,  (local3).XmlType ) )) {
                                if (AllowReplace(XmlILOptimization.NormalizeXsltConvertNe, local0)) {
                                    return Replace(XmlILOptimization.NormalizeXsltConvertNe, local0, VisitNe(f.Ne(local3,  FoldXsltConvert(local2,  (local3).XmlType ) )));
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeIdNe]) {
                if (local1.NodeType == QilNodeType.XsltGenerateId) {
                    QilNode local3 = local1[0];
                    if ( ( (local3).XmlType ).IsSingleton ) {
                        if (local2.NodeType == QilNodeType.XsltGenerateId) {
                            QilNode local4 = local2[0];
                            if ( ( (local4).XmlType ).IsSingleton ) {
                                if (AllowReplace(XmlILOptimization.NormalizeIdNe, local0)) {
                                    return Replace(XmlILOptimization.NormalizeIdNe, local0, VisitNot(f.Not(VisitIs(f.Is(local3, local4)))));
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeLengthNe]) {
                if (local1.NodeType == QilNodeType.Length) {
                    QilNode local3 = local1[0];
                    if (local2.NodeType == QilNodeType.LiteralInt32) {
                        int local4 = (int)((QilLiteral)local2).Value;
                        if (local4 == 0) {
                            if (AllowReplace(XmlILOptimization.NormalizeLengthNe, local0)) {
                                return Replace(XmlILOptimization.NormalizeLengthNe, local0, VisitNot(f.Not(VisitIsEmpty(f.IsEmpty(local3)))));
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateMaxLengthNe]) {
                if (local1.NodeType == QilNodeType.Length) {
                    if (local2.NodeType == QilNodeType.LiteralInt32) {
                        int local4 = (int)((QilLiteral)local2).Value;
                        if (AllowReplace(XmlILOptimization.AnnotateMaxLengthNe, local0)) {
                             OptimizerPatterns.Write((QilNode) (local1)).AddPattern(OptimizerPatternName.MaxPosition);  OptimizerPatterns.Write((QilNode) (local1)).AddArgument(OptimizerPatternArgument.MaxPosition, local4);  }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitEq(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateEq]) {
                if ( IsLiteral((local1)) ) {
                    if ( IsLiteral((local2)) ) {
                        if (AllowReplace(XmlILOptimization.EliminateEq, local0)) {
                            return Replace(XmlILOptimization.EliminateEq, local0,  FoldComparison(QilNodeType.Eq, local1, local2) );
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeEqLiteral]) {
                if ( IsLiteral((local1)) ) {
                    if (!( IsLiteral((local2)) )) {
                        if (AllowReplace(XmlILOptimization.NormalizeEqLiteral, local0)) {
                            return Replace(XmlILOptimization.NormalizeEqLiteral, local0, VisitEq(f.Eq(local2, local1)));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeXsltConvertEq]) {
                if (local1.NodeType == QilNodeType.XsltConvert) {
                    QilNode local3 = local1[0];
                    QilNode local4 = local1[1];
                    if (local4.NodeType == QilNodeType.LiteralType) {
                        XmlQueryType local5 = (XmlQueryType)((QilLiteral)local4).Value;
                        if (( IsPrimitiveNumeric( (local3).XmlType ) ) && ( IsPrimitiveNumeric(local5) )) {
                            if (( IsLiteral((local2)) ) && ( CanFoldXsltConvertNonLossy(local2,  (local3).XmlType ) )) {
                                if (AllowReplace(XmlILOptimization.NormalizeXsltConvertEq, local0)) {
                                    return Replace(XmlILOptimization.NormalizeXsltConvertEq, local0, VisitEq(f.Eq(local3,  FoldXsltConvert(local2,  (local3).XmlType ) )));
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeAddEq]) {
                if (local1.NodeType == QilNodeType.Add) {
                    QilNode local3 = local1[0];
                    QilNode local4 = local1[1];
                    if ( IsLiteral((local4)) ) {
                        if (( IsLiteral((local2)) ) && ( CanFoldArithmetic(QilNodeType.Subtract, (QilLiteral) local2, (QilLiteral) local4) )) {
                            if (AllowReplace(XmlILOptimization.NormalizeAddEq, local0)) {
                                return Replace(XmlILOptimization.NormalizeAddEq, local0, VisitEq(f.Eq(local3,  FoldArithmetic(QilNodeType.Subtract, (QilLiteral) local2, (QilLiteral) local4) )));
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeIdEq]) {
                if (local1.NodeType == QilNodeType.XsltGenerateId) {
                    QilNode local3 = local1[0];
                    if ( ( (local3).XmlType ).IsSingleton ) {
                        if (local2.NodeType == QilNodeType.XsltGenerateId) {
                            QilNode local4 = local2[0];
                            if ( ( (local4).XmlType ).IsSingleton ) {
                                if (AllowReplace(XmlILOptimization.NormalizeIdEq, local0)) {
                                    return Replace(XmlILOptimization.NormalizeIdEq, local0, VisitIs(f.Is(local3, local4)));
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeIdEq]) {
                if (local1.NodeType == QilNodeType.XsltGenerateId) {
                    QilNode local3 = local1[0];
                    if ( ( (local3).XmlType ).IsSingleton ) {
                        if (local2.NodeType == QilNodeType.StrConcat) {
                            QilNode local5 = local2[1];
                            if (local5.NodeType == QilNodeType.Loop) {
                                QilNode local6 = local5[0];
                                QilNode local8 = local5[1];
                                if (local6.NodeType == QilNodeType.For) {
                                    QilNode local7 = local6[0];
                                    if ( !( (local7).XmlType ).MaybeMany ) {
                                        if (local8.NodeType == QilNodeType.XsltGenerateId) {
                                            QilNode local9 = local8[0];
                                            if (local9 == local6) {
                                                if (AllowReplace(XmlILOptimization.NormalizeIdEq, local0)) {
                                                    QilNode local10 = VisitFor(f.For(local7));
                                                    return Replace(XmlILOptimization.NormalizeIdEq, local0, VisitNot(f.Not(VisitIsEmpty(f.IsEmpty(VisitFilter(f.Filter(local10, VisitIs(f.Is(local3, local10)))))))));
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeIdEq]) {
                if (local1.NodeType == QilNodeType.StrConcat) {
                    QilNode local4 = local1[1];
                    if (local4.NodeType == QilNodeType.Loop) {
                        QilNode local5 = local4[0];
                        QilNode local7 = local4[1];
                        if (local5.NodeType == QilNodeType.For) {
                            QilNode local6 = local5[0];
                            if ( !( (local6).XmlType ).MaybeMany ) {
                                if (local7.NodeType == QilNodeType.XsltGenerateId) {
                                    QilNode local8 = local7[0];
                                    if (local8 == local5) {
                                        if (local2.NodeType == QilNodeType.XsltGenerateId) {
                                            QilNode local9 = local2[0];
                                            if ( ( (local9).XmlType ).IsSingleton ) {
                                                if (AllowReplace(XmlILOptimization.NormalizeIdEq, local0)) {
                                                    QilNode local10 = VisitFor(f.For(local6));
                                                    return Replace(XmlILOptimization.NormalizeIdEq, local0, VisitNot(f.Not(VisitIsEmpty(f.IsEmpty(VisitFilter(f.Filter(local10, VisitIs(f.Is(local9, local10)))))))));
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeMuenchian]) {
                if (local1.NodeType == QilNodeType.Length) {
                    QilNode local3 = local1[0];
                    if (local3.NodeType == QilNodeType.Union) {
                        QilNode local4 = local3[0];
                        QilNode local5 = local3[1];
                        if (( ( (local4).XmlType ).IsSingleton ) && ( !( (local5).XmlType ).MaybeMany )) {
                            if (local2.NodeType == QilNodeType.LiteralInt32) {
                                int local6 = (int)((QilLiteral)local2).Value;
                                if (local6 == 1) {
                                    if (AllowReplace(XmlILOptimization.NormalizeMuenchian, local0)) {
                                        QilNode local7 = VisitFor(f.For(local5));
                                        return Replace(XmlILOptimization.NormalizeMuenchian, local0, VisitIsEmpty(f.IsEmpty(VisitFilter(f.Filter(local7, VisitNot(f.Not(VisitIs(f.Is(local4, local7)))))))));
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeMuenchian]) {
                if (local1.NodeType == QilNodeType.Length) {
                    QilNode local3 = local1[0];
                    if (local3.NodeType == QilNodeType.Union) {
                        QilNode local4 = local3[0];
                        QilNode local5 = local3[1];
                        if (( !( (local4).XmlType ).MaybeMany ) && ( ( (local5).XmlType ).IsSingleton )) {
                            if (local2.NodeType == QilNodeType.LiteralInt32) {
                                int local6 = (int)((QilLiteral)local2).Value;
                                if (local6 == 1) {
                                    if (AllowReplace(XmlILOptimization.NormalizeMuenchian, local0)) {
                                        QilNode local7 = VisitFor(f.For(local4));
                                        return Replace(XmlILOptimization.NormalizeMuenchian, local0, VisitIsEmpty(f.IsEmpty(VisitFilter(f.Filter(local7, VisitNot(f.Not(VisitIs(f.Is(local7, local5)))))))));
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateMaxLengthEq]) {
                if (local1.NodeType == QilNodeType.Length) {
                    if (local2.NodeType == QilNodeType.LiteralInt32) {
                        int local4 = (int)((QilLiteral)local2).Value;
                        if (AllowReplace(XmlILOptimization.AnnotateMaxLengthEq, local0)) {
                             OptimizerPatterns.Write((QilNode) (local1)).AddPattern(OptimizerPatternName.MaxPosition);  OptimizerPatterns.Write((QilNode) (local1)).AddArgument(OptimizerPatternArgument.MaxPosition, local4);  }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitGt(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateGt]) {
                if ( IsLiteral((local1)) ) {
                    if ( IsLiteral((local2)) ) {
                        if (AllowReplace(XmlILOptimization.EliminateGt, local0)) {
                            return Replace(XmlILOptimization.EliminateGt, local0,  FoldComparison(QilNodeType.Gt, local1, local2) );
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeGtLiteral]) {
                if ( IsLiteral((local1)) ) {
                    if (!( IsLiteral((local2)) )) {
                        if (AllowReplace(XmlILOptimization.NormalizeGtLiteral, local0)) {
                            return Replace(XmlILOptimization.NormalizeGtLiteral, local0, VisitLt(f.Lt(local2, local1)));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeXsltConvertGt]) {
                if (local1.NodeType == QilNodeType.XsltConvert) {
                    QilNode local3 = local1[0];
                    QilNode local4 = local1[1];
                    if (local4.NodeType == QilNodeType.LiteralType) {
                        XmlQueryType local5 = (XmlQueryType)((QilLiteral)local4).Value;
                        if (( IsPrimitiveNumeric( (local3).XmlType ) ) && ( IsPrimitiveNumeric(local5) )) {
                            if (( IsLiteral((local2)) ) && ( CanFoldXsltConvertNonLossy(local2,  (local3).XmlType ) )) {
                                if (AllowReplace(XmlILOptimization.NormalizeXsltConvertGt, local0)) {
                                    return Replace(XmlILOptimization.NormalizeXsltConvertGt, local0, VisitGt(f.Gt(local3,  FoldXsltConvert(local2,  (local3).XmlType ) )));
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeLengthGt]) {
                if (local1.NodeType == QilNodeType.Length) {
                    QilNode local3 = local1[0];
                    if (local2.NodeType == QilNodeType.LiteralInt32) {
                        int local4 = (int)((QilLiteral)local2).Value;
                        if (local4 == 0) {
                            if (AllowReplace(XmlILOptimization.NormalizeLengthGt, local0)) {
                                return Replace(XmlILOptimization.NormalizeLengthGt, local0, VisitNot(f.Not(VisitIsEmpty(f.IsEmpty(local3)))));
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateMaxLengthGt]) {
                if (local1.NodeType == QilNodeType.Length) {
                    if (local2.NodeType == QilNodeType.LiteralInt32) {
                        int local4 = (int)((QilLiteral)local2).Value;
                        if (AllowReplace(XmlILOptimization.AnnotateMaxLengthGt, local0)) {
                             OptimizerPatterns.Write((QilNode) (local1)).AddPattern(OptimizerPatternName.MaxPosition);  OptimizerPatterns.Write((QilNode) (local1)).AddArgument(OptimizerPatternArgument.MaxPosition, local4);  }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitGe(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateGe]) {
                if ( IsLiteral((local1)) ) {
                    if ( IsLiteral((local2)) ) {
                        if (AllowReplace(XmlILOptimization.EliminateGe, local0)) {
                            return Replace(XmlILOptimization.EliminateGe, local0,  FoldComparison(QilNodeType.Ge, local1, local2) );
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeGeLiteral]) {
                if ( IsLiteral((local1)) ) {
                    if (!( IsLiteral((local2)) )) {
                        if (AllowReplace(XmlILOptimization.NormalizeGeLiteral, local0)) {
                            return Replace(XmlILOptimization.NormalizeGeLiteral, local0, VisitLe(f.Le(local2, local1)));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeXsltConvertGe]) {
                if (local1.NodeType == QilNodeType.XsltConvert) {
                    QilNode local3 = local1[0];
                    QilNode local4 = local1[1];
                    if (local4.NodeType == QilNodeType.LiteralType) {
                        XmlQueryType local5 = (XmlQueryType)((QilLiteral)local4).Value;
                        if (( IsPrimitiveNumeric( (local3).XmlType ) ) && ( IsPrimitiveNumeric(local5) )) {
                            if (( IsLiteral((local2)) ) && ( CanFoldXsltConvertNonLossy(local2,  (local3).XmlType ) )) {
                                if (AllowReplace(XmlILOptimization.NormalizeXsltConvertGe, local0)) {
                                    return Replace(XmlILOptimization.NormalizeXsltConvertGe, local0, VisitGe(f.Ge(local3,  FoldXsltConvert(local2,  (local3).XmlType ) )));
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateMaxLengthGe]) {
                if (local1.NodeType == QilNodeType.Length) {
                    if (local2.NodeType == QilNodeType.LiteralInt32) {
                        int local4 = (int)((QilLiteral)local2).Value;
                        if (AllowReplace(XmlILOptimization.AnnotateMaxLengthGe, local0)) {
                             OptimizerPatterns.Write((QilNode) (local1)).AddPattern(OptimizerPatternName.MaxPosition);  OptimizerPatterns.Write((QilNode) (local1)).AddArgument(OptimizerPatternArgument.MaxPosition, local4);  }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitLt(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateLt]) {
                if ( IsLiteral((local1)) ) {
                    if ( IsLiteral((local2)) ) {
                        if (AllowReplace(XmlILOptimization.EliminateLt, local0)) {
                            return Replace(XmlILOptimization.EliminateLt, local0,  FoldComparison(QilNodeType.Lt, local1, local2) );
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeLtLiteral]) {
                if ( IsLiteral((local1)) ) {
                    if (!( IsLiteral((local2)) )) {
                        if (AllowReplace(XmlILOptimization.NormalizeLtLiteral, local0)) {
                            return Replace(XmlILOptimization.NormalizeLtLiteral, local0, VisitGt(f.Gt(local2, local1)));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeXsltConvertLt]) {
                if (local1.NodeType == QilNodeType.XsltConvert) {
                    QilNode local3 = local1[0];
                    QilNode local4 = local1[1];
                    if (local4.NodeType == QilNodeType.LiteralType) {
                        XmlQueryType local5 = (XmlQueryType)((QilLiteral)local4).Value;
                        if (( IsPrimitiveNumeric( (local3).XmlType ) ) && ( IsPrimitiveNumeric(local5) )) {
                            if (( IsLiteral((local2)) ) && ( CanFoldXsltConvertNonLossy(local2,  (local3).XmlType ) )) {
                                if (AllowReplace(XmlILOptimization.NormalizeXsltConvertLt, local0)) {
                                    return Replace(XmlILOptimization.NormalizeXsltConvertLt, local0, VisitLt(f.Lt(local3,  FoldXsltConvert(local2,  (local3).XmlType ) )));
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateMaxLengthLt]) {
                if (local1.NodeType == QilNodeType.Length) {
                    if (local2.NodeType == QilNodeType.LiteralInt32) {
                        int local4 = (int)((QilLiteral)local2).Value;
                        if (AllowReplace(XmlILOptimization.AnnotateMaxLengthLt, local0)) {
                             OptimizerPatterns.Write((QilNode) (local1)).AddPattern(OptimizerPatternName.MaxPosition);  OptimizerPatterns.Write((QilNode) (local1)).AddArgument(OptimizerPatternArgument.MaxPosition, local4);  }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitLe(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateLe]) {
                if ( IsLiteral((local1)) ) {
                    if ( IsLiteral((local2)) ) {
                        if (AllowReplace(XmlILOptimization.EliminateLe, local0)) {
                            return Replace(XmlILOptimization.EliminateLe, local0,  FoldComparison(QilNodeType.Le, local1, local2) );
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeLeLiteral]) {
                if ( IsLiteral((local1)) ) {
                    if (!( IsLiteral((local2)) )) {
                        if (AllowReplace(XmlILOptimization.NormalizeLeLiteral, local0)) {
                            return Replace(XmlILOptimization.NormalizeLeLiteral, local0, VisitGe(f.Ge(local2, local1)));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeXsltConvertLe]) {
                if (local1.NodeType == QilNodeType.XsltConvert) {
                    QilNode local3 = local1[0];
                    QilNode local4 = local1[1];
                    if (local4.NodeType == QilNodeType.LiteralType) {
                        XmlQueryType local5 = (XmlQueryType)((QilLiteral)local4).Value;
                        if (( IsPrimitiveNumeric( (local3).XmlType ) ) && ( IsPrimitiveNumeric(local5) )) {
                            if (( IsLiteral((local2)) ) && ( CanFoldXsltConvertNonLossy(local2,  (local3).XmlType ) )) {
                                if (AllowReplace(XmlILOptimization.NormalizeXsltConvertLe, local0)) {
                                    return Replace(XmlILOptimization.NormalizeXsltConvertLe, local0, VisitLe(f.Le(local3,  FoldXsltConvert(local2,  (local3).XmlType ) )));
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateMaxLengthLe]) {
                if (local1.NodeType == QilNodeType.Length) {
                    if (local2.NodeType == QilNodeType.LiteralInt32) {
                        int local4 = (int)((QilLiteral)local2).Value;
                        if (AllowReplace(XmlILOptimization.AnnotateMaxLengthLe, local0)) {
                             OptimizerPatterns.Write((QilNode) (local1)).AddPattern(OptimizerPatternName.MaxPosition);  OptimizerPatterns.Write((QilNode) (local1)).AddArgument(OptimizerPatternArgument.MaxPosition, local4);  }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        #endregion // value comparison operators
        
        #region node comparison operators
        protected override QilNode VisitIs(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateIs]) {
                if (local2 == local1) {
                    if (AllowReplace(XmlILOptimization.EliminateIs, local0)) {
                        return Replace(XmlILOptimization.EliminateIs, local0, VisitTrue(f.True()));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitAfter(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateAfter]) {
                if (local2 == local1) {
                    if (AllowReplace(XmlILOptimization.EliminateAfter, local0)) {
                        return Replace(XmlILOptimization.EliminateAfter, local0, VisitFalse(f.False()));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitBefore(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateBefore]) {
                if (local2 == local1) {
                    if (AllowReplace(XmlILOptimization.EliminateBefore, local0)) {
                        return Replace(XmlILOptimization.EliminateBefore, local0, VisitFalse(f.False()));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        #endregion // node comparison operators
        
        #region loops
        protected override QilNode VisitLoop(QilLoop local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop( (QilNode) (local1)[0] )));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateIterator]) {
                if (local1.NodeType == QilNodeType.For) {
                    QilNode local3 = local1[0];
                    if (local3.NodeType == QilNodeType.For) {
                        if ( !OptimizerPatterns.Read(local1).MatchesPattern(OptimizerPatternName.IsPositional) ) {
                            if (AllowReplace(XmlILOptimization.EliminateIterator, local0)) {
                                return Replace(XmlILOptimization.EliminateIterator, local0,  Subs(local2, local1, local3) );
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateLoop]) {
                if (local1.NodeType == QilNodeType.For) {
                    QilNode local3 = local1[0];
                    if (local3.NodeType == QilNodeType.Sequence) {
                        if ( (local3).Count == (0) ) {
                            if (AllowReplace(XmlILOptimization.EliminateLoop, local0)) {
                                return Replace(XmlILOptimization.EliminateLoop, local0, VisitSequence(f.Sequence()));
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateLoop]) {
                if ( !OptimizerPatterns.Read(local1).MatchesPattern(OptimizerPatternName.MaybeSideEffects) ) {
                    if (local2.NodeType == QilNodeType.Sequence) {
                        if ( (local2).Count == (0) ) {
                            if (AllowReplace(XmlILOptimization.EliminateLoop, local0)) {
                                return Replace(XmlILOptimization.EliminateLoop, local0, VisitSequence(f.Sequence()));
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateLoop]) {
                if (local2 == local1) {
                    if (AllowReplace(XmlILOptimization.EliminateLoop, local0)) {
                        return Replace(XmlILOptimization.EliminateLoop, local0,  (QilNode) (local1)[0] );
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeLoopText]) {
                if (local1.NodeType == QilNodeType.For) {
                    QilNode local3 = local1[0];
                    if ( ( (local3).XmlType ).IsSingleton ) {
                        if (local2.NodeType == QilNodeType.TextCtor) {
                            QilNode local4 = local2[0];
                            if (AllowReplace(XmlILOptimization.NormalizeLoopText, local0)) {
                                return Replace(XmlILOptimization.NormalizeLoopText, local0, VisitTextCtor(f.TextCtor(VisitLoop(f.Loop(local1, local4)))));
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateIteratorUsedAtMostOnce]) {
                if ((( (local1).NodeType == QilNodeType.Let ) || ( ( ( (QilNode) (local1)[0] ).XmlType ).IsSingleton )) && ( !OptimizerPatterns.Read(local1).MatchesPattern(OptimizerPatternName.MaybeSideEffects) )) {
                    if (( nodeCounter.Count(local2, local1) <= 1 ) && ( !OptimizerPatterns.Read(local2).MatchesPattern(OptimizerPatternName.MaybeSideEffects) )) {
                        if (AllowReplace(XmlILOptimization.EliminateIteratorUsedAtMostOnce, local0)) {
                            return Replace(XmlILOptimization.EliminateIteratorUsedAtMostOnce, local0,  Subs(local2, local1,  (QilNode) (local1)[0] ) );
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeLoopConditional]) {
                if (local2.NodeType == QilNodeType.Conditional) {
                    QilNode local3 = local2[0];
                    QilNode local4 = local2[1];
                    QilNode local5 = local2[2];
                    if (local4.NodeType == QilNodeType.Sequence) {
                        if ( (local4).Count == (0) ) {
                            if (local5 == local1) {
                                if (AllowReplace(XmlILOptimization.NormalizeLoopConditional, local0)) {
                                    return Replace(XmlILOptimization.NormalizeLoopConditional, local0, VisitFilter(f.Filter(local1, VisitNot(f.Not(local3)))));
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeLoopConditional]) {
                if (local2.NodeType == QilNodeType.Conditional) {
                    QilNode local3 = local2[0];
                    QilNode local4 = local2[1];
                    QilNode local5 = local2[2];
                    if (local4 == local1) {
                        if (local5.NodeType == QilNodeType.Sequence) {
                            if ( (local5).Count == (0) ) {
                                if (AllowReplace(XmlILOptimization.NormalizeLoopConditional, local0)) {
                                    return Replace(XmlILOptimization.NormalizeLoopConditional, local0, VisitFilter(f.Filter(local1, local3)));
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeLoopConditional]) {
                if (local1.NodeType == QilNodeType.For) {
                    if (local2.NodeType == QilNodeType.Conditional) {
                        QilNode local4 = local2[0];
                        QilNode local5 = local2[1];
                        QilNode local6 = local2[2];
                        if (local5.NodeType == QilNodeType.Sequence) {
                            if ( (local5).Count == (0) ) {
                                if ( NonPositional(local6, local1) ) {
                                    if (AllowReplace(XmlILOptimization.NormalizeLoopConditional, local0)) {
                                        QilNode local7 = VisitFor(f.For(VisitFilter(f.Filter(local1, VisitNot(f.Not(local4))))));
                                        return Replace(XmlILOptimization.NormalizeLoopConditional, local0, VisitLoop(f.Loop(local7,  Subs(local6, local1, local7) )));
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeLoopConditional]) {
                if (local1.NodeType == QilNodeType.For) {
                    if (local2.NodeType == QilNodeType.Conditional) {
                        QilNode local4 = local2[0];
                        QilNode local5 = local2[1];
                        QilNode local6 = local2[2];
                        if ( NonPositional(local5, local1) ) {
                            if (local6.NodeType == QilNodeType.Sequence) {
                                if ( (local6).Count == (0) ) {
                                    if (AllowReplace(XmlILOptimization.NormalizeLoopConditional, local0)) {
                                        QilNode local7 = VisitFor(f.For(VisitFilter(f.Filter(local1, local4))));
                                        return Replace(XmlILOptimization.NormalizeLoopConditional, local0, VisitLoop(f.Loop(local7,  Subs(local5, local1, local7) )));
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeLoopLoop]) {
                if (local2.NodeType == QilNodeType.Loop) {
                    QilNode local3 = local2[0];
                    QilNode local5 = local2[1];
                    if (local3.NodeType == QilNodeType.For) {
                        QilNode local4 = local3[0];
                        if ((!(DependsOn(local5,local1))) && ( NonPositional(local5, local3) )) {
                            if (AllowReplace(XmlILOptimization.NormalizeLoopLoop, local0)) {
                                QilNode local6 = VisitFor(f.For(VisitLoop(f.Loop(local1, local4))));
                                return Replace(XmlILOptimization.NormalizeLoopLoop, local0, VisitLoop(f.Loop(local6,  Subs(local5, local3, local6) )));
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateSingletonLoop]) {
                if (local1.NodeType == QilNodeType.For) {
                    QilNode local3 = local1[0];
                    if ( !( (local3).XmlType ).MaybeMany ) {
                        if (AllowReplace(XmlILOptimization.AnnotateSingletonLoop, local0)) {
                             OptimizerPatterns.Inherit((QilNode) (local2), (QilNode) (local0), OptimizerPatternName.IsDocOrderDistinct);  OptimizerPatterns.Inherit((QilNode) (local2), (QilNode) (local0), OptimizerPatternName.SameDepth);  }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateRootLoop]) {
                if ( IsStepPattern(local2, QilNodeType.Root) ) {
                    if (AllowReplace(XmlILOptimization.AnnotateRootLoop, local0)) {
                         OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.SameDepth);  }
                }
            }
            if (this[XmlILOptimization.AnnotateContentLoop]) {
                if (local1.NodeType == QilNodeType.For) {
                    QilNode local3 = local1[0];
                    if ( OptimizerPatterns.Read((QilNode) (local3)).MatchesPattern(OptimizerPatternName.SameDepth) ) {
                        if ((( IsStepPattern(local2, QilNodeType.Content) ) || ( IsStepPattern(local2, QilNodeType.Union) )) && ( (local1) == ( OptimizerPatterns.Read((QilNode) (local2)).GetArgument(OptimizerPatternArgument.StepInput) ) )) {
                            if (AllowReplace(XmlILOptimization.AnnotateContentLoop, local0)) {
                                 OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.SameDepth);  OptimizerPatterns.Inherit((QilNode) (local3), (QilNode) (local0), OptimizerPatternName.IsDocOrderDistinct);  }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateAttrNmspLoop]) {
                if (local1.NodeType == QilNodeType.For) {
                    QilNode local3 = local1[0];
                    if (((( IsStepPattern(local2, QilNodeType.Attribute) ) || ( IsStepPattern(local2, QilNodeType.XPathNamespace) )) || ( OptimizerPatterns.Read((QilNode) (local2)).MatchesPattern(OptimizerPatternName.FilterAttributeKind) )) && ( (local1) == ( OptimizerPatterns.Read((QilNode) (local2)).GetArgument(OptimizerPatternArgument.StepInput) ) )) {
                        if (AllowReplace(XmlILOptimization.AnnotateAttrNmspLoop, local0)) {
                             OptimizerPatterns.Inherit((QilNode) (local3), (QilNode) (local0), OptimizerPatternName.SameDepth);  OptimizerPatterns.Inherit((QilNode) (local3), (QilNode) (local0), OptimizerPatternName.IsDocOrderDistinct);  }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateDescendantLoop]) {
                if (local1.NodeType == QilNodeType.For) {
                    QilNode local3 = local1[0];
                    if ( OptimizerPatterns.Read((QilNode) (local3)).MatchesPattern(OptimizerPatternName.SameDepth) ) {
                        if ((( IsStepPattern(local2, QilNodeType.Descendant) ) || ( IsStepPattern(local2, QilNodeType.DescendantOrSelf) )) && ( (local1) == ( OptimizerPatterns.Read((QilNode) (local2)).GetArgument(OptimizerPatternArgument.StepInput) ) )) {
                            if (AllowReplace(XmlILOptimization.AnnotateDescendantLoop, local0)) {
                                 OptimizerPatterns.Inherit((QilNode) (local3), (QilNode) (local0), OptimizerPatternName.IsDocOrderDistinct);  }
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitFilter(QilLoop local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitLoop(f.Loop(local1, local2)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateFilter]) {
                if ( !OptimizerPatterns.Read(local1).MatchesPattern(OptimizerPatternName.MaybeSideEffects) ) {
                    if (local2.NodeType == QilNodeType.False) {
                        if (AllowReplace(XmlILOptimization.EliminateFilter, local0)) {
                            return Replace(XmlILOptimization.EliminateFilter, local0, VisitSequence(f.Sequence()));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateFilter]) {
                if (local2.NodeType == QilNodeType.True) {
                    if (AllowReplace(XmlILOptimization.EliminateFilter, local0)) {
                        return Replace(XmlILOptimization.EliminateFilter, local0,  (QilNode) (local1)[0] );
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeAttribute]) {
                if (local1.NodeType == QilNodeType.For) {
                    QilNode local3 = local1[0];
                    if (local3.NodeType == QilNodeType.Content) {
                        QilNode local4 = local3[0];
                        if (local2.NodeType == QilNodeType.And) {
                            QilNode local5 = local2[0];
                            QilNode local9 = local2[1];
                            if (local5.NodeType == QilNodeType.IsType) {
                                QilNode local6 = local5[0];
                                QilNode local7 = local5[1];
                                if (local6 == local1) {
                                    if (local7.NodeType == QilNodeType.LiteralType) {
                                        XmlQueryType local8 = (XmlQueryType)((QilLiteral)local7).Value;
                                        if ( (local8) == ( XmlQueryTypeFactory.Attribute ) ) {
                                            if (local9.NodeType == QilNodeType.Eq) {
                                                QilNode local10 = local9[0];
                                                QilNode local12 = local9[1];
                                                if (local10.NodeType == QilNodeType.NameOf) {
                                                    QilNode local11 = local10[0];
                                                    if (local11 == local1) {
                                                        if (local12.NodeType == QilNodeType.LiteralQName) {
                                                            if (AllowReplace(XmlILOptimization.NormalizeAttribute, local0)) {
                                                                return Replace(XmlILOptimization.NormalizeAttribute, local0, VisitAttribute(f.Attribute(local4, local12)));
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.CommuteFilterLoop]) {
                if (local1.NodeType == QilNodeType.For) {
                    QilNode local3 = local1[0];
                    if (local3.NodeType == QilNodeType.Loop) {
                        QilNode local4 = local3[0];
                        QilNode local5 = local3[1];
                        if ( NonPositional(local2, local1) ) {
                            if (AllowReplace(XmlILOptimization.CommuteFilterLoop, local0)) {
                                QilNode local6 = VisitFor(f.For(local5));
                                return Replace(XmlILOptimization.CommuteFilterLoop, local0, VisitLoop(f.Loop(local4, VisitFilter(f.Filter(local6,  Subs(local2, local1, local6) )))));
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.NormalizeLoopInvariant]) {
                if ( !OptimizerPatterns.Read(local1).MatchesPattern(OptimizerPatternName.MaybeSideEffects) ) {
                    if ((!(DependsOn(local2,local1))) && ( !OptimizerPatterns.Read(local2).MatchesPattern(OptimizerPatternName.MaybeSideEffects) )) {
                        if (AllowReplace(XmlILOptimization.NormalizeLoopInvariant, local0)) {
                            return Replace(XmlILOptimization.NormalizeLoopInvariant, local0, VisitConditional(f.Conditional(local2,  (QilNode) (local1)[0] , VisitSequence(f.Sequence()))));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateMaxPositionEq]) {
                if (local2.NodeType == QilNodeType.Eq) {
                    QilNode local3 = local2[0];
                    QilNode local5 = local2[1];
                    if (local3.NodeType == QilNodeType.PositionOf) {
                        QilNode local4 = local3[0];
                        if (local4 == local1) {
                            if (local5.NodeType == QilNodeType.LiteralInt32) {
                                int local6 = (int)((QilLiteral)local5).Value;
                                if (AllowReplace(XmlILOptimization.AnnotateMaxPositionEq, local0)) {
                                     OptimizerPatterns.Write((QilNode) (local1)).AddPattern(OptimizerPatternName.MaxPosition);  OptimizerPatterns.Write((QilNode) (local1)).AddArgument(OptimizerPatternArgument.MaxPosition, local6);  }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateMaxPositionLe]) {
                if (local2.NodeType == QilNodeType.Le) {
                    QilNode local3 = local2[0];
                    QilNode local5 = local2[1];
                    if (local3.NodeType == QilNodeType.PositionOf) {
                        QilNode local4 = local3[0];
                        if (local4 == local1) {
                            if (local5.NodeType == QilNodeType.LiteralInt32) {
                                int local6 = (int)((QilLiteral)local5).Value;
                                if (AllowReplace(XmlILOptimization.AnnotateMaxPositionLe, local0)) {
                                     OptimizerPatterns.Write((QilNode) (local1)).AddPattern(OptimizerPatternName.MaxPosition);  OptimizerPatterns.Write((QilNode) (local1)).AddArgument(OptimizerPatternArgument.MaxPosition, local6);  }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateMaxPositionLt]) {
                if (local2.NodeType == QilNodeType.Lt) {
                    QilNode local3 = local2[0];
                    QilNode local5 = local2[1];
                    if (local3.NodeType == QilNodeType.PositionOf) {
                        QilNode local4 = local3[0];
                        if (local4 == local1) {
                            if (local5.NodeType == QilNodeType.LiteralInt32) {
                                int local6 = (int)((QilLiteral)local5).Value;
                                if (AllowReplace(XmlILOptimization.AnnotateMaxPositionLt, local0)) {
                                     OptimizerPatterns.Write((QilNode) (local1)).AddPattern(OptimizerPatternName.MaxPosition);  OptimizerPatterns.Write((QilNode) (local1)).AddArgument(OptimizerPatternArgument.MaxPosition,  local6 - 1 );  }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateFilter]) {
                if (local1.NodeType == QilNodeType.For) {
                    QilNode local3 = local1[0];
                    if (AllowReplace(XmlILOptimization.AnnotateFilter, local0)) {
                         OptimizerPatterns.Inherit((QilNode) (local3), (QilNode) (local0), OptimizerPatternName.Step);  OptimizerPatterns.Inherit((QilNode) (local3), (QilNode) (local0), OptimizerPatternName.IsDocOrderDistinct);  OptimizerPatterns.Inherit((QilNode) (local3), (QilNode) (local0), OptimizerPatternName.SameDepth);  }
                }
            }
            if (this[XmlILOptimization.AnnotateFilterElements]) {
                if (local1.NodeType == QilNodeType.For) {
                    QilNode local3 = local1[0];
                    if ( OptimizerPatterns.Read((QilNode) (local3)).MatchesPattern(OptimizerPatternName.Axis) ) {
                        if (local2.NodeType == QilNodeType.And) {
                            QilNode local4 = local2[0];
                            QilNode local8 = local2[1];
                            if (local4.NodeType == QilNodeType.IsType) {
                                QilNode local5 = local4[0];
                                QilNode local6 = local4[1];
                                if (local5 == local1) {
                                    if (local6.NodeType == QilNodeType.LiteralType) {
                                        XmlQueryType local7 = (XmlQueryType)((QilLiteral)local6).Value;
                                        if ( (local7) == ( XmlQueryTypeFactory.Element ) ) {
                                            if (local8.NodeType == QilNodeType.Eq) {
                                                QilNode local9 = local8[0];
                                                QilNode local11 = local8[1];
                                                if (local9.NodeType == QilNodeType.NameOf) {
                                                    QilNode local10 = local9[0];
                                                    if (local10 == local1) {
                                                        if (local11.NodeType == QilNodeType.LiteralQName) {
                                                            if (AllowReplace(XmlILOptimization.AnnotateFilterElements, local0)) {
                                                                 OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.FilterElements);  OptimizerPatterns.Write((QilNode) (local0)).AddArgument(OptimizerPatternArgument.ElementQName, local11);  }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateFilterContentKind]) {
                if (local1.NodeType == QilNodeType.For) {
                    QilNode local3 = local1[0];
                    if ( OptimizerPatterns.Read((QilNode) (local3)).MatchesPattern(OptimizerPatternName.Axis) ) {
                        if (local2.NodeType == QilNodeType.IsType) {
                            QilNode local4 = local2[0];
                            QilNode local5 = local2[1];
                            if (local4 == local1) {
                                if (local5.NodeType == QilNodeType.LiteralType) {
                                    XmlQueryType local6 = (XmlQueryType)((QilLiteral)local5).Value;
                                    if ( MatchesContentTest(local6) ) {
                                        if (AllowReplace(XmlILOptimization.AnnotateFilterContentKind, local0)) {
                                             OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.FilterContentKind);  OptimizerPatterns.Write((QilNode) (local0)).AddArgument(OptimizerPatternArgument.KindTestType, local6);  }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateFilterAttributeKind]) {
                if (local1.NodeType == QilNodeType.For) {
                    QilNode local3 = local1[0];
                    if (local3.NodeType == QilNodeType.Content) {
                        if (local2.NodeType == QilNodeType.IsType) {
                            QilNode local5 = local2[0];
                            QilNode local6 = local2[1];
                            if (local5 == local1) {
                                if (local6.NodeType == QilNodeType.LiteralType) {
                                    XmlQueryType local7 = (XmlQueryType)((QilLiteral)local6).Value;
                                    if ( (local7) == ( XmlQueryTypeFactory.Attribute ) ) {
                                        if (AllowReplace(XmlILOptimization.AnnotateFilterAttributeKind, local0)) {
                                             OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.FilterAttributeKind);  }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        #endregion // loops
        
        #region sorting
        protected override QilNode VisitSort(QilLoop local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop( (QilNode) (local1)[0] )));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateSort]) {
                if (local1.NodeType == QilNodeType.For) {
                    QilNode local3 = local1[0];
                    if ( ( (local3).XmlType ).IsSingleton ) {
                        if (AllowReplace(XmlILOptimization.EliminateSort, local0)) {
                            return Replace(XmlILOptimization.EliminateSort, local0, VisitNop(f.Nop(local3)));
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitSortKey(QilSortKey local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.NormalizeSortXsltConvert]) {
                if (local1.NodeType == QilNodeType.XsltConvert) {
                    QilNode local3 = local1[0];
                    QilNode local4 = local1[1];
                    if (local4.NodeType == QilNodeType.LiteralType) {
                        XmlQueryType local5 = (XmlQueryType)((QilLiteral)local4).Value;
                        if (( ( (local3).XmlType ) == ( XmlQueryTypeFactory.IntX ) ) && ( (local5) == ( XmlQueryTypeFactory.DoubleX ) )) {
                            if (AllowReplace(XmlILOptimization.NormalizeSortXsltConvert, local0)) {
                                return Replace(XmlILOptimization.NormalizeSortXsltConvert, local0, VisitSortKey(f.SortKey(local3, local2)));
                            }
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitDocOrderDistinct(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateDod]) {
                if ( IsDocOrderDistinct(local1) ) {
                    if (AllowReplace(XmlILOptimization.EliminateDod, local0)) {
                        return Replace(XmlILOptimization.EliminateDod, local0, local1);
                    }
                }
            }
            if (this[XmlILOptimization.FoldNamedDescendants]) {
                if (local1.NodeType == QilNodeType.Loop) {
                    QilNode local2 = local1[0];
                    QilNode local7 = local1[1];
                    if (local2.NodeType == QilNodeType.For) {
                        QilNode local3 = local2[0];
                        if (local3.NodeType == QilNodeType.Loop) {
                            QilNode local4 = local3[0];
                            QilNode local5 = local3[1];
                            if (local5.NodeType == QilNodeType.DescendantOrSelf) {
                                QilNode local6 = local5[0];
                                if (local7.NodeType == QilNodeType.Filter) {
                                    QilNode local8 = local7[0];
                                    QilNode local9 = local7[1];
                                    if ((( OptimizerPatterns.Read((QilNode) (local7)).MatchesPattern(OptimizerPatternName.FilterElements) ) || ( OptimizerPatterns.Read((QilNode) (local7)).MatchesPattern(OptimizerPatternName.FilterContentKind) )) && ( IsStepPattern(local7, QilNodeType.Content) )) {
                                        if (AllowReplace(XmlILOptimization.FoldNamedDescendants, local0)) {
                                            QilNode local10 = VisitFor(f.For(VisitDescendant(f.Descendant(local6))));
                                            return Replace(XmlILOptimization.FoldNamedDescendants, local0, VisitDocOrderDistinct(f.DocOrderDistinct(VisitLoop(f.Loop(local4, VisitFilter(f.Filter(local10,  Subs(local9, local8, local10) )))))));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.FoldNamedDescendants]) {
                if (local1.NodeType == QilNodeType.Loop) {
                    QilNode local2 = local1[0];
                    QilNode local5 = local1[1];
                    if (local2.NodeType == QilNodeType.For) {
                        QilNode local3 = local2[0];
                        if (local3.NodeType == QilNodeType.DescendantOrSelf) {
                            QilNode local4 = local3[0];
                            if (local5.NodeType == QilNodeType.Filter) {
                                QilNode local6 = local5[0];
                                QilNode local7 = local5[1];
                                if ((( OptimizerPatterns.Read((QilNode) (local5)).MatchesPattern(OptimizerPatternName.FilterElements) ) || ( OptimizerPatterns.Read((QilNode) (local5)).MatchesPattern(OptimizerPatternName.FilterContentKind) )) && ( IsStepPattern(local5, QilNodeType.Content) )) {
                                    if (AllowReplace(XmlILOptimization.FoldNamedDescendants, local0)) {
                                        QilNode local8 = VisitFor(f.For(VisitDescendant(f.Descendant(local4))));
                                        return Replace(XmlILOptimization.FoldNamedDescendants, local0, VisitFilter(f.Filter(local8,  Subs(local7, local6, local8) )));
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.CommuteDodFilter]) {
                if (local1.NodeType == QilNodeType.Filter) {
                    QilNode local2 = local1[0];
                    QilNode local4 = local1[1];
                    if (local2.NodeType == QilNodeType.For) {
                        QilNode local3 = local2[0];
                        if ( !OptimizerPatterns.Read(local2).MatchesPattern(OptimizerPatternName.IsPositional) ) {
                            if (((!( OptimizerPatterns.Read((QilNode) (local1)).MatchesPattern(OptimizerPatternName.FilterElements) )) && (!( OptimizerPatterns.Read((QilNode) (local1)).MatchesPattern(OptimizerPatternName.FilterContentKind) ))) && (!( OptimizerPatterns.Read((QilNode) (local1)).MatchesPattern(OptimizerPatternName.FilterAttributeKind) ))) {
                                if (AllowReplace(XmlILOptimization.CommuteDodFilter, local0)) {
                                    QilNode local5 = VisitFor(f.For(VisitDocOrderDistinct(f.DocOrderDistinct(local3))));
                                    return Replace(XmlILOptimization.CommuteDodFilter, local0, VisitFilter(f.Filter(local5,  Subs(local4, local2, local5) )));
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.CommuteDodFilter]) {
                if (local1.NodeType == QilNodeType.Loop) {
                    QilNode local2 = local1[0];
                    QilNode local3 = local1[1];
                    if (local3.NodeType == QilNodeType.Filter) {
                        QilNode local4 = local3[0];
                        QilNode local6 = local3[1];
                        if (local4.NodeType == QilNodeType.For) {
                            QilNode local5 = local4[0];
                            if ( !OptimizerPatterns.Read(local4).MatchesPattern(OptimizerPatternName.IsPositional) ) {
                                if (!(DependsOn(local6,local2))) {
                                    if (((!( OptimizerPatterns.Read((QilNode) (local3)).MatchesPattern(OptimizerPatternName.FilterElements) )) && (!( OptimizerPatterns.Read((QilNode) (local3)).MatchesPattern(OptimizerPatternName.FilterContentKind) ))) && (!( OptimizerPatterns.Read((QilNode) (local3)).MatchesPattern(OptimizerPatternName.FilterAttributeKind) ))) {
                                        if (AllowReplace(XmlILOptimization.CommuteDodFilter, local0)) {
                                            QilNode local7 = VisitFor(f.For(VisitDocOrderDistinct(f.DocOrderDistinct(VisitLoop(f.Loop(local2, local5))))));
                                            return Replace(XmlILOptimization.CommuteDodFilter, local0, VisitFilter(f.Filter(local7,  Subs(local6, local4, local7) )));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.IntroduceDod]) {
                if (local1.NodeType == QilNodeType.Loop) {
                    QilNode local2 = local1[0];
                    QilNode local4 = local1[1];
                    if (local2.NodeType == QilNodeType.For) {
                        QilNode local3 = local2[0];
                        if (!( IsDocOrderDistinct(local3) )) {
                            if (( !OptimizerPatterns.Read(local2).MatchesPattern(OptimizerPatternName.IsPositional) ) && (  (local3).XmlType .IsSubtypeOf( XmlQueryTypeFactory.NodeNotRtfS ) )) {
                                if (((!( OptimizerPatterns.Read((QilNode) (local1)).MatchesPattern(OptimizerPatternName.FilterElements) )) && (!( OptimizerPatterns.Read((QilNode) (local1)).MatchesPattern(OptimizerPatternName.FilterContentKind) ))) && (!( OptimizerPatterns.Read((QilNode) (local1)).MatchesPattern(OptimizerPatternName.FilterAttributeKind) ))) {
                                    if (AllowReplace(XmlILOptimization.IntroduceDod, local0)) {
                                        QilNode local5 = VisitFor(f.For(VisitDocOrderDistinct(f.DocOrderDistinct(local3))));
                                        return Replace(XmlILOptimization.IntroduceDod, local0, VisitDocOrderDistinct(f.DocOrderDistinct(VisitLoop(f.Loop(local5,  Subs(local4, local2, local5) )))));
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.IntroducePrecedingDod]) {
                if (local1.NodeType == QilNodeType.Loop) {
                    QilNode local2 = local1[0];
                    QilNode local3 = local1[1];
                    if ((!( IsDocOrderDistinct(local3) )) && ( IsStepPattern(local3, QilNodeType.PrecedingSibling) )) {
                        if (AllowReplace(XmlILOptimization.IntroducePrecedingDod, local0)) {
                            return Replace(XmlILOptimization.IntroducePrecedingDod, local0, VisitDocOrderDistinct(f.DocOrderDistinct(VisitLoop(f.Loop(local2, VisitDocOrderDistinct(f.DocOrderDistinct(local3)))))));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateReturnDod]) {
                if (local1.NodeType == QilNodeType.Loop) {
                    QilNode local2 = local1[0];
                    QilNode local3 = local1[1];
                    if (local3.NodeType == QilNodeType.DocOrderDistinct) {
                        QilNode local4 = local3[0];
                        if (!( IsStepPattern(local4, QilNodeType.PrecedingSibling) )) {
                            if (AllowReplace(XmlILOptimization.EliminateReturnDod, local0)) {
                                return Replace(XmlILOptimization.EliminateReturnDod, local0, VisitDocOrderDistinct(f.DocOrderDistinct(VisitLoop(f.Loop(local2, local4)))));
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateDod]) {
                if (AllowReplace(XmlILOptimization.AnnotateDod, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.IsDocOrderDistinct);  OptimizerPatterns.Inherit((QilNode) (local1), (QilNode) (local0), OptimizerPatternName.SameDepth);  }
            }
            if (this[XmlILOptimization.AnnotateDodReverse]) {
                if ( AllowDodReverse(local1) ) {
                    if (AllowReplace(XmlILOptimization.AnnotateDodReverse, local0)) {
                         OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.DodReverse);  OptimizerPatterns.Write((QilNode) (local0)).AddArgument(OptimizerPatternArgument.DodStep, local1);  }
                }
            }
            if (this[XmlILOptimization.AnnotateJoinAndDod]) {
                if (local1.NodeType == QilNodeType.Loop) {
                    QilNode local2 = local1[0];
                    QilNode local4 = local1[1];
                    if (local2.NodeType == QilNodeType.For) {
                        QilNode local3 = local2[0];
                        if ( IsDocOrderDistinct(local3) ) {
                            if (( AllowJoinAndDod(local4) ) && ( (local2) == ( OptimizerPatterns.Read((QilNode) (local4)).GetArgument(OptimizerPatternArgument.StepInput) ) )) {
                                if (AllowReplace(XmlILOptimization.AnnotateJoinAndDod, local0)) {
                                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.JoinAndDod);  OptimizerPatterns.Write((QilNode) (local0)).AddArgument(OptimizerPatternArgument.DodStep, local4);  }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateDodMerge]) {
                if (local1.NodeType == QilNodeType.Loop) {
                    QilNode local3 = local1[1];
                    if (local3.NodeType == QilNodeType.Invoke) {
                        if ( IsDocOrderDistinct(local3) ) {
                            if (AllowReplace(XmlILOptimization.AnnotateDodMerge, local0)) {
                                 OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.DodMerge);  }
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        #endregion // sorting
        
        #region function definition and invocation
        protected override QilNode VisitFunction(QilFunction local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            QilNode local3 = local0[2];
            XmlQueryType local4 = (XmlQueryType)((QilFunction)local0).XmlType;
            if ((  (local0).XmlType .IsSubtypeOf( XmlQueryTypeFactory.NodeS ) ) && (this[XmlILOptimization.AnnotateIndex1])) {
                if ((( local1.Count == 2 ) && (  ( (QilNode) (local1)[0] ).XmlType .IsSubtypeOf( XmlQueryTypeFactory.Node ) )) && ( ( ( (QilNode) (local1)[1] ).XmlType ) == ( XmlQueryTypeFactory.StringX ) )) {
                    if (local2.NodeType == QilNodeType.Filter) {
                        QilNode local5 = local2[0];
                        QilNode local7 = local2[1];
                        if (local5.NodeType == QilNodeType.For) {
                            QilNode local6 = local5[0];
                            if (local7.NodeType == QilNodeType.Not) {
                                QilNode local8 = local7[0];
                                if (local8.NodeType == QilNodeType.IsEmpty) {
                                    QilNode local9 = local8[0];
                                    if (local9.NodeType == QilNodeType.Filter) {
                                        QilNode local10 = local9[0];
                                        QilNode local12 = local9[1];
                                        if (local10.NodeType == QilNodeType.For) {
                                            QilNode local11 = local10[0];
                                            if (local12.NodeType == QilNodeType.Eq) {
                                                QilNode local13 = local12[0];
                                                QilNode local14 = local12[1];
                                                if (local13 == local10) {
                                                    if (local14.NodeType == QilNodeType.Parameter) {
                                                        if ( (local14) == ( (QilNode) (local1)[1] ) ) {
                                                            if (AllowReplace(XmlILOptimization.AnnotateIndex1, local0)) {
                                                                
    // The following conditions must be true for this pattern to match:
    //   1. The function must have exactly two arguments
    //   2. The type of the first argument must be a subtype of Node
    //   3. The type of the second argument must be String
    //   4. The return type must be a subtype of Node*
    //   5. Every reference to $args[0] (context document) must be wrapped in an (Root *) function
    //   6. $keyParam cannot be used with the $bindingNodes and $bindingKeys expressions

    EqualityIndexVisitor visitor = new EqualityIndexVisitor();
    if (visitor.Scan(local6, local1[0], local14) && visitor.Scan(local11, local1[0], local14)) {
        // All conditions were true, so annotate Filter with the EqualityIndex pattern
        OptimizerPatterns patt = OptimizerPatterns.Write(local2);
        patt.AddPattern(OptimizerPatternName.EqualityIndex);
        patt.AddArgument(OptimizerPatternArgument.IndexedNodes, local5);
        patt.AddArgument(OptimizerPatternArgument.KeyExpression, local11);
    }
}
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateIndex2]) {
                if ((( local1.Count == 2 ) && ( ( ( (QilNode) (local1)[0] ).XmlType ) == ( XmlQueryTypeFactory.Node ) )) && ( ( ( (QilNode) (local1)[1] ).XmlType ) == ( XmlQueryTypeFactory.StringX ) )) {
                    if (local2.NodeType == QilNodeType.Filter) {
                        QilNode local5 = local2[0];
                        QilNode local7 = local2[1];
                        if (local5.NodeType == QilNodeType.For) {
                            QilNode local6 = local5[0];
                            if (local7.NodeType == QilNodeType.Eq) {
                                QilNode local8 = local7[0];
                                QilNode local9 = local7[1];
                                if (local9.NodeType == QilNodeType.Parameter) {
                                    if ( (local9) == ( (QilNode) (local1)[1] ) ) {
                                        if (AllowReplace(XmlILOptimization.AnnotateIndex2, local0)) {
                                            
    // Same as EqualityIndex1, except that each nodes has at most one key value

    EqualityIndexVisitor visitor = new EqualityIndexVisitor();
    if (visitor.Scan(local6, local1[0], local9) && visitor.Scan(local8, local1[0], local9)) {
        // All conditions were true, so annotate Filter with the EqualityIndex pattern
        OptimizerPatterns patt = OptimizerPatterns.Write(local2);
        patt.AddPattern(OptimizerPatternName.EqualityIndex);
        patt.AddArgument(OptimizerPatternArgument.IndexedNodes, local5);
        patt.AddArgument(OptimizerPatternArgument.KeyExpression, local8);
    }
}
                                    }
                                }
                            }
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitInvoke(QilInvoke local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.NormalizeInvokeEmpty]) {
                if (local1.NodeType == QilNodeType.Function) {
                    QilNode local4 = local1[1];
                    if (local4.NodeType == QilNodeType.Sequence) {
                        if ( (local4).Count == (0) ) {
                            if (AllowReplace(XmlILOptimization.NormalizeInvokeEmpty, local0)) {
                                return Replace(XmlILOptimization.NormalizeInvokeEmpty, local0, VisitSequence(f.Sequence()));
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateTrackCallers]) {
                if (AllowReplace(XmlILOptimization.AnnotateTrackCallers, local0)) {
                     XmlILConstructInfo.Write(local1).CallersInfo.Add(XmlILConstructInfo.Write(local0));  }
            }
            if (this[XmlILOptimization.AnnotateInvoke]) {
                if (local1.NodeType == QilNodeType.Function) {
                    QilNode local4 = local1[1];
                    if (AllowReplace(XmlILOptimization.AnnotateInvoke, local0)) {
                         OptimizerPatterns.Inherit((QilNode) (local4), (QilNode) (local0), OptimizerPatternName.IsDocOrderDistinct);  OptimizerPatterns.Inherit((QilNode) (local4), (QilNode) (local0), OptimizerPatternName.SameDepth);  }
                }
            }
            return NoReplace(local0);
        }
        
        #endregion // function definition and invocation
        
        #region XML navigation
        protected override QilNode VisitContent(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateContent]) {
                if (AllowReplace(XmlILOptimization.AnnotateContent, local0)) {
                     AddStepPattern((QilNode) (local0), (QilNode) (local1));  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.Axis);  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.IsDocOrderDistinct);  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.SameDepth);  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitAttribute(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateAttribute]) {
                if (AllowReplace(XmlILOptimization.AnnotateAttribute, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.Axis);  AddStepPattern((QilNode) (local0), (QilNode) (local1));  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.IsDocOrderDistinct);  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.SameDepth);  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitParent(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateParent]) {
                if (AllowReplace(XmlILOptimization.AnnotateParent, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.Axis);  AddStepPattern((QilNode) (local0), (QilNode) (local1));  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.IsDocOrderDistinct);  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.SameDepth);  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitRoot(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateRoot]) {
                if (AllowReplace(XmlILOptimization.AnnotateRoot, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.Axis);  AddStepPattern((QilNode) (local0), (QilNode) (local1));  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.IsDocOrderDistinct);  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.SameDepth);  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitDescendant(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateDescendant]) {
                if (AllowReplace(XmlILOptimization.AnnotateDescendant, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.Axis);  AddStepPattern((QilNode) (local0), (QilNode) (local1));  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.IsDocOrderDistinct);  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitDescendantOrSelf(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateDescendantSelf]) {
                if (AllowReplace(XmlILOptimization.AnnotateDescendantSelf, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.Axis);  AddStepPattern((QilNode) (local0), (QilNode) (local1));  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.IsDocOrderDistinct);  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitAncestor(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateAncestor]) {
                if (AllowReplace(XmlILOptimization.AnnotateAncestor, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.Axis);  AddStepPattern((QilNode) (local0), (QilNode) (local1));  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitAncestorOrSelf(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateAncestorSelf]) {
                if (AllowReplace(XmlILOptimization.AnnotateAncestorSelf, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.Axis);  AddStepPattern((QilNode) (local0), (QilNode) (local1));  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitPreceding(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotatePreceding]) {
                if (AllowReplace(XmlILOptimization.AnnotatePreceding, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.Axis);  AddStepPattern((QilNode) (local0), (QilNode) (local1));  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitFollowingSibling(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateFollowingSibling]) {
                if (AllowReplace(XmlILOptimization.AnnotateFollowingSibling, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.Axis);  AddStepPattern((QilNode) (local0), (QilNode) (local1));  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.IsDocOrderDistinct);  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.SameDepth);  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitPrecedingSibling(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotatePrecedingSibling]) {
                if (AllowReplace(XmlILOptimization.AnnotatePrecedingSibling, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.Axis);  AddStepPattern((QilNode) (local0), (QilNode) (local1));  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.SameDepth);  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitNodeRange(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateNodeRange]) {
                if (AllowReplace(XmlILOptimization.AnnotateNodeRange, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.Axis);  AddStepPattern((QilNode) (local0), (QilNode) (local1));  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.IsDocOrderDistinct);  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitDeref(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        #endregion // XML navigation
        
        #region XML construction
        protected override QilNode VisitElementCtor(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateConstruction]) {
                if (AllowReplace(XmlILOptimization.AnnotateConstruction, local0)) {
                    
    // The analysis occasionally makes small changes to the content of constructors, which is
    // why the result of Analyze is assigned to $ctor.Right.
    local0.Right = this.elemAnalyzer.Analyze(local0, local2);
}
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitAttributeCtor(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateConstruction]) {
                if (AllowReplace(XmlILOptimization.AnnotateConstruction, local0)) {
                    
    local0.Right = this.contentAnalyzer.Analyze(local0, local2);
}
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitCommentCtor(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateConstruction]) {
                if (AllowReplace(XmlILOptimization.AnnotateConstruction, local0)) {
                    
    local0.Child = this.contentAnalyzer.Analyze(local0, local1);
}
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitPICtor(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateConstruction]) {
                if (AllowReplace(XmlILOptimization.AnnotateConstruction, local0)) {
                    
    local0.Right = this.contentAnalyzer.Analyze(local0, local2);
}
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitTextCtor(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateConstruction]) {
                if (AllowReplace(XmlILOptimization.AnnotateConstruction, local0)) {
                    
    this.contentAnalyzer.Analyze(local0, null);
}
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitRawTextCtor(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateConstruction]) {
                if (AllowReplace(XmlILOptimization.AnnotateConstruction, local0)) {
                    
    this.contentAnalyzer.Analyze(local0, null);
}
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitDocumentCtor(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateConstruction]) {
                if (AllowReplace(XmlILOptimization.AnnotateConstruction, local0)) {
                    
    local0.Child = this.contentAnalyzer.Analyze(local0, local1);
}
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitNamespaceDecl(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (( XmlILConstructInfo.Read(local0).IsNamespaceInScope ) && (this[XmlILOptimization.EliminateNamespaceDecl])) {
                if (AllowReplace(XmlILOptimization.EliminateNamespaceDecl, local0)) {
                    return Replace(XmlILOptimization.EliminateNamespaceDecl, local0, VisitSequence(f.Sequence()));
                }
            }
            if (this[XmlILOptimization.AnnotateConstruction]) {
                if (AllowReplace(XmlILOptimization.AnnotateConstruction, local0)) {
                    
    this.contentAnalyzer.Analyze(local0, null);
}
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitRtfCtor(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateConstruction]) {
                if (AllowReplace(XmlILOptimization.AnnotateConstruction, local0)) {
                    
    local0.Left = this.contentAnalyzer.Analyze(local0, local1);
}
            }
            if (this[XmlILOptimization.AnnotateSingleTextRtf]) {
                if (local1.NodeType == QilNodeType.TextCtor) {
                    QilNode local3 = local1[0];
                    if (AllowReplace(XmlILOptimization.AnnotateSingleTextRtf, local0)) {
                         OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.SingleTextRtf);  OptimizerPatterns.Write((QilNode) (local0)).AddArgument(OptimizerPatternArgument.RtfText, local3); 
    // In this case, Rtf will be pushed onto the stack rather than pushed to the writer
    XmlILConstructInfo.Write(local0).PullFromIteratorFirst = true;
}
                }
            }
            return NoReplace(local0);
        }
        
        #endregion // XML construction
        
        #region Node properties
        protected override QilNode VisitNameOf(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitLocalNameOf(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitNamespaceUriOf(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitPrefixOf(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        #endregion // Node properties
        
        #region Type operators
        protected override QilNode VisitTypeAssert(QilTargetType local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateTypeAssert]) {
                if (local2.NodeType == QilNodeType.LiteralType) {
                    XmlQueryType local3 = (XmlQueryType)((QilLiteral)local2).Value;
                    if (  (local1).XmlType .NeverSubtypeOf(local3) ) {
                        if (AllowReplace(XmlILOptimization.EliminateTypeAssert, local0)) {
                            return Replace(XmlILOptimization.EliminateTypeAssert, local0, VisitError(f.Error(VisitLiteralString(f.LiteralString("")))));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateTypeAssert]) {
                if (local2.NodeType == QilNodeType.LiteralType) {
                    XmlQueryType local3 = (XmlQueryType)((QilLiteral)local2).Value;
                    if (   (local1).XmlType .Prime .NeverSubtypeOf( local3.Prime ) ) {
                        if (AllowReplace(XmlILOptimization.EliminateTypeAssert, local0)) {
                            return Replace(XmlILOptimization.EliminateTypeAssert, local0, VisitConditional(f.Conditional(VisitIsEmpty(f.IsEmpty(local1)), VisitSequence(f.Sequence()), VisitError(f.Error(VisitLiteralString(f.LiteralString("")))))));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateTypeAssertOptional]) {
                if (local2.NodeType == QilNodeType.LiteralType) {
                    XmlQueryType local3 = (XmlQueryType)((QilLiteral)local2).Value;
                    if (  (local1).XmlType .IsSubtypeOf(local3) ) {
                        if (AllowReplace(XmlILOptimization.EliminateTypeAssertOptional, local0)) {
                            return Replace(XmlILOptimization.EliminateTypeAssertOptional, local0, local1);
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitIsType(QilTargetType local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateIsType]) {
                if ( !OptimizerPatterns.Read(local1).MatchesPattern(OptimizerPatternName.MaybeSideEffects) ) {
                    if (local2.NodeType == QilNodeType.LiteralType) {
                        XmlQueryType local3 = (XmlQueryType)((QilLiteral)local2).Value;
                        if (  (local1).XmlType .IsSubtypeOf(local3) ) {
                            if (AllowReplace(XmlILOptimization.EliminateIsType, local0)) {
                                return Replace(XmlILOptimization.EliminateIsType, local0, VisitTrue(f.True()));
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateIsType]) {
                if ( !OptimizerPatterns.Read(local1).MatchesPattern(OptimizerPatternName.MaybeSideEffects) ) {
                    if (local2.NodeType == QilNodeType.LiteralType) {
                        XmlQueryType local3 = (XmlQueryType)((QilLiteral)local2).Value;
                        if (  (local1).XmlType .NeverSubtypeOf(local3) ) {
                            if (AllowReplace(XmlILOptimization.EliminateIsType, local0)) {
                                return Replace(XmlILOptimization.EliminateIsType, local0, VisitFalse(f.False()));
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateIsType]) {
                if (local2.NodeType == QilNodeType.LiteralType) {
                    XmlQueryType local3 = (XmlQueryType)((QilLiteral)local2).Value;
                    if (   (local1).XmlType .Prime .NeverSubtypeOf( local3.Prime ) ) {
                        if (AllowReplace(XmlILOptimization.EliminateIsType, local0)) {
                            return Replace(XmlILOptimization.EliminateIsType, local0, VisitIsEmpty(f.IsEmpty(local1)));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateIsType]) {
                if (!( !OptimizerPatterns.Read(local1).MatchesPattern(OptimizerPatternName.MaybeSideEffects) )) {
                    if (local2.NodeType == QilNodeType.LiteralType) {
                        XmlQueryType local3 = (XmlQueryType)((QilLiteral)local2).Value;
                        if (  (local1).XmlType .IsSubtypeOf(local3) ) {
                            if (AllowReplace(XmlILOptimization.EliminateIsType, local0)) {
                                return Replace(XmlILOptimization.EliminateIsType, local0, VisitLoop(f.Loop(VisitLet(f.Let(local1)), VisitTrue(f.True()))));
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateIsType]) {
                if (!( !OptimizerPatterns.Read(local1).MatchesPattern(OptimizerPatternName.MaybeSideEffects) )) {
                    if (local2.NodeType == QilNodeType.LiteralType) {
                        XmlQueryType local3 = (XmlQueryType)((QilLiteral)local2).Value;
                        if (  (local1).XmlType .NeverSubtypeOf(local3) ) {
                            if (AllowReplace(XmlILOptimization.EliminateIsType, local0)) {
                                return Replace(XmlILOptimization.EliminateIsType, local0, VisitLoop(f.Loop(VisitLet(f.Let(local1)), VisitFalse(f.False()))));
                            }
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitIsEmpty(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateIsEmpty]) {
                if (local1.NodeType == QilNodeType.Sequence) {
                    if ( (local1).Count == (0) ) {
                        if (AllowReplace(XmlILOptimization.EliminateIsEmpty, local0)) {
                            return Replace(XmlILOptimization.EliminateIsEmpty, local0, VisitTrue(f.True()));
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateIsEmpty]) {
                if (( !( (local1).XmlType ).MaybeEmpty ) && ( !OptimizerPatterns.Read(local1).MatchesPattern(OptimizerPatternName.MaybeSideEffects) )) {
                    if (AllowReplace(XmlILOptimization.EliminateIsEmpty, local0)) {
                        return Replace(XmlILOptimization.EliminateIsEmpty, local0, VisitFalse(f.False()));
                    }
                }
            }
            if (this[XmlILOptimization.EliminateIsEmpty]) {
                if ( !( (local1).XmlType ).MaybeEmpty ) {
                    if (AllowReplace(XmlILOptimization.EliminateIsEmpty, local0)) {
                        return Replace(XmlILOptimization.EliminateIsEmpty, local0, VisitLoop(f.Loop(VisitLet(f.Let(local1)), VisitFalse(f.False()))));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        #endregion // Type operators
        
        #region XPath operators
        protected override QilNode VisitXPathNodeValue(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitXPathFollowing(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateXPathFollowing]) {
                if (AllowReplace(XmlILOptimization.AnnotateXPathFollowing, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.Axis);  AddStepPattern((QilNode) (local0), (QilNode) (local1));  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.IsDocOrderDistinct);  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitXPathPreceding(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateXPathPreceding]) {
                if (AllowReplace(XmlILOptimization.AnnotateXPathPreceding, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.Axis);  AddStepPattern((QilNode) (local0), (QilNode) (local1));  }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitXPathNamespace(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateNamespace]) {
                if (AllowReplace(XmlILOptimization.AnnotateNamespace, local0)) {
                     OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.Axis);  AddStepPattern((QilNode) (local0), (QilNode) (local1));  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.IsDocOrderDistinct);  OptimizerPatterns.Write((QilNode) (local0)).AddPattern(OptimizerPatternName.SameDepth);  }
            }
            return NoReplace(local0);
        }
        
        #endregion // XPath operators
        
        #region XSLT
        protected override QilNode VisitXsltGenerateId(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitXsltCopy(QilBinary local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local2).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local2)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateConstruction]) {
                if (AllowReplace(XmlILOptimization.AnnotateConstruction, local0)) {
                    
    local0.Right = this.contentAnalyzer.Analyze(local0, local2);
}
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitXsltCopyOf(QilUnary local0) {
            QilNode local1 = local0[0];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.AnnotateConstruction]) {
                if (AllowReplace(XmlILOptimization.AnnotateConstruction, local0)) {
                    
    this.contentAnalyzer.Analyze(local0, null);
}
            }
            return NoReplace(local0);
        }
        
        protected override QilNode VisitXsltConvert(QilTargetType local0) {
            QilNode local1 = local0[0];
            QilNode local2 = local0[1];
            if (this[XmlILOptimization.FoldNone]) {
                if ( (object) ( (local1).XmlType ) == (object) XmlQueryTypeFactory.None ) {
                    if (AllowReplace(XmlILOptimization.FoldNone, local0)) {
                        return Replace(XmlILOptimization.FoldNone, local0, VisitNop(f.Nop(local1)));
                    }
                }
            }
            if (this[XmlILOptimization.FoldXsltConvertLiteral]) {
                if ( IsLiteral((local1)) ) {
                    if (local2.NodeType == QilNodeType.LiteralType) {
                        XmlQueryType local3 = (XmlQueryType)((QilLiteral)local2).Value;
                        if ( CanFoldXsltConvert(local1, local3) ) {
                            if (AllowReplace(XmlILOptimization.FoldXsltConvertLiteral, local0)) {
                                return Replace(XmlILOptimization.FoldXsltConvertLiteral, local0,  FoldXsltConvert(local1, local3) );
                            }
                        }
                    }
                }
            }
            if (this[XmlILOptimization.EliminateXsltConvert]) {
                if (local2.NodeType == QilNodeType.LiteralType) {
                    XmlQueryType local3 = (XmlQueryType)((QilLiteral)local2).Value;
                    if ( ( (local1).XmlType ) == (local3) ) {
                        if (AllowReplace(XmlILOptimization.EliminateXsltConvert, local0)) {
                            return Replace(XmlILOptimization.EliminateXsltConvert, local0, local1);
                        }
                    }
                }
            }
            return NoReplace(local0);
        }
        
        #endregion // XSLT
        
        #endregion // AUTOGENERATED

        /// <summary>
        /// Selectively enable/disable optimizations
        /// </summary>
        private bool this[XmlILOptimization ann] {
            get { return Patterns.IsSet((int)ann); }
        }

        private class NodeCounter : QilVisitor {
            protected QilNode target;
            protected int cnt;

            /// <summary>
            /// Returns number of occurrences of "target" node within the subtree of "expr".
            /// </summary>
            public int Count(QilNode expr, QilNode target) {
                this.cnt = 0;
                this.target = target;
                Visit(expr);
                return this.cnt;
            }

            protected override QilNode Visit(QilNode n) {
                if (n == null)
                    return null;

                if (n == this.target)
                    this.cnt++;

                return VisitChildren(n);
            }

            protected override QilNode VisitReference(QilNode n) {
                if (n == this.target)
                    this.cnt++;

                return n;
            }
        }

        private class NodeFinder : QilVisitor {
            protected bool result;
            protected QilNode target, parent;

            /// <summary>
            /// Returns true if "target" node exists within the subtree of "expr".
            /// </summary>
            public bool Find(QilNode expr, QilNode target) {
                this.result = false;
                this.target = target;
                this.parent = null;
                VisitAssumeReference(expr);
                return this.result;
            }

            /// <summary>
            /// Recursively visit, searching for target.  If/when found, call OnFound() method.
            /// </summary>
            protected override QilNode Visit(QilNode expr) {
                if (!this.result) {
                    if (expr == this.target)
                        this.result = OnFound(expr);

                    if (!this.result) {
                        QilNode parentOld = this.parent;
                        this.parent = expr;
                        VisitChildren(expr);
                        this.parent = parentOld;
                    }
                }

                return expr;
            }

            /// <summary>
            /// Determine whether target is a reference.
            /// </summary>
            protected override QilNode VisitReference(QilNode expr) {
                if (expr == this.target)
                    this.result = OnFound(expr);

                return expr;
            }

            /// <summary>
            /// By default, just return true.
            /// </summary>
            protected virtual bool OnFound(QilNode expr) {
                return true;
            }
        }

        private class PositionOfFinder : NodeFinder {
            /// <summary>
            /// Return true only if parent node type is PositionOf.
            /// </summary>
            protected override bool OnFound(QilNode expr) {
                return this.parent != null && this.parent.NodeType == QilNodeType.PositionOf;
            }
        }

        private class EqualityIndexVisitor : QilVisitor {
            protected bool result;
            protected QilNode ctxt, key;

            /// <summary>
            /// Returns true if the subtree of "expr" meets the following requirements:
            ///   1. Does not contain a reference to "key"
            ///   2. Every reference to "ctxt" is wrapped by a QilNodeType.Root node
            /// </summary>
            public bool Scan(QilNode expr, QilNode ctxt, QilNode key) {
                this.result = true;
                this.ctxt = ctxt;
                this.key = key;
                Visit(expr);
                return this.result;
            }

            /// <summary>
            /// Recursively visit, looking for references to "key" and "ctxt".
            /// </summary>
            protected override QilNode VisitReference(QilNode expr) {
                if (this.result) {
                    if (expr == this.key || expr == this.ctxt) {
                        this.result = false;
                        return expr;
                    }
                }
                return expr;
            }

            /// <summary>
            /// If Root wraps a reference to "ctxt", then don't visit "ctxt" and continue scan.
            /// </summary>
            protected override QilNode VisitRoot(QilUnary root) {
                if (root.Child == this.ctxt)
                    return root;

                return VisitChildren(root);
            }
        }

        /// <summary>
        /// Returns true if any operator within the "expr" subtree references "target".
        /// </summary>
        private bool DependsOn(QilNode expr, QilNode target) {
            return new NodeFinder().Find(expr, target);
        }

        /// <summary>
        /// Returns true if there is no PositionOf operator within the "expr" subtree that references iterator "iter".
        /// </summary>
        protected bool NonPositional(QilNode expr, QilNode iter) {
            return !(new PositionOfFinder().Find(expr, iter));
        }

        /// <summary>
        /// Scans "expr" subtree, looking for "refOld" references and replacing them with "refNew" references.
        /// </summary>
        private QilNode Subs(QilNode expr, QilNode refOld, QilNode refNew) {
            QilNode result;

            this.subs.AddSubstitutionPair(refOld, refNew);
            if (expr is QilReference)
                result = VisitReference(expr);
            else
                result = Visit(expr);
            this.subs.RemoveLastSubstitutionPair();

            return result;
        }

        /// <summary>
        /// True if the specified iterator is a global variable Let iterator.
        /// </summary>
        private bool IsGlobalVariable(QilIterator iter) {
            return this.qil.GlobalVariableList.Contains(iter);
        }

        /// <summary>
        /// Return true if "typ" is xs:decimal=, xs:integer=, xs:int=, xs:double=, or xs:float=.
        /// </summary>
        private bool IsPrimitiveNumeric(XmlQueryType typ) {
            if (typ == XmlQueryTypeFactory.IntX) return true;
            if (typ == XmlQueryTypeFactory.IntegerX) return true;
            if (typ == XmlQueryTypeFactory.DecimalX) return true;
            if (typ == XmlQueryTypeFactory.FloatX) return true;
            if (typ == XmlQueryTypeFactory.DoubleX) return true;

            return false;
        }

        /// <summary>
        /// Returns true if "typ" matches one of the XPath content node tests: *, text(), comment(), pi(), or node().
        /// </summary>
        private bool MatchesContentTest(XmlQueryType typ) {
            if (typ == XmlQueryTypeFactory.Element) return true;
            if (typ == XmlQueryTypeFactory.Text) return true;
            if (typ == XmlQueryTypeFactory.Comment) return true;
            if (typ == XmlQueryTypeFactory.PI) return true;
            if (typ == XmlQueryTypeFactory.Content) return true;

            return false;
        }

        /// <summary>
        /// True if the specified expression constructs one or more nodes using QilExpression constructor operators.
        /// This information is used to determine whether the results of a function should be streamed to a writer
        /// rather than cached.
        /// </summary>
        private bool IsConstructedExpression(QilNode nd) {
            QilTernary ndCond;

            // In debug mode, all functions should return void (streamed to writer), so that call stack
            // consistently shows caller's line number
            if (this.qil.IsDebug)
                return true;

            if (nd.XmlType.IsNode) {
                switch (nd.NodeType) {
                    case QilNodeType.ElementCtor:
                    case QilNodeType.AttributeCtor:
                    case QilNodeType.CommentCtor:
                    case QilNodeType.PICtor:
                    case QilNodeType.TextCtor:
                    case QilNodeType.RawTextCtor:
                    case QilNodeType.DocumentCtor:
                    case QilNodeType.NamespaceDecl:
                    case QilNodeType.XsltCopy:
                    case QilNodeType.XsltCopyOf:
                    case QilNodeType.Choice:
                        return true;

                    case QilNodeType.Loop:
                        // Return true if the return expression is constructed
                        return IsConstructedExpression(((QilLoop) nd).Body);

                    case QilNodeType.Sequence:
                        // Return true if the list is empty or at least one expression in the list is constructed
                        if (nd.Count == 0)
                            return true;

                        foreach (QilNode ndItem in nd) {
                            if (IsConstructedExpression(ndItem))
                                return true;
                        }
                        break;

                    case QilNodeType.Conditional:
                        // Return true if either left and right branches of the conditional are constructed
                        ndCond = (QilTernary) nd;
                        return IsConstructedExpression(ndCond.Center) || IsConstructedExpression(ndCond.Right);

                    case QilNodeType.Invoke:
                        // Return true if the function might return nodes
                        return !((QilInvoke) nd).Function.XmlType.IsAtomicValue;
                }
            }

            return false;
        }

        /// <summary>
        /// True if the specified expression is a literal value.
        /// </summary>
        private bool IsLiteral(QilNode nd) {
            switch (nd.NodeType) {
                case QilNodeType.True:
                case QilNodeType.False:
                case QilNodeType.LiteralString:
                case QilNodeType.LiteralInt32:
                case QilNodeType.LiteralInt64:
                case QilNodeType.LiteralDouble:
                case QilNodeType.LiteralDecimal:
                case QilNodeType.LiteralQName:
                    return true;
            }
            return false;
        }

        /// <summary>
        /// Return true if all children of "nd" are constant.
        /// </summary>
        private bool AreLiteralArgs(QilNode nd) {
            foreach (QilNode child in nd)
                if (!IsLiteral(child))
                    return false;

            return true;
        }

        /// <summary>
        /// Extract the value of a literal.
        /// </summary>
        private object ExtractLiteralValue(QilNode nd) {
            if (nd.NodeType == QilNodeType.True)
                return true;
            else if (nd.NodeType == QilNodeType.False)
                return false;
            else if (nd.NodeType == QilNodeType.LiteralQName)
                return nd;

            Debug.Assert(nd is QilLiteral, "All literals except True, False, and QName must use QilLiteral");
            return ((QilLiteral) nd).Value;
        }

        /// <summary>
        /// Return true if "nd" has a child of type Sequence.
        /// </summary>
        private bool HasNestedSequence(QilNode nd) {
            foreach (QilNode child in nd) {
                if (child.NodeType == QilNodeType.Sequence)
                    return true;
            }
            return false;
        }

        /// <summary>
        /// True if the JoinAndDod pattern is allowed to match the specified node.
        /// </summary>
        private bool AllowJoinAndDod(QilNode nd) {
            OptimizerPatterns patt = OptimizerPatterns.Read(nd);

            // AllowJoinAndDod if this pattern is the descendant, descendant-or-self, content, preceding, following, or
            // following-sibling axis, filtered by either an element name or a node kind test.
            if (patt.MatchesPattern(OptimizerPatternName.FilterElements) || patt.MatchesPattern(OptimizerPatternName.FilterContentKind)) {
                if (IsStepPattern(patt, QilNodeType.DescendantOrSelf) || IsStepPattern(patt, QilNodeType.Descendant) ||
                    IsStepPattern(patt, QilNodeType.Content) || IsStepPattern(patt, QilNodeType.XPathPreceding) ||
                    IsStepPattern(patt, QilNodeType.XPathFollowing) || IsStepPattern(patt, QilNodeType.FollowingSibling)) {
                    return true;
                }
            }
            return false;
        }

        /// <summary>
        /// True if the DodReverse pattern is allowed to match the specified node.
        /// </summary>
        private bool AllowDodReverse(QilNode nd) {
            OptimizerPatterns patt = OptimizerPatterns.Read(nd);

            // AllowDodReverse if this pattern is the ancestor, ancestor-or-self, preceding, or preceding-sibling axis,
            // filtered by either an element name or a node kind test.
            if (patt.MatchesPattern(OptimizerPatternName.Axis) ||
                patt.MatchesPattern(OptimizerPatternName.FilterElements) ||
                patt.MatchesPattern(OptimizerPatternName.FilterContentKind)) {
                if (IsStepPattern(patt, QilNodeType.Ancestor) || IsStepPattern(patt, QilNodeType.AncestorOrSelf) ||
                    IsStepPattern(patt, QilNodeType.XPathPreceding) || IsStepPattern(patt, QilNodeType.PrecedingSibling)) {
                    return true;
                }
            }
            return false;
        }

        /// <summary>
        /// Return true if XsltConvert applied to a Literal can be folded (i.e. the XsltConvert eliminated).
        /// </summary>
        private bool CanFoldXsltConvert(QilNode ndLiteral, XmlQueryType typTarget) {
            // Attempt to fold--on failure, an unfolded XsltConvert node will be returned
            return FoldXsltConvert(ndLiteral, typTarget).NodeType != QilNodeType.XsltConvert;
        }

        /// <summary>
        /// Return true if XsltConvert applied to a Literal can be folded (i.e. the XsltConvert eliminated), without
        /// any loss of information.
        /// </summary>
        private bool CanFoldXsltConvertNonLossy(QilNode ndLiteral, XmlQueryType typTarget) {
            QilNode ndDest;

            // Fold conversion to target type; if conversion cannot be folded, a XsltConvert node is returned
            ndDest = FoldXsltConvert(ndLiteral, typTarget);
            if (ndDest.NodeType == QilNodeType.XsltConvert)
                return false;

            // Convert back to source type; if conversion cannot be folded, a XsltConvert node is returned
            ndDest = FoldXsltConvert(ndDest, ndLiteral.XmlType);
            if (ndDest.NodeType == QilNodeType.XsltConvert)
                return false;

            // If original value is the same as the round-tripped value, then conversion is non-lossy
            return ExtractLiteralValue(ndLiteral).Equals(ExtractLiteralValue(ndDest));
        }

        /// <summary>
        /// Fold a XsltConvert applied to a Literal into another Literal.  If the fold results in some kind of
        /// conversion error, or if the QilExpression cannot represent the result as a Literal, return an unfolded
        /// XsltConvert expression.
        /// </summary>
        private QilNode FoldXsltConvert(QilNode ndLiteral, XmlQueryType typTarget) {
            try {
                if (typTarget.IsAtomicValue) {
                    // Convert the literal to an XmlAtomicValue
                    XmlAtomicValue value = new XmlAtomicValue(ndLiteral.XmlType.SchemaType, ExtractLiteralValue(ndLiteral));
                    value = XsltConvert.ConvertToType(value, typTarget);

                    if (typTarget == TypeFactory.StringX)
                        return this.f.LiteralString(value.Value);
                    else if (typTarget == TypeFactory.IntX)
                        return this.f.LiteralInt32(value.ValueAsInt);
                    else if (typTarget == TypeFactory.IntegerX)
                        return this.f.LiteralInt64(value.ValueAsLong);
                    else if (typTarget == TypeFactory.DecimalX)
                        return this.f.LiteralDecimal((decimal) value.ValueAs(XsltConvert.DecimalType));
                    else if (typTarget == TypeFactory.DoubleX)
                        return this.f.LiteralDouble(value.ValueAsDouble);
                    else if (typTarget == TypeFactory.BooleanX)
                        return value.ValueAsBoolean ? this.f.True() : this.f.False();
                }
            }
            catch (OverflowException) {}
            catch (FormatException) {}

            // Conversion error or QilExpression cannot represent resulting literal
            return this.f.XsltConvert(ndLiteral, typTarget);
        }

        /// <summary>
        /// Compute the arithmetic operation "opType" over two literal operands and return the result as a QilLiteral.
        /// In the case of an overflow or divide by zero exception, return the unfolded result.
        /// </summary>
        private QilNode FoldComparison(QilNodeType opType, QilNode left, QilNode right) {
            object litLeft, litRight;
            int cmp;
            Debug.Assert(left.XmlType == right.XmlType, "Comparison is not defined between " + left.XmlType + " and " + right.XmlType);

            // Extract objects that represent each literal value
            litLeft = ExtractLiteralValue(left);
            litRight = ExtractLiteralValue(right);

            if (left.NodeType == QilNodeType.LiteralDouble) {
                // Equals and CompareTo do not handle NaN correctly
                if (Double.IsNaN((double) litLeft) || Double.IsNaN((double) litRight))
                    return (opType == QilNodeType.Ne) ? f.True() : f.False();
            }

            if (opType == QilNodeType.Eq)
                return litLeft.Equals(litRight) ? f.True() : f.False();

            if (opType == QilNodeType.Ne)
                return litLeft.Equals(litRight) ? f.False() : f.True();

            if (left.NodeType == QilNodeType.LiteralString) {
                // CompareTo does not use Ordinal comparison
                cmp = string.CompareOrdinal((string) litLeft, (string) litRight);
            }
            else {
                cmp = ((IComparable) litLeft).CompareTo(litRight);
            }

            switch (opType) {
                case QilNodeType.Gt: return cmp > 0 ? f.True() : f.False();
                case QilNodeType.Ge: return cmp >= 0 ? f.True() : f.False();
                case QilNodeType.Lt: return cmp < 0 ? f.True() : f.False();
                case QilNodeType.Le: return cmp <= 0 ? f.True() : f.False();
            }

            Debug.Assert(false, "Cannot fold this comparison operation: " + opType);
            return null;
        }

        /// <summary>
        /// Return true if arithmetic operation "opType" can be computed over two literal operands without causing
        /// an overflow or divide by zero exception.
        /// </summary>
        private bool CanFoldArithmetic(QilNodeType opType, QilLiteral left, QilLiteral right) {
            return (FoldArithmetic(opType, left, right) is QilLiteral);
        }

        /// <summary>
        /// Compute the arithmetic operation "opType" over two literal operands and return the result as a QilLiteral.
        /// Arithmetic operations are always checked; in the case of an overflow or divide by zero exception, return
        /// the unfolded result.
        /// </summary>
        private QilNode FoldArithmetic(QilNodeType opType, QilLiteral left, QilLiteral right) {
            Debug.Assert(left.NodeType == right.NodeType);

            // Catch any overflow or divide by zero exceptions
            try {
                checked {
                    switch (left.NodeType) {
                        case QilNodeType.LiteralInt32: {
                            int intLeft = left;
                            int intRight = right;

                            switch (opType) {
                                case QilNodeType.Add: return f.LiteralInt32(intLeft + intRight);
                                case QilNodeType.Subtract: return f.LiteralInt32(intLeft - intRight);
                                case QilNodeType.Multiply: return f.LiteralInt32(intLeft * intRight);
                                case QilNodeType.Divide: return f.LiteralInt32(intLeft / intRight);
                                case QilNodeType.Modulo: return f.LiteralInt32(intLeft % intRight);
                            }
                            break;
                        }

                        case QilNodeType.LiteralInt64: {
                            long lngLeft = left;
                            long lngRight = right;

                            switch (opType) {
                                case QilNodeType.Add: return f.LiteralInt64(lngLeft + lngRight);
                                case QilNodeType.Subtract: return f.LiteralInt64(lngLeft - lngRight);
                                case QilNodeType.Multiply: return f.LiteralInt64(lngLeft * lngRight);
                                case QilNodeType.Divide: return f.LiteralInt64(lngLeft / lngRight);
                                case QilNodeType.Modulo: return f.LiteralInt64(lngLeft % lngRight);
                            }
                            break;
                        }

                        case QilNodeType.LiteralDecimal: {
                            decimal lngLeft = left;
                            decimal lngRight = right;

                            switch (opType) {
                                case QilNodeType.Add: return f.LiteralDecimal(lngLeft + lngRight);
                                case QilNodeType.Subtract: return f.LiteralDecimal(lngLeft - lngRight);
                                case QilNodeType.Multiply: return f.LiteralDecimal(lngLeft * lngRight);
                                case QilNodeType.Divide: return f.LiteralDecimal(lngLeft / lngRight);
                                case QilNodeType.Modulo: return f.LiteralDecimal(lngLeft % lngRight);
                            }
                            break;
                        }

                        case QilNodeType.LiteralDouble: {
                            double lngLeft = left;
                            double lngRight = right;

                            switch (opType) {
                                case QilNodeType.Add: return f.LiteralDouble(lngLeft + lngRight);
                                case QilNodeType.Subtract: return f.LiteralDouble(lngLeft - lngRight);
                                case QilNodeType.Multiply: return f.LiteralDouble(lngLeft * lngRight);
                                case QilNodeType.Divide: return f.LiteralDouble(lngLeft / lngRight);
                                case QilNodeType.Modulo: return f.LiteralDouble(lngLeft % lngRight);
                            }
                            break;
                        }
                    }
                }
            }
            catch (OverflowException) {
            }
            catch (DivideByZeroException) {
            }

            // An error occurred, so don't fold operationo
            switch (opType) {
                case QilNodeType.Add: return f.Add(left, right);
                case QilNodeType.Subtract: return f.Subtract(left, right);
                case QilNodeType.Multiply: return f.Multiply(left, right);
                case QilNodeType.Divide: return f.Divide(left, right);
                case QilNodeType.Modulo: return f.Modulo(left, right);
            }

            Debug.Assert(false, "Cannot fold this arithmetic operation: " + opType);
            return null;
        }

        /// <summary>
        /// Mark the specified node as matching the Step pattern and set the step node and step input arguments.
        /// </summary>
        private void AddStepPattern(QilNode nd, QilNode input) {
            OptimizerPatterns patt = OptimizerPatterns.Write(nd);
            patt.AddPattern(OptimizerPatternName.Step);
            patt.AddArgument(OptimizerPatternArgument.StepNode, nd);
            patt.AddArgument(OptimizerPatternArgument.StepInput, input);
        }

        /// <summary>
        /// Return true if "nd" matches the Step pattern and the StepType argument is equal to "stepType".
        /// </summary>
        private bool IsDocOrderDistinct(QilNode nd) {
            return OptimizerPatterns.Read(nd).MatchesPattern(OptimizerPatternName.IsDocOrderDistinct);
        }

        /// <summary>
        /// Return true if "nd" matches the Step pattern and the StepType argument is equal to "stepType".
        /// </summary>
        private bool IsStepPattern(QilNode nd, QilNodeType stepType) {
            return IsStepPattern(OptimizerPatterns.Read(nd), stepType);
        }

        /// <summary>
        /// Return true if "patt" matches the Step pattern and the StepType argument is equal to "stepType".
        /// </summary>
        private bool IsStepPattern(OptimizerPatterns patt, QilNodeType stepType) {
            return patt.MatchesPattern(OptimizerPatternName.Step) && ((QilNode) patt.GetArgument(OptimizerPatternArgument.StepNode)).NodeType == stepType;
        }
    }
}
