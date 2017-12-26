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
using System.IO; 
using System;
public class Co5127GetLength_int
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Array.Length";
 public static String s_strTFName        = "Co5127GetLength_int";
 public static String s_strTFAbbrev      = "Co5127";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc;
   short[] in2Arr = new Int16[10]; 
   int[] in4Arr = new Int32[5]; 
   long[] in8Arr = new Int64[0]; 
   String[] strArr = new String[6]; 
   Boolean[] boArr = new Boolean[3]; 
   Double[] dblArr = new Double[2]; 
   Single[] snglArr = new Single[32000]; 
   Char[] chArr = new Char[10000]; 
   int rank;
   try {
   LABEL_860_GENERAL:
   do
     {
     strLoc = "Loc_819yt";
     rank = -1;
     in2Arr = new Int16[5];
     iCountTestcases++;
     try {
     in2Arr.GetLength(rank);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_499ws! , GetLength=="+in2Arr.Length);
     } catch (IndexOutOfRangeException ioorExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_758! exc=="+exc);
     }
     strLoc = "Loc_819ee";
     rank = 1;
     in2Arr = new Int16[5];
     iCountTestcases++;
     try {
     in2Arr.GetLength(rank);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_500ws! , GetLength=="+in2Arr.Length);
     } catch (IndexOutOfRangeException ioorExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_750! exc=="+exc);
     }
     strLoc = "Loc_482wu";
     rank = 0;
     in2Arr = new Int16[10];
     iCountTestcases++;
     if(in2Arr.GetLength(rank) != 10)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_481ua! , GetLength=="+in2Arr.Length);
       }
     strLoc = "Loc_471ay";
     in4Arr = new Int32[5];
     iCountTestcases++;
     if(in4Arr.GetLength(rank) != 5)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_29qaq! , GetLength=="+in4Arr.Length);
       }
     strLoc = "Loc_982uq";
     in8Arr = new Int64[0];
     iCountTestcases++;
     if(in8Arr.GetLength(rank) != 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_237sy! , GetLength=="+in8Arr.Length);
       }
     strLoc = "Loc_172ms";
     boArr = new Boolean[3];
     iCountTestcases++;
     if(boArr.GetLength(rank) != 3)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_382! , GetLength=="+boArr.Length);
       }
     strLoc = "Loc_49su";
     dblArr = new Double[2];
     iCountTestcases++;
     if(dblArr.GetLength(rank) != 2)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_200su! , GetLength=="+dblArr.Length);
       }
     strLoc = "Loc_371su";
     snglArr = new Single[32000];
     iCountTestcases++;
     if(snglArr.GetLength(rank) != 32000)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_319aw! , GetLength=="+snglArr.Length);
       }
     strLoc = "Loc_129wi";
     strArr = new String[5];
     strArr[2] = null;
     iCountTestcases++;
     if(strArr.GetLength(rank) != 5)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_71ahw! , GetLength=="+strArr.Length);
       }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine(s_strTFAbbrev +" Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
   }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main(String[] args) 
   {
   Boolean bResult = false;
   Co5127GetLength_int cbA = new Co5127GetLength_int();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev+ "FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
   }
   if (!bResult)
     {
     Console.WriteLine(s_strTFName+ s_strTFPath);
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.Error.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
