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
public class Co5315ToUInt16
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Decimal.ToUInt16()";
 public static readonly String s_strTFName        = "Co5315ToUInt16.cs";
 public static readonly String s_strTFAbbrev      = "Co5315";
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
     Decimal dec1, cur2;
     String str1, str2;
     UInt16 ui16;
     int iMaxCounter = 0;
     Object oBoxed;
     strLoc = "Loc_843ry";
     dec1 = new Decimal(-0.25);
     iCountTestcases++;
     try
       {
       ui16 = Decimal.ToUInt16(dec1);                
       if(ui16!=0){
       iCountErrors++;
       printerr("Error_3928h! OverflowException expected, got value=="+ui16);
       }
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
     strLoc = "Loc_3298f";
     dec1 = Decimal.Parse("65536");
     iCountTestcases++;
     try
       {
       ui16 = Decimal.ToUInt16(dec1);
       iCountErrors++;
       printerr("Error_238fh! OverflowException expected, got value=="+ui16);
       }
     catch (OverflowException oExc)
       {
       Console.WriteLine("Info_89t4h! Caught expected OverflowException , exc=="+oExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_6rg33! OverflowException expected, got exc=="+exc.ToString());
       }
     strLoc = "Loc_948hs";
     dec1 = new Decimal(0);
     ui16 = Decimal.ToUInt16(dec1);
     iCountTestcases++;
     if(ui16 != 0)
       {
       iCountErrors++;
       printerr("Error_98y43! expected value==0 , got value=="+ui16);
       }
     strLoc = "Loc_358rf";
     dec1 = new Decimal(UInt16.MaxValue);
     ui16 = Decimal.ToUInt16(dec1);
     iCountTestcases++;
     if(ui16 != 65535)
       {
       iCountErrors++;
       printerr("Error_8733h! Expected value==65535 , got value=="+ui16);
       }
     strLoc = "Loc_389fh";
     dec1 = new Decimal(184.388);
     ui16 = Decimal.ToUInt16(dec1);
     iCountTestcases++;
     if(ui16 != 184)
       {
       iCountErrors++;
       printerr("Error_2938hf! Expected value==184, got value=="+ui16);
       }
     strLoc = "Loc_29ty47";
     dec1 = new Decimal(1E-14);
     ui16 = Decimal.ToUInt16(dec1);
     iCountTestcases++;
     if(ui16 != 0)
       {
       iCountErrors++;
       printerr("Error_92848! Expected value==0, got value=="+ui16);
       }
     strLoc = "Loc_92ygh";
     dec1 = new Decimal(-1E-4);
     iCountTestcases++;
     try
       {
       ui16 = Decimal.ToUInt16(dec1);
       if(ui16!=0){
       iCountErrors++;
       printerr("Error_84ghw! OverflowException expected, got value=="+ui16);
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
     ui16 = Decimal.ToUInt16(dec1);
     iCountTestcases++;
     if(ui16 != 1599)
       {
       iCountErrors++;
       printerr("Error_199dj! Expected value==1599 , got value=="+ui16);
       }
     strLoc = "Loc_875hg";
     dec1 = new Decimal(-5);
     iCountTestcases++;
     try
       {
       ui16 = Decimal.ToUInt16(dec1);
       iCountErrors++;
       printerr("Error_234oo! OverflowException expected, got value=="+ui16);
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
   Co5315ToUInt16 cbA = new Co5315ToUInt16();
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
