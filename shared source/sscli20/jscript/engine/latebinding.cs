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
    using System.Diagnostics;
    using System.Collections;
    using System.Globalization;
    using System.Reflection;
    using System.Runtime.InteropServices;
    using System.Runtime.InteropServices.Expando;

    public sealed class LateBinding{
      private Object last_ir;
      internal MemberInfo last_member;
      internal MemberInfo[] last_members;
      internal Object last_object;
      private String name; //If this is null, the default indexed property should be called
      public Object obj;
      private bool checkForDebugger;

      public LateBinding(String name)
        : this(name, null, false){
      }

      public LateBinding(String name, Object obj)
        : this(name, obj, false){
      }

      internal LateBinding(String name, Object obj, bool checkForDebugger){
        this.last_ir = null;
        this.last_member = null;
        this.last_members = null;
        this.last_object = null;
        this.name = name;
        this.obj = obj;
        this.checkForDebugger = checkForDebugger;
      }

      internal MemberInfo BindToMember(){
        Debug.Assert(this.obj != null && this.name != null);
        if (this.obj == this.last_object && this.last_member != null)
          return this.last_member;
        BindingFlags flags = BindingFlags.Instance|BindingFlags.Public;
        Object obj = this.obj;
        Type t = obj.GetType();
        TypeReflector tr = TypeReflector.GetTypeReflectorFor(t);
        IReflect ir = null;
        if (tr.ImplementsIReflect()){
          ir = obj as ScriptObject;
          if (ir != null){
            if (obj is ClassScope)
              flags = BindingFlags.Static|BindingFlags.Public;
          }else{
            ir = obj as Type;
            if (ir != null)
              flags = BindingFlags.Static|BindingFlags.Public;
            else
              ir = (IReflect)obj;
          }
        }else
          ir = tr;
        this.last_object = this.obj;
        this.last_ir = ir;
        MemberInfo[] members = this.last_members = ir.GetMember(this.name, flags);
        this.last_member = LateBinding.SelectMember(members);
        if (this.obj is Type){ //Add instance members of type Type to the member list
          MemberInfo[] tmembers = typeof(Type).GetMember(this.name, BindingFlags.Instance|BindingFlags.Public);
          int n = 0, m = 0;
          if (tmembers != null && (n = tmembers.Length) > 0){
            if (members == null || (m = members.Length) == 0)
              this.last_member = LateBinding.SelectMember(this.last_members = tmembers);
            else{
              MemberInfo[] jmembers = new MemberInfo[n + m];
              ArrayObject.Copy(members, 0, jmembers, 0, m);
              ArrayObject.Copy(tmembers, 0, jmembers, m, n);
              this.last_member = LateBinding.SelectMember(this.last_members = jmembers);
            }
          }
        }
        return this.last_member;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      public Object Call(Object[] arguments, bool construct, bool brackets, VsaEngine engine){
        try{
          if (this.name == null)
            //Happens when (non simple expr)(args) is evaluated from Eval or the Debugger
            return LateBinding.CallValue(this.obj, arguments, construct, brackets, engine,
              ((IActivationObject)engine.ScriptObjectStackTop()).GetDefaultThisObject(), JSBinder.ob, null, null);
          else
            return this.Call(JSBinder.ob, arguments, null, null, null, construct, brackets, engine);
        }catch(TargetInvocationException e){
          throw e.InnerException;
        }
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal Object Call(Binder binder, Object[] arguments, ParameterModifier[] modifiers, CultureInfo culture,
                           String[] namedParameters, bool construct, bool brackets, VsaEngine engine){
        MemberInfo member = this.BindToMember(); //Do a GetMember call and remember the result of the lookup for next time around


        //On ScriptObjects, fields/properties pre-empt everything else. Call whatever is the value of the field/property. If it is junk, throw an exception.
        if (this.obj is ScriptObject || this.obj is GlobalObject){
          //Handle WithObjects that wrap COMObjects specially
          if (this.obj is WithObject){
            Object wob = ((WithObject)this.obj).contained_object;
            if (!(wob is ScriptObject)){
              IReflect irw = LateBinding.GetIRForObjectThatRequiresInvokeMember(wob, VsaEngine.executeForJSEE);
              if (irw != null)
                return LateBinding.CallCOMObject(irw, this.name, wob, binder, arguments, modifiers, culture, namedParameters, construct, brackets, engine);
            }
          }
          
          if (member is FieldInfo)
            return LateBinding.CallValue(((FieldInfo)member).GetValue(this.obj), arguments, construct, brackets, engine, this.obj, JSBinder.ob, null, null);
          else if (member is PropertyInfo && !(member is JSProperty)){ //JSProperty comes up when running the evaluator and using proper properties.
            if (!brackets){
              JSWrappedPropertyAndMethod propAndMethod = member as JSWrappedPropertyAndMethod;
              if (propAndMethod != null){
                BindingFlags flags = arguments == null || arguments.Length == 0
                  ? BindingFlags.InvokeMethod
                  : BindingFlags.InvokeMethod | BindingFlags.GetProperty;
                return propAndMethod.Invoke(this.obj, flags, JSBinder.ob, arguments, null);
              }
            }
            return LateBinding.CallValue(JSProperty.GetValue((PropertyInfo)member, this.obj, null), arguments, construct, brackets, engine, this.obj, JSBinder.ob, null, null);
          }
          //The code paths below happen because Lookup and Member set last_member. Without the cache, BindToMember will not deliver a MethodInfo.
          else if (member is MethodInfo){
            if (member is JSMethod)
              if (construct)
                return ((JSMethod)member).Construct(arguments);
              else
                return ((JSMethod)member).Invoke(this.obj, this.obj, (BindingFlags)0, JSBinder.ob, arguments, null);
            else{
              Type dt = member.DeclaringType;
              if (dt == typeof(Object))
                return LateBinding.CallMethod((MethodInfo)member, arguments, this.obj, binder, culture, namedParameters);
              else if (dt == typeof(String))
                return LateBinding.CallMethod((MethodInfo)member, arguments, Convert.ToString(this.obj), binder, culture, namedParameters);
              else if (Convert.IsPrimitiveNumericType(dt))
                return LateBinding.CallMethod((MethodInfo)member, arguments, Convert.CoerceT(this.obj, dt), binder, culture, namedParameters);
              else if (dt == typeof(Boolean))
                return LateBinding.CallMethod((MethodInfo)member, arguments, Convert.ToBoolean(this.obj), binder, culture, namedParameters);
              else if (dt == typeof(StringObject) || dt == typeof(BooleanObject) || dt == typeof(NumberObject) || brackets)
                return LateBinding.CallMethod((MethodInfo)member, arguments, Convert.ToObject(this.obj, engine), binder, culture, namedParameters);
              else if (dt == typeof(GlobalObject) && ((MethodInfo)member).IsSpecialName)
                return LateBinding.CallValue(((MethodInfo)member).Invoke(this.obj, null), arguments, construct, false, engine, this.obj, JSBinder.ob, null, null);
              else if (!(this.obj is ClassScope)){
                if (CustomAttribute.IsDefined(member, typeof(JSFunctionAttribute), false)){
                  FieldInfo f = LateBinding.SelectMember(this.last_members) as FieldInfo;
                  if (f != null){ //Field holds a closure, call the closure
                    Object obj = this.obj;
                    if (!(obj is Closure))
                      obj = f.GetValue(this.obj);
                    return LateBinding.CallValue(obj, arguments, construct, brackets, engine, this.obj, JSBinder.ob, null, null);
                  }
                }
                return LateBinding.CallValue(new BuiltinFunction(this.obj, (MethodInfo)member), arguments, construct, false, engine, this.obj, JSBinder.ob, null, null);
              }
            }
          }
        }

        MethodInfo meth = member as MethodInfo;
        if (meth != null) //Inside the evaluator, and dealing with an early bound method
          //Get here because of EvaluateAsLateBinding in Lookup and Member. BindToMember will not have set this.member to be a method
          return LateBinding.CallMethod(meth, arguments, this.obj, binder, culture, namedParameters);
        JSConstructor jscons = member as JSConstructor;
        if (jscons != null) //Inside the evaluator, and dealing with an early bound constructor
          return LateBinding.CallValue(jscons.cons, arguments, construct, brackets, engine, this.obj, JSBinder.ob, null, null);
        if (member is Type) //Inside the evaluator and dealing with early bound conversion
          return LateBinding.CallValue(member, arguments, construct, brackets, engine, this.obj, JSBinder.ob, null, null);
        if (member is ConstructorInfo) //Inside the evaluator, and dealing with an early bound constructor
          return LateBinding.CallOneOfTheMembers(new MemberInfo[]{this.last_member}, arguments, true, this.obj, binder, culture, namedParameters, engine);

        if (!construct && member is PropertyInfo){
          //Might be a parameterless property that results in an object that has a default method or default indexed property that can be called with the params
          //Need to special case this, otherwise the next if will end up calling the parameterless property getter, discarding the parameters
          if (((PropertyInfo)member).GetIndexParameters().Length == 0){
            Type rtype = ((PropertyInfo)member).PropertyType;
            if (rtype == typeof(Object)){
              MethodInfo getter = JSProperty.GetGetMethod((PropertyInfo)member, false);
              if (getter != null){
                Object ob = getter.Invoke(this.obj, null);
                return LateBinding.CallValue(ob, arguments, construct, brackets, engine, this.obj, JSBinder.ob, null, null);
              }
            }
            MemberInfo[] defaultMembers = TypeReflector.GetTypeReflectorFor(rtype).GetDefaultMembers();
            if (defaultMembers != null && defaultMembers.Length > 0){
              MethodInfo getter = JSProperty.GetGetMethod((PropertyInfo)member, false);
              if (getter != null){
                Object ob = getter.Invoke(this.obj, null);
                return LateBinding.CallOneOfTheMembers(defaultMembers, arguments, false, ob, binder, culture, namedParameters, engine);
              }
            }
          }
        }

        //Otherwise, if there are members, we give preference to constructors, methods and properties with matching formal parameter lists.
        if (this.last_members != null && this.last_members.Length > 0)
        {
          bool memberCalled;
          object retval = LateBinding.CallOneOfTheMembers(this.last_members, arguments, construct, this.obj, binder, culture, namedParameters, engine, out memberCalled);
          if (memberCalled)
            return retval;
          //Fall through if no suitable constructor or method or property getter because the value of (this.obj).(this.name) might contain a callable thing.
        }

        //If the object is an IDispatch(Ex) use InvokeMember to do the call
        //In all other cases we can assume that BindToMember has already found all the members that are to be found.
        IReflect ir = LateBinding.GetIRForObjectThatRequiresInvokeMember(this.obj, VsaEngine.executeForJSEE);
        if (ir != null)
          return LateBinding.CallCOMObject(ir, this.name, this.obj, binder, arguments, modifiers, culture, namedParameters, construct, brackets, engine);

        //If the object has a field or parameterless property called this.name, we have to try and call the value of that field or property
        Object value = LateBinding.GetMemberValue(this.obj, this.name, this.last_member, this.last_members);
        if (!(value is Missing))
          return LateBinding.CallValue(value, arguments, construct, brackets, engine, this.obj, JSBinder.ob, null, null);

        //Give up
        if (brackets)
          if (this.obj is IActivationObject)
            throw new JScriptException(JSError.ObjectExpected);
          else
            throw new JScriptException(JSError.OLENoPropOrMethod);
        else
          throw new JScriptException(JSError.FunctionExpected);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      private static Object CallCOMObject(IReflect ir, String name, Object ob, Binder binder, Object[] arguments,
                                          ParameterModifier[] modifiers, CultureInfo culture, String[] namedParameters,
                                          bool construct, bool brackets, VsaEngine engine){
        try{
          try{
            LateBinding.Change64bitIntegersToDouble(arguments);
            BindingFlags flags =
              BindingFlags.Public|BindingFlags.Instance|BindingFlags.Static|BindingFlags.OptionalParamBinding;
            if (construct)
              return ir.InvokeMember(name, flags|BindingFlags.CreateInstance, binder, ob, arguments, modifiers, culture, namedParameters);
            else if (brackets){
              // Expression like ob.name[args]. First try to invoke as an indexed property. If that
              // results in a ArgumentException, try to evaluate (ob.name) and then invoke
              // the default indexed property on the result.
              try {
                return ir.InvokeMember(name, flags|BindingFlags.GetProperty|BindingFlags.GetField, binder, ob, arguments, modifiers, culture, namedParameters);
              }catch(TargetInvocationException){
                Object ob1 = ir.InvokeMember(name, flags|BindingFlags.GetProperty|BindingFlags.GetField, binder, ob, new Object[0], modifiers, culture, new String[0]);
                return LateBinding.CallValue(ob1, arguments, construct, brackets, engine, ob1, binder, culture, namedParameters);
              }
            }else{
              int length = arguments == null ? 0 : arguments.Length;
              if (namedParameters != null && namedParameters.Length > 0 && (namedParameters[0].Equals("[DISPID=-613]") || namedParameters[0].Equals("this")))
                length--;
              flags |= length > 0
                ? BindingFlags.InvokeMethod | BindingFlags.GetProperty
                : BindingFlags.InvokeMethod;
              return ir.InvokeMember(name, flags, binder, ob, arguments, modifiers, culture, namedParameters);
            }
          }catch(MissingMemberException){
            if (brackets) //ob["x"]. Tried ob.Item()["x"] but failed. So return undefined.
              return null;
            else //ob("x"). Tried ob.Item()("x") but failed. Complain that ob is not a callable thing.
              throw new JScriptException(JSError.FunctionExpected);
          }catch(COMException e){
            int code = e.ErrorCode;
            if (((uint)code) == 0x80020006 || ((uint)code) == 0x80020003) //Unknown name or Member not found
              if (brackets)
                return null;
              else
                throw new JScriptException(JSError.FunctionExpected);
            if ((code & 0xFFFF0000) == 0x800A0000){
              String source = e.Source;
              if (source != null && source.IndexOf("JScript") != -1)
                throw new JScriptException(e, null);
            }
            throw e;
          }
        }catch(JScriptException e){
          if ((e.Number & 0xFFFF) == (int)JSError.FunctionExpected){
            //try to find a method on System.Object
            MemberInfo[] members = typeof(Object).GetMember(name, BindingFlags.Public|BindingFlags.Instance);
            if (members != null && members.Length > 0)
              return LateBinding.CallOneOfTheMembers(members, arguments, construct, ob, binder, culture, namedParameters, engine);
          }
          throw e;
        }
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      private static Object CallMethod(MethodInfo method, Object[] arguments, Object thisob, Binder binder, CultureInfo culture, String[] namedParameters){
        if (namedParameters != null && namedParameters.Length > 0)
          if (arguments.Length >= namedParameters.Length)
            arguments = JSBinder.ArrangeNamedArguments(method, arguments, namedParameters);
          else
            throw new JScriptException(JSError.MoreNamedParametersThanArguments);
        Object[] newargs = LateBinding.LickArgumentsIntoShape(method.GetParameters(), arguments, binder, culture);
        try{
          Object result = method.Invoke(thisob, BindingFlags.SuppressChangeType, null, newargs, null);
          if (newargs != arguments && newargs != null && arguments != null){
            //Copy elements of newargs back to arguments, in case any of them were passed by reference
            int n = arguments.Length;
            int m = newargs.Length;
            if (m < n) n = m;
            for (int i = 0; i < n; i++) arguments[i] = newargs[i];
          }
          return result;
        }catch(TargetException e){
          ClassScope csc = thisob as ClassScope;
          if (csc != null) return csc.FakeCallToTypeMethod(method, newargs, e);
          throw;
        }
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static Object CallOneOfTheMembers(MemberInfo[] members, Object[] arguments, bool construct, Object thisob, Binder binder, CultureInfo culture, String[] namedParameters, VsaEngine engine){
        bool memberCalled;
        object retval = LateBinding.CallOneOfTheMembers(members, arguments, construct, thisob, binder, culture, namedParameters, engine, out memberCalled);
        if (!memberCalled)
          throw new MissingMemberException();
        return retval;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static Object CallOneOfTheMembers(MemberInfo[] members, Object[] arguments, bool construct, Object thisob, Binder binder, CultureInfo culture, String[] namedParameters, VsaEngine engine, out bool memberCalled){
        //Note that SelectConstructor and SelectMethod take care of named parameters
        memberCalled = true;
        if (construct){
          ConstructorInfo cons = JSBinder.SelectConstructor(Runtime.TypeRefs, members, ref arguments, namedParameters);
          if (cons != null){
            if (CustomAttribute.IsDefined(cons, typeof(JSFunctionAttribute), false)){
              if (thisob is StackFrame) thisob = ((StackFrame)thisob).closureInstance;
              int n = arguments.Length;
              Object[] newArguments = new Object[n+1];
              ArrayObject.Copy(arguments, 0, newArguments, 0, n);
              newArguments[n] = thisob;
              arguments = newArguments;
            }
            Object result = null;
            JSConstructor jscons = cons as JSConstructor;
            if (jscons != null)
              result = jscons.Construct(thisob, LateBinding.LickArgumentsIntoShape(cons.GetParameters(), arguments, JSBinder.ob, culture));
            else
              result = cons.Invoke(BindingFlags.SuppressChangeType, null,
                LateBinding.LickArgumentsIntoShape(cons.GetParameters(), arguments, JSBinder.ob, culture), null);
            if (result is INeedEngine)
              ((INeedEngine)result).SetEngine(engine);
            return result;
          }
        }else{
          Object[] originalArguments = arguments;
          MethodInfo meth = JSBinder.SelectMethod(Runtime.TypeRefs, members, ref arguments, namedParameters);
          if (meth != null) {
            if (meth is JSMethod )
              return ((JSMethod)meth).Invoke(thisob, thisob, (BindingFlags)0, JSBinder.ob, arguments, null);
            if (CustomAttribute.IsDefined(meth, typeof(JSFunctionAttribute), false)){ //Dealing with a builtin function
              if (!construct){
                JSBuiltin biFunc = ((JSFunctionAttribute)CustomAttribute.GetCustomAttributes(meth, typeof(JSFunctionAttribute), false)[0]).builtinFunction;
                if (biFunc != 0){
                  IActivationObject iaob = thisob as IActivationObject;
                  if (iaob != null) thisob = iaob.GetDefaultThisObject();
                  return BuiltinFunction.QuickCall(arguments, thisob, biFunc, null, engine);
                }
              }
              return LateBinding.CallValue(new BuiltinFunction(thisob, meth), arguments, construct, false, engine, thisob, JSBinder.ob, null, null);
            }else{
              Object[] args = LateBinding.LickArgumentsIntoShape(meth.GetParameters(), arguments, JSBinder.ob, culture);
              if (thisob != null && !meth.DeclaringType.IsAssignableFrom(thisob.GetType())){
                if (thisob is StringObject)
                  return meth.Invoke(((StringObject)thisob).value, BindingFlags.SuppressChangeType, null, args, null);
                if (thisob is NumberObject)
                  return meth.Invoke(((NumberObject)thisob).value, BindingFlags.SuppressChangeType, null, args, null);
                if (thisob is BooleanObject)
                  return meth.Invoke(((BooleanObject)thisob).value, BindingFlags.SuppressChangeType, null, args, null);
                if (thisob is ArrayWrapper)
                  return meth.Invoke(((ArrayWrapper)thisob).value, BindingFlags.SuppressChangeType, null, args, null);
              }
              Object result = meth.Invoke(thisob, BindingFlags.SuppressChangeType, null, args, null);
              if (args != originalArguments && arguments == originalArguments && args != null && arguments != null){
                //Different because of optional parameters or param arrays. Update the original array in case there were call by reference parameters.
                int n = arguments.Length;
                int m = args.Length;
                if (m < n) n = m;
                for (int i = 0; i < n; i++) arguments[i] = args[i];
              }
              return result;
            }
          }
        }
        memberCalled = false;
        return null;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      public static Object CallValue(Object thisob, Object val, Object[] arguments, bool construct, bool brackets, VsaEngine engine){
        try{
          return LateBinding.CallValue(val, arguments, construct, brackets, engine, thisob, JSBinder.ob, null, null);
        }catch(TargetInvocationException e){
          throw e.InnerException;
        }
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      public static Object CallValue2(Object val, Object thisob, Object[] arguments, bool construct, bool brackets, VsaEngine engine){
        try{
          return LateBinding.CallValue(val, arguments, construct, brackets, engine, thisob, JSBinder.ob, null, null);
        }catch(TargetInvocationException e){
          throw e.InnerException;
        }
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static Object CallValue(Object val, Object[] arguments, bool construct, bool brackets, VsaEngine engine, Object thisob,
                                        Binder binder, CultureInfo culture, String[] namedParameters){
        if (construct){
          if (val is ScriptFunction){
            ScriptFunction fun = (ScriptFunction)val;
            if (!brackets) goto old_style_behavior;
            Object propVal = propVal = fun[arguments];
            if (propVal != null)
              return CallValue(propVal, new Object[0], true, false, engine, thisob, binder, culture, namedParameters);
            Type t = (Type)Runtime.TypeRefs.GetPredefinedType(fun.name);
            if (t != null){ //It is a function that could be used in a type expression
              int n = arguments.Length;
              int[] indices = new int[n];
              n = 0;
              foreach (Object arg in arguments){
                if (arg is Int32){
                  indices[n++] = (int)arg;
                  continue;
                }
                IConvertible ic = Convert.GetIConvertible(arg);
                if (ic == null || !Convert.IsPrimitiveNumericTypeCode(ic.GetTypeCode())) goto old_style_behavior;
                double d = ic.ToDouble(null);
                int i = (int)d;
                if (d != (double)i) goto old_style_behavior;
                indices[n++] = i;
              }
              return System.Array.CreateInstance(t, indices);
            }
          old_style_behavior:
            FunctionObject func = fun as FunctionObject;
            if (func != null){
              return func.Construct(thisob as JSObject, arguments == null ? new Object[0] : arguments);
            }else{
              Object result = fun.Construct(arguments == null ? new Object[0] : arguments);
              JSObject jsob = result as JSObject;
              if (jsob != null) jsob.outer_class_instance = thisob as JSObject;
              return result;
            }
          }else if (val is ClassScope){
            if (brackets)
              return System.Array.CreateInstance(typeof(Object), LateBinding.ToIndices(arguments));
            JSObject result = (JSObject)LateBinding.CallOneOfTheMembers(((ClassScope)val).constructors, arguments, construct,
              thisob, binder, culture, namedParameters, engine);
            result.noExpando = ((ClassScope)val).noExpando;
            return result;
          }else if (val is Type){
            Type t = (Type)val;
            if (brackets) //we have "new val[x, y, z]" where val is a Type, so create an array of val.
              return System.Array.CreateInstance(t, LateBinding.ToIndices(arguments));
            ConstructorInfo[] cons = t.GetConstructors();
            Object result = null;
            if (cons == null || cons.Length == 0) //Happens for interop
              result = Activator.CreateInstance(t, (BindingFlags)0, JSBinder.ob, arguments, null);
            else
              result = LateBinding.CallOneOfTheMembers(cons, arguments, construct, thisob, binder, culture, namedParameters, engine);
            if (result is INeedEngine)
              ((INeedEngine)result).SetEngine(engine);
            return result;
          }else if (val is TypedArray && brackets)
            return System.Array.CreateInstance(typeof(Object), LateBinding.ToIndices(arguments));
        }

        if (brackets){
          ScriptObject scrob = val as ScriptObject;
          if (scrob != null){
            Object v = scrob[arguments];
            if (construct) return LateBinding.CallValue(thisob, v, new Object[0], true, false, engine);
            return v;
          }
        }else{
          if (val is ScriptFunction){
            if (thisob is IActivationObject)
              thisob = ((IActivationObject)thisob).GetDefaultThisObject();
            return ((ScriptFunction)val).Call(arguments == null ? new Object[0] : arguments, thisob, binder, culture);
          }else if (val is Delegate)
            return CallMethod(((Delegate)val).Method, arguments, thisob, binder, culture, namedParameters);
          else if (val is MethodInfo)
            return CallMethod((MethodInfo)val, arguments, thisob, binder, culture, namedParameters);
          else if (val is Type && arguments.Length == 1)
            return Convert.CoerceT(arguments[0], (Type)val, true);
          else if (val is ClassScope){
            if (arguments == null || arguments.Length != 1)
              throw new JScriptException(JSError.FunctionExpected);
            if (((ClassScope)val).HasInstance(arguments[0])) return arguments[0];
            throw new InvalidCastException(null);
          }else if (val is TypedArray && arguments.Length == 1)
            return Convert.Coerce(arguments[0], val, true);
          else if (val is ScriptObject)
            throw new JScriptException(JSError.FunctionExpected);
          else if (val is MemberInfo[])
            return LateBinding.CallOneOfTheMembers((MemberInfo[])val, arguments, construct, thisob, binder, culture, namedParameters, engine);
        }

        if (val != null){
          System.Array arr = val as System.Array;
          if (arr != null){
            if (arguments.Length != arr.Rank)
              throw new JScriptException(JSError.IncorrectNumberOfIndices);
            return arr.GetValue(LateBinding.ToIndices(arguments));
          }
          //See if val has a default indexed property that we can call
          val = Convert.ToObject(val, engine);
          ScriptObject scrob = val as ScriptObject;
          if (scrob != null)
            if (brackets)
              return scrob[arguments];
            else{
              ScriptFunction sf = scrob as ScriptFunction;
              if (sf != null){
                IActivationObject iaob = thisob as IActivationObject;
                if (iaob != null) thisob = iaob.GetDefaultThisObject();
                return sf.Call(arguments == null ? new Object[0] : arguments, thisob, binder, culture);
              }
              throw new JScriptException(JSError.InvalidCall);
            }
          IReflect ir = LateBinding.GetIRForObjectThatRequiresInvokeMember(val, VsaEngine.executeForJSEE);
          if (ir != null){
            if (brackets){
              String name = String.Empty;
              int n = arguments.Length;
              if (n > 0)
                name = Convert.ToString(arguments[n-1]);
              return CallCOMObject(ir, name, val, binder, null, null, culture, namedParameters, false, true, engine);
            }

            if (!(val is IReflect))
              return CallCOMObject(ir, String.Empty, val, binder, arguments, null, culture, namedParameters, false, brackets, engine);

            // Pass in the 'this' object as an additional named parameter
            Object[] arguments2 = new Object[(arguments != null ? arguments.Length : 0) + 1];
            arguments2[0] = thisob;
            if (arguments != null)
              ArrayObject.Copy(arguments, 0, arguments2, 1, arguments.Length);
            String[] namedParameters2 = new String[(namedParameters != null ? namedParameters.Length : 0) + 1];
            namedParameters2[0] = "[DISPID=-613]";
            if (namedParameters != null)
              ArrayObject.Copy(namedParameters, 0, namedParameters2, 1, namedParameters.Length);
            return CallCOMObject(ir, "[DISPID=0]", val, binder, arguments2, null, culture, namedParameters2, false, brackets, engine);
          }


          MemberInfo[] defaultMembers = TypeReflector.GetTypeReflectorFor(val.GetType()).GetDefaultMembers();
          if (defaultMembers != null && defaultMembers.Length > 0){
            MethodInfo meth = JSBinder.SelectMethod(Runtime.TypeRefs, defaultMembers, ref arguments, namedParameters); //This also selects property getters
            if (meth != null)
              return LateBinding.CallMethod(meth, arguments, val, binder, culture, namedParameters);
          }
        }

        throw new JScriptException(JSError.FunctionExpected);
      }

      private static void Change64bitIntegersToDouble(Object[] arguments){
        if (arguments == null) return;
        for (int i = 0, n = arguments.Length; i < n; i++){
          Object val = arguments[i];
          IConvertible ic = Convert.GetIConvertible(val);
          TypeCode code = Convert.GetTypeCode(val, ic);
          switch(code){
            case TypeCode.Int64:
            case TypeCode.UInt64:
              arguments[i] = ic.ToDouble(null);
              break;
          }
        }
      }

      public bool Delete(){
        return LateBinding.DeleteMember(this.obj, this.name);
      }

      public static bool DeleteMember(Object obj, String name){
        if (name == null || obj == null) return false;
        if (obj is ScriptObject)
          return ((ScriptObject)obj).DeleteMember(name);
        if (obj is IExpando)
          try{
            IExpando eob = (IExpando)obj;
            MemberInfo[] members = eob.GetMember(name, BindingFlags.Instance|BindingFlags.Public);
            MemberInfo member = LateBinding.SelectMember(members);
            if (member != null){
              eob.RemoveMember(member);
              return true;
            }
            return false;
          }catch{
            return false;
          }
        if (obj is IDictionary){
          IDictionary dict = (IDictionary)obj;
          if (dict.Contains(name)){
            dict.Remove(name);
            return true;
          }
          return false;
        }
        Type obType = obj.GetType();
        MethodInfo deleteOp = TypeReflector.GetTypeReflectorFor(obType).GetMethod(
          "op_Delete", BindingFlags.ExactBinding|BindingFlags.Public|BindingFlags.Static, null, new Type[]{obType, typeof(Object[])}, null);
        if (deleteOp == null || (deleteOp.Attributes & MethodAttributes.SpecialName) == 0 || deleteOp.ReturnType != typeof(Boolean))
          return false; //This is in the spirit of the spec, but not backwards compatible with JS5. The latter throws an exception.
        else
          return (bool)deleteOp.Invoke(null, new Object[]{obj, new Object[]{name}});
      }

      internal static bool DeleteValueAtIndex(Object obj, ulong index){
        if (obj is ArrayObject && index < UInt32.MaxValue)
          return ((ArrayObject)obj).DeleteValueAtIndex((uint)index);
        return LateBinding.DeleteMember(obj, index.ToString(CultureInfo.InvariantCulture));
      }

      private static IReflect GetIRForObjectThatRequiresInvokeMember(Object obj, bool checkForDebugger){
        return null;
      }

      private static IReflect GetIRForObjectThatRequiresInvokeMember(Object obj, bool checkForDebugger, TypeCode tcode){
        return tcode != TypeCode.Object ? null : LateBinding.GetIRForObjectThatRequiresInvokeMember(obj, checkForDebugger);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static Object GetMemberValue(Object obj, String name){
        if (obj is ScriptObject)
          return ((ScriptObject)obj).GetMemberValue(name);
        LateBinding lb = new LateBinding(name, obj);
        return lb.GetNonMissingValue();
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static Object GetMemberValue2(Object obj, String name){
        if (obj is ScriptObject)
          return ((ScriptObject)obj).GetMemberValue(name);
        LateBinding lb = new LateBinding(name, obj);
        return lb.GetValue();
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static Object GetMemberValue(Object obj, String name, MemberInfo member, MemberInfo[] members){
        if (member != null){
          try{
            switch(member.MemberType){
              case MemberTypes.Field:
                Object val = ((FieldInfo)member).GetValue(obj);
                Type et = obj as Type;
                if (et != null && et.IsEnum)
                  try{
                    val = Enum.ToObject(et, ((IConvertible)val).ToUInt64(null));
                  }catch{}
                return val;
              case MemberTypes.Property:
                PropertyInfo prop = (PropertyInfo)member;
                if (prop.DeclaringType == typeof(ArrayObject)){
                  ArrayObject aob = obj as ArrayObject;
                  if (aob != null) return aob.length;
                }else if (prop.DeclaringType == typeof(StringObject)){
                  StringObject strob = obj as StringObject;
                  if (strob != null) return strob.length;
                }
                return JSProperty.GetValue(prop, obj, null);
              case MemberTypes.Event: return null;
              case MemberTypes.NestedType: return member;
            }
          }catch{
            if (obj is StringObject)
              return LateBinding.GetMemberValue(((StringObject)obj).value, name, member, members);
            if (obj is NumberObject)
              return LateBinding.GetMemberValue(((NumberObject)obj).value, name, member, members);
            if (obj is BooleanObject)
              return LateBinding.GetMemberValue(((BooleanObject)obj).value, name, member, members);
            if (obj is ArrayWrapper)
              return LateBinding.GetMemberValue(((ArrayWrapper)obj).value, name, member, members);
            throw;
          }
        }

        if (members != null && members.Length > 0){
          //Special case check for methods on builtin objects
          if (members.Length == 1 && members[0].MemberType == MemberTypes.Method){
            MethodInfo meth = (MethodInfo)members[0];
            Type dt = meth.DeclaringType;
            if (dt == typeof(GlobalObject) ||
               (dt != null && dt != typeof(StringObject) && dt != typeof(NumberObject) && dt != typeof(BooleanObject) && dt.IsSubclassOf(typeof(JSObject)))){
              //This only happens in fast mode. We could add InitOnly fields to the fast predefined objects and initialize them
              //with instances of BuiltinFunction objects, in which case we would never get here, but we would like to avoid
              //the start up cost of allocating these objects, particularly if they end up never being used (the expected common case).
              return Globals.BuiltinFunctionFor(obj, meth);
            }
          }

          return new FunctionWrapper(name, obj, members);
          //That way function wrappers will preserve object identity
        }

        //Check for expando field on ScriptObject, despite the fact that it did not show up in member.
        //This could happen if the object is referred to in a loop and the expando was created after the first time the reference is encountered
        //It is also need to get to the $ properties on RegExp
        if (obj is ScriptObject)
          return ((ScriptObject)obj).GetMemberValue(name);


        //A namespace does not implement IReflect and hence cannot deliver a member for the cache. Check it now.
        if (obj is Namespace){
          Namespace ns = (Namespace)obj;
          String qualid = ns.Name+"."+name;
          Type type = ns.GetType(qualid);
          if (type != null) return type;
          return Namespace.GetNamespace(qualid, ns.engine);
        }

        //Handle COM objects and IExpando objects.
        IReflect ir = LateBinding.GetIRForObjectThatRequiresInvokeMember(obj, true);
        if (ir != null){
          try{
            const BindingFlags flags =
              BindingFlags.Public|BindingFlags.Instance|BindingFlags.OptionalParamBinding|
              BindingFlags.GetProperty|BindingFlags.GetField;
            return ir.InvokeMember(name, flags, JSBinder.ob, obj, null, null, null, null);
          }catch(MissingMemberException){
          }
        }

        //If we get here the object does not have such a member
        return Missing.Value;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      public Object GetNonMissingValue(){
        Object result = this.GetValue();
        if (result is Missing)
          return null;
        return result;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal Object GetValue(){
        this.BindToMember(); //Do a GetMember call and remember the result of the lookup for next time around
        return LateBinding.GetMemberValue(this.obj, this.name, this.last_member, this.last_members);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      public Object GetValue2(){
        Object result = this.GetValue();
        if (result == Missing.Value){
          throw new JScriptException(JSError.UndefinedIdentifier, new Context(new DocumentContext("", null), this.name));
        }
        return result;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static Object GetValueAtIndex(Object obj, ulong index){
        if (obj is ScriptObject)
          if (index < UInt32.MaxValue)
            return ((ScriptObject)obj).GetValueAtIndex((uint)index);
          else
            return ((ScriptObject)obj).GetMemberValue(index.ToString(CultureInfo.InvariantCulture));
      default_property_case:
        if (obj is IList)
          checked {return ((IList)obj)[(int)index];}
        if (obj is Array)
          checked {return ((Array)obj).GetValue((int)index);}
        Type type = obj.GetType();
        if (type.IsCOMObject || obj is IReflect || index > Int32.MaxValue)
          return LateBinding.GetMemberValue(obj, index.ToString(CultureInfo.InvariantCulture));

        //Assume that the object does not itself have a field/property with numerical name.
        //See if the object has a default indexed property, or default property that results in an Array or List and work with that.
        MethodInfo getter = JSBinder.GetDefaultPropertyForArrayIndex(type, (int)index, null, false);
        if (getter != null){
          ParameterInfo[] pars = getter.GetParameters();
          if (pars == null || pars.Length == 0){
            obj = getter.Invoke(obj, BindingFlags.SuppressChangeType, null, null, null);
            goto default_property_case;
          }else
            return getter.Invoke(obj, (BindingFlags)0, JSBinder.ob, new Object[]{(int)index}, null);
        }

        return Missing.Value;
      }

      //Do not call this method for arguments that are going out to COM2 objects via interop.
      private static Object[] LickArgumentsIntoShape(ParameterInfo[] pars, Object[] arguments, Binder binder, CultureInfo culture){
        if (arguments == null)
          return null;
        int formals = pars.Length;
        if (formals == 0)
          return null;
        Object[] newArgs = arguments;
        int actuals = arguments.Length;
        if (actuals != formals)
          newArgs = new Object[formals];
        int m = formals-1; //Position of last formal param
        int k = actuals < m ? actuals : m; //Number of formal params that have corresponding actual arguments, bar the last one, which may be a varArg array.
        //Copy all actual args that match formal param, bar the last one. Change DBNull to null. Coerce actual value to formal param type.
        for (int i = 0; i < k; i++){
          Object arg = arguments[i];
          if (arg is DBNull)
            newArgs[i] = null; //Outside of the world of OLE Variants, undefined == null == the null pointer.
          else
            newArgs[i] = binder.ChangeType(arguments[i], pars[i].ParameterType, culture);
        }
        //Supply default values for all formal params, bar the last one, that do not have corresponding actual arguments
        for (int i = k; i < m; i++){
          Object dv = TypeReferences.GetDefaultParameterValue(pars[i]);
          if (dv == System.Convert.DBNull) //No default value was specified
            dv = binder.ChangeType(null, pars[i].ParameterType, culture); //Substitute undefined
          newArgs[i] = dv;
        }
        //If the last formal param is a vararg param, treat it specially.
        if (CustomAttribute.IsDefined(pars[m], typeof(ParamArrayAttribute), false)){
          int numVarArgs = actuals - m;
          if (numVarArgs < 0) numVarArgs = 0;
          Type t = pars[m].ParameterType.GetElementType();
          Array pa = Array.CreateInstance(t, numVarArgs);
          for (int j = 0; j < numVarArgs; j++)
            pa.SetValue(binder.ChangeType(arguments[j+m], t, culture), j);
          newArgs[m] = pa;
        }else if (actuals < formals){
          Object dv = TypeReferences.GetDefaultParameterValue(pars[m]);
          if (dv == System.Convert.DBNull) //No default value was specified
            dv = binder.ChangeType(null, pars[m].ParameterType, culture); //Substitute undefined
          newArgs[m] = dv;
        }else
          newArgs[m] = binder.ChangeType(arguments[m], pars[m].ParameterType, culture);
        return newArgs;
      }

      internal static MemberInfo SelectMember(MemberInfo[] mems){
        if (mems == null) return null;
        MemberInfo result = null;
        foreach (MemberInfo mem in mems){
          switch(mem.MemberType){
            case MemberTypes.Field:
              if (result == null || result.MemberType != MemberTypes.Field)
                result = mem;
              break;
            case MemberTypes.Property:
              if (result == null || (result.MemberType != MemberTypes.Field && result.MemberType != MemberTypes.Property)){
                ParameterInfo[] pars = ((PropertyInfo)mem).GetIndexParameters();
                if (pars != null && pars.Length == 0)
                  result = mem;
              }
              break;
            case MemberTypes.TypeInfo:
            case MemberTypes.NestedType:
              if (result == null)
                result = mem;
              break;
          }
        }
        return result;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal void SetIndexedDefaultPropertyValue(Object ob, Object[] arguments, Object value){
        ScriptObject scrob = ob as ScriptObject;
        if (scrob != null){
          scrob[arguments] = value;
          return;
        }

        System.Array arr = ob as System.Array;
        if (arr != null){
          if (arguments.Length != arr.Rank)
            throw new JScriptException(JSError.IncorrectNumberOfIndices);
          arr.SetValue(value, LateBinding.ToIndices(arguments));
          return;
        }

        TypeCode tcode = Convert.GetTypeCode(ob);
        if (Convert.NeedsWrapper(tcode)) return; //senseless assignment to implicit wrapper. Do nothing.
        IReflect ir = LateBinding.GetIRForObjectThatRequiresInvokeMember(ob, this.checkForDebugger, tcode);

        if (ir != null){
          try{
            int n = arguments.Length + 1;
            Object[] newargs = new Object[n];
            ArrayObject.Copy(arguments, 0, newargs, 0, n-1);
            newargs[n-1] = value;
            const BindingFlags flags =
              BindingFlags.Public|BindingFlags.Instance|BindingFlags.OptionalParamBinding|
              BindingFlags.SetProperty|BindingFlags.SetField;
            ir.InvokeMember(String.Empty, flags, JSBinder.ob, ob, newargs, null, null, null);
            return;
          }catch(MissingMemberException){
            throw new JScriptException(JSError.OLENoPropOrMethod);
          }
        }

        MemberInfo[] defaultMembers = TypeReflector.GetTypeReflectorFor(ob.GetType()).GetDefaultMembers();
        if (defaultMembers != null && defaultMembers.Length > 0){
          PropertyInfo prop = JSBinder.SelectProperty(Runtime.TypeRefs, defaultMembers, arguments);
          if (prop != null){
            MethodInfo setter = JSProperty.GetSetMethod(prop, false);
            if (setter != null){
              arguments = LateBinding.LickArgumentsIntoShape(prop.GetIndexParameters(), arguments, JSBinder.ob, null);
              value = Convert.CoerceT(value, prop.PropertyType);
              int n = arguments.Length + 1;
              Object[] newargs = new Object[n];
              ArrayObject.Copy(arguments, 0, newargs, 0, n-1);
              newargs[n-1] = value;
              setter.Invoke(ob, newargs);
              return;
            }
          }
        }
        throw new JScriptException(JSError.OLENoPropOrMethod);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal void SetIndexedPropertyValue(Object[] arguments, Object value){
        if (this.obj == null)
          throw new JScriptException(JSError.ObjectExpected);
        if (this.name == null){
          this.SetIndexedDefaultPropertyValue(this.obj, arguments, value);
          return;
        }
        MemberInfo member = this.BindToMember(); //Do a GetMember call and remember the result of the lookup for next time around

        //If the object has any members called this.name, we try to find a suitable indexed property.
        if (this.last_members != null && this.last_members.Length > 0){
          PropertyInfo prop = JSBinder.SelectProperty(Runtime.TypeRefs, this.last_members, arguments);
          if (prop != null){
            if (arguments.Length > 0 && prop.GetIndexParameters().Length == 0 
                ){
              //Might be a parameterless property that results in an object that has a indexed property that can be assigned to
              MethodInfo getter = JSProperty.GetGetMethod(prop, false);
              if (getter != null){
                LateBinding.SetIndexedPropertyValueStatic(getter.Invoke(this.obj, null), arguments, value);
                return;
              }
            }
            arguments = LateBinding.LickArgumentsIntoShape(prop.GetIndexParameters(), arguments, JSBinder.ob, null);
            value = Convert.CoerceT(value, prop.PropertyType);
            JSProperty.SetValue(prop, this.obj, value, arguments);
            return;
          }
        }

        TypeCode tcode = Convert.GetTypeCode(obj);
        if (Convert.NeedsWrapper(tcode)) return; //senseless assignment to implicit wrapper. Do nothing.
        IReflect ir = LateBinding.GetIRForObjectThatRequiresInvokeMember(obj, this.checkForDebugger, tcode);
        if (ir != null){
          int n = arguments.Length + 1;
          Object[] newargs = new Object[n];
          ArrayObject.Copy(arguments, 0, newargs, 0, n-1);
          newargs[n-1] = value;
          const BindingFlags flags =
            BindingFlags.Public|BindingFlags.Instance|BindingFlags.OptionalParamBinding|
            BindingFlags.SetProperty|BindingFlags.SetField;
          ir.InvokeMember(this.name, flags, JSBinder.ob, this.obj, newargs, null, null, null);
          return;
        }

        //<ob.foo> might be an object that has a default indexed property
        Object ob = this.GetValue();
        if (ob != null && !(ob is Missing)){
          this.SetIndexedDefaultPropertyValue(ob, arguments, value);
          return;
        }

        //Give up
        throw new JScriptException(JSError.OLENoPropOrMethod);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      public static void SetIndexedPropertyValueStatic(Object obj, Object[] arguments, Object value){
        if (obj == null)
          throw new JScriptException(JSError.ObjectExpected);

        ScriptObject scrob = obj as ScriptObject;
        if (scrob != null){
          scrob[arguments] = value;
          return;
        }

        Array arr = obj as Array;
        if (arr != null){
          if (arguments.Length != arr.Rank)
            throw new JScriptException(JSError.IncorrectNumberOfIndices);
          arr.SetValue(value, LateBinding.ToIndices(arguments));
          return;
        }

        TypeCode tcode = Convert.GetTypeCode(obj);
        if (Convert.NeedsWrapper(tcode)) return; //senseless assignment to implicit wrapper. Do nothing.
        IReflect ir = LateBinding.GetIRForObjectThatRequiresInvokeMember(obj, true, tcode); //Either called from IL, or on an type object
        if (ir != null){
          //Legacy behavior is to convert the last argument to a string and treat it as the name of the property being set
          String name = String.Empty;
          int n = arguments.Length;
          if (n > 0)
            name = Convert.ToString(arguments[n-1]);
          const BindingFlags flags =
            BindingFlags.Public|BindingFlags.Instance|BindingFlags.OptionalParamBinding|
            BindingFlags.SetProperty|BindingFlags.SetField;
          ir.InvokeMember(name, flags, JSBinder.ob, obj, new Object[]{value}, null, null, null);
          return;
        }

        MemberInfo[] defaultMembers = TypeReflector.GetTypeReflectorFor(obj.GetType()).GetDefaultMembers();
        if (defaultMembers != null && defaultMembers.Length > 0){
          PropertyInfo prop = JSBinder.SelectProperty(Runtime.TypeRefs, defaultMembers, arguments);
          if (prop != null){
            MethodInfo setter = JSProperty.GetSetMethod(prop, false);
            if (setter != null){
              arguments = LateBinding.LickArgumentsIntoShape(prop.GetIndexParameters(), arguments, JSBinder.ob, null);
              value = Convert.CoerceT(value, prop.PropertyType);
              int n = arguments.Length + 1;
              Object[] newargs = new Object[n];
              ArrayObject.Copy(arguments, 0, newargs, 0, n-1);
              newargs[n-1] = value;
              setter.Invoke(obj, newargs);
              return;
            }
          }
        }

        throw new JScriptException(JSError.OLENoPropOrMethod);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      private static void SetMember(Object obj, Object value, MemberInfo member){
        switch(member.MemberType){
          case MemberTypes.Field:
            FieldInfo field = (FieldInfo)member;
            if (!field.IsLiteral && !field.IsInitOnly){
              if (field is JSField)
                field.SetValue(obj, value);
              else
                field.SetValue(obj, Convert.CoerceT(value, field.FieldType), BindingFlags.SuppressChangeType, null, null);
            }
            return;
          case MemberTypes.Property:
            PropertyInfo property = (PropertyInfo)member;
            if (property is JSProperty || property is JSWrappedProperty){
              property.SetValue(obj, value, null);
              return;
            }
            MethodInfo setMeth = JSProperty.GetSetMethod(property, false);
            if (setMeth != null)
              try{
                setMeth.Invoke(obj, BindingFlags.SuppressChangeType, null, new Object[]{Convert.CoerceT(value, property.PropertyType)}, null);
              }catch(TargetInvocationException e){
                throw e.InnerException;
              }
            return;
        }
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static void SetMemberValue(Object obj, String name, Object value){
        if (obj is ScriptObject){
          ((ScriptObject)obj).SetMemberValue(name, value);
          return;
        }
        LateBinding lb = new LateBinding(name, obj);
        lb.SetValue(value);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static void SetMemberValue(Object obj, String name, Object value, MemberInfo member, MemberInfo[] members){
        if (member != null){
          LateBinding.SetMember(obj, value, member);
          return;
        }
        if (obj is ScriptObject){  //Create an expando the fast way. Also needed for $ properties on RegExp
          ((ScriptObject)obj).SetMemberValue(name, value);
          return;
        }

        TypeCode tcode = Convert.GetTypeCode(obj);
        if (Convert.NeedsWrapper(tcode)) return; //senseless assignment to implicit wrapper. Do nothing.
        IReflect ir = LateBinding.GetIRForObjectThatRequiresInvokeMember(obj, true, tcode);
        if (ir != null){
          try{
            Object[] args = new Object[]{value};
            IConvertible iconv = value as IConvertible;
            
            //BindingFlags.SetProperty makes interop call with DISPATCH_PROPERTYPUT|DISPATCH_PROPERTYPUTREF .
            BindingFlags flags = BindingFlags.Public|BindingFlags.Instance|BindingFlags.OptionalParamBinding|
                    BindingFlags.SetProperty|BindingFlags.SetField;
            ir.InvokeMember(name, flags, JSBinder.ob, obj, args, null, null, null);
            return;
          }catch(MissingMemberException){
          }
        }

        if (obj is IExpando){ //Create an expando the slow way
          PropertyInfo prop = ((IExpando)obj).AddProperty(name);
          if (prop != null)
            prop.SetValue(obj, value, null);
          else{
            FieldInfo field = ((IExpando)obj).AddField(name);
            if (field != null){
              field.SetValue(obj, value);
            }
          }
          return;
        }
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      internal static void SetValueAtIndex(Object obj, ulong index, Object value){
        if (obj is ScriptObject){
          if (index < UInt32.MaxValue)
            ((ScriptObject)obj).SetValueAtIndex((uint)index, value);
          else
            ((ScriptObject)obj).SetMemberValue(index.ToString(CultureInfo.InvariantCulture), value);
          return;
        }
      default_property_case:
        if (obj is IList){
          IList list = (IList)obj;
          if (index < (ulong)list.Count)
            list[(int)index] = value;
          else
            list.Insert((int)index, value);
          return;
        }
        if (obj is Array){
          checked{((Array)obj).SetValue(Convert.CoerceT(value, obj.GetType().GetElementType()), (int)index);}
          return;
        }
        Type type = obj.GetType();
        if (type.IsCOMObject || obj is IReflect || index > Int32.MaxValue){
          LateBinding.SetMemberValue(obj, index.ToString(CultureInfo.InvariantCulture), value);
          return;
        }

        //Assume that the object does not itself have a field/property with numerical name.
        //See if the object has a default indexed property, or default property that results in an Array or List and work with that.
        MethodInfo setter = JSBinder.GetDefaultPropertyForArrayIndex(type, (int)index, null, true);
        if (setter != null){
          ParameterInfo[] pars = setter.GetParameters();
          if (pars == null || pars.Length == 0){
            obj = setter.Invoke(obj, BindingFlags.SuppressChangeType, null, null, null);
            goto default_property_case;
          }else
            setter.Invoke(obj, (BindingFlags)0, JSBinder.ob, new Object[]{(int)index, value}, null);
        }
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif
      public void SetValue(Object value){
        this.BindToMember(); //Do a GetMember call and remember the result of the lookup for next time around
        LateBinding.SetMemberValue(this.obj, this.name, value, this.last_member, this.last_members);
      }

      internal static void SwapValues(Object obj, uint left, uint right){
        if (obj is JSObject){
          ((JSObject)obj).SwapValues(left, right);
          return;
        }
        if (obj is IList){
          IList list = (IList)obj;
          Object temp = list[(int)left];
          list[(int)left] = list[(int)right];
          list[(int)right] = temp;
          return;
        }
        if (obj is Array){
          Array arr = (Array)obj;
          Object temp = arr.GetValue((int)left);
          arr.SetValue(arr.GetValue((int)right), (int)left);
          arr.SetValue(temp, (int)right);
          return;
        }
        if (obj is IExpando){
          String left_name = System.Convert.ToString(left, CultureInfo.InvariantCulture);
          String right_name = System.Convert.ToString(right, CultureInfo.InvariantCulture);
          IExpando eob = (IExpando)obj;
          FieldInfo left_field = eob.GetField(left_name, BindingFlags.Instance|BindingFlags.Public);
          FieldInfo right_field = eob.GetField(right_name, BindingFlags.Instance|BindingFlags.Public);
          if (left_field == null)
            if (right_field == null)
              return;
            else
              try{
                left_field = eob.AddField(left_name);
                left_field.SetValue(obj, right_field.GetValue(obj));
                eob.RemoveMember(right_field);
              }catch{
                throw new JScriptException(JSError.ActionNotSupported);
              }
          else if (right_field == null)
            try{
              right_field = eob.AddField(right_name);
              right_field.SetValue(obj, left_field.GetValue(obj));
              eob.RemoveMember(left_field);
            }catch{
              throw new JScriptException(JSError.ActionNotSupported);
            }
          Object temp = left_field.GetValue(obj);
          left_field.SetValue(obj, right_field.GetValue(obj));
          right_field.SetValue(obj, temp);
          return;
        }
        Object lval = LateBinding.GetValueAtIndex(obj, left);
        Object rval = LateBinding.GetValueAtIndex(obj, right);
        if (lval is Missing)
          LateBinding.DeleteValueAtIndex(obj, right);
        else
          LateBinding.SetValueAtIndex(obj, right, lval);
        if (rval is Missing)
          LateBinding.DeleteValueAtIndex(obj, left);
        else
          LateBinding.SetValueAtIndex(obj, left, rval);
      }

      private static int[] ToIndices(Object[] arguments){
        int n = arguments.Length;
        int[] result = new int[n];
        for (int i = 0; i < n; i++)
          result[i] = Convert.ToInt32(arguments[i]);
        return result;
      }
    }
}
