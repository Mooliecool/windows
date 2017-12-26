//------------------------------------------------------------------------------
// <copyright file="Action.cs" company="Microsoft">
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

namespace System.Xml.Xsl.XsltOld {
    using Res = System.Xml.Utils.Res;
    using System;
    using System.Xml;
    using System.Xml.XPath;
    using MS.Internal.Xml.XPath;
    using System.Xml.Xsl.XsltOld.Debugger;

    internal abstract class Action {
        internal const int Initialized  =  0;
        internal const int Finished     = -1;

        internal abstract void Execute(Processor processor, ActionFrame frame);

        internal virtual void ReplaceNamespaceAlias(Compiler compiler){}

        // -------------- Debugger related stuff ---------
        // We have to pass ActionFrame to GetNavigator and GetVariables
        // because CopyCodeAction can't implement them without frame.count

        internal virtual DbgData GetDbgData(ActionFrame frame) { return DbgData.Empty; }
    }
}
