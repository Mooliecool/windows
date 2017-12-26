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
using System.Text;
public class Co1232GetBytes_char
{
 public Boolean runTest()
   {
   Console.Error.WriteLine( "Co1232GetBytes_char  runTest started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   int inNumByteArrElems = 2;  
   char ch2z = '?';
   Byte[] byArr2 = null; 
   Byte[] byArr3 = null; 
   try
     {
     do
       {
       strLoc="Loc_109ip.  Vanilla, covering each nibble with nonZero.";
       ch2z = (char)0x6f;
       byArr3 = new Byte[2];
       if (BitConverter.IsLittleEndian) {
           byArr3[0] = (Byte)0x6f;  
           byArr3[1] = (Byte)0x00;
       }
       else {
           byArr3[1] = (Byte)0x6f;  
           byArr3[0] = (Byte)0x00;
       }
       byArr2 = BitConverter.GetBytes( ch2z );
       ++iCountTestcases;
       if ( byArr2.Length != inNumByteArrElems )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_135ci!  byArr2.Length==" + byArr2.Length  );
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
       strLoc="Loc_748fb.";
       ch2z = (char)0x126f;
       byArr3 = new Byte[2];
       if (BitConverter.IsLittleEndian) {
           byArr3[0] = (Byte)0x6f;  
           byArr3[1] = (Byte)0x12;
       }
       else {
           byArr3[1] = (Byte)0x6f;  
           byArr3[0] = (Byte)0x12;
       }
       byArr2 = BitConverter.GetBytes( ch2z );
       ++iCountTestcases;
       if ( byArr2.Length != inNumByteArrElems )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_735sl!  byArr2.Length==" + byArr2.Length  );
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
       strLoc="Loc_611wq.  0";
       ch2z = (Char)0x0;
       byArr3 = new Byte[2];
       byArr3[0] = (Byte)0x00;
       byArr3[1] = (Byte)0x00;
       byArr2 = BitConverter.GetBytes( ch2z );
       ++iCountTestcases;
       if ( byArr2.Length != inNumByteArrElems )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_627zb!  byArr2.Length==" + byArr2.Length  );
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
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error E_343un! (Co1232GetBytes_char) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (E_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   BitConverter\\Co1232GetBytes_char.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   BitConverter\\Co1232GetBytes_char.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1232GetBytes_char cbA = new Co1232GetBytes_char();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error E_999zzz! (Co1232GetBytes_char) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     {
     Console.Error.WriteLine(  "Co1232GetBytes_char.cs   FAiL!"  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
