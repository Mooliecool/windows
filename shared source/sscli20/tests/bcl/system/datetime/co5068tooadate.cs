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
public class Co5068ToOADate
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.ToOADate()";
 public static readonly String s_strTFName        = "Co5068ToOADate.cs";
 public static readonly String s_strTFAbbrev      = "Co5068";
 public static readonly String s_strTFPath        = "";
 public virtual Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   DateTime dt1 ;
   DateTime dt2 ;
   Double dbl1a;
   Double dbl2a;
   try {
   do
     {
     strLoc = "Loc_487iw";
     dbl1a = -30.75;
     dt1 = DateTime.FromOADate(dbl1a);
     dt2 = new DateTime(1899, 11, 30, 18, 0, 0);
     dbl2a = dt2.ToOADate();
     iCountTestcases++;
     if(dbl1a != dbl2a)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_394hw! , dbl2a=="+dbl2a);
       }
     strLoc = "Loc_123nq";
     dbl1a = (365*100+24+0.5);
     dt1 = DateTime.FromOADate(dbl1a);
     dt2 = new DateTime(1999, 12, 30, 12, 0, 0);
     dbl2a = dt2.ToOADate();
     iCountTestcases++;
     if(dbl1a != dbl2a)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_137ne! , dbl2a=="+dbl2a);
       }
     strLoc = "Loc_473jo";
     dbl1a = -(365*100+24+0.355);
     dt1 = DateTime.FromOADate(dbl1a);
     dt2 = new DateTime(1799, 12, 30, 8, 31, 12);
     dbl2a = dt2.ToOADate();
     iCountTestcases++;
     if(dbl1a != dbl2a)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_128jw! ,dbl2a=="+dbl2a);
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
   Co5068ToOADate cbA = new Co5068ToOADate();
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
