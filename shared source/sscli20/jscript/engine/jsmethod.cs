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
    using System.Runtime.InteropServices;
    
    [GuidAttribute("561AC104-8869-4368-902F-4E0D7DDEDDDD")]
    [ComVisible(true)]
    public abstract class JSMethod : MethodInfo{
      internal Object obj;
      
      internal JSMethod(Object obj){
        this.obj = obj;
      }
      
      internal abstract Object Construct(Object[] args);
    
      public override MethodInfo GetBaseDefinition(){
        return this;
      }

      internal virtual String GetClassFullName(){
        if (this.obj is ClassScope){
          return ((ClassScope)this.obj).GetFullName();
        }
        throw new JScriptException(JSError.InternalError);
      }

      public override Object[] GetCustomAttributes(Type t, bool inherit){
        return new Object[0];
      }

      public override Object[] GetCustomAttributes(bool inherit){
        return new Object[0];
      }
    
      public override MethodImplAttributes GetMethodImplementationFlags(){
        return (MethodImplAttributes)0;
      }
      
      internal abstract MethodInfo GetMethodInfo(CompilerGlobals compilerGlobals);
      
      internal virtual PackageScope GetPackage(){
        if (this.obj is ClassScope){
          return ((ClassScope)this.obj).GetPackage();
        }
        throw new JScriptException(JSError.InternalError);
      }
            
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public override Object Invoke(Object obj, BindingFlags options, Binder binder, Object[] parameters, CultureInfo culture){
        return this.Invoke(obj, obj, options, binder, parameters, culture);
      }
    
      internal abstract Object Invoke(Object obj, Object thisob, BindingFlags options, Binder binder, Object[] parameters, CultureInfo culture);
  
      public sealed override bool IsDefined(Type type, bool inherit){ 
        return false;
      }

      public override MemberTypes MemberType{
        get{
          return MemberTypes.Method;
        }
      }
      
      public override RuntimeMethodHandle MethodHandle{
        get{
          return this.GetMethodInfo(null).MethodHandle;
        }
      }
      
      public override Type ReflectedType{
        get{
          return this.DeclaringType;
        }
      }
      
      public override ICustomAttributeProvider ReturnTypeCustomAttributes {
        get {
          return null;
        }
      }

    }
}
