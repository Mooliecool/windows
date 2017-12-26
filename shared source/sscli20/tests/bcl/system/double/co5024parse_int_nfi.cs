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
using System.Globalization;
using System;
public class Co5024Parse_int_nfi
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Double.Parse(String s, int style, NumberFormatInfo info)";
 public static readonly String s_strTFName        = "Co5024Parse_int_nfi.cs";
 public static readonly String s_strTFAbbrev      = "Co5024";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   Double dbla = (Double)0;
   Double dblb = (Double)0;
   String strOut = null;
   String str2 = null;
   Double [] dblTestValues = {Double.MinValue,
			      -1000.54,
			      -99.339,
			      -5.45,
			      -0.0,
			      0.0,
			      5.621,
			      13.4223,
			      101.81,
			      1000.999,
			      Double.MaxValue
   };
   Double [] dblResultValues1 = {Double.MinValue,
				 -1000.54,
				 -99.34,
				 -5.45,
				 0,
				 0,
				 5.62,
				 13.42,
				 101.81,
				 1001.00,
				 Double.MaxValue
   };
   Double [] dblResultValues2 = {Double.MinValue,
				 -1000.5,
				 -99.339,
				 -5.45,
				 0,
				 0,
				 5.621,
				 13.422,
				 101.81,
				 1001.00,
				 Double.MaxValue
   };
   Double [] dblResultValues3 = {Double.MinValue,
				 -1000.54,
				 -99.339,
				 -5.45,
				 0,
				 0,
				 5.621,
				 13.422,
				 101.81,
				 1000.999,
				 Double.MaxValue
   };
   NumberFormatInfo nfi1 = new NumberFormatInfo();
   nfi1.CurrencySymbol = "&";  
   nfi1.CurrencyDecimalDigits = 3;
   nfi1.NegativeSign = "^";  
   nfi1.NumberDecimalDigits = 3;    
   try {
   LABEL_860_GENERAL:
   do
     {
     strBaseLoc = "Loc_1100ds_";
     for (int i=1; i < dblTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc+ i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "", nfi1);
       dbla = Double.Parse(strOut, NumberStyles.Any, nfi1);
       if(dbla != dblTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i=="+i+" dbla=="+dbla+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1200er_";
     for (int i=1; i < dblTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "G", nfi1);
       dbla = Double.Parse(strOut, NumberStyles.Any, nfi1);
       if(dbla != dblTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347ew! , i=="+i+" dbla=="+dbla+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1300we_";
     for (int i=1; i < dblTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "G8", nfi1);
       dbla = Double.Parse(strOut, NumberStyles.Any, nfi1);
       if(dbla != dblTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_349ex! , i=="+i+" dbla=="+dbla+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1400fs_";
     for (int i=1; i < dblTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "C", nfi1);
       dbla = Double.Parse(strOut, NumberStyles.Any, nfi1);
       if(dbla != dblResultValues3[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_832ee! , i=="+i+" dbla=="+dbla+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1500ez_";
     for (int i=1; i < dblTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "C4", nfi1);
       dbla = Double.Parse(strOut, NumberStyles.Any, nfi1);
       if(dbla != dblTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_273oi! , i=="+i+" dbla=="+dbla+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1800ns_";
     for (int i=1; i < dblTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "E", nfi1);
       dbla = Double.Parse(strOut, NumberStyles.Any, nfi1);
       if(dbla != dblTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347sq! , i=="+i+" dbla=="+dbla+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1900wq_";
     for (int i=1; i < dblTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "e4", nfi1);
       dbla = Double.Parse(strOut, NumberStyles.Any, nfi1);
       if(dbla != dblResultValues2[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_873op! , i=="+i+" dbla=="+dbla+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2000ne_";
     for (int i=1; i < dblTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "N", nfi1);
       dbla = Double.Parse(strOut, NumberStyles.Any, nfi1);
       if(dbla != dblResultValues3[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_129we! , i=="+i+" dbla=="+dbla+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2100qu_";
     for (int i=1; i < dblTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "N4", nfi1);
       dbla = Double.Parse(strOut, NumberStyles.Any, nfi1);
       if(dbla != dblTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_321sj! , i=="+i+" dbla=="+dbla+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2500qi_";
     for (int i=1; i < dblTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "F", nfi1);
       dbla = Double.Parse(strOut, NumberStyles.Any, nfi1);
       if(dbla != dblResultValues3[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_815jd! , i=="+i+" dbla=="+dbla+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2600qi_";
     for (int i=1; i < dblTestValues.Length-1;i++)
       {
       try{
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "F4", nfi1);
       dbla = Double.Parse(strOut, NumberStyles.Any, nfi1);
       if(dbla != dblTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_193jd! , i=="+i+" dbla=="+dbla+" strOut=="+strOut);
	 }
       }catch(Exception ex){
       Console.WriteLine(strOut);
       }
       }
     strOut = null;
     iCountTestcases++;
     try {
     dbla = Double.Parse(strOut, NumberStyles.Any, nfi1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_273qp! , dbla=="+dbla);
     } catch (ArgumentException aExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_982qo! ,exc=="+exc);
     }
     strOut = "2E308";
     iCountTestcases++;
     try{
     dbla = Double.Parse(strOut, NumberStyles.Any, nfi1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_481sm! , dbla=="+dbla);
     }catch(OverflowException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_481sm! , dbla=="+ex.GetType().Name);
     }
     strOut ="^2E308";
     iCountTestcases++;
     try{
     dbla = Double.Parse(strOut, NumberStyles.Any, nfi1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_382er! ,dbla=="+dbla);
     }catch(OverflowException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_481sm! , dbla=="+ex.GetType().Name);
     }
     strOut = "-25";
     iCountTestcases++;
     try {
     dbla = Double.Parse(strOut, NumberStyles.Any, nfi1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_327wu! ,dbla=="+dbla);
     } catch (FormatException fExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_381! ,exc=="+exc);
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
   bool bResult = false;
   Co5024Parse_int_nfi cbA = new Co5024Parse_int_nfi();
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
