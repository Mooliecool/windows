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
public class Co3559Equals_obj
{
 internal static String strName = "Decimal.Equals";
 internal static String strTest = "Co3559Equals_obj.cs";
 internal static String strPath = Environment.CurrentDirectory;
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strPath + strTest );
   Console.Out.WriteLine( " runTest started..." );
   Decimal dcml1;
   Decimal dcml2;
   Decimal cncy1;
   try
     {
     do
       {
       dcml1 = new Decimal(12.1235);
       dcml2 = Decimal.Parse("12.1235", CultureInfo.InvariantCulture);
       ++iCountTestcases;
       if ( dcml1.Equals(dcml2) != true )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr! dcml1.Equals(dcml2) ==" + dcml1.Equals(dcml2)  );
	 }
       dcml1 = new Decimal(12.1235);
       dcml2 = Decimal.Parse("-12.1235", CultureInfo.InvariantCulture);
       ++iCountTestcases;
       if ( dcml1.Equals(dcml2) != false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_356dq! dcml1.Equals(dcml2) ==" + dcml1.Equals(dcml2)  );
	 }
       dcml1 = new Decimal(12.1235);
       cncy1 = Decimal.Parse("12.1233", CultureInfo.InvariantCulture);
       ++iCountTestcases;
       if ( dcml1.Equals(cncy1) != false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_394fg! dcml1.Equals(cncy1) ==" + dcml1.Equals(cncy1)  );
	 }
       dcml1 = new Decimal(12.1235);
       Co3559Equals_obj ob1 = new Co3559Equals_obj();
       ++iCountTestcases;
       if ( dcml1.Equals(ob1) != false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_935sd! dcml1.Equals(ob1) ==" + dcml1.Equals(ob1)  );
	 }
       dcml1 = new Decimal(12.1235);
       Co3559Equals_obj ob2 = null;
       ++iCountTestcases;
       if ( dcml1.Equals(ob2) != false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_745we! dcml1.Equals(ob2) ==" + dcml1.Equals(ob2)  );
	 }
       dcml1 = new Decimal(12.1235);
       ++iCountTestcases;
       if ( cncy1.Equals(null) != false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_359er! dcml1.Equals(null) ==" + dcml1.Equals(null)  );
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
   bool bResult = false;	
   Co3559Equals_obj oCbTest = new Co3559Equals_obj();
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
