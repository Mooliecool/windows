//------------------------------------------------------------------------------
// <copyright file="IXPathEnvironment.cs" company="Microsoft">
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
using System.Xml.Xsl.Qil;

namespace System.Xml.Xsl.XPath {

    // <spec>http://www.w3.org/TR/xslt20/#dt-focus</spec>
    internal interface IFocus {
        // The context item:     the item currently being processed
        QilNode GetCurrent();

        // The context position: the position of the context item within the sequence of items
        // currently being processed
        QilNode GetPosition();

        // The context size:     the number of items in the sequence of items currently being processed
        QilNode GetLast();
    }

    internal interface IXPathEnvironment : IFocus {
        XPathQilFactory Factory { get; }

        // Resolution of variables, functions, and prefixes
        QilNode ResolveVariable(string prefix, string name);
        QilNode ResolveFunction(string prefix, string name, IList<QilNode> args, IFocus env);
        string  ResolvePrefix  (string prefix);
    }
}
