//------------------------------------------------------------------------------
// <copyright file="ICodeCompiler.cs" company="Microsoft">
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

    using System.Diagnostics;
    using System.IO;
    using System.Security.Permissions;

    /// <devdoc>
    ///    <para>
    ///       Provides a
    ///       code compilation
    ///       interface.
    ///    </para>
    /// </devdoc>
    public interface ICodeCompiler {

        /// <devdoc>
        ///    <para>
        ///       Creates an assembly based on options, with the information from the compile units
        ///    </para>
        /// </devdoc>
        [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
        [PermissionSet(SecurityAction.InheritanceDemand, Name="FullTrust")]
        CompilerResults CompileAssemblyFromDom(CompilerParameters options, CodeCompileUnit compilationUnit);

        /// <devdoc>
        ///    <para>
        ///       Creates an assembly based on options, with the contents of
        ///       fileName.
        ///    </para>
        /// </devdoc>
        [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
        [PermissionSet(SecurityAction.InheritanceDemand, Name="FullTrust")]
        CompilerResults CompileAssemblyFromFile(CompilerParameters options, string fileName);

        /// <devdoc>
        ///    <para>
        ///       Creates an assembly based on options, with the information from
        ///       source.
        ///    </para>
        /// </devdoc>
        [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
        [PermissionSet(SecurityAction.InheritanceDemand, Name="FullTrust")]
        CompilerResults CompileAssemblyFromSource(CompilerParameters options, string source);

        /// <devdoc>
        ///    <para>
        ///       Compiles an assembly based on the specified options and
        ///       information.
        ///    </para>
        /// </devdoc>
        [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
        [PermissionSet(SecurityAction.InheritanceDemand, Name="FullTrust")]
        CompilerResults CompileAssemblyFromDomBatch(CompilerParameters options, CodeCompileUnit[] compilationUnits);

        /// <devdoc>
        ///    <para>
        ///       Compiles
        ///       an
        ///       assembly based on the specified options and contents of the specified
        ///       filenames.
        ///    </para>
        /// </devdoc>
        [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
        [PermissionSet(SecurityAction.InheritanceDemand, Name="FullTrust")]
        CompilerResults CompileAssemblyFromFileBatch(CompilerParameters options, string[] fileNames);

        /// <devdoc>
        ///    <para>
        ///       Compiles an assembly based on the specified options and information from the specified
        ///       sources.
        ///    </para>
        /// </devdoc>
        [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
        [PermissionSet(SecurityAction.InheritanceDemand, Name="FullTrust")]
        CompilerResults CompileAssemblyFromSourceBatch(CompilerParameters options, string[] sources);

    }
}
