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
    using System.Reflection.Emit;
    using System.Globalization;
    using System.Diagnostics;
    
    public sealed class JSConstructor : ConstructorInfo{
      internal FunctionObject cons;
      
      internal JSConstructor(FunctionObject cons){
        this.cons = cons;
      }
      
      public override MethodAttributes Attributes{
        get{
          return this.cons.attributes;
        }
      }
      
    
      internal Object Construct(Object thisob, Object[] args){
        return LateBinding.CallValue(this.cons, args, true, false, cons.engine, thisob, JSBinder.ob, null, null);
      }
    
      public override String Name{
        get{
          return this.cons.name;
        }
      }
    
      public override Type DeclaringType{
        get{
          return Convert.ToType(this.cons.enclosing_scope);
        }
      }
      
      internal String GetClassFullName(){
        return ((ClassScope)this.cons.enclosing_scope).GetFullName();
      }
      
      internal ClassScope GetClassScope(){
        return (ClassScope)this.cons.enclosing_scope;
      }

      internal ConstructorInfo GetConstructorInfo(CompilerGlobals compilerGlobals){
        return this.cons.GetConstructorInfo(compilerGlobals);
      }
      
      public override Object[] GetCustomAttributes(Type t, bool inherit){
        return new Object[0];
      }

      public override Object[] GetCustomAttributes(bool inherit){
        if (this.cons != null){
          CustomAttributeList caList = this.cons.customAttributes;
          if (caList != null) return (Object[])caList.Evaluate(false);
        }
        return new Object[0];
      }
    
    
      public override MethodImplAttributes GetMethodImplementationFlags(){
        return (MethodImplAttributes)0;
      }
      
      internal PackageScope GetPackage(){
        return ((ClassScope)this.cons.enclosing_scope).GetPackage();
      }
            
      public override ParameterInfo[] GetParameters(){
        return this.cons.parameter_declarations;
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public override Object Invoke(BindingFlags options, Binder binder, Object[] parameters, CultureInfo culture){
        return LateBinding.CallValue(this.cons, parameters, true, false, this.cons.engine, null, binder, culture, null);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public override Object Invoke(Object obj, BindingFlags options, Binder binder, Object[] parameters, CultureInfo culture){
        return this.cons.Call(parameters, obj, binder, culture);
      }
    
      internal bool IsAccessibleFrom(ScriptObject scope){ //Never call this if the member is public
        while (scope != null && !(scope is ClassScope))
          scope = scope.GetParent();
        ClassScope thisScope = (ClassScope)this.cons.enclosing_scope;
        if (this.IsPrivate)
          if (scope == null)
            return false;
          else
            return scope == thisScope || ((ClassScope)scope).IsNestedIn(thisScope, false);
        else if (this.IsFamily)
          if (scope == null)
            return false;
          else
            return ((ClassScope)scope).IsSameOrDerivedFrom(thisScope) || ((ClassScope)scope).IsNestedIn(thisScope, false);
        else { // if (this.IsAssembly || this.isFamilyOrAssembly)
          if (this.IsFamilyOrAssembly && scope != null &&
              (((ClassScope)scope).IsSameOrDerivedFrom(thisScope) || ((ClassScope)scope).IsNestedIn(thisScope, false)))
            return true;
          else if (scope == null) //Code is not in a class and hence it is in the default package
            return thisScope.GetPackage() == null; //null indicates default package
          else
            return thisScope.GetPackage() == ((ClassScope)scope).GetPackage();
        }
      }      
      
      public override bool IsDefined(Type type, bool inherit){ 
        return false;
      }

      public override MemberTypes MemberType{
        get{
          return MemberTypes.Constructor;
        }
      }
      
      public override RuntimeMethodHandle MethodHandle{
        get{
          return this.GetConstructorInfo(null).MethodHandle;
        }
      }
      
      internal Type OuterClassType(){
        FieldInfo ocField = ((ClassScope)this.cons.enclosing_scope).outerClassField;
        return ocField == null ? null : ocField.FieldType;
      }
      
      public override Type ReflectedType{
        get{
          return this.DeclaringType;
        }
      }
    }
}
