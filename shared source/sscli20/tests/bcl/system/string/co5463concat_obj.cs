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
using GenStrings;
using System.Reflection;
using System;
using System.IO;
public class Co5463Concat_obj
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "String.Concat(Object)";
 public static String s_strTFName        = "Co5463Concat_obj.";
 public static String s_strTFAbbrev      = "Co5463";
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
     String str1;
     Object obj1;
     strLoc = "Loc_299cu";
     obj1 = null;
     iCountTestcases++;
     if(String.Concat(obj1) != String.Empty)
       {
       iCountErrors++;
       printerr( "Error_2029s! String.Empty not returned as expected");
       }
     IntlStrings intl = new IntlStrings();
     str1 = intl.GetString(20, false, true);
     iCountTestcases++;
     if(!String.Concat(str1).Equals((object)str1))
       {
       iCountErrors++;
       printerr( "Error_209sj! got value=="+String.Concat(str1));
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
   Co5463Concat_obj cbA = new Co5463Concat_obj();
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
