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
  using System;
  using System.Reflection;
  using Microsoft.JScript.Vsa;
  
  internal sealed class TypeReferences {
    internal TypeReferences(Module jscriptReferenceModule) {
      this._jscriptReferenceModule = jscriptReferenceModule;
      this._typeTable = new Type[(int)TypeReferenceArrayLength];
    }    

    // PREDEFINED TYPES    
    // GetPredefinedType uses the following mapping of predefined type names to actual types.
    // Note that types in mscorlib live in both the execution and reflection only loader
    // contexts and thus do not have to be mapped via GetTypeReference.
    private static readonly SimpleHashtable _predefinedTypeTable = new SimpleHashtable(34); 
    
    internal Type GetPredefinedType(string typeName) { 
      Object value = TypeReferences._predefinedTypeTable[typeName];
      Type predefinedType = value as Type;
      if (predefinedType == null) {
        if (value is TypeReference)
          predefinedType = this.GetTypeReference((TypeReference)value);
      }
      return predefinedType;
    }      
    
    static TypeReferences() {
      _predefinedTypeTable["boolean"] = typeof(System.Boolean);
      _predefinedTypeTable["byte"   ] = typeof(System.Byte);
      _predefinedTypeTable["char"   ] = typeof(System.Char);
      _predefinedTypeTable["decimal"] = typeof(System.Decimal);
      _predefinedTypeTable["double" ] = typeof(System.Double);
      _predefinedTypeTable["float"  ] = typeof(System.Single);
      _predefinedTypeTable["int"    ] = typeof(System.Int32);
      _predefinedTypeTable["long"   ] = typeof(System.Int64);
      _predefinedTypeTable["sbyte"  ] = typeof(System.SByte);
      _predefinedTypeTable["short"  ] = typeof(System.Int16);
      _predefinedTypeTable["void"   ] = typeof(void);
      _predefinedTypeTable["uint"   ] = typeof(System.UInt32);
      _predefinedTypeTable["ulong"  ] = typeof(System.UInt64);
      _predefinedTypeTable["ushort" ] = typeof(System.UInt16);

      _predefinedTypeTable["ActiveXObject" ] = typeof(System.Object);
      _predefinedTypeTable["Boolean"       ] = typeof(System.Boolean);
      _predefinedTypeTable["Number"        ] = typeof(System.Double);
      _predefinedTypeTable["Object"        ] = typeof(System.Object);
      _predefinedTypeTable["String"        ] = typeof(System.String);
      _predefinedTypeTable["Type"          ] = typeof(System.Type);
      
      _predefinedTypeTable["Array"         ] = TypeReference.ArrayObject;
      _predefinedTypeTable["Date"          ] = TypeReference.DateObject;
      _predefinedTypeTable["Enumerator"    ] = TypeReference.EnumeratorObject;
      _predefinedTypeTable["Error"         ] = TypeReference.ErrorObject;
      _predefinedTypeTable["EvalError"     ] = TypeReference.EvalErrorObject;
      _predefinedTypeTable["Function"      ] = TypeReference.ScriptFunction;
      _predefinedTypeTable["RangeError"    ] = TypeReference.RangeErrorObject;
      _predefinedTypeTable["ReferenceError"] = TypeReference.ReferenceErrorObject;
      _predefinedTypeTable["RegExp"        ] = TypeReference.RegExpObject;
      _predefinedTypeTable["SyntaxError"   ] = TypeReference.SyntaxErrorObject;
      _predefinedTypeTable["TypeError"     ] = TypeReference.TypeErrorObject;
      _predefinedTypeTable["URIError"      ] = TypeReference.URIErrorObject;
      _predefinedTypeTable["VBArray"       ] = TypeReference.VBArrayObject;
    }


    
    // TYPE AND MEMBER REFERENCES
    private Type[] _typeTable; // Size is TypeReference.Length
    private Module _jscriptReferenceModule;    
    private Module JScriptReferenceModule  { get { return this._jscriptReferenceModule; } }
          
    
    // Get the type associated with the TypeReference.
    private Type GetTypeReference(TypeReference typeRef) {
      Type type = this._typeTable[(int)typeRef];
      if (null == type) {
        string prefix = "Microsoft.JScript.";
        if ((int)typeRef >= TypeReferenceStartOfSpecialCases) {
          // Special Cases
          switch (typeRef) {
            case TypeReference.BaseVsaStartup: prefix = "Microsoft.Vsa."; break;
            case TypeReference.VsaEngine: prefix = "Microsoft.JScript.Vsa."; break;
          }
        }
        
        type = this.JScriptReferenceModule.GetType(prefix+System.Enum.GetName(typeof(TypeReference), (int)typeRef));
        this._typeTable[(int)typeRef] = type;
      }
      return type;
    }

    // This is the first value in TypeReference which is handled specially by GetTypeReference.
    private const int TypeReferenceStartOfSpecialCases = (int)TypeReference.BaseVsaStartup; 
    private const int TypeReferenceArrayLength = (int)TypeReference.VsaEngine+1;
    
    private enum TypeReference {
      // GetTypeReference will get prepended with "Microsoft.JScript." to generate the full type name.
      ArgumentsObject,
      ArrayConstructor,
      ArrayObject,
      ArrayWrapper,
      Binding,
      BitwiseBinary,
      BooleanObject,
      BreakOutOfFinally,
      BuiltinFunction,
      ClassScope,
      Closure,
      ContinueOutOfFinally,
      Convert,
      DateObject,
      Empty,
      EnumeratorObject,
      Equality,
      ErrorObject,
      Eval,
      EvalErrorObject,
      Expando,
      FieldAccessor,
      ForIn,
      FunctionDeclaration,
      FunctionExpression,
      FunctionObject,
      FunctionWrapper,
      GlobalObject,
      GlobalScope,
      Globals,
      Hide,
      IActivationObject,
      INeedEngine,
      Import,
      In,
      Instanceof,
      JSError,
      JSFunctionAttribute,
      JSFunctionAttributeEnum,
      JSLocalField,
      JSObject,
      JScriptException,
      LateBinding,
      LenientGlobalObject,
      MathObject,
      MethodInvoker,
      Missing,
      Namespace,
      NotRecommended,
      NumberObject,
      NumericBinary,
      NumericUnary,
      ObjectConstructor,
      Override,
      Package,
      Plus,
      PostOrPrefixOperator,
      RangeErrorObject,
      ReferenceAttribute,
      ReferenceErrorObject,
      RegExpConstructor,
      RegExpObject,
      Relational,
      ReturnOutOfFinally,
      Runtime,
      ScriptFunction,
      ScriptObject,
      ScriptStream,
      SimpleHashtable,
      StackFrame,
      StrictEquality,
      StringObject,
      SyntaxErrorObject,
      Throw,
      Try,
      TypedArray,
      TypeErrorObject,
      Typeof,
      URIErrorObject,
      VBArrayObject,
      With,
      
      // GetTypeReference has a switch to check for the special cases that follow.
      BaseVsaStartup,
      VsaEngine
    }

    // [Microsoft.JScript]Microsoft.JScript
    internal Type ArgumentsObject { get { return GetTypeReference(TypeReference.ArgumentsObject); } }
    internal Type ArrayConstructor { get { return GetTypeReference(TypeReference.ArrayConstructor); } }
    internal Type ArrayObject { get { return GetTypeReference(TypeReference.ArrayObject); } }
    internal Type ArrayWrapper { get { return GetTypeReference(TypeReference.ArrayWrapper); } }
    internal Type BaseVsaStartup { get { return GetTypeReference(TypeReference.BaseVsaStartup); } }
    internal Type Binding { get { return GetTypeReference(TypeReference.Binding); } }
    internal Type BitwiseBinary { get { return GetTypeReference(TypeReference.BitwiseBinary); } }
    internal Type BooleanObject { get { return GetTypeReference(TypeReference.BooleanObject); } }
    internal Type BreakOutOfFinally { get { return GetTypeReference(TypeReference.BreakOutOfFinally); } }
    internal Type BuiltinFunction { get { return GetTypeReference(TypeReference.BuiltinFunction); } }
    internal Type ClassScope { get { return GetTypeReference(TypeReference.ClassScope); } }
    internal Type Closure { get { return GetTypeReference(TypeReference.Closure); } }
    internal Type ContinueOutOfFinally { get { return GetTypeReference(TypeReference.ContinueOutOfFinally); } }
    internal Type Convert { get { return GetTypeReference(TypeReference.Convert); } }
    internal Type DateObject { get { return GetTypeReference(TypeReference.DateObject); } }
    internal Type Empty { get { return GetTypeReference(TypeReference.Empty); } }
    internal Type EnumeratorObject { get { return GetTypeReference(TypeReference.EnumeratorObject); } }
    internal Type Equality { get { return GetTypeReference(TypeReference.Equality); } }
    internal Type ErrorObject { get { return GetTypeReference(TypeReference.ErrorObject); } }
    internal Type Eval { get { return GetTypeReference(TypeReference.Eval); } }
    internal Type EvalErrorObject { get { return GetTypeReference(TypeReference.EvalErrorObject); } }
    internal Type Expando { get { return GetTypeReference(TypeReference.Expando); } }
    internal Type FieldAccessor { get { return GetTypeReference(TypeReference.FieldAccessor); } }
    internal Type ForIn { get { return GetTypeReference(TypeReference.ForIn); } }
    internal Type FunctionDeclaration { get { return GetTypeReference(TypeReference.FunctionDeclaration); } }
    internal Type FunctionExpression { get { return GetTypeReference(TypeReference.FunctionExpression); } }
    internal Type FunctionObject { get { return GetTypeReference(TypeReference.FunctionObject); } }
    internal Type FunctionWrapper { get { return GetTypeReference(TypeReference.FunctionWrapper); } }
    internal Type GlobalObject { get { return GetTypeReference(TypeReference.GlobalObject); } }
    internal Type GlobalScope { get { return GetTypeReference(TypeReference.GlobalScope); } }
    internal Type Globals { get { return GetTypeReference(TypeReference.Globals); } }
    internal Type Hide { get { return GetTypeReference(TypeReference.Hide); } }
    internal Type IActivationObject { get { return GetTypeReference(TypeReference.IActivationObject); } }
    internal Type INeedEngine { get { return GetTypeReference(TypeReference.INeedEngine); } }
    internal Type Import { get { return GetTypeReference(TypeReference.Import); } }
    internal Type In { get { return GetTypeReference(TypeReference.In); } }
    internal Type Instanceof { get { return GetTypeReference(TypeReference.Instanceof); } }
    internal Type JSError { get { return GetTypeReference(TypeReference.JSError); } }
    internal Type JSFunctionAttribute { get { return GetTypeReference(TypeReference.JSFunctionAttribute); } }
    internal Type JSFunctionAttributeEnum { get { return GetTypeReference(TypeReference.JSFunctionAttributeEnum); } }
    internal Type JSLocalField { get { return GetTypeReference(TypeReference.JSLocalField); } }
    internal Type JSObject { get { return GetTypeReference(TypeReference.JSObject); } }
    internal Type JScriptException { get { return GetTypeReference(TypeReference.JScriptException); } }
    internal Type LateBinding { get { return GetTypeReference(TypeReference.LateBinding); } }
    internal Type LenientGlobalObject { get { return GetTypeReference(TypeReference.LenientGlobalObject); } }
    internal Type MathObject { get { return GetTypeReference(TypeReference.MathObject); } }
    internal Type MethodInvoker { get { return GetTypeReference(TypeReference.MethodInvoker); } }
    internal Type Missing { get { return GetTypeReference(TypeReference.Missing); } }
    internal Type Namespace { get { return GetTypeReference(TypeReference.Namespace); } }
    internal Type NotRecommended { get { return GetTypeReference(TypeReference.NotRecommended); } }
    internal Type NumberObject { get { return GetTypeReference(TypeReference.NumberObject); } }
    internal Type NumericBinary { get { return GetTypeReference(TypeReference.NumericBinary); } }
    internal Type NumericUnary { get { return GetTypeReference(TypeReference.NumericUnary); } }
    internal Type ObjectConstructor { get { return GetTypeReference(TypeReference.ObjectConstructor); } }
    internal Type Override { get { return GetTypeReference(TypeReference.Override); } }
    internal Type Package { get { return GetTypeReference(TypeReference.Package); } }
    internal Type Plus { get { return GetTypeReference(TypeReference.Plus); } }
    internal Type PostOrPrefixOperator { get { return GetTypeReference(TypeReference.PostOrPrefixOperator); } }
    internal Type RangeErrorObject { get { return GetTypeReference(TypeReference.RangeErrorObject); } }
    internal Type ReferenceAttribute { get { return GetTypeReference(TypeReference.ReferenceAttribute); } }
    internal Type ReferenceErrorObject { get { return GetTypeReference(TypeReference.ReferenceErrorObject); } }
    internal Type RegExpConstructor { get { return GetTypeReference(TypeReference.RegExpConstructor); } }
    internal Type RegExpObject { get { return GetTypeReference(TypeReference.RegExpObject); } }
    internal Type Relational { get { return GetTypeReference(TypeReference.Relational); } }
    internal Type ReturnOutOfFinally { get { return GetTypeReference(TypeReference.ReturnOutOfFinally); } }
    internal Type Runtime { get { return GetTypeReference(TypeReference.Runtime); } }
    internal Type ScriptFunction { get { return GetTypeReference(TypeReference.ScriptFunction); } }
    internal Type ScriptObject { get { return GetTypeReference(TypeReference.ScriptObject); } }
    internal Type ScriptStream { get { return GetTypeReference(TypeReference.ScriptStream); } }
    internal Type SimpleHashtable { get { return GetTypeReference(TypeReference.SimpleHashtable); } }
    internal Type StackFrame { get { return GetTypeReference(TypeReference.StackFrame); } }
    internal Type StrictEquality { get { return GetTypeReference(TypeReference.StrictEquality); } }
    internal Type StringObject { get { return GetTypeReference(TypeReference.StringObject); } }
    internal Type SyntaxErrorObject { get { return GetTypeReference(TypeReference.SyntaxErrorObject); } }
    internal Type Throw { get { return GetTypeReference(TypeReference.Throw); } }
    internal Type Try { get { return GetTypeReference(TypeReference.Try); } }
    internal Type TypedArray { get { return GetTypeReference(TypeReference.TypedArray); } }
    internal Type TypeErrorObject { get { return GetTypeReference(TypeReference.TypeErrorObject); } }
    internal Type Typeof { get { return GetTypeReference(TypeReference.Typeof); } }
    internal Type URIErrorObject { get { return GetTypeReference(TypeReference.URIErrorObject); } }
    internal Type VBArrayObject { get { return GetTypeReference(TypeReference.VBArrayObject); } }
    internal Type With { get { return GetTypeReference(TypeReference.With); } }
    internal Type VsaEngine { get { return GetTypeReference(TypeReference.VsaEngine); } }    
    
    // The types in mscorlib live in both the execution and reflection only loader contexts
    // and thus do not need to be mapped.
    internal Type Array { get { return typeof(System.Array); } }
    internal Type Attribute { get { return typeof(System.Attribute); } }
    internal Type AttributeUsageAttribute { get { return typeof(System.AttributeUsageAttribute); } }
    internal Type Byte { get { return typeof(System.Byte); } }
    internal Type Boolean { get { return typeof(System.Boolean); } }
    internal Type Char { get { return typeof(System.Char); } }
    internal Type CLSCompliantAttribute { get { return typeof(System.CLSCompliantAttribute); } }
    internal Type ContextStaticAttribute { get { return typeof(System.ContextStaticAttribute); } }
    internal Type DateTime { get { return typeof(System.DateTime); } }
    internal Type DBNull { get { return typeof(System.DBNull); } }
    internal Type Delegate { get { return typeof(System.Delegate); } }
    internal Type Decimal { get { return typeof(System.Decimal); } }
    internal Type Double { get { return typeof(System.Double); } }
    internal Type Enum { get { return typeof(System.Enum); } }
    internal Type Exception { get { return typeof(System.Exception); } }
    internal Type IConvertible { get { return typeof(System.IConvertible); } }
    internal Type IntPtr { get { return typeof(System.IntPtr); } }
    internal Type Int16 { get { return typeof(System.Int16); } }
    internal Type Int32 { get { return typeof(System.Int32); } }
    internal Type Int64 { get { return typeof(System.Int64); } }
    internal Type Object { get { return typeof(System.Object); } }
    internal Type ObsoleteAttribute { get { return typeof(System.ObsoleteAttribute); } }
    internal Type ParamArrayAttribute { get { return typeof(System.ParamArrayAttribute); } }
    internal Type RuntimeTypeHandle { get { return typeof(System.RuntimeTypeHandle); } }
    internal Type SByte { get { return typeof(System.SByte); } }
    internal Type Single { get { return typeof(System.Single); } }
    internal Type String { get { return typeof(System.String); } }
    internal Type Type { get { return typeof(System.Type); } }
    internal Type TypeCode { get { return typeof(System.TypeCode); } }
    internal Type UIntPtr { get { return typeof(System.UIntPtr); } }
    internal Type UInt16 { get { return typeof(System.UInt16); } }
    internal Type UInt32 { get { return typeof(System.UInt32); } }
    internal Type UInt64 { get { return typeof(System.UInt64); } }
    internal Type ValueType { get { return typeof(System.ValueType); } }
    internal Type Void { get { return typeof(void); } }
    
    internal Type IEnumerable { get { return typeof(System.Collections.IEnumerable); } }
    internal Type IEnumerator { get { return typeof(System.Collections.IEnumerator); } }
    internal Type IList { get { return typeof(System.Collections.IList); } }
    
    internal Type Debugger { get { return typeof(System.Diagnostics.Debugger); } }
    internal Type DebuggableAttribute { get { return typeof(System.Diagnostics.DebuggableAttribute); } }
    internal Type DebuggerHiddenAttribute { get { return typeof(System.Diagnostics.DebuggerHiddenAttribute); } }
    internal Type DebuggerStepThroughAttribute { get { return typeof(System.Diagnostics.DebuggerStepThroughAttribute); } }

    internal Type DefaultMemberAttribute { get { return typeof(System.Reflection.DefaultMemberAttribute); } }
    internal Type EventInfo { get { return typeof(System.Reflection.EventInfo); } }
    internal Type FieldInfo { get { return typeof(System.Reflection.FieldInfo); } }
      
    internal Type CompilerGlobalScopeAttribute { get { return typeof(System.Runtime.CompilerServices.CompilerGlobalScopeAttribute); } }
    internal Type RequiredAttributeAttribute { get { return typeof(System.Runtime.CompilerServices.RequiredAttributeAttribute); } }
    
    internal Type IExpando { get { return typeof(System.Runtime.InteropServices.Expando.IExpando); } }
    
    internal Type CodeAccessSecurityAttribute { get { return typeof(System.Security.Permissions.CodeAccessSecurityAttribute); } }
    internal Type AllowPartiallyTrustedCallersAttribute { get { return typeof(System.Security.AllowPartiallyTrustedCallersAttribute); } }
    
    // Special cases where name isn't exactly the unqualified type name.
    internal Type ArrayOfObject { get { return typeof(System.Object[]); } }
    internal Type ArrayOfString { get { return typeof(System.String[]); } }
    internal Type SystemConvert { get { return typeof(System.Convert); } }
    internal Type ReflectionMissing { get { return typeof(System.Reflection.Missing); } }

    // Member infos
    internal MethodInfo constructArrayMethod                  { get { return this.ArrayConstructor.GetMethod("ConstructArray"); } }
    internal MethodInfo isMissingMethod                       { get { return this.Binding.GetMethod("IsMissing"); } }
    internal ConstructorInfo bitwiseBinaryConstructor         { get { return this.BitwiseBinary.GetConstructor(new Type[]{this.Int32}); } }
    internal MethodInfo evaluateBitwiseBinaryMethod           { get { return this.BitwiseBinary.GetMethod("EvaluateBitwiseBinary"); } }
    internal ConstructorInfo breakOutOfFinallyConstructor     { get { return this.BreakOutOfFinally.GetConstructor(new Type[]{this.Int32}); } }
    internal ConstructorInfo closureConstructor               { get { return this.Closure.GetConstructor(new Type[]{this.FunctionObject}); } }
    internal ConstructorInfo continueOutOfFinallyConstructor  { get { return this.ContinueOutOfFinally.GetConstructor(new Type[]{this.Int32}); } }
    internal MethodInfo checkIfDoubleIsIntegerMethod          { get { return this.Convert.GetMethod("CheckIfDoubleIsInteger"); } }
    internal MethodInfo checkIfSingleIsIntegerMethod          { get { return this.Convert.GetMethod("CheckIfSingleIsInteger"); } }
    internal MethodInfo coerce2Method                         { get { return this.Convert.GetMethod("Coerce2"); } }
    internal MethodInfo coerceTMethod                         { get { return this.Convert.GetMethod("CoerceT"); } }
    internal MethodInfo throwTypeMismatch                     { get { return this.Convert.GetMethod("ThrowTypeMismatch"); } }
    internal MethodInfo doubleToBooleanMethod                 { get { return this.Convert.GetMethod("ToBoolean", new Type[]{this.Double}); } }
    internal MethodInfo toBooleanMethod                       { get { return this.Convert.GetMethod("ToBoolean", new Type[]{this.Object, this.Boolean}); } }  
    internal MethodInfo toForInObjectMethod                   { get { return this.Convert.GetMethod("ToForInObject", new Type[]{this.Object, this.VsaEngine}); } }  
    internal MethodInfo toInt32Method                         { get { return this.Convert.GetMethod("ToInt32", new Type[]{this.Object}); } }
    internal MethodInfo toNativeArrayMethod                   { get { return this.Convert.GetMethod("ToNativeArray"); } }
    internal MethodInfo toNumberMethod                        { get { return this.Convert.GetMethod("ToNumber", new Type[]{this.Object}); } }
    internal MethodInfo toObjectMethod                        { get { return this.Convert.GetMethod("ToObject", new Type[]{this.Object, this.VsaEngine}); } }  
    internal MethodInfo toObject2Method                       { get { return this.Convert.GetMethod("ToObject2", new Type[]{this.Object, this.VsaEngine}); } }  
    internal MethodInfo doubleToStringMethod                  { get { return this.Convert.GetMethod("ToString", new Type[]{this.Double}); } }
    internal MethodInfo toStringMethod                        { get { return this.Convert.GetMethod("ToString", new Type[]{this.Object, this.Boolean}); } }
    internal FieldInfo undefinedField                         { get { return this.Empty.GetField("Value"); } }
    internal ConstructorInfo equalityConstructor              { get { return this.Equality.GetConstructor(new Type[]{this.Int32}); } }
    internal MethodInfo evaluateEqualityMethod                { get { return this.Equality.GetMethod("EvaluateEquality", new Type[]{this.Object, this.Object}); } }
    internal MethodInfo jScriptEqualsMethod                   { get { return this.Equality.GetMethod("JScriptEquals"); } }
    internal MethodInfo jScriptEvaluateMethod1                { get { return this.Eval.GetMethod("JScriptEvaluate", new Type[]{this.Object, this.VsaEngine}); } }
    internal MethodInfo jScriptEvaluateMethod2                { get { return this.Eval.GetMethod("JScriptEvaluate", new Type[]{this.Object, this.Object, this.VsaEngine}); } }
    internal MethodInfo jScriptGetEnumeratorMethod            { get { return this.ForIn.GetMethod("JScriptGetEnumerator"); } }
    internal MethodInfo jScriptFunctionDeclarationMethod      { get { return this.FunctionDeclaration.GetMethod("JScriptFunctionDeclaration"); } }
    internal MethodInfo jScriptFunctionExpressionMethod       { get { return this.FunctionExpression.GetMethod("JScriptFunctionExpression"); } }
    internal FieldInfo contextEngineField                     { get { return this.Globals.GetField("contextEngine"); } }
    internal MethodInfo fastConstructArrayLiteralMethod       { get { return this.Globals.GetMethod("ConstructArrayLiteral"); } }
    internal ConstructorInfo globalScopeConstructor           { get { return this.GlobalScope.GetConstructor(new Type[]{this.GlobalScope, this.VsaEngine}); } }
    internal MethodInfo getDefaultThisObjectMethod            { get { return this.IActivationObject.GetMethod("GetDefaultThisObject"); } }
    internal MethodInfo getFieldMethod                        { get { return this.IActivationObject.GetMethod("GetField", new Type[]{this.String, this.Int32}); } }
    internal MethodInfo getGlobalScopeMethod                  { get { return this.IActivationObject.GetMethod("GetGlobalScope"); } }
    internal MethodInfo getMemberValueMethod                  { get { return this.IActivationObject.GetMethod("GetMemberValue", new Type[]{this.String, this.Int32}); } }
    internal MethodInfo jScriptImportMethod                   { get { return this.Import.GetMethod("JScriptImport"); } }
    internal MethodInfo jScriptInMethod                       { get { return this.In.GetMethod("JScriptIn"); } }
    internal MethodInfo getEngineMethod                       { get { return this.INeedEngine.GetMethod("GetEngine"); } }
    internal MethodInfo setEngineMethod                       { get { return this.INeedEngine.GetMethod("SetEngine"); } }
    internal MethodInfo jScriptInstanceofMethod               { get { return this.Instanceof.GetMethod("JScriptInstanceof"); } }
    internal ConstructorInfo scriptExceptionConstructor       { get { return this.JScriptException.GetConstructor(new Type[]{this.JSError}); } }
    internal ConstructorInfo jsFunctionAttributeConstructor   { get { return this.JSFunctionAttribute.GetConstructor(new Type[] {this.JSFunctionAttributeEnum}); } }
    internal ConstructorInfo jsLocalFieldConstructor          { get { return this.JSLocalField.GetConstructor(new Type[]{this.String, this.RuntimeTypeHandle, this.Int32}); } }
    internal MethodInfo setMemberValue2Method                 { get { return this.JSObject.GetMethod("SetMemberValue2", new Type[]{this.String, this.Object}); } }
    internal ConstructorInfo lateBindingConstructor2          { get { return this.LateBinding.GetConstructor(new Type[]{this.String, this.Object}); } }
    internal ConstructorInfo lateBindingConstructor           { get { return this.LateBinding.GetConstructor(new Type[]{this.String}); } }
    internal FieldInfo objectField                            { get { return this.LateBinding.GetField("obj"); } }
    internal MethodInfo callMethod                            { get { return this.LateBinding.GetMethod("Call", new Type[]{this.ArrayOfObject, this.Boolean, this.Boolean, this.VsaEngine}); } }
    internal MethodInfo callValueMethod                       { get { return this.LateBinding.GetMethod("CallValue", new Type[]{this.Object, this.Object, this.ArrayOfObject, this.Boolean, this.Boolean, this.VsaEngine}); } }
    internal MethodInfo callValue2Method                      { get { return this.LateBinding.GetMethod("CallValue2", new Type[]{this.Object, this.Object, this.ArrayOfObject, this.Boolean, this.Boolean, this.VsaEngine}); } }
    internal MethodInfo deleteMethod                          { get { return this.LateBinding.GetMethod("Delete"); } }
    internal MethodInfo deleteMemberMethod                    { get { return this.LateBinding.GetMethod("DeleteMember"); } }
    internal MethodInfo getNonMissingValueMethod              { get { return this.LateBinding.GetMethod("GetNonMissingValue"); } }
    internal MethodInfo getValue2Method                       { get { return this.LateBinding.GetMethod("GetValue2"); } }
    internal MethodInfo setIndexedPropertyValueStaticMethod   { get { return this.LateBinding.GetMethod("SetIndexedPropertyValueStatic"); } }
    internal MethodInfo setValueMethod                        { get { return this.LateBinding.GetMethod("SetValue"); } }
    internal FieldInfo missingField                           { get { return this.Missing.GetField("Value"); } }
    internal MethodInfo getNamespaceMethod                    { get { return this.Namespace.GetMethod("GetNamespace"); } }
    internal ConstructorInfo numericBinaryConstructor         { get { return this.NumericBinary.GetConstructor(new Type[]{this.Int32}); } }
    internal MethodInfo numericbinaryDoOpMethod               { get { return this.NumericBinary.GetMethod("DoOp"); } }
    internal MethodInfo evaluateNumericBinaryMethod           { get { return this.NumericBinary.GetMethod("EvaluateNumericBinary"); } }
    internal ConstructorInfo numericUnaryConstructor          { get { return this.NumericUnary.GetConstructor(new Type[]{this.Int32}); } }
    internal MethodInfo evaluateUnaryMethod                   { get { return this.NumericUnary.GetMethod("EvaluateUnary"); } }
    internal MethodInfo constructObjectMethod                 { get { return this.ObjectConstructor.GetMethod("ConstructObject"); } }
    internal MethodInfo jScriptPackageMethod                  { get { return this.Package.GetMethod("JScriptPackage"); } }
    internal ConstructorInfo plusConstructor                  { get { return this.Plus.GetConstructor(new Type[]{}); } }
    internal MethodInfo plusDoOpMethod                        { get { return this.Plus.GetMethod("DoOp"); } }
    internal MethodInfo evaluatePlusMethod                    { get { return this.Plus.GetMethod("EvaluatePlus"); } }
    internal ConstructorInfo postOrPrefixConstructor          { get { return this.PostOrPrefixOperator.GetConstructor(new Type[]{this.Int32}); } }
    internal MethodInfo evaluatePostOrPrefixOperatorMethod    { get { return this.PostOrPrefixOperator.GetMethod("EvaluatePostOrPrefix"); } }
    internal ConstructorInfo referenceAttributeConstructor    { get { return this.ReferenceAttribute.GetConstructor(new Type[]{this.String}); } }
    internal MethodInfo regExpConstructMethod                 { get { return this.RegExpConstructor.GetMethod("Construct", new Type[]{this.String, this.Boolean, this.Boolean, this.Boolean}); } }
    internal ConstructorInfo relationalConstructor            { get { return this.Relational.GetConstructor(new Type[]{this.Int32}); } }
    internal MethodInfo evaluateRelationalMethod              { get { return this.Relational.GetMethod("EvaluateRelational"); } }
    internal MethodInfo jScriptCompareMethod                  { get { return this.Relational.GetMethod("JScriptCompare"); } }
    internal ConstructorInfo returnOutOfFinallyConstructor    { get { return this.ReturnOutOfFinally.GetConstructor(new Type[]{}); } }
    internal MethodInfo doubleToInt64                         { get { return this.Runtime.GetMethod("DoubleToInt64"); } }
    internal MethodInfo uncheckedDecimalToInt64Method         { get { return this.Runtime.GetMethod("UncheckedDecimalToInt64"); } }
    internal FieldInfo engineField                            { get { return this.ScriptObject.GetField("engine"); } }
    internal MethodInfo getParentMethod                       { get { return this.ScriptObject.GetMethod("GetParent"); } }
    internal MethodInfo writeMethod                           { get { return this.ScriptStream.GetMethod("Write"); } }
    internal MethodInfo writeLineMethod                       { get { return this.ScriptStream.GetMethod("WriteLine"); } }
    internal ConstructorInfo hashtableCtor                    { get { return this.SimpleHashtable.GetConstructor(new Type[]{this.UInt32}); } }
    internal MethodInfo hashtableGetItem                      { get { return this.SimpleHashtable.GetMethod("get_Item", new Type[]{this.Object}); } }
    internal MethodInfo hashTableGetEnumerator                { get { return this.SimpleHashtable.GetMethod("GetEnumerator", Type.EmptyTypes); } }
    internal MethodInfo hashtableRemove                       { get { return this.SimpleHashtable.GetMethod("Remove", new Type[1]{this.Object}); } }
    internal MethodInfo hashtableSetItem                      { get { return this.SimpleHashtable.GetMethod("set_Item", new Type[]{this.Object,this.Object}); } }
    internal FieldInfo closureInstanceField                   { get { return this.StackFrame.GetField("closureInstance"); } }
    internal FieldInfo localVarsField                         { get { return this.StackFrame.GetField("localVars"); } }
    internal MethodInfo pushStackFrameForMethod               { get { return this.StackFrame.GetMethod("PushStackFrameForMethod"); } }
    internal MethodInfo pushStackFrameForStaticMethod         { get { return this.StackFrame.GetMethod("PushStackFrameForStaticMethod"); } }
    internal MethodInfo jScriptStrictEqualsMethod             { get { return this.StrictEquality.GetMethod("JScriptStrictEquals", new Type[]{this.Object, this.Object}); } }
    internal MethodInfo jScriptThrowMethod                    { get { return this.Throw.GetMethod("JScriptThrow"); } }
    internal MethodInfo jScriptExceptionValueMethod           { get { return this.Try.GetMethod("JScriptExceptionValue"); } }
    internal MethodInfo jScriptTypeofMethod                   { get { return this.Typeof.GetMethod("JScriptTypeof"); } }
    internal ConstructorInfo vsaEngineConstructor             { get { return this.VsaEngine.GetConstructor(new Type[]{}); } }
    internal MethodInfo createVsaEngine                       { get { return this.VsaEngine.GetMethod("CreateEngine", new Type[]{}); } }
    internal MethodInfo createVsaEngineWithType               { get { return this.VsaEngine.GetMethod("CreateEngineWithType", new Type[]{this.RuntimeTypeHandle}); } }
    internal MethodInfo getOriginalArrayConstructorMethod     { get { return this.VsaEngine.GetMethod("GetOriginalArrayConstructor"); } }
    internal MethodInfo getOriginalObjectConstructorMethod    { get { return this.VsaEngine.GetMethod("GetOriginalObjectConstructor"); } }
    internal MethodInfo getOriginalRegExpConstructorMethod    { get { return this.VsaEngine.GetMethod("GetOriginalRegExpConstructor"); } }
    internal MethodInfo popScriptObjectMethod                 { get { return this.VsaEngine.GetMethod("PopScriptObject"); } }
    internal MethodInfo pushScriptObjectMethod                { get { return this.VsaEngine.GetMethod("PushScriptObject"); } }
    internal MethodInfo scriptObjectStackTopMethod            { get { return this.VsaEngine.GetMethod("ScriptObjectStackTop"); } }
    internal MethodInfo getLenientGlobalObjectMethod          { get { return this.VsaEngine.GetProperty("LenientGlobalObject").GetGetMethod(); } }
    internal MethodInfo jScriptWithMethod                     { get { return this.With.GetMethod("JScriptWith"); } }      

    internal ConstructorInfo clsCompliantAttributeCtor        { get { return this.CLSCompliantAttribute.GetConstructor(new Type[]{this.Boolean}); } }
    internal MethodInfo getEnumeratorMethod                   { get { return this.IEnumerable.GetMethod("GetEnumerator", Type.EmptyTypes); } }
    internal MethodInfo moveNextMethod                        { get { return this.IEnumerator.GetMethod("MoveNext", Type.EmptyTypes); } }
    internal MethodInfo getCurrentMethod                      { get { return this.IEnumerator.GetProperty("Current", Type.EmptyTypes).GetGetMethod(); } }
    internal ConstructorInfo contextStaticAttributeCtor       { get { return this.ContextStaticAttribute.GetConstructor(new Type[]{}); } }
    internal MethodInfo changeTypeMethod                      { get { return this.SystemConvert.GetMethod("ChangeType", new Type[]{this.Object, this.TypeCode}); } }
    internal MethodInfo convertCharToStringMethod             { get { return this.SystemConvert.GetMethod("ToString", new Type[]{this.Char}); } }
    internal ConstructorInfo dateTimeConstructor              { get { return this.DateTime.GetConstructor(new Type[]{this.Int64}); } }
    internal MethodInfo dateTimeToStringMethod                { get { return this.DateTime.GetMethod("ToString", new Type[]{}); } }
    internal MethodInfo dateTimeToInt64Method                 { get { return this.DateTime.GetProperty("Ticks").GetGetMethod(); } }
    internal ConstructorInfo decimalConstructor               { get { return this.Decimal.GetConstructor(new Type[]{this.Int32, this.Int32, this.Int32, this.Boolean, this.Byte}); } }
    internal FieldInfo decimalZeroField                       { get { return this.Decimal.GetField("Zero"); } }
    internal MethodInfo decimalCompare                        { get { return this.Decimal.GetMethod("Compare", new Type[]{this.Decimal, this.Decimal}); } }
    internal MethodInfo doubleToDecimalMethod                 { get { return this.Decimal.GetMethod("op_Explicit", new Type[]{this.Double}); } }
    internal MethodInfo int32ToDecimalMethod                  { get { return this.Decimal.GetMethod("op_Implicit", new Type[]{this.Int32}); } }
    internal MethodInfo int64ToDecimalMethod                  { get { return this.Decimal.GetMethod("op_Implicit", new Type[]{this.Int64}); } }
    internal MethodInfo uint32ToDecimalMethod                 { get { return this.Decimal.GetMethod("op_Implicit", new Type[]{this.UInt32}); } }
    internal MethodInfo uint64ToDecimalMethod                 { get { return this.Decimal.GetMethod("op_Implicit", new Type[]{this.UInt64}); } }
    internal MethodInfo decimalToDoubleMethod                 { get { return this.Decimal.GetMethod("ToDouble", new Type[]{this.Decimal}); } }
    internal MethodInfo decimalToInt32Method                  { get { return this.Decimal.GetMethod("ToInt32", new Type[]{this.Decimal}); } }
    internal MethodInfo decimalToInt64Method                  { get { return this.Decimal.GetMethod("ToInt64", new Type[]{this.Decimal}); } }
    internal MethodInfo decimalToStringMethod                 { get { return this.Decimal.GetMethod("ToString", new Type[]{}); } }
    internal MethodInfo decimalToUInt32Method                 { get { return this.Decimal.GetMethod("ToUInt32", new Type[]{this.Decimal}); } }
    internal MethodInfo decimalToUInt64Method                 { get { return this.Decimal.GetMethod("ToUInt64", new Type[]{this.Decimal}); } }
    internal MethodInfo debugBreak                            { get { return this.Debugger.GetMethod("Break", new Type[] {}); } }
    internal ConstructorInfo debuggerHiddenAttributeCtor      { get { return this.DebuggerHiddenAttribute.GetConstructor(new Type[]{}); } }
    internal ConstructorInfo debuggerStepThroughAttributeCtor { get { return this.DebuggerStepThroughAttribute.GetConstructor(new Type[]{}); } }
    internal MethodInfo int32ToStringMethod                   { get { return this.Int32.GetMethod("ToString", new Type[]{}); } }
    internal MethodInfo int64ToStringMethod                   { get { return this.Int64.GetMethod("ToString", new Type[]{}); } } 
    internal MethodInfo equalsMethod                          { get { return this.Object.GetMethod("Equals", new Type[]{this.Object}); } }
    internal ConstructorInfo defaultMemberAttributeCtor       { get { return this.DefaultMemberAttribute.GetConstructor(new Type[]{this.String}); } }
    internal MethodInfo getFieldValueMethod                   { get { return this.FieldInfo.GetMethod("GetValue", new Type[]{this.Object}); } }
    internal MethodInfo setFieldValueMethod                   { get { return this.FieldInfo.GetMethod("SetValue", new Type[]{this.Object, this.Object}); } }
    internal FieldInfo systemReflectionMissingField           { get { return this.ReflectionMissing.GetField("Value"); } }
    internal ConstructorInfo compilerGlobalScopeAttributeCtor { get { return this.CompilerGlobalScopeAttribute.GetConstructor(new Type[]{}); } }
    internal MethodInfo stringConcatArrMethod                 { get { return this.String.GetMethod("Concat", new Type[]{this.ArrayOfString}); } }
    internal MethodInfo stringConcat4Method                   { get { return this.String.GetMethod("Concat", new Type[]{this.String, this.String, this.String, this.String}); } }
    internal MethodInfo stringConcat3Method                   { get { return this.String.GetMethod("Concat", new Type[]{this.String, this.String, this.String}); } }
    internal MethodInfo stringConcat2Method                   { get { return this.String.GetMethod("Concat", new Type[]{this.String, this.String}); } }
    internal MethodInfo stringEqualsMethod                    { get { return this.String.GetMethod("Equals", new Type[]{this.String, this.String}); } }
    internal MethodInfo stringLengthMethod                    { get { return this.String.GetProperty("Length").GetGetMethod(); } }
    internal MethodInfo getMethodMethod                       { get { return this.Type.GetMethod("GetMethod", new Type[]{this.String}); } }
    internal MethodInfo getTypeMethod                         { get { return this.Type.GetMethod("GetType", new Type[]{this.String}); } }
    internal MethodInfo getTypeFromHandleMethod               { get { return this.Type.GetMethod("GetTypeFromHandle", new Type[]{this.RuntimeTypeHandle}); } }
    internal MethodInfo uint32ToStringMethod                  { get { return this.UInt32.GetMethod("ToString", new Type[]{}); } }
    internal MethodInfo uint64ToStringMethod                  { get { return this.UInt64.GetMethod("ToString", new Type[]{}); } }
    
    
    // MAPPING TO REFERENCE CONTEXT
    internal Type ToReferenceContext(Type type) {
      if (this.InReferenceContext(type))
        return type;
      if (type.IsArray)
        return Microsoft.JScript.Convert.ToType(Microsoft.JScript.TypedArray.ToRankString(type.GetArrayRank()), this.ToReferenceContext(type.GetElementType()));
      return this.JScriptReferenceModule.ResolveType(type.MetadataToken, null, null);
    }      
    
    internal IReflect ToReferenceContext(IReflect ireflect) { 
      if (ireflect is Type)
        return this.ToReferenceContext((Type)ireflect);
      return ireflect;
    }
    
    internal MethodInfo ToReferenceContext(MethodInfo method) {
      if (method is JSMethod)
        method = ((JSMethod)method).GetMethodInfo(null);
      else if (method is JSMethodInfo)
        method = ((JSMethodInfo)method).method;
      return (MethodInfo)this.MapMemberInfoToReferenceContext(method);
    }

    internal PropertyInfo ToReferenceContext(PropertyInfo property) { return (PropertyInfo)this.MapMemberInfoToReferenceContext(property); }
    internal FieldInfo ToReferenceContext(FieldInfo field) { return (FieldInfo)this.MapMemberInfoToReferenceContext(field); }      
    internal ConstructorInfo ToReferenceContext(ConstructorInfo constructor) { return (ConstructorInfo)this.MapMemberInfoToReferenceContext(constructor); }

    // Helper to implement others. We don't want to make this internal since we want to make
    // sure the appropriate subclass overload is called as they do special things.
    private MemberInfo MapMemberInfoToReferenceContext(MemberInfo member) {
      if (this.InReferenceContext(member.DeclaringType))
        return member;
      return this.JScriptReferenceModule.ResolveMember(member.MetadataToken);
    }
    
    internal bool InReferenceContext(Type type) {
      if (type == null)
        return true;
      Assembly assembly = type.Assembly;
      return assembly.ReflectionOnly || assembly != typeof(TypeReferences).Assembly || !this.JScriptReferenceModule.Assembly.ReflectionOnly; 
    }
    
    internal bool InReferenceContext(MemberInfo member) {
      if (member == null)
        return true;
      if (member is JSMethod)
        member = ((JSMethod)member).GetMethodInfo(null);
      else if (member is JSMethodInfo)
        member = ((JSMethodInfo)member).method;
      return this.InReferenceContext(member.DeclaringType);
    }

    internal bool InReferenceContext(IReflect ireflect) { 
      if (ireflect == null)
        return true;
      if (ireflect is Type)
        return this.InReferenceContext((Type)ireflect);
      return true;
    }

    
    // MAPPING TO EXECUTION CONTEXT
    internal static Type ToExecutionContext(Type type) {
      if (TypeReferences.InExecutionContext(type))
        return type;
      return typeof(TypeReferences).Module.ResolveType(type.MetadataToken, null, null);
    }
    
    internal static IReflect ToExecutionContext(IReflect ireflect) { 
      if (ireflect is Type)
        return TypeReferences.ToExecutionContext((Type)ireflect);
      return ireflect;
    }
   
    internal static MethodInfo ToExecutionContext(MethodInfo method) {
      if (method is JSMethod)
        method = ((JSMethod)method).GetMethodInfo(null);
      else if (method is JSMethodInfo)
        method = ((JSMethodInfo)method).method;
      return (MethodInfo)TypeReferences.MapMemberInfoToExecutionContext(method);
    }
        
    internal static PropertyInfo ToExecutionContext(PropertyInfo property) { return (PropertyInfo)TypeReferences.MapMemberInfoToExecutionContext(property); }
    internal static FieldInfo ToExecutionContext(FieldInfo field) { return (FieldInfo)TypeReferences.MapMemberInfoToExecutionContext(field); }      
    internal static ConstructorInfo ToExecutionContext(ConstructorInfo constructor) { return (ConstructorInfo)TypeReferences.MapMemberInfoToExecutionContext(constructor); }
    
    private static  MemberInfo MapMemberInfoToExecutionContext(MemberInfo member) {
      if (TypeReferences.InExecutionContext(member.DeclaringType))
        return member;
      return typeof(TypeReferences).Module.ResolveMember(member.MetadataToken);
    }
    
    internal static bool InExecutionContext(Type type) {
      if (type == null)
        return true;
        
      Assembly assembly = type.Assembly;
      return !assembly.ReflectionOnly || assembly.Location != typeof(TypeReferences).Assembly.Location;
    }

    internal static Object GetDefaultParameterValue(ParameterInfo parameter) {
      if (parameter.GetType().Assembly == typeof(TypeReferences).Assembly ||
          !parameter.Member.DeclaringType.Assembly.ReflectionOnly)
        return parameter.DefaultValue;
      return parameter.RawDefaultValue;
    }
    
    internal static Object GetConstantValue(FieldInfo field) {
      if (field.GetType().Assembly == typeof(TypeReferences).Assembly ||
          !field.DeclaringType.Assembly.ReflectionOnly)
        return field.GetValue(null);
      
      Type type = field.FieldType;
      Object value = field.GetRawConstantValue();
      if (type.IsEnum)
        return MetadataEnumValue.GetEnumValue(type, value);
      return value;
    }
  }
}
