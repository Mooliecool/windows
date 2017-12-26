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
using System.Globalization;
using System.Text;
public class Co5381Ctor_iiiiiid
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Datetime.Ctor(int,int,int,int,int,int,double)";
 public static String s_strTFName        = "Co5381Ctor_iiiiiid.cs";
 public static String s_strTFAbbrev      = "Cb5381";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   try
     {
     DateTime dt2;
     int iYear;
     int iMonth;
     int iDay;
     int iHour;
     int iMinute;
     int iSecond;
     int iMilliSecond;
     strLoc = "Loc_98fhr";
     iYear = 1999;
     iMonth = 7;
     iDay = 10;
     iHour = 15;
     iMinute = 12;
     iSecond = 13;
     iMilliSecond = 135;
     dt2 = new DateTime(iYear, iMonth, iDay, iHour, iMinute, iSecond, iMilliSecond);
     iCountTestcases++;
     if(dt2.Millisecond != 135)
       {
       iCountErrors++;
       printerr("Error_298bh! Expected==135 , got value=="+dt2.Millisecond.ToString());
       }
     iCountTestcases++;
     strLoc = "Loc_92902";
     try{
     iMilliSecond = -135;
     dt2 = new DateTime(iYear, iMonth, iDay, iHour, iMinute, iSecond, iMilliSecond);
     iCountTestcases++;
     if(dt2.Millisecond != 865)
       {
       iCountErrors++;
       printerr("Error_98ygy! Expected==-135, got value=="+dt2.Millisecond.ToString());
       }
     }catch(ArgumentOutOfRangeException){}
     iCountTestcases++;
     strLoc = "Loc_98ytv";
     iMilliSecond = (Int32)(DateTime.MaxValue-DateTime.MinValue).TotalSeconds+1;
     iCountTestcases++;
     try
       {
       dt2 = new DateTime(9999, 12, 31, 0, 0, 0, iMilliSecond);
       iCountErrors++;                     
       printerr("Error_29vhw! ArgumentOutOfRangeException expected, got value=="+dt2.ToString());
       }
     catch (ArgumentOutOfRangeException aexc)
       {
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_1299s! ArgumentOutOfRangeException expected, got exc=="+exc.ToString());
       }
     strLoc = "Loc_2fdht";
     iMilliSecond = -135;
     iCountTestcases++;
     try
       {
       dt2 = new DateTime(1, 1, 1, 0, 0, 0, iMilliSecond);
       iCountErrors++;
       printerr("Error_2hv99! ArgumentOutOfRangeException expected, got value=="+dt2.ToString());
       }
     catch (ArgumentOutOfRangeException aexc)
       {
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_98775! ArgumentOutOfRangeException expected, got exc=="+exc.ToString());
       }
     } catch (Exception exc_general ) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
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
   bool bResult = false;
   Co5381Ctor_iiiiiid cbA = new Co5381Ctor_iiiiiid();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
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
