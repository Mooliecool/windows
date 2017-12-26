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
    using System.Reflection;
    using System.Reflection.Emit;
    using System.Runtime.InteropServices.Expando;
    
    internal sealed class Member : Binding{
      private bool fast;
      private bool isImplicitWrapper;
      private LateBinding lateBinding;
      private Context memberNameContext;
      internal AST rootObject;
      private IReflect rootObjectInferredType; //Keeps track of the type inference that was used to do a binding. A binding is invalid if this changes.
      private LocalBuilder refLoc;
      private LocalBuilder temp;
     
      internal Member(Context context, AST rootObject, AST memberName)
        : base(context, memberName.context.GetCode()){
        this.fast = this.Engine.doFast;
        this.isImplicitWrapper = false;
        this.isNonVirtual = rootObject is ThisLiteral && ((ThisLiteral)rootObject).isSuper;
        this.lateBinding = null;
        this.memberNameContext = memberName.context;
        this.rootObject = rootObject;
        this.rootObjectInferredType = null;
        this.refLoc = null;
        this.temp = null;
      }
      
      private void BindName(JSField inferenceTarget){
        MemberInfo[] members = null;
        this.rootObject = this.rootObject.PartiallyEvaluate();
        IReflect obType = this.rootObjectInferredType = this.rootObject.InferType(inferenceTarget);
        if (this.rootObject is ConstantWrapper){
          Object ob = Convert.ToObject2(this.rootObject.Evaluate(), this.Engine);
          if (ob == null){
            this.rootObject.context.HandleError(JSError.ObjectExpected);
            return;
          }
          ClassScope csc = ob as ClassScope;
          Type t = ob as Type;
          if (csc != null || t != null){ 
            //object is a type. Look for static members on the type only. If none are found, look for instance members on type Type.
            if (csc != null)
              this.members = members = csc.GetMember(this.name, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Static|BindingFlags.DeclaredOnly);
            else
              this.members = members = t.GetMember(this.name, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Static|BindingFlags.DeclaredOnly);
            if (members.Length > 0) return; //found a binding
            //Look for instance members on type Type
            this.members = members = Typeob.Type.GetMember(this.name, BindingFlags.Public|BindingFlags.Instance);
            return;
          }
          Namespace ns = ob as Namespace;
          if (ns != null){
            String fullname = ns.Name+"."+this.name;
            csc = this.Engine.GetClass(fullname);
            if (csc != null){
              FieldAttributes attrs = FieldAttributes.Literal;
              if ((csc.owner.attributes&TypeAttributes.Public) == 0) attrs |= FieldAttributes.Private;
              this.members = new MemberInfo[]{new JSGlobalField(null, this.name, csc, attrs)};
              return;
            }else{
              t = this.Engine.GetType(fullname);
              if (t != null){
                this.members = new MemberInfo[]{t};
                return;
              }
            }
          }else if (ob is MathObject || ob is ScriptFunction && !(ob is FunctionObject)) //It is a built in constructor function
            obType = (IReflect)ob;
        }
        obType = this.ProvideWrapperForPrototypeProperties(obType);

        //Give up and go late bound if not enough is known about the object at compile time.
        if (obType == Typeob.Object && !this.isNonVirtual){ //The latter provides for super in classes that extend System.Object
          this.members = new MemberInfo[0];
          return;
        }

        Type ty = obType as Type;         
        //Interfaces are weird, call a helper
        if (ty != null && ty.IsInterface){
          this.members = JSBinder.GetInterfaceMembers(this.name, ty);
          return;
        }
        ClassScope cs = obType as ClassScope;
        if (cs != null && cs.owner.isInterface){
          this.members = cs.owner.GetInterfaceMember(this.name);
          return;
        }

        //Now run up the inheritance chain until a member is found
        while (obType != null){
          cs = obType as ClassScope;
          if (cs != null){
            //The FlattenHierachy flag here tells ClassScope to add in any overloads found on base classes
            members = this.members = obType.GetMember(this.name, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance|BindingFlags.DeclaredOnly|BindingFlags.FlattenHierarchy);
            if (members.Length > 0) return;
            obType = cs.GetSuperType();
            continue;
          }
          ty = obType as Type;
          if (ty == null){ //Dealing with the global scope via the this literal or with a built in object in fast mode
            this.members = obType.GetMember(this.name, BindingFlags.Public|BindingFlags.Instance);
            return;
          }
          members = this.members = ty.GetMember(this.name, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance|BindingFlags.DeclaredOnly);
          if (members.Length > 0){
            MemberInfo mem = LateBinding.SelectMember(members);
            if (mem == null){
              //Found a method or methods. Need to add any overloads found in base classes.              
              //Do another lookup, this time with the DeclaredOnly flag cleared and asking only for methods
              members = this.members = ty.GetMember(this.name, MemberTypes.Method, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance);
              if (members.Length == 0) //Dealing with an indexed property, ask again
                this.members = ty.GetMember(this.name, MemberTypes.Property, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance);
            }
            return;
          }
          obType = ty.BaseType;
        }
      }

      internal override Object Evaluate(){
        Object value = base.Evaluate();
        if (value is Missing)
          value = null;
        return value;
      }      
    
      internal override LateBinding EvaluateAsLateBinding(){
        LateBinding lb = this.lateBinding;
        if (lb == null){
          if (this.member != null && !this.rootObjectInferredType.Equals(this.rootObject.InferType(null)))
            this.InvalidateBinding();
          this.lateBinding = lb = new LateBinding(this.name, null, VsaEngine.executeForJSEE);
          lb.last_member = this.member;
        }
        Object val = this.rootObject.Evaluate();
        try{
          lb.obj = val = Convert.ToObject(val, this.Engine);
          if (this.defaultMember == null && this.member != null)
            lb.last_object = val;
        }catch(JScriptException e){
          if (e.context == null){
            e.context = this.rootObject.context;
          }
          throw e;
        }
        return lb;
      }

      internal Object EvaluateAsType(){
        WrappedNamespace ns = this.rootObject.EvaluateAsWrappedNamespace(false);
        Object result = ns.GetMemberValue(this.name);
        if (result != null && !(result is Missing)) return result;
        Object ob = null;
        Member root = this.rootObject as Member;
        if (root == null){
          Lookup lookup = this.rootObject as Lookup;
          if (lookup == null) return null;
          ob = lookup.PartiallyEvaluate();
          ConstantWrapper cw = ob as ConstantWrapper;
          if (cw != null)
            ob = cw.value;
          else{
            JSGlobalField f = lookup.member as JSGlobalField;
            if (f != null && f.IsLiteral)
              ob = f.value;
            else
              return null;
          }
        }else
          ob = root.EvaluateAsType();
        ClassScope csc = ob as ClassScope;
        if (csc != null){
          MemberInfo[] members = csc.GetMember(this.name, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Static|BindingFlags.Instance);
          if (members.Length == 0) return null;
          JSMemberField field =  members[0] as JSMemberField;
          if (field == null || !field.IsLiteral || !(field.value is ClassScope) || !field.IsPublic && !field.IsAccessibleFrom(this.Engine.ScriptObjectStackTop())) return null;
          return field.value;
        }
        Type t = ob as Type;
        if (t != null) return t.GetNestedType(this.name);
        return null;
      }
      
      internal override WrappedNamespace EvaluateAsWrappedNamespace(bool giveErrorIfNameInUse){
        WrappedNamespace root = this.rootObject.EvaluateAsWrappedNamespace(giveErrorIfNameInUse);
        String name = this.name;
        root.AddFieldOrUseExistingField(name, Namespace.GetNamespace(root.ToString()+"."+name, this.Engine), FieldAttributes.Literal);
        return new WrappedNamespace(root.ToString()+"."+name, this.Engine);
      }      
      
      protected override Object GetObject(){
        return Convert.ToObject(this.rootObject.Evaluate(), this.Engine);
      }

      protected override void HandleNoSuchMemberError(){
        IReflect obType = this.rootObject.InferType(null);
        Object obVal = null;

        if (this.rootObject is ConstantWrapper)
          obVal = this.rootObject.Evaluate();
        
        if ((obType == Typeob.Object && !this.isNonVirtual) || 
            (obType is JSObject && !((JSObject)obType).noExpando) ||
            (obType is GlobalScope && !((GlobalScope)obType).isKnownAtCompileTime))
          return;
        if (obType is Type){
          Type t = (Type)obType;
          if (Typeob.ScriptFunction.IsAssignableFrom(t) || t == Typeob.MathObject){
            //dealing with an assigment to a member of a builtin constructor function. 
            Debug.Assert(this.fast);
            this.memberNameContext.HandleError(JSError.OLENoPropOrMethod);
            return;
          }
          if (Typeob.IExpando.IsAssignableFrom(t)) return;
          if (!this.fast)
            if (t == Typeob.Boolean || t == Typeob.String || Convert.IsPrimitiveNumericType(t)) return;

          // Check to see if we couldn't get the member because it is non-static.
          if (obVal is ClassScope){
            MemberInfo[] members = ((ClassScope)obVal).GetMember(this.name, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance);
            if (members.Length > 0){
               this.memberNameContext.HandleError(JSError.NonStaticWithTypeName);
               return;
            }
          }
        }

        if (obVal is FunctionObject){
          this.rootObject = new ConstantWrapper(((FunctionObject)obVal).name, this.rootObject.context);
          this.memberNameContext.HandleError(JSError.OLENoPropOrMethod);
          return;
        }
        
        // Check to see if we couldn't get the member because it is static.
        if (obType is ClassScope){
          MemberInfo[] members = ((ClassScope)obType).GetMember(this.name, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Static);
          if (members.Length > 0){
            this.memberNameContext.HandleError(JSError.StaticRequiresTypeName);
            return;
          }
        }
        
        if (obVal is Type)
          this.memberNameContext.HandleError(JSError.NoSuchStaticMember, Convert.ToTypeName((Type)obVal));
        else if (obVal is ClassScope)
          this.memberNameContext.HandleError(JSError.NoSuchStaticMember, Convert.ToTypeName((ClassScope)obVal));
        else if (obVal is Namespace)
          this.memberNameContext.HandleError(JSError.NoSuchType, ((Namespace)obVal).Name+"."+this.name);
        else{
          if (obType == FunctionPrototype.ob && this.rootObject is Binding && 
            ((Binding)this.rootObject).member is JSVariableField && ((JSVariableField)((Binding)this.rootObject).member).value is FunctionObject)
            return;            
          this.memberNameContext.HandleError(JSError.NoSuchMember, Convert.ToTypeName(obType));
        }
      }
      
      internal override IReflect InferType(JSField inference_target){
        if (this.members == null){
          this.BindName(inference_target);
        }else{
          if (!this.rootObjectInferredType.Equals(this.rootObject.InferType(inference_target)))
            this.InvalidateBinding();
        }
        return base.InferType(null);
      }
      
      internal override IReflect InferTypeOfCall(JSField inference_target, bool isConstructor){
        if (!this.rootObjectInferredType.Equals(this.rootObject.InferType(inference_target)))
          this.InvalidateBinding();
        return base.InferTypeOfCall(null, isConstructor);
      }
      
      internal override AST PartiallyEvaluate(){
        this.BindName(null);
        if (this.members == null || this.members.Length == 0){
          //if this rootObject is a constant and its value is a namespace, turn this node also into a namespace constant
          if (this.rootObject is ConstantWrapper){
            Object val = this.rootObject.Evaluate();
            if (val is Namespace){
              return new ConstantWrapper(Namespace.GetNamespace(((Namespace)val).Name+"."+this.name, this.Engine), this.context);
            }
          }
          this.HandleNoSuchMemberError();
          return this;
        }
        this.ResolveRHValue();
        if (this.member is FieldInfo && ((FieldInfo)this.member).IsLiteral){
          Object val = this.member is JSVariableField ? ((JSVariableField)this.member).value : TypeReferences.GetConstantValue((FieldInfo)this.member);
          if (val is AST){
            AST pval = ((AST)val).PartiallyEvaluate();
            if (pval is ConstantWrapper)
              return pval;
            val = null;
          }
          if (!(val is FunctionObject) && (!(val is ClassScope) || ((ClassScope)val).owner.IsStatic))
            return new ConstantWrapper(val, this.context);
        }else if (this.member is Type)
          return new ConstantWrapper(this.member, this.context);
        return this;
      }
      
      internal override AST PartiallyEvaluateAsCallable(){
        this.BindName(null);
        //Resolving the call is delayed until the arguments have been partially evaluated
        return this;
      }  
    
      internal override AST PartiallyEvaluateAsReference(){
        this.BindName(null);
        if (this.members == null || this.members.Length == 0){
          if (this.isImplicitWrapper && !Convert.IsArray(this.rootObjectInferredType))
            this.context.HandleError(JSError.UselessAssignment);
          else
            this.HandleNoSuchMemberError();
          return this;
        }
        this.ResolveLHValue();
        if (this.isImplicitWrapper)
          if (this.member == null || (!(this.member is JSField) && Typeob.JSObject.IsAssignableFrom(this.member.DeclaringType)))
            this.context.HandleError(JSError.UselessAssignment);
        return this;
      }
      
      private IReflect ProvideWrapperForPrototypeProperties(IReflect obType){
        //Provide for early binding to prototype methods in fast mode, by fudging the type
        if (obType == Typeob.String){
          obType = Globals.globalObject.originalString.Construct();
          ((JSObject)obType).noExpando = this.fast;
          this.isImplicitWrapper = true;
        }else if ((obType is Type && Typeob.Array.IsAssignableFrom((Type)obType)) || obType is TypedArray){
          obType = Globals.globalObject.originalArray.ConstructWrapper();
          ((JSObject)obType).noExpando = this.fast;
          this.isImplicitWrapper = true;
        }else if (obType == Typeob.Boolean){
          obType = Globals.globalObject.originalBoolean.Construct();
          ((JSObject)obType).noExpando = this.fast;
          this.isImplicitWrapper = true;
        }else if (Convert.IsPrimitiveNumericType(obType)){
          Type baseType = (Type)obType;
          obType = Globals.globalObject.originalNumber.Construct();
          ((JSObject)obType).noExpando = this.fast;
          ((NumberObject)obType).baseType = baseType;
          this.isImplicitWrapper = true;
        }else if (obType is Type)
          obType = Convert.ToIReflect((Type)obType, this.Engine);
        return obType;
      }
      
      internal override Object ResolveCustomAttribute(ASTList args, IReflect[] argIRs, AST target){
        this.name = this.name + "Attribute";
        this.BindName(null);
        if (this.members == null || this.members.Length == 0){
          this.name = this.name.Substring(0, this.name.Length-9);
          this.BindName(null);
        }
        return base.ResolveCustomAttribute(args, argIRs, target);
      }

      //code in parser relies on the member string (x.y.z...) being returned from here 
      public override String ToString(){
        return this.rootObject.ToString() + "." + this.name;
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        this.rootObject.TranslateToILInitializer(il);
        if (!this.rootObjectInferredType.Equals(this.rootObject.InferType(null)))
          this.InvalidateBinding();
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
        il.Emit(OpCodes.Newobj, CompilerGlobals.lateBindingConstructor);
        il.Emit(OpCodes.Stloc, this.refLoc);
      }
      
      protected override void TranslateToILObject(ILGenerator il, Type obType, bool noValue){
        if (noValue && obType.IsValueType && obType != Typeob.Enum){
          if (this.temp == null)
            this.rootObject.TranslateToILReference(il, obType);
          else{
            Type tempType = Convert.ToType(this.rootObject.InferType(null));
            if (tempType == obType)
              il.Emit(OpCodes.Ldloca, this.temp);
            else{
              il.Emit(OpCodes.Ldloc, this.temp);
              Convert.Emit(this, il, tempType, obType);
              Convert.EmitLdloca(il, obType);
            }
          }
        }else{
          if (this.temp == null || this.rootObject is ThisLiteral)
            this.rootObject.TranslateToIL(il, obType);
          else{
            il.Emit(OpCodes.Ldloc, this.temp);
            Type tempType = Convert.ToType(this.rootObject.InferType(null));
            Convert.Emit(this, il, tempType, obType);
          }
        }
      }
      
      protected override void TranslateToILWithDupOfThisOb(ILGenerator il){
        IReflect ir = this.rootObject.InferType(null);
        Type tempType = Convert.ToType(ir);
        this.rootObject.TranslateToIL(il, tempType);
        if (ir == Typeob.Object || ir == Typeob.String || ir is TypedArray || (ir == tempType && Typeob.Array.IsAssignableFrom(tempType))){
          tempType = Typeob.Object;
          this.EmitILToLoadEngine(il);
          il.Emit(OpCodes.Call, CompilerGlobals.toObjectMethod);
        }
        il.Emit(OpCodes.Dup);
        this.temp = il.DeclareLocal(tempType);
        il.Emit(OpCodes.Stloc, temp);
        Convert.Emit(this, il, tempType, Typeob.Object);
        this.TranslateToIL(il, Typeob.Object); 
      }
      
      internal void TranslateToLateBinding(ILGenerator il, bool speculativeEarlyBindingsExist){
        if (speculativeEarlyBindingsExist){
          LocalBuilder temp = il.DeclareLocal(Typeob.Object);
          il.Emit(OpCodes.Stloc, temp);
          il.Emit(OpCodes.Ldloc, this.refLoc);
          il.Emit(OpCodes.Dup);
          il.Emit(OpCodes.Ldloc, temp);
        }else{
          il.Emit(OpCodes.Ldloc, this.refLoc);
          il.Emit(OpCodes.Dup);
          this.TranslateToILObject(il, Typeob.Object, false);
        }
        IReflect ir = this.rootObject.InferType(null);
        if (ir == Typeob.Object || ir == Typeob.String || ir is TypedArray || (ir is Type && ((Type)ir).IsPrimitive) || (ir is Type && Typeob.Array.IsAssignableFrom((Type)ir))){
          this.EmitILToLoadEngine(il);
          il.Emit(OpCodes.Call, CompilerGlobals.toObjectMethod);
        }
        il.Emit(OpCodes.Stfld, CompilerGlobals.objectField);
      }      
    }
}
