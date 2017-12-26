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

    internal sealed class JSKeyword{
      private JSKeyword next;
      private JSToken token;
      private string name;
      private int length;

      private JSKeyword(JSToken token, string name){
        this.name = name;
        this.next = null;
        this.token = token;
        this.length = this.name.Length;
      }

      private JSKeyword(JSToken token, string name, JSKeyword next){
        this.name = name;
        this.next = next;
        this.token = token;
        this.length = this.name.Length;
      }

      internal static string CanBeIdentifier(JSToken keyword){
        switch (keyword){
          case JSToken.Abstract: return "abstract";
          case JSToken.Assert: return "assert";
          case JSToken.Boolean: return "boolean";
          case JSToken.Byte: return "byte";
          case JSToken.Char: return "char";
          case JSToken.Decimal: return "decimal";
          case JSToken.Double: return "double";
          case JSToken.Ensure: return "ensure";
          case JSToken.Enum: return "enum";
          case JSToken.Event: return "event";
          case JSToken.Final: return "final";
          case JSToken.Float: return "float";
          case JSToken.Get: return "get";
          case JSToken.Goto: return "goto";
          case JSToken.Implements: return "implements";
          case JSToken.Int: return "int";
          case JSToken.Interface: return "interface";
          case JSToken.Internal: return "internal";
          case JSToken.Invariant: return "invariant";
          case JSToken.Long: return "long";
          case JSToken.Namespace: return "namespace";
          case JSToken.Native: return "native";
          case JSToken.Package: return "package";
          case JSToken.Private: return "private";
          case JSToken.Protected: return "protected";
          case JSToken.Public: return "public";
          case JSToken.Require: return "require";
          case JSToken.Sbyte: return "sbyte";
          case JSToken.Set: return "set";
          case JSToken.Short: return "short";
          case JSToken.Static: return "static";
          case JSToken.Synchronized: return "synchronized";
          case JSToken.Throws: return "throws";
          case JSToken.Transient: return "transient";
          case JSToken.Void: return "void";
          case JSToken.Volatile: return "volatile";
          case JSToken.Uint : return "uint";
          case JSToken.Ulong : return "ulong";
          case JSToken.Ushort : return "ushort";
          case JSToken.Use : return "use";
          default: return null;
        }
      }

      internal JSToken GetKeyword(Context token, int length){
        JSKeyword keyword = this;

      nextToken:
        while (null != keyword){
          if (length == keyword.length){
            // we know the first char has to match
            for (int i = 1, j = token.startPos + 1; i < length; i++, j++){
              char ch1 = keyword.name[i];
              char ch2 = token.source_string[j];
              if (ch1 == ch2)
                continue;
              else if (ch2 < ch1)
                return JSToken.Identifier;
              else{
                keyword = keyword.next;
                goto nextToken;
              }
            }
            return keyword.token;
          }else if (length < keyword.length)
            return JSToken.Identifier;

          keyword = keyword.next;
        }
        return JSToken.Identifier;
      }

      internal static JSKeyword[] InitKeywords(){
        JSKeyword[] keywords = new JSKeyword[26];
        JSKeyword keyword;
        // a
        keyword = new JSKeyword(JSToken.Abstract, "abstract");
        keyword = new JSKeyword(JSToken.Assert, "assert", keyword);
        keywords['a' - 'a'] = keyword;
        // b
        keyword = new JSKeyword(JSToken.Boolean, "boolean");
        keyword = new JSKeyword(JSToken.Break, "break", keyword);
        keyword = new JSKeyword(JSToken.Byte, "byte", keyword);
        keywords['b' - 'a'] = keyword;
        // c
        keyword = new JSKeyword(JSToken.Continue, "continue");
        keyword = new JSKeyword(JSToken.Const, "const", keyword);
        keyword = new JSKeyword(JSToken.Class, "class", keyword);
        keyword = new JSKeyword(JSToken.Catch, "catch", keyword);
        keyword = new JSKeyword(JSToken.Char, "char", keyword);
        keyword = new JSKeyword(JSToken.Case, "case", keyword);
        keywords['c' - 'a'] = keyword;
        // d
        keyword= new JSKeyword(JSToken.Debugger, "debugger");
        keyword = new JSKeyword(JSToken.Default, "default", keyword);
        keyword = new JSKeyword(JSToken.Double, "double", keyword);
        keyword = new JSKeyword(JSToken.Delete, "delete", keyword);
        keyword = new JSKeyword(JSToken.Do, "do", keyword);
        keywords['d' - 'a'] = keyword;
        // e
        keyword = new JSKeyword(JSToken.Extends, "extends");
        keyword = new JSKeyword(JSToken.Export, "export", keyword);
        keyword = new JSKeyword(JSToken.Ensure, "ensure", keyword);
        keyword = new JSKeyword(JSToken.Event, "event", keyword);
        keyword = new JSKeyword(JSToken.Enum, "enum", keyword);
        keyword = new JSKeyword(JSToken.Else, "else", keyword);
        keywords['e' - 'a'] = keyword;
        // f
        keyword = new JSKeyword(JSToken.Function, "function");
        keyword = new JSKeyword(JSToken.Finally, "finally", keyword);
        keyword = new JSKeyword(JSToken.Float, "float", keyword);
        keyword = new JSKeyword(JSToken.Final, "final", keyword);
        keyword = new JSKeyword(JSToken.False, "false", keyword);
        keyword = new JSKeyword(JSToken.For, "for", keyword);
        keywords['f' - 'a'] = keyword;
        // g
        keyword = new JSKeyword(JSToken.Goto, "goto");
        keyword = new JSKeyword(JSToken.Get, "get", keyword);
        keywords['g' - 'a'] = keyword;
        // i
        keyword = new JSKeyword(JSToken.Instanceof, "instanceof");
        keyword = new JSKeyword(JSToken.Implements, "implements", keyword);
        keyword = new JSKeyword(JSToken.Invariant, "invariant", keyword);
        keyword = new JSKeyword(JSToken.Interface, "interface", keyword);
        keyword = new JSKeyword(JSToken.Internal, "internal", keyword);
        keyword = new JSKeyword(JSToken.Import, "import", keyword);
        keyword = new JSKeyword(JSToken.Int, "int", keyword);
        keyword = new JSKeyword(JSToken.In, "in", keyword);
        keyword = new JSKeyword(JSToken.If, "if", keyword);
        keywords['i' - 'a'] = keyword;
        // l
        keyword = new JSKeyword(JSToken.Long, "long");
        keywords['l' - 'a'] = keyword;
        // n
        keyword = new JSKeyword(JSToken.Namespace, "namespace");
        keyword = new JSKeyword(JSToken.Native, "native", keyword);
        keyword = new JSKeyword(JSToken.Null, "null", keyword);
        keyword = new JSKeyword(JSToken.New, "new", keyword);
        keywords['n' - 'a'] = keyword;
        // p
        keyword = new JSKeyword(JSToken.Protected, "protected");
        keyword = new JSKeyword(JSToken.Private, "private", keyword);
        keyword = new JSKeyword(JSToken.Package, "package", keyword);
        keyword = new JSKeyword(JSToken.Public, "public", keyword);
        keywords['p' - 'a'] = keyword;
        // r
        keyword = new JSKeyword(JSToken.Require, "require");
        keyword = new JSKeyword(JSToken.Return, "return", keyword);
        keywords['r' - 'a'] = keyword;
        // s
        keyword = new JSKeyword(JSToken.Synchronized, "synchronized");
        keyword = new JSKeyword(JSToken.Switch, "switch", keyword);
        keyword = new JSKeyword(JSToken.Static, "static", keyword);
        keyword = new JSKeyword(JSToken.Super, "super", keyword);
        keyword = new JSKeyword(JSToken.Short, "short", keyword);
        keyword = new JSKeyword(JSToken.Set, "set", keyword);
        keywords['s' - 'a'] = keyword;
        // t
        keyword = new JSKeyword(JSToken.Transient, "transient");
        keyword = new JSKeyword(JSToken.Typeof, "typeof", keyword);
        keyword = new JSKeyword(JSToken.Throws, "throws", keyword);
        keyword = new JSKeyword(JSToken.Throw, "throw", keyword);
        keyword = new JSKeyword(JSToken.True, "true", keyword);
        keyword = new JSKeyword(JSToken.This, "this", keyword);
        keyword = new JSKeyword(JSToken.Try, "try", keyword);
        keywords['t' - 'a'] = keyword;
        // v
        keyword = new JSKeyword(JSToken.Volatile, "volatile");
        keyword = new JSKeyword(JSToken.Void, "void", keyword);
        keyword = new JSKeyword(JSToken.Var, "var", keyword);
        keywords['v' - 'a'] = keyword;
        // u
        keyword = new JSKeyword(JSToken.Use, "use");
        keywords['u' - 'a'] = keyword;
        // w
        keyword = new JSKeyword(JSToken.While, "while");
        keyword = new JSKeyword(JSToken.With, "with", keyword);
        keywords['w' - 'a'] = keyword;

        return keywords;
      }

    }
}    