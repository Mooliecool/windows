//------------------------------------------------------------------------------
// <copyright file="CompilerParameters.cs" company="Microsoft">
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
    using System.Collections;
    using System.Collections.Specialized;
    using Microsoft.Win32;
    using Microsoft.Win32.SafeHandles;
    using System.Runtime.InteropServices;
    using System.Security.Permissions;
    using System.Security.Policy;
    using System.Runtime.Serialization;

    /// <devdoc>
    ///    <para>
    ///       Represents the parameters used in to invoke the compiler.
    ///    </para>
    /// </devdoc>
    [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
    [PermissionSet(SecurityAction.InheritanceDemand, Name="FullTrust")]
    [Serializable]
    public class CompilerParameters {
        private StringCollection assemblyNames = new StringCollection();

        [OptionalField] 
        private StringCollection embeddedResources = new StringCollection();
        [OptionalField]         
        private StringCollection linkedResources = new StringCollection();
        
        private string outputName;
        private string mainClass;
        private bool generateInMemory = false;
        private bool includeDebugInformation = false;
        private int warningLevel = -1;  // -1 means not set (use compiler default)
        private string compilerOptions;
        private string win32Resource;
        private bool treatWarningsAsErrors = false;
        private bool generateExecutable = false;
        private TempFileCollection tempFiles;
        [NonSerializedAttribute]
        private SafeUserTokenHandle userToken;
        private Evidence evidence = null;

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.Compiler.CompilerParameters'/>.
        ///    </para>
        /// </devdoc>
        public CompilerParameters() :
            this(null, null) {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.Compiler.CompilerParameters'/> using the specified
        ///       assembly names.
        ///    </para>
        /// </devdoc>
        public CompilerParameters(string[] assemblyNames) :
            this(assemblyNames, null, false) {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.Compiler.CompilerParameters'/> using the specified
        ///       assembly names and output name.
        ///    </para>
        /// </devdoc>
        public CompilerParameters(string[] assemblyNames, string outputName) :
            this(assemblyNames, outputName, false) {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.Compiler.CompilerParameters'/> using the specified
        ///       assembly names, output name and a whether to include debug information flag.
        ///    </para>
        /// </devdoc>
        public CompilerParameters(string[] assemblyNames, string outputName, bool includeDebugInformation) {
            if (assemblyNames != null) {
                ReferencedAssemblies.AddRange(assemblyNames);
            }
            this.outputName = outputName;
            this.includeDebugInformation = includeDebugInformation;
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets whether to generate an executable.
        ///    </para>
        /// </devdoc>
        public bool GenerateExecutable {
            get {
                return generateExecutable;
            }
            set {
                generateExecutable = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets whether to generate in memory.
        ///    </para>
        /// </devdoc>
        public bool GenerateInMemory {
            get {
                return generateInMemory;
            }
            set {
                generateInMemory = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the assemblies referenced by the source to compile.
        ///    </para>
        /// </devdoc>
        public StringCollection ReferencedAssemblies {
            get {
                return assemblyNames;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the main class.
        ///    </para>
        /// </devdoc>
        public string MainClass {
            get {
                return mainClass;
            }
            set {
                mainClass = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the output assembly.
        ///    </para>
        /// </devdoc>
        public string OutputAssembly {
            get {
                return outputName;
            }
            set {
                outputName = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the temp files.
        ///    </para>
        /// </devdoc>
        public TempFileCollection TempFiles {
            get {
                if (tempFiles == null)
                    tempFiles = new TempFileCollection();
                return tempFiles;
            }
            set {
                tempFiles = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets whether to include debug information in the compiled
        ///       executable.
        ///    </para>
        /// </devdoc>
        public bool IncludeDebugInformation {
            get {
                return includeDebugInformation;
            }
            set {
                includeDebugInformation = value;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool TreatWarningsAsErrors {
            get {
                return treatWarningsAsErrors;
            }
            set {
                treatWarningsAsErrors = value;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int WarningLevel {
            get {
                return warningLevel;
            }
            set {
                warningLevel = value;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string CompilerOptions {
            get {
                return compilerOptions;
            }
            set {
                compilerOptions = value;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string Win32Resource {
            get {
                return win32Resource;
            }
            set {
                win32Resource = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the resources to be compiled into the target
        ///    </para>
        /// </devdoc>
        [System.Runtime.InteropServices.ComVisible(false)]        
        public StringCollection EmbeddedResources {
            get {
                return embeddedResources;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the linked resources
        ///    </para>
        /// </devdoc>
        [System.Runtime.InteropServices.ComVisible(false)]        
        public StringCollection LinkedResources {
            get {
                return linkedResources;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets user token to be employed when creating the compiler process.
        ///    </para>
        /// </devdoc>
        public IntPtr UserToken {
            get {
                if (userToken != null)
                    return userToken.DangerousGetHandle();
                else
                    return IntPtr.Zero;
            }
            set {
                if (userToken != null)
                    userToken.Close();
                
                userToken = new SafeUserTokenHandle(value, false);
            }
        }

        internal SafeUserTokenHandle SafeUserToken {
            get {
                return userToken;
            }
        }
        
        /// <devdoc>
        ///    <para>
        ///       Set the evidence for partially trusted scenarios.
        ///    </para>
        /// </devdoc>
        public Evidence Evidence {
            get {
                Evidence e = null;
                if (evidence != null)
                    e = CompilerResults.CloneEvidence(evidence);
                return e;
            }
            [SecurityPermissionAttribute( SecurityAction.Demand, ControlEvidence = true )]
            set {
                if (value != null)
                    evidence = CompilerResults.CloneEvidence(value);
                else
                    evidence = null;
            }
        }
    }
}
