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

namespace Microsoft.JScript{
    
    using System;
    using System.Collections;
    using System.Globalization;
    using System.Reflection;
    using System.Text;
    
    public sealed class TypedArray : IReflect{
      internal IReflect elementType;
      internal int rank;
      
      public TypedArray(IReflect elementType, int rank){
        this.elementType = elementType;
        this.rank = rank;
      }
      
      public override bool Equals(Object obj){
        if (obj is TypedArray) return this.ToString().Equals(obj.ToString());
        Type t = obj as Type;
        if (t == null) return false;
        if (!t.IsArray) return false;
        if (t.GetArrayRank() != this.rank) return false;
        return this.elementType.Equals(t.GetElementType());
      }
      
      public FieldInfo GetField(String name, BindingFlags bindingAttr){
        return Typeob.Array.GetField(name, bindingAttr);
      }
      
      public FieldInfo[] GetFields(BindingFlags bindingAttr){
        return Typeob.Array.GetFields(bindingAttr);
      }
      
      public override int GetHashCode(){
        return this.ToString().GetHashCode();
      }
      
      public MemberInfo[] GetMember(String name, BindingFlags bindingAttr){
        return Typeob.Array.GetMember(name, bindingAttr);
      }
      
      public MemberInfo[] GetMembers(BindingFlags bindingAttr){
        return Typeob.Array.GetMembers(bindingAttr);
      }
      
      public MethodInfo GetMethod(String name, BindingFlags bindingAttr){
        return Typeob.Array.GetMethod(name, bindingAttr);
      }

      public MethodInfo GetMethod(String name, BindingFlags bindingAttr, Binder binder, Type[] types, ParameterModifier[] modifiers){
        return Typeob.Array.GetMethod(name, bindingAttr, binder, types, modifiers);
      }

      public MethodInfo[] GetMethods(BindingFlags bindingAttr){
        return Typeob.Array.GetMethods(bindingAttr);
      }
          
      public PropertyInfo GetProperty(String name, BindingFlags bindingAttr){
        return Typeob.Array.GetProperty(name, bindingAttr);
      }

      public PropertyInfo GetProperty(String name, BindingFlags bindingAttr, Binder binder, Type returnType, Type[] types, ParameterModifier[] modifiers){
        return Typeob.Array.GetProperty(name, bindingAttr, binder, returnType, types, modifiers);
      }

      public PropertyInfo[] GetProperties(BindingFlags bindingAttr){
        return Typeob.Array.GetProperties(bindingAttr);
      }
      
      public Object InvokeMember(String name, BindingFlags flags, Binder binder, Object target, 
                                 Object[] args, ParameterModifier[] modifiers, CultureInfo locale, String[] namedParameters){
        if ((flags & BindingFlags.CreateInstance) == 0)
          //Try to create an instance of the array
          return LateBinding.CallValue(this.elementType, args, true, true, null, null, binder, locale, namedParameters);
        return Typeob.Array.InvokeMember(name, flags, binder, target, args, modifiers, locale, namedParameters);
      }
      
      static internal String ToRankString(int rank){
        switch(rank){
          case 1 : return "[]";
          case 2 : return "[,]";
          case 3 : return "[,,]";
          default:
            StringBuilder str = new StringBuilder(rank+1);
            str.Append('[');
            for (int i = 1; i < rank; i++) str.Append(',');
            str.Append(']');
            return str.ToString();
        }
      }
      
      public override String ToString(){
        Type elemType = this.elementType as Type;
        if (elemType != null)
          return elemType.FullName + TypedArray.ToRankString(this.rank);
        ClassScope csc = this.elementType as ClassScope;
        if (csc != null)
          return csc.GetFullName() + TypedArray.ToRankString(this.rank);
        TypedArray tarr = this.elementType as TypedArray;
        if (tarr != null)
          return tarr.ToString() + TypedArray.ToRankString(this.rank);
        return Convert.ToType(this.elementType).FullName + TypedArray.ToRankString(this.rank);
      }
      
      internal Type ToType(){
        Type elemType = Convert.ToType(this.elementType);
        return Convert.ToType(TypedArray.ToRankString(this.rank), elemType);
      }
      
      public Type UnderlyingSystemType{
        get{
          return this.GetType();
        } 
      }
    }
}
