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
public class Co3431GetBytes_float
{
 internal static String strName = "BitConverter.GetBytes";
 internal static String strTest = "Co3431GetBytes_float.cs";
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
   int iNumByteArrElems = 4;  
   float flt1 = 0.0F;
   float flt2 = 0.0F;
   byte[] byArr2 = null;
   byte[] byArr3Expected = null;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       flt1 = 0.0F;
       byArr3Expected = new byte[iNumByteArrElems];
       byArr3Expected[0] = (byte)0x00;
       byArr3Expected[1] = (byte)0x00;
       byArr3Expected[2] = (byte)0x00;
       byArr3Expected[3] = (byte)0x00;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( flt1 );	
       ++iCountTestcases;
       if ( byArr2.Length != iNumByteArrElems )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_101ci!  byArr2.length==" + byArr2.Length  );
	 }
       ++iCountTestcases;
       for ( int aa = 0 ;aa < byArr2.Length ;aa++ )
	 {
	 if ( byArr2[aa] != byArr3Expected[aa] )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_102vo!  aa==" + aa + " ,byArr2[aa]==" + byArr2[aa]  );
	   break;
	   }
	 }
       flt1 = Single.MinValue;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( flt1 );
       flt2 = BitConverter.ToSingle(byArr2, 0);
       ++iCountTestcases;
       if ( byArr2.Length != iNumByteArrElems )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr!  byArr2.length==" + byArr2.Length  );
	 }
       ++iCountTestcases;
       if (flt2 != flt1)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_185eg! , flt1==" + flt1 + ", flt2 == " + flt2  );
	 }
       flt1 = Single.Epsilon;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( flt1 );
       flt2 = BitConverter.ToSingle(byArr2, 0);
       ++iCountTestcases;
       if ( byArr2.Length != iNumByteArrElems )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_874rf!  byArr2.length==" + byArr2.Length  );
	 }
       ++iCountTestcases;
       if (flt2 != flt1)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_036qp! , flt1==" + flt1 + ", flt2 == " + flt2  );
	 }
       flt1 = Single.MaxValue;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( flt1 );
       flt2 = BitConverter.ToSingle(byArr2, 0);
       ++iCountTestcases;
       if ( byArr2.Length != iNumByteArrElems )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_072ma!  byArr2.length==" + byArr2.Length  );
	 }
       ++iCountTestcases;
       if (flt2 != flt1)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_070re! , flt1==" + flt1 + ", flt2 == " + flt2  );
	 }
       flt1 = Single.PositiveInfinity;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( flt1 );
       flt2 = BitConverter.ToSingle(byArr2, 0);
       ++iCountTestcases;
       if ( byArr2.Length != iNumByteArrElems )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_087fd!  byArr2.length==" + byArr2.Length  );
	 }
       ++iCountTestcases;
       if (flt2.ToString().Equals(flt1.ToString()) != true)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_083ew! , flt1==" + flt1 + ", flt2 == " + flt2  );
	 }
       flt1 = Single.NegativeInfinity;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( flt1 );
       flt2 = BitConverter.ToSingle(byArr2, 0);
       ++iCountTestcases;
       if ( byArr2.Length != iNumByteArrElems )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_021ya!  byArr2.length==" + byArr2.Length  );
	 }
       ++iCountTestcases;
       if (flt2.ToString().Equals(flt1.ToString()) != true)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_234kq! , flt1==" + flt1 + ", flt2 == " + flt2  );
	 }
       flt1 = Single.NaN;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( flt1 );
       flt2 = BitConverter.ToSingle(byArr2, 0);
       ++iCountTestcases;
       if ( byArr2.Length != iNumByteArrElems )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_082fw!  byArr2.length==" + byArr2.Length  );
	 }
       ++iCountTestcases;
       if (flt2.ToString().Equals(flt1.ToString()) != true)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_087gf! , flt1==" + flt1 + ", flt2 == " + flt2  );
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
   Co3431GetBytes_float oCbTest = new Co3431GetBytes_float();
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
