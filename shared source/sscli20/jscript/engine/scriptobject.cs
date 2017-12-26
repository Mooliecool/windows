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
    using System.Diagnostics;
    using System.Globalization;
    using System.Reflection;
    
    [System.Runtime.InteropServices.ComVisible(true)]
    public abstract class ScriptObject : IReflect{
      protected ScriptObject parent;
      
      //Used by JSObject.GetMembers to cache members that must be delegated to other objects
      internal SimpleHashtable wrappedMemberCache; 
      
      public VsaEngine engine; //This is only really useful for ScriptFunctions, IActivation objects and prototype objects. It lives here for the sake of simplicity.
      //Prototype objects do not need the scope stack, so in fast mode, all prototype objects can share a common engine.

      internal ScriptObject(ScriptObject parent){
        this.parent = parent;
        this.wrappedMemberCache = null;
        if (this.parent != null)
          this.engine = parent.engine;
        else
          this.engine = null;
      }

      internal virtual bool DeleteMember(String name){
        return false;
      }

      internal virtual Object GetDefaultValue(PreferredType preferred_type){
        throw new JScriptException(JSError.InternalError);
      }

      public FieldInfo GetField(String name, BindingFlags bindingAttr){
        foreach (MemberInfo member in this.GetMember(name, bindingAttr))
          if (member.MemberType == MemberTypes.Field) return (FieldInfo)member;
        return null;
      }

      public virtual FieldInfo[] GetFields(BindingFlags bindingAttr){
        ArrayObject arr = this as ArrayObject;
        if (arr != null && arr.denseArrayLength > 0){ 
          //Interop fails to see elements unless they are returned in a GetXXXs call. An element in
          //the sparse portion of the array is returned as a FieldInfo. When interop asks for these
          //migrate the elements stored in the dense portion of the array to the sparse portion so that
          //they will be accessible.
          uint i, length;
          length = arr.denseArrayLength;
          if (length > arr.len)
            length = arr.len;
          for (i = 0; i < length; i++){
            Object element = arr.denseArray[i];
            if (element != Missing.Value)
              arr.SetMemberValue2(i.ToString(CultureInfo.InvariantCulture), element); //Implemented on JSObject and bypasses length checking
          }
          arr.denseArrayLength = 0;
          arr.denseArray = null;
        }
      
        MemberInfo[] members = this.GetMembers(bindingAttr);
        if (members == null) return new FieldInfo[0];
        int fieldCount = 0;
        foreach (MemberInfo member in members)
          if (member.MemberType == MemberTypes.Field) fieldCount++;
        FieldInfo[] fields = new FieldInfo[fieldCount];
        fieldCount = 0;
        foreach (MemberInfo member in members)
          if (member.MemberType == MemberTypes.Field) fields[fieldCount++] = (FieldInfo)member;
        return fields;
      }

      public abstract MemberInfo[] GetMember(String name, BindingFlags bindingAttr);

      internal virtual Object GetMemberValue(String name){
        MemberInfo[] members = this.GetMember(name, BindingFlags.Instance|BindingFlags.Public);
        if (members.Length == 0)
          return Missing.Value;
        return LateBinding.GetMemberValue(this, name, LateBinding.SelectMember(members), members);
      }

      public abstract MemberInfo[] GetMembers(BindingFlags bindingAttr);

      public MethodInfo GetMethod(String name, BindingFlags bindingAttr){
        return this.GetMethod(name, bindingAttr, JSBinder.ob, Type.EmptyTypes, null);
      }

      public MethodInfo GetMethod(String name, BindingFlags bindingAttr, Binder binder, Type[] types, ParameterModifier[] modifiers){
        MemberInfo[] members = this.GetMember(name, bindingAttr);
        if (members.Length == 1) return members[0] as MethodInfo;
        int methodCount = 0;
        foreach (MemberInfo member in members)
          if (member.MemberType == MemberTypes.Method) methodCount++;
        if (methodCount == 0) return null;
        MethodInfo[] methods = new MethodInfo[methodCount];
        methodCount = 0;
        foreach (MemberInfo member in members)
          if (member.MemberType == MemberTypes.Method) methods[methodCount++] = (MethodInfo)member;
        if (binder == null) binder = JSBinder.ob;
        return (MethodInfo)binder.SelectMethod(bindingAttr, methods, types, modifiers);
      }

      public virtual MethodInfo[] GetMethods(BindingFlags bindingAttr){
        MemberInfo[] members = this.GetMembers(bindingAttr);
        if (members == null) return new MethodInfo[0];
        int methodCount = 0;
        foreach (MemberInfo member in members)
          if (member.MemberType == MemberTypes.Method) methodCount++;
        MethodInfo[] methods = new MethodInfo[methodCount];
        methodCount = 0;
        foreach (MemberInfo member in members)
          if (member.MemberType == MemberTypes.Method) methods[methodCount++] = (MethodInfo)member;
        return methods;
      }

      //Returns the parent object (the object to which the script object delegates requests for properties/methods it does not implement itself).
      public ScriptObject GetParent(){
        return this.parent;
      }

      internal virtual void GetPropertyEnumerator(ArrayList enums, ArrayList objects){
        MemberInfo[] members = this.GetMembers(BindingFlags.Instance|BindingFlags.Public);
        if (members.Length > 0){
          enums.Add(members.GetEnumerator());
          objects.Add(this);
        }
        ScriptObject parent = this.GetParent();
        if (parent != null)
          parent.GetPropertyEnumerator(enums, objects);
      }

      public PropertyInfo GetProperty(String name, BindingFlags bindingAttr){
        return this.GetProperty(name, bindingAttr, JSBinder.ob, null, Type.EmptyTypes, null);
      }

      public PropertyInfo GetProperty(String name, BindingFlags bindingAttr, Binder binder, Type returnType, Type[] types, ParameterModifier[] modifiers){
        MemberInfo[] members = this.GetMember(name, bindingAttr);
        if (members.Length == 1) return members[0] as PropertyInfo;
        int propertyCount = 0;
        foreach (MemberInfo member in members)
          if (member.MemberType == MemberTypes.Property) propertyCount++;
        if (propertyCount == 0) return null;
        PropertyInfo[] properties = new PropertyInfo[propertyCount];
        propertyCount = 0;
        foreach (MemberInfo member in members)
          if (member.MemberType == MemberTypes.Property) properties[propertyCount++] = (PropertyInfo)member;
        if (binder == null) binder = JSBinder.ob;
        return (PropertyInfo)binder.SelectProperty(bindingAttr, properties, returnType, types, modifiers);
      }

      public virtual PropertyInfo[] GetProperties(BindingFlags bindingAttr){
        MemberInfo[] members = this.GetMembers(bindingAttr);
        if (members == null) return new PropertyInfo[0];
        int propertyCount = 0;
        foreach (MemberInfo member in members)
          if (member.MemberType == MemberTypes.Property) propertyCount++;
        PropertyInfo[] properties = new PropertyInfo[propertyCount];
        propertyCount = 0;
        foreach (MemberInfo member in members)
          if (member.MemberType == MemberTypes.Property) properties[propertyCount++] = (PropertyInfo)member;
        return properties;
      }

      internal virtual Object GetValueAtIndex(uint index){
        return this.GetMemberValue(index.ToString(CultureInfo.CurrentUICulture));
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      public virtual Object InvokeMember(String name, BindingFlags invokeAttr, Binder binder, Object target,
                                         Object[] args, ParameterModifier[] modifiers, CultureInfo locale, String[] namedParameters){
        if (target != this)
          throw new TargetException();

        bool preferredTypeSpecified = name.StartsWith("< JScript-", StringComparison.Ordinal);
        bool dispid0 = (name == null || name == String.Empty || name.Equals("[DISPID=0]") || preferredTypeSpecified);
        
        if ((invokeAttr & BindingFlags.CreateInstance) != 0){
          if ((invokeAttr & (BindingFlags.InvokeMethod|BindingFlags.GetField|BindingFlags.GetProperty|
                BindingFlags.SetField|BindingFlags.SetProperty|BindingFlags.PutDispProperty)) != 0)
            throw new ArgumentException(JScriptException.Localize("Bad binding flags", locale));
          //js: x = new foo() --> dispid0, create
          if (dispid0)
            throw new MissingMethodException();
          
          //js: x = new foo.name() --> dispid0, create
          LateBinding lb = new LateBinding(name, this);
          return lb.Call(binder, args, modifiers, locale, namedParameters, true, false, this.engine);
        }
        
        //According to docs, name == null is only valid for CreateInstance
        if (name == null)
          throw new ArgumentException(JScriptException.Localize("Bad name", locale));

        if ((invokeAttr & (BindingFlags.InvokeMethod|BindingFlags.GetField|BindingFlags.GetProperty)) != 0){
          if ((invokeAttr & (BindingFlags.SetField|BindingFlags.SetProperty|BindingFlags.PutDispProperty)) != 0)
            throw new ArgumentException(JScriptException.Localize("Bad binding flags", locale));
            
          if (dispid0){
            //All callable functions inherit from ScriptFunction which overrides this method to handle
            //the InvokeMethod case.
            
            //js,vbs: x = foo() --> dispid0, invoke
            if ((invokeAttr & (BindingFlags.GetField|BindingFlags.GetProperty)) == 0)
              throw new MissingMethodException();
              
            //js: x = foo --> dispid0, propget; vbs: x = foo --> dispid0, invoke|propget
            if (args == null || args.Length == 0){
              if (this is JSObject || this is GlobalScope || this is ClassScope){
                PreferredType preferredType = PreferredType.Either;
                if (preferredTypeSpecified){
                  if (name.StartsWith("< JScript-Number", StringComparison.Ordinal))
                    preferredType = PreferredType.Number;
                  else if (name.StartsWith("< JScript-String", StringComparison.Ordinal))
                    preferredType = PreferredType.String;
                  else if (name.StartsWith("< JScript-LocaleString", StringComparison.Ordinal))
                    preferredType = PreferredType.LocaleString;
                }
                return this.GetDefaultValue(preferredType);
              }
              throw new MissingFieldException();
            }
            
            //We support indexed properties with exactly one index on all script objects.
            
            //js,vbs: x = foo(1,2) --> dispid0, invoke|propget
            if (args.Length > 1) 
              throw new ArgumentException(JScriptException.Localize("Too many arguments", locale));
              
            //js,vbs: x = foo(1) --> dispid0, invoke|propget
            Object val = args[0];
            if (val is Int32)
              return this[(int)val];
            IConvertible ic = Convert.GetIConvertible(val);
            if (ic != null && Convert.IsPrimitiveNumericTypeCode(ic.GetTypeCode())){
              double d = ic.ToDouble(null);
              if (d >= 0 && d <= Int32.MaxValue && d == System.Math.Round(d))
                return this[(int)d];
            }
            return this[Convert.ToString(val)];
          }
            
          //If no arguments are supplied, prefer GetXXXX rather than Invoke.
          //js: x = foo.bar --> name="bar", propget; vbs: x = foo.bar --> name="bar", propget|invoke
          if ((args == null || args.Length == 0) && (invokeAttr & (BindingFlags.GetField|BindingFlags.GetProperty)) != 0){
            Object member = this.GetMemberValue(name);
            if (member != Missing.Value)
              return member;
            
            //js: x = foo.bar --> name="bar", propget
            if ((invokeAttr & BindingFlags.InvokeMethod) == 0)
              throw new MissingFieldException();
          }
          
          //Use LateBinding to call because arguments have been supplied.
          //vbs: x = foo.bar --> name="bar", propget|invoke
          //js,vbs: x = foo.bar() --> name="bar", invoke
          //js,vbs: x = foo.bar(1) --> name="bar", invoke|propget
          LateBinding lb = new LateBinding(name, this);
          return lb.Call(binder, args, modifiers, locale, namedParameters, false, false, this.engine);
        }

        if ((invokeAttr & (BindingFlags.SetField | BindingFlags.SetProperty | BindingFlags.PutDispProperty)) != 0){
          if (dispid0){
            if (args == null || args.Length < 2)
              throw new ArgumentException(JScriptException.Localize("Too few arguments", locale));
            else if (args.Length > 2)
              throw new ArgumentException(JScriptException.Localize("Too many arguments", locale));
            Object val = args[0];
            if (val is Int32){
              this[(int)val] = args[1];
              return null;
            }
            IConvertible ic = Convert.GetIConvertible(val);
            if (ic != null && Convert.IsPrimitiveNumericTypeCode(ic.GetTypeCode())){
              double d = ic.ToDouble(null);
              if (d >= 0 && d <= Int32.MaxValue && d == System.Math.Round(d)){
                this[(int)d] = args[1];
                return null;
              }
            }
            this[Convert.ToString(val)] = args[1];
            return null;
          }
          if (args == null || args.Length < 1)
            throw new ArgumentException(JScriptException.Localize("Too few arguments", locale));
          else if (args.Length > 1)
            throw new ArgumentException(JScriptException.Localize("Too many arguments", locale));
          this.SetMemberValue(name, args[0]);
          return null;
        }
                
        throw new ArgumentException(JScriptException.Localize("Bad binding flags", locale));
      }

      //This routine combines a GetField/GetProperty (with expando) and a subsequent SetValue operation.
      //It is used when the Field/Property cannot be cached. It is overridden by JSObject and GlobalScope to provide expando
      internal virtual void SetMemberValue(String name, Object value){
        MemberInfo[] members = this.GetMember(name, BindingFlags.Instance|BindingFlags.Public);
        LateBinding.SetMemberValue(this, name, value, LateBinding.SelectMember(members), members);
      }

      internal void SetParent(ScriptObject parent){
        this.parent = parent;
        if (parent != null)
          this.engine = parent.engine;
      }

      //This routine combines a GetField/GetProperty and a subsequent SetValue operation.
      //It is used when the Field/Property cannot be cached.
      //It has the advantage that Arrays need never create a JSField object to access array elements.
      //Also, it avoids the need to convert the index into a string.
      internal virtual void SetValueAtIndex(uint index, Object value){
        this.SetMemberValue(index.ToString(CultureInfo.InvariantCulture), value);
      }

      public Object this[double index]{
        get{
          if (this == null) throw new JScriptException(JSError.ObjectExpected);
          Object result;
          if (index >= 0 && index <= UInt32.MaxValue && index == System.Math.Round(index))
            result = this.GetValueAtIndex((uint)index);
          else
            result = this.GetMemberValue(Convert.ToString(index));
          return result is Missing ? null : result;
        }
        set{
          if (index >= 0 && index <= UInt32.MaxValue && index == System.Math.Round(index))
            this.SetValueAtIndex((uint)index, value);
          else
            this.SetMemberValue(Convert.ToString(index), value);
        }
      }

      public Object this[int index]{
        get{
          if (this == null) throw new JScriptException(JSError.ObjectExpected);
          Object result;
          if (index >= 0)
            result = this.GetValueAtIndex((uint)index);
          else
            result = this.GetMemberValue(Convert.ToString(index));
          return result is Missing ? null : result;
        }
        set{
          if (this == null) throw new JScriptException(JSError.ObjectExpected);
          if (index >= 0)
            this.SetValueAtIndex((uint)index, value);
          else
            this.SetMemberValue(Convert.ToString(index), value);
        }
      }

      public Object this[String name]{
        get{
          if (this == null) throw new JScriptException(JSError.ObjectExpected);
          Object result = this.GetMemberValue(name);
          return result is Missing ? null : result;
        }
        set{
          if (this == null) throw new JScriptException(JSError.ObjectExpected);
          this.SetMemberValue(name, value);
        }
      }

      public Object this[params Object[] pars]{
        get{
          int n = pars.Length;
          if (n == 0)
            if (this is ScriptFunction || this == null)
              throw new JScriptException(JSError.FunctionExpected);
            else
              throw new JScriptException(JSError.TooFewParameters);
          if (this == null) throw new JScriptException(JSError.ObjectExpected);
          Object val = pars[n-1];
          if (val is Int32)
            return this[(int)val];
          IConvertible ic = Convert.GetIConvertible(val);
          if (ic != null && Convert.IsPrimitiveNumericTypeCode(ic.GetTypeCode())){
            double d = ic.ToDouble(null);
            if (d >= 0 && d <= Int32.MaxValue && d == System.Math.Round(d))
              return this[(int)d];
          }
          return this[Convert.ToString(val)];
        }
        set{
          int n = pars.Length;
          if (n == 0)
            if (this == null)
              throw new JScriptException(JSError.FunctionExpected);
            else if (this is ScriptFunction)
              throw new JScriptException(JSError.CannotAssignToFunctionResult);
            else
              throw new JScriptException(JSError.TooFewParameters);
          if (this == null) throw new JScriptException(JSError.ObjectExpected);
          Object val = pars[n-1];
          if (val is Int32){
            this[(int)val] = value;
            return;
          }
          IConvertible ic = Convert.GetIConvertible(val);
          if (ic != null && Convert.IsPrimitiveNumericTypeCode(ic.GetTypeCode())){
            double d = ic.ToDouble(null);
            if (d >= 0 && d <= Int32.MaxValue && d == System.Math.Round(d)){
              this[(int)d] = value;
              return;
            }
          }
          this[Convert.ToString(val)] = value;
        }
      }

      public virtual Type UnderlyingSystemType{
        get{
          return this.GetType();
        }
      }

      protected static MemberInfo[] WrapMembers(MemberInfo[] members, Object obj){
        if (members == null) return null;
        int n = members.Length;
        if (n == 0) return members;
        MemberInfo[] result = new MemberInfo[n];
        for (int i = 0; i < n; i++)
          result[i] = ScriptObject.WrapMember(members[i], obj);
        return result;
      }

      protected static MemberInfo[] WrapMembers(MemberInfo member, Object obj){
        MemberInfo[] result = new MemberInfo[1];
        result[0] = ScriptObject.WrapMember(member, obj);
        return result;
      }

      protected static MemberInfo[] WrapMembers(MemberInfo[] members, Object obj, SimpleHashtable cache){
        if (members == null) return null;
        int n = members.Length;
        if (n == 0) return members;
        MemberInfo[] result = new MemberInfo[n];
        for (int i = 0; i < n; i++){
          MemberInfo wrappedMember = (MemberInfo)cache[members[i]];
          if (null == wrappedMember){
            wrappedMember = ScriptObject.WrapMember(members[i], obj);
            cache[members[i]] = wrappedMember;
          }
          result[i] = wrappedMember;
        }
        return result;
      }
      
      internal static MemberInfo WrapMember(MemberInfo member, Object obj){
        switch (member.MemberType){
          case MemberTypes.Field:
            FieldInfo field = (FieldInfo)member;
            if (field.IsStatic || field.IsLiteral)
              return field;
            else if (!(field is JSWrappedField))
              return new JSWrappedField(field, obj);
            else
              return field;
          case MemberTypes.Method:
            MethodInfo method = (MethodInfo)member;
            if (method.IsStatic)
              return method;
            else if (!(method is JSWrappedMethod))
              return new JSWrappedMethod(method, obj);
            else
              return method;
          case MemberTypes.Property:
            PropertyInfo property = (PropertyInfo)member;
            if (property is JSWrappedProperty)
              return property;
            MethodInfo getMethod = JSProperty.GetGetMethod(property, true);
            MethodInfo setMethod = JSProperty.GetSetMethod(property, true);
            if ((getMethod == null || getMethod.IsStatic) && (setMethod == null || setMethod.IsStatic))
              return property;
            else
              return new JSWrappedProperty(property, obj);
          default:
            return member;
        }
      }
    }
}
