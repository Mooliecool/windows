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
public class Co5221get_TargetSite
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Exception.TargetSite";
 public static readonly String s_strTFName        = "Co5221GetMethod.cs";
 public static readonly String s_strTFAbbrev      = "Co5221";
 public static readonly String s_strTFPath        = ""; 
 public virtual void ThrowException()
   {
   throw new Exception("Test_1");
   }
 public virtual void ThrowInner()
   {
   throw new Exception("Test_1", new Exception("Test_2"));
   }
 public virtual bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "";
   System.Reflection.MethodInfo methi2 = null;
   String strValue = String.Empty;
   Co5221get_TargetSite cc = new Co5221get_TargetSite();
   try {
   LABEL_860_GENERAL:
   do
     {
     Exception exception;
     strLoc = "Loc_209dh";
     exception = new Exception("This is a test", new ArgumentException("This is an ArgumentTest"));
     iCountTestcases++;
     methi2 = (MethodInfo) exception.TargetSite;
     ++iCountTestcases;
     if ( methi2 != null )
       {
       iCountErrors++;
       printerr("Error_736eg!  methi should be null.");
       }
     strLoc = "Loc_20tfd";
     iCountTestcases++;
     try
       {
       throw exception;
       }
     catch (Exception exc)
       {
       methi2 = (MethodInfo) exc.TargetSite;
       if(!methi2.Name.Equals("runTest"))
	 {
	 iCountErrors++;
	 printerr("Error_20t9t! Incorrect method for exception");
	 }
       methi2 = (MethodInfo) exc.InnerException.TargetSite;
       if ( methi2 != null )
	 {
	 iCountErrors++;
	 printerr("Error_2309f! Inner exception has incorrect method");
	 }
       }
     strLoc = "Loc_2039f";
     iCountTestcases++;
     try
       {
       ThrowException();
       }
     catch (Exception exc)
       {
       if(!((MethodInfo) exc.TargetSite).Name.Equals("ThrowException"))
	 {
	 iCountErrors++;
	 printerr("Error_2093f! Incorrect method for exception");
	 }
       }
     strLoc = "Loc_t209f";
     iCountTestcases++;
     try
       {
       ThrowInner();
       }
     catch (Exception exc)
       {
       methi2 = (MethodInfo) exc.TargetSite;
       if(!methi2.Name.Equals("ThrowInner"))
	 {
	 iCountErrors++;
	 printerr("Error_209td! Incorrect method for exception");
	 }
       methi2 = (MethodInfo) exc.InnerException.TargetSite;
       if ( methi2 != null )
	 {
	 iCountErrors++;
	 printerr("Error_861hug! methi2 should be null here.");
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
   Co5221get_TargetSite cbA = new Co5221get_TargetSite();
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
