//------------------------------------------------------------------------------
// <copyright file="CodeParser.cs" company="Microsoft">
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

namespace System.CodeDom.Compiler {
    using System.Text;

    using System.Diagnostics;
    using System;
    using Microsoft.Win32;
    using System.IO;
    using System.Collections;
    using System.Reflection;
    using System.CodeDom;
    using System.Security.Permissions;

    /// <devdoc>
    ///    <para>
    ///       Provides a code parsing abstract base class.
    ///    </para>
    /// </devdoc>
    [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
    [PermissionSet(SecurityAction.InheritanceDemand, Name="FullTrust")]
    public abstract class CodeParser : ICodeParser {
    
        /// <devdoc>
        ///    <para>
        ///       Compiles the given text stream into a CodeCompile unit.  
        ///    </para>
        /// </devdoc>
        public abstract CodeCompileUnit Parse(TextReader codeStream);
    }

}
