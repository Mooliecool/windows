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
public class Co4647Max_Byte
{
 static String strTest = "Co4647Max_Byte";
 static String strPath = "";
 static String strBug = "";		
 public Boolean runTest()
   {
   Console.Error.WriteLine( strPath + strTest + "  runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   Byte expVal = 0x0;
   Byte retVal = 0x0;
   Byte inpVal = 0x0;
   Byte inpVal2 = 0x0;
   try {
   strLoc = "Loc_1_101.0";
   iCountTestcases++;
   inpVal = unchecked((Byte) 512);
   inpVal2 = unchecked((Byte) 512);
   expVal = inpVal;
   Console.WriteLine ("(case: 512) - Input1: " + inpVal + "; Input2: " + inpVal2 +"; Expected: " + expVal);
   retVal = Math.Max (inpVal, inpVal2);
   strLoc = "Loc_1_101.2";
   iCountTestcases++;
   if (retVal != expVal) {
   iCountErrors++;
   printerr("Error_1_2ysa - retVal != expVal; Expected: " + expVal + "Returned: " + retVal);
   }
   strLoc = "Loc_2_102.0";
   iCountTestcases++;
   inpVal = unchecked((Byte) (-512));
   inpVal2 = unchecked((Byte) 512);
   expVal = unchecked((Byte) 512);
   Console.WriteLine ("(case: -512) - Input1: " + inpVal + "; Input2: " + inpVal2 +"; Expected: " + expVal);
   retVal = Math.Max (inpVal, inpVal2);
   strLoc = "Loc_2_102.2";
   iCountTestcases++;
   if (retVal != expVal) {
   iCountErrors++;
   printerr("Error_2_2ysa - retVal != expVal; Expected: " + expVal + "Returned: " + retVal);
   }
   strLoc = "Loc_3_102.0";
   iCountTestcases++;
   inpVal = -0;
   inpVal2 = +0;
   expVal = +0;
   Console.WriteLine ("(case: -0) - Input1: " + inpVal + "; Input2: " + inpVal2 +"; Expected: " + expVal);
   retVal = Math.Max (inpVal, inpVal2);
   strLoc = "Loc_3_102.2";
   iCountTestcases++;
   if (retVal != expVal) {
   iCountErrors++;
   printerr("Error_3_2ysa - retVal != expVal; Expected: " + expVal + "Returned: " + retVal);
   }
   strLoc = "Loc_4_102.0";
   iCountTestcases++;
   inpVal = +0;
   inpVal2 = +0;
   expVal = +0;
   Console.WriteLine ("(case: +0) - Input1: " + inpVal + "; Input2: " + inpVal2 +"; Expected: " + expVal);
   retVal = Math.Max (inpVal, inpVal2);
   strLoc = "Loc_4_102.2";
   iCountTestcases++;
   if (retVal != expVal) {
   iCountErrors++;
   printerr("Error_4_2ysa - retVal != expVal; Expected: " + expVal + "Returned: " + retVal);
   }
   strLoc = "Loc_5_102.0";
   iCountTestcases++;
   inpVal = Byte.MinValue;
   inpVal2 = Byte.MaxValue;
   expVal = Byte.MaxValue;
   Console.WriteLine ("(case: Byte.MinValue, MaxValue) - Input1: " + inpVal + "; Input2: " + inpVal2 +"; Expected: " + expVal);
   retVal = Math.Max (inpVal, inpVal2);
   strLoc = "Loc_5_102.2";
   iCountTestcases++;
   if (retVal != expVal) {
   iCountErrors++;
   printerr("Error_5_2ysa - retVal != expVal; Expected: " + expVal + "Returned: " + retVal);
   }
   strLoc = "Loc_6_102.0";
   iCountTestcases++;
   inpVal = 5;
   inpVal2 = 5+2;
   expVal = 7;
   Console.WriteLine ("(case: 5,7 ) - Input1: " + inpVal + "; Input2: " + inpVal2 +"; Expected: " + expVal);
   retVal = Math.Max (inpVal, inpVal2);
   strLoc = "Loc_6_102.2";
   iCountTestcases++;
   if (retVal != expVal) {
   iCountErrors++;
   printerr("Error_6_2ysa - retVal != expVal; Expected: " + expVal + "Returned: " + retVal);
   }
   strLoc = "Loc_7_102.0";
   iCountTestcases++;
   inpVal = unchecked((Byte)(-1)); 
   inpVal2 = +1; 
   expVal = unchecked((Byte)(-1));
   Console.WriteLine ("(case: -1) - Input1: " + inpVal + "; Input2: " + inpVal2 +"; Expected: " + expVal);
   retVal = Math.Max (inpVal, inpVal2);
   strLoc = "Loc_7_102.2";
   iCountTestcases++;
   if (retVal != expVal) {
   iCountErrors++;
   printerr("Error_7_2ysa - retVal != expVal; Expected: " + expVal + "Returned: " + retVal);
   }
   }
   catch (Exception exc_runTest) {
   ++iCountErrors;
   printerr ("Err_888un! - Uncaught Exception caught in runTest(); strLoc == " + strLoc);
   printexc (exc_runTest);
   print (exc_runTest.StackTrace);
   }
   if (iCountErrors == 0) {
   Console.Error.WriteLine ("paSs.  " + strPath + strTest + "  iCountTestcases==" + iCountTestcases);
   return true;
   }
   else {
   Console.Error.WriteLine ("Related Bugs: " + strBug);
   Console.Error.WriteLine ("FAiL!   " + strPath +strTest + "  iCountErrors==" + iCountErrors);
   return false;
   }
   }
 private static void print (String str)
   {
   Console.Error.WriteLine (str);
   }
 private static void printerr (String errLoc)
   {
   String output = "POINTTOBREAK: find " + errLoc + " (" + strTest + ")";
   Console.Error.WriteLine (output);
   }
 private static void printexc (Exception exc)
   {
   String output = "EXTENDEDINFO: "+ exc.ToString();
   Console.Error.WriteLine (output);
   }
 public static void Main (String[] args) 
   {
   Boolean bResult = false; 
   Co4647Max_Byte cbA = new Co4647Max_Byte ();
   try {
   bResult = cbA.runTest();
   }
   catch (Exception exc_main) {
   bResult = false;
   printerr ("Err_999zzz! - Uncaught Exception caught in main(): ");
   printexc (exc_main);
   print (exc_main.StackTrace);
   }
   if (!bResult)
     Console.Error.WriteLine ("PATHTOSOURCE:  " + strPath + strTest + "  FAiL!");
   Environment.ExitCode = (bResult) ? 0 : 1;  
   }
}
