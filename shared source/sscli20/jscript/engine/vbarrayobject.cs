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

    public class VBArrayObject : JSObject{
      private Array array;
     
      public VBArrayObject(VBArrayPrototype parent, Array array)
        : base(parent){
        this.array = array;
        this.noExpando = false;
      }
      
      internal virtual int dimensions(){
        return this.array.Rank;
      }
      
      internal virtual Object getItem(Object[] args){
        if (args == null || args.Length == 0)
          throw new JScriptException(JSError.TooFewParameters);
        if (args.Length == 1)
          return this.array.GetValue(Convert.ToInt32(args[0]));
        if (args.Length == 2)
          return this.array.GetValue(Convert.ToInt32(args[0]), Convert.ToInt32(args[1]));
        if (args.Length == 3)
          return this.array.GetValue(Convert.ToInt32(args[0]), Convert.ToInt32(args[1]), Convert.ToInt32(args[2]));
        int n = args.Length;
        int[] intArgs = new int[n];
        for (int i = 0; i < n; i++)
          intArgs[i] = Convert.ToInt32(args[i]);
        return this.array.GetValue(intArgs);
      }
      
      internal virtual int lbound(Object dimension){
        int dim = Convert.ToInt32(dimension);
        return this.array.GetLowerBound(dim);
      }
      
      internal virtual ArrayObject toArray(VsaEngine engine){
        IList aList = this.array;
        ArrayObject result = engine.GetOriginalArrayConstructor().Construct();
        uint i = 0;
        int n = aList.Count;
        IEnumerator e = aList.GetEnumerator();
        result.length = n;
        while (e.MoveNext())
          result.SetValueAtIndex(i++, e.Current);
        return result;
      }
      
      internal virtual int ubound(Object dimension){
        int dim = Convert.ToInt32(dimension);
        return this.array.GetUpperBound(dim);
      }
      
    }
}
