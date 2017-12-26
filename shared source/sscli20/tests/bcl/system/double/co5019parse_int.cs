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
public class Co5019Parse_int
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Double.Parse(String s, int style)";
 public static String s_strTFName        = "Co5019Parse_int.cs";
 public static String s_strTFAbbrev      = "Co5019";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   Double dbla = (Double)0;
   String strOut = null;
   String str2 = null;
   int numberStyle;
   Double[] dblTestValues = {Double.MinValue, 
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
   Double[] dblResultValues1 = {Double.MinValue, 
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
   Double[] dblResultValues2 = {Double.MinValue, 
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
   try {
   do
     {
     strBaseLoc = "Loc_1100ds_";
     for (int i=1; i < dblTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc+ i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "");
       dbla = Double.Parse(strOut, NumberStyles.Any);
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
       strOut = dblTestValues[i].ToString( "G");
       dbla = Double.Parse(strOut, NumberStyles.Any);
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
       strOut = dblTestValues[i].ToString( "G8");
       dbla = Double.Parse(strOut, NumberStyles.Any);
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
       strOut = dblTestValues[i].ToString( "C");
       dbla = Double.Parse(strOut, NumberStyles.Any);
       if(dbla != dblResultValues1[i])
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
       strOut = dblTestValues[i].ToString( "C4");
       dbla = Double.Parse(strOut, NumberStyles.Any);
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
       strOut = dblTestValues[i].ToString( "E");
       dbla = Double.Parse(strOut, NumberStyles.Any);
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
       strOut = dblTestValues[i].ToString( "e4");
       dbla = Double.Parse(strOut, NumberStyles.Any);
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
       strOut = dblTestValues[i].ToString( "N");
       dbla = Double.Parse(strOut, NumberStyles.Any);
       if(dbla != dblResultValues1[i])
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
       strOut = dblTestValues[i].ToString( "N4");
       dbla = Double.Parse(strOut, NumberStyles.Any);
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
       strOut = dblTestValues[i].ToString( "F");
       dbla = Double.Parse(strOut, NumberStyles.Any);
       if(dbla != dblResultValues1[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_815jd! , i=="+i+" dbla=="+dbla+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2600qi_";
     for (int i=1; i < dblTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = dblTestValues[i].ToString( "F4");
       dbla = Double.Parse(strOut, NumberStyles.Any);
       if(dbla != dblTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_193jd! , i=="+i+" dbla=="+dbla+" strOut=="+strOut);
	 }
       }
     strOut = null;
     iCountTestcases++;
     try {
     dbla = Double.Parse(strOut, NumberStyles.Any);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_273qp! , dbla=="+dbla);
     } catch (ArgumentException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_982qo! ,exc=="+exc);
     }
     strOut = "2E308";
     iCountTestcases++;
     try{
     dbla = Double.Parse(strOut, NumberStyles.Any);
     iCountErrors++;
     Console.WriteLine("Err_36tsg! no exception thrown");
     }catch(OverflowException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_36tsg! wrong exception thrown, " + ex.GetType().Name);
     }
     strOut ="-2E308";
     iCountTestcases++;
     try{
     dbla = Double.Parse(strOut, NumberStyles.Any);
     iCountErrors++;
     Console.WriteLine("Err_36tsg! no exception thrown");
     }catch(OverflowException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_36tsg! wrong exception thrown, " + ex.GetType().Name);
     }
     str2 = "23.45";
     numberStyle = 0;
     iCountTestcases++;
     try {
     dbla = Double.Parse(str2, (NumberStyles)0);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_387uq");
     } catch (FormatException ) {}
     catch ( Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_9082w! , exc=="+exc);
     }
     str2 = "23.45";
     iCountTestcases++;
     if(Double.Parse(str2, NumberStyles.Number) != 23.45)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_348sy");
       }
     str2 = "23.34";
     iCountTestcases++;
     try {
     dbla = Double.Parse(str2, NumberStyles.Integer);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_8347y! , dbla=="+dbla);
     } catch ( FormatException) {}
     catch ( Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_408su! , exc=="+exc);
     }
     str2 = "             $834.54      ";
     iCountTestcases++;
     if((dbla = Double.Parse(str2, NumberStyles.Any)) != 834.54)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_874sd! , dbla=="+dbla);
       }
     str2 = "       $834.54    ";
     numberStyle = (int)NumberStyles.Any-1;
     iCountTestcases++;
     try {
     dbla = Double.Parse(str2, (NumberStyles)numberStyle);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_948us! , dbla=="+dbla);
     } catch ( FormatException ) {}
     catch ( Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_89eus! , exc=="+exc);
     }
     str2 = "$858.45";
     numberStyle = (int)NumberStyles.Any-256;
     iCountTestcases++;
     try {
     dbla = Double.Parse(str2, (NumberStyles)numberStyle);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_3094u! , dbla=="+dbla);
     } catch ( FormatException ) {}
     catch ( Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_984iw! , exc=="+exc);
     }
     str2 = "483.3";
     numberStyle = (int)NumberStyles.Any-32;
     iCountTestcases++;
     try {
     dbla = Double.Parse(str2, (NumberStyles)numberStyle);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_3984s! , dbla=="+dbla);
     } catch (FormatException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_947oi! , exc=="+exc);
     }
     str2 = "345,345.34";
     numberStyle = (int)NumberStyles.Any-64;
     iCountTestcases++;
     try {
     dbla = Double.Parse(str2, (NumberStyles)numberStyle);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_98suq! , dbla=="+dbla);
     } catch (FormatException ) {}
     catch ( Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_1248e! , exc=="+exc);
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
   Co5019Parse_int cbA = new Co5019Parse_int();
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
