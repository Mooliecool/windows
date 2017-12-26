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
public class Co3562GetBits
{
 internal static String strName = "Decimal.Decimal";
 internal static String strTest = "Co3562GetBits.cs";
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
   int [] iValues = null;
   try
     {
     do
       {
       dcml1 = Decimal.Zero;
       iValues = Decimal.GetBits(dcml1);
       ++iCountTestcases;
       if ((iValues[0] != 0) || (iValues[1] != 0)  || (iValues[2] != 0)  || (iValues[3] != 0))
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "POINTTOBREAK: Error E_972qr! dcml1 ==" + dcml1 );
	 }
       iValues = null;
       dcml1 = Decimal.One;
       iValues = Decimal.GetBits(dcml1);
       ++iCountTestcases;
       if ((iValues[0] != 1) || (iValues[1] != 0)  || (iValues[2] != 0)  || (iValues[3] != 0))
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "POINTTOBREAK: Error E_350re! dcml1 ==" + dcml1 );
	 }
       iValues = null;
       dcml1 = Decimal.MinusOne;
       iValues = Decimal.GetBits(dcml1);
       ++iCountTestcases;
       if ((iValues[0] != 1) || (iValues[1] != 0)  || (iValues[2] != 0)  || (iValues[3] != unchecked((int)0x80000000)))
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "POINTTOBREAK: Error E_936qw! dcml1 ==" + dcml1 );
	 Console.WriteLine("iValues[0]=" + iValues[0] + ", iValues[1]=" + iValues[1] + ", iValues[2]=" + iValues[2] + ", iValues[3]=" + iValues[3]);
	 }
       iValues = null;
       dcml1 = Decimal.Parse("123.456789", CultureInfo.InvariantCulture);
       iValues = Decimal.GetBits(dcml1);
       ++iCountTestcases;
       if ((iValues[0] != (int)0x75BCD15) || (iValues[1] != 0)  || (iValues[2] != 0)  || (iValues[3] != (int)0x00060000))
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "POINTTOBREAK: Error E_346df! bits ==" + iValues[0] + " " + iValues[1] + " " + iValues[2] + " " + iValues[3] );
	 }
       iValues = null;
       dcml1 = Decimal.Parse("-123.456789", CultureInfo.InvariantCulture);
       iValues = Decimal.GetBits(dcml1);
       ++iCountTestcases;
       if ((iValues[0] != 0x75BCD15) || (iValues[1] != 0)  || (iValues[2] != 0)  || (iValues[3] != unchecked((int)0x80060000)))
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "POINTTOBREAK: Error E_873ved! bits ==" + iValues[0] + " " + iValues[1] + " " + iValues[2] + " " + iValues[3] );
	 }
       iValues = null;
       dcml1 = Decimal.MaxValue;
       iValues = Decimal.GetBits(dcml1);
       ++iCountTestcases;
       if ((iValues[0] != unchecked((int)0xFFFFFFFF)) || (iValues[1] != unchecked((int)0xFFFFFFFF))  || (iValues[2] != unchecked((int)0xFFFFFFFF))  || (iValues[3] != 0))
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "POINTTOBREAK: Error E_394_fd! dcml1 ==" + dcml1 );
	 }
       iValues = null;
       dcml1 = Decimal.MinValue;
       iValues = Decimal.GetBits(dcml1);
       ++iCountTestcases;
       if ((iValues[0] != unchecked((int)0xFFFFFFFF)) || (iValues[1] != unchecked((int)0xFFFFFFFF))  || (iValues[2] != unchecked((int)0xFFFFFFFF))  || (iValues[3] != unchecked((int)0x80000000)))
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "POINTTOBREAK: Error E_356qp! dcml1 ==" + dcml1 );
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
   Co3562GetBits oCbTest = new Co3562GetBits();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine( "POINTTOBREAK:  FAiL!  Error Err_999zzz! (" + strTest + ") Uncaught Exception caught in main(), exc_main==" + exc_main );
     }
   if ( ! bResult )
     Console.Error.WriteLine( "PATHTOSOURCE:  " + strPath + strTest + "   FAiL!" );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
