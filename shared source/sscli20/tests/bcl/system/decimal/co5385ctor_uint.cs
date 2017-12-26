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
public class Co5385Ctor_uint
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Decimal(usigned int)";
 public static String s_strTFName        = "Co5385Ctor_uint.cs";
 public static String s_strTFAbbrev      = "Co5385";
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
     UInt32 ui32;
     iCountTestcases++;
     if(Convert.ToInt64(UInt32.MaxValue) != 0xffffffffL)
       {
       iCountErrors++;
       printerr("Error_98fud! UInt32.MaxValue is incorrect");
       }
     for(long l = 0 ; l < Convert.ToInt64(UInt32.MaxValue) ; l+=1000000)
       {
       ui32 = Convert.ToUInt32(l);
       dec1 = new Decimal(ui32);
       iCountTestcases++;
       if(dec1 != (Decimal)l)
	 {
	 iCountErrors++;
	 printerr("Error_938fh! Expected=="+l.ToString()+" , got value=="+dec1.ToString());
	 }
       }
     Console.WriteLine(UInt32.MaxValue);
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
   Co5385Ctor_uint cbA = new Co5385Ctor_uint();
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
