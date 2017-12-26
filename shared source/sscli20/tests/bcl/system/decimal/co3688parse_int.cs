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
public class Co3688Parse_int
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Decimal.Parse(String format, int style)";
 public static String s_strTFName        = "Co3688Parse_int.cs";
 public static String s_strTFAbbrev      = "Cb3685";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   Decimal dcml1;
   String str1 = null;
   String str2 = null;
   Decimal[] dcmExpValues = {Decimal.MinValue, 
			     Decimal.MinusOne,
			     0,
			     Decimal.One,
			     Decimal.Parse("1.234567890123456789012345678"),
			     Decimal.MaxValue
   };
   String[] strCFormat1Values = {"($79,228,162,514,264,337,593,543,950,335.00)", 
				 "($1.00)",
				 "$0.00",
				 "$1.00",
				 "$1.23",
				 "$79,228,162,514,264,337,593,543,950,335.00"
   };
   Decimal[] dcmCFormat1ExpValues = {Decimal.MinValue, 
				     Decimal.MinusOne,
				     0,
				     Decimal.One,
				     Decimal.Parse("1.23"),
				     Decimal.MaxValue
   };
   String[] strCFormat2Values = {"($79,228,162,514,264,337,593,543,950,335.0000)", 
				 "($1.0000)",
				 "$0.0000",
				 "$1.0000",
				 "$1.2346",
				 "$79,228,162,514,264,337,593,543,950,335.0000"
   };
   Decimal[] dcmCFormat2ExpValues = {Decimal.MinValue, 
				     Decimal.MinusOne,
				     0,
				     Decimal.One,
				     Decimal.Parse("1.2346"),
				     Decimal.MaxValue
   };
   String[] strEFormat1Values = {"-7.922816E+028", 
				 "-1.000000E+000",
				 "0.000000E+000",
				 "1.000000E+000",
				 "1.234568E+000",
				 "7.922816E+028"
   };
   Decimal[] dcmEFormat1ExpValues = {Decimal.Parse("-79228160000000000000000000000"), 
				     -1,
				     0,
				     1,
				     (Decimal)1.234568,
				     Decimal.Parse("79228160000000000000000000000")
   };
   String[] strEFormat2alues = {"-7.9228e+028", 
                                "-1.0000e+000",
                                "0.0000e+000",
                                "1.0000e+000",
                                "1.2346e+000",
                                "7.9228e+028"
   };
   Decimal[] dcmEFormat2ExpValues = {Decimal.Parse("-79228000000000000000000000000"), 
				     -1,
				     0,
				     1,
				     (Decimal)1.2346,
				     Decimal.Parse("79228000000000000000000000000")
   };
   String[] strFFormat1Values = {"-79228162514264337593543950335.00", 
				 "-1.00",
				 "0.00",
				 "1.00",
				 "1.23",
				 "79228162514264337593543950335.00"
   };
   String[] strFFormat2Values = {"-79228162514264337593543950335.0000", 
				 "-1.0000",
				 "0.0000",
				 "1.0000",
				 "1.2346",
				 "79228162514264337593543950335.0000"
   };
   String[] strGFormat1Values = {"-79228162514264337593543950335", 
				 "-1",
				 "0",
				 "1",
				 "1.234567890123456789012345678",
				 "79228162514264337593543950335"
   };
   String[] strGFormat2Values = {"-7.922E28", 
				 "-1",
				 "0",
				 "1",
				 "1.235",
				 "7.922E28"
   };
   Decimal[] dcmGFormat2ExpValues = {Decimal.Parse("-79220000000000000000000000000"), 
				     -1,
				     0,
				     1,
				     (Decimal)1.235,
				     Decimal.Parse("79220000000000000000000000000")
   };
   String[] strNFormat1Values = {"-79,228,162,514,264,337,593,543,950,335.00", 
				 "-1.00",
				 "0.00",
				 "1.00",
				 "1.23",
				 "79,228,162,514,264,337,593,543,950,335.00"
   };
   Decimal[] dcmNFormat1ExpValues = {Decimal.MinValue, 
				     Decimal.MinusOne,
				     0,
				     Decimal.One,
				     Decimal.Parse("1.23"),
				     Decimal.MaxValue
   };
   String[] strNFormat2Values = {"-79,228,162,514,264,337,593,543,950,335.0000", 
				 "-1.0000",
				 "0.0000",
				 "1.0000",
				 "1.2346",
				 "79,228,162,514,264,337,593,543,950,335.0000"
   };
   Decimal[] dcmNFormat2ExpValues = {Decimal.MinValue, 
				     Decimal.MinusOne,
				     0,
				     Decimal.One,
				     Decimal.Parse("1.2346"),
				     Decimal.MaxValue
   };
   try {
   LABEL_860_GENERAL:
   do
     {
     strBaseLoc = "Loc_1100ds_";
     for (int i=0; i < dcmExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       dcml1 = Decimal.Parse(strGFormat1Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(dcml1 != dcmExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i==" + i + " dcml1==" + dcml1);
	 }
       }
     strBaseLoc = "Loc_1200er_";
     for (int i=0; i < dcmExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       dcml1 = Decimal.Parse(strGFormat2Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(dcml1 != dcmGFormat2ExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347ew! , i==" + i + " dcml1==" + dcml1);
	 }
       }
     strBaseLoc = "Loc_1400fs_";
     for (int i=0; i < dcmExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       dcml1 = Decimal.Parse(strCFormat1Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(dcml1 != dcmCFormat1ExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_832ee! , i==" + i + " dcml1==" + dcml1);
	 }
       }
     strBaseLoc = "Loc_1500ez_";
     for (int i=0; i < dcmExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       dcml1 = Decimal.Parse(strCFormat2Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(dcml1 != dcmCFormat2ExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_273oi! , i==" + i + " dcml1==" + dcml1);
	 }
       }
     strBaseLoc = "Loc_1800ns_";
     for (int i=0; i < dcmExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       dcml1 = Decimal.Parse(strEFormat1Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(dcml1 != dcmEFormat1ExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347sq! , i==" + i + " dcml1==" + dcml1);
	 }
       }
     strBaseLoc = "Loc_1900wq_";
     for (int i=0; i < dcmExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       dcml1 = Decimal.Parse(strEFormat2alues[i], NumberStyles.Any);
       iCountTestcases++;
       if(dcml1 != dcmEFormat2ExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_873op! , i==" + i + " dcml1==" + dcml1);
	 }
       }
     strBaseLoc = "Loc_2000ne_";
     for (int i=0; i < dcmExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       dcml1 = Decimal.Parse(strNFormat1Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(dcml1 != dcmNFormat1ExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_129we! , i==" + i + " dcml1==" + dcml1);
	 }
       }
     strBaseLoc = "Loc_2100qu_";
     for (int i=0; i < dcmExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       dcml1 = Decimal.Parse(strNFormat2Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(dcml1 != dcmNFormat2ExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_321sj! , i==" + i + " dcml1==" + dcml1);
	 }
       }
     strBaseLoc = "Loc_2500qi_";
     for (int i=0; i < dcmExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i;
       dcml1 = Decimal.Parse(strFFormat1Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(dcml1 != dcmNFormat1ExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_815jd! , i==" + i + " dcml1==" + dcml1);
	 }
       }
     strBaseLoc = "Loc_2600qi_";
     for (int i=0; i < dcmExpValues.Length;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       dcml1 = Decimal.Parse(strFFormat2Values[i], NumberStyles.Any);
       iCountTestcases++;
       if(dcml1 != dcmNFormat2ExpValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_193jd! , i=="+i+" str1=="+str1);
	 }
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
   Co3688Parse_int cbA = new Co3688Parse_int();
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
