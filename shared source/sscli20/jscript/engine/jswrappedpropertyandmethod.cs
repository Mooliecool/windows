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
    using System.Globalization;

    internal class JSWrappedPropertyAndMethod : JSWrappedProperty {
      protected MethodInfo method;
      private ParameterInfo[] parameters;

      internal JSWrappedPropertyAndMethod(PropertyInfo property, MethodInfo method, Object obj)
        : base(property, obj) {
        this.method = method;
        this.parameters = method.GetParameters();
      }

      private Object[] CheckArguments(Object[] arguments){
        if (arguments == null)
          return arguments;
        int cArguments = arguments.Length;
        int cParameters = this.parameters.Length;
        if (cArguments >= cParameters)
          return arguments;
        Object[] newArgs = new Object[cParameters];
        ArrayObject.Copy(arguments, newArgs, cArguments);
        for (int i = cArguments; i < cParameters; i++)
          newArgs[i] = System.Type.Missing;
        return newArgs;
      }

      internal Object Invoke(Object obj, BindingFlags options, Binder binder, Object[] parameters, CultureInfo culture){
        parameters = CheckArguments(parameters);
        if (this.obj != null && !(this.obj is Type))
          obj = this.obj;
        return this.method.Invoke(obj, options, binder, parameters, culture);
      }

      public MethodInfo Method{
        get{
          return this.method;
        }
      }
    }
}
