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
using System.Collections;
using System.Globalization;
using System.Text;
using System.Threading;
public class Co7084ctor_Int32_Int32_Int32_Boolean_Byte
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Decimal(Int32,Int32,Int32,Boolean,Byte)"; 
 public static String s_strTFName        = "Co7084ctor_Int32_Int32_Int32_Boolean_Byte.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = "";
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   try
     {
     Byte byScale;
     Int32 nHi,nMid,nLo;
     Boolean bIsNeg;
     Decimal dRet;
     Random rand = new Random ();
     Int32 [] iRets;
     Int32 nLastBit;
     strLoc = "Loc_498yg";
     iCountTestcases += 1;
     nLo = 0;
     nMid = 0;
     nHi = 0;
     bIsNeg = true;
     byScale = 20;
     dRet = new Decimal (nLo,nMid,nHi,bIsNeg,byScale);
     if (dRet != 0)
       {
       iCountErrors += 1;
       printerr ( "Error_323sc! Send in==(" + nLo.ToString() + "," + nMid.ToString() + "," + nHi.ToString() + "," + bIsNeg.ToString() + "," + byScale.ToString() + ") Expected==0 Value==" + dRet.ToString());
       }
     strLoc = "Loc_233fd";
     iCountTestcases += 1;
     nLo = unchecked ((int)4294967295);
     nMid = unchecked ((int)4294967295);
     nHi = unchecked ((int)4294967295);
     bIsNeg = false;
     byScale = 0;
     dRet = new Decimal (nLo,nMid,nHi,bIsNeg,byScale);
     if (!dRet.Equals(Decimal.MaxValue))
       {
       iCountErrors += 1;
       printerr ( "Error_763cd! Send in==(" + nLo.ToString() + "," + nMid.ToString() + "," + nHi.ToString() + "," + bIsNeg.ToString() + "," + byScale.ToString() + ") Expected==79228162514264337593543950335 Value==" + dRet.ToString());
       }
     strLoc = "Loc_893ko";
     iCountTestcases += 1;
     nLo = unchecked ((int)4294967295);
     nMid = unchecked ((int)4294967295);
     nHi = unchecked ((int)4294967295);
     bIsNeg = true;
     byScale = 0;
     dRet = new Decimal (nLo,nMid,nHi,bIsNeg,byScale);
     if (!dRet.Equals(Decimal.MinValue))
       {
       iCountErrors += 1;
       printerr ( "Error_168jk! Send in==(" + nLo.ToString() + "," + nMid.ToString() + "," + nHi.ToString() + "," + bIsNeg.ToString() + "," + byScale.ToString() + ") Expected==79228162514264337593543950335 Value==" + dRet.ToString());
       }
     strLoc = "Loc_486nj";
     for (int count = 0 ; count < 1000 ; count++)
       {
       iCountTestcases++;
       nLo = rand.Next(0, Int32.MaxValue);
       nMid = rand.Next(0, Int32.MaxValue);
       nHi = rand.Next(0, Int32.MaxValue);
       bIsNeg = Convert.ToBoolean (rand.Next(0, 2));
       byScale = (Byte) rand.Next(0, 28);
       dRet = new Decimal (nLo,nMid,nHi,bIsNeg,byScale);
       iRets = Decimal.GetBits (dRet);
       nLastBit = ((((IConvertible)bIsNeg).ToInt32(null)) << 31) | ((((IConvertible)byScale).ToInt32(null)) << 16);
       if ((nLo != iRets [0]) || (nMid != iRets [1]) || (nHi != iRets [2]) || (nLastBit != iRets[3]  ))
	 {
	 iCountErrors += 1;
	 printerr ( "Error_452ds_" + count + "! Send in==(" + nLo.ToString() + "," + nMid.ToString() + "," + nHi.ToString() + "," + bIsNeg.ToString() + "," + byScale.ToString() + ") Expected==79228162514264337593543950335 Value==" + dRet.ToString());
	 } 
       }
     strLoc = "Loc_481kd";
     iCountTestcases++;
     nLo = unchecked (20);
     nMid = unchecked (5);
     nHi = unchecked (9000);
     bIsNeg = true;
     byScale = 29;
     try
       {
       dRet = new Decimal (nLo,nMid,nHi,bIsNeg,byScale);
       iCountErrors++;
       printerr( "Error_812jc! no exception thrown");
       }
     catch (ArgumentOutOfRangeException)
       {
       printinfo( "Info_932ds! Caught ArgumentOutOfRangeException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_001cs! Wrong exception thrown: " + e.ToString());
       }
     } catch (Exception exc_general ) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL! "+s_strTFName+" ,inCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public void printerr ( String err )
   {
   Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public void printinfo ( String info )
   {
   Console.WriteLine ("INFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co7084ctor_Int32_Int32_Int32_Boolean_Byte cbA = new Co7084ctor_Int32_Int32_Int32_Boolean_Byte();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFPath+"\\"+s_strTFName);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
