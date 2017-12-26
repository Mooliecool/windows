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
public class Co3552Compare
{
 static String strName = "Decimal.Compare";
 static String strTest = "Co3552Compare.cs";
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
   Decimal cncy1;
   Decimal[] dcmlFirstValues = {Decimal.MaxValue, Decimal.MinusOne, 
				Decimal.Parse("1.4214206543486529434634231456"),
				new Decimal(0.0001), Decimal.Zero, new Decimal(Int64.MaxValue),
				Decimal.MaxValue, new Decimal(1234.5678), new Decimal(-1234.5677),
				new Decimal(1234.5678)};
   Decimal[] dcmlSecondValues = {Decimal.One, Decimal.MinValue, 
				 Decimal.Parse("-7.9228162514264337593543950335"),
				 new Decimal(-0.0001), new Decimal(-0.0001), new Decimal(Int64.MinValue),
				 Decimal.MinValue, new Decimal(-1234.5678), new Decimal(-1234.5678),
				 new Decimal(1234.5677)};
   Decimal[] dcmlExceptions = {}; 
   int ina;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       for (int aa = 0; aa < dcmlFirstValues.Length; aa++)
	 {
	 ina = Decimal.Compare(dcmlFirstValues[aa], dcmlSecondValues[aa]);
	 ++iCountTestcases;
	 if ( ina != 1)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa + "  ina ==" + ina  );
	   }
	 }
       for (int aa = 0; aa < dcmlFirstValues.Length; aa++)
	 {
	 ina = Decimal.Compare(dcmlSecondValues[aa], dcmlFirstValues[aa]);
	 ++iCountTestcases;
	 if ( ina != -1)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_346mk_" + aa + "  ina ==" + ina  );
	   }
	 }
       for (int aa = 0; aa < dcmlFirstValues.Length; aa++)
	 {
	 ina = Decimal.Compare(dcmlFirstValues[aa], dcmlFirstValues[aa]);
	 ++iCountTestcases;
	 if ( ina != 0)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_934qm_" + aa + "  ina ==" + ina  );
	   }
	 }
       for (int aa = 0; aa < dcmlFirstValues.Length; aa++)
	 {
	 ina = Decimal.Compare(dcmlSecondValues[aa], dcmlSecondValues[aa]);
	 ++iCountTestcases;
	 if ( ina != 0)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_134ak_" + aa + "  ina ==" + ina  );
	   }
	 }
       for (int aa = 0; aa < dcmlExceptions.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   ina = Decimal.Compare(dcmlExceptions[aa], dcmlExceptions[aa]);
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
   Co3552Compare oCbTest = new Co3552Compare();
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
