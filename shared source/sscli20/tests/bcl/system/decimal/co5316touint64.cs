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
public class Co5316ToUInt64
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Decimal.ToUInt64()";
 public static readonly String s_strTFName        = "Co5316ToUInt64.cs";
 public static readonly String s_strTFAbbrev      = "Co5316";
 public static readonly String s_strTFPath        = Environment.CurrentDirectory;
 public virtual bool runTest()
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
     Decimal dec1, dec2;
     String str1, str2;
     UInt64 ui64;
     int iMaxCounter = 0;
     Object oBoxed;
     strLoc = "Loc_843ry";
     Console.WriteLine(UInt64.MaxValue);
     dec1 = new Decimal(-1.25);
     iCountTestcases++;
     try
       {
       ui64 = Decimal.ToUInt64(dec1);
       iCountErrors++;
       printerr("Error_3928h! OverflowException expected, got value=="+ui64);
       }
     catch (OverflowException oExc)
       {
       Console.WriteLine("Info_859hf! Caught expected OverflowException , exc=="+oExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_43298! OverflowException expected, got exc=="+exc.ToString());
       }
     strLoc = "Loc_948hs";
     dec1 = new Decimal(0);
     ui64 = Decimal.ToUInt64(dec1);
     iCountTestcases++;
     if(ui64 != 0)
       {
       iCountErrors++;
       printerr("Error_98y43! expected value==0 , got value=="+ui64);
       }
     strLoc = "Loc_358rf";
     dec1 = Decimal.Parse("922337203685477");
     ui64 = Decimal.ToUInt64(dec1);
     iCountTestcases++;
     Console.WriteLine(ui64);
     if(!ui64.ToString().Equals("922337203685477"))
       {
       iCountErrors++;
       printerr("Error_8733h! Expected value==922337203685477 , got value=="+ui64);
       }
     strLoc = "Loc_389fh";
     dec1 = new Decimal(184.388);
     ui64 = Decimal.ToUInt64(dec1);
     iCountTestcases++;
     if(ui64 != 184)
       {
       iCountErrors++;
       printerr("Error_2938hf! Expected value==184, got value=="+ui64);
       }
     strLoc = "Loc_29ty47";
     dec1 = new Decimal(1E-14);
     ui64 = Decimal.ToUInt64(dec1);
     iCountTestcases++;
     if(ui64 != 0)
       {
       iCountErrors++;
       printerr("Error_92848! Expected value==0, got value=="+ui64);
       }
     strLoc = "Loc_92ygh";
     dec1 = new Decimal(-1E-4);
     iCountTestcases++;
     try
       {
       ui64 = Decimal.ToUInt64(dec1);
       if(ui64!=0){
       iCountErrors++;
       printerr("Error_84ghw! OverflowException expected, got value=="+ui64);
       }
       }
     catch (OverflowException oExc)
       {
       Console.WriteLine("Info_8274g! Caught Expected OverflowException , exc=="+oExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_2874y! OverflowException expected, got exc=="+exc.ToString());
       }
     strLoc = "Loc_784hf";
     dec1 = new Decimal(1599.34456);
     ui64 = Decimal.ToUInt64(dec1);
     iCountTestcases++;
     if(ui64 != 1599)
       {
       iCountErrors++;
       printerr("Error_199dj! Expected value==1599 , got value=="+ui64);
       }
     strLoc = "Loc_875hg";
     dec1 = new Decimal(-5);
     iCountTestcases++;
     try
       {
       ui64 = Decimal.ToUInt64(dec1);
       iCountErrors++;
       printerr("Error_234oo! OverflowException expected, got value=="+ui64);
       }
     catch (OverflowException oExc)
       {
       Console.WriteLine("Info_88dnh! Caught Expected OverflowException , exc=="+oExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_785bn! OverflowException expected, got exc=="+exc.ToString());
       }
     strLoc = "Loc_2748g";
     dec1 = Decimal.Parse("18446744073709551620");
     iCountTestcases++;
     try
       {
       ui64 = Decimal.ToUInt64(dec1);
       iCountErrors++;
       printerr("Error_87t7y! OverflowException expected, got value=="+ui64);
       }
     catch (OverflowException oExc)
       {
       Console.WriteLine("Info_768gh! Caught Expected OverflowException , exc=="+oExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_911vn! OverflowException expected, got exc=="+exc.ToString());
       }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
   Console.WriteLine(exc_general.StackTrace);
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
 public virtual void printerr ( String err )
   {
   Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public virtual void printinfo ( String info )
   {
   Console.WriteLine ("EXTENDEDINFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co5316ToUInt64 cbA = new Co5316ToUInt64();
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
