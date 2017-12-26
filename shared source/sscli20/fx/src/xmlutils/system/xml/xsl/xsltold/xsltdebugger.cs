//------------------------------------------------------------------------------
// <copyright file="XsltDebugger.cs" company="Microsoft">
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

namespace System.Xml.Xsl.XsltOld.Debugger {
    using Res = System.Xml.Utils.Res;
    using System;
    using System.Xml;
    using System.Xml.XPath;
    
    internal interface IStackFrame {
        XPathNavigator    Instruction     { get; }
        XPathNodeIterator NodeSet         { get; }
        // Variables:
        int               GetVariablesCount();
        XPathNavigator    GetVariable(int varIndex);
        object            GetVariableValue(int varIndex);
    }

    internal interface IXsltProcessor {
        int         StackDepth { get; }
        IStackFrame GetStackFrame(int depth);
    }

    internal interface IXsltDebugger {
        string GetBuiltInTemplatesUri();
        void   OnInstructionCompile(XPathNavigator styleSheetNavigator);
        void   OnInstructionExecute(IXsltProcessor xsltProcessor);
    }
}
