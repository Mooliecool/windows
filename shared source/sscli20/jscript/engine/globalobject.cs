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
    using System.Reflection;
    using System.Runtime.InteropServices;
    using System.Configuration.Assemblies;
    using System.Text;
    using System.Security.Permissions;

    public class GlobalObject{
      internal static readonly GlobalObject commonInstance = new GlobalObject();
      public const double Infinity = Double.PositiveInfinity;
      public const double NaN = Double.NaN;
      public static readonly Empty undefined = null;

      protected ActiveXObjectConstructor originalActiveXObjectField;
      protected ArrayConstructor originalArrayField;
      protected BooleanConstructor originalBooleanField;
      protected DateConstructor originalDateField;
      protected EnumeratorConstructor originalEnumeratorField;
      protected ErrorConstructor originalErrorField;
      protected ErrorConstructor originalEvalErrorField;
      protected FunctionConstructor originalFunctionField;
      protected NumberConstructor originalNumberField;
      protected ObjectConstructor originalObjectField;
      protected ObjectPrototype originalObjectPrototypeField;
      protected ErrorConstructor originalRangeErrorField;
      protected ErrorConstructor originalReferenceErrorField;
      protected RegExpConstructor originalRegExpField;
      protected StringConstructor originalStringField;
      protected ErrorConstructor originalSyntaxErrorField;
      protected ErrorConstructor originalTypeErrorField;
      protected VBArrayConstructor originalVBArrayField;
      protected ErrorConstructor originalURIErrorField;

      internal GlobalObject(){
        this.originalActiveXObjectField = null;
        this.originalArrayField = null;
        this.originalBooleanField = null;
        this.originalDateField = null;
        this.originalEnumeratorField = null;
        this.originalEvalErrorField = null;
        this.originalErrorField = null;
        this.originalFunctionField = null;
        this.originalNumberField = null;
        this.originalObjectField = null;
        this.originalObjectPrototypeField = null;
        this.originalRangeErrorField = null;
        this.originalReferenceErrorField = null;
        this.originalRegExpField = null;
        this.originalStringField = null;
        this.originalSyntaxErrorField = null;
        this.originalTypeErrorField = null;
        this.originalVBArrayField = null;
        this.originalURIErrorField = null;
      }

      public static ActiveXObjectConstructor ActiveXObject{
        get{
          return ActiveXObjectConstructor.ob;
        }
      }

      private static void AppendInHex(StringBuilder bs, int value){
        bs.Append('%');
        int nibble = value >> 4 & 0x0F;
        bs.Append((char)(nibble >= 10 ? nibble - 10 + 'A' : nibble + '0'));
        nibble = value & 0x0F;
        bs.Append((char)(nibble >= 10 ? nibble - 10 + 'A' : nibble + '0'));
      }

      public static ArrayConstructor Array{
        get{
          return ArrayConstructor.ob;
        }
      }

      public static BooleanConstructor Boolean{
        get{
          return BooleanConstructor.ob;
        }
      }

      public static Type boolean{
        get{
          return Typeob.Boolean;
        }
      }

      public static Type @byte{
        get{
          return Typeob.Byte;
        }
      }

      public static Type @char{
        get{
          return Typeob.Char;
        }
      }

      [JSFunctionAttribute(0, JSBuiltin.Global_CollectGarbage)]
      public static void CollectGarbage(){
        System.GC.Collect();
      }

      public static DateConstructor Date{
        get{
          return DateConstructor.ob;
        }
      }

      public static Type @decimal{
        get{
          return Typeob.Decimal;
        }
      }

      private static String Decode(Object encodedURI, URISetType flags){
        String encodedURIStr = Convert.ToString(encodedURI);
        StringBuilder resultStr = new StringBuilder();
        for (int i = 0; i < encodedURIStr.Length; i++){
          char ch = encodedURIStr[i];
          if (ch != '%')
            resultStr.Append(ch);
          else{
            int start = i;
            if (i+2 >= encodedURIStr.Length)
              throw new JScriptException(JSError.URIDecodeError);
            byte b = HexValue(encodedURIStr[i+1], encodedURIStr[i+2]);
            i += 2;

            char ch1;
            if ((b & 0x80) == 0)
              ch1 = (char)b;
            else{
              int n;
              for (n = 1; ((b << n) & 0x80) != 0; n++) ;
              if (n == 1 || n > 4 || i + (n-1) * 3 >= encodedURIStr.Length)
                throw new JScriptException(JSError.URIDecodeError);
              int value = (int)b & 0xFF >> (n+1);
              for (; n > 1; n--){
                if (encodedURIStr[i+1] != '%')
                  throw new JScriptException(JSError.URIDecodeError);
                b = HexValue(encodedURIStr[i+2], encodedURIStr[i+3]);
                i += 3;
                // The two leading bits should be 10 for a valid UTF-8 encoding.
                if ((b & 0xC0) != 0x80)
                  throw new JScriptException(JSError.URIDecodeError);
                value = value << 6 | (int)(b & 0x3F);
              }
              if (value >= 0xD800 && value < 0xE000)
                throw new JScriptException(JSError.URIDecodeError);
              if (value < 0x10000)
                ch1 = (char)value;
              else{
                if (value > 0x10FFFF)
                  throw new JScriptException(JSError.URIDecodeError);
                resultStr.Append((char)((value - 0x10000 >> 10 & 0x3FF) + 0xD800));
                resultStr.Append((char)((value - 0x10000       & 0x3FF) + 0xDC00));
                continue;
              }
            }

            if (GlobalObject.InURISet(ch1, flags))
              resultStr.Append(encodedURIStr, start, i-start+1);
            else
              resultStr.Append(ch1);
          }
        }

        return resultStr.ToString();
      }

      [JSFunctionAttribute(0, JSBuiltin.Global_decodeURI)]
      public static String decodeURI(Object encodedURI){
        return Decode(encodedURI, URISetType.Reserved);
      }

      [JSFunctionAttribute(0, JSBuiltin.Global_decodeURIComponent)]
      public static String decodeURIComponent(Object encodedURI){
        return Decode(encodedURI, URISetType.None);
      }

      public static Type @double{
        get{
          return Typeob.Double;
        }
      }

      private static String Encode(Object uri, URISetType flags){
        String uriStr = Convert.ToString(uri);
        StringBuilder resultStr = new StringBuilder();
        for (int i = 0; i < uriStr.Length; i++){
          char ch = uriStr[i];
          if (GlobalObject.InURISet(ch, flags))
            resultStr.Append(ch);
          else{
            int value = (int)ch;
            if (value >= 0 && value <= 0x7F)
              GlobalObject.AppendInHex(resultStr, value);
            else if (value >= 0x0080 && value <= 0x07FF){
              GlobalObject.AppendInHex(resultStr, value >>  6        | 0xC0);
              GlobalObject.AppendInHex(resultStr, value       & 0x3F | 0x80);
            }else if (value < 0xD800 || value > 0xDFFF){
              GlobalObject.AppendInHex(resultStr, value >> 12        | 0xE0);
              GlobalObject.AppendInHex(resultStr, value >>  6 & 0x3F | 0x80);
              GlobalObject.AppendInHex(resultStr, value       & 0x3F | 0x80);
            }else{
              if (value >= 0xDC00 && value <= 0xDFFF)
                throw new JScriptException(JSError.URIEncodeError);
              if (++i >= uriStr.Length)
                throw new JScriptException(JSError.URIEncodeError);
              int value1 = (int)uriStr[i];
              if (value1 < 0xDC00 || value1 > 0xDFFF)
                throw new JScriptException(JSError.URIEncodeError);
              value = (value - 0xD800 << 10) + value1 + 0x2400;
              GlobalObject.AppendInHex(resultStr, value >> 18        | 0xF0);
              GlobalObject.AppendInHex(resultStr, value >> 12 & 0x3F | 0x80);
              GlobalObject.AppendInHex(resultStr, value >>  6 & 0x3F | 0x80);
              GlobalObject.AppendInHex(resultStr, value       & 0x3F | 0x80);
            }
          }
        }
        return resultStr.ToString();
      }

      [JSFunctionAttribute(0, JSBuiltin.Global_encodeURI)]
      public static String encodeURI(Object uri){
        return GlobalObject.Encode(uri, URISetType.Reserved | URISetType.Unescaped);
      }

      [JSFunctionAttribute(0, JSBuiltin.Global_encodeURIComponent)]
      public static String encodeURIComponent(Object uriComponent){
        return GlobalObject.Encode(uriComponent, URISetType.Unescaped);
      }

      public static EnumeratorConstructor Enumerator{
        get{
          return EnumeratorConstructor.ob;
        }
      }

      public static ErrorConstructor Error{
        get{
          return ErrorConstructor.ob;
        }
      }

      [NotRecommended ("escape")]
      [JSFunctionAttribute(0, JSBuiltin.Global_escape)]
      public static String escape(Object @string){
        String str = Convert.ToString(@string);
        String hex = "0123456789ABCDEF";
        int n = str.Length;
        StringBuilder ustr = new StringBuilder(n*2);
        char c; int d;
        for (int k = -1; ++k < n; ustr.Append(c)){
          c = str[k]; d = (int)c;
          if ((int)'A' <= d && d <= (int)'Z')
            continue;
          if ((int)'a' <= d && d <= (int)'z')
            continue;
          if ((int)'0' <= d && d <= (int)'9')
            continue;
          if (c == '@' || c == '*' || c == '_' || c == '+' || c == '-' || c == '.' || c == '/')
            continue;
          ustr.Append('%');
          if (d < 256){
            ustr.Append(hex[d/16]);
            c = hex[d%16];
          }else{
            ustr.Append('u');
            ustr.Append(hex[(d>>12)%16]);
            ustr.Append(hex[(d>>8)%16]);
            ustr.Append(hex[(d>>4)%16]);
            c = hex[d%16];
          }
        }
        return ustr.ToString();
      }


      [JSFunctionAttribute(0, JSBuiltin.Global_eval)]
      public static Object eval(Object x){
        //The eval function is never called by JScript code because it is a special form that gets its own AST node.
        //If this function is called, it is because of an illegal program.
        throw new JScriptException(JSError.IllegalEval);
      }

      public static ErrorConstructor EvalError{
        get{
          return ErrorConstructor.evalOb;
        }
      }

      public static Type @float{
        get{
          return Typeob.Single;
        }
      }

      public static FunctionConstructor Function{
        get{
          return FunctionConstructor.ob;
        }
      }

      [JSFunctionAttribute(0, JSBuiltin.Global_GetObject)]
      [SecurityPermissionAttribute(SecurityAction.Demand, Flags=SecurityPermissionFlag.UnmanagedCode)]
      public static Object GetObject(Object moniker, Object progId){
        // GetObject is not supported for FEATURE_PAL.
        throw new JScriptException(JSError.InvalidCall);
      }

      internal static int HexDigit(char c){
        if (c >= '0' && c <= '9') return ((int)c) - ((int)'0');
        if (c >= 'A' && c <= 'F') return 10 + ((int)c) - ((int)'A');
        if (c >= 'a' && c <= 'f') return 10 + ((int)c) - ((int)'a');
        return -1;
      }

      private static byte HexValue(char ch1, char ch2){
        int d1, d2;
        if ((d1 = HexDigit(ch1)) < 0
         || (d2 = HexDigit(ch2)) < 0)
          throw new JScriptException(JSError.URIDecodeError);
        return (byte)(d1 << 4 | d2);
      }

      public static Type @int{
        get{
          return Typeob.Int32;
        }
      }

      private static bool InURISet(char ch, URISetType flags){
        if ((flags & URISetType.Unescaped) != URISetType.None){
          if (ch >= '0' && ch <= '9'
           || ch >= 'A' && ch <= 'Z'
           || ch >= 'a' && ch <= 'z')
            return true;
          switch (ch){
            case '-':  case '_': case '.':
            case '!':  case '~': case '*':
            case '\'': case '(': case ')':
              return true;
          }
        }
        if ((flags & URISetType.Reserved) != URISetType.None){
          switch (ch){
            case ';': case '/': case '?':
            case ':': case '@': case '&':
            case '=': case '+': case '$':
            case ',': case '#':
              return true;
          }
        }
        return false;
      }

      [JSFunctionAttribute(0, JSBuiltin.Global_isNaN)]
      public static bool isNaN(Object num){
        double number = Convert.ToNumber(num);
        return number != number;
      }

      [JSFunctionAttribute(0, JSBuiltin.Global_isFinite)]
      public static bool isFinite(double number){
        return !(Double.IsInfinity(number) || Double.IsNaN(number));
      }

      public static Type @long{
        get{
          return Typeob.Int64;
        }
      }

      public static MathObject Math{
        get{
          if (MathObject.ob == null)
            MathObject.ob = new MathObject(ObjectPrototype.ob);
          return MathObject.ob;
        }
      }

      public static NumberConstructor Number{
        get{
          return NumberConstructor.ob;
        }
      }

      public static ObjectConstructor Object{
        get{
          return ObjectConstructor.ob;
        }
      }

      internal virtual ActiveXObjectConstructor originalActiveXObject{
        get{
          if (this.originalActiveXObjectField == null)
            this.originalActiveXObjectField = ActiveXObjectConstructor.ob;
          return this.originalActiveXObjectField;
        }
      }

      internal virtual ArrayConstructor originalArray{
        get{
          if (this.originalArrayField == null)
            this.originalArrayField = ArrayConstructor.ob;
          return this.originalArrayField;
        }
      }

      internal virtual BooleanConstructor originalBoolean{
        get{
          if (this.originalBooleanField == null)
            this.originalBooleanField = BooleanConstructor.ob;
          return this.originalBooleanField;
        }
      }

      internal virtual DateConstructor originalDate{
        get{
          if (this.originalDateField == null)
            this.originalDateField = DateConstructor.ob;
          return this.originalDateField;
        }
      }

      internal virtual EnumeratorConstructor originalEnumerator{
        get{
          if (this.originalEnumeratorField == null)
            this.originalEnumeratorField = EnumeratorConstructor.ob;
          return this.originalEnumeratorField;
        }
      }

      internal virtual ErrorConstructor originalError{
        get{
          if (this.originalErrorField == null)
            this.originalErrorField = ErrorConstructor.ob;
          return this.originalErrorField;
        }
      }

      internal virtual ErrorConstructor originalEvalError{
        get{
          if (this.originalEvalErrorField == null)
            this.originalEvalErrorField = ErrorConstructor.evalOb;
          return this.originalEvalErrorField;
        }
      }

      internal virtual FunctionConstructor originalFunction{
        get{
          if (this.originalFunctionField == null)
            this.originalFunctionField = FunctionConstructor.ob;
          return this.originalFunctionField;
        }
      }

      internal virtual NumberConstructor originalNumber{
        get{
          if (this.originalNumberField == null)
            this.originalNumberField = NumberConstructor.ob;
          return this.originalNumberField;
        }
      }

      internal virtual ObjectConstructor originalObject{
        get{
          if (this.originalObjectField == null)
            this.originalObjectField = ObjectConstructor.ob;
          return this.originalObjectField;
        }
      }

      internal virtual ObjectPrototype originalObjectPrototype{
        get{
          if (this.originalObjectPrototypeField == null)
            this.originalObjectPrototypeField = ObjectPrototype.ob;
          return this.originalObjectPrototypeField;
        }
      }

      internal virtual ErrorConstructor originalRangeError{
        get{
          if (this.originalRangeErrorField == null)
            this.originalRangeErrorField = ErrorConstructor.rangeOb;
          return this.originalRangeErrorField;
        }
      }

      internal virtual ErrorConstructor originalReferenceError{
        get{
          if (this.originalReferenceErrorField == null)
            this.originalReferenceErrorField = ErrorConstructor.referenceOb;
          return this.originalReferenceErrorField;
        }
      }

      internal virtual RegExpConstructor originalRegExp{
        get{
          if (this.originalRegExpField == null)
            this.originalRegExpField = RegExpConstructor.ob;
          return this.originalRegExpField;
        }
      }

      internal virtual StringConstructor originalString{
        get{
          if (this.originalStringField == null)
            this.originalStringField = StringConstructor.ob;
          return this.originalStringField;
        }
      }

      internal virtual ErrorConstructor originalSyntaxError{
        get{
          if (this.originalSyntaxErrorField == null)
            this.originalSyntaxErrorField = ErrorConstructor.syntaxOb;
          return this.originalSyntaxErrorField;
        }
      }

      internal virtual ErrorConstructor originalTypeError{
        get{
          if (this.originalTypeErrorField == null)
            this.originalTypeErrorField = ErrorConstructor.typeOb;
          return this.originalTypeErrorField;
        }
      }

      internal virtual ErrorConstructor originalURIError{
        get{
          if (this.originalURIErrorField == null)
            this.originalURIErrorField = ErrorConstructor.uriOb;
          return this.originalURIErrorField;
        }
      }

      internal virtual VBArrayConstructor originalVBArray{
        get{
          if (this.originalVBArrayField == null)
            this.originalVBArrayField = VBArrayConstructor.ob;
          return this.originalVBArrayField;
        }
      }

      [JSFunctionAttribute(0, JSBuiltin.Global_parseFloat)]
      public static double parseFloat(Object @string){
        String str = Convert.ToString(@string);
        return Convert.ToNumber(str, false, false, Missing.Value);
      }

      [JSFunctionAttribute(0, JSBuiltin.Global_parseInt)]
      public static double parseInt(Object @string, Object radix){
        String str = Convert.ToString(@string);
        return Convert.ToNumber(str, true, true, radix);
      }

      public static ErrorConstructor RangeError{
        get{
          return ErrorConstructor.rangeOb;
        }
      }

      public static ErrorConstructor ReferenceError{
        get{
          return ErrorConstructor.referenceOb;
        }
      }

      public static RegExpConstructor RegExp{
        get{
          return RegExpConstructor.ob;
        }
      }

      [JSFunctionAttribute(0, JSBuiltin.Global_ScriptEngine)]
      public static String ScriptEngine(){
        return "JScript";
      }

      [JSFunctionAttribute(0, JSBuiltin.Global_ScriptEngineBuildVersion)]
      public static int ScriptEngineBuildVersion(){
        return BuildVersionInfo.Build;
      }

      [JSFunctionAttribute(0, JSBuiltin.Global_ScriptEngineMajorVersion)]
      public static int ScriptEngineMajorVersion(){
        return BuildVersionInfo.MajorVersion;
      }

      [JSFunctionAttribute(0, JSBuiltin.Global_ScriptEngineMinorVersion)]
      public static int ScriptEngineMinorVersion(){
        return BuildVersionInfo.MinorVersion;
      }

      public static Type @sbyte{
        get{
          return Typeob.SByte;
        }
      }

      public static Type @short{
        get{
          return Typeob.Int16;
        }
      }

      public static StringConstructor String{
        get{
          return StringConstructor.ob;
        }
      }

      public static ErrorConstructor SyntaxError{
        get{
          return ErrorConstructor.syntaxOb;
        }
      }

      public static ErrorConstructor TypeError{
        get{
          return ErrorConstructor.typeOb;
        }
      }

      [NotRecommended ("unescape")]
      [JSFunctionAttribute(0, JSBuiltin.Global_unescape)]
      public static String unescape(Object @string){
        String str = Convert.ToString(@string);
        int n = str.Length;
        StringBuilder ustr = new StringBuilder(n);
        char c; int d1, d2, d3, d4;
        for (int k = -1; ++k < n; ustr.Append(c)){
          c = str[k];
          if (c == '%'){
            if (k+5 < n && str[k+1] == 'u'
            && (d1 = HexDigit(str[k+2])) != -1
            && (d2 = HexDigit(str[k+3])) != -1
            && (d3 = HexDigit(str[k+4])) != -1
            && (d4 = HexDigit(str[k+5])) != -1){
              c = (char)((d1<<12)+(d2<<8)+(d3<<4)+d4);
              k += 5;
            }else if (k+2 < n
            && (d1 = HexDigit(str[k+1])) != -1
            && (d2 = HexDigit(str[k+2])) != -1){
              c = (char)((d1<<4)+d2);
              k += 2;
            }
          }
        }
        return ustr.ToString();
      }

      public static ErrorConstructor URIError{
        get{
          return ErrorConstructor.uriOb;
        }
      }

      private enum URISetType{ None = 0x0, Reserved = 0x1, Unescaped = 0x2 };

      public static VBArrayConstructor VBArray{
        get{
          return VBArrayConstructor.ob;
        }
      }

      public static Type @void{
        get{
          return Typeob.Void;
        }
      }

      public static Type @uint{
        get{
          return Typeob.UInt32;
        }
      }

      public static Type @ulong{
        get{
          return Typeob.UInt64;
        }
      }

      public static Type @ushort{
        get{
          return Typeob.UInt16;
        }
      }
    }
}
