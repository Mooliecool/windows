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
public class Co5384Floor
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Decimal.Floor(Decimal)";
 public static String s_strTFName        = "Co5384Floor.cs";
 public static String s_strTFAbbrev      = "Co5384";
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
     strLoc = "Loc_298vh";
     dec1 = Decimal.Floor(Decimal.MinValue);
     iCountTestcases++;
     if(dec1 != Decimal.MinValue)
       {
       iCountErrors++;
       printerr("Error_98gbb! Expected value=="+Decimal.MinValue.ToString()+" , got value=="+dec1.ToString());
       }
     strLoc = "Loc_0999c";
     dec1 = Decimal.Floor(Decimal.MaxValue);
     iCountTestcases++;
     if(dec1 != Decimal.MaxValue)
       {
       iCountErrors++;
       printerr("Error_8ygg3! Expected value=="+Decimal.MaxValue.ToString()+" , got value=="+dec1.ToString());
       }
     strLoc = "Loc_29ugs";
     dec1 = new Decimal(1.99);
     dec2 = Decimal.Floor(dec1);
     iCountTestcases++;
     if(dec2 != 1)
       {
       iCountErrors++;
       printerr("Error_7gjjw! Expected value==1 , got value=="+dec2.ToString());
       }
     strLoc = "Loc_2u93w";
     dec1 = new Decimal(-1.01);
     dec2 = Decimal.Floor(dec1);
     iCountTestcases++;
     if(dec2 != -2)
       {
       iCountErrors++;
       printerr("Error_9838s! Expected value==-2 , got value=="+dec2.ToString());
       }
     strLoc = "Loc_298fh";
     dec2 = Decimal.Floor(new Decimal(100.01));
     iCountTestcases++;
     if(dec2 != 100)
       {
       iCountErrors++;
       printerr("Error_298hc! Expected value==100 , got value=="+dec2.ToString());
       }
     strLoc = "Loc_903ug";
     dec2 = Decimal.Floor(new Decimal(-200.99));
     iCountTestcases++;
     if(dec2 != -201)
       {
       iCountErrors++;
       printerr("Error_1092x! Expected value==-201 , got value=="+dec2.ToString());
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
   Co5384Floor cbA = new Co5384Floor();
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
