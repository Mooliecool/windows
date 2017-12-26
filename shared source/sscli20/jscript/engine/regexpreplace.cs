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
    using System.Text.RegularExpressions;

    internal abstract class RegExpReplace {
      internal Match lastMatch;

      internal RegExpReplace() {
        this.lastMatch = null;
      }

      internal abstract String Evaluate(Match match);
    }

    internal class ReplaceUsingFunction : RegExpReplace {
      private ScriptFunction function;
      private int cArgs;
      private int[] groupNumbers;
      private String source;

      internal ReplaceUsingFunction(Regex regex, ScriptFunction function, String source) {
        this.function = function;
        this.cArgs = function.GetNumberOfFormalParameters();
        bool hasArgumentsObject = (function is Closure) && ((Closure)function).func.hasArgumentsObject;
        this.groupNumbers = null;
        this.source = source;
        if (this.cArgs > 1 || hasArgumentsObject) {
          String[] groupNames = regex.GetGroupNames();
          int cGroupNumbers = groupNames.Length - 1;
          if (hasArgumentsObject) this.cArgs = cGroupNumbers+3;
          if (cGroupNumbers > 0) {
            if (cGroupNumbers > this.cArgs - 1)
              cGroupNumbers = this.cArgs - 1;
            this.groupNumbers = new int[cGroupNumbers];
            for (int i = 0; i < cGroupNumbers; i++)
              this.groupNumbers[i] = regex.GroupNumberFromName(groupNames[i+1]);
          }
        }
      }

      internal override String Evaluate(Match match) {
        this.lastMatch = match;
        Object[] args = new Object[this.cArgs];
        if (this.cArgs > 0) {
          args[0] = match.ToString();
          if (this.cArgs > 1) {
            int i = 1;
            if (this.groupNumbers != null)
              for (; i <= this.groupNumbers.Length; i++) {
                Group group = match.Groups[this.groupNumbers[i-1]];
                args[i] = group.Success ? group.ToString() : null;
              }
            if (i < this.cArgs) {
              args[i++] = match.Index;
              if (i < this.cArgs) {
                args[i++] = this.source;
                for (; i < this.cArgs; i++)
                  args[i] = null;
              }
            }
          }
        }
        Object result = this.function.Call(args, null);
        return match.Result(result is Empty ? "" : Convert.ToString(result));
      }
    }

    internal class ReplaceWithString : RegExpReplace {
      private String replaceString;

      internal ReplaceWithString(String replaceString) {
        this.replaceString = replaceString;
      }

      internal override String Evaluate(Match match) {
        this.lastMatch = match;
        return match.Result(replaceString);
      }
    }
}
