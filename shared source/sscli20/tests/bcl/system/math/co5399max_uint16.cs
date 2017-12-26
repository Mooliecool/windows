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
public class Co5399Max_UInt16
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Math.Max(long,long)";
 public static String s_strTFName        = "Co5399Max_UInt16.cs";
 public static String s_strTFAbbrev      = "Co5399";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   try
     {
     UInt16 i16a, i16b, i16ret;
     strLoc = "Loc_09utg";
     i16a = UInt16.Parse("0");
     i16b = UInt16.Parse("0");
     i16ret = Math.Max(i16a, i16b);
     iCountTestcases++;
     if(i16ret != UInt16.Parse("0"))
       {
       iCountErrors++;
       printerr("Error_20xxu");
       }
     strLoc = "Loc_209uc";
     i16a = UInt16.Parse("1");
     i16b = UInt16.Parse("0");
     i16ret = Math.Max(i16a,i16b);
     iCountTestcases++;
     if(i16ret != UInt16.Parse("1"))
       {
       iCountErrors++;
       printerr("Error_230uc! Expected==0 , value=="+i16ret.ToString());
       }
     strLoc = "Loc_209cc";
     i16a  = UInt16.Parse("0");
     i16b = UInt16.Parse("1");
     i16ret = Math.Max(i16a, i16b);
     iCountTestcases++;
     if(i16ret != UInt16.Parse("1"))
       {
       iCountErrors++;
       printerr("Error_948nc! Expected==1 , value=="+i16ret.ToString());
       }
     strLoc = "Loc_20uxw";
     i16a = UInt16.MaxValue;
     i16b = UInt16.MinValue;
     i16ret = Math.Max(i16a, i16b);
     iCountTestcases++;
     if(i16ret != UInt16.MaxValue)
       {
       iCountErrors++;
       printerr("Error_2908x! expected=="+UInt16.MaxValue.ToString()+" , value=="+i16ret.ToString());
       }
     strLoc = "Loc_6745x";
     i16a = UInt16.Parse("10");
     i16b = UInt16.Parse("9");
     i16ret = Math.Max(i16a, i16b);
     iCountTestcases++;
     if(i16ret != UInt16.Parse("10"))
       {
       iCountErrors++;
       printerr("Error_298xz! Expected==10 , value=="+i16ret.ToString());
       }
     strLoc = "Loc_298yx";
     i16a = UInt16.Parse("9");
     i16b = UInt16.Parse("10");
     i16ret = Math.Max(i16a, i16b);
     iCountTestcases++;
     if(i16ret != UInt16.Parse("10"))
       {
       iCountErrors++;
       printerr("Error_298di! Expeceted==10 , value=="+i16ret.ToString());
       }
     strLoc = "Loc_90uxf";
     i16a = UInt16.MaxValue;
     i16b = UInt16.MaxValue;
     i16ret = Math.Max(i16a, i16b);
     iCountTestcases++;
     if(i16ret != UInt16.MaxValue)
       {
       iCountErrors++;
       printerr("Error_298xq! Expected=="+UInt16.MaxValue.ToString()+" , value=="+i16ret.ToString());
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
     Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
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
   Co5399Max_UInt16 cbA = new Co5399Max_UInt16();
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
