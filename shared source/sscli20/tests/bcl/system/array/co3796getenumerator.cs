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
using System.IO;
using System.Collections;
public class Co3796GetEnumerator
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Array.GetEnumerator(bool)";
 public static String s_strTFName        = "Co3796GetEnumerator";
 public static String s_strTFAbbrev      = "Co3796";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   IEnumerator en1 = null;
   Array arrTest = null;
   int iLoop;
   Int32 iValue;
   try
     {
     arrTest = Array.CreateInstance(typeof(System.Int32), 10);
     for(int i=0; i< arrTest.Length; i++)
       {
       arrTest.SetValue((Object)(i+10), i);
       }
     en1 = (IEnumerator)arrTest.GetEnumerator();
     iLoop = 0;
     while (en1.MoveNext())
       {
       iValue = (Int32)en1.Current;
       iCountTestcases++;
       if(!iValue.Equals(arrTest.GetValue(iLoop)))
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_3765t2! Hmm, enumerator does not return in order? iLoop=" + iLoop.ToString() + ", arrTest.GetValue(iLoop)=" + arrTest.GetValue(iLoop).ToString() + ", iValue=" + iValue.ToString());
	 }
       iLoop++;
       }
     en1.Reset();
     iLoop = 0;
     while (en1.MoveNext())
       {
       iValue = (Int32)en1.Current;
       iCountTestcases++;
       if(!iValue.Equals(arrTest.GetValue(iLoop)))
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_9056vsf! Hmm, enumerator does not return in order? iLoop=" + iLoop.ToString() + ", arrTest.GetValue(iLoop)=" + arrTest.GetValue(iLoop).ToString() + ", iValue=" + iValue.ToString());
	 }
       iLoop++;
       }
     } catch (Exception exc_general ) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co3796GetEnumerator cbA = new Co3796GetEnumerator();
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
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
