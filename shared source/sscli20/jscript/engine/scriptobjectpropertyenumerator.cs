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
    using System.Reflection;
    
    internal class ScriptObjectPropertyEnumerator : IEnumerator{
      private ArrayList enumerators;
      private ArrayList objects;
      private int index;
      private SimpleHashtable visited_names;
      
      internal ScriptObjectPropertyEnumerator(ScriptObject obj){
        obj.GetPropertyEnumerator(this.enumerators = new ArrayList(), this.objects = new ArrayList());
        this.index = 0;
        this.visited_names = new SimpleHashtable(16);
      }
      
      public virtual bool MoveNext(){
        if (this.index >= this.enumerators.Count)
          return false;
        IEnumerator ienum = (IEnumerator)this.enumerators[this.index];
        bool result = ienum.MoveNext();
        if (!result){
          this.index++; return this.MoveNext();
        }
        Object ob = ienum.Current;
        String pname;
        FieldInfo field = ob as FieldInfo;
        if (field != null){
          JSPrototypeField pfield = ob as JSPrototypeField;
          if (pfield != null && pfield.value is Missing)
            return this.MoveNext();
          pname = field.Name;
          Object val = field.GetValue(this.objects[this.index]);
          if (val is Missing)
            return this.MoveNext();
        }else if (ob is String)
          pname = (String)ob;
        else if (ob is MemberInfo)
          pname = ((MemberInfo)ob).Name;
        else
          pname = ob.ToString();
        if (this.visited_names[pname] != null)
          return this.MoveNext();
        this.visited_names[pname] = pname;
        return true;
      }
      
      public virtual Object Current{
        get{
          Object ob = ((IEnumerator)this.enumerators[this.index]).Current;
          if (ob is MemberInfo)
            return ((MemberInfo)ob).Name;
          else
            return ob.ToString();
        }
      }

      public virtual void Reset(){
        this.index = 0;
        foreach(System.Collections.IEnumerator e in this.enumerators)
          e.Reset();
        this.visited_names = new SimpleHashtable(16);
      }
    }
}
