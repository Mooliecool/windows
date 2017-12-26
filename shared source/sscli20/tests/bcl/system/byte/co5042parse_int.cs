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
public class Co5042Parse_int
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Byte.Parse(String s, int style)";
 public static String s_strTFName        = "Co5042Parse_int.cs";
 public static String s_strTFAbbrev      = "Co5042";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   Byte byt1a = (Byte)0;
   Byte bytb = (Byte)0;
   String strOut = null;
   String str2 = null;
   Byte[] bytTestValues = {Byte.MinValue, 
			   -0,
			   0,
			   5,
			   13,
			   50,
			   101,
			   200,
			   Byte.MaxValue
   };
   try {
   LABEL_860_GENERAL:
   do
     {
     strBaseLoc = "Loc_1100ds_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc+ i.ToString();
       iCountTestcases++;
       strOut = bytTestValues[i].ToString( "");
       byt1a = Byte.Parse(strOut, NumberStyles.Any);
       if(byt1a != bytTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i=="+i+" byt1a=="+byt1a);
	 }
       }
     strBaseLoc = "Loc_1200er_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = bytTestValues[i].ToString( "G");
       byt1a = Byte.Parse(strOut, NumberStyles.Any);
       if(byt1a != bytTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347ew! , i=="+i+" byt1a=="+byt1a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1300we_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = bytTestValues[i].ToString( "G5");
       byt1a = Byte.Parse(strOut, NumberStyles.Any);
       if(byt1a != bytTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_349ex! , i=="+i+" byt1a=="+byt1a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1400fs_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = bytTestValues[i].ToString( "C");
       byt1a = Byte.Parse(strOut, NumberStyles.Any);
       if(byt1a != bytTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_832ee! , i=="+i+" byt1a=="+byt1a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1500ez_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = bytTestValues[i].ToString( "C4");
       byt1a = Byte.Parse(strOut, NumberStyles.Any);
       if(byt1a != bytTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_273oi! , i=="+i+" byt1a=="+byt1a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1600nd_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = bytTestValues[i].ToString( "D");
       byt1a = Byte.Parse(strOut, NumberStyles.Any);
       if(byt1a != bytTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_901sn! , i=="+i+" byt1a=="+byt1a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1700eu_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = bytTestValues[i].ToString( "D4");
       byt1a = Byte.Parse(strOut, NumberStyles.Any);
       if(byt1a != bytTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_172sn! , i=="+i+" byt1a=="+byt1a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1800ns_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = bytTestValues[i].ToString( "E");
       byt1a = Byte.Parse(strOut, NumberStyles.Any);
       if(byt1a != bytTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347sq! , i=="+i+" byt1a=="+byt1a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1900wq_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = bytTestValues[i].ToString( "e4");
       byt1a = Byte.Parse(strOut, NumberStyles.Any);
       if(byt1a != bytTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_873op! , i=="+i+" byt1a=="+byt1a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2000ne_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = bytTestValues[i].ToString( "N");
       byt1a = Byte.Parse(strOut, NumberStyles.Any);
       if(byt1a != bytTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_129we! , i=="+i+" byt1a=="+byt1a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2100qu_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = bytTestValues[i].ToString( "N4");
       byt1a = Byte.Parse(strOut, NumberStyles.Any);
       if(byt1a != bytTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_321sj! , i=="+i+" byt1a=="+byt1a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2500qi_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = bytTestValues[i].ToString( "F");
       byt1a = Byte.Parse(strOut, NumberStyles.Any);
       if(byt1a != bytTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_815jd! , i=="+i+" byt1a=="+byt1a+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2600qi_";
     for (int i=0; i < bytTestValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = bytTestValues[i].ToString( "F4");
       byt1a = Byte.Parse(strOut, NumberStyles.Any);
       if(byt1a != bytTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_193jd! , i=="+i+" byt1a=="+byt1a+" strOut=="+strOut);
	 }
       }
     strOut = null;
     iCountTestcases++;
     try {
     byt1a = Byte.Parse(strOut, NumberStyles.Any);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_273qp! , byt1a=="+byt1a);
     } catch (ArgumentException aExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_982qo! ,exc=="+exc);
     }
     strOut = "256";
     iCountTestcases++;
     try {
     byt1a = Byte.Parse(strOut, NumberStyles.Any);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_481sm! , byt1a=="+byt1a);
     } catch (OverflowException ofExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_523eu! ,exc=="+exc);
     }
     strOut ="-1";
     iCountTestcases++;
     try {
     byt1a = Byte.Parse(strOut, NumberStyles.Any);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_382er! ,byt1a=="+byt1a);
     } catch (OverflowException fExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_371jy! ,exc=="+exc);
     }
     } while (false);
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
   Co5042Parse_int cbA = new Co5042Parse_int();
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
