//------------------------------------------------------------------------------
// <copyright file="CompilerInfo.cs" company="Microsoft">
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
    using System.Reflection;
    using System.Security.Permissions;
    using System.CodeDom.Compiler;
    using System.Configuration;

    [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
    public sealed class CompilerInfo {
        internal String _codeDomProviderTypeName; // This can never by null
        internal CompilerParameters _compilerParams; // This can never by null
        internal String[] _compilerLanguages; // This can never by null
        internal String[] _compilerExtensions; // This can never by null
        internal String configFileName;
        internal int configFileLineNumber;
        internal Boolean _mapped;

        private Type type;
        
        private CompilerInfo() {} // Not createable

        public String[] GetLanguages() { 
            return CloneCompilerLanguages();
        }

        public String[] GetExtensions() { 
            return CloneCompilerExtensions();
        }

        public Type CodeDomProviderType { 
            get {
                if( type == null) {
                    lock(this) {
                        if( type == null) {
                            type = Type.GetType(_codeDomProviderTypeName);
                            if (type == null) {                    
                                if( configFileName == null) {
                                    throw new ConfigurationErrorsException(SR.GetString(SR.Unable_To_Locate_Type, 
                                                                      _codeDomProviderTypeName, string.Empty, 0));
                                }
                                else {
                                    throw new ConfigurationErrorsException(SR.GetString(SR.Unable_To_Locate_Type,
                                                                      _codeDomProviderTypeName), configFileName, configFileLineNumber);
                                }
                            }
                        }                                                        
                    }
                }
                    
                return type;
            }
        }

        public bool IsCodeDomProviderTypeValid {
            get {
                Type type = Type.GetType(_codeDomProviderTypeName);
                return (type != null);
            }
        }

        public CodeDomProvider CreateProvider() {
            return (CodeDomProvider)Activator.CreateInstance(CodeDomProviderType);
        }

        public CompilerParameters CreateDefaultCompilerParameters() {
            return CloneCompilerParameters();
        }


        internal CompilerInfo(CompilerParameters compilerParams, String codeDomProviderTypeName, String [] compilerLanguages, String [] compilerExtensions) {
            _compilerLanguages = compilerLanguages;
            _compilerExtensions = compilerExtensions;
            _codeDomProviderTypeName = codeDomProviderTypeName;
            if (compilerParams == null)
                compilerParams = new CompilerParameters();

            _compilerParams = compilerParams;
        }

        internal CompilerInfo(CompilerParameters compilerParams, String codeDomProviderTypeName) {
            _codeDomProviderTypeName = codeDomProviderTypeName;
            if (compilerParams == null)
                compilerParams = new CompilerParameters();

            _compilerParams = compilerParams;
        }


        public override int GetHashCode() {
            return _codeDomProviderTypeName.GetHashCode();
        }

        public override bool Equals(Object o) {
            CompilerInfo other = o as CompilerInfo;
            if (o == null)
                return false;

            return CodeDomProviderType == other.CodeDomProviderType &&
                CompilerParams.WarningLevel == other.CompilerParams.WarningLevel &&
                CompilerParams.IncludeDebugInformation == other.CompilerParams.IncludeDebugInformation &&
                CompilerParams.CompilerOptions == other.CompilerParams.CompilerOptions;
        }

        private CompilerParameters CloneCompilerParameters() {
            CompilerParameters copy = new CompilerParameters();
            copy.IncludeDebugInformation = _compilerParams.IncludeDebugInformation;
            copy.TreatWarningsAsErrors = _compilerParams.TreatWarningsAsErrors;
            copy.WarningLevel = _compilerParams.WarningLevel;
            copy.CompilerOptions = _compilerParams.CompilerOptions;
            return copy;
        }

        private String[] CloneCompilerLanguages() {
            String[] compilerLanguages = new String[_compilerLanguages.Length]; 
            Array.Copy(_compilerLanguages, compilerLanguages, _compilerLanguages.Length);
            return compilerLanguages;
        }

        private String[] CloneCompilerExtensions() {
            String[] compilerExtensions = new String[_compilerExtensions.Length]; 
            Array.Copy(_compilerExtensions, compilerExtensions, _compilerExtensions.Length);
            return compilerExtensions;
        }

        internal CompilerParameters CompilerParams {
            get {
                return _compilerParams;
            }
        }
    }
}

