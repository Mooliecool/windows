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
public class Co5373ToUInt16_ubArr_i
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Bitconverter.ToUint16(unsigned SByte[], int)"; 
 public static String s_strTFName        = "Co5373ToUInt16_ubArr_i.cs";
 public static String s_strTFAbbrev      = "Co5373";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   Co5373ToUInt16_ubArr_i cc = new Co5373ToUInt16_ubArr_i();
   try {
   do
     {
     UInt16 ui16 = 0;
     int iIndex;
     Byte[] byArr3 = null; 
     strLoc = "Loc_85yfd";
     iIndex = 0;
     iCountTestcases++;
     try
       {
       ui16 = BitConverter.ToUInt16(null, iIndex);
       iCountErrors++;
       printerr("Error_289fy! ArgumentNullException Expected, got value=="+ui16);
       }
     catch (ArgumentNullException aexc)
       {
       Console.WriteLine("Info_478g: Caught Expected ArgumentNullException, exc=="+aexc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_27ycq! ArgumentNullException expected, got exc=="+exc.ToString());
       }
     strLoc = "Loc_29yfv";
     byArr3 = new Byte[2];
     byArr3[0] = (Byte)0x78;  
     byArr3[1] = (Byte)0x56;  
     iIndex = -1;
     iCountTestcases++;
     Random random = new Random();
     for(int i=0; i<20; i++)
       {
       try
	 {
	 ui16 = BitConverter.ToUInt16(byArr3, random.Next(Int32.MinValue, 0));
	 iCountErrors++;
	 printerr("Error_2fd93! Expected ArgumentOutOfRangeException, got value=="+ui16.ToString());
	 }
       catch (ArgumentOutOfRangeException aexc)
	 {
	 }
       catch (Exception exc)
	 {
	 iCountErrors++;
	 printerr("Error_28bhc! ArgumenOutOfRangeException expected, exc=="+exc.ToString());
	 }
       }
     strLoc = "Loc_10jcx";
     iCountTestcases++;
     byArr3 = new Byte[3];
     byArr3[0] = (Byte)0x78;
     byArr3[1] = (Byte)0x56;
     byArr3[2] = (Byte)0x34;
     iIndex = 2;
     try
       {
       ui16 = BitConverter.ToUInt16(byArr3, iIndex);
       iCountErrors++;
       printerr("Error_28vhc! Expected ArgumentOutOfRangeException, got value=="+ui16.ToString());
       }
     catch (ArgumentException aexc)
       {
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_y72fn! ArgumentOutOfRangeException expected, exc=="+exc.ToString());
       }
     try
       {
       ui16 = BitConverter.ToUInt16(byArr3, Int32.MinValue);
       iCountErrors++;
       printerr("Error_28vhc! Expected ArgumentOutOfRangeException, got value=="+ui16.ToString());
       }
     catch (ArgumentException aexc)
       {
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_y72fn! ArgumentOutOfRangeException expected, exc=="+exc.ToString());
       }
     try
       {
       ui16 = BitConverter.ToUInt16(byArr3, Int32.MaxValue);
       iCountErrors++;
       printerr("Error_28vhc! Expected ArgumentOutOfRangeException, got value=="+ui16.ToString());
       }
     catch (ArgumentException aexc)
       {
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_y72fn! ArgumentOutOfRangeException expected, exc=="+exc.ToString());
       }
     strLoc = "Loc_982vn";
     iCountTestcases++;
     byArr3 = new Byte[1];
     byArr3[0] = (Byte)0x78;
     iIndex = 0;
     try
       {
       ui16 = BitConverter.ToUInt16(byArr3, 0);
       iCountErrors++;
       printerr("Error_298cz! ArgumentException Expected , got value=="+ui16);
       }
     catch (ArgumentException aexc)
       {
       Console.WriteLine("Caught Expected ArgumentException: exc=="+aexc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_298vh! ArgumentException Expected, got exc=="+exc.ToString());
       }
     strLoc = "Loc_87hgg";
     byArr3 = new Byte[4];
     if (BitConverter.IsLittleEndian) {
       byArr3[0] = (Byte)0x78;
       byArr3[1] = (Byte)0x56;
       byArr3[2] = (Byte)0x34;
       byArr3[3] = (Byte)0x12;
     }
     else {
       byArr3[3] = (Byte)0x78;
       byArr3[2] = (Byte)0x56;
       byArr3[1] = (Byte)0x34;
       byArr3[0] = (Byte)0x12;
     }
     iIndex = 1;
     ui16 = BitConverter.ToUInt16(byArr3, iIndex);
     iCountTestcases++;
     if(ui16 != 0x3456)
       {
       iCountErrors++;
       printerr("Error_287cx! Expected=="+0x3456+" , got=="+ui16);
       }
     strLoc = "Loc_29hhx";
     byArr3 = new Byte[200];
     byArr3[0] = (Byte)0x0;
     byArr3[1] = (Byte)0x0;
     byArr3[198] = (Byte)0xff;
     byArr3[199] = (Byte)0xff;
     iIndex = 0;
     ui16 = BitConverter.ToUInt16(byArr3, iIndex);
     iCountTestcases++;
     if(ui16 != 0)
       {
       iCountErrors++;
       printerr("Error_2h90s! Expected==0 , got value=="+ui16);
       }
     iIndex = 198;
     ui16 = BitConverter.ToUInt16(byArr3, iIndex);
     iCountTestcases++;
     if(ui16 != UInt16.MaxValue)
       {
       iCountErrors++;
       printerr("Error_29s19! Expected=="+UInt16.MaxValue+" , got value=="+ui16);
       }
     ui16 = BitConverter.ToUInt16(byArr3, 15);
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
   }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
     return true;
     }
   else
     {
     Console.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
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
   Co5373ToUInt16_ubArr_i cbA = new Co5373ToUInt16_ubArr_i();
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
