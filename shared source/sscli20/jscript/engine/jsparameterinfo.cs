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
    
    internal sealed class JSParameterInfo : ParameterInfo{
      private ParameterInfo parameter;
      private Type parameterType;
      private Object[] attributes = null;
      
      internal JSParameterInfo(ParameterInfo parameter){
        this.parameter = parameter;
      }
    
      public override Object DefaultValue{
        get{
          return TypeReferences.GetDefaultParameterValue(parameter);
        }
      }
    
      public sealed override Object[] GetCustomAttributes(bool inherit){
        Object[] attrs = this.attributes;
        if (attrs != null) return attrs;
        return this.attributes = this.parameter.GetCustomAttributes(true);
      }
    
      public sealed override Object[] GetCustomAttributes(Type type, bool inherit){
        Debug.PreCondition(type == typeof(ParamArrayAttribute));
        Object[] attrs = this.attributes;
        if (attrs != null) return attrs;
        return this.attributes = CustomAttribute.GetCustomAttributes(this.parameter, type, true);
      }

      public sealed override bool IsDefined(Type type, bool inherit){ 
        Debug.PreCondition(type == typeof(ParamArrayAttribute));
        Object[] attrs = this.attributes;
        if (attrs == null)
          this.attributes = attrs = CustomAttribute.GetCustomAttributes(this.parameter, type, true);
        return attrs.Length > 0;
      }
      
      public override String Name{
        get{
          return this.parameter.Name;
        }
      }
      
      public override Type ParameterType{
        get{
          Type result = this.parameterType;
          if (result != null) return result;
          return this.parameterType = this.parameter.ParameterType;
        }
      }
      
    }
}
