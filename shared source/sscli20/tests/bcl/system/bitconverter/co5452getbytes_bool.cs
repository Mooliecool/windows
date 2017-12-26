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
public class Co5452GetBytes_bool
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Bitconverter.GetBytes(Boolean)";
 public static String s_strTFName        = "Co5452GetBytes_bool.cs";
 public static String s_strTFAbbrev      = "Co5452";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "";
   String strValue = String.Empty;
   Co5452GetBytes_bool cc = new Co5452GetBytes_bool();
   try {
   LABEL_860_GENERAL:
   do
     {
     UInt32 ui32 = 0;
     Byte[] byArr2 = null; 
     Byte[] byArr3 = null; 
     strLoc = "Loc_209xy";
     byArr2 = BitConverter.GetBytes(true);
     iCountTestcases++;
     if(byArr2.Length != 1)
       {
       iCountErrors++;
       printerr( "Error_209cj! Incorrect Length of returned SByte array");
       }
     if(byArr2[0] == 0)
       {
       iCountErrors++;
       printerr( "Error_289cx! Incorrect value of returned SByte array");
       }
     strLoc = "Loc_029xu";
     byArr2 = BitConverter.GetBytes(false);
     iCountTestcases++;
     if(byArr2.Length != 1)
       {
       iCountErrors++;
       printerr( "Error_190zh! Incorrect Length of returned SByte array");
       }
     if(byArr2[0] != 0)
       {
       iCountErrors++;
       printerr( "Error_199s1! Incorrect value of returned SByte array");
       }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
   }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
     return true;
     }
   else
     {
     Console.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
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
   Boolean bResult = false;
   Co5452GetBytes_bool cbA = new Co5452GetBytes_bool();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
