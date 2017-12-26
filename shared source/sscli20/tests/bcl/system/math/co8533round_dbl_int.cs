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
using System.Reflection;
using Microsoft.Win32;
public class Co8533Round_dbl_int
{
 public static String s_strActiveBugNums			= "";
 public static readonly String s_strDtTmVer      = "";
 public static String s_strClassMethod			= "Math.Round(Double, Int32)";
 public static String s_strTFName				= "Co8533Round_dbl_int.cs";
 public static readonly String s_strTFAbbrev     = s_strTFName.Substring(0, 6);
 public static readonly String s_strTFPath       = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   Double dbl1;
   Double dbl2;
   Int32 digits;
   Random random = new Random();
   Decimal dcm1;
   int j;
   Double[] expected = new Double[]{ 0.346899383529388,
				     0.34689938352939,
				     0.3468993835294,
				     0.346899383529,
				     0.34689938353,
				     0.3468993835,
				     0.346899384,
				     0.34689938,
				     0.3468994,
				     0.346899,
				     0.3469,
				     0.3469,
				     0.347,
				     0.35,
				     0.3,
				     0.0,
   };
   try
     {
     strLoc = "Loc_001oo";
     dbl1 = 1.0;
     digits = -1;
     iCountTestcases++;
     for(int i=0; i<20; i++)
       {
       try 
	 {
	 Math.Round(dbl1, random.Next(Int32.MinValue, 0));
	 iCountErrors++;
	 Console.WriteLine( "Err_765sgs! Exception now thrown");
	 }
       catch(ArgumentException)
	 {
	 }
       catch (Exception exc) 
	 {
	 iCountErrors++;
	 Console.WriteLine( "Error_94f8v! Unexpected Exception, got exc=="+exc.GetType().Name);
	 }
       }
     dbl1 = 1.0;
     digits = 16;
     iCountTestcases++;
     for(int i=0; i<20; i++)
       {
       try 
	 {
	 Math.Round(dbl1, random.Next(16, Int32.MaxValue));
	 iCountErrors++;
	 Console.WriteLine( "Err_876sg! Exception now thrown, {0}", Math.Round(dbl1, digits));
	 }
       catch(ArgumentException)
	 {
	 }
       catch (Exception exc) 
	 {
	 iCountErrors++;
	 Console.WriteLine( "Error_1756sdg! Unexpected Exception, got exc=="+exc.GetType().Name);
	 }
       }
     dbl1 = 1.0;
     try 
       {
       Math.Round(dbl1, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine( "Err_876sg! Exception now thrown, {0}", Math.Round(dbl1, digits));
       }
     catch(ArgumentException)
       {
       }
     catch (Exception exc) 
       {
       iCountErrors++;
       Console.WriteLine( "Error_1756sdg! Unexpected Exception, got exc=="+exc.GetType().Name);
       }
     try 
       {
       Math.Round(dbl1, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine( "Err_876sg! Exception now thrown, {0}", Math.Round(dbl1, digits));
       }
     catch(ArgumentException)
       {
       }
     catch (Exception exc) 
       {
       iCountErrors++;
       Console.WriteLine( "Error_1756sdg! Unexpected Exception, got exc=="+exc.GetType().Name);
       }
     dcm1= 1.0m;
     iCountTestcases++;
     for(int i=0; i<20; i++)
       {
       try 
	 {
	 Math.Round(dcm1, random.Next(Int32.MinValue, 0));
	 iCountErrors++;
	 Console.WriteLine( "Err_765sgs! Exception now thrown");
	 }
       catch(ArgumentException)
	 {
	 }
       catch (Exception exc) 
	 {
	 iCountErrors++;
	 Console.WriteLine( "Error_94f8v! Unexpected Exception, got exc=="+exc.GetType().Name);
	 }
       }
     for(int i=0; i<20; i++)
       {
       try 
	 {
	 Math.Round(dcm1, random.Next(16, Int32.MaxValue));
	 iCountErrors++;
	 Console.WriteLine( "Err_876sg! Exception now thrown, {0}", Math.Round(dbl1, digits));
	 }
       catch(ArgumentException)
	 {
	 }
       catch (Exception exc) 
	 {
	 iCountErrors++;
	 Console.WriteLine( "Error_1756sdg! Unexpected Exception, got exc=="+exc.GetType().Name);
	 }
       }
     try 
       {
       Math.Round(dcm1, Int32.MaxValue);
       iCountErrors++;
       Console.WriteLine( "Err_876sg! Exception now thrown, {0}", Math.Round(dbl1, digits));
       }
     catch(ArgumentException)
       {
       }
     catch (Exception exc) 
       {
       iCountErrors++;
       Console.WriteLine( "Error_1756sdg! Unexpected Exception, got exc=="+exc.GetType().Name);
       }
     try 
       {
       Math.Round(dcm1, Int32.MinValue);
       iCountErrors++;
       Console.WriteLine( "Err_876sg! Exception now thrown, {0}", Math.Round(dbl1, digits));
       }
     catch(ArgumentException)
       {
       }
     catch (Exception exc) 
       {
       iCountErrors++;
       Console.WriteLine( "Error_1756sdg! Unexpected Exception, got exc=="+exc.GetType().Name);
       }
     dbl1 = Double.PositiveInfinity;
     digits = 1;
     dbl2 = Math.Round(dbl1, digits);
     iCountTestcases++;
     if(dbl1 != dbl2){
     iCountErrors++;
     Console.WriteLine( "Error_8743dgs! wrong value returned");
     }
     dbl1 = Double.PositiveInfinity;
     digits = 1;
     dbl2 = Math.Round(dbl1, digits);
     iCountTestcases++;
     if(dbl1 != dbl2){
     iCountErrors++;
     Console.WriteLine( "Error_8743dgs! wrong value returned");
     }
     dbl1 = Double.NegativeInfinity;
     digits = 1;
     dbl2 = Math.Round(dbl1, digits);
     iCountTestcases++;
     if(dbl1 != dbl2){
     iCountErrors++;
     Console.WriteLine( "Error_8734sdg! wrong value returned");
     }
     dbl1 = Double.NaN;
     digits = 1;
     dbl2 = Math.Round(dbl1, digits);
     iCountTestcases++;
     if(!Double.IsNaN(dbl2)){
     iCountErrors++;
     Console.WriteLine( "Error_8743wdsg! wrong value returned, {0} {1}", dbl1, dbl2);
     }			
     dbl1 = Double.Epsilon;
     iCountTestcases++;			
     for(digits=0; digits<16; digits++){
     dbl2 = Math.Round(dbl1, digits);
     if(dbl2 != 0){
     iCountErrors++;
     Console.WriteLine( "Error_8743sdg_" + digits + "! wrong value returned, {0}", dbl2);
     }
     }
     iCountTestcases++;			
     dbl1 = Double.MaxValue;
     for(digits=0; digits<16; digits++){
     dbl2 = Math.Round(dbl1, digits);
     if(dbl2 != dbl1){
     iCountErrors++;
     Console.WriteLine( "Error_8735sg_" + digits + "! wrong value returned, {0}", dbl2);
     }
     }
     dbl1 = Double.MinValue;
     for(digits=0; digits<16; digits++){
     dbl2 = Math.Round(dbl1, digits);
     if(dbl2 != dbl1){
     iCountErrors++;
     Console.WriteLine( "Error_275sg_" + digits + "! wrong value returned, {0}", dbl2);
     }
     }
     dbl1 = 1.1;
     digits = 0;
     dbl2 = Math.Round(dbl1, digits);
     iCountTestcases++;
     if(dbl2 != 1.0){
     iCountErrors++;
     Console.WriteLine( "Error_865fg! wrong value returned");
     }
     dbl1 = 1.5;
     digits = 0;
     dbl2 = Math.Round(dbl1, digits);
     iCountTestcases++;
     if(dbl2 != 2.0){
     iCountErrors++;
     Console.WriteLine( "Error_9734sdg! wrong value returned, {0}", dbl2);
     }
     dbl1 = 0.346899383529387642079245;
     dbl2 = Math.Round(dbl1, 15);
     iCountTestcases++;			
     digits = 0;
     for(digits=15, j=0; digits>=0; digits--, j++){
     dbl2 = Math.Round(dbl1, digits);
     if(dbl2 != expected[j]){
     iCountErrors++;
     Console.WriteLine("Error_8746gs_" + digits + "! wrong value \r\nreturned, <{0}> \r\nExpected, <{1}>", dbl2, expected[j]);
     }
     }
     dbl1 = -0.346899383529387642079245;
     iCountTestcases++;			
     digits = 0;
     for(digits=15, j=0; digits>=0; digits--, j++){
     dbl2 = Math.Round(dbl1, digits);
     if(dbl2 != -expected[j]){
     iCountErrors++;
     Console.WriteLine("Error_29745sg_" + digits + "! wrong value \r\nreturned, <{0}> \r\nExpected, <{1}>", dbl2, expected[j]);
     }
     }
     dbl1 = (double)1.0/3.0;
     digits = 15;
     dbl2 = Math.Round(dbl1, digits);
     iCountTestcases++;
     if(dbl2 != 0.333333333333333){
     iCountErrors++;
     Console.WriteLine( "Error_874sgd! wrong value returned, {0}", dbl2);
     }
     dbl1 = (double)1.0/7.0;
     digits = 15;
     dbl2 = Math.Round(dbl1, digits);
     iCountTestcases++;
     if(dbl2 != 0.142857142857143){
     iCountErrors++;
     Console.WriteLine( "Error_89743sg! wrong value returned, {0}", dbl2);
     }
     dbl1 = (double)0.22/7.0;
     digits = 15;
     dbl2 = Math.Round(dbl1, digits);
     iCountTestcases++;
     if(dbl2 != 0.031428571428571){
     iCountErrors++;
     Console.WriteLine( "Error_3976sdg! wrong value returned, {0}", dbl2);
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
     Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co8533Round_dbl_int cbA = new Co8533Round_dbl_int();
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
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
