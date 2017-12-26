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
    using System.Collections.Specialized;
    using System.Globalization;

    /**************************************************************
        Simple option        
            e.g. /nologo
            argument: N/A
            
        Option with argument
            e.g. /out:<filename>
            argument: <filename>
            
        Boolean option
            /debug[+|-]
            argument: bool (true for +, false for -)
    **************************************************************/
    public class CmdLineOptionParser{
      public static bool IsSimpleOption(string option, string prefix){
        return (String.Compare(option, prefix, StringComparison.OrdinalIgnoreCase) == 0) ? true : false;
      }

      public static string IsArgumentOption(string option, string prefix){
        int prefixLength = prefix.Length;
        if (option.Length < prefixLength || String.Compare(option, 0, prefix, 0, prefixLength, StringComparison.OrdinalIgnoreCase) != 0)
          return null;
        if (option.Length == prefixLength)
          return "";
        if (':' != option[prefixLength])
          return null;
        return option.Substring(prefixLength+1);
      }

      public static string IsArgumentOption(string option, string shortPrefix, string longPrefix){
        string argument = IsArgumentOption(option, shortPrefix);
        if (argument == null)
          argument = IsArgumentOption(option, longPrefix);
        return argument;
      }

      public static object IsBooleanOption(string option, string prefix){
        int prefixLength = prefix.Length;
        if (option.Length < prefix.Length || String.Compare(option, 0, prefix, 0, prefixLength, StringComparison.OrdinalIgnoreCase) != 0)
          return null;
        if (option.Length == prefixLength)
          return true;
        if (option.Length != prefixLength+1)
          return null;
        if ('-' == option[prefixLength])
          return false;
        if ('+' == option[prefixLength])
          return true;
        return null;
      }

      public static object IsBooleanOption(string option, string shortPrefix, string longPrefix){
        object argument = IsBooleanOption(option, shortPrefix);
        if (argument == null)
          argument = IsBooleanOption(option, longPrefix);
        return argument;
      }

    }
}