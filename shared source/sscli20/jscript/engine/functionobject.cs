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
    using System.Diagnostics;
    using System.Globalization;
    using System.Reflection;
    using System.Reflection.Emit;
    
    public sealed class FunctionObject : ScriptFunction{
      internal ParameterDeclaration[] parameter_declarations;
      internal String[] formal_parameters;
      internal TypeExpression return_type_expr;
      private Block body;
      private MethodInfo method;
      private ParameterInfo[] parameterInfos;
      internal Context funcContext; 
      private int argumentsSlotNumber;
      internal JSLocalField[] fields;
      internal FunctionScope own_scope;
      internal ScriptObject enclosing_scope;
      internal bool must_save_stack_locals; //because it has a nested function (that accesses one of its locals) or an eval statement
      internal bool hasArgumentsObject;
      internal IReflect implementedIface;
      internal MethodInfo implementedIfaceMethod;
      internal bool isMethod;
      internal bool isExpandoMethod;
      internal bool isConstructor;
      internal bool isImplicitCtor;
      internal bool isStatic;
      internal bool noVersionSafeAttributeSpecified;
      internal bool suppressIL;
      internal String text;
      private MethodBuilder mb;
      private ConstructorBuilder cb;
      internal TypeBuilder classwriter;
      internal MethodAttributes attributes;
      internal Globals globals;
      private ConstructorInfo superConstructor;
      internal ConstructorCall superConstructorCall;
      internal CustomAttributeList customAttributes; // assigned from within the FunctionDeclaration constructor
      private CLSComplianceSpec clsCompliance;
      private LocalBuilder engineLocal;
      private bool partiallyEvaluated;
      internal Label returnLabel;
      
      internal FunctionObject(String name, ParameterDeclaration[] parameter_declarations, TypeExpression return_type_expr, Block body, FunctionScope own_scope, 
        ScriptObject enclosing_scope, Context funcContext, MethodAttributes attributes)
        : this(name, parameter_declarations, return_type_expr, body, own_scope, enclosing_scope, funcContext, attributes, null, false){
      }
      
      internal FunctionObject(String name, ParameterDeclaration[] parameter_declarations, TypeExpression return_type_expr, Block body, FunctionScope own_scope, 
        ScriptObject enclosing_scope, Context funcContext, MethodAttributes attributes, CustomAttributeList customAttributes, bool isMethod)
        : base(body.Globals.globalObject.originalFunction.originalPrototype, name, parameter_declarations.Length) {
        this.parameter_declarations = parameter_declarations;
        int n = parameter_declarations.Length;
        this.formal_parameters = new String[n];
        for (int i = 0; i < n; i++)
          this.formal_parameters[i] = parameter_declarations[i].identifier;
        this.argumentsSlotNumber = 0;
        this.return_type_expr = return_type_expr;
        if (this.return_type_expr != null)
          own_scope.AddReturnValueField();
        this.body = body;
        this.method = null;
        this.parameterInfos = null;
        this.funcContext = funcContext;
        this.own_scope = own_scope;
        this.own_scope.owner = this;
        if ((!(enclosing_scope is ActivationObject) || !((ActivationObject)(enclosing_scope)).fast) && !isMethod){
          this.argumentsSlotNumber = this.own_scope.GetNextSlotNumber();
          JSLocalField argsf = (JSLocalField)this.own_scope.AddNewField("arguments", null, (FieldAttributes)0);
          argsf.type = new TypeExpression(new ConstantWrapper(Typeob.Object, funcContext));
          argsf.isDefined = true;
          this.hasArgumentsObject = true;
        }else
          this.hasArgumentsObject = false;
        this.implementedIface = null;
        this.implementedIfaceMethod = null;
        this.isMethod = isMethod;
        this.isExpandoMethod = customAttributes == null ? false : customAttributes.ContainsExpandoAttribute();
        this.isStatic = this.own_scope.isStatic = (attributes&MethodAttributes.Static) != (MethodAttributes)0;
        this.suppressIL = false;
        this.noVersionSafeAttributeSpecified = true; // PartiallyEvaluate may change this value
        this.fields = this.own_scope.GetLocalFields();
        this.enclosing_scope = enclosing_scope;
        this.must_save_stack_locals = false; //This is set to true, if need be by PartiallyEvaluate.
        this.text = null;
        this.mb = null;
        this.cb = null;
        this.attributes = attributes;
        if (!this.isStatic) this.attributes |= MethodAttributes.HideBySig;
        this.globals = body.Globals;
        this.superConstructor = null;
        this.superConstructorCall = null;
        this.customAttributes = customAttributes;
        this.noExpando = false;
        this.clsCompliance = CLSComplianceSpec.NotAttributed;
        this.engineLocal = null;
        this.partiallyEvaluated = false;
      }
      
      internal FunctionObject(Type t, String name, String method_name, String[] formal_parameters, JSLocalField[] fields,
                              bool must_save_stack_locals, bool hasArgumentsObject, String text, VsaEngine engine)
        : base(engine.Globals.globalObject.originalFunction.originalPrototype, name, formal_parameters.Length){
        this.engine = engine;
        this.formal_parameters = formal_parameters;
        this.argumentsSlotNumber = 0;
        this.body = null;
        TypeReflector tr = TypeReflector.GetTypeReflectorFor(Globals.TypeRefs.ToReferenceContext(t));
        this.method = tr.GetMethod(method_name, BindingFlags.Public|BindingFlags.Static);
        this.parameterInfos = this.method.GetParameters();
        if (!CustomAttribute.IsDefined(this.method, typeof(JSFunctionAttribute), false))
          this.isMethod = true;
        else{
          Object[] attrs = CustomAttribute.GetCustomAttributes(method, typeof(JSFunctionAttribute), false);
          JSFunctionAttributeEnum attr = ((JSFunctionAttribute)attrs[0]).attributeValue;
          this.isExpandoMethod = (attr & JSFunctionAttributeEnum.IsExpandoMethod) != 0;
        }
        this.funcContext = null;
        this.own_scope = null;
        this.fields = fields;
        this.must_save_stack_locals = must_save_stack_locals;
        this.hasArgumentsObject = hasArgumentsObject;
        this.text = text;
        this.attributes = MethodAttributes.Public;
        this.globals = engine.Globals;
        this.superConstructor = null;
        this.superConstructorCall = null;
        this.enclosing_scope = this.globals.ScopeStack.Peek();
        this.noExpando = false;
        this.clsCompliance = CLSComplianceSpec.NotAttributed;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal override Object Call(Object[] args, Object thisob){
        return this.Call(args, thisob, null, null);
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal override Object Call(Object[] args, Object thisob, Binder binder, CultureInfo culture){
        if (this.body == null) //Calling a compiled function directly (not via a Closure). Happens when constructor property is used in new expression.
          return this.Call(args, thisob, this.enclosing_scope, new Closure(this), binder, culture);
        //Get here from the Evaluator when in fast mode and the function is in the global scope.
        StackFrame sf = new StackFrame(thisob is JSObject ? (JSObject)thisob : this.enclosing_scope, this.fields, new Object[this.fields.Length], thisob);
        if (this.isConstructor){
          sf.closureInstance = thisob;
          if (this.superConstructor != null){
            if (this.superConstructorCall == null){
              if (this.superConstructor is JSConstructor)
                this.superConstructor.Invoke(thisob, new Object[0]);
            }else{
              ASTList argASTs = this.superConstructorCall.arguments;
              int n = argASTs.count;
              Object[] supArgs = new Object[n];
              for (int i = 0; i < n; i++) supArgs[i] = argASTs[i].Evaluate();
              this.superConstructor.Invoke(thisob, (BindingFlags)0, binder, supArgs, culture);
            }
          }
          globals.ScopeStack.GuardedPush(thisob is JSObject ? (JSObject)thisob : this.enclosing_scope);
          try{
            ((ClassScope)this.enclosing_scope).owner.body.EvaluateInstanceVariableInitializers();
          }finally{
            globals.ScopeStack.Pop();
          }
        }else if (this.isMethod && !this.isStatic)
          if (((ClassScope)this.enclosing_scope).HasInstance(thisob))
            sf.closureInstance = thisob;
          else
            throw new JScriptException(JSError.TypeMismatch);
        globals.ScopeStack.GuardedPush(sf);
        try{
          this.own_scope.CloseNestedFunctions(sf);
          this.ConvertArguments(args, sf.localVars, 0, args.Length, this.formal_parameters.Length, binder, culture);
          Completion result = (Completion)(this.body.Evaluate());
          if (result.Return)
            return result.value;
          else
            return null;
        }finally{
          globals.ScopeStack.Pop();
        }
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal override Object Call(Object[] args, Object thisob, ScriptObject enclosing_scope, Closure calleeClosure, Binder binder, CultureInfo culture){
        if (this.body != null)
          return CallASTFunc(args, thisob, enclosing_scope, calleeClosure, binder, culture);
        
        Object saved_caller = calleeClosure.caller;
        calleeClosure.caller = this.globals.caller;
        this.globals.caller = calleeClosure;
        Object saved_args = calleeClosure.arguments;
        ScriptObject calling_scope = globals.ScopeStack.Peek();
        ArgumentsObject caller_arguments = (calling_scope is StackFrame) ? ((StackFrame)calling_scope).caller_arguments : null;
        StackFrame sf = new StackFrame(enclosing_scope, this.fields, this.must_save_stack_locals ? new Object[this.fields.Length] : null, thisob);
        globals.ScopeStack.GuardedPush(sf);
        ArgumentsObject arguments = new ArgumentsObject(globals.globalObject.originalObjectPrototype, args, this, calleeClosure, sf, caller_arguments);
        sf.caller_arguments = arguments;
        calleeClosure.arguments = arguments;
        try{
          try{
            int n = this.formal_parameters.Length;
            int m = args.Length;
            if (this.hasArgumentsObject){
              Object[] newargs = new Object[n+3];
              newargs[0] = thisob;
              newargs[1] = this.engine;
              newargs[2] = arguments;
              this.ConvertArguments(args, newargs, 3, m, n, binder, culture);
              return this.method.Invoke(thisob, BindingFlags.SuppressChangeType, null, newargs, null);
            }else if (!this.isMethod){
              Object[] newargs = new Object[n+2];
              newargs[0] = thisob;
              newargs[1] = this.engine;
              this.ConvertArguments(args, newargs, 2, m, n, binder, culture);
              return this.method.Invoke(thisob, BindingFlags.SuppressChangeType, null, newargs, null);
            }else if (n == m){
              this.ConvertArguments(args, args, 0, m, n, binder, culture);
              return this.method.Invoke(thisob, BindingFlags.SuppressChangeType, null, args, null);
            }else{
              Object[] newargs = new Object[n];
              this.ConvertArguments(args, newargs, 0, m, n, binder, culture);
              return this.method.Invoke(thisob, BindingFlags.SuppressChangeType, null, newargs, null);
            }
          }catch(TargetInvocationException e){
            throw e.InnerException;
          }
        }finally{
          globals.ScopeStack.Pop();
          calleeClosure.arguments = saved_args;
          this.globals.caller = calleeClosure.caller;
          calleeClosure.caller = saved_caller;
        }
      }
      
      private Object CallASTFunc(Object[] args, Object thisob, ScriptObject enclosing_scope, Closure calleeClosure, Binder binder, CultureInfo culture){
        Object saved_caller = calleeClosure.caller;
        calleeClosure.caller = this.globals.caller;
        this.globals.caller = calleeClosure;
        Object saved_args = calleeClosure.arguments;
        ScriptObject calling_scope = globals.ScopeStack.Peek();
        ArgumentsObject caller_arguments = (calling_scope is StackFrame) ? ((StackFrame)calling_scope).caller_arguments : null;
        StackFrame sf = new StackFrame(enclosing_scope, this.fields, new Object[this.fields.Length], thisob);
        if (this.isMethod && !this.isStatic)
          sf.closureInstance = thisob;
        globals.ScopeStack.GuardedPush(sf);
        try{
          this.own_scope.CloseNestedFunctions(sf);
          ArgumentsObject arguments = null;
          if (this.hasArgumentsObject){
            arguments = new ArgumentsObject(this.globals.globalObject.originalObjectPrototype, args, this, calleeClosure, sf, caller_arguments);
            sf.localVars[this.argumentsSlotNumber] = arguments;
          }
          sf.caller_arguments = arguments;
          calleeClosure.arguments = arguments;
          this.ConvertArguments(args, sf.localVars, 0, args.Length, this.formal_parameters.Length, binder, culture);
          Completion result = (Completion)(this.body.Evaluate());
          if (result.Return)
            return result.value;
          else
            return null;
        }finally{
          this.globals.ScopeStack.Pop();
          calleeClosure.arguments = saved_args;
          this.globals.caller = calleeClosure.caller;
          calleeClosure.caller = saved_caller;
        }
      }

      //assume this code is called after PartiallyEvaluate
      internal void CheckCLSCompliance(bool classIsCLSCompliant){
        if (classIsCLSCompliant){
          if (this.clsCompliance != CLSComplianceSpec.NonCLSCompliant){
            for (int i = 0, length = this.parameter_declarations.Length; i < length; i++){
              IReflect ir  = this.parameter_declarations[i].ParameterIReflect;
              if (ir != null && !TypeExpression.TypeIsCLSCompliant(ir)){
                this.clsCompliance = CLSComplianceSpec.NonCLSCompliant;
                this.funcContext.HandleError(JSError.NonCLSCompliantMember);
                return;
              }
            }
            if (this.return_type_expr != null && !this.return_type_expr.IsCLSCompliant()){
              this.clsCompliance = CLSComplianceSpec.NonCLSCompliant;
              this.funcContext.HandleError(JSError.NonCLSCompliantMember);
            }
          }
        }else if (this.clsCompliance == CLSComplianceSpec.CLSCompliant)
          this.funcContext.HandleError(JSError.MemberTypeCLSCompliantMismatch);

      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal Object Construct(JSObject thisob, Object[] args){
        JSObject newob = new JSObject(null, false);
        newob.SetParent(this.GetPrototypeForConstructedObject());
        newob.outer_class_instance = thisob;
        Object result = this.Call(args, newob);
        if (result is ScriptObject) return result;
        return newob;
      }

      private void ConvertArguments(Object[] args, Object[] newargs, int offset, int length, int n, Binder binder, CultureInfo culture){
        ParameterInfo[] pars = this.parameterInfos;
        if (pars != null){
          for (int i = 0, j = offset; i < n; i++, j++){
            Type t = pars[j].ParameterType;
            if (i == n-1 && CustomAttribute.IsDefined(pars[j], typeof(ParamArrayAttribute), false)){
              int k = length-i; if (k < 0) k = 0;
              newargs[j] = FunctionObject.CopyToNewParamArray(t.GetElementType(), k, args, i, binder, culture);
              return;
            }
            Object argval = i < length ? args[i] : null;
            if (t == Typeob.Object)
              newargs[j] = argval;
            else if (binder != null)
              newargs[j] = binder.ChangeType(argval, t, culture);
            else
              newargs[j] = Convert.CoerceT(argval, t);
          }
        }else{
          ParameterDeclaration[] pds = this.parameter_declarations;
          for (int i = 0, j = offset; i < n; i++, j++){
            IReflect ir = pds[i].ParameterIReflect;
            if (i == n-1 && CustomAttribute.IsDefined(pds[j], typeof(ParamArrayAttribute), false)){
              int k = length-i; if (k < 0) k = 0;
              newargs[j] = FunctionObject.CopyToNewParamArray(((TypedArray)ir).elementType, k, args, i);
              return;
            }
            Object argval = i < length ? args[i] : null;
            if (ir == Typeob.Object){
              newargs[j] = argval; continue;
            }
            if (ir is ClassScope)
              newargs[j] = Convert.Coerce(argval, ir);
            else if (binder != null)
              newargs[j] = binder.ChangeType(argval, Convert.ToType(ir), culture);
            else
              newargs[j] = Convert.CoerceT(argval, Convert.ToType(ir));
          }
        }
      }
      
      private static Object[] CopyToNewParamArray(IReflect ir, int n, Object[] args, int offset){
        Object[] paramArray = new Object[n];
        for (int i = 0; i < n; i++)
          paramArray[i] = Convert.Coerce(args[i+offset], ir);
        return paramArray;
      }
    
      private static System.Array CopyToNewParamArray(Type t, int n, Object[] args, int offset, Binder binder, CultureInfo culture){
        System.Array paramArray = System.Array.CreateInstance(t, n);
        for (int i = 0; i < n; i++)
          paramArray.SetValue(binder.ChangeType(args[i+offset], t, culture), i);
        return paramArray;
      }
    
      internal void EmitLastLineInfo(ILGenerator il){
        if (!this.isImplicitCtor){
          // this is the only case so far where debug info does not go through Context and it's explicitely called here using DocumentContext
          int endLine = this.body.context.EndLine;
          int endColumn = this.body.context.EndColumn;
          this.body.context.document.EmitLineInfo(il, endLine, endColumn, endLine, endColumn + 1);
        }
      }

      internal String GetName(){
        return this.name;
      }
      
      internal override int GetNumberOfFormalParameters(){
        return this.formal_parameters.Length;
      }
      
      internal ConstructorInfo GetConstructorInfo(CompilerGlobals compilerGlobals){
        return (ConstructorInfo)this.GetMethodBase(compilerGlobals);
      }
      
      internal MethodInfo GetMethodInfo(CompilerGlobals compilerGlobals){
        return (MethodInfo)this.GetMethodBase(compilerGlobals);
      }
      
      internal MethodBase GetMethodBase(CompilerGlobals compilerGlobals){
        if (this.mb != null)
          return this.mb;
        if (this.cb != null)
          return this.cb;  
        JSFunctionAttributeEnum jsFunctionAttributes = 0;
        int extras = 3;
        if (this.isMethod){
          if (this.isConstructor && ((ClassScope)this.enclosing_scope).outerClassField != null){
            extras = 1;
            jsFunctionAttributes |= JSFunctionAttributeEnum.IsInstanceNestedClassConstructor;
          }else
            extras = 0;
        }else if (!this.hasArgumentsObject)
          extras = 2;
        int np = this.formal_parameters.Length+extras;
        Type[] ptypes = new Type[np];
        Type rtype = Convert.ToType(this.ReturnType(null));
        if (extras > 0){
          if (this.isConstructor)
            ptypes[np-1] = ((ClassScope)this.enclosing_scope).outerClassField.FieldType;
          else
            ptypes[0] = Typeob.Object;
          jsFunctionAttributes |= JSFunctionAttributeEnum.HasThisObject;
        }
        if (extras > 1){
          ptypes[1] = Typeob.VsaEngine;
          jsFunctionAttributes |= JSFunctionAttributeEnum.HasEngine;
        }
        if (extras > 2){
          ptypes[2] = Typeob.Object;
          jsFunctionAttributes |= JSFunctionAttributeEnum.HasArguments;
        }
        if (this.must_save_stack_locals)
          jsFunctionAttributes |= JSFunctionAttributeEnum.HasStackFrame;
        if (this.isExpandoMethod)
          jsFunctionAttributes |= JSFunctionAttributeEnum.IsExpandoMethod;
        if (this.isConstructor){
          for (int i = 0; i < np-extras; i++)
            ptypes[i] = this.parameter_declarations[i].ParameterType;
        }else{
          for (int i = extras; i < np; i++)
            ptypes[i] = this.parameter_declarations[i-extras].ParameterType;
        }
        if (this.enclosing_scope is ClassScope)
          if (this.isConstructor){
            this.cb = ((ClassScope)(this.enclosing_scope)).GetTypeBuilder().DefineConstructor(this.attributes&MethodAttributes.MemberAccessMask, 
              CallingConventions.Standard, ptypes);
          }else{
            String name = this.name;
            if (this.implementedIfaceMethod != null){
              JSMethod jsm = this.implementedIfaceMethod as JSMethod;
              if (jsm != null) this.implementedIfaceMethod = jsm.GetMethodInfo(compilerGlobals);
              name = this.implementedIfaceMethod.DeclaringType.FullName + "." + name;
            }
            TypeBuilder tb = ((ClassScope)(this.enclosing_scope)).GetTypeBuilder();
            if (this.mb != null) return this.mb; //Might have been defined as a result of the above call
            this.mb = tb.DefineMethod(name, this.attributes, rtype, ptypes);
            if (this.implementedIfaceMethod != null)
              ((ClassScope)(this.enclosing_scope)).GetTypeBuilder().DefineMethodOverride(this.mb, this.implementedIfaceMethod);
          }
        else{
          if (enclosing_scope is FunctionScope){ //dealing with a nested function/function expression. Mangle the name.
            if (((FunctionScope)enclosing_scope).owner != null){
              this.name = ((FunctionScope)enclosing_scope).owner.name+ "." + this.name;
              jsFunctionAttributes |= JSFunctionAttributeEnum.IsNested;
            }else{ //dealing with the dummy outer scope of a function expression
              ScriptObject escope = enclosing_scope;
              while (escope != null){
                if (escope is FunctionScope && ((FunctionScope)escope).owner != null){
                  this.name = ((FunctionScope)escope).owner.name+ "." + this.name;
                  jsFunctionAttributes |= JSFunctionAttributeEnum.IsNested;
                  break;
                }
                escope = escope.GetParent();
              }
            }
          }
          //The name could still be a duplicate, so check against a table of names already used.
          if (compilerGlobals.usedNames[this.name] != null)
            this.name = this.name + ":" + (compilerGlobals.usedNames.count).ToString(CultureInfo.InvariantCulture);
          compilerGlobals.usedNames[this.name] = this;
          
          // Use the class writer associated with the global scope if the function is a global
          // function or a nested function inside a global function. In these cases, 
          // compilerGlobals.classwriter will be setup incorrectly if the function is called
          // from a code inside a class before it is declared.
          ScriptObject classScope = this.enclosing_scope;
          while (classScope != null && !(classScope is ClassScope))
            classScope = classScope.GetParent();
          this.classwriter = (classScope == null ? compilerGlobals.globalScopeClassWriter : compilerGlobals.classwriter);
          
          this.mb = this.classwriter.DefineMethod(this.name, this.attributes, rtype, ptypes);
        }
        // the method is defined, we can now put debug info about the parameters
        if (extras > 0){
          if (this.mb != null)
            this.mb.DefineParameter(1, (ParameterAttributes)0, "this");
          else{
            ParameterBuilder pb = this.cb.DefineParameter(np, (ParameterAttributes)0, "this");
            pb.SetConstant(null);
            extras = 0;
            np--;
          }
        }
        if (extras > 1)
          this.mb.DefineParameter(2, (ParameterAttributes)0, "vsa Engine");
        if (extras > 2)
          this.mb.DefineParameter(3, (ParameterAttributes)0, "arguments");
        for (int i = extras; i < np; i++){
          ParameterBuilder pb = this.mb != null ? 
            this.mb.DefineParameter(i + 1, (ParameterAttributes)0, this.parameter_declarations[i-extras].identifier) :
            this.cb.DefineParameter(i + 1, (ParameterAttributes)0, this.parameter_declarations[i-extras].identifier);
          CustomAttributeList paramCustAttList = this.parameter_declarations[i-extras].customAttributes;
          if (paramCustAttList != null){
            CustomAttributeBuilder[] paramCustomAttributes = paramCustAttList.GetCustomAttributeBuilders(false);
            for (int attIndex = 0; attIndex < paramCustomAttributes.Length; attIndex++)
              pb.SetCustomAttribute(paramCustomAttributes[attIndex]);
          }
        }
        if (jsFunctionAttributes > 0){
          CustomAttributeBuilder cab = new CustomAttributeBuilder(CompilerGlobals.jsFunctionAttributeConstructor, new Object[1] {jsFunctionAttributes});
          if (this.mb != null)
            this.mb.SetCustomAttribute(cab);
          else
            this.cb.SetCustomAttribute(cab);
        }
        // deal with custom attributes
        if (this.customAttributes != null){
          CustomAttributeBuilder[] custAtt = this.customAttributes.GetCustomAttributeBuilders(false);
          for (int j = 0; j < custAtt.Length; j++){
            if (this.mb != null)
              this.mb.SetCustomAttribute(custAtt[j]);
            else
              this.cb.SetCustomAttribute(custAtt[j]);
          }
        }
        if (this.clsCompliance == CLSComplianceSpec.CLSCompliant)
          if (this.mb != null)
            this.mb.SetCustomAttribute(new CustomAttributeBuilder(CompilerGlobals.clsCompliantAttributeCtor, new Object[]{true}));
          else
            this.cb.SetCustomAttribute(new CustomAttributeBuilder(CompilerGlobals.clsCompliantAttributeCtor, new Object[]{true}));
        else if (this.clsCompliance == CLSComplianceSpec.NonCLSCompliant)
          if (this.mb != null)
            this.mb.SetCustomAttribute(new CustomAttributeBuilder(CompilerGlobals.clsCompliantAttributeCtor, new Object[]{false}));
          else
            this.cb.SetCustomAttribute(new CustomAttributeBuilder(CompilerGlobals.clsCompliantAttributeCtor, new Object[]{false}));
        if (this.mb != null){
          this.mb.InitLocals = true;
          return this.mb;
        }else{
          this.cb.InitLocals = true;
          return this.cb;
        }
      }
      
      private static bool IsPresentIn(FieldInfo field, FieldInfo[] fields){
        for (int i = 0, n = fields.Length; i < n; i++)
          if (field == fields[i]) return true;
        return false;
      }
      
      internal bool Must_save_stack_locals{
        get{
          if (!this.partiallyEvaluated)
            this.PartiallyEvaluate();
          return this.must_save_stack_locals;
        }
      }
      
      internal void PartiallyEvaluate(){
        if (this.partiallyEvaluated)
          return;
        
        // 49885 - Putting class definitions in a package makes class order dependent
        // For methods, the logic in FunctionObject.PartiallyEvaluate must be called by
        // Class.PartiallyEvaluate in order to ensure that the scope chain is set up correctly.
        ClassScope classScope = this.enclosing_scope as ClassScope;
        if (null != classScope)
          classScope.owner.PartiallyEvaluate();

        // In the case where the the caller is Class.PartiallyEvaluate, the call returns
        // immediately since Class.partiallyEvaluated wil be set and this.partiallEvaluated
        // will still be false to allow the partial evaluation to be computed. In the case where 
        // the caller isn't Class.PartiallyEvaluate, the call to Class.PartiallyEvaluate results
        // in a call back to FunctionObject.PartiallyEvaluate. This call will set this.partiallyEvaluated
        // and thus, we will not need to evaluate again.
        if (this.partiallyEvaluated)
          return;
        
          
        this.partiallyEvaluated = true;
        // deal with custom attributes
        this.clsCompliance = CLSComplianceSpec.NotAttributed;
        if (this.customAttributes != null){
          this.customAttributes.PartiallyEvaluate();
          CustomAttribute clsAttr = this.customAttributes.GetAttribute(Typeob.CLSCompliantAttribute);
          if (clsAttr != null){
            this.clsCompliance = clsAttr.GetCLSComplianceValue();
            this.customAttributes.Remove(clsAttr);
          }
          clsAttr = this.customAttributes.GetAttribute(Typeob.Override);
          if (clsAttr != null){
            if (this.isStatic)
              clsAttr.context.HandleError(JSError.StaticMethodsCannotOverride);
            else
              this.attributes &= ~MethodAttributes.NewSlot;
            this.noVersionSafeAttributeSpecified = false;
            this.customAttributes.Remove(clsAttr);
          }
          clsAttr = this.customAttributes.GetAttribute(Typeob.Hide);
          if (clsAttr != null){
            if (!this.noVersionSafeAttributeSpecified){
              clsAttr.context.HandleError(JSError.OverrideAndHideUsedTogether);
              this.attributes |= MethodAttributes.NewSlot;
              this.noVersionSafeAttributeSpecified = true;
            }else{
              if (this.isStatic)
                clsAttr.context.HandleError(JSError.StaticMethodsCannotHide);
              this.noVersionSafeAttributeSpecified = false;
            }
            this.customAttributes.Remove(clsAttr);
          }
          CustomAttribute expAttr = this.customAttributes.GetAttribute(Typeob.Expando);
          if (expAttr != null){
            if (!this.noVersionSafeAttributeSpecified && (this.attributes & MethodAttributes.NewSlot) == 0){
              expAttr.context.HandleError(JSError.ExpandoPrecludesOverride);
              this.attributes |= MethodAttributes.NewSlot;
              this.noVersionSafeAttributeSpecified = true;
            }
            if (this.isConstructor)
              expAttr.context.HandleError(JSError.NotValidForConstructor);
            else if ((this.attributes & MethodAttributes.Abstract) != 0)
              expAttr.context.HandleError(JSError.ExpandoPrecludesAbstract);
            else if ((this.attributes & MethodAttributes.Static) != 0)
              expAttr.context.HandleError(JSError.ExpandoPrecludesStatic);
            else if ((this.attributes & MethodAttributes.MemberAccessMask) != MethodAttributes.Public)
              expAttr.context.HandleError(JSError.ExpandoMustBePublic);
            else{
              this.own_scope.isMethod = false;
              this.isMethod = false;
              this.isExpandoMethod = true;
              this.isStatic = true;
              this.attributes &= ~MethodAttributes.Virtual;
              this.attributes &= ~MethodAttributes.NewSlot;
              this.attributes |= MethodAttributes.Static;
            }
          }
        }
        for (int i = 0, n = this.parameter_declarations.Length; i < n; i++){
          this.parameter_declarations[i].PartiallyEvaluate();
          JSLocalField pfield = (JSLocalField)this.own_scope.name_table[this.formal_parameters[i]];
          pfield.type = this.parameter_declarations[i].type;
          if (pfield.type == null)
            pfield.type = new TypeExpression(new ConstantWrapper(Typeob.Object, this.parameter_declarations[i].context));
          pfield.isDefined = true;
        }
        if (this.return_type_expr != null){
          this.return_type_expr.PartiallyEvaluate();
          this.own_scope.returnVar.type = this.return_type_expr;
          if (this.own_scope.returnVar.type.ToIReflect() == Typeob.Void){
            this.own_scope.returnVar.type = null;
            this.own_scope.returnVar = null; //This does not completely get rid of the returnVar value, but acts as flag, hence the above.
          }
        }
        globals.ScopeStack.Push(this.own_scope);
        if (!this.own_scope.isKnownAtCompileTime) //Function contains an eval. Type inference is impossible for local vars
          for (int i = 0, n = this.fields.Length; i < n; i++)
            this.fields[i].SetInferredType(Typeob.Object, null);
        if (!this.isConstructor)
          this.body.PartiallyEvaluate();
        else{
          this.body.MarkSuperOKIfIsFirstStatement();
          this.body.PartiallyEvaluate(); //super constructor calls that have not been marked OK will generate errors
          ClassScope cscope = (ClassScope)this.enclosing_scope;
          int n = this.superConstructorCall == null ? 0 : this.superConstructorCall.arguments.count;
          Type[] argTypes = n == 0 ? Type.EmptyTypes : new Type[n];
          IReflect[] argIRs = new IReflect[n];
          for (int i = 0; i < n; i++)
            argIRs[i] = this.superConstructorCall.arguments[i].InferType(null);
          Context errorContext = this.superConstructorCall == null ? this.funcContext : this.superConstructorCall.context;
          try{
            if (this.superConstructorCall != null && !this.superConstructorCall.isSuperConstructorCall)
              this.superConstructor = JSBinder.SelectConstructor(cscope.constructors, argIRs);
            else
              this.superConstructor = cscope.owner.GetSuperConstructor(argIRs);
            if (this.superConstructor == null)
              errorContext.HandleError(JSError.SuperClassConstructorNotAccessible);
            else{
              ConstructorInfo cons = (ConstructorInfo)this.superConstructor;
              if (!cons.IsPublic && !cons.IsFamily && !cons.IsFamilyOrAssembly &&
                !(this.superConstructor is JSConstructor && ((JSConstructor)this.superConstructor).IsAccessibleFrom(this.enclosing_scope))){
                errorContext.HandleError(JSError.SuperClassConstructorNotAccessible);
                  this.superConstructor = null;
              }else if (n > 0)
                if (!Binding.CheckParameters(cons.GetParameters(), argIRs, this.superConstructorCall.arguments, this.superConstructorCall.context))
                  this.superConstructor = null;
            }
          }catch(AmbiguousMatchException){
            errorContext.HandleError(JSError.AmbiguousConstructorCall);
          }  
        }
        this.own_scope.HandleUnitializedVariables();
        globals.ScopeStack.Pop();
        this.must_save_stack_locals = this.own_scope.mustSaveStackLocals;
        this.fields = this.own_scope.GetLocalFields();
      }      
      
      internal IReflect ReturnType(JSField inference_target){
        if (!this.partiallyEvaluated)
          this.PartiallyEvaluate();
        if (this.own_scope.returnVar == null)
          return Typeob.Void;
        else if (this.return_type_expr != null)
          return this.return_type_expr.ToIReflect();
        else
          return this.own_scope.returnVar.GetInferredType(inference_target);
      }
    
      public override String ToString(){
        if (this.text != null)
          return this.text;
        else
          return this.funcContext.GetCode();
      }

      internal void TranslateBodyToIL(ILGenerator il, CompilerGlobals compilerGlobals){
        this.returnLabel = il.DefineLabel();
        // Emit all the namespaces used by this function
        if (this.body.Engine.GenerateDebugInfo){
          ScriptObject ns = this.enclosing_scope.GetParent();
          while (ns != null){
            if (ns is PackageScope)
              il.UsingNamespace(((PackageScope)ns).name);
            else if (ns is WrappedNamespace && !((WrappedNamespace)ns).name.Equals(""))
              il.UsingNamespace(((WrappedNamespace)ns).name);
            ns = ns.GetParent();
          }
        }
        int startLine = this.body.context.StartLine;
        int startCol = this.body.context.StartColumn;
        this.body.context.document.EmitLineInfo(il, startLine, startCol, startLine, startCol + 1);
        if (this.body.context.document.debugOn)
          il.Emit(OpCodes.Nop);
        //set up the compiler to emit direct accesses to the locals and pars.
        int n = this.fields.Length;
        for (int i = 0; i < n; i++){
          if (!this.fields[i].IsLiteral || this.fields[i].value is FunctionObject){
            Type t = this.fields[i].FieldType;
            LocalBuilder tok = il.DeclareLocal(t);
            if (this.fields[i].debugOn)
              tok.SetLocalSymInfo(this.fields[i].debuggerName);
            this.fields[i].metaData = tok;
          }
        }
        globals.ScopeStack.Push(this.own_scope);
        try{
          if (this.must_save_stack_locals){
            this.TranslateToMethodWithStackFrame(il, compilerGlobals, true);
            return;
          }
          this.body.TranslateToILInitializer(il);
          this.body.TranslateToIL(il, Typeob.Void);
          il.MarkLabel(this.returnLabel);
        }finally{
          globals.ScopeStack.Pop();
        }
      }
      
      internal void TranslateToIL(CompilerGlobals compilerGlobals){
        if (this.suppressIL) return;
        globals.ScopeStack.Push(this.own_scope);
        try{
          if (this.mb == null && this.cb == null)
            this.GetMethodBase(compilerGlobals);
          int offset = (this.attributes&MethodAttributes.Static) == MethodAttributes.Static ? 0 : 1;
          int extras = 3;
          if (this.isMethod)
            extras = 0;
          else if (!this.hasArgumentsObject)
            extras = 2;
          ILGenerator il = this.mb != null ? this.mb.GetILGenerator() : this.cb.GetILGenerator();
          this.returnLabel = il.DefineLabel();
          // Emit all the namespaces used by this function
          if (this.body.Engine.GenerateDebugInfo){
            ScriptObject ns = this.enclosing_scope.GetParent();
            while (ns != null){
              if (ns is PackageScope)
                il.UsingNamespace(((PackageScope)ns).name);
              else if (ns is WrappedNamespace && !((WrappedNamespace)ns).name.Equals(""))
                il.UsingNamespace(((WrappedNamespace)ns).name);
              ns = ns.GetParent();
            }
          }
          if (!this.isImplicitCtor && this.body != null){
            int startLine = this.body.context.StartLine;
            int startCol = this.body.context.StartColumn;
            this.body.context.document.EmitLineInfo(il, startLine, startCol, startLine, startCol + 1);
            if (this.body.context.document.debugOn)
              il.Emit(OpCodes.Nop);
          }
          //set up the compiler to emit direct accesses to the locals and pars.
          int n = this.fields.Length;
          for (int i = 0; i < n; i++){
            // Use indexed lookup for the parameters. We must check to make sure the field
            // is not a nested function as they might have the same name as a parameter.
            int j = this.IsNestedFunctionField(this.fields[i]) ? -1 : System.Array.IndexOf(this.formal_parameters, this.fields[i].Name);
            if (j >= 0)
              this.fields[i].metaData = (short)(j+extras+offset);
            else if (this.hasArgumentsObject && this.fields[i].Name.Equals("arguments"))
              this.fields[i].metaData = (short)(2+offset);
            else if (!this.fields[i].IsLiteral || this.fields[i].value is FunctionObject){
              Type t = this.fields[i].FieldType;
              LocalBuilder tok = il.DeclareLocal(t);
              if (this.fields[i].debugOn)
                tok.SetLocalSymInfo(this.fields[i].debuggerName);
              this.fields[i].metaData = tok;
            }else if (this.own_scope.mustSaveStackLocals){ //Need to emit a field. FunctionDeclaration will initialize it with a closure
              LocalBuilder tok = il.DeclareLocal(this.fields[i].FieldType);
              this.fields[i].metaData = tok;
            }
          }
          if (this.isConstructor){
            int pn = formal_parameters.Length+1;
            ClassScope cscope = (ClassScope)this.enclosing_scope;
            //Call the default super constructor followed by the field initializers
            if (this.superConstructor == null)
              cscope.owner.EmitInitialCalls(il, null, null, null, 0);
            else{
              ParameterInfo[] pars = this.superConstructor.GetParameters();
              if (this.superConstructorCall != null)
                cscope.owner.EmitInitialCalls(il, this.superConstructor, pars, this.superConstructorCall.arguments, pn);
              else
                cscope.owner.EmitInitialCalls(il, this.superConstructor, pars, null, pn);
            }
          }
          if ((this.isMethod || this.isConstructor) && this.must_save_stack_locals){
            this.TranslateToMethodWithStackFrame(il, compilerGlobals, false);
            return;
          }
          this.TranslateToILToCopyOuterScopeLocals(il, true, null);
          
          bool savedInsideProtectedRegion = compilerGlobals.InsideProtectedRegion;
          compilerGlobals.InsideProtectedRegion = false;
          bool savedInsideFinally = compilerGlobals.InsideFinally;
          int savedFinallyStackTop = compilerGlobals.FinallyStackTop;
          compilerGlobals.InsideFinally = false;
          this.body.TranslateToILInitializer(il);
          this.body.TranslateToIL(il, Typeob.Void);
          compilerGlobals.InsideProtectedRegion = savedInsideProtectedRegion;
          compilerGlobals.InsideFinally = savedInsideFinally;
          compilerGlobals.FinallyStackTop = savedFinallyStackTop;
          il.MarkLabel(this.returnLabel);
          if (this.body.context.document.debugOn){
            this.EmitLastLineInfo(il);
            il.Emit(OpCodes.Nop);
          }
          this.TranslateToILToSaveLocals(il); //put locals on heap so that nested functions can get to them
          if (this.own_scope.returnVar != null)
            il.Emit(OpCodes.Ldloc, (LocalBuilder)this.own_scope.returnVar.GetMetaData());
          il.Emit(OpCodes.Ret);
        }finally{
          globals.ScopeStack.Pop();
        }
      }
      
      private bool IsNestedFunctionField(JSLocalField field) {
        return field.value != null && field.value is FunctionObject;
      }
      
      internal void TranslateToILToLoadEngine(ILGenerator il){
        this.TranslateToILToLoadEngine(il, false);
      }

      private void TranslateToILToLoadEngine(ILGenerator il, bool allocateLocal){
        if (this.isMethod)
          if (this.isStatic)
            if (this.body.Engine.doCRS)   
              il.Emit(OpCodes.Ldsfld, CompilerGlobals.contextEngineField);
            else{
              if (this.engineLocal == null){
                if (allocateLocal)
                  this.engineLocal = il.DeclareLocal(Typeob.VsaEngine);
                if (this.body.Engine.PEFileKind == PEFileKinds.Dll) {
                  il.Emit(OpCodes.Ldtoken, ((ClassScope)this.own_scope.GetParent()).GetTypeBuilder());
                  il.Emit(OpCodes.Call, CompilerGlobals.createVsaEngineWithType);
                }else
                  il.Emit(OpCodes.Call, CompilerGlobals.createVsaEngine);
                if (allocateLocal)
                  il.Emit(OpCodes.Stloc, this.engineLocal);
                else
                  return;
              }
              il.Emit(OpCodes.Ldloc, this.engineLocal);
            }
          else{
            il.Emit(OpCodes.Ldarg_0);
            il.Emit(OpCodes.Callvirt, CompilerGlobals.getEngineMethod);
          }
        else
          il.Emit(OpCodes.Ldarg_1); //get it from a parameter when inside a non-method
      }
      
      private void TranslateToMethodWithStackFrame(ILGenerator il, CompilerGlobals compilerGlobals, bool staticInitializer){
        //Can get here if method contains a nested function or if method contains an eval
        if (this.isStatic)
          il.Emit(OpCodes.Ldtoken, ((ClassScope)this.own_scope.GetParent()).GetTypeBuilder());
        else
          il.Emit(OpCodes.Ldarg_0);
        int n = this.fields.Length;
        ConstantWrapper.TranslateToILInt(il, n);
        il.Emit(OpCodes.Newarr, Typeob.JSLocalField);
        for (int i = 0; i < n; i++){
          JSLocalField field = this.fields[i];
          il.Emit(OpCodes.Dup);
          ConstantWrapper.TranslateToILInt(il, i);
          il.Emit(OpCodes.Ldstr, field.Name);
          il.Emit(OpCodes.Ldtoken, field.FieldType);
          ConstantWrapper.TranslateToILInt(il, field.slotNumber);
          il.Emit(OpCodes.Newobj, CompilerGlobals.jsLocalFieldConstructor);
          il.Emit(OpCodes.Stelem_Ref);
        }
        this.TranslateToILToLoadEngine(il, true);
        if (this.isStatic)
          il.Emit(OpCodes.Call, CompilerGlobals.pushStackFrameForStaticMethod);
        else
          il.Emit(OpCodes.Call, CompilerGlobals.pushStackFrameForMethod);
        bool savedInsideProtectedRegion = compilerGlobals.InsideProtectedRegion;
        compilerGlobals.InsideProtectedRegion = true;
        il.BeginExceptionBlock(); 
        this.body.TranslateToILInitializer(il);
        this.body.TranslateToIL(il, Typeob.Void);
        il.MarkLabel(this.returnLabel);
        this.TranslateToILToSaveLocals(il); //put locals on heap so that nested functions can get to them
        Label retLabel = il.DefineLabel();
        il.Emit(OpCodes.Leave, retLabel);
        il.BeginFinallyBlock();
        this.TranslateToILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.popScriptObjectMethod);
        il.Emit(OpCodes.Pop);
        il.EndExceptionBlock();
        il.MarkLabel(retLabel);
        if (!staticInitializer){
          if (this.body.context.document.debugOn){
            this.EmitLastLineInfo(il);
            il.Emit(OpCodes.Nop);
          }
          if (this.own_scope.returnVar != null)
            il.Emit(OpCodes.Ldloc, (LocalBuilder)this.own_scope.returnVar.GetMetaData());
          il.Emit(OpCodes.Ret);
        }
        compilerGlobals.InsideProtectedRegion = savedInsideProtectedRegion;
      }
      
      //Parameters and local variables might be accessed from eval or from nested functions. 
      //These cannot get to the locals via IL, so the locals are saved to an array of Objects before every eval or call out of the function.
      //Upon return, they are restored from the array to the locals.
      
      internal void TranslateToILToRestoreLocals(ILGenerator il){
        this.TranslateToILToRestoreLocals(il, null);
      }
      
      internal void TranslateToILToRestoreLocals(ILGenerator il, JSLocalField[] notToBeRestored){  
        this.TranslateToILToCopyOuterScopeLocals(il, true, notToBeRestored);
        if (!this.must_save_stack_locals)
          return;
        int offset = (this.attributes&MethodAttributes.Static) == MethodAttributes.Static ? 0 : 1;
        int extras = 3;
        if (this.isMethod)
          extras = 0;
        else if (!this.hasArgumentsObject)
          extras = 2;
        int n = this.fields.Length;
        this.TranslateToILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.scriptObjectStackTopMethod);
        ScriptObject scope = globals.ScopeStack.Peek();
        while (scope is WithObject || scope is BlockScope){
          il.Emit(OpCodes.Call, CompilerGlobals.getParentMethod);
          scope = scope.GetParent();
        }
        il.Emit(OpCodes.Castclass, Typeob.StackFrame);
        il.Emit(OpCodes.Ldfld, CompilerGlobals.localVarsField);
        for (int i = 0; i < n; i++){
          if (notToBeRestored != null && IsPresentIn(this.fields[i], notToBeRestored)) continue;
          if (this.fields[i].IsLiteral) continue;
          il.Emit(OpCodes.Dup);
          int j = System.Array.IndexOf(this.formal_parameters, this.fields[i].Name);
          ConstantWrapper.TranslateToILInt(il, this.fields[i].slotNumber);
          il.Emit(OpCodes.Ldelem_Ref);
          Convert.Emit(this.body, il, Typeob.Object, this.fields[i].FieldType);
          if (j >= 0 || (this.fields[i].Name.Equals("arguments") && this.hasArgumentsObject)){
            il.Emit(OpCodes.Starg, (short)(j+extras+offset));
          }else{
            il.Emit(OpCodes.Stloc, (LocalBuilder)this.fields[i].metaData);
          }
        }
        il.Emit(OpCodes.Pop);
      }
      
      internal void TranslateToILToSaveLocals(ILGenerator il){
        this.TranslateToILToCopyOuterScopeLocals(il, false, null);
        if (!this.must_save_stack_locals)
          return;
        int offset = (this.attributes&MethodAttributes.Static) == MethodAttributes.Static ? 0 : 1;
        int extras = 3;
        if (this.isMethod)
          extras = 0;
        else if (!this.hasArgumentsObject)
          extras = 2;
        int n = this.fields.Length;
        this.TranslateToILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.scriptObjectStackTopMethod);
        ScriptObject scope = globals.ScopeStack.Peek();
        while (scope is WithObject || scope is BlockScope){
          il.Emit(OpCodes.Call, CompilerGlobals.getParentMethod);
          scope = scope.GetParent();
        }
        il.Emit(OpCodes.Castclass, Typeob.StackFrame);
        il.Emit(OpCodes.Ldfld, CompilerGlobals.localVarsField);
        for (int i = 0; i < n; i++){
          JSLocalField f = this.fields[i];
          if (f.IsLiteral && !(f.value is FunctionObject))
            continue;
          il.Emit(OpCodes.Dup);
          ConstantWrapper.TranslateToILInt(il, f.slotNumber);
          int j = System.Array.IndexOf(this.formal_parameters, f.Name);
          if (j >= 0 || (f.Name.Equals("arguments") && this.hasArgumentsObject)){
            Convert.EmitLdarg(il, (short)(j+extras+offset));
          }else{
            il.Emit(OpCodes.Ldloc, (LocalBuilder)f.metaData);
          }
          Convert.Emit(this.body, il, f.FieldType, Typeob.Object);
          il.Emit(OpCodes.Stelem_Ref);
        }
        il.Emit(OpCodes.Pop);
        
      }
      
      //Nested functions can access the locals of their containing functions. There is no way to do this in IL.
      //The work around is to copy the values of the outer scope locals to true locals at function entry and
      //then to copy the modified values back at function exit.
      //The outer scopes save their locals in an array before making calls, hence they are accessible.
      
      private void TranslateToILToCopyOuterScopeLocals(ILGenerator il, bool copyToNested, JSLocalField[] notToBeRestored){
        if (this.own_scope.ProvidesOuterScopeLocals == null || this.own_scope.ProvidesOuterScopeLocals.count == 0)
          return;
        this.TranslateToILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.scriptObjectStackTopMethod);
        ScriptObject scope = globals.ScopeStack.Peek();
        while (scope is WithObject || scope is BlockScope){
          il.Emit(OpCodes.Call, CompilerGlobals.getParentMethod);
          scope = scope.GetParent();
        }
        scope = this.enclosing_scope;
        while (scope != null){
          //Run through the scope chain, copying locals from outer scopes that provide them.
          il.Emit(OpCodes.Call, CompilerGlobals.getParentMethod);
          if (scope is FunctionScope && ((FunctionScope)scope).owner != null && this.own_scope.ProvidesOuterScopeLocals[scope] != null){
            il.Emit(OpCodes.Dup);
            il.Emit(OpCodes.Castclass, Typeob.StackFrame);
            il.Emit(OpCodes.Ldfld, CompilerGlobals.localVarsField);
            if (copyToNested)
              ((FunctionScope)scope).owner.TranslateToILToCopyLocalsToNestedScope(il, this.own_scope, notToBeRestored);
            else
              ((FunctionScope)scope).owner.TranslateToILToCopyLocalsFromNestedScope(il, this.own_scope);
          }else if (scope is GlobalScope || scope is ClassScope)
            break;
          scope = scope.GetParent();
        }
        il.Emit(OpCodes.Pop);
      }
      
      private void TranslateToILToCopyLocalsToNestedScope(ILGenerator il, FunctionScope nestedScope, JSLocalField[] notToBeRestored){
        int offset = (this.attributes&MethodAttributes.Static) == MethodAttributes.Static ? 0 : 1;
        int n = this.fields.Length;
        for (int i = 0; i < n; i++){
          JSLocalField field = nestedScope.GetOuterLocalField(this.fields[i].Name);
          if (field == null || field.outerField != this.fields[i] || (notToBeRestored != null && IsPresentIn(field, notToBeRestored)))
            continue;
          il.Emit(OpCodes.Dup);
          ConstantWrapper.TranslateToILInt(il, this.fields[i].slotNumber);
          il.Emit(OpCodes.Ldelem_Ref);
          Convert.Emit(this.body, il, Typeob.Object, this.fields[i].FieldType);
          il.Emit(OpCodes.Stloc, (LocalBuilder)field.metaData);
        }
        il.Emit(OpCodes.Pop);
      }
      
      private void TranslateToILToCopyLocalsFromNestedScope(ILGenerator il, FunctionScope nestedScope){
        int offset = (this.attributes&MethodAttributes.Static) == MethodAttributes.Static ? 0 : 1;
        int n = this.fields.Length;
        for (int i = 0; i < n; i++){
          JSLocalField field = nestedScope.GetOuterLocalField(this.fields[i].Name);
          if (field == null || field.outerField != this.fields[i])
            continue;
          il.Emit(OpCodes.Dup);
          ConstantWrapper.TranslateToILInt(il, this.fields[i].slotNumber);
          il.Emit(OpCodes.Ldloc, (LocalBuilder)field.metaData);
          Convert.Emit(this.body, il, this.fields[i].FieldType, Typeob.Object);
          il.Emit(OpCodes.Stelem_Ref);
        }
        il.Emit(OpCodes.Pop);
      }
    }
}
