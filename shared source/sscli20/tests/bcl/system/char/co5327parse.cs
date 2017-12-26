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
public class Co5327Parse
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Char.Parse(String)";
 public static String s_strTFName        = "Co5327Parse.cs";
 public static String s_strTFAbbrev      = "Co5327";
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
     Char ch1, ch2;
     int i32a, i32b;
     String str1, str2;
     strLoc = "Loc_208th";
     iCountTestcases++;
     try
       {
       ch1 = Char.Parse(null);
       iCountErrors++;
       printerr("Error_09tuc! ArgumentNullException expected, got value=="+ch1);
       }
     catch (ArgumentNullException aExc)
       {
       Console.WriteLine("Info_902ut! Caught expected ArgumentNullException , exc=="+aExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_128ud! ArgumentNullException expected, got exc=="+exc.ToString());
       }
     strLoc = "Loc_09jtt";
     iCountTestcases++;
     try
       {
       ch1 = Char.Parse("  ");
       iCountErrors++;
       printerr("Error_98yt4! FormatException expected, got value=="+ch1);
       }
     catch (FormatException aExc)
       {
       Console.WriteLine("Info_9ty8y! Caught expected FormatException , exc=="+aExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_85yyd! FormatException expected, got exc=="+exc.ToString());
       }
     strLoc = "Loc_0t98u";
     iCountTestcases++;
     try
       {
       ch1 = Char.Parse("");
       iCountErrors++;
       printerr("Error_2098t! FormatException expected, got value=="+ch1);
       }
     catch (FormatException fExc)
       {
       Console.WriteLine("Info_2t9ht! Caught expected FormatException , exc=="+fExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_8ty84! FormatException expeceted, got exc=="+exc.ToString());
       }
     strLoc = "Loc_98ty4";
     ch1 = Char.Parse("\0");
     iCountTestcases++;
     if(ch1 != '\0')
       {
       iCountErrors++;
       printerr("Error_298th! Expected null character, got value=="+ch1);
       }
     for(int ii = 0 ; ii < 512 ; ii++)
       {
       str1 = ((Char)ii).ToString();
       ch1 = Char.Parse(str1);
       iCountTestcases++;
       if((int)ch1 != ii)
	 {
	 iCountErrors++;
	 printerr("Error_28th1! ii=="+ii+" , ch1=="+(int)ch1);
	 }
       if(!ch1.ToString().Equals(str1))
	 {
	 iCountErrors++;
	 printerr("Error_278th! str1=="+str1+" , ch1=="+ch1);
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
   Co5327Parse cbA = new Co5327Parse();
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
