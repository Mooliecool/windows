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

//This class is a compile time helper only. It is used by class definitions marked as expando. The evaluator never sees instances of this class.

namespace Microsoft.JScript {
    
  using System;
  using System.Reflection;
  using System.Globalization;
  using System.Diagnostics;
  
  internal sealed class JSExpandoIndexerMethod : JSMethod{
    private ClassScope classScope;
    private bool isGetter;
    private MethodInfo token;

    private ParameterInfo[] GetterParams;
    private ParameterInfo[] SetterParams;
  
    internal JSExpandoIndexerMethod(ClassScope classScope, bool isGetter)
      : base(null){ //The object is never used, but it is convenient to have the field on JSMethod.
      this.isGetter = isGetter;
      this.classScope = classScope;
      
      this.GetterParams= new ParameterInfo[]{new ParameterDeclaration(Typeob.String, "field")};
      this.SetterParams = new ParameterInfo[]{new ParameterDeclaration(Typeob.String, "field"), 
                new ParameterDeclaration(Typeob.Object, "value")};
    }

    internal override Object Construct(Object[] args){
      throw new JScriptException(JSError.InvalidCall);
    }
    
    public override MethodAttributes Attributes{
      get{
        return MethodAttributes.Public;
      }
    }

    public override Type DeclaringType{
      get{
        return this.classScope.GetTypeBuilderOrEnumBuilder();
      }
    }
    
    public override ParameterInfo[] GetParameters(){
      if (this.isGetter)
        return this.GetterParams;
      else
        return this.SetterParams;
    }
    
    internal override MethodInfo GetMethodInfo(CompilerGlobals compilerGlobals){
      if (this.isGetter){
        if (this.token == null)
          this.token = this.classScope.owner.GetExpandoIndexerGetter();
      }else{
        if (this.token == null)
          this.token = this.classScope.owner.GetExpandoIndexerSetter();
      }
      return this.token;
    }
    
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
    internal override Object Invoke(Object obj, Object thisob, BindingFlags options, Binder binder, Object[] parameters, CultureInfo culture){
      throw new JScriptException(JSError.InvalidCall);
    }     
    
    public override String Name{
      get{
        if (this.isGetter)
          return "get_Item";
        else
          return "set_Item";
      }
    }
  
    public override Type ReturnType{
      get {
        if (this.isGetter)
          return Typeob.Object;
        else
          return Typeob.Void;
      }
    }
  }

}
