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
public class Co5070ToFileTime
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.FromFileTime()";
 public static readonly String s_strTFName        = "Co5070ToFileTime.cs";
 public static readonly String s_strTFAbbrev      = "Co5070";
 public static readonly String s_strTFPath        = "";
 public virtual Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   DateTime dt1 ;
   Int64 in8a;
   try {
   do
     {
     strLoc = "Loc_472ji";
     in8a = 0;
     dt1 = new DateTime(1601,1,1).ToLocalTime();
     in8a = dt1.ToFileTime();
     iCountTestcases++;
     if(in8a != 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_148nq! ,in8a=="+in8a);
       }
     strLoc = "Loc_482ne";
     in8a = (Int64)(10000000) * (Int64)(60*60*24*365)*(Int64)(400)+
       (Int64)(10000000) * (Int64)(60*60*24*97);
     dt1 = new DateTime(2001,1,1).ToLocalTime();
     iCountTestcases++;
     if(dt1.ToFileTime() != in8a)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_923hd! , filetime=="+dt1.ToFileTime());
       }
     strLoc = "Loc_382nw";
     in8a = (Int64)10000000*(Int64)(60*60)*(Int64)(24*5);
     dt1 = new DateTime(1600,12,26);
     iCountTestcases++;
     try {
     if(dt1.ToFileTime() != -in8a)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_147np! , filetime=="+dt1.ToFileTime());
       }
     } catch (ArgumentException) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_947ni! , exc=="+exc);
     }
     strLoc = "Loc_653wu";
     in8a = 10000000*10;
     dt1 = new DateTime(1601,1,1,0,0,10).ToLocalTime();
     iCountTestcases++;
     if(dt1.ToFileTime() != in8a)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_857qw! , filetime=="+dt1.ToFileTime());
       }
     strLoc = "Loc_724au";
     in8a = -10000000*10;
     dt1 = new DateTime(1600,12,31,15,59,50);
     iCountTestcases++;
     try {
     if(dt1.ToFileTime() != in8a)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_142sa! , filetime=="+dt1.ToFileTime());
       }
     } catch (ArgumentException) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_518la! ,exc=="+exc);
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
   Co5070ToFileTime cbA = new Co5070ToFileTime();
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
