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
public class Co5371GetBytes_ulong
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Bitconverter.GetBytes(UInt64)";
 public static String s_strTFName        = "Co5371GetBytes_ulong.cs";
 public static String s_strTFAbbrev      = "Co5371";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "";
   String strValue = String.Empty;
   Co5371GetBytes_ulong cc = new Co5371GetBytes_ulong();
   try {
   LABEL_860_GENERAL:
   do
     {
     UInt64 ui64 = 0;
     Byte[] byArr2 = null; 
     Byte[] byArr3 = null; 
     strLoc = "Loc_298hv";
     ui64 = (UInt64)0x1234567812345678;
     byArr3 = new Byte[8];
     if (BitConverter.IsLittleEndian) {
       byArr3[0] = (Byte)0x78;  
       byArr3[1] = (Byte)0x56;  
       byArr3[2] = (Byte)0x34;  
       byArr3[3] = (Byte)0x12;  
       byArr3[4] = (Byte)0x78;  
       byArr3[5] = (Byte)0x56;  
       byArr3[6] = (Byte)0x34;  
       byArr3[7] = (Byte)0x12;  
     }
     else {
       byArr3[7] = (Byte)0x78;  
       byArr3[6] = (Byte)0x56;  
       byArr3[5] = (Byte)0x34;  
       byArr3[4] = (Byte)0x12;  
       byArr3[3] = (Byte)0x78;  
       byArr3[2] = (Byte)0x56;  
       byArr3[1] = (Byte)0x34;  
       byArr3[0] = (Byte)0x12;  
     }
     byArr2 = BitConverter.GetBytes(ui64);
     iCountTestcases++;
     if(byArr2.Length != byArr3.Length)
       {
       iCountErrors++;
       printerr("Error_2hfhd! Incorrect Array Length , Length=="+byArr2.Length);
       }
     iCountTestcases++;
     for ( int i = 0 ; i < byArr2.Length ; i++)
       {
       if(byArr2[i] != byArr3[i])
	 {
	 iCountErrors++;
	 printerr("Error_48yg2! expected=="+byArr3[i]+" , value=="+byArr2[i]);
	 }
       }
     strLoc = "Loc_2f8yv";
     ui64 = UInt64.MinValue;
     byArr3[0] = (Byte)0x0;
     byArr3[1] = (Byte)0x0;
     byArr3[2] = (Byte)0x0;
     byArr3[3] = (Byte)0x0;
     byArr3[4] = (Byte)0x0;
     byArr3[5] = (Byte)0x0;
     byArr3[6] = (Byte)0x0;
     byArr3[7] = (Byte)0x0;
     byArr2 = BitConverter.GetBytes(ui64);
     iCountTestcases++;
     if(byArr2.Length != byArr3.Length)
       {
       iCountErrors++;
       printerr("Error_4yg7c! Incorrect Length, Length=="+byArr2.Length);
       }
     iCountTestcases++;
     for(int i = 0 ; i < byArr2.Length ; i++)
       {
       if(byArr2[i] != byArr3[i])
	 {
	 iCountErrors++;
	 printerr("Error_2398c! expected=="+byArr3[i]+" , value=="+byArr2[i]);
	 }
       }
     strLoc = "Loc_2890v";
     ui64 = UInt64.MaxValue;
     byArr3[0] = (Byte)0xff;
     byArr3[1] = (Byte)0xff;
     byArr3[2] = (Byte)0xff;
     byArr3[3] = (Byte)0xff;
     byArr3[4] = (Byte)0xff;
     byArr3[5] = (Byte)0xff;
     byArr3[6] = (Byte)0xff;
     byArr3[7] = (Byte)0xff;
     byArr2 = BitConverter.GetBytes(ui64);
     iCountTestcases++;
     if(byArr2.Length != byArr3.Length)
       {
       iCountErrors++;
       printerr("Error_398fv! Incorrect Length, Length=="+byArr2.Length);
       }
     iCountTestcases++;
     for(int i = 0 ; i < byArr2.Length ; i++)
       {
       if(byArr2[i] != byArr3[i])
	 {
	 iCountErrors++;
	 printerr("Error_9894s! expected=="+byArr3[i]+" , value=="+byArr2[i]);
	 }
       }
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
   Co5371GetBytes_ulong cbA = new Co5371GetBytes_ulong();
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
