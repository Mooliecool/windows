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
public class Co1231GetBytes_short
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1231GetBytes_short  runTest started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int inNumByteArrElems = 2;  
   short sh2z = -2;
   byte[] byArr2 = null;
   byte[] byArr3 = null;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="Loc_109ip.  Vanilla, covering each nibble with nonZero.";
       sh2z = (short)0x1234;
       byArr3 = new byte[2];
       if (BitConverter.IsLittleEndian) {
           byArr3[0] = (byte)0x34;
           byArr3[1] = (byte)0x12;
       }
       else
       {
           byArr3[1] = (byte)0x34;
           byArr3[0] = (byte)0x12;
       }
       byArr2 = BitConverter.GetBytes( sh2z );
       ++iCountTestcases;
       if ( byArr2.Length != inNumByteArrElems )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_135ci!  byArr2.length==" + byArr2.Length  );
	 }
       ++iCountTestcases;
       for ( int aa = 0 ;aa < byArr2.Length ;aa++ )
	 {
	 if ( byArr2[aa] != byArr3[aa] )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_186vo!  aa==" + aa + " ,byArr2[aa]==" + byArr2[aa]  );
	   break;
	   }
	 }
       strLoc="Loc_748fb.  Negative number.";
       sh2z = unchecked((short)0x8765);
       byArr3 = new byte[2];
       if (BitConverter.IsLittleEndian) {
           byArr3[0] = (byte)0x65;  
           byArr3[1] = (byte)0x87;
       }
       else
       {
           byArr3[1] = (byte)0x65;  
           byArr3[0] = (byte)0x87;  
       }
       byArr2 = BitConverter.GetBytes( sh2z );
       ++iCountTestcases;
       if ( byArr2.Length != inNumByteArrElems )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_735sl!  byArr2.length==" + byArr2.Length  );
	 }
       ++iCountTestcases;
       for ( int aa = 0 ;aa < byArr2.Length ;aa++ )
	 {
	 if ( byArr2[aa] != byArr3[aa] )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_792pd!  aa==" + aa + " ,byArr2[aa]==" + byArr2[aa]  );
	   break;
	   }
	 }
       strLoc="Loc_611wq.  MIN number (MAX negative number).";
       sh2z = unchecked((short)0x8000);
       byArr3 = new byte[2];
       if (BitConverter.IsLittleEndian) {
           byArr3[0] = (byte)0x00;
           byArr3[1] = (byte)0x80;
       }
       else {
           byArr3[1] = (byte)0x00;
           byArr3[0] = (byte)0x80;
       }
       byArr2 = BitConverter.GetBytes( sh2z );
       ++iCountTestcases;
       if ( byArr2.Length != inNumByteArrElems )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_627zb!  byArr2.length==" + byArr2.Length  );
	 }
       ++iCountTestcases;
       for ( int aa = 0 ;aa < byArr2.Length ;aa++ )
	 {
	 if ( byArr2[aa] != byArr3[aa] )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_660ks!  aa==" + aa + " ,byArr2[aa]==" + byArr2[aa]  );
	   break;
	   }
	 }
       strLoc="Loc_522rv.  MAX number (MAX positive number).";
       sh2z = (short)0x7FFF;
       byArr3 = new byte[2];
       if (BitConverter.IsLittleEndian) {
           byArr3[0] = (byte)0xFF;
           byArr3[1] = (byte)0x7F;
       }
       else {
           byArr3[1] = (byte)0xFF;
           byArr3[0] = (byte)0x7F;
       }
       byArr2 = BitConverter.GetBytes( sh2z );
       ++iCountTestcases;
       if ( byArr2.Length != inNumByteArrElems )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_573dc!  byArr2.length==" + byArr2.Length  );
	 }
       ++iCountTestcases;
       for ( int aa = 0 ;aa < byArr2.Length ;aa++ )
	 {
	 if ( byArr2[aa] != byArr3[aa] )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_550km!  aa==" + aa + " ,byArr2[aa]==" + byArr2[aa]  );
	   break;
	   }
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error E_343un! (Co1231GetBytes_short) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (E_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   BitConverter\\Co1231GetBytes_short.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   BitConverter\\Co1231GetBytes_short.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1231GetBytes_short cbA = new Co1231GetBytes_short();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error E_999zzz! (Co1231GetBytes_short) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     {
     Console.Error.WriteLine(  "Co1231GetBytes_short.cs   FAiL!"  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
