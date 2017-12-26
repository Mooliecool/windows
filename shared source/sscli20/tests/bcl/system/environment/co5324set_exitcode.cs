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
public class Co5324set_ExitCode
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Environment.SetExitCode(int)";
 public static String s_strTFName        = "Co5324set_ExitCode.cs";
 public static String s_strTFAbbrev      = "Co5324";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "";
   Random rand;
   try {
   LABEL_860_GENERAL:
   do
     {
     int iExitCode;
     rand = new Random((int)DateTime.Now.Ticks);
     strLoc = "Loc_29fhj";
     iExitCode = rand.Next(Int32.MinValue, Int32.MaxValue);
     Environment.ExitCode = iExitCode;
     iCountTestcases++;
     if(Environment.ExitCode != iExitCode)
       {
       iCountErrors++;
       printerr("Error_29tht! Expected value=="+iExitCode+" , got value=="+Environment.ExitCode);
       }
     iExitCode = Int32.MaxValue;
     Environment.ExitCode = iExitCode;
     iCountTestcases++;
     if(Environment.ExitCode != iExitCode)
       {
       iCountErrors++;
       printerr("Error_29tht! Expected value=="+iExitCode+" , got value=="+Environment.ExitCode);
       }
     iExitCode = Int32.MinValue;
     Environment.ExitCode = iExitCode;
     iCountTestcases++;
     if(Environment.ExitCode != iExitCode)
       {
       iCountErrors++;
       printerr("Error_29tht! Expected value=="+iExitCode+" , got value=="+Environment.ExitCode);
       }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
   Console.WriteLine(exc_general.StackTrace);
   }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs.   "+s_strTFPath +"\\"+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
     return true;
     }
   else
     {
     Console.WriteLine("FAiL!   "+s_strTFPath+"\\"+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
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
   Co5324set_ExitCode cbA = new Co5324set_ExitCode();
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
