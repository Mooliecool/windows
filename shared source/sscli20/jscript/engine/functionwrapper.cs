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
    using System.Diagnostics;
    using System.Globalization;
    using System.Reflection;
    using System.Text;
    using System.Security.Permissions;
    
    public sealed class FunctionWrapper : ScriptFunction{
      private Object obj;
      private MemberInfo[] members;

      internal FunctionWrapper(String name, Object obj, MemberInfo[] members)
        : base(FunctionPrototype.ob, name, 0){
        this.obj = obj;
        this.members = members;
        Debug.Assert(members.Length > 0);
        foreach (MemberInfo mem in members){
          if (mem is MethodInfo){
            this.ilength = ((MethodInfo)mem).GetParameters().Length;
            break;
          }
        }
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
        MethodInfo m = this.members[0] as MethodInfo;
        if (thisob is GlobalScope || thisob == null || m != null && (m.Attributes & MethodAttributes.Static) != 0) 
          thisob = this.obj;
        else if ((!this.obj.GetType().IsInstanceOfType(thisob) && !(this.obj is ClassScope))){
          if (this.members.Length == 1){
            JSWrappedMethod wm = this.members[0] as JSWrappedMethod;
            if (wm != null && wm.DeclaringType == Typeob.Object)
              return LateBinding.CallOneOfTheMembers(new MemberInfo[]{wm.method}, args, false, thisob, binder, culture, null, this.engine);
          }
          throw new JScriptException(JSError.TypeMismatch);
        }
        return LateBinding.CallOneOfTheMembers(this.members, args, false, thisob, binder, culture, null, this.engine);
      }
      
     [PermissionSet(SecurityAction.Demand, Name="FullTrust")]
      internal Delegate ConvertToDelegate(Type delegateType){
        return Delegate.CreateDelegate(delegateType, this.obj, this.name);
      }
      
      public override String ToString(){
        Type t = this.members[0].DeclaringType;
        MethodInfo meth = (t == null ? null :  t.GetMethod(this.name + " source"));
        if (meth == null){
          StringBuilder result = new StringBuilder();
          bool firstTime = true;
          foreach (MemberInfo mem in this.members)
            if (mem is MethodInfo || (mem is PropertyInfo && JSProperty.GetGetMethod((PropertyInfo)mem, false) != null)){
              if (!firstTime)
                result.Append("\n");
              else
                firstTime = false;
              result.Append(mem.ToString());
            }
          if (result.Length > 0) return result.ToString();
          return "function "+this.name+"() {\n    [native code]\n}";
        }
        return (String)meth.Invoke(null, null);
      }
    }
}
