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
using System.Globalization;
public class Co8585Parse_str_ifp
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "SByte.Parse(String sNumberFormatInfo nfi)";
 public static String s_strTFName        = "Co8585Parse_str_ifp.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   SByte byt1a = (SByte)0;
   String strOut = null;
   SByte[] sbytTestValues = {SByte.MinValue, 
			     -100,
			     -5,
			     0,
			     13,
			     50,
			     101,
			     SByte.MaxValue
   };
   NumberFormatInfo nfi1 = new NumberFormatInfo();
   nfi1.NegativeSign = "^"; 
   nfi1.PositiveSign = "~"; 
   try {
   strBaseLoc = "Loc_1100ds_";
   for (int i=0; i < sbytTestValues.Length;i++)
     {
     strLoc = strBaseLoc+ i.ToString();
     iCountTestcases++;
     strOut = sbytTestValues[i].ToString( "", nfi1);
     byt1a = SByte.Parse(strOut, nfi1);
     if(byt1a != sbytTestValues[i])
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i=="+i+" byt1a=="+byt1a);
       }
     }
   strBaseLoc = "Loc_1300we_";
   for (int i=0; i < sbytTestValues.Length;i++)
     {
     strLoc = strBaseLoc + i.ToString();
     iCountTestcases++;
     strOut = sbytTestValues[i].ToString( "G5", nfi1);
     byt1a = SByte.Parse(strOut, nfi1);
     if(byt1a != sbytTestValues[i])
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_349ex! , i=="+i+" byt1a=="+byt1a+" strOut=="+strOut);
       }
     }
   strOut = null;
   iCountTestcases++;
   try {
   byt1a = SByte.Parse(strOut, nfi1);
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_273qp! , byt1a=="+byt1a);
   } catch (ArgumentException) {}
   catch (Exception exc) {
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_982qo! ,exc=="+exc);
   }
   strOut = "128";
   iCountTestcases++;
   try {
   byt1a = SByte.Parse(strOut, nfi1);
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_481sm! , byt1a=="+byt1a);
   } catch (OverflowException) {}
   catch (Exception exc) {
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_523eu! ,exc=="+exc);
   }
   strOut ="^129";
   iCountTestcases++;
   try {
   byt1a = SByte.Parse(strOut, nfi1);
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_382er! ,byt1a=="+byt1a);
   } catch (OverflowException) {}
   catch (Exception exc) {
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_84rwesdg! ,exc=="+exc);
   }
   iCountTestcases++;
   strOut = sbytTestValues[0].ToString( "C", nfi1);
   try {
   byt1a = SByte.Parse(strOut, nfi1);
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_382er! ,byt1a=="+byt1a);
   } catch (FormatException) {}
   catch (Exception exc) {
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_8756tedfsg! ,exc=="+exc);
   }
   byt1a = SByte.Parse("~123", nfi1);
   iCountTestcases++;
   if(byt1a != 123)
     {
     iCountErrors++;
     Console.WriteLine("Err_93475sdg! byt1a=="+byt1a);
     }
   nfi1.NumberNegativePattern = 3;
   strOut ="123^";
   iCountTestcases++;
   try {
   byt1a = SByte.Parse(strOut, nfi1);
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_382er! ,byt1a=="+byt1a);
   } catch (FormatException) {}
   catch (Exception exc) {
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_371jy! ,exc=="+exc);
   }
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine(s_strTFAbbrev +" Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
   }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
     return true;
     }
   else
     {
     Console.Error.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public static void Main(String[] args) 
   {
   Boolean bResult = false;
   Co8585Parse_str_ifp cbA = new Co8585Parse_str_ifp();
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
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
