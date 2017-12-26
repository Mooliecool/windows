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
  using System;
  using System.Collections;
  
  static internal class Typeob{
    // [Microsoft.JScript]Microsoft.JScript
    internal static Type ArgumentsObject { get { return Microsoft.JScript.Globals.TypeRefs.ArgumentsObject; } }
    internal static Type ArrayConstructor { get { return Microsoft.JScript.Globals.TypeRefs.ArrayConstructor; } }
    internal static Type ArrayObject { get { return Microsoft.JScript.Globals.TypeRefs.ArrayObject; } }
    internal static Type ArrayWrapper { get { return Microsoft.JScript.Globals.TypeRefs.ArrayWrapper; } }
    internal static Type BaseVsaStartup { get { return Microsoft.JScript.Globals.TypeRefs.BaseVsaStartup; } }
    internal static Type Binding { get { return Microsoft.JScript.Globals.TypeRefs.Binding; } }
    internal static Type BitwiseBinary { get { return Microsoft.JScript.Globals.TypeRefs.BitwiseBinary; } }
    internal static Type BooleanObject { get { return Microsoft.JScript.Globals.TypeRefs.BooleanObject; } }
    internal static Type BreakOutOfFinally { get { return Microsoft.JScript.Globals.TypeRefs.BreakOutOfFinally; } }
    internal static Type BuiltinFunction { get { return Microsoft.JScript.Globals.TypeRefs.BuiltinFunction; } }
    internal static Type ClassScope { get { return Microsoft.JScript.Globals.TypeRefs.ClassScope; } }
    internal static Type Closure { get { return Microsoft.JScript.Globals.TypeRefs.Closure; } }
    internal static Type ContinueOutOfFinally { get { return Microsoft.JScript.Globals.TypeRefs.ContinueOutOfFinally; } }
    internal static Type Convert { get { return Microsoft.JScript.Globals.TypeRefs.Convert; } }
    internal static Type DateObject { get { return Microsoft.JScript.Globals.TypeRefs.DateObject; } }
    internal static Type Empty { get { return Microsoft.JScript.Globals.TypeRefs.Empty; } }
    internal static Type EnumeratorObject { get { return Microsoft.JScript.Globals.TypeRefs.EnumeratorObject; } }
    internal static Type Equality { get { return Microsoft.JScript.Globals.TypeRefs.Equality; } }
    internal static Type ErrorObject { get { return Microsoft.JScript.Globals.TypeRefs.ErrorObject; } }
    internal static Type Eval { get { return Microsoft.JScript.Globals.TypeRefs.Eval; } }
    internal static Type EvalErrorObject { get { return Microsoft.JScript.Globals.TypeRefs.EvalErrorObject; } }
    internal static Type Expando { get { return Microsoft.JScript.Globals.TypeRefs.Expando; } }
    internal static Type FieldAccessor { get { return Microsoft.JScript.Globals.TypeRefs.FieldAccessor; } }
    internal static Type ForIn { get { return Microsoft.JScript.Globals.TypeRefs.ForIn; } }
    internal static Type FunctionDeclaration { get { return Microsoft.JScript.Globals.TypeRefs.FunctionDeclaration; } }
    internal static Type FunctionExpression { get { return Microsoft.JScript.Globals.TypeRefs.FunctionExpression; } }
    internal static Type FunctionObject { get { return Microsoft.JScript.Globals.TypeRefs.FunctionObject; } }
    internal static Type FunctionWrapper { get { return Microsoft.JScript.Globals.TypeRefs.FunctionWrapper; } }
    internal static Type GlobalObject { get { return Microsoft.JScript.Globals.TypeRefs.GlobalObject; } }
    internal static Type GlobalScope { get { return Microsoft.JScript.Globals.TypeRefs.GlobalScope; } }
    internal static Type Globals { get { return Microsoft.JScript.Globals.TypeRefs.Globals; } }
    internal static Type Hide { get { return Microsoft.JScript.Globals.TypeRefs.Hide; } }
    internal static Type IActivationObject { get { return Microsoft.JScript.Globals.TypeRefs.IActivationObject; } }
    internal static Type INeedEngine { get { return Microsoft.JScript.Globals.TypeRefs.INeedEngine; } }
    internal static Type Import { get { return Microsoft.JScript.Globals.TypeRefs.Import; } }
    internal static Type In { get { return Microsoft.JScript.Globals.TypeRefs.In; } }
    internal static Type Instanceof { get { return Microsoft.JScript.Globals.TypeRefs.Instanceof; } }
    internal static Type JSError { get { return Microsoft.JScript.Globals.TypeRefs.JSError; } }
    internal static Type JSFunctionAttribute { get { return Microsoft.JScript.Globals.TypeRefs.JSFunctionAttribute; } }
    internal static Type JSFunctionAttributeEnum { get { return Microsoft.JScript.Globals.TypeRefs.JSFunctionAttributeEnum; } }
    internal static Type JSLocalField { get { return Microsoft.JScript.Globals.TypeRefs.JSLocalField; } }
    internal static Type JSObject { get { return Microsoft.JScript.Globals.TypeRefs.JSObject; } }
    internal static Type JScriptException { get { return Microsoft.JScript.Globals.TypeRefs.JScriptException; } }
    internal static Type LateBinding { get { return Microsoft.JScript.Globals.TypeRefs.LateBinding; } }
    internal static Type LenientGlobalObject { get { return Microsoft.JScript.Globals.TypeRefs.LenientGlobalObject; } }
    internal static Type MathObject { get { return Microsoft.JScript.Globals.TypeRefs.MathObject; } }
    internal static Type MethodInvoker { get { return Microsoft.JScript.Globals.TypeRefs.MethodInvoker; } }
    internal static Type Missing { get { return Microsoft.JScript.Globals.TypeRefs.Missing; } }
    internal static Type Namespace { get { return Microsoft.JScript.Globals.TypeRefs.Namespace; } }
    internal static Type NotRecommended { get { return Microsoft.JScript.Globals.TypeRefs.NotRecommended; } }
    internal static Type NumberObject { get { return Microsoft.JScript.Globals.TypeRefs.NumberObject; } }
    internal static Type NumericBinary { get { return Microsoft.JScript.Globals.TypeRefs.NumericBinary; } }
    internal static Type NumericUnary { get { return Microsoft.JScript.Globals.TypeRefs.NumericUnary; } }
    internal static Type ObjectConstructor { get { return Microsoft.JScript.Globals.TypeRefs.ObjectConstructor; } }
    internal static Type Override { get { return Microsoft.JScript.Globals.TypeRefs.Override; } }
    internal static Type Package { get { return Microsoft.JScript.Globals.TypeRefs.Package; } }
    internal static Type Plus { get { return Microsoft.JScript.Globals.TypeRefs.Plus; } }
    internal static Type PostOrPrefixOperator { get { return Microsoft.JScript.Globals.TypeRefs.PostOrPrefixOperator; } }
    internal static Type RangeErrorObject { get { return Microsoft.JScript.Globals.TypeRefs.RangeErrorObject; } }
    internal static Type ReferenceAttribute { get { return Microsoft.JScript.Globals.TypeRefs.ReferenceAttribute; } }
    internal static Type ReferenceErrorObject { get { return Microsoft.JScript.Globals.TypeRefs.ReferenceErrorObject; } }
    internal static Type RegExpConstructor { get { return Microsoft.JScript.Globals.TypeRefs.RegExpConstructor; } }
    internal static Type RegExpObject { get { return Microsoft.JScript.Globals.TypeRefs.RegExpObject; } }
    internal static Type Relational { get { return Microsoft.JScript.Globals.TypeRefs.Relational; } }
    internal static Type ReturnOutOfFinally { get { return Microsoft.JScript.Globals.TypeRefs.ReturnOutOfFinally; } }
    internal static Type Runtime { get { return Microsoft.JScript.Globals.TypeRefs.Runtime; } }
    internal static Type ScriptFunction { get { return Microsoft.JScript.Globals.TypeRefs.ScriptFunction; } }
    internal static Type ScriptObject { get { return Microsoft.JScript.Globals.TypeRefs.ScriptObject; } }
    internal static Type ScriptStream { get { return Microsoft.JScript.Globals.TypeRefs.ScriptStream; } }
    internal static Type SimpleHashtable { get { return Microsoft.JScript.Globals.TypeRefs.SimpleHashtable; } }
    internal static Type StackFrame { get { return Microsoft.JScript.Globals.TypeRefs.StackFrame; } }
    internal static Type StrictEquality { get { return Microsoft.JScript.Globals.TypeRefs.StrictEquality; } }
    internal static Type StringObject { get { return Microsoft.JScript.Globals.TypeRefs.StringObject; } }
    internal static Type SyntaxErrorObject { get { return Microsoft.JScript.Globals.TypeRefs.SyntaxErrorObject; } }
    internal static Type Throw { get { return Microsoft.JScript.Globals.TypeRefs.Throw; } }
    internal static Type Try { get { return Microsoft.JScript.Globals.TypeRefs.Try; } }
    internal static Type TypedArray { get { return Microsoft.JScript.Globals.TypeRefs.TypedArray; } }
    internal static Type TypeErrorObject { get { return Microsoft.JScript.Globals.TypeRefs.TypeErrorObject; } }
    internal static Type Typeof { get { return Microsoft.JScript.Globals.TypeRefs.Typeof; } }
    internal static Type URIErrorObject { get { return Microsoft.JScript.Globals.TypeRefs.URIErrorObject; } }
    internal static Type VBArrayObject { get { return Microsoft.JScript.Globals.TypeRefs.VBArrayObject; } }
    internal static Type With { get { return Microsoft.JScript.Globals.TypeRefs.With; } }
    internal static Type VsaEngine { get { return Microsoft.JScript.Globals.TypeRefs.VsaEngine; } }    
    
    // The types in mscorlib live in both the execution and reflection only loader contexts
    // and thus do not need to be mapped.
    internal static Type Array { get { return Microsoft.JScript.Globals.TypeRefs.Array; } }
    internal static Type Attribute { get { return Microsoft.JScript.Globals.TypeRefs.Attribute; } }
    internal static Type AttributeUsageAttribute { get { return Microsoft.JScript.Globals.TypeRefs.AttributeUsageAttribute; } }
    internal static Type Byte { get { return Microsoft.JScript.Globals.TypeRefs.Byte; } }
    internal static Type Boolean { get { return Microsoft.JScript.Globals.TypeRefs.Boolean; } }
    internal static Type Char { get { return Microsoft.JScript.Globals.TypeRefs.Char; } }
    internal static Type CLSCompliantAttribute { get { return Microsoft.JScript.Globals.TypeRefs.CLSCompliantAttribute; } }
    internal static Type ContextStaticAttribute { get { return Microsoft.JScript.Globals.TypeRefs.ContextStaticAttribute; } }
    internal static Type DateTime { get { return Microsoft.JScript.Globals.TypeRefs.DateTime; } }
    internal static Type Null { get { return Microsoft.JScript.Globals.TypeRefs.DBNull; } }
    internal static Type Delegate { get { return Microsoft.JScript.Globals.TypeRefs.Delegate; } }
    internal static Type Decimal { get { return Microsoft.JScript.Globals.TypeRefs.Decimal; } }
    internal static Type Double { get { return Microsoft.JScript.Globals.TypeRefs.Double; } }
    internal static Type Enum { get { return Microsoft.JScript.Globals.TypeRefs.Enum; } }
    internal static Type Exception { get { return Microsoft.JScript.Globals.TypeRefs.Exception; } }
    internal static Type IConvertible { get { return Microsoft.JScript.Globals.TypeRefs.IConvertible; } }
    internal static Type IntPtr { get { return Microsoft.JScript.Globals.TypeRefs.IntPtr; } }
    internal static Type Int16 { get { return Microsoft.JScript.Globals.TypeRefs.Int16; } }
    internal static Type Int32 { get { return Microsoft.JScript.Globals.TypeRefs.Int32; } }
    internal static Type Int64 { get { return Microsoft.JScript.Globals.TypeRefs.Int64; } }
    internal static Type Object { get { return Microsoft.JScript.Globals.TypeRefs.Object; } }
    internal static Type ObsoleteAttribute { get { return Microsoft.JScript.Globals.TypeRefs.ObsoleteAttribute; } }
    internal static Type ParamArrayAttribute { get { return Microsoft.JScript.Globals.TypeRefs.ParamArrayAttribute; } }
    internal static Type RuntimeTypeHandle { get { return Microsoft.JScript.Globals.TypeRefs.RuntimeTypeHandle; } }
    internal static Type SByte { get { return Microsoft.JScript.Globals.TypeRefs.SByte; } }
    internal static Type Single { get { return Microsoft.JScript.Globals.TypeRefs.Single; } }
    internal static Type String { get { return Microsoft.JScript.Globals.TypeRefs.String; } }
    internal static Type Type { get { return Microsoft.JScript.Globals.TypeRefs.Type; } }
    internal static Type TypeCode { get { return Microsoft.JScript.Globals.TypeRefs.TypeCode; } }
    internal static Type UIntPtr { get { return Microsoft.JScript.Globals.TypeRefs.UIntPtr; } }
    internal static Type UInt16 { get { return Microsoft.JScript.Globals.TypeRefs.UInt16; } }
    internal static Type UInt32 { get { return Microsoft.JScript.Globals.TypeRefs.UInt32; } }
    internal static Type UInt64 { get { return Microsoft.JScript.Globals.TypeRefs.UInt64; } }
    internal static Type ValueType { get { return Microsoft.JScript.Globals.TypeRefs.ValueType; } }
    internal static Type Void { get { return Microsoft.JScript.Globals.TypeRefs.Void; } }
    
    internal static Type IEnumerable { get { return Microsoft.JScript.Globals.TypeRefs.IEnumerable; } }
    internal static Type IEnumerator { get { return Microsoft.JScript.Globals.TypeRefs.IEnumerator; } }
    internal static Type IList { get { return Microsoft.JScript.Globals.TypeRefs.IList; } }
    
    internal static Type Debugger { get { return Microsoft.JScript.Globals.TypeRefs.Debugger; } }
    internal static Type DebuggableAttribute { get { return Microsoft.JScript.Globals.TypeRefs.DebuggableAttribute; } }
    internal static Type DebuggerHiddenAttribute { get { return Microsoft.JScript.Globals.TypeRefs.DebuggerHiddenAttribute; } }
    internal static Type DebuggerStepThroughAttribute { get { return Microsoft.JScript.Globals.TypeRefs.DebuggerStepThroughAttribute; } }

    internal static Type DefaultMemberAttribute { get { return Microsoft.JScript.Globals.TypeRefs.DefaultMemberAttribute; } }
    internal static Type EventInfo { get { return Microsoft.JScript.Globals.TypeRefs.EventInfo; } }
    internal static Type FieldInfo { get { return Microsoft.JScript.Globals.TypeRefs.FieldInfo; } }
      
    internal static Type CompilerGlobalScopeAttribute { get { return Microsoft.JScript.Globals.TypeRefs.CompilerGlobalScopeAttribute; } }
    internal static Type RequiredAttributeAttribute { get { return Microsoft.JScript.Globals.TypeRefs.RequiredAttributeAttribute; } }

    internal static Type IExpando { get { return Microsoft.JScript.Globals.TypeRefs.IExpando; } }
    
    internal static Type CodeAccessSecurityAttribute { get { return Microsoft.JScript.Globals.TypeRefs.CodeAccessSecurityAttribute; } }
    internal static Type AllowPartiallyTrustedCallersAttribute { get { return Microsoft.JScript.Globals.TypeRefs.AllowPartiallyTrustedCallersAttribute; } }
    
    // Special cases where name isn't exactly the unqualified type name.
    internal static Type ArrayOfObject { get { return Microsoft.JScript.Globals.TypeRefs.ArrayOfObject; } }
    internal static Type ArrayOfString { get { return Microsoft.JScript.Globals.TypeRefs.ArrayOfString; } }
    internal static Type SystemConvert { get { return Microsoft.JScript.Globals.TypeRefs.SystemConvert; } }
    internal static Type ReflectionMissing { get { return Microsoft.JScript.Globals.TypeRefs.ReflectionMissing; } }
  }
}
    
