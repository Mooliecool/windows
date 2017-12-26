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
public class Co5322ToByteArray
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Guid.ToByteArray()";
 public static String s_strTFName        = "Co5322ToByteArray.cs";
 public static String s_strTFAbbrev      = "Co5322";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "";
   Random rand;
   try {
   LABEL_860_GENERAL:
   do
     {
     Guid gui1, gui2;
     String str1, str2;
     Byte[] bytArr; 
     Byte[] bytResultArr; 
     rand = new Random((int)DateTime.Now.Ticks);
     strLoc = "Loc_29thx";
     bytArr = null;
     iCountTestcases++;
     try
       {
       gui1 = new Guid(bytArr);
       iCountErrors++;
       printerr("Error_92tg8! ArgumentNullException expected, got value=="+gui1);
       }
     catch (ArgumentNullException anExc)
       {
       Console.WriteLine("Info_298jj Caught Expected ArgumentNullException , exc=="+anExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_11y1f! ArgumentNullException expected, got exc=="+exc.ToString());
       }
     strLoc = "Loc_290tj";
     bytArr = new Byte[0];
     iCountTestcases++;
     try
       {
       gui1 = new Guid(bytArr);
       iCountErrors++;
       printerr("Error_20utj! ArgumentException expected, got value=="+gui1);
       }
     catch (ArgumentException aExc)
       {
       Console.WriteLine("Info_2982h! Caught Expected ArgumentException, exc=="+aExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_298th! ArgumentException expected, got exc=="+exc.ToString());
       }
     bytArr = new Byte[17];
     iCountTestcases++;
     try
       {
       gui1 = new Guid(bytArr);
       iCountErrors++;
       printerr("Error_094jf! ArgumentException expected, got value=="+gui1);
       }
     catch (ArgumentException aExc)
       {
       Console.WriteLine("Info_198th! Caught Expected ArgumentException , exc=="+aExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_190th! ArgumentException expected, got exc=="+exc.ToString());
       }
     bytArr = new Byte[16];
     gui1 = new Guid(bytArr);
     iCountTestcases++;
     if(!gui1.ToString().Equals("00000000-0000-0000-0000-000000000000"))
       {
       iCountErrors++;
       printerr("Error_9181t! Incorrect value constructor in guid , got value=="+gui1);
       }
     bytArr = new Byte[16];
     for(int i = 0 ; i < 16 ; i++)
       bytArr[i] = (Byte)rand.Next(0, 255);
     gui1 = new Guid(bytArr);
     bytResultArr = gui1.ToByteArray();
     iCountTestcases++;
     Console.WriteLine(gui1);
     for(int i = 0 ; i < bytResultArr.Length ; i++)
       {
       iCountTestcases++;
       if(bytResultArr[i] != bytArr[i])
	 {
	 iCountErrors++;
	 printerr("Error_298hff , Expected value=="+bytArr[i]+" , got value=="+bytResultArr[i]);
	 }
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
   Co5322ToByteArray cbA = new Co5322ToByteArray();
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
