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

/* This class wraps a MethodInfo so that it appears to be a script function. It is used to provide access to the methods in the builtin object model */

namespace Microsoft.JScript {
    
    using Microsoft.JScript.Vsa;
    using System;
    using System.Reflection;
    
    public enum JSBuiltin{
      None = 0,
      Array_concat = 1,
      Array_join,
      Array_pop,
      Array_push,
      Array_reverse,
      Array_shift,
      Array_slice,
      Array_sort,
      Array_splice,
      Array_toLocaleString,
      Array_toString,
      Array_unshift,
      Boolean_toString,
      Boolean_valueOf,
      Date_getDate,
      Date_getDay,
      Date_getFullYear,
      Date_getHours,
      Date_getMilliseconds,
      Date_getMinutes,
      Date_getMonth,
      Date_getSeconds,
      Date_getTime,
      Date_getTimezoneOffset,
      Date_getUTCDate,
      Date_getUTCDay,
      Date_getUTCFullYear,
      Date_getUTCHours,
      Date_getUTCMilliseconds,
      Date_getUTCMinutes,
      Date_getUTCMonth,
      Date_getUTCSeconds,
      Date_getVarDate,
      Date_getYear,
      Date_parse,
      Date_setDate,
      Date_setFullYear,
      Date_setHours,
      Date_setMinutes,
      Date_setMilliseconds,
      Date_setMonth,
      Date_setSeconds,
      Date_setTime,
      Date_setUTCDate,
      Date_setUTCFullYear,
      Date_setUTCHours,
      Date_setUTCMinutes,
      Date_setUTCMilliseconds,
      Date_setUTCMonth,
      Date_setUTCSeconds,
      Date_setYear,
      Date_toDateString,
      Date_toGMTString,
      Date_toLocaleDateString,
      Date_toLocaleString,
      Date_toLocaleTimeString,
      Date_toString,
      Date_toTimeString,
      Date_toUTCString,
      Date_UTC,
      Date_valueOf,
      Enumerator_atEnd,
      Enumerator_item,
      Enumerator_moveFirst,
      Enumerator_moveNext,
      Error_toString,
      Function_apply,
      Function_call,
      Function_toString,
      Global_CollectGarbage,
      Global_decodeURI,
      Global_decodeURIComponent,
      Global_encodeURI,
      Global_encodeURIComponent,
      Global_escape,
      Global_eval,
      Global_GetObject,
      Global_isNaN,
      Global_isFinite,
      Global_parseFloat,
      Global_parseInt,
      Global_ScriptEngine,
      Global_ScriptEngineBuildVersion,
      Global_ScriptEngineMajorVersion,
      Global_ScriptEngineMinorVersion,
      Global_unescape,
      Math_abs,
      Math_acos,
      Math_asin,
      Math_atan,
      Math_atan2,
      Math_ceil,
      Math_cos,
      Math_exp,
      Math_floor,
      Math_log,
      Math_max,
      Math_min,
      Math_pow,
      Math_random,
      Math_round,
      Math_sin,
      Math_sqrt,
      Math_tan,
      Number_toExponential,
      Number_toFixed,
      Number_toLocaleString,
      Number_toPrecision,
      Number_toString,
      Number_valueOf,
      Object_hasOwnProperty,
      Object_isPrototypeOf,
      Object_propertyIsEnumerable,
      Object_toLocaleString,
      Object_toString,
      Object_valueOf,
      RegExp_compile,
      RegExp_exec,
      RegExp_test,
      RegExp_toString,
      String_anchor,
      String_big,
      String_blink,
      String_bold,
      String_charAt,
      String_charCodeAt,
      String_concat,
      String_fixed,
      String_fontcolor,
      String_fontsize,
      String_fromCharCode,
      String_indexOf,
      String_italics,
      String_lastIndexOf,
      String_link,
      String_localeCompare,
      String_match,
      String_replace,
      String_search,
      String_slice,
      String_small,
      String_split,
      String_strike,
      String_sub,
      String_substr,
      String_substring,
      String_sup,
      String_toLocaleLowerCase,
      String_toLocaleUpperCase,
      String_toLowerCase,
      String_toString,
      String_toUpperCase,
      String_valueOf,
      VBArray_dimensions,
      VBArray_getItem,
      VBArray_lbound,
      VBArray_toArray,
      VBArray_ubound,
    }
    
    internal sealed class BuiltinFunction : ScriptFunction{
      internal MethodInfo method;
      private JSBuiltin biFunc;
      
      internal BuiltinFunction(Object obj, MethodInfo method)
        : this(method.Name, obj, method, FunctionPrototype.ob){
      }
      
      internal BuiltinFunction(String name, Object obj, MethodInfo method, ScriptFunction parent)
        : base(parent, name) {
        this.noExpando = false;
        ParameterInfo[] pars = method.GetParameters();
        int n = this.ilength = pars.Length;
        Object[] attrs = CustomAttribute.GetCustomAttributes(method, typeof(JSFunctionAttribute), false);
        JSFunctionAttribute attr = attrs.Length > 0 ? (JSFunctionAttribute)attrs[0] : new JSFunctionAttribute((JSFunctionAttributeEnum)0);
        JSFunctionAttributeEnum attrVal = attr.attributeValue;
        if ((attrVal & JSFunctionAttributeEnum.HasThisObject) != 0)
          this.ilength--;
        if ((attrVal & JSFunctionAttributeEnum.HasEngine) != 0)
          this.ilength--;
        if ((attrVal & JSFunctionAttributeEnum.HasVarArgs) != 0)
          this.ilength--;
        this.biFunc = attr.builtinFunction;
        if (this.biFunc == 0)
          this.method = new JSNativeMethod(method, obj, this.engine);
        else
          this.method = null;
      }
      
      internal override Object Call(Object[] args, Object thisob){
        return BuiltinFunction.QuickCall(args, thisob, this.biFunc, this.method, this.engine);
      }
      
      internal static Object QuickCall(Object[] args, Object thisob, JSBuiltin biFunc, MethodInfo method, VsaEngine engine){
        int n = args.Length;
        switch (biFunc){
          case JSBuiltin.Array_concat : return ArrayPrototype.concat(thisob, engine, args);
          case JSBuiltin.Array_join : return ArrayPrototype.join(thisob, GetArg(args, 0, n));
          case JSBuiltin.Array_pop : return ArrayPrototype.pop(thisob);
          case JSBuiltin.Array_push : return ArrayPrototype.push(thisob, args);
          case JSBuiltin.Array_reverse : return ArrayPrototype.reverse(thisob);
          case JSBuiltin.Array_shift : return ArrayPrototype.shift(thisob);
          case JSBuiltin.Array_slice : return ArrayPrototype.slice(thisob, engine, Convert.ToNumber(GetArg(args, 0, n)), GetArg(args, 1, n));
          case JSBuiltin.Array_sort : return ArrayPrototype.sort(thisob, GetArg(args, 0, n));
          case JSBuiltin.Array_splice : 
            return ArrayPrototype.splice(thisob, engine, Convert.ToNumber(GetArg(args, 0, n)), Convert.ToNumber(GetArg(args, 1, n)), VarArgs(args, 2, n));
          case JSBuiltin.Array_toLocaleString : return ArrayPrototype.toLocaleString(thisob);
          case JSBuiltin.Array_toString : return ArrayPrototype.toString(thisob);
          case JSBuiltin.Array_unshift : return ArrayPrototype.unshift(thisob, args);
          case JSBuiltin.Boolean_toString : return BooleanPrototype.toString(thisob);
          case JSBuiltin.Boolean_valueOf : return BooleanPrototype.valueOf(thisob);
          case JSBuiltin.Date_getDate : return DatePrototype.getDate(thisob);
          case JSBuiltin.Date_getDay : return DatePrototype.getDay(thisob);
          case JSBuiltin.Date_getFullYear : return DatePrototype.getFullYear(thisob);
          case JSBuiltin.Date_getHours : return DatePrototype.getHours(thisob);
          case JSBuiltin.Date_getMilliseconds : return DatePrototype.getMilliseconds(thisob);
          case JSBuiltin.Date_getMinutes : return DatePrototype.getMinutes(thisob);
          case JSBuiltin.Date_getMonth : return DatePrototype.getMonth(thisob);
          case JSBuiltin.Date_getSeconds : return DatePrototype.getSeconds(thisob);
          case JSBuiltin.Date_getTime : return DatePrototype.getTime(thisob);
          case JSBuiltin.Date_getTimezoneOffset : return DatePrototype.getTimezoneOffset(thisob);
          case JSBuiltin.Date_getUTCDate : return DatePrototype.getUTCDate(thisob);
          case JSBuiltin.Date_getUTCDay : return DatePrototype.getUTCDay(thisob);
          case JSBuiltin.Date_getUTCFullYear : return DatePrototype.getUTCFullYear(thisob);
          case JSBuiltin.Date_getUTCHours : return DatePrototype.getUTCHours(thisob);
          case JSBuiltin.Date_getUTCMilliseconds : return DatePrototype.getUTCMilliseconds(thisob);
          case JSBuiltin.Date_getUTCMinutes : return DatePrototype.getUTCMinutes(thisob);
          case JSBuiltin.Date_getUTCMonth : return DatePrototype.getUTCMonth(thisob);
          case JSBuiltin.Date_getUTCSeconds : return DatePrototype.getUTCSeconds(thisob);
          case JSBuiltin.Date_getVarDate : return DatePrototype.getVarDate(thisob);
          case JSBuiltin.Date_getYear : return DatePrototype.getYear(thisob);
          case JSBuiltin.Date_parse : return DateConstructor.parse(Convert.ToString(GetArg(args, 0, n)));
          case JSBuiltin.Date_setDate : return DatePrototype.setDate(thisob, Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Date_setFullYear : return DatePrototype.setFullYear(thisob, Convert.ToNumber(GetArg(args, 0, n)), GetArg(args, 1, n), GetArg(args, 2, n));
          case JSBuiltin.Date_setHours : return DatePrototype.setHours(thisob, Convert.ToNumber(GetArg(args, 0, n)), GetArg(args, 1, n), GetArg(args, 2, n), GetArg(args, 3, n));
          case JSBuiltin.Date_setMinutes : return DatePrototype.setMinutes(thisob, Convert.ToNumber(GetArg(args, 0, n)), GetArg(args, 1, n), GetArg(args, 2, n));
          case JSBuiltin.Date_setMilliseconds : return DatePrototype.setMilliseconds(thisob, Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Date_setMonth : return DatePrototype.setMonth(thisob, Convert.ToNumber(GetArg(args, 0, n)), GetArg(args, 1, n));
          case JSBuiltin.Date_setSeconds : return DatePrototype.setSeconds(thisob, Convert.ToNumber(GetArg(args, 0, n)), GetArg(args, 1, n));
          case JSBuiltin.Date_setTime : return DatePrototype.setTime(thisob, Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Date_setUTCDate : return DatePrototype.setUTCDate(thisob, Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Date_setUTCFullYear : return DatePrototype.setUTCFullYear(thisob, Convert.ToNumber(GetArg(args, 0, n)), GetArg(args, 1, n), GetArg(args, 2, n));
          case JSBuiltin.Date_setUTCHours : return DatePrototype.setUTCHours(thisob, Convert.ToNumber(GetArg(args, 0, n)), GetArg(args, 1, n), GetArg(args, 2, n), GetArg(args, 3, n));
          case JSBuiltin.Date_setUTCMinutes : return DatePrototype.setUTCMinutes(thisob, Convert.ToNumber(GetArg(args, 0, n)), GetArg(args, 1, n), GetArg(args, 2, n));
          case JSBuiltin.Date_setUTCMilliseconds : return DatePrototype.setUTCMilliseconds(thisob, Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Date_setUTCMonth : return DatePrototype.setUTCMonth(thisob, Convert.ToNumber(GetArg(args, 0, n)), GetArg(args, 1, n));
          case JSBuiltin.Date_setUTCSeconds : return DatePrototype.setUTCSeconds(thisob, Convert.ToNumber(GetArg(args, 0, n)), GetArg(args, 1, n));
          case JSBuiltin.Date_setYear : return DatePrototype.setYear(thisob, Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Date_toDateString : return DatePrototype.toDateString(thisob);
          case JSBuiltin.Date_toGMTString : return DatePrototype.toGMTString(thisob);
          case JSBuiltin.Date_toLocaleDateString : return DatePrototype.toLocaleDateString(thisob);
          case JSBuiltin.Date_toLocaleString : return DatePrototype.toLocaleString(thisob);
          case JSBuiltin.Date_toLocaleTimeString : return DatePrototype.toLocaleTimeString(thisob);
          case JSBuiltin.Date_toString : return DatePrototype.toString(thisob);
          case JSBuiltin.Date_toTimeString : return DatePrototype.toTimeString(thisob);
          case JSBuiltin.Date_toUTCString : return DatePrototype.toUTCString(thisob);
          case JSBuiltin.Date_UTC : return DateConstructor.UTC(GetArg(args, 0, n), GetArg(args, 1, n), GetArg(args, 2, n), GetArg(args, 3, n), GetArg(args, 4, n), GetArg(args, 5, n), GetArg(args, 6, n));
          case JSBuiltin.Date_valueOf : return DatePrototype.valueOf(thisob);
          case JSBuiltin.Enumerator_atEnd : return EnumeratorPrototype.atEnd(thisob);
          case JSBuiltin.Enumerator_item : return EnumeratorPrototype.item(thisob);
          case JSBuiltin.Enumerator_moveFirst : EnumeratorPrototype.moveFirst(thisob); return null;
          case JSBuiltin.Enumerator_moveNext : EnumeratorPrototype.moveNext(thisob); return null;
          case JSBuiltin.Error_toString : return ErrorPrototype.toString(thisob);
          case JSBuiltin.Function_apply : return FunctionPrototype.apply(thisob, GetArg(args, 0, n), GetArg(args, 1, n));
          case JSBuiltin.Function_call : return FunctionPrototype.call(thisob, GetArg(args, 0, n), VarArgs(args, 1, n));
          case JSBuiltin.Function_toString : return FunctionPrototype.toString(thisob);
          case JSBuiltin.Global_CollectGarbage : GlobalObject.CollectGarbage(); return null;
          case JSBuiltin.Global_decodeURI : return GlobalObject.decodeURI(GetArg(args, 0, n));
          case JSBuiltin.Global_decodeURIComponent : return GlobalObject.decodeURIComponent(GetArg(args, 0, n));
          case JSBuiltin.Global_encodeURI : return GlobalObject.encodeURI(GetArg(args, 0, n));
          case JSBuiltin.Global_encodeURIComponent : return GlobalObject.encodeURIComponent(GetArg(args, 0, n));
          case JSBuiltin.Global_escape : return GlobalObject.escape(GetArg(args, 0, n));
          case JSBuiltin.Global_eval : return GlobalObject.eval(GetArg(args, 0, n));
          case JSBuiltin.Global_GetObject : return GlobalObject.GetObject(GetArg(args, 0, n), GetArg(args, 1, n));
          case JSBuiltin.Global_isNaN : return GlobalObject.isNaN(GetArg(args, 0, n));
          case JSBuiltin.Global_isFinite : return GlobalObject.isFinite(Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Global_parseFloat : return GlobalObject.parseFloat(GetArg(args, 0, n));
          case JSBuiltin.Global_parseInt : return GlobalObject.parseInt(GetArg(args, 0, n), GetArg(args, 1, n));
          case JSBuiltin.Global_ScriptEngine : return GlobalObject.ScriptEngine();
          case JSBuiltin.Global_ScriptEngineBuildVersion : return GlobalObject.ScriptEngineBuildVersion();
          case JSBuiltin.Global_ScriptEngineMajorVersion : return GlobalObject.ScriptEngineMajorVersion();
          case JSBuiltin.Global_ScriptEngineMinorVersion : return GlobalObject.ScriptEngineMinorVersion();
          case JSBuiltin.Global_unescape : return GlobalObject.unescape(GetArg(args, 0, n));
          case JSBuiltin.Math_abs : return MathObject.abs(Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Math_acos : return MathObject.acos(Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Math_asin : return MathObject.asin(Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Math_atan : return MathObject.atan(Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Math_atan2 : return MathObject.atan2(Convert.ToNumber(GetArg(args, 0, n)), Convert.ToNumber(GetArg(args, 1, n)));
          case JSBuiltin.Math_ceil : return MathObject.ceil(Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Math_cos : return MathObject.cos(Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Math_exp : return MathObject.exp(Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Math_floor : return MathObject.floor(Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Math_log : return MathObject.log(Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Math_max : return MathObject.max(GetArg(args, 0, n), GetArg(args, 1, n), VarArgs(args, 2, n));
          case JSBuiltin.Math_min : return MathObject.min(GetArg(args, 0, n), GetArg(args, 1, n), VarArgs(args, 2, n));
          case JSBuiltin.Math_pow : return MathObject.pow(Convert.ToNumber(GetArg(args, 0, n)), Convert.ToNumber(GetArg(args, 1, n)));
          case JSBuiltin.Math_random : return MathObject.random();
          case JSBuiltin.Math_round : return MathObject.round(Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Math_sin : return MathObject.sin(Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Math_sqrt : return MathObject.sqrt(Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Math_tan : return MathObject.tan(Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Number_toExponential : return NumberPrototype.toExponential(thisob, GetArg(args, 0, n));
          case JSBuiltin.Number_toFixed : return NumberPrototype.toFixed(thisob, Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.Number_toLocaleString: return NumberPrototype.toLocaleString(thisob);
          case JSBuiltin.Number_toPrecision : return NumberPrototype.toPrecision(thisob, GetArg(args, 0, n));
          case JSBuiltin.Number_toString : return NumberPrototype.toString(thisob, GetArg(args, 0, n));
          case JSBuiltin.Number_valueOf : return NumberPrototype.valueOf(thisob);
          case JSBuiltin.Object_hasOwnProperty : return ObjectPrototype.hasOwnProperty(thisob, GetArg(args, 0, n));
          case JSBuiltin.Object_isPrototypeOf : return ObjectPrototype.isPrototypeOf(thisob, GetArg(args, 0, n));
          case JSBuiltin.Object_propertyIsEnumerable : return ObjectPrototype.propertyIsEnumerable(thisob, GetArg(args, 0, n));
          case JSBuiltin.Object_toLocaleString : return ObjectPrototype.toLocaleString(thisob);
          case JSBuiltin.Object_toString : return ObjectPrototype.toString(thisob);
          case JSBuiltin.Object_valueOf : return ObjectPrototype.valueOf(thisob);
          case JSBuiltin.RegExp_compile : return RegExpPrototype.compile(thisob, GetArg(args, 0, n), GetArg(args, 1, n));
          case JSBuiltin.RegExp_exec : return RegExpPrototype.exec(thisob, GetArg(args, 0, n));
          case JSBuiltin.RegExp_test : return RegExpPrototype.test(thisob, GetArg(args, 0, n));
          case JSBuiltin.RegExp_toString : return RegExpPrototype.toString(thisob);
          case JSBuiltin.String_anchor : return StringPrototype.anchor(thisob, GetArg(args, 0, n));
          case JSBuiltin.String_big : return StringPrototype.big(thisob);
          case JSBuiltin.String_blink : return StringPrototype.blink(thisob);
          case JSBuiltin.String_bold : return StringPrototype.bold(thisob);
          case JSBuiltin.String_charAt : return StringPrototype.charAt(thisob, Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.String_charCodeAt : return StringPrototype.charCodeAt(thisob, Convert.ToNumber(GetArg(args, 0, n)));
          case JSBuiltin.String_concat : return StringPrototype.concat(thisob, args);
          case JSBuiltin.String_fixed : return StringPrototype.@fixed(thisob);
          case JSBuiltin.String_fontcolor : return StringPrototype.fontcolor(thisob, GetArg(args, 0, n));
          case JSBuiltin.String_fontsize : return StringPrototype.fontsize(thisob, GetArg(args, 0, n));
          case JSBuiltin.String_fromCharCode : return StringConstructor.fromCharCode(args);
          case JSBuiltin.String_indexOf : return StringPrototype.indexOf(thisob, GetArg(args, 0, n), Convert.ToNumber(GetArg(args, 1, n)));
          case JSBuiltin.String_italics : return StringPrototype.italics(thisob);
          case JSBuiltin.String_lastIndexOf : return StringPrototype.lastIndexOf(thisob, GetArg(args, 0, n), Convert.ToNumber(GetArg(args, 1, n)));
          case JSBuiltin.String_link : return StringPrototype.link(thisob, GetArg(args, 0, n));
          case JSBuiltin.String_localeCompare : return StringPrototype.localeCompare(thisob, GetArg(args, 0, n));
          case JSBuiltin.String_match : return StringPrototype.match(thisob, engine, GetArg(args, 0, n));
          case JSBuiltin.String_replace : return StringPrototype.replace(thisob, GetArg(args, 0, n), GetArg(args, 1, n));
          case JSBuiltin.String_search : return StringPrototype.search(thisob, engine, GetArg(args, 0, n));
          case JSBuiltin.String_slice : return StringPrototype.slice(thisob, Convert.ToNumber(GetArg(args, 0, n)), GetArg(args, 1, n));
          case JSBuiltin.String_small : return StringPrototype.small(thisob);
          case JSBuiltin.String_split : return StringPrototype.split(thisob, engine, GetArg(args, 0, n), GetArg(args, 1, n));
          case JSBuiltin.String_strike : return StringPrototype.strike(thisob);
          case JSBuiltin.String_sub : return StringPrototype.sub(thisob);
          case JSBuiltin.String_substr : return StringPrototype.substr(thisob, Convert.ToNumber(GetArg(args, 0, n)), GetArg(args, 1, n));
          case JSBuiltin.String_substring : return StringPrototype.substring(thisob, Convert.ToNumber(GetArg(args, 0, n)), GetArg(args, 1, n));
          case JSBuiltin.String_sup : return StringPrototype.sup(thisob);
          case JSBuiltin.String_toLocaleLowerCase : return StringPrototype.toLocaleLowerCase(thisob);
          case JSBuiltin.String_toLocaleUpperCase : return StringPrototype.toLocaleUpperCase(thisob);
          case JSBuiltin.String_toLowerCase : return StringPrototype.toLowerCase(thisob);
          case JSBuiltin.String_toString : return StringPrototype.toString(thisob);
          case JSBuiltin.String_toUpperCase : return StringPrototype.toUpperCase(thisob);
          case JSBuiltin.String_valueOf : return StringPrototype.valueOf(thisob);
          case JSBuiltin.VBArray_dimensions : return VBArrayPrototype.dimensions(thisob);
          case JSBuiltin.VBArray_getItem : return VBArrayPrototype.getItem(thisob, args);
          case JSBuiltin.VBArray_lbound : return VBArrayPrototype.lbound(thisob, GetArg(args, 0, n));
          case JSBuiltin.VBArray_toArray : return VBArrayPrototype.toArray(thisob, engine);
          case JSBuiltin.VBArray_ubound : return VBArrayPrototype.ubound(thisob, GetArg(args, 0, n));
        }
        return method.Invoke(thisob, (BindingFlags)0, JSBinder.ob, args, null);
      }
      
      private static Object GetArg(Object[] args, int i, int n){
        return i < n ? args[i] : Missing.Value;
      }
      
      private static Object[] VarArgs(Object[] args, int offset, int n){
        Object[] result = new Object[n >= offset ? n-offset : 0];
        for (int i = offset; i < n; i++)
          result[i-offset] = args[i];
        return result;
      }
      
      public override String ToString(){
        return "function "+this.name+"() {\n    [native code]\n}";
      }
    }
}
