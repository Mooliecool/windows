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
using System.Text;
public class Co5064ToShortDateString
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.ToShortDateString()";
 public static readonly String s_strTFName        = "Co5064ToShortDateString.cs";
 public static readonly String s_strTFAbbrev      = "Co5064";
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
     strLoc = "Loc_471hq";
     iCountTestcases++;
     dt1 = DateTime.Parse("01/01/1999 12:32:59");
     dt2 = new DateTime(1999,1,1,12,32,59);
     if(!dt2.ToShortDateString().Equals(dt1.ToShortDateString()))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_834hw! , return=="+dt1.ToShortDateString());
       }
     strLoc = "Loc_487iw";
     iCountTestcases++;
     dt1 = DateTime.Parse("02/29/2000");
     dt2 = new DateTime(2000, 2, 29);
     if(!dt2.ToShortDateString().Equals(dt1.ToShortDateString()))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_394hw! , return=="+dt1.ToShortDateString());
       }
     strLoc = "Loc_123nq";
     iCountTestcases++;
     dt1 = DateTime.Parse("3/4/1987 14:44:23");
     dt2 = new DateTime(1987, 3, 4, 14, 44, 23);
     if(!dt2.ToShortDateString().Equals(dt1.ToShortDateString()))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_137ne! , return=="+dt1.ToShortDateString());
       }
     strLoc = "Loc_473jo";
     dt1 = DateTime.Parse("04/07/1754 12:32:23");
     iCountTestcases++;
     if(!"04/07/1754".Equals(dt1.ToShortDateString()) &&
	!"4/7/1754".Equals(dt1.ToShortDateString()) &&
	!"04/07/54".Equals(dt1.ToShortDateString()) &&
	!"4/7/54".Equals(dt1.ToShortDateString())
	)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ " Err_128jw! ,return=="+dt1.ToShortDateString());
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
   Co5064ToShortDateString cbA = new Co5064ToShortDateString();
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
