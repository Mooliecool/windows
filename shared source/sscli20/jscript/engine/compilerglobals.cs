// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==

namespace Microsoft.JScript {
    
    using Microsoft.JScript.Vsa;
    using Microsoft.Vsa;
    using System;
    using System.Collections;
    using System.Configuration.Assemblies;
    using System.Diagnostics;
    using System.IO;
    using System.Reflection;
    using System.Reflection.Emit;
    using System.Threading;
    using System.Runtime.Remoting;
    using System.Runtime.CompilerServices;
    using System.Globalization;
    using System.Security.Policy;


    internal sealed class CompilerGlobals{
      internal Stack BreakLabelStack = new Stack();
      internal Stack ContinueLabelStack = new Stack();
      internal bool InsideProtectedRegion = false;
      internal bool InsideFinally = false;
      internal int FinallyStackTop = 0;
      
      internal ModuleBuilder module;
      internal AssemblyBuilder assemblyBuilder;
      internal TypeBuilder classwriter;
      internal TypeBuilder globalScopeClassWriter;
      
      internal SimpleHashtable documents = new SimpleHashtable(8);
      internal SimpleHashtable usedNames = new SimpleHashtable(32);

      // Saves the evidence used to define the assembly so that if the executionEvidence
      // is changed after Compile, the engine can determine that it must reload the assembly.
      internal Evidence compilationEvidence;
      
      
      internal CompilerGlobals(VsaEngine engine, String assemName, String assemblyFileName, PEFileKinds PEFileKind, bool save, bool run, bool debugOn, bool isCLSCompliant, Version version, Globals globals) {
        String moduleFileName = null;
        String directory = null; //Default has assembly stored in current directory
        
        if (assemblyFileName != null){
          //The directory is an absolute path where the assembly is written
          try{
            directory = Path.GetDirectoryName(Path.GetFullPath(assemblyFileName));
          }catch(Exception e){
            throw new VsaException(VsaError.AssemblyNameInvalid, assemblyFileName, e);
          }catch{
            throw new JScriptException(JSError.NonClsException);
          }
          
          //For a single file assembly module filename is set to assembly filename
          moduleFileName = Path.GetFileName(assemblyFileName);
          
          //If simple name is not specified, get it by extracting the basename from assembly name
          if (null == assemName || String.Empty == assemName){
            assemName = Path.GetFileName(assemblyFileName);
            if (Path.HasExtension(assemName))
              assemName = assemName.Substring(0, assemName.Length - Path.GetExtension(assemName).Length);
          }
        }
        
        //Setup default simple assembly name and module name for the case where no assemblyFileName is specified.
        if (assemName == null || assemName == String.Empty)
          assemName = "JScriptAssembly";
        if (moduleFileName == null) {
          if (PEFileKind == PEFileKinds.Dll)
            moduleFileName = "JScriptModule.dll";
          else
            moduleFileName = "JScriptModule.exe";
        }
        
        AssemblyName assemblyName = new AssemblyName();
        assemblyName.CodeBase = assemblyFileName;
        if (globals.assemblyCulture != null) assemblyName.CultureInfo = globals.assemblyCulture;
        assemblyName.Flags = AssemblyNameFlags.None;
        if ((globals.assemblyFlags & AssemblyFlags.PublicKey) != 0) assemblyName.Flags = AssemblyNameFlags.PublicKey;
        switch ((AssemblyFlags)(globals.assemblyFlags & AssemblyFlags.CompatibilityMask)){
          case AssemblyFlags.NonSideBySideAppDomain:
            assemblyName.VersionCompatibility = AssemblyVersionCompatibility.SameDomain; break;
          case AssemblyFlags.NonSideBySideMachine:
            assemblyName.VersionCompatibility = AssemblyVersionCompatibility.SameMachine; break;
          case AssemblyFlags.NonSideBySideProcess:
            assemblyName.VersionCompatibility = AssemblyVersionCompatibility.SameProcess; break;
          default:
            assemblyName.VersionCompatibility = (AssemblyVersionCompatibility)0; break;
        }

        assemblyName.HashAlgorithm = globals.assemblyHashAlgorithm;
        if (globals.assemblyKeyFileName != null){
          try
          {
            using (FileStream fs = new FileStream(globals.assemblyKeyFileName, FileMode.Open, FileAccess.Read))
            {
              StrongNameKeyPair keyPair = new StrongNameKeyPair(fs);

              if (globals.assemblyDelaySign)
                if (fs.Length == 160)
                {
                  Byte[] pkey = new Byte[160];
                  fs.Seek(0, SeekOrigin.Begin);
                  int len = fs.Read(pkey, 0, 160);
                  assemblyName.SetPublicKey(pkey);
                }
                else
                  assemblyName.SetPublicKey(keyPair.PublicKey);
              else {
                // Fetch to force reflection to try and process the key file data and throw an exception
                byte[] publicKey = keyPair.PublicKey; 
                assemblyName.KeyPair = keyPair;
              }
            }
          }
          catch
          {
            globals.assemblyKeyFileNameContext.HandleError(JSError.InvalidAssemblyKeyFile, globals.assemblyKeyFileName);
          }
        }else if (globals.assemblyKeyName != null){
          try{
            StrongNameKeyPair keyPair = new StrongNameKeyPair(globals.assemblyKeyName);
            byte[] publicKey = keyPair.PublicKey; // Force validation
            assemblyName.KeyPair = keyPair;
          }catch{
            globals.assemblyKeyNameContext.HandleError(JSError.InvalidAssemblyKeyFile, globals.assemblyKeyName);
          }
        }

        assemblyName.Name = assemName;
        if (version != null) assemblyName.Version = version;
        else if (globals.assemblyVersion != null) assemblyName.Version = globals.assemblyVersion;

        AssemblyBuilderAccess access = save ? (run ? AssemblyBuilderAccess.RunAndSave : AssemblyBuilderAccess.Save) : AssemblyBuilderAccess.Run;
        if (engine.ReferenceLoaderAPI == LoaderAPI.ReflectionOnlyLoadFrom)
          access = AssemblyBuilderAccess.ReflectionOnly;
        // Supply the evidence to assemblies built for VSA
        if (globals.engine.genStartupClass)
          this.assemblyBuilder = Thread.GetDomain().DefineDynamicAssembly(assemblyName, access, directory, globals.engine.Evidence);
        else
          this.assemblyBuilder = Thread.GetDomain().DefineDynamicAssembly(assemblyName, access, directory);
        if (save)
          this.module = this.assemblyBuilder.DefineDynamicModule("JScript Module", moduleFileName, debugOn);
        else
          this.module = this.assemblyBuilder.DefineDynamicModule("JScript Module", debugOn);
        if (isCLSCompliant)
          this.module.SetCustomAttribute(new CustomAttributeBuilder(CompilerGlobals.clsCompliantAttributeCtor, new Object[]{isCLSCompliant}));
        if (debugOn){
          ConstructorInfo debuggableAttr = Typeob.DebuggableAttribute.GetConstructor(new Type[] {Typeob.Boolean, Typeob.Boolean});
          assemblyBuilder.SetCustomAttribute(new CustomAttributeBuilder(debuggableAttr, 
            new Object[] {(globals.assemblyFlags & AssemblyFlags.EnableJITcompileTracking) != 0, 
                          (globals.assemblyFlags & AssemblyFlags.DisableJITcompileOptimizer) != 0}));
        }
        this.compilationEvidence = globals.engine.Evidence;
        this.classwriter = null;
      }
      
      
      // Member infos
      internal static MethodInfo constructArrayMethod                  { get { return Globals.TypeRefs.constructArrayMethod; } }
      internal static MethodInfo isMissingMethod                       { get { return Globals.TypeRefs.isMissingMethod; } }
      internal static ConstructorInfo bitwiseBinaryConstructor         { get { return Globals.TypeRefs.bitwiseBinaryConstructor; } }
      internal static MethodInfo evaluateBitwiseBinaryMethod           { get { return Globals.TypeRefs.evaluateBitwiseBinaryMethod; } }
      internal static ConstructorInfo breakOutOfFinallyConstructor     { get { return Globals.TypeRefs.breakOutOfFinallyConstructor; } }
      internal static ConstructorInfo closureConstructor               { get { return Globals.TypeRefs.closureConstructor; } }
      internal static ConstructorInfo continueOutOfFinallyConstructor  { get { return Globals.TypeRefs.continueOutOfFinallyConstructor; } }
      internal static MethodInfo checkIfDoubleIsIntegerMethod          { get { return Globals.TypeRefs.checkIfDoubleIsIntegerMethod; } }
      internal static MethodInfo checkIfSingleIsIntegerMethod          { get { return Globals.TypeRefs.checkIfSingleIsIntegerMethod; } }
      internal static MethodInfo coerce2Method                         { get { return Globals.TypeRefs.coerce2Method; } }
      internal static MethodInfo coerceTMethod                         { get { return Globals.TypeRefs.coerceTMethod; } }
      internal static MethodInfo throwTypeMismatch                     { get { return Globals.TypeRefs.throwTypeMismatch; } }
      internal static MethodInfo doubleToBooleanMethod                 { get { return Globals.TypeRefs.doubleToBooleanMethod; } }
      internal static MethodInfo toBooleanMethod                       { get { return Globals.TypeRefs.toBooleanMethod; } }  
      internal static MethodInfo toForInObjectMethod                   { get { return Globals.TypeRefs.toForInObjectMethod; } }  
      internal static MethodInfo toInt32Method                         { get { return Globals.TypeRefs.toInt32Method; } }
      internal static MethodInfo toNativeArrayMethod                   { get { return Globals.TypeRefs.toNativeArrayMethod; } }
      internal static MethodInfo toNumberMethod                        { get { return Globals.TypeRefs.toNumberMethod; } }
      internal static MethodInfo toObjectMethod                        { get { return Globals.TypeRefs.toObjectMethod; } }  
      internal static MethodInfo toObject2Method                       { get { return Globals.TypeRefs.toObject2Method; } }  
      internal static MethodInfo doubleToStringMethod                  { get { return Globals.TypeRefs.doubleToStringMethod; } }
      internal static MethodInfo toStringMethod                        { get { return Globals.TypeRefs.toStringMethod; } }
      internal static FieldInfo undefinedField                         { get { return Globals.TypeRefs.undefinedField; } }
      internal static ConstructorInfo equalityConstructor              { get { return Globals.TypeRefs.equalityConstructor; } }
      internal static MethodInfo evaluateEqualityMethod                { get { return Globals.TypeRefs.evaluateEqualityMethod; } }
      internal static MethodInfo jScriptEqualsMethod                   { get { return Globals.TypeRefs.jScriptEqualsMethod; } }
      internal static MethodInfo jScriptEvaluateMethod1                { get { return Globals.TypeRefs.jScriptEvaluateMethod1; } }
      internal static MethodInfo jScriptEvaluateMethod2                { get { return Globals.TypeRefs.jScriptEvaluateMethod2; } }
      internal static MethodInfo jScriptGetEnumeratorMethod            { get { return Globals.TypeRefs.jScriptGetEnumeratorMethod; } }
      internal static MethodInfo jScriptFunctionDeclarationMethod      { get { return Globals.TypeRefs.jScriptFunctionDeclarationMethod; } }
      internal static MethodInfo jScriptFunctionExpressionMethod       { get { return Globals.TypeRefs.jScriptFunctionExpressionMethod; } }
      internal static FieldInfo contextEngineField                     { get { return Globals.TypeRefs.contextEngineField; } }
      internal static MethodInfo fastConstructArrayLiteralMethod       { get { return Globals.TypeRefs.fastConstructArrayLiteralMethod; } }
      internal static ConstructorInfo globalScopeConstructor           { get { return Globals.TypeRefs.globalScopeConstructor; } }
      internal static MethodInfo getDefaultThisObjectMethod            { get { return Globals.TypeRefs.getDefaultThisObjectMethod; } }
      internal static MethodInfo getFieldMethod                        { get { return Globals.TypeRefs.getFieldMethod; } }
      internal static MethodInfo getGlobalScopeMethod                  { get { return Globals.TypeRefs.getGlobalScopeMethod; } }
      internal static MethodInfo getMemberValueMethod                  { get { return Globals.TypeRefs.getMemberValueMethod; } }
      internal static MethodInfo jScriptImportMethod                   { get { return Globals.TypeRefs.jScriptImportMethod; } }
      internal static MethodInfo jScriptInMethod                       { get { return Globals.TypeRefs.jScriptInMethod; } }
      internal static MethodInfo getEngineMethod                       { get { return Globals.TypeRefs.getEngineMethod; } }
      internal static MethodInfo setEngineMethod                       { get { return Globals.TypeRefs.setEngineMethod; } }
      internal static MethodInfo jScriptInstanceofMethod               { get { return Globals.TypeRefs.jScriptInstanceofMethod; } }
      internal static ConstructorInfo scriptExceptionConstructor       { get { return Globals.TypeRefs.scriptExceptionConstructor; } }
      internal static ConstructorInfo jsFunctionAttributeConstructor   { get { return Globals.TypeRefs.jsFunctionAttributeConstructor; } }
      internal static ConstructorInfo jsLocalFieldConstructor          { get { return Globals.TypeRefs.jsLocalFieldConstructor; } }
      internal static MethodInfo setMemberValue2Method                 { get { return Globals.TypeRefs.setMemberValue2Method; } }
      internal static ConstructorInfo lateBindingConstructor2          { get { return Globals.TypeRefs.lateBindingConstructor2; } }
      internal static ConstructorInfo lateBindingConstructor           { get { return Globals.TypeRefs.lateBindingConstructor; } }
      internal static FieldInfo objectField                            { get { return Globals.TypeRefs.objectField; } }
      internal static MethodInfo callMethod                            { get { return Globals.TypeRefs.callMethod; } }
      internal static MethodInfo callValueMethod                       { get { return Globals.TypeRefs.callValueMethod; } }
      internal static MethodInfo callValue2Method                      { get { return Globals.TypeRefs.callValue2Method; } }
      internal static MethodInfo deleteMethod                          { get { return Globals.TypeRefs.deleteMethod; } }
      internal static MethodInfo deleteMemberMethod                    { get { return Globals.TypeRefs.deleteMemberMethod; } }
      internal static MethodInfo getNonMissingValueMethod              { get { return Globals.TypeRefs.getNonMissingValueMethod; } }
      internal static MethodInfo getValue2Method                       { get { return Globals.TypeRefs.getValue2Method; } }
      internal static MethodInfo setIndexedPropertyValueStaticMethod   { get { return Globals.TypeRefs.setIndexedPropertyValueStaticMethod; } }
      internal static MethodInfo setValueMethod                        { get { return Globals.TypeRefs.setValueMethod; } }
      internal static FieldInfo missingField                           { get { return Globals.TypeRefs.missingField; } }
      internal static MethodInfo getNamespaceMethod                    { get { return Globals.TypeRefs.getNamespaceMethod; } }
      internal static ConstructorInfo numericBinaryConstructor         { get { return Globals.TypeRefs.numericBinaryConstructor; } }
      internal static MethodInfo numericbinaryDoOpMethod               { get { return Globals.TypeRefs.numericbinaryDoOpMethod; } }
      internal static MethodInfo evaluateNumericBinaryMethod           { get { return Globals.TypeRefs.evaluateNumericBinaryMethod; } }
      internal static ConstructorInfo numericUnaryConstructor          { get { return Globals.TypeRefs.numericUnaryConstructor; } }
      internal static MethodInfo evaluateUnaryMethod                   { get { return Globals.TypeRefs.evaluateUnaryMethod; } }
      internal static MethodInfo constructObjectMethod                 { get { return Globals.TypeRefs.constructObjectMethod; } }
      internal static MethodInfo jScriptPackageMethod                  { get { return Globals.TypeRefs.jScriptPackageMethod; } }
      internal static ConstructorInfo plusConstructor                  { get { return Globals.TypeRefs.plusConstructor; } }
      internal static MethodInfo plusDoOpMethod                        { get { return Globals.TypeRefs.plusDoOpMethod; } }
      internal static MethodInfo evaluatePlusMethod                    { get { return Globals.TypeRefs.evaluatePlusMethod; } }
      internal static ConstructorInfo postOrPrefixConstructor          { get { return Globals.TypeRefs.postOrPrefixConstructor; } }
      internal static MethodInfo evaluatePostOrPrefixOperatorMethod    { get { return Globals.TypeRefs.evaluatePostOrPrefixOperatorMethod; } }
      internal static ConstructorInfo referenceAttributeConstructor    { get { return Globals.TypeRefs.referenceAttributeConstructor; } }
      internal static MethodInfo regExpConstructMethod                 { get { return Globals.TypeRefs.regExpConstructMethod; } }
      internal static ConstructorInfo relationalConstructor            { get { return Globals.TypeRefs.relationalConstructor; } }
      internal static MethodInfo evaluateRelationalMethod              { get { return Globals.TypeRefs.evaluateRelationalMethod; } }
      internal static MethodInfo jScriptCompareMethod                  { get { return Globals.TypeRefs.jScriptCompareMethod; } }
      internal static ConstructorInfo returnOutOfFinallyConstructor    { get { return Globals.TypeRefs.returnOutOfFinallyConstructor; } }
      internal static MethodInfo doubleToInt64                         { get { return Globals.TypeRefs.doubleToInt64; } }
      internal static MethodInfo uncheckedDecimalToInt64Method         { get { return Globals.TypeRefs.uncheckedDecimalToInt64Method; } }
      internal static FieldInfo engineField                            { get { return Globals.TypeRefs.engineField; } }
      internal static MethodInfo getParentMethod                       { get { return Globals.TypeRefs.getParentMethod; } }
      internal static MethodInfo writeMethod                           { get { return Globals.TypeRefs.writeMethod; } }
      internal static MethodInfo writeLineMethod                       { get { return Globals.TypeRefs.writeLineMethod; } }
      internal static ConstructorInfo hashtableCtor                    { get { return Globals.TypeRefs.hashtableCtor; } }
      internal static MethodInfo hashtableGetItem                      { get { return Globals.TypeRefs.hashtableGetItem; } }
      internal static MethodInfo hashTableGetEnumerator                { get { return Globals.TypeRefs.hashTableGetEnumerator; } }
      internal static MethodInfo hashtableRemove                       { get { return Globals.TypeRefs.hashtableRemove; } }
      internal static MethodInfo hashtableSetItem                      { get { return Globals.TypeRefs.hashtableSetItem; } }
      internal static FieldInfo closureInstanceField                   { get { return Globals.TypeRefs.closureInstanceField; } }
      internal static FieldInfo localVarsField                         { get { return Globals.TypeRefs.localVarsField; } }
      internal static MethodInfo pushStackFrameForMethod               { get { return Globals.TypeRefs.pushStackFrameForMethod; } }
      internal static MethodInfo pushStackFrameForStaticMethod         { get { return Globals.TypeRefs.pushStackFrameForStaticMethod; } }
      internal static MethodInfo jScriptStrictEqualsMethod             { get { return Globals.TypeRefs.jScriptStrictEqualsMethod; } }
      internal static MethodInfo jScriptThrowMethod                    { get { return Globals.TypeRefs.jScriptThrowMethod; } }
      internal static MethodInfo jScriptExceptionValueMethod           { get { return Globals.TypeRefs.jScriptExceptionValueMethod; } }
      internal static MethodInfo jScriptTypeofMethod                   { get { return Globals.TypeRefs.jScriptTypeofMethod; } }
      internal static ConstructorInfo vsaEngineConstructor             { get { return Globals.TypeRefs.vsaEngineConstructor; } }
      internal static MethodInfo createVsaEngine                       { get { return Globals.TypeRefs.createVsaEngine; } }
      internal static MethodInfo createVsaEngineWithType               { get { return Globals.TypeRefs.createVsaEngineWithType; } }
      internal static MethodInfo getOriginalArrayConstructorMethod     { get { return Globals.TypeRefs.getOriginalArrayConstructorMethod; } }
      internal static MethodInfo getOriginalObjectConstructorMethod    { get { return Globals.TypeRefs.getOriginalObjectConstructorMethod; } }
      internal static MethodInfo getOriginalRegExpConstructorMethod    { get { return Globals.TypeRefs.getOriginalRegExpConstructorMethod; } }
      internal static MethodInfo popScriptObjectMethod                 { get { return Globals.TypeRefs.popScriptObjectMethod; } }
      internal static MethodInfo pushScriptObjectMethod                { get { return Globals.TypeRefs.pushScriptObjectMethod; } }
      internal static MethodInfo scriptObjectStackTopMethod            { get { return Globals.TypeRefs.scriptObjectStackTopMethod; } }
      internal static MethodInfo getLenientGlobalObjectMethod          { get { return Globals.TypeRefs.getLenientGlobalObjectMethod; } }
      internal static MethodInfo jScriptWithMethod                     { get { return Globals.TypeRefs.jScriptWithMethod; } }      
      
      internal static ConstructorInfo clsCompliantAttributeCtor        { get { return Globals.TypeRefs.clsCompliantAttributeCtor; } }
      internal static MethodInfo getEnumeratorMethod                   { get { return Globals.TypeRefs.getEnumeratorMethod; } }
      internal static MethodInfo moveNextMethod                        { get { return Globals.TypeRefs.moveNextMethod; } }
      internal static MethodInfo getCurrentMethod                      { get { return Globals.TypeRefs.getCurrentMethod; } }
      internal static ConstructorInfo contextStaticAttributeCtor       { get { return Globals.TypeRefs.contextStaticAttributeCtor; } }
      internal static MethodInfo changeTypeMethod                      { get { return Globals.TypeRefs.changeTypeMethod; } }
      internal static MethodInfo convertCharToStringMethod             { get { return Globals.TypeRefs.convertCharToStringMethod; } }
      internal static ConstructorInfo dateTimeConstructor              { get { return Globals.TypeRefs.dateTimeConstructor; } }
      internal static MethodInfo dateTimeToStringMethod                { get { return Globals.TypeRefs.dateTimeToStringMethod; } }
      internal static MethodInfo dateTimeToInt64Method                 { get { return Globals.TypeRefs.dateTimeToInt64Method; } }
      internal static ConstructorInfo decimalConstructor               { get { return Globals.TypeRefs.decimalConstructor; } }
      internal static FieldInfo decimalZeroField                       { get { return Globals.TypeRefs.decimalZeroField; } }
      internal static MethodInfo decimalCompare                        { get { return Globals.TypeRefs.decimalCompare; } }
      internal static MethodInfo doubleToDecimalMethod                 { get { return Globals.TypeRefs.doubleToDecimalMethod; } }
      internal static MethodInfo int32ToDecimalMethod                  { get { return Globals.TypeRefs.int32ToDecimalMethod; } }
      internal static MethodInfo int64ToDecimalMethod                  { get { return Globals.TypeRefs.int64ToDecimalMethod; } }
      internal static MethodInfo uint32ToDecimalMethod                 { get { return Globals.TypeRefs.uint32ToDecimalMethod; } }
      internal static MethodInfo uint64ToDecimalMethod                 { get { return Globals.TypeRefs.uint64ToDecimalMethod; } }
      internal static MethodInfo decimalToDoubleMethod                 { get { return Globals.TypeRefs.decimalToDoubleMethod; } }
      internal static MethodInfo decimalToInt32Method                  { get { return Globals.TypeRefs.decimalToInt32Method; } }
      internal static MethodInfo decimalToInt64Method                  { get { return Globals.TypeRefs.decimalToInt64Method; } }
      internal static MethodInfo decimalToStringMethod                 { get { return Globals.TypeRefs.decimalToStringMethod; } }
      internal static MethodInfo decimalToUInt32Method                 { get { return Globals.TypeRefs.decimalToUInt32Method; } }
      internal static MethodInfo decimalToUInt64Method                 { get { return Globals.TypeRefs.decimalToUInt64Method; } }
      internal static MethodInfo debugBreak                            { get { return Globals.TypeRefs.debugBreak; } }
      internal static ConstructorInfo debuggerHiddenAttributeCtor      { get { return Globals.TypeRefs.debuggerHiddenAttributeCtor; } }
      internal static ConstructorInfo debuggerStepThroughAttributeCtor { get { return Globals.TypeRefs.debuggerStepThroughAttributeCtor; } }
      internal static MethodInfo int32ToStringMethod                   { get { return Globals.TypeRefs.int32ToStringMethod; } }
      internal static MethodInfo int64ToStringMethod                   { get { return Globals.TypeRefs.int64ToStringMethod; } } 
      internal static MethodInfo equalsMethod                          { get { return Globals.TypeRefs.equalsMethod; } }
      internal static ConstructorInfo defaultMemberAttributeCtor       { get { return Globals.TypeRefs.defaultMemberAttributeCtor; } }
      internal static MethodInfo getFieldValueMethod                   { get { return Globals.TypeRefs.getFieldValueMethod; } }
      internal static MethodInfo setFieldValueMethod                   { get { return Globals.TypeRefs.setFieldValueMethod; } }
      internal static FieldInfo systemReflectionMissingField           { get { return Globals.TypeRefs.systemReflectionMissingField; } }
      internal static ConstructorInfo compilerGlobalScopeAttributeCtor { get { return Globals.TypeRefs.compilerGlobalScopeAttributeCtor; } }
      internal static MethodInfo stringConcatArrMethod                 { get { return Globals.TypeRefs.stringConcatArrMethod; } }
      internal static MethodInfo stringConcat4Method                   { get { return Globals.TypeRefs.stringConcat4Method; } }
      internal static MethodInfo stringConcat3Method                   { get { return Globals.TypeRefs.stringConcat3Method; } }
      internal static MethodInfo stringConcat2Method                   { get { return Globals.TypeRefs.stringConcat2Method; } }
      internal static MethodInfo stringEqualsMethod                    { get { return Globals.TypeRefs.stringEqualsMethod; } }
      internal static MethodInfo stringLengthMethod                    { get { return Globals.TypeRefs.stringLengthMethod; } }
      internal static MethodInfo getMethodMethod                       { get { return Globals.TypeRefs.getMethodMethod; } }
      internal static MethodInfo getTypeMethod                         { get { return Globals.TypeRefs.getTypeMethod; } }
      internal static MethodInfo getTypeFromHandleMethod               { get { return Globals.TypeRefs.getTypeFromHandleMethod; } }
      internal static MethodInfo uint32ToStringMethod                  { get { return Globals.TypeRefs.uint32ToStringMethod; } }
      internal static MethodInfo uint64ToStringMethod                  { get { return Globals.TypeRefs.uint64ToStringMethod; } }
   }  
}
