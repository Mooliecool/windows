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
using System.Collections;
using System.Globalization;
using System.Text;
using System.Threading;
public class Co7037ctor_iii
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Version(Int32,Int32,Int32)"; 
 public static String s_strTFName        = "Co7037ctor_iii.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = "";
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   try
     {
     strLoc = "Loc_400vy";
     iCountTestcases++;
     Version vTest = new Version (Int32.MaxValue, 2, 0);
     if (vTest.Major != Int32.MaxValue)
       {
       ++iCountErrors;	
       printerr( "Error_100aa! Version.Major not set properly, Expected==Int32.MaxValue Value==" + vTest.Major);
       }
     if (vTest.Minor != 2)
       {
       ++iCountErrors;	
       printerr( "Error_100bb! Version.Minor not set properly, Expected==2 Value==" + vTest.Minor);
       }
     if (vTest.Revision != -1)
       {
       ++iCountErrors;	
       printerr( "Error_100cc! Version.Revision not set properly, Expected==0 Value==" + vTest.Revision);
       }
     if (vTest.Build != 0)
       {
       ++iCountErrors;	
       printerr( "Error_100dd! Version.Build not set properly, Expected==-1 Value==" + vTest.Build);
       }
     strLoc = "Loc_200vy";
     iCountTestcases++;
     try
       {
       Version vTest2 = new Version (Int32.MinValue, 2, 0);
       ++iCountErrors;	
       printerr( "Error_200aa! Expected ArgumentOutOfRangeException not thrown");
       }
     catch (ArgumentOutOfRangeException aorexc)
       {
       printinfo( "Info_200cc! Caught ArgumentOutOfRangeException, aorexc=="+aorexc.Message);
       }
     catch (Exception exc_general)
       {
       ++iCountErrors;	
       Console.WriteLine (s_strTFAbbrev + " : Error Err_200bb!  Incorrect Exception thrown strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
       }
     strLoc = "Loc_300vy";
     iCountTestcases++;
     try
       {
       Version vTest3 = new Version (Int32.MaxValue, -1, 0);
       ++iCountErrors;	
       printerr( "Error_300aa! Expected ArgumentOutOfRangeException not thrown");
       }
     catch (ArgumentOutOfRangeException aorexc)
       {
       printinfo( "Info_300cc! Caught ArgumentOutOfRangeException, aorexc=="+aorexc.Message);
       }
     catch (Exception exc_general)
       {
       ++iCountErrors;	
       Console.WriteLine (s_strTFAbbrev + " : Error Err_300bb!  Incorrect Exception thrown strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
       }
     strLoc = "Loc_400vy";
     iCountTestcases++;
     try
       {
       Version vTest4 = new Version (Int32.MaxValue, 2, -100);
       ++iCountErrors;	
       printerr( "Error_400aa! Expected ArgumentOutOfRangeException not thrown");
       }
     catch (ArgumentOutOfRangeException aorexc)
       {
       printinfo( "Info_400cc! Caught ArgumentOutOfRangeException, aorexc=="+aorexc.Message);
       }
     catch (Exception exc_general)
       {
       ++iCountErrors;	
       Console.WriteLine (s_strTFAbbrev + " : Error Err_400bb!  Incorrect Exception thrown strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
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
   Console.WriteLine ("INFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co7037ctor_iii cbA = new Co7037ctor_iii();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFPath+"\\"+s_strTFName);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
