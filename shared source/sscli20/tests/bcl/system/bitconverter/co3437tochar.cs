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
public class Co3437ToChar
{
 internal static String strName = "BitConverter.ToChar";
 internal static String strTest = "Co3437ToChar.cs";
 internal static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   String strInfo = null;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strPath + strTest );
   Console.Out.WriteLine( " runTest started..." );
   char ch1 = '?';
   char ch2 = '\0';
   byte[] byArr2 = null;
   byte[] byArr3 = null;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       byArr3 = new byte[2];
       if (BitConverter.IsLittleEndian) {
           byArr3[0] = (byte)0x6f;  
           byArr3[1] = (byte)0x00;
       }
       else {
           byArr3[1] = (byte)0x6f;  
           byArr3[0] = (byte)0x00;
       }
       ch1 = BitConverter.ToChar(byArr3, 0);
       ch2 = (char)0x6f;
       ++iCountTestcases;
       if ( ch1 != ch2 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr!  ch1==" + ch1  );
	 }
       byArr3 = new byte[3];
       if (BitConverter.IsLittleEndian) {
           byArr3[1] = (byte)0x6f;  
           byArr3[2] = (byte)0x00;
       }
       else {
           byArr3[2] = (byte)0x6f;  
           byArr3[1] = (byte)0x00;
       }
       ch1 = BitConverter.ToChar(byArr3, 1);
       ch2 = (char)0x6f;
       ++iCountTestcases;
       if ( ch1 != ch2 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_942fd!  ch1==" + ch1  );
	 }
       byArr3 = new byte[2];
       byArr3[0] = (byte)0x00;
       byArr3[1] = (byte)0x00;
       ch1 = BitConverter.ToChar(byArr3, 0);
       ch2 = '\0';
       ++iCountTestcases;
       if ( ch1 != ch2 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_942fd!  ch1==" + ch1  );
	 }
       byArr3 = new byte[2];
       if (BitConverter.IsLittleEndian) {
           byArr3[0] = (byte)0x6f;  
           byArr3[1] = (byte)0x00;
       }
       else {
           byArr3[1] = (byte)0x6f;  
           byArr3[0] = (byte)0x00;
       }

       Random random = new Random();
       for(int i=0; i<20; i++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   ch1 = BitConverter.ToChar(byArr3, random.Next(Int32.MinValue, 0));
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_023ds!  Exception not thrown"  );
	   }
	 catch (ArgumentOutOfRangeException ex)
	   {
	   }
	 catch (Exception ex)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_042re!  , Wrong Exception thrown == " + ex.ToString()  );
	   }
	 }
       try
	 {
	 ++iCountTestcases;
	 BitConverter.ToChar( byArr3 ,Int32.MinValue );  
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_211wl!  str3==" );
	 }
       catch ( ArgumentOutOfRangeException argexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_212jc!  exc.ToString()==" + exc.ToString()  );
	 }
       try
	 {
	 ++iCountTestcases;
	 BitConverter.ToChar( byArr3 ,Int32.MaxValue );  
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_211wl!  str3==");
	 }
       catch ( ArgumentOutOfRangeException argexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_212jc!  exc.ToString()==" + exc.ToString()  );
	 }
       byArr3 = new byte[2];
       if (BitConverter.IsLittleEndian) {
           byArr3[0] = (byte)0x6f;  
           byArr3[1] = (byte)0x00;
       }
       else {
           byArr3[1] = (byte)0x6f;  
           byArr3[0] = (byte)0x00;
       }

       try
	 {
	 ++iCountTestcases;
	 ch1 = BitConverter.ToChar(byArr3, 1);
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_453km!  Exception not thrown"  );
	 }
       catch (ArgumentException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_137pa!  , Wrong Exception thrown == " + ex.ToString()  );
	 }
       byArr3 = null;
       try
	 {
	 ++iCountTestcases;
	 ch1 = BitConverter.ToChar(byArr3, 0);
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_085fg!  Exception not thrown"  );
	 }
       catch (ArgumentNullException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_032mr!  , Wrong Exception thrown == " + ex.ToString()  );
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
   Co3437ToChar oCbTest = new Co3437ToChar();
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
