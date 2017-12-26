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
using System.Resources;
using System.IO;
using System.Reflection;
public class Co5220get_Source
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Exception.Source";
 public static readonly String s_strTFName        = "Co5220get_Source.cs";
 public static readonly String s_strTFAbbrev      = "Co5220";
 public static readonly String s_strTFPath        = Environment.CurrentDirectory;
 public virtual bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "";
   String strValue = String.Empty;
   Co5220get_Source cc = new Co5220get_Source();
   try {
   LABEL_860_GENERAL:
   do
     {
     Exception exception;
     strLoc = "Loc_209dh";
     exception = new Exception("This is a test", new ArgumentException("This is an ArgumentTest"));
     exception.Source = "Co5220get_Source";
     try
       {
       throw exception;
       }
     catch (Exception exc)
       {
       strLoc = "Loc_2309S";
       iCountTestcases++;
       if(!exception.Source.Equals("Co5220get_Source"))
	 {
	 iCountErrors++;
	 printerr("Error_20t9t! Incorrect source for exception");
	 }
       iCountTestcases++;
       if(exception.InnerException.Source != null)
	 {
	 iCountErrors++;
	 printerr("Error_2309t! InnerException source is not set and should be null");
	 }
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
 public virtual void printerr ( String err )
   {
   Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public virtual void printinfo ( String info )
   {
   Console.WriteLine ("EXTENDEDINFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co5220get_Source cbA = new Co5220get_Source();
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
