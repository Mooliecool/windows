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

    internal sealed class Stack{
    
      private Object[] elements;
      private int top;
    
      internal Stack(){
        this.elements = new Object[32];
        this.top = -1;
      }
    
    
      internal void GuardedPush(Object item){
        if (this.top > 500)
          throw new JScriptException(JSError.OutOfStack);
        if (++this.top >= this.elements.Length){
          Object[] newelems = new Object[this.elements.Length+32];
          ArrayObject.Copy(this.elements, newelems, this.elements.Length);
          this.elements = newelems;
        }
        this.elements[this.top] = item;
      }
    
      internal void Push(Object item){
        if (++this.top >= this.elements.Length){
          Object[] newelems = new Object[this.elements.Length+32];
          ArrayObject.Copy(this.elements, newelems, this.elements.Length);
          this.elements = newelems;
        }
        this.elements[this.top] = item;
      }
    
      internal Object Pop(){
        Object result = this.elements[this.top];
        this.elements[this.top--] = null;
        return result;
      }
    
      internal ScriptObject Peek(){
        if (this.top < 0)
          return null;
        else
          return (ScriptObject)this.elements[this.top];
      }
    
      internal Object Peek(int i){
        return this.elements[this.top-i];
      }
    
      
      internal int Size(){
        return this.top+1;
      }
      
      internal void TrimToSize(int i){
        this.top = i-1;
      }
    }
}
