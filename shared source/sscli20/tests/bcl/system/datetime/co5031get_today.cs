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
public class Co5031get_Today
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.GetTimeOfDay()";
 public static readonly String s_strTFName        = "Co5031get_Today.cs";
 public static readonly String s_strTFAbbrev      = "Co5031";
 public static readonly String s_strTFPath        = "";
 public virtual Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   DateTime dt1 ;
   DateTime dt2 ;
   try {
   do
     {
     iCountTestcases++; 
     strLoc = "Loc_471uu";
     dt1 = DateTime.Today;
     dt2 = DateTime.Now;
     iCountTestcases++;
     if(!dt1.Date.Equals(dt2.Date))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_181hq!");
       }
     strLoc = "Loc_387hq";
     iCountTestcases++;
     if(dt1.Month != dt2.Month)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_871wy!");
       }
     strLoc = "Loc_127eu";
     iCountTestcases++;
     if(dt1.Year != dt2.Year)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_897he!");
       }
     strLoc = "Loc_227eu";
     iCountTestcases++;
     if(dt1.Day != dt2.Day)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_888he!");
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
   Co5031get_Today cbA = new Co5031get_Today();
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
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
