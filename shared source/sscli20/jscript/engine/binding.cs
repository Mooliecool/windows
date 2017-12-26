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
    using System.Collections;
    using System.Globalization;
    using System.Reflection;
    using System.Reflection.Emit;
    
    public abstract class Binding : AST{
      private IReflect[] argIRs;
      protected MemberInfo defaultMember;
      private IReflect defaultMemberReturnIR;
      private bool isArrayElementAccess;
      private bool isArrayConstructor;
      protected bool isAssignmentToDefaultIndexedProperty;
      protected bool isFullyResolved;
      protected bool isNonVirtual;
      internal MemberInfo[] members;
      internal MemberInfo member;
      protected String name;
      private bool giveErrors;

      static internal ConstantWrapper ReflectionMissingCW = new ConstantWrapper(System.Reflection.Missing.Value, null); 
      static private ConstantWrapper JScriptMissingCW = new ConstantWrapper(Missing.Value, null); 

      internal Binding(Context context, String name)
        : base(context) {
        this.argIRs = null;
        this.defaultMember = null;
        this.defaultMemberReturnIR = null;
        this.isArrayElementAccess = false;
        this.isArrayConstructor = false;
        this.isAssignmentToDefaultIndexedProperty = false;
        this.isFullyResolved = true;
        this.isNonVirtual = false;
        this.members = null;
        this.member = null;
        this.name = name;
        this.giveErrors = true;
      }
      
      private bool Accessible(bool checkSetter){
        if (this.member == null) return false;
        switch(this.member.MemberType){
          case MemberTypes.Constructor: return AccessibleConstructor();
          case MemberTypes.Event: return false;
          case MemberTypes.Field: return AccessibleField(checkSetter);
          case MemberTypes.Method: return AccessibleMethod();
          case MemberTypes.NestedType: 
            if (!((Type)this.member).IsNestedPublic){
              if (this.giveErrors)
                this.context.HandleError(JSError.NotAccessible, this.isFullyResolved);
              return false;
            }
            if(checkSetter)
              return false;
            return true;
          case MemberTypes.TypeInfo: 
            if (!((Type)this.member).IsPublic){
              if (this.giveErrors)
                this.context.HandleError(JSError.NotAccessible, this.isFullyResolved);
              return false;
            }
            if(checkSetter)
              return false;
            return true;
          case MemberTypes.Property: return AccessibleProperty(checkSetter);
        }
        return false;
      }
      
      private bool AccessibleConstructor(){
        ConstructorInfo cons = (ConstructorInfo)this.member;
        if ((cons is JSConstructor && ((JSConstructor)member).GetClassScope().owner.isAbstract) || 
             (!(cons is JSConstructor) && cons.DeclaringType.IsAbstract)){
          this.context.HandleError(JSError.CannotInstantiateAbstractClass);
          return false;
        }
        if (cons.IsPublic) return true;
        if (cons is JSConstructor){
          if (((JSConstructor)cons).IsAccessibleFrom(Globals.ScopeStack.Peek())) return true;
        }
        if (this.giveErrors)
          this.context.HandleError(JSError.NotAccessible, this.isFullyResolved);
        return false;
      }
      
      private bool AccessibleField(bool checkWritable){
        FieldInfo field = (FieldInfo)this.member;
        if (checkWritable && (field.IsInitOnly || field.IsLiteral)) return false;
        if (!field.IsPublic){
          JSWrappedField wfield = field as JSWrappedField;
          if (wfield != null)
            this.member = field = wfield.wrappedField;
          JSClosureField cfield = field as JSClosureField;
          JSMemberField mfield = (cfield != null ? cfield.field : field) as JSMemberField;
          if (mfield == null){
            if ((!field.IsFamily && !field.IsFamilyOrAssembly) || !Binding.InsideClassThatExtends(Globals.ScopeStack.Peek(), field.ReflectedType)){
              if (this.giveErrors)
                this.context.HandleError(JSError.NotAccessible, this.isFullyResolved);
              return false;
            }
          }else if (!mfield.IsAccessibleFrom(Globals.ScopeStack.Peek())){
            if (this.giveErrors)
              this.context.HandleError(JSError.NotAccessible, this.isFullyResolved);
            return false;
          }
        }
        if (field.IsLiteral && field is JSVariableField){
          ClassScope csc = ((JSVariableField)field).value as ClassScope;
          if (csc != null && !csc.owner.IsStatic){
            Lookup lookup = this as Lookup;
            if (lookup == null || !lookup.InStaticCode() || lookup.InFunctionNestedInsideInstanceMethod()) return true;
            if (this.giveErrors)
              this.context.HandleError(JSError.InstanceNotAccessibleFromStatic, this.isFullyResolved);
            return true;
          }
        }
        if (field.IsStatic || field.IsLiteral || this.defaultMember != null || !(this is Lookup) || !((Lookup)this).InStaticCode()) return true;
        if (field is JSWrappedField && field.DeclaringType == Typeob.LenientGlobalObject) return true;
        if (this.giveErrors) {
          if (!field.IsStatic && this is Lookup && ((Lookup)this).InStaticCode())
            this.context.HandleError(JSError.InstanceNotAccessibleFromStatic, this.isFullyResolved);
          else
            this.context.HandleError(JSError.NotAccessible, this.isFullyResolved);
        }
        return false;
      }
      
      private bool AccessibleMethod(){
        MethodInfo meth = (MethodInfo)this.member;
        return this.AccessibleMethod(meth);
      }
      
      private bool AccessibleMethod(MethodInfo meth){
        if (meth == null) return false;
        if (this.isNonVirtual && meth.IsAbstract){
          this.context.HandleError(JSError.InvalidCall);
          return false;
        }
        if (!meth.IsPublic){
          JSWrappedMethod wmeth = meth as JSWrappedMethod;
          if (wmeth != null)
            meth = wmeth.method;
          JSClosureMethod cmeth = meth as JSClosureMethod;
          JSFieldMethod fmeth = (cmeth != null ? cmeth.method : meth) as JSFieldMethod;
          if (fmeth == null){
            if ((meth.IsFamily || meth.IsFamilyOrAssembly) && Binding.InsideClassThatExtends(Globals.ScopeStack.Peek(), meth.ReflectedType))
              return true;
            if (this.giveErrors)
              this.context.HandleError(JSError.NotAccessible, this.isFullyResolved);
            return false;
          }else if (!fmeth.IsAccessibleFrom(Globals.ScopeStack.Peek())){
            if (this.giveErrors)
              this.context.HandleError(JSError.NotAccessible, this.isFullyResolved);
            return false;
          }
        }
        if (meth.IsStatic || this.defaultMember != null || !(this is Lookup) || !((Lookup)this).InStaticCode()) return true;
        if (meth is JSWrappedMethod && ((Lookup)this).CanPlaceAppropriateObjectOnStack(((JSWrappedMethod)meth).obj)) return true;
        if (this.giveErrors) {
          if (!meth.IsStatic && this is Lookup && ((Lookup)this).InStaticCode())
            this.context.HandleError(JSError.InstanceNotAccessibleFromStatic, this.isFullyResolved);
          else
            this.context.HandleError(JSError.NotAccessible, this.isFullyResolved);
        }
        return false;
      }
      
      private bool AccessibleProperty(bool checkSetter){
        PropertyInfo prop = (PropertyInfo)this.member;
        if (this.AccessibleMethod(checkSetter ? JSProperty.GetSetMethod(prop, true) : JSProperty.GetGetMethod(prop, true))) return true;
        if (this.giveErrors && !checkSetter) this.context.HandleError(JSError.WriteOnlyProperty);
        return false;
      }
      
      internal static bool AssignmentCompatible(IReflect lhir, AST rhexpr, IReflect rhir, bool reportError){
        if (rhexpr is ConstantWrapper){
          Object rhval = rhexpr.Evaluate();
          if (rhval is ClassScope){
            if (lhir == Typeob.Type || lhir == Typeob.Object || lhir == Typeob.String) return true;
            else{
              if (reportError) rhexpr.context.HandleError(JSError.TypeMismatch);
              return false;
            }
          }
          ClassScope csc = lhir as ClassScope;
          if (csc != null){
            EnumDeclaration ed = csc.owner as EnumDeclaration;
            if (ed != null){
              ConstantWrapper cw = rhexpr as ConstantWrapper;
              if (cw != null && cw.value is String){
                FieldInfo field = csc.GetField((String)cw.value, BindingFlags.Public|BindingFlags.Static);
                if (field == null) return false;
                ed.PartiallyEvaluate();
                cw.value = new DeclaredEnumValue(((JSMemberField)field).value, field.Name, csc);
              }
              if (rhir == Typeob.String) return true;
              lhir = ed.baseType.ToType();
            }
          } else if (lhir is Type) {
            Type type = lhir as Type;
            if (type.IsEnum) {
              ConstantWrapper cw = rhexpr as ConstantWrapper;
              if (cw != null && cw.value is String){
                FieldInfo field = type.GetField((String)cw.value, BindingFlags.Public|BindingFlags.Static);
                if (field == null) return false;
                cw.value = MetadataEnumValue.GetEnumValue(field.FieldType, field.GetRawConstantValue());
              }
              if (rhir == Typeob.String) return true;
              lhir = Enum.GetUnderlyingType(type);
            }
          }
          
          if (lhir is Type){
            try{
              Convert.CoerceT(rhval, (Type)lhir);
              return true;
            }catch{
              if (lhir == Typeob.Single && rhval is Double){
                if (((ConstantWrapper)rhexpr).isNumericLiteral) return true;
                Double d = (Double)rhval;
                Single s = (Single)d;
                if (d.ToString(CultureInfo.InvariantCulture).Equals(s.ToString(CultureInfo.InvariantCulture))){
                  ((ConstantWrapper)rhexpr).value = s;
                  return true;
                }
              }
              if (lhir == Typeob.Decimal){
                ConstantWrapper cw = rhexpr as ConstantWrapper;
                if (cw != null && cw.isNumericLiteral){
                  try{
                    Convert.CoerceT(cw.context.GetCode(), Typeob.Decimal);
                    return true;
                  }catch{}
                }
              }
              if (reportError)
                rhexpr.context.HandleError(JSError.TypeMismatch);
            }
            return false;
          }
        }else if (rhexpr is ArrayLiteral)
          return ((ArrayLiteral)rhexpr).AssignmentCompatible(lhir, reportError);
        if (rhir == Typeob.Object) return true; //Too little is known about the expression to complain at compile time
        if (rhir == Typeob .Double && Convert.IsPrimitiveNumericType(lhir)) return true; //Arithmetic expressions infer to Double, but might have the right result.
        if (lhir is Type && Typeob.Delegate.IsAssignableFrom((Type)lhir) && rhir == Typeob.ScriptFunction && 
            rhexpr is Binding && ((Binding)rhexpr).IsCompatibleWithDelegate((Type)lhir))
          return true;
        if (Convert.IsPromotableTo(rhir, lhir))
          return true; //rhexpr delivers a value that can be converted to something expected by the assignment target
        if (Convert.IsJScriptArray(rhir) && Binding.ArrayAssignmentCompatible(rhexpr, lhir))
          return true;
        if (lhir == Typeob.String) 
          return true;  // Everything is assignment-compatible to string.
        if (rhir == Typeob.String && (lhir == Typeob.Boolean || Convert.IsPrimitiveNumericType(lhir))){
          if (reportError) rhexpr.context.HandleError(JSError.PossibleBadConversionFromString);
          return true;
        }
        if ((lhir == Typeob.Char && rhir == Typeob.String) || Convert.IsPromotableTo(lhir, rhir) ||
            (Convert.IsPrimitiveNumericType(lhir) && Convert.IsPrimitiveNumericType(rhir))){
          if (reportError) rhexpr.context.HandleError(JSError.PossibleBadConversion);
          return true;
        }
        
        if (reportError)
          rhexpr.context.HandleError(JSError.TypeMismatch);
        return false;
      }

      private static bool ArrayAssignmentCompatible(AST ast, IReflect lhir){
        // If we've made it here then we already know that lhir is not Object or String
        // and that a JScript array is not promotable to lhir.  There are cases where
        // a JScript array is not promotable but is assignable with a warning.  A
        // JScript array is assignable to a System.Array or to a rank-one typed array.
        if (!Convert.IsArray(lhir))
          return false;
        else if (lhir == Typeob.Array){
          ast.context.HandleError(JSError.ArrayMayBeCopied);
          return true;
        }else if (Convert.GetArrayRank(lhir) == 1){
          ast.context.HandleError(JSError.ArrayMayBeCopied);
          return true;
        }else
          return false;
      }
      
      internal void CheckIfDeletable(){
        if (this.member != null || this.defaultMember != null)
          this.context.HandleError(JSError.NotDeletable);
        this.member = null;
        this.defaultMember = null;
      }
      
      internal void CheckIfUseless(){
        // This is called for expression statements. There are no cases where an 
        // expression which just retrieves a property should not give a warning.
        // We don't bother giving a warning if members is empty because more than
        // likely an warning or error was already issued.
        if (this.members == null || this.members.Length == 0)
          return;
        this.context.HandleError(JSError.UselessExpression);
      }
      
      internal static bool CheckParameters(ParameterInfo[] pars, IReflect[] argIRs, ASTList argAST, Context ctx){
        return Binding.CheckParameters(pars, argIRs, argAST, ctx, 0, false, true);
      }
      
      internal static bool CheckParameters(ParameterInfo[] pars, IReflect[] argIRs, ASTList argAST, Context ctx, int offset, bool defaultIsUndefined, bool reportError){
        int n = argIRs.Length;
        int m = pars.Length;
        bool tooManyParams = false;
        if (n > m-offset) {n = m-offset; tooManyParams = true;}
        for (int i = 0; i < n; i++){
          IReflect formalIR = (pars[i+offset] is ParameterDeclaration) ? ((ParameterDeclaration)pars[i+offset]).ParameterIReflect : pars[i+offset].ParameterType;
          IReflect actualIR = argIRs[i];
          if (i == n-1 && ((formalIR is Type && Typeob.Array.IsAssignableFrom((Type)formalIR)) || formalIR is TypedArray) && 
          CustomAttribute.IsDefined(pars[i+offset], typeof(ParamArrayAttribute), false)){
            tooManyParams = false;
            int k = argIRs.Length;
            if (i == k - 1){
              if (Binding.AssignmentCompatible(formalIR, argAST[i], argIRs[i], false))
                return true;
            }
            IReflect elemIR = formalIR is Type ? ((Type)formalIR).GetElementType() : ((TypedArray)formalIR).elementType;
            for (int j = i; j < k; j++)
              if (!Binding.AssignmentCompatible(elemIR, argAST[j], argIRs[j], reportError)) return false;
            return true;
          }
          if (!Binding.AssignmentCompatible(formalIR, argAST[i], actualIR, reportError)) return false;
        }
        if (tooManyParams && reportError)
          ctx.HandleError(JSError.TooManyParameters);
        if (offset == 0 && n < m && !defaultIsUndefined) //Fewer actual parameters than formal parameters
          for (int j = n; j < m; j++)
            if (TypeReferences.GetDefaultParameterValue(pars[j]) == System.Convert.DBNull){ //No default value specified
              //Ensure that custom attribute has been resolved before checking
              ParameterDeclaration pardecl = pars[j] as ParameterDeclaration;
              if (pardecl != null)
                pardecl.PartiallyEvaluate();
              if (j < m-1 || !CustomAttribute.IsDefined(pars[j], typeof(ParamArrayAttribute), false)){
                if (reportError)
                  ctx.HandleError(JSError.TooFewParameters);
                IReflect formalIR = (pars[j+offset] is ParameterDeclaration) ? ((ParameterDeclaration)pars[j+offset]).ParameterIReflect : pars[j+offset].ParameterType;
                Type formalType = formalIR as Type;
                if (formalType != null && formalType.IsValueType && !formalType.IsPrimitive && !formalType.IsEnum)
                  return false; //Can't generate valid code for this since there is no general mapping from undefined to value types
              }
            }
        return true;
      }
      
      internal override bool Delete(){
        return this.EvaluateAsLateBinding().Delete();
      }
      
      internal override Object Evaluate(){
        Object ob = this.GetObject();
        MemberInfo member = this.member;
        if (member != null)
          switch(member.MemberType){
            case MemberTypes.Field : return ((FieldInfo)member).GetValue(ob);
            case MemberTypes.Property : 
              MemberInfo[] members = new MemberInfo[]{JSProperty.GetGetMethod((PropertyInfo)member, false)};
              return LateBinding.CallOneOfTheMembers(members, new Object[0], false, ob, null, null, null, this.Engine); 
            case MemberTypes.Event : return null;
            case MemberTypes.NestedType : return member;
          }
        if (this.members != null && this.members.Length > 0){
          //Special case check for methods on builtin objects
          if (this.members.Length == 1 && this.members[0].MemberType == MemberTypes.Method){
            MethodInfo meth = (MethodInfo)members[0];
            Type dt = meth is JSMethod ? null : meth.DeclaringType;
            
            if (dt == Typeob.GlobalObject || 
               (dt != null && dt != Typeob.StringObject && dt != Typeob.NumberObject && dt != Typeob.BooleanObject && dt.IsSubclassOf(Typeob.JSObject))){
              //This only happens in fast mode. We could add InitOnly fields to the fast predefined objects and initialize them
              //with instances of BuiltinFunction objects, in which case we would never get here, but we would like to avoid
              //the start up cost of allocating these objects, particularly if they end up never being used (the expected common case).
              return Globals.BuiltinFunctionFor(ob, TypeReferences.ToExecutionContext(meth));
              //Note that meth is not wrapped because it is static
            }
          }
          
          return new FunctionWrapper(this.name, ob, this.members);
        }
        return this.EvaluateAsLateBinding().GetValue();
      }

      //Returns a list of member infos that are sorted by declaring type. Superclass members always follow derived class members.
      private MemberInfoList GetAllKnownInstanceBindingsForThisName(){
        IReflect[] classes = this.GetAllEligibleClasses();
        MemberInfoList result = new MemberInfoList();
        foreach (IReflect c in classes){
          if (c is ClassScope){
            if (((ClassScope)c).ParentIsInSamePackage())
              result.AddRange(c.GetMember(name, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance|BindingFlags.DeclaredOnly));
            else
              result.AddRange(c.GetMember(name, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance));
          }else
            result.AddRange(c.GetMember(name, BindingFlags.Public|BindingFlags.Instance));
        }
        return result;
      }

      //Returns a list of the classes visible from the current scope.
      //Currently, classes are returned only if they are ancestors of the current class, or reside in the same package.
      private IReflect[] GetAllEligibleClasses(){      
        ArrayList classes = new ArrayList(16);
        ClassScope currentClass = null;
        PackageScope currentPackage = null;
        ScriptObject scope = Globals.ScopeStack.Peek();
        while (scope is WithObject || scope is BlockScope)
          scope = scope.GetParent();
        if (scope is FunctionScope)
          scope = ((FunctionScope)scope).owner.enclosing_scope;
        if (scope is ClassScope){
          currentClass = (ClassScope)scope;
          currentPackage = currentClass.package;
        }
        if (currentClass != null)
          currentClass.AddClassesFromInheritanceChain(this.name, classes);
        if (currentPackage != null)
          currentPackage.AddClassesExcluding(currentClass, this.name, classes);
        else
          ((IActivationObject)scope).GetGlobalScope().AddClassesExcluding(currentClass, this.name, classes);
        IReflect[] result = new IReflect[classes.Count]; classes.CopyTo(result);
        return result;
      }

      protected abstract Object GetObject();      
      
      protected abstract void HandleNoSuchMemberError();
      
      internal override IReflect InferType(JSField inference_target){
        Debug.Assert(this.members != null); //Have to call a PartiallyEvaluate routine before calling InferType
        if (this.isArrayElementAccess){
          IReflect ir = this.defaultMemberReturnIR;
          return ir is TypedArray ? ((TypedArray)ir).elementType : ((Type)ir).GetElementType();
        }
        if (this.isAssignmentToDefaultIndexedProperty){
          if (this.member is PropertyInfo) //Could be null if no binding was found
            return ((PropertyInfo)this.member).PropertyType;
          return Typeob.Object;
        }
        MemberInfo member = this.member;
        if (member is FieldInfo){
          JSWrappedField wf = member as JSWrappedField;
          if (wf != null) member = wf.wrappedField;
          if (member is JSVariableField)
            return ((JSVariableField)member).GetInferredType(inference_target);
          else
            return ((FieldInfo)member).FieldType;
        }
        if (member is PropertyInfo) {
          JSWrappedProperty wp = member as JSWrappedProperty;
          if (wp != null) member = wp.property;
          if (member is JSProperty)
            return ((JSProperty)member).PropertyIR();
          else{
            PropertyInfo prop = (PropertyInfo)member;
            if (prop.DeclaringType == Typeob.GlobalObject)
              return (IReflect)prop.GetValue(this.Globals.globalObject, null);
            else
              return prop.PropertyType;
          }
        }
        
        if (member is Type)
          return Typeob.Type;
        if (member is EventInfo)
          return Typeob.EventInfo;
        if (this.members.Length > 0 && this.Engine.doFast)
          return Typeob.ScriptFunction;
        return Typeob.Object;
      }
      
      internal virtual IReflect InferTypeOfCall(JSField inference_target, bool isConstructor){
        Debug.Assert(this.members != null); //Have to call a PartiallyEvaluate routine before calling InferTypeOfCall
        if (!this.isFullyResolved) return Typeob.Object;
        if (this.isArrayConstructor)
          return this.defaultMemberReturnIR;
        if (this.isArrayElementAccess){
          IReflect ir = this.defaultMemberReturnIR;
          return ir is TypedArray ? ((TypedArray)ir).elementType : ((Type)ir).GetElementType();
        }
        MemberInfo member = this.member;
        if (member is JSFieldMethod) return isConstructor ? Typeob.Object : ((JSFieldMethod)member).ReturnIR();
        if (member is MethodInfo) return ((MethodInfo)member).ReturnType;
        if (member is JSConstructor) return ((JSConstructor)member).GetClassScope();
        if (member is ConstructorInfo) return ((ConstructorInfo)member).DeclaringType;
        if (member is Type) return (Type)member;
        if (member is FieldInfo && ((FieldInfo)member).IsLiteral){
          Object val = member is JSVariableField ? ((JSVariableField)member).value : TypeReferences.GetConstantValue((FieldInfo)member);
          if (val is ClassScope || val is TypedArray) return (IReflect)val;
        }
        return Typeob.Object;
      }
      
      private static bool InsideClassThatExtends(ScriptObject scope, Type type){
        while (scope is WithObject || scope is BlockScope)
          scope = scope.GetParent();
        if (scope is ClassScope)
          return type.IsAssignableFrom(((ClassScope)scope).GetBakedSuperType());
        if (scope is FunctionScope)
          return Binding.InsideClassThatExtends(((FunctionScope)scope).owner.enclosing_scope, type);
        //Eval does not see non public members, so don't worry about StackFrame
        return false;
      }
      
      internal void InvalidateBinding(){
        this.isAssignmentToDefaultIndexedProperty = false;
        this.isArrayConstructor = false;
        this.isArrayElementAccess = false;
        this.defaultMember = null;
        this.member = null;
        this.members = new MemberInfo[0];
      }
      
      internal bool IsCompatibleWithDelegate(Type delegateType){
        Debug.Assert(this.members != null && this.members.Length > 0);
        MethodInfo invoke = delegateType.GetMethod("Invoke");
        ParameterInfo[] dparams = invoke.GetParameters();
        Type drtype = invoke.ReturnType;
        foreach (MemberInfo mem in this.members){
          if (mem is MethodInfo){
            MethodInfo meth = (MethodInfo)mem;
            Type returnType = null;
            if (meth is JSFieldMethod){
              IReflect returnIR = ((JSFieldMethod)meth).ReturnIR();
              if (returnIR is ClassScope)
                returnType = ((ClassScope)returnIR).GetBakedSuperType(); //JScript cannot define delegates, so baked is OK.
              else if (returnIR is Type)
                returnType = (Type)returnIR;
              else
                returnType = Convert.ToType(returnIR);
              if (((JSFieldMethod)meth).func.isExpandoMethod) return false;
            }else
              returnType = meth.ReturnType;
            if (returnType == drtype && Class.ParametersMatch(dparams, meth.GetParameters())){
              this.member = meth;
              this.isFullyResolved = true;
              return true;
            }
          }
        }
        return false;
      }
      
      public static bool IsMissing(Object value){
        return value is Missing;
      }
      
      private MethodInfo LookForParameterlessPropertyGetter(){
        // We only get here as a result of an AmbiguousMatchException.  If all the matches are parameterless
        // property getters, we try selecting a match without providing arguments.
        for (int i=0, n=this.members.Length; i<n; i++){
          PropertyInfo prop = this.members[i] as PropertyInfo;
          if (prop != null){
            MethodInfo meth = prop.GetGetMethod(true);
            if (meth == null) continue;
            ParameterInfo[] parameters = meth.GetParameters();
            if (parameters == null || parameters.Length == 0) continue;
          }
          return null;
        }
        try{
          MethodInfo meth = JSBinder.SelectMethod(this.members, new IReflect[0]); //Returns property getters as well
          if (meth != null && meth.IsSpecialName){ //Property getter.
            return meth;
          }
        }catch(AmbiguousMatchException){
        }
        return null;
      }
      
      internal override bool OkToUseAsType(){
        MemberInfo member = this.member;
        if (member is Type) return this.isFullyResolved = true;
        if (member is FieldInfo){
          FieldInfo field = (FieldInfo)member;
          if (field.IsLiteral){
            if (field is JSMemberField && ((JSMemberField)field).value is ClassScope && !field.IsStatic)
              return false;
            return this.isFullyResolved = true;
          }
          if (!(member is JSField) && field.IsStatic && field.GetValue(null) is Type) return this.isFullyResolved = true;
        }
        return false;
      }
      
      private int PlaceValuesForHiddenParametersOnStack(ILGenerator il, MethodInfo meth, ParameterInfo[] pars){
        int offset = 0;
        if (meth is JSFieldMethod){
          FunctionObject func = ((JSFieldMethod)meth).func;
          if (func != null && func.isMethod) return 0;
          if (this is Lookup)
            ((Lookup)this).TranslateToILDefaultThisObject(il);
          else
            this.TranslateToILObject(il, Typeob.Object, false);
          this.EmitILToLoadEngine(il);
          return 0;
        }
        Object[] attrs = CustomAttribute.GetCustomAttributes(meth, typeof(JSFunctionAttribute), false);
        if (attrs == null || attrs.Length == 0) return 0;
        JSFunctionAttributeEnum attr = ((JSFunctionAttribute)attrs[0]).attributeValue;
        if ((attr & JSFunctionAttributeEnum.HasThisObject) != 0){
          offset = 1;
          Type pt = pars[0].ParameterType;
          if (this is Lookup && pt == Typeob.Object)
            ((Lookup)this).TranslateToILDefaultThisObject(il);
          else {
            if (Typeob.ArrayObject.IsAssignableFrom(member.DeclaringType)) 
              this.TranslateToILObject(il, Typeob.ArrayObject, false);
            else
              this.TranslateToILObject(il, pt, false);
          }
        }
        if ((attr & JSFunctionAttributeEnum.HasEngine) != 0){
          offset += 1;
          this.EmitILToLoadEngine(il);
        }
        return offset;
      }

      private bool ParameterlessPropertyValueIsCallable(MethodInfo meth, ASTList args, IReflect[] argIRs, bool constructor, bool brackets){
        ParameterInfo[] pars = meth.GetParameters();
        if (pars == null || pars.Length == 0){
          if ((meth is JSWrappedMethod && ((JSWrappedMethod)meth).GetWrappedObject() is GlobalObject) ||
            argIRs.Length > 0 || 
            (!(meth is JSMethod) && Typeob.ScriptFunction.IsAssignableFrom(meth.ReturnType))){
            this.member = this.ResolveOtherKindOfCall(args, argIRs, constructor, brackets);
            return true;
          }
          IReflect mrt = meth is JSFieldMethod ? ((JSFieldMethod)meth).ReturnIR() : meth.ReturnType;
          if (mrt != Typeob.Object && mrt != Typeob.ScriptFunction)
            this.context.HandleError(JSError.InvalidCall);
          else{
            this.member = this.ResolveOtherKindOfCall(args, argIRs, constructor, brackets);
            return true;
          }
        }
        return false;
      }

      internal static void PlaceArgumentsOnStack(ILGenerator il, ParameterInfo[] pars, ASTList args, int offset, int rhoffset, AST missing){
        int k = args.count;
        int n = k+offset;
        int m = pars.Length-rhoffset;
        bool varargs = m > 0 && CustomAttribute.IsDefined(pars[m-1], typeof(ParamArrayAttribute), false) && 
         !(k == m && Convert.IsArrayType(args[k-1].InferType(null)));
        Type varargElemType = varargs ? pars[--m].ParameterType.GetElementType() : null;
        if (n > m) n = m;
        for (int i = offset; i < n; i++){
          Type ptype = pars[i].ParameterType;
          AST arg = args[i-offset];
          if (arg is ConstantWrapper && ((ConstantWrapper)arg).value == System.Reflection.Missing.Value){
            Object defVal = TypeReferences.GetDefaultParameterValue(pars[i]);
            ((ConstantWrapper)arg).value =  defVal != System.Convert.DBNull ? defVal : null;
          }
          if (ptype.IsByRef)
            arg.TranslateToILReference(il, ptype.GetElementType());
          else
            arg.TranslateToIL(il, ptype);
        }
        if (n < m){
          for (int i = n; i < m; i++){
            Type ptype = pars[i].ParameterType;
            if (TypeReferences.GetDefaultParameterValue(pars[i]) == System.Convert.DBNull) //No default value was specified
              if (ptype.IsByRef)
                missing.TranslateToILReference(il, ptype.GetElementType());
              else
                missing.TranslateToIL(il, ptype); 
            else
              if (ptype.IsByRef)
                (new ConstantWrapper(TypeReferences.GetDefaultParameterValue(pars[i]), null)).TranslateToILReference(il, ptype.GetElementType());
              else
                (new ConstantWrapper(TypeReferences.GetDefaultParameterValue(pars[i]), null)).TranslateToIL(il, ptype);
          }
        }
        if (varargs){
          n -= offset; //The number of arguments in argList that are already on the stack
          m = k>n ? k-n : 0; //The number of arguments in argList that are to be placed in the vararg array
          ConstantWrapper.TranslateToILInt(il, m);
          il.Emit(OpCodes.Newarr, varargElemType);
          bool doLdelema = varargElemType.IsValueType && !varargElemType.IsPrimitive;
          for (int i = 0; i < m; i++){
            il.Emit(OpCodes.Dup);
            ConstantWrapper.TranslateToILInt(il, i);
            if (doLdelema)
              il.Emit(OpCodes.Ldelema, varargElemType);
            args[i+n].TranslateToIL(il, varargElemType);
            Binding.TranslateToStelem(il, varargElemType);
          }
        }
      }
      
      internal bool RefersToMemoryLocation(){
        if (this.isFullyResolved){
          if (this.isArrayElementAccess) return true;
          return this.member is FieldInfo;
        }
        return false;
      }
      
      internal override void ResolveCall(ASTList args, IReflect[] argIRs, bool constructor, bool brackets){
        this.argIRs = argIRs;
        if (this.members == null || this.members.Length == 0){
          if (constructor && this.isFullyResolved && this.Engine.doFast)
            if (this.member != null && (this.member is Type || (this.member is FieldInfo && ((FieldInfo)this.member).IsLiteral)))
              this.context.HandleError(JSError.NoConstructor);
            else
              this.HandleNoSuchMemberError();
          else
            this.HandleNoSuchMemberError();
          return; //Have to do a runtime lookup
        }
        MemberInfo member = null;
        if (!(this is CallableExpression) && !(constructor && brackets))
          try{
            if (constructor)
              this.member = member = JSBinder.SelectConstructor(this.members, argIRs);
            else{
              MethodInfo meth;
              this.member = member = meth = JSBinder.SelectMethod(this.members, argIRs); //Returns property getters as well
              if (meth != null && meth.IsSpecialName){ //Property getter.
                if (this.name == meth.Name){
                  if (this.name.StartsWith("get_", StringComparison.Ordinal) || this.name.StartsWith("set_", StringComparison.Ordinal)){
                    this.context.HandleError(JSError.NotMeantToBeCalledDirectly);
                    this.member = null;
                    return;
                  }
                }else if (this.ParameterlessPropertyValueIsCallable(meth, args, argIRs, constructor, brackets))
                  return; // this.member was set by ParameterlessPropertyValueIsCallable
              }
            }
          }catch(AmbiguousMatchException){
            if (constructor)
              this.context.HandleError(JSError.AmbiguousConstructorCall, this.isFullyResolved);
            else{
              MethodInfo meth = this.LookForParameterlessPropertyGetter();
              if (meth != null && this.ParameterlessPropertyValueIsCallable(meth, args, argIRs, constructor, brackets))
                return; // this.member was set by ParameterlessPropertyValueIsCallable
              this.context.HandleError(JSError.AmbiguousMatch, this.isFullyResolved);
              this.member = null;
            }
            return;
          }catch(JScriptException e){
            this.context.HandleError((Microsoft.JScript.JSError)(e.ErrorNumber & 0xffff), e.Message, true);
            return;
          }
        if (member == null)
          member = this.ResolveOtherKindOfCall(args, argIRs, constructor, brackets);
        if (member == null) return; //Already complained about it, if applicable. Do runtime lookup.
        if (!this.Accessible(false)){
          this.member = null;
          return;
        }
        WarnIfObsolete();
        if (member is MethodBase){
          if (CustomAttribute.IsDefined(member, typeof(JSFunctionAttribute), false) && !(this.defaultMember is PropertyInfo)){
            int hidden = 0;
            Object[] attrs = CustomAttribute.GetCustomAttributes(member, typeof(JSFunctionAttribute), false);
            JSFunctionAttributeEnum attr = ((JSFunctionAttribute)attrs[0]).attributeValue;
            if ((constructor && !(member is ConstructorInfo)) || (attr & JSFunctionAttributeEnum.HasArguments) != 0){
              //Can only happen when doing an Eval from slow mode code
              //Cannot call such methods directly, so bind to the associated field instead
              this.member = LateBinding.SelectMember(this.members);
              this.defaultMember = null;
              return;
            }
            if ((attr & JSFunctionAttributeEnum.HasThisObject) != 0)
              hidden = 1;
            if ((attr & JSFunctionAttributeEnum.HasEngine) != 0)
              hidden += 1;
            if (!Binding.CheckParameters(((MethodBase)member).GetParameters(), argIRs, args, this.context, hidden, true, this.isFullyResolved)){
              this.member = null;
              return;
            }
          }else if (constructor && member is JSFieldMethod){
            //Do a late bound call so that the function as constructor semantics work out
            this.member = LateBinding.SelectMember(this.members);
            return;
          }else if (constructor && member is ConstructorInfo && !(member is JSConstructor) && Typeob.Delegate.IsAssignableFrom(member.DeclaringType)){
            this.context.HandleError(JSError.DelegatesShouldNotBeExplicitlyConstructed);
            this.member = null;
            return;
          }else
            if (!Binding.CheckParameters(((MethodBase)member).GetParameters(), argIRs, args, this.context, 0, false, this.isFullyResolved)){
              this.member = null;
              return;
            }
        }
        return;
      }

      internal override Object ResolveCustomAttribute(ASTList args, IReflect[] argIRs, AST target){
        try{
          this.ResolveCall(args, argIRs, true, false);
        }catch(AmbiguousMatchException){
          this.context.HandleError(JSError.AmbiguousConstructorCall);
          return null;
        }
        JSConstructor jscons = this.member as JSConstructor;
        if (jscons != null){
          ClassScope csc = jscons.GetClassScope();
          if (csc.owner.IsCustomAttribute()) return csc;
        }else{
          ConstructorInfo cons = this.member as ConstructorInfo;
          if (cons != null){
            Type attrType = cons.DeclaringType;
            if (Typeob.Attribute.IsAssignableFrom(attrType)){
              Object[] usageAttrs = CustomAttribute.GetCustomAttributes(attrType, typeof(AttributeUsageAttribute), false);
              if (usageAttrs.Length > 0) return attrType;
            }
          }
        }
        this.context.HandleError(JSError.InvalidCustomAttributeClassOrCtor);
        return null;
      }

      internal void ResolveLHValue(){
        MemberInfo member = this.member = LateBinding.SelectMember(this.members);
        if ((member != null && !this.Accessible(true)) || (this.member == null && this.members.Length > 0)){
          this.context.HandleError(JSError.AssignmentToReadOnly, this.isFullyResolved);
          this.member = null; //Recover by going late bound. The runtime routine will fail silently
          this.members = new MemberInfo[0];
          return;
        }
        if (member is JSPrototypeField){ //Go late bound
          this.member = null;
          this.members = new MemberInfo[0];
          return;
        }
        this.WarnIfNotFullyResolved();
        this.WarnIfObsolete();
        return;
      }  
      
      private MemberInfo ResolveOtherKindOfCall(ASTList argList, IReflect[] argIRs, bool constructor, bool brackets){
        //There is no method or constructor corresponding to this binding. But there is something else.
        MemberInfo member = this.member = LateBinding.SelectMember(this.members); //Choose something
        
        //If the chosen member is a read-only property on the global object of type Type, replace it with the type.
        if (member is PropertyInfo && !(member is JSProperty) && member.DeclaringType == Typeob.GlobalObject){
          PropertyInfo prop = (PropertyInfo)member;
          Type ptype = prop.PropertyType;
          if (ptype == Typeob.Type)
            member = (Type)prop.GetValue(null, null);
          else if (constructor && brackets){ //Map properties returning fast mode constructor functions to types
            MethodInfo meth = ptype.GetMethod("CreateInstance", BindingFlags.Public|BindingFlags.Instance|BindingFlags.DeclaredOnly);
            if (meth != null){
              Type rtype = meth.ReturnType;
              if (rtype == Typeob.BooleanObject)
                member = Typeob.Boolean;
              else if (rtype == Typeob.StringObject)
                member = Typeob.String;
              else 
                member = rtype;
            }
          }
        }
        
        //Check for casts to type expressions, i.e. int[](x)
        CallableExpression ce = this as CallableExpression;
        if (ce != null){
          ConstantWrapper cw = ce.expression as ConstantWrapper;
          if (cw != null && cw.InferType(null) is Type)
            member = new JSGlobalField(null, null, cw.value, FieldAttributes.Literal|FieldAttributes.Public);
        }

        //If the chosen member is a literal field containing a class/function or a nested type, we replace members and try again.
        if (member is Type){
          if (constructor){
            if (brackets){
              this.isArrayConstructor = true;
              this.defaultMember = member;
              this.defaultMemberReturnIR = new TypedArray((Type)member, argIRs.Length);
              for (int i = 0, n = argIRs.Length; i < n; i++){
                if (argIRs[i] != Typeob.Object && !Convert.IsPrimitiveNumericType(argIRs[i])){
                  argList[i].context.HandleError(JSError.TypeMismatch, this.isFullyResolved);
                  break;
                }
              }
              return this.member = member;
            }else{
              ConstructorInfo[] constructors = ((Type)member).GetConstructors(BindingFlags.Instance|BindingFlags.Public);
              if (constructors == null || constructors.Length == 0){
                this.context.HandleError(JSError.NoConstructor);
                this.member = null;
                return null;
              }
              this.members = constructors;
              this.ResolveCall(argList, argIRs, true, brackets);
              return this.member;
            }
          }else{
            if (!brackets && argIRs.Length == 1){
              //Dealing with a type cast
              return member;
            }
            this.context.HandleError(JSError.InvalidCall);
            return this.member = null;
          }
        }
        if (member is JSPrototypeField)
          //Have to go late-bound
          return this.member = null;
        if (member is FieldInfo && ((FieldInfo)member).IsLiteral){
          if (!this.AccessibleField(false)) return this.member = null;
          Object val = member is JSVariableField ? ((JSVariableField)member).value : TypeReferences.GetConstantValue((FieldInfo)member);
          if ((val is ClassScope || val is Type)){
            if (constructor){
              if (brackets){
                this.isArrayConstructor = true;
                this.defaultMember = member;
                this.defaultMemberReturnIR = new TypedArray((val is ClassScope ? (IReflect)val : (IReflect)val), argIRs.Length);
                for (int i = 0, n = argIRs.Length; i < n; i++){
                  if (argIRs[i] != Typeob.Object && !Convert.IsPrimitiveNumericType(argIRs[i])){
                    argList[i].context.HandleError(JSError.TypeMismatch, this.isFullyResolved);
                    break;
                  }
                }
                return this.member = member;
              }else{
                if (val is ClassScope && !((ClassScope)val).owner.isStatic){
                  ConstantWrapper cw = null;
                  if (this is Member && (cw = ((Member)this).rootObject as ConstantWrapper) != null && !(cw.Evaluate() is Namespace)){
                    ((Member)this).rootObject.context.HandleError(JSError.NeedInstance);
                    return null;
                  }  
                }
                this.members = val is ClassScope ? ((ClassScope)val).constructors : ((Type)val).GetConstructors(BindingFlags.Instance|BindingFlags.Public);
                if (this.members == null || this.members.Length == 0){
                  this.context.HandleError(JSError.NoConstructor);
                  this.member = null;
                  return null;
                }
                this.ResolveCall(argList, argIRs, true, brackets);
                return this.member;
              }
            }else{
              if (!brackets && argIRs.Length == 1){
                Type ty = val as Type;
                return this.member = (ty != null ? ty : member);
              }
              this.context.HandleError(JSError.InvalidCall);
              return this.member = null;
            }
          }
          if (val is TypedArray){
            if (!constructor){ //Casting a value to an array type
              if (argIRs.Length != 1 || brackets) goto ReportError;
              return this.member = member;
            }
            if (!brackets) goto ReportError; //new T(...) where T is an array type.
            if (argIRs.Length == 0) goto ReportError; //new T[] where T is an array type.
            this.isArrayConstructor = true;
            this.defaultMember = member;
            this.defaultMemberReturnIR = new TypedArray((IReflect)val, argIRs.Length);
            for (int i = 0, n = argIRs.Length; i < n; i++){
              if (argIRs[i] != Typeob.Object && !Convert.IsPrimitiveNumericType(argIRs[i])){
                argList[i].context.HandleError(JSError.TypeMismatch, this.isFullyResolved);
                break;
              }
            }
            return this.member = member;
          }
          if (val is FunctionObject){
            FunctionObject func = (FunctionObject)val;
            if (!func.isExpandoMethod && !func.Must_save_stack_locals && (func.own_scope.ProvidesOuterScopeLocals == null || func.own_scope.ProvidesOuterScopeLocals.count == 0))
              return this.member = ((JSVariableField)this.member).GetAsMethod(func.own_scope);
            return this.member; //Have to call via the field so that the stack frame is set up correctly
          }
        }
        
        //Check the type of this binding for more information
        IReflect ir = this.InferType(null);
        Type t = ir as Type;
        if (!brackets && ((t != null && Typeob.ScriptFunction.IsAssignableFrom(t)) || ir is ScriptFunction)){
          this.defaultMember = member;
          if (t == null){
            this.defaultMemberReturnIR = Globals.TypeRefs.ToReferenceContext(ir.GetType()); //The appropriate subtype of ScriptFunction
            this.member = this.defaultMemberReturnIR.GetMethod(constructor ? "CreateInstance" : "Invoke", 
              BindingFlags.DeclaredOnly|BindingFlags.Public|BindingFlags.Instance);
            if (this.member == null){
              this.defaultMemberReturnIR = Typeob.ScriptFunction;
              this.member = this.defaultMemberReturnIR.GetMethod(constructor ? "CreateInstance" : "Invoke", 
                BindingFlags.DeclaredOnly|BindingFlags.Public|BindingFlags.Instance);
            }
            return this.member;
          }else{
            // We have a script function but it might not be an "expando" function.
            if (constructor && this.members.Length != 0 && this.members[0] is JSFieldMethod){
              JSFieldMethod fieldmethod = (JSFieldMethod)this.members[0];
              fieldmethod.func.PartiallyEvaluate();
              if (!fieldmethod.func.isExpandoMethod)
                this.context.HandleError(JSError.NotAnExpandoFunction, this.isFullyResolved);
            }
            this.defaultMemberReturnIR = t;
            return this.member = t.GetMethod(constructor ? "CreateInstance" : "Invoke",
              BindingFlags.DeclaredOnly|BindingFlags.Public|BindingFlags.Instance);
          }
        }
        if (ir == Typeob.Type){ //Type cast to type only known at runtime
          //Have to go late-bound
          this.member = null;
          return null;
        }
        if (ir == Typeob.Object || (ir is ScriptObject && brackets && !(ir is ClassScope)))
          //The result of evaluating this binding could be a callable/constructable thing or a thing with a default indexed property.
          //Or it could not, but we don't know enough at compile time to moan about it.
          return member;
          
        //Perhaps this is an array element access
        if (ir is TypedArray || (ir is Type && ((Type)ir).IsArray)){
          int n = argIRs.Length;
          int m = ir is TypedArray ? ((TypedArray)ir).rank : ((Type)ir).GetArrayRank();
          if (n != m)
            this.context.HandleError(JSError.IncorrectNumberOfIndices, this.isFullyResolved);
          else{
            for (int i = 0; i < m; i++){
              if (argIRs[i] != Typeob.Object && (!Convert.IsPrimitiveNumericType(argIRs[i]) || Convert.IsBadIndex(argList[i]))){
                argList[i].context.HandleError(JSError.TypeMismatch, this.isFullyResolved);
                break;
              }
            }
          }
          if (constructor){
            if (!brackets) goto ReportError;
            //dealing with new Arr[...]. Give an error if the array element type is not Object, Type, or ScriptFunction
            IReflect elemIR = ir is TypedArray ? ((TypedArray)ir).elementType : ((Type)ir).GetElementType();
            if (ir != Typeob.Object && !(ir is ClassScope) &&
                !(ir is Type && !Typeob.Type.IsAssignableFrom((Type)ir) && !Typeob.ScriptFunction.IsAssignableFrom((Type)ir)))
              goto ReportError;
          }
          this.isArrayElementAccess = true;
          this.defaultMember = member;
          this.defaultMemberReturnIR = ir;
          return null; //Delay looking up the method to call until code gen time.
        }
                 
        //Perhaps The result of evaluating this binding is an object that has a default indexed property/method that can be called
        if (constructor) goto ReportError; //new binding(....) does not make sense for a default indexed property or method
 
        //Check for a default indexed property or default method
        if (brackets && ir == Typeob.String && (this.argIRs.Length != 1 || !Convert.IsPrimitiveNumericType(argIRs[0])))
          ir = Typeob.StringObject;
        MemberInfo[] defaultMembers = brackets || !(ir is ScriptObject) ? JSBinder.GetDefaultMembers(ir) : null;
        if (defaultMembers != null && defaultMembers.Length > 0){
          try{
            this.defaultMember = member;
            this.defaultMemberReturnIR = ir;
            return this.member = JSBinder.SelectMethod(this.members = defaultMembers, argIRs); //Returns property getters as well
          }catch(AmbiguousMatchException){
            this.context.HandleError(JSError.AmbiguousMatch, this.isFullyResolved);
            return this.member = null;
          }
        }
        
        //Perhaps this is calling a delegate
        if (!brackets && ir is Type && Typeob.Delegate.IsAssignableFrom((Type)ir)){
          this.defaultMember = member;
          this.defaultMemberReturnIR = ir;
          return this.member = ((Type)ir).GetMethod("Invoke");
        }
        
      ReportError:   
        //We now know enough to say that this binding really is not callable/constructable. Say so.
        if (constructor)
          this.context.HandleError(JSError.NeedType, this.isFullyResolved);
        else if (brackets)
          this.context.HandleError(JSError.NotIndexable, this.isFullyResolved);
        else
          this.context.HandleError(JSError.FunctionExpected, this.isFullyResolved);
        return this.member = null;
      }
        
      protected void ResolveRHValue(){
        MemberInfo member = this.member = LateBinding.SelectMember(this.members);
        JSLocalField lfield = this.member as JSLocalField;
        if (lfield != null){
          FunctionObject funcOb = lfield.value as FunctionObject;
          if (funcOb != null){
            FunctionScope enclosingScope = funcOb.enclosing_scope as FunctionScope;
            if (enclosingScope != null) enclosingScope.closuresMightEscape = true;
          }
        }
        if (member is JSPrototypeField){ //Have to go late bound
          this.member = null;
          return;
        }
        if (!this.Accessible(false)){
          this.member = null;
          return;
        }
        WarnIfObsolete();
        this.WarnIfNotFullyResolved();
      }
      
      internal override void SetPartialValue(AST partial_value){
        Binding.AssignmentCompatible(this.InferType(null), partial_value, partial_value.InferType(null), this.isFullyResolved);
      }
      
      internal void SetPartialValue(ASTList argList, IReflect[] argIRs, AST partial_value, bool inBrackets){
        if (this.members == null || this.members.Length == 0){
          this.HandleNoSuchMemberError();
          this.isAssignmentToDefaultIndexedProperty = true;
          return; //Have to do a runtime lookup
        }

        this.PartiallyEvaluate(); //The rhside value of the binding delivers the object with the default indexed property we are assigning to
        IReflect ir = this.InferType(null);
        this.isAssignmentToDefaultIndexedProperty = true;
        if (ir == Typeob.Object){
          JSVariableField jsvf = this.member as JSVariableField;
          if (jsvf == null || !jsvf.IsLiteral || !(jsvf.value is ClassScope))
            return; //Not enough is known at compile time to give an error
          ir = Typeob.Type;
          goto giveError;
        }
        
        //Might be an assignment to an array element
        if ((ir is TypedArray || (ir is Type && ((Type)ir).IsArray))){
          bool gaveAnError = false;
          //Check dimension
          int n = argIRs.Length;
          int m = ir is TypedArray ? ((TypedArray)ir).rank : ((Type)ir).GetArrayRank();
          if (n != m){
            this.context.HandleError(JSError.IncorrectNumberOfIndices, this.isFullyResolved);
            gaveAnError = true;
          }
          //Check type of indices
          for (int i = 0; i < m; i++){
            if (!gaveAnError && i < n && argIRs[i] != Typeob.Object &&
                (!Convert.IsPrimitiveNumericType(argIRs[i]) || Convert.IsBadIndex(argList[i]))){
              argList[i].context.HandleError(JSError.TypeMismatch, this.isFullyResolved);       
              gaveAnError = true;
            }
          }
          this.isArrayElementAccess = true;
          this.isAssignmentToDefaultIndexedProperty = false;
          this.defaultMember = member;
          this.defaultMemberReturnIR = ir;
          
          //Check type of rhside
          IReflect elemIR = ir is TypedArray ? ((TypedArray)ir).elementType : ((Type)ir).GetElementType();
          Binding.AssignmentCompatible(elemIR, partial_value, partial_value.InferType(null), this.isFullyResolved);
          return;
        }
        
        //Might be an assignment to a default indexed property
        MemberInfo[] defaultMembers = JSBinder.GetDefaultMembers(ir);
        if (defaultMembers != null && defaultMembers.Length > 0 && this.member != null){
          try{
            PropertyInfo prop = JSBinder.SelectProperty(defaultMembers, argIRs);
            if (prop == null){ 
              this.context.HandleError(JSError.NotIndexable, Convert.ToTypeName(ir));
              return;
            }
            if (JSProperty.GetSetMethod(prop, true) == null){
              if (ir == Typeob.String)
                this.context.HandleError(JSError.UselessAssignment);
              else
                this.context.HandleError(JSError.AssignmentToReadOnly, this.isFullyResolved&&this.Engine.doFast);
              return;
            }
            if (!Binding.CheckParameters(prop.GetIndexParameters(), argIRs, argList, this.context, 0, false, true)){
              return;
            }
            this.defaultMember = this.member;
            this.defaultMemberReturnIR = ir;
            this.members = defaultMembers;
            this.member = prop;
          }catch(AmbiguousMatchException){
            this.context.HandleError(JSError.AmbiguousMatch, this.isFullyResolved);
            this.member = null;
          }
          return;
        }
        
      giveError:
        this.member = null;
        if (!inBrackets)
          this.context.HandleError(JSError.IllegalAssignment);
        else
          this.context.HandleError(JSError.NotIndexable, Convert.ToTypeName(ir));
      }
      
      internal override void SetValue(Object value){
        MemberInfo member = this.member;
        Object ob = this.GetObject();
        if (member is FieldInfo){
          FieldInfo field = (FieldInfo)member;
          if (field.IsLiteral || field.IsInitOnly) return;
          if (!(field is JSField) || field is JSWrappedField)
            value = Convert.CoerceT(value, field.FieldType, false);
          field.SetValue(ob, value, BindingFlags.SuppressChangeType, null, null);
          return;
        }
        if (member is PropertyInfo){
          PropertyInfo prop = (PropertyInfo)member;
          if (ob is ClassScope && !(prop is JSProperty)){
            JSProperty.SetValue(prop, ((WithObject)((ClassScope)ob).GetParent()).contained_object, value, null);
            return;
          }
          if (!(prop is JSProperty))
            value = Convert.CoerceT(value, prop.PropertyType, false);
          JSProperty.SetValue(prop, ob, value, null);
          return;
        }
        if (this.members == null || this.members.Length == 0){
          this.EvaluateAsLateBinding().SetValue(value);
          return;
        }
        //The name has been bound to something and that something is not a valid assignment target.
        throw new JScriptException(JSError.IllegalAssignment);
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        this.TranslateToIL(il, rtype, false, false);
      }
      
      internal void TranslateToIL(ILGenerator il, Type rtype, bool calledFromDelete){
        this.TranslateToIL(il, rtype, false, false, calledFromDelete);
      }
      
      private void TranslateToIL(ILGenerator il, Type rtype, bool preSet, bool preSetPlusGet){
        this.TranslateToIL(il, rtype, preSet, preSetPlusGet, false);
      }
      
      private void TranslateToIL(ILGenerator il, Type rtype, bool preSet, bool preSetPlusGet, bool calledFromDelete){
        if (this.member is FieldInfo){
          FieldInfo field = (FieldInfo)this.member;
          bool isStatic = field.IsStatic || field.IsLiteral;
          if (field.IsLiteral && field is JSMemberField){
            Object val = ((JSMemberField)field).value;
            FunctionObject func = val as FunctionObject;
            isStatic = func == null || !func.isExpandoMethod;
          }
          if (!isStatic || field is JSClosureField){
            this.TranslateToILObject(il, field.DeclaringType, true);
            if (preSetPlusGet)
              il.Emit(OpCodes.Dup);
            isStatic = false;
          }
          if (!preSet){
            Object tok = field is JSField ? ((JSField)field).GetMetaData() : field is JSFieldInfo ? ((JSFieldInfo)field).field : field;
            if (tok is FieldInfo && !((FieldInfo)tok).IsLiteral)
              il.Emit(isStatic ? OpCodes.Ldsfld : OpCodes.Ldfld, (FieldInfo)tok);
            else if (tok is LocalBuilder)
              il.Emit(OpCodes.Ldloc, (LocalBuilder)tok);
            else if (field.IsLiteral){
              (new ConstantWrapper(TypeReferences.GetConstantValue(field), this.context)).TranslateToIL(il, rtype);
              return;
            }else
              Convert.EmitLdarg(il, (short)tok);
            Convert.Emit(this, il, field.FieldType, rtype);
          }
          return;
        }
        if (this.member is PropertyInfo){
          PropertyInfo prop = (PropertyInfo)this.member;
          MethodInfo meth = preSet ? JSProperty.GetSetMethod(prop, true) : JSProperty.GetGetMethod(prop, true);
          if (meth == null){
            if (preSet) return;
            if (this is Lookup){
              il.Emit(OpCodes.Ldc_I4, (int)JSError.WriteOnlyProperty);
              il.Emit(OpCodes.Newobj, CompilerGlobals.scriptExceptionConstructor);
              il.Emit(OpCodes.Throw);
            }else
              il.Emit(OpCodes.Ldnull);
            return;
          }
          bool isStatic = meth.IsStatic && !(meth is JSClosureMethod);
          if (!isStatic){
            Type obType = meth.DeclaringType;
            if (obType == Typeob.StringObject && meth.Name.Equals("get_length")){
              this.TranslateToILObject(il, Typeob.String, false);
              meth = CompilerGlobals.stringLengthMethod;
            }else
              this.TranslateToILObject(il, obType, true);
          }
          if (!preSet){
            Debug.Assert(meth.GetParameters().Length == 0);
            meth = this.GetMethodInfoMetadata(meth);
            if (isStatic){
              il.Emit(OpCodes.Call, meth);
            }else{
              if (preSetPlusGet)
                il.Emit(OpCodes.Dup);
              if (!this.isNonVirtual && meth.IsVirtual && !meth.IsFinal && (!meth.ReflectedType.IsSealed || !meth.ReflectedType.IsValueType))
                il.Emit(OpCodes.Callvirt, meth);
              else
                il.Emit(OpCodes.Call, meth);
            }
            Convert.Emit(this, il, meth.ReturnType, rtype);
          }
          return;
        }
        if (this.member is MethodInfo){
          MethodInfo meth = this.GetMethodInfoMetadata((MethodInfo)this.member);
          if (Typeob.Delegate.IsAssignableFrom(rtype)){
            if (!meth.IsStatic){
              Type obType = meth.DeclaringType;
              this.TranslateToILObject(il, obType, false);
              if (obType.IsValueType)
                il.Emit(OpCodes.Box, obType);
            }else
              il.Emit(OpCodes.Ldnull);
            if (meth.IsVirtual && !meth.IsFinal && (!meth.ReflectedType.IsSealed || !meth.ReflectedType.IsValueType)){
              il.Emit(OpCodes.Dup);
              il.Emit(OpCodes.Ldvirtftn, meth);
            }else
              il.Emit(OpCodes.Ldftn, meth);
            ConstructorInfo cons = rtype.GetConstructor(new Type[]{Typeob.Object, Typeob.UIntPtr});
            if (cons == null) cons = rtype.GetConstructor(new Type[]{Typeob.Object, Typeob.IntPtr});
            il.Emit(OpCodes.Newobj, cons);
            return;
          }
          //ResolveRHValue will never set this.member to a MethodInfo. The above case only happens
          //because IsCompatibleWithDelegate sets this.member to the method that matches the delegate type.
          if (member is JSExpandoIndexerMethod){
            //Need to put the expando class instance on the stack as the this object in expressions such as exInstance["foo"]90
            MemberInfo mem = this.member;
            this.member = this.defaultMember;
            this.TranslateToIL(il, Typeob.Object);
            this.member = mem;
            return;
          }
          il.Emit(OpCodes.Ldnull); //get something on the stack, in case we DO get here
          Convert.Emit(this, il, Typeob.Object, rtype);
          return;
        }
        Object done = null;
        if (this is Lookup)
          ((Lookup)this).TranslateToLateBinding(il);
        else{
          if (!this.isFullyResolved && !preSet && !preSetPlusGet)
            done = this.TranslateToSpeculativeEarlyBindings(il, rtype, false);
          ((Member)this).TranslateToLateBinding(il, done != null);
          if (!this.isFullyResolved && preSetPlusGet)
            done = this.TranslateToSpeculativeEarlyBindings(il, rtype, true);
        }
        if (preSetPlusGet)
          il.Emit(OpCodes.Dup);
        if (!preSet){
          if (this is Lookup && !calledFromDelete)
            il.Emit(OpCodes.Call, CompilerGlobals.getValue2Method);
          else
            il.Emit(OpCodes.Call, CompilerGlobals.getNonMissingValueMethod);
          Convert.Emit(this, il, Typeob.Object, rtype);
          if (done != null)
            il.MarkLabel((Label)done);
        }
      }

      internal override void TranslateToILCall(ILGenerator il, Type rtype, ASTList argList, bool construct, bool brackets){
        MemberInfo member = this.member;
        if (this.defaultMember != null){
          if (this.isArrayConstructor){
            TypedArray tArr = (TypedArray)this.defaultMemberReturnIR;
            Type eType = Convert.ToType(tArr.elementType);
            Debug.Assert(tArr.rank == argList.count);
            int r = tArr.rank;
            if (r == 1){
              argList[0].TranslateToIL(il, Typeob.Int32);
              il.Emit(OpCodes.Newarr, eType);
            }else{
              Type aType = tArr.ToType();
              Type[] dTypes = new Type[r];
              for (int i = 0; i < r; i++) dTypes[i] = Typeob.Int32;
              for (int i = 0, n = argList.count; i < n; i++)
                argList[i].TranslateToIL(il, Typeob.Int32);
              TypeBuilder eTypeB = eType as TypeBuilder;
              if (eTypeB != null){
                MethodInfo cons = ((ModuleBuilder)aType.Module).GetArrayMethod(aType, ".ctor", CallingConventions.HasThis, Typeob.Void, dTypes);
                il.Emit(OpCodes.Newobj, cons);
              }else{
                ConstructorInfo cons = aType.GetConstructor(dTypes);
                il.Emit(OpCodes.Newobj, cons);
              }
            }
            Convert.Emit(this, il, tArr.ToType(), rtype);
            return;
          }
          this.member = this.defaultMember;
          IReflect defIR = this.defaultMemberReturnIR;
          Type defType = defIR is Type ? (Type)defIR : Convert.ToType(defIR);
          this.TranslateToIL(il, defType);
          if (this.isArrayElementAccess){
            Debug.Assert(defType.IsArray);
            for (int i = 0, m = argList.count; i < m; i++)
              argList[i].TranslateToIL(il, Typeob.Int32);
            Type etype = defType.GetElementType();
            int n = defType.GetArrayRank();
            if (n == 1){
              Binding.TranslateToLdelem(il, etype);
            }else{
              Type[] indexTypes = new Type[n];
              for (int i = 0; i < n; i++) indexTypes[i] = Typeob.Int32;
              MethodInfo getter = compilerGlobals.module.GetArrayMethod(defType, "Get", CallingConventions.HasThis, etype, indexTypes);
              il.Emit(OpCodes.Call, getter);
            }
            Convert.Emit(this, il, etype, rtype);
            return;
          }
          this.member = member;
        }
        if (member is MethodInfo){
          MethodInfo meth = (MethodInfo)member;
          Type dt = meth.DeclaringType;
          Type rt = meth.ReflectedType;
          ParameterInfo[] pars = meth.GetParameters();
          bool isStatic = meth.IsStatic;
          if (!isStatic && this.defaultMember == null)
            this.TranslateToILObject(il, dt, true);
          if (meth is JSClosureMethod)
            this.TranslateToILObject(il, dt, false);
          ConstantWrapper missing = null;
          int offset = 0;
          if (meth is JSFieldMethod || CustomAttribute.IsDefined(meth, typeof(JSFunctionAttribute), false)){
            offset = this.PlaceValuesForHiddenParametersOnStack(il, meth, pars);
            missing = Binding.JScriptMissingCW;
          }else
            missing = Binding.ReflectionMissingCW;
          if (argList.count == 1 && missing == Binding.JScriptMissingCW && this.defaultMember is PropertyInfo){
            //Dealing with the CreateInstance method of a constructor function
            Debug.Assert(meth.Name == "CreateInstance" || meth.Name == "Invoke");
            il.Emit(OpCodes.Ldc_I4_1);
            il.Emit(OpCodes.Newarr, Typeob.Object);
            il.Emit(OpCodes.Dup);
            il.Emit(OpCodes.Ldc_I4_0);
            argList[0].TranslateToIL(il, Typeob.Object);
            il.Emit(OpCodes.Stelem_Ref);
          }else
            Binding.PlaceArgumentsOnStack(il, pars, argList, offset, 0, missing);
          meth = this.GetMethodInfoMetadata(meth);
          if (!this.isNonVirtual && meth.IsVirtual && !meth.IsFinal && (!rt.IsSealed || !rt.IsValueType))
            il.Emit(OpCodes.Callvirt, meth);
          else
            il.Emit(OpCodes.Call, meth);
          Convert.Emit(this, il, meth.ReturnType, rtype);
          return;
        }
        if (member is ConstructorInfo){
          Debug.Assert(construct);
          ConstructorInfo cons = (ConstructorInfo)member;
          ParameterInfo[] pars = cons.GetParameters();
          bool instanceNestedClassConstructor = false;
          if (CustomAttribute.IsDefined(cons, typeof(JSFunctionAttribute), false)){
            Object[] attrs = CustomAttribute.GetCustomAttributes(cons, typeof(JSFunctionAttribute), false);
            instanceNestedClassConstructor = (((JSFunctionAttribute)attrs[0]).attributeValue & JSFunctionAttributeEnum.IsInstanceNestedClassConstructor) != 0;
          }
          if (instanceNestedClassConstructor){
            Binding.PlaceArgumentsOnStack(il, pars, argList, 0, 1, Binding.ReflectionMissingCW);
            this.TranslateToILObject(il, pars[pars.Length-1].ParameterType, false);
          }else
            Binding.PlaceArgumentsOnStack(il, pars, argList, 0, 0, Binding.ReflectionMissingCW);
          Type outerClass = null;
          if (member is JSConstructor && (outerClass = ((JSConstructor)member).OuterClassType()) != null)
            this.TranslateToILObject(il, outerClass, false);
          bool needEngine = false;
          Type t = cons.DeclaringType;
          if (cons is JSConstructor){
            cons = ((JSConstructor)cons).GetConstructorInfo(compilerGlobals);
            needEngine = true;
          }else
            needEngine = Typeob.INeedEngine.IsAssignableFrom(t);
          il.Emit(OpCodes.Newobj, cons);
          if (needEngine){
            il.Emit(OpCodes.Dup);
            this.EmitILToLoadEngine(il);
            il.Emit(OpCodes.Callvirt, CompilerGlobals.setEngineMethod);
          }
          Convert.Emit(this, il, t, rtype);
          return;
        }
        Type mt = member as Type;
        if (mt != null){
          Debug.Assert(!construct && !brackets);
          Debug.Assert(argList.count == 1);
          AST arg0 = argList[0];
          if (arg0 is NullLiteral){ //Skip the double conversion path below that involves runtime helper
            arg0.TranslateToIL(il, mt);
            Convert.Emit(this, il, mt, rtype);
            return;
          }
          IReflect arg0ir = arg0.InferType(null);
          if (arg0ir == Typeob.ScriptFunction && Typeob.Delegate.IsAssignableFrom(mt))
            arg0.TranslateToIL(il, mt);
          else{
            Type argType = Convert.ToType(arg0ir);
            arg0.TranslateToIL(il, argType);
            Convert.Emit(this, il, argType, mt, true);
          }
          Convert.Emit(this, il, mt, rtype);
          return;
        }
        if (member is FieldInfo && ((FieldInfo)member).IsLiteral){
          Object val = member is JSVariableField ? ((JSVariableField)member).value : TypeReferences.GetConstantValue((FieldInfo)member);
          if (val is Type || val is ClassScope || val is TypedArray){
            Debug.Assert(argList.count == 1);
            AST arg0 = argList[0];
            if (arg0 is NullLiteral){
              il.Emit(OpCodes.Ldnull);
              return;
            }
            ClassScope csc = val as ClassScope;
            if (csc != null){
              EnumDeclaration ed = csc.owner as EnumDeclaration;
              if (ed != null)
                val = ed.baseType.ToType();
            }
            Type argType = Convert.ToType(arg0.InferType(null));
            arg0.TranslateToIL(il, argType);
            Type t = val is Type ? (Type)val : val is ClassScope ? Convert.ToType((ClassScope)val) : ((TypedArray)val).ToType();
            Convert.Emit(this, il, argType, t, true);
            if (!rtype.IsEnum)
              Convert.Emit(this, il, t, rtype);
            return;
          }
        }
        LocalBuilder loc = null;
        for (int i = 0, n = argList.count; i < n; i++){
          if (argList[i] is AddressOf){
            loc = il.DeclareLocal(Typeob.ArrayOfObject);
            break;
          }
        }
        Object done = null;
        if (member == null && (this.members == null || this.members.Length == 0)){
          if (this is Lookup)
            ((Lookup)this).TranslateToLateBinding(il);
          else{
            done = this.TranslateToSpeculativeEarlyBoundCalls(il, rtype, argList, construct, brackets);
            ((Member)this).TranslateToLateBinding(il, done != null);
          }
          argList.TranslateToIL(il, Typeob.ArrayOfObject);
          if (loc != null){
            il.Emit(OpCodes.Dup);
            il.Emit(OpCodes.Stloc, loc);
          }
          if (construct)
            il.Emit(OpCodes.Ldc_I4_1);
          else
            il.Emit(OpCodes.Ldc_I4_0);
          if (brackets)
            il.Emit(OpCodes.Ldc_I4_1);
          else
            il.Emit(OpCodes.Ldc_I4_0);
          this.EmitILToLoadEngine(il);
          il.Emit(OpCodes.Call, CompilerGlobals.callMethod);
          Convert.Emit(this, il, Typeob.Object, rtype);
          if (loc != null){
            for (int i = 0, n = argList.count; i < n; i++){
              AddressOf addr = argList[i] as AddressOf;
              if (addr != null){
                addr.TranslateToILPreSet(il);
                il.Emit(OpCodes.Ldloc, loc);
                ConstantWrapper.TranslateToILInt(il, i);
                il.Emit(OpCodes.Ldelem_Ref);
                Convert.Emit(this, il, Typeob.Object, Convert.ToType(addr.InferType(null)));
                addr.TranslateToILSet(il, null);
              }
            }
          }
          if (done != null)
            il.MarkLabel((Label)done);
          return;
        }
        this.TranslateToILWithDupOfThisOb(il);
        argList.TranslateToIL(il, Typeob.ArrayOfObject);
        if (loc != null){
          il.Emit(OpCodes.Dup);
          il.Emit(OpCodes.Stloc, loc);
        }
        if (construct)
          il.Emit(OpCodes.Ldc_I4_1);
        else
          il.Emit(OpCodes.Ldc_I4_0);
        if (brackets)
          il.Emit(OpCodes.Ldc_I4_1);
        else
          il.Emit(OpCodes.Ldc_I4_0);
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.callValueMethod);
        Convert.Emit(this, il, Typeob.Object, rtype);
        if (loc != null){
          for (int i = 0, n = argList.count; i < n; i++){
            AddressOf addr = argList[i] as AddressOf;
            if (addr != null){
              addr.TranslateToILPreSet(il);
              il.Emit(OpCodes.Ldloc, loc);
              ConstantWrapper.TranslateToILInt(il, i);
              il.Emit(OpCodes.Ldelem_Ref);
              Convert.Emit(this, il, Typeob.Object, Convert.ToType(addr.InferType(null)));
              addr.TranslateToILSet(il, null);
            }
          }
        }
      }
      
      internal override void TranslateToILDelete(ILGenerator il, Type rtype){
        if (this is Lookup)
          ((Lookup)this).TranslateToLateBinding(il);
        else
          ((Member)this).TranslateToLateBinding(il, false);
        il.Emit(OpCodes.Call, CompilerGlobals.deleteMethod);
        Convert.Emit(this, il, Typeob.Boolean, rtype);
      }
      
      protected abstract void TranslateToILObject(ILGenerator il, Type obtype, bool noValue);
      
      internal override void TranslateToILPreSet(ILGenerator il){
        this.TranslateToIL(il, null, true, false);
      }
      
      internal override void TranslateToILPreSet(ILGenerator il, ASTList argList){
        if (this.isArrayElementAccess){
          this.member = this.defaultMember;
          IReflect defIR = this.defaultMemberReturnIR;
          Type defType = defIR is Type ? (Type)defIR : Convert.ToType(defIR);
          this.TranslateToIL(il, defType);
          Debug.Assert(defType.IsArray);
          for (int i = 0, m = argList.count; i < m; i++)
            argList[i].TranslateToIL(il, Typeob.Int32);
          if (defType.GetArrayRank() == 1){
            Type etype = defType.GetElementType();
            if (etype.IsValueType && !etype.IsPrimitive && !etype.IsEnum)
              il.Emit(OpCodes.Ldelema, etype);
          }
          return;
        }
        Debug.Assert(this.isAssignmentToDefaultIndexedProperty);
        if (this.member is PropertyInfo && this.defaultMember != null){ //early bound to default indexed property
          PropertyInfo prop = (PropertyInfo)this.member;
          this.member = this.defaultMember;
          this.TranslateToIL(il, Convert.ToType(this.defaultMemberReturnIR));
          this.member = prop;
          Binding.PlaceArgumentsOnStack(il, prop.GetIndexParameters(), argList, 0, 0, Binding.ReflectionMissingCW);
          return;
        }
        base.TranslateToILPreSet(il, argList);
      }
      
      internal override void TranslateToILPreSetPlusGet(ILGenerator il){
        this.TranslateToIL(il, Convert.ToType(this.InferType(null)), false, true);
      }
      
      internal override void TranslateToILPreSetPlusGet(ILGenerator il, ASTList argList, bool inBrackets){
        if (this.isArrayElementAccess){
          this.member = this.defaultMember;
          IReflect defIR = this.defaultMemberReturnIR;
          Type defType = defIR is Type ? (Type)defIR : Convert.ToType(defIR);
          Debug.Assert(defType.IsArray);
          this.TranslateToIL(il, defType);
          il.Emit(OpCodes.Dup);
          int n = defType.GetArrayRank();
          LocalBuilder[] iTemp = new LocalBuilder[n];
          for (int i = 0, m = argList.count; i < m; i++){
            argList[i].TranslateToIL(il, Typeob.Int32);
            iTemp[i] = il.DeclareLocal(Typeob.Int32);
            il.Emit(OpCodes.Dup);
            il.Emit(OpCodes.Stloc, iTemp[i]);
          }
          Type etype = defType.GetElementType();
          if (n == 1){
            Binding.TranslateToLdelem(il, etype);
          }else{
            Type[] indexTypes = new Type[n];
            for (int i = 0; i < n; i++) indexTypes[i] = Typeob.Int32;
            MethodInfo getter = defType.GetMethod("Get", indexTypes);
            il.Emit(OpCodes.Call, getter);
          }
          LocalBuilder eTemp = il.DeclareLocal(etype);
          il.Emit(OpCodes.Stloc, eTemp);
          for (int i = 0; i < n; i++)
            il.Emit(OpCodes.Ldloc, iTemp[i]);
          if (n == 1 && etype.IsValueType && !etype.IsPrimitive)
              il.Emit(OpCodes.Ldelema, etype);
          il.Emit(OpCodes.Ldloc, eTemp);
          return;
        }
        Debug.Assert(this.isAssignmentToDefaultIndexedProperty);
        if (this.member != null){
          //Go late bound. It is too much work, for too little gain, to do this early bound
          if (this.defaultMember != null){
            this.member = this.defaultMember;
            this.defaultMember = null;
          }
        }
        base.TranslateToILPreSetPlusGet(il, argList, inBrackets);
      }
      
      internal override Object TranslateToILReference(ILGenerator il, Type rtype){
        if (this.member is FieldInfo){
          FieldInfo field = (FieldInfo)this.member;
          Type ftype = field.FieldType;
          if (rtype == ftype){
            bool isStatic = field.IsStatic;
            if (!isStatic)
              this.TranslateToILObject(il, field.DeclaringType, true);
            Object tok = field is JSField ? ((JSField)field).GetMetaData() : field is JSFieldInfo ? ((JSFieldInfo)field).field : field;
            if (tok is FieldInfo)
              if (field.IsInitOnly){
                LocalBuilder loc = il.DeclareLocal(ftype);
                il.Emit(isStatic ? OpCodes.Ldsfld : OpCodes.Ldfld, (FieldInfo)tok);
                il.Emit(OpCodes.Stloc, loc);
                il.Emit(OpCodes.Ldloca, loc);
              }else
                il.Emit(isStatic ? OpCodes.Ldsflda : OpCodes.Ldflda, (FieldInfo)tok);
            else if (tok is LocalBuilder)
              il.Emit(OpCodes.Ldloca, (LocalBuilder)tok);
            else
              il.Emit(OpCodes.Ldarga, (short)tok);
            return null;
          }
        }
        return base.TranslateToILReference(il, rtype);
      }
      
      internal override void TranslateToILSet(ILGenerator il, AST rhvalue){
        if (this.isArrayElementAccess){
          IReflect defIR = this.defaultMemberReturnIR;
          Type defType = defIR is Type ? (Type)defIR : Convert.ToType(defIR);
          Debug.Assert(defType.IsArray);
          int n = defType.GetArrayRank();
          Type etype = defType.GetElementType();
          if (rhvalue != null)
            rhvalue.TranslateToIL(il, etype);
          if (n == 1){
            Binding.TranslateToStelem(il, etype);
          }else{
            Type[] indexTypes = new Type[n+1];
            for (int i = 0; i < n; i++) indexTypes[i] = Typeob.Int32;
            indexTypes[n] = etype;
            MethodInfo setter = compilerGlobals.module.GetArrayMethod(defType, "Set", CallingConventions.HasThis, Typeob.Void, indexTypes);
            il.Emit(OpCodes.Call, setter);
          }
          return;
        }
        if (this.isAssignmentToDefaultIndexedProperty){
          if (this.member is PropertyInfo && this.defaultMember != null){ //early bound to default indexed property
            PropertyInfo prop = (PropertyInfo)this.member;
            MethodInfo meth = JSProperty.GetSetMethod(prop, false);
            //Guard against trying to assign to properties on the Global object
            JSWrappedMethod wmeth = meth as JSWrappedMethod;
            if (wmeth == null || !(wmeth.GetWrappedObject() is GlobalObject)){
              meth = this.GetMethodInfoMetadata(meth);
              if (rhvalue != null)
                rhvalue.TranslateToIL(il, prop.PropertyType);
              if (meth.IsVirtual && !meth.IsFinal && (!meth.ReflectedType.IsSealed || !meth.ReflectedType.IsValueType))
                il.Emit(OpCodes.Callvirt, meth);
              else
                il.Emit(OpCodes.Call, meth);
              return;
            }
          }
          base.TranslateToILSet(il, rhvalue);
          return;
        }
        if (this.member is FieldInfo){
          FieldInfo field = (FieldInfo)this.member;
          if (rhvalue != null)
            rhvalue.TranslateToIL(il, field.FieldType);
          if (field.IsLiteral || field.IsInitOnly){
            il.Emit(OpCodes.Pop);
            return;
          }
          Object tok = field is JSField ? ((JSField)field).GetMetaData() : field is JSFieldInfo ? ((JSFieldInfo)field).field : field;
          FieldInfo f = tok as FieldInfo;
          if (f != null)
            il.Emit(f.IsStatic ? OpCodes.Stsfld : OpCodes.Stfld, f);
          else if (tok is LocalBuilder)
            il.Emit(OpCodes.Stloc, (LocalBuilder)tok);
          else
            il.Emit(OpCodes.Starg, (short)tok);
          return;
        }
        if (this.member is PropertyInfo){
          PropertyInfo prop = (PropertyInfo)this.member;
          if (rhvalue != null)
            rhvalue.TranslateToIL(il, prop.PropertyType);
          MethodInfo meth = JSProperty.GetSetMethod(prop, true);
          if (meth == null){
            il.Emit(OpCodes.Pop);
            return;
          }
          meth = this.GetMethodInfoMetadata(meth);
          if (meth.IsStatic && !(meth is JSClosureMethod))
            il.Emit(OpCodes.Call, meth);
          else{
            if (!this.isNonVirtual && meth.IsVirtual && !meth.IsFinal && (!meth.ReflectedType.IsSealed || !meth.ReflectedType.IsValueType))
              il.Emit(OpCodes.Callvirt, meth);
            else
              il.Emit(OpCodes.Call, meth);
          }
          return;
        }
        //do speculative early bound assignments
        Object done = this.TranslateToSpeculativeEarlyBoundSet(il, rhvalue);
        if (rhvalue != null)
          rhvalue.TranslateToIL(il, Typeob.Object);
        il.Emit(OpCodes.Call, CompilerGlobals.setValueMethod);
        if (done != null)
          il.MarkLabel((Label)done);
      }
      
      protected abstract void TranslateToILWithDupOfThisOb(ILGenerator il);
      
      private static void TranslateToLdelem(ILGenerator il, Type etype){
        switch(Type.GetTypeCode(etype)){
          case TypeCode.SByte:
            il.Emit(OpCodes.Ldelem_I1); break;
          case TypeCode.Boolean:
          case TypeCode.Byte:
            il.Emit(OpCodes.Ldelem_U1); break;
          case TypeCode.Int16:
            il.Emit(OpCodes.Ldelem_I2); break;
          case TypeCode.Char:
          case TypeCode.UInt16:
            il.Emit(OpCodes.Ldelem_U2); break;
          case TypeCode.Int32:
            il.Emit(OpCodes.Ldelem_I4); break;
          case TypeCode.UInt32:
            il.Emit(OpCodes.Ldelem_U4); break;
          case TypeCode.Int64:
          case TypeCode.UInt64:
            il.Emit(OpCodes.Ldelem_I8); break;
          case TypeCode.Single:
            il.Emit(OpCodes.Ldelem_R4); break;
          case TypeCode.Double:
            il.Emit(OpCodes.Ldelem_R8); break;
          case TypeCode.Decimal:
          case TypeCode.DateTime:
          case TypeCode.String:
          case TypeCode.Object:
            if (etype.IsValueType){
              il.Emit(OpCodes.Ldelema, etype);
              il.Emit(OpCodes.Ldobj, etype);
            }else
              il.Emit(OpCodes.Ldelem_Ref);
            break;
        }
      }
      
      private Object TranslateToSpeculativeEarlyBoundSet(ILGenerator il, AST rhvalue){
        this.giveErrors = false;
        Object done = null;
        bool needObject = true;
        LocalBuilder objectLocal = null;
        LocalBuilder valueLocal = null;
        Label next = il.DefineLabel();
        MemberInfoList members = this.GetAllKnownInstanceBindingsForThisName();
        for (int i = 0, n = members.count; i < n; i++){
          MemberInfo member = members[i];
          FieldInfo field = null;
          MethodInfo setter = null;
          PropertyInfo prop = null;
          if (member is FieldInfo){
            field = (FieldInfo)member;
            if (field.IsLiteral || field.IsInitOnly) continue;
          }else if (member is PropertyInfo){
            prop = (PropertyInfo)member;
            if (prop.GetIndexParameters().Length > 0 ||
                (setter = JSProperty.GetSetMethod(prop, true)) == null) continue;
          }else
            continue;
          this.member = member;
          if (!this.Accessible(true)) continue;
          if (needObject){
            needObject = false;
            if (rhvalue == null){
              valueLocal = il.DeclareLocal(Typeob.Object);
              il.Emit(OpCodes.Stloc, valueLocal);
            }
            il.Emit(OpCodes.Dup);
            il.Emit(OpCodes.Ldfld, CompilerGlobals.objectField);
            objectLocal = il.DeclareLocal(Typeob.Object);
            il.Emit(OpCodes.Stloc, objectLocal);
            done = il.DefineLabel();
          }
          Type t = member.DeclaringType;
          il.Emit(OpCodes.Ldloc, objectLocal);
          il.Emit(OpCodes.Isinst, t);
          LocalBuilder objectTemp = il.DeclareLocal(t);
          il.Emit(OpCodes.Dup);
          il.Emit(OpCodes.Stloc, objectTemp);
          il.Emit(OpCodes.Brfalse, next);
          il.Emit(OpCodes.Ldloc, objectTemp);
          if (rhvalue == null)
            il.Emit(OpCodes.Ldloc, valueLocal);
          if (field != null){
            if (rhvalue == null)
              Convert.Emit(this, il, Typeob.Object, field.FieldType);
            else
              rhvalue.TranslateToIL(il, field.FieldType);
            if (field is JSField)
              il.Emit(OpCodes.Stfld, (FieldInfo)((JSField)field).GetMetaData());
            else if (field is JSFieldInfo)
              il.Emit(OpCodes.Stfld, ((JSFieldInfo)field).field);
            else
              il.Emit(OpCodes.Stfld, field);
          }else{
            if (rhvalue == null)
              Convert.Emit(this, il, Typeob.Object, prop.PropertyType);
            else
              rhvalue.TranslateToIL(il, prop.PropertyType);
            setter = this.GetMethodInfoMetadata(setter);
            if (setter.IsVirtual && !setter.IsFinal && (!t.IsSealed || !t.IsValueType))
              il.Emit(OpCodes.Callvirt, setter);
            else
              il.Emit(OpCodes.Call, setter);
          }
          il.Emit(OpCodes.Pop); //Get rid of the LateBound instance
          il.Emit(OpCodes.Br, (Label)done);
          il.MarkLabel(next);
          next = il.DefineLabel();
        }
        if (valueLocal != null)
          il.Emit(OpCodes.Ldloc, valueLocal);
        this.member = null;
        return done;
      }
      
      private Object TranslateToSpeculativeEarlyBindings(ILGenerator il, Type rtype, bool getObjectFromLateBindingInstance){
        //Find all members named this.name to which the current context might have access to.
        //Generate early bound access to these, guarded by runtime type checks.
        this.giveErrors = false;
        Object done = null;
        bool needObject = true;
        LocalBuilder objectLocal = null;
        Label next = il.DefineLabel();
        MemberInfoList members = this.GetAllKnownInstanceBindingsForThisName();
        for (int i = 0, n = members.count; i < n; i++){
          MemberInfo member = members[i];
          if (!(member is FieldInfo) && (!(member is PropertyInfo) || 
              ((PropertyInfo)member).GetIndexParameters().Length > 0 ||
              JSProperty.GetGetMethod((PropertyInfo)member, true) == null)) continue;
          this.member = member;
          if (!this.Accessible(false)) continue;
          if (needObject){
            needObject = false;
            if (getObjectFromLateBindingInstance){
              il.Emit(OpCodes.Dup);
              il.Emit(OpCodes.Ldfld, CompilerGlobals.objectField);
            }else
              this.TranslateToILObject(il, Typeob.Object, false);
            objectLocal = il.DeclareLocal(Typeob.Object);
            il.Emit(OpCodes.Stloc, objectLocal);
            done = il.DefineLabel();
          }
          Type t = member.DeclaringType;
          il.Emit(OpCodes.Ldloc, objectLocal);
          il.Emit(OpCodes.Isinst, t);
          LocalBuilder objectTemp = il.DeclareLocal(t);
          il.Emit(OpCodes.Dup);
          il.Emit(OpCodes.Stloc, objectTemp);
          il.Emit(OpCodes.Brfalse_S, next);
          il.Emit(OpCodes.Ldloc, objectTemp);
          if (member is FieldInfo){
            FieldInfo field = (FieldInfo)member;
            if (field.IsLiteral){
              il.Emit(OpCodes.Pop);
              continue; //instance nested class, bail out.                                             
            }
            if (field is JSField)
              il.Emit(OpCodes.Ldfld, (FieldInfo)((JSField)field).GetMetaData());
            else if (field is JSFieldInfo)
              il.Emit(OpCodes.Ldfld, ((JSFieldInfo)field).field);
            else
              il.Emit(OpCodes.Ldfld, (FieldInfo)field);
            Convert.Emit(this, il, field.FieldType, rtype);
          }else if (member is PropertyInfo){
            MethodInfo getter = JSProperty.GetGetMethod((PropertyInfo)member, true);
            getter = this.GetMethodInfoMetadata(getter);
            if (getter.IsVirtual && !getter.IsFinal && (!t.IsSealed || t.IsValueType))
              il.Emit(OpCodes.Callvirt, getter);
            else
              il.Emit(OpCodes.Call, getter);
            Convert.Emit(this, il, getter.ReturnType, rtype);
          }
          il.Emit(OpCodes.Br, (Label)done);
          il.MarkLabel(next);
          next = il.DefineLabel();
        }
        il.MarkLabel(next);
        if (!needObject && !getObjectFromLateBindingInstance)
          il.Emit(OpCodes.Ldloc, objectLocal);
        this.member = null;
        return done;
      }
      
      private Object TranslateToSpeculativeEarlyBoundCalls(ILGenerator il, Type rtype, ASTList argList, bool construct, bool brackets){
        this.giveErrors = false;
        Object done = null;
        bool needObject = true;
        LocalBuilder objectLocal = null;
        Label next = il.DefineLabel();
        IReflect[] classes = this.GetAllEligibleClasses();
        if (construct){
          //dealing with "new ob.foo(...)" or "new ob.foo[...]".
          //Early binding only makes sense if foo is a nested type marked as protected or package
          //But nested types are static and hence will not show up late bound via the object.
          //There is the possibility that a programmer can declare a static field and initialize it to a type.
          //In that case late bound access works if the field is public, but not otherwise.
          //This does not seem like a core scenario that justifies a lot of code inside this if statement.
          return done;
        }
        Debug.Assert(argList.count == 0 || this.argIRs.Length == argList.count);
        foreach (IReflect c in classes){
          MemberInfo[] members = c.GetMember(this.name, BindingFlags.Public|BindingFlags.NonPublic|BindingFlags.Instance);
          try{
            // This code generation does not account for the case ob.foo(...) where ob.foo is a property
            // with zero arguments. The code should get the property followed by a call on the
            // returned object. For this case, we don't emit the speculative IL and let the late bound
            // code handle it.
            MethodInfo meth;
            MemberInfo mem = JSBinder.SelectCallableMember(members, this.argIRs);
            if (mem != null && mem.MemberType == MemberTypes.Property) 
            {
              ParameterInfo[] parameters;
              meth = ((PropertyInfo)mem).GetGetMethod(true);
              if (meth == null || (parameters = meth.GetParameters()) == null || parameters.Length == 0)
                continue;
            }
            else
              meth = mem as MethodInfo;

            if (meth != null){
              if (!Binding.CheckParameters(meth.GetParameters(), this.argIRs, argList, this.context, 0, true, false))
                continue;
              //If meth is a base class method or if meth is an override of a base class method and the base class is in the same package as c
              //there is no need to have an instance check for c
              if (meth is JSFieldMethod){
                FunctionObject func = ((JSFieldMethod)meth).func;
                if (func != null && (func.attributes & MethodAttributes.NewSlot) == 0 && ((ClassScope)c).ParentIsInSamePackage())
                  continue;
              }else if (meth is JSWrappedMethod && ((JSWrappedMethod)meth).obj is ClassScope && ((JSWrappedMethod)meth).GetPackage() == ((ClassScope)c).package)
                continue;
              this.member = meth;
              if (!this.Accessible(false)) continue;
              if (needObject){
                needObject = false;
                this.TranslateToILObject(il, Typeob.Object, false);
                objectLocal = il.DeclareLocal(Typeob.Object);
                il.Emit(OpCodes.Stloc, objectLocal);
                done = il.DefineLabel();
              }
              Type t = meth.DeclaringType;
              il.Emit(OpCodes.Ldloc, objectLocal);
              il.Emit(OpCodes.Isinst, t);
              LocalBuilder objectTemp = il.DeclareLocal(t);
              il.Emit(OpCodes.Dup);
              il.Emit(OpCodes.Stloc, objectTemp);
              il.Emit(OpCodes.Brfalse, next);
              il.Emit(OpCodes.Ldloc, objectTemp);
              Binding.PlaceArgumentsOnStack(il, meth.GetParameters(), argList, 0, 0, Binding.ReflectionMissingCW);
              meth = this.GetMethodInfoMetadata(meth);
              if (meth.IsVirtual && !meth.IsFinal && (!t.IsSealed || t.IsValueType))
                il.Emit(OpCodes.Callvirt, meth);
              else
                il.Emit(OpCodes.Call, meth);
              Convert.Emit(this, il, meth.ReturnType, rtype);
              il.Emit(OpCodes.Br, (Label)done);
              il.MarkLabel(next);
              next = il.DefineLabel();
            }
          }catch(AmbiguousMatchException){}
        }
        il.MarkLabel(next);
        if (!needObject)
          il.Emit(OpCodes.Ldloc, objectLocal);
        this.member = null;
        return done;
      }
      
      internal static void TranslateToStelem(ILGenerator il, Type etype){
        switch(Type.GetTypeCode(etype)){
          case TypeCode.SByte:
          case TypeCode.Boolean:
          case TypeCode.Byte:
            il.Emit(OpCodes.Stelem_I1); break;
          case TypeCode.Int16:
          case TypeCode.Char:
          case TypeCode.UInt16:
            il.Emit(OpCodes.Stelem_I2); break;
          case TypeCode.Int32:
          case TypeCode.UInt32:
            il.Emit(OpCodes.Stelem_I4); break;
          case TypeCode.Int64:
          case TypeCode.UInt64:
            il.Emit(OpCodes.Stelem_I8); break;
          case TypeCode.Single:
            il.Emit(OpCodes.Stelem_R4); break;
          case TypeCode.Double:
            il.Emit(OpCodes.Stelem_R8); break;
          case TypeCode.Decimal:
          case TypeCode.DateTime:
          case TypeCode.String:
          case TypeCode.Object:
            if (etype.IsValueType){
              il.Emit(OpCodes.Stobj, etype);
            }else
              il.Emit(OpCodes.Stelem_Ref);
            break;
        }
      }
      
      private void WarnIfNotFullyResolved(){
        if (this.isFullyResolved || this.member == null) return;
        if (this.member is JSVariableField && ((JSVariableField)this.member).type == null) return; 
        if (!this.Engine.doFast && this.member is IWrappedMember) return;
        ScriptObject scope = Globals.ScopeStack.Peek();
        while (scope != null){
          if (scope is WithObject && !((WithObject)scope).isKnownAtCompileTime){
            this.context.HandleError(JSError.AmbiguousBindingBecauseOfWith);
            return;
          }else if (scope is ActivationObject && !((ActivationObject)scope).isKnownAtCompileTime){
            this.context.HandleError(JSError.AmbiguousBindingBecauseOfEval);
            return;
          }
          scope = scope.GetParent();
        }
      }
      
      private void WarnIfObsolete(){
        Binding.WarnIfObsolete(this.member, this.context);
      }

      internal static void WarnIfObsolete(MemberInfo member, Context context){
        if (member == null)
          return;
        String message = null;
        bool isError = false;
        Object[] custAttribs = CustomAttribute.GetCustomAttributes(member, typeof(ObsoleteAttribute), false);
        if (custAttribs != null && custAttribs.Length > 0){
          ObsoleteAttribute attr = (ObsoleteAttribute)custAttribs[0];
          message =  attr.Message;
          isError = attr.IsError;
        }else{
          custAttribs = CustomAttribute.GetCustomAttributes(member, typeof(NotRecommended), false);
          if (custAttribs != null && custAttribs.Length > 0){
            NotRecommended attr = (NotRecommended)custAttribs[0];
            message = ": " + attr.Message;
            isError = false;
          }else
            return;
        }
        context.HandleError(JSError.Deprecated, message, isError);

      }
      
      private MethodInfo GetMethodInfoMetadata(MethodInfo method) {
        if (method is JSMethod)
          return ((JSMethod)method).GetMethodInfo(this.compilerGlobals);
        if (method is JSMethodInfo)
          return ((JSMethodInfo)method).method;
        return method;
      }
    }    
}
