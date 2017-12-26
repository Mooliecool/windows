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
public class Co5069FromFileTime
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.FromFileTime()";
 public static readonly String s_strTFName        = "Co5069FromFileTime.cs";
 public static readonly String s_strTFAbbrev      = "Co5069";
 public static readonly String s_strTFPath        = Environment.CurrentDirectory;
 public virtual Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   DateTime dt1 ;
   DateTime dt2 ;
   Int64 in8a;
   try {
   do
     {
     strLoc = "Loc_471hq";
     in8a = Int64.MinValue;
     iCountTestcases++;
     try {
     dt1 = DateTime.FromFileTime(in8a);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_834hw! , dt1=="+DateTime.FromFileTime(in8a));
     } catch (ArgumentException) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_418si! , exc=="+exc);
     }
     strLoc = "Loc_381hi";
     in8a = Int64.MaxValue;
     iCountTestcases++;
     try {
     dt1 = DateTime.FromFileTime(in8a);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_948jq! , dt1=="+DateTime.FromFileTime(in8a));
     } catch (ArgumentException) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_318ni! ,exc=="+exc);
     }
     strLoc = "Loc_472ji";
     in8a = 0;
     dt1 = DateTime.FromFileTime(in8a);
     dt2 = new DateTime(1601,1,1).ToLocalTime();
     Console.WriteLine(dt1.ToLongDateString());
     Console.WriteLine(dt2.ToLongDateString());
     iCountTestcases++;
     if(dt1 != dt2)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_148nq! ,dt1=="+dt1);
       }
     strLoc = "Loc_482ne";
     in8a = (Int64)(10000000) * (Int64)(60*60*24*365)*(Int64)(400)+
       (Int64)(10000000) * (Int64)(60*60*24*97);
     dt1 = DateTime.FromFileTime(in8a);
     dt2 = new DateTime(2001,1,1).ToLocalTime();
     iCountTestcases++;
     if(!dt1.Equals(dt2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_923hd! , dt1=="+dt1);
       }
     strLoc = "Loc_382nw";
     in8a = (Int64)10000000*(Int64)(60*60)*(Int64)(24*5);
     iCountTestcases++;
     try {
     dt1 = DateTime.FromFileTime(-in8a);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_471hq! ,dt1=="+dt1);
     } catch (ArgumentException) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_517ig! , exc=="+exc);
     }
     strLoc = "Loc_653wu";
     in8a = 10000000*10;
     dt1 = DateTime.FromFileTime(in8a);
     dt2 = new DateTime(1601,1,1,0,0,10).ToLocalTime();
     iCountTestcases++;
     if(!dt1.Equals(dt2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_857qw! , dt1=="+dt1);
       }
     strLoc = "Loc_724au";
     in8a = -10000000*10;
     iCountTestcases++;
     try {
     dt1 = DateTime.FromFileTime(in8a);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_481jd! , dt1=="+dt1);
     } catch (ArgumentException) {}
     catch (Exception) {
     iCountTestcases++;
     Console.WriteLine( s_strTFAbbrev+ "Err_471dh! , dt1=="+dt1);
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
   Co5069FromFileTime cbA = new Co5069FromFileTime();
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
