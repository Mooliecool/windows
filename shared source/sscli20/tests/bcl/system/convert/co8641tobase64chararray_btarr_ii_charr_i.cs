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
using System.Collections;
using System.Globalization;
using System.IO;
using System.Text;
using System.Reflection;
public class Co8641ToBase64CharArray_btArr_ii_chArr_i
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Convert.ToBase64CharArray(Byte[], int, int, Char[], i)";
 public static String s_strTFName        = "Co8641ToBase64CharArray_btArr_ii_chArr_i.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Byte[] bits;
   Char[] chars;
   String str1;
   Int32 returnValue;
   Random random = new Random();
   try
     {
     strLoc = "Loc_934sdg";
     iCountTestcases++;
     str1 = "test";
     chars = str1.ToCharArray();
     bits = Convert.FromBase64CharArray(chars, 0, chars.Length);
     chars = new Char[4];
     returnValue = Convert.ToBase64CharArray(bits, 0, bits.Length, chars, 0);
     if(returnValue != str1.Length){
     iCountErrors++;
     Console.WriteLine("Err_87345sdg! Unexpected returned result");
     }
     if(!str1.Equals(new String(chars))){
     iCountErrors++;
     Console.WriteLine("Err_87345sdg! Unexpected returned result");
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     bits = null;
     chars = new Char[1];
     try{
     returnValue = Convert.ToBase64CharArray(bits, 0, 1, chars, 0);
     iCountErrors++;
     Console.WriteLine("Err_3475sdg! No Exception returned");
     }catch(ArgumentNullException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     str1 = "test";
     chars = str1.ToCharArray();
     bits = Convert.FromBase64CharArray(chars, 0, chars.Length);
     chars = null;
     try{
     returnValue = Convert.ToBase64CharArray(bits, 0, bits.Length, chars, 0);
     iCountErrors++;
     Console.WriteLine("Err_3475sdg! No Exception returned");
     }catch(ArgumentNullException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     str1 = "test";
     chars = str1.ToCharArray();
     bits = Convert.FromBase64CharArray(chars, 0, chars.Length);
     chars = new Char[4];
     for(int i=0; i<20; i++)
       {
       try
	 {
	 returnValue = Convert.ToBase64CharArray(bits, random.Next(Int32.MinValue, 0), bits.Length, chars, 0);
	 iCountErrors++;
	 Console.WriteLine("Err_3475sdg! No Exception returned");
	 }
       catch(ArgumentOutOfRangeException)
	 {
	 }
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
	 }
       }
     try
       {
       returnValue = Convert.ToBase64CharArray(bits, Int32.MinValue, bits.Length, chars, 0);
       iCountErrors++;
       Console.WriteLine("Err_3475sdg! No Exception returned");
       }
     catch(ArgumentOutOfRangeException)
       {
       }
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
       }
     try
       {
       returnValue = Convert.ToBase64CharArray(bits, Int32.MaxValue, bits.Length, chars, 0);
       iCountErrors++;
       Console.WriteLine("Err_3475sdg! No Exception returned");
       }
     catch(ArgumentOutOfRangeException)
       {
       }
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
       }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     str1 = "test";
     chars = str1.ToCharArray();
     bits = Convert.FromBase64CharArray(chars, 0, chars.Length);
     chars = new Char[4];
     for(int i=0; i<20; i++)
       {
       try
	 {
	 returnValue = Convert.ToBase64CharArray(bits, 0, random.Next(bits.Length + 1, Int32.MaxValue), chars, 0);
	 iCountErrors++;
	 Console.WriteLine("Err_3475sdg! No Exception returned");
	 }
       catch(ArgumentOutOfRangeException)
	 {
	 }
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
	 }
       }
     for(int i=0; i<20; i++)
       {
       try
	 {
	 returnValue = Convert.ToBase64CharArray(bits, 0, random.Next(Int32.MinValue, 0), chars, 0);
	 iCountErrors++;
	 Console.WriteLine("Err_3475sdg! No Exception returned");
	 }
       catch(ArgumentOutOfRangeException)
	 {
	 }
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
	 }
       }
     try
       {
       returnValue = Convert.ToBase64CharArray(bits, 0, Int32.MinValue, chars, 0);
       iCountErrors++;
       Console.WriteLine("Err_3475sdg! No Exception returned");
       }
     catch(ArgumentOutOfRangeException)
       {
       }
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
       }
     try
       {
       returnValue = Convert.ToBase64CharArray(bits, 0, Int32.MaxValue, chars, 0);
       iCountErrors++;
       Console.WriteLine("Err_3475sdg! No Exception returned");
       }
     catch(ArgumentOutOfRangeException)
       {
       }
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
       }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     str1 = "test";
     chars = str1.ToCharArray();
     bits = Convert.FromBase64CharArray(chars, 0, chars.Length);
     chars = new Char[4];
     try{
     returnValue = Convert.ToBase64CharArray(bits, 1, bits.Length, chars, 0);
     iCountErrors++;
     Console.WriteLine("Err_3475sdg! No Exception returned");
     }catch(ArgumentOutOfRangeException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     str1 = "test";
     chars = str1.ToCharArray();
     bits = Convert.FromBase64CharArray(chars, 0, chars.Length);
     chars = new Char[4];
     for(int i=0; i<20; i++)
       {
       try
	 {
	 returnValue = Convert.ToBase64CharArray(bits, 0, bits.Length, chars, random.Next(Int32.MinValue, 0));
	 iCountErrors++;
	 Console.WriteLine("Err_3475sdg! No Exception returned");
	 }
       catch(ArgumentOutOfRangeException)
	 {
	 }
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
	 }
       }
     for(int i=0; i<20; i++)
       {
       try
	 {
	 returnValue = Convert.ToBase64CharArray(bits, 0, bits.Length, chars, random.Next(100, Int32.MaxValue));
	 iCountErrors++;
	 Console.WriteLine("Err_3475sdg! No Exception returned");
	 }
       catch(ArgumentOutOfRangeException)
	 {
	 }
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
	 }
       }
     try
       {
       returnValue = Convert.ToBase64CharArray(bits, 0, bits.Length, chars, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_3475sdg! No Exception returned");
       }
     catch(ArgumentOutOfRangeException)
       {
       }
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
       }
     try
       {
       returnValue = Convert.ToBase64CharArray(bits, 0, bits.Length, chars, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_3475sdg! No Exception returned");
       }
     catch(ArgumentOutOfRangeException)
       {
       }
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
       }
     strLoc = "Loc_84sdg";
     iCountTestcases++;
     str1 = "test";
     chars = str1.ToCharArray();
     bits = Convert.FromBase64CharArray(chars, 0, chars.Length);
     chars = new Char[4];
     try{
     returnValue = Convert.ToBase64CharArray(bits, 0, bits.Length, chars, 1);
     iCountErrors++;
     Console.WriteLine("Err_3475sdg! No Exception returned");
     }catch(ArgumentOutOfRangeException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_87345sdg! Unexpected Exception returned, " + ex.GetType().Name);
     }
     strLoc = "Loc_934sdg";
     iCountTestcases++;
     str1 = "test";
     chars = str1.ToCharArray();
     bits = Convert.FromBase64CharArray(chars, 0, chars.Length);
     chars = new Char[4];
     returnValue = Convert.ToBase64CharArray(bits, 0, bits.Length-1, chars, 0);
     if(returnValue != str1.Length){
     iCountErrors++;
     Console.WriteLine("Err_87345sdg! Unexpected returned result, " + returnValue);
     }
     if(!str1.Substring(0, 3).Equals((new String(chars)).Substring(0, 3))){
     iCountErrors++;
     Console.WriteLine("Err_87345sdg! Unexpected returned result, " + new String(chars));
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
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co8641ToBase64CharArray_btArr_ii_chArr_i cbA = new Co8641ToBase64CharArray_btArr_ii_chArr_i();
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
