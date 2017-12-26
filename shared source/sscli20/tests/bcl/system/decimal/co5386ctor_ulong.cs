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
public class Co5386Ctor_ulong
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Decimal(usigned int)";
 public static String s_strTFName        = "Co5386Ctor_ulong.cs";
 public static String s_strTFAbbrev      = "Co5386";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public static Random rand               = new Random( ( ( DateTime.Now.Second * 1000 ) + DateTime.Now.Millisecond ) );
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   try
     {
     Decimal dec1, dec2;
     UInt64 ui64;
     strLoc = "Loc_289hv";
     for(long l = 0 ; l < Int32.MaxValue ; l+=1000000)
       {
       ui64 = Convert.ToUInt64(l);
       dec1 = new Decimal(ui64);
       iCountTestcases++;
       if(dec1 != (Decimal)l)
	 {
	 iCountErrors++;
	 printerr("Error_938fh! Expected=="+l.ToString()+" , got value=="+dec1.ToString());
	 }
       }
     strLoc = "Loc_298fy";
     dec1 = new Decimal(UInt64.MaxValue);
     iCountTestcases++;
     if(dec1 != Convert.ToDecimal(UInt64.MaxValue))
       {
       iCountErrors++;
       printerr("Error_28hfd! Expected=="+UInt64.MaxValue.ToString()+", got value=="+dec1.ToString());
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
   Co5386Ctor_ulong cbA = new Co5386Ctor_ulong();
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
