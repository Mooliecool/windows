//------------------------------------------------------------------------------
// <copyright file="XPathOperator.cs" company="Microsoft">
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

namespace System.Xml.Xsl.XPath {
    // order is importent. We are using them as an index in OperatorGroup & QilOperator & XPathOperatorToQilNodeType arrays
    // (ValEq - Eq) == (ValGe - Ge)
    internal enum XPathOperator {
        /*Unknown   */ 
        Unknown = 0,
        // XPath 1.0 operators:
        /*Logical   */ 
        Or, 
        And,
        /*Equality  */ 
        Eq, 
        Ne,
        /*Relational*/ 
        Lt, 
        Le, 
        Gt, 
        Ge,
        /*Arithmetic*/ 
        Plus, 
        Minus, 
        Multiply, 
        Divide, 
        Modulo,
        /*Negate    */
        UnaryMinus,
        /*Union     */ 
        Union,
        LastXPath1Operator = Union,
        /* XQuery & XPath 2.0 Operators: */
        UnaryPlus,
        Idiv,
        Is,
        After,
        Before,
        Range,
        Except,
        Intersect,
        ValEq,
        ValNe,
        ValLt,
        ValLe,
        ValGt,
        ValGe
    }
}
