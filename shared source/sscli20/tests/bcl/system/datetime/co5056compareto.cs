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
using System.Globalization;
using System;
public class Co5056CompareTo
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.CompareTo(Object obj)";
 public static readonly String s_strTFName        = "Co5056CompareTo.cs";
 public static readonly String s_strTFAbbrev      = "Co5056";
 public static readonly String s_strTFPath        = "";
 public virtual Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   DateTime dt1 ;
   DateTime dt2 ;
   String strInput1;
   String strInput2;
   int inYear = 0;
   int inMonth = 0;
   int inDay = 0;
   int inHour = 0;
   int inMinute = 0;
   int inSecond = 0;
   try {
   LABEL_860_GENERAL:
   do
     {
     strLoc = "Loc_300nn";
     strInput1 = "03/23/15 14:12:12";
     dt1 = DateTime.Parse(strInput1, CultureInfo.InvariantCulture);
     iCountTestcases++;
     try {
     dt1.CompareTo(strInput1);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_458ls! , result="+dt1.CompareTo(strInput1));
     } catch (ArgumentException aExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_290! , exc=="+exc);
     }
     strLoc = "Loc_199qi";
     strInput1 = "03/23/15 05:23:34";
     dt1 = DateTime.Parse(strInput1, CultureInfo.InvariantCulture);
     iCountTestcases++;
     if(dt1.CompareTo(null) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_129qi! , result=="+dt1.CompareTo(null));
       }
     strLoc = "Loc_825uw";
     strInput1 = "01/01/00 00:00:01";
     dt1 = DateTime.Parse(strInput1, CultureInfo.InvariantCulture);
     iCountTestcases++;
     if(dt1.CompareTo(null) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_829uw! , return=="+dt1.CompareTo(null));
       }
     strLoc = "Loc_312hr";
     inYear = 1200;
     inMonth = 2;
     inDay = 29;
     inHour = 13;
     inMinute = 32;
     inSecond = 21;
     strInput1 = "02/29/1200 13:32:21";
     dt1 = new DateTime(inYear, inMonth, inDay, inHour, inMinute, inSecond);
     dt2 = DateTime.Parse(strInput1, CultureInfo.InvariantCulture);
     iCountTestcases++;
     if(dt1.CompareTo(dt2) != 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_282hj! ,result=="+dt1.CompareTo(dt2));
       }
     strLoc = "Loc_322re";
     strInput1 = "02/29/2000 13:32:21";
     strInput2 = "02/29/00 13:32:21";
     dt1 = DateTime.Parse(strInput1, CultureInfo.InvariantCulture);
     dt2 = DateTime.Parse(strInput2, CultureInfo.InvariantCulture);
     iCountTestcases++;
     if(dt1.CompareTo(dt2) != 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_292hj! ,result=="+dt1.CompareTo(dt2));
       }
     strLoc = "Loc_394as";
     strInput1 = "05/25/98 13:33:33";
     strInput2 = "05/25/98 13:33:34";
     dt1 = DateTime.Parse(strInput1, CultureInfo.InvariantCulture);
     dt2 = DateTime.Parse(strInput2, CultureInfo.InvariantCulture);
     iCountTestcases++;
     if(dt1.CompareTo(dt2) >= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_123jj! ,result=="+dt1.CompareTo(dt2));
       }
     strLoc = "Loc_384as";
     strInput1 = "05/26/98 13:33:33";
     strInput2 = "05/25/98 13:33:34";
     dt1 = DateTime.Parse(strInput1, CultureInfo.InvariantCulture);
     dt2 = DateTime.Parse(strInput2, CultureInfo.InvariantCulture);
     iCountTestcases++;
     if(dt1.CompareTo(dt2) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_223jj! ,result=="+dt1.CompareTo(dt2));
       }
     strLoc = "Loc_385ew";
     strInput1 = "05/24/99 13:33:33";
     strInput2 = "05/25/98 13:33:34";
     dt1 = DateTime.Parse(strInput1, CultureInfo.InvariantCulture);
     dt2 = DateTime.Parse(strInput2, CultureInfo.InvariantCulture);
     iCountTestcases++;
     if(dt1.CompareTo(dt2) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_343jj! ,result=="+dt1.CompareTo(dt2));
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
   Co5056CompareTo cbA = new Co5056CompareTo();
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
