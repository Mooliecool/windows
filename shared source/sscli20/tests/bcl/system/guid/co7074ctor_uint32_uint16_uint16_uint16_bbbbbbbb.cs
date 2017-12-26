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
public class Co7074ctor_UInt32_UInt16_UInt16_UInt16_bbbbbbbb
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Guid(UInt32, UInt16, UInt16, Byte, Byte, Byte, Byte, Byte, Byte, Byte, Byte)"; 
 public static String s_strTFName        = "Co7074ctor_UInt32_UInt16_UInt16_UInt16_bbbbbbbb.cs";
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
     Guid gTest;	
     Random rand = new Random((int)DateTime.Now.Ticks);
     strLoc = "Loc_498yg";
     iCountTestcases++;
     gTest = new Guid ((UInt32)0xa,(UInt16)0xb,(UInt16)0xc,0,1,2,3,4,5,6,7);
     if (!gTest.ToString().Equals("0000000a-000b-000c-0001-020304050607"))
       {
       iCountErrors++;
       printerr("Error_100aa SentIn==\"(a,b,c,0,1,2,3,4,5,6,7)\" Expected==0000000a-000b-000c-0001-020304050607, value==" + gTest.ToString());
       printerr("Error_100aa WARNING THIS TEST WAS TAKEN FROM THE DOCUMENTATION -- IF IT FAILS CHECK DOCUMENTATION!!!");
       }
     strLoc = "Loc_298vy";
     iCountTestcases++;
     gTest = new Guid ((UInt32)0,(UInt16)0,(UInt16)0,0,0,0,0,0,0,0,0);
     if (!gTest.ToString().Equals("00000000-0000-0000-0000-000000000000"))
       {
       iCountErrors++;
       printerr("Error_200aa SentIn==\"(0,0,0,0,0,0,0,0,0,0,0)\" Expected==00000000-0000-0000-0000-000000000000, value==" + gTest.ToString());
       }
     strLoc = "Loc_298vy";
     iCountTestcases++;
     gTest = new Guid (UInt32.MaxValue,UInt16.MaxValue,UInt16.MaxValue,Byte.MaxValue,Byte.MaxValue,Byte.MaxValue,Byte.MaxValue,Byte.MaxValue,Byte.MaxValue,Byte.MaxValue,Byte.MaxValue);
     if (!gTest.ToString().Equals("ffffffff-ffff-ffff-ffff-ffffffffffff"))
       {
       iCountErrors++;
       printerr("Error_230aa SentIn==\"(UInt32.MaxValue,UInt16.MaxValue,UInt16.MaxValue,Byte.MaxValue,Byte.MaxValue,Byte.MaxValue,Byte.MaxValue,Byte.MaxValue,Byte.MaxValue,Byte.MaxValue,Byte.MaxValue)\" Expected==ffffffff-ffff-ffff-ffff-ffffffffffff, value==" + gTest.ToString());
       }
     strLoc = "Loc_398vy";
     UInt32 a;
     UInt16  b, c;
     Byte d, e, f, g, h, i , j, k;
     NumberFormatInfo nfi = new NumberFormatInfo();
     for (int count = 0 ; count < 1000 ; count++)
       {
       iCountTestcases++;
       a = (UInt32) (rand.Next(0, Int32.MaxValue) * 2);
       b = (UInt16)rand.Next(UInt16.MinValue, UInt16.MaxValue);
       c = (UInt16)rand.Next(UInt16.MinValue, UInt16.MaxValue);
       d = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       e = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       f = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       g = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       h = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       i = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       j = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       k = (Byte)rand.Next(Byte.MinValue, Byte.MaxValue);
       gTest = new Guid(a, b, c, d, e, f, g, h, i, j, k);				
       if (!gTest.ToString().Equals(a.ToString("x8",nfi) + "-" + b.ToString("x4",nfi) + "-" + c.ToString("x4",nfi) +"-" + d.ToString("x2",nfi) + e.ToString("x2",nfi) +"-" +f.ToString("x2",nfi)+g.ToString("x2",nfi)+h.ToString("x2",nfi)+i.ToString("x2",nfi)+j.ToString("x2",nfi)+k.ToString("x2",nfi) ))
	 {
	 iCountErrors++;
	 printerr("Error_330aa SentIn==(" + a + "," + b+ "," + c+ "," + d+ "," + e+ "," + f+ "," + g+ "," + h+ "," + i+ "," + j+ "," + k + ") Expected==\"" + a.ToString("x8",nfi) + "-" + b.ToString("x4",nfi) + "-" + c.ToString("x4",nfi) +"-" + d.ToString("x2",nfi) + e.ToString("x2",nfi) +"-" +f.ToString("x2",nfi)+g.ToString("x2",nfi)+h.ToString("x2",nfi)+i.ToString("x2",nfi)+j.ToString("x2",nfi)+k.ToString("x2",nfi) +"\", value==" + gTest.ToString());
	 } 
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
   Co7074ctor_UInt32_UInt16_UInt16_UInt16_bbbbbbbb cbA = new Co7074ctor_UInt32_UInt16_UInt16_UInt16_bbbbbbbb();
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
