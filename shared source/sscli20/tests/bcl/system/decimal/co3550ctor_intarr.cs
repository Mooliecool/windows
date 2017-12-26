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
public class Co3550ctor_intArr
{
 internal static String strName = "Decimal.Decimal";
 internal static String strTest = "Co3550ctor_intArr.cs";
 internal static String strPath = "";
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
       iValues = new int[4];
       iValues[0] = 0;
       iValues[1] = 0;
       iValues[2] = 0;
       iValues[3] = 0;
       strLoc="249_sd";
       dcml1 = new Decimal(iValues);
       ++iCountTestcases;
       if ( dcml1 != Decimal.Zero )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr! dcml1 ==" + dcml1  );
	 }
       iValues = new int[4];
       iValues[0] = 0;
       iValues[1] = 0;
       iValues[2] = 0;
       iValues[3] = unchecked( (int) 0x80000000) ;
       strLoc="367_we";
       dcml1 = new Decimal(iValues);
       ++iCountTestcases;
       if ( dcml1 != Decimal.Zero )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_309ds! dcml1 ==" + dcml1  );
	 }
       iValues = new int[4];
       iValues[0] = 1;
       iValues[1] = 0;
       iValues[2] = 0;
       iValues[3] = 0;
       dcml1 = new Decimal(iValues);
       ++iCountTestcases;
       if ( dcml1 != Decimal.One )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_350re! dcml1 ==" + dcml1  );
	 }
       iValues = new int[4];
       iValues[0] = 1;
       iValues[1] = 0;
       iValues[2] = 0;
       iValues[3] = 0;
       dcml1 = new Decimal(iValues);
       ++iCountTestcases;
       if ( dcml1 != Decimal.One )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_350re! dcml1 ==" + dcml1  );
	 }
       iValues = new int[4];
       iValues[0] = 1;
       iValues[1] = 0;
       iValues[2] = 0;
       iValues[3] = unchecked( (int) 0x80000000 );
       dcml1 = new Decimal(iValues);
       ++iCountTestcases;
       if ( dcml1 != Decimal.MinusOne )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_936qw! dcml1 ==" + dcml1  );
	 }
       iValues = new int[4];
       iValues[0] = unchecked( (int) 0xa7640000);
       iValues[1] = (int) 0xde0b6b3;
       iValues[2] = 0;
       iValues[3] = unchecked( (int) 0x80120000 );
       dcml1 = new Decimal(iValues);
       ++iCountTestcases;
       if ( dcml1 != Decimal.MinusOne )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_035tr! dcml1 ==" + dcml1  );
	 }
       iValues = new int[4];
       iValues[0] = unchecked((int) 0xFFFFFFFF);
       iValues[1] = unchecked((int) 0xFFFFFFFF);
       iValues[2] = unchecked((int) 0xFFFFFFFF);
       iValues[3] = 0;
       dcml1 = new Decimal(iValues);
       ++iCountTestcases;
       if ( dcml1 != Decimal.MaxValue )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_394_fd! dcml1 ==" + dcml1  );
	 }
       iValues = new int[4];
       iValues[0] = unchecked((int) 0xFFFFFFFF);
       iValues[1] = unchecked((int) 0xFFFFFFFF);
       iValues[2] = unchecked((int) 0xFFFFFFFF);
       iValues[3] = unchecked((int) 0x80000000);
       dcml1 = new Decimal(iValues);
       ++iCountTestcases;
       if ( dcml1 != Decimal.MinValue )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_356qp! dcml1 ==" + dcml1  );
	 }
       try
	 {
	 iValues = new int[4];
	 iValues[0] = 0;
	 iValues[1] = 0;
	 iValues[2] = 0;
	 iValues[3] = 0x1000;
	 ++iCountTestcases;
	 dcml1 = new Decimal(iValues);
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_394et!  Exception not thrown"  );
	 }
       catch (ArgumentException)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_340wt!  , Wrong Exception thrown == " + ex.ToString()  );
	 }
       try
	 {
	 iValues = null;
	 ++iCountTestcases;
	 dcml1 = new Decimal(iValues);
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr!  Exception not thrown"  );
	 }
       catch (ArgumentException)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_240sd!  , Wrong Exception thrown == " + ex.ToString()  );
	 }
       try
	 {
	 iValues = new int[4];
	 iValues[0] = 0;
	 iValues[1] = 0;
	 iValues[2] = 0;
	 iValues[3] = 0x1000000;
	 ++iCountTestcases;
	 dcml1 = new Decimal(iValues);
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_305sd!  Exception not thrown"  );
	 }
       catch (ArgumentException)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_234sr!  , Wrong Exception thrown == " + ex.ToString()  );
	 }
       try
	 {
	 iValues = new int[5];
	 iValues[0] = 0;
	 iValues[1] = 0;
	 iValues[2] = 0;
	 iValues[3] = 0;
	 iValues[4] = 0;
	 ++iCountTestcases;
	 dcml1 = new Decimal(iValues);
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_340sd!  Exception not thrown"  );
	 }
       catch (ArgumentException)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_394sd!  , Wrong Exception thrown == " + ex.ToString()  );
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
   Co3550ctor_intArr oCbTest = new Co3550ctor_intArr();
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
