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
public class Co3433ToSingle
{
 internal static String strName = "BitConverter.ToSingle";
 internal static String strTest = "Co3433ToSingle.cs";
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
   byte[] byArr3 = null;
   byte[] byArr3Expected = null;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       byArr2 = new byte[iNumByteArrElems];
       byArr2[0] = (byte)0x00;
       byArr2[1] = (byte)0x00;
       byArr2[2] = (byte)0x00;
       byArr2[3] = (byte)0x00;
       flt1 = BitConverter.ToSingle( byArr2, 0 );
       flt2 = 0.0F;
       ++iCountTestcases;
       if (flt2 != flt1)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_102vo! flt1== " + flt1  );
	 }
       flt1 = Single.MinValue;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( flt1);
       flt2 = BitConverter.ToSingle(byArr2, 0);
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
       if (flt2 != flt1)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_070re! , flt1==" + flt1 + ", flt2 == " + flt2  );
	 }
       flt1 = Single.MaxValue;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( flt1 );
       byArr3 = new byte[iNumByteArrElems + 1];
       for ( int aa = 0 ;aa < byArr2.Length ;aa++ )
	 {
	 byArr3[aa] = byArr2[aa];
	 }
       byArr3[iNumByteArrElems] = (byte)0x00;
       flt2 = BitConverter.ToSingle(byArr3, 1);
       ++iCountTestcases;
       if (flt2 == flt1)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_076sf! , flt1==" + flt1 + ", flt2 == " + flt2  );
	 }
       flt1 = Single.PositiveInfinity;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( flt1 );
       flt2 = BitConverter.ToSingle(byArr2, 0);
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
       if (flt2.ToString().Equals(flt1.ToString()) != true)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_087gf! , flt1==" + flt1 + ", flt2 == " + flt2  );
	 }
       byArr2 = null;
       try
	 {
	 ++iCountTestcases;
	 flt2 = BitConverter.ToSingle(byArr2, 0);
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_201wl!  flt2==" + flt2  );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_202jc!  exc.ToString()==" + exc.ToString()  );
	 }
       ++iCountTestcases;
       Random random = new Random();
       int randValue=0;
       byArr3 = new byte[9];
       byArr3[1] = (byte)0x78;
       byArr3[2] = (byte)0x56;
       byArr3[3] = (byte)0x34;
       byArr3[4] = (byte)0x12;
       for(int i=0; i<20; i++)
	 {
	 randValue = random.Next(-1000, 0);
	 try
	   {
	   BitConverter.ToSingle( byArr3 ,randValue);
	   ++iCountErrors;
	   Console.WriteLine("Err_3947tsfg! Exception not thrown, Value: {0}", randValue);
	   }
	 catch(ArgumentOutOfRangeException)
	   {
	   }
	 catch(Exception ex)
	   {
	   ++iCountErrors;
	   Console.WriteLine("Err_0745rtgf! Wrong exception thrown, {0}, Value: {1}", ex.GetType().Name, randValue);
	   }
	 }
       ++iCountTestcases;
       try
	 {
	 BitConverter.ToSingle( byArr3 ,Int32.MaxValue );
	 ++iCountErrors;
	 Console.WriteLine("Err_3947tsfg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine("Err_0745rtgf! Wrong exception thrown, " + ex.GetType().Name);
	 }
       ++iCountTestcases;
       try
	 {
	 BitConverter.ToSingle( byArr3 ,Int32.MinValue );
	 ++iCountErrors;
	 Console.WriteLine("Err_3947tsfg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine("Err_0745rtgf! Wrong exception thrown, " + ex.GetType().Name);
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
   Co3433ToSingle oCbTest = new Co3433ToSingle();
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
