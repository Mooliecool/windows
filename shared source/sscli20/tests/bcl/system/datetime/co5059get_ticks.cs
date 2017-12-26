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
using System.IO;
using System;
using System.Threading;
using System.Globalization;
public class Co5059get_Ticks
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "DateTime.Ticks";
 public static String s_strTFName        = "Co5059get_Ticks.cs";
 public static String s_strTFAbbrev      = "Co5059";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc;
   DateTime dt1 ;
   DateTime dt2 ;
   String strInput1;
   String strInput2;
   Int64 in8a;
   try {
   LABEL_860_GENERAL:
   do
     {
     iCountTestcases = 2;
     if(Thread.CurrentThread.CurrentCulture.LCID==0x0409){
     dt1 = DateTime.Parse("01/01/100 00:00:00");
     in8a = (Int64)(1E7*60*60*24*365*99+1E7*60*60*24*24);
     iCountTestcases++;
     if(dt1.Ticks != in8a)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_213oe! , ticks=="+dt1.Ticks);
       }
     dt1 = DateTime.Parse("01/01/1000 00:00:00");
     in8a = (Int64)(1E7*60*60*24*365*999+1E7*60*60*24*242);
     iCountTestcases++;
     if(dt1.Ticks != in8a)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_892jk! ,ticks=="+dt1.Ticks);
       }
     }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine(s_strTFAbbrev +" Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
   }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
     return true;
     }
   else
     {
     Console.Error.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public static void Main(String[] args) 
   {
   Boolean bResult = false;
   Co5059get_Ticks cbA = new Co5059get_Ticks();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev+ "FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
   }
   if (!bResult)
     {
     Console.WriteLine(s_strTFName+ s_strTFPath);
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.Error.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode=0; else Environment.ExitCode=1;
   }
}
