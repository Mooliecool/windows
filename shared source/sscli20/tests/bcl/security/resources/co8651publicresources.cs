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
using System;
using System.IO;
using System.Resources;
class Co8651PublicResources
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Testing security in resources";
 public static String s_strTFName        = "Co8651PublicResources.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 Boolean runTest()
   {
   ResourceManager manager = new ResourceManager("co8651publicresources", this.GetType().Assembly);
   if (manager == null)
     return false;
   if (manager.GetString("1")!= "2")
     return false;
   Console.WriteLine("pass");
   Environment.ExitCode = 0;
   return true;
   }
 public static void Main()
   {
   Boolean bResult = false;
   Co8651PublicResources cbA = new Co8651PublicResources();
   try 
     {
     bResult = cbA.runTest();
     } 
   catch (Exception exc_main)
     {
     bResult = false;
     Console.WriteLine(s_strTFAbbrev+ "FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
     }
   if (!bResult)
     {
     Console.WriteLine(s_strTFName+ s_strTFPath);
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.Error.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
