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
public class Co3556operator_plus_dec
{
 static String strName = "Decimal.+";
 static String strTest = "Co3556operator_plus_dec.cs";
 static String strPath = "";
 public Boolean runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   String strInfo = null;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strPath + strTest );
   Console.Out.WriteLine( " runTest started..." );
   Decimal dcml1;
   Decimal[] dcmlLeftValues = {Decimal.One, Decimal.MinusOne, Decimal.MaxValue, Decimal.MinValue, 
			       new Decimal(-1234.5678)};
   Decimal[] dcmlRightValues = {Decimal.One, Decimal.One, Decimal.MinValue, new Decimal(0.1), 
				new Decimal(-8765.43218)};
   Decimal[] dcmlExceptionLeftValues = {}; 
   Decimal[] dcmlExceptionRightValues = {}; 
   Decimal cncyValue;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       for (int aa = 0; aa < dcmlLeftValues.Length; aa++)
	 {
	 dcml1 =  + dcmlLeftValues[aa];
	 ++iCountTestcases;
	 if ( dcml1 != dcmlLeftValues[aa] )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa + "  dcml1 ==" + dcml1  );
	   }
	 }
       for (int aa = 0; aa < dcmlRightValues.Length; aa++)
	 {
	 dcml1 =  +dcmlRightValues[aa];
	 ++iCountTestcases;
	 if ( dcml1 != dcmlRightValues[aa] )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_346qw_" + aa + "  dcml1 ==" + dcml1  );
	   }
	 }
       for (int aa = 0; aa < dcmlExceptionLeftValues.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   dcml1 = dcmlExceptionLeftValues[aa] + dcmlExceptionRightValues[aa];
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_852sp_" + aa + "!  Exception not thrown"  );
	   }
	 catch (OverflowException ex)
	   {
	   }
	 catch (Exception ex)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_259pl_" + aa + "!  , Wrong Exception thrown == " + ex.ToString()  );
	   }
	 }
       } while ( false );
     }
   catch (Exception exc_general)
     {
     ++iCountErrors;
     Console.Error.WriteLine ( "POINTTOBREAK: Error Err_103! strLoc=="+ strLoc + " ,exc_general=="+exc_general  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs. " + strPath + strTest + "  iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL! " + strPath + strTest + "   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   Co3556operator_plus_dec oCbTest = new Co3556operator_plus_dec();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (" + strTest + ") Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "PATHTOSOURCE:  " + strPath + strTest + "   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
