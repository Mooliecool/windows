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
    
    /* 
    This class accumulates members from super types and interfaces and then returns them as an array that is sorted
    so that all members with the same name are contiguous and so that members from more derived classes precede members from less derived classes.
    */
    
    using System;
    using System.Reflection;
    using System.Collections;
    
    public sealed class SuperTypeMembersSorter{
      private SimpleHashtable members;
      private ArrayList names;
      private int count;
      
      internal SuperTypeMembersSorter(){
        this.members = new SimpleHashtable(64);
        this.names = new ArrayList();
        this.count = 0;
      }
      
      //This routine is to be called in order of derivation.
      internal void Add(MemberInfo[] members){
        foreach (MemberInfo member in members)
          this.Add(member);
      }

      internal void Add(MemberInfo member){
        this.count++;
        String name = member.Name;
        Object ob = this.members[name];
        if (ob == null){
          this.members[name] = member;
          this.names.Add(name);
        }else if (ob is MemberInfo){
          ArrayList mems = new ArrayList(8);
          mems.Add(ob);
          mems.Add(member);
          this.members[name] = mems;
        }else
          ((ArrayList)ob).Add(member);
      }
      
      internal Object[] GetMembers(){
        Object[] result = new Object[this.count];
        int i = 0;
        foreach (Object name in this.names){
          Object ob = this.members[name];
          if (ob is MemberInfo)
            result[i++] = ob;
          else{
            foreach (Object m in (ArrayList)ob)
              result[i++] = m;
          }
        }
        return result;
      }
    }
}    
