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
public class Co3432ToDouble
{
 internal static String strName = "BitConverter.GetBytes";
 internal static String strTest = "Co3432ToDouble.cs";
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
   int iNumByteArrElems = 8;  
   Double dbl1 = 0.0D;
   Double dbl2 = 0.0D;
   byte[] byArr2 = null;
   byte[] byArr3 = null;
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
       byArr2[4] = (byte)0x00;
       byArr2[5] = (byte)0x00;
       byArr2[6] = (byte)0x00;
       byArr2[7] = (byte)0x00;
       dbl1 = BitConverter.ToDouble( byArr2, 0 );
       dbl2 = 0.0D;
       ++iCountTestcases;
       if (dbl2 != dbl1)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_102vo!  dbl1 == " + dbl1 );
	 }
       dbl1 = Double.MinValue;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( dbl1 );
       dbl2 = BitConverter.ToDouble(byArr2, 0);
       ++iCountTestcases;
       if (dbl2 != dbl1)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_185eg! , dbl1==" + dbl1 + ", dbl2 == " + dbl2  );
	 }
       dbl1 = Double.Epsilon;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( dbl1 );
       dbl2 = BitConverter.ToDouble(byArr2, 0);
       ++iCountTestcases;
       if (dbl2 != dbl1)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_036qp! , dbl1==" + dbl1 + ", dbl2 == " + dbl2  );
	 }
       dbl1 = Double.MinValue;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( dbl1 );
       byArr3 = new byte[iNumByteArrElems + 1];
       for ( int aa = 0 ;aa < byArr2.Length ;aa++ )
	 {
	 byArr3[aa] = byArr2[aa];
	 }
       byArr3[iNumByteArrElems] = (byte)0x01;
       dbl2 = BitConverter.ToDouble(byArr3, 1);
       ++iCountTestcases;
       if (dbl2 == dbl1)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_076sf! , dbl1==" + dbl1 + ", dbl2 == " + dbl2  );
	 }
       dbl1 = Double.MaxValue;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( dbl1 );
       dbl2 = BitConverter.ToDouble(byArr2, 0);
       ++iCountTestcases;
       if (dbl2 != dbl1)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_070re! , dbl1==" + dbl1 + ", dbl2 == " + dbl2  );
	 }
       dbl1 = Double.PositiveInfinity;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( dbl1 );
       dbl2 = BitConverter.ToDouble(byArr2, 0);
       ++iCountTestcases;
       if (dbl2.ToString().Equals(dbl1.ToString()) != true)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_083ew! , dbl1==" + dbl1 + ", dbl2 == " + dbl2  );
	 }
       dbl1 = Double.NegativeInfinity;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( dbl1 );
       dbl2 = BitConverter.ToDouble(byArr2, 0);
       ++iCountTestcases;
       if (dbl2.ToString().Equals(dbl1.ToString()) != true)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_234kq! , dbl1==" + dbl1 + ", dbl2 == " + dbl2  );
	 }
       dbl1 = Double.NaN;
       byArr2 = null;
       byArr2 = BitConverter.GetBytes( dbl1 );
       dbl2 = BitConverter.ToDouble(byArr2, 0);
       ++iCountTestcases;
       if (dbl2.ToString().Equals(dbl1.ToString()) != true)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_087gf! , dbl1==" + dbl1 + ", dbl2 == " + dbl2  );
	 }
       byArr2 = null;
       try
	 {
	 ++iCountTestcases;
	 dbl2 = BitConverter.ToDouble(byArr2, 0);
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_201wl!  dbl2==" + dbl2  );
	 }
       catch ( ArgumentNullException argexc )
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
	   dbl1 = BitConverter.ToDouble( byArr3 ,randValue);
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
	 dbl1 = BitConverter.ToDouble( byArr3 ,Int32.MaxValue );
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
	 dbl1 = BitConverter.ToDouble( byArr3 ,Int32.MinValue );
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
   Co3432ToDouble oCbTest = new Co3432ToDouble();
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
