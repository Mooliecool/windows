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
public class Co1233GetBytes_long
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1233GetBytes_long  runTest started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int inNumByteArrElems = 8;  
   long lo2z = -2;
   byte[] byArr2 = null;
   byte[] byArr3 = null;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="Loc_109ip.  Vanilla, covering each nibble with nonZero.";
       lo2z = 0x1234567812345678L;
       byArr3 = new byte[8];
       if (BitConverter.IsLittleEndian) {
           byArr3[0] = (byte)0x78;
           byArr3[1] = (byte)0x56;
           byArr3[2] = (byte)0x34;
           byArr3[3] = (byte)0x12;
           byArr3[4] = (byte)0x78;
           byArr3[5] = (byte)0x56;
           byArr3[6] = (byte)0x34;
           byArr3[7] = (byte)0x12;
       }
       else {
           byArr3[7] = (byte)0x78;
           byArr3[6] = (byte)0x56;
           byArr3[5] = (byte)0x34;
           byArr3[4] = (byte)0x12;
           byArr3[3] = (byte)0x78;
           byArr3[2] = (byte)0x56;
           byArr3[1] = (byte)0x34;
           byArr3[0] = (byte)0x12;
       }
       byArr2 = BitConverter.GetBytes( lo2z );
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
       lo2z = unchecked((long)0x8765432187654321L);
       byArr3 = new byte[8];
       if (BitConverter.IsLittleEndian) {
           byArr3[0] = (byte)0x21;
           byArr3[1] = (byte)0x43;
           byArr3[2] = (byte)0x65;
           byArr3[3] = (byte)0x87;
           byArr3[4] = (byte)0x21;
           byArr3[5] = (byte)0x43;
           byArr3[6] = (byte)0x65;
           byArr3[7] = (byte)0x87;
       }
       else {
           byArr3[7] = (byte)0x21;
           byArr3[6] = (byte)0x43;
           byArr3[5] = (byte)0x65;
           byArr3[4] = (byte)0x87;
           byArr3[3] = (byte)0x21;
           byArr3[2] = (byte)0x43;
           byArr3[1] = (byte)0x65;
           byArr3[0] = (byte)0x87;
       }
       byArr2 = BitConverter.GetBytes( lo2z );
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
       lo2z = unchecked((long)0x8000000000000000L);
       byArr3 = new byte[8];
       if (BitConverter.IsLittleEndian) {
           byArr3[0] = (byte)0x00;
           byArr3[1] = (byte)0x00;
           byArr3[2] = (byte)0x00;
           byArr3[3] = (byte)0x00;
           byArr3[4] = (byte)0x00;
           byArr3[5] = (byte)0x00;
           byArr3[6] = (byte)0x00;
           byArr3[7] = (byte)0x80;
       }
       else {
           byArr3[7] = (byte)0x00;
           byArr3[6] = (byte)0x00;
           byArr3[5] = (byte)0x00;
           byArr3[4] = (byte)0x00;
           byArr3[3] = (byte)0x00;
           byArr3[2] = (byte)0x00;
           byArr3[1] = (byte)0x00;
           byArr3[0] = (byte)0x80;
       }
       byArr2 = BitConverter.GetBytes( lo2z );
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
       lo2z = 0x7fffffffffffffffL;
       byArr3 = new byte[8];
       if (BitConverter.IsLittleEndian) {
           byArr3[0] = (byte)0xff;
           byArr3[1] = (byte)0xff;
           byArr3[2] = (byte)0xff;
           byArr3[3] = (byte)0xff;
           byArr3[4] = (byte)0xff;
           byArr3[5] = (byte)0xff;
           byArr3[6] = (byte)0xff;
           byArr3[7] = (byte)0x7f;
       }
       else {
           byArr3[7] = (byte)0xff;
           byArr3[6] = (byte)0xff;
           byArr3[5] = (byte)0xff;
           byArr3[4] = (byte)0xff;
           byArr3[3] = (byte)0xff;
           byArr3[2] = (byte)0xff;
           byArr3[1] = (byte)0xff;
           byArr3[0] = (byte)0x7f;
       }
       byArr2 = BitConverter.GetBytes( lo2z );
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
     Console.Error.WriteLine(  "POINTTOBREAK: Error E_343un! (Co1233GetBytes_long) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (E_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   BitConverter\\Co1233GetBytes_long.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   BitConverter\\Co1233GetBytes_long.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1233GetBytes_long cbA = new Co1233GetBytes_long();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error E_999zzz! (Co1233GetBytes_long) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     {
     Console.Error.WriteLine(  "Co1233GetBytes_long.cs   FAiL!"  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
