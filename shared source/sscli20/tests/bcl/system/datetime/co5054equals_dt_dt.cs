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
public class Co5054Equals_dt_dt
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.Equals(DateTime, DateTime)";
 public static readonly String s_strTFName        = "Co5054Equals_dt_dt.cs";
 public static readonly String s_strTFAbbrev      = "Co5054";
 public static readonly String s_strTFPath        = Environment.CurrentDirectory;
 public virtual Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   DateTime dt1 ;
   DateTime dt2 ;
   int inYear = 0;
   int inMonth = 0;
   int inDay = 0;
   int inHour = 0;
   int inMinute = 0;
   int inSecond = 0;
   try {
   do
     {
     strLoc = "Loc_825uw";
     inYear = 800;
     inMonth = 2;
     inDay = 29;
     inHour = 13;
     inMinute = 32;
     inSecond = 21;
     dt1 = new DateTime(inYear, inMonth, inDay, inHour, inMinute, inSecond);
     dt2 = new DateTime(inYear, inMonth, inDay, inHour, inMinute, inSecond);
     iCountTestcases++;
     if(!DateTime.Equals(dt1, dt2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_829uw! , return=="+DateTime.Equals(dt1,dt2));
       }
     strLoc = "Loc_938hr";
     inYear = 800;
     inMonth = 2;
     inDay = 29;
     inHour = 13;
     inMinute = 32;
     inSecond = 21;
     dt1 = new DateTime(inYear, inMonth, inDay, inHour, inMinute, inSecond);
     dt2 = new DateTime(1200,2,29,12,32,21);
     iCountTestcases++;
     if(DateTime.Equals(dt1, dt2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_182hj! ,result=="+DateTime.Equals(dt1, dt2));
       }
     strLoc = "Loc_312hr";
     inYear = 1200;
     inMonth = 2;
     inDay = 29;
     inHour = 13;
     inMinute = 32;
     inSecond = 21;
     dt1 = new DateTime(inYear, inMonth, inDay, inHour, inMinute, inSecond);
     dt2 = new DateTime(1200,2,29,13,32,21);
     iCountTestcases++;
     if(!DateTime.Equals(dt1, dt2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_282hj! ,result=="+DateTime.Equals(dt1, dt2));
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
   Co5054Equals_dt_dt cbA = new Co5054Equals_dt_dt();
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
