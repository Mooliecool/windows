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
public class Co7069FromString_Str
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "UInt32.FromString(String)"; 
 public static String s_strTFName        = "Co7069FromString_Str.cs";
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
     UInt32 ReturnValue;
     String strTest;			
     strLoc = "Loc_498yg";
     iCountTestcases++;
     strTest = "3";
     ReturnValue = UInt32.Parse(strTest);
     if (ReturnValue != 3)
       {
       iCountErrors++;
       printerr("Error_100aa SentIn==\"" + strTest + "\" Expected==3, value==" + ReturnValue.ToString());
       }
     strLoc = "Loc_298vy";
     iCountTestcases++;
     strTest = "0";
     ReturnValue = UInt32.Parse(strTest);
     if (ReturnValue != 0)
       {
       iCountErrors++;
       printerr("Error_200aa SentIn==\"" + strTest + "\" Expected==0, value==" + ReturnValue.ToString());
       }
     strLoc = "Loc_398vy";
     iCountTestcases++;
     strTest = UInt32.MaxValue.ToString();
     ReturnValue = UInt32.Parse(strTest);
     if (ReturnValue != UInt32.MaxValue)
       {
       iCountErrors++;
       printerr("Error_300aa SentIn==\"" + strTest + "\" Expected==4294967295, value==" + ReturnValue.ToString());
       }
     strLoc = "Loc_398vy";
     iCountTestcases++;
     try
       {
       strTest = "-1";
       ReturnValue = UInt32.Parse (strTest);
       iCountErrors++;
       printerr( "Error_400bb! no exception thrown");
       }
     catch (OverflowException)
       {
       printinfo( "Info_412ad! Caught OverflowException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_400aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_698vy";
     iCountTestcases++;
     try
       {
       strTest = "4294967296";
       ReturnValue = UInt32.Parse (strTest);
       iCountErrors++;
       printerr( "Error_500bb! no exception thrown");
       }
     catch (OverflowException)
       {
       printinfo( "Info_512ad! Caught OverflowException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_500aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_798vy";
     iCountTestcases++;
     try
       {
       strTest = null;
       ReturnValue = UInt32.Parse (strTest);
       iCountErrors++;
       printerr( "Error_600bb! no exception thrown");
       }
     catch (ArgumentNullException)
       {
       printinfo( "Info_612ad! Caught ArgumentNullException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_600aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_898vy";
     iCountTestcases++;
     try
       {
       strTest = "8,000,000";
       ReturnValue = UInt32.Parse (strTest);
       iCountErrors++;
       printerr( "Error_700bb! no exception thrown");
       }
     catch (FormatException)
       {
       printinfo( "Info_712ad! Caught FormatException");
       }
     catch (Exception e)
       {
       ++iCountErrors;	
       printerr( "Error_700aa! Wrong exception thrown: " + e.ToString());
       }
     strLoc = "Loc_898vy";
     iCountTestcases++;
     strTest = "\n\t   923   \t\n";
     ReturnValue = UInt32.Parse(strTest);
     if (ReturnValue != 923)
       {
       iCountErrors++;
       printerr("Error_800aa SentIn==\"" + strTest + "\" Expected==923, value==" + ReturnValue.ToString());
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
   Co7069FromString_Str cbA = new Co7069FromString_Str();
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
