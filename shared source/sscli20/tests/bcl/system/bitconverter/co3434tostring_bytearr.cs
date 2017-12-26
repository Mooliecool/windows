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
using System.Text;
using System;
public class Co3434ToString_ByteArr
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co3434ToString_ByteArr  runTest started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int inStartOffset = -2;
   String str2 = null;
   String str3 = null;
   byte[] byArr3 = null;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="Loc_100ku";
       str2 = "49-4A";
       str3 = "wrong 849589";
       byArr3 = new byte[2];
       byArr3[ 0] = (byte)0x49;  
       byArr3[ 1] = (byte)0x4A;
       str3 = BitConverter.ToString( byArr3 );  
       ++iCountTestcases;
       if ( str3.Equals( str2 ) == false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_101as!  str3==" + str3  );
	 }
       strLoc="Loc_200xt";
       str3 = "wrong 849589";
       byArr3 = null;
       try
	 {
	 ++iCountTestcases;
	 str3 = BitConverter.ToString( byArr3 );  
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_201wl!  str3==" + str3  );
	 }
       catch ( ArgumentNullException argexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_202jc!  exc.ToString()==" + exc.ToString()  );
	 }
       strLoc="Loc_400dh";
       inStartOffset = 0;  
       str2 = "41-42-43-44-45-46-00-00-00-00-00-00-00-00-00-00";
       str3 = "wrong 24350";
       byArr3 = new byte[16];
       byArr3[ 0] = (byte)0x41;  
       byArr3[ 1] = (byte)0x42;
       byArr3[ 2] = (byte)0x43;
       byArr3[ 3] = (byte)0x44;
       byArr3[ 4] = (byte)0x45;
       byArr3[ 5] = (byte)0x46;
       str3 = BitConverter.ToString( byArr3 );  
       ++iCountTestcases;
       if ( str3.Equals( str2 ) == false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_401as!  str3==" + str3  );
	 }
       ++iCountTestcases;
       if ( str2.Length == byArr3.Length - inStartOffset )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_402yo!  str3==" + str3  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error E_343un! (Cb1241ToString) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (E_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   BitConverter\\Cb1241ToString.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   BitConverter\\Cb1241ToString.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co3434ToString_ByteArr cbA = new Co3434ToString_ByteArr();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error E_999zzz! (Cb1241ToString) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     {
     Console.Error.WriteLine(  "Cb1241ToString.cs   FAiL!"  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
