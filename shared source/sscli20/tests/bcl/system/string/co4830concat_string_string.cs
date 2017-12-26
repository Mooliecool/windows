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
using System;
using System.IO;
using System.Collections;
public class co4830Concat_string_string
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "String(string, string, string)";
 public static String s_strTFName        = "co4830Concat_string_string";
 public static String s_strTFAbbrev      = "Co5504";
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
     String str1 = String.Empty, str2 = String.Empty, str3 = String.Empty;
     String str = null;
     strLoc = "Loc_498hv";
     iCountTestcases++;
     str = String.Concat (str1, str2);
     if(!str.Equals (String.Empty))
       {
       iCountErrors++;
       printerr( "Error_498ch! incorrect string returned for null argument=="+str);
       } 
     strLoc = "Loc_982hc.2";			
     iCountTestcases++;
     try {
     str = String.Concat (null, str2);
     if(!str.Equals (String.Empty))
       {
       iCountErrors++;
       printerr( "Error_687ch.2! incorrect string returned for null argument=="+str);
       } 
     }
     catch (ArgumentException aexc) {
     iCountErrors++;
     printerr( "Error_498ch.2! Unexpcted! " + aexc);
     }
     strLoc = "Loc_982hc.3";			
     iCountTestcases++;
     try {
     str = String.Concat (str1, null);
     if(!str.Equals (String.Empty))
       {
       iCountErrors++;
       printerr( "Error_687ch.3! incorrect string returned for null argument=="+str);
       } 
     }
     catch (ArgumentException aexc) {
     iCountErrors++;
     printerr( "Error_498ch.3! Unexpcted! " + aexc);
     }
     strLoc = "Loc_982hc.4";			
     iCountTestcases++;
     try {
     str = String.Concat (null, null);
     if(str != String.Empty)
       {
       iCountErrors++;
       printerr( "Error_687ch.4! incorrect string returned for null argument=="+str);
       Console.WriteLine (str == String.Empty);
       } 
     }
     catch (ArgumentException aexc) {
     iCountErrors++;
     printerr( "Error_498ch.4! Unexpcted! " + aexc);
     }
     strLoc = "Loc_498hv";
     iCountTestcases++;
     str1 = "abc";
     str2 = "123";
     str = String.Concat (str1, str2);
     if(!str.Equals ("abc123"))
       {
       iCountErrors++;
       printerr( "Error_498ch! incorrect string returned for null argument=="+str);
       } 
     IntlStrings intl = new IntlStrings();
     str1 = intl.GetString(5, true, true);
     str2 = intl.GetString(4, true, true);
     str3 = intl.GetString(9, true, true);
     String testString = String.Concat(str1, str2, null);
     str = String.Concat (str1, str2);
     if(! str.Equals(testString)) {
     ++iCountErrors;
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
   co4830Concat_string_string cbA = new co4830Concat_string_string();
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
