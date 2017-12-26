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
public class Co3674Ctor_double
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Decimal.Decimal(double)";
 public static String s_strTFName        = "Co3674Ctor_double.cs";
 public static String s_strTFAbbrev      = "Co3674";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   String strInfo = null;
   Console.Out.Write( s_strClassMethod );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFPath + s_strTFName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strDtTmVer );
   Console.Out.WriteLine( " runTest started..." );
   Decimal dcml1;
   double[] dblValues = { (double)Int64.MinValue,  0d, 12.5, 
			  (double)Int64.MaxValue, (double)Int32.MinValue, (double)Int32.MaxValue,
			  1.23e14, -1.23e14};
   double[] dblExceptions = {Double.MinValue, Double.MaxValue, Double.NaN, 
			     Double.PositiveInfinity, Double.NegativeInfinity};
   double da;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       for (int aa = 0; aa < dblValues.Length; aa++)
	 {
	 dcml1 = new Decimal(dblValues[aa]);
	 ++iCountTestcases;
	 da = GetPropSmallDivByBig((double)dcml1, dblValues[aa]);
	 if (da > 0.00001)
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_972qr_" + aa + "  dcml1 ==" + dcml1 + "  dblValues[aa] ==" + dblValues[aa]  );
	   }
	 }
       for (int aa = 0; aa < dblExceptions.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   dcml1 = new Decimal(dblExceptions[aa]);
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_852sp_" + aa + "!  Exception not thrown"  );
	   }
	 catch (OverflowException ex)
	   {
	   }
	 catch (Exception ex)
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err259pl_" + aa + "!  , Wrong Exception thrown == " + ex  );
	   }
	 }
       } while ( false );
     }
   catch (Exception exc_general)
     {
     ++iCountErrors;
     Console.WriteLine( s_strTFAbbrev +"Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases=="+ iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors=="+ iCountErrors +" ,BugNums?: "+ s_strActiveBugNums );
     return false;
     }
   }
 public static double GetPropSmallDivByBig
   (
    double p_do8a
    ,double p_do8b
    )
   {
   double do8Big;
   double do8Small;
   double do8Ret;
   double do8w;
   if ( p_do8a >= p_do8b )
     {
     do8Big = p_do8a;
     do8Small = p_do8b;
     }
   else
     {
     do8Big = p_do8b;
     do8Small = p_do8a;
     }
   if( do8Small == (double)0.0 && do8Big == (double)0.0)
     return (double)0.0;
   if ( do8Small <= (double)0.0  &&  do8Big <= (double)0.0 )
     {
     do8w = do8Big;
     do8Big = do8Small;
     do8Small = do8w;
     }
   if (
       ( do8Big < (double)0.0  &&  do8Small > (double)0.0 )
       ||  ( do8Big > (double)0.0  &&  do8Small < (double)0.0 )
       )
     {
     do8Ret = ( Double.NaN );  
     return do8Ret;
     }
   if ( do8Big == (double)0.0 )
     {
     do8Ret = ( Double.NaN );
     return do8Ret;
     }
   do8Ret = ( (double)(do8Small / do8Big) );
   do8Ret = (double)1.0 - do8Ret;
   return do8Ret;
   } 
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   Co3674Ctor_double oCbTest = new Co3674Ctor_double();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.WriteLine( s_strTFAbbrev +"FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
     }
   if ( ! bResult )
     {
     Console.WriteLine( s_strTFName +s_strTFPath );
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
