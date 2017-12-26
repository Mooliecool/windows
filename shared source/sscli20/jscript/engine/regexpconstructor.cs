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
    using System.Diagnostics;
    using System.Reflection;
    using System.Text.RegularExpressions;

    public sealed class RegExpConstructor : ScriptFunction {
      internal static readonly RegExpConstructor ob = new RegExpConstructor();

      private RegExpPrototype originalPrototype;
      internal ArrayPrototype arrayPrototype;
      private Regex regex;
      private Match lastRegexMatch;
      internal Object inputString;
      private String lastInput;

      internal RegExpConstructor()
        : base(FunctionPrototype.ob, "RegExp", 2) {
        this.originalPrototype = RegExpPrototype.ob;
        RegExpPrototype._constructor = this;
        this.proto = RegExpPrototype.ob;
        this.arrayPrototype = ArrayPrototype.ob;
        this.regex = null;
        this.lastRegexMatch = null;
        this.inputString = "";
        this.lastInput = null;
      }

      internal RegExpConstructor(LenientFunctionPrototype parent,
          LenientRegExpPrototype prototypeProp, LenientArrayPrototype arrayPrototype)
        : base(parent, "RegExp", 2) {
        this.originalPrototype = prototypeProp;
        prototypeProp.constructor = this;
        this.proto = prototypeProp;
        this.arrayPrototype = arrayPrototype;
        this.regex = null;
        this.lastRegexMatch = null;
        this.inputString = "";
        this.lastInput = null;
        this.noExpando = false;
      }

      internal override Object Call(Object[] args, Object thisob) {
        return this.Invoke(args);
      }

      internal override Object Construct(Object[] args) {
        return this.CreateInstance(args);
      }

      private RegExpObject ConstructNew(Object[] args) {
        String source = args.Length > 0 && args[0] != null ? Convert.ToString(args[0]) : "";
        if (args.Length > 0 && args[0] is Regex)
          throw new JScriptException(JSError.TypeMismatch);
        bool ignoreCase = false;
        bool global     = false;
        bool multiline  = false;
        if (args.Length >= 2 && args[1] != null) {
          String flags = Convert.ToString(args[1]);
          for (int i = 0; i < flags.Length; i++) {
            switch (flags[i]) {
              case 'i':
                ignoreCase = true;
                continue;
              case 'g':
                global = true;
                continue;
              case 'm':
                multiline = true;
                continue;
            }
            throw new JScriptException(JSError.RegExpSyntax);
          }
        }
        return new RegExpObject(this.originalPrototype,
          source, ignoreCase, global, multiline, this);
      }

      public Object Construct(String pattern, bool ignoreCase, bool global, bool multiline) {
        return new RegExpObject(this.originalPrototype,
          pattern, ignoreCase, global, multiline, this);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public new RegExpObject CreateInstance(params Object[] args){
        RegExpObject regExpObject;
        if (args == null || args.Length <= 0 || (regExpObject = args[0] as RegExpObject) == null)
          return ConstructNew(args);
        if (args.Length > 1 && args[1] != null)
          throw new JScriptException(JSError.RegExpSyntax);
        return new RegExpObject(this.originalPrototype, regExpObject.source,
          regExpObject.ignoreCase, regExpObject.global, regExpObject.multiline, this);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public RegExpObject Invoke(params Object[] args){
        RegExpObject regExpObject;
        if (args == null || args.Length <= 0 || (regExpObject = args[0] as RegExpObject) == null)
          return ConstructNew(args);
        if (args.Length > 1 && args[1] != null)
          throw new JScriptException(JSError.RegExpSyntax);
        return regExpObject;
      }

      private Object GetIndex() {
        return this.lastRegexMatch == null ? -1 : this.lastRegexMatch.Index;
      }

      private Object GetInput() {
        return this.inputString;
      }

      private Object GetLastIndex() {
        return this.lastRegexMatch == null ? -1
          : this.lastRegexMatch.Length == 0 ? this.lastRegexMatch.Index + 1
          : this.lastRegexMatch.Index + this.lastRegexMatch.Length;
      }

      private Object GetLastMatch() {
        return this.lastRegexMatch == null ? "" : this.lastRegexMatch.ToString();
      }

      private Object GetLastParen() {
        if (this.regex == null || this.lastRegexMatch == null)
          return "";
        String[] groupNames = this.regex.GetGroupNames();
        if (groupNames.Length <= 1)
          return "";
        int lastGroupNumber = this.regex.GroupNumberFromName(
          groupNames[groupNames.Length - 1]);
        Group group = this.lastRegexMatch.Groups[lastGroupNumber];
        return group.Success ? group.ToString() : "";
      }

      private Object GetLeftContext() {
        return this.lastRegexMatch == null || this.lastInput == null ? "" : this.lastInput.Substring(0, this.lastRegexMatch.Index);
      }

      internal override Object GetMemberValue(String name) {
        if (name.Length == 2 && name[0] == '$') {
          char ch = name[1];
          switch (ch) {
            case '1': case '2': case '3':
            case '4': case '5': case '6':
            case '7': case '8': case '9': {
              if (this.lastRegexMatch == null)
                return "";
              Group group = this.lastRegexMatch.Groups[ch.ToString()];
              return group.Success ? group.ToString() : "";
            }
            case '`':
              return this.GetLeftContext();
            case '\'':
              return this.GetRightContext();
            case '&':
              return this.GetLastMatch();
            case '+':
              return this.GetLastParen();
            case '_':
              return this.GetInput();
          }
        }
        return base.GetMemberValue(name);
      }

      private Object GetRightContext() {
        return this.lastRegexMatch == null || this.lastInput == null
          ? ""
          : this.lastInput.Substring(this.lastRegexMatch.Index + this.lastRegexMatch.Length);
      }

      private void SetInput(Object value) {
        this.inputString = value;
      }

      internal override void SetMemberValue(String name, Object value) {
        if (this.noExpando)
          throw new JScriptException(JSError.AssignmentToReadOnly);
        if (name.Length == 2 && name[0] == '$') {
          switch (name[1]) {
            case '1': case '2': case '3':
            case '4': case '5': case '6':
            case '7': case '8': case '9':
            case '`':
            case '\'':
            case '&':
            case '+':
              return;
            case '_':
              this.SetInput(value);
              return;
          }
        }
        base.SetMemberValue(name, value);
      }

      internal int UpdateConstructor(Regex regex, Match match, String input) {
        if (!this.noExpando) {
          this.regex = regex;
          this.lastRegexMatch = match;
          this.inputString = input;
          this.lastInput = input;
        }
        return match.Length == 0
          ? match.Index + 1
          : match.Index + match.Length;
      }

      public Object index {
        get { return this.GetIndex(); }
      }

      public Object input {
        get {
          return this.GetInput();
        }
        set {
          if (this.noExpando)
            throw new JScriptException(JSError.AssignmentToReadOnly);
          this.SetInput(value);
        }
      }

      public Object lastIndex {
        get { return this.GetLastIndex(); }
      }

      public Object lastMatch {
        get { return this.GetLastMatch(); }
      }

      public Object lastParen {
        get { return this.GetLastParen(); }
      }

      public Object leftContext {
        get { return this.GetLeftContext(); }
      }

      public Object rightContext {
        get { return this.GetRightContext(); }
      }
    }
}
