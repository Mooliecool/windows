//------------------------------------------------------------------------------
// <copyright file="XPathContext.cs" company="Microsoft">
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

#if DontUse
// XPathContext is not used any more but comments in it and Replacer visitor may be used to 
// optimize code XSLT generates on last().
using System;
using System.Diagnostics;
using System.Collections;
using System.Xml;
using System.Xml.XPath;
using MS.Internal.Xml;

namespace System.Xml.Xsl.XPath {

    internal class XPathContext {
        //
        //
        //
        //
        //


        public static QilNode GetCurrentNode(QilTuple context) {
            Debug.Assert(context != null);
            Debug.Assert(GetTuple(context).For.Type == QilNodeType.For);
            return GetTuple(context).For;
        }

        public static QilNode GetCurrentPosition(QilFactory f, QilTuple context) {
            Debug.Assert(context != null);
            if (context.Where.Type != QilNodeType.True) {
                Debug.Assert(context.For.Type == QilNodeType.For);
                // convert context (1) --> (2)
                QilIterator for2 = f.For(context.For.Binding);
                QilNode     cnd2 = new Replacer(f).Replace(/*inExpr:*/context.Where, /*from:*/context.For, /*to:*/for2);
                context.For.Binding = f.OldTuple(for2, cnd2, for2);
                context.Where = f.True();
            }
            return f.Convert(f.PositionOf((QilIterator)XPathContext.GetCurrentNode(context)), f.TypeFactory.Double());
        }

        public static QilNode GetLastPosition(QilFactory f, QilTuple context) {
            return f.Convert(f.Length(context.Clone(f)), f.TypeFactory.Double());
        }

        public static QilTuple GetTuple(QilTuple context) {
            Debug.Assert(context != null);
            if (context.For.Type == QilNodeType.Let) {
                Debug.Assert(context.Where.Type == QilNodeType.True);
                Debug.Assert(context.Return.Type == QilNodeType.OldTuple);
                return (QilTuple) context.Return;
            }
            return context;
        }

        private class Replacer : QilActiveVisitor {
            QilIterator from, to;

            public Replacer(QilFactory f) : base(f) {}

            public QilNode Replace(QilNode inExpr, QilIterator from, QilIterator to) {
                this.from = from;
                this.to   = to  ;
                return Visit(inExpr);
            }

            protected override QilNode VisitClassReference(QilNode it) {
                if (it == from) {
                    return to;
                }
                return it;
            }
        }
    }
}
#endif