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
public class Co4828ctor_sbyte
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "String(sbyte*)";
 public static String s_strTFName        = "Co4828ctor_sbyte";
 public static String s_strTFAbbrev      = "Co5504";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public unsafe bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   try
     {
     sbyte* chptr = null, baseptr = null;
     String str1 = String.Empty, str2 = String.Empty;
     strLoc = "Loc_498hv";
     try {
     iCountTestcases++;
     str2 = new String(chptr);
     if(!str2.Equals(String.Empty))
       {
       iCountErrors++;
       printerr( "Error_498ch! incorrect string returned for null argument=="+str2);
       } 
     } catch(Exception e) {Console.WriteLine(e.ToString());};
     strLoc = "Loc_982hc";
     iCountTestcases++;
     try {
     iCountTestcases++;
     IntlStrings intl = new IntlStrings();
     String testString = intl.GetString(1, true, true);
     fixed (char *charptr = testString) {
     str2 =new  String(charptr);
     }
     if(str2 != testString)
       {
       iCountErrors++;
       printerr( "Error_49hcx! incorrect string constructed=="+str2);
       }
     } catch(Exception e) {Console.WriteLine("WRONG");}
     try {
     strLoc = "Loc_9fhc1";
     *chptr = (sbyte)'\t';
     str2 = new String(chptr);
     iCountTestcases++;
     if(str2[0]!='\t')
       {
       iCountErrors++;
       printerr( "Error_89hfd! incorrect string constructed=="+str2);
       }
     } catch(Exception e) {Console.WriteLine(e.ToString());}
     try {
     strLoc = "Loc_9fh56";
     iCountTestcases++;
     sbyte* p = stackalloc sbyte[10];
     for (int i = 0; i < 10; i++) {
     p[i] = (sbyte)(i + (int)'0');
     str1 += ((sbyte) i);
     }
     strLoc = "Loc_93286";
     iCountTestcases++;
     str2 = new String (p);
     strLoc = "Loc_93287";
     if(!str2.Equals(str1))
       { 
       iCountErrors++;
       printerr( "Error_209xu! Expected=="+str1+"\nIncorrect string=="+str2);
       printinfo ("str2.Length == " + str2.Length);
       } }
     catch(Exception e) {Console.WriteLine(e.ToString());}
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
   Co4828ctor_sbyte cbA = new Co4828ctor_sbyte();
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
