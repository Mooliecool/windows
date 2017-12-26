//------------------------------------------------------------------------------
// <copyright file="CompilerResults.cs" company="Microsoft">
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
    using System;
    using System.CodeDom;
    using System.Reflection;
    using System.Collections;
    using System.Collections.Specialized;
    using System.Security;
    using System.Security.Permissions;
    using System.Security.Policy;
    using System.Runtime.Serialization;
    using System.Runtime.Serialization.Formatters.Binary;
    using System.IO;


    /// <devdoc>
    ///    <para>
    ///       Represents the results
    ///       of compilation from the compiler.
    ///    </para>
    /// </devdoc>
    [Serializable()]
    [PermissionSet(SecurityAction.InheritanceDemand, Name="FullTrust")]
    public class CompilerResults {
        private CompilerErrorCollection errors = new CompilerErrorCollection();
        private StringCollection output = new StringCollection();
        private Assembly compiledAssembly;
        private string pathToAssembly;
        private int nativeCompilerReturnValue;
        private TempFileCollection tempFiles;
        private Evidence evidence;

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.Compiler.CompilerResults'/>
        ///       that uses the specified
        ///       temporary files.
        ///    </para>
        /// </devdoc>
        [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
        public CompilerResults(TempFileCollection tempFiles) {
            this.tempFiles = tempFiles;
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the temporary files to use.
        ///    </para>
        /// </devdoc>
        public TempFileCollection TempFiles {
            [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
            get {
                return tempFiles;
            }

            [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
            set {
                tempFiles = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Set the evidence for partially trusted scenarios.
        ///    </para>
        /// </devdoc>
        public Evidence Evidence {
            [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
            get {
                Evidence e = null;
                if (evidence != null)
                    e = CloneEvidence(evidence);
                return e;
            }

            [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
            [SecurityPermissionAttribute( SecurityAction.Demand, ControlEvidence = true )]
            set {
                if (value != null)
                    evidence = CloneEvidence(value);
                else
                    evidence = null;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       The compiled assembly.
        ///    </para>
        /// </devdoc>
        public Assembly CompiledAssembly {
            [SecurityPermissionAttribute(SecurityAction.Assert, Flags=SecurityPermissionFlag.ControlEvidence)]
            get {
                if (compiledAssembly == null && pathToAssembly != null) {
                    AssemblyName assemName = new AssemblyName();
                    assemName.CodeBase = pathToAssembly;
                    compiledAssembly = Assembly.Load(assemName,evidence);
                }
                return compiledAssembly;
            }

            [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
            set {
                compiledAssembly = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the collection of compiler errors.
        ///    </para>
        /// </devdoc>
        public CompilerErrorCollection Errors {
            get {
                return errors;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the compiler output messages.
        ///    </para>
        /// </devdoc>
        public StringCollection Output {
            [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
            get {
                return output;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the path to the assembly.
        ///    </para>
        /// </devdoc>
        public string PathToAssembly {
            [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
            get {
                return pathToAssembly;
            }

            [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
            set {
                pathToAssembly = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the compiler's return value.
        ///    </para>
        /// </devdoc>
        public int NativeCompilerReturnValue {
            get {
                return nativeCompilerReturnValue;
            }

            [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
            set {
                nativeCompilerReturnValue = value;
            }
        }

        internal static Evidence CloneEvidence(Evidence ev) {
            new PermissionSet( PermissionState.Unrestricted ).Assert();

            MemoryStream stream = new MemoryStream();

            BinaryFormatter formatter = new BinaryFormatter();

            formatter.Serialize( stream, ev );

            stream.Position = 0;

            return (Evidence)formatter.Deserialize( stream );
        }
    }
}

