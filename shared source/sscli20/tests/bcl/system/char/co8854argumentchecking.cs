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
public class Co8854ArgumentChecking
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer   = "";
 public static String s_strClassMethod   = "Argument Checking";
 public static String s_strTFName= "Co8854ArgumentChecking.cs";
 public static String s_strTFAbbrev  = s_strTFName.Substring(0,6);
 public static String s_strTFPath= Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int parm;
   String value;
   Random random;
   try
     {
     random = new Random();
     strLoc = "Loc_3597tfg";
     iCountTestcases++;
     value = String.Empty;
     for(int i=0; i<20; i++)
       {
       parm = random.Next(Int32.MinValue, Int32.MaxValue);
       try
	 {				
	 Char.IsControl(value, parm);
	 iCountErrors++;
	 Console.WriteLine("Err_7234tg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
	 }
       }
     try
       {				
       Char.IsControl(value, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     try
       {				
       Char.IsControl(value, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     strLoc = "Loc_3597tfg";
     iCountTestcases++;
     value = new String('a',1<<16);
     for(int i=0; i<20; i++)
       {
       parm = random.Next(Int32.MinValue, 0);
       try
	 {				
	 Char.IsDigit(value, parm);
	 iCountErrors++;
	 Console.WriteLine("Err_7234tg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
	 }
       }
     try
       {				
       Char.IsDigit(value, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     try
       {				
       Char.IsDigit(value, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     strLoc = "Loc_3597tfg";
     iCountTestcases++;
     value = new String('a',1<<16);
     for(int i=0; i<20; i++)
       {
       parm = random.Next(Int32.MinValue, 0);
       try
	 {				
	 Char.IsLetter(value, parm);
	 iCountErrors++;
	 Console.WriteLine("Err_7234tg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
	 }
       }
     try
       {				
       Char.IsLetter(value, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     try
       {				
       Char.IsLetter(value, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     strLoc = "Loc_3597tfg";
     iCountTestcases++;
     value = new String('a',1<<16);
     for(int i=0; i<20; i++)
       {
       parm = random.Next(Int32.MinValue, 0);
       try
	 {				
	 Char.IsLetterOrDigit(value, parm);
	 iCountErrors++;
	 Console.WriteLine("Err_7234tg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
	 }
       }
     try
       {				
       Char.IsLetterOrDigit(value, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     try
       {				
       Char.IsLetterOrDigit(value, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     strLoc = "Loc_3597tfg";
     iCountTestcases++;
     value = new String('a',1<<16);
     for(int i=0; i<20; i++)
       {
       parm = random.Next(Int32.MinValue, 0);
       try
	 {				
	 Char.IsLower(value, parm);
	 iCountErrors++;
	 Console.WriteLine("Err_7234tg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
	 }
       }
     try
       {				
       Char.IsLower(value, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     try
       {				
       Char.IsLower(value, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     strLoc = "Loc_3597tfg";
     iCountTestcases++;
     value = new String('a',1<<16);
     for(int i=0; i<20; i++)
       {
       parm = random.Next(Int32.MinValue, 0);
       try
	 {				
	 Char.IsNumber(value, parm);
	 iCountErrors++;
	 Console.WriteLine("Err_7234tg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
	 }
       }
     try
       {				
       Char.IsNumber(value, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     try
       {				
       Char.IsNumber(value, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     strLoc = "Loc_3597tfg";
     iCountTestcases++;
     value = new String('a',1<<16);
     for(int i=0; i<20; i++)
       {
       parm = random.Next(Int32.MinValue, 0);
       try
	 {				
	 Char.IsPunctuation(value, parm);
	 iCountErrors++;
	 Console.WriteLine("Err_7234tg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
	 }
       }
     try
       {				
       Char.IsPunctuation(value, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     try
       {				
       Char.IsPunctuation(value, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     strLoc = "Loc_3597tfg";
     iCountTestcases++;
     value = new String('a',1<<16);
     for(int i=0; i<20; i++)
       {
       parm = random.Next(Int32.MinValue, 0);
       try
	 {				
	 Char.IsSeparator(value, parm);
	 iCountErrors++;
	 Console.WriteLine("Err_7234tg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
	 }
       }
     try
       {				
       Char.IsSeparator(value, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     try
       {				
       Char.IsSeparator(value, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     strLoc = "Loc_3597tfg";
     iCountTestcases++;
     value = new String('a',1<<16);
     for(int i=0; i<20; i++)
       {
       parm = random.Next(Int32.MinValue, 0);
       try
	 {				
	 Char.IsSurrogate(value, parm);
	 iCountErrors++;
	 Console.WriteLine("Err_7234tg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
	 }
       }
     try
       {				
       Char.IsSurrogate(value, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     try
       {				
       Char.IsSurrogate(value, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     strLoc = "Loc_3597tfg";
     iCountTestcases++;
     value = new String('a',1<<16);
     for(int i=0; i<20; i++)
       {
       parm = random.Next(Int32.MinValue, 0);
       try
	 {				
	 Char.IsSymbol(value, parm);
	 iCountErrors++;
	 Console.WriteLine("Err_7234tg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
	 }
       }
     try
       {				
       Char.IsSymbol(value, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     try
       {				
       Char.IsSymbol(value, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     strLoc = "Loc_3597tfg";
     iCountTestcases++;
     value = new String('a',1<<16);
     for(int i=0; i<20; i++)
       {
       parm = random.Next(Int32.MinValue, 0);
       try
	 {				
	 Char.IsUpper(value, parm);
	 iCountErrors++;
	 Console.WriteLine("Err_7234tg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
	 }
       }
     try
       {				
       Char.IsUpper(value, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     try
       {				
       Char.IsUpper(value, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     strLoc = "Loc_3597tfg";
     iCountTestcases++;
     value = new String('a',1<<16);
     for(int i=0; i<20; i++)
       {
       parm = random.Next(Int32.MinValue, 0);
       try
	 {				
	 Char.IsWhiteSpace(value, parm);
	 iCountErrors++;
	 Console.WriteLine("Err_7234tg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
	 }
       }
     try
       {				
       Char.IsWhiteSpace(value, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     try
       {				
       Char.IsWhiteSpace(value, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     strLoc = "Loc_3597tfg";
     iCountTestcases++;
     value = new String('a',1<<16);
     for(int i=0; i<20; i++)
       {
       parm = random.Next(Int32.MinValue, 0);
       try
	 {				
	 Char.GetUnicodeCategory(value, parm);
	 iCountErrors++;
	 Console.WriteLine("Err_7234tg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
	 }
       }
     try
       {				
       Char.GetUnicodeCategory(value, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     try
       {				
       Char.GetUnicodeCategory(value, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     strLoc = "Loc_3597tfg";
     iCountTestcases++;
     value = new String('a',1<<16);
     for(int i=0; i<20; i++)
       {
       parm = random.Next(Int32.MinValue, 0);
       try
	 {				
	 Char.GetNumericValue(value, parm);
	 iCountErrors++;
	 Console.WriteLine("Err_7234tg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
	 }
       }
     try
       {				
       Char.GetNumericValue(value, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     try
       {				
       Char.GetNumericValue(value, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine("Err_7234tg! Exception not thrown");
       }
     catch(ArgumentOutOfRangeException){}
     catch(Exception ex)
       {
       iCountErrors++;
       Console.WriteLine("Err_3497tsg! Unexpected exception returned, {0}", ex.GetType().Name);
       }
     } 
   catch (Exception exc_general ) 
     {
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
   Co8854ArgumentChecking cbA = new Co8854ArgumentChecking();
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
