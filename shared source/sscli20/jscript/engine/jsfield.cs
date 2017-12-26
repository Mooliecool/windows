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

/* 
A base class for all fields defined by JScript. Provides dummy implementations for stuff were not interested in. 
*/

namespace Microsoft.JScript {
    
    using System;
    using System.Collections;
    using System.Reflection;
    
    public abstract class JSField : FieldInfo{
      
      public override FieldAttributes Attributes{
        get{
          return (FieldAttributes)0;
        }
      }
       
      public override Type DeclaringType{
        get{
          return null;
        }
      }
    
      public override RuntimeFieldHandle FieldHandle{
        get{
          return ((FieldInfo)this.GetMetaData()).FieldHandle;
        }
      }
      
      public override Type FieldType{
        get{
          return Typeob.Object;
        }
      }
      
      public override Object[] GetCustomAttributes(Type t, bool inherit){
        return new FieldInfo[0];
      }

      public override Object[] GetCustomAttributes(bool inherit){
        return new FieldInfo[0];
      }
    
      internal virtual Object GetMetaData(){
        throw new JScriptException(JSError.InternalError);
      }

      internal virtual String GetClassFullName(){
        throw new JScriptException(JSError.InternalError);
      }
            
      internal virtual PackageScope GetPackage(){
        throw new JScriptException(JSError.InternalError);
      }
            
      public override bool IsDefined(Type type, bool inherit){ 
        return false;
      }

      public override MemberTypes MemberType{
        get{
          return MemberTypes.Field;
        }
      }
      
      public override String Name{
        get{
          return "";
        }
      }
    
      public override Type ReflectedType{
        get{
          return this.DeclaringType;
        }
      }
    }
}
