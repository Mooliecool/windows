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
using System.Globalization; 
using System.Reflection;
using GenStrings;
using System;
using System.IO;
public class Co5465Concat_ooo
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "String.Concat(Object)";
 public static String s_strTFName        = "Co5465Concat_ooo.";
 public static String s_strTFAbbrev      = "Co5465";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   try
     {
     strLoc = "Loc_299cu";
     iCountTestcases++;
     if(!String.Concat(null,null,null).Equals(String.Empty))
       {
       iCountErrors++;
       printerr( "Error_2029s! String.Empty not returned as expected");
       }
     strLoc = "Loc_109xh";
     iCountTestcases++;
     if(!String.Concat("Hello","World","End").Equals("HelloWorldEnd"))
       {
       iCountErrors++;
       printerr( "Error_209sj! Expected==HelloWorldEnd");
       }
     iCountTestcases++;
     if(!String.Concat("Hi", null, "There").Equals("HiThere"))
       {
       iCountErrors++;
       printerr( "Error_2091x! Expected==HiThere");
       }
     iCountTestcases++;
     if(!String.Concat(null, "Hi", "There").Equals("HiThere"))
       {
       iCountErrors++;
       printerr( "Error_099xj! Expected==HiThere");
       }
     iCountTestcases++;
     if(!String.Concat("Hi", "There", null).Equals("HiThere"))
       {
       iCountErrors++;
       printerr( "Error_1909x! Expected==HiThere");
       }
     IntlStrings intl = new IntlStrings();
     String str1 = intl.GetString(3, true, true);
     String str2 = intl.GetString(5, true, true);
     if(!String.Concat(str1,str2).Equals(String.Concat(str1, str2, null)))
       {
       iCountErrors++;
       printerr( "Error_209sj! Expected==HelloWorld");
       }
     } catch (Exception exc_general ) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public void printerr ( String err )
   {
   Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public void printinfo ( String info )
   {
   Console.WriteLine ("EXTENDEDINFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co5465Concat_ooo cbA = new Co5465Concat_ooo();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
