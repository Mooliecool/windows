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
public class Co5326CompareTo
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Char.CompareTo(Object)";
 public static String s_strTFName        = "Co5326CompareTo.cs";
 public static String s_strTFAbbrev      = "Cb5325";
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
     strLoc = "Loc_982ht";
     ch1 = '\0';
     i32a = ch1.CompareTo(null);
     iCountTestcases++;
     if(i32a <= 0)
       {
       iCountErrors++;
       printerr("Error_92hfa! Null should be less than any instance");
       }
     strLoc = "Loc_290th";
     ch1 = '\0';
     ch2 = '\0';
     i32a = ch1.CompareTo(ch2);
     iCountTestcases++;
     if (i32a != 0)
       {
       iCountErrors++;
       printerr("Error_102id! Expected value==0 , got value=="+i32a);
       }
     strLoc = "Loc_092ut";
     ch1 = '\0';
     ch2 = '\0';
     i32a = ch1.CompareTo(ch2);
     iCountTestcases++;
     if(i32a != 0)
       {
       iCountErrors++;
       printerr("Error_919jd! Expected value==0, got value=="+i32a);
       }
     strLoc = "Loc_20thf";
     for(int ii = 0 ; ii < 255 ; ii++)
       {
       ch1 = (Char)ii;
       ch2 = (Char)ii;
       iCountTestcases++;
       if(ch1.CompareTo(ch2) != 0)
	 {
	 iCountErrors++;
	 printerr("Error_298th! Characters not equal as expected, ch=="+(int)ch1);
	 }
       }
     strLoc = "Loc_209jm";
     ch1 = 'a';
     ch2 = 'b';
     iCountTestcases++;
     if(ch1.CompareTo(ch2) >= 0)
       {
       iCountErrors++;
       printerr("Error_202hd! , ch1=="+(int)ch1+" , ch2=="+(int)ch2+" , got value=="+ch1.CompareTo(ch2));
       }
     strLoc = "Loc_2908t";
     ch1 = 'b';
     ch2 = 'a';
     iCountTestcases++;
     if(ch1.CompareTo(ch2) <= 0)
       {
       iCountErrors++;
       printerr("Error_092jf! , ch1=="+(int)ch1+" , ch2=="+(int)ch2+" , got value=="+ch1.CompareTo(ch2));
       }
     strLoc = "Loc_20ure";
     iCountTestcases++;
     try
       {
       i32a = ch1.CompareTo(35);
       iCountErrors++;
       printerr("Error_20jfq! Expected ArgumentException, got value=="+i32a);
       }
     catch (ArgumentException aExc)
       {
       Console.WriteLine("Info_8t2ht! Caught Expected ArgumentException, exc=="+aExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_928hg! Expected ArgumentException, got exc=="+exc.ToString());
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
   Co5326CompareTo cbA = new Co5326CompareTo();
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
