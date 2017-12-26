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
    
    public sealed class MemberInfoList{
      internal int count;
      private MemberInfo[] list;
    
      internal MemberInfoList(){
        this.count = 0;
        this.list = new MemberInfo[16];
      }
    
      internal void Add(MemberInfo elem){
        int n = this.count++;
        if (this.list.Length == n) this.Grow();
        this.list[n] = elem;
      }
       
      internal void AddRange(MemberInfo[] elems){
        foreach (MemberInfo mem in elems)
          this.Add(mem);
      }
      
      private void Grow(){
        MemberInfo[] list = this.list;
        int n = list.Length;
        MemberInfo[] newList = this.list = new MemberInfo[n+16];
        for (int i = 0; i < n; i++)
          newList[i] = list[i];
      }
      
      internal MemberInfo this[int i]{
        get{
          return this.list[i];
        }
        set{
          this.list[i] = value;
        }
      }
      
      internal MemberInfo[] ToArray(){
        int n = this.count;
        MemberInfo[] result = new MemberInfo[n];
        MemberInfo[] list = this.list;
        for (int i = 0; i < n; i++) result[i] = list[i];
        return result;
      }
    }

}
