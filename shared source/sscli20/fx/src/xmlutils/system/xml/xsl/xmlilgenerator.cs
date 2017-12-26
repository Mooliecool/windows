//------------------------------------------------------------------------------
// <copyright file="XmlIlGenerator.cs" company="Microsoft">
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
// <spec>http://webdata/xml/specs/querylowlevel.xml</spec>
//------------------------------------------------------------------------------
using System;
using System.Xml;
using System.Xml.XPath;
using System.Xml.Schema;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using System.IO;
using System.Reflection;
using System.Reflection.Emit;
using System.Xml.Xsl.Qil;
using System.Xml.Xsl.IlGen;
using System.Xml.Xsl.Runtime;

namespace System.Xml.Xsl {

    internal delegate void ExecuteDelegate(XmlQueryRuntime runtime);


    /// <summary>
    /// This internal class is the entry point for creating Msil assemblies from QilExpression.
    /// </summary>
    /// <remarks>
    /// Generate will return an AssemblyBuilder with the following setup:
    /// Assembly Name = "MS.Internal.Xml.CompiledQuery"
    /// Module Dll Name = "MS.Internal.Xml.CompiledQuery.dll"
    /// public class MS.Internal.Xml.CompiledQuery.Test {
    ///     public static void Execute(XmlQueryRuntime runtime);
    ///     public static void Root(XmlQueryRuntime runtime);
    ///     private static ... UserMethod1(XmlQueryRuntime runtime, ...);
    ///     ...
    ///     private static ... UserMethodN(XmlQueryRuntime runtime, ...);
    /// }
    ///
    /// XmlILGenerator incorporates a number of different technologies in order to generate efficient code that avoids caching
    /// large result sets in memory:
    ///
    /// 1. Code Iterators - Query results are computed using a set of composable, interlocking iterators that alone perform a
    /// simple task, but together execute complex queries.  The iterators are actually little blocks of code
    /// that are connected to each other using a series of jumps.  Because each iterator is not instantiated
    /// as a separate object, the number of objects and number of function calls is kept to a minimum during
    /// execution.  Also, large result sets are often computed incrementally, with each iterator performing one step in a
    /// pipeline of sequence items.
    ///
    /// 2. Analyzers - During code generation, QilToMsil traverses the semantic tree representation of the query (QIL) several times.
    /// As visits to each node in the tree start and end, various Analyzers are invoked.  These Analyzers incrementally
    /// collect and store information that is later used to generate faster and smaller code.
    /// </remarks>
    internal class XmlILGenerator {
        private QilExpression qil;
        private GenerateHelper helper;
        private XmlILOptimizerVisitor optVisitor;
        private XmlILVisitor xmlIlVisitor;
        private XmlILModule module;

        /// <summary>
        /// Always output debug information in debug mode.
        /// </summary>
        public XmlILGenerator() {
        }

        /// <summary>
        /// Given the logical query plan (QilExpression) generate a physical query plan (MSIL) that can be executed.
        /// </summary>
        public XmlCommand Generate(QilExpression query, AssemblyName asmName) {
            MethodInfo methRoot, methExec;
            bool useLRE, emitSymbols;
            ExecuteDelegate delExec;
            XmlILMethodAttributes methAttrs;

            this.qil = query;

            useLRE = !this.qil.IsDebug && (asmName == null);
            emitSymbols = this.qil.IsDebug;

            // In debug code, ensure that input QIL is correct
            QilValidationVisitor.Validate(this.qil);

            // Trace Qil before optimization
            XmlILTrace.WriteQil(this.qil, "qilbefore.xml");

            // Trace optimizations
            XmlILTrace.TraceOptimizations(this.qil, "qilopt.xml");

            if (XmlILTrace.IsEnabled) {
                // Dump assembly to disk; can't do this when using LRE
                useLRE = false;
            }

            // Optimize and annotate the Qil graph
            this.optVisitor = new XmlILOptimizerVisitor(this.qil, !this.qil.IsDebug);
            this.qil = this.optVisitor.Optimize();

            // In debug code, ensure that output QIL is correct
            QilValidationVisitor.Validate(this.qil);

            // Trace Qil after optimization
            XmlILTrace.WriteQil(this.qil, "qilafter.xml");

            // Create module in which methods will be generated
            this.module = new XmlILModule(useLRE, emitSymbols, asmName);

            // Create a code generation helper for the module; enable optimizations if IsDebug is false
            this.helper = new GenerateHelper(this.module, this.qil.IsDebug);

            // Create helper methods
            CreateHelperFunctions();

            // Create metadata for the root expression
            // public void Root()
            Debug.Assert(this.qil.Root != null);
            methAttrs = (this.qil.Root.SourceLine == null) ? XmlILMethodAttributes.NonUser : XmlILMethodAttributes.None;
            methRoot = this.module.DefineMethod("Root", typeof(void), new Type[] {}, new string[] {}, methAttrs);

            // Create metadata for each QilExpression function that has at least one caller
            CreateFunctionMetadata(this.qil.FunctionList);

            // Create metadata for each QilExpression global variable and parameter
            CreateGlobalValueMetadata(this.qil.GlobalVariableList);
            CreateGlobalValueMetadata(this.qil.GlobalParameterList);

            // Create Execute method
            methExec = CreateExecuteFunction(methRoot);

            // Visit the QilExpression graph
            this.xmlIlVisitor = new XmlILVisitor();
            this.xmlIlVisitor.Visit(this.qil, this.helper, methRoot);

            this.module.BakeMethods();

            // Create delegate over "Execute" method
            delExec = (ExecuteDelegate) this.module.CreateDelegate("Execute", typeof(ExecuteDelegate));

            return new XmlILCommand(delExec, this.qil, this.helper.StaticData);
        }

        /// <summary>
        /// Create MethodBuilder metadata for the specified QilExpression function.  Annotate ndFunc with the
        /// MethodBuilder.  Also, each QilExpression argument type should be converted to a corresponding Clr type.
        /// Each argument QilExpression node should be annotated with the resulting ParameterBuilder.
        /// </summary>
        private void CreateFunctionMetadata(IList<QilNode> funcList) {
            MethodInfo methInfo;
            Type[] paramTypes;
            string[] paramNames;
            Type typReturn;
            XmlILMethodAttributes methAttrs;

            foreach (QilFunction ndFunc in funcList) {
                paramTypes = new Type[ndFunc.Arguments.Count];
                paramNames = new string[ndFunc.Arguments.Count];

                // Loop through all other parameters and save their types in the array
                for (int arg = 0; arg < ndFunc.Arguments.Count; arg ++) {
                    QilParameter ndParam = (QilParameter) ndFunc.Arguments[arg];
                    Debug.Assert(ndParam.NodeType == QilNodeType.Parameter);

                    // Get the type of each argument as a Clr type
                    paramTypes[arg] = XmlILTypeHelper.GetStorageType(ndParam.XmlType);

                    // Get the name of each argument
                    if (ndParam.DebugName != null)
                        paramNames[arg] = ndParam.DebugName;
                }

                // Get the type of the return value
                if (XmlILConstructInfo.Read(ndFunc).PushToWriterLast) {
                    // Push mode functions do not have a return value
                    typReturn = typeof(void);
                }
                else {
                    // Pull mode functions have a return value
                    typReturn = XmlILTypeHelper.GetStorageType(ndFunc.XmlType);
                }

                // Create the method metadata
                methAttrs = ndFunc.SourceLine == null ? XmlILMethodAttributes.NonUser : XmlILMethodAttributes.None;
                methInfo = this.module.DefineMethod(ndFunc.DebugName, typReturn, paramTypes, paramNames, methAttrs);

                for (int arg = 0; arg < ndFunc.Arguments.Count; arg ++) {
                    // Set location of parameter on Let node annotation
                    XmlILAnnotation.Write(ndFunc.Arguments[arg]).ArgumentPosition = arg;
                }

                // Annotate function with the MethodInfo
                XmlILAnnotation.Write(ndFunc).FunctionBinding = methInfo;
            }
        }

        /// <summary>
        /// Generate metadata for a method that calculates a global value.
        /// </summary>
        private void CreateGlobalValueMetadata(IList<QilNode> globalList) {
            MethodInfo methInfo;
            Type typReturn;
            XmlILMethodAttributes methAttrs;

            foreach (QilReference ndRef in globalList) {
                // public T GlobalValue()
                typReturn = XmlILTypeHelper.GetStorageType(ndRef.XmlType);
                methAttrs = ndRef.SourceLine == null ? XmlILMethodAttributes.NonUser : XmlILMethodAttributes.None;
                methInfo = this.module.DefineMethod(ndRef.DebugName.ToString(), typReturn, new Type[] {}, new string[] {}, methAttrs);

                // Annotate function with MethodBuilder
                XmlILAnnotation.Write(ndRef).FunctionBinding = methInfo;
            }
        }

        /// <summary>
        /// Create and generate the "Execute" method, which is the entry point to the query.
        /// </summary>
        private MethodInfo CreateExecuteFunction(MethodInfo methRoot) {
            MethodInfo methExec;

            // public static void Execute(XmlQueryRuntime);
            methExec = this.module.DefineMethod(
                            "Execute",
                            typeof(void),
                            new Type[] {},
                            new string[] {},
                            XmlILMethodAttributes.NonUser);

            this.helper.MethodBegin(methExec, null, false);

            // Force some or all global values to be evaluated at start of query
            EvaluateGlobalValues(this.qil.GlobalVariableList);
            EvaluateGlobalValues(this.qil.GlobalParameterList);

            // Root(runtime);
            this.helper.LoadQueryRuntime();
            this.helper.Call(methRoot);

            this.helper.MethodEnd();

            return methExec;
        }

        /// <summary>
        /// Create and generate various helper methods, which are called by the generated code.
        /// </summary>
        private void CreateHelperFunctions() {
            MethodInfo meth;
            Label lblClone;

            // public static XPathNavigator SyncToNavigator(XPathNavigator, XPathNavigator);
            meth = this.module.DefineMethod(
                            "SyncToNavigator",
                            typeof(XPathNavigator),
                            new Type[] {typeof(XPathNavigator), typeof(XPathNavigator)},
                            new string[] {null, null},
                            XmlILMethodAttributes.NonUser | XmlILMethodAttributes.Raw);

            this.helper.MethodBegin(meth, null, false);

            // if (navigatorThis != null && navigatorThis.MoveTo(navigatorThat))
            //     return navigatorThis;
            lblClone = this.helper.DefineLabel();
            this.helper.Emit(OpCodes.Ldarg_0);
            this.helper.Emit(OpCodes.Brfalse, lblClone);
            this.helper.Emit(OpCodes.Ldarg_0);
            this.helper.Emit(OpCodes.Ldarg_1);
            this.helper.Call(XmlILMethods.NavMoveTo);
            this.helper.Emit(OpCodes.Brfalse, lblClone);
            this.helper.Emit(OpCodes.Ldarg_0);
            this.helper.Emit(OpCodes.Ret);

            // LabelClone:
            // return navigatorThat.Clone();
            this.helper.MarkLabel(lblClone);
            this.helper.Emit(OpCodes.Ldarg_1);
            this.helper.Call(XmlILMethods.NavClone);

            this.helper.MethodEnd();
        }

        /// <summary>
        /// Generate code to force evaluation of some or all global variables and/or parameters.
        /// </summary>
        private void EvaluateGlobalValues(IList<QilNode> iterList) {
            MethodInfo methInfo;

            foreach (QilIterator ndIter in iterList) {
                // Evaluate global if generating debug code, or if global might have side effects
                if (this.qil.IsDebug || OptimizerPatterns.Read(ndIter).MatchesPattern(OptimizerPatternName.MaybeSideEffects)) {
                    // Get MethodInfo that evaluates the global value and discard its return value
                    methInfo = XmlILAnnotation.Write(ndIter).FunctionBinding;
                    Debug.Assert(methInfo != null, "MethodInfo for global value should have been created previously.");

                    this.helper.LoadQueryRuntime();
                    this.helper.Call(methInfo);
                    this.helper.Emit(OpCodes.Pop);
                }
            }
        }
    }


    /// <summary>
    /// This is a simple implementation of MessageEventArgs.
    /// </summary>
    internal class XmlILQueryEventArgs : XsltMessageEncounteredEventArgs {
        private string message;

        public XmlILQueryEventArgs(string message) {
            this.message = message;
        }

        public override string Message {
            get { return this.message; }
        }
    }


    /// <summary>
    /// This is the executable command generated by the XmlILGenerator.
    /// </summary>
    internal class XmlILCommand : XmlCommand {
        private XsltMessageEncounteredEventHandler onMessage;
        private ExecuteDelegate delExec;
        private XmlWriterSettings defaultWriterSettings;
        private IList<WhitespaceRule> wsRules;
        private string[] globalNames;
        private string[] names;
        private StringPair[][] prefixMappingsList;
        private Int32Pair[] filters;
        private XmlQueryType[] types;
        private XmlCollation[] collations;
        private EarlyBoundInfo[] earlyInfo;

        /// <summary>
        /// Constructor.
        /// </summary>
        public XmlILCommand(ExecuteDelegate delExec, QilExpression qil, StaticDataManager staticData) {
            Debug.Assert(qil != null);
            this.delExec = delExec;
            this.defaultWriterSettings = qil.DefaultWriterSettings;
            this.wsRules = qil.WhitespaceRules;
            this.names = staticData.Names;
            this.prefixMappingsList = staticData.PrefixMappingsList;
            this.filters = staticData.NameFilters;
            this.types = staticData.XmlTypes;
            this.collations = staticData.Collations;
            this.globalNames = staticData.GlobalNames;
            this.earlyInfo = staticData.EarlyBound;
        }

        /// <summary>
        /// Return the rules used for whitespace stripping/preservation.
        /// </summary>
        public IList<WhitespaceRule> WhitespaceRules {
            get { return this.wsRules; }
        }

        /// <summary>
        /// Return names of all global variables and parameters used by this query.
        /// </summary>
        public string[] GlobalNames {
            get { return this.globalNames; }
        }

        /// <summary>
        /// Return array of names used by this query.
        /// </summary>
        public string[] Names {
            get { return this.names; }
        }

        /// <summary>
        /// Return array of prefix mappings used by this query.
        /// </summary>
        public StringPair[][] PrefixMappingsList {
            get { return this.prefixMappingsList; }
        }

        /// <summary>
        /// Return array of name filter specifications used by this query.
        /// </summary>
        public Int32Pair[] Filters {
            get { return this.filters; }
        }

        /// <summary>
        /// Return array of types used by this query.
        /// </summary>
        public XmlQueryType[] Types {
            get { return this.types; }
        }

        /// <summary>
        /// Return array of collations used by this query.
        /// </summary>
        public XmlCollation[] Collations {
            get { return this.collations; }
        }

        /// <summary>
        /// Return array of early bound object information used by this query.
        /// </summary>
        public EarlyBoundInfo[] EarlyBound {
            get { return this.earlyInfo; }
        }

        /// <summary>
        /// Fire the OnMessageEvent, passing the specified text as the message.
        /// </summary>
        public void FireOnMessageEvent(string message) {
            if (this.onMessage != null)
                this.onMessage(this, new XmlILQueryEventArgs(message));
            else
                Console.WriteLine(message);
        }

        /// <summary>
        /// Default document as XPathNavigator.
        /// </summary>
        public override void Execute(IXPathNavigable contextDocument, XmlResolver dataSources, XsltArgumentList argumentList, XmlWriter results) {
            if (results == null)
                throw new ArgumentNullException("results");

            if (contextDocument != null)
                Execute(contextDocument.CreateNavigator(), dataSources, argumentList, results, false);
            else
                Execute(null, dataSources, argumentList, results, false);
        }

        /// <summary>
        /// Default document as XPathNavigator.
        /// </summary>
        public override void Execute(IXPathNavigable contextDocument, XmlResolver dataSources, XsltArgumentList argumentList, TextWriter results) {
            if (results == null)
                throw new ArgumentNullException("results");

            Execute(contextDocument, dataSources, argumentList, XmlWriter.Create(results, this.defaultWriterSettings));
        }

        /// <summary>
        /// Default document as XPathNavigator.
        /// </summary>
        public override void Execute(IXPathNavigable contextDocument, XmlResolver dataSources, XsltArgumentList argumentList, Stream results) {
            if (results == null)
                throw new ArgumentNullException("results");

            Execute(contextDocument, dataSources, argumentList, XmlWriter.Create(results, this.defaultWriterSettings));
        }

        /// <summary>
        /// Executes the query by accessing datasources via the XmlResolver and using run-time parameters
        /// as provided by the XsltArgumentList. The default document is mapped into the XmlResolver with the
        /// provided name. The results are output to the provided XmlWriter.
        /// </summary>
        public void Execute(string contextDocumentUri, XmlResolver dataSources, XsltArgumentList argumentList, XmlWriter results) {
            if (results == null)
                throw new ArgumentNullException("results");

            Execute(contextDocumentUri, dataSources, argumentList, results, false);
        }

        /// <summary>
        /// Executes the query by accessing datasources via the XmlResolver and using
        /// run-time parameters as provided by the XsltArgumentList. The default document
        /// is mapped into the XmlResolver with the provided name. The results are returned
        /// as an IList.
        /// </summary>
        public IList Evaluate(string contextDocumentUri, XmlResolver dataSources, XsltArgumentList argumentList) {
            XmlCachedSequenceWriter seqwrt = new XmlCachedSequenceWriter();
            Execute(contextDocumentUri, dataSources, argumentList, seqwrt);
            return seqwrt.ResultSequence;
        }

        /// <summary>
        /// Executes the query by accessing datasources via the XmlResolver and using run-time parameters
        /// as provided by the XsltArgumentList. The default document is mapped into the XmlResolver with the
        /// provided name. The results are output to the provided XmlWriter.
        /// </summary>
        public override void Execute(XmlReader contextDocument, XmlResolver dataSources, XsltArgumentList argumentList, XmlWriter results) {
            if (results == null)
                throw new ArgumentNullException("results");

            Execute(contextDocument, dataSources, argumentList, results, false);
        }

        /// <summary>
        /// Executes the query by accessing datasources via the XmlResolver and using run-time parameters
        /// as provided by the XsltArgumentList. The default document is mapped into the XmlResolver with the
        /// provided name. The results are output to the provided TextWriter.
        /// </summary>
        public override void Execute(XmlReader contextDocument, XmlResolver dataSources, XsltArgumentList argumentList, TextWriter results) {
            if (results == null)
                throw new ArgumentNullException("results");

            Execute(contextDocument, dataSources, argumentList, XmlWriter.Create(results, this.defaultWriterSettings), true);
        }

        /// <summary>
        /// Executes the query by accessing datasources via the XmlResolver and using run-time parameters
        /// as provided by the XsltArgumentList. The default document is mapped into the XmlResolver with the
        /// provided name. The results are output to the provided Stream.
        /// </summary>
        public override void Execute(XmlReader contextDocument, XmlResolver dataSources, XsltArgumentList argumentList, Stream results) {
            if (results == null)
                throw new ArgumentNullException("results");

            Execute(contextDocument, dataSources, argumentList, XmlWriter.Create(results, this.defaultWriterSettings), true);
        }
        /// <summary>
        /// Executes the query by accessing datasources via the XmlResolver and using
        /// run-time parameters as provided by the XsltArgumentList. The default document
        /// is mapped into the XmlResolver with the provided name. The results are returned
        /// as an IList.
        /// </summary>
        public override IList Evaluate(XmlReader contextDocument, XmlResolver dataSources, XsltArgumentList argumentList) {
            XmlCachedSequenceWriter seqwrt = new XmlCachedSequenceWriter();
            Execute(contextDocument, dataSources, argumentList, seqwrt);
            return seqwrt.ResultSequence;
        }

        /// <summary>
        /// Execute the dynamic assembly generated by the XmlILGenerator.
        /// </summary>
        private void Execute(object defaultDocument, XmlResolver dataSources, XsltArgumentList argumentList, XmlWriter writer, bool closeWriter) {
            try {
                // Try to extract a RawWriter
                XmlWellFormedWriter wellFormedWriter = writer as XmlWellFormedWriter;

                if (wellFormedWriter != null &&
                    wellFormedWriter.WriteState == WriteState.Start &&
                    wellFormedWriter.Settings.ConformanceLevel != ConformanceLevel.Document) {

                    // Extracted RawWriter from WellFormedWriter
                    Execute(defaultDocument, dataSources, argumentList, new XmlMergeSequenceWriter(wellFormedWriter.RawWriter));
                }
                else {
                    // Wrap Writer in RawWriter
                    Execute(defaultDocument, dataSources, argumentList, new XmlMergeSequenceWriter(new XmlRawWriterWrapper(writer)));
                }
            }
            finally {
                // Close Writers that are created by XmlILGenerator; flush external writers
                if (closeWriter)
                    writer.Close();
                else
                    writer.Flush();
            }
        }

        /// <summary>
        /// Execute the dynamic assembly generated by the XmlILGenerator.
        /// </summary>
        private void Execute(object defaultDocument, XmlResolver dataSources, XsltArgumentList argumentList, XmlSequenceWriter results) {
            Debug.Assert(results != null);

            // Set up message sink
            this.onMessage = argumentList != null ? argumentList.xsltMessageEncountered : null;

            // Ensure that dataSources is always non-null
            if (dataSources == null)
                dataSources = XmlNullResolver.Singleton;

            this.delExec(new XmlQueryRuntime(this, defaultDocument, dataSources, argumentList, results));
        }
    }
}
