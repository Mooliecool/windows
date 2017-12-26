//------------------------------------------------------------------------------
// <copyright file="XmlILModule.cs" company="Microsoft">
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
using System;
using System.IO;
using System.Threading;
using System.Reflection;
using System.Reflection.Emit;
using System.CodeDom.Compiler;
using System.Diagnostics;
using System.Collections;
using System.Security;
using System.Security.Policy;
using System.Security.Permissions;
using System.Diagnostics.SymbolStore;
using System.Xml.Xsl.Runtime;

namespace System.Xml.Xsl.IlGen {
    using DebuggingModes = DebuggableAttribute.DebuggingModes;

    internal enum XmlILMethodAttributes {
        None = 0,
        NonUser = 1,    // Non-user method which should debugger should step through
        Raw = 2,        // Raw method which should not add an implicit first argument of type XmlQueryRuntime
    }

    internal class XmlILModule {
        private static readonly PermissionSet CreateDelegatePermissionSet;  // Permission set that contains ReflectionEmit [MemberAccess] permissions
        private static readonly PermissionSet CreateModulePermissionSet;    // Permission set that contains ReflectionEmit permissions
        private static readonly PermissionSet CreateMethodPermissionSet;    // Permission set that contains ReflectionEmit and ControlEvidence permissions
        private static long AssemblyId;                                     // Unique identifier used to ensure that assembly names are unique within AppDomain
        private static ModuleBuilder LREModule;                             // Module used to emit dynamic lightweight-reflection-emit (LRE) methods

        private TypeBuilder typeBldr;
        private Hashtable methods, urlToSymWriter;
        private string modFile;
        private bool persistAsm, useLRE, emitSymbols;

        private static readonly Guid LanguageGuid = new Guid(0x462d4a3e, 0xb257, 0x4aee, 0x97, 0xcd, 0x59, 0x18, 0xc7, 0x53, 0x17, 0x58);
        private static readonly Guid VendorGuid = new Guid(0x994b45c4, 0xe6e9, 0x11d2, 0x90, 0x3f, 0x00, 0xc0, 0x4f, 0xa3, 0x02, 0xa1);
        private const string RuntimeName = "{" + XmlReservedNs.NsXslDebug + "}" + "runtime";

        static XmlILModule() {
            AssemblyName asmName;
            AssemblyBuilder asmBldr;

            CreateDelegatePermissionSet = new PermissionSet(PermissionState.None);
            CreateDelegatePermissionSet.AddPermission(new ReflectionPermission(ReflectionPermissionFlag.MemberAccess));

            CreateModulePermissionSet = new PermissionSet(PermissionState.None);
            CreateModulePermissionSet.AddPermission(new ReflectionPermission(ReflectionPermissionFlag.ReflectionEmit));

            CreateMethodPermissionSet = new PermissionSet(PermissionState.None);
            CreateMethodPermissionSet.AddPermission(new SecurityPermission(SecurityPermissionFlag.ControlEvidence));
            CreateMethodPermissionSet.AddPermission(new ReflectionPermission(ReflectionPermissionFlag.ReflectionEmit));

            AssemblyId = 0;

            // 1. LRE assembly only needs to execute
            // 2. No temp files need be created
            // 3. Never allow assembly to Assert permissions
            asmName = CreateAssemblyName();
            asmBldr = AppDomain.CurrentDomain.DefineDynamicAssembly(asmName, AssemblyBuilderAccess.Run);

            // Add custom attribute to assembly marking it as security transparent so that Assert will not be allowed
            // and link demands will be converted to full demands.
            asmBldr.SetCustomAttribute(new CustomAttributeBuilder(XmlILConstructors.Transparent, new object[] {}));

            try {
                // DefineDynamicModule demands ReflectionEmit permission
                CreateModulePermissionSet.Assert();

                // Store LREModule once.  If multiple threads are doing this, then some threads might get different
                // modules.  This is OK, since it's not mandatory to share, just preferable.
                LREModule = asmBldr.DefineDynamicModule("System.Xml.Xsl.CompiledQuery", false);
            }
            finally {
                CodeAccessPermission.RevertAssert();
            }
        }

        public XmlILModule(bool useLRE, bool emitSymbols, AssemblyName asmName) {
            AssemblyBuilder asmBldr;
            ModuleBuilder modBldr;
            Debug.Assert(!useLRE || (!emitSymbols && (asmName == null)));

            this.useLRE = useLRE;
            this.emitSymbols = emitSymbols;
            this.persistAsm = (asmName != null);

            // Index all methods added to this module by unique name
            this.methods = new Hashtable();

            if (!useLRE) {
                // 1. If assembly needs to support debugging, then it must be saved and re-loaded (rule of CLR)
                // 2. Get path of temp directory, where assembly will be saved
                // 3. Never allow assembly to Assert permissions
                if (asmName != null) {
                    this.modFile = asmName.Name;
                }
                else {
                    asmName = CreateAssemblyName();
                    if (XmlILTrace.IsEnabled) {
                        this.modFile = "System.Xml.Xsl.CompiledQuery";
                        this.persistAsm = true;
                    }
                }

                asmBldr = AppDomain.CurrentDomain.DefineDynamicAssembly(
                            asmName,
                            this.persistAsm ? AssemblyBuilderAccess.RunAndSave : AssemblyBuilderAccess.Run);

                // Add custom attribute to assembly marking it as security transparent so that Assert will not be allowed
                // and link demands will be converted to full demands.
                asmBldr.SetCustomAttribute(new CustomAttributeBuilder(XmlILConstructors.Transparent, new object[] { }));

                if (emitSymbols) {
                    // Create mapping from source document to symbol writer
                    this.urlToSymWriter = new Hashtable();

                    // Add DebuggableAttribute to assembly so that debugging is a better experience
                    DebuggingModes debuggingModes = DebuggingModes.Default | DebuggingModes.IgnoreSymbolStoreSequencePoints | DebuggingModes.DisableOptimizations;
                    asmBldr.SetCustomAttribute(new CustomAttributeBuilder(XmlILConstructors.Debuggable, new object[] {debuggingModes}));
                }

                // Create ModuleBuilder
                if (this.persistAsm)
                    modBldr = asmBldr.DefineDynamicModule("System.Xml.Xsl.CompiledQuery", this.modFile + ".dll", emitSymbols);
                else
                    modBldr = asmBldr.DefineDynamicModule("System.Xml.Xsl.CompiledQuery", emitSymbols);

                this.typeBldr = modBldr.DefineType("System.Xml.Xsl.CompiledQuery.Query", TypeAttributes.Public);
            }
        }

        /// <summary>
        /// Define a method in this module with the specified name and parameters.
        /// </summary>
        public MethodInfo DefineMethod(string name, Type returnType, Type[] paramTypes, string[] paramNames, XmlILMethodAttributes xmlAttrs) {
            MethodInfo methResult;
            int uniqueId = 1;
            string nameOrig = name;
            Type[] paramTypesNew;
            bool isRaw = (xmlAttrs & XmlILMethodAttributes.Raw) != 0;

            // Ensure that name is unique
            while (this.methods[name] != null) {
                // Add unique id to end of name in order to make it unique within this module
                uniqueId++;
                name = nameOrig + " (" + uniqueId + ")";
            }

            if (!isRaw) {
                // XmlQueryRuntime is always 0th parameter
                paramTypesNew = new Type[paramTypes.Length + 1];
                paramTypesNew[0] = typeof(XmlQueryRuntime);
                Array.Copy(paramTypes, 0, paramTypesNew, 1, paramTypes.Length);
                paramTypes = paramTypesNew;
            }

            if (!this.useLRE) {
                MethodBuilder methBldr;

                methBldr = this.typeBldr.DefineMethod(
                            name,
                            MethodAttributes.Public | MethodAttributes.Static,
                            returnType,
                            paramTypes);

                if (emitSymbols && (xmlAttrs & XmlILMethodAttributes.NonUser) != 0) {
                    // Add DebuggerStepThroughAttribute and DebuggerNonUserCodeAttribute to non-user methods so that debugging is a better experience
                    methBldr.SetCustomAttribute(new CustomAttributeBuilder(XmlILConstructors.StepThrough, new object[] {}));
                    methBldr.SetCustomAttribute(new CustomAttributeBuilder(XmlILConstructors.NonUserCode, new object[] {}));
                }

                if (!isRaw)
                    methBldr.DefineParameter(1, ParameterAttributes.None, RuntimeName);

                for (int i = 0; i < paramNames.Length; i++) {
                    if (paramNames[i] != null && paramNames[i].Length != 0)
                        methBldr.DefineParameter(i + (isRaw ? 1 : 2), ParameterAttributes.None, paramNames[i]);
                }

                methResult = methBldr;
            }
            else {
                try {
                    // DynamicMethod constructor demands ReflectionEmit and ControlEvidence permission
                    CreateMethodPermissionSet.Assert();

                    DynamicMethod methDyn = new DynamicMethod(name, returnType, paramTypes, LREModule);
                    methDyn.InitLocals = true;

                    if (!isRaw)
                        methDyn.DefineParameter(1, ParameterAttributes.None, RuntimeName);

                    for (int i = 0; i < paramNames.Length; i++) {
                        if (paramNames[i] != null && paramNames[i].Length != 0)
                            methDyn.DefineParameter(i + (isRaw ? 1 : 2), ParameterAttributes.None, paramNames[i]);
                    }

                    methResult = methDyn;
                }
                finally {
                    CodeAccessPermission.RevertAssert();
                }
            }

            // Index method by name
            this.methods[name] = methResult;
            return methResult;
        }

        /// <summary>
        /// Get an XmlILGenerator that can be used to generate the body of the specified method.
        /// </summary>
        public static ILGenerator DefineMethodBody(MethodInfo methInfo) {
            DynamicMethod methDyn = methInfo as DynamicMethod;

            if (methDyn != null)
                return methDyn.GetILGenerator();

            return ((MethodBuilder) methInfo).GetILGenerator();
        }

        /// <summary>
        /// Find a MethodInfo of the specified name and return it.  Return null if no such method exists.
        /// </summary>
        public MethodInfo FindMethod(string name) {
            return (MethodInfo) this.methods[name];
        }

        /// <summary>
        /// Add the file name of a document containing source code for this module and return a symbol writer.
        /// </summary>
        public ISymbolDocumentWriter AddSourceDocument(string fileName) {
            ISymbolDocumentWriter symDoc;
            Debug.Assert(this.emitSymbols, "Cannot add source information to a module that doesn't allow symbols.");

            symDoc = this.urlToSymWriter[fileName] as ISymbolDocumentWriter;
            if (symDoc == null) {
                symDoc = ((ModuleBuilder) this.typeBldr.Module).DefineDocument(fileName, LanguageGuid, VendorGuid, Guid.Empty);
                this.urlToSymWriter.Add(fileName, symDoc);
            }

            return symDoc;
        }

        /// <summary>
        /// Once all methods have been defined, CreateModule must be called in order to "bake" the methods within
        /// this module.
        /// </summary>
        public void BakeMethods() {
            Type typBaked;
            Hashtable methodsBaked;

            if (!this.useLRE) {
                typBaked = this.typeBldr.CreateType();

                if (this.persistAsm) {
                    // Persist the assembly to disk
                    ((AssemblyBuilder) this.typeBldr.Module.Assembly).Save(this.modFile + ".dll");
                }

                // Replace all MethodInfos in this.methods
                methodsBaked = new Hashtable(this.methods.Count);
                foreach (string methName in this.methods.Keys) {
                    methodsBaked[methName] = typBaked.GetMethod(methName);
                }
                this.methods = methodsBaked;

                // Release TypeBuilder and symbol writer resources
                this.typeBldr = null;
                this.urlToSymWriter = null;
            }
        }

        /// <summary>
        /// Wrap a delegate around a MethodInfo of the specified name and type and return it.
        /// </summary>
        public Delegate CreateDelegate(string name, Type typDelegate) {
            try {
                // CreateDelegate demands MemberAccess permission
                CreateDelegatePermissionSet.Assert();

                if (!this.useLRE)
                    return Delegate.CreateDelegate(typDelegate, (MethodInfo) this.methods[name]);

                return ((DynamicMethod) this.methods[name]).CreateDelegate(typDelegate);
            }
            finally {
                CodeAccessPermission.RevertAssert();
            }
        }

        /// <summary>
        /// Define unique assembly name (within AppDomain).
        /// </summary>
        private static AssemblyName CreateAssemblyName() {
            AssemblyName name;

            Interlocked.Increment(ref AssemblyId);
            name = new AssemblyName();
            name.Name = "System.Xml.Xsl.CompiledQuery." + AssemblyId;

            return name;
        }
    }
}
