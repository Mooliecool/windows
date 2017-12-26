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
    using System.Reflection;
    using System.Reflection.Emit;
    using System.Globalization;
    
    internal sealed class RegExpLiteral : AST {
      private String source;
      private bool ignoreCase, global, multiline;
      private JSGlobalField regExpVar;
      
      private static int counter = 0;


      internal RegExpLiteral(String source, String flags, Context context)
        : base(context) {
        this.source = source;
        this.ignoreCase = this.global = this.multiline = false;
        if (flags != null)
          for (int i = 0; i < flags.Length; i++)
            switch (flags[i]) {
              case 'i':
                if (this.ignoreCase)
                  throw new JScriptException(JSError.RegExpSyntax);
                this.ignoreCase = true;
                break;
              case 'g':
                if (this.global)
                  throw new JScriptException(JSError.RegExpSyntax);
                this.global = true;
                break;
              case 'm':
                if (this.multiline)
                  throw new JScriptException(JSError.RegExpSyntax);
                this.multiline = true;
                break;
              default:
                throw new JScriptException(JSError.RegExpSyntax);
            }
      }

      internal override Object Evaluate() {
        if( VsaEngine.executeForJSEE )
          throw new JScriptException(JSError.NonSupportedInDebugger);
        RegExpObject regExpObject = (RegExpObject)Globals.RegExpTable[this];
        if (regExpObject == null) {
          regExpObject = (RegExpObject)this.Engine.GetOriginalRegExpConstructor().Construct
            (this.source, this.ignoreCase, this.global, this.multiline);
          Globals.RegExpTable[this] = regExpObject;
        }
        return regExpObject;
      }

      internal override IReflect InferType(JSField inferenceTarget){
        return Typeob.RegExpObject;
      }

      internal override AST PartiallyEvaluate() {
        String id = "regexp "+(RegExpLiteral.counter++).ToString(CultureInfo.InvariantCulture);
        GlobalScope gs = (GlobalScope)this.Engine.GetGlobalScope().GetObject();
        JSGlobalField v = (JSGlobalField)gs.AddNewField(id, null, FieldAttributes.Assembly);
        v.type = new TypeExpression(new ConstantWrapper(Typeob.RegExpObject, this.context));
        this.regExpVar = v;
        return this;
      }

      internal override void TranslateToIL(ILGenerator il, Type rtype) {
        il.Emit(OpCodes.Ldsfld, (FieldInfo)this.regExpVar.GetMetaData());
        Convert.Emit(this, il, Typeob.RegExpObject, rtype);
      }

      internal override void TranslateToILInitializer(ILGenerator il) {
        ScriptObject scope = this.Engine.ScriptObjectStackTop();
        while (scope != null && (scope is WithObject || scope is BlockScope))
          scope = scope.GetParent();
        if (scope is FunctionScope){
          this.EmitILToLoadEngine(il); //Make sure engine gets initialized every time function is entered
          il.Emit(OpCodes.Pop);
        }
        il.Emit(OpCodes.Ldsfld, (FieldInfo)this.regExpVar.GetMetaData());
        Label exit = il.DefineLabel();        
        il.Emit(OpCodes.Brtrue_S, exit);
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.getOriginalRegExpConstructorMethod);
        il.Emit(OpCodes.Ldstr, this.source);
        if (this.ignoreCase)
          il.Emit(OpCodes.Ldc_I4_1);
        else
          il.Emit(OpCodes.Ldc_I4_0);
        if (this.global)
          il.Emit(OpCodes.Ldc_I4_1);
        else
          il.Emit(OpCodes.Ldc_I4_0);
        if (this.multiline)
          il.Emit(OpCodes.Ldc_I4_1);
        else
          il.Emit(OpCodes.Ldc_I4_0);
        il.Emit(OpCodes.Call, CompilerGlobals.regExpConstructMethod);
        il.Emit(OpCodes.Castclass, Typeob.RegExpObject);
        il.Emit(OpCodes.Stsfld, (FieldInfo)this.regExpVar.GetMetaData());
        il.MarkLabel(exit);
      }
    }
}
