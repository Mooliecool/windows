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

    using Microsoft.JScript.Vsa;
    using System;
    using System.Collections;
    using System.Globalization;
    using System.Text;
    using System.Text.RegularExpressions;

    public class StringPrototype : StringObject{
      internal static readonly StringPrototype ob = new StringPrototype(FunctionPrototype.ob, ObjectPrototype.ob);
      internal static StringConstructor _constructor;

      internal StringPrototype(FunctionPrototype funcprot, ObjectPrototype parent)
        : base(parent, ""){
        this.noExpando = true;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_anchor)]
      public static String anchor(Object thisob, Object anchorName){
        String thisStr = Convert.ToString(thisob);
        String anchorStr = Convert.ToString(anchorName);
        return "<A NAME=\""+anchorStr+"\">"+thisStr+"</A>";
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_big)]
      public static String big(Object thisob){
        return "<BIG>"+Convert.ToString(thisob)+"</BIG>";
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_blink)]
      public static String blink(Object thisob){
        return "<BLINK>"+Convert.ToString(thisob)+"</BLINK>";
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_bold)]
      public static String bold(Object thisob){
        return "<B>"+Convert.ToString(thisob)+"</B>";
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_charAt)]
      public static String charAt(Object thisob, double pos){
        String thisStr = Convert.ToString(thisob);
        double position = Convert.ToInteger(pos);
        if (position < 0 || !(position < thisStr.Length))
          return "";
        return thisStr.Substring((int)position, 1);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_charCodeAt)]
      public static Object charCodeAt(Object thisob, double pos){ //This returns an object so that integers stay integers
        String thisStr = Convert.ToString(thisob);
        double position = Convert.ToInteger(pos);
        if (position < 0 || !(position < thisStr.Length))
          return Double.NaN;
        return (int)(thisStr[(int)position]);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject|JSFunctionAttributeEnum.HasVarArgs, JSBuiltin.String_concat)]
      public static String concat(Object thisob, params Object[] args){
        StringBuilder concat = new StringBuilder(Convert.ToString(thisob));
        for (int i = 0; i < args.Length; i++)
          concat.Append(Convert.ToString(args[i]));
        return concat.ToString();
      }

      public static StringConstructor constructor{
        get{
          return StringPrototype._constructor;
        }
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_fixed)]
      public static String @fixed(Object thisob){
        return "<TT>"+Convert.ToString(thisob)+"</TT>";
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_fontcolor)]
      public static String fontcolor(Object thisob, Object colorName){
        String thisStr = Convert.ToString(thisob);
        String colorStr = Convert.ToString(thisob);
        return "<FONT COLOR=\""+colorStr+"\">"+thisStr+"</FONT>";
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_fontsize)]
      public static String fontsize(Object thisob, Object fontSize){
        String thisStr = Convert.ToString(thisob);
        String fontStr = Convert.ToString(fontSize);
        return "<FONT SIZE=\""+fontStr+"\">"+thisStr+"</FONT>";
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_indexOf)]
      public static int indexOf(Object thisob, Object searchString, double position){
        String thisStr = Convert.ToString(thisob);
        String searchStr = Convert.ToString(searchString);
        double startIndex = Convert.ToInteger(position);
        int length = thisStr.Length;
        if (startIndex < 0)
          startIndex = 0;
        if (startIndex >= length)
          return searchStr.Length == 0 ? 0 : -1;
        return thisStr.IndexOf(searchStr, (int)startIndex);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_italics)]
      public static String italics(Object thisob){
        return "<I>"+Convert.ToString(thisob)+"</I>";
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_lastIndexOf)]
      public static int lastIndexOf(Object thisob, Object searchString, double position){
        String thisStr = Convert.ToString(thisob);
        String searchStr = Convert.ToString(searchString);
        int length = thisStr.Length;
        int j = position != position || position > length ? length : (int)position;
        if (j < 0)
          j = 0;
        if (j >= length)
          j = length;
        int slength = searchStr.Length;
        if (slength == 0)
          return j;
        int k = j - 1 + slength;
        if (k >= length)
          k = length-1;
        if (k < 0)
          return -1;
        return thisStr.LastIndexOf(searchStr, k);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_link)]
      public static String link(Object thisob, Object linkRef){
        String thisStr = Convert.ToString(thisob);
        String linkStr = Convert.ToString(linkRef);
        return "<A HREF=\""+linkStr+"\">"+thisStr+"</A>";
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_localeCompare)]
      public static int localeCompare(Object thisob, Object thatob){
        return String.Compare(Convert.ToString(thisob), Convert.ToString(thatob), StringComparison.CurrentCulture);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject|JSFunctionAttributeEnum.HasEngine, JSBuiltin.String_match)]
      public static Object match(Object thisob, VsaEngine engine, Object regExp){
        String thisStr = Convert.ToString(thisob);
        RegExpObject regExpObject = StringPrototype.ToRegExpObject(regExp, engine);
        Match match;
        if (!regExpObject.globalInt){
          match = regExpObject.regex.Match(thisStr);
          if (!match.Success){
            regExpObject.lastIndexInt = 0;
            return DBNull.Value;
          }
          if (regExpObject.regExpConst != null){
            regExpObject.lastIndexInt = regExpObject.regExpConst.UpdateConstructor(regExpObject.regex, match, thisStr);
            return new RegExpMatch(regExpObject.regExpConst.arrayPrototype, regExpObject.regex, match, thisStr);
          }else
            return new RegExpMatch(engine.Globals.globalObject.originalRegExp.arrayPrototype, regExpObject.regex, match, thisStr);
        }
        MatchCollection matches = regExpObject.regex.Matches(thisStr);
        if (matches.Count == 0){
          regExpObject.lastIndexInt = 0;
          return DBNull.Value;
        }
        match = matches[matches.Count - 1];
        regExpObject.lastIndexInt = regExpObject.regExpConst.UpdateConstructor(regExpObject.regex, match, thisStr);
        return new RegExpMatch(
          regExpObject.regExpConst.arrayPrototype, regExpObject.regex, matches, thisStr);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_replace)]
      public static String replace(Object thisob, Object regExp, Object replacement){
        String thisStr = Convert.ToString(thisob);
        RegExpObject regExpObject = regExp as RegExpObject;
        if (regExpObject != null) return StringPrototype.ReplaceWithRegExp(thisStr, regExpObject, replacement);
        Regex regex = regExp as Regex;
        if (regex != null) return StringPrototype.ReplaceWithRegExp(thisStr, new RegExpObject(regex), replacement);
        return StringPrototype.ReplaceWithString(thisStr, Convert.ToString(regExp), Convert.ToString(replacement));
      }

      private static String ReplaceWithRegExp(String thisob, RegExpObject regExpObject, Object replacement){
        RegExpReplace replacer = replacement is ScriptFunction
          ? (RegExpReplace)(new ReplaceUsingFunction(regExpObject.regex, (ScriptFunction)replacement, thisob))
          : (RegExpReplace)(new ReplaceWithString(Convert.ToString(replacement)));
        MatchEvaluator matchEvaluator = new MatchEvaluator(replacer.Evaluate);
        String newString = regExpObject.globalInt
          ? regExpObject.regex.Replace(thisob, matchEvaluator)
          : regExpObject.regex.Replace(thisob, matchEvaluator, 1);
        regExpObject.lastIndexInt = replacer.lastMatch == null
          ? 0
          : regExpObject.regExpConst.UpdateConstructor(regExpObject.regex, replacer.lastMatch, thisob);
        return newString;
      }

      private static String ReplaceWithString(String thisob, String searchString, String replaceString){
        int index = thisob.IndexOf(searchString);
        if (index < 0)
          return thisob;
        StringBuilder newString = new StringBuilder(thisob.Substring(0, index));
        newString.Append(replaceString);
        newString.Append(thisob.Substring(index + searchString.Length));
        return newString.ToString();
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject|JSFunctionAttributeEnum.HasEngine, JSBuiltin.String_search)]
      public static int search(Object thisob, VsaEngine engine, Object regExp){
        String thisStr = Convert.ToString(thisob);
        RegExpObject regExpObject = StringPrototype.ToRegExpObject(regExp, engine);
        Match match = regExpObject.regex.Match(thisStr);
        if (!match.Success){
          regExpObject.lastIndexInt = 0;
          return -1;
        }
        regExpObject.lastIndexInt = regExpObject.regExpConst.UpdateConstructor(regExpObject.regex, match, thisStr);
        return match.Index;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_slice)]
      public static String slice(Object thisob, double start, Object end){
        String thisStr = Convert.ToString(thisob);
        int length = thisStr.Length;
        double startIndex = Convert.ToInteger(start);
        double endIndex = (end == null || end is Missing)
          ? length : Convert.ToInteger(end);
        if (startIndex < 0){
          startIndex = length + startIndex;
          if (startIndex < 0)
            startIndex = 0;
        }else
          if (startIndex > length)
            startIndex = length;
        if (endIndex < 0){
          endIndex = length + endIndex;
          if (endIndex < 0)
            endIndex = 0;
        }else
          if (endIndex > length)
            endIndex = length;
        int nChars = (int)(endIndex - startIndex);
        if (nChars <= 0)
          return "";
        else
          return thisStr.Substring((int)startIndex, nChars);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_small)]
      public static String small(Object thisob){
        return "<SMALL>"+Convert.ToString(thisob)+"</SMALL>";
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject|JSFunctionAttributeEnum.HasEngine, JSBuiltin.String_split)]
      public static ArrayObject split(Object thisob, VsaEngine engine, Object separator, Object limit){
        String thisStr = Convert.ToString(thisob);
        uint limitValue = UInt32.MaxValue;
        if (limit != null && !(limit is Missing) && limit != DBNull.Value){
          double lmt = Convert.ToInteger(limit);
          if (lmt >= 0 && lmt < UInt32.MaxValue)
            limitValue = (uint)lmt;
        }
        if (limitValue == 0)
          return (ArrayObject)engine.GetOriginalArrayConstructor().Construct();
        if (separator == null || separator is Missing){
          ArrayObject array = (ArrayObject)engine.GetOriginalArrayConstructor().Construct();
          array.SetValueAtIndex(0, thisob);
          return array;
        }
        RegExpObject regExpObject = separator as RegExpObject;
        if (regExpObject != null) return StringPrototype.SplitWithRegExp(thisStr, engine, regExpObject, limitValue);
        Regex regex = separator as Regex;
        if (regex != null) return StringPrototype.SplitWithRegExp(thisStr, engine, new RegExpObject(regex), limitValue);
        return StringPrototype.SplitWithString(thisStr, engine, Convert.ToString(separator), limitValue);
      }

      private static ArrayObject SplitWithRegExp(String thisob, VsaEngine engine, RegExpObject regExpObject, uint limit){
        ArrayObject array = (ArrayObject)engine.GetOriginalArrayConstructor().Construct();
        Match match = regExpObject.regex.Match(thisob);

        if (!match.Success){
          array.SetValueAtIndex(0, thisob);
          regExpObject.lastIndexInt = 0;
          return array;
        }


        Match lastMatch;
        int prevIndex = 0;
        uint i = 0;

        do{
          int len = match.Index - prevIndex;
          if (len > 0)
          {
            array.SetValueAtIndex(i++, thisob.Substring(prevIndex, len));
            if (limit > 0 && i >= limit){
              regExpObject.lastIndexInt = regExpObject.regExpConst.UpdateConstructor(regExpObject.regex, match, thisob);
              return array;
            }
          }


          prevIndex = match.Index + match.Length;
          lastMatch = match;
          match = match.NextMatch();
        }while(match.Success);

        if (prevIndex < thisob.Length)
          array.SetValueAtIndex(i, thisob.Substring(prevIndex));
        regExpObject.lastIndexInt = regExpObject.regExpConst.UpdateConstructor(regExpObject.regex, lastMatch, thisob);

        return array;
      }

      private static ArrayObject SplitWithString(String thisob, VsaEngine engine, String separator, uint limit){
        ArrayObject array = (ArrayObject)engine.GetOriginalArrayConstructor().Construct();
        if (separator.Length == 0){
          if (limit > thisob.Length)
            limit = (uint)thisob.Length;
          for (int i = 0; i < limit; i++)
            array.SetValueAtIndex((uint)i, thisob[i].ToString());
        }else{
          int prevIndex = 0;
          uint i = 0;
          int index;
          while ((index = thisob.IndexOf(separator, prevIndex)) >= 0){
            array.SetValueAtIndex(i++, thisob.Substring(prevIndex, index-prevIndex));
            if (i >= limit)
              return array;
            prevIndex = index + separator.Length;
          }
          if (i == 0)
            array.SetValueAtIndex(0, thisob);
          else
            array.SetValueAtIndex(i, thisob.Substring(prevIndex));
        }
        return array;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_strike)]
      public static String strike(Object thisob){
        return "<STRIKE>"+Convert.ToString(thisob)+"</STRIKE>";
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_sub)]
      public static String sub(Object thisob){
        return "<SUB>"+Convert.ToString(thisob)+"</SUB>";
      }

      [NotRecommended("substr")]
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_substr)]
      public static String substr(Object thisob, double start, Object count){
        String thisStr = thisob as String;
        if (thisStr == null) thisStr = Convert.ToString(thisob);
        int length = thisStr.Length;
        double startIndex = Convert.ToInteger(start);
        if (startIndex < 0)
          startIndex += length;
        if (startIndex < 0)
          startIndex = 0;
        else if (startIndex > length)
          startIndex = length;
        int nChars = count is int ? (int)count :
          ((count == null || count is Missing) ? length-(int)Runtime.DoubleToInt64(startIndex) : (int)Runtime.DoubleToInt64(Convert.ToInteger(count)));
        if (startIndex+nChars > length)
          nChars = length - (int)startIndex;
        if (nChars <= 0)
          return "";
        else
          return thisStr.Substring((int)startIndex, nChars);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_substring)]
      public static String substring(Object thisob, double start, Object end){
        String thisStr = thisob as String;
        if (thisStr == null) thisStr = Convert.ToString(thisob);
        int length = thisStr.Length;
        double startIndex = Convert.ToInteger(start);
        if (startIndex < 0)
          startIndex = 0;
        else if (startIndex > length)
          startIndex = length;
        double endIndex = (end == null || end is Missing)
          ? length : Convert.ToInteger(end);
        if (endIndex < 0)
          endIndex = 0;
        else if (endIndex > length)
          endIndex = length;
        if (startIndex > endIndex){
          double temp = startIndex;
          startIndex = endIndex;
          endIndex = temp;
        }
        return thisStr.Substring((int)startIndex, (int)(endIndex - startIndex));
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_sup)]
      public static String sup(Object thisob){
        return "<SUP>"+Convert.ToString(thisob)+"</SUP>";
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_toLocaleLowerCase)]
      public static String toLocaleLowerCase(Object thisob){
        return Convert.ToString(thisob).ToLower(CultureInfo.CurrentUICulture);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_toLocaleUpperCase)]
      public static String toLocaleUpperCase(Object thisob){
        return Convert.ToString(thisob).ToUpperInvariant();
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_toLowerCase)]
      public static String toLowerCase(Object thisob){
        return Convert.ToString(thisob).ToLowerInvariant();
      }

      private static RegExpObject ToRegExpObject(Object regExp, VsaEngine engine){
        if (regExp == null || regExp is Missing)
          return (RegExpObject)engine.GetOriginalRegExpConstructor().Construct("", false, false, false);
        RegExpObject result = regExp as RegExpObject;
        if (result != null) return result;
        Regex regex = regExp as Regex;
        if (regex != null) return new RegExpObject(regex);
        return (RegExpObject)engine.GetOriginalRegExpConstructor().Construct(Convert.ToString(regExp), false, false, false);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_toString)]
      public static String toString(Object thisob){
        StringObject strob = thisob as StringObject;
        if (strob != null) return strob.value;
        ConcatString concatStr = thisob as ConcatString;
        if (concatStr != null) return concatStr.ToString();
        IConvertible ic = Convert.GetIConvertible(thisob);
        if (Convert.GetTypeCode(thisob, ic) == TypeCode.String) return ic.ToString(null);
        throw new JScriptException(JSError.StringExpected);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_toUpperCase)]
      public static String toUpperCase(Object thisob){
        return Convert.ToString(thisob).ToUpperInvariant();
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.String_valueOf)]
      public static Object valueOf(Object thisob){
        return StringPrototype.toString(thisob);
      }
    }
}
