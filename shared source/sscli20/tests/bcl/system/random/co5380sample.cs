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
public class Cb5380Sample : Random
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Random.Sample()";
 public static String s_strTFName        = "Cb5380Sample.cs";
 public static String s_strTFAbbrev      = "Cb5380";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "";
   String strValue = String.Empty;
   Cb5380Sample cc = new Cb5380Sample();
   try
     {
     Random rand;
     Double dbl2;
     strLoc = "Loc_29fhd";
     rand = new Random();
     for(int i = 0 ; i < 1000 ; i++)
       {
       iCountTestcases++;
       dbl2 = Sample();
       if(dbl2 < 0 || dbl2 > 1)
	 {
	 iCountErrors++;
	 printerr("Error_28ygg Value is not between 0 and 1");
	 }
       }
     int i1=0, i2=0, i3=0, i4=0, i5=0, i6=0, i7=0, i8=0, i9=0, i0=0;
     for(int i = 0 ; i < 10000 ; i++)
       {
       dbl2 = Sample();
       if(dbl2 <= 0.1) i1++;
       else if(dbl2 <= 0.2) i2++;
       else if(dbl2 <= 0.3) i3++;
       else if(dbl2 <= 0.4) i4++;
       else if(dbl2 <= 0.5) i5++;
       else if(dbl2 <= 0.6) i6++;
       else if(dbl2 <= 0.7) i7++;
       else if(dbl2 <= 0.8) i8++;
       else if(dbl2 <= 0.9) i9++;
       else if(dbl2 <= 1) i0++;
       else {
       iCountErrors++;
       printerr( "Error_278v8! OUCH!!! random sample didn't fall in any category");
       }
       }
     iCountTestcases++;
     if(i1 < 800 || i2 < 800 || i3 < 800 || i4 < 800 || i5 < 800 || i6 < 800 || i7 < 800 || i8 < 800 || i9 < 800 || i0 < 800)
       {
       iCountErrors++;
       printerr( "Error_298yvc! Bad distribution");
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
   Cb5380Sample cbA = new Cb5380Sample();
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
