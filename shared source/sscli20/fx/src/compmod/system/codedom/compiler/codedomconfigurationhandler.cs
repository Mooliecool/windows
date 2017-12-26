//------------------------------------------------------------------------------
// <copyright file="CodeDomCompilationConfiguration.cs" company="Microsoft">
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

/*
 * Code related to the <assemblies> config section
 *
 * Copyright (c) 1999 Microsoft Corporation
 */

namespace System.CodeDom.Compiler {

    using System;
    using System.CodeDom;
    using System.Collections;
    using System.Collections.Specialized;
    using System.Configuration;
    using System.IO;
    using System.Reflection;
    using System.Runtime.Serialization.Formatters;
    using System.Xml;
    using System.Globalization;
    
    internal class CodeDomCompilationConfiguration {

        internal const String sectionName = "system.codedom";

        static readonly char[] s_fieldSeparators = new char[] {';'};

        // _compilerLanguages : Hashtable <string, CompilerInfo>
        internal Hashtable _compilerLanguages;

        // _compilerExtensions : Hashtable <string, CompilerInfo>
        internal Hashtable _compilerExtensions;
        internal ArrayList _allCompilerInfo;
        
        private static CodeDomCompilationConfiguration defaultInstance = new CodeDomCompilationConfiguration();
        
        internal static CodeDomCompilationConfiguration Default {
            get {
                return defaultInstance;
            }
        }

        internal CodeDomCompilationConfiguration() {
            // First time initialization. This must be kept consistent with machine.config.comments in that it 
            // must initialize the config system as if that block was present.

            _compilerLanguages = new Hashtable(StringComparer.OrdinalIgnoreCase);
            _compilerExtensions = new Hashtable(StringComparer.OrdinalIgnoreCase);
            _allCompilerInfo = new ArrayList();            
            
            CompilerInfo compilerInfo;
            CompilerParameters compilerParameters;
            String typeName;
            
            // C#
            compilerParameters = new CompilerParameters();
            compilerParameters.WarningLevel = 4;
            typeName = "Microsoft.CSharp.CSharpCodeProvider, " + AssemblyRef.System;
            compilerInfo = new CompilerInfo(compilerParameters, typeName);
            compilerInfo._compilerLanguages = new string[] {"c#", "cs", "csharp"};
            compilerInfo._compilerExtensions = new string[] {".cs", "cs"};
            AddCompilerInfo(compilerInfo);


            // JScript
            compilerParameters = new CompilerParameters();
            compilerParameters.WarningLevel = 4;
            typeName = "Microsoft.JScript.JScriptCodeProvider, " + AssemblyRef.MicrosoftJScript;
            compilerInfo = new CompilerInfo(compilerParameters, typeName);
            compilerInfo._compilerLanguages = new string[] {"js", "jscript", "javascript"};
            compilerInfo._compilerExtensions = new string[] {".js", "js"};
            AddCompilerInfo(compilerInfo);


        }

        private CodeDomCompilationConfiguration(CodeDomCompilationConfiguration original) {
            if (original._compilerLanguages != null)
                _compilerLanguages  = (Hashtable)original._compilerLanguages.Clone();

            if (original._compilerExtensions != null)
                _compilerExtensions = (Hashtable)original._compilerExtensions.Clone();

            if (original. _allCompilerInfo != null)
                _allCompilerInfo = (ArrayList)original._allCompilerInfo.Clone();
        }
        
        private void AddCompilerInfo(CompilerInfo compilerInfo) {
        
            foreach (string language in compilerInfo._compilerLanguages) {
                _compilerLanguages[language] = compilerInfo;
            }

            foreach (string extension in compilerInfo._compilerExtensions) {
                _compilerExtensions[extension] = compilerInfo;
            }
            
            _allCompilerInfo.Add(compilerInfo);            
        }
        
        private void RemoveUnmapped() {
            // Allow config compilers to replace redundant compiler entries
            
            // clear out the mapped marker
            for (int i = 0; i < _allCompilerInfo.Count; i++) {
                ((CompilerInfo)_allCompilerInfo[i])._mapped = false;
            }
            
            // Re-mark only the ones that still have a mapping
            foreach (CompilerInfo destinationCompilerInfo in _compilerLanguages.Values) {
                destinationCompilerInfo._mapped = true;
            }
            foreach (CompilerInfo destinationCompilerInfo in _compilerExtensions.Values) {
                destinationCompilerInfo._mapped = true;
            }
            
            // Remove the ones that were not marked
            for (int i = _allCompilerInfo.Count - 1; i >= 0; i--) {
                if (!((CompilerInfo)_allCompilerInfo[i])._mapped) {
                    _allCompilerInfo.RemoveAt(i);
                }
            }
        }


        internal class SectionHandler {
            private SectionHandler () {
            }

            internal static object CreateStatic(object inheritedObject, XmlNode node) {
                CodeDomCompilationConfiguration inherited = (CodeDomCompilationConfiguration)inheritedObject;
                CodeDomCompilationConfiguration result;

                if (inherited == null)
                    result = new CodeDomCompilationConfiguration();
                else
                    result = new CodeDomCompilationConfiguration(inherited);

                HandlerBase.CheckForUnrecognizedAttributes(node);

                //
                // Handle child elements (if they exist)
                //   - compilers
                //
                foreach (XmlNode child in node.ChildNodes) {

                    // skip whitespace and comments
                    // reject nonelements
                    if (HandlerBase.IsIgnorableAlsoCheckForNonElement(child))
                        continue;

                    // handle <compilers>
                    if (child.Name == "compilers") {
                        ProcessCompilersElement(result, child);
                    }
                    else {
                        HandlerBase.ThrowUnrecognizedElement(child);
                    }
                }

                return result;
            }

            private static void ProcessCompilersElement(CodeDomCompilationConfiguration result, XmlNode node) {

                // reject attributes
                HandlerBase.CheckForUnrecognizedAttributes(node);

                String configFile = ConfigurationErrorsException.GetFilename(node);

                foreach(XmlNode child in node.ChildNodes) {
                    int configLineNumber = ConfigurationErrorsException.GetLineNumber(child);

                    // skip whitespace and comments
                    // reject nonelements
                    if (HandlerBase.IsIgnorableAlsoCheckForNonElement(child))
                        continue;

                    if (child.Name != "compiler") {
                        HandlerBase.ThrowUnrecognizedElement(child);
                    }

                    String languages = String.Empty;
                    XmlNode languageNode = HandlerBase.GetAndRemoveRequiredNonEmptyStringAttribute(child, "language", ref languages);
                    String extensions = String.Empty;
                    HandlerBase.GetAndRemoveRequiredNonEmptyStringAttribute(child, "extension", ref extensions);
                    String compilerTypeName = null;
                    HandlerBase.GetAndRemoveRequiredNonEmptyStringAttribute(child, "type", ref compilerTypeName);

                    // Create a CompilerParameters for this compiler.
                    CompilerParameters compilerParams = new CompilerParameters();

                    int warningLevel = 0;
                    if (HandlerBase.GetAndRemoveNonNegativeIntegerAttribute(child, "warningLevel", ref warningLevel) != null) {
                        compilerParams.WarningLevel = warningLevel;

                        // Need to be false if the warning level is 0
                        compilerParams.TreatWarningsAsErrors = (warningLevel>0);
                    }
                    String compilerOptions = null;
                    if (HandlerBase.GetAndRemoveStringAttribute(child, "compilerOptions", ref compilerOptions) != null) {
                        compilerParams.CompilerOptions = compilerOptions;
                    }

                    HandlerBase.CheckForUnrecognizedAttributes(child);
                    HandlerBase.CheckForChildNodes(child);

                    // Create a CompilerInfo structure for this compiler
                    CompilerInfo compilerInfo = new CompilerInfo(compilerParams, compilerTypeName);
                    compilerInfo.configFileName = configFile;
                    compilerInfo.configFileLineNumber = configLineNumber;

                    // Parse the semicolon separated lists
                    string[] languageList = languages.Split(s_fieldSeparators);
                    string[] extensionList = extensions.Split(s_fieldSeparators);

                    for( int i =0 ; i < languageList.Length; i++) {
                        languageList[i] = languageList[i].Trim();
                    }

                    for( int i =0 ; i < extensionList.Length; i++) {
                        extensionList[i] = extensionList[i].Trim();
                    }


                    // Validate language names, language names must have length and extensions must start with a period.
                    foreach (string language in languageList) {
                        if (language.Length == 0)
                            throw new ConfigurationErrorsException(SR.GetString(SR.Language_Names_Cannot_Be_Empty));
                    }

                    foreach (string extension in extensionList) {
                        if (extension.Length == 0 || extension[0] != '.')
                            throw new ConfigurationErrorsException(SR.GetString(SR.Extension_Names_Cannot_Be_Empty_Or_Non_Period_Based));
                    }

                    
                    compilerInfo._compilerLanguages = languageList;
                    compilerInfo._compilerExtensions = extensionList;
                    
                    result.AddCompilerInfo(compilerInfo);
                }
                // Allow config options to replace redundant compiler entries
                result.RemoveUnmapped();
            }
        }
    }

    internal class CodeDomConfigurationHandler : IConfigurationSectionHandler {
        internal CodeDomConfigurationHandler() {
        }

        public virtual object Create(object inheritedObject, object configContextObj, XmlNode node) {
            return CodeDomCompilationConfiguration.SectionHandler.CreateStatic(inheritedObject, node);
        }
    }




    internal static class HandlerBase {

        //
        // XML Attribute Helpers
        //

        private static XmlNode GetAndRemoveAttribute(XmlNode node, string attrib, bool fRequired) {
            XmlNode a = node.Attributes.RemoveNamedItem(attrib);

            // If the attribute is required and was not present, throw
            if (fRequired && a == null) {
                throw new ConfigurationErrorsException(
                    SR.GetString(SR.Config_missing_required_attribute, attrib, node.Name), node);
            }

            return a;
        }

        private static XmlNode GetAndRemoveStringAttributeInternal(XmlNode node, string attrib, bool fRequired, ref string val) {
            XmlNode a = GetAndRemoveAttribute(node, attrib, fRequired);
            if (a != null)
                val = a.Value;

            return a;
        }

        internal static XmlNode GetAndRemoveStringAttribute(XmlNode node, string attrib, ref string val) {
            return GetAndRemoveStringAttributeInternal(node, attrib, false /*fRequired*/, ref val);
        }

        internal static XmlNode GetAndRemoveRequiredNonEmptyStringAttribute(XmlNode node, string attrib, ref string val) {
            return GetAndRemoveNonEmptyStringAttributeInternal(node, attrib, true /*fRequired*/, ref val);
        }

        private static XmlNode GetAndRemoveNonEmptyStringAttributeInternal(XmlNode node, string attrib, bool fRequired, ref string val) {
            XmlNode a = GetAndRemoveStringAttributeInternal(node, attrib, fRequired, ref val);
            if (a != null && val.Length == 0) {
                throw new ConfigurationErrorsException(
                    SR.GetString(SR.Empty_attribute, attrib), a);
            }

            return a;
        }

        private static XmlNode GetAndRemoveIntegerAttributeInternal(XmlNode node, string attrib, bool fRequired, ref int val) {
            XmlNode a = GetAndRemoveAttribute(node, attrib, fRequired);
            if (a != null) {
                if (a.Value.Trim() != a.Value) {
                    throw new ConfigurationErrorsException(SR.GetString(SR.Config_invalid_integer_attribute, a.Name),a);
                }

                try {
                    val = int.Parse(a.Value, CultureInfo.InvariantCulture);
                }
                catch (Exception e) {
                    throw new ConfigurationErrorsException(
                        SR.GetString(SR.Config_invalid_integer_attribute, a.Name), e, a);
                }
            }

            return a;
        }

        private static XmlNode GetAndRemoveNonNegativeAttributeInternal(XmlNode node, string attrib, bool fRequired, ref int val) {
            XmlNode a = GetAndRemoveIntegerAttributeInternal(node, attrib, fRequired, ref val);

            if (a != null && val < 0) {
                throw new ConfigurationErrorsException(
                    SR.GetString(SR.Invalid_nonnegative_integer_attribute, attrib), a);
            }

            return a;
        }

        internal static XmlNode GetAndRemoveNonNegativeIntegerAttribute(XmlNode node, string attrib, ref int val) {
            return GetAndRemoveNonNegativeAttributeInternal(node, attrib, false /*fRequired*/, ref val);
        }

        internal static void CheckForUnrecognizedAttributes(XmlNode node) {
            if (node.Attributes.Count != 0) {
                throw new ConfigurationErrorsException(
                                SR.GetString(SR.Config_base_unrecognized_attribute, node.Attributes[0].Name),
                                node.Attributes[0]);
            }
        }

        //
        // XML Element Helpers
        //

        internal static void CheckForNonElement(XmlNode node) {
            if (node.NodeType != XmlNodeType.Element) {
                throw new ConfigurationErrorsException(
                                SR.GetString(SR.Config_base_elements_only),
                                node);                
            }
        }


        internal static bool IsIgnorableAlsoCheckForNonElement(XmlNode node) {
            if (node.NodeType == XmlNodeType.Comment || node.NodeType == XmlNodeType.Whitespace) {
                return true;
            }

            CheckForNonElement(node);

            return false;
        }

        internal static void CheckForChildNodes(XmlNode node) {
            if (node.HasChildNodes) {
                throw new ConfigurationErrorsException(
                                SR.GetString(SR.Config_base_no_child_nodes),
                                node.FirstChild);                
            }
        }

        internal static void ThrowUnrecognizedElement(XmlNode node) {
            throw new ConfigurationErrorsException(
                            SR.GetString(SR.Config_base_unrecognized_element),
                            node);
        }

    }
}


