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

    internal sealed class Lookup : Binding{
      private int lexLevel; //Keeps track of the number of frames on the explicit scope stack that needs to be searched for a late binding
      private int evalLexLevel; //Keeps track of the number of frames to GetObject should skip
      private LocalBuilder fieldLoc;
      private LocalBuilder refLoc;
      private LateBinding lateBinding;
      private bool thereIsAnObjectOnTheStack;

      // this is the typical Lookup constructor. The context points to the identifier
      internal Lookup(Context context)
        : base(context, context.GetCode()){
        this.lexLevel = 0;
        this.evalLexLevel = 0;
        this.fieldLoc = null;
        this.refLoc = null;
        this.lateBinding = null;
        this.thereIsAnObjectOnTheStack = false;
      }

      // this constructor is invoked when there has been a parse error. The typical scenario is a missing identifier.
      internal Lookup(String name, Context context)
        : this(context){
        this.name = name;
      }

      internal String Name{
        get{
          return this.name;
        }
      }

      private void BindName(){
        int lexLevel = 0;
        int evalLexLevel = 0;
        ScriptObject scope = Globals.ScopeStack.Peek();
        BindingFlags flags = BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance|BindingFlags.Static|BindingFlags.DeclaredOnly;
        bool staticClassFound = false; // is there a static class on the scope chain
        bool enclosingClassForStaticClassFound = false; // has the class enclosing a static nested class been found
        while (scope != null){
          MemberInfo[] members = null;
          WithObject withScope = scope as WithObject;
          if (withScope != null && enclosingClassForStaticClassFound)
            members = withScope.GetMember(this.name, flags, false);
          else
            members = scope.GetMember(this.name, flags);
          this.members = members;
          if (members.Length > 0) break;
          if (scope is WithObject){
            this.isFullyResolved = this.isFullyResolved && ((WithObject)scope).isKnownAtCompileTime;
            lexLevel++;
          }else if (scope is ActivationObject){
            this.isFullyResolved = this.isFullyResolved && ((ActivationObject)scope).isKnownAtCompileTime;
            if (scope is BlockScope || (scope is FunctionScope && ((FunctionScope)scope).mustSaveStackLocals))
              lexLevel++;
            if (scope is ClassScope){
              if (staticClassFound)
                enclosingClassForStaticClassFound = true;
              if (((ClassScope)scope).owner.isStatic){
                flags &= ~BindingFlags.Instance;
                staticClassFound = true;
              }
            }
          }else if (scope is StackFrame)
            lexLevel++;
          evalLexLevel++;
          scope = scope.GetParent();
        }
        if (this.members.Length > 0){
          this.lexLevel = lexLevel;
          this.evalLexLevel = evalLexLevel;
        }
      }

      internal bool CanPlaceAppropriateObjectOnStack(Object ob){
        if (ob is LenientGlobalObject) return true;
        ScriptObject scope = Globals.ScopeStack.Peek();
        int lexLev = this.lexLevel;
        while (lexLev > 0 && (scope is WithObject || scope is BlockScope)){
          if (scope is WithObject) lexLev--;
          scope = scope.GetParent();
        }
        return scope is WithObject || scope is GlobalScope;
      }

      internal override void CheckIfOKToUseInSuperConstructorCall(){
        FieldInfo f = this.member as FieldInfo;
        if (f != null){
          if (!f.IsStatic)
            this.context.HandleError(JSError.NotAllowedInSuperConstructorCall);
          return;
        }
        MethodInfo m = this.member as MethodInfo;
        if (m != null){
          if (!m.IsStatic)
            this.context.HandleError(JSError.NotAllowedInSuperConstructorCall);
          return;
        }
        PropertyInfo p = this.member as PropertyInfo;
        if (p != null){
          m = JSProperty.GetGetMethod(p, true);
          if (m != null && !m.IsStatic)
            this.context.HandleError(JSError.NotAllowedInSuperConstructorCall);
          else{
            m = JSProperty.GetSetMethod(p, true);
            if (m != null && !m.IsStatic)
              this.context.HandleError(JSError.NotAllowedInSuperConstructorCall);
          }
        }
      }

      internal override Object Evaluate(){
        Object result = null;
        ScriptObject scope = Globals.ScopeStack.Peek();
        if (!this.isFullyResolved){
          result = ((IActivationObject)scope).GetMemberValue(this.name, this.evalLexLevel);
          if (!(result is Missing))
            return result;
        }
        if (this.members == null && !VsaEngine.executeForJSEE){
          this.BindName();
          this.ResolveRHValue();
        }
        result = base.Evaluate();
        if (result is Missing)
          throw new JScriptException(JSError.UndefinedIdentifier, this.context);
        return result;
      }

      internal override LateBinding EvaluateAsLateBinding(){
        if (!this.isFullyResolved){
          this.BindName(); //Doing this at runtime. Binding is now final (for this call). Do it here so that scope chain logic works as at compile time.
          this.isFullyResolved = false;
        }
        if (this.defaultMember == this.member) this.defaultMember = null;
        Object ob = this.GetObject();
        LateBinding lb = this.lateBinding;
        if (lb == null)
          lb = this.lateBinding = new LateBinding(this.name, ob, VsaEngine.executeForJSEE);
        lb.obj = ob;
        lb.last_object = ob;
        lb.last_members = this.members;
        lb.last_member = this.member;
        if (!this.isFullyResolved)
          this.members = null;
        return lb;
      }

      internal override WrappedNamespace EvaluateAsWrappedNamespace(bool giveErrorIfNameInUse){
        Namespace ns = Namespace.GetNamespace(this.name, this.Engine);
        GlobalScope scope = ((IActivationObject)Globals.ScopeStack.Peek()).GetGlobalScope();
        FieldInfo field = giveErrorIfNameInUse ? scope.GetLocalField(this.name) : scope.GetField(this.name, BindingFlags.Public|BindingFlags.Static);
        if (field != null){
          if (giveErrorIfNameInUse && (!field.IsLiteral || !(field.GetValue(null) is Namespace)))
            this.context.HandleError(JSError.DuplicateName, true);
        }else{
          field = scope.AddNewField(this.name, ns, FieldAttributes.Literal|FieldAttributes.Public);
          ((JSVariableField)field).type = new TypeExpression(new ConstantWrapper(Typeob.Namespace, this.context));
          ((JSVariableField)field).originalContext = this.context;
        }
        return new WrappedNamespace(this.name, this.Engine);
      }

      protected override Object GetObject(){
        Object result;
        ScriptObject scope = Globals.ScopeStack.Peek();
        if (this.member is JSMemberField){
          while (scope != null){
            StackFrame sf = scope as StackFrame;
            if (sf != null) {result = sf.closureInstance; goto finish;}
            scope = scope.GetParent();
          }
          return null;
        }
        for (int i = this.evalLexLevel; i > 0; i--)
          scope = scope.GetParent();
        result = scope;
      finish:
        if (this.defaultMember != null)
          switch(this.defaultMember.MemberType){
            case MemberTypes.Field: return ((FieldInfo)this.defaultMember).GetValue(result);
            case MemberTypes.Method: return ((MethodInfo)this.defaultMember).Invoke(result, new Object[0]);
            case MemberTypes.Property: return ((PropertyInfo)this.defaultMember).GetValue(result, null);
            case MemberTypes.Event: return null;
            case MemberTypes.NestedType: return member;
          }
        return result;
      }

      protected override void HandleNoSuchMemberError(){
        if (!this.isFullyResolved) return;
        this.context.HandleError(JSError.UndeclaredVariable, this.Engine.doFast);
      }

      internal override IReflect InferType(JSField inference_target){
        if (!this.isFullyResolved)
          return Typeob.Object;
        return base.InferType(inference_target);
      }

      internal bool InFunctionNestedInsideInstanceMethod(){
        ScriptObject scope = this.Globals.ScopeStack.Peek();
        while (scope is WithObject || scope is BlockScope)
          scope = scope.GetParent();
        FunctionScope fscope = scope as FunctionScope;
        while (fscope != null){
          if (fscope.owner.isMethod) return !fscope.owner.isStatic;
          scope = fscope.owner.enclosing_scope;
          while (scope is WithObject || scope is BlockScope)
            scope = scope.GetParent();
          fscope = scope as FunctionScope;
        }
        return false;
      }

      internal bool InStaticCode(){
        ScriptObject scope = this.Globals.ScopeStack.Peek();
        while (scope is WithObject || scope is BlockScope)
          scope = scope.GetParent();
        FunctionScope fscope = scope as FunctionScope;
        if (fscope != null)
          return fscope.isStatic;
        StackFrame sframe = scope as StackFrame; //Might be a StackFrame if called from an eval
        if (sframe != null)
          return sframe.thisObject is Type;
        ClassScope cscope = scope as ClassScope; //Happens when the initializers of variable declarations are partially evaluated
        if (cscope != null)
          return cscope.inStaticInitializerCode;
        return true;
      }

      internal override AST PartiallyEvaluate(){
        this.BindName();
        if (this.members == null || this.members.Length == 0){
          //Give a warning, unless inside a with statement
          ScriptObject scope = Globals.ScopeStack.Peek();
          while (scope is FunctionScope) scope = scope.GetParent();
          if (!(scope is WithObject) || this.isFullyResolved)
            this.context.HandleError(JSError.UndeclaredVariable, this.isFullyResolved && this.Engine.doFast);
        }else{
          this.ResolveRHValue();
          MemberInfo member = this.member;
          if (member is FieldInfo){
            FieldInfo field = (FieldInfo)member;
            if (field is JSLocalField && !((JSLocalField)field).isDefined){
              ((JSLocalField)field).isUsedBeforeDefinition = true;
              this.context.HandleError(JSError.VariableMightBeUnitialized);
            }
            if (field.IsLiteral){
              Object val = field is JSVariableField ? ((JSVariableField)field).value : TypeReferences.GetConstantValue(field);
              if (val is AST){
                AST pval = ((AST)val).PartiallyEvaluate();
                if (pval is ConstantWrapper && this.isFullyResolved)
                  return pval;
                val = null;
              }
              if (!(val is FunctionObject) && this.isFullyResolved)
                return new ConstantWrapper(val, this.context);
            }else if (field.IsInitOnly && field.IsStatic && field.DeclaringType == Typeob.GlobalObject && this.isFullyResolved)
              return new ConstantWrapper(field.GetValue(null), this.context);
          }else if (member is PropertyInfo){
            PropertyInfo prop = (PropertyInfo)member;
            if (!prop.CanWrite && !(prop is JSProperty) && prop.DeclaringType == Typeob.GlobalObject && this.isFullyResolved)
              return new ConstantWrapper(prop.GetValue(null, null), this.context);
          }
          if (member is Type && this.isFullyResolved)
            return new ConstantWrapper(member, this.context);
        }
        return this;
      }

      internal override AST PartiallyEvaluateAsCallable(){
        this.BindName();
        return this;
      }

      internal override AST PartiallyEvaluateAsReference(){
        this.BindName();
        if (this.members == null || this.members.Length == 0){
          //Give a warning, unless inside a with statement
          ScriptObject scope = Globals.ScopeStack.Peek();
          if (!(scope is WithObject) || this.isFullyResolved)
            this.context.HandleError(JSError.UndeclaredVariable, this.isFullyResolved && this.Engine.doFast);
        }else
          this.ResolveLHValue();
        return this;
      }


      internal override Object ResolveCustomAttribute(ASTList args, IReflect[] argIRs, AST target){
        if (this.name == "expando")
          this.members = Typeob.Expando.GetConstructors(BindingFlags.Instance|BindingFlags.Public);
        else if (this.name == "override")
          this.members = Typeob.Override.GetConstructors(BindingFlags.Instance|BindingFlags.Public);
        else if (this.name == "hide")
          this.members = Typeob.Hide.GetConstructors(BindingFlags.Instance|BindingFlags.Public);
        else if (this.name == "...")
          this.members = Typeob.ParamArrayAttribute.GetConstructors(BindingFlags.Instance|BindingFlags.Public);
        else{
          this.name = this.name + "Attribute";
          this.BindName();
          if (this.members == null || this.members.Length == 0){
            this.name = this.name.Substring(0, this.name.Length-9);
            this.BindName();
          }
        }
        return base.ResolveCustomAttribute(args, argIRs, target);
      }

      internal override void SetPartialValue(AST partial_value){
        //ResolveLHValue has already been called and has already checked if the target is accessible and assignable
        if (this.members == null || this.members.Length == 0) return; //Assignment to an undeclared variable. Nothing further to do.
        if (this.member is JSLocalField){
          //If we are dealing with an assignment to an untyped local variable, we need to tell the type inferencer about the assignment
          JSLocalField lfield = (JSLocalField)this.member;
          if (lfield.type == null){
            IReflect ir = partial_value.InferType(lfield);
            if (ir == Typeob.String && partial_value is Plus)
              lfield.SetInferredType(Typeob.Object, partial_value);
            else
              lfield.SetInferredType(ir, partial_value);
            //but then we are done
            return;
          }
          lfield.isDefined = true;
        }
        Binding.AssignmentCompatible(this.InferType(null), partial_value, partial_value.InferType(null), this.isFullyResolved);
      }

      internal override void SetValue(Object value){
        if (!this.isFullyResolved){
          this.EvaluateAsLateBinding().SetValue(value);
          return;
        }
        base.SetValue(value);
      }

      internal void SetWithValue(WithObject scope, Object value){
        Debug.Assert(!this.isFullyResolved);
        FieldInfo withField = scope.GetField(this.name, this.lexLevel);
        if (withField != null){
          withField.SetValue(scope, value);
          return;
        }
      }

      //code in parser relies on this.name being returned from here
      public override String ToString(){
        return this.name;
      }

      internal override void TranslateToIL(ILGenerator il, Type rtype){
        if (this.isFullyResolved){
          base.TranslateToIL(il, rtype);
          return;
        }
        //Need to do a dynamic lookup to see if there is a dynamically introduced (by eval or with) hiding definition.
        Label done = il.DefineLabel();
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.scriptObjectStackTopMethod);
        il.Emit(OpCodes.Castclass, Typeob.IActivationObject);
        il.Emit(OpCodes.Ldstr, this.name);
        ConstantWrapper.TranslateToILInt(il, this.lexLevel);
        il.Emit(OpCodes.Callvirt, CompilerGlobals.getMemberValueMethod);
        il.Emit(OpCodes.Dup);
        il.Emit(OpCodes.Call, CompilerGlobals.isMissingMethod);
        il.Emit(OpCodes.Brfalse, done); //dynamic lookup succeeded, do not use the value from the early bound location.
        il.Emit(OpCodes.Pop); //The dynamic lookup returned Missing.Value, discard it and go on to use the value from the early bound location.
        base.TranslateToIL(il, Typeob.Object);
        il.MarkLabel(done);
        Convert.Emit(this, il, Typeob.Object, rtype);
      }

      internal override void TranslateToILCall(ILGenerator il, Type rtype, ASTList argList, bool construct, bool brackets){
        if (this.isFullyResolved){
          base.TranslateToILCall(il, rtype, argList, construct, brackets);
          return;
        }
        //Need to do a dynamic lookup to see if there is a dynamically introduced (by eval or with) hiding definition.
        Label lateBoundCall = il.DefineLabel();
        Label done = il.DefineLabel();
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.scriptObjectStackTopMethod);
        il.Emit(OpCodes.Castclass, Typeob.IActivationObject);
        il.Emit(OpCodes.Ldstr, this.name);
        ConstantWrapper.TranslateToILInt(il, this.lexLevel);
        il.Emit(OpCodes.Callvirt, CompilerGlobals.getMemberValueMethod);
        il.Emit(OpCodes.Dup);
        il.Emit(OpCodes.Call, CompilerGlobals.isMissingMethod);
        il.Emit(OpCodes.Brfalse, lateBoundCall); //dynamic lookup succeeded, do not call early bound method
        il.Emit(OpCodes.Pop); //The dynamic lookup returned Missing.Value, discard it and go on to use the value from the early bound location.
        base.TranslateToILCall(il, Typeob.Object, argList, construct, brackets);
        il.Emit(OpCodes.Br, done); //Skip over the latebound call sequence
        il.MarkLabel(lateBoundCall);
        this.TranslateToILDefaultThisObject(il);
        argList.TranslateToIL(il, Typeob.ArrayOfObject);
        if (construct)
          il.Emit(OpCodes.Ldc_I4_1);
        else
          il.Emit(OpCodes.Ldc_I4_0);
        if (brackets)
          il.Emit(OpCodes.Ldc_I4_1);
        else
          il.Emit(OpCodes.Ldc_I4_0);
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.callValue2Method);
        il.MarkLabel(done);
        Convert.Emit(this, il, Typeob.Object, rtype);
      }

      internal void TranslateToILDefaultThisObject(ILGenerator il){
        this.TranslateToILDefaultThisObject(il, 0);
      }

      private void TranslateToILDefaultThisObject(ILGenerator il, int lexLevel){
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.scriptObjectStackTopMethod);
        while (lexLevel-- > 0)
          il.Emit(OpCodes.Call, CompilerGlobals.getParentMethod);
        il.Emit(OpCodes.Castclass, Typeob.IActivationObject);
        il.Emit(OpCodes.Callvirt, CompilerGlobals.getDefaultThisObjectMethod);
      }

      internal override void TranslateToILInitializer(ILGenerator il){
        if (this.defaultMember != null) return;
        if (this.member != null)
          switch(this.member.MemberType){
            case MemberTypes.Constructor:
            case MemberTypes.Method:
            case MemberTypes.NestedType:
            case MemberTypes.Property:
            case MemberTypes.TypeInfo: return;
            case MemberTypes.Field:
              if (this.member is JSExpandoField){
                this.member = null; break;
              }
              return;
          }
        this.refLoc = il.DeclareLocal(Typeob.LateBinding);
        il.Emit(OpCodes.Ldstr, this.name);        
        
        if (this.isFullyResolved && this.member == null && this.IsBoundToMethodInfos()) {
          MethodInfo method = this.members[0] as MethodInfo;
          if (method.IsStatic) {
            il.Emit(OpCodes.Ldtoken, method.DeclaringType);
            il.Emit(OpCodes.Call, CompilerGlobals.getTypeFromHandleMethod);
          } else
            this.TranslateToILObjectForMember(il, method.DeclaringType, false, method);
        } else {
          this.EmitILToLoadEngine(il);
          il.Emit(OpCodes.Call, CompilerGlobals.scriptObjectStackTopMethod);
        }
        il.Emit(OpCodes.Newobj, CompilerGlobals.lateBindingConstructor2);
        il.Emit(OpCodes.Stloc, this.refLoc);
      }
      
      private bool IsBoundToMethodInfos() {
        if (this.members == null || this.members.Length == 0)
          return false;
        for (int i = 0; i < this.members.Length; i++) 
          if (!(this.members[i] is MethodInfo))
            return false;            
        return true;
      }

      protected override void TranslateToILObject(ILGenerator il, Type obType, bool noValue){
        this.TranslateToILObjectForMember(il, obType, noValue, this.member);
      }
      
      private void TranslateToILObjectForMember(ILGenerator il, Type obType, bool noValue, MemberInfo mem) {
        this.thereIsAnObjectOnTheStack = true;
        if (mem is IWrappedMember){
          Object obj = ((IWrappedMember)mem).GetWrappedObject();
          if (obj is LenientGlobalObject){
            this.EmitILToLoadEngine(il);
            il.Emit(OpCodes.Call, CompilerGlobals.getLenientGlobalObjectMethod);
          }else if (obj is Type || obj is ClassScope){
            if (obType.IsAssignableFrom(Typeob.Type)){
              (new ConstantWrapper(obj, null)).TranslateToIL(il, Typeob.Type);
              return;
            }
            //this.name is the name of an instance member of the superclass of the class (or an outer class) in which this expression appears
            //get class instance on stack
            ScriptObject scope = Globals.ScopeStack.Peek();
            while (scope is WithObject || scope is BlockScope)
              scope = scope.GetParent();
            if (scope is FunctionScope){
              FunctionObject func = ((FunctionScope)scope).owner;
              if (func.isMethod)
                il.Emit(OpCodes.Ldarg_0);
              else{ //Need to get the StackFrame.closureInstance on the stack
                this.EmitILToLoadEngine(il);
                il.Emit(OpCodes.Call, CompilerGlobals.scriptObjectStackTopMethod);
                scope = this.Globals.ScopeStack.Peek();
                while (scope is WithObject || scope is BlockScope){
                  il.Emit(OpCodes.Call, CompilerGlobals.getParentMethod);
                  scope = scope.GetParent();
                }
                il.Emit(OpCodes.Castclass, Typeob.StackFrame);
                il.Emit(OpCodes.Ldfld, CompilerGlobals.closureInstanceField);
              }
            }else if (scope is ClassScope){
              il.Emit(OpCodes.Ldarg_0); //Inside instance initializer
            }

            //If dealing with a member of an outer class, get the outer class instance on the stack
            scope = Globals.ScopeStack.Peek();
            while (scope != null){
              ClassScope csc = scope as ClassScope;
              if (csc != null){ //Might be dealing with a reference from within a nested class to an instance member of an outer class
                if (csc.IsSameOrDerivedFrom(obType)) break;
                il.Emit(OpCodes.Ldfld, csc.outerClassField);
              }
              scope = scope.GetParent();
            }
          }else{
            this.TranslateToILDefaultThisObject(il, this.lexLevel);
            Convert.Emit(this, il, Typeob.Object, obType);
          }
        }else{
          ScriptObject scope = Globals.ScopeStack.Peek();
          while (scope is WithObject || scope is BlockScope)
            scope = scope.GetParent();
          if (scope is FunctionScope){
            FunctionObject func = ((FunctionScope)scope).owner;
            if (!func.isMethod){ //Need to get the StackFrame.closureInstance on the stack
              this.EmitILToLoadEngine(il);
              il.Emit(OpCodes.Call, CompilerGlobals.scriptObjectStackTopMethod);
              scope = this.Globals.ScopeStack.Peek();
              while (scope is WithObject || scope is BlockScope){
                il.Emit(OpCodes.Call, CompilerGlobals.getParentMethod);
                scope = scope.GetParent();
              }
              il.Emit(OpCodes.Castclass, Typeob.StackFrame);
              il.Emit(OpCodes.Ldfld, CompilerGlobals.closureInstanceField);
              while (scope != null){
                if (scope is ClassScope){ //Might be dealing with a reference from within a nested class to an instance member of an outer class
                  ClassScope csc = (ClassScope)scope;
                  if (csc.IsSameOrDerivedFrom(obType)) break;
                  il.Emit(OpCodes.Castclass, csc.GetTypeBuilder());
                  il.Emit(OpCodes.Ldfld, csc.outerClassField);
                }
                scope = scope.GetParent();
              }
              il.Emit(OpCodes.Castclass, obType);
              return;
            }
          }
          il.Emit(OpCodes.Ldarg_0);
          while (scope != null){
            if (scope is ClassScope){ //Might be dealing with a reference from within a nested class to an instance member of an outer class
              ClassScope csc = (ClassScope)scope;
              if (csc.IsSameOrDerivedFrom(obType)) break;
              il.Emit(OpCodes.Ldfld, csc.outerClassField);
            }
            scope = scope.GetParent();
          }
        }
      }

      internal override void TranslateToILPreSet(ILGenerator il){
        this.TranslateToILPreSet(il, false);
      }

      internal void TranslateToILPreSet(ILGenerator il, bool doBoth){
        if (this.isFullyResolved){
          base.TranslateToILPreSet(il);
          return;
        }
        //Need to do a dynamic lookup to see if there is a dynamically introduced (by eval or with) hiding definition.
        //Leave a FieldInfo on the stack if there is such a member, otherwise leave null on the stack
        Label done = il.DefineLabel();
        LocalBuilder fieldLoc = this.fieldLoc = il.DeclareLocal(Typeob.FieldInfo);
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.scriptObjectStackTopMethod);
        il.Emit(OpCodes.Castclass, Typeob.IActivationObject);
        il.Emit(OpCodes.Ldstr, this.name);
        ConstantWrapper.TranslateToILInt(il, this.lexLevel);
        il.Emit(OpCodes.Callvirt, CompilerGlobals.getFieldMethod);
        il.Emit(OpCodes.Stloc, fieldLoc);
        if (!doBoth){
          il.Emit(OpCodes.Ldloc, fieldLoc);
          il.Emit(OpCodes.Ldnull);
          il.Emit(OpCodes.Bne_Un_S, done);
        }
        base.TranslateToILPreSet(il);
        if (this.thereIsAnObjectOnTheStack){
          Label reallyDone = il.DefineLabel();
          il.Emit(OpCodes.Br_S, reallyDone);
          il.MarkLabel(done);
          il.Emit(OpCodes.Ldnull); //Push a dummy object so that both latebound and early bound code paths leave an object on the stack
          il.MarkLabel(reallyDone);
        }else
          il.MarkLabel(done);
      }

      internal override void TranslateToILPreSetPlusGet(ILGenerator il){
        if (this.isFullyResolved){
          base.TranslateToILPreSetPlusGet(il);
          return;
        }
        //Need to do a dynamic lookup to see if there is a dynamically introduced (by eval or with) hiding definition.
        //Leave a FieldInfo on the stack if there is such a member, otherwise leave null on the stack
        Label done = il.DefineLabel();
        Label latebound = il.DefineLabel();
        LocalBuilder fieldTok = this.fieldLoc = il.DeclareLocal(Typeob.FieldInfo);
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.scriptObjectStackTopMethod);
        il.Emit(OpCodes.Castclass, Typeob.IActivationObject);
        il.Emit(OpCodes.Ldstr, this.name);
        ConstantWrapper.TranslateToILInt(il, this.lexLevel);
        il.Emit(OpCodes.Callvirt, CompilerGlobals.getFieldMethod);
        il.Emit(OpCodes.Stloc, fieldTok);
        il.Emit(OpCodes.Ldloc, fieldTok);
        il.Emit(OpCodes.Ldnull);
        il.Emit(OpCodes.Bne_Un_S, latebound);
        base.TranslateToILPreSetPlusGet(il);
        il.Emit(OpCodes.Br_S, done);
        il.MarkLabel(latebound);
        if (this.thereIsAnObjectOnTheStack)
          il.Emit(OpCodes.Ldnull); //Push a dummy object so that both latebound and early bound code paths leave an object on the stack
        il.Emit(OpCodes.Ldloc, this.fieldLoc);
        il.Emit(OpCodes.Ldnull);
        il.Emit(OpCodes.Callvirt, CompilerGlobals.getFieldValueMethod);
        il.MarkLabel(done);
      }

      internal override void TranslateToILSet(ILGenerator il, AST rhvalue){
        this.TranslateToILSet(il, false, rhvalue);
      }

      internal void TranslateToILSet(ILGenerator il, bool doBoth, AST rhvalue){
        if (this.isFullyResolved){
          base.TranslateToILSet(il, rhvalue);
          return;
        }
        if (rhvalue != null)
          rhvalue.TranslateToIL(il, Typeob.Object);
        if (this.fieldLoc == null){
          //There is a callable value plus parameters on the stack
          il.Emit(OpCodes.Call, CompilerGlobals.setIndexedPropertyValueStaticMethod);
          return;
        }
        LocalBuilder temp = il.DeclareLocal(Typeob.Object);
        if (doBoth){
          //save copy of rh value
          il.Emit(OpCodes.Dup);
          il.Emit(OpCodes.Stloc, temp);
          //store it in early bound location
          this.isFullyResolved = true;
          Convert.Emit(this, il, Typeob.Object, Convert.ToType(this.InferType(null)));
          base.TranslateToILSet(il, null);
        }
        //See if there is a late bound field
        Label earlyBound = il.DefineLabel();
        il.Emit(OpCodes.Ldloc, this.fieldLoc);
        il.Emit(OpCodes.Ldnull);
        il.Emit(OpCodes.Beq_S, earlyBound); //No late bound field

        //store it in the late bound field
        Label done = il.DefineLabel();
        if (!doBoth){
          il.Emit(OpCodes.Stloc, temp);
          if (this.thereIsAnObjectOnTheStack)
            il.Emit(OpCodes.Pop);
        }
        il.Emit(OpCodes.Ldloc, this.fieldLoc);
        il.Emit(OpCodes.Ldnull);
        il.Emit(OpCodes.Ldloc, temp);
        il.Emit(OpCodes.Callvirt, CompilerGlobals.setFieldValueMethod);
        il.Emit(OpCodes.Br_S, done);

        //Alternative store it in the early bound location
        il.MarkLabel(earlyBound);
        if (!doBoth){
          this.isFullyResolved = true;
          Convert.Emit(this, il, Typeob.Object, Convert.ToType(this.InferType(null)));
          base.TranslateToILSet(il, null);
        }
        il.MarkLabel(done);
      }

      protected override void TranslateToILWithDupOfThisOb(ILGenerator il){
        this.TranslateToILDefaultThisObject(il);
        this.TranslateToIL(il, Typeob.Object);
      }

      internal void TranslateToLateBinding(ILGenerator il){
        this.thereIsAnObjectOnTheStack = true;
        il.Emit(OpCodes.Ldloc, this.refLoc);
      }
    }
}
