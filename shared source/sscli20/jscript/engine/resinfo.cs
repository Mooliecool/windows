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

namespace Microsoft.JScript.Vsa {

    using System;
    using System.Globalization;
    using System.IO;

    // ResInfo is the argument supplied to the /resource or /linkresource switches
    // on the command line.  The argument is required to be in the following format:
    //   <filename>[,<name>[,public|private]]
    // Since <filename> can legally contain commas, we require that <name> does not
    // contain commas (should be a valid identifier), and <name> cannot be "public"
    // or "private".  That way, we can use commas as delimiters, check the optional
    // parts first, and use the remainder as the filename.
    [Obsolete(VsaObsolete.Description)]
    public class ResInfo{
      public string filename;
      public string fullpath;
      public string name;
      public bool isPublic;
      public bool isLinked;

      public ResInfo(string filename, string name, bool isPublic, bool isLinked){
        this.filename = filename;
        this.fullpath = Path.GetFullPath(filename);
        this.name = name;
        this.isPublic = isPublic;
        this.isLinked = isLinked;
      }

      public ResInfo(string resinfo, bool isLinked){
        // resinfo: <filename>[,<name>[,public|private]]
        string[] info = resinfo.Split(new char[] {','});
        int numParts = info.Length;

        // set the defaults
        this.filename = info[0];
        this.name = Path.GetFileName(this.filename);
        this.isPublic = true;
        this.isLinked = isLinked;

        if (numParts == 2){
          this.name = info[1];
        }else if (numParts > 2){
          // check the last part for "public" or "private", then the next part is the name, and
          // the rest should be rejoined as the filename (commas are valid filename characters)
          bool flagPresent = false;
          if (String.Compare(info[numParts-1], "public", StringComparison.OrdinalIgnoreCase) == 0){
            flagPresent = true;
          }else if (String.Compare(info[numParts-1], "private", StringComparison.OrdinalIgnoreCase) == 0){
            this.isPublic = false;
            flagPresent = true;
          }
          this.name = info[numParts - (flagPresent? 2: 1)];
          // this.filename is made up of the rest...
          this.filename = String.Join(",", info, 0, numParts - (flagPresent? 2: 1));
        }
        this.fullpath = Path.GetFullPath(this.filename);
      }
    }
}