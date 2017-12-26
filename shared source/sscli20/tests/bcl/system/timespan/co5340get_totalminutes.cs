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
using System.Threading;
using System.IO; 
public class Co5340get_TotalMinutes
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "TimeSpan.TotalMinutes()";
 public static String s_strTFName        = "Co5340get_TotalMinutes.cs";
 public static String s_strTFAbbrev      = "Co5340";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "";
   try {
   LABEL_860_GENERAL:
   do
     {
     TimeSpan ts1, ts2;
     String str1, str2, strRec, strExp;
     Double dbl1, dbl2;
     strLoc = "Loc_20sje";
     dbl1 = 10675200;
     ts1 = TimeSpan.FromMinutes(dbl1);
     dbl2 = ts1.TotalMinutes;
     iCountTestcases++;
     if(dbl1 != dbl2)
       {
       iCountErrors++;
       printerr("Error_2s8ya!");
       }
     strLoc = "Loc_3r8jd";
     dbl1 = -10675200;
     ts1 = TimeSpan.FromMinutes(dbl1);
     dbl2 = ts1.TotalMinutes;
     iCountTestcases++;
     if(dbl1 != dbl2)
       {
       iCountErrors++;
       printerr("Error_209uf");
       }
     strLoc = "Loc_09ngn";
     dbl1 = 1.5;
     ts1 = TimeSpan.FromMinutes(dbl1);
     dbl2 = ts1.TotalMinutes;
     iCountTestcases++;
     if(dbl1 != dbl2)
       {
       iCountErrors++;
       printerr("Error_298fh");
       }
     dbl1 = 4.16666666666666666666666666666e-2;
     ts1 = TimeSpan.FromMinutes(dbl1);
     dbl2 = ts1.TotalMinutes;
     iCountTestcases++;
     if(dbl1 != dbl2)
       {
       iCountErrors++;
       printerr("Error_2980y");
       }
     strLoc = "Loc_208ud";
     dbl1 = 1;
     ts1 = TimeSpan.FromMinutes(dbl1);
     dbl2 = ts1.TotalMinutes;
     iCountTestcases++;
     if(dbl1 != dbl2)
       {
       iCountErrors++;
       printerr("Error_83hgd!");
       }
     strLoc = "Loc_28yhg";
     dbl1 = 60;
     ts1 = TimeSpan.FromMinutes(dbl1);
     dbl2 = ts1.TotalMinutes;
     iCountTestcases++;
     if(dbl1 != dbl2)
       {
       iCountErrors++;
       printerr("Error_28ysh");
       }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
   }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs.   "+s_strTFPath +"\\"+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
     return true;
     }
   else
     {
     Console.WriteLine("FAiL!   "+s_strTFPath+"\\"+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public void printerr ( String err )
   {
   Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public void printinfo ( String info )
   {
   Console.WriteLine ("EXTENDEDINFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args) 
   {
   Boolean bResult = false;
   Co5340get_TotalMinutes cbA = new Co5340get_TotalMinutes();
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
