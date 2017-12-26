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
    using System.Text;
    using System.Text.RegularExpressions;

    public sealed class RegExpObject : JSObject{
      internal RegExpConstructor regExpConst;
      private String sourceInt;
      internal bool ignoreCaseInt, globalInt, multilineInt;
      internal Regex regex;
      internal Object lastIndexInt;

      internal RegExpObject(RegExpPrototype parent, String source,
          bool ignoreCase, bool global, bool multiline, RegExpConstructor regExpConst)
        : base(parent){
        this.regExpConst = regExpConst;
        this.sourceInt = source;
        this.ignoreCaseInt = ignoreCase;
        this.globalInt = global;
        this.multilineInt = multiline;

        RegexOptions flags = RegexOptions.CultureInvariant | RegexOptions.ECMAScript;
        if (ignoreCase)
          flags |= RegexOptions.IgnoreCase;
        if (multiline)
          flags |= RegexOptions.Multiline;
        try{
          this.regex = new Regex(source, flags);
        }catch (System.ArgumentException){
          throw new JScriptException(JSError.RegExpSyntax);
        }
        this.lastIndexInt = 0;
        this.noExpando = false;
      }

      //Only to be used by StringPrototype
      internal RegExpObject(Regex regex)
        : base(null){
        this.regExpConst = null;
        this.sourceInt = "";
        this.ignoreCaseInt = (regex.Options & RegexOptions.IgnoreCase) != 0;
        this.globalInt = false;
        this.multilineInt = (regex.Options & RegexOptions.Multiline) != 0;
        this.regex = regex;
        this.lastIndexInt = 0;
        this.noExpando = true;
      }

      internal RegExpObject compile(String source, String flags){
        this.sourceInt = source;
        this.ignoreCaseInt = this.globalInt = this.multilineInt = false;
        RegexOptions regexFlags = RegexOptions.CultureInvariant | RegexOptions.ECMAScript;
        for (int i = 0; i < flags.Length; i++)
          switch (flags[i]){
            case 'i':
              if (this.ignoreCaseInt)
                throw new JScriptException(JSError.RegExpSyntax);
              this.ignoreCaseInt = true;
              regexFlags |= RegexOptions.IgnoreCase;
              break;
            case 'g':
              if (this.globalInt)
                throw new JScriptException(JSError.RegExpSyntax);
              this.globalInt = true;
              break;
            case 'm':
              if (this.multilineInt)
                throw new JScriptException(JSError.RegExpSyntax);
              this.multilineInt = true;
              regexFlags |= RegexOptions.Multiline;
              break;
            default:
              throw new JScriptException(JSError.RegExpSyntax);
          }
        try{
          this.regex = new Regex(source, regexFlags);
        }catch (System.ArgumentException){
          throw new JScriptException(JSError.RegExpSyntax);
        }
        return this;
      }

      internal Object exec(String input){
        Match match = null;
        if (!this.globalInt)
          match = this.regex.Match(input);
        else{
          int lastIndex = (int)Runtime.DoubleToInt64(Convert.ToInteger(this.lastIndexInt));
          if (lastIndex <= 0)
            match = this.regex.Match(input);
          else if (lastIndex <= input.Length)
            match = this.regex.Match(input, lastIndex);
        }
        if (match == null || !match.Success){
          this.lastIndexInt = 0;
          return DBNull.Value;
        }
        this.lastIndexInt = this.regExpConst.UpdateConstructor(this.regex, match, input);
        return new RegExpMatch(this.regExpConst.arrayPrototype, this.regex, match, input);
      }

      internal override String GetClassName(){
        return "RegExp";
      }

      internal bool test(String input){
        Match match = null;
        if (!this.globalInt)
          match = this.regex.Match(input);
        else{
          int lastIndex = (int)Runtime.DoubleToInt64(Convert.ToInteger(this.lastIndexInt));
          if (lastIndex <= 0)
            match = this.regex.Match(input);
          else if (lastIndex <= input.Length)
            match = this.regex.Match(input, lastIndex);
        }
        if (match == null || !match.Success){
          this.lastIndexInt = 0;
          return false;
        }
        this.lastIndexInt = this.regExpConst.UpdateConstructor(this.regex, match, input);
        return true;
      }

      public override String ToString(){
        return "/" + this.sourceInt + "/"
          + (this.ignoreCaseInt ? "i" : "")
          + (this.globalInt ? "g" : "")
          + (this.multilineInt ? "m" : "");
      }

      public String source{
        get{
          return this.sourceInt;
        }
      }

      public bool ignoreCase{
        get{
          return this.ignoreCaseInt;
        }
      }

      public bool global{
        get{
          return this.globalInt;
        }
      }

      public bool multiline{
        get{
          return this.multilineInt;
        }
      }

      public Object lastIndex{
        get{
          return this.lastIndexInt;
        }
        set{
          this.lastIndexInt = value;
        }
      }
    }
}
