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
using System.Configuration.Assemblies;
public class Co5479set_Minor
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Version.Minor";
 public static String s_strTFName        = "Co5479set_Minor.cs";
 public static String s_strTFAbbrev      = "Co5479";
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
     strLoc = "Loc_4988s";
     Version ver2 = new Version(3, 33, Int32.MaxValue, 5 );
     iCountTestcases++;
     if(ver2.Build != Int32.MaxValue)
       {
       iCountErrors++;
       printerr ( "Error_49xy7! Incorrect build value set");
       }
     iCountTestcases++;
     if(ver2.Major != 3)
       {
       iCountErrors++;
       printerr( "Error_49x8a! Incorrect major value set");
       }
     iCountTestcases++;
     if(ver2.Minor != 33)
       {
       iCountErrors++;
       printerr( "Error_498x8! Incorrect minor value set");
       }
     iCountTestcases++;
     if(ver2.Revision != 5)
       {
       iCountErrors++;
       printerr( "Error_498vy! Incorrect Revision set");
       }
     Version ver3 = (Version)ver2.Clone();
     if(ver3.Build != Int32.MaxValue)
       {
       iCountErrors++;
       printerr ( "Error_49xy7! Incorrect build value set");
       }
     iCountTestcases++;
     if(ver3.Major != 3)
       {
       iCountErrors++;
       printerr( "Error_49x8a! Incorrect major value set");
       }
     iCountTestcases++;
     if(ver3.Minor != 33)
       {
       iCountErrors++;
       printerr( "Error_498x8! Incorrect minor value set");
       }
     iCountTestcases++;
     if(ver3.Revision != 5)
       {
       iCountErrors++;
       printerr( "Error_498vy! Incorrect Revision set");
       }
     } catch (Exception exc_general ) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
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
   bool bResult = false;
   Co5479set_Minor cbA = new Co5479set_Minor();
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
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
