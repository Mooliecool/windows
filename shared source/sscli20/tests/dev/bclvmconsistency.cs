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
/*=============================================================================
**
** Source: bclvmconsistency.cs
**
** Purpose: Dummy test for BCL-VM consistency check - complements bclvmconsistency.pl
**
=============================================================================*/

using System;
using System.Reflection;

[assembly: AssemblyVersion("1.0.*")]

class MainApp {

   public static int Main() {
      if (Environment.GetEnvironmentVariable("COMPlus_ConsistencyCheck") != "1") {      
          Console.WriteLine("Need to run with COMPlus_ConsistencyCheck=1");
          return -1;
      }

      Console.WriteLine("Passed");
      return 0;
   }
}
