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
public class Co3554Divide
{
 static String strName = "Decimal.Divide";
 static String strTest = "Co3554Divide.cs";
 static String strPath = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="Loc_100aa";
   String strLoBase=strLoc;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strPath + strTest );
   Console.Out.WriteLine( " runTest started..." );
   Decimal dcml1;
   Decimal[] dcmlLeftValues = {Decimal.One, Decimal.MaxValue, 
			       Decimal.Parse("0.9214206543486529434634231456", CultureInfo.InvariantCulture),  
			       Decimal.Parse("38214206543486529434634231456", CultureInfo.InvariantCulture),
			       Decimal.Parse("-78228162514264337593543950335", CultureInfo.InvariantCulture)};
   Decimal[] dcmlRightValues = {Decimal.One, Decimal.MinValue, 
				Decimal.MaxValue,
				Decimal.Parse("0.49214206543486529434634231456", CultureInfo.InvariantCulture),
				Decimal.MaxValue};
   Decimal[] dcmlExpValues = {Decimal.One, Decimal.MinusOne, 
			      Decimal.Zero,
			      Decimal.Parse("77648730371625094566866001277", CultureInfo.InvariantCulture),
			      Decimal.Parse("-0.987378225516463811113412343", CultureInfo.InvariantCulture)};
   Decimal[] dcmlExceptionLeftValues = {Decimal.One, Decimal.Zero}; 
   Decimal[] dcmlExceptionRightValues = {Decimal.Zero, Decimal.Zero}; 
   Decimal[] dcmlOverflowExceptionLeftValues = {Decimal.Parse("79228162514264337593543950335", CultureInfo.InvariantCulture), 
						Decimal.Parse("792281625142643.37593543950335", CultureInfo.InvariantCulture)};
   Decimal[] dcmlOverflowExceptionRightValues = {Decimal.Parse("-0.9999999999999999999999999", CultureInfo.InvariantCulture), 
						 Decimal.Parse(".0000000000000079228162514264337593543950335", CultureInfo.InvariantCulture)};
   try
     {
     do
       {
       strLoc="Loc_110aa";
       strLoBase=strLoc;
       for (int aa = 0; aa < dcmlLeftValues.Length; aa++)
	 {
	 strLoc=strLoBase +"_"+ aa;
	 dcml1 = Decimal.Divide(dcmlLeftValues[aa], dcmlRightValues[aa]);
	 ++iCountTestcases;
	 if ( dcml1 != dcmlExpValues[aa] )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa + "  dcml1 ==" + dcml1  );
	   }
	 }
       strLoc="Loc_120aa";
       strLoBase=strLoc;
       for (int aa = 0; aa < dcmlExceptionLeftValues.Length; aa++)
	 {
	 strLoc=strLoBase +"_"+ aa;
	 try
	   {
	   ++iCountTestcases;
	   dcml1 = Decimal.Divide(dcmlExceptionLeftValues[aa], dcmlExceptionRightValues[aa]);
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_852sp_" + aa + "!  Exception not thrown"  );
	   }
	 catch (DivideByZeroException)
	   {
	   }
	 catch (Exception ex)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_259pl_" + aa + "!  , Wrong Exception thrown == " + ex.ToString()  );
	   }
	 }
       strLoc="Loc_130aa";
       strLoBase=strLoc;
       for (int aa = 0; aa < dcmlOverflowExceptionLeftValues.Length; aa++)
	 {
	 strLoc=strLoBase +"_"+ aa;
	 try
	   {
	   ++iCountTestcases;
	   dcml1 = Decimal.Divide(dcmlOverflowExceptionLeftValues[aa], dcmlOverflowExceptionRightValues[aa]);
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_304sd_" + aa + "!  Exception not thrown"  );
	   }
	 catch (OverflowException)
	   {
	   }
	 catch (Exception ex)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_306se_" + aa + "!  , Wrong Exception thrown == " + ex.ToString()  );
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
   Co3554Divide oCbTest = new Co3554Divide();
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
