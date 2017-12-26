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
    
    internal class JSClosureProperty : JSWrappedProperty{
      private MethodInfo getMeth;
      private MethodInfo setMeth;
    
      internal JSClosureProperty(PropertyInfo property, MethodInfo getMeth, MethodInfo setMeth)
        :base(property, null){
        this.getMeth = getMeth;
        this.setMeth = setMeth;
      }

      public override Object GetValue(Object obj, BindingFlags invokeAttr, Binder binder, Object[] index, CultureInfo culture) {
        if (this.getMeth == null)
          throw new MissingMethodException();
        return this.getMeth.Invoke(obj, invokeAttr, binder, index, culture);
      }
      
      public override MethodInfo GetGetMethod(bool nonPublic){
        if (nonPublic || (this.getMeth != null && this.getMeth.IsPublic))
          return this.getMeth;
        else
          return null;
      }
      
      public override MethodInfo GetSetMethod(bool nonPublic){
        if (nonPublic || (this.setMeth != null && this.setMeth.IsPublic))
          return this.setMeth;
        else
          return null;
      }
      
      public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, Object[] index, CultureInfo culture) {
        if (this.setMeth == null)
          throw new MissingMethodException();
        int n = index == null ? 0 : index.Length;
        Object[] pars = new Object[n+1];
        pars[0] = value;
        if (n > 0)
          ArrayObject.Copy(index, 0, pars, 1, n);
        this.setMeth.Invoke(obj, invokeAttr, binder, pars, culture);
      }
    
    }
}
